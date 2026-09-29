#include "BanCo.h"
#include "Xe.h"
#include "Ma.h"
#include "Voi.h"
#include "Phao.h"
#include "Tuong.h"
#include "Si.h"
#include "Tot.h"
#include <algorithm>
#include <fstream>
#include <sstream>

BanCo::BanCo() : luotChoi(Mau::DO), ketThuc(false), nguoiThang(Mau::DO) {
    khoiTaoBanCo();
}

void BanCo::khoiTaoBanCo() {
    cacQuan.clear();
    lichSuNuocDi.clear();
    lichSuRedo.clear();
    
    // Quan Do (hang 9 o duoi)
    cacQuan.push_back(std::make_shared<Xe>(9, 0, Mau::DO));
    cacQuan.push_back(std::make_shared<Ma>(9, 1, Mau::DO));
    cacQuan.push_back(std::make_shared<Voi>(9, 2, Mau::DO));
    cacQuan.push_back(std::make_shared<Si>(9, 3, Mau::DO));
    cacQuan.push_back(std::make_shared<Tuong>(9, 4, Mau::DO));
    cacQuan.push_back(std::make_shared<Si>(9, 5, Mau::DO));
    cacQuan.push_back(std::make_shared<Voi>(9, 6, Mau::DO));
    cacQuan.push_back(std::make_shared<Ma>(9, 7, Mau::DO));
    cacQuan.push_back(std::make_shared<Xe>(9, 8, Mau::DO));
    
    // Phao Do
    cacQuan.push_back(std::make_shared<Phao>(7, 1, Mau::DO));
    cacQuan.push_back(std::make_shared<Phao>(7, 7, Mau::DO));
    
    // Tot Do
    for (int i = 0; i < 9; i += 2) {
        cacQuan.push_back(std::make_shared<Tot>(6, i, Mau::DO));
    }
    
    // Quan Den (hang 0 o tren)
    cacQuan.push_back(std::make_shared<Xe>(0, 0, Mau::DEN));
    cacQuan.push_back(std::make_shared<Ma>(0, 1, Mau::DEN));
    cacQuan.push_back(std::make_shared<Voi>(0, 2, Mau::DEN));
    cacQuan.push_back(std::make_shared<Si>(0, 3, Mau::DEN));
    cacQuan.push_back(std::make_shared<Tuong>(0, 4, Mau::DEN));
    cacQuan.push_back(std::make_shared<Si>(0, 5, Mau::DEN));
    cacQuan.push_back(std::make_shared<Voi>(0, 6, Mau::DEN));
    cacQuan.push_back(std::make_shared<Ma>(0, 7, Mau::DEN));
    cacQuan.push_back(std::make_shared<Xe>(0, 8, Mau::DEN));
    
    // Phao Den
    cacQuan.push_back(std::make_shared<Phao>(2, 1, Mau::DEN));
    cacQuan.push_back(std::make_shared<Phao>(2, 7, Mau::DEN));
    
    // Tot Den
    for (int i = 0; i < 9; i += 2) {
        cacQuan.push_back(std::make_shared<Tot>(3, i, Mau::DEN));
    }
}

void BanCo::lamMoi() {
    khoiTaoBanCo();
    luotChoi = Mau::DO;
    ketThuc = false;
    nguoiThang = Mau::DO;
}

std::shared_ptr<QuanCo> BanCo::timQuan(int hang, int cot) const {
    for (const auto& quan : cacQuan) {
        if (!quan->getDaBiAn() && quan->getHang() == hang && quan->getCot() == cot) {
            return quan;
        }
    }
    return nullptr;
}

bool BanCo::trongBanCo(int hang, int cot) const {
    return hang >= 0 && hang < SO_HANG && cot >= 0 && cot < SO_COT;
}

bool BanCo::coQuanTai(int hang, int cot) const {
    return timQuan(hang, cot) != nullptr;
}

bool BanCo::duongThangTrong(int hang1, int cot1, int hang2, int cot2) const {
    return demQuanTrenDuong(hang1, cot1, hang2, cot2) == 0;
}

