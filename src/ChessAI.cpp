#include "ChessAI.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cmath>

int ChessAI::layGiaTriQuan(const std::shared_ptr<QuanCo>& quan) {
    if (!quan || quan->getDaBiAn()) return 0;
    return quan->layGiaTri();
}

int ChessAI::layDiemViTri(const std::shared_ptr<QuanCo>& quan) {
    if (!quan || quan->getDaBiAn()) return 0;
    return quan->layDiemViTri();
}

int ChessAI::danhGiaTheTran(const BanCo& banCo, Mau mauAI) {
    Mau mauDoiThu = (mauAI == Mau::DO) ? Mau::DEN : Mau::DO;
    int diemAI = 0;
    int diemDoiThu = 0;

    // Tìm vị trí Tướng 2 bên để tính cự ly bao vây và săn lùng
    int hTuongDoiThu = (mauDoiThu == Mau::DO) ? 9 : 0;
    int cTuongDoiThu = 4;
    int hTuongAI = (mauAI == Mau::DO) ? 9 : 0;
    int cTuongAI = 4;

    int soSiDoiThu = 0;
    int soTuongDoiThu = 0;
    int soSiAI = 0;
    int soTuongAI = 0;

    for (const auto& quan : banCo.getCacQuan()) {
        if (!quan || quan->getDaBiAn()) continue;
        if (quan->laTuong()) {
            if (quan->getMau() == mauDoiThu) {
                hTuongDoiThu = quan->getHang();
                cTuongDoiThu = quan->getCot();
            } else {
                hTuongAI = quan->getHang();
                cTuongAI = quan->getCot();
            }
        } else if (quan->getMau() == mauDoiThu) {
            std::string ten = quan->layTen();
            if (ten == "Si") soSiDoiThu++;
            else if (ten == "Voi") soTuongDoiThu++;
        } else if (quan->getMau() == mauAI) {
            std::string ten = quan->layTen();
            if (ten == "Si") soSiAI++;
            else if (ten == "Voi") soTuongAI++;
        }
    }

    for (const auto& quan : banCo.getCacQuan()) {
        if (!quan || quan->getDaBiAn()) continue;
        int giaTri = layGiaTriQuan(quan) + layDiemViTri(quan);
        std::string ten = quan->layTen();

        if (quan->getMau() == mauAI) {
            diemAI += giaTri;

            // 1. Quân tấn công áp sát Tướng địch (King-hunting heuristic)
            if (ten == "Xe" || ten == "Phao" || ten == "Ma") {
                int dist = std::abs(quan->getHang() - hTuongDoiThu) + std::abs(quan->getCot() - cTuongDoiThu);
                diemAI += (14 - dist) * 14;

                // Thâm nhập khu vực cung tướng đối phương
                bool apSatCung = (mauDoiThu == Mau::DEN)
                    ? (quan->getHang() <= 3 && quan->getCot() >= 2 && quan->getCot() <= 6)
                    : (quan->getHang() >= 6 && quan->getCot() >= 2 && quan->getCot() <= 6);
                if (apSatCung) {
                    diemAI += 120;
                }
            } else if (ten == "Tot") {
                bool quaSong = (mauAI == Mau::DO) ? (quan->getHang() <= 4) : (quan->getHang() >= 5);
                if (quaSong) {
                    int dist = std::abs(quan->getHang() - hTuongDoiThu) + std::abs(quan->getCot() - cTuongDoiThu);
                    diemAI += 70 + (12 - dist) * 16;
                }
            }
        } else {
            diemDoiThu += giaTri;

            if (ten == "Xe" || ten == "Phao" || ten == "Ma") {
                int dist = std::abs(quan->getHang() - hTuongAI) + std::abs(quan->getCot() - cTuongAI);
                diemDoiThu += (14 - dist) * 14;
            } else if (ten == "Tot") {
                bool quaSong = (mauDoiThu == Mau::DO) ? (quan->getHang() <= 4) : (quan->getHang() >= 5);
                if (quaSong) {
                    int dist = std::abs(quan->getHang() - hTuongAI) + std::abs(quan->getCot() - cTuongAI);
                    diemDoiThu += 70 + (12 - dist) * 16;
                }
            }
        }
    }

    // Phá tan hàng phòng thủ Sĩ - Tượng đối phương
    diemAI += (2 - soSiDoiThu) * 45;
    diemAI += (2 - soTuongDoiThu) * 35;
    diemDoiThu += (2 - soSiAI) * 45;
    diemDoiThu += (2 - soTuongAI) * 35;

    // Thưởng lớn khi tạo thế chiếu tướng
    if (banCo.kiemTraChieu(mauDoiThu)) {
        diemAI += 160;
    }
    if (banCo.kiemTraChieu(mauAI)) {
        diemDoiThu += 160;
    }

    return diemAI - diemDoiThu;
}

