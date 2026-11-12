#include "GameManager.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include "Localization.h"
#include "FantasyUI.h"

namespace fs = std::filesystem;

static bool fileExistsUtf8(const std::string& path) {
    std::error_code ec;
    return fs::exists(fs::u8path(path), ec);
}

static bool tryLoadFontCandidates(sf::Font& font, const std::initializer_list<const char*> paths) {
    for (const char* p : paths) {
        if (fileExistsUtf8(p) && font.loadFromFile(p)) {
            return true;
        }
    }
    return false;
}

static bool tryLoadTextureCandidates(sf::Texture& tex, const std::initializer_list<const char*> paths) {
    for (const char* p : paths) {
        if (fileExistsUtf8(p) && tex.loadFromFile(p)) {
            return true;
        }
    }
    return false;
}

static bool tryLoadImageCandidates(sf::Image& img, const std::initializer_list<const char*> paths) {
    for (const char* p : paths) {
        if (fileExistsUtf8(p) && img.loadFromFile(p)) {
            return true;
        }
    }
    return false;
}

static std::string layThoiGianHienTai() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
    localtime_s(&tm, &now_c);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

GameManager::GameManager() 
    : window(sf::VideoMode(1200, 800), "Co Tuong - Chinese Chess (HCMUS OOP)", sf::Style::Default),
      trangThai(TrangThai::MENU_CHINH),
      cheDoChoi(CheDoChoi::HAI_NGUOI),
      rebindingAction(KeyAction::COUNT),
      activeSettingsTab(SettingsTab::AUDIO),
      boardTheme(BoardTheme::CLASSIC_WOOD),
      pieceStyle(PieceStyle::REALISTIC_WOOD),
      playSetupMode(0),
      daTaiTextures(false),
      daTaiAvatarTextures(false),
      hienGoiY(true),
      hienNuocDiCuoi(true),
      hienToaDo(true),
      fullscreenMode(false),
      atmosphere(1200.0f, 800.0f),
      p1Aura(PieceAura::IMPERIAL_GOLD),
      p2Aura(PieceAura::DRAGON_FLAME),
      auraDo(PieceAura::IMPERIAL_GOLD),
      auraDen(PieceAura::DRAGON_FLAME),
      aiThinkRealistic(true),
      checkAlarmSound(true),
      p1CharIndex(0),
      p2CharIndex(1),
      pvpFirstPlayer(1),
      pvpRollBanner(""),
      aiPlayerCharIndex(0),
      aiDifficulty(DoKho::TRUNG_BINH),
      aiFirstChoice(0),
      aiPlayerSide(Mau::DO),
      aiRollBanner(""),
      tenNguoiChoiDo("Player 1"),
      tenNguoiChoiDen("Player 2"),
      nhanVatDo(0),
      nhanVatDen(1),
      selectedSlot(0),
      quanDangChon(nullptr),
      viTriConTro(9, 4),
      kichThuocO(68.0f),
      offsetX(328.0f),
      offsetY(110.0f),
      thoiGianVanDau(600.0f), // 10 minutes default
      thoiGianConDo(600.0f),
      thoiGianConDen(600.0f),
      aiDangSuyNghi(false),
      thoiGianChoAI(0.0f),
      toastTimer(0.0f),
      lyDoKetThuc(""),
      hienMenuPopUp(false),
      trangThaiTruocSettings(TrangThai::MENU_CHINH),
      trangThaiTruocLoad(TrangThai::MENU_CHINH) {
    menuChinh.laMenuChinh = true;
    
    window.setFramerateLimit(60);
    khoiTao();
}

void GameManager::khoiTao() {
    window.setMouseCursorVisible(false);
    window.clear(sf::Color(14, 18, 26));
    window.display();

    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    if (desktop.width > window.getSize().x && desktop.height > window.getSize().y) {
        window.setPosition(sf::Vector2i(
            static_cast<int>((desktop.width - window.getSize().x) / 2),
            static_cast<int>((desktop.height - window.getSize().y) / 2)
        ));
    }

    // Load fonts
    tryLoadFontCandidates(fontVn, {
        "assets/font/ViaodaLibre-Regular.ttf",
        "resources/fonts/ViaodaLibre-Regular.ttf",
        "assets/font/NomNaTong-Regular.ttf",
        "resources/fonts/NomNaTong-Regular.ttf",
        "resources/fonts/arial.ttf",
        "C:/Windows/Fonts/arial.ttf"
    });

    tryLoadFontCandidates(fontVnBold, {
        "assets/font/ViaodaLibre-Regular.ttf",
        "resources/fonts/ViaodaLibre-Regular.ttf",
        "C:/Windows/Fonts/arialbd.ttf",
        "resources/fonts/arial.ttf"
    });

    tryLoadFontCandidates(fontEn, {
        "assets/font/ViaodaLibre-Regular.ttf",
        "resources/fonts/ViaodaLibre-Regular.ttf",
        "assets/font/PixeloidSans.ttf",
        "resources/fonts/PixeloidSans.ttf"
    });

    tryLoadFontCandidates(fontEnBold, {
        "assets/font/ViaodaLibre-Regular.ttf",
        "resources/fonts/ViaodaLibre-Regular.ttf",
        "assets/font/PixeloidSansBold.ttf",
        "resources/fonts/PixeloidSansBold.ttf"
    });

    tryLoadFontCandidates(fontHan, {
        "assets/font/NomNaTong-Regular.ttf",
        "resources/fonts/NomNaTong-Regular.ttf"
    });

    taiTextures();
    soundManager.khoiTao();
    docCaiDat();
    if (fullscreenMode) {
        apDungCheDoManHinh();
    } else {
        capNhatViewManHinh();
    }
    capNhatFontTheoNgonNgu();
    
    // Create text inputs
    inputP1Name = std::make_unique<TextInput>(150.0f, 192.0f, 320.0f, 38.0f, font, "Player 1", 14);
    inputP1Name->setValue("Player 1");
    inputP1Name->setParchmentStyle(true);

    inputP2Name = std::make_unique<TextInput>(710.0f, 192.0f, 320.0f, 38.0f, font, "Player 2", 14);
    inputP2Name->setValue("Player 2");
    inputP2Name->setParchmentStyle(true);

    inputAiPlayerName = std::make_unique<TextInput>(150.0f, 192.0f, 320.0f, 38.0f, font, "Player", 14);
    inputAiPlayerName->setValue("Player");
    inputAiPlayerName->setParchmentStyle(true);

    slotInfos.resize(3);
    capNhatThongTinSlots();

    khoiTaoTatCaMenu();
    khoiTaoDashboardSettings();
    khoiTaoNutTrongGame();
    khoiTaoNutPopup();
}

void GameManager::capNhatFontTheoNgonNgu() {
    bool vi = (Loc::getLanguage() == Language::TIENG_VIET);
    font = vi ? fontVn : (fontEn.getInfo().family != "" ? fontEn : fontVn);
    fontBold = vi ? (fontVnBold.getInfo().family != "" ? fontVnBold : fontVn)
                  : (fontEnBold.getInfo().family != "" ? fontEnBold : font);

    khoiTaoNutTrongGame();
    khoiTaoNutPopup();
    if (inputP1Name) inputP1Name->setFont(font);
    if (inputP2Name) inputP2Name->setFont(font);
    if (inputAiPlayerName) inputAiPlayerName->setFont(font);
    menuChinh.capNhatNutTrangPhai();
}

void GameManager::phatAmThanhKetThuc() {
    daPhatAmKetThuc = true;
    Mau winner = banCo.getNguoiThang();
    if (winner == Mau::HOA) {
        soundManager.play(SoundType::CLICK);
    } else if (cheDoChoi == CheDoChoi::VOI_MAY) {
        if (winner == aiPlayerSide) {
            soundManager.play(SoundType::VICTORY);
        } else {
            soundManager.play(SoundType::DEFEAT);
        }
    } else {
        soundManager.play(SoundType::VICTORY);
    }
}

void GameManager::khoiTaoTatCaMenu() {
    float bw = 340.0f;
    float bh = 48.0f;
    float cx = (1200.0f - bw) / 2.0f;
    float sp = 62.0f;

    // Main menu
    menuChinh.setFont(font); 
    menuChinh.xoaTatCaButton();
    menuChinh.laMenuChinh = true;

    // Menu buttons (standard proportional ~ 220px x 48px)
    bool isVi = (Loc::getLanguage() == Language::TIENG_VIET);
    menuChinh.themButton(0, 0, 240.0f, 44.0f, isVi ? "CHƠI MỚI" : "PLAY", [this]() {
        soundManager.play(SoundType::CLICK);
        menuChinh.setSection(MenuSection::PLAY);
    });
    menuChinh.themButton(0, 0, 240.0f, 44.0f, isVi ? "TIẾP TỤC" : "LOAD", [this]() {
        soundManager.play(SoundType::CLICK);
        capNhatThongTinSlots();
        menuChinh.setSection(MenuSection::LOAD);
    });
    menuChinh.themButton(0, 0, 240.0f, 44.0f, Loc::get(LocKey::MENU_SETTINGS), [this]() {
        soundManager.play(SoundType::CLICK);
        menuChinh.setSection(MenuSection::SETTINGS);
    });
    menuChinh.themButton(0, 0, 240.0f, 44.0f, isVi ? "THÔNG TIN" : "INFORMATION", [this]() {
        soundManager.play(SoundType::CLICK);
        menuChinh.setSection(MenuSection::INFO);
    });
    menuChinh.themButton(0, 0, 240.0f, 44.0f, Loc::get(LocKey::MENU_EXIT), [this]() {
        soundManager.play(SoundType::CLICK);
        menuChinh.setSection(MenuSection::EXIT);
    });

    // Gan Icons tu bo TravelBook cho 5 nut
    auto& mAssets = getMenuAssets();
    if (auto b = menuChinh.layButton(0)) b->setIcon(&mAssets.iconPlay);
    if (auto b = menuChinh.layButton(1)) b->setIcon(&mAssets.iconLoad);
    if (auto b = menuChinh.layButton(2)) b->setIcon(&mAssets.iconSettings);
    if (auto b = menuChinh.layButton(3)) b->setIcon(&mAssets.iconInfo);
    if (auto b = menuChinh.layButton(4)) b->setIcon(&mAssets.iconExit);

    // Callbacks xu ly noi dung trang phai
    menuChinh.onPlayPvp = [this]() {
        soundManager.play(SoundType::CLICK);
        pvpRollBanner = "";
        playSetupMode = 1;
        khoiTaoPlaySetupButtons();
        trangThai = TrangThai::PVP_SETUP;
    };
    menuChinh.onPlayPvai = [this]() {
        soundManager.play(SoundType::CLICK);
        aiRollBanner = "";
        playSetupMode = 0;
        khoiTaoPlaySetupButtons();
        trangThai = TrangThai::PVAI_SETUP;
    };
    menuChinh.onPlayDemo = [this]() {
        soundManager.play(SoundType::CLICK);
        playSetupMode = 2;
        khoiTaoPlaySetupButtons();
        trangThai = TrangThai::PVP_SETUP;
    };

    menuChinh.onSelectSlot = [this](int slot) {
        selectedSlot = slot;
    };
    menuChinh.onLoadSlot = [this](int slot) {
        soundManager.play(SoundType::CLICK);
        docTuSlot(slot);
    };
    menuChinh.onDeleteSlot = [this](int slot) {
        soundManager.play(SoundType::CLICK);
        xoaSlot(slot);
        capNhatThongTinSlots();
    };

    menuChinh.getSlotInfoStr = [this](int slot, int infoType) -> std::string {
        if (slot < 0 || slot >= static_cast<int>(slotInfos.size())) return "";
        const auto& info = slotInfos[slot];
        if (infoType == 0) {
            if (!info.tonTai) return (Loc::getLanguage() == Language::TIENG_VIET) ? "(Ô lưu trống - Chưa có dữ liệu)" : "(Empty save slot)";
            return info.tenCheDo + " • " + info.thoiGian;
        } else {
            if (!info.tonTai) return "";
            std::string pStr = (Loc::getLanguage() == Language::TIENG_VIET)
                ? ("Đỏ [" + info.tenDo + "] vs Đen [" + info.tenDen + "] • " + std::to_string(info.soNuoc) + " nước")
                : ("Red [" + info.tenDo + "] vs Black [" + info.tenDen + "] • " + std::to_string(info.soNuoc) + " moves");
            return pStr;
        }
    };

    menuChinh.onMasterVolStep = [this](float delta) {
        float cur = soundManager.getMasterVolume();
        float next = std::max(0.0f, std::min(100.0f, cur + delta));
        soundManager.setMasterVolume(next);
        soundManager.play(SoundType::CLICK);
        menuChinh.capNhatNutTrangPhai();
    };

    menuChinh.onSetMasterVolume = [this](float v) {
        soundManager.setMasterVolume(v);
    };

    menuChinh.onSetBgmVolume = [this](float v) {
        soundManager.setBgmVolume(v);
    };

    menuChinh.onSetSfxVolume = [this](float v) {
        soundManager.setSfxVolume(v);
    };

    menuChinh.getMasterVolume = [this]() {
        return soundManager.getMasterVolume();
    };

    menuChinh.getBgmVolume = [this]() {
        return soundManager.getBgmVolume();
    };

    menuChinh.getSfxVolume = [this]() {
        return soundManager.getSfxVolume();
    };

    menuChinh.getMasterBat = [this]() {
        return soundManager.getAmThanhBat();
    };

    menuChinh.getBgmBat = [this]() {
        return soundManager.getBgmBat();
    };

    menuChinh.getSfxBat = [this]() {
        return soundManager.getSfxBat();
    };

    menuChinh.onToggleMasterSound = [this]() {
        soundManager.setAmThanhBat(!soundManager.getAmThanhBat());
        soundManager.play(SoundType::CLICK);
    };

    menuChinh.onToggleSfx = [this]() {
        soundManager.setSfxBat(!soundManager.getSfxBat());
        soundManager.play(SoundType::CLICK);
    };

    menuChinh.onToggleBgm = [this]() {
        soundManager.setBgmBat(!soundManager.getBgmBat());
        soundManager.play(SoundType::CLICK);
    };

    menuChinh.onTestSound = [this]() {
        soundManager.play(SoundType::MOVE);
    };

    menuChinh.onToggleBoardTheme = [this]() {
        int t = (static_cast<int>(boardTheme) + 1) % 4;
        boardTheme = static_cast<BoardTheme>(t);
        soundManager.play(SoundType::CLICK);
    };

    menuChinh.onTogglePieceStyle = [this]() {
        int s = (static_cast<int>(pieceStyle) + 1) % 4;
        pieceStyle = static_cast<PieceStyle>(s);
        soundManager.play(SoundType::CLICK);
    };

    menuChinh.onToggleAtmosphere = [this]() {
        int cur = static_cast<int>(atmosphere.getStyle());
        int next = (cur + 1) % 3;
        atmosphere.setStyle(static_cast<AtmosphereStyle>(next));
        soundManager.play(SoundType::CLICK);
    };

    menuChinh.onToggleWindowMode = [this]() {
        fullscreenMode = !fullscreenMode;
        soundManager.play(SoundType::CLICK);
        apDungCheDoManHinh();
    };

    menuChinh.onToggleLang = [this]() {
        Loc::toggleLanguage();
        soundManager.play(SoundType::CLICK);
        capNhatFontTheoNgonNgu();
        capNhatNhanNut();
        khoiTaoPlaySetupButtons();
        khoiTaoTatCaMenu();
        luuCaiDat();
    };

    menuChinh.onDrawPiecePreview = [this](sf::RenderWindow& win, float x, float y, float w, float h) {
        vePieceStylePreview(win, x, y, w, h);
    };

    menuChinh.onToggleAiDelay = [this]() {
        aiThinkRealistic = !aiThinkRealistic;
        soundManager.play(SoundType::CLICK);
    };

    menuChinh.onToggleCheckAlarm = [this]() {
        checkAlarmSound = !checkAlarmSound;
        soundManager.play(SoundType::CLICK);
    };

    menuChinh.onPresetWasd = [this]() {
        keyConfig.setPreset(KeyPreset::WASD);
        soundManager.play(SoundType::CLICK);
        hienToast(Loc::getLanguage() == Language::TIENG_VIET ? "Đã áp dụng bộ phím WASD!" : "Applied WASD controls!");
    };

    menuChinh.onPresetArrows = [this]() {
        keyConfig.setPreset(KeyPreset::ARROWS);
        soundManager.play(SoundType::CLICK);
        hienToast(Loc::getLanguage() == Language::TIENG_VIET ? "Đã áp dụng bộ phím Mũi tên!" : "Applied Arrow controls!");
    };

    menuChinh.onReturnToGame = [this]() {
        soundManager.play(SoundType::CLICK);
        if (matchMenuTuDangChoi) {
            trangThai = TrangThai::DANG_CHOI;
            matchMenuTuDangChoi = false;
            hienMenuPopUp = false;
        } else {
            menuChinh.triggerCloseAnimation();
        }
    };

    menuChinh.onApplySettings = [this]() {
        luuCaiDat();
        soundManager.play(SoundType::CLICK);
        hienToast(Loc::getLanguage() == Language::TIENG_VIET ? "Đã lưu cài đặt thành công!" : "Settings saved successfully!");
        if (matchMenuTuDangChoi) {
            trangThai = TrangThai::DANG_CHOI;
            matchMenuTuDangChoi = false;
            hienMenuPopUp = false;
        }
    };

    menuChinh.onRestoreDefaults = [this]() {
        khoiPhucCaiDatMacDinh();
        soundManager.play(SoundType::CLICK);
        hienToast(Loc::getLanguage() == Language::TIENG_VIET ? "Đã khôi phục cài đặt mặc định!" : "Restored default settings!");
    };

    menuChinh.onConfirmExit = [this]() {
        soundManager.play(SoundType::CLICK);
        luuCaiDat();
        window.close();
    };

    menuChinh.getSettingStatusStr = [this](int id) -> std::string {
        bool vi = (Loc::getLanguage() == Language::TIENG_VIET);
        switch (id) {
            case 0: return std::to_string(static_cast<int>(soundManager.getMasterVolume())) + "%";
            case 1: return soundManager.getSfxBat() ? (vi ? "BẬT" : "ON") : (vi ? "TẮT" : "OFF");
            case 2: return soundManager.getBgmBat() ? (vi ? "BẬT" : "ON") : (vi ? "TẮT" : "OFF");
            case 3: {
                if (boardTheme == BoardTheme::CLASSIC_WOOD) return Loc::get(LocKey::THEME_WOOD);
                if (boardTheme == BoardTheme::IMPERIAL_JADE) return Loc::get(LocKey::THEME_JADE);
                if (boardTheme == BoardTheme::MIDNIGHT_INK) return Loc::get(LocKey::THEME_DARK);
                return Loc::get(LocKey::THEME_BAMBOO);
            }
            case 4: {
                if (pieceStyle == PieceStyle::REALISTIC_WOOD) return Loc::get(LocKey::PIECE_REALISTIC);
                if (pieceStyle == PieceStyle::VIETNAMESE) return Loc::get(LocKey::PIECE_VIETNAMESE);
                if (pieceStyle == PieceStyle::SHORT_CODE) return Loc::get(LocKey::PIECE_SHORT);
                return Loc::get(LocKey::PIECE_INTL);
            }
            case 5: {
                if (atmosphere.getStyle() == AtmosphereStyle::BAMBOO_LEAVES) return Loc::get(LocKey::ATMOSPHERE_LEAVES);
                if (atmosphere.getStyle() == AtmosphereStyle::PEACH_BLOSSOMS) return Loc::get(LocKey::ATMOSPHERE_PETALS);
                return Loc::get(LocKey::ATMOSPHERE_SPARKS);
            }
            case 6: return fullscreenMode ? Loc::get(LocKey::MODE_FULLSCREEN) : Loc::get(LocKey::MODE_WINDOWED);
            case 7: return vi ? "Tiếng Việt" : "English";
            case 8: return aiThinkRealistic ? Loc::get(LocKey::AI_THINK_REALISTIC) : Loc::get(LocKey::AI_THINK_INSTANT);
            case 9: return checkAlarmSound ? (vi ? "BẬT" : "ON") : (vi ? "TẮT" : "OFF");
            default: return "";
        }
    };
    // Play submenu
    menuPlaySub.setFont(font);
    menuPlaySub.xoaTatCaButton();
    float psubY = 235.0f;

    menuPlaySub.themButton(cx, psubY, bw, bh, Loc::get(LocKey::PLAY_PVP), [this]() {
        soundManager.play(SoundType::CLICK);
        pvpRollBanner = "";
        playSetupMode = 1;
        khoiTaoPlaySetupButtons();
        trangThai = TrangThai::PVP_SETUP;
    });
    menuPlaySub.themButton(cx, psubY + sp, bw, bh, Loc::get(LocKey::PLAY_PVAI), [this]() {
        soundManager.play(SoundType::CLICK);
        aiRollBanner = "";
        playSetupMode = 0;
        khoiTaoPlaySetupButtons();
        trangThai = TrangThai::PVAI_SETUP;
    });
    menuPlaySub.themButton(cx, psubY + sp * 2, bw, bh, Loc::get(LocKey::PLAY_DEMO), [this]() {
        soundManager.play(SoundType::CLICK);
        tenNguoiChoiDo = (Loc::getLanguage() == Language::TIENG_VIET) ? "Quan Do (Xe & Voi)" : "Red (Chariot & Elephant)";
        tenNguoiChoiDen = (Loc::getLanguage() == Language::TIENG_VIET) ? "Quan Den (Xe & Voi)" : "Black (Chariot & Elephant)";
        nhanVatDo = 0;
        nhanVatDen = 0;
        playSetupMode = 2;
        khoiTaoPlaySetupButtons();
        trangThai = TrangThai::PVP_SETUP;
    });
    menuPlaySub.themButton(cx, psubY + sp * 3, bw, bh, Loc::get(LocKey::BACK), [this]() {
        soundManager.play(SoundType::CLICK);
        trangThai = TrangThai::MENU_CHINH;
        menuChinh.triggerOpenAnimation();
    });
    // PvP setup
    menuPvpSetup.setFont(font);
    menuPvpSetup.xoaTatCaButton();
    
    // P1 Char selector buttons (0, 1, 2)
    menuPvpSetup.themButton(150.0f, 265.0f, 38.0f, 34.0f, "<", [this]() {
        soundManager.play(SoundType::CLICK);
        p1CharIndex = (p1CharIndex + Loc::getCharCount() - 1) % Loc::getCharCount();
        capNhatNhanNut();
    });
    menuPvpSetup.themButton(194.0f, 265.0f, 168.0f, 34.0f, Loc::getCharName(p1CharIndex), [this]() {
        soundManager.play(SoundType::CLICK);
        p1CharIndex = (p1CharIndex + 1) % Loc::getCharCount();
        capNhatNhanNut();
    });
    menuPvpSetup.themButton(368.0f, 265.0f, 38.0f, 34.0f, ">", [this]() {
        soundManager.play(SoundType::CLICK);
        p1CharIndex = (p1CharIndex + 1) % Loc::getCharCount();
        capNhatNhanNut();
    });

    // P1 Aura selector buttons (3, 4, 5)
    menuPvpSetup.themButton(150.0f, 306.0f, 38.0f, 34.0f, "<", [this]() {
        soundManager.play(SoundType::CLICK);
        p1Aura = static_cast<PieceAura>((static_cast<int>(p1Aura) + 5 - 1) % 5);
        capNhatNhanNut();
    });
    menuPvpSetup.themButton(194.0f, 306.0f, 168.0f, 34.0f, layTenPieceAura(p1Aura), [this]() {
        soundManager.play(SoundType::CLICK);
        p1Aura = static_cast<PieceAura>((static_cast<int>(p1Aura) + 1) % 5);
        capNhatNhanNut();
    });
    menuPvpSetup.themButton(368.0f, 306.0f, 38.0f, 34.0f, ">", [this]() {
        soundManager.play(SoundType::CLICK);
        p1Aura = static_cast<PieceAura>((static_cast<int>(p1Aura) + 1) % 5);
        capNhatNhanNut();
    });

    // P2 Char selector buttons (6, 7, 8)
    menuPvpSetup.themButton(710.0f, 265.0f, 38.0f, 34.0f, "<", [this]() {
        soundManager.play(SoundType::CLICK);
        p2CharIndex = (p2CharIndex + Loc::getCharCount() - 1) % Loc::getCharCount();
        capNhatNhanNut();
    });
    menuPvpSetup.themButton(754.0f, 265.0f, 168.0f, 34.0f, Loc::getCharName(p2CharIndex), [this]() {
        soundManager.play(SoundType::CLICK);
        p2CharIndex = (p2CharIndex + 1) % Loc::getCharCount();
        capNhatNhanNut();
    });
    menuPvpSetup.themButton(928.0f, 265.0f, 38.0f, 34.0f, ">", [this]() {
        soundManager.play(SoundType::CLICK);
        p2CharIndex = (p2CharIndex + 1) % Loc::getCharCount();
        capNhatNhanNut();
    });

    // P2 Aura selector buttons (9, 10, 11)
    menuPvpSetup.themButton(710.0f, 306.0f, 38.0f, 34.0f, "<", [this]() {
        soundManager.play(SoundType::CLICK);
        p2Aura = static_cast<PieceAura>((static_cast<int>(p2Aura) + 5 - 1) % 5);
        capNhatNhanNut();
    });
    menuPvpSetup.themButton(754.0f, 306.0f, 168.0f, 34.0f, layTenPieceAura(p2Aura), [this]() {
        soundManager.play(SoundType::CLICK);
        p2Aura = static_cast<PieceAura>((static_cast<int>(p2Aura) + 1) % 5);
        capNhatNhanNut();
    });
    menuPvpSetup.themButton(928.0f, 306.0f, 38.0f, 34.0f, ">", [this]() {
        soundManager.play(SoundType::CLICK);
        p2Aura = static_cast<PieceAura>((static_cast<int>(p2Aura) + 1) % 5);
        capNhatNhanNut();
    });

    // Match timer
    auto getTimerStr = [this]() {
        if (thoiGianVanDau <= 0.0f) return Loc::get(LocKey::MATCH_TIMER_LABEL) + Loc::get(LocKey::TIME_UNLIMITED);
        if (thoiGianVanDau == 300.0f) return Loc::get(LocKey::MATCH_TIMER_LABEL) + Loc::get(LocKey::TIME_5M);
        if (thoiGianVanDau == 600.0f) return Loc::get(LocKey::MATCH_TIMER_LABEL) + Loc::get(LocKey::TIME_10M);
        return Loc::get(LocKey::MATCH_TIMER_LABEL) + Loc::get(LocKey::TIME_15M);
    };

    menuPvpSetup.themButton(360.0f, 405.0f, 480.0f, 46.0f, getTimerStr(), [this, getTimerStr]() {
        soundManager.play(SoundType::CLICK);
        if (thoiGianVanDau == 300.0f) thoiGianVanDau = 600.0f;
        else if (thoiGianVanDau == 600.0f) thoiGianVanDau = 900.0f;
        else if (thoiGianVanDau == 900.0f) thoiGianVanDau = 0.0f;
        else thoiGianVanDau = 300.0f;
        if (auto b = menuPvpSetup.layButton(12)) b->setLabel(getTimerStr());
    });

    // Center Random Roll Button (13)
    menuPvpSetup.themButton(360.0f, 465.0f, 480.0f, 48.0f, Loc::get(LocKey::TOSS_COIN), [this]() {
        soundManager.play(SoundType::CAPTURE);
        pvpFirstPlayer = (rand() % 2 == 0) ? 1 : 2;
        std::string winner = (pvpFirstPlayer == 1) ? inputP1Name->getValue() : inputP2Name->getValue();
        if (winner.empty()) winner = (pvpFirstPlayer == 1) ? "Player 1" : "Player 2";
        
        if (Loc::getLanguage() == Language::TIENG_VIET) {
            pvpRollBanner = "KET QUA: " + winner + " cam quan DO (Di truoc)!";
        } else {
            pvpRollBanner = "RESULT: " + winner + " plays RED (First move)!";
        }
    });

    // Bottom Start & Back (14, 15)
    menuPvpSetup.themButton(340.0f, 650.0f, 240.0f, 52.0f, Loc::get(LocKey::START_GAME), [this]() {
        soundManager.play(SoundType::CLICK);
        batDauPvp();
    });
    menuPvpSetup.themButton(620.0f, 650.0f, 240.0f, 52.0f, Loc::get(LocKey::BACK), [this]() {
        soundManager.play(SoundType::CLICK);
        trangThai = TrangThai::PLAY_SUBMENU;
    });
    // PvAI setup
    menuPvaiSetup.setFont(font);
    menuPvaiSetup.xoaTatCaButton();

    // Character buttons
    menuPvaiSetup.themButton(150.0f, 265.0f, 38.0f, 34.0f, "<", [this]() {
        soundManager.play(SoundType::CLICK);
        aiPlayerCharIndex = (aiPlayerCharIndex + Loc::getCharCount() - 1) % Loc::getCharCount();
        capNhatNhanNut();
    });
    menuPvaiSetup.themButton(194.0f, 265.0f, 168.0f, 34.0f, Loc::getCharName(aiPlayerCharIndex), [this]() {
        soundManager.play(SoundType::CLICK);
        aiPlayerCharIndex = (aiPlayerCharIndex + 1) % Loc::getCharCount();
        capNhatNhanNut();
    });
    menuPvaiSetup.themButton(368.0f, 265.0f, 38.0f, 34.0f, ">", [this]() {
        soundManager.play(SoundType::CLICK);
        aiPlayerCharIndex = (aiPlayerCharIndex + 1) % Loc::getCharCount();
        capNhatNhanNut();
    });

    // Aura buttons
    menuPvaiSetup.themButton(150.0f, 306.0f, 38.0f, 34.0f, "<", [this]() {
        soundManager.play(SoundType::CLICK);
        p1Aura = static_cast<PieceAura>((static_cast<int>(p1Aura) + 5 - 1) % 5);
        capNhatNhanNut();
    });
    menuPvaiSetup.themButton(194.0f, 306.0f, 168.0f, 34.0f, layTenPieceAura(p1Aura), [this]() {
        soundManager.play(SoundType::CLICK);
        p1Aura = static_cast<PieceAura>((static_cast<int>(p1Aura) + 1) % 5);
        capNhatNhanNut();
    });
    menuPvaiSetup.themButton(368.0f, 306.0f, 38.0f, 34.0f, ">", [this]() {
        soundManager.play(SoundType::CLICK);
        p1Aura = static_cast<PieceAura>((static_cast<int>(p1Aura) + 1) % 5);
        capNhatNhanNut();
    });

    auto getDiffLabel = [this]() {
        LocKey k = LocKey::DIFF_MED;
        if (aiDifficulty == DoKho::DE) k = LocKey::DIFF_EASY;
        else if (aiDifficulty == DoKho::KHO) k = LocKey::DIFF_HARD;
        return Loc::get(LocKey::AI_DIFFICULTY) + Loc::get(k);
    };

    // AI Difficulty Button (6)
    menuPvaiSetup.themButton(710.0f, 192.0f, 320.0f, 38.0f, getDiffLabel(), [this, getDiffLabel]() {
        soundManager.play(SoundType::CLICK);
        if (aiDifficulty == DoKho::DE) aiDifficulty = DoKho::TRUNG_BINH;
        else if (aiDifficulty == DoKho::TRUNG_BINH) aiDifficulty = DoKho::KHO;
        else aiDifficulty = DoKho::DE;
        if (auto b = menuPvaiSetup.layButton(6)) b->setLabel(getDiffLabel());
    });

    // AI Aura selector buttons (7, 8, 9)
    menuPvaiSetup.themButton(710.0f, 275.0f, 38.0f, 34.0f, "<", [this]() {
        soundManager.play(SoundType::CLICK);
        p2Aura = static_cast<PieceAura>((static_cast<int>(p2Aura) + 5 - 1) % 5);
        capNhatNhanNut();
    });
    menuPvaiSetup.themButton(754.0f, 275.0f, 168.0f, 34.0f, layTenPieceAura(p2Aura), [this]() {
        soundManager.play(SoundType::CLICK);
        p2Aura = static_cast<PieceAura>((static_cast<int>(p2Aura) + 1) % 5);
        capNhatNhanNut();
    });
    menuPvaiSetup.themButton(928.0f, 275.0f, 38.0f, 34.0f, ">", [this]() {
        soundManager.play(SoundType::CLICK);
        p2Aura = static_cast<PieceAura>((static_cast<int>(p2Aura) + 1) % 5);
        capNhatNhanNut();
    });

    // Match Timer (10)
    menuPvaiSetup.themButton(360.0f, 395.0f, 480.0f, 44.0f, getTimerStr(), [this, getTimerStr]() {
        soundManager.play(SoundType::CLICK);
        if (thoiGianVanDau == 300.0f) thoiGianVanDau = 600.0f;
        else if (thoiGianVanDau == 600.0f) thoiGianVanDau = 900.0f;
        else if (thoiGianVanDau == 900.0f) thoiGianVanDau = 0.0f;
        else thoiGianVanDau = 300.0f;
        if (auto b = menuPvaiSetup.layButton(10)) b->setLabel(getTimerStr());
    });

    auto getFirstChoiceLabel = [this]() {
        std::string modeStr;
        if (aiFirstChoice == 0) modeStr = Loc::get(LocKey::PLAYER_FIRST);
        else if (aiFirstChoice == 1) modeStr = Loc::get(LocKey::AI_FIRST);
        else modeStr = Loc::get(LocKey::RANDOM_FIRST);
        return Loc::get(LocKey::WHO_GOES_FIRST) + modeStr;
    };

    // Who Goes First (11)
    menuPvaiSetup.themButton(360.0f, 450.0f, 480.0f, 44.0f, getFirstChoiceLabel(), [this, getFirstChoiceLabel]() {
        soundManager.play(SoundType::CLICK);
        aiFirstChoice = (aiFirstChoice + 1) % 3;
        if (auto b = menuPvaiSetup.layButton(11)) b->setLabel(getFirstChoiceLabel());
    });

    // Toss Coin (12)
    menuPvaiSetup.themButton(360.0f, 505.0f, 480.0f, 46.0f, Loc::get(LocKey::TOSS_COIN), [this]() {
        soundManager.play(SoundType::CAPTURE);
        bool playerRed = (rand() % 2 == 0);
        aiPlayerSide = playerRed ? Mau::DO : Mau::DEN;
        aiFirstChoice = playerRed ? 0 : 1;
        if (Loc::getLanguage() == Language::TIENG_VIET) {
            aiRollBanner = playerRed ? "KET QUA: Ban cam quan DO (Di truoc)!" : "KET QUA: May AI cam quan DO (May di truoc)!";
        } else {
            aiRollBanner = playerRed ? "RESULT: You play RED (Move first)!" : "RESULT: AI plays RED (AI moves first)!";
        }
        capNhatNhanNut();
    });

    // Start & Back (13, 14)
    menuPvaiSetup.themButton(340.0f, 650.0f, 240.0f, 52.0f, Loc::get(LocKey::START_GAME), [this]() {
        soundManager.play(SoundType::CLICK);
        batDauPvai();
    });
    menuPvaiSetup.themButton(620.0f, 650.0f, 240.0f, 52.0f, Loc::get(LocKey::BACK), [this]() {
        soundManager.play(SoundType::CLICK);
        trangThai = TrangThai::PLAY_SUBMENU;
    });

    // Load game menu
    menuLoad.setFont(font);
    menuLoad.xoaTatCaButton();

    menuLoad.themButton(260.0f, 650.0f, 220.0f, 52.0f, Loc::get(LocKey::LOAD_BUTTON), [this]() {
        docTuSlot(selectedSlot);
    });
    menuLoad.themButton(500.0f, 650.0f, 200.0f, 52.0f, Loc::get(LocKey::DELETE_BUTTON), [this]() {
        xoaSlot(selectedSlot);
    });
    menuLoad.themButton(720.0f, 650.0f, 200.0f, 52.0f, Loc::get(LocKey::BACK), [this]() {
        soundManager.play(SoundType::CLICK);
        trangThai = trangThaiTruocLoad;
        if (trangThai == TrangThai::DANG_CHOI) {
            hienMenuPopUp = true;
        }
    });

    // Intro menu
    menuIntro.setFont(font);
    menuIntro.xoaTatCaButton();
    menuIntro.themButton((1200.0f - 220.0f) / 2.0f, 695.0f, 220.0f, 48.0f, Loc::get(LocKey::BACK), [this]() {
        soundManager.play(SoundType::CLICK);
        trangThai = TrangThai::MENU_CHINH;
        menuChinh.triggerOpenAnimation();
    });

    capNhatNhanNut();
}

