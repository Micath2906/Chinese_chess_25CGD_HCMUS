#include "Si.h"
#include "BanCo.h"
#include <cmath>

Si::Si(int hang, int cot, Mau mau) : QuanCo(hang, cot, mau) {
}

bool Si::kiemTraNuocDi(const BanCo& banCo, int hangMoi, int cotMoi) const {
    if (!banCo.trongBanCo(hangMoi, cotMoi)) {
        return false;
    }
    
    if (hangMoi == hang && cotMoi == cot) {
        return false;
    }
    
    // Si chi duoc o trong cung (3x3)
    if (mau == Mau::DO) {
        if (hangMoi < 7 || hangMoi > 9 || cotMoi < 3 || cotMoi > 5) {
            return false;
        }
    } else {
        if (hangMoi < 0 || hangMoi > 2 || cotMoi < 3 || cotMoi > 5) {
            return false;
        }
    }
    
    int dh = abs(hangMoi - hang);
    int dc = abs(cotMoi - cot);
    
    // Si di cheo 1 o
    if (dh != 1 || dc != 1) {
        return false;
    }

    
    // Kiem tra quan tai dich
    auto quanTaiDich = banCo.timQuan(hangMoi, cotMoi);
    if (quanTaiDich != nullptr && quanTaiDich->getMau() == this->mau) {
        return false;
    }
    
    return true;
}