int BanCo::demQuanTrenDuong(int hang1, int cot1, int hang2, int cot2) const {
    int dem = 0;
    
    if (hang1 == hang2) {
        int batDau = std::min(cot1, cot2) + 1;
        int ketThuc = std::max(cot1, cot2);
        
        for (int c = batDau; c < ketThuc; c++) {
            if (coQuanTai(hang1, c)) {
                dem++;
            }
        }
    } else if (cot1 == cot2) {
        int batDau = std::min(hang1, hang2) + 1;
        int ketThuc = std::max(hang1, hang2);
        
        for (int h = batDau; h < ketThuc; h++) {
            if (coQuanTai(h, cot1)) {
                dem++;
            }
        }
    }
    
    return dem;
}

bool BanCo::kiemTraChieu(Mau mauTuong) const {
    // Tim vi tri Tuong
    std::shared_ptr<QuanCo> tuong = nullptr;
    for (const auto& quan : cacQuan) {
        if (!quan->getDaBiAn() && quan->getMau() == mauTuong && quan->layTen() == "Tuong") {
            tuong = quan;
            break;
        }
    }
    
    if (!tuong) return false;
    
    int hangTuong = tuong->getHang();
    int cotTuong = tuong->getCot();
    
    // Kiem tra tat ca quan dich xem co an duoc Tuong khong
    for (const auto& quan : cacQuan) {
        if (!quan->getDaBiAn() && quan->getMau() != mauTuong) {
            if (quan->kiemTraNuocDi(*this, hangTuong, cotTuong)) {
                return true;
            }
        }
    }
    
    return false;
}

bool BanCo::kiemTraChieuTuong() const {
    // Kiem tra 2 Tuong doi mat (chong mat Tuong)
    std::shared_ptr<QuanCo> tuongDo = nullptr;
    std::shared_ptr<QuanCo> tuongDen = nullptr;
    
    for (const auto& quan : cacQuan) {
        if (!quan->getDaBiAn() && quan->layTen() == "Tuong") {
            if (quan->getMau() == Mau::DO) {
                tuongDo = quan;
            } else {
                tuongDen = quan;
            }
        }
    }
    
    if (tuongDo && tuongDen) {
        if (tuongDo->getCot() == tuongDen->getCot()) {
            if (duongThangTrong(tuongDo->getHang(), tuongDo->getCot(),
                               tuongDen->getHang(), tuongDen->getCot())) {
                return true;
            }
        }
    }
    
    return false;
}

bool BanCo::kiemTraNuocDiHopLe(int hangBD, int cotBD, int hangKT, int cotKT) const {
    auto quan = timQuan(hangBD, cotBD);
    if (!quan) return false;
    
    if (!quan->kiemTraNuocDi(*this, hangKT, cotKT)) {
        return false;
    }
    
    // Mo phong nuoc di de kiem tra khong lam Tuong phe minh bi chieu / khong lo mat Tuong
    auto quanBiAn = timQuan(hangKT, cotKT);
    
    // Thuc hien mo phong
    const_cast<QuanCo*>(quan.get())->datViTri(hangKT, cotKT);
    if (quanBiAn) {
        const_cast<QuanCo*>(quanBiAn.get())->datDaBiAn(true);
    }
    
    bool biChieu = kiemTraChieuTuong() || kiemTraChieu(quan->getMau());
    
    // Hoan nguyen mo phong
    const_cast<QuanCo*>(quan.get())->datViTri(hangBD, cotBD);
    if (quanBiAn) {
        const_cast<QuanCo*>(quanBiAn.get())->datDaBiAn(false);
    }
    
    return !biChieu;
}