void GameManager::capNhatNhanNut() {
    // 1. Menu Chinh (5 mục)
    bool isVi = (Loc::getLanguage() == Language::TIENG_VIET);
    if (auto b = menuChinh.layButton(0)) b->setLabel(isVi ? "CHƠI MỚI" : "PLAY");
    if (auto b = menuChinh.layButton(1)) b->setLabel(isVi ? "TIẾP TỤC" : "LOAD");
    if (auto b = menuChinh.layButton(2)) b->setLabel(Loc::get(LocKey::MENU_SETTINGS));
    if (auto b = menuChinh.layButton(3)) b->setLabel(isVi ? "THÔNG TIN" : "INFORMATION");
    if (auto b = menuChinh.layButton(4)) b->setLabel(Loc::get(LocKey::MENU_EXIT));
    menuChinh.capNhatNutTrangPhai();

    // 2. Settings Dashboard Tabs & Buttons
    if (settingsTabButtons.size() >= 4) {
        settingsTabButtons[0]->setLabel(Loc::get(LocKey::TAB_AUDIO));
        settingsTabButtons[1]->setLabel(Loc::get(LocKey::TAB_GRAPHICS));
        settingsTabButtons[2]->setLabel(Loc::get(LocKey::TAB_GAMEPLAY));
        settingsTabButtons[3]->setLabel(Loc::get(LocKey::TAB_CONTROLS));
    }
    if (sliderMaster) sliderMaster->setTitle(Loc::get(LocKey::SETTING_MASTER_VOL));
    if (sliderSfx) sliderSfx->setTitle(Loc::get(LocKey::SETTING_SFX_VOL));
    if (sliderBgm) sliderBgm->setTitle(Loc::get(LocKey::SETTING_BGM_VOL));
    if (sliderAtmosphere) sliderAtmosphere->setTitle(Loc::get(LocKey::SETTING_ATMOSPHERE_DENSITY));

    if (settingsAudioButtons.size() >= 2) {
        settingsAudioButtons[0]->setLabel(
            (isVi ? "Âm hiệu ứng (SFX): " : "Sound FX (SFX): ") +
            std::string(soundManager.getSfxBat() ? (isVi ? "BẬT" : "ON") : (isVi ? "TẮT" : "OFF"))
        );
        settingsAudioButtons[1]->setLabel(
            (isVi ? "Nhạc nền (BGM): " : "Music (BGM): ") +
            std::string(soundManager.getBgmBat() ? (isVi ? "BẬT" : "ON") : (isVi ? "TẮT" : "OFF"))
        );
    }

    if (settingsGraphicsButtons.size() >= 7) {
        std::string themeName = (boardTheme == BoardTheme::CLASSIC_WOOD) ? (isVi ? "Gỗ Cổ Điển" : "Aged Wood")
                              : (boardTheme == BoardTheme::IMPERIAL_JADE) ? (isVi ? "Ngọc Bích" : "Imperial Jade")
                              : (boardTheme == BoardTheme::MIDNIGHT_INK) ? (isVi ? "Mực Đen" : "Midnight Ink")
                              : (isVi ? "Trúc Thư" : "Warm Bamboo");
        std::string pieceName = (pieceStyle == PieceStyle::REALISTIC_WOOD) ? (isVi ? "Gỗ Cổ Điển" : "Classic Wood")
                              : (pieceStyle == PieceStyle::VIETNAMESE) ? (isVi ? "Việt Hóa" : "Vietnamese")
                              : (pieceStyle == PieceStyle::SHORT_CODE) ? (isVi ? "Rút Gọn" : "Short Code")
                              : (isVi ? "Quốc Tế" : "International");
        std::string atmoName = (atmosphere.getStyle() == AtmosphereStyle::BAMBOO_LEAVES) ? (isVi ? "Lá Trúc" : "Bamboo Leaves")
                             : (atmosphere.getStyle() == AtmosphereStyle::PEACH_BLOSSOMS) ? (isVi ? "Hoa Đào" : "Peach Blossoms")
                             : (isVi ? "Đốm Lửa" : "Floating Sparks");

        settingsGraphicsButtons[0]->setLabel((isVi ? "Mặt bàn cờ: " : "Board Theme: ") + themeName);
        settingsGraphicsButtons[1]->setLabel((isVi ? "Kiểu quân cờ: " : "Piece Style: ") + pieceName);
        settingsGraphicsButtons[2]->setLabel((isVi ? "Gợi ý nước đi: " : "Move Hints: ") + std::string(hienGoiY ? (isVi ? "BẬT" : "ON") : (isVi ? "TẮT" : "OFF")));
        settingsGraphicsButtons[3]->setLabel((isVi ? "Nước vừa đi: " : "Last Move: ") + std::string(hienNuocDiCuoi ? (isVi ? "BẬT" : "ON") : (isVi ? "TẮT" : "OFF")));
        settingsGraphicsButtons[4]->setLabel((isVi ? "Tọa độ bàn cờ: " : "Coordinates: ") + std::string(hienToaDo ? (isVi ? "BẬT" : "ON") : (isVi ? "TẮT" : "OFF")));
        settingsGraphicsButtons[5]->setLabel((isVi ? "Hiển thị: " : "Display: ") + std::string(fullscreenMode ? (isVi ? "Toàn Màn Hình" : "Fullscreen") : (isVi ? "Cửa Sổ" : "Windowed")));
        settingsGraphicsButtons[6]->setLabel((isVi ? "Khí quyển: " : "Atmosphere: ") + atmoName);
    }

    if (settingsGameplayButtons.size() >= 3) {
        settingsGameplayButtons[0]->setLabel(isVi ? "Ngôn ngữ: Tiếng Việt" : "Language: English");
        settingsGameplayButtons[1]->setLabel((isVi ? "Tốc độ AI: " : "AI Pacing: ") + std::string(aiThinkRealistic ? (isVi ? "Tự nhiên (450ms)" : "Natural (450ms)") : (isVi ? "Tức thì" : "Instant")));
        settingsGameplayButtons[2]->setLabel((isVi ? "Báo chiếu tướng: " : "Check Alarm: ") + std::string(checkAlarmSound ? (isVi ? "BẬT" : "ON") : (isVi ? "TẮT" : "OFF")));
    }

    if (settingsControlButtons.size() >= 7) {
        const char* actionNamesVi[7] = { "Lên", "Xuống", "Trái", "Phải", "Chọn cờ", "Bỏ chọn", "Thoát nhanh" };
        const char* actionNamesEn[7] = { "Cursor Up", "Cursor Down", "Cursor Left", "Cursor Right", "Select Piece", "Deselect", "Quick Quit" };
        for (int a = 0; a < 7; ++a) {
            KeyAction act = static_cast<KeyAction>(a);
            std::string actLabel = isVi ? actionNamesVi[a] : actionNamesEn[a];
            std::string keyVal = (rebindingAction == act)
                ? (isVi ? "[ Nhấn phím bất kỳ... ]" : "[ Press any key... ]")
                : ("[" + KeyConfig::getKeyName(keyConfig.getActionKey(act)) + "]");
            settingsControlButtons[a]->setLabel(actLabel + ":  " + keyVal);
            settingsControlButtons[a]->setSelected(rebindingAction == act);
        }
    }

    if (settingsBottomButtons.size() >= 3) {
        settingsBottomButtons[0]->setLabel(Loc::get(LocKey::BTN_APPLY_SAVE));
        settingsBottomButtons[1]->setLabel(Loc::get(LocKey::BTN_RESTORE_DEFAULTS));
        settingsBottomButtons[2]->setLabel(Loc::get(LocKey::BACK));
    }

    khoiTaoPlaySetupButtons();
    menuChinh.setFont(fontBold);
}

void GameManager::moMenuPopUp() {
    if (banCo.getKetThuc()) return;
    if (quanDangChon != nullptr) {
        quanDangChon = nullptr;
        cacNuocDiHopLe.clear();
    }
    hienMenuPopUp = true;
    soundManager.play(SoundType::CLICK);
}

void GameManager::dongMenuPopUp() {
    hienMenuPopUp = false;
    soundManager.play(SoundType::CLICK);
}

void GameManager::khoiTaoDashboardSettings() {
    settingsTabButtons.clear();
    settingsAudioButtons.clear();
    settingsGraphicsButtons.clear();
    settingsGameplayButtons.clear();
    settingsControlButtons.clear();
    settingsBottomButtons.clear();

    // Dashboard geometry: X=160, Y=120, W=880, H=560 in 1200x800
    // Top Horizontal Jade Tab Bar (Y = 140, H = 42, 4 tabs centered in 880px)
    float tabW = 190.0f;
    float tabH = 42.0f;
    float tabY = 140.0f;
    float tabGap = 16.0f;
    float tabStartX = 160.0f + (880.0f - (tabW * 4.0f + tabGap * 3.0f)) / 2.0f; // 196.0f

    settingsTabButtons.push_back(std::make_unique<Button>(tabStartX, tabY, tabW, tabH, Loc::get(LocKey::TAB_AUDIO), font, [this]() {
        activeSettingsTab = SettingsTab::AUDIO;
        rebindingAction = KeyAction::COUNT;
        soundManager.play(SoundType::CLICK);
    }));
    settingsTabButtons.push_back(std::make_unique<Button>(tabStartX + (tabW + tabGap), tabY, tabW, tabH, Loc::get(LocKey::TAB_GRAPHICS), font, [this]() {
        activeSettingsTab = SettingsTab::GRAPHICS;
        rebindingAction = KeyAction::COUNT;
        soundManager.play(SoundType::CLICK);
    }));
    settingsTabButtons.push_back(std::make_unique<Button>(tabStartX + (tabW + tabGap) * 2.0f, tabY, tabW, tabH, Loc::get(LocKey::TAB_GAMEPLAY), font, [this]() {
        activeSettingsTab = SettingsTab::GAMEPLAY;
        rebindingAction = KeyAction::COUNT;
        soundManager.play(SoundType::CLICK);
    }));
    settingsTabButtons.push_back(std::make_unique<Button>(tabStartX + (tabW + tabGap) * 3.0f, tabY, tabW, tabH, Loc::get(LocKey::TAB_CONTROLS), font, [this]() {
        activeSettingsTab = SettingsTab::CONTROLS;
        rebindingAction = KeyAction::COUNT;
        soundManager.play(SoundType::CLICK);
    }));
    for (auto& tb : settingsTabButtons) {
        tb->setCharacterSize(16);
    }

    // Symmetrical Left Column (X=188..592, interior colX=206, colW=368, Y=202..600)
    float colX = 206.0f;
    float colW = 368.0f;
    float startItemY = 232.0f;

    sliderMaster = std::make_unique<Slider>(colX, startItemY, colW, 20.0f, 0.0f, 100.0f,
        soundManager.getMasterVolume(), Loc::get(LocKey::SETTING_MASTER_VOL), font,
        [this](float val) { soundManager.setMasterVolume(val); });

    settingsAudioButtons.clear();
    settingsAudioButtons.push_back(std::make_unique<Button>(colX, 266.0f, colW, 36.0f, "", font, [this]() {
        soundManager.setSfxBat(!soundManager.getSfxBat());
        soundManager.play(SoundType::CLICK);
        capNhatNhanNut();
    }));
    settingsAudioButtons.back()->setCharacterSize(15);

    sliderSfx = std::make_unique<Slider>(colX, 336.0f, colW, 20.0f, 0.0f, 100.0f,
        soundManager.getSfxVolume(), Loc::get(LocKey::SETTING_SFX_VOL), font,
        [this](float val) { soundManager.setSfxVolume(val); });

    settingsAudioButtons.push_back(std::make_unique<Button>(colX, 370.0f, colW, 36.0f, "", font, [this]() {
        soundManager.setBgmBat(!soundManager.getBgmBat());
        soundManager.play(SoundType::CLICK);
        capNhatNhanNut();
    }));
    settingsAudioButtons.back()->setCharacterSize(15);

    sliderBgm = std::make_unique<Slider>(colX, 440.0f, colW, 20.0f, 0.0f, 100.0f,
        soundManager.getBgmVolume(), Loc::get(LocKey::SETTING_BGM_VOL), font,
        [this](float val) { soundManager.setBgmVolume(val); });

    // Interactive Soundboard (5 compact jade buttons inside 368px width)
    soundboardButtons.clear();
    float sbY = 516.0f;
    float sbW = 68.0f;
    float sbH = 38.0f;
    float sbGap = 7.0f;

    soundboardButtons.push_back(std::make_unique<Button>(colX, sbY, sbW, sbH, "Move", font, [this]() {
        soundManager.play(SoundType::MOVE);
    }));
    soundboardButtons.push_back(std::make_unique<Button>(colX + (sbW + sbGap), sbY, sbW, sbH, "Capture", font, [this]() {
        soundManager.play(SoundType::CAPTURE);
    }));
    soundboardButtons.push_back(std::make_unique<Button>(colX + (sbW + sbGap) * 2.0f, sbY, sbW, sbH, "Check", font, [this]() {
        soundManager.play(SoundType::CHECK);
    }));
    soundboardButtons.push_back(std::make_unique<Button>(colX + (sbW + sbGap) * 3.0f, sbY, sbW, sbH, "Win", font, [this]() {
        soundManager.play(SoundType::VICTORY);
    }));
    soundboardButtons.push_back(std::make_unique<Button>(colX + (sbW + sbGap) * 4.0f, sbY, sbW, sbH, "Click", font, [this]() {
        soundManager.play(SoundType::CLICK);
    }));
    for (auto& sb : soundboardButtons) {
        sb->setCharacterSize(13);
    }

    // Graphics Tab Controls
    settingsGraphicsButtons.clear();
    float gfxStartY = 216.0f;
    float gfxStep = 40.0f;
    float gfxH = 34.0f;

    settingsGraphicsButtons.push_back(std::make_unique<Button>(colX, gfxStartY, colW, gfxH, "", font, [this]() {
        int t = (static_cast<int>(boardTheme) + 1) % 4;
        boardTheme = static_cast<BoardTheme>(t);
        soundManager.play(SoundType::CLICK);
        capNhatNhanNut();
    }));
    settingsGraphicsButtons.push_back(std::make_unique<Button>(colX, gfxStartY + gfxStep, colW, gfxH, "", font, [this]() {
        int s = (static_cast<int>(pieceStyle) + 1) % 4;
        pieceStyle = static_cast<PieceStyle>(s);
        soundManager.play(SoundType::CLICK);
        capNhatNhanNut();
    }));
    settingsGraphicsButtons.push_back(std::make_unique<Button>(colX, gfxStartY + gfxStep * 2.0f, colW, gfxH, "", font, [this]() {
        hienGoiY = !hienGoiY;
        soundManager.play(SoundType::CLICK);
        capNhatNhanNut();
    }));
    settingsGraphicsButtons.push_back(std::make_unique<Button>(colX, gfxStartY + gfxStep * 3.0f, colW, gfxH, "", font, [this]() {
        hienNuocDiCuoi = !hienNuocDiCuoi;
        soundManager.play(SoundType::CLICK);
        capNhatNhanNut();
    }));
    settingsGraphicsButtons.push_back(std::make_unique<Button>(colX, gfxStartY + gfxStep * 4.0f, colW, gfxH, "", font, [this]() {
        hienToaDo = !hienToaDo;
        soundManager.play(SoundType::CLICK);
        capNhatNhanNut();
    }));
    settingsGraphicsButtons.push_back(std::make_unique<Button>(colX, gfxStartY + gfxStep * 5.0f, colW, gfxH, "", font, [this]() {
        fullscreenMode = !fullscreenMode;
        apDungCheDoManHinh();
        soundManager.play(SoundType::CLICK);
        capNhatNhanNut();
    }));
    settingsGraphicsButtons.push_back(std::make_unique<Button>(colX, gfxStartY + gfxStep * 6.0f, colW, gfxH, "", font, [this]() {
        atmosphere.cycleStyle();
        soundManager.play(SoundType::CLICK);
        capNhatNhanNut();
    }));
    for (auto& gb : settingsGraphicsButtons) {
        gb->setCharacterSize(15);
    }

    sliderAtmosphere = std::make_unique<Slider>(colX, 534.0f, colW, 20.0f, 0.0f, 100.0f,
        atmosphere.getDensity(), Loc::get(LocKey::SETTING_ATMOSPHERE_DENSITY), font,
        [this](float val) { atmosphere.setDensity(val); });

    // Gameplay Tab Controls
    float gpStartY = 236.0f;
    float gpItemH = 48.0f;
    float gpGapY = 68.0f;

    settingsGameplayButtons.push_back(std::make_unique<Button>(colX, gpStartY, colW, gpItemH, "", font, [this]() {
        Loc::toggleLanguage();
        capNhatFontTheoNgonNgu();
        capNhatNhanNut();
        soundManager.play(SoundType::CLICK);
    }));
    settingsGameplayButtons.push_back(std::make_unique<Button>(colX, gpStartY + gpGapY, colW, gpItemH, "", font, [this]() {
        aiThinkRealistic = !aiThinkRealistic;
        soundManager.play(SoundType::CLICK);
        capNhatNhanNut();
    }));
    settingsGameplayButtons.push_back(std::make_unique<Button>(colX, gpStartY + gpGapY * 2.0f, colW, gpItemH, "", font, [this]() {
        checkAlarmSound = !checkAlarmSound;
        soundManager.play(SoundType::CLICK);
        capNhatNhanNut();
    }));
    for (auto& pb : settingsGameplayButtons) {
        pb->setCharacterSize(15);
    }

    // Controls tab (7 keybinds + 3 presets)
    float ctrlStartY = 216.0f;
    float ctrlItemH = 34.0f;
    float ctrlGapY = 41.0f;

    for (int a = 0; a < 7; ++a) {
        KeyAction act = static_cast<KeyAction>(a);
        settingsControlButtons.push_back(std::make_unique<Button>(colX, ctrlStartY + ctrlGapY * a, colW, ctrlItemH, "", font, [this, act]() {
            rebindingAction = act;
            capNhatNhanNut();
            soundManager.play(SoundType::CLICK);
        }));
        settingsControlButtons.back()->setCharacterSize(15);
    }

    float pbtnW = 116.0f;
    float pbtnGap = 10.0f;
    float pbtnY = 516.0f;
    settingsControlButtons.push_back(std::make_unique<Button>(colX, pbtnY, pbtnW, 38.0f, "WASD", font, [this]() {
        keyConfig.setPreset(KeyPreset::WASD);
        rebindingAction = KeyAction::COUNT;
        capNhatNhanNut();
        soundManager.play(SoundType::CLICK);
    }));
    settingsControlButtons.push_back(std::make_unique<Button>(colX + (pbtnW + pbtnGap), pbtnY, pbtnW, 38.0f, "Arrows", font, [this]() {
        keyConfig.setPreset(KeyPreset::ARROWS);
        rebindingAction = KeyAction::COUNT;
        capNhatNhanNut();
        soundManager.play(SoundType::CLICK);
    }));
    settingsControlButtons.push_back(std::make_unique<Button>(colX + (pbtnW + pbtnGap) * 2.0f, pbtnY, pbtnW, 38.0f, "IJKL", font, [this]() {
        keyConfig.setPreset(KeyPreset::IJKL);
        rebindingAction = KeyAction::COUNT;
        capNhatNhanNut();
        soundManager.play(SoundType::CLICK);
    }));

    // Bottom action bar inside dashboard (Y = 618, centered across [160..1040])
    float bY = 618.0f;
    settingsBottomButtons.push_back(std::make_unique<Button>(240.0f, bY, 220.0f, 46.0f, Loc::get(LocKey::BTN_APPLY_SAVE), font, [this]() {
        luuCaiDat();
        soundManager.play(SoundType::VICTORY);
        hienToast((Loc::getLanguage() == Language::TIENG_VIET) ? "Đã lưu và áp dụng cài đặt!" : "Settings applied & saved!");
    }));

    settingsBottomButtons.push_back(std::make_unique<Button>(490.0f, bY, 220.0f, 46.0f, Loc::get(LocKey::BTN_RESTORE_DEFAULTS), font, [this]() {
        khoiPhucCaiDatMacDinh();
        soundManager.play(SoundType::CLICK);
        hienToast((Loc::getLanguage() == Language::TIENG_VIET) ? "Đã khôi phục mặc định." : "Restored defaults.");
    }));

    settingsBottomButtons.push_back(std::make_unique<Button>(740.0f, bY, 220.0f, 46.0f, Loc::get(LocKey::BACK), font, [this]() {
        rebindingAction = KeyAction::COUNT;
        soundManager.play(SoundType::CLICK);
        trangThai = trangThaiTruocSettings;
        if (trangThai == TrangThai::DANG_CHOI) {
            hienMenuPopUp = true;
        }
    }));

    capNhatNhanNut();
}

void GameManager::khoiTaoNutTrongGame() {
    inGameButtons.clear();
    inGameMenuButtons.clear();

    // 1. In-game Left Panel 2x2 Grid (Stage 3.2)
    // Row 1 (Y=330): Undo (35, 330, 115, 46) & Redo (160, 330, 115, 46)
    auto btnUndo = std::make_unique<Button>(35.0f, 330.0f, 115.0f, 46.0f, Loc::get(LocKey::UNDO), font, [this]() {
        thucHienHoanTac();
    });
    btnUndo->setCharacterSize(14);
    btnUndo->setColors(sf::Color(55, 42, 32, 240), sf::Color(85, 65, 48, 255), sf::Color(40, 30, 22, 255));
    inGameButtons.push_back(std::move(btnUndo));

    auto btnRedo = std::make_unique<Button>(160.0f, 330.0f, 115.0f, 46.0f, Loc::get(LocKey::REDO), font, [this]() {
        thucHienDiTiep();
    });
    btnRedo->setCharacterSize(14);
    btnRedo->setColors(sf::Color(55, 42, 32, 240), sf::Color(85, 65, 48, 255), sf::Color(40, 30, 22, 255));
    inGameButtons.push_back(std::move(btnRedo));

    // Row 2 (Y=386): Draw (35, 386, 115, 46) & Resign (160, 386, 115, 46)
    auto btnDraw = std::make_unique<Button>(35.0f, 386.0f, 115.0f, 46.0f, Loc::get(LocKey::DRAW_OFFER), font, [this]() {
        thucHienXinHoa();
    });
    btnDraw->setCharacterSize(14);
    btnDraw->setColors(sf::Color(45, 52, 65, 240), sf::Color(65, 80, 105, 255), sf::Color(35, 40, 52, 255));
    inGameButtons.push_back(std::move(btnDraw));

    auto btnResign = std::make_unique<Button>(160.0f, 386.0f, 115.0f, 46.0f, Loc::get(LocKey::SURRENDER), font, [this]() {
        thucHienDauHang();
    });
    btnResign->setCharacterSize(14);
    btnResign->setColors(sf::Color(75, 32, 32, 240), sf::Color(110, 45, 45, 255), sf::Color(55, 22, 22, 255));
    inGameButtons.push_back(std::move(btnResign));

    // Help '?' button (46x46 at 35, 445)
    auto btnHelp = std::make_unique<Button>(35.0f, 445.0f, 46.0f, 46.0f, "?", fontBold, [this]() {
        soundManager.play(SoundType::CLICK);
        hienToast(Loc::getLanguage() == Language::TIENG_VIET 
            ? "[WASD/Mui ten]: Chon o • [Enter/Space]: Di co • [ESC]: Menu" 
            : "[WASD/Arrows]: Select • [Enter/Space]: Move • [ESC]: Menu");
    });
    btnHelp->setCharacterSize(16);
    btnHelp->setColors(sf::Color(65, 55, 35, 240), sf::Color(105, 90, 55, 255), sf::Color(45, 38, 25, 255));
    inGameButtons.push_back(std::move(btnHelp));

    // Match Menu [ESC] button at X = 35.0f, Y = 660.0f, W = 240.0f, H = 48.0f
    auto btnOpen = std::make_unique<Button>(35.0f, 660.0f, 240.0f, 48.0f, Loc::get(LocKey::BTN_GAME_MENU), font, [this]() {
        moMenuPopUp();
    });
    btnOpen->setCharacterSize(15);
    btnOpen->setColors(sf::Color(45, 70, 115, 250), sf::Color(65, 100, 160, 255), sf::Color(30, 50, 85, 255));
    inGameButtons.push_back(std::move(btnOpen));

    // 2. Pause Modal Buttons (Stage 4, Modal centered at 400, 170, W=400, H=460)
    // Row 0 (Y=245): Large Resume button (320 x 44px, X=440)
    auto btnResume = std::make_unique<Button>(440.0f, 245.0f, 320.0f, 44.0f, Loc::get(LocKey::RESUME), font, [this]() {
        dongMenuPopUp();
    });
    btnResume->setColors(sf::Color(35, 125, 75, 245), sf::Color(45, 160, 95, 255), sf::Color(25, 95, 55, 255));
    inGameMenuButtons.push_back(std::move(btnResume));

    // Row 1 (Y=300): New Game (440, 300, 155, 40) & Draw Offer (605, 300, 155, 40)
    auto btnNew = std::make_unique<Button>(440.0f, 300.0f, 155.0f, 40.0f, Loc::get(LocKey::NEW_GAME), font, [this]() {
        dongMenuPopUp();
        batDauTroChoiMoi(cheDoChoi);
    });
    btnNew->setColors(sf::Color(55, 42, 32, 240), sf::Color(85, 65, 48, 255), sf::Color(40, 30, 22, 255));
    inGameMenuButtons.push_back(std::move(btnNew));

    auto btnDrawOffer = std::make_unique<Button>(605.0f, 300.0f, 155.0f, 40.0f, Loc::get(LocKey::DRAW_OFFER), font, [this]() {
        dongMenuPopUp();
        thucHienXinHoa();
    });
    btnDrawOffer->setColors(sf::Color(45, 52, 65, 240), sf::Color(65, 80, 105, 255), sf::Color(35, 40, 52, 255));
    inGameMenuButtons.push_back(std::move(btnDrawOffer));

    // Row 2 (Y=350): Save Game (440, 350, 155, 40) & Settings (605, 350, 155, 40)
    auto btnSave = std::make_unique<Button>(440.0f, 350.0f, 155.0f, 40.0f, Loc::get(LocKey::SAVE_GAME), font, [this]() {
        dongMenuPopUp();
        thucHienLuuGame();
    });
    btnSave->setColors(sf::Color(50, 48, 38, 240), sf::Color(78, 74, 58, 255), sf::Color(38, 36, 28, 255));
    inGameMenuButtons.push_back(std::move(btnSave));

    auto btnSettings = std::make_unique<Button>(605.0f, 350.0f, 155.0f, 40.0f, Loc::get(LocKey::MENU_SETTINGS), font, [this]() {
        dongMenuPopUp();
        trangThaiTruocSettings = TrangThai::DANG_CHOI;
        matchMenuTuDangChoi = true;
        trangThai = TrangThai::MENU_CHINH;
        menuChinh.triggerOpenAnimation(MenuSection::SETTINGS);
    });
    btnSettings->setColors(sf::Color(48, 55, 68, 240), sf::Color(72, 82, 102, 255), sf::Color(36, 42, 54, 255));
    inGameMenuButtons.push_back(std::move(btnSettings));

    // Row 3 (Y=400): Main Menu (440, 400, 320, 44)
    auto btnQuit = std::make_unique<Button>(440.0f, 400.0f, 320.0f, 44.0f, Loc::get(LocKey::BACK_MENU), font, [this]() {
        dongMenuPopUp();
        matchMenuTuDangChoi = false;
        trangThai = TrangThai::MENU_CHINH;
        menuChinh.triggerOpenAnimation(MenuSection::PLAY);
    });
    btnQuit->setColors(sf::Color(140, 40, 40, 245), sf::Color(175, 55, 55, 255), sf::Color(105, 28, 28, 255));
    inGameMenuButtons.push_back(std::move(btnQuit));
}

void GameManager::khoiTaoNutPopup() {
    popupButtons.clear();
    const float dw = 600.0f;
    const float dh = 440.0f;
    const float dx = (1200.0f - dw) / 2.0f;
    const float dy = (800.0f - dh) / 2.0f;
    const float pw = 230.0f;
    const float ph = 46.0f;
    const float py = dy + dh - 68.0f;

    bool tiengViet = (Loc::getLanguage() == Language::TIENG_VIET);
    sf::Font& fBtn = getMenuFont(tiengViet, true);

    auto& assets = getMenuAssets();

    std::string againLabel = Loc::get(LocKey::PLAY_AGAIN) + " (Enter)";
    auto btnAgain = std::make_unique<Button>(dx + 45.0f, py, pw, ph, againLabel, fBtn, [this]() {
        soundManager.play(SoundType::CLICK);
        batDauTroChoiMoi(cheDoChoi);
    });
    btnAgain->setColors(sf::Color(35, 125, 75, 245), sf::Color(45, 165, 95, 255), sf::Color(25, 95, 55, 255));
    if (assets.iconPlay.getSize().x > 0) btnAgain->setIcon(&assets.iconPlay);
    popupButtons.push_back(std::move(btnAgain));

    std::string backLabel = Loc::get(LocKey::BACK_MENU) + " (ESC)";
    auto btnBack = std::make_unique<Button>(dx + dw - 45.0f - pw, py, pw, ph, backLabel, fBtn, [this]() {
        soundManager.play(SoundType::CLICK);
        quayLaiMenu();
    });
    btnBack->setColors(sf::Color(140, 45, 45, 245), sf::Color(180, 60, 60, 255), sf::Color(105, 30, 30, 255));
    if (assets.iconExit.getSize().x > 0) btnBack->setIcon(&assets.iconExit);
    popupButtons.push_back(std::move(btnBack));
}

    // Config persistence
