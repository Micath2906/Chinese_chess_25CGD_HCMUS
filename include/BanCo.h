#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <string>
#include "QuanCo.h"

struct NuocDi {
    int hangBatDau;
    int cotBatDau;
    int hangKetThuc;
    int cotKetThuc;
    std::shared_ptr<QuanCo> quanBiAn;
    std::string tenQuan;
    Mau mauQuan;
    
    NuocDi() 
        : hangBatDau(-1), cotBatDau(-1), hangKetThuc(-1), cotKetThuc(-1), 
          quanBiAn(nullptr), tenQuan(""), mauQuan(Mau::DO) {}

    NuocDi(int hbd, int cbd, int hkt, int ckt, std::shared_ptr<QuanCo> qa = nullptr, 
           const std::string& ten = "", Mau mau = Mau::DO)
        : hangBatDau(hbd), cotBatDau(cbd), hangKetThuc(hkt), cotKetThuc(ckt), 
          quanBiAn(qa), tenQuan(ten), mauQuan(mau) {}
};

class BanCo {
private:
    static const int SO_HANG = 10;
    static const int SO_COT = 9;
    
    std::vector<std::shared_ptr<QuanCo>> cacQuan;
    std::vector<NuocDi> lichSuNuocDi;
    std::vector<NuocDi> lichSuRedo;
    
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
    bool kiemTraNuocDiHopLe(int hangBD, int cotBD, int hangKT, int cotKT) const;
    bool diChuyen(int hangBD, int cotBD, int hangKT, int cotKT);
    bool coNuocDiHopLe(Mau mau) const;
    
    // Hoàn tác & Đi tiếp (Undo & Redo)
    bool hoanTac();
    bool diTiep();
    bool coTheHoanTac() const { return !lichSuNuocDi.empty(); }
    bool coTheDiTiep() const { return !lichSuRedo.empty(); }
    
    // Kết thúc ván đấu
    void dauHang(Mau mauDauHang);
    void xinHoa();
    
    // Lưu & Tải game
    bool luuFile(const std::string& duongDan) const;
    bool docFile(const std::string& duongDan);
    
    // Getters & Setters
    Mau getLuotChoi() const { return luotChoi; }
    void setLuotChoi(Mau m) { luotChoi = m; }
    bool getKetThuc() const { return ketThuc; }
    void setKetThuc(bool kt) { ketThuc = kt; }
    Mau getNguoiThang() const { return nguoiThang; }
    void setNguoiThang(Mau nt) { nguoiThang = nt; }
    const std::vector<std::shared_ptr<QuanCo>>& getCacQuan() const { return cacQuan; }
    const std::vector<NuocDi>& getLichSuNuocDi() const { return lichSuNuocDi; }
    NuocDi getNuocDiCuoi() const;
    
    // Hằng số
    static int getSOHANG() { return SO_HANG; }
    static int getSOCOT() { return SO_COT; }
    
    // Mô tả nước đi
    std::string layMoTaNuocDi(const NuocDi& nd) const;
};