bool BanCo::diChuyen(int hangBD, int cotBD, int hangKT, int cotKT) {
    auto quan = timQuan(hangBD, cotBD);
    if (!quan || quan->getMau() != luotChoi) {
        return false;
    }
    
    if (!kiemTraNuocDiHopLe(hangBD, cotBD, hangKT, cotKT)) {
        return false;
    }
    
    auto quanBiAn = timQuan(hangKT, cotKT);
    
    // Luu lich su nuoc di
    lichSuNuocDi.emplace_back(hangBD, cotBD, hangKT, cotKT, quanBiAn, quan->layTen(), quan->getMau());
    lichSuRedo.clear(); // Xoa Redo khi co nuoc di moi
    
    // Thuc hien an quan neu co
    if (quanBiAn) {
        quanBiAn->datDaBiAn(true);
    }
    
    // Di chuyen quan
    quan->datViTri(hangKT, cotKT);
    
    // Chuyen luot choi
    luotChoi = (luotChoi == Mau::DO) ? Mau::DEN : Mau::DO;
    
    // Kiem tra het co (chieu bi hoac bi nuoc)
    if (!coNuocDiHopLe(luotChoi)) {
        ketThuc = true;
        nguoiThang = (luotChoi == Mau::DO) ? Mau::DEN : Mau::DO;
    }
    
    return true;
}