void GameManager::luuCaiDat() {
    std::ofstream f("config.cfg");
    if (!f.is_open()) return;

    f << "# Chinese Chess Commercial Configuration\n";
    f << "masterVolume=" << soundManager.getMasterVolume() << "\n";
    f << "sfxBat=" << (soundManager.getSfxBat() ? 1 : 0) << "\n";
    f << "sfxVolume=" << soundManager.getSfxVolume() << "\n";
    f << "bgmBat=" << (soundManager.getBgmBat() ? 1 : 0) << "\n";
    f << "bgmVolume=" << soundManager.getBgmVolume() << "\n";
    f << "boardTheme=" << static_cast<int>(boardTheme) << "\n";
    f << "pieceStyle=" << static_cast<int>(pieceStyle) << "\n";
    f << "hienGoiY=" << (hienGoiY ? 1 : 0) << "\n";
    f << "hienNuocDiCuoi=" << (hienNuocDiCuoi ? 1 : 0) << "\n";
    f << "hienToaDo=" << (hienToaDo ? 1 : 0) << "\n";
    f << "fullscreenMode=" << (fullscreenMode ? 1 : 0) << "\n";
    f << "language=" << ((Loc::getLanguage() == Language::TIENG_VIET) ? 1 : 0) << "\n";
    f << "aiRealistic=" << (aiThinkRealistic ? 1 : 0) << "\n";
    f << "checkAlarm=" << (checkAlarmSound ? 1 : 0) << "\n";
    f << "keyUp=" << static_cast<int>(keyConfig.keyUp) << "\n";
    f << "keyDown=" << static_cast<int>(keyConfig.keyDown) << "\n";
    f << "keyLeft=" << static_cast<int>(keyConfig.keyLeft) << "\n";
    f << "keyRight=" << static_cast<int>(keyConfig.keyRight) << "\n";
    f << "keySelect=" << static_cast<int>(keyConfig.keySelect) << "\n";
    f << "keyDeselect=" << static_cast<int>(keyConfig.keyDeselect) << "\n";
    f << "keyQuit=" << static_cast<int>(keyConfig.keyQuit) << "\n";
    f << "atmosphereStyle=" << static_cast<int>(atmosphere.getStyle()) << "\n";
    f << "atmosphereDensity=" << atmosphere.getDensity() << "\n";
    f << "p1Aura=" << static_cast<int>(p1Aura) << "\n";
    f << "p2Aura=" << static_cast<int>(p2Aura) << "\n";
}

void GameManager::docCaiDat() {
    std::ifstream f("config.cfg");
    if (!f.is_open()) return;

    std::string line;
    while (std::getline(f, line)) {
        if (line.empty() || line[0] == '#') continue;
        size_t eq = line.find('=');
        if (eq == std::string::npos) continue;

        std::string key = line.substr(0, eq);
        std::string val = line.substr(eq + 1);

        try {
            if (key == "masterVolume") {
                float v = std::stof(val);
                soundManager.setMasterVolume(v);
                if (sliderMaster) sliderMaster->setValue(v);
            }
            else if (key == "sfxBat") soundManager.setSfxBat(std::stoi(val) == 1);
            else if (key == "sfxVolume") {
                float v = std::stof(val);
                soundManager.setSfxVolume(v);
                if (sliderSfx) sliderSfx->setValue(v);
            }
            else if (key == "bgmBat") soundManager.setBgmBat(std::stoi(val) == 1);
            else if (key == "bgmVolume") {
                float v = std::stof(val);
                soundManager.setBgmVolume(v);
                if (sliderBgm) sliderBgm->setValue(v);
            }
            else if (key == "boardTheme") boardTheme = static_cast<BoardTheme>(std::stoi(val));
            else if (key == "pieceStyle") pieceStyle = static_cast<PieceStyle>(std::stoi(val));
            else if (key == "hienGoiY") hienGoiY = (std::stoi(val) == 1);
            else if (key == "hienNuocDiCuoi") hienNuocDiCuoi = (std::stoi(val) == 1);
            else if (key == "hienToaDo") hienToaDo = (std::stoi(val) == 1);
            else if (key == "fullscreenMode") fullscreenMode = (std::stoi(val) == 1);
            else if (key == "language") Loc::setLanguage((std::stoi(val) == 1) ? Language::TIENG_VIET : Language::ENGLISH);
            else if (key == "aiRealistic") aiThinkRealistic = (std::stoi(val) == 1);
            else if (key == "checkAlarm") checkAlarmSound = (std::stoi(val) == 1);
            else if (key == "keyUp") keyConfig.keyUp = static_cast<sf::Keyboard::Key>(std::stoi(val));
            else if (key == "keyDown") keyConfig.keyDown = static_cast<sf::Keyboard::Key>(std::stoi(val));
            else if (key == "keyLeft") keyConfig.keyLeft = static_cast<sf::Keyboard::Key>(std::stoi(val));
            else if (key == "keyRight") keyConfig.keyRight = static_cast<sf::Keyboard::Key>(std::stoi(val));
            else if (key == "keySelect") keyConfig.keySelect = static_cast<sf::Keyboard::Key>(std::stoi(val));
            else if (key == "keyDeselect") keyConfig.keyDeselect = static_cast<sf::Keyboard::Key>(std::stoi(val));
            else if (key == "keyQuit") keyConfig.keyQuit = static_cast<sf::Keyboard::Key>(std::stoi(val));
            else if (key == "atmosphereStyle") atmosphere.setStyle(static_cast<AtmosphereStyle>(std::stoi(val)));
            else if (key == "atmosphereDensity") {
                float d = std::stof(val);
                atmosphere.setDensity(d);
                if (sliderAtmosphere) sliderAtmosphere->setValue(d);
            }
            else if (key == "p1Aura") p1Aura = static_cast<PieceAura>(std::stoi(val));
            else if (key == "p2Aura") p2Aura = static_cast<PieceAura>(std::stoi(val));
        } catch (...) {}
    }
}

void GameManager::khoiPhucCaiDatMacDinh() {
    soundManager.setMasterVolume(80.0f);
    soundManager.setSfxBat(true);
    soundManager.setSfxVolume(80.0f);
    soundManager.setBgmBat(true);
    soundManager.setBgmVolume(50.0f);

    atmosphere.setStyle(AtmosphereStyle::BAMBOO_LEAVES);
    atmosphere.setDensity(50.0f);
    p1Aura = PieceAura::IMPERIAL_GOLD;
    p2Aura = PieceAura::DRAGON_FLAME;

    if (sliderMaster) sliderMaster->setValue(80.0f);
    if (sliderSfx) sliderSfx->setValue(80.0f);
    if (sliderBgm) sliderBgm->setValue(50.0f);
    if (sliderAtmosphere) sliderAtmosphere->setValue(50.0f);

    boardTheme = BoardTheme::CLASSIC_WOOD;
    pieceStyle = PieceStyle::REALISTIC_WOOD;
    hienGoiY = true;
    hienNuocDiCuoi = true;
    hienToaDo = true;
    aiThinkRealistic = true;
    checkAlarmSound = true;
    keyConfig.setPreset(KeyPreset::WASD);

    if (fullscreenMode) {
        fullscreenMode = false;
        apDungCheDoManHinh();
    }

    capNhatNhanNut();
}

void GameManager::capNhatViewManHinh() {
    sf::Vector2u winSz = window.getSize();
    if (winSz.x == 0 || winSz.y == 0) return;

    const float baseWidth = 1200.0f;
    const float baseHeight = 800.0f;
    const float targetRatio = baseWidth / baseHeight; // 1.5f

    float windowWidth = static_cast<float>(winSz.x);
    float windowHeight = static_cast<float>(winSz.y);
    float windowRatio = windowWidth / windowHeight;

    sf::View view(sf::FloatRect(0.0f, 0.0f, baseWidth, baseHeight));
    if (windowRatio >= targetRatio) {
        float viewportW = targetRatio / windowRatio;
        float viewportX = (1.0f - viewportW) / 2.0f;
        view.setViewport(sf::FloatRect(viewportX, 0.0f, viewportW, 1.0f));
    } else {
        float viewportH = windowRatio / targetRatio;
        float viewportY = (1.0f - viewportH) / 2.0f;
        view.setViewport(sf::FloatRect(0.0f, viewportY, 1.0f, viewportH));
    }
    window.setView(view);
}

void GameManager::apDungCheDoManHinh() {
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    if (fullscreenMode) {
        window.create(desktop, "Co Tuong - Chinese Chess (HCMUS OOP)", sf::Style::Fullscreen);
    } else {
        unsigned int targetW = 1200;
        unsigned int targetH = 800;
        if (desktop.width < targetW || desktop.height < targetH) {
            targetW = std::min(1200u, desktop.width > 60u ? desktop.width - 60u : desktop.width);
            targetH = std::min(800u, desktop.height > 80u ? desktop.height - 80u : desktop.height);
        }
        window.create(sf::VideoMode(targetW, targetH), "Co Tuong - Chinese Chess (HCMUS OOP)", sf::Style::Default);
        if (desktop.width > window.getSize().x && desktop.height > window.getSize().y) {
            window.setPosition(sf::Vector2i(
                static_cast<int>((desktop.width - window.getSize().x) / 2),
                static_cast<int>((desktop.height - window.getSize().y) / 2)
            ));
        }
    }
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);
    window.setMouseCursorVisible(false);
    capNhatViewManHinh();
}

void GameManager::batDauPvp() {
    std::string n1 = inputP1Name->getValue();
    if (n1.empty()) n1 = "Player 1";
    std::string n2 = inputP2Name->getValue();
    if (n2.empty()) n2 = "Player 2";

    if (pvpFirstPlayer == 1) {
        tenNguoiChoiDo = n1;
        nhanVatDo = p1CharIndex;
        auraDo = p1Aura;
        tenNguoiChoiDen = n2;
        nhanVatDen = p2CharIndex;
        auraDen = p2Aura;
    } else {
        tenNguoiChoiDo = n2;
        nhanVatDo = p2CharIndex;
        auraDo = p2Aura;
        tenNguoiChoiDen = n1;
        nhanVatDen = p1CharIndex;
        auraDen = p1Aura;
    }

    batDauTroChoiMoi(CheDoChoi::HAI_NGUOI);
    std::string msg = (Loc::getLanguage() == Language::TIENG_VIET)
        ? (tenNguoiChoiDo + " cam quan DO di truoc!")
        : (tenNguoiChoiDo + " plays RED and moves first!");
    hienToast(msg);
}

void GameManager::batDauPvai() {
    std::string n = inputAiPlayerName->getValue();
    if (n.empty()) n = "Player";

    Mau chosenSide = Mau::DO;
    if (aiFirstChoice == 0) chosenSide = Mau::DO;
    else if (aiFirstChoice == 1) chosenSide = Mau::DEN;
    else chosenSide = (rand() % 2 == 0) ? Mau::DO : Mau::DEN;

    aiPlayerSide = chosenSide;
    
    std::string aiName = "AI (" + Loc::get((aiDifficulty == DoKho::DE) ? LocKey::DIFF_EASY : ((aiDifficulty == DoKho::TRUNG_BINH) ? LocKey::DIFF_MED : LocKey::DIFF_HARD)) + ")";

    if (chosenSide == Mau::DO) {
        tenNguoiChoiDo = n;
        nhanVatDo = aiPlayerCharIndex;
        auraDo = p1Aura;
        tenNguoiChoiDen = aiName;
        nhanVatDen = 1;
        auraDen = p2Aura;
    } else {
        tenNguoiChoiDo = aiName;
        nhanVatDo = 1;
        auraDo = p2Aura;
        tenNguoiChoiDen = n;
        nhanVatDen = aiPlayerCharIndex;
        auraDen = p1Aura;
    }

    batDauTroChoiMoi(CheDoChoi::VOI_MAY);
    std::string msg = (Loc::getLanguage() == Language::TIENG_VIET)
        ? (tenNguoiChoiDo + " cam quan DO di truoc!")
        : (tenNguoiChoiDo + " plays RED and moves first!");
    hienToast(msg);
}

void GameManager::batDauTroChoiMoi(CheDoChoi cheDo) {
    cheDoChoi = cheDo;
    if (cheDo == CheDoChoi::DEMO_2XE_2VOI) {
        banCo.khoiTao2XeVa2Voi();
        viTriConTro = sf::Vector2i(9, 0);
        hienToast(Loc::get(LocKey::PLAY_DEMO));
    } else {
        banCo.lamMoi();
        viTriConTro = sf::Vector2i(9, 4);
    }

    quanDangChon = nullptr;
    cacNuocDiHopLe.clear();
    trangThai = TrangThai::DANG_CHOI;
    hienMenuPopUp = false;
    
    thoiGianConDo = thoiGianVanDau;
    thoiGianConDen = thoiGianVanDau;
    dtClock.restart();
    
    aiDangSuyNghi = false;
    thoiGianChoAI = 0.0f;
    lyDoKetThuc = "";
    daPhatAmKetThuc = false;
    toastTimer = 0.0f;

    soundManager.play(SoundType::MOVE);
}

    // Save and load
std::string GameManager::layDuongDanSlot(int slotIndex) const {
    if (slotIndex == 0) return "saves/slot1.txt";
    if (slotIndex == 1) return "saves/slot2.txt";
    return "saves/slot3.txt";
}

void GameManager::capNhatThongTinSlots() {
    try {
        fs::create_directories("saves");
    } catch (...) {}

    for (int i = 0; i < 3; ++i) {
        std::string path = layDuongDanSlot(i);
        std::string metaPath = path + ".meta";
        SaveSlotInfo& info = slotInfos[i];

        if (fs::exists(path)) {
            info.tonTai = true;
            info.thoiGian = layThoiGianHienTai();
            info.tenCheDo = "Classic Match";
            info.tenDo = "Red";
            info.tenDen = "Black";
            info.soNuoc = 0;
            info.luot = Mau::DO;

            if (fs::exists(metaPath)) {
                std::ifstream mf(metaPath);
                if (mf.is_open()) {
                    std::getline(mf, info.thoiGian);
                    std::getline(mf, info.tenCheDo);
                    std::getline(mf, info.tenDo);
                    std::getline(mf, info.tenDen);
                    std::string soNuocStr, luotStr;
                    if (std::getline(mf, soNuocStr)) info.soNuoc = std::atoi(soNuocStr.c_str());
                    if (std::getline(mf, luotStr)) info.luot = (luotStr == "DEN") ? Mau::DEN : Mau::DO;
                }
            }
        } else {
            info.tonTai = false;
        }
    }
}

void GameManager::luuVaoSlot(int slotIndex) {
    try {
        fs::create_directories("saves");
    } catch (...) {}

    std::string path = layDuongDanSlot(slotIndex);
    if (banCo.luuFile(path)) {
        banCo.luuFile("savegame.txt");

        std::string metaPath = path + ".meta";
        std::ofstream mf(metaPath);
        if (mf.is_open()) {
            mf << layThoiGianHienTai() << "\n";
            mf << ((cheDoChoi == CheDoChoi::HAI_NGUOI) ? "PvP" : ((cheDoChoi == CheDoChoi::VOI_MAY) ? "PvAI" : "Demo")) << "\n";
            mf << tenNguoiChoiDo << "\n";
            mf << tenNguoiChoiDen << "\n";
            mf << banCo.getLichSuNuocDi().size() << "\n";
            mf << ((banCo.getLuotChoi() == Mau::DO) ? "DO" : "DEN") << "\n";
        }

        capNhatThongTinSlots();
        soundManager.play(SoundType::CLICK);
        std::string msg = (Loc::getLanguage() == Language::TIENG_VIET)
            ? ("Da luu vao o so " + std::to_string(slotIndex + 1) + " thanh cong!")
            : ("Saved to Slot " + std::to_string(slotIndex + 1) + " successfully!");
        hienToast(msg);
    }
}

bool GameManager::docTuSlot(int slotIndex) {
    std::string path = layDuongDanSlot(slotIndex);
    if (!fs::exists(path) && fs::exists("savegame.txt")) {
        path = "savegame.txt";
    }

    if (banCo.docFile(path)) {
        std::string metaPath = layDuongDanSlot(slotIndex) + ".meta";
        if (fs::exists(metaPath)) {
            std::ifstream mf(metaPath);
            if (mf.is_open()) {
                std::string timeStr, modeStr, d, b;
                std::getline(mf, timeStr);
                std::getline(mf, modeStr);
                std::getline(mf, d);
                std::getline(mf, b);
                if (!d.empty()) tenNguoiChoiDo = d;
                if (!b.empty()) tenNguoiChoiDen = b;
                if (modeStr == "PvAI") cheDoChoi = CheDoChoi::VOI_MAY;
                else if (modeStr == "Demo") cheDoChoi = CheDoChoi::DEMO_2XE_2VOI;
                else cheDoChoi = CheDoChoi::HAI_NGUOI;
            }
        }

        quanDangChon = nullptr;
        cacNuocDiHopLe.clear();
        viTriConTro = sf::Vector2i(9, 4);
        trangThai = TrangThai::DANG_CHOI;
        hienMenuPopUp = false;
        aiDangSuyNghi = false;
        thoiGianChoAI = 0.0f;
        daPhatAmKetThuc = false;
        dtClock.restart();

        soundManager.play(SoundType::CLICK);
        std::string msg = (Loc::getLanguage() == Language::TIENG_VIET)
            ? ("Da tai van co tu o so " + std::to_string(slotIndex + 1) + "!")
            : ("Loaded game from Slot " + std::to_string(slotIndex + 1) + "!");
        hienToast(msg);
        return true;
    }
    return false;
}

void GameManager::xoaSlot(int slotIndex) {
    std::string path = layDuongDanSlot(slotIndex);
    std::string metaPath = path + ".meta";
    try {
        if (fs::exists(path)) fs::remove(path);
        if (fs::exists(metaPath)) fs::remove(metaPath);
    } catch (...) {}

    capNhatThongTinSlots();
    soundManager.play(SoundType::CLICK);
    std::string msg = (Loc::getLanguage() == Language::TIENG_VIET)
        ? ("Da xoa ban luu o so " + std::to_string(slotIndex + 1))
        : ("Deleted save in Slot " + std::to_string(slotIndex + 1));
    hienToast(msg);
}

    // In-game logic
void GameManager::chay() {
    sf::Clock clock;
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        
        xuLySuKien();
        
        if (trangThai == TrangThai::DANG_CHOI && !banCo.getKetThuc() && !hienMenuPopUp) {
            capNhatDongHo(dt);
            if (cheDoChoi == CheDoChoi::VOI_MAY) {
                xuLyLuotAI(dt);
            }
        }
        
        if (toastTimer > 0.0f) {
            toastTimer -= dt;
        }
        
        atmosphere.update(dt);
        ve();
    }
}

void GameManager::xuLySuKien() {
    capNhatViewManHinh();
    sf::Event event;
    sf::Vector2f worldCoords = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    sf::Vector2i mousePos(static_cast<int>(std::round(worldCoords.x)), static_cast<int>(std::round(worldCoords.y)));
    
    // Update button hovers
    if (trangThai == TrangThai::MENU_CHINH) menuChinh.update(mousePos);
    else if (trangThai == TrangThai::PLAY_SUBMENU) menuPlaySub.update(mousePos);
    else if (trangThai == TrangThai::PVP_SETUP || trangThai == TrangThai::PVAI_SETUP) {
        for (auto& btn : playSetupButtons) btn->update(mousePos);
    }

    else if (trangThai == TrangThai::LOAD_MENU) menuLoad.update(mousePos);
    else if (trangThai == TrangThai::SETTINGS_MENU) {
        for (auto& btn : settingsTabButtons) btn->update(mousePos);
        for (auto& btn : settingsBottomButtons) btn->update(mousePos);
        if (activeSettingsTab == SettingsTab::AUDIO) {
            for (auto& btn : settingsAudioButtons) btn->update(mousePos);
            for (auto& btn : soundboardButtons) btn->update(mousePos);
        } else if (activeSettingsTab == SettingsTab::GRAPHICS) {
            for (auto& btn : settingsGraphicsButtons) btn->update(mousePos);
        } else if (activeSettingsTab == SettingsTab::GAMEPLAY) {
            for (auto& btn : settingsGameplayButtons) btn->update(mousePos);
        } else if (activeSettingsTab == SettingsTab::CONTROLS) {
            for (auto& btn : settingsControlButtons) btn->update(mousePos);
        }
    }
    else if (trangThai == TrangThai::INTRODUCTION_MENU) menuIntro.update(mousePos);
    else if (trangThai == TrangThai::DANG_CHOI) {
        if (banCo.getKetThuc()) {
            for (auto& btn : popupButtons) btn->update(mousePos);
        } else if (hienMenuPopUp) {
            for (auto& btn : inGameMenuButtons) btn->update(mousePos);
        } else {
            for (auto& btn : inGameButtons) btn->update(mousePos);
        }
    }

    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            luuCaiDat();
            window.close();
        }
        else if (event.type == sf::Event::Resized) {
            capNhatViewManHinh();
        }
        else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Return && event.key.alt) {
            fullscreenMode = !fullscreenMode;
            apDungCheDoManHinh();
            capNhatNhanNut();
        }
        else if (trangThai == TrangThai::SETTINGS_MENU) {
            if (activeSettingsTab == SettingsTab::AUDIO) {
                if (sliderMaster) sliderMaster->handleEvent(event, mousePos);
                if (sliderSfx) sliderSfx->handleEvent(event, mousePos);
                if (sliderBgm) sliderBgm->handleEvent(event, mousePos);
            } else if (activeSettingsTab == SettingsTab::GRAPHICS) {
                if (sliderAtmosphere) sliderAtmosphere->handleEvent(event, mousePos);
            }
        }
        
        if (event.type == sf::Event::MouseMoved) {
            sf::Vector2f mc = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
            mousePos = sf::Vector2i(static_cast<int>(std::round(mc.x)), static_cast<int>(std::round(mc.y)));
            if (trangThai == TrangThai::MENU_CHINH) menuChinh.update(mousePos);
            else if (trangThai == TrangThai::PLAY_SUBMENU) menuPlaySub.update(mousePos);
            else if (trangThai == TrangThai::PVP_SETUP || trangThai == TrangThai::PVAI_SETUP) {
                for (auto& btn : playSetupButtons) btn->update(mousePos);
            }
            else if (trangThai == TrangThai::LOAD_MENU) menuLoad.update(mousePos);
            else if (trangThai == TrangThai::SETTINGS_MENU) {
                for (auto& btn : settingsTabButtons) btn->update(mousePos);
                for (auto& btn : settingsBottomButtons) btn->update(mousePos);
                if (activeSettingsTab == SettingsTab::AUDIO) {
                    for (auto& btn : settingsAudioButtons) btn->update(mousePos);
                    for (auto& btn : soundboardButtons) btn->update(mousePos);
                } else if (activeSettingsTab == SettingsTab::GRAPHICS) {
                    for (auto& btn : settingsGraphicsButtons) btn->update(mousePos);
                } else if (activeSettingsTab == SettingsTab::GAMEPLAY) {
                    for (auto& btn : settingsGameplayButtons) btn->update(mousePos);
                } else if (activeSettingsTab == SettingsTab::CONTROLS) {
                    for (auto& btn : settingsControlButtons) btn->update(mousePos);
                }
            }
            else if (trangThai == TrangThai::INTRODUCTION_MENU) menuIntro.update(mousePos);
            else if (trangThai == TrangThai::DANG_CHOI) {
                if (banCo.getKetThuc()) {
                    for (auto& btn : popupButtons) btn->update(mousePos);
                } else if (hienMenuPopUp) {
                    for (auto& btn : inGameMenuButtons) btn->update(mousePos);
                } else {
                    for (auto& btn : inGameButtons) btn->update(mousePos);
                }
            }
        }
        else if (event.type == sf::Event::TextEntered) {
            xuLyTextEntered(event.text.unicode);
        }
        else if (event.type == sf::Event::MouseButtonPressed) {
            sf::Vector2f mc = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
            sf::Vector2i clickPos(static_cast<int>(std::round(mc.x)), static_cast<int>(std::round(mc.y)));
            if (event.mouseButton.button == sf::Mouse::Left) {
                if (trangThai == TrangThai::MENU_CHINH) {
                    menuChinh.handleMouseDown(clickPos);
                }
                xuLyClickChuot(clickPos);
            } else if (event.mouseButton.button == sf::Mouse::Right) {
                if (trangThai == TrangThai::DANG_CHOI && !hienMenuPopUp && !banCo.getKetThuc()) {
                    if (quanDangChon != nullptr) {
                        quanDangChon = nullptr;
                        cacNuocDiHopLe.clear();
                        soundManager.play(SoundType::CLICK);
                    }
                }
            }
        }
        else if (event.type == sf::Event::MouseButtonReleased) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mc = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                sf::Vector2i relPos(static_cast<int>(std::round(mc.x)), static_cast<int>(std::round(mc.y)));
                if (trangThai == TrangThai::MENU_CHINH) {
                    menuChinh.handleMouseUp(relPos);
                }
            }
        }
        else if (event.type == sf::Event::KeyPressed) {
            xuLyBanPhim(event.key.code, event.key.control);
        }
    }
}

void GameManager::xuLyTextEntered(sf::Uint32 unicode) {
    if (trangThai == TrangThai::PVP_SETUP || trangThai == TrangThai::PVAI_SETUP) {
        if (playSetupMode == 0) {
            inputAiPlayerName->handleTextEntered(unicode);
        } else if (playSetupMode == 1) {
            inputP1Name->handleTextEntered(unicode);
            inputP2Name->handleTextEntered(unicode);
        }
    }
}


void GameManager::xuLyClickChuot(const sf::Vector2i& viTri) {
    if (trangThai == TrangThai::MENU_CHINH) {
        menuChinh.handleClick(viTri);
    }
    else if (trangThai == TrangThai::PLAY_SUBMENU) {
        menuPlaySub.handleClick(viTri);
    }
    else if (trangThai == TrangThai::PVP_SETUP || trangThai == TrangThai::PVAI_SETUP) {
        xuLyClickPlaySetup(viTri);
    }
    else if (trangThai == TrangThai::LOAD_MENU) {
        for (int i = 0; i < 3; ++i) {
            float sy = 160.0f + i * 145.0f;
            sf::FloatRect slotBounds(150.0f, sy, 900.0f, 125.0f);
            if (slotBounds.contains(static_cast<float>(viTri.x), static_cast<float>(viTri.y))) {
                selectedSlot = i;
                soundManager.play(SoundType::CLICK);
            }
        }
        menuLoad.handleClick(viTri);
    }
    else if (trangThai == TrangThai::SETTINGS_MENU) {
        for (auto& btn : settingsTabButtons) btn->handleClick(viTri);
        for (auto& btn : settingsBottomButtons) btn->handleClick(viTri);
        if (activeSettingsTab == SettingsTab::AUDIO) {
            for (auto& btn : settingsAudioButtons) btn->handleClick(viTri);
            for (auto& btn : soundboardButtons) btn->handleClick(viTri);
        } else if (activeSettingsTab == SettingsTab::GRAPHICS) {
            for (auto& btn : settingsGraphicsButtons) btn->handleClick(viTri);
        } else if (activeSettingsTab == SettingsTab::GAMEPLAY) {
            for (auto& btn : settingsGameplayButtons) btn->handleClick(viTri);
        } else if (activeSettingsTab == SettingsTab::CONTROLS) {
            for (auto& btn : settingsControlButtons) btn->handleClick(viTri);
        }
    }
    else if (trangThai == TrangThai::INTRODUCTION_MENU) {
        menuIntro.handleClick(viTri);
    }
    else if (trangThai == TrangThai::DANG_CHOI) {
        if (banCo.getKetThuc()) {
            for (auto& btn : popupButtons) {
                btn->handleClick(viTri);
            }
            return;
        }

        if (hienMenuPopUp) {
            for (auto& btn : inGameMenuButtons) {
                if (btn->contains(viTri)) {
                    btn->handleClick(viTri);
                    break;
                }
            }
            return;
        }

        bool clickNutInGame = false;
        for (auto& btn : inGameButtons) {
            if (btn->contains(viTri)) {
                btn->handleClick(viTri);
                clickNutInGame = true;
                break;
            }
        }
        if (clickNutInGame) return;

        if (cheDoChoi == CheDoChoi::VOI_MAY && banCo.getLuotChoi() == aiPlayerSide && aiDangSuyNghi) {
            return;
        }

        sf::Vector2i toaDoBanCo = chuyenDoiToaDoManHinhThanhBanCo(viTri);
        if (toaDoBanCo.x != -1) {
            viTriConTro = toaDoBanCo;
            if (quanDangChon == nullptr) {
                chonQuan(toaDoBanCo.x, toaDoBanCo.y);
            } else {
                diChuyenQuan(toaDoBanCo.x, toaDoBanCo.y);
            }
        }
    }
}

void GameManager::xuLyBanPhim(sf::Keyboard::Key key, bool ctrl) {
    // Key rebinding
    if (trangThai == TrangThai::SETTINGS_MENU && activeSettingsTab == SettingsTab::CONTROLS && rebindingAction != KeyAction::COUNT) {
        if (key != sf::Keyboard::Escape) {
            keyConfig.setActionKey(rebindingAction, key);
            soundManager.play(SoundType::CLICK);
        }
        rebindingAction = KeyAction::COUNT;
        capNhatNhanNut();
        return;
    }

    // Quick quit
    if (keyConfig.isQuitKey(key)) {
        if (trangThai != TrangThai::MENU_CHINH || !menuChinh.isClosed()) {
            return;
        }
        soundManager.play(SoundType::CLICK);
        luuCaiDat();
        window.close();
        return;
    }

    if (trangThai == TrangThai::MENU_CHINH) {
        if (key == sf::Keyboard::W || key == sf::Keyboard::Up) {
            menuChinh.navigateUp();
            soundManager.play(SoundType::CLICK);
            return;
        }
        if (key == sf::Keyboard::S || key == sf::Keyboard::Down) {
            menuChinh.navigateDown();
            soundManager.play(SoundType::CLICK);
            return;
        }
        if (key == sf::Keyboard::A || key == sf::Keyboard::Left) {
            menuChinh.navigateLeft();
            soundManager.play(SoundType::CLICK);
            return;
        }
        if (key == sf::Keyboard::D || key == sf::Keyboard::Right) {
            menuChinh.navigateRight();
            soundManager.play(SoundType::CLICK);
            return;
        }
        if (key == sf::Keyboard::Escape) {
            if (!menuChinh.isClosed()) {
                if (menuChinh.getFocus() == Menu::MenuFocus::SUB_MENU) {
                    menuChinh.switchToMainMenu();
                    soundManager.play(SoundType::CLICK);
                    return;
                }
                if (matchMenuTuDangChoi) {
                    trangThai = TrangThai::DANG_CHOI;
                    matchMenuTuDangChoi = false;
                    hienMenuPopUp = false;
                    soundManager.play(SoundType::CLICK);
                    return;
                }
                menuChinh.closeBook();
                soundManager.play(SoundType::CLICK);
            }
            return;
        }
        if (key == sf::Keyboard::Return || key == sf::Keyboard::Space) {
            menuChinh.activateCurrent();
            soundManager.play(SoundType::CLICK);
            return;
        }
        return;
    }

    if (trangThai == TrangThai::PVP_SETUP || trangThai == TrangThai::PVAI_SETUP) {
        // Text input active
        bool textInputFocused = (inputAiPlayerName && inputAiPlayerName->isFocused()) ||
                                (inputP1Name && inputP1Name->isFocused()) ||
                                (inputP2Name && inputP2Name->isFocused());
        if (textInputFocused) {
            if (key == sf::Keyboard::Escape || key == sf::Keyboard::Return) {
                if (inputAiPlayerName) inputAiPlayerName->setFocused(false);
                if (inputP1Name) inputP1Name->setFocused(false);
                if (inputP2Name) inputP2Name->setFocused(false);
            }
            return;
        }

        if (key == sf::Keyboard::W || key == sf::Keyboard::Up) {
            soundManager.play(SoundType::CLICK);
            playSetupMode = (playSetupMode + 2) % 3;
            khoiTaoPlaySetupButtons();
            return;
        }
        if (key == sf::Keyboard::S || key == sf::Keyboard::Down) {
            soundManager.play(SoundType::CLICK);
            playSetupMode = (playSetupMode + 1) % 3;
            khoiTaoPlaySetupButtons();
            return;
        }
        if (key == sf::Keyboard::A || key == sf::Keyboard::Left) {
            soundManager.play(SoundType::CLICK);
            if (thoiGianVanDau == 300.0f) thoiGianVanDau = 0.0f;
            else if (thoiGianVanDau == 600.0f) thoiGianVanDau = 300.0f;
            else if (thoiGianVanDau == 900.0f) thoiGianVanDau = 600.0f;
            else if (thoiGianVanDau == 1800.0f) thoiGianVanDau = 900.0f;
            else thoiGianVanDau = 1800.0f;
            khoiTaoPlaySetupButtons();
            return;
        }
        if (key == sf::Keyboard::D || key == sf::Keyboard::Right) {
            soundManager.play(SoundType::CLICK);
            if (thoiGianVanDau == 0.0f) thoiGianVanDau = 300.0f;
            else if (thoiGianVanDau == 300.0f) thoiGianVanDau = 600.0f;
            else if (thoiGianVanDau == 600.0f) thoiGianVanDau = 900.0f;
            else if (thoiGianVanDau == 900.0f) thoiGianVanDau = 1800.0f;
            else thoiGianVanDau = 0.0f;
            khoiTaoPlaySetupButtons();
            return;
        }
        if (key == sf::Keyboard::Escape) {
            soundManager.play(SoundType::CLICK);
            trangThai = TrangThai::MENU_CHINH;
            menuChinh.triggerOpenAnimation(MenuSection::PLAY);
            return;
        }
        if (key == sf::Keyboard::Return) {
            soundManager.play(SoundType::CLICK);
            if (playSetupMode == 0) batDauPvai();
            else if (playSetupMode == 1) batDauPvp();
            else {
                nhanVatDo = 0; nhanVatDen = 0;
                batDauTroChoiMoi(CheDoChoi::DEMO_2XE_2VOI);
            }
            return;
        }
        return;
    }

    // 3. Deselect or Return back
    if (keyConfig.isDeselectKey(key)) {
        soundManager.play(SoundType::CLICK);
        if (trangThai == TrangThai::PLAY_SUBMENU || trangThai == TrangThai::INTRODUCTION_MENU) {
            rebindingAction = KeyAction::COUNT;
            trangThai = TrangThai::MENU_CHINH;
            return;
        }
        if (trangThai == TrangThai::LOAD_MENU) {
            trangThai = trangThaiTruocLoad;
            if (trangThai == TrangThai::DANG_CHOI) {
                hienMenuPopUp = true;
            }
            return;
        }
        if (trangThai == TrangThai::SETTINGS_MENU) {
            rebindingAction = KeyAction::COUNT;
            trangThai = trangThaiTruocSettings;
            if (trangThai == TrangThai::DANG_CHOI) {
                hienMenuPopUp = true;
            }
            return;
        }
        if (trangThai == TrangThai::PVP_SETUP || trangThai == TrangThai::PVAI_SETUP) {
            soundManager.play(SoundType::CLICK);
            trangThai = TrangThai::MENU_CHINH;
            menuChinh.triggerOpenAnimation(MenuSection::PLAY);
            return;
        }
        if (trangThai == TrangThai::DANG_CHOI) {
            if (banCo.getKetThuc()) {
                soundManager.play(SoundType::CLICK);
                quayLaiMenu();
                return;
            }

            if (hienMenuPopUp) {
                dongMenuPopUp();
            } else {
                moMenuPopUp();
            }
            return;
        }
    }

    // 4. In game navigation and shortcuts
    if (trangThai == TrangThai::DANG_CHOI) {
        if (banCo.getKetThuc()) {
            if (key == sf::Keyboard::Return || key == sf::Keyboard::Space) {
                soundManager.play(SoundType::CLICK);
                batDauTroChoiMoi(cheDoChoi);
                return;
            }
            return;
        }
        if (hienMenuPopUp) return;

        if (keyConfig.isUpKey(key)) {
            viTriConTro.x = std::max(0, viTriConTro.x - 1);
            soundManager.play(SoundType::CLICK);
        } else if (keyConfig.isDownKey(key)) {
            viTriConTro.x = std::min(BanCo::getSOHANG() - 1, viTriConTro.x + 1);
            soundManager.play(SoundType::CLICK);
        } else if (keyConfig.isLeftKey(key)) {
            viTriConTro.y = std::max(0, viTriConTro.y - 1);
            soundManager.play(SoundType::CLICK);
        } else if (keyConfig.isRightKey(key)) {
            viTriConTro.y = std::min(BanCo::getSOCOT() - 1, viTriConTro.y + 1);
            soundManager.play(SoundType::CLICK);
        } else if (keyConfig.isSelectKey(key)) {
            if (cheDoChoi == CheDoChoi::VOI_MAY && banCo.getLuotChoi() == aiPlayerSide && aiDangSuyNghi) {
                return;
            }
            if (quanDangChon == nullptr) {
                chonQuan(viTriConTro.x, viTriConTro.y);
            } else {
                diChuyenQuan(viTriConTro.x, viTriConTro.y);
            }
        } else if (key == keyConfig.keyUndo || (ctrl && key == sf::Keyboard::Z)) {
            thucHienHoanTac();
        } else if (key == keyConfig.keyRedo || (ctrl && key == sf::Keyboard::Y)) {
            thucHienDiTiep();
        }
    }
}

