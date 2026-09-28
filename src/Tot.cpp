#include "Tot.h"
#include "BanCo.h"
#include <cmath>

Tot::Tot(int hang, int cot, Mau mau) 
    : QuanCo(hang, cot, mau), daQuaSong(false) {
    capNhatTrangThaiQuaSong();
}

void Tot::capNhatTrangThaiQuaSong() {
    if (mau == Mau::DO) {
        daQuaSong = hang < 5;
    } else {
        daQuaSong = hang >= 5;
    }
}

bool Tot::kiemTraNuocDi(const BanCo& banCo, int hangMoi, int cotMoi) const {
    if (!banCo.trongBanCo(hangMoi, cotMoi)) {
        return false;
    }
    
    if (hangMoi == hang && cotMoi == cot) {
        return false;
    }
    
    int dh = hangMoi - hang;
    int dc = abs(cotMoi - cot);
    
    // Tot chua qua song
    if (!daQuaSong) {
        // Chi di thang ve phia truoc
        if (mau == Mau::DO) {
            if (dh != -1 || dc != 0) return false;
        } else {
            if (dh != 1 || dc != 0) return false;
        }
    } else {
        // Da qua song: di thang hoac ngang 1 o
        if (abs(dh) + dc != 1) {
            return false;
        }
        
        // Khong duoc lui
        if (mau == Mau::DO && dh > 0) return false;
        if (mau == Mau::DEN && dh < 0) return false;
    }
    
    // Kiem tra quan tai dich
    auto quanTaiDich = banCo.timQuan(hangMoi, cotMoi);
    if (quanTaiDich != nullptr && quanTaiDich->getMau() == this->mau) {
        return false;
    }
    
    return true;
}
