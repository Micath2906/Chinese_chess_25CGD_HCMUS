#pragma once
#include "QuanCo.h"

class Xe : public QuanCo {
public:
    Xe(int hang, int cot, Mau mau);
    
    bool kiemTraNuocDi(const BanCo& banCo, int hangMoi, int cotMoi) const override;
    std::string layTen() const override { return "Xe"; }
    std::string layKyHieu() const override { return "X"; }
};