void GameManager::capNhatDongHo(float dt) {
    if (thoiGianVanDau <= 0.0f) return;

    if (banCo.getLuotChoi() == Mau::DO) {
        thoiGianConDo -= dt;
        if (thoiGianConDo <= 0.0f) {
            thoiGianConDo = 0.0f;
            banCo.setKetThuc(true);
            banCo.setNguoiThang(Mau::DEN);
            lyDoKetThuc = (Loc::getLanguage() == Language::TIENG_VIET) ? "Quan Do het gio!" : "Red time out!";
            soundManager.play(SoundType::DEFEAT);
        }
    } else {
        thoiGianConDen -= dt;
        if (thoiGianConDen <= 0.0f) {
            thoiGianConDen = 0.0f;
            banCo.setKetThuc(true);
            banCo.setNguoiThang(Mau::DO);
            lyDoKetThuc = (Loc::getLanguage() == Language::TIENG_VIET) ? "Quan Den het gio!" : "Black time out!";
            soundManager.play(SoundType::VICTORY);
        }
    }
}

void GameManager::xuLyLuotAI(float dt) {
    if (banCo.getKetThuc()) return;
    
    Mau aiMau = (aiPlayerSide == Mau::DO) ? Mau::DEN : Mau::DO;
    if (banCo.getLuotChoi() != aiMau) return;

    if (!aiDangSuyNghi) {
        aiDangSuyNghi = true;
        thoiGianChoAI = aiThinkRealistic ? 0.45f : 0.05f;
        return;
    }

    thoiGianChoAI -= dt;
    if (thoiGianChoAI <= 0.0f) {
        aiDangSuyNghi = false;
        
        NuocDi aiMove = ChessAI::timNuocDi(banCo, aiDifficulty, aiMau);
        
        if (aiMove.hangBatDau != -1) {
            bool anQuan = banCo.coQuanTai(aiMove.hangKetThuc, aiMove.cotKetThuc);
            if (banCo.diChuyen(aiMove.hangBatDau, aiMove.cotBatDau, aiMove.hangKetThuc, aiMove.cotKetThuc)) {
                if (anQuan) soundManager.play(SoundType::CAPTURE);
                else soundManager.play(SoundType::MOVE);

                if (banCo.kiemTraChieuTuong()) {
                    if (checkAlarmSound) soundManager.play(SoundType::CHECK);
                    hienToast(Loc::get(LocKey::CHECK_ALERT));
                }

                if (banCo.getKetThuc()) {
                    if (banCo.getNguoiThang() == aiPlayerSide) soundManager.play(SoundType::VICTORY);
                    else soundManager.play(SoundType::DEFEAT);
                }
            }
        } else {
            banCo.setKetThuc(true);
            banCo.setNguoiThang(aiPlayerSide);
            lyDoKetThuc = (Loc::getLanguage() == Language::TIENG_VIET) ? "May AI khong con nuoc di hop le." : "AI has no legal moves left.";
            soundManager.play(SoundType::VICTORY);
        }
    }
}

void GameManager::chonQuan(int hang, int cot) {
    auto q = banCo.timQuan(hang, cot);
    if (q && q->getMau() == banCo.getLuotChoi()) {
        quanDangChon = q;
        soundManager.play(SoundType::CLICK);
        tinhCacNuocDiHopLe();
    }
}

void GameManager::diChuyenQuan(int hang, int cot) {
    if (!quanDangChon) return;

    if (hang == quanDangChon->getHang() && cot == quanDangChon->getCot()) {
        quanDangChon = nullptr;
        cacNuocDiHopLe.clear();
        return;
    }

    auto quanDich = banCo.timQuan(hang, cot);
    if (quanDich && quanDich->getMau() == banCo.getLuotChoi()) {
        chonQuan(hang, cot);
        return;
    }

    bool nuocDiDung = false;
    for (const auto& nd : cacNuocDiHopLe) {
        if (nd.x == hang && nd.y == cot) {
            nuocDiDung = true;
            break;
        }
    }

    if (nuocDiDung) {
        bool anQuan = (quanDich != nullptr);
        int hbd = quanDangChon->getHang();
        int cbd = quanDangChon->getCot();

        if (banCo.diChuyen(hbd, cbd, hang, cot)) {
            quanDangChon = nullptr;
            cacNuocDiHopLe.clear();

            if (anQuan) soundManager.play(SoundType::CAPTURE);
            else soundManager.play(SoundType::MOVE);

            if (banCo.kiemTraChieuTuong()) {
                if (checkAlarmSound) soundManager.play(SoundType::CHECK);
                hienToast(Loc::get(LocKey::CHECK_ALERT));
            }

            if (banCo.getKetThuc()) {
                soundManager.play(SoundType::VICTORY);
            }
        }
    } else {
        hienToast((Loc::getLanguage() == Language::TIENG_VIET) ? "Nuoc di khong hop le!" : "Illegal move!");
    }
}

void GameManager::tinhCacNuocDiHopLe() {
    cacNuocDiHopLe.clear();
    if (!quanDangChon) return;

    int h = quanDangChon->getHang();
    int c = quanDangChon->getCot();

    for (int r = 0; r < BanCo::getSOHANG(); ++r) {
        for (int col = 0; col < BanCo::getSOCOT(); ++col) {
            if (banCo.kiemTraNuocDiHopLe(h, c, r, col)) {
                cacNuocDiHopLe.emplace_back(r, col);
            }
        }
    }
}

void GameManager::thucHienHoanTac() {
    if (!banCo.coTheHoanTac()) {
        hienToast((Loc::getLanguage() == Language::TIENG_VIET) ? "Khong the hoan tac them!" : "Cannot undo further!");
        return;
    }

    if (cheDoChoi == CheDoChoi::VOI_MAY) {
        banCo.hoanTac();
        if (banCo.coTheHoanTac()) {
            banCo.hoanTac();
        }
    } else {
        banCo.hoanTac();
    }

    quanDangChon = nullptr;
    cacNuocDiHopLe.clear();
    soundManager.play(SoundType::CLICK);
    hienToast(Loc::get(LocKey::UNDO));
}

void GameManager::thucHienDiTiep() {
    if (!banCo.coTheDiTiep()) {
        hienToast((Loc::getLanguage() == Language::TIENG_VIET) ? "Khong the di tiep nua!" : "Cannot redo further!");
        return;
    }

    if (cheDoChoi == CheDoChoi::VOI_MAY) {
        banCo.diTiep();
        if (banCo.coTheDiTiep()) {
            banCo.diTiep();
        }
    } else {
        banCo.diTiep();
    }

    quanDangChon = nullptr;
    cacNuocDiHopLe.clear();
    soundManager.play(SoundType::CLICK);
    hienToast(Loc::get(LocKey::REDO));
}

void GameManager::thucHienDauHang() {
    banCo.dauHang(banCo.getLuotChoi());
    lyDoKetThuc = (banCo.getLuotChoi() == Mau::DO) 
        ? ((Loc::getLanguage() == Language::TIENG_VIET) ? "Quan Do da xin dau hang." : "Red resigned.")
        : ((Loc::getLanguage() == Language::TIENG_VIET) ? "Quan Den da xin dau hang." : "Black resigned.");
    soundManager.play(SoundType::DEFEAT);
}

void GameManager::thucHienXinHoa() {
    banCo.xinHoa();
    lyDoKetThuc = (Loc::getLanguage() == Language::TIENG_VIET) ? "Hai ben dong y hoa co." : "Both players agreed to a draw.";
    soundManager.play(SoundType::CLICK);
}

void GameManager::thucHienLuuGame() {
    luuVaoSlot(0);
}

void GameManager::thucHienLoadGame() {
    capNhatThongTinSlots();
    trangThai = TrangThai::LOAD_MENU;
    soundManager.play(SoundType::CLICK);
}

void GameManager::quayLaiMenu() {
    hienMenuPopUp = false;
    trangThai = TrangThai::MENU_CHINH;
    menuChinh.triggerOpenAnimation();
    soundManager.play(SoundType::CLICK);
}

void GameManager::hienToast(const std::string& msg) {
    toastMessage = msg;
    toastTimer = 2.5f;
}

sf::Vector2i GameManager::chuyenDoiToaDoManHinhThanhBanCo(const sf::Vector2i& viTriChuot) {
    float startX = offsetX;
    float startY = offsetY;
    float endX = offsetX + (BanCo::getSOCOT() - 1) * kichThuocO;
    float endY = offsetY + (BanCo::getSOHANG() - 1) * kichThuocO;

    float r = kichThuocO * 0.45f;
    if (viTriChuot.x < startX - r || viTriChuot.x > endX + r ||
        viTriChuot.y < startY - r || viTriChuot.y > endY + r) {
        return sf::Vector2i(-1, -1);
    }

    int cot = static_cast<int>(std::round((viTriChuot.x - startX) / kichThuocO));
    int hang = static_cast<int>(std::round((viTriChuot.y - startY) / kichThuocO));

    if (hang >= 0 && hang < BanCo::getSOHANG() && cot >= 0 && cot < BanCo::getSOCOT()) {
        return sf::Vector2i(hang, cot);
    }
    return sf::Vector2i(-1, -1);
}

sf::Vector2f GameManager::chuyenDoiToaDoBanCoThanhManHinh(int hang, int cot) {
    return sf::Vector2f(offsetX + cot * kichThuocO, offsetY + hang * kichThuocO);
}

void GameManager::taiTextures() {
    daTaiTextures = false;

    // Load background pixel art texture
    tryLoadTextureCandidates(textureBackground, {
        "assets/xiangqi_background_pavilion.png",
        "../assets/xiangqi_background_pavilion.png",
        "resources/textures/background.png",
        "resources/textures/background_game.png"
    });
    textureBackground.setSmooth(true);

    // Load background
    tryLoadTextureCandidates(texturePlayBg, {
        "assets/xiangqi_background_pavilion.png",
        "../assets/xiangqi_background_pavilion.png",
        "resources/textures/background play.jpg"
    });
    texturePlayBg.setSmooth(true);

    // Load board texture (Strict Alpha Masking)
    {
        sf::Image imgBanCo;
        if (tryLoadImageCandidates(imgBanCo, {
                "assets/bancofinal.png",
                "../assets/bancofinal.png",
                "resources/textures/ban_co.png"
            })) {
            
            // Explicit color mask required by user
            imgBanCo.createMaskFromColor(sf::Color::White);
            sf::Vector2u sz = imgBanCo.getSize();
            for (unsigned y = 0; y < sz.y; ++y) {
                for (unsigned x = 0; x < sz.x; ++x) {
                    sf::Color px = imgBanCo.getPixel(x, y);
                    if (px.r >= 238 && px.g >= 238 && px.b >= 238) {
                        imgBanCo.setPixel(x, y, sf::Color::Transparent);
                    }
                }
            }
            textureBanCo.loadFromImage(imgBanCo);
            textureBanCo.setSmooth(true);

            // Board parameters (centered on 1200x800)
            kichThuocO = 68.0f;
            offsetX    = 328.0f;
            offsetY    = 110.0f;
        }
    }

    auto loadPieceSafe = [](sf::Texture& tex, const std::string& filename) {
        const std::vector<std::string> prefixes = {
            "resources/textures/pieces/",
            "../resources/textures/pieces/",
            "../../resources/textures/pieces/"
        };
        for (const auto& p : prefixes) {
            std::string fullPath = p + filename;
            if (!fileExistsUtf8(fullPath)) {
                continue;
            }
            sf::Image img;
            if (img.loadFromFile(fullPath)) {
                bool hasAlpha = false;
                sf::Vector2u sz = img.getSize();
                for (unsigned int y = 0; y < sz.y && !hasAlpha; ++y) {
                    for (unsigned int x = 0; x < sz.x; ++x) {
                        if (img.getPixel(x, y).a < 250) {
                            hasAlpha = true;
                            break;
                        }
                    }
                }
                if (!hasAlpha) {
                    img.createMaskFromColor(sf::Color::White);
                }
                tex.loadFromImage(img);
                tex.setSmooth(true);
                return true;
            }
        }
        return false;
    };

    // Load red pieces
    loadPieceSafe(textureQuanDo["Tuong"], "r_tuong.png");
    loadPieceSafe(textureQuanDo["Si"], "r_si.png");
    loadPieceSafe(textureQuanDo["Voi"], "r_voi.png");
    loadPieceSafe(textureQuanDo["Ma"], "r_ma.png");
    loadPieceSafe(textureQuanDo["Xe"], "r_xe.png");
    loadPieceSafe(textureQuanDo["Phao"], "r_phao.png");
    loadPieceSafe(textureQuanDo["Tot"], "r_tot.png");

    // Load black pieces
    loadPieceSafe(textureQuanDen["Tuong"], "b_tuong.png");
    loadPieceSafe(textureQuanDen["Si"], "b_si.png");
    loadPieceSafe(textureQuanDen["Voi"], "b_voi.png");
    loadPieceSafe(textureQuanDen["Ma"], "b_ma.png");
    loadPieceSafe(textureQuanDen["Xe"], "b_xe.png");
    loadPieceSafe(textureQuanDen["Phao"], "b_phao.png");
    loadPieceSafe(textureQuanDen["Tot"], "b_tot.png");

    for (auto& p : textureQuanDo) p.second.setSmooth(true);
    for (auto& p : textureQuanDen) p.second.setSmooth(true);

    if (textureQuanDo["Tuong"].getSize().x > 0 && textureQuanDen["Tuong"].getSize().x > 0) {
        daTaiTextures = true;
    }
    taiAvatarTextures();
}

void GameManager::taiAvatarTextures() {
    if (daTaiAvatarTextures) return;
    const std::string paths[5] = {
        "assets/avatar/images/emoji_cho_ava_03.png",
        "assets/avatar/images/emoji_cho_ava_07.png",
        "assets/avatar/images/emoji_cho_ava_10.png",
        "assets/avatar/images/emoji_cho_ava_18.png",
        "assets/avatar/images/emoji_cho_ava_24.png"
    };
    for (int i = 0; i < 5; ++i) {
        if (fileExistsUtf8(paths[i])) {
            avatarTextures[i].loadFromFile(paths[i]);
        } else if (fileExistsUtf8("../" + paths[i])) {
            avatarTextures[i].loadFromFile("../" + paths[i]);
        }
        avatarTextures[i].setSmooth(false);
    }
    daTaiAvatarTextures = true;
}


const sf::Texture* GameManager::layTextureQuan(const std::string& tenQuan, Mau mau) const {
    if (!daTaiTextures) return nullptr;
    if (mau == Mau::DO) {
        auto it = textureQuanDo.find(tenQuan);
        if (it != textureQuanDo.end() && it->second.getSize().x > 0) {
            return &(it->second);
        }
    } else if (mau == Mau::DEN) {
        auto it = textureQuanDen.find(tenQuan);
        if (it != textureQuanDen.end() && it->second.getSize().x > 0) {
            return &(it->second);
        }
    }
    return nullptr;
}

std::string GameManager::layKyHieuQuanTheoStyle(const std::string& tenGoc, Mau mau) const {
    if (pieceStyle == PieceStyle::REALISTIC_WOOD || pieceStyle == PieceStyle::VIETNAMESE) {
        return tenGoc; // "Xe", "Ma", "Voi", "Si", "Tuong", "Phao", "Tot"
    }

    if (pieceStyle == PieceStyle::SHORT_CODE) {
        if (tenGoc == "Xe") return "X";
        if (tenGoc == "Ma") return "M";
        if (tenGoc == "Voi") return "V";
        if (tenGoc == "Si") return "S";
        if (tenGoc == "Tuong") return "TG";
        if (tenGoc == "Phao") return "P";
        if (tenGoc == "Tot") return "T";
    }

    if (pieceStyle == PieceStyle::INTERNATIONAL) {
        if (tenGoc == "Xe") return "R";      // Rook
        if (tenGoc == "Ma") return "H";      // Horse / Knight
        if (tenGoc == "Voi") return "E";     // Elephant
        if (tenGoc == "Si") return "A";      // Advisor
        if (tenGoc == "Tuong") return "K";   // King
        if (tenGoc == "Phao") return "C";    // Cannon
        if (tenGoc == "Tot") return "P";     // Pawn
    }

    return tenGoc;
}

std::string GameManager::layTenPieceAura(PieceAura aura) const {
    bool vi = (Loc::getLanguage() == Language::TIENG_VIET);
    switch (aura) {
        case PieceAura::CLASSIC_WOOD:
            return vi ? "Vanh Go Co Dien" : "Classic Wood Trim";
        case PieceAura::IMPERIAL_GOLD:
            return vi ? "Hoang Kim (Vang)" : "Imperial Gold";
        case PieceAura::DRAGON_FLAME:
            return vi ? "Xich Long (Lua Do)" : "Dragon Flame";
        case PieceAura::THUNDER_CYAN:
            return vi ? "Lam Loi (Ngoc Bich)" : "Thunder Jade";
        case PieceAura::SHADOW_VOID:
            return vi ? "Hac Dieu (Ma Thach)" : "Shadow Void";
    }
    return "";
}

void GameManager::veAuraQuan(sf::Vector2f pos, float r, PieceAura aura, bool isKing) {
    // Task 4: Smooth non-pixel-snapped circular aura glows and floating vector crown rings
    // are intentionally disabled so wooden pieces render crisply with their 1px dark rim.
    (void)pos;
    (void)r;
    (void)aura;
    (void)isKing;
}

// Rendering

void GameManager::ve() {
    window.clear(sf::Color(14, 18, 26));

    // Task 1: Draw full-window cover background BEFORE applying centered 1200x800 viewport,
    // covering 100% of any aspect ratio (16:9 fullscreen, ultrawide, maximized window) with zero black bars.
    sf::Vector2u winSz = window.getSize();
    auto& mAssets = getMenuAssets();
    if (winSz.x > 0 && winSz.y > 0) {
        float winW = static_cast<float>(winSz.x);
        float winH = static_cast<float>(winSz.y);
        sf::View fullWinView(sf::FloatRect(0.0f, 0.0f, winW, winH));
        window.setView(fullWinView);

        if (trangThai == TrangThai::DANG_CHOI) {
            const sf::Texture* pBg = (texturePlayBg.getSize().x > 0) ? &texturePlayBg
                                   : ((mAssets.playBg.getSize().x > 0) ? &mAssets.playBg
                                   : ((textureBackground.getSize().x > 0) ? &textureBackground : nullptr));
            if (pBg && pBg->getSize().x > 0 && pBg->getSize().y > 0) {
                sf::Sprite bgSprite(*pBg);
                float tw = static_cast<float>(pBg->getSize().x);
                float th = static_cast<float>(pBg->getSize().y);
                float coverScale = std::max(winW / tw, winH / th);
                bgSprite.setOrigin(tw / 2.0f, th / 2.0f);
                bgSprite.setPosition(winW / 2.0f, winH / 2.0f);
                bgSprite.setScale(coverScale, coverScale);
                window.draw(bgSprite);
            }
        } else {
            if (mAssets.cleanBg.getSize().x > 0) {
                veNenMenuGiaoDien(window);
            } else if (textureBackground.getSize().x > 0 && textureBackground.getSize().y > 0) {
                sf::Sprite bgSprite(textureBackground);
                float tw = static_cast<float>(textureBackground.getSize().x);
                float th = static_cast<float>(textureBackground.getSize().y);
                float coverScale = std::max(winW / tw, winH / th);
                bgSprite.setOrigin(tw / 2.0f, th / 2.0f);
                bgSprite.setPosition(winW / 2.0f, winH / 2.0f);
                bgSprite.setScale(coverScale, coverScale);
                window.draw(bgSprite);
            }
        }
    }

    capNhatViewManHinh();
    atmosphere.draw(window);

    if (trangThai == TrangThai::MENU_CHINH) {
        veMenuChinh();
    }
    else if (trangThai == TrangThai::PLAY_SUBMENU) {
        vePlaySubmenu();
    } else if (trangThai == TrangThai::PVP_SETUP || trangThai == TrangThai::PVAI_SETUP) {
        vePlaySetup();
    }
    else if (trangThai == TrangThai::LOAD_MENU) {
        veLoadMenu();
    } else if (trangThai == TrangThai::SETTINGS_MENU) {
        veSettingsMenu();
    } else if (trangThai == TrangThai::INTRODUCTION_MENU) {
        veIntroMenu();
    } else if (trangThai == TrangThai::DANG_CHOI) {
        // 1. Board, highlights, pieces, and keyboard cursor
        veBanCoTruyenThong();
        veHighlights();
        veCacQuanCo();
        veConTroBanPhim();

        // 2. Side player/history panels & turn/incense ribbon
        veThanhBenTrai();
        veThanhBenPhai();
        veThanhThongBao();

        // 3. Modals
        if (banCo.getKetThuc()) {
            vePopupKetThuc();
        } else if (hienMenuPopUp) {
            veMenuPopUpInGame();
        }
    }

    veToast();

    // Draw cursor
    if (mAssets.cursorNormal.getSize().x > 0) {
        sf::Vector2i mPos = sf::Mouse::getPosition(window);
        if (mPos.x >= 0 && mPos.x < static_cast<int>(winSz.x) &&
            mPos.y >= 0 && mPos.y < static_cast<int>(winSz.y)) {
            bool isPressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);
            const sf::Texture& curTex = (isPressed && mAssets.cursorPressed.getSize().x > 0)
                                        ? mAssets.cursorPressed
                                        : mAssets.cursorNormal;
            sf::Sprite curSprite(curTex);
            curSprite.setOrigin(1.0f, 0.0f);
            curSprite.setScale(2.2f, 2.2f);

            sf::View prevView = window.getView();
            window.setView(sf::View(sf::FloatRect(
                0.0f, 0.0f,
                static_cast<float>(winSz.x),
                static_cast<float>(winSz.y))));
            curSprite.setPosition(static_cast<float>(mPos.x), static_cast<float>(mPos.y));
            window.draw(curSprite);
            window.setView(prevView);
        }
    }

    window.display();
}
void GameManager::veBanCoTruyenThong() {
    float gridW = (BanCo::getSOCOT() - 1) * kichThuocO;
    float gridH = (BanCo::getSOHANG() - 1) * kichThuocO;
    float margin = 34.0f;
    float bx = offsetX - margin;
    float by = offsetY - margin;
    float boardW = gridW + margin * 2.0f;
    float boardH = gridH + margin * 2.0f;

    // Wrap board cleanly with dark ebony wood bezel + brass cloud corner brackets
    FantasyUI::get().drawPanel(window, sf::FloatRect(bx - 12.0f, by - 12.0f, boardW + 24.0f, boardH + 24.0f), OrientalPanelStyle::EBONY_WOOD);

    // Crafted antique board palette (removes bright mint & mismatched shiny textures)
    sf::Color boardBgCol(220, 185, 135);
    sf::Color borderCol(70, 40, 20);
    sf::Color lineCol(70, 40, 20);
    sf::Color riverCol(70, 40, 20, 220);
    sf::Color riverEmbossCol(248, 232, 198, 95);
    sf::Color grainCol(95, 58, 28, 16);

    if (boardTheme == BoardTheme::IMPERIAL_JADE) {
        boardBgCol = sf::Color(168, 194, 162);
        borderCol = sf::Color(35, 58, 42);
        lineCol = sf::Color(35, 58, 42);
        riverCol = sf::Color(30, 52, 36, 225);
        riverEmbossCol = sf::Color(225, 240, 220, 90);
        grainCol = sf::Color(35, 60, 42, 14);
    } else if (boardTheme == BoardTheme::MIDNIGHT_INK) {
        boardBgCol = sf::Color(34, 40, 52);
        borderCol = sf::Color(150, 118, 62);
        lineCol = sf::Color(125, 150, 182);
        riverCol = sf::Color(208, 178, 116, 225);
        riverEmbossCol = sf::Color(10, 12, 18, 160);
        grainCol = sf::Color(255, 255, 255, 8);
    } else if (boardTheme == BoardTheme::WARM_BAMBOO) {
        boardBgCol = sf::Color(210, 175, 125);
        borderCol = sf::Color(68, 38, 18);
        lineCol = sf::Color(68, 38, 18);
        riverCol = sf::Color(68, 38, 18, 220);
        riverEmbossCol = sf::Color(242, 224, 188, 90);
        grainCol = sf::Color(85, 50, 22, 18);
    }

    // Main antique board surface
    sf::RectangleShape boardBg(sf::Vector2f(boardW, boardH));
    boardBg.setPosition(bx, by);
    boardBg.setFillColor(boardBgCol);
    boardBg.setOutlineThickness(2.0f);
    boardBg.setOutlineColor(borderCol);
    window.draw(boardBg);

    // Subtle horizontal wood / bamboo grain stripes
    for (float gy = by + 8.0f; gy < by + boardH - 6.0f; gy += 14.0f) {
        sf::RectangleShape grain(sf::Vector2f(boardW - 8.0f, 2.0f));
        grain.setPosition(bx + 4.0f, std::round(gy));
        grain.setFillColor(grainCol);
        window.draw(grain);
    }

    // Thick ink line helper (2.0px for crisp readability)
    auto drawThickLine = [this](sf::Vector2f a, sf::Vector2f b, float thickness, sf::Color col) {
        sf::Vector2f diff = b - a;
        float len = std::sqrt(diff.x * diff.x + diff.y * diff.y);
        if (len <= 0.001f) {
            return;
        }
        sf::RectangleShape seg(sf::Vector2f(len, thickness));
        seg.setOrigin(0.0f, thickness / 2.0f);
        seg.setPosition(a);
        seg.setRotation(std::atan2(diff.y, diff.x) * 180.0f / 3.14159265f);
        seg.setFillColor(col);
        window.draw(seg);
    };

    // Double outer framing line around the grid
    sf::RectangleShape outerFrame(sf::Vector2f(gridW + 10.0f, gridH + 10.0f));
    outerFrame.setPosition(offsetX - 5.0f, offsetY - 5.0f);
    outerFrame.setFillColor(sf::Color::Transparent);
    outerFrame.setOutlineThickness(2.0f);
    outerFrame.setOutlineColor(lineCol);
    window.draw(outerFrame);

    // Horizontal grid rows (10)
    for (int r = 0; r < BanCo::getSOHANG(); ++r) {
        sf::Vector2f p1 = chuyenDoiToaDoBanCoThanhManHinh(r, 0);
        sf::Vector2f p2 = chuyenDoiToaDoBanCoThanhManHinh(r, BanCo::getSOCOT() - 1);
        drawThickLine(p1, p2, 2.0f, lineCol);
    }

    // Vertical grid columns (outer solid, inner split at Chu-Han river)
    for (int c = 0; c < BanCo::getSOCOT(); ++c) {
        if (c == 0 || c == BanCo::getSOCOT() - 1) {
            drawThickLine(
                chuyenDoiToaDoBanCoThanhManHinh(0, c),
                chuyenDoiToaDoBanCoThanhManHinh(BanCo::getSOHANG() - 1, c),
                2.0f, lineCol
            );
        } else {
            drawThickLine(chuyenDoiToaDoBanCoThanhManHinh(0, c), chuyenDoiToaDoBanCoThanhManHinh(4, c), 2.0f, lineCol);
            drawThickLine(chuyenDoiToaDoBanCoThanhManHinh(5, c), chuyenDoiToaDoBanCoThanhManHinh(9, c), 2.0f, lineCol);
        }
    }

    // Palace diagonals (top & bottom 3x3 fortresses)
    drawThickLine(chuyenDoiToaDoBanCoThanhManHinh(0, 3), chuyenDoiToaDoBanCoThanhManHinh(2, 5), 2.0f, lineCol);
    drawThickLine(chuyenDoiToaDoBanCoThanhManHinh(0, 5), chuyenDoiToaDoBanCoThanhManHinh(2, 3), 2.0f, lineCol);
    drawThickLine(chuyenDoiToaDoBanCoThanhManHinh(7, 3), chuyenDoiToaDoBanCoThanhManHinh(9, 5), 2.0f, lineCol);
    drawThickLine(chuyenDoiToaDoBanCoThanhManHinh(7, 5), chuyenDoiToaDoBanCoThanhManHinh(9, 3), 2.0f, lineCol);

    // Traditional Cannon & Soldier intersection L-markers
    auto drawMarkerTicks = [&](int row, int col, bool leftEdge, bool rightEdge) {
        sf::Vector2f p = chuyenDoiToaDoBanCoThanhManHinh(row, col);
        float gap = 5.0f;
        float arm = 7.0f;
        auto tickCorner = [&](float sx, float sy) {
            sf::Vector2f corner(p.x + sx * gap, p.y + sy * gap);
            drawThickLine(corner, sf::Vector2f(corner.x + sx * arm, corner.y), 1.6f, lineCol);
            drawThickLine(corner, sf::Vector2f(corner.x, corner.y + sy * arm), 1.6f, lineCol);
        };
        if (!leftEdge) {
            tickCorner(-1.0f, -1.0f);
            tickCorner(-1.0f, 1.0f);
        }
        if (!rightEdge) {
            tickCorner(1.0f, -1.0f);
            tickCorner(1.0f, 1.0f);
        }
    };
    drawMarkerTicks(2, 1, false, false);
    drawMarkerTicks(2, 7, false, false);
    drawMarkerTicks(7, 1, false, false);
    drawMarkerTicks(7, 7, false, false);
    for (int col : {0, 2, 4, 6, 8}) {
        drawMarkerTicks(3, col, col == 0, col == 8);
        drawMarkerTicks(6, col, col == 0, col == 8);
    }

    // Engraved Traditional River Calligraphy: "楚 河" and "漢 界" using NomNaTong Hanzi font
    const sf::Font& riverFont = (!fontHan.getInfo().family.empty()) ? fontHan : fontBold;
    auto drawRiverPhrase = [&](const wchar_t* glyphText, float centerX) {
        sf::Text emboss;
        emboss.setFont(riverFont);
        emboss.setCharacterSize(30);
        emboss.setString(sf::String(glyphText));
        sf::FloatRect rb = emboss.getLocalBounds();
        emboss.setOrigin(std::round(rb.left + rb.width / 2.0f), std::round(rb.top + rb.height / 2.0f));
        emboss.setPosition(std::round(centerX + 1.0f), std::round(offsetY + 4.5f * kichThuocO + 1.0f));
        emboss.setFillColor(riverEmbossCol);
        window.draw(emboss);

        sf::Text mainTxt = emboss;
        mainTxt.setPosition(std::round(centerX), std::round(offsetY + 4.5f * kichThuocO));
        mainTxt.setFillColor(riverCol);
        window.draw(mainTxt);
    };
    drawRiverPhrase(L"\u695A  \u6CB3", offsetX + 2.0f * kichThuocO);
    drawRiverPhrase(L"\u6F22  \u754C", offsetX + 6.0f * kichThuocO);

    // Subtle engraved board coordinates around inner rim
    if (hienToaDo) {
        sf::Color coordCol = lineCol;
        coordCol.a = 175;

        for (int c = 0; c < BanCo::getSOCOT(); ++c) {
            sf::Text tTop;
            tTop.setFont(font);
            tTop.setCharacterSize(14);
            tTop.setStyle(sf::Text::Bold);
            tTop.setString(std::to_string(c + 1));
            tTop.setFillColor(coordCol);
            sf::FloatRect tb = tTop.getLocalBounds();
            tTop.setOrigin(std::round(tb.left + tb.width / 2.0f), 0.0f);
            tTop.setPosition(std::round(offsetX + c * kichThuocO), std::round(by + 5.0f));
            window.draw(tTop);

            sf::Text tBot;
            tBot.setFont(font);
            tBot.setCharacterSize(14);
            tBot.setStyle(sf::Text::Bold);
            tBot.setString(std::to_string(BanCo::getSOCOT() - c));
            tBot.setFillColor(coordCol);
            sf::FloatRect bb = tBot.getLocalBounds();
            tBot.setOrigin(std::round(bb.left + bb.width / 2.0f), 0.0f);
            tBot.setPosition(std::round(offsetX + c * kichThuocO), std::round(by + boardH - 22.0f));
            window.draw(tBot);
        }

        const char rowLetters[] = "ABCDEFGHIJ";
        for (int r = 0; r < BanCo::getSOHANG(); ++r) {
            sf::Text tLeft;
            tLeft.setFont(font);
            tLeft.setCharacterSize(14);
            tLeft.setStyle(sf::Text::Bold);
            tLeft.setString(std::string(1, rowLetters[r]));
            tLeft.setFillColor(coordCol);
            tLeft.setPosition(std::round(bx + 10.0f), std::round(offsetY + r * kichThuocO - 9.0f));
            window.draw(tLeft);

            sf::Text tRight;
            tRight.setFont(font);
            tRight.setCharacterSize(14);
            tRight.setStyle(sf::Text::Bold);
            tRight.setString(std::string(1, rowLetters[r]));
            tRight.setFillColor(coordCol);
            tRight.setPosition(std::round(bx + boardW - 20.0f), std::round(offsetY + r * kichThuocO - 9.0f));
            window.draw(tRight);
        }
    }
}

