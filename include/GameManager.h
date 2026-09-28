#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "BanCo.h"
#include "Menu.h"

enum class TrangThai {
    MENU_CHINH,
    CHOI_2_NGUOI,
    CHOI_VOI_MAY,
    SETTINGS,
    HUONG_DAN,
    DANG_CHOI
};

class GameManager {
private:
    sf::RenderWindow window;
    sf::Font font;
    
    TrangThai trangThai;
    BanCo banCo;
    Menu menuChinh;
    Menu menuSettings;
    
    // Game state
    std::shared_ptr<QuanCo> quanDangChon;
    std::vector<sf::Vector2i> cacNuocDiHopLe;
    
    // UI parameters
    float khoangCachBanCo;
    float kichThuocO;
    float offsetX;
    float offsetY;
    
    // Audio
    sf::SoundBuffer bufferDiChuyen;
    sf::SoundBuffer bufferAnQuan;
    sf::Sound amThanh;
    
    bool amThanhBat;
    
public:
    GameManager();
    
    void khoiTao();
    void chay();
    
private:
    // Khoi tao
    void khoiTaoMenu();
    void khoiTaoAmThanh();
    
    // Xu ly su kien
    void xuLySuKien();
    void xuLyClickChuot(const sf::Vector2i& viTri);
    
    // Chuyen doi toa do
    sf::Vector2i chuyenDoiToaDoManHinhThanhBanCo(const sf::Vector2i& viTriChuot);
    sf::Vector2f chuyenDoiToaDoBanCoThanhManHinh(int hang, int cot);
    
    // Ve
    void ve();
    void veBanCo();
    void veQuanCo();
    void veHighlight();
    void veThongTin();
    void veMenu();
    
    // Game logic
    void batDauTroChoiMoi(bool cheDo2Nguoi);
    void chonQuan(int hang, int cot);
    void diChuyenQuan(int hang, int cot);
    void tinhCacNuocDiHopLe();
    
    // Menu actions
    void veMenuChinh();
    void veMenuSettings();
    void quayLaiMenu();
};
