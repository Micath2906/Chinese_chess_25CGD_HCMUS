#pragma once
#include "QuanCo.h"

class Ma : public QuanCo {
public:
    Ma(int hang, int cot, Mau mau);
    
    bool kiemTraNuocDi(const BanCo& banCo, int hangMoi, int cotMoi) const override;
    std::string layTen() const override { return "Ma"; }
    std::string layKyHieu() const override { return "M"; }
    int layGiaTri() const override { return 400; }
    int layDiemViTri() const override {
        int diem = 0;
        if (cot >= 2 && cot <= 6 && hang >= 2 && hang <= 7) diem += 25;
        if (cot == 0 || cot == 8) diem -= 20;
        return diem;
    }
    std::shared_ptr<QuanCo> clone() const override { return std::make_shared<Ma>(*this); }
};