void GameManager::veHighlights() {
    // 1. Last Move Highlight (if enabled)
    if (hienNuocDiCuoi) {
        NuocDi lastMove = banCo.getNuocDiCuoi();
        if (lastMove.hangBatDau != -1) {
            sf::Vector2f p1 = chuyenDoiToaDoBanCoThanhManHinh(lastMove.hangBatDau, lastMove.cotBatDau);
            sf::Vector2f p2 = chuyenDoiToaDoBanCoThanhManHinh(lastMove.hangKetThuc, lastMove.cotKetThuc);
            
            sf::CircleShape c1(28.0f);
            c1.setOrigin(28.0f, 28.0f);
            c1.setPosition(p1);
            c1.setFillColor(sf::Color(80, 160, 240, 90));
            window.draw(c1);

            sf::CircleShape c2(28.0f);
            c2.setOrigin(28.0f, 28.0f);
            c2.setPosition(p2);
            c2.setFillColor(sf::Color(80, 160, 240, 120));
            window.draw(c2);
        }
    }

    // Move hints
    if (quanDangChon) {
        sf::Vector2f pos = chuyenDoiToaDoBanCoThanhManHinh(quanDangChon->getHang(), quanDangChon->getCot());
        sf::CircleShape selCircle(30.0f);
        selCircle.setOrigin(30.0f, 30.0f);
        selCircle.setPosition(pos);
        selCircle.setFillColor(sf::Color(255, 215, 0, 140));
        selCircle.setOutlineThickness(2.5f);
        selCircle.setOutlineColor(sf::Color(255, 230, 80));
        window.draw(selCircle);

        if (hienGoiY) {
            for (const auto& nd : cacNuocDiHopLe) {
                sf::Vector2f destPos = chuyenDoiToaDoBanCoThanhManHinh(nd.x, nd.y);
                bool coQuan = banCo.coQuanTai(nd.x, nd.y);
                if (coQuan) {
                    sf::CircleShape capCircle(28.0f);
                    capCircle.setOrigin(28.0f, 28.0f);
                    capCircle.setPosition(destPos);
                    capCircle.setFillColor(sf::Color(230, 40, 40, 120));
                    capCircle.setOutlineThickness(2.5f);
                    capCircle.setOutlineColor(sf::Color(250, 80, 80));
                    window.draw(capCircle);
                } else {
                    sf::CircleShape dot(8.0f);
                    dot.setOrigin(8.0f, 8.0f);
                    dot.setPosition(destPos);
                    dot.setFillColor(sf::Color(30, 180, 60, 210));
                    dot.setOutlineThickness(1.5f);
                    dot.setOutlineColor(sf::Color::White);
                    window.draw(dot);
                }
            }
        }
    }
}

void GameManager::veCacQuanCo() {
    float r = 29.0f;
    float targetDiameter = r * 2.0f;

    for (const auto& q : banCo.getCacQuan()) {
        if (!q || q->getDaBiAn()) continue;
        sf::Vector2f rawPos = chuyenDoiToaDoBanCoThanhManHinh(q->getHang(), q->getCot());
        sf::Vector2f pos(std::round(rawPos.x), std::round(rawPos.y));

        const sf::Texture* tex = nullptr;
        if (pieceStyle == PieceStyle::REALISTIC_WOOD) {
            tex = layTextureQuan(q->layTen(), q->getMau());
        }

        PieceAura aura = (q->getMau() == Mau::DO) ? auraDo : auraDen;
        bool isKing = q->laTuong();
        veAuraQuan(pos, r, aura, isKing);

        if (tex != nullptr) {
            // Crisp pixel-snapped drop shadow
            sf::CircleShape shadow(r);
            shadow.setOrigin(r, r);
            shadow.setPosition(pos.x + 2.0f, pos.y + 3.0f);
            shadow.setFillColor(sf::Color(10, 6, 4, 120));
            window.draw(shadow);

            // Crisp 1px dark ink contour ring so textured pieces never wash into wood tones
            sf::CircleShape darkRim(r + 1.0f);
            darkRim.setOrigin(r + 1.0f, r + 1.0f);
            darkRim.setPosition(pos);
            darkRim.setFillColor(sf::Color(26, 14, 8));
            window.draw(darkRim);

            sf::Sprite sprite(*tex);
            sf::Vector2u texSize = tex->getSize();
            sprite.setOrigin(texSize.x / 2.0f, texSize.y / 2.0f);
            float scale = targetDiameter / static_cast<float>(texSize.x);
            sprite.setScale(scale, scale);
            sprite.setPosition(pos);
            window.draw(sprite);
        } else {
            bool isRed = (q->getMau() == Mau::DO);

            sf::CircleShape shadow(r + 2.0f);
            shadow.setOrigin(r + 2.0f, r + 2.0f);
            shadow.setPosition(pos.x + 2.0f, pos.y + 3.0f);
            shadow.setFillColor(sf::Color(10, 6, 4, 115));
            window.draw(shadow);

            // Crisp 1px dark pixel contour + outer sandalwood / rosewood bezel
            sf::CircleShape outer(r + 2.5f);
            outer.setOrigin(r + 2.5f, r + 2.5f);
            outer.setPosition(pos);
            outer.setFillColor(isRed ? sf::Color(135, 26, 18) : sf::Color(24, 30, 40));
            outer.setOutlineThickness(1.2f);
            outer.setOutlineColor(sf::Color(26, 14, 8));
            window.draw(outer);

            // Piece body (Warm ivory vs pale celadon jade)
            sf::CircleShape piece(r);
            piece.setOrigin(r, r);
            piece.setPosition(pos);
            piece.setFillColor(isRed ? sf::Color(248, 236, 216) : sf::Color(232, 240, 236));
            piece.setOutlineThickness(1.5f);
            piece.setOutlineColor(isRed ? sf::Color(185, 45, 30) : sf::Color(45, 60, 75));
            window.draw(piece);

            // Inner carved groove
            sf::CircleShape inner(r - 3.5f);
            inner.setOrigin(r - 3.5f, r - 3.5f);
            inner.setPosition(pos);
            inner.setFillColor(sf::Color::Transparent);
            inner.setOutlineThickness(1.0f);
            inner.setOutlineColor(isRed ? sf::Color(200, 50, 40, 165) : sf::Color(60, 80, 100, 165));
            window.draw(inner);

            std::string label = layKyHieuQuanTheoStyle(q->layTen(), q->getMau());

            sf::Text t;
            t.setFont(fontBold);
            t.setString(toUtf8(label));
            t.setCharacterSize((label.size() > 2) ? 17 : 21);
            t.setStyle(sf::Text::Bold);
            t.setFillColor(isRed ? sf::Color(180, 20, 15) : sf::Color(20, 30, 42));

            sf::FloatRect tb = t.getLocalBounds();
            t.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));
            t.setPosition(pos.x, pos.y - 1.0f);
            window.draw(t);
        }
    }
}

void GameManager::veConTroBanPhim() {
    sf::Vector2f pos = chuyenDoiToaDoBanCoThanhManHinh(viTriConTro.x, viTriConTro.y);
    float size = 32.0f;
    float len = 10.0f;
    sf::Color yellow(255, 220, 0);

    auto drawBracket = [this, pos, yellow, len](float dx, float dy) {
        sf::Vertex h[] = {
            sf::Vertex(sf::Vector2f(pos.x + dx, pos.y + dy), yellow),
            sf::Vertex(sf::Vector2f(pos.x + dx + ((dx < 0) ? len : -len), pos.y + dy), yellow)
        };
        sf::Vertex v[] = {
            sf::Vertex(sf::Vector2f(pos.x + dx, pos.y + dy), yellow),
            sf::Vertex(sf::Vector2f(pos.x + dx, pos.y + dy + ((dy < 0) ? len : -len)), yellow)
        };
        window.draw(h, 2, sf::Lines);
        window.draw(v, 2, sf::Lines);
    };

    drawBracket(-size, -size);
    drawBracket(size, -size);
    drawBracket(-size, size);
    drawBracket(size, size);
}

void GameManager::veThanhThongBao() {
    bool checkAlert = banCo.kiemTraChieuTuong();
    bool tiengViet = (Loc::getLanguage() == Language::TIENG_VIET);
    bool isRed = (banCo.getLuotChoi() == Mau::DO);

    if (checkAlert) {
        float bw = 320.0f;
        float bh = 46.0f;
        float bx = (1200.0f - bw) / 2.0f;
        float by = 20.0f;

        sf::RectangleShape banner(sf::Vector2f(bw, bh));
        banner.setPosition(bx, by);
        banner.setFillColor(sf::Color(140, 20, 20, 240));
        banner.setOutlineThickness(2.0f);
        banner.setOutlineColor(sf::Color(255, 215, 0));
        window.draw(banner);

        ve4GocLuaChon(window, sf::Vector2f(bx, by), sf::Vector2f(bw, bh), 2.0f, 1.5f);

        sf::Text t;
        t.setFont(fontBold);
        t.setCharacterSize(16);
        t.setStyle(sf::Text::Bold);
        std::string sideStr = (banCo.getLuotChoi() == Mau::DO) ? (tiengViet ? "BÊN ĐỎ ĐANG BỊ CHIẾU TƯỚNG!" : "RED KING IS UNDER CHECK!")
                                                              : (tiengViet ? "BÊN ĐEN ĐANG BỊ CHIẾU TƯỚNG!" : "BLACK KING IS UNDER CHECK!");
        t.setString(toUtf8("[!]  " + sideStr + "  [!]"));
        t.setFillColor(sf::Color(255, 235, 120));
        sf::FloatRect tb = t.getLocalBounds();
        t.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));
        t.setPosition(std::round(1200.0f / 2.0f), std::round(by + bh / 2.0f));
        window.draw(t);
    } else {
        std::string turnStr = isRed ? Loc::get(LocKey::TURN_RED) : Loc::get(LocKey::TURN_BLACK);

        FantasyUI::get().drawRibbonTitle(window, 1200.0f / 2.0f, 20.0f, 260.0f, 46.0f,
                                         turnStr, font, 16,
                                         isRed ? sf::Color(255, 230, 220) : sf::Color(220, 240, 255),
                                         isRed ? sf::Color(230, 110, 100) : sf::Color(100, 150, 220));
    }

    // Incense countdown timer (Stage 3.4) at X = 450.0f, Y = 72.0f, W = 300.0f, H = 16.0f
    float totalTime = (thoiGianVanDau > 0.0f ? thoiGianVanDau : 600.0f);
    float curTime = isRed ? thoiGianConDo : thoiGianConDen;
    FantasyUI::get().drawIncenseTimer(window, sf::FloatRect(450.0f, 72.0f, 300.0f, 16.0f), curTime, totalTime);
}

void GameManager::veThanhBenTrai() {
    float px = 35.0f;
    float py = 110.0f;
    float pw = 240.0f;
    float ph = 200.0f;
    bool tiengViet = (Loc::getLanguage() == Language::TIENG_VIET);
    bool isRedTurn = (banCo.getLuotChoi() == Mau::DO);

    auto formatTime = [](float t) {
        if (t <= 0.0f) return std::string("--:--");
        int m = static_cast<int>(t / 60.0f);
        int s = static_cast<int>(t) % 60;
        char buf[16];
        snprintf(buf, sizeof(buf), "%02d:%02d", m, s);
        return std::string(buf);
    };

    // 1. Player 1 Card (Red) - EBONY_WOOD (X=35, Y=110, W=240, H=200)
    FantasyUI::get().drawPanel(window, sf::FloatRect(px, py, pw, ph), OrientalPanelStyle::EBONY_WOOD);

    // Centered Avatar P1 at X=115, Y=120, size 80x44
    float avaW = 80.0f;
    float avaH = 44.0f;
    float avaX = 115.0f;
    float avaY = 120.0f;
    FantasyUI::get().drawPanel(window, sf::FloatRect(avaX, avaY, avaW, avaH),
                               OrientalPanelStyle::JADE_INSET,
                               isRedTurn ? sf::Color(255, 180, 180) : sf::Color(170, 170, 170));

    if (daTaiAvatarTextures && nhanVatDo >= 0 && nhanVatDo < 5 && avatarTextures[nhanVatDo].getSize().x > 0) {
        sf::Sprite spAva(avatarTextures[nhanVatDo]);
        float scale = 38.0f / static_cast<float>(avatarTextures[nhanVatDo].getSize().y);
        spAva.setScale(scale, scale);
        float sw = static_cast<float>(avatarTextures[nhanVatDo].getSize().x) * scale;
        spAva.setPosition(avaX + (avaW - sw) / 2.0f, avaY + 3.0f);
        window.draw(spAva);
    }

    // Player Name at Y = 172
    sf::Text rName;
    rName.setFont(fontBold);
    rName.setCharacterSize(15);
    rName.setStyle(sf::Text::Bold);
    rName.setString(toUtf8(tenNguoiChoiDo));
    rName.setFillColor(sf::Color(255, 230, 220));
    sf::FloatRect nb = rName.getLocalBounds();
    rName.setOrigin(std::round(nb.left + nb.width / 2.0f), std::round(nb.top));
    rName.setPosition(std::round(px + pw / 2.0f), 172.0f);
    window.draw(rName);

    // Digital Countdown Timer in JADE_INSET slot at Y = 240
    float timerSlotY = py + 130.0f; // 240.0f
    float timerSlotW = pw - 30.0f;
    float timerSlotX = px + 15.0f;
    float timerSlotH = 32.0f;
    FantasyUI::get().drawPanel(window, sf::FloatRect(timerSlotX, timerSlotY, timerSlotW, timerSlotH), OrientalPanelStyle::JADE_INSET);

    sf::Text timerTxt;
    timerTxt.setFont(fontBold);
    timerTxt.setCharacterSize(16);
    timerTxt.setStyle(sf::Text::Bold);
    timerTxt.setString(toUtf8(formatTime(thoiGianConDo)));
    timerTxt.setFillColor(isRedTurn && thoiGianConDo < 30.0f && thoiGianConDo > 0.0f
                          ? sf::Color(255, 70, 60)
                          : sf::Color(255, 225, 140));
    sf::FloatRect tb = timerTxt.getLocalBounds();
    timerTxt.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));
    timerTxt.setPosition(std::round(timerSlotX + timerSlotW / 2.0f), std::round(timerSlotY + timerSlotH / 2.0f));
    window.draw(timerTxt);

    // 2. Captured pieces tray (Y=505, H=140, W=240, X=35)
    float capX = 35.0f;
    float capY = 505.0f;
    float capW = 240.0f;
    float capH = 140.0f;
    FantasyUI::get().drawPanel(window, sf::FloatRect(capX, capY, capW, capH), OrientalPanelStyle::JADE_INSET);

    sf::Text capTitle;
    capTitle.setFont(fontBold);
    capTitle.setCharacterSize(12);
    capTitle.setStyle(sf::Text::Bold);
    capTitle.setString(toUtf8(tiengViet ? "QUÂN ĐÃ BẮT" : "CAPTURED PIECES"));
    capTitle.setFillColor(sf::Color(210, 185, 140));
    sf::FloatRect ctbTitle = capTitle.getLocalBounds();
    capTitle.setOrigin(std::round(ctbTitle.left + ctbTitle.width / 2.0f), std::round(ctbTitle.top));
    capTitle.setPosition(std::round(capX + capW / 2.0f), capY + 8.0f);
    window.draw(capTitle);

    int capCount = 0;
    float startChipX = capX + 16.0f;
    float startChipY = capY + 34.0f;
    for (const auto& q : banCo.getCacQuan()) {
        if (q && q->getDaBiAn() && q->getMau() == Mau::DEN) {
            float chipR = 9.0f;
            sf::CircleShape chip(chipR);
            chip.setPosition(startChipX + (capCount % 6) * 36.0f, startChipY + (capCount / 6) * 30.0f);
            chip.setFillColor(sf::Color(35, 42, 52));
            chip.setOutlineThickness(1.0f);
            chip.setOutlineColor(sf::Color(80, 110, 145));
            window.draw(chip);

            sf::Text chipTxt;
            chipTxt.setFont(fontBold);
            std::string code = layKyHieuQuanTheoStyle(q->layTen(), Mau::DEN);
            if (code.size() > 2) code = code.substr(0, 2);
            chipTxt.setString(toUtf8(code));
            chipTxt.setCharacterSize(10);
            chipTxt.setStyle(sf::Text::Bold);
            chipTxt.setFillColor(sf::Color(180, 210, 245));
            sf::FloatRect ctb = chipTxt.getLocalBounds();
            chipTxt.setOrigin(std::round(ctb.left + ctb.width / 2.0f), std::round(ctb.top + ctb.height / 2.0f));
            chipTxt.setPosition(std::round(chip.getPosition().x + chipR), std::round(chip.getPosition().y + chipR));
            window.draw(chipTxt);

            capCount++;
            if (capCount >= 16) break;
        }
    }
    if (capCount == 0) {
        sf::Text noCap;
        noCap.setFont(font);
        noCap.setCharacterSize(12);
        noCap.setString(toUtf8(tiengViet ? "[ Chưa có quân bị bắt ]" : "[ No captured pieces ]"));
        noCap.setFillColor(sf::Color(160, 145, 130));
        sf::FloatRect ncb = noCap.getLocalBounds();
        noCap.setOrigin(std::round(ncb.left + ncb.width / 2.0f), std::round(ncb.top + ncb.height / 2.0f));
        noCap.setPosition(std::round(capX + capW / 2.0f), std::round(capY + capH / 2.0f + 6.0f));
        window.draw(noCap);
    }

    // 3. In-game Left Panel Action Buttons (Undo, Redo, Draw, Resign, Help, Menu)
    for (auto& btn : inGameButtons) {
        btn->draw(window);
    }
}

void GameManager::veThanhBenPhai() {
    float px = 925.0f;
    float py = 110.0f;
    float pw = 240.0f;
    float ph = 200.0f;
    bool tiengViet = (Loc::getLanguage() == Language::TIENG_VIET);
    bool isBlackTurn = (banCo.getLuotChoi() == Mau::DEN);

    auto formatTime = [](float t) {
        if (t <= 0.0f) return std::string("--:--");
        int m = static_cast<int>(t / 60.0f);
        int s = static_cast<int>(t) % 60;
        char buf[16];
        snprintf(buf, sizeof(buf), "%02d:%02d", m, s);
        return std::string(buf);
    };

    // 1. Player 2 Card (Black) - EBONY_WOOD (X=925, Y=110, W=240, H=200)
    FantasyUI::get().drawPanel(window, sf::FloatRect(px, py, pw, ph), OrientalPanelStyle::EBONY_WOOD);

    // Centered Avatar P2 at X=1005 (925 + 80), Y=120, size 80x44
    float avaW = 80.0f;
    float avaH = 44.0f;
    float avaX = 1005.0f;
    float avaY = 120.0f;
    FantasyUI::get().drawPanel(window, sf::FloatRect(avaX, avaY, avaW, avaH),
                               OrientalPanelStyle::JADE_INSET,
                               isBlackTurn ? sf::Color(160, 210, 255) : sf::Color(170, 170, 170));

    if (daTaiAvatarTextures && nhanVatDen >= 0 && nhanVatDen < 5 && avatarTextures[nhanVatDen].getSize().x > 0) {
        sf::Sprite spAva(avatarTextures[nhanVatDen]);
        float scale = 38.0f / static_cast<float>(avatarTextures[nhanVatDen].getSize().y);
        spAva.setScale(scale, scale);
        float sw = static_cast<float>(avatarTextures[nhanVatDen].getSize().x) * scale;
        spAva.setPosition(avaX + (avaW - sw) / 2.0f, avaY + 3.0f);
        window.draw(spAva);
    }

    // Player Name at Y = 172
    sf::Text bName;
    bName.setFont(fontBold);
    bName.setCharacterSize(15);
    bName.setStyle(sf::Text::Bold);
    bName.setString(toUtf8(tenNguoiChoiDen));
    bName.setFillColor(sf::Color(220, 240, 255));
    sf::FloatRect nb = bName.getLocalBounds();
    bName.setOrigin(std::round(nb.left + nb.width / 2.0f), std::round(nb.top));
    bName.setPosition(std::round(px + pw / 2.0f), 172.0f);
    window.draw(bName);

    // Digital Countdown Timer in JADE_INSET slot at Y = 240
    float timerSlotY = py + 130.0f;
    float timerSlotW = pw - 30.0f;
    float timerSlotX = px + 15.0f;
    float timerSlotH = 32.0f;
    FantasyUI::get().drawPanel(window, sf::FloatRect(timerSlotX, timerSlotY, timerSlotW, timerSlotH), OrientalPanelStyle::JADE_INSET);

    sf::Text timerTxt;
    timerTxt.setFont(fontBold);
    timerTxt.setCharacterSize(16);
    timerTxt.setStyle(sf::Text::Bold);
    timerTxt.setString(toUtf8(formatTime(thoiGianConDen)));
    timerTxt.setFillColor(isBlackTurn && thoiGianConDen < 30.0f && thoiGianConDen > 0.0f
                          ? sf::Color(255, 70, 60)
                          : sf::Color(180, 220, 255));
    sf::FloatRect tb = timerTxt.getLocalBounds();
    timerTxt.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));
    timerTxt.setPosition(std::round(timerSlotX + timerSlotW / 2.0f), std::round(timerSlotY + timerSlotH / 2.0f));
    window.draw(timerTxt);

    // 2. Move History Box (KỲ PHỔ) at X=925.0f, Y=330.0f, W=240.0f, H=378.0f (Dark Ebony Ledger)
    float histX = 925.0f;
    float histY = 330.0f;
    float histW = 240.0f;
    float histH = 378.0f;

    FantasyUI::get().drawPanel(window, sf::FloatRect(histX, histY, histW, histH), OrientalPanelStyle::EBONY_WOOD);

    sf::RectangleShape histInset(sf::Vector2f(histW - 18.0f, histH - 18.0f));
    histInset.setPosition(histX + 9.0f, histY + 9.0f);
    histInset.setFillColor(sf::Color(32, 20, 15));
    histInset.setOutlineThickness(1.0f);
    histInset.setOutlineColor(sf::Color(140, 102, 48, 175));
    window.draw(histInset);

    // Header: "KỲ PHỔ" / "MOVE HISTORY" in gold ivory with 1px shadow
    sf::Text titleHistShadow;
    titleHistShadow.setFont(fontBold);
    titleHistShadow.setCharacterSize(15);
    titleHistShadow.setStyle(sf::Text::Bold);
    titleHistShadow.setString(toUtf8(tiengViet ? "KỲ PHỔ" : "MOVE HISTORY"));
    titleHistShadow.setFillColor(sf::Color(0, 0, 0, 140));
    sf::FloatRect thb = titleHistShadow.getLocalBounds();
    titleHistShadow.setOrigin(std::round(thb.left + thb.width / 2.0f), std::round(thb.top));
    titleHistShadow.setPosition(std::round(histX + histW / 2.0f) + 1.0f, histY + 15.0f);
    window.draw(titleHistShadow);

    sf::Text titleHist = titleHistShadow;
    titleHist.setFillColor(sf::Color(235, 198, 118));
    titleHist.setPosition(std::round(histX + histW / 2.0f), histY + 14.0f);
    window.draw(titleHist);

    sf::RectangleShape div(sf::Vector2f(histW - 36.0f, 1.5f));
    div.setPosition(histX + 18.0f, histY + 38.0f);
    div.setFillColor(sf::Color(175, 132, 62, 190));
    window.draw(div);

    const auto& lichSu = banCo.getLichSuNuocDi();
    size_t count = lichSu.size();
    size_t maxDisplay = 13;
    size_t start = (count > maxDisplay) ? (count - maxDisplay) : 0;
    float textY = histY + 48.0f;

    if (lichSu.empty()) {
        sf::Text emptyT;
        emptyT.setFont(font);
        emptyT.setCharacterSize(14);
        emptyT.setStyle(sf::Text::Bold);
        emptyT.setString(toUtf8(Loc::get(LocKey::NO_MOVES_YET)));
        emptyT.setFillColor(sf::Color(180, 158, 125));
        sf::FloatRect etb = emptyT.getLocalBounds();
        emptyT.setOrigin(std::round(etb.left + etb.width / 2.0f), std::round(etb.top + etb.height / 2.0f));
        emptyT.setPosition(std::round(histX + histW / 2.0f), std::round(histY + histH / 2.0f + 10.0f));
        window.draw(emptyT);
    } else {
        for (size_t i = start; i < count; ++i) {
            if ((i - start) % 2 == 1) {
                sf::RectangleShape strip(sf::Vector2f(histW - 26.0f, 22.0f));
                strip.setPosition(histX + 13.0f, textY - 2.0f);
                strip.setFillColor(sf::Color(255, 236, 196, 14));
                window.draw(strip);
            }

            bool isRedMove = (lichSu[i].mauQuan == Mau::DO);
            sf::CircleShape dot(3.5f);
            dot.setPosition(histX + 18.0f, textY + 6.0f);
            dot.setFillColor(isRedMove ? sf::Color(215, 68, 56) : sf::Color(135, 185, 165));
            window.draw(dot);

            sf::Text ndText;
            ndText.setFont(font);
            ndText.setCharacterSize(13);
            ndText.setStyle(sf::Text::Bold);
            std::string line = std::to_string(i + 1) + ". " + banCo.layMoTaNuocDi(lichSu[i]);
            ndText.setString(toUtf8(line));
            ndText.setFillColor(sf::Color(230, 210, 170));
            ndText.setPosition(histX + 31.0f, textY);
            window.draw(ndText);
            textY += 23.0f;
        }
    }
}

