#pragma once
#include "QuanCo.h"

class Xe : public QuanCo {
public:
    Xe(int hang, int cot, Mau mau);
    
    bool kiemTraNuocDi(const BanCo& banCo, int hangMoi, int cotMoi) const override;
    std::string layTen() const override { return "Xe"; }
    std::string layKyHieu() const override { return "X"; }
    int layGiaTri() const override { return 900; }
    int layDiemViTri() const override {
        int diem = 0;
        if (cot >= 3 && cot <= 5) diem += 20;
        if ((mau == Mau::DO && hang <= 2) || (mau == Mau::DEN && hang >= 7)) diem += 30;
        return diem;
    }
    std::shared_ptr<QuanCo> clone() const override { return std::make_shared<Xe>(*this); }
};
