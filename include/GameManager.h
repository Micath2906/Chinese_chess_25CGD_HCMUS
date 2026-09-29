#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <string>

#include "BanCo.h"
#include "Menu.h"
#include "SoundManager.h"
#include "ChessAI.h"
#include "Localization.h"
#include "KeyConfig.h"
#include "TextInput.h"

enum class TrangThai {
    MENU_CHINH,
    PLAY_SUBMENU,
    PVP_SETUP,
    PVAI_SETUP,
    LOAD_MENU,
    SETTINGS_MENU,
    KEYBINDING_MENU,
    INTRODUCTION_MENU,
    DANG_CHOI
};

enum class CheDoChoi {
    DEMO_2XE_2VOI,
    HAI_NGUOI,
    VOI_MAY
};

struct SaveSlotInfo {
    bool tonTai;
    std::string thoiGian;
    std::string tenCheDo;
    std::string tenDo;
    std::string tenDen;
    int soNuoc;
    Mau luot;
};

class GameManager {
private:
    sf::RenderWindow window;
    sf::Font font;
    
    TrangThai trangThai;
    CheDoChoi cheDoChoi;
    BanCo banCo;
    SoundManager soundManager;
    KeyConfig keyConfig;
    KeyAction rebindingAction;
    
    // Menus
    Menu menuChinh;
    Menu menuPlaySub;
    Menu menuPvpSetup;
    Menu menuPvaiSetup;
    Menu menuLoad;
    Menu menuSettings;
    Menu menuKeybinding;
    Menu menuIntro;
    
    // In-game buttons & popup buttons
    std::vector<std::unique_ptr<Button>> inGameButtons;
    std::vector<std::unique_ptr<Button>> popupButtons;
    
    // Text inputs
    std::unique_ptr<TextInput> inputP1Name;
    std::unique_ptr<TextInput> inputP2Name;
    std::unique_ptr<TextInput> inputAiPlayerName;
    
    // PvP setup state
    int p1CharIndex;
    int p2CharIndex;
    int pvpFirstPlayer; // 1 = P1 Red (goes first), 2 = P2 Red (goes first)
    std::string pvpRollBanner;
    
    // PvAI setup state
    int aiPlayerCharIndex;
    DoKho aiDifficulty;
    int aiFirstChoice; // 0 = Player Red, 1 = AI Red, 2 = Random
    Mau aiPlayerSide;
    std::string aiRollBanner;
    
    // Active match player profiles
    std::string tenNguoiChoiDo;
    std::string tenNguoiChoiDen;
    int nhanVatDo;
    int nhanVatDen;
    
    // Save slots
    int selectedSlot;
    std::vector<SaveSlotInfo> slotInfos;
    
    // Game selection state
    std::shared_ptr<QuanCo> quanDangChon;
    std::vector<sf::Vector2i> cacNuocDiHopLe;
    
    // Keyboard cursor
    sf::Vector2i viTriConTro; // (hang, cot)
    
    // Board UI parameters
    float kichThuocO;
    float offsetX;
    float offsetY;
    
    // Match settings
    float thoiGianVanDau; // 0 = unlimited, 300 = 5m, 600 = 10m, 900 = 15m
    bool hienGoiY;
    
    // Chess clocks
    float thoiGianConDo;
    float thoiGianConDen;
    sf::Clock dtClock;
    
    // AI think state
    bool aiDangSuyNghi;
    float thoiGianChoAI;
    
    // Notifications & End Game
    std::string toastMessage;
    float toastTimer;
    std::string lyDoKetThuc;

public:
    GameManager();
    
    void khoiTao();
    void chay();
    
private:
    void khoiTaoTatCaMenu();
    void capNhatNhanNut();
    void khoiTaoNutTrongGame();
    void khoiTaoNutPopup();
    
    // Event handling
    void xuLySuKien();
    void xuLyClickChuot(const sf::Vector2i& viTri);
    void xuLyBanPhim(sf::Keyboard::Key key, bool ctrl);
    void xuLyTextEntered(sf::Uint32 unicode);
    
    // Game flow
    void batDauPvp();
    void batDauPvai();
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
    
    // Save/Load system
    void capNhatThongTinSlots();
    void luuVaoSlot(int slotIndex);
    bool docTuSlot(int slotIndex);
    void xoaSlot(int slotIndex);
    std::string layDuongDanSlot(int slotIndex) const;
    
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
    void veConTroBanPhim();
    void veThanhBenPhai();
    void vePopupKetThuc();
    void veToast();
    
    // Screen rendering
    void veMenuChinh();
    void vePlaySubmenu();
    void vePvpSetup();
    void vePvaiSetup();
    void veLoadMenu();
    void veSettingsMenu();
    void veKeybindingMenu();
    void veIntroMenu();
};
