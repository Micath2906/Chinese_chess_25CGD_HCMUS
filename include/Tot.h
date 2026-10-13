#pragma once
#include "QuanCo.h"

class Tot : public QuanCo {
private:
    bool daQuaSong;
    
public:
    Tot(int hang, int cot, Mau mau);
    
    void datViTri(int hangMoi, int cotMoi) override;
    bool kiemTraNuocDi(const BanCo& banCo, int hangMoi, int cotMoi) const override;
    std::string layTen() const override { return "Tot"; }
    std::string layKyHieu() const override { return "t"; }
    int layGiaTri() const override { return daQuaSong ? 200 : 100; }
    int layDiemViTri() const override {
        int diem = 0;
        if (daQuaSong) {
            if (cot >= 2 && cot <= 6) diem += 30;
            if (mau == Mau::DO) diem += (4 - hang) * 15;
            else diem += (hang - 5) * 15;
        }
        return diem;
    }
    std::shared_ptr<QuanCo> clone() const override { return std::make_shared<Tot>(*this); }
    
    void capNhatTrangThaiQuaSong();
};
