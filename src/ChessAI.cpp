#include "ChessAI.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>

int ChessAI::layGiaTriQuan(const std::shared_ptr<QuanCo>& quan) {
    if (!quan || quan->getDaBiAn()) return 0;
    return quan->layGiaTri();
}

int ChessAI::layDiemViTri(const std::shared_ptr<QuanCo>& quan) {
    if (!quan || quan->getDaBiAn()) return 0;
    return quan->layDiemViTri();
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
