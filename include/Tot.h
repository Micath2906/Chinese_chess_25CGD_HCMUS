#pragma once
#include "QuanCo.h"

class Tot : public QuanCo {
private:
    bool daQuaSong;
    
public:
    Tot(int hang, int cot, Mau mau);
    
    bool kiemTraNuocDi(const BanCo& banCo, int hangMoi, int cotMoi) const override;
    std::string layTen() const override { return "Tot"; }
    std::string layKyHieu() const override { return "t"; }
    
    void capNhatTrangThaiQuaSong();
};
