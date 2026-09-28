#include "Voi.h"
#include "BanCo.h"
#include <cmath>

Voi::Voi(int hang, int cot, Mau mau) : QuanCo(hang, cot, mau) {
}

bool Voi::kiemTraNuocDi(const BanCo& banCo, int hangMoi, int cotMoi) const {
    if (!banCo.trongBanCo(hangMoi, cotMoi)) {
        return false;
    }
    
    if (hangMoi == hang && cotMoi == cot) {
        return false;
    }
    
    int dh = abs(hangMoi - hang);
    int dc = abs(cotMoi - cot);
    
    // Voi di cheo dung 2 o
    if (dh != 2 || dc != 2) {
        return false;
    }
    
    // Kiem tra khong vuot song
    if (mau == Mau::DO) {
        if (hangMoi < 5) return false;
    } else {
        if (hangMoi >= 5) return false;
    }
    
    // Kiem tra quan tai dich
    auto quanTaiDich = banCo.timQuan(hangMoi, cotMoi);
    if (quanTaiDich != nullptr && quanTaiDich->getMau() == this->mau) {
        return false;
    }
    
    // Kiem tra mat voi
    int hangGiua = (hang + hangMoi) / 2;
    int cotGiua = (cot + cotMoi) / 2;
    
    if (banCo.coQuanTai(hangGiua, cotGiua)) {
        return false;
    }
    
    return true;
}
