#include "BanCo.h"
#include "Xe.h"
#include "Ma.h"
#include "Voi.h"
#include "Phao.h"
#include "Tuong.h"
#include "Si.h"
#include "Tot.h"
#include <algorithm>

BanCo::BanCo() : luotChoi(Mau::DO), ketThuc(false), nguoiThang(Mau::DO) {
    khoiTaoBanCo();
}

void BanCo::khoiTaoBanCo() {
    cacQuan.clear();
    lichSuNuocDi.clear();
    
    // Quan Do (hang duoi - hang 9)
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
    
    // Quan Den (hang tren - hang 0)
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
    // Tim vi tri tuong
    std::shared_ptr<QuanCo> tuong = nullptr;
    for (const auto& quan : cacQuan) {
        if (!quan->getDaBiAn() && quan->getMau() == mauTuong && 
            quan->layTen() == "Tuong") {
            tuong = quan;
            break;
        }
    }
    
    if (!tuong) return false;
    
    int hangTuong = tuong->getHang();
    int cotTuong = tuong->getCot();
    
    // Kiem tra tat ca quan dich co the an tuong khong
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
    // Kiem tra tuong doi dien (luat dac biet)
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

bool BanCo::diChuyen(int hangBD, int cotBD, int hangKT, int cotKT) {
    auto quan = timQuan(hangBD, cotBD);
    if (!quan || quan->getMau() != luotChoi) {
        return false;
    }
    
    if (!quan->kiemTraNuocDi(*this, hangKT, cotKT)) {
        return false;
    }
    
    // Luu nuoc di
    auto quanBiAn = timQuan(hangKT, cotKT);
    lichSuNuocDi.emplace_back(hangBD, cotBD, hangKT, cotKT, quanBiAn);
    
    // Thuc hien nuoc di
    if (quanBiAn) {
        quanBiAn->datDaBiAn(true);
    }
    
    quan->datViTri(hangKT, cotKT);
    
    // Cap nhat trang thai Tot neu can
    if (quan->layTen() == "Tot") {
        std::static_pointer_cast<Tot>(quan)->capNhatTrangThaiQuaSong();
    }
    
    // Kiem tra chieu tuong
    if (kiemTraChieuTuong() || kiemTraChieu(luotChoi)) {
        // Nuoc di khong hop le - hoan tac
        hoanTac();
        return false;
    }
    
    // Chuyen luot choi
    luotChoi = (luotChoi == Mau::DO) ? Mau::DEN : Mau::DO;
    
    // Kiem tra ket thuc van co
    if (kiemTraChieu(luotChoi)) {
        if (!coNuocDiHopLe(luotChoi)) {
            ketThuc = true;
            nguoiThang = (luotChoi == Mau::DO) ? Mau::DEN : Mau::DO;
        }
    }
    
    return true;
}

bool BanCo::coNuocDiHopLe(Mau mau) const {
    for (const auto& quan : cacQuan) {
        if (!quan->getDaBiAn() && quan->getMau() == mau) {
            for (int h = 0; h < SO_HANG; h++) {
                for (int c = 0; c < SO_COT; c++) {
                    if (quan->kiemTraNuocDi(*this, h, c)) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

void BanCo::hoanTac() {
    if (lichSuNuocDi.empty()) return;
    
    NuocDi nuocDiCuoi = lichSuNuocDi.back();
    lichSuNuocDi.pop_back();
    
    auto quan = timQuan(nuocDiCuoi.hangKetThuc, nuocDiCuoi.cotKetThuc);
    if (quan) {
        quan->datViTri(nuocDiCuoi.hangBatDau, nuocDiCuoi.cotBatDau);
    }
    
    if (nuocDiCuoi.quanBiAn) {
        nuocDiCuoi.quanBiAn->datDaBiAn(false);
    }
    
    luotChoi = (luotChoi == Mau::DO) ? Mau::DEN : Mau::DO;
}

void BanCo::ve(sf::RenderWindow& window, sf::Font& font) const {
    // Implementation will be in GameManager
}
