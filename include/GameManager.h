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
#include "Slider.h"
#include "Atmosphere.h"

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
    REALISTIC_WOOD,
    VIETNAMESE,
    SHORT_CODE,
    INTERNATIONAL
};

enum class PieceAura {
    CLASSIC_WOOD,
    IMPERIAL_GOLD,
    DRAGON_FLAME,
    THUNDER_CYAN,
    SHADOW_VOID
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
    sf::Font fontBold;
    sf::Font fontEn;
    sf::Font fontEnBold;
    sf::Font fontVn;
    sf::Font fontVnBold;
    
    TrangThai trangThai;
    CheDoChoi cheDoChoi;
    BanCo banCo;
    SoundManager soundManager;
    KeyConfig keyConfig;
    KeyAction rebindingAction;

    // Settings state
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

    // Play setup mode
    int playSetupMode;
    std::vector<std::unique_ptr<Button>> playSetupButtons;
    void khoiTaoPlaySetupButtons();
    void vePlaySetup();
    void vePieceStylePreview(sf::RenderWindow& win, float x, float y, float w, float h);
    void xuLyClickPlaySetup(const sf::Vector2i& viTri);

    // Settings buttons
    std::vector<std::unique_ptr<Button>> settingsTabButtons;
    std::vector<std::unique_ptr<Button>> settingsAudioButtons;
    std::vector<std::unique_ptr<Button>> settingsGraphicsButtons;
    std::vector<std::unique_ptr<Button>> settingsGameplayButtons;
    std::vector<std::unique_ptr<Button>> settingsControlButtons;
    std::vector<std::unique_ptr<Button>> settingsBottomButtons;
    
    // Atmosphere system
    AtmosphereSystem atmosphere;

    // Piece aura
    PieceAura p1Aura;
    PieceAura p2Aura;
    PieceAura auraDo;
    PieceAura auraDen;

    // Sliders
    std::unique_ptr<Slider> sliderMaster;
    std::unique_ptr<Slider> sliderSfx;
    std::unique_ptr<Slider> sliderBgm;
    std::unique_ptr<Slider> sliderAtmosphere;
    std::vector<std::unique_ptr<Button>> soundboardButtons;

    // In-game buttons
    std::vector<std::unique_ptr<Button>> inGameButtons;
    std::vector<std::unique_ptr<Button>> popupButtons;
    std::vector<std::unique_ptr<Button>> inGameMenuButtons;
    bool hienMenuPopUp;
    TrangThai trangThaiTruocSettings;
    TrangThai trangThaiTruocLoad;
    bool matchMenuTuDangChoi = false;
    bool daPhatAmKetThuc = false;
    void capNhatFontTheoNgonNgu();
    void phatAmThanhKetThuc();
    
    // Text inputs
    std::unique_ptr<TextInput> inputP1Name;
    std::unique_ptr<TextInput> inputP2Name;
    std::unique_ptr<TextInput> inputAiPlayerName;
    
    // PvP setup state
    int p1CharIndex;
    int p2CharIndex;
    int pvpFirstPlayer; // 1 = P1, 2 = P2
    std::string pvpRollBanner;
    
    // PvAI setup state
    int aiPlayerCharIndex;
    DoKho aiDifficulty;
    int aiFirstChoice; // 0 = Player, 1 = AI, 2 = Random
    Mau aiPlayerSide;
    std::string aiRollBanner;
    
    // Player profiles
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
    float thoiGianVanDau; // Match duration
    
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
    
    // Config
    void luuCaiDat();
    void docCaiDat();
    void khoiPhucCaiDatMacDinh();
    void apDungCheDoManHinh();
    void capNhatViewManHinh();
    
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
    void moMenuPopUp();
    void dongMenuPopUp();
    
    // Save and load
    void capNhatThongTinSlots();
    void luuVaoSlot(int slotIndex);
    bool docTuSlot(int slotIndex);
    void xoaSlot(int slotIndex);
    std::string layDuongDanSlot(int slotIndex) const;
    
    // Textures
    std::map<std::string, sf::Texture> textureQuanDo;
    std::map<std::string, sf::Texture> textureQuanDen;
    sf::Texture textureBackground;
    sf::Texture texturePlayBg;
    sf::Texture textureBanCo;
    sf::Texture avatarTextures[5];
    bool daTaiTextures;
    bool daTaiAvatarTextures;
    void taiAvatarTextures();
    void taiTextures();
    const sf::Texture* layTextureQuan(const std::string& tenQuan, Mau mau) const;

    // Toast
    void hienToast(const std::string& msg);
    
    // Coordinates
    sf::Vector2i chuyenDoiToaDoManHinhThanhBanCo(const sf::Vector2i& viTriChuot);
    sf::Vector2f chuyenDoiToaDoBanCoThanhManHinh(int hang, int cot);
    std::string layKyHieuQuanTheoStyle(const std::string& tenGoc, Mau mau) const;
    std::string layTenPieceAura(PieceAura aura) const;
    void veAuraQuan(sf::Vector2f pos, float r, PieceAura aura, bool isKing);
    
    // Rendering
    void ve();
    void veBanCoTruyenThong();
    void veCacQuanCo();
    void veHighlights();
    void veConTroBanPhim();
    void veThanhBenTrai();
    void veThanhBenPhai();
    void veThanhThongBao();
    void vePopupKetThuc();
    void veMenuPopUpInGame();
    void veToast();
    
    // Screens
    void veMenuChinh();
    void vePlaySubmenu();
    void vePvpSetup();
    void vePvaiSetup();
    void veLoadMenu();
    void veSettingsMenu();
    void veKeybindingMenu();
    void veIntroMenu();
};
