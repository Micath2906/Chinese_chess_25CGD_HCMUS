#pragma once
#include "QuanCo.h"

class Voi : public QuanCo {
public:
    Voi(int hang, int cot, Mau mau);
    
    bool kiemTraNuocDi(const BanCo& banCo, int hangMoi, int cotMoi) const override;
    std::string layTen() const override { return "Voi"; }
    std::string layKyHieu() const override { return "V"; }
    int layGiaTri() const override { return 220; }
    std::shared_ptr<QuanCo> clone() const override { return std::make_shared<Voi>(*this); }
};
