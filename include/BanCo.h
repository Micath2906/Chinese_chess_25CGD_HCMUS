#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "QuanCo.h"

struct NuocDi {
    int hangBatDau;
    int cotBatDau;
    int hangKetThuc;
    int cotKetThuc;
    std::shared_ptr<QuanCo> quanBiAn;
    
    NuocDi(int hbd, int cbd, int hkt, int ckt, std::shared_ptr<QuanCo> qa = nullptr)
        : hangBatDau(hbd), cotBatDau(cbd), hangKetThuc(hkt), cotKetThuc(ckt), quanBiAn(qa) {}
};

class BanCo {
private:
    static const int SO_HANG = 10;
    static const int SO_COT = 9;
    
    std::vector<std::shared_ptr<QuanCo>> cacQuan;
    std::vector<NuocDi> lichSuNuocDi;
    
    Mau luotChoi;
    bool ketThuc;
    Mau nguoiThang;
    
public:
    BanCo();
    
    // Khoi tao
    void khoiTaoBanCo();
    void lamMoi();
    
    // Truy van trang thai
    std::shared_ptr<QuanCo> timQuan(int hang, int cot) const;
    bool trongBanCo(int hang, int cot) const;
    bool coQuanTai(int hang, int cot) const;
    bool duongThangTrong(int hang1, int cot1, int hang2, int cot2) const;
    int demQuanTrenDuong(int hang1, int cot1, int hang2, int cot2) const;
    
    // Luật chơi
    bool kiemTraChieu(Mau mauTuong) const;
    bool kiemTraChieuTuong() const;
    bool diChuyen(int hangBD, int cotBD, int hangKT, int cotKT);
    bool coNuocDiHopLe(Mau mau) const;
    void hoanTac();
    
    // Getters
    Mau getLuotChoi() const { return luotChoi; }
    bool getKetThuc() const { return ketThuc; }
    Mau getNguoiThang() const { return nguoiThang; }
    const std::vector<std::shared_ptr<QuanCo>>& getCacQuan() const { return cacQuan; }
    
    // Hằng số
    static int getSOHANG() { return SO_HANG; }
    static int getSOCOT() { return SO_COT; }
    
    // Ve
    void ve(sf::RenderWindow& window, sf::Font& font) const;
};