std::vector<NuocDi> ChessAI::layTatCaNuocDiHopLe(const BanCo& banCo, Mau mau) {
    std::vector<NuocDi> danhSach;

    for (const auto& quan : banCo.getCacQuan()) {
        if (!quan || quan->getDaBiAn() || quan->getMau() != mau) continue;
        int hBD = quan->getHang();
        int cBD = quan->getCot();
        std::string ten = quan->layTen();

        auto thuThemNuoc = [&](int hKT, int cKT) {
            if (banCo.trongBanCo(hKT, cKT) && banCo.kiemTraNuocDiHopLe(hBD, cBD, hKT, cKT)) {
                auto qAn = banCo.timQuan(hKT, cKT);
                danhSach.emplace_back(hBD, cBD, hKT, cKT, qAn, ten, mau);
            }
        };

        if (ten == "Tuong") {
            static const int dH[] = {1, -1, 0, 0};
            static const int dC[] = {0, 0, 1, -1};
            for (int i = 0; i < 4; ++i) {
                thuThemNuoc(hBD + dH[i], cBD + dC[i]);
            }
            // Lộ mặt tướng
            for (const auto& qDich : banCo.getCacQuan()) {
                if (qDich && !qDich->getDaBiAn() && qDich->getMau() != mau && qDich->laTuong()) {
                    if (qDich->getCot() == cBD) {
                        thuThemNuoc(qDich->getHang(), qDich->getCot());
                    }
                    break;
                }
            }
        } else if (ten == "Si") {
            static const int dH[] = {1, 1, -1, -1};
            static const int dC[] = {1, -1, 1, -1};
            for (int i = 0; i < 4; ++i) {
                thuThemNuoc(hBD + dH[i], cBD + dC[i]);
            }
        } else if (ten == "Voi") {
            static const int dH[] = {2, 2, -2, -2};
            static const int dC[] = {2, -2, 2, -2};
            for (int i = 0; i < 4; ++i) {
                thuThemNuoc(hBD + dH[i], cBD + dC[i]);
            }
        } else if (ten == "Ma") {
            static const int jumps[8][2] = {
                {2, 1}, {2, -1}, {-2, 1}, {-2, -1},
                {1, 2}, {1, -2}, {-1, 2}, {-1, -2}
            };
            for (int i = 0; i < 8; ++i) {
                thuThemNuoc(hBD + jumps[i][0], cBD + jumps[i][1]);
            }
        } else if (ten == "Tot") {
            int stepH = (mau == Mau::DO) ? -1 : 1;
            thuThemNuoc(hBD + stepH, cBD);
            bool quaSong = (mau == Mau::DO) ? (hBD <= 4) : (hBD >= 5);
            if (quaSong) {
                thuThemNuoc(hBD, cBD - 1);
                thuThemNuoc(hBD, cBD + 1);
            }
        } else if (ten == "Xe") {
            static const int dH[] = {1, -1, 0, 0};
            static const int dC[] = {0, 0, 1, -1};
            for (int i = 0; i < 4; ++i) {
                for (int step = 1; step <= 9; ++step) {
                    int nh = hBD + dH[i] * step;
                    int nc = cBD + dC[i] * step;
                    if (!banCo.trongBanCo(nh, nc)) break;
                    thuThemNuoc(nh, nc);
                    if (banCo.coQuanTai(nh, nc)) break;
                }
            }
        } else if (ten == "Phao") {
            static const int dH[] = {1, -1, 0, 0};
            static const int dC[] = {0, 0, 1, -1};
            for (int i = 0; i < 4; ++i) {
                bool coNgoi = false;
                for (int step = 1; step <= 9; ++step) {
                    int nh = hBD + dH[i] * step;
                    int nc = cBD + dC[i] * step;
                    if (!banCo.trongBanCo(nh, nc)) break;
                    if (!coNgoi) {
                        if (!banCo.coQuanTai(nh, nc)) {
                            thuThemNuoc(nh, nc);
                        } else {
                            coNgoi = true;
                        }
                    } else {
                        if (banCo.coQuanTai(nh, nc)) {
                            thuThemNuoc(nh, nc);
                            break;
                        }
                    }
                }
            }
        }
    }

    // Move ordering: ăn quân và tấn công xếp trước để tăng tốc tỉa nhánh Alpha-Beta
    std::sort(danhSach.begin(), danhSach.end(), [&](const NuocDi& a, const NuocDi& b) {
        int vA = a.quanBiAn ? a.quanBiAn->layGiaTri() * 10 : 0;
        int vB = b.quanBiAn ? b.quanBiAn->layGiaTri() * 10 : 0;
        if (a.tenQuan == "Xe" || a.tenQuan == "Phao" || a.tenQuan == "Ma") vA += 30;
        if (b.tenQuan == "Xe" || b.tenQuan == "Phao" || b.tenQuan == "Ma") vB += 30;
        return vA > vB;
    });

    return danhSach;
}

