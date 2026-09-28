#include "Xe.h"
#include "BanCo.h"
#include <cmath>

Xe::Xe(int hang, int cot, Mau mau) : QuanCo(hang, cot, mau) {
}

bool Xe::kiemTraNuocDi(const BanCo& banCo, int hangMoi, int cotMoi) const {
    // Kiem tra trong ban co
    if (!banCo.trongBanCo(hangMoi, cotMoi)) {
        return false;
    }
    
    // Khong di chuyen
    if (hangMoi == hang && cotMoi == cot) {
        return false;
    }
    
    // Xe chi di ngang hoac doc
    if (hangMoi != hang && cotMoi != cot) {
        return false;
    }
    
    // Kiem tra quan o vi tri dich
    auto quanTaiDich = banCo.timQuan(hangMoi, cotMoi);
    if (quanTaiDich != nullptr && quanTaiDich->getMau() == this->mau) {
        return false;
    }
    
    // Kiem tra duong di co bi chan khong
    if (!banCo.duongThangTrong(hang, cot, hangMoi, cotMoi)) {
        return false;
    }
    
    return true;
}
