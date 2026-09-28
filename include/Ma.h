#pragma once
#include "QuanCo.h"

class Ma : public QuanCo {
public:
    Ma(int hang, int cot, Mau mau);
    
    bool kiemTraNuocDi(const BanCo& banCo, int hangMoi, int cotMoi) const override;
    std::string layTen() const override { return "Ma"; }
    std::string layKyHieu() const override { return "M"; }
};
