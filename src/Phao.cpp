#include "Phao.h"
#include "BanCo.h"

Phao::Phao(int hang, int cot, Mau mau) : QuanCo(hang, cot, mau) {
}

bool Phao::kiemTraNuocDi(const BanCo& banCo, int hangMoi, int cotMoi) const {
    if (!banCo.trongBanCo(hangMoi, cotMoi)) {
        return false;
    }
    
    if (hangMoi == hang && cotMoi == cot) {
        return false;
    }
    
    // Phao chi di ngang hoac doc
    if (hangMoi != hang && cotMoi != cot) {
        return false;
    }
    
    auto quanTaiDich = banCo.timQuan(hangMoi, cotMoi);
    int soQuanGiua = banCo.demQuanTrenDuong(hang, cot, hangMoi, cotMoi);
    
    // Neu di chuyen khong an quan: khong co quan nao o giua
    if (quanTaiDich == nullptr) {
        return soQuanGiua == 0;
    }
    
    // Neu an quan: phai co dung 1 quan lam cau
    if (quanTaiDich->getMau() != this->mau) {
        return soQuanGiua == 1;
    }
    
    return false;
}