void GameManager::vePopupKetThuc() {
    if (!daPhatAmKetThuc) {
        phatAmThanhKetThuc();
    }

    // Dark overlay covering 100% of physical window pixels
    FantasyUI::get().drawFullDimOverlay(window, sf::Color(0, 0, 0, 195));

    const float dw = 600.0f;
    const float dh = 450.0f;
    const float dx = (1200.0f - dw) / 2.0f;
    const float dy = (800.0f - dh) / 2.0f;

    bool tiengViet = (Loc::getLanguage() == Language::TIENG_VIET);
    sf::Font& fTitle = getMenuFont(tiengViet, true);
    sf::Font& fBody = getMenuFont(tiengViet, false);
    sf::Font& fBodyBold = getMenuFont(tiengViet, true);

    // Match result
    Mau winner = banCo.getNguoiThang();
    enum class MatchOutcome { WIN, LOSE, DRAW };
    MatchOutcome outcome = MatchOutcome::DRAW;

    if (winner == Mau::HOA) {
        outcome = MatchOutcome::DRAW;
    } else if (cheDoChoi == CheDoChoi::VOI_MAY) {
        if (winner == aiPlayerSide) outcome = MatchOutcome::WIN;
        else outcome = MatchOutcome::LOSE;
    } else {
        outcome = MatchOutcome::WIN;
    }

    // Result colors
    sf::Color borderColor;
    sf::Color badgeBgColor;
    sf::Color titleColor;
    std::string badgeStr;
    std::string subHeaderStr;

    if (outcome == MatchOutcome::WIN) {
        borderColor = sf::Color(225, 185, 60);         // Hoàng kim rực rỡ
        badgeBgColor = sf::Color(32, 95, 55, 230);      // Xanh ngọc lục bảo chiến thắng
        titleColor = sf::Color(255, 225, 95);          // Vàng kim sáng
        badgeStr = tiengViet ? "[ CHIẾN THẮNG QUANG VINH ]" : "[ GLORIOUS VICTORY ]";
        if (cheDoChoi == CheDoChoi::VOI_MAY) {
            subHeaderStr = tiengViet
                ? "Chúc mừng bạn đã xuất sắc đánh bại Trí Tuệ Nhân Tạo!"
                : "Outstanding match! You have conquered the AI!";
        } else if (cheDoChoi == CheDoChoi::HAI_NGUOI) {
            std::string tenThang = (winner == Mau::DO) ? tenNguoiChoiDo : tenNguoiChoiDen;
            subHeaderStr = tenThang + (tiengViet ? " đã giành chiến thắng thuyết phục!" : " won the match!");
        } else {
            std::string tenThang = (winner == Mau::DO) ? tenNguoiChoiDo : tenNguoiChoiDen;
            subHeaderStr = tenThang + (tiengViet ? " thắng thế trong trận diễn tập!" : " won the simulation!");
        }
    } else if (outcome == MatchOutcome::LOSE) {
        borderColor = sf::Color(215, 55, 45);          // Đỏ chu sa thẫm
        badgeBgColor = sf::Color(105, 25, 20, 230);     // Đỏ ruby sẫm
        titleColor = sf::Color(255, 110, 100);         // Đỏ tươi nổi bật
        badgeStr = tiengViet ? "[ THẤT BẠI TIẾC NUỐI ]" : "[ DEFEAT ]";
        subHeaderStr = tiengViet
            ? "Trí Tuệ Nhân Tạo đã giành thế trận áp đảo. Hãy thử lại để phục thù!"
            : "The AI has claimed victory this round. Play again to fight back!";
    } else { // DRAW
        borderColor = sf::Color(140, 175, 215);        // Xanh lam bạc trang nhã
        badgeBgColor = sf::Color(45, 60, 85, 230);      // Lam sẫm
        titleColor = sf::Color(185, 220, 255);         // Lam sáng thanh nhã
        badgeStr = tiengViet ? "[ BẤT PHÂN THẮNG BẠI ]" : "[ HONORABLE DRAW ]";
        subHeaderStr = tiengViet
            ? "Cục diện cân bằng tuyệt đối, hai bên cùng ký biên bản hòa kỳ!"
            : "Equal strength displayed! The game concluded in a noble draw!";
    }

    // 3. Khung hộp thông báo chính
    FantasyUI::get().drawPanel(window, sf::FloatRect(dx, dy, dw, dh), FantasyPanelStyle::ORNATE_WOOD, borderColor);

    // Result badge: ornate fantasy ribbon banner
    const float badgeW = 420.0f;
    FantasyUI::get().drawRibbonTitle(window, dx + dw / 2.0f, dy + 16.0f, badgeW, 44.0f,
                                     badgeStr, fTitle, 19, titleColor, borderColor);

    // 7. Dòng phụ đề thông điệp kết quả
    sf::Text tSub;
    tSub.setFont(fBodyBold);
    tSub.setCharacterSize(14);
    tSub.setStyle(sf::Text::Bold);
    tSub.setString(toUtf8(subHeaderStr));
    tSub.setFillColor(sf::Color(245, 230, 200));
    sf::FloatRect sb = tSub.getLocalBounds();
    tSub.setOrigin(sb.left + sb.width / 2.0f, sb.top + sb.height / 2.0f);
    tSub.setPosition(dx + dw / 2.0f, dy + 76.0f);
    window.draw(tSub);

    // Match statistics
    const float statW = dw - 60.0f;
    const float statH = 195.0f;
    const float statX = dx + 30.0f;
    const float statY = dy + 102.0f;

    FantasyUI::get().drawPanel(window, sf::FloatRect(statX, statY, statW, statH), FantasyPanelStyle::WOOD_SLOT);

    // Tiêu đề bảng thống kê
    sf::Text sHeader;
    sHeader.setFont(fBodyBold);
    sHeader.setCharacterSize(13);
    sHeader.setStyle(sf::Text::Bold);
    sHeader.setString(toUtf8(tiengViet ? "--- TỔNG KẾT VÁN ĐẤU ---" : "--- MATCH REPORT ---"));
    sHeader.setFillColor(sf::Color(210, 175, 105));
    sf::FloatRect shb = sHeader.getLocalBounds();
    sHeader.setOrigin(shb.left + shb.width / 2.0f, shb.top + shb.height / 2.0f);
    sHeader.setPosition(statX + statW / 2.0f, statY + 16.0f);
    window.draw(sHeader);

    // Đường kẻ phân cách
    sf::RectangleShape line(sf::Vector2f(statW - 36.0f, 1.0f));
    line.setPosition(statX + 18.0f, statY + 28.0f);
    line.setFillColor(sf::Color(70, 50, 32));
    window.draw(line);

    auto formatTime = [](float t) {
        if (t <= 0.0f) return std::string("--:--");
        int m = static_cast<int>(t / 60.0f);
        int s = static_cast<int>(t) % 60;
        char buf[16];
        snprintf(buf, sizeof(buf), "%02d:%02d", m, s);
        return std::string(buf);
    };

    std::string modeStr;
    if (cheDoChoi == CheDoChoi::HAI_NGUOI) modeStr = tiengViet ? "Đấu hai người (PvP)" : "Two Players (PvP)";
    else if (cheDoChoi == CheDoChoi::VOI_MAY) modeStr = tiengViet ? "Đấu với máy (PvAI)" : "Play vs AI (PvAI)";
    else modeStr = tiengViet ? "Mô phỏng tự động (Demo)" : "Auto Simulation (Demo)";

    std::string reasonStr = lyDoKetThuc.empty() ? (tiengViet ? "Chiếu bí đối phương" : "Checkmate") : lyDoKetThuc;
    size_t movesCount = banCo.getLichSuNuocDi().size();
    size_t roundsCount = (movesCount + 1) / 2;

    struct StatRow {
        std::string label;
        std::string val;
        sf::Color valCol;
    };

    std::vector<StatRow> rows = {
        { tiengViet ? "Chế độ chơi:" : "Game Mode:", modeStr, sf::Color(245, 230, 205) },
        { tiengViet ? "Nguyên nhân kết thúc:" : "Outcome Reason:", reasonStr, sf::Color(255, 195, 100) },
        { tiengViet ? "Tổng số nước đi:" : "Total Moves:",
          std::to_string(movesCount) + (tiengViet ? " nước (" : " moves (") + std::to_string(roundsCount) + (tiengViet ? " hiệp)" : " rounds)"),
          sf::Color(220, 210, 190) },
        { tiengViet ? "Thời gian bên Đỏ còn:" : "Red Remaining Time:", formatTime(thoiGianConDo), sf::Color(255, 130, 120) },
        { tiengViet ? "Thời gian bên Đen còn:" : "Black Remaining Time:", formatTime(thoiGianConDen), sf::Color(135, 190, 255) }
    };

    float curRowY = statY + 38.0f;
    for (size_t r = 0; r < rows.size(); ++r) {
        if (r % 2 == 1) {
            sf::RectangleShape rowStrip(sf::Vector2f(statW - 16.0f, 24.0f));
            rowStrip.setPosition(statX + 8.0f, curRowY - 2.0f);
            rowStrip.setFillColor(sf::Color(255, 255, 255, 6));
            window.draw(rowStrip);
        }

        sf::Text tLbl;
        tLbl.setFont(fBodyBold);
        tLbl.setCharacterSize(13);
        tLbl.setStyle(sf::Text::Bold);
        tLbl.setString(toUtf8(rows[r].label));
        tLbl.setFillColor(sf::Color(215, 195, 170));
        tLbl.setPosition(statX + 20.0f, curRowY + 1.0f);
        window.draw(tLbl);

        sf::Text tVal;
        tVal.setFont(fBodyBold);
        tVal.setCharacterSize(13);
        tVal.setStyle(sf::Text::Bold);
        tVal.setString(toUtf8(rows[r].val));
        tVal.setFillColor(rows[r].valCol);
        sf::FloatRect vb = tVal.getLocalBounds();
        tVal.setPosition(statX + statW - 20.0f - vb.width, curRowY + 1.0f);
        window.draw(tVal);

        curRowY += 29.0f;
    }

    // Action buttons
    for (auto& btn : popupButtons) {
        btn->draw(window);
    }

    // Shortcut prompts
    sf::Text tHint;
    tHint.setFont(fBodyBold);
    tHint.setCharacterSize(13);
    tHint.setStyle(sf::Text::Bold);
    tHint.setString(toUtf8(tiengViet ? "[ Phím Enter: Bắt đầu ván mới  |  Phím ESC: Về Menu chính ]"
                                     : "[ Enter: Play Again  |  ESC: Main Menu ]"));
    tHint.setFillColor(sf::Color(220, 190, 150));
    sf::FloatRect hb = tHint.getLocalBounds();
    tHint.setOrigin(hb.left + hb.width / 2.0f, hb.top + hb.height / 2.0f);
    tHint.setPosition(dx + dw / 2.0f, dy + dh - 14.0f);
    window.draw(tHint);
}

void GameManager::veMenuPopUpInGame() {
    FantasyUI::get().drawFullDimOverlay(window, sf::Color(0, 0, 0, 160));

    // Stage 4: Modal W = 400.0f, H = 460.0f, centered at X = 400.0f, Y = 170.0f
    const float dw = 400.0f;
    const float dh = 460.0f;
    const float dx = 400.0f;
    const float dy = 170.0f;

    // 1. Modal background - EBONY_WOOD
    FantasyUI::get().drawPanel(window, sf::FloatRect(dx, dy, dw, dh), OrientalPanelStyle::EBONY_WOOD);

    // 2. Y = 185: Ribbon tiêu đề "TẠM DỪNG" (W = 240px)
    FantasyUI::get().drawRibbonTitle(window, 1200.0f / 2.0f, 185.0f, 240.0f, 42.0f,
                                     Loc::get(LocKey::PAUSE_MENU_TITLE), font, 16,
                                     sf::Color(255, 235, 140), sf::Color(255, 210, 100));

    // 3. Draw popup menu buttons (245, 300, 350, 400)
    for (auto& btn : inGameMenuButtons) {
        btn->draw(window);
    }

    // 4. Y = 455: Hint text "Nhấn [ESC] để quay lại ngay"
    bool tiengViet = (Loc::getLanguage() == Language::TIENG_VIET);
    sf::Text tHint;
    tHint.setFont(font);
    tHint.setCharacterSize(12);
    tHint.setStyle(sf::Text::Bold);
    tHint.setString(toUtf8(tiengViet ? "Nhấn [ESC] để quay lại ngay" : "Press [ESC] to quick resume"));
    tHint.setFillColor(sf::Color(190, 170, 145, 220));
    sf::FloatRect hb = tHint.getLocalBounds();
    tHint.setOrigin(std::round(hb.left + hb.width / 2.0f), std::round(hb.top + hb.height / 2.0f));
    tHint.setPosition(1200.0f / 2.0f, 458.0f);
    window.draw(tHint);

    // 5. Y = 485: Match indicator slot tray in lower space (zero dead space)
    float slotY = 485.0f;
    float slotH = 75.0f;
    float slotW = 320.0f;
    float slotX = 440.0f;
    FantasyUI::get().drawPanel(window, sf::FloatRect(slotX, slotY, slotW, slotH), OrientalPanelStyle::JADE_INSET);

    bool isRedTurn = (banCo.getLuotChoi() == Mau::DO);
    sf::Text tStatus;
    tStatus.setFont(fontBold);
    tStatus.setCharacterSize(12);
    tStatus.setStyle(sf::Text::Bold);
    std::string turnStr = isRedTurn ? (tiengViet ? "Lượt đi: Bên Đỏ" : "Turn: Red") : (tiengViet ? "Lượt đi: Bên Đen" : "Turn: Black");
    std::string s1 = turnStr + (tiengViet ? "  •  Nước đi: " : "  •  Move: ") + std::to_string(banCo.getLichSuNuocDi().size() + 1);
    tStatus.setString(toUtf8(s1));
    tStatus.setFillColor(isRedTurn ? sf::Color(255, 175, 160) : sf::Color(170, 215, 255));
    sf::FloatRect sb = tStatus.getLocalBounds();
    tStatus.setOrigin(std::round(sb.left + sb.width / 2.0f), std::round(sb.top + sb.height / 2.0f));
    tStatus.setPosition(1200.0f / 2.0f, slotY + 22.0f);
    window.draw(tStatus);

    sf::Text tFooter;
    tFooter.setFont(font);
    tFooter.setCharacterSize(11);
    tFooter.setString(toUtf8(tiengViet ? "[F11] Toàn màn hình  •  [M] Bật/Tắt âm" : "[F11] Fullscreen  •  [M] Mute"));
    tFooter.setFillColor(sf::Color(170, 155, 140, 200));
    sf::FloatRect fb = tFooter.getLocalBounds();
    tFooter.setOrigin(std::round(fb.left + fb.width / 2.0f), std::round(fb.top + fb.height / 2.0f));
    tFooter.setPosition(1200.0f / 2.0f, slotY + 50.0f);
    window.draw(tFooter);
}

void GameManager::veToast() {
    if (toastTimer <= 0.0f || toastMessage.empty()) return;

    sf::Text t;
    t.setFont(font);
    t.setCharacterSize(16);
    t.setString(toUtf8(toastMessage));
    t.setFillColor(sf::Color(255, 235, 180));
    t.setStyle(sf::Text::Bold);

    sf::FloatRect tb = t.getLocalBounds();
    float tw = tb.width + 48.0f;
    float th = 40.0f;
    float tx = (1200.0f - tw) / 2.0f;
    float ty = 800.0f - 55.0f;

    FantasyUI::get().drawPanel(window, sf::FloatRect(tx, ty, tw, th), FantasyPanelStyle::ORNATE_WOOD);

    t.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
    t.setPosition(tx + tw / 2.0f, ty + th / 2.0f - 1.0f);

    sf::Text shadow = t;
    shadow.setPosition(tx + tw / 2.0f + 1.2f, ty + th / 2.0f + 0.2f);
    shadow.setFillColor(sf::Color(10, 6, 4, 220));
    window.draw(shadow);
    window.draw(t);
}

    // Screen renderers
void GameManager::veMenuChinh() {
    
    
    
    bool isVietnamese = (Loc::getLanguage() == Language::TIENG_VIET);
    menuChinh.draw(window, isVietnamese);
}

void GameManager::vePlaySubmenu() {
    sf::Text title;
    title.setFont(fontBold);
    title.setString(toUtf8(Loc::get(LocKey::PLAY_SUB_TITLE)));
    title.setCharacterSize(38);
    title.setFillColor(sf::Color(240, 195, 95));
    title.setStyle(sf::Text::Bold);

    sf::FloatRect tb = title.getLocalBounds();
    title.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));
    title.setPosition(1200.0f / 2.0f, 110.0f);
    window.draw(title);

    sf::RectangleShape goldLine(sf::Vector2f(320.0f, 3.0f));
    goldLine.setPosition((1200.0f - 320.0f) / 2.0f, 150.0f);
    goldLine.setFillColor(sf::Color(240, 190, 70));
    window.draw(goldLine);

    menuPlaySub.draw(window);
}

    // Play setup — Unified 1200x800 Oriental Dashboard

void GameManager::khoiTaoPlaySetupButtons() {
    playSetupButtons.clear();
    auto& mAssets = getMenuAssets();
    bool tiengViet = (Loc::getLanguage() == Language::TIENG_VIET);
    sf::Font& f = mAssets.vnFontLoaded ? mAssets.vnFont : font;

    // 1. Top Horizontal Mode Tab Bar inside Dashboard (X=160, Y=120, W=880, H=560)
    // Visual order: [PvP Mode] [Player vs AI] [Demo Match] while preserving playSetupMode IDs (1, 0, 2)
    struct ModeTabSpec {
        int modeId;
        std::string label;
        const sf::Texture* icon;
    };
    const std::vector<ModeTabSpec> tabSpecs = {
        { 1, tiengViet ? "Hai Người (PvP)" : "PvP Mode",       &mAssets.iconPlay },
        { 0, tiengViet ? "Đấu Với AI"       : "Player vs AI",   &mAssets.iconSettings },
        { 2, tiengViet ? "Máy Đấu Demo"    : "Demo Match",     &mAssets.iconInfo }
    };

    const float tabStartX = 196.0f;
    const float tabY = 140.0f;
    const float tabW = 256.0f;
    const float tabH = 42.0f;
    const float tabStep = 276.0f;

    for (size_t i = 0; i < tabSpecs.size(); ++i) {
        int targetMode = tabSpecs[i].modeId;
        float tx = tabStartX + static_cast<float>(i) * tabStep;
        auto tabBtn = std::make_unique<Button>(tx, tabY, tabW, tabH, tabSpecs[i].label, f, [this, targetMode]() {
            soundManager.play(SoundType::CLICK);
            playSetupMode = targetMode;
            khoiTaoPlaySetupButtons();
        });
        bool active = (playSetupMode == targetMode);
        tabBtn->setSelected(active);
        tabBtn->setCharacterSize(16);
        tabBtn->setColors(
            active ? sf::Color(255, 226, 128) : sf::Color(242, 232, 206),
            sf::Color(255, 234, 150),
            sf::Color(255, 250, 224)
        );
        tabBtn->setIcon(tabSpecs[i].icon);
        playSetupButtons.push_back(std::move(tabBtn));
    }

    // Labels helper
    auto getPieceStyleLabel = [this, tiengViet]() -> std::string {
        if (pieceStyle == PieceStyle::REALISTIC_WOOD) return tiengViet ? "Gỗ Cổ Điển" : "Classic Wood";
        if (pieceStyle == PieceStyle::VIETNAMESE) return tiengViet ? "Việt Hóa" : "Vietnamese";
        if (pieceStyle == PieceStyle::SHORT_CODE) return tiengViet ? "Rút Gọn" : "Short Code";
        return tiengViet ? "Quốc Tế" : "International";
    };

    auto getBoardThemeLabel = [this, tiengViet]() -> std::string {
        if (boardTheme == BoardTheme::CLASSIC_WOOD) return tiengViet ? "Gỗ Cổ Điển" : "Classic Wood";
        if (boardTheme == BoardTheme::IMPERIAL_JADE) return tiengViet ? "Ngọc Bích" : "Imperial Jade";
        if (boardTheme == BoardTheme::MIDNIGHT_INK) return tiengViet ? "Mực Đen" : "Midnight Ink";
        return tiengViet ? "Trúc Cổ" : "Warm Bamboo";
    };

    auto getTimerLabel = [this, tiengViet]() -> std::string {
        if (thoiGianVanDau == 0.0f) return tiengViet ? "Không Giới Hạn" : "Unlimited";
        if (thoiGianVanDau == 300.0f) return tiengViet ? "5 Phút / Bên" : "5 Mins / Side";
        if (thoiGianVanDau == 600.0f) return tiengViet ? "10 Phút / Bên" : "10 Mins / Side";
        if (thoiGianVanDau == 900.0f) return tiengViet ? "15 Phút / Bên" : "15 Mins / Side";
        return tiengViet ? "30 Phút / Bên" : "30 Mins / Side";
    };

    // Left Column Selector Helper: aligns nicely inside Left Ebony Panel (X=188..592)
    const float ctrlX = 348.0f;
    const float arrowW = 28.0f;
    const float valW = 166.0f;
    const float rowH = 34.0f;

    auto addCycleRow = [&](float y, const std::string& valText,
                           std::function<void()> onPrev, std::function<void()> onNext) {
        auto bPrev = std::make_unique<Button>(ctrlX, y, arrowW, rowH, "<", f, std::move(onPrev));
        bPrev->setCharacterSize(15);
        bPrev->setColors(sf::Color(242, 232, 206), sf::Color(255, 224, 128), sf::Color(255, 250, 220));
        playSetupButtons.push_back(std::move(bPrev));

        auto bVal = std::make_unique<Button>(ctrlX + arrowW + 3.0f, y, valW, rowH, valText, f, onNext);
        bVal->setCharacterSize(14);
        bVal->setColors(sf::Color(255, 232, 165), sf::Color(255, 244, 195), sf::Color(255, 250, 224));
        playSetupButtons.push_back(std::move(bVal));

        auto bNext = std::make_unique<Button>(ctrlX + arrowW + 3.0f + valW + 3.0f, y, arrowW, rowH, ">", f, std::move(onNext));
        bNext->setCharacterSize(15);
        bNext->setColors(sf::Color(242, 232, 206), sf::Color(255, 224, 128), sf::Color(255, 250, 220));
        playSetupButtons.push_back(std::move(bNext));
    };

    auto addCommonBoardRows = [&](float yPiece, float yBoard, float yTimer, float yAura) {
        addCycleRow(yPiece, getPieceStyleLabel(),
            [this]() {
                soundManager.play(SoundType::CLICK);
                pieceStyle = static_cast<PieceStyle>((static_cast<int>(pieceStyle) + 3) % 4);
                khoiTaoPlaySetupButtons();
            },
            [this]() {
                soundManager.play(SoundType::CLICK);
                pieceStyle = static_cast<PieceStyle>((static_cast<int>(pieceStyle) + 1) % 4);
                khoiTaoPlaySetupButtons();
            });

        addCycleRow(yBoard, getBoardThemeLabel(),
            [this]() {
                soundManager.play(SoundType::CLICK);
                boardTheme = static_cast<BoardTheme>((static_cast<int>(boardTheme) + 3) % 4);
                khoiTaoPlaySetupButtons();
            },
            [this]() {
                soundManager.play(SoundType::CLICK);
                boardTheme = static_cast<BoardTheme>((static_cast<int>(boardTheme) + 1) % 4);
                khoiTaoPlaySetupButtons();
            });

        if (yTimer > 0.0f) {
            addCycleRow(yTimer, getTimerLabel(),
                [this]() {
                    soundManager.play(SoundType::CLICK);
                    if (thoiGianVanDau == 0.0f) thoiGianVanDau = 1800.0f;
                    else if (thoiGianVanDau == 300.0f) thoiGianVanDau = 0.0f;
                    else if (thoiGianVanDau == 600.0f) thoiGianVanDau = 300.0f;
                    else if (thoiGianVanDau == 900.0f) thoiGianVanDau = 600.0f;
                    else thoiGianVanDau = 900.0f;
                    khoiTaoPlaySetupButtons();
                },
                [this]() {
                    soundManager.play(SoundType::CLICK);
                    if (thoiGianVanDau == 0.0f) thoiGianVanDau = 300.0f;
                    else if (thoiGianVanDau == 300.0f) thoiGianVanDau = 600.0f;
                    else if (thoiGianVanDau == 600.0f) thoiGianVanDau = 900.0f;
                    else if (thoiGianVanDau == 900.0f) thoiGianVanDau = 1800.0f;
                    else thoiGianVanDau = 0.0f;
                    khoiTaoPlaySetupButtons();
                });
        }

        if (yAura > 0.0f) {
            addCycleRow(yAura, layTenPieceAura(p1Aura),
                [this]() {
                    soundManager.play(SoundType::CLICK);
                    p1Aura = static_cast<PieceAura>((static_cast<int>(p1Aura) + 4) % 5);
                    khoiTaoPlaySetupButtons();
                },
                [this]() {
                    soundManager.play(SoundType::CLICK);
                    p1Aura = static_cast<PieceAura>((static_cast<int>(p1Aura) + 1) % 5);
                    khoiTaoPlaySetupButtons();
                });
        }
    };

    auto addAvatarRow = [&](float y, int& charIdx) {
        auto avaPrev = std::make_unique<Button>(360.0f, y, 26.0f, 32.0f, "<", f, [this, &charIdx]() {
            soundManager.play(SoundType::CLICK);
            charIdx = (charIdx + Loc::getCharCount() - 1) % Loc::getCharCount();
            khoiTaoPlaySetupButtons();
        });
        avaPrev->setColors(sf::Color(242, 232, 206), sf::Color(255, 224, 128), sf::Color(255, 250, 220));
        playSetupButtons.push_back(std::move(avaPrev));

        auto avaVal = std::make_unique<Button>(389.0f, y, 118.0f, 32.0f, Loc::getCharName(charIdx), f, [this, &charIdx]() {
            soundManager.play(SoundType::CLICK);
            charIdx = (charIdx + 1) % Loc::getCharCount();
            khoiTaoPlaySetupButtons();
        });
        avaVal->setCharacterSize(13);
        avaVal->setColors(sf::Color(248, 238, 212), sf::Color(255, 232, 160), sf::Color(255, 250, 224));
        playSetupButtons.push_back(std::move(avaVal));

        auto avaNext = std::make_unique<Button>(510.0f, y, 26.0f, 32.0f, ">", f, [this, &charIdx]() {
            soundManager.play(SoundType::CLICK);
            charIdx = (charIdx + 1) % Loc::getCharCount();
            khoiTaoPlaySetupButtons();
        });
        avaNext->setColors(sf::Color(242, 232, 206), sf::Color(255, 224, 128), sf::Color(255, 250, 220));
        playSetupButtons.push_back(std::move(avaNext));
    };

    // 2. Mode-Specific Left Column Controls
    if (playSetupMode == 0) {
        // PvAI Mode
        inputAiPlayerName->setPosition(204.0f, 250.0f);
        inputAiPlayerName->setSize(148.0f, 32.0f);
        inputAiPlayerName->setParchmentStyle(true);
        addAvatarRow(250.0f, aiPlayerCharIndex);

        auto getDiffLabel = [this, tiengViet]() -> std::string {
            if (aiDifficulty == DoKho::DE) return tiengViet ? "Dễ" : "Easy";
            if (aiDifficulty == DoKho::TRUNG_BINH) return tiengViet ? "Trung Bình" : "Medium";
            return tiengViet ? "Khó" : "Hard";
        };
        addCycleRow(304.0f, getDiffLabel(),
            [this]() {
                soundManager.play(SoundType::CLICK);
                if (aiDifficulty == DoKho::KHO) aiDifficulty = DoKho::TRUNG_BINH;
                else if (aiDifficulty == DoKho::TRUNG_BINH) aiDifficulty = DoKho::DE;
                else aiDifficulty = DoKho::KHO;
                khoiTaoPlaySetupButtons();
            },
            [this]() {
                soundManager.play(SoundType::CLICK);
                if (aiDifficulty == DoKho::DE) aiDifficulty = DoKho::TRUNG_BINH;
                else if (aiDifficulty == DoKho::TRUNG_BINH) aiDifficulty = DoKho::KHO;
                else aiDifficulty = DoKho::DE;
                khoiTaoPlaySetupButtons();
            });

        auto getSideLabel = [this, tiengViet]() -> std::string {
            if (aiFirstChoice == 0) return tiengViet ? "Người Chơi (Đỏ)" : "Player (Red)";
            if (aiFirstChoice == 1) return tiengViet ? "Máy Đi Trước (Đỏ)" : "AI First (Red)";
            return tiengViet ? "Ngẫu Nhiên" : "Random Side";
        };
        addCycleRow(352.0f, getSideLabel(),
            [this]() {
                soundManager.play(SoundType::CLICK);
                aiFirstChoice = (aiFirstChoice + 2) % 3;
                khoiTaoPlaySetupButtons();
            },
            [this]() {
                soundManager.play(SoundType::CLICK);
                aiFirstChoice = (aiFirstChoice + 1) % 3;
                khoiTaoPlaySetupButtons();
            });

        addCommonBoardRows(400.0f, 448.0f, 496.0f, 544.0f);
    } else if (playSetupMode == 1) {
        // PvP Mode
        inputP1Name->setPosition(204.0f, 246.0f);
        inputP1Name->setSize(148.0f, 32.0f);
        inputP1Name->setParchmentStyle(true);
        addAvatarRow(246.0f, p1CharIndex);

        inputP2Name->setPosition(204.0f, 290.0f);
        inputP2Name->setSize(148.0f, 32.0f);
        inputP2Name->setParchmentStyle(true);
        addAvatarRow(290.0f, p2CharIndex);

        auto tossBtn = std::make_unique<Button>(204.0f, 336.0f, 372.0f, 34.0f,
            tiengViet ? "Tung Đồng Xu Chọn Bên Đi Trước" : "Toss Coin For First Move", f, [this, tiengViet]() {
            soundManager.play(SoundType::CAPTURE);
            bool p1Red = (rand() % 2 == 0);
            pvpFirstPlayer = p1Red ? 1 : 2;
            std::string n1 = inputP1Name->getValue().empty() ? "Player 1" : inputP1Name->getValue();
            std::string n2 = inputP2Name->getValue().empty() ? "Player 2" : inputP2Name->getValue();
            pvpRollBanner = p1Red
                ? (tiengViet ? (n1 + " nhận quân ĐỎ và đi trước!") : (n1 + " plays RED and moves first!"))
                : (tiengViet ? (n2 + " nhận quân ĐỎ và đi trước!") : (n2 + " plays RED and moves first!"));
        });
        tossBtn->setCharacterSize(14);
        tossBtn->setColors(sf::Color(255, 224, 130), sf::Color(255, 245, 195), sf::Color(255, 250, 220));
        playSetupButtons.push_back(std::move(tossBtn));

        addCommonBoardRows(406.0f, 452.0f, 498.0f, 544.0f);
    } else {
        // Demo Mode
        addCommonBoardRows(416.0f, 468.0f, -1.0f, -1.0f);
    }

    // 3. Bottom Dashboard Action Bar (Back + Centered Prominent Start Match Button)
    auto backBtn = std::make_unique<Button>(188.0f, 616.0f, 136.0f, 42.0f,
        tiengViet ? "Quay Lại" : "Back", f, [this]() {
        soundManager.play(SoundType::CLICK);
        trangThai = TrangThai::MENU_CHINH;
        menuChinh.triggerOpenAnimation(MenuSection::PLAY);
    });
    backBtn->setCharacterSize(16);
    backBtn->setColors(sf::Color(242, 232, 206), sf::Color(255, 224, 128), sf::Color(255, 250, 220));
    playSetupButtons.push_back(std::move(backBtn));

    std::string startLabel = (playSetupMode == 2)
        ? (tiengViet ? "BẮT ĐẦU MÔ PHỎNG" : "START DEMO")
        : (tiengViet ? "BẮT ĐẦU VÁN ĐẤU" : "START MATCH");

    auto startBtn = std::make_unique<Button>(460.0f, 612.0f, 280.0f, 50.0f, startLabel, f, [this, tiengViet]() {
        soundManager.play(SoundType::CLICK);
        if (playSetupMode == 0) {
            batDauPvai();
        } else if (playSetupMode == 1) {
            batDauPvp();
        } else {
            tenNguoiChoiDo = tiengViet ? "Quân Đỏ (Xe & Voi)" : "Red (Chariot & Elephant)";
            tenNguoiChoiDen = tiengViet ? "Quân Đen (Xe & Voi)" : "Black (Chariot & Elephant)";
            nhanVatDo = 0;
            nhanVatDen = 0;
            batDauTroChoiMoi(CheDoChoi::DEMO_2XE_2VOI);
        }
    });
    startBtn->setCharacterSize(18);
    startBtn->setSelected(true);
    startBtn->setColors(sf::Color(255, 236, 175), sf::Color(255, 248, 210), sf::Color(255, 255, 235));
    playSetupButtons.push_back(std::move(startBtn));
}

void GameManager::vePieceStylePreview(sf::RenderWindow& win, float x, float y, float w, float h) {
    auto& assets = getMenuAssets();
    sf::Font& f = assets.vnFontLoaded ? assets.vnFont : font;
    bool vi = (Loc::getLanguage() == Language::TIENG_VIET);

    // Subtle Aged Xuan inset box (no ruled lines)
    sf::RectangleShape box(sf::Vector2f(w, h));
    box.setPosition(x, y);
    box.setFillColor(sf::Color(225, 208, 176));
    box.setOutlineThickness(1.5f);
    box.setOutlineColor(sf::Color(65, 38, 20));
    win.draw(box);

    sf::Text lbl;
    lbl.setFont(f);
    lbl.setString(toUtf8(vi ? "Quân cờ mẫu:" : "Piece Sample:"));
    lbl.setCharacterSize(16);
    lbl.setStyle(sf::Text::Bold);
    lbl.setFillColor(sf::Color(35, 18, 10));
    lbl.setPosition(x + 14.0f, std::round(y + (h - 20.0f) / 2.0f));
    win.draw(lbl);

    float r = std::min(21.0f, (h - 12.0f) / 2.0f);
    sf::Vector2f posRed(std::round(x + w * 0.65f), std::round(y + h / 2.0f));
    sf::Vector2f posBlack(std::round(x + w * 0.86f), std::round(y + h / 2.0f));

    auto drawSamplePiece = [&](sf::Vector2f pos, Mau side) {
        bool isRed = (side == Mau::DO);
        const sf::Texture* tex = (pieceStyle == PieceStyle::REALISTIC_WOOD) ? layTextureQuan("Tuong", side) : nullptr;

        sf::CircleShape shadow(r);
        shadow.setOrigin(r, r);
        shadow.setPosition(pos.x + 1.5f, pos.y + 2.5f);
        shadow.setFillColor(sf::Color(15, 8, 4, 110));
        win.draw(shadow);

        if (tex != nullptr && tex->getSize().x > 0) {
            sf::CircleShape darkRim(r + 1.0f);
            darkRim.setOrigin(r + 1.0f, r + 1.0f);
            darkRim.setPosition(pos);
            darkRim.setFillColor(sf::Color(26, 14, 8));
            win.draw(darkRim);

            sf::Sprite sp(*tex);
            sf::Vector2u sz = tex->getSize();
            sp.setOrigin(sz.x / 2.0f, sz.y / 2.0f);
            float sc = (r * 2.0f) / static_cast<float>(sz.x);
            sp.setScale(sc, sc);
            sp.setPosition(pos);
            win.draw(sp);
        } else {
            sf::CircleShape outer(r + 2.0f);
            outer.setOrigin(r + 2.0f, r + 2.0f);
            outer.setPosition(pos);
            outer.setFillColor(isRed ? sf::Color(140, 28, 20) : sf::Color(24, 30, 40));
            outer.setOutlineThickness(1.0f);
            outer.setOutlineColor(sf::Color(26, 14, 8));
            win.draw(outer);

            sf::CircleShape piece(r);
            piece.setOrigin(r, r);
            piece.setPosition(pos);
            piece.setFillColor(isRed ? sf::Color(248, 236, 216) : sf::Color(232, 240, 236));
            win.draw(piece);

            sf::Text sym;
            sym.setFont(fontBold);
            std::string s = layKyHieuQuanTheoStyle("Tuong", side);
            sym.setString(toUtf8(s));
            sym.setCharacterSize(s.length() > 2 ? 12 : 14);
            sym.setStyle(sf::Text::Bold);
            sym.setFillColor(isRed ? sf::Color(175, 25, 20) : sf::Color(22, 34, 48));
            sf::FloatRect sb = sym.getLocalBounds();
            sym.setOrigin(std::round(sb.left + sb.width / 2.0f), std::round(sb.top + sb.height / 2.0f));
            sym.setPosition(pos.x, pos.y - 1.0f);
            win.draw(sym);
        }
    };

    drawSamplePiece(posRed, Mau::DO);
    drawSamplePiece(posBlack, Mau::DEN);
}

