#pragma once
#include <SFML/Graphics.hpp>
#include "BanCo.h"
#include "Menu.h"
#include "SoundManager.h"
#include "ChessAI.h"

enum class TrangThai {
    MENU_CHINH,
    SETTINGS,
    HUONG_DAN,
    CHON_PHE_AI,
    DANG_CHOI
};

enum class CheDoChoi {
    HAI_NGUOI,
    VOI_MAY
};

class GameManager {
private:
    sf::RenderWindow window;
    sf::Font font;
    
    TrangThai trangThai;
    CheDoChoi cheDoChoi;
    BanCo banCo;
    SoundManager soundManager;
    
    // Menus
    Menu menuChinh;
    Menu menuSettings;
    Menu menuHuongDan;
    Menu menuChonPhe;
    
    // In-game buttons
    std::vector<std::unique_ptr<Button>> inGameButtons;
    std::vector<std::unique_ptr<Button>> popupButtons;
    
    // Game selection state
    std::shared_ptr<QuanCo> quanDangChon;
    std::vector<sf::Vector2i> cacNuocDiHopLe;
    
    // Board UI parameters
    float kichThuocO;
    float offsetX;
    float offsetY;
    
    // Settings state
    DoKho doKhoAI;
    Mau mauNguoiChoi;
    Mau mauAI;
    float thoiGianVanDau; // 0 = unlimited, 300 = 5m, 600 = 10m, 900 = 15m
    bool hienGoiY;
    
    // Chess clocks
    float thoiGianConDo;
    float thoiGianConDen;
    sf::Clock dtClock;
    
    // AI think state
    bool aiDangSuyNghi;
    float thoiGianChoAI;
    
    // Notifications
    std::string toastMessage;
    float toastTimer;
    
    // End game reason
    std::string lyDoKetThuc;

public:
    GameManager();
    
    void khoiTao();
    void chay();
    
private:
    void khoiTaoCacMenu();
    void khoiTaoNutTrongGame();
    void khoiTaoNutPopup();
    
    // Event handling
    void xuLySuKien();
    void xuLyClickChuot(const sf::Vector2i& viTri);
    void xuLyBanPhim(sf::Keyboard::Key key, bool ctrl);
    
    // Game flow
    void batDauTroChoiMoi(CheDoChoi cheDo);
    void xuLyLuotAI(float dt);
    void capNhatDongHo(float dt);
    
    // In-game actions
    void chonQuan(int hang, int cot);
    void diChuyenQuan(int hang, int cot);
    void tinhCacNuocDiHopLe();
    void thucHienHoanTac();
    void thucHienDiTiep();
    void thucHienDauHang();
    void thucHienXinHoa();
    void thucHienLuuGame();
    void thucHienLoadGame();
    void quayLaiMenu();
    
    // Toast
    void hienToast(const std::string& msg);
    
    // Coordinates conversion
    sf::Vector2i chuyenDoiToaDoManHinhThanhBanCo(const sf::Vector2i& viTriChuot);
    sf::Vector2f chuyenDoiToaDoBanCoThanhManHinh(int hang, int cot);
    
    // Rendering
    void ve();
    void veBanCoTruyenThong();
    void veCacQuanCo();
    void veHighlights();
    void veThanhBenPhai();
    void vePopupKetThuc();
    void veToast();
    
    // Menu rendering
    void veMenuChinh();
    void veMenuSettings();
    void veMenuHuongDan();
    void veMenuChonPhe();
};
