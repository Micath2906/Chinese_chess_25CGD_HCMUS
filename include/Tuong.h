#pragma once
#include "QuanCo.h"

class Tuong : public QuanCo {
public:
    Tuong(int hang, int cot, Mau mau);
    
    bool kiemTraNuocDi(const BanCo& banCo, int hangMoi, int cotMoi) const override;
    std::string layTen() const override { return "Tuong"; }
    std::string layKyHieu() const override { return "T"; }
};
