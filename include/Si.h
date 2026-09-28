#pragma once
#include "QuanCo.h"

class Si : public QuanCo {
public:
    Si(int hang, int cot, Mau mau);
    
    bool kiemTraNuocDi(const BanCo& banCo, int hangMoi, int cotMoi) const override;
    std::string layTen() const override { return "Si"; }
    std::string layKyHieu() const override { return "S"; }
};
