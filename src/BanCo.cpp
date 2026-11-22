#include "BanCo.h"
#include "QuanCoFactory.h"
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

namespace {
    // Scoped move guard
    class ScopedMoveGuard {
    private:
        std::shared_ptr<QuanCo> quanDi;
        std::shared_ptr<QuanCo> quanBiAn;
        int hangCu;
        int cotCu;
    public:
        ScopedMoveGuard(const std::shared_ptr<QuanCo>& qDi, int hMoi, int cMoi, const std::shared_ptr<QuanCo>& qAn)
            : quanDi(qDi), quanBiAn(qAn), hangCu(qDi->getHang()), cotCu(qDi->getCot()) {
            quanDi->datViTri(hMoi, cMoi);
            if (quanBiAn) quanBiAn->datDaBiAn(true);
        }
        ~ScopedMoveGuard() {
            quanDi->datViTri(hangCu, cotCu);
            if (quanBiAn) quanBiAn->datDaBiAn(false);
        }
    };
}

BanCo::BanCo() : luotChoi(Mau::DO), ketThuc(false), nguoiThang(Mau::DO), hoaDoLapLai(false) {
    khoiTaoBanCo();
}

BanCo::BanCo(const BanCo& other)
    : lichSuNuocDi(other.lichSuNuocDi),
      lichSuRedo(other.lichSuRedo),
      lichSuTrangThai(other.lichSuTrangThai),
      hoaDoLapLai(other.hoaDoLapLai),
      luotChoi(other.luotChoi),
      ketThuc(other.ketThuc),
      nguoiThang(other.nguoiThang) {
    cacQuan.reserve(other.cacQuan.size());
    for (const auto& q : other.cacQuan) {
        if (q) {
            cacQuan.push_back(q->clone()); // Clone piece
        }
    }
}

BanCo& BanCo::operator=(const BanCo& other) {
    if (this != &other) {
        lichSuNuocDi = other.lichSuNuocDi;
        lichSuRedo = other.lichSuRedo;
        lichSuTrangThai = other.lichSuTrangThai;
        hoaDoLapLai = other.hoaDoLapLai;
        luotChoi = other.luotChoi;
        ketThuc = other.ketThuc;
        nguoiThang = other.nguoiThang;
        cacQuan.clear();
        cacQuan.reserve(other.cacQuan.size());
        for (const auto& q : other.cacQuan) {
            if (q) {
                cacQuan.push_back(q->clone());
            }
        }
    }
    return *this;
}

void BanCo::khoiTaoBanCo() {
    cacQuan.clear();
    lichSuNuocDi.clear();
    lichSuRedo.clear();
    lichSuTrangThai.clear();
    hoaDoLapLai = false;
    
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

    lichSuTrangThai.push_back(layChuoiTrangThai());
}

void BanCo::khoiTao2XeVa2Voi() {
    cacQuan.clear();
    lichSuNuocDi.clear();
    lichSuRedo.clear();
    lichSuTrangThai.clear();
    hoaDoLapLai = false;

    // Setup 2 Xe and 2 Voi
    cacQuan.push_back(std::make_shared<Xe>(9, 0, Mau::DO));
    cacQuan.push_back(std::make_shared<Voi>(9, 2, Mau::DO));
    cacQuan.push_back(std::make_shared<Xe>(0, 8, Mau::DEN));
    cacQuan.push_back(std::make_shared<Voi>(0, 6, Mau::DEN));

    luotChoi = Mau::DO;
    ketThuc = false;
    nguoiThang = Mau::DO;
    lichSuTrangThai.push_back(layChuoiTrangThai());
}

