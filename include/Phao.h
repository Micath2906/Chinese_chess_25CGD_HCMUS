#pragma once
#include "QuanCo.h"

class Phao : public QuanCo {
public:
    Phao(int hang, int cot, Mau mau);
    
    bool kiemTraNuocDi(const BanCo& banCo, int hangMoi, int cotMoi) const override;
    std::string layTen() const override { return "Phao"; }
    std::string layKyHieu() const override { return "P"; }
    int layGiaTri() const override { return 450; }
    int layDiemViTri() const override { return (cot == 4) ? 25 : 0; }
    std::shared_ptr<QuanCo> clone() const override { return std::make_shared<Phao>(*this); }
};