bool BanCo::coNuocDiHopLe(Mau mau) const {
    for (const auto& quan : cacQuan) {
        if (!quan->getDaBiAn() && quan->getMau() == mau) {
            int hBD = quan->getHang();
            int cBD = quan->getCot();
            for (int h = 0; h < SO_HANG; h++) {
                for (int c = 0; c < SO_COT; c++) {
                    if (kiemTraNuocDiHopLe(hBD, cBD, h, c)) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool BanCo::hoanTac() {
    if (lichSuNuocDi.empty()) return false;
    
    NuocDi nuocDiCuoi = lichSuNuocDi.back();
    lichSuNuocDi.pop_back();
    
    auto quan = timQuan(nuocDiCuoi.hangKetThuc, nuocDiCuoi.cotKetThuc);
    if (quan) {
        quan->datViTri(nuocDiCuoi.hangBatDau, nuocDiCuoi.cotBatDau);
    }
    
    if (nuocDiCuoi.quanBiAn) {
        nuocDiCuoi.quanBiAn->datDaBiAn(false);
    }
    
    lichSuRedo.push_back(nuocDiCuoi);
    luotChoi = (luotChoi == Mau::DO) ? Mau::DEN : Mau::DO;
    ketThuc = false;
    return true;
}

bool BanCo::diTiep() {
    if (lichSuRedo.empty()) return false;
    
    NuocDi nd = lichSuRedo.back();
    lichSuRedo.pop_back();
    
    auto quan = timQuan(nd.hangBatDau, nd.cotBatDau);
    if (!quan) return false;
    
    if (nd.quanBiAn) {
        nd.quanBiAn->datDaBiAn(true);
    }
    
    quan->datViTri(nd.hangKetThuc, nd.cotKetThuc);
    lichSuNuocDi.push_back(nd);
    
    luotChoi = (luotChoi == Mau::DO) ? Mau::DEN : Mau::DO;
    
    if (!coNuocDiHopLe(luotChoi)) {
        ketThuc = true;
        nguoiThang = (luotChoi == Mau::DO) ? Mau::DEN : Mau::DO;
    }
    
    return true;
}

void BanCo::dauHang(Mau mauDauHang) {
    ketThuc = true;
    nguoiThang = (mauDauHang == Mau::DO) ? Mau::DEN : Mau::DO;
}

void BanCo::xinHoa() {
    ketThuc = true;
    nguoiThang = Mau::HOA;
}

NuocDi BanCo::getNuocDiCuoi() const {
    if (lichSuNuocDi.empty()) return NuocDi();
    return lichSuNuocDi.back();
}

std::string BanCo::layMoTaNuocDi(const NuocDi& nd) const {
    if (nd.hangBatDau < 0) return "";
    std::string phe = (nd.mauQuan == Mau::DO) ? "Do" : "Den";
    std::string text = phe + ": " + nd.tenQuan + " (" + 
                       std::to_string(nd.hangBatDau) + "," + std::to_string(nd.cotBatDau) + ") -> (" +
                       std::to_string(nd.hangKetThuc) + "," + std::to_string(nd.cotKetThuc) + ")";
    if (nd.quanBiAn) {
        text += " [An " + nd.quanBiAn->layTen() + "]";
    }
    return text;
}

bool BanCo::luuFile(const std::string& duongDan) const {
    std::ofstream file(duongDan);
    if (!file.is_open()) return false;
    
    file << (luotChoi == Mau::DO ? "DO" : "DEN") << "\n";
    file << (ketThuc ? "1" : "0") << "\n";
    file << (nguoiThang == Mau::DO ? "DO" : (nguoiThang == Mau::DEN ? "DEN" : "HOA")) << "\n";
    
    // Luu danh sach quan co
    file << cacQuan.size() << "\n";
    for (const auto& q : cacQuan) {
        file << q->layTen() << " "
             << (q->getMau() == Mau::DO ? "DO" : "DEN") << " "
             << q->getHang() << " "
             << q->getCot() << " "
             << (q->getDaBiAn() ? "1" : "0") << "\n";
    }
    
    // Luu so luong nuoc di lich su
    file << lichSuNuocDi.size() << "\n";
    for (const auto& nd : lichSuNuocDi) {
        file << nd.hangBatDau << " " << nd.cotBatDau << " "
             << nd.hangKetThuc << " " << nd.cotKetThuc << " "
             << nd.tenQuan << " "
             << (nd.mauQuan == Mau::DO ? "DO" : "DEN") << "\n";
    }
    
    return true;
}

bool BanCo::docFile(const std::string& duongDan) {
    std::ifstream file(duongDan);
    if (!file.is_open()) return false;
    
    std::string strLuot, strKetThuc, strThang;
    if (!(file >> strLuot >> strKetThuc >> strThang)) return false;
    
    luotChoi = (strLuot == "DO") ? Mau::DO : Mau::DEN;
    ketThuc = (strKetThuc == "1");
    if (strThang == "DO") nguoiThang = Mau::DO;
    else if (strThang == "DEN") nguoiThang = Mau::DEN;
    else nguoiThang = Mau::HOA;
    
    size_t soQuan;
    if (!(file >> soQuan)) return false;
    
    cacQuan.clear();
    lichSuNuocDi.clear();
    lichSuRedo.clear();
    
    for (size_t i = 0; i < soQuan; ++i) {
        std::string ten, mauStr;
        int h, c, daBiAn;
        file >> ten >> mauStr >> h >> c >> daBiAn;
        Mau mau = (mauStr == "DO") ? Mau::DO : Mau::DEN;
        
        std::shared_ptr<QuanCo> q = nullptr;
        if (ten == "Xe") q = std::make_shared<Xe>(h, c, mau);
        else if (ten == "Ma") q = std::make_shared<Ma>(h, c, mau);
        else if (ten == "Voi") q = std::make_shared<Voi>(h, c, mau);
        else if (ten == "Si") q = std::make_shared<Si>(h, c, mau);
        else if (ten == "Tuong") q = std::make_shared<Tuong>(h, c, mau);
        else if (ten == "Phao") q = std::make_shared<Phao>(h, c, mau);
        else if (ten == "Tot") q = std::make_shared<Tot>(h, c, mau);
        
        if (q) {
            q->datDaBiAn(daBiAn == 1);
            cacQuan.push_back(q);
        }
    }
    
    size_t soNuoc;
    if (file >> soNuoc) {
        for (size_t i = 0; i < soNuoc; ++i) {
            int hbd, cbd, hkt, ckt;
            std::string ten, mauStr;
            file >> hbd >> cbd >> hkt >> ckt >> ten >> mauStr;
            Mau mau = (mauStr == "DO") ? Mau::DO : Mau::DEN;
            lichSuNuocDi.emplace_back(hbd, cbd, hkt, ckt, nullptr, ten, mau);
        }
    }
    
    return true;
}
