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

enum class SettingsTab {
    AUDIO,
    GRAPHICS,
    GAMEPLAY,
    CONTROLS
};

enum class BoardTheme {
    CLASSIC_WOOD,
    IMPERIAL_JADE,
    MIDNIGHT_INK,
    WARM_BAMBOO
};

enum class PieceStyle {
    REALISTIC_WOOD, // Traditional wooden sprite tokens with calligraphy
    VIETNAMESE,     // "Xe", "Mã", "Voi", "Sĩ", "Tướng", "Pháo", "Tốt"
    SHORT_CODE,     // "X", "M", "V", "S", "TG", "P", "T"
    INTERNATIONAL   // "R", "H", "E", "A", "K", "C", "P"
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

    // Commercial Settings State
    SettingsTab activeSettingsTab;
    BoardTheme boardTheme;
    PieceStyle pieceStyle;
    bool hienGoiY;
    bool hienNuocDiCuoi;
    bool hienToaDo;
    bool fullscreenMode;
    bool aiThinkRealistic;
    bool checkAlarmSound;
    
    // Menus
    Menu menuChinh;
    Menu menuPlaySub;
    Menu menuPvpSetup;
    Menu menuPvaiSetup;
    Menu menuLoad;
    Menu menuSettings;
    Menu menuKeybinding;
    Menu menuIntro;

    // Settings dashboard sub-menus/buttons
    std::vector<std::unique_ptr<Button>> settingsTabButtons;
    std::vector<std::unique_ptr<Button>> settingsAudioButtons;
    std::vector<std::unique_ptr<Button>> settingsGraphicsButtons;
    std::vector<std::unique_ptr<Button>> settingsGameplayButtons;
    std::vector<std::unique_ptr<Button>> settingsControlButtons;
    std::vector<std::unique_ptr<Button>> settingsBottomButtons;
    
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
    void khoiTaoDashboardSettings();
    void capNhatNhanNut();
    void khoiTaoNutTrongGame();
    void khoiTaoNutPopup();
    
    // Config Persistence
    void luuCaiDat();
    void docCaiDat();
    void khoiPhucCaiDatMacDinh();
    void apDungCheDoManHinh();
    
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
    
    // Textures & Graphic Assets
    std::map<std::string, sf::Texture> textureQuanDo;
    std::map<std::string, sf::Texture> textureQuanDen;
    bool daTaiTextures;
    void taiTextures();
    const sf::Texture* layTextureQuan(const std::string& tenQuan, Mau mau) const;

    // Toast
    void hienToast(const std::string& msg);
    
    // Coordinates conversion
    sf::Vector2i chuyenDoiToaDoManHinhThanhBanCo(const sf::Vector2i& viTriChuot);
    sf::Vector2f chuyenDoiToaDoBanCoThanhManHinh(int hang, int cot);
    std::string layKyHieuQuanTheoStyle(const std::string& tenGoc, Mau mau) const;
    
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
