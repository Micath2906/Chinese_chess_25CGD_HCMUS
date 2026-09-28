#include "Ma.h"
#include "BanCo.h"
#include <cmath>

Ma::Ma(int hang, int cot, Mau mau) : QuanCo(hang, cot, mau) {
}

bool Ma::kiemTraNuocDi(const BanCo& banCo, int hangMoi, int cotMoi) const {
    if (!banCo.trongBanCo(hangMoi, cotMoi)) {
        return false;
    }
    
    if (hangMoi == hang && cotMoi == cot) {
        return false;
    }
    
    int dh = abs(hangMoi - hang);
    int dc = abs(cotMoi - cot);
    
    // Ma di chu "nhat": 2-1 hoac 1-2
    if (!((dh == 2 && dc == 1) || (dh == 1 && dc == 2))) {
        return false;
    }
    
    // Kiem tra quan tai dich
    auto quanTaiDich = banCo.timQuan(hangMoi, cotMoi);
    if (quanTaiDich != nullptr && quanTaiDich->getMau() == this->mau) {
        return false;
    }
    
    // Kiem tra chan chan Ma (bi)
    int hangChan, cotChan;
    if (dh == 2) {
        hangChan = hang + (hangMoi - hang) / 2;
        cotChan = cot;
    } else {
        hangChan = hang;
        cotChan = cot + (cotMoi - cot) / 2;
    }
    
    if (banCo.coQuanTai(hangChan, cotChan)) {
        return false;
    }
    
    return true;
}