void GameManager::vePlaySetup() {
    auto& assets = getMenuAssets();
    bool tiengViet = (Loc::getLanguage() == Language::TIENG_VIET);
    sf::Font& f = assets.vnFontLoaded ? assets.vnFont : font;
    sf::Font& fBold = assets.fontBoldLoaded ? assets.fontBold : fontBold;

    // 1. Subtle Fullscreen Dim Backdrop over Atmospheric Courtyard (1200x800)
    FantasyUI::get().drawFullDimOverlay(window, sf::Color(10, 8, 12, 75));

    // Top Antique Silk Ribbon Title
    FantasyUI::get().drawRibbonTitle(window, 600.0f, 38.0f, 380.0f, 56.0f,
        tiengViet ? "THIẾT LẬP TRẬN ĐẤU" : "MATCH SETUP", fBold, 20);

    // 2. Unified Oriental Ebony Wood Dashboard Frame (880x560 at 160, 120)
    FantasyUI::get().drawPanel(window, sf::FloatRect(160.0f, 120.0f, 880.0f, 560.0f), OrientalPanelStyle::EBONY_WOOD);

    // Active Mode Tab Gold Corner Highlight (Tab 0=PvP, Tab 1=PvAI, Tab 2=Demo)
    int activeVisualTab = (playSetupMode == 1) ? 0 : ((playSetupMode == 0) ? 1 : 2);
    ve4GocLuaChon(window,
        sf::Vector2f(196.0f + activeVisualTab * 276.0f, 140.0f),
        sf::Vector2f(256.0f, 42.0f),
        1.5f, 1.5f);

    // 3. Two Symmetrical Columns: Left Ebony Controls & Right Clean Aged Xuan Paper
    FantasyUI::get().drawPanel(window, sf::FloatRect(188.0f, 198.0f, 404.0f, 402.0f), OrientalPanelStyle::EBONY_WOOD);
    sf::RectangleShape leftInset(sf::Vector2f(388.0f, 386.0f));
    leftInset.setPosition(196.0f, 206.0f);
    leftInset.setFillColor(sf::Color(24, 16, 12, 195));
    leftInset.setOutlineThickness(1.0f);
    leftInset.setOutlineColor(sf::Color(150, 112, 54, 145));
    window.draw(leftInset);

    FantasyUI::get().drawPanel(window, sf::FloatRect(608.0f, 198.0f, 404.0f, 402.0f), OrientalPanelStyle::PARCHMENT_SCROLL);

    // Helper: crisp dark ink or vermilion header with 1px drop shadow on bright Xuan paper
    auto drawShadowHeader = [&](const std::string& str, float cx, float y, unsigned int sz, sf::Color col) {
        sf::Text sh;
        sh.setFont(fBold);
        sh.setString(toUtf8(str));
        sh.setCharacterSize(sz);
        sh.setStyle(sf::Text::Bold);
        sh.setFillColor(sf::Color(0, 0, 0, 100));
        sf::FloatRect b = sh.getLocalBounds();
        sh.setOrigin(std::round(b.left + b.width / 2.0f), std::round(b.top + b.height / 2.0f));
        sh.setPosition(std::round(cx + 1.0f), std::round(y + 1.0f));
        window.draw(sh);

        sf::Text tx = sh;
        tx.setFillColor(col);
        tx.setPosition(std::round(cx), std::round(y));
        window.draw(tx);
    };

    // --- Left Column Content ---
    std::string leftHeader = (playSetupMode == 0)
        ? (tiengViet ? "TÙY CHỈNH ĐẤU MÁY" : "AI MATCH OPTIONS")
        : ((playSetupMode == 1)
            ? (tiengViet ? "KỲ THỦ & LUẬT ĐẤU" : "PLAYERS & RULES")
            : (tiengViet ? "CÀI ĐẶT MÔ PHỎNG" : "SIMULATION OPTIONS"));
    drawShadowHeader(leftHeader, 390.0f, 220.0f, 17, sf::Color(235, 198, 118));

    sf::RectangleShape lDiv(sf::Vector2f(356.0f, 1.5f));
    lDiv.setPosition(212.0f, 236.0f);
    lDiv.setFillColor(sf::Color(160, 118, 56, 170));
    window.draw(lDiv);

    auto drawLeftRowLabels = [&](const std::vector<std::string>& labels, const float rowYs[]) {
        for (size_t i = 0; i < labels.size(); ++i) {
            sf::Text lbl;
            lbl.setFont(f);
            lbl.setString(toUtf8(labels[i]));
            lbl.setCharacterSize(15);
            lbl.setStyle(sf::Text::Bold);
            lbl.setFillColor(sf::Color(235, 218, 185));
            lbl.setPosition(204.0f, rowYs[i] + 6.0f);
            window.draw(lbl);
        }
    };

    auto drawPortrait = [&](float px, float py, int charIdx, sf::Color borderCol) {
        sf::RectangleShape avaBox(sf::Vector2f(34.0f, 34.0f));
        avaBox.setPosition(px, py);
        avaBox.setFillColor(sf::Color(235, 222, 195));
        avaBox.setOutlineThickness(1.5f);
        avaBox.setOutlineColor(borderCol);
        window.draw(avaBox);
        if (daTaiAvatarTextures && charIdx >= 0 && charIdx < 5 && avatarTextures[charIdx].getSize().x > 0) {
            sf::Sprite spAva(avatarTextures[charIdx]);
            spAva.setPosition(px + 2.0f, py + 2.0f);
            spAva.setScale(30.0f / static_cast<float>(avatarTextures[charIdx].getSize().x),
                           30.0f / static_cast<float>(avatarTextures[charIdx].getSize().y));
            window.draw(spAva);
        }
    };

    if (playSetupMode == 0) {
        inputAiPlayerName->draw(window);
        drawPortrait(542.0f, 249.0f, aiPlayerCharIndex, sf::Color(210, 165, 80));

        const std::vector<std::string> labels = {
            tiengViet ? "Độ khó AI:" : "AI Difficulty:",
            tiengViet ? "Lượt đi đầu:" : "First Move:",
            tiengViet ? "Kiểu quân cờ:" : "Piece Style:",
            tiengViet ? "Màu bàn cờ:" : "Board Theme:",
            tiengViet ? "Thời gian ván:" : "Match Timer:",
            tiengViet ? "Ấn ký Tướng:" : "King Seal:"
        };
        const float ys[] = { 304.0f, 352.0f, 400.0f, 448.0f, 496.0f, 544.0f };
        drawLeftRowLabels(labels, ys);
    } else if (playSetupMode == 1) {
        inputP1Name->draw(window);
        drawPortrait(542.0f, 245.0f, p1CharIndex, sf::Color(195, 45, 35));

        inputP2Name->draw(window);
        drawPortrait(542.0f, 289.0f, p2CharIndex, sf::Color(70, 115, 175));

        if (!pvpRollBanner.empty()) {
            sf::Text bText;
            bText.setFont(f);
            bText.setString(toUtf8(pvpRollBanner));
            bText.setCharacterSize(14);
            bText.setStyle(sf::Text::Bold);
            bText.setFillColor(sf::Color(255, 215, 115));
            sf::FloatRect btb = bText.getLocalBounds();
            bText.setOrigin(std::round(btb.left + btb.width / 2.0f), std::round(btb.top + btb.height / 2.0f));
            bText.setPosition(390.0f, 386.0f);
            window.draw(bText);
        }

        const std::vector<std::string> labels = {
            tiengViet ? "Kiểu quân cờ:" : "Piece Style:",
            tiengViet ? "Màu bàn cờ:" : "Board Theme:",
            tiengViet ? "Thời gian ván:" : "Match Timer:",
            tiengViet ? "Ấn ký Tướng:" : "King Seal:"
        };
        const float ys[] = { 406.0f, 452.0f, 498.0f, 544.0f };
        drawLeftRowLabels(labels, ys);
    } else {
        sf::RectangleShape demoInfoBox(sf::Vector2f(372.0f, 134.0f));
        demoInfoBox.setPosition(204.0f, 254.0f);
        demoInfoBox.setFillColor(sf::Color(36, 24, 18, 220));
        demoInfoBox.setOutlineThickness(1.5f);
        demoInfoBox.setOutlineColor(sf::Color(165, 125, 62));
        window.draw(demoInfoBox);

        sf::Text demoDesc;
        demoDesc.setFont(f);
        demoDesc.setCharacterSize(15);
        demoDesc.setStyle(sf::Text::Bold);
        demoDesc.setFillColor(sf::Color(235, 218, 185));
        demoDesc.setString(toUtf8(tiengViet
            ? "Mô phỏng ván cờ tàn tự động:\n"
              "- Quân Đỏ: AI Minimax (2 Xe, 2 Voi)\n"
              "- Quân Đen: AI Alpha-Beta (2 Xe, 2 Voi)\n"
              "- Quan sát chiến thuật tàn cuộc kinh điển"
            : "Automatic Endgame AI Simulation:\n"
              "- Red Side: Minimax AI (2R, 2E)\n"
              "- Black Side: Alpha-Beta AI (2R, 2E)\n"
              "- Watch classic endgame tactics unfold"
        ));
        demoDesc.setPosition(218.0f, 266.0f);
        window.draw(demoDesc);

        const std::vector<std::string> labels = {
            tiengViet ? "Kiểu quân cờ:" : "Piece Style:",
            tiengViet ? "Màu bàn cờ:" : "Board Theme:"
        };
        const float ys[] = { 416.0f, 468.0f };
        drawLeftRowLabels(labels, ys);

        sf::Text tipNote;
        tipNote.setFont(f);
        tipNote.setCharacterSize(14);
        tipNote.setStyle(sf::Text::Bold);
        tipNote.setFillColor(sf::Color(200, 178, 140));
        tipNote.setString(toUtf8(tiengViet
            ? "Nhấn ESC bất kỳ lúc nào để tạm dừng hoặc thoát."
            : "Press ESC anytime during demo to pause or exit."));
        tipNote.setPosition(204.0f, 536.0f);
        window.draw(tipNote);
    }

    // --- Right Column: Aged Xuan Paper Live Preview & Mode Summary ---
    std::string rightTitleStr;
    std::string rightSubStr;
    if (playSetupMode == 0) {
        rightTitleStr = tiengViet ? "ĐẤU VỚI MÁY (PvAI)" : "PLAYER VS AI";
        rightSubStr = tiengViet ? "Thách thức trí tuệ Minimax & Alpha-Beta" : "Challenge Minimax & Alpha-Beta AI";
    } else if (playSetupMode == 1) {
        rightTitleStr = tiengViet ? "HAI NGƯỜI CHƠI (PvP)" : "TWO PLAYERS (PvP)";
        rightSubStr = tiengViet ? "Đối kháng trực tiếp trên cùng bàn cờ" : "Head-to-head classic board match";
    } else {
        rightTitleStr = tiengViet ? "MÁY ĐẤU DEMO" : "BOT VS BOT DEMO";
        rightSubStr = tiengViet ? "Thế cờ tàn song Xe song Voi tự động" : "Automated Double-Chariot Endgame";
    }

    drawShadowHeader(rightTitleStr, 810.0f, 224.0f, 21, sf::Color(175, 25, 20));

    sf::Text rSub;
    rSub.setFont(fBold);
    rSub.setString(toUtf8(rightSubStr));
    rSub.setCharacterSize(16);
    rSub.setStyle(sf::Text::Bold);
    rSub.setFillColor(sf::Color(35, 18, 10));
    sf::FloatRect rsb = rSub.getLocalBounds();
    rSub.setOrigin(std::round(rsb.left + rsb.width / 2.0f), std::round(rsb.top + rsb.height / 2.0f));
    rSub.setPosition(810.0f, 252.0f);
    window.draw(rSub);

    sf::RectangleShape rDiv(sf::Vector2f(336.0f, 2.0f));
    rDiv.setPosition(642.0f, 270.0f);
    rDiv.setFillColor(sf::Color(135, 92, 42));
    window.draw(rDiv);

    // Live Piece Style Preview (Clean Aged Xuan inset)
    vePieceStylePreview(window, 628.0f, 286.0f, 364.0f, 70.0f);

    // Live Board Theme Mini Swatch + Bold Dark Ink Summary Box
    sf::RectangleShape summaryBox(sf::Vector2f(364.0f, 214.0f));
    summaryBox.setPosition(628.0f, 368.0f);
    summaryBox.setFillColor(sf::Color(225, 208, 176));
    summaryBox.setOutlineThickness(1.5f);
    summaryBox.setOutlineColor(sf::Color(65, 38, 20));
    window.draw(summaryBox);

    // Miniature 4-line Board Swatch on left of Summary Box
    sf::Color swBg(220, 185, 135), swLine(70, 40, 20);
    if (boardTheme == BoardTheme::IMPERIAL_JADE) { swBg = sf::Color(168, 194, 162); swLine = sf::Color(35, 58, 42); }
    else if (boardTheme == BoardTheme::MIDNIGHT_INK) { swBg = sf::Color(34, 40, 52); swLine = sf::Color(125, 150, 182); }
    else if (boardTheme == BoardTheme::WARM_BAMBOO) { swBg = sf::Color(210, 175, 125); swLine = sf::Color(68, 38, 18); }

    sf::RectangleShape swatch(sf::Vector2f(104.0f, 114.0f));
    swatch.setPosition(644.0f, 412.0f);
    swatch.setFillColor(swBg);
    swatch.setOutlineThickness(2.0f);
    swatch.setOutlineColor(sf::Color(45, 25, 15));
    window.draw(swatch);

    for (int i = 0; i < 4; ++i) {
        sf::RectangleShape hL(sf::Vector2f(84.0f, 1.5f));
        hL.setPosition(654.0f, 424.0f + i * 30.0f);
        hL.setFillColor(swLine);
        window.draw(hL);
        sf::RectangleShape vL(sf::Vector2f(1.5f, 90.0f));
        vL.setPosition(654.0f + i * 28.0f, 424.0f);
        vL.setFillColor(swLine);
        window.draw(vL);
    }

    sf::Text swTitle;
    swTitle.setFont(fBold);
    swTitle.setString(toUtf8(tiengViet ? "Tổng Quan Trận Đấu" : "Match Summary"));
    swTitle.setCharacterSize(18);
    swTitle.setStyle(sf::Text::Bold);
    swTitle.setFillColor(sf::Color(175, 25, 20));
    swTitle.setPosition(644.0f, 380.0f);
    window.draw(swTitle);

    std::string modeSummaryText;
    std::string timerSummary = (thoiGianVanDau == 0.0f)
        ? (tiengViet ? "Không giới hạn" : "Unlimited")
        : (std::to_string(static_cast<int>(thoiGianVanDau / 60.0f)) + (tiengViet ? " phút/bên" : " min/side"));
    if (playSetupMode == 0) {
        std::string diffName = (aiDifficulty == DoKho::DE) ? (tiengViet ? "Dễ" : "Easy")
            : ((aiDifficulty == DoKho::TRUNG_BINH) ? (tiengViet ? "Trung Bình" : "Medium") : (tiengViet ? "Khó" : "Hard"));
        modeSummaryText = (tiengViet ? "Chế độ: Đấu AI\nĐộ khó: " : "Mode: Vs AI\nLevel: ") + diffName
            + (tiengViet ? "\nĐồng hồ: " : "\nClock: ") + timerSummary
            + (tiengViet ? "\nLuật: Tướng đối mặt cấm" : "\nRules: No flying general");
    } else if (playSetupMode == 1) {
        std::string firstStr = (pvpFirstPlayer == 1)
            ? (tiengViet ? "Kỳ thủ 1 Đỏ" : "P1 Red First")
            : (tiengViet ? "Kỳ thủ 2 Đỏ" : "P2 Red First");
        modeSummaryText = (tiengViet ? "Chế độ: 2 Người\nĐi trước: " : "Mode: 2 Players\nFirst: ") + firstStr
            + (tiengViet ? "\nĐồng hồ: " : "\nClock: ") + timerSummary
            + (tiengViet ? "\nHoàn tác & Gợi ý mở" : "\nUndo & Hints enabled");
    } else {
        modeSummaryText = tiengViet
            ? "Chế độ: Máy vs Máy\nĐỏ: Minimax\nĐen: Alpha-Beta\nTự động biểu diễn"
            : "Mode: Bot vs Bot\nRed: Minimax\nBlack: Alpha-Beta\nAuto simulation";
    }

    sf::Text sumBody;
    sumBody.setFont(fBold);
    sumBody.setString(toUtf8(modeSummaryText));
    sumBody.setCharacterSize(16);
    sumBody.setStyle(sf::Text::Bold);
    sumBody.setFillColor(sf::Color(35, 18, 10));
    sumBody.setLineSpacing(1.25f);
    sumBody.setPosition(762.0f, 412.0f);
    window.draw(sumBody);

    // 4. Bottom Action Bar Gold Frame around primary Start Match CTA + Interactive Buttons
    ve4GocLuaChon(window, sf::Vector2f(460.0f, 612.0f), sf::Vector2f(280.0f, 50.0f), 2.0f, 2.0f);

    for (auto& btn : playSetupButtons) {
        btn->draw(window);
    }

    // Minimalist Footer Hint Strip over dark backdrop (Y=716)
    sf::Text footer;
    footer.setFont(fBold);
    footer.setString(toUtf8(tiengViet
        ? "[W/S] Đổi chế độ  |  [A/D] Thời gian  |  [Enter] Bắt đầu  |  [ESC] Quay lại"
        : "[W/S] Switch Mode  |  [A/D] Timer  |  [Enter] Start  |  [ESC] Back"));
    footer.setCharacterSize(15);
    footer.setStyle(sf::Text::Bold);
    footer.setFillColor(sf::Color(235, 215, 175));
    sf::FloatRect fb = footer.getLocalBounds();
    footer.setOrigin(std::round(fb.left + fb.width / 2.0f), std::round(fb.top + fb.height / 2.0f));
    footer.setPosition(600.0f, 718.0f);
    window.draw(footer);
}

void GameManager::xuLyClickPlaySetup(const sf::Vector2i& viTri) {
    // 1. Text Inputs
    if (playSetupMode == 0) {
        inputAiPlayerName->handleClick(viTri);
    } else if (playSetupMode == 1) {
        inputP1Name->handleClick(viTri);
        inputP2Name->handleClick(viTri);
    }

    // 2. Dashboard Tabs, Selectors & Bottom Action Buttons
    for (size_t i = 0; i < playSetupButtons.size(); ++i) {
        if (playSetupButtons[i]->contains(viTri)) {
            playSetupButtons[i]->handleClick(viTri);
            break;
        }
    }
}

void GameManager::vePvpSetup() {
    sf::Text title;
    title.setFont(font);
    title.setString(Loc::get(LocKey::SETUP_PVP_TITLE));
    title.setCharacterSize(44);
    title.setFillColor(sf::Color(240, 190, 70));
    title.setStyle(sf::Text::Bold);
    
    sf::FloatRect tb = title.getLocalBounds();
    title.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
    title.setPosition(1200.0f / 2.0f, 60.0f);
    window.draw(title);

    // Left Column: Player 1 Card
    FantasyUI::get().drawPanel(window, sf::FloatRect(100.0f, 115.0f, 440.0f, 255.0f), FantasyPanelStyle::ORNATE_WOOD);

    sf::Text p1Label;
    p1Label.setFont(font);
    p1Label.setCharacterSize(22);
    p1Label.setStyle(sf::Text::Bold);
    p1Label.setString(Loc::get(LocKey::P1_LABEL));
    p1Label.setFillColor(sf::Color(255, 140, 140));
    p1Label.setPosition(150.0f, 130.0f);
    window.draw(p1Label);

    sf::Text p1NamePrompt;
    p1NamePrompt.setFont(font);
    p1NamePrompt.setCharacterSize(15);
    p1NamePrompt.setString(Loc::get(LocKey::ENTER_NAME_P1));
    p1NamePrompt.setFillColor(sf::Color(180, 195, 215));
    p1NamePrompt.setPosition(150.0f, 165.0f);
    window.draw(p1NamePrompt);

    inputP1Name->draw(window);

    sf::Text p1CharPrompt;
    p1CharPrompt.setFont(font);
    p1CharPrompt.setCharacterSize(15);
    p1CharPrompt.setString((Loc::getLanguage() == Language::TIENG_VIET) ? "Nhan vat & Hao quang quan co:" : "Character & Piece Aura:");
    p1CharPrompt.setFillColor(sf::Color(180, 195, 215));
    p1CharPrompt.setPosition(150.0f, 240.0f);
    window.draw(p1CharPrompt);

    // Live King Piece Preview for P1
    sf::Vector2f p1KingPos(465.0f, 300.0f);
    float previewR = 25.0f;
    veAuraQuan(p1KingPos, previewR, p1Aura, true);
    
    const sf::Texture* p1Tex = (pieceStyle == PieceStyle::REALISTIC_WOOD) ? layTextureQuan("Tuong", Mau::DO) : nullptr;
    if (p1Tex != nullptr) {
        sf::CircleShape shadow(previewR - 1.0f);
        shadow.setOrigin(previewR - 1.0f, previewR - 1.0f);
        shadow.setPosition(p1KingPos.x + 2.0f, p1KingPos.y + 2.5f);
        shadow.setFillColor(sf::Color(0, 0, 0, 110));
        window.draw(shadow);

        sf::Sprite sp(*p1Tex);
        sf::Vector2u sz = p1Tex->getSize();
        sp.setOrigin(sz.x / 2.0f, sz.y / 2.0f);
        float scale = (previewR * 2.0f) / static_cast<float>(sz.x);
        sp.setScale(scale, scale);
        sp.setPosition(p1KingPos);
        window.draw(sp);
    } else {
        sf::CircleShape disc(previewR);
        disc.setOrigin(previewR, previewR);
        disc.setPosition(p1KingPos);
        disc.setFillColor(sf::Color(235, 205, 155));
        disc.setOutlineThickness(2.2f);
        disc.setOutlineColor(sf::Color(190, 30, 30));
        window.draw(disc);

        sf::Text txt;
        txt.setFont(font);
        txt.setCharacterSize(17);
        txt.setStyle(sf::Text::Bold);
        txt.setString(layKyHieuQuanTheoStyle("Tuong", Mau::DO));
        txt.setFillColor(sf::Color(190, 30, 30));
        sf::FloatRect b = txt.getLocalBounds();
        txt.setOrigin(b.left + b.width / 2.0f, b.top + b.height / 2.0f);
        txt.setPosition(p1KingPos);
        window.draw(txt);
    }

    sf::Text p1PrevLabel;
    p1PrevLabel.setFont(font);
    p1PrevLabel.setCharacterSize(11);
    p1PrevLabel.setStyle(sf::Text::Bold);
    p1PrevLabel.setString("AURA PREVIEW");
    p1PrevLabel.setFillColor(sf::Color(255, 215, 0));
    sf::FloatRect plbnd = p1PrevLabel.getLocalBounds();
    p1PrevLabel.setOrigin(plbnd.left + plbnd.width / 2.0f, plbnd.top + plbnd.height / 2.0f);
    p1PrevLabel.setPosition(p1KingPos.x, p1KingPos.y + previewR + 12.0f);
    window.draw(p1PrevLabel);

    // Right Column: Player 2 Card
    FantasyUI::get().drawPanel(window, sf::FloatRect(660.0f, 115.0f, 440.0f, 255.0f), FantasyPanelStyle::ORNATE_WOOD);

    sf::Text p2Label;
    p2Label.setFont(font);
    p2Label.setCharacterSize(22);
    p2Label.setStyle(sf::Text::Bold);
    p2Label.setString(Loc::get(LocKey::P2_LABEL));
    p2Label.setFillColor(sf::Color(140, 200, 255));
    p2Label.setPosition(710.0f, 130.0f);
    window.draw(p2Label);

    sf::Text p2NamePrompt;
    p2NamePrompt.setFont(font);
    p2NamePrompt.setCharacterSize(15);
    p2NamePrompt.setString(Loc::get(LocKey::ENTER_NAME_P2));
    p2NamePrompt.setFillColor(sf::Color(180, 195, 215));
    p2NamePrompt.setPosition(710.0f, 165.0f);
    window.draw(p2NamePrompt);

    inputP2Name->draw(window);

    sf::Text p2CharPrompt;
    p2CharPrompt.setFont(font);
    p2CharPrompt.setCharacterSize(15);
    p2CharPrompt.setString((Loc::getLanguage() == Language::TIENG_VIET) ? "Nhan vat & Hao quang quan co:" : "Character & Piece Aura:");
    p2CharPrompt.setFillColor(sf::Color(180, 195, 215));
    p2CharPrompt.setPosition(710.0f, 240.0f);
    window.draw(p2CharPrompt);

    // Live King Piece Preview for P2
    sf::Vector2f p2KingPos(1025.0f, 300.0f);
    veAuraQuan(p2KingPos, previewR, p2Aura, true);
    
    const sf::Texture* p2Tex = (pieceStyle == PieceStyle::REALISTIC_WOOD) ? layTextureQuan("Tuong", Mau::DEN) : nullptr;
    if (p2Tex != nullptr) {
        sf::CircleShape shadow(previewR - 1.0f);
        shadow.setOrigin(previewR - 1.0f, previewR - 1.0f);
        shadow.setPosition(p2KingPos.x + 2.0f, p2KingPos.y + 2.5f);
        shadow.setFillColor(sf::Color(0, 0, 0, 110));
        window.draw(shadow);

        sf::Sprite sp(*p2Tex);
        sf::Vector2u sz = p2Tex->getSize();
        sp.setOrigin(sz.x / 2.0f, sz.y / 2.0f);
        float scale = (previewR * 2.0f) / static_cast<float>(sz.x);
        sp.setScale(scale, scale);
        sp.setPosition(p2KingPos);
        window.draw(sp);
    } else {
        sf::CircleShape disc(previewR);
        disc.setOrigin(previewR, previewR);
        disc.setPosition(p2KingPos);
        disc.setFillColor(sf::Color(235, 205, 155));
        disc.setOutlineThickness(2.2f);
        disc.setOutlineColor(sf::Color(30, 30, 30));
        window.draw(disc);

        sf::Text txt;
        txt.setFont(font);
        txt.setCharacterSize(17);
        txt.setStyle(sf::Text::Bold);
        txt.setString(layKyHieuQuanTheoStyle("Tuong", Mau::DEN));
        txt.setFillColor(sf::Color(30, 30, 30));
        sf::FloatRect b = txt.getLocalBounds();
        txt.setOrigin(b.left + b.width / 2.0f, b.top + b.height / 2.0f);
        txt.setPosition(p2KingPos);
        window.draw(txt);
    }

    sf::Text p2PrevLabel;
    p2PrevLabel.setFont(font);
    p2PrevLabel.setCharacterSize(11);
    p2PrevLabel.setStyle(sf::Text::Bold);
    p2PrevLabel.setString("AURA PREVIEW");
    p2PrevLabel.setFillColor(sf::Color(140, 200, 255));
    sf::FloatRect p2lbnd = p2PrevLabel.getLocalBounds();
    p2PrevLabel.setOrigin(p2lbnd.left + p2lbnd.width / 2.0f, p2lbnd.top + p2lbnd.height / 2.0f);
    p2PrevLabel.setPosition(p2KingPos.x, p2KingPos.y + previewR + 12.0f);
    window.draw(p2PrevLabel);

    // Match controls
    sf::RectangleShape rollCard(sf::Vector2f(600.0f, 230.0f));
    rollCard.setPosition(300.0f, 390.0f);
    rollCard.setFillColor(sf::Color(22, 28, 40));
    rollCard.setOutlineThickness(1.5f);
    rollCard.setOutlineColor(sf::Color(65, 80, 110));
    window.draw(rollCard);

    sf::Text bannerText;
    bannerText.setFont(font);
    bannerText.setCharacterSize(16);
    bannerText.setStyle(sf::Text::Bold);
    if (!pvpRollBanner.empty()) {
        bannerText.setString(pvpRollBanner);
        bannerText.setFillColor(sf::Color(255, 220, 100));
    } else {
        std::string def = (Loc::getLanguage() == Language::TIENG_VIET)
            ? "(Nhan nut tren de tung dong xu ngau nhien chon nguoi cam quan DO di truoc)"
            : "(Click roll button to randomly select who plays RED and moves first)";
        bannerText.setString(def);
        bannerText.setFillColor(sf::Color(160, 175, 195));
    }
    tb = bannerText.getLocalBounds();
    bannerText.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
    bannerText.setPosition(1200.0f / 2.0f, 545.0f);
    window.draw(bannerText);

    menuPvpSetup.draw(window);
}

