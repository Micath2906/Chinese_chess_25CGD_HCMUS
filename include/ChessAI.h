#pragma once
#include "BanCo.h"
#include <vector>

enum class DoKho {
    DE,
    TRUNG_BINH,
    KHO
};

class ChessAI {
public:
    static NuocDi timNuocDi(BanCo& banCo, DoKho doKho, Mau mauAI);

private:
    static int danhGiaTheTran(const BanCo& banCo, Mau mauAI);
    static int layGiaTriQuan(const std::shared_ptr<QuanCo>& quan);
    static int layDiemViTri(const std::shared_ptr<QuanCo>& quan);

    static int minimax(BanCo& banCo, int doSau, int alpha, int beta, bool laLuotAI, Mau mauAI);
    static std::vector<NuocDi> layTatCaNuocDiHopLe(const BanCo& banCo, Mau mau);
};