int ChessAI::minimax(BanCo& banCo, int doSau, int alpha, int beta, bool laLuotAI, Mau mauAI) {
    if (banCo.getKetThuc()) {
        if (banCo.getNguoiThang() == mauAI) {
            return 100000 + doSau * 100;
        } else if (banCo.getNguoiThang() == Mau::HOA) {
            return 0;
        } else {
            return -100000 - doSau * 100;
        }
    }

    if (doSau == 0) {
        return danhGiaTheTran(banCo, mauAI);
    }

    Mau mauHienTai = laLuotAI ? mauAI : ((mauAI == Mau::DO) ? Mau::DEN : Mau::DO);
    auto danhSachNuocDi = layTatCaNuocDiHopLe(banCo, mauHienTai);

    if (danhSachNuocDi.empty()) {
        if (banCo.kiemTraChieu(mauHienTai)) {
            // Chiếu bí! Ưu tiên chiếu bí càng nhanh càng tốt
            return laLuotAI ? (-100000 - doSau * 100) : (100000 + doSau * 100);
        } else {
            return 0; // Hết nước đi hợp lệ -> Hòa
        }
    }

    if (laLuotAI) {
        int maxEval = -999999;
        for (const auto& nd : danhSachNuocDi) {
            banCo.diChuyen(nd.hangBatDau, nd.cotBatDau, nd.hangKetThuc, nd.cotKetThuc);
            int eval = minimax(banCo, doSau - 1, alpha, beta, false, mauAI);
            banCo.hoanTac();

            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) break;
        }
        return maxEval;
    } else {
        int minEval = 999999;
        for (const auto& nd : danhSachNuocDi) {
            banCo.diChuyen(nd.hangBatDau, nd.cotBatDau, nd.hangKetThuc, nd.cotKetThuc);
            int eval = minimax(banCo, doSau - 1, alpha, beta, true, mauAI);
            banCo.hoanTac();

            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) break;
        }
        return minEval;
    }
}

NuocDi ChessAI::timNuocDi(BanCo& banCo, DoKho doKho, Mau mauAI) {
    auto danhSachNuocDi = layTatCaNuocDiHopLe(banCo, mauAI);
    if (danhSachNuocDi.empty()) {
        return NuocDi(-1, -1, -1, -1);
    }

    // Thiết lập độ sâu tìm kiếm:
    // Dễ: độ sâu 2, chọn ngẫu nhiên trong top 3 nước tốt
    // Trung bình: độ sâu 3
    // Khó: độ sâu 4
    int doSau = (doKho == DoKho::DE) ? 2 : ((doKho == DoKho::TRUNG_BINH) ? 3 : 4);

    std::vector<std::pair<int, NuocDi>> danhGiaList;
    danhGiaList.reserve(danhSachNuocDi.size());

    const auto& lichSu = banCo.getLichSuNuocDi();
    NuocDi nuocTruocCuaAI;
    if (lichSu.size() >= 2) {
        nuocTruocCuaAI = lichSu[lichSu.size() - 2];
    }

    for (const auto& nd : danhSachNuocDi) {
        banCo.diChuyen(nd.hangBatDau, nd.cotBatDau, nd.hangKetThuc, nd.cotKetThuc);

        int score = 0;
        if (banCo.getKetThuc()) {
            if (banCo.getNguoiThang() == mauAI) {
                score = 100000 + doSau * 100;
            } else if (banCo.getNguoiThang() == Mau::HOA) {
                score = 0;
            } else {
                score = -100000;
            }
        } else {
            score = minimax(banCo, doSau - 1, -999999, 999999, false, mauAI);
        }

        // 1. Phạt nặng nước lặp lại thế cờ (Threefold repetition / lặp nước)
        int repCount = banCo.demSoLanLapTrangThai(banCo.layChuoiTrangThai());
        if (repCount >= 2) {
            score -= 4000;
        }
        if (repCount >= 3) {
            score -= 10000;
        }

        // 2. Phạt nước đi qua lại vô nghĩa (quay lại vị trí vừa đi của cùng quân đó)
        if (nuocTruocCuaAI.hangBatDau != -1 && !nd.quanBiAn) {
            if (nd.hangBatDau == nuocTruocCuaAI.hangKetThuc && nd.cotBatDau == nuocTruocCuaAI.cotKetThuc &&
                nd.hangKetThuc == nuocTruocCuaAI.hangBatDau && nd.cotKetThuc == nuocTruocCuaAI.cotBatDau) {
                score -= 2500;
            }
        }

        // Thêm độ nhiễu cực nhỏ (0-3 điểm) để tránh lặp máy móc khi hòa điểm
        score += (rand() % 4);

        banCo.hoanTac();
        danhGiaList.emplace_back(score, nd);
    }

    std::sort(danhGiaList.begin(), danhGiaList.end(), [](const auto& a, const auto& b) {
        return a.first > b.first;
    });

    if (doKho == DoKho::DE) {
        int maxChoices = std::min(static_cast<int>(danhGiaList.size()), 3);
        int choice = rand() % maxChoices;
        return danhGiaList[choice].second;
    }

    return danhGiaList[0].second;
}