void BanCo::lamMoi() {
    khoiTaoBanCo();
    luotChoi = Mau::DO;
    ketThuc = false;
    nguoiThang = Mau::DO;
    hoaDoLapLai = false;
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
    // Find general
    std::shared_ptr<QuanCo> tuong = nullptr;
    for (const auto& quan : cacQuan) {
        if (!quan->getDaBiAn() && quan->getMau() == mauTuong && quan->laTuong()) {
            tuong = quan;
            break;
        }
    }
    
    if (!tuong) return false;
    
    int hangTuong = tuong->getHang();
    int cotTuong = tuong->getCot();
    
    // Check enemy attacks
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
    // Check flying general
    std::shared_ptr<QuanCo> tuongDo = nullptr;
    std::shared_ptr<QuanCo> tuongDen = nullptr;
    
    for (const auto& quan : cacQuan) {
        if (!quan->getDaBiAn() && quan->laTuong()) {
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
    
    // In Chinese Chess / Xiangqi, general cannot be captured directly
    auto quanDich = timQuan(hangKT, cotKT);
    if (quanDich && quanDich->laTuong()) {
        return false;
    }

    if (!quan->kiemTraNuocDi(*this, hangKT, cotKT)) {
        return false;
    }
    
    // Simulate move
    auto quanBiAn = timQuan(hangKT, cotKT);
    ScopedMoveGuard guard(quan, hangKT, cotKT, quanBiAn);
    
    bool biChieu = kiemTraChieuTuong() || kiemTraChieu(quan->getMau());
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
    lichSuRedo.clear(); // Clear redo
    
    // Thuc hien an quan neu co
    if (quanBiAn) {
        quanBiAn->datDaBiAn(true);
    }
    
    // Di chuyen quan
    quan->datViTri(hangKT, cotKT);
    
    // Chuyen luot choi
    luotChoi = (luotChoi == Mau::DO) ? Mau::DEN : Mau::DO;
    
    // Luu trang thai ban co de kiem tra lap nuoc
    std::string stateHienTai = layChuoiTrangThai();
    lichSuTrangThai.push_back(stateHienTai);

    // Kiem tra luat lap lai 3 nuoc -> Xu hoa
    if (kiemTraLapLai3Nuoc()) {
        ketThuc = true;
        nguoiThang = Mau::HOA;
        hoaDoLapLai = true;
    }
    // Check checkmate or stalemate
    else if (!coNuocDiHopLe(luotChoi)) {
        ketThuc = true;
        if (kiemTraChieu(luotChoi)) {
            // Đang bị chiếu mà không còn nước đi hợp lệ -> Chiếu bí! Bên chiếu thắng
            nguoiThang = (luotChoi == Mau::DO) ? Mau::DEN : Mau::DO;
        } else {
            // Không bị chiếu nhưng không còn nước đi hợp lệ -> Xử hòa
            nguoiThang = Mau::HOA;
        }
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
    if (!lichSuTrangThai.empty()) {
        lichSuTrangThai.pop_back();
    }
    hoaDoLapLai = false;
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
    
    std::string stateHienTai = layChuoiTrangThai();
    lichSuTrangThai.push_back(stateHienTai);
    if (kiemTraLapLai3Nuoc()) {
        ketThuc = true;
        nguoiThang = Mau::HOA;
        hoaDoLapLai = true;
    } else if (!coNuocDiHopLe(luotChoi)) {
        ketThuc = true;
        if (kiemTraChieu(luotChoi)) {
            nguoiThang = (luotChoi == Mau::DO) ? Mau::DEN : Mau::DO;
        } else {
            nguoiThang = Mau::HOA;
        }
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

static std::string translatePieceName(const std::string& vnName, bool tiengViet) {
    if (tiengViet) {
        if (vnName == "Tuong" || vnName == "Tướng") return "Tướng";
        if (vnName == "Si" || vnName == "Sĩ") return "Sĩ";
        if (vnName == "Voi" || vnName == "Tượng") return "Tượng";
        if (vnName == "Xe") return "Xe";
        if (vnName == "Phao" || vnName == "Pháo") return "Pháo";
        if (vnName == "Ma" || vnName == "Mã") return "Mã";
        if (vnName == "Tot" || vnName == "Tốt") return "Tốt";
        return vnName;
    } else {
        if (vnName == "Tuong" || vnName == "Tướng") return "General";
        if (vnName == "Si" || vnName == "Sĩ") return "Advisor";
        if (vnName == "Voi" || vnName == "Tượng") return "Elephant";
        if (vnName == "Xe") return "Chariot";
        if (vnName == "Phao" || vnName == "Pháo") return "Cannon";
        if (vnName == "Ma" || vnName == "Mã") return "Horse";
        if (vnName == "Tot" || vnName == "Tốt") return "Soldier";
        return vnName;
    }
}

std::string BanCo::layMoTaNuocDi(const NuocDi& nd, bool tiengViet) const {
    if (nd.hangBatDau < 0) return "";
    std::string phe = (nd.mauQuan == Mau::DO) 
        ? (tiengViet ? "Đỏ" : "Red") 
        : (tiengViet ? "Đen" : "Black");
    std::string tenQ = translatePieceName(nd.tenQuan, tiengViet);
    std::string text = phe + ": " + tenQ + " (" + 
                       std::to_string(nd.hangBatDau) + "," + std::to_string(nd.cotBatDau) + ") -> (" +
                       std::to_string(nd.hangKetThuc) + "," + std::to_string(nd.cotKetThuc) + ")";
    if (nd.quanBiAn) {
        std::string tenAn = translatePieceName(nd.quanBiAn->layTen(), tiengViet);
        if (tiengViet) {
            text += " [Ăn " + tenAn + "]";
        } else {
            text += " [Captures " + tenAn + "]";
        }
    }
    return text;
}

std::string BanCo::layMoTaNuocDiNgan(const NuocDi& nd, bool tiengViet) const {
    if (nd.hangBatDau < 0) return "";
    std::string tenQ = translatePieceName(nd.tenQuan, tiengViet);
    std::string sep = nd.quanBiAn ? "x" : "->";
    return tenQ + " " + std::to_string(nd.hangBatDau) + "," + std::to_string(nd.cotBatDau) + 
           sep + std::to_string(nd.hangKetThuc) + "," + std::to_string(nd.cotKetThuc);
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
        
        // Create piece
        std::shared_ptr<QuanCo> q = QuanCoFactory::taoQuanCo(ten, h, c, mau);
        
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
    
    lichSuTrangThai.clear();
    hoaDoLapLai = false;
    lichSuTrangThai.push_back(layChuoiTrangThai());

    return true;
}

std::string BanCo::layChuoiTrangThai() const {
    std::string s(91, '.');
    for (const auto& q : cacQuan) {
        if (!q || q->getDaBiAn()) continue;
        int idx = q->getHang() * 9 + q->getCot();
        if (idx >= 0 && idx < 90) {
            char c = '?';
            std::string ten = q->layTen();
            if (ten == "Tuong") c = 'K';
            else if (ten == "Si") c = 'A';
            else if (ten == "Voi") c = 'B';
            else if (ten == "Xe") c = 'R';
            else if (ten == "Phao") c = 'C';
            else if (ten == "Ma") c = 'N';
            else if (ten == "Tot") c = 'P';
            
            if (q->getMau() == Mau::DEN) {
                c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            }
            s[idx] = c;
        }
    }
    s[90] = (luotChoi == Mau::DO) ? 'R' : 'B';
    return s;
}

int BanCo::demSoLanLapTrangThai(const std::string& state) const {
    int count = 0;
    for (const auto& s : lichSuTrangThai) {
        if (s == state) {
            count++;
        }
    }
    return count;
}

bool BanCo::kiemTraNuocDiLapLaiCuaNguoiChoi(Mau mau) const {
    std::vector<NuocDi> nuocCuaMau;
    for (const auto& nd : lichSuNuocDi) {
        if (nd.mauQuan == mau) {
            nuocCuaMau.push_back(nd);
        }
    }
    if (nuocCuaMau.size() < 6) return false;
    size_t n = nuocCuaMau.size();

    const auto& m1 = nuocCuaMau[n - 6];
    const auto& m2 = nuocCuaMau[n - 5];
    const auto& m3 = nuocCuaMau[n - 4];
    const auto& m4 = nuocCuaMau[n - 3];
    const auto& m5 = nuocCuaMau[n - 2];
    const auto& m6 = nuocCuaMau[n - 1];

    auto sameMove = [](const NuocDi& a, const NuocDi& b) {
        return a.hangBatDau == b.hangBatDau && a.cotBatDau == b.cotBatDau &&
               a.hangKetThuc == b.hangKetThuc && a.cotKetThuc == b.cotKetThuc;
    };

    auto isReverseMove = [](const NuocDi& a, const NuocDi& b) {
        return a.hangBatDau == b.hangKetThuc && a.cotBatDau == b.cotKetThuc &&
               a.hangKetThuc == b.hangBatDau && a.cotKetThuc == b.cotBatDau;
    };

    if (isReverseMove(m1, m2) &&
        sameMove(m1, m3) && sameMove(m2, m4) &&
        sameMove(m1, m5) && sameMove(m2, m6)) {
        return true;
    }
    return false;
}

bool BanCo::kiemTraLapLai3Nuoc() const {
    if (lichSuTrangThai.empty()) return false;
    const std::string& curState = lichSuTrangThai.back();
    if (demSoLanLapTrangThai(curState) >= 3) {
        return true;
    }
    if (kiemTraNuocDiLapLaiCuaNguoiChoi(Mau::DO) || kiemTraNuocDiLapLaiCuaNguoiChoi(Mau::DEN)) {
        return true;
    }
    return false;
}
