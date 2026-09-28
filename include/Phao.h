#pragma once
#include "QuanCo.h"

class Phao : public QuanCo {
public:
    Phao(int hang, int cot, Mau mau);
    
    bool kiemTraNuocDi(const BanCo& banCo, int hangMoi, int cotMoi) const override;
    std::string layTen() const override { return "Phao"; }
    std::string layKyHieu() const override { return "P"; }
};