void GameManager::vePvaiSetup() {
    sf::Text title;
    title.setFont(font);
    title.setString(Loc::get(LocKey::SETUP_PVAI_TITLE));
    title.setCharacterSize(44);
    title.setFillColor(sf::Color(240, 190, 70));
    title.setStyle(sf::Text::Bold);
    
    sf::FloatRect tb = title.getLocalBounds();
    title.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
    title.setPosition(1200.0f / 2.0f, 60.0f);
    window.draw(title);

    // Left Column: Player
    FantasyUI::get().drawPanel(window, sf::FloatRect(100.0f, 115.0f, 440.0f, 255.0f), FantasyPanelStyle::ORNATE_WOOD);

    sf::Text pLabel;
    pLabel.setFont(font);
    pLabel.setCharacterSize(22);
    pLabel.setStyle(sf::Text::Bold);
    pLabel.setString(Loc::get(LocKey::PLAYER_LABEL));
    pLabel.setFillColor(sf::Color(255, 215, 120));
    pLabel.setPosition(150.0f, 130.0f);
    window.draw(pLabel);

    sf::Text pNamePrompt;
    pNamePrompt.setFont(font);
    pNamePrompt.setCharacterSize(15);
    pNamePrompt.setString(Loc::get(LocKey::ENTER_NAME_PLAYER));
    pNamePrompt.setFillColor(sf::Color(180, 195, 215));
    pNamePrompt.setPosition(150.0f, 165.0f);
    window.draw(pNamePrompt);

    inputAiPlayerName->draw(window);

    sf::Text pCharPrompt;
    pCharPrompt.setFont(font);
    pCharPrompt.setCharacterSize(15);
    pCharPrompt.setString((Loc::getLanguage() == Language::TIENG_VIET) ? "Nhan vat & Hao quang quan co:" : "Character & Piece Aura:");
    pCharPrompt.setFillColor(sf::Color(180, 195, 215));
    pCharPrompt.setPosition(150.0f, 240.0f);
    window.draw(pCharPrompt);

    // Live King Piece Preview for Player
    sf::Vector2f pKingPos(465.0f, 300.0f);
    float previewR = 25.0f;
    veAuraQuan(pKingPos, previewR, p1Aura, true);
    
    const sf::Texture* pTex = (pieceStyle == PieceStyle::REALISTIC_WOOD) ? layTextureQuan("Tuong", Mau::DO) : nullptr;
    if (pTex != nullptr) {
        sf::CircleShape shadow(previewR - 1.0f);
        shadow.setOrigin(previewR - 1.0f, previewR - 1.0f);
        shadow.setPosition(pKingPos.x + 2.0f, pKingPos.y + 2.5f);
        shadow.setFillColor(sf::Color(0, 0, 0, 110));
        window.draw(shadow);

        sf::Sprite sp(*pTex);
        sf::Vector2u sz = pTex->getSize();
        sp.setOrigin(sz.x / 2.0f, sz.y / 2.0f);
        float scale = (previewR * 2.0f) / static_cast<float>(sz.x);
        sp.setScale(scale, scale);
        sp.setPosition(pKingPos);
        window.draw(sp);
    } else {
        sf::CircleShape disc(previewR);
        disc.setOrigin(previewR, previewR);
        disc.setPosition(pKingPos);
        disc.setFillColor(sf::Color(235, 205, 155));
        disc.setOutlineThickness(2.2f);
        disc.setOutlineColor(sf::Color(190, 30, 30));
        window.draw(disc);

        sf::Text txt;
        txt.setFont(font);
        txt.setCharacterSize(17);
        txt.setStyle(sf::Text::Bold);
        txt.setString(layKyHieuQuanTheoStyle("Tuong", Mau::DO));
        txt.setFillColor(sf::Color(190, 30, 30));
        sf::FloatRect b = txt.getLocalBounds();
        txt.setOrigin(b.left + b.width / 2.0f, b.top + b.height / 2.0f);
        txt.setPosition(pKingPos);
        window.draw(txt);
    }

    sf::Text pPrevLabel;
    pPrevLabel.setFont(font);
    pPrevLabel.setCharacterSize(11);
    pPrevLabel.setStyle(sf::Text::Bold);
    pPrevLabel.setString("AURA PREVIEW");
    pPrevLabel.setFillColor(sf::Color(255, 215, 0));
    sf::FloatRect plb = pPrevLabel.getLocalBounds();
    pPrevLabel.setOrigin(plb.left + plb.width / 2.0f, plb.top + plb.height / 2.0f);
    pPrevLabel.setPosition(pKingPos.x, pKingPos.y + previewR + 12.0f);
    window.draw(pPrevLabel);

    // Right Column: AI Engine
    FantasyUI::get().drawPanel(window, sf::FloatRect(660.0f, 115.0f, 440.0f, 255.0f), FantasyPanelStyle::ORNATE_WOOD);

    sf::Text aiLabel;
    aiLabel.setFont(font);
    aiLabel.setCharacterSize(22);
    aiLabel.setStyle(sf::Text::Bold);
    aiLabel.setString(Loc::get(LocKey::AI_LABEL));
    aiLabel.setFillColor(sf::Color(140, 200, 255));
    aiLabel.setPosition(710.0f, 130.0f);
    window.draw(aiLabel);

    sf::Text aiDiffPrompt;
    aiDiffPrompt.setFont(font);
    aiDiffPrompt.setCharacterSize(15);
    aiDiffPrompt.setString((Loc::getLanguage() == Language::TIENG_VIET) ? "Chon cap do thu thach:" : "Select challenge level:");
    aiDiffPrompt.setFillColor(sf::Color(180, 195, 215));
    aiDiffPrompt.setPosition(710.0f, 165.0f);
    window.draw(aiDiffPrompt);

    sf::Text aiAuraPrompt;
    aiAuraPrompt.setFont(font);
    aiAuraPrompt.setCharacterSize(15);
    aiAuraPrompt.setString((Loc::getLanguage() == Language::TIENG_VIET) ? "Hao quang cua May AI:" : "AI Piece Aura:");
    aiAuraPrompt.setFillColor(sf::Color(180, 195, 215));
    aiAuraPrompt.setPosition(710.0f, 245.0f);
    window.draw(aiAuraPrompt);

    // AI King Piece Preview
    sf::Vector2f aiKingPos(1025.0f, 290.0f);
    veAuraQuan(aiKingPos, previewR, p2Aura, true);
    
    const sf::Texture* aiTex = (pieceStyle == PieceStyle::REALISTIC_WOOD) ? layTextureQuan("Tuong", Mau::DEN) : nullptr;
    if (aiTex != nullptr) {
        sf::CircleShape shadow(previewR - 1.0f);
        shadow.setOrigin(previewR - 1.0f, previewR - 1.0f);
        shadow.setPosition(aiKingPos.x + 2.0f, aiKingPos.y + 2.5f);
        shadow.setFillColor(sf::Color(0, 0, 0, 110));
        window.draw(shadow);

        sf::Sprite sp(*aiTex);
        sf::Vector2u sz = aiTex->getSize();
        sp.setOrigin(sz.x / 2.0f, sz.y / 2.0f);
        float scale = (previewR * 2.0f) / static_cast<float>(sz.x);
        sp.setScale(scale, scale);
        sp.setPosition(aiKingPos);
        window.draw(sp);
    } else {
        sf::CircleShape disc(previewR);
        disc.setOrigin(previewR, previewR);
        disc.setPosition(aiKingPos);
        disc.setFillColor(sf::Color(235, 205, 155));
        disc.setOutlineThickness(2.2f);
        disc.setOutlineColor(sf::Color(30, 30, 30));
        window.draw(disc);

        sf::Text txt;
        txt.setFont(font);
        txt.setCharacterSize(17);
        txt.setStyle(sf::Text::Bold);
        txt.setString(layKyHieuQuanTheoStyle("Tuong", Mau::DEN));
        txt.setFillColor(sf::Color(30, 30, 30));
        sf::FloatRect b = txt.getLocalBounds();
        txt.setOrigin(b.left + b.width / 2.0f, b.top + b.height / 2.0f);
        txt.setPosition(aiKingPos);
        window.draw(txt);
    }

    sf::Text aiPrevLabel;
    aiPrevLabel.setFont(font);
    aiPrevLabel.setCharacterSize(11);
    aiPrevLabel.setStyle(sf::Text::Bold);
    aiPrevLabel.setString("AI AURA");
    aiPrevLabel.setFillColor(sf::Color(140, 200, 255));
    sf::FloatRect albnd = aiPrevLabel.getLocalBounds();
    aiPrevLabel.setOrigin(albnd.left + albnd.width / 2.0f, albnd.top + albnd.height / 2.0f);
    aiPrevLabel.setPosition(aiKingPos.x, aiKingPos.y + previewR + 12.0f);
    window.draw(aiPrevLabel);

    sf::Text aiDesc;
    aiDesc.setFont(font);
    aiDesc.setCharacterSize(13);
    std::string aiDescStr = (aiDifficulty == DoKho::DE) 
        ? ((Loc::getLanguage() == Language::TIENG_VIET) ? "Cap do De: Nuoc di co ban, thich hop luyen tap." : "Easy mode: Basic play, great for training.")
        : ((aiDifficulty == DoKho::TRUNG_BINH) 
            ? ((Loc::getLanguage() == Language::TIENG_VIET) ? "Cap do Vua: Minimax 2 lop + Danh gia the tran." : "Medium mode: Minimax 2-ply + Position evaluation.")
            : ((Loc::getLanguage() == Language::TIENG_VIET) ? "Cap do Kho: Alpha-Beta 4 lop + Sap xep nuoc di." : "Hard mode: Alpha-Beta 4-ply + Move ordering."));
    aiDesc.setString(aiDescStr);
    aiDesc.setFillColor(sf::Color(160, 180, 205));
    aiDesc.setPosition(710.0f, 335.0f);
    window.draw(aiDesc);

    // Center Match Controls Card
    sf::RectangleShape rollCard(sf::Vector2f(600.0f, 245.0f));
    rollCard.setPosition(300.0f, 375.0f);
    rollCard.setFillColor(sf::Color(22, 28, 40));
    rollCard.setOutlineThickness(1.5f);
    rollCard.setOutlineColor(sf::Color(65, 80, 110));
    window.draw(rollCard);

    sf::Text bannerText;
    bannerText.setFont(font);
    bannerText.setCharacterSize(16);
    bannerText.setStyle(sf::Text::Bold);
    if (!aiRollBanner.empty()) {
        bannerText.setString(aiRollBanner);
        bannerText.setFillColor(sf::Color(255, 220, 100));
    } else {
        std::string def = (Loc::getLanguage() == Language::TIENG_VIET)
            ? "(Chon ai di truoc hoac nhan nut tung dong xu ngau nhien)"
            : "(Select first move or click to roll random coin toss)";
        bannerText.setString(def);
        bannerText.setFillColor(sf::Color(160, 175, 195));
    }
    tb = bannerText.getLocalBounds();
    bannerText.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
    bannerText.setPosition(1200.0f / 2.0f, 575.0f);
    window.draw(bannerText);

    menuPvaiSetup.draw(window);
}

void GameManager::veLoadMenu() {
    FantasyUI::get().drawFullDimOverlay(window, sf::Color(10, 8, 12, 75));

    FantasyUI::get().drawRibbonTitle(window, 600.0f, 38.0f, 360.0f, 56.0f,
        Loc::get(LocKey::LOAD_TITLE), fontBold, 20);

    FantasyUI::get().drawPanel(window, sf::FloatRect(126.0f, 134.0f, 948.0f, 466.0f), OrientalPanelStyle::EBONY_WOOD);

    for (int i = 0; i < 3; ++i) {
        float sy = 160.0f + i * 145.0f;
        sf::FloatRect cardRect(150.0f, sy, 900.0f, 125.0f);
        bool isSel = (selectedSlot == i);

        FantasyUI::get().drawPanel(window, cardRect,
            isSel ? OrientalPanelStyle::PARCHMENT_SCROLL : OrientalPanelStyle::EBONY_WOOD);
        if (isSel) {
            ve4GocLuaChon(window, sf::Vector2f(150.0f, sy), sf::Vector2f(900.0f, 125.0f), 2.0f, 2.0f);
        }

        sf::Text slotHeader;
        slotHeader.setFont(fontBold);
        slotHeader.setCharacterSize(21);
        slotHeader.setStyle(sf::Text::Bold);
        slotHeader.setString(toUtf8(Loc::get(LocKey::SLOT_LABEL) + std::to_string(i + 1)));
        slotHeader.setFillColor(isSel ? sf::Color(175, 25, 20) : sf::Color(235, 198, 118));
        slotHeader.setPosition(180.0f, sy + 18.0f);
        window.draw(slotHeader);

        const SaveSlotInfo& info = slotInfos[i];
        if (info.tonTai) {
            sf::Text det1;
            det1.setFont(font);
            det1.setCharacterSize(16);
            det1.setStyle(sf::Text::Bold);
            std::string d1 = "Mode: " + info.tenCheDo + "  |  Date: " + info.thoiGian;
            det1.setString(toUtf8(d1));
            det1.setFillColor(isSel ? sf::Color(35, 18, 10) : sf::Color(230, 212, 178));
            det1.setPosition(180.0f, sy + 54.0f);
            window.draw(det1);

            sf::Text det2;
            det2.setFont(font);
            det2.setCharacterSize(16);
            det2.setStyle(sf::Text::Bold);
            std::string d2 = "Red: " + info.tenDo + "  vs  Black: " + info.tenDen +
                             "  |  Moves: " + std::to_string(info.soNuoc) +
                             "  |  Turn: " + ((info.luot == Mau::DO) ? "RED" : "BLACK");
            det2.setString(toUtf8(d2));
            det2.setFillColor(isSel ? sf::Color(70, 40, 20) : sf::Color(195, 175, 140));
            det2.setPosition(180.0f, sy + 82.0f);
            window.draw(det2);
        } else {
            sf::Text emp;
            emp.setFont(font);
            emp.setCharacterSize(17);
            emp.setStyle(sf::Text::Bold);
            emp.setString(toUtf8(Loc::get(LocKey::EMPTY_SLOT)));
            emp.setFillColor(isSel ? sf::Color(110, 75, 45) : sf::Color(165, 148, 120));
            emp.setPosition(180.0f, sy + 60.0f);
            window.draw(emp);
        }
    }

    menuLoad.draw(window);
}

void GameManager::veSettingsMenu() {
    bool tiengViet = (Loc::getLanguage() == Language::TIENG_VIET);

    // 1. Subtle Dim Backdrop + Antique Ribbon Title (1200x800)
    FantasyUI::get().drawFullDimOverlay(window, sf::Color(10, 8, 12, 75));

    FantasyUI::get().drawRibbonTitle(window, 600.0f, 38.0f, 360.0f, 56.0f,
        Loc::get(LocKey::SETTINGS_TITLE), fontBold, 20);

    // 2. Unified Oriental Ebony Wood Dashboard Frame (880x560 at 160, 120)
    FantasyUI::get().drawPanel(window, sf::FloatRect(160.0f, 120.0f, 880.0f, 560.0f), OrientalPanelStyle::EBONY_WOOD);

    // 3. Top Horizontal Jade-Plaque Navigation Tabs (aligned with khoiTaoDashboardSettings)
    for (size_t i = 0; i < settingsTabButtons.size(); ++i) {
        bool isActive = (static_cast<int>(activeSettingsTab) == static_cast<int>(i));
        settingsTabButtons[i]->setSelected(isActive);
        settingsTabButtons[i]->setColors(
            isActive ? sf::Color(255, 226, 128) : sf::Color(242, 232, 206),
            sf::Color(255, 234, 150),
            sf::Color(255, 250, 224)
        );
        settingsTabButtons[i]->draw(window);
        if (isActive) {
            ve4GocLuaChon(window, settingsTabButtons[i]->getPosition(), settingsTabButtons[i]->getSize(), 1.5f, 1.5f);
        }
    }

    // 4. Symmetrical Inner Split Columns: Left Dark Ebony Controls & Right Clean Aged Xuan Paper
    FantasyUI::get().drawPanel(window, sf::FloatRect(188.0f, 198.0f, 404.0f, 402.0f), OrientalPanelStyle::EBONY_WOOD);
    sf::RectangleShape leftInset(sf::Vector2f(388.0f, 386.0f));
    leftInset.setPosition(196.0f, 206.0f);
    leftInset.setFillColor(sf::Color(24, 16, 12, 195));
    leftInset.setOutlineThickness(1.0f);
    leftInset.setOutlineColor(sf::Color(150, 112, 54, 145));
    window.draw(leftInset);

    FantasyUI::get().drawPanel(window, sf::FloatRect(608.0f, 198.0f, 404.0f, 402.0f), OrientalPanelStyle::PARCHMENT_SCROLL);

    auto drawXuanHeader = [&](const std::string& headerText) {
        sf::Text sh;
        sh.setFont(fontBold);
        sh.setString(toUtf8(headerText));
        sh.setCharacterSize(20);
        sh.setStyle(sf::Text::Bold);
        sh.setFillColor(sf::Color(0, 0, 0, 100));
        sf::FloatRect b = sh.getLocalBounds();
        sh.setOrigin(std::round(b.left + b.width / 2.0f), std::round(b.top + b.height / 2.0f));
        sh.setPosition(811.0f, 225.0f);
        window.draw(sh);

        sf::Text tx = sh;
        tx.setFillColor(sf::Color(175, 25, 20));
        tx.setPosition(810.0f, 224.0f);
        window.draw(tx);

        sf::RectangleShape div(sf::Vector2f(336.0f, 2.0f));
        div.setPosition(642.0f, 246.0f);
        div.setFillColor(sf::Color(135, 92, 42));
        window.draw(div);
    };

    const sf::Color inkDark(35, 18, 10);

    // 5. Tab Specific Content
    if (activeSettingsTab == SettingsTab::AUDIO) {
        if (sliderMaster) sliderMaster->draw(window);
        if (settingsAudioButtons.size() >= 2) settingsAudioButtons[0]->draw(window);
        if (sliderSfx) sliderSfx->draw(window);
        if (settingsAudioButtons.size() >= 2) settingsAudioButtons[1]->draw(window);
        if (sliderBgm) sliderBgm->draw(window);

        sf::Text sbTitle;
        sbTitle.setFont(fontBold);
        sbTitle.setCharacterSize(14);
        sbTitle.setStyle(sf::Text::Bold);
        sbTitle.setString(toUtf8(tiengViet ? "THỬ NGHIỆM ÂM THANH:" : "SOUNDBOARD TEST:"));
        sbTitle.setFillColor(sf::Color(235, 198, 118));
        sbTitle.setPosition(206.0f, 496.0f);
        window.draw(sbTitle);

        for (auto& btn : soundboardButtons) btn->draw(window);

        drawXuanHeader(tiengViet ? "HỆ THỐNG ÂM THANH" : "AUDIO SYSTEM PREVIEW");

        std::string audioDetails = tiengViet
            ? ("- ÂM LƯỢNG TỔNG: " + std::to_string(static_cast<int>(soundManager.getMasterVolume())) + "%\n"
               "- HIỆU ỨNG (SFX): " + std::string(soundManager.getSfxBat() ? "BẬT" : "TẮT") + " (" + std::to_string(static_cast<int>(soundManager.getSfxVolume())) + "%)\n"
               "- NHẠC NỀN (BGM): " + std::string(soundManager.getBgmBat() ? "ĐANG PHÁT" : "TẮT") + " (" + std::to_string(static_cast<int>(soundManager.getBgmVolume())) + "%)\n\n"
               "Âm thanh quân cờ gỗ & nhạc cổ phong\n"
               "tùy chỉnh tức thì theo thanh trượt.\n\n"
               "Nhấn các nút bên trái để nghe thử!")
            : ("- MASTER VOLUME: " + std::to_string(static_cast<int>(soundManager.getMasterVolume())) + "%\n"
               "- SFX SYSTEM: " + std::string(soundManager.getSfxBat() ? "ONLINE" : "MUTED") + " (" + std::to_string(static_cast<int>(soundManager.getSfxVolume())) + "%)\n"
               "- BGM AMBIENT: " + std::string(soundManager.getBgmBat() ? "PLAYING" : "MUTED") + " (" + std::to_string(static_cast<int>(soundManager.getBgmVolume())) + "%)\n\n"
               "High-fidelity wooden piece impacts &\n"
               "Guzheng ambient audio with live sliders.\n\n"
               "Click soundboard buttons on the left to test!");

        sf::Text audioDesc;
        audioDesc.setFont(fontBold);
        audioDesc.setCharacterSize(16);
        audioDesc.setStyle(sf::Text::Bold);
        audioDesc.setLineSpacing(1.22f);
        audioDesc.setString(toUtf8(audioDetails));
        audioDesc.setFillColor(inkDark);
        audioDesc.setPosition(630.0f, 266.0f);
        window.draw(audioDesc);

        // Antique lacquered equalizer visual bars inside Xuan paper footer
        float barStartX = 644.0f;
        float barY = 556.0f;
        for (int b = 0; b < 10; ++b) {
            float bh = 16.0f + (b % 4) * 11.0f;
            sf::RectangleShape eqBar(sf::Vector2f(22.0f, bh));
            eqBar.setPosition(barStartX + b * 34.0f, barY - bh);
            bool lit = (b < static_cast<int>(soundManager.getMasterVolume() / 10.0f));
            eqBar.setFillColor(lit ? sf::Color(175, 42, 30) : sf::Color(195, 178, 148));
            eqBar.setOutlineThickness(1.0f);
            eqBar.setOutlineColor(sf::Color(65, 38, 20));
            window.draw(eqBar);
        }
    } else if (activeSettingsTab == SettingsTab::GRAPHICS) {
        for (auto& btn : settingsGraphicsButtons) btn->draw(window);
        if (sliderAtmosphere) sliderAtmosphere->draw(window);

        drawXuanHeader(tiengViet ? "XEM TRƯỚC GIAO DIỆN" : "LIVE THEME PREVIEW");

        // Mini Preview Board using Task 4 crafted palette
        float prevW = 228.0f;
        float prevH = 118.0f;
        float prevX = 696.0f;
        float prevY = 264.0f;

        sf::Color prevBg(220, 185, 135), prevLine(70, 40, 20);
        if (boardTheme == BoardTheme::IMPERIAL_JADE) { prevBg = sf::Color(168, 194, 162); prevLine = sf::Color(35, 58, 42); }
        else if (boardTheme == BoardTheme::MIDNIGHT_INK) { prevBg = sf::Color(34, 40, 52); prevLine = sf::Color(125, 150, 182); }
        else if (boardTheme == BoardTheme::WARM_BAMBOO) { prevBg = sf::Color(210, 175, 125); prevLine = sf::Color(68, 38, 18); }

        sf::RectangleShape prevBoard(sf::Vector2f(prevW, prevH));
        prevBoard.setPosition(prevX, prevY);
        prevBoard.setFillColor(prevBg);
        prevBoard.setOutlineThickness(2.5f);
        prevBoard.setOutlineColor(sf::Color(45, 25, 15));
        window.draw(prevBoard);

        sf::RectangleShape hCross(sf::Vector2f(prevW - 16.0f, 2.0f));
        hCross.setPosition(prevX + 8.0f, prevY + prevH / 2.0f - 1.0f);
        hCross.setFillColor(prevLine);
        window.draw(hCross);
        sf::RectangleShape vCross(sf::Vector2f(2.0f, prevH - 16.0f));
        vCross.setPosition(prevX + prevW / 2.0f - 1.0f, prevY + 8.0f);
        vCross.setFillColor(prevLine);
        window.draw(vCross);

        auto drawPreviewPiece = [this](float x, float y, const std::string& name, Mau m) {
            float r = 23.0f;
            float px = std::round(x), py = std::round(y);
            const sf::Texture* tex = (pieceStyle == PieceStyle::REALISTIC_WOOD) ? layTextureQuan(name, m) : nullptr;

            sf::CircleShape shadow(r);
            shadow.setOrigin(r, r);
            shadow.setPosition(px + 2.0f, py + 2.5f);
            shadow.setFillColor(sf::Color(10, 6, 4, 115));
            window.draw(shadow);

            if (tex != nullptr) {
                sf::CircleShape rim(r + 1.0f);
                rim.setOrigin(r + 1.0f, r + 1.0f);
                rim.setPosition(px, py);
                rim.setFillColor(sf::Color(26, 14, 8));
                window.draw(rim);

                sf::Sprite sprite(*tex);
                sf::Vector2u sz = tex->getSize();
                sprite.setOrigin(sz.x / 2.0f, sz.y / 2.0f);
                float sc = (r * 2.0f) / static_cast<float>(sz.x);
                sprite.setScale(sc, sc);
                sprite.setPosition(px, py);
                window.draw(sprite);
            } else {
                bool isRed = (m == Mau::DO);
                sf::CircleShape outer(r + 2.0f);
                outer.setOrigin(r + 2.0f, r + 2.0f);
                outer.setPosition(px, py);
                outer.setFillColor(isRed ? sf::Color(135, 26, 18) : sf::Color(24, 30, 40));
                outer.setOutlineThickness(1.0f);
                outer.setOutlineColor(sf::Color(26, 14, 8));
                window.draw(outer);

                sf::CircleShape body(r);
                body.setOrigin(r, r);
                body.setPosition(px, py);
                body.setFillColor(isRed ? sf::Color(248, 236, 216) : sf::Color(232, 240, 236));
                body.setOutlineThickness(1.5f);
                body.setOutlineColor(isRed ? sf::Color(180, 40, 30) : sf::Color(40, 50, 65));
                window.draw(body);

                std::string label = layKyHieuQuanTheoStyle(name, m);
                sf::Text t;
                t.setFont(fontBold);
                t.setString(toUtf8(label));
                t.setCharacterSize((label.size() > 2) ? 14 : 17);
                t.setStyle(sf::Text::Bold);
                t.setFillColor(isRed ? sf::Color(180, 25, 20) : sf::Color(20, 25, 35));
                sf::FloatRect tb = t.getLocalBounds();
                t.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));
                t.setPosition(px, py - 1.0f);
                window.draw(t);
            }
        };

        drawPreviewPiece(prevX + 64.0f, prevY + prevH / 2.0f, "Xe", Mau::DO);
        drawPreviewPiece(prevX + 164.0f, prevY + prevH / 2.0f, "Ma", Mau::DEN);

        std::string gfxDetails = tiengViet
            ? ("- Gợi ý nước đi: " + std::string(hienGoiY ? "BẬT" : "TẮT") + "\n"
               "- Đánh dấu nước cuối: " + std::string(hienNuocDiCuoi ? "BẬT" : "TẮT") + "\n"
               "- Tọa độ bàn cờ: " + std::string(hienToaDo ? "HIỆN (1-9, A-J)" : "ẨN") + "\n"
               "- Chế độ màn hình: " + std::string(fullscreenMode ? "TOÀN MÀN HÌNH" : "CỬA SỔ"))
            : ("- Move Hints: " + std::string(hienGoiY ? "ENABLED" : "DISABLED") + "\n"
               "- Last Move Highlight: " + std::string(hienNuocDiCuoi ? "ENABLED" : "DISABLED") + "\n"
               "- Board Coordinates: " + std::string(hienToaDo ? "1-9, A-J ON" : "HIDDEN") + "\n"
               "- Display Mode: " + std::string(fullscreenMode ? "FULLSCREEN" : "WINDOWED"));

        sf::Text prevDesc;
        prevDesc.setFont(fontBold);
        prevDesc.setCharacterSize(16);
        prevDesc.setStyle(sf::Text::Bold);
        prevDesc.setLineSpacing(1.28f);
        prevDesc.setString(toUtf8(gfxDetails));
        prevDesc.setFillColor(inkDark);
        prevDesc.setPosition(632.0f, 412.0f);
        window.draw(prevDesc);
    } else if (activeSettingsTab == SettingsTab::GAMEPLAY) {
        for (auto& btn : settingsGameplayButtons) btn->draw(window);

        drawXuanHeader(tiengViet ? "LUẬT CHƠI & AI ENGINE" : "GAMEPLAY & AI ENGINE");

        std::string gameDetails = tiengViet
            ? ("- THUẬT TOÁN AI: Minimax & Alpha-Beta\n"
               "  Tích hợp bảng điểm vị trí & sắp xếp nước đi.\n\n"
               "- NHỊP ĐỘ SUY NGHĨ: " + std::string(aiThinkRealistic ? "450ms (Tự nhiên)" : "Tức thì") + "\n\n"
               "- CẢNH BÁO CHIẾU TƯỚNG: " + std::string(checkAlarmSound ? "Âm thanh & Nhấp nháy" : "Yên lặng") + "\n\n"
               "- NGÔN NGỮ: Tiếng Việt\n\n"
               "Mọi thay đổi áp dụng ngay vào ván đấu.")
            : ("- AI ENGINE: Minimax with Alpha-Beta\n"
               "  Integrated Piece-Square Tables & Move Ordering.\n\n"
               "- THINK PACING: " + std::string(aiThinkRealistic ? "450ms (Natural)" : "Instant") + "\n\n"
               "- CHECK ALARM: " + std::string(checkAlarmSound ? "Audio & Visual Flash" : "Silent") + "\n\n"
               "- LANGUAGE: English\n\n"
               "All changes apply seamlessly in-game.");

        sf::Text gameDesc;
        gameDesc.setFont(fontBold);
        gameDesc.setCharacterSize(16);
        gameDesc.setStyle(sf::Text::Bold);
        gameDesc.setLineSpacing(1.22f);
        gameDesc.setString(toUtf8(gameDetails));
        gameDesc.setFillColor(inkDark);
        gameDesc.setPosition(630.0f, 266.0f);
        window.draw(gameDesc);
    } else if (activeSettingsTab == SettingsTab::CONTROLS) {
        for (auto& btn : settingsControlButtons) btn->draw(window);

        drawXuanHeader(tiengViet ? "HƯỚNG DẪN ĐỔI PHÍM" : "KEY REBINDING GUIDE");

        std::string ctrlDetails = tiengViet
            ? ("1. Nhấn vào hành động muốn đổi bên trái.\n"
               "2. Khi hiện '[ Press any key... ]',\n"
               "   hãy nhấn phím bất kỳ để gán mới.\n"
               "3. Nhấn [Esc] để hủy giữ nguyên.\n\n"
               "BỘ PHÍM HIỆN TẠI: " + keyConfig.getPresetName() + "\n\n"
               "ĐIỀU KHIỂN CHUỘT:\n"
               "Luôn hỗ trợ click trực tiếp trên bàn cờ.")
            : ("1. Click on any action on the left.\n"
               "2. When it shows '[ Press any key... ]',\n"
               "   press ANY keyboard key to bind.\n"
               "3. Press [Esc] to cancel without changes.\n\n"
               "CURRENT PRESET: " + keyConfig.getPresetName() + "\n\n"
               "MOUSE NAVIGATION:\n"
               "Direct clicking on board squares is\n"
               "always supported simultaneously.");

        sf::Text ctrlDesc;
        ctrlDesc.setFont(fontBold);
        ctrlDesc.setCharacterSize(16);
        ctrlDesc.setStyle(sf::Text::Bold);
        ctrlDesc.setLineSpacing(1.22f);
        ctrlDesc.setString(toUtf8(ctrlDetails));
        ctrlDesc.setFillColor(inkDark);
        ctrlDesc.setPosition(630.0f, 266.0f);
        window.draw(ctrlDesc);

        if (rebindingAction != KeyAction::COUNT) {
            sf::RectangleShape listeningBox(sf::Vector2f(364.0f, 48.0f));
            listeningBox.setPosition(628.0f, 524.0f);
            listeningBox.setFillColor(sf::Color(175, 25, 20));
            listeningBox.setOutlineThickness(2.0f);
            listeningBox.setOutlineColor(sf::Color(235, 195, 115));
            window.draw(listeningBox);

            sf::Text listenT;
            listenT.setFont(fontBold);
            listenT.setCharacterSize(16);
            listenT.setStyle(sf::Text::Bold);
            listenT.setString(toUtf8(tiengViet ? ">>> ĐANG CHỜ NHẤN PHÍM <<<" : ">>> LISTENING FOR KEYPRESS <<<"));
            listenT.setFillColor(sf::Color(255, 240, 190));
            sf::FloatRect ltb = listenT.getLocalBounds();
            listenT.setOrigin(std::round(ltb.left + ltb.width / 2.0f), std::round(ltb.top + ltb.height / 2.0f));
            listenT.setPosition(810.0f, 548.0f);
            window.draw(listenT);
        }
    }

    // 6. Bottom Dashboard Action Bar (X=240, 490, 740, Y=618, W=220, H=46)
    for (auto& btn : settingsBottomButtons) {
        btn->setColors(sf::Color(246, 236, 206), sf::Color(255, 224, 128), sf::Color(255, 250, 220));
        btn->draw(window);
    }
}

void GameManager::veKeybindingMenu() {
    activeSettingsTab = SettingsTab::CONTROLS;
    veSettingsMenu();
}

void GameManager::veIntroMenu() {
    bool tiengViet = (Loc::getLanguage() == Language::TIENG_VIET);

    FantasyUI::get().drawFullDimOverlay(window, sf::Color(10, 8, 12, 75));

    FantasyUI::get().drawRibbonTitle(window, 600.0f, 38.0f, 380.0f, 56.0f,
        Loc::get(LocKey::INTRO_TITLE), fontBold, 20);

    // Unified 1200x800 Ebony + Clean Aged Xuan Paper Container
    FantasyUI::get().drawPanel(window, sf::FloatRect(120.0f, 116.0f, 960.0f, 554.0f), OrientalPanelStyle::EBONY_WOOD);
    FantasyUI::get().drawPanel(window, sf::FloatRect(146.0f, 140.0f, 908.0f, 506.0f), OrientalPanelStyle::PARCHMENT_SCROLL);

    sf::Text headerShadow;
    headerShadow.setFont(fontBold);
    headerShadow.setCharacterSize(20);
    headerShadow.setStyle(sf::Text::Bold);
    headerShadow.setString(toUtf8(tiengViet ? "CỜ TƯỚNG CỔ PHONG - KIẾN TRÚC OOP & LUẬT CỜ" : "ANCIENT XIANGQI - OOP ARCHITECTURE & RULES"));
    headerShadow.setFillColor(sf::Color(0, 0, 0, 100));
    sf::FloatRect hb = headerShadow.getLocalBounds();
    headerShadow.setOrigin(std::round(hb.left + hb.width / 2.0f), std::round(hb.top + hb.height / 2.0f));
    headerShadow.setPosition(601.0f, 169.0f);
    window.draw(headerShadow);

    sf::Text header = headerShadow;
    header.setFillColor(sf::Color(175, 25, 20));
    header.setPosition(600.0f, 168.0f);
    window.draw(header);

    sf::RectangleShape hDiv(sf::Vector2f(820.0f, 2.0f));
    hDiv.setPosition(190.0f, 190.0f);
    hDiv.setFillColor(sf::Color(135, 92, 42));
    window.draw(hDiv);

    std::string text = tiengViet
        ? "- GIỚI THIỆU ĐỒ ÁN:\n"
          "  Đồ án Lập trình Hướng đối tượng (OOP) - Trường ĐH Khoa học Tự nhiên (HCMUS).\n"
          "  Game Cờ Tướng hiện thực bằng C++17 và thư viện đồ họa SFML.\n\n"
          "- KIẾN TRÚC OOP & DESIGN PATTERNS:\n"
          "  - Đa hình: Lớp trừu tượng QuanCo (kiemTraNuocDi, layGiaTri, layDiemViTri, laTuong).\n"
          "  - Kế thừa: Xe, Voi, Phao, Ma, Tuong, Si, Tot & QuanCoFactory, Deep Copy clone().\n\n"
          "- LUẬT DI CHUYỂN CÁC QUÂN CỜ:\n"
          "  1. XE: Đi ngang/dọc không giới hạn, không nhảy qua quân khác.\n"
          "  2. TƯỢNG (VOI): Đi chéo 2 ô trong sân nhà, bị cản khi có quân ở mắt Tượng.\n"
          "  3. PHÁO: Đi như Xe; khi ăn quân phải nhảy qua đúng 1 quân làm ngòi.\n"
          "  4. MÃ: Đi chữ Nhật (2x1), bị cản nếu có quân đứng ngay sát bên.\n"
          "  5. TƯỚNG: Đi 1 ô ngang/dọc trong Cung Cửu Cung; hai Tướng không đối mặt.\n"
          "  6. SĨ: Đi chéo 1 ô trong Cung Cửu Cung.\n"
          "  7. TỐT: Đi thẳng 1 bước; qua sông được đi ngang hoặc thẳng, không đi lùi.\n\n"
          "- ĐIỀU KHIỂN: W/A/S/D di chuyển con trỏ, Enter chọn/đi, ESC bỏ chọn hoặc tạm dừng."
        : "- PROJECT INTRODUCTION:\n"
          "  Object-Oriented Programming (OOP) Project - VNU-HCM University of Science (HCMUS).\n"
          "  Ancient Chinese Chess (Xiangqi) built with C++17 and SFML graphics.\n\n"
          "- OOP ARCHITECTURE & DESIGN PATTERNS:\n"
          "  - Polymorphism: Abstract QuanCo base (kiemTraNuocDi, layGiaTri, layDiemViTri, laTuong).\n"
          "  - Derived Pieces: Xe, Voi, Phao, Ma, Tuong, Si, Tot & Factory / Prototype clone().\n\n"
          "- PIECE MOVEMENT RULES:\n"
          "  1. CHARIOT (Xe): Moves orthogonally any distance; blocked by pieces.\n"
          "  2. ELEPHANT (Voi): Moves diagonally 2 squares; stays on own side; blocked by eye.\n"
          "  3. CANNON (Phao): Moves like Chariot; captures by jumping over exactly 1 screen.\n"
          "  4. HORSE (Ma): Moves in L-shape (1+1 diagonal); blocked by adjacent hobbling piece.\n"
          "  5. GENERAL (Tuong): Moves 1 step orthogonally in Palace; Flying Generals forbidden.\n"
          "  6. ADVISOR (Si): Moves 1 step diagonally within the Palace.\n"
          "  7. SOLDIER (Tot): 1 step forward; gains sideways movement after crossing the river.\n\n"
          "- CONTROLS: W/A/S/D move cursor, Enter select/move, ESC cancel or pause.";

    sf::Text content;
    content.setFont(fontBold);
    content.setString(toUtf8(text));
    content.setCharacterSize(16);
    content.setStyle(sf::Text::Bold);
    content.setLineSpacing(1.16f);
    content.setFillColor(sf::Color(35, 18, 10));
    content.setPosition(176.0f, 206.0f);
    window.draw(content);

    menuIntro.draw(window);
}
