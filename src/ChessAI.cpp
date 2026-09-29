#include "ChessAI.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>

int ChessAI::layGiaTriQuan(const std::shared_ptr<QuanCo>& quan) {
    if (!quan || quan->getDaBiAn()) return 0;
    
    std::string ten = quan->layTen();
    if (ten == "Tuong") return 10000;
    if (ten == "Xe") return 900;
    if (ten == "Phao") return 450;
    if (ten == "Ma") return 400;
    if (ten == "Voi") return 220;
    if (ten == "Si") return 220;
    if (ten == "Tot") {
        int h = quan->getHang();
        bool quaSong = (quan->getMau() == Mau::DO) ? (h < 5) : (h >= 5);
        return quaSong ? 200 : 100;
    }
    return 0;
}

int ChessAI::layDiemViTri(const std::shared_ptr<QuanCo>& quan) {
    if (!quan || quan->getDaBiAn()) return 0;
    
    int h = quan->getHang();
    int c = quan->getCot();
    std::string ten = quan->layTen();
    int diem = 0;

    if (ten == "Ma") {
        // Ma o trung tam manh hon o ria
        if (c >= 2 && c <= 6 && h >= 2 && h <= 7) diem += 25;
        if (c == 0 || c == 8) diem -= 20;
    } else if (ten == "Xe") {
        // Xe chiem trung lo
        if (c >= 3 && c <= 5) diem += 20;
        // Xe xong vao hang sau doi phuong
        if ((quan->getMau() == Mau::DO && h <= 2) || (quan->getMau() == Mau::DEN && h >= 7)) {
            diem += 30;
        }
    } else if (ten == "Phao") {
        // Phao o trung lo
        if (c == 4) diem += 25;
    } else if (ten == "Tot") {
        bool quaSong = (quan->getMau() == Mau::DO) ? (h < 5) : (h >= 5);
        if (quaSong) {
            // Tot ap sat cung
            if (c >= 2 && c <= 6) diem += 30;
            if (quan->getMau() == Mau::DO) diem += (4 - h) * 15;
            else diem += (h - 5) * 15;
        }
    }

    return diem;
}

int ChessAI::danhGiaTheTran(const BanCo& banCo, Mau mauAI) {
    int diemAI = 0;
    int diemDoiThu = 0;

    for (const auto& quan : banCo.getCacQuan()) {
        if (!quan->getDaBiAn()) {
            int giaTri = layGiaTriQuan(quan) + layDiemViTri(quan);
            if (quan->getMau() == mauAI) {
                diemAI += giaTri;
            } else {
                diemDoiThu += giaTri;
            }
        }
    }

    return diemAI - diemDoiThu;
}

std::vector<NuocDi> ChessAI::layTatCaNuocDiHopLe(const BanCo& banCo, Mau mau) {
    std::vector<NuocDi> danhSach;
    for (const auto& quan : banCo.getCacQuan()) {
        if (!quan->getDaBiAn() && quan->getMau() == mau) {
            int hBD = quan->getHang();
            int cBD = quan->getCot();
            for (int h = 0; h < BanCo::getSOHANG(); ++h) {
                for (int c = 0; c < BanCo::getSOCOT(); ++c) {
                    if (banCo.kiemTraNuocDiHopLe(hBD, cBD, h, c)) {
                        auto quanBiAn = banCo.timQuan(h, c);
                        danhSach.emplace_back(hBD, cBD, h, c, quanBiAn);
                    }
                }
            }
        }
    }

    // Move ordering: nuoc an quan xep truoc
    std::sort(danhSach.begin(), danhSach.end(), [&](const NuocDi& a, const NuocDi& b) {
        int vA = a.quanBiAn ? layGiaTriQuan(a.quanBiAn) : 0;
        int vB = b.quanBiAn ? layGiaTriQuan(b.quanBiAn) : 0;
        return vA > vB;
    });

    return danhSach;
}

int ChessAI::minimax(BanCo& banCo, int doSau, int alpha, int beta, bool laLuotAI, Mau mauAI) {
    if (doSau == 0 || banCo.getKetThuc()) {
        return danhGiaTheTran(banCo, mauAI);
    }

    Mau mauHienTai = laLuotAI ? mauAI : ((mauAI == Mau::DO) ? Mau::DEN : Mau::DO);
    auto danhSachNuocDi = layTatCaNuocDiHopLe(banCo, mauHienTai);

    if (danhSachNuocDi.empty()) {
        // Bi nuoc / Chieu bi
        return laLuotAI ? -99999 : 99999;
    }

    if (laLuotAI) {
        int maxEval = -999999;
        for (const auto& nd : danhSachNuocDi) {
            banCo.diChuyen(nd.hangBatDau, nd.cotBatDau, nd.hangKetThuc, nd.cotKetThuc);
            int eval = minimax(banCo, doSau - 1, alpha, beta, false, mauAI);
            banCo.hoanTac();

            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) break; // Alpha-beta cutoff
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
            if (beta <= alpha) break; // Alpha-beta cutoff
        }
        return minEval;
    }
}

NuocDi ChessAI::timNuocDi(BanCo& banCo, DoKho doKho, Mau mauAI) {
    auto danhSachNuocDi = layTatCaNuocDiHopLe(banCo, mauAI);
    if (danhSachNuocDi.empty()) {
        return NuocDi(-1, -1, -1, -1);
    }

    // Do kho DE: chon ngau nhien trong top 3 nuoc di tot
    if (doKho == DoKho::DE) {
        std::vector<std::pair<int, NuocDi>> danhGiaList;
        for (const auto& nd : danhSachNuocDi) {
            banCo.diChuyen(nd.hangBatDau, nd.cotBatDau, nd.hangKetThuc, nd.cotKetThuc);
            int diem = danhGiaTheTran(banCo, mauAI);
            banCo.hoanTac();
            danhGiaList.emplace_back(diem, nd);
        }
        std::sort(danhGiaList.begin(), danhGiaList.end(), [](const auto& a, const auto& b) {
            return a.first > b.first;
        });

        int maxChoices = std::min(static_cast<int>(danhGiaList.size()), 3);
        int choice = rand() % maxChoices;
        return danhGiaList[choice].second;
    }

    int doSau = (doKho == DoKho::TRUNG_BINH) ? 2 : 3;
    int bestScore = -999999;
    NuocDi bestMove = danhSachNuocDi[0];

    for (const auto& nd : danhSachNuocDi) {
        banCo.diChuyen(nd.hangBatDau, nd.cotBatDau, nd.hangKetThuc, nd.cotKetThuc);
        int score = minimax(banCo, doSau - 1, -999999, 999999, false, mauAI);
        banCo.hoanTac();

        if (score > bestScore) {
            bestScore = score;
            bestMove = nd;
        }
    }

    return bestMove;
}
