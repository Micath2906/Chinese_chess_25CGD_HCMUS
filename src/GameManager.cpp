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

namespace fs = std::filesystem;

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
    : window(sf::VideoMode(1600, 900), "Co Tuong - Chinese Chess (HCMUS OOP)", sf::Style::Close | sf::Style::Titlebar),
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
      atmosphere(1920.0f, 1080.0f),
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
      kichThuocO(80.0f),
      offsetX(640.0f),
      offsetY(180.0f),
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
    fontVn.loadFromFile("assets/font/ViaodaLibre-Regular.ttf") ||
    fontVn.loadFromFile("resources/fonts/ViaodaLibre-Regular.ttf") ||
    fontVn.loadFromFile("assets/font/NomNaTong-Regular.ttf") ||
    fontVn.loadFromFile("resources/fonts/NomNaTong-Regular.ttf") ||
    fontVn.loadFromFile("resources/fonts/arial.ttf") ||
    fontVn.loadFromFile("C:/Windows/Fonts/arial.ttf");

    fontVnBold.loadFromFile("assets/font/ViaodaLibre-Regular.ttf") ||
    fontVnBold.loadFromFile("resources/fonts/ViaodaLibre-Regular.ttf") ||
    fontVnBold.loadFromFile("C:/Windows/Fonts/arialbd.ttf") ||
    fontVnBold.loadFromFile("resources/fonts/arial.ttf");

    fontEn.loadFromFile("assets/font/ViaodaLibre-Regular.ttf") ||
    fontEn.loadFromFile("resources/fonts/ViaodaLibre-Regular.ttf") ||
    fontEn.loadFromFile("assets/font/PixeloidSans.ttf") ||
    fontEn.loadFromFile("resources/fonts/PixeloidSans.ttf");

    fontEnBold.loadFromFile("assets/font/ViaodaLibre-Regular.ttf") ||
    fontEnBold.loadFromFile("resources/fonts/ViaodaLibre-Regular.ttf") ||
    fontEnBold.loadFromFile("assets/font/PixeloidSansBold.ttf") ||
    fontEnBold.loadFromFile("resources/fonts/PixeloidSansBold.ttf");

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
    float cx = (1920.0f - bw) / 2.0f;
    float sp = 62.0f;

    // Main menu
    menuChinh.setFont(font); 
    menuChinh.xoaTatCaButton();
    menuChinh.laMenuChinh = true;

    // Menu buttons
    bool isVi = (Loc::getLanguage() == Language::TIENG_VIET);
    menuChinh.themButton(0, 0, 370.0f, 54.0f, isVi ? "CHƠI MỚI" : "PLAY", [this]() {
        soundManager.play(SoundType::CLICK);
        menuChinh.setSection(MenuSection::PLAY);
    });
    menuChinh.themButton(0, 0, 370.0f, 54.0f, isVi ? "TIẾP TỤC" : "LOAD", [this]() {
        soundManager.play(SoundType::CLICK);
        capNhatThongTinSlots();
        menuChinh.setSection(MenuSection::LOAD);
    });
    menuChinh.themButton(0, 0, 370.0f, 54.0f, Loc::get(LocKey::MENU_SETTINGS), [this]() {
        soundManager.play(SoundType::CLICK);
        menuChinh.setSection(MenuSection::SETTINGS);
    });
    menuChinh.themButton(0, 0, 370.0f, 54.0f, isVi ? "THÔNG TIN" : "INFORMATION", [this]() {
        soundManager.play(SoundType::CLICK);
        menuChinh.setSection(MenuSection::INFO);
    });
    menuChinh.themButton(0, 0, 370.0f, 54.0f, Loc::get(LocKey::MENU_EXIT), [this]() {
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
    menuIntro.themButton((1920.0f - 220.0f) / 2.0f, 715.0f, 220.0f, 48.0f, Loc::get(LocKey::BACK), [this]() {
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

    // Tab buttons
    float tabW = 230.0f;
    float tabH = 46.0f;
    float tabY = 135.0f;
    float tabStartX = (1920.0f - (tabW * 4 + 30.0f)) / 2.0f;

    settingsTabButtons.push_back(std::make_unique<Button>(tabStartX, tabY, tabW, tabH, Loc::get(LocKey::TAB_AUDIO), font, [this]() {
        activeSettingsTab = SettingsTab::AUDIO;
        rebindingAction = KeyAction::COUNT;
        soundManager.play(SoundType::CLICK);
    }));
    settingsTabButtons.push_back(std::make_unique<Button>(tabStartX + tabW + 10.0f, tabY, tabW, tabH, Loc::get(LocKey::TAB_GRAPHICS), font, [this]() {
        activeSettingsTab = SettingsTab::GRAPHICS;
        rebindingAction = KeyAction::COUNT;
        soundManager.play(SoundType::CLICK);
    }));
    settingsTabButtons.push_back(std::make_unique<Button>(tabStartX + (tabW + 10.0f) * 2, tabY, tabW, tabH, Loc::get(LocKey::TAB_GAMEPLAY), font, [this]() {
        activeSettingsTab = SettingsTab::GAMEPLAY;
        rebindingAction = KeyAction::COUNT;
        soundManager.play(SoundType::CLICK);
    }));
    settingsTabButtons.push_back(std::make_unique<Button>(tabStartX + (tabW + 10.0f) * 3, tabY, tabW, tabH, Loc::get(LocKey::TAB_CONTROLS), font, [this]() {
        activeSettingsTab = SettingsTab::CONTROLS;
        rebindingAction = KeyAction::COUNT;
        soundManager.play(SoundType::CLICK);
    }));
    // Audio controls
    float colX = 140.0f;
    float colW = 460.0f;
    float startItemY = 220.0f;

    sliderMaster = std::make_unique<Slider>(colX, startItemY, colW, 24.0f, 0.0f, 100.0f,
        soundManager.getMasterVolume(), Loc::get(LocKey::SETTING_MASTER_VOL), font,
        [this](float val) { soundManager.setMasterVolume(val); });

    settingsAudioButtons.clear();
    // SFX Toggle
    settingsAudioButtons.push_back(std::make_unique<Button>(colX, startItemY + 58.0f, colW, 36.0f, "", font, [this]() {
        soundManager.setSfxBat(!soundManager.getSfxBat());
        soundManager.play(SoundType::CLICK);
        capNhatNhanNut();
    }));

    sliderSfx = std::make_unique<Slider>(colX, startItemY + 130.0f, colW, 24.0f, 0.0f, 100.0f,
        soundManager.getSfxVolume(), Loc::get(LocKey::SETTING_SFX_VOL), font,
        [this](float val) { soundManager.setSfxVolume(val); });

    // BGM Toggle
    settingsAudioButtons.push_back(std::make_unique<Button>(colX, startItemY + 188.0f, colW, 36.0f, "", font, [this]() {
        soundManager.setBgmBat(!soundManager.getBgmBat());
        soundManager.play(SoundType::CLICK);
        capNhatNhanNut();
    }));

    sliderBgm = std::make_unique<Slider>(colX, startItemY + 260.0f, colW, 24.0f, 0.0f, 100.0f,
        soundManager.getBgmVolume(), Loc::get(LocKey::SETTING_BGM_VOL), font,
        [this](float val) { soundManager.setBgmVolume(val); });

    // Interactive Soundboard
    soundboardButtons.clear();
    float sbY = startItemY + 338.0f;
    float sbW = 85.0f;
    float sbH = 38.0f;
    float sbGap = 8.5f;

    soundboardButtons.push_back(std::make_unique<Button>(colX, sbY, sbW, sbH, "Move", font, [this]() {
        soundManager.play(SoundType::MOVE);
    }));
    soundboardButtons.push_back(std::make_unique<Button>(colX + (sbW + sbGap), sbY, sbW, sbH, "Capture", font, [this]() {
        soundManager.play(SoundType::CAPTURE);
    }));
    soundboardButtons.push_back(std::make_unique<Button>(colX + (sbW + sbGap) * 2, sbY, sbW, sbH, "Check", font, [this]() {
        soundManager.play(SoundType::CHECK);
    }));
    soundboardButtons.push_back(std::make_unique<Button>(colX + (sbW + sbGap) * 3, sbY, sbW, sbH, "Win", font, [this]() {
        soundManager.play(SoundType::VICTORY);
    }));
    soundboardButtons.push_back(std::make_unique<Button>(colX + (sbW + sbGap) * 4, sbY, sbW, sbH, "Click", font, [this]() {
        soundManager.play(SoundType::CLICK);
    }));

    // 3. Graphics Tab Controls
    settingsGraphicsButtons.clear();
    // Board Theme
    settingsGraphicsButtons.push_back(std::make_unique<Button>(colX, startItemY, colW, 38.0f, "", font, [this]() {
        int t = (static_cast<int>(boardTheme) + 1) % 4;
        boardTheme = static_cast<BoardTheme>(t);
        soundManager.play(SoundType::CLICK);
        capNhatNhanNut();
    }));

    // Piece Style
    settingsGraphicsButtons.push_back(std::make_unique<Button>(colX, startItemY + 46.0f, colW, 38.0f, "", font, [this]() {
        int s = (static_cast<int>(pieceStyle) + 1) % 4;
        pieceStyle = static_cast<PieceStyle>(s);
        soundManager.play(SoundType::CLICK);
        capNhatNhanNut();
    }));

    // Move Hints
    settingsGraphicsButtons.push_back(std::make_unique<Button>(colX, startItemY + 92.0f, colW, 38.0f, "", font, [this]() {
        hienGoiY = !hienGoiY;
        soundManager.play(SoundType::CLICK);
        capNhatNhanNut();
    }));

    // Last Move Highlight
    settingsGraphicsButtons.push_back(std::make_unique<Button>(colX, startItemY + 138.0f, colW, 38.0f, "", font, [this]() {
        hienNuocDiCuoi = !hienNuocDiCuoi;
        soundManager.play(SoundType::CLICK);
        capNhatNhanNut();
    }));

    // Board Coordinates
    settingsGraphicsButtons.push_back(std::make_unique<Button>(colX, startItemY + 184.0f, colW, 38.0f, "", font, [this]() {
        hienToaDo = !hienToaDo;
        soundManager.play(SoundType::CLICK);
        capNhatNhanNut();
    }));

    // Window Display Mode
    settingsGraphicsButtons.push_back(std::make_unique<Button>(colX, startItemY + 230.0f, colW, 38.0f, "", font, [this]() {
        fullscreenMode = !fullscreenMode;
        apDungCheDoManHinh();
        soundManager.play(SoundType::CLICK);
        capNhatNhanNut();
    }));
    // Atmosphere style
    settingsGraphicsButtons.push_back(std::make_unique<Button>(colX, startItemY + 276.0f, colW, 38.0f, "", font, [this]() {
        atmosphere.cycleStyle();
        soundManager.play(SoundType::CLICK);
        capNhatNhanNut();
    }));

    // Atmosphere Density Slider
    sliderAtmosphere = std::make_unique<Slider>(colX, startItemY + 348.0f, colW, 24.0f, 0.0f, 100.0f,
        atmosphere.getDensity(), Loc::get(LocKey::SETTING_ATMOSPHERE_DENSITY), font,
        [this](float val) { atmosphere.setDensity(val); });

    // 4. Gameplay Tab Controls
    float gpItemH = 42.0f;
    float gpGapY = 56.0f;

    // Language
    settingsGameplayButtons.push_back(std::make_unique<Button>(colX, startItemY, colW, gpItemH, "", font, [this]() {
        Loc::toggleLanguage();
        capNhatNhanNut();
        soundManager.play(SoundType::CLICK);
    }));

    // AI Thinking Speed
    settingsGameplayButtons.push_back(std::make_unique<Button>(colX, startItemY + gpGapY, colW, gpItemH, "", font, [this]() {
        aiThinkRealistic = !aiThinkRealistic;
        soundManager.play(SoundType::CLICK);
        capNhatNhanNut();
    }));

    // Check Alarm
    settingsGameplayButtons.push_back(std::make_unique<Button>(colX, startItemY + gpGapY * 2, colW, gpItemH, "", font, [this]() {
        checkAlarmSound = !checkAlarmSound;
        soundManager.play(SoundType::CLICK);
        capNhatNhanNut();
    }));

    // Controls tab
    float ctrlColW = 480.0f;
    float ctrlItemH = 38.0f;
    float ctrlGapY = 46.0f;
    float ctrlStartY = 210.0f;
    float ctrlColX = 140.0f;

    for (int a = 0; a < 7; ++a) {
        KeyAction act = static_cast<KeyAction>(a);
        settingsControlButtons.push_back(std::make_unique<Button>(ctrlColX, ctrlStartY + ctrlGapY * a, ctrlColW, ctrlItemH, "", font, [this, act]() {
            rebindingAction = act;
            capNhatNhanNut();
            soundManager.play(SoundType::CLICK);
        }));
    }

    // Presets buttons in Controls Tab
    float pbtnW = 150.0f;
    float pbtnY = ctrlStartY + ctrlGapY * 7 + 10.0f;
    settingsControlButtons.push_back(std::make_unique<Button>(ctrlColX, pbtnY, pbtnW, 40.0f, "WASD", font, [this]() {
        keyConfig.setPreset(KeyPreset::WASD);
        rebindingAction = KeyAction::COUNT;
        capNhatNhanNut();
        soundManager.play(SoundType::CLICK);
    }));
    settingsControlButtons.push_back(std::make_unique<Button>(ctrlColX + 165.0f, pbtnY, pbtnW, 40.0f, "Arrows", font, [this]() {
        keyConfig.setPreset(KeyPreset::ARROWS);
        rebindingAction = KeyAction::COUNT;
        capNhatNhanNut();
        soundManager.play(SoundType::CLICK);
    }));
    settingsControlButtons.push_back(std::make_unique<Button>(ctrlColX + 330.0f, pbtnY, pbtnW, 40.0f, "IJKL", font, [this]() {
        keyConfig.setPreset(KeyPreset::IJKL);
        rebindingAction = KeyAction::COUNT;
        capNhatNhanNut();
        soundManager.play(SoundType::CLICK);
    }));

    // Bottom action bar
    float bY = 705.0f;
    settingsBottomButtons.push_back(std::make_unique<Button>(260.0f, bY, 210.0f, 48.0f, Loc::get(LocKey::BTN_APPLY_SAVE), font, [this]() {
        luuCaiDat();
        soundManager.play(SoundType::VICTORY);
        hienToast((Loc::getLanguage() == Language::TIENG_VIET) ? "Da luu va ap dung toan bo cai dat!" : "Settings successfully applied & saved!");
    }));

    settingsBottomButtons.push_back(std::make_unique<Button>(490.0f, bY, 220.0f, 48.0f, Loc::get(LocKey::BTN_RESTORE_DEFAULTS), font, [this]() {
        khoiPhucCaiDatMacDinh();
        soundManager.play(SoundType::CLICK);
        hienToast((Loc::getLanguage() == Language::TIENG_VIET) ? "Da khoi phuc cai dat mac dinh ban dau." : "Restored default settings.");
    }));

    settingsBottomButtons.push_back(std::make_unique<Button>(730.0f, bY, 190.0f, 48.0f, Loc::get(LocKey::BACK), font, [this]() {
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

    // Left panel buttons
    // Undo and redo
    auto btnUndo = std::make_unique<Button>(85.0f, 668.0f, 205.0f, 46.0f, Loc::get(LocKey::UNDO), font, [this]() {
        thucHienHoanTac();
    });
    btnUndo->setColors(sf::Color(55, 42, 32, 240), sf::Color(85, 65, 48, 255), sf::Color(40, 30, 22, 255));
    inGameButtons.push_back(std::move(btnUndo));

    auto btnRedo = std::make_unique<Button>(310.0f, 668.0f, 205.0f, 46.0f, Loc::get(LocKey::REDO), font, [this]() {
        thucHienDiTiep();
    });
    btnRedo->setColors(sf::Color(55, 42, 32, 240), sf::Color(85, 65, 48, 255), sf::Color(40, 30, 22, 255));
    inGameButtons.push_back(std::move(btnRedo));

    // Draw and resign
    auto btnDraw = std::make_unique<Button>(85.0f, 724.0f, 205.0f, 46.0f, Loc::get(LocKey::DRAW_OFFER), font, [this]() {
        thucHienXinHoa();
    });
    btnDraw->setColors(sf::Color(45, 52, 65, 240), sf::Color(65, 80, 105, 255), sf::Color(35, 40, 52, 255));
    inGameButtons.push_back(std::move(btnDraw));

    auto btnResign = std::make_unique<Button>(310.0f, 724.0f, 205.0f, 46.0f, Loc::get(LocKey::SURRENDER), font, [this]() {
        thucHienDauHang();
    });
    btnResign->setColors(sf::Color(75, 32, 32, 240), sf::Color(110, 45, 45, 255), sf::Color(55, 22, 22, 255));
    inGameButtons.push_back(std::move(btnResign));

    // Hàng 3: Nút Mở Menu trận đấu chính
    auto btnOpen = std::make_unique<Button>(85.0f, 782.0f, 430.0f, 54.0f, Loc::get(LocKey::BTN_GAME_MENU), font, [this]() {
        moMenuPopUp();
    });
    btnOpen->setColors(sf::Color(45, 70, 115, 250), sf::Color(65, 100, 160, 255), sf::Color(30, 50, 85, 255));
    inGameButtons.push_back(std::move(btnOpen));

    // Popup menu buttons
    // Toạ độ thế giới ảo cố định 1920x1080
    const float dw = 580.0f;
    const float dh = 600.0f;
    const float dx = (1920.0f - dw) / 2.0f; // 670.0f
    const float dy = (1080.0f - dh) / 2.0f; // 240.0f

    const float innerX = dx + 40.0f;
    const float fullW = dw - 80.0f;
    const float colW = (fullW - 20.0f) / 2.0f;
    const float col1X = innerX;
    const float col2X = innerX + colW + 20.0f;

    const float r0Y = dy + 115.0f;
    const float r1Y = dy + 180.0f;
    const float r2Y = dy + 238.0f;
    const float r3Y = dy + 296.0f;
    const float r4Y = dy + 354.0f;
    const float r5Y = dy + 424.0f;

    // Resume button
    auto btnResume = std::make_unique<Button>(innerX, r0Y, fullW, 50.0f, Loc::get(LocKey::RESUME), font, [this]() {
        dongMenuPopUp();
    });
    btnResume->setColors(sf::Color(35, 125, 75, 245), sf::Color(45, 160, 95, 255), sf::Color(25, 95, 55, 255));
    inGameMenuButtons.push_back(std::move(btnResume));

    // New game and draw
    auto btnNew = std::make_unique<Button>(col1X, r1Y, colW, 46.0f, Loc::get(LocKey::NEW_GAME), font, [this]() {
        dongMenuPopUp();
        batDauTroChoiMoi(cheDoChoi);
    });
    btnNew->setColors(sf::Color(55, 42, 32, 240), sf::Color(85, 65, 48, 255), sf::Color(40, 30, 22, 255));
    inGameMenuButtons.push_back(std::move(btnNew));

    auto btnDrawOffer = std::make_unique<Button>(col2X, r1Y, colW, 46.0f, Loc::get(LocKey::DRAW_OFFER), font, [this]() {
        dongMenuPopUp();
        thucHienXinHoa();
    });
    btnDrawOffer->setColors(sf::Color(45, 52, 65, 240), sf::Color(65, 80, 105, 255), sf::Color(35, 40, 52, 255));
    inGameMenuButtons.push_back(std::move(btnDrawOffer));

    // Undo and redo
    auto btnPUndo = std::make_unique<Button>(col1X, r2Y, colW, 46.0f, Loc::get(LocKey::UNDO), font, [this]() {
        dongMenuPopUp();
        thucHienHoanTac();
    });
    btnPUndo->setColors(sf::Color(55, 42, 32, 240), sf::Color(85, 65, 48, 255), sf::Color(40, 30, 22, 255));
    inGameMenuButtons.push_back(std::move(btnPUndo));

    auto btnPRedo = std::make_unique<Button>(col2X, r2Y, colW, 46.0f, Loc::get(LocKey::REDO), font, [this]() {
        dongMenuPopUp();
        thucHienDiTiep();
    });
    btnPRedo->setColors(sf::Color(55, 42, 32, 240), sf::Color(85, 65, 48, 255), sf::Color(40, 30, 22, 255));
    inGameMenuButtons.push_back(std::move(btnPRedo));

    // Save and load
    auto btnSave = std::make_unique<Button>(col1X, r3Y, colW, 46.0f, Loc::get(LocKey::SAVE_GAME), font, [this]() {
        dongMenuPopUp();
        thucHienLuuGame();
    });
    btnSave->setColors(sf::Color(50, 48, 38, 240), sf::Color(78, 74, 58, 255), sf::Color(38, 36, 28, 255));
    inGameMenuButtons.push_back(std::move(btnSave));

    auto btnLoad = std::make_unique<Button>(col2X, r3Y, colW, 46.0f, Loc::get(LocKey::LOAD_GAME), font, [this]() {
        dongMenuPopUp();
        capNhatThongTinSlots();
        trangThaiTruocLoad = TrangThai::DANG_CHOI;
        matchMenuTuDangChoi = true;
        trangThai = TrangThai::MENU_CHINH;
        menuChinh.triggerOpenAnimation(MenuSection::LOAD);
    });
    btnLoad->setColors(sf::Color(50, 48, 38, 240), sf::Color(78, 74, 58, 255), sf::Color(38, 36, 28, 255));
    inGameMenuButtons.push_back(std::move(btnLoad));

    // Settings and resign
    auto btnSettings = std::make_unique<Button>(col1X, r4Y, colW, 46.0f, Loc::get(LocKey::MENU_SETTINGS), font, [this]() {
        dongMenuPopUp();
        trangThaiTruocSettings = TrangThai::DANG_CHOI;
        matchMenuTuDangChoi = true;
        trangThai = TrangThai::MENU_CHINH;
        menuChinh.triggerOpenAnimation(MenuSection::SETTINGS);
    });
    btnSettings->setColors(sf::Color(48, 55, 68, 240), sf::Color(72, 82, 102, 255), sf::Color(36, 42, 54, 255));
    inGameMenuButtons.push_back(std::move(btnSettings));

    auto btnRes = std::make_unique<Button>(col2X, r4Y, colW, 46.0f, Loc::get(LocKey::SURRENDER), font, [this]() {
        dongMenuPopUp();
        thucHienDauHang();
    });
    btnRes->setColors(sf::Color(75, 32, 32, 240), sf::Color(110, 45, 45, 255), sf::Color(55, 22, 22, 255));
    inGameMenuButtons.push_back(std::move(btnRes));

    // Exit to menu
    auto btnQuit = std::make_unique<Button>(innerX, r5Y, fullW, 50.0f, Loc::get(LocKey::BACK_MENU), font, [this]() {
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
    const float dw = 760.0f;
    const float dh = 520.0f;
    const float dx = (1920.0f - dw) / 2.0f;
    const float dy = (1080.0f - dh) / 2.0f;
    const float pw = 280.0f;
    const float ph = 50.0f;
    const float py = dy + dh - 76.0f;

    bool tiengViet = (Loc::getLanguage() == Language::TIENG_VIET);
    sf::Font& fBtn = getMenuFont(tiengViet, true);

    auto& assets = getMenuAssets();

    std::string againLabel = Loc::get(LocKey::PLAY_AGAIN) + " (Enter)";
    auto btnAgain = std::make_unique<Button>(dx + 55.0f, py, pw, ph, againLabel, fBtn, [this]() {
        soundManager.play(SoundType::CLICK);
        batDauTroChoiMoi(cheDoChoi);
    });
    btnAgain->setColors(sf::Color(35, 125, 75, 245), sf::Color(45, 165, 95, 255), sf::Color(25, 95, 55, 255));
    if (assets.iconPlay.getSize().x > 0) btnAgain->setIcon(&assets.iconPlay);
    popupButtons.push_back(std::move(btnAgain));

    std::string backLabel = Loc::get(LocKey::BACK_MENU) + " (ESC)";
    auto btnBack = std::make_unique<Button>(dx + dw - 55.0f - pw, py, pw, ph, backLabel, fBtn, [this]() {
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
    const float targetW = 1920.0f;
    const float targetH = 1080.0f;
    const float targetRatio = targetW / targetH;

    sf::Vector2u winSz = window.getSize();
    if (winSz.x == 0 || winSz.y == 0) return;

    float winRatio = static_cast<float>(winSz.x) / static_cast<float>(winSz.y);

    float vpX = 0.0f;
    float vpY = 0.0f;
    float vpW = 1.0f;
    float vpH = 1.0f;

    if (winRatio > targetRatio) {
        vpW = targetRatio / winRatio;
        vpX = (1.0f - vpW) / 2.0f;
    } else if (winRatio < targetRatio) {
        vpH = winRatio / targetRatio;
        vpY = (1.0f - vpH) / 2.0f;
    }

    sf::View view;
    view.setSize(targetW, targetH);
    view.setCenter(targetW / 2.0f, targetH / 2.0f);
    view.setViewport(sf::FloatRect(vpX, vpY, vpW, vpH));
    window.setView(view);
}

void GameManager::apDungCheDoManHinh() {
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    if (fullscreenMode) {
        window.create(desktop, "Co Tuong - Chinese Chess (HCMUS OOP)", sf::Style::Fullscreen);
    } else {
        unsigned int targetW = 1920;
        unsigned int targetH = 1080;
        if (desktop.width <= targetW || desktop.height <= targetH) {
            targetW = std::min(1600u, desktop.width > 60u ? desktop.width - 60u : desktop.width);
            targetH = std::min(900u, desktop.height > 80u ? desktop.height - 80u : desktop.height);
        }
        window.create(sf::VideoMode(targetW, targetH), "Co Tuong - Chinese Chess (HCMUS OOP)", sf::Style::Close | sf::Style::Titlebar);
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
        
        if (trangThai == TrangThai::MENU_CHINH) {
            atmosphere.update(dt);
        }
        ve();
    }
}

void GameManager::xuLySuKien() {
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
    if (!textureBackground.loadFromFile("assets/background.png") &&
        !textureBackground.loadFromFile("../assets/background.png") &&
        !textureBackground.loadFromFile("resources/textures/background.png") &&
        !textureBackground.loadFromFile("resources/textures/background_game.png")) {
        // Fallback
    }
    textureBackground.setSmooth(false); // keep pixel-art sharp

    // Load background
    if (!texturePlayBg.loadFromFile("assets/background play.jpg") &&
        !texturePlayBg.loadFromFile("../assets/background play.jpg") &&
        !texturePlayBg.loadFromFile("resources/textures/background play.jpg")) {
        // Fallback
    }
    texturePlayBg.setSmooth(true);

    // Load board texture
    {
        sf::Image imgBanCo;
        if (imgBanCo.loadFromFile("assets/bancofinal.png") || imgBanCo.loadFromFile("../assets/bancofinal.png") || imgBanCo.loadFromFile("resources/textures/ban_co.png")) {
            // Alpha mask
            sf::Vector2u sz = imgBanCo.getSize();
            for (unsigned y = 0; y < sz.y; ++y) {
                for (unsigned x = 0; x < sz.x; ++x) {
                    sf::Color px = imgBanCo.getPixel(x, y);
                    int minCh = std::min({(int)px.r, (int)px.g, (int)px.b});
                    if (minCh > 230) {
                        // Pure white background → transparent
                        imgBanCo.setPixel(x, y, sf::Color::Transparent);
                    } else if (minCh > 200) {
                        // Anti-aliasing edge → semi-transparent
                        sf::Uint8 alpha = static_cast<sf::Uint8>(
                            255.0f * (1.0f - (minCh - 200) / 55.0f));
                        imgBanCo.setPixel(x, y, sf::Color(px.r, px.g, px.b, alpha));
                    }
                }
            }
            textureBanCo.loadFromImage(imgBanCo);
            textureBanCo.setSmooth(true);

            // Board parameters
            kichThuocO = 80.0f;
            offsetX    = 640.0f;
            offsetY    = 180.0f;
        }
    }

    // Load red pieces
    textureQuanDo["Tuong"].loadFromFile("resources/textures/pieces/r_tuong.png");
    textureQuanDo["Si"].loadFromFile("resources/textures/pieces/r_si.png");
    textureQuanDo["Voi"].loadFromFile("resources/textures/pieces/r_voi.png");
    textureQuanDo["Ma"].loadFromFile("resources/textures/pieces/r_ma.png");
    textureQuanDo["Xe"].loadFromFile("resources/textures/pieces/r_xe.png");
    textureQuanDo["Phao"].loadFromFile("resources/textures/pieces/r_phao.png");
    textureQuanDo["Tot"].loadFromFile("resources/textures/pieces/r_tot.png");

    // Load black pieces
    textureQuanDen["Tuong"].loadFromFile("resources/textures/pieces/b_tuong.png");
    textureQuanDen["Si"].loadFromFile("resources/textures/pieces/b_si.png");
    textureQuanDen["Voi"].loadFromFile("resources/textures/pieces/b_voi.png");
    textureQuanDen["Ma"].loadFromFile("resources/textures/pieces/b_ma.png");
    textureQuanDen["Xe"].loadFromFile("resources/textures/pieces/b_xe.png");
    textureQuanDen["Phao"].loadFromFile("resources/textures/pieces/b_phao.png");
    textureQuanDen["Tot"].loadFromFile("resources/textures/pieces/b_tot.png");

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
        if (!avatarTextures[i].loadFromFile(paths[i])) {
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
    if (aura == PieceAura::IMPERIAL_GOLD) {
        sf::CircleShape glow(r + 5.0f);
        glow.setOrigin(r + 5.0f, r + 5.0f);
        glow.setPosition(pos);
        glow.setFillColor(sf::Color(255, 215, 0, 75));
        window.draw(glow);

        sf::CircleShape ring(r + 3.0f);
        ring.setOrigin(r + 3.0f, r + 3.0f);
        ring.setPosition(pos);
        ring.setFillColor(sf::Color::Transparent);
        ring.setOutlineThickness(2.2f);
        ring.setOutlineColor(sf::Color(255, 225, 90, 220));
        window.draw(ring);

        for (int i = 0; i < 4; ++i) {
            float ang = i * 3.14159265f / 2.0f;
            sf::CircleShape gem(2.2f);
            gem.setOrigin(2.2f, 2.2f);
            gem.setPosition(pos.x + std::cos(ang) * (r + 3.0f), pos.y + std::sin(ang) * (r + 3.0f));
            gem.setFillColor(sf::Color(255, 250, 180));
            window.draw(gem);
        }
    } else if (aura == PieceAura::DRAGON_FLAME) {
        sf::CircleShape glow(r + 5.0f);
        glow.setOrigin(r + 5.0f, r + 5.0f);
        glow.setPosition(pos);
        glow.setFillColor(sf::Color(255, 60, 30, 85));
        window.draw(glow);

        sf::CircleShape ring(r + 3.0f);
        ring.setOrigin(r + 3.0f, r + 3.0f);
        ring.setPosition(pos);
        ring.setFillColor(sf::Color::Transparent);
        ring.setOutlineThickness(2.0f);
        ring.setOutlineColor(sf::Color(255, 110, 40, 220));
        window.draw(ring);
    } else if (aura == PieceAura::THUNDER_CYAN) {
        sf::CircleShape glow(r + 5.0f);
        glow.setOrigin(r + 5.0f, r + 5.0f);
        glow.setPosition(pos);
        glow.setFillColor(sf::Color(0, 220, 200, 80));
        window.draw(glow);

        sf::CircleShape ring(r + 3.0f);
        ring.setOrigin(r + 3.0f, r + 3.0f);
        ring.setPosition(pos);
        ring.setFillColor(sf::Color::Transparent);
        ring.setOutlineThickness(2.0f);
        ring.setOutlineColor(sf::Color(80, 255, 240, 220));
        window.draw(ring);
    } else if (aura == PieceAura::SHADOW_VOID) {
        sf::CircleShape glow(r + 5.0f);
        glow.setOrigin(r + 5.0f, r + 5.0f);
        glow.setPosition(pos);
        glow.setFillColor(sf::Color(170, 70, 240, 80));
        window.draw(glow);

        sf::CircleShape ring(r + 3.0f);
        ring.setOrigin(r + 3.0f, r + 3.0f);
        ring.setPosition(pos);
        ring.setFillColor(sf::Color::Transparent);
        ring.setOutlineThickness(2.0f);
        ring.setOutlineColor(sf::Color(215, 120, 255, 220));
        window.draw(ring);
    }

    if (isKing) {
        sf::ConvexShape crown(5);
        crown.setPoint(0, sf::Vector2f(-7.0f, -r - 1.0f));
        crown.setPoint(1, sf::Vector2f(-4.0f, -r - 7.0f));
        crown.setPoint(2, sf::Vector2f(0.0f, -r - 4.0f));
        crown.setPoint(3, sf::Vector2f(4.0f, -r - 7.0f));
        crown.setPoint(4, sf::Vector2f(7.0f, -r - 1.0f));
        crown.setPosition(pos);
        crown.setFillColor(sf::Color(255, 215, 0));
        crown.setOutlineThickness(1.0f);
        crown.setOutlineColor(sf::Color(180, 120, 20));
        window.draw(crown);

        sf::CircleShape jewel(2.0f);
        jewel.setOrigin(2.0f, 2.0f);
        jewel.setPosition(pos.x, pos.y - r - 4.0f);
        jewel.setFillColor(sf::Color(255, 40, 40));
        window.draw(jewel);
    }
}


// Rendering

void GameManager::ve() {
    window.clear(sf::Color(14, 18, 26));
    if (trangThai != TrangThai::DANG_CHOI) {
        veNenMenuGiaoDien(window);
    }

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
        veBanCoTruyenThong();
        veHighlights();
        veCacQuanCo();
        veConTroBanPhim();
        veThanhBenTrai();
        veThanhBenPhai();
        veThanhThongBao();

        if (banCo.getKetThuc()) {
            vePopupKetThuc();
        } else if (hienMenuPopUp) {
            veMenuPopUpInGame();
        }
    }

    if (trangThai == TrangThai::MENU_CHINH) {
        atmosphere.draw(window);
    }
    veToast();

    // Draw cursor
    auto& mAssets = getMenuAssets();
    if (mAssets.cursorNormal.getSize().x > 0) {
        sf::Vector2i mPos = sf::Mouse::getPosition(window);
        sf::Vector2u winSz = window.getSize();
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
            window.setView(window.getDefaultView());
            curSprite.setPosition(static_cast<float>(mPos.x), static_cast<float>(mPos.y));
            window.draw(curSprite);
            window.setView(prevView);
        }
    }

    window.display();
}
void GameManager::veBanCoTruyenThong() {
    float boardW = (BanCo::getSOCOT() - 1) * kichThuocO + 80.0f;
    float boardH = (BanCo::getSOHANG() - 1) * kichThuocO + 80.0f;
    float bx = offsetX - 40.0f;
    float by = offsetY - 40.0f;

    bool useImageBoard = (boardTheme == BoardTheme::CLASSIC_WOOD && textureBanCo.getSize().x > 0);

    // Draw background
    sf::Vector2f viewSz = window.getView().getSize();
    auto& mAssets = getMenuAssets();
    const sf::Texture* pBg = (texturePlayBg.getSize().x > 0) ? &texturePlayBg
                           : ((mAssets.playBg.getSize().x > 0) ? &mAssets.playBg : nullptr);

    if (pBg) {
        sf::Sprite bgSprite(*pBg);
        bgSprite.setPosition(0.0f, 0.0f);
        bgSprite.setScale(viewSz.x / static_cast<float>(pBg->getSize().x),
                          viewSz.y / static_cast<float>(pBg->getSize().y));
        window.draw(bgSprite);
    } else if (textureBackground.getSize().x > 0) {
        sf::Sprite bgSprite(textureBackground);
        bgSprite.setPosition(0.0f, 0.0f);
        bgSprite.setScale(viewSz.x / static_cast<float>(textureBackground.getSize().x),
                          viewSz.y / static_cast<float>(textureBackground.getSize().y));
        window.draw(bgSprite);
    }

    // Board colors
    sf::Color boardBgCol(228, 192, 138);
    sf::Color borderCol(90, 50, 25);
    sf::Color lineCol(90, 50, 25);
    sf::Color riverCol(140, 75, 35, 180);

    if (boardTheme == BoardTheme::IMPERIAL_JADE) {
        boardBgCol = sf::Color(190, 222, 200);
        borderCol = sf::Color(30, 70, 50);
        lineCol = sf::Color(35, 80, 55);
        riverCol = sf::Color(25, 75, 48, 190);
    }
    else if (boardTheme == BoardTheme::MIDNIGHT_INK) {
        boardBgCol = sf::Color(32, 38, 50);
        borderCol = sf::Color(18, 22, 30);
        lineCol = sf::Color(75, 95, 125);
        riverCol = sf::Color(120, 150, 190, 190);
    }
    else if (boardTheme == BoardTheme::WARM_BAMBOO) {
        boardBgCol = sf::Color(242, 225, 190);
        borderCol = sf::Color(115, 75, 40);
        lineCol = sf::Color(110, 70, 35);
        riverCol = sf::Color(145, 90, 45, 190);
    }

    // Board surface
    if (useImageBoard) {
        sf::Sprite boardSprite(textureBanCo);

        // Crop board
        boardSprite.setTextureRect(sf::IntRect(348, 47, 678, 676));

        // Grid size
        float gridW = (BanCo::getSOCOT() - 1) * kichThuocO;  // Rộng 8 ô
        float gridH = (BanCo::getSOHANG() - 1) * kichThuocO; // Dài 9 ô

        // Margin offset
        float leX = 40.0f;
        float leY = 40.0f;

        float targetW = gridW + leX * 2.0f;
        float targetH = gridH + leY * 2.0f;

        // Board scale
        boardSprite.setScale(targetW / 678.0f, targetH / 676.0f);

        // Board position
        boardSprite.setPosition(offsetX - leX, offsetY - leY);

        window.draw(boardSprite);
    }
    else {
        // Solid board
        sf::RectangleShape boardBg(sf::Vector2f(boardW, boardH));
        boardBg.setPosition(bx, by);
        boardBg.setFillColor(boardBgCol);
        boardBg.setOutlineThickness(4.0f);
        boardBg.setOutlineColor(borderCol);
        window.draw(boardBg);

        // Grid lines (horizontal rows)
        for (int r = 0; r < BanCo::getSOHANG(); ++r) {
            sf::Vertex line[] = {
                sf::Vertex(chuyenDoiToaDoBanCoThanhManHinh(r, 0), lineCol),
                sf::Vertex(chuyenDoiToaDoBanCoThanhManHinh(r, BanCo::getSOCOT() - 1), lineCol)
            };
            window.draw(line, 2, sf::Lines);
        }

        // Grid lines
        for (int c = 0; c < BanCo::getSOCOT(); ++c) {
            if (c == 0 || c == BanCo::getSOCOT() - 1) {
                sf::Vertex line[] = {
                    sf::Vertex(chuyenDoiToaDoBanCoThanhManHinh(0, c), lineCol),
                    sf::Vertex(chuyenDoiToaDoBanCoThanhManHinh(BanCo::getSOHANG() - 1, c), lineCol)
                };
                window.draw(line, 2, sf::Lines);
            }
            else {
                sf::Vertex lineTop[] = {
                    sf::Vertex(chuyenDoiToaDoBanCoThanhManHinh(0, c), lineCol),
                    sf::Vertex(chuyenDoiToaDoBanCoThanhManHinh(4, c), lineCol)
                };
                window.draw(lineTop, 2, sf::Lines);
                sf::Vertex lineBot[] = {
                    sf::Vertex(chuyenDoiToaDoBanCoThanhManHinh(5, c), lineCol),
                    sf::Vertex(chuyenDoiToaDoBanCoThanhManHinh(9, c), lineCol)
                };
                window.draw(lineBot, 2, sf::Lines);
            }
        }

        // Palace diagonals
        sf::Vertex palace1[] = { sf::Vertex(chuyenDoiToaDoBanCoThanhManHinh(0, 3), lineCol), sf::Vertex(chuyenDoiToaDoBanCoThanhManHinh(2, 5), lineCol) };
        sf::Vertex palace2[] = { sf::Vertex(chuyenDoiToaDoBanCoThanhManHinh(0, 5), lineCol), sf::Vertex(chuyenDoiToaDoBanCoThanhManHinh(2, 3), lineCol) };
        sf::Vertex palace3[] = { sf::Vertex(chuyenDoiToaDoBanCoThanhManHinh(7, 3), lineCol), sf::Vertex(chuyenDoiToaDoBanCoThanhManHinh(9, 5), lineCol) };
        sf::Vertex palace4[] = { sf::Vertex(chuyenDoiToaDoBanCoThanhManHinh(7, 5), lineCol), sf::Vertex(chuyenDoiToaDoBanCoThanhManHinh(9, 3), lineCol) };
        window.draw(palace1, 2, sf::Lines); window.draw(palace2, 2, sf::Lines);
        window.draw(palace3, 2, sf::Lines); window.draw(palace4, 2, sf::Lines);

        // River label
        sf::Text riverText;
        riverText.setFont(font);
        riverText.setCharacterSize(22);
        riverText.setStyle(sf::Text::Bold);
        bool isViRiver = (Loc::getLanguage() == Language::TIENG_VIET); riverText.setString(toUtf8(isViRiver ? "SỞ HÀ                HÁN GIỚI" : "CHU RIVER            HAN BORDER"));
        riverText.setFillColor(riverCol);
        sf::FloatRect rtb = riverText.getLocalBounds();
        riverText.setOrigin(rtb.left + rtb.width / 2.0f, rtb.top + rtb.height / 2.0f);
        riverText.setPosition(offsetX + 4 * kichThuocO, offsetY + 4.5f * kichThuocO);
        window.draw(riverText);
    }

    // Board coordinates
    if (hienToaDo) {
        // Coordinates color
        sf::Color coordCol = useImageBoard ? sf::Color(100, 20, 20, 180) : lineCol;
        coordCol.a = 180;

        // Columns: 1 to 9 (or 9 to 1)
        for (int c = 0; c < BanCo::getSOCOT(); ++c) {
            sf::Text tTop;
            tTop.setFont(font);
            tTop.setCharacterSize(15);
            tTop.setStyle(sf::Text::Bold);
            tTop.setString(std::to_string(c + 1));
            tTop.setFillColor(coordCol);
            tTop.setPosition(offsetX + c * kichThuocO - 4.0f, by + 10.0f);
            window.draw(tTop);

            sf::Text tBot;
            tBot.setFont(font);
            tBot.setCharacterSize(15);
            tBot.setStyle(sf::Text::Bold);
            tBot.setString(std::to_string(BanCo::getSOCOT() - c));
            tBot.setFillColor(coordCol);
            tBot.setPosition(offsetX + c * kichThuocO - 4.0f, by + boardH - 26.0f);
            window.draw(tBot);
        }

        // Rows: A to J
        const char rowLetters[] = "ABCDEFGHIJ";
        for (int r = 0; r < BanCo::getSOHANG(); ++r) {
            sf::Text tLeft;
            tLeft.setFont(font);
            tLeft.setCharacterSize(15);
            tLeft.setStyle(sf::Text::Bold);
            tLeft.setString(std::string(1, rowLetters[r]));
            tLeft.setFillColor(coordCol);
            tLeft.setPosition(bx + 12.0f, offsetY + r * kichThuocO - 9.0f);
            window.draw(tLeft);

            sf::Text tRight;
            tRight.setFont(font);
            tRight.setCharacterSize(15);
            tRight.setStyle(sf::Text::Bold);
            tRight.setString(std::string(1, rowLetters[r]));
            tRight.setFillColor(coordCol);
            tRight.setPosition(bx + boardW - 22.0f, offsetY + r * kichThuocO - 9.0f);
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
            
            sf::CircleShape c1(34.0f);
            c1.setOrigin(34.0f, 34.0f);
            c1.setPosition(p1);
            c1.setFillColor(sf::Color(80, 160, 240, 90));
            window.draw(c1);

            sf::CircleShape c2(34.0f);
            c2.setOrigin(34.0f, 34.0f);
            c2.setPosition(p2);
            c2.setFillColor(sf::Color(80, 160, 240, 120));
            window.draw(c2);
        }
    }

    // Move hints
    if (quanDangChon) {
        sf::Vector2f pos = chuyenDoiToaDoBanCoThanhManHinh(quanDangChon->getHang(), quanDangChon->getCot());
        sf::CircleShape selCircle(36.0f);
        selCircle.setOrigin(36.0f, 36.0f);
        selCircle.setPosition(pos);
        selCircle.setFillColor(sf::Color(255, 215, 0, 140));
        selCircle.setOutlineThickness(3.0f);
        selCircle.setOutlineColor(sf::Color(255, 230, 80));
        window.draw(selCircle);

        if (hienGoiY) {
            for (const auto& nd : cacNuocDiHopLe) {
                sf::Vector2f destPos = chuyenDoiToaDoBanCoThanhManHinh(nd.x, nd.y);
                bool coQuan = banCo.coQuanTai(nd.x, nd.y);
                if (coQuan) {
                    sf::CircleShape capCircle(33.0f);
                    capCircle.setOrigin(33.0f, 33.0f);
                    capCircle.setPosition(destPos);
                    capCircle.setFillColor(sf::Color(230, 40, 40, 120));
                    capCircle.setOutlineThickness(3.0f);
                    capCircle.setOutlineColor(sf::Color(250, 80, 80));
                    window.draw(capCircle);
                } else {
                    sf::CircleShape dot(10.0f);
                    dot.setOrigin(10.0f, 10.0f);
                    dot.setPosition(destPos);
                    dot.setFillColor(sf::Color(30, 180, 60, 210));
                    dot.setOutlineThickness(2.0f);
                    dot.setOutlineColor(sf::Color::White);
                    window.draw(dot);
                }
            }
        }
    }
}

void GameManager::veCacQuanCo() {
    float r = 35.0f;
    float targetDiameter = r * 2.0f;

    for (const auto& q : banCo.getCacQuan()) {
        if (!q || q->getDaBiAn()) continue;
        sf::Vector2f pos = chuyenDoiToaDoBanCoThanhManHinh(q->getHang(), q->getCot());

        const sf::Texture* tex = nullptr;
        if (pieceStyle == PieceStyle::REALISTIC_WOOD) {
            tex = layTextureQuan(q->layTen(), q->getMau());
        }

        PieceAura aura = (q->getMau() == Mau::DO) ? auraDo : auraDen;
        bool isKing = q->laTuong();
        veAuraQuan(pos, r, aura, isKing);

        if (tex != nullptr) {
            // Drop shadow for 3D realism
            sf::CircleShape shadow(r - 1.0f);
            shadow.setOrigin(r - 1.0f, r - 1.0f);
            shadow.setPosition(pos.x + 2.0f, pos.y + 2.5f);
            shadow.setFillColor(sf::Color(0, 0, 0, 110));
            window.draw(shadow);

            // Sprite rendering
            sf::Sprite sprite(*tex);
            sf::Vector2u texSize = tex->getSize();
            sprite.setOrigin(texSize.x / 2.0f, texSize.y / 2.0f);
            float scale = targetDiameter / static_cast<float>(texSize.x);
            sprite.setScale(scale, scale);
            sprite.setPosition(pos);
            window.draw(sprite);
        } else {
            // Outer rim
            sf::CircleShape outer(r + 3.0f);
            outer.setOrigin(r + 3.0f, r + 3.0f);
            outer.setPosition(pos);
            outer.setFillColor((q->getMau() == Mau::DO) ? sf::Color(140, 30, 20) : sf::Color(30, 40, 50));
            window.draw(outer);

            // Piece body
            sf::CircleShape piece(r);
            piece.setOrigin(r, r);
            piece.setPosition(pos);
            piece.setFillColor(sf::Color(250, 238, 215));
            piece.setOutlineThickness(2.0f);
            piece.setOutlineColor((q->getMau() == Mau::DO) ? sf::Color(180, 40, 30) : sf::Color(40, 50, 65));
            window.draw(piece);

            // Inner circle
            sf::CircleShape inner(r - 4.0f);
            inner.setOrigin(r - 4.0f, r - 4.0f);
            inner.setPosition(pos);
            inner.setFillColor(sf::Color::Transparent);
            inner.setOutlineThickness(1.2f);
            inner.setOutlineColor((q->getMau() == Mau::DO) ? sf::Color(200, 50, 40, 180) : sf::Color(60, 75, 95, 180));
            window.draw(inner);

            // Label according to PieceStyle
            std::string label = layKyHieuQuanTheoStyle(q->layTen(), q->getMau());

            sf::Text t;
            t.setFont(font);
            t.setString(toUtf8(label));
            t.setCharacterSize((label.size() > 2) ? 22 : 26);
            t.setStyle(sf::Text::Bold);
            t.setFillColor((q->getMau() == Mau::DO) ? sf::Color(190, 25, 20) : sf::Color(20, 25, 35));

            sf::FloatRect tb = t.getLocalBounds();
            t.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
            t.setPosition(pos);
            window.draw(t);
        }
    }
}

void GameManager::veConTroBanPhim() {
    sf::Vector2f pos = chuyenDoiToaDoBanCoThanhManHinh(viTriConTro.x, viTriConTro.y);
    float size = 38.0f;
    float len = 12.0f;
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

    if (checkAlert) {
        float bw = 640.0f;
        float bh = 48.0f;
        float bx = (1920.0f - bw) / 2.0f;
        float by = 72.0f;

        sf::RectangleShape banner(sf::Vector2f(bw, bh));
        banner.setPosition(bx, by);
        banner.setFillColor(sf::Color(140, 20, 20, 235));
        banner.setOutlineThickness(2.5f);
        banner.setOutlineColor(sf::Color(255, 215, 0));
        window.draw(banner);

        ve4GocLuaChon(window, sf::Vector2f(bx, by), sf::Vector2f(bw, bh), 2.0f, 1.5f);

        sf::Text t;
        t.setFont(font);
        t.setCharacterSize(20);
        t.setStyle(sf::Text::Bold);
        std::string sideStr = (banCo.getLuotChoi() == Mau::DO) ? (tiengViet ? "BÊN ĐỎ ĐANG BỊ CHIẾU TƯỚNG!" : "RED KING IS UNDER CHECK!")
                                                              : (tiengViet ? "BÊN ĐEN ĐANG BỊ CHIẾU TƯỚNG!" : "BLACK KING IS UNDER CHECK!");
        t.setString(toUtf8("[!]  " + sideStr + "  [!]"));
        t.setFillColor(sf::Color(255, 235, 120));
        sf::FloatRect tb = t.getLocalBounds();
        t.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
        t.setPosition(1920.0f / 2.0f, by + bh / 2.0f);
        window.draw(t);
    } else {
        float bw = 460.0f;
        float bh = 42.0f;
        float bx = (1920.0f - bw) / 2.0f;
        float by = 76.0f;

        sf::RectangleShape banner(sf::Vector2f(bw, bh));
        banner.setPosition(bx, by);
        banner.setFillColor(sf::Color(32, 22, 16, 215));
        banner.setOutlineThickness(1.5f);
        banner.setOutlineColor(sf::Color(120, 85, 45));
        window.draw(banner);

        ve4GocLuaChon(window, sf::Vector2f(bx, by), sf::Vector2f(bw, bh), 2.0f, 1.5f);

        sf::Text t;
        t.setFont(font);
        t.setCharacterSize(17);
        t.setStyle(sf::Text::Bold);
        bool isRed = (banCo.getLuotChoi() == Mau::DO);
        t.setString(toUtf8(isRed ? Loc::get(LocKey::TURN_RED) : Loc::get(LocKey::TURN_BLACK)));
        t.setFillColor(isRed ? sf::Color(255, 140, 140) : sf::Color(150, 210, 255));
        sf::FloatRect tb = t.getLocalBounds();
        t.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
        t.setPosition(1920.0f / 2.0f, by + bh / 2.0f);
        window.draw(t);
    }
}

void GameManager::veThanhBenTrai() {
    float px = 70.0f;
    float py = 140.0f;
    float pw = 460.0f;
    float ph = 800.0f;
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

    // 1. Panel Background
    sf::RectangleShape panel(sf::Vector2f(pw, ph));
    panel.setPosition(px, py);
    panel.setFillColor(sf::Color(35, 25, 20, 245));
    panel.setOutlineThickness(2.0f);
    panel.setOutlineColor(sf::Color(115, 75, 40));
    window.draw(panel);
    ve4GocLuaChon(window, sf::Vector2f(px, py), sf::Vector2f(pw, ph), 2.5f, 2.0f);

    // 2. Ribbon Header: Bên Đỏ (Tiên hành)
    sf::RectangleShape rib(sf::Vector2f(pw - 30.0f, 34.0f));
    rib.setPosition(px + 15.0f, py + 15.0f);
    rib.setFillColor(sf::Color(65, 22, 22, 230));
    rib.setOutlineThickness(1.2f);
    rib.setOutlineColor(sf::Color(160, 50, 50));
    window.draw(rib);

    sf::Text tRib;
    tRib.setFont(font);
    tRib.setCharacterSize(16);
    tRib.setStyle(sf::Text::Bold);
    tRib.setString(toUtf8(tiengViet ? "BÊN ĐỎ (TIÊN HÀNH)" : "RED PLAYER (FIRST)"));
    tRib.setFillColor(sf::Color(255, 140, 140));
    sf::FloatRect rtb = tRib.getLocalBounds();
    tRib.setOrigin(rtb.left + rtb.width / 2.0f, rtb.top + rtb.height / 2.0f);
    tRib.setPosition(px + pw / 2.0f, py + 31.0f);
    window.draw(tRib);

    // 3. Player 1 Profile Card (Red)
    sf::RectangleShape profBox(sf::Vector2f(pw - 30.0f, 130.0f));
    profBox.setPosition(px + 15.0f, py + 58.0f);
    profBox.setFillColor(sf::Color(45, 22, 22, 230));
    profBox.setOutlineThickness(isRedTurn ? 2.5f : 1.2f);
    profBox.setOutlineColor(isRedTurn ? sf::Color(255, 75, 75) : sf::Color(90, 45, 45));
    window.draw(profBox);

    // Avatar Frame & Image
    float avaX = px + 28.0f;
    float avaY = py + 68.0f;
    float avaSz = 110.0f;
    sf::RectangleShape avaBg(sf::Vector2f(avaSz, avaSz));
    avaBg.setPosition(avaX, avaY);
    avaBg.setFillColor(sf::Color(20, 14, 14, 255));
    avaBg.setOutlineThickness(1.5f);
    avaBg.setOutlineColor(isRedTurn ? sf::Color(240, 90, 90) : sf::Color(100, 50, 50));
    window.draw(avaBg);

    if (daTaiAvatarTextures && nhanVatDo >= 0 && nhanVatDo < 5 && avatarTextures[nhanVatDo].getSize().x > 0) {
        sf::Sprite spAva(avatarTextures[nhanVatDo]);
        spAva.setPosition(avaX + 5.0f, avaY + 5.0f);
        spAva.setScale(100.0f / static_cast<float>(avatarTextures[nhanVatDo].getSize().x),
                       100.0f / static_cast<float>(avatarTextures[nhanVatDo].getSize().y));
        window.draw(spAva);
    }

    // Name, Title, Turn Badge
    sf::Text rName;
    rName.setFont(font);
    rName.setCharacterSize(22);
    rName.setStyle(sf::Text::Bold);
    rName.setString(toUtf8(tenNguoiChoiDo));
    rName.setFillColor(sf::Color(255, 230, 230));
    rName.setPosition(px + 152.0f, py + 70.0f);
    window.draw(rName);

    sf::Text rChar;
    rChar.setFont(font);
    rChar.setCharacterSize(15);
    rChar.setStyle(sf::Text::Bold);
    rChar.setString(toUtf8("[" + Loc::getCharName(nhanVatDo) + "]"));
    rChar.setFillColor(sf::Color(245, 195, 150));
    rChar.setPosition(px + 152.0f, py + 104.0f);
    window.draw(rChar);

    sf::Text rBadge;
    rBadge.setFont(font);
    rBadge.setCharacterSize(16);
    rBadge.setStyle(sf::Text::Bold);
    rBadge.setString(toUtf8(isRedTurn ? (tiengViet ? "[ ĐẾN LƯỢT ĐI ]" : "[ YOUR TURN ]")
                                      : (tiengViet ? "[ Chờ đối thủ ]" : "[ Waiting ]")));
    rBadge.setFillColor(isRedTurn ? sf::Color(100, 255, 140) : sf::Color(170, 160, 160));
    rBadge.setPosition(px + 152.0f, py + 138.0f);
    window.draw(rBadge);

    // 4. Timer Box
    sf::RectangleShape timeBox(sf::Vector2f(pw - 30.0f, 60.0f));
    timeBox.setPosition(px + 15.0f, py + 198.0f);
    timeBox.setFillColor(sf::Color(26, 18, 14, 230));
    timeBox.setOutlineThickness(1.0f);
    timeBox.setOutlineColor(sf::Color(100, 70, 40));
    window.draw(timeBox);

    sf::Text tTimeLbl;
    tTimeLbl.setFont(font);
    tTimeLbl.setCharacterSize(15);
    tTimeLbl.setStyle(sf::Text::Bold);
    tTimeLbl.setString(toUtf8(tiengViet ? "THỜI GIAN CÒN LẠI:" : "TIME REMAINING:"));
    tTimeLbl.setFillColor(sf::Color(210, 190, 160));
    tTimeLbl.setPosition(px + 30.0f, py + 218.0f);
    window.draw(tTimeLbl);

    sf::Text tTimeVal;
    tTimeVal.setFont(font);
    tTimeVal.setCharacterSize(30);
    tTimeVal.setStyle(sf::Text::Bold);
    tTimeVal.setString(toUtf8(formatTime(thoiGianConDo)));
    tTimeVal.setFillColor(isRedTurn ? sf::Color(255, 220, 90) : sf::Color(200, 200, 200));
    sf::FloatRect tvb = tTimeVal.getLocalBounds();
    tTimeVal.setOrigin(tvb.left + tvb.width, tvb.top + tvb.height / 2.0f);
    tTimeVal.setPosition(px + pw - 35.0f, py + 228.0f);
    window.draw(tTimeVal);

    // Captured pieces
    sf::RectangleShape capBox(sf::Vector2f(pw - 30.0f, 95.0f));
    capBox.setPosition(px + 15.0f, py + 268.0f);
    capBox.setFillColor(sf::Color(26, 18, 14, 230));
    capBox.setOutlineThickness(1.0f);
    capBox.setOutlineColor(sf::Color(100, 70, 40));
    window.draw(capBox);

    sf::Text capTitle;
    capTitle.setFont(font);
    capTitle.setCharacterSize(14);
    capTitle.setStyle(sf::Text::Bold);
    capTitle.setString(toUtf8(tiengViet ? "QUÂN ĐÃ BẮT ĐƯỢC (TÙ BINH):" : "CAPTURED PIECES:"));
    capTitle.setFillColor(sf::Color(200, 180, 150));
    capTitle.setPosition(px + 25.0f, py + 276.0f);
    window.draw(capTitle);

    int capCount = 0;
    float startChipX = px + 28.0f;
    float startChipY = py + 302.0f;
    for (const auto& q : banCo.getCacQuan()) {
        if (q && q->getDaBiAn() && q->getMau() == Mau::DEN) {
            float chipR = 14.0f;
            sf::CircleShape chip(chipR);
            chip.setPosition(startChipX + (capCount % 12) * 33.0f, startChipY + (capCount / 12) * 30.0f);
            chip.setFillColor(sf::Color(35, 42, 52));
            chip.setOutlineThickness(1.5f);
            chip.setOutlineColor(sf::Color(80, 110, 145));
            window.draw(chip);

            sf::Text chipTxt;
            chipTxt.setFont(font);
            std::string code = layKyHieuQuanTheoStyle(q->layTen(), Mau::DEN);
            if (code.size() > 2) code = code.substr(0, 2);
            chipTxt.setString(toUtf8(code));
            chipTxt.setCharacterSize(11);
            chipTxt.setStyle(sf::Text::Bold);
            chipTxt.setFillColor(sf::Color(180, 210, 245));
            sf::FloatRect ctb = chipTxt.getLocalBounds();
            chipTxt.setOrigin(ctb.left + ctb.width / 2.0f, ctb.top + ctb.height / 2.0f);
            chipTxt.setPosition(chip.getPosition().x + chipR, chip.getPosition().y + chipR);
            window.draw(chipTxt);

            capCount++;
            if (capCount >= 24) break;
        }
    }
    if (capCount == 0) {
        sf::Text noCap;
        noCap.setFont(font);
        noCap.setCharacterSize(14);
        noCap.setStyle(sf::Text::Bold);
        noCap.setString(toUtf8(tiengViet ? "[ Chưa bắt được quân nào ]" : "[ No pieces captured ]"));
        noCap.setFillColor(sf::Color(170, 155, 140));
        noCap.setPosition(px + 30.0f, py + 312.0f);
        window.draw(noCap);
    }

    // 6. Controls Guide Box
    sf::RectangleShape guideBox(sf::Vector2f(pw - 30.0f, 136.0f));
    guideBox.setPosition(px + 15.0f, py + 373.0f);
    guideBox.setFillColor(sf::Color(26, 18, 14, 230));
    guideBox.setOutlineThickness(1.0f);
    guideBox.setOutlineColor(sf::Color(90, 65, 35));
    window.draw(guideBox);

    sf::Text gTitle;
    gTitle.setFont(font);
    gTitle.setCharacterSize(14);
    gTitle.setStyle(sf::Text::Bold);
    gTitle.setString(toUtf8(tiengViet ? "HƯỚNG DẪN ĐIỀU KHIỂN & PHÍM TẮT:" : "CONTROLS & SHORTCUTS:"));
    gTitle.setFillColor(sf::Color(240, 215, 120));
    gTitle.setPosition(px + 25.0f, py + 382.0f);
    window.draw(gTitle);

    const std::vector<std::string> shortcuts = {
        tiengViet ? "• [ W, A, S, D / Mũi tên ]: Di chuyển ô chọn" : "• [ W, A, S, D / Arrows ]: Move cursor",
        tiengViet ? "• [ Space / Enter ]: Chọn / Hạ quân cờ" : "• [ Space / Enter ]: Select / Place piece",
        tiengViet ? "• [ Chuột trái ]: Chọn & Đi  |  [ Phải ]: Hủy" : "• [ Left Click ]: Select & Move | [ Right ]: Cancel",
        tiengViet ? "• [ ESC ]: Mở Bảng menu trận đấu" : "• [ ESC ]: Open Match Menu"
    };

    for (size_t i = 0; i < shortcuts.size(); ++i) {
        sf::Text sTxt;
        sTxt.setFont(font);
        sTxt.setCharacterSize(14);
        sTxt.setStyle(sf::Text::Bold);
        sTxt.setString(toUtf8(shortcuts[i]));
        sTxt.setFillColor(sf::Color(250, 235, 205));
        sTxt.setPosition(px + 25.0f, py + 406.0f + i * 23.0f);
        window.draw(sTxt);
    }

    // Left panel buttons
    for (auto& btn : inGameButtons) {
        btn->draw(window);
    }
}

void GameManager::veThanhBenPhai() {
    float px = 1390.0f;
    float py = 140.0f;
    float pw = 460.0f;
    float ph = 800.0f;
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

    // 1. Panel Background
    sf::RectangleShape panel(sf::Vector2f(pw, ph));
    panel.setPosition(px, py);
    panel.setFillColor(sf::Color(35, 25, 20, 245));
    panel.setOutlineThickness(2.0f);
    panel.setOutlineColor(sf::Color(115, 75, 40));
    window.draw(panel);
    ve4GocLuaChon(window, sf::Vector2f(px, py), sf::Vector2f(pw, ph), 2.5f, 2.0f);

    // 2. Ribbon Header: Bên Đen
    sf::RectangleShape rib(sf::Vector2f(pw - 30.0f, 34.0f));
    rib.setPosition(px + 15.0f, py + 15.0f);
    rib.setFillColor(sf::Color(22, 32, 50, 230));
    rib.setOutlineThickness(1.2f);
    rib.setOutlineColor(sf::Color(55, 80, 130));
    window.draw(rib);

    sf::Text tRib;
    tRib.setFont(font);
    tRib.setCharacterSize(16);
    tRib.setStyle(sf::Text::Bold);
    std::string ribStr = (cheDoChoi == CheDoChoi::VOI_MAY)
                         ? (tiengViet ? "BÊN ĐEN (MÁY AI)" : "BLACK (AI BOT)")
                         : (tiengViet ? "BÊN ĐEN (HẬU THỦ)" : "BLACK PLAYER (SECOND)");
    tRib.setString(toUtf8(ribStr));
    tRib.setFillColor(sf::Color(140, 200, 255));
    sf::FloatRect rtb = tRib.getLocalBounds();
    tRib.setOrigin(rtb.left + rtb.width / 2.0f, rtb.top + rtb.height / 2.0f);
    tRib.setPosition(px + pw / 2.0f, py + 31.0f);
    window.draw(tRib);

    // 3. Player 2 Profile Card (Black)
    sf::RectangleShape profBox(sf::Vector2f(pw - 30.0f, 130.0f));
    profBox.setPosition(px + 15.0f, py + 58.0f);
    profBox.setFillColor(sf::Color(22, 26, 38, 230));
    profBox.setOutlineThickness(isBlackTurn ? 2.5f : 1.2f);
    profBox.setOutlineColor(isBlackTurn ? sf::Color(80, 160, 255) : sf::Color(45, 55, 75));
    window.draw(profBox);

    // Avatar Frame & Image
    float avaX = px + 28.0f;
    float avaY = py + 68.0f;
    float avaSz = 110.0f;
    sf::RectangleShape avaBg(sf::Vector2f(avaSz, avaSz));
    avaBg.setPosition(avaX, avaY);
    avaBg.setFillColor(sf::Color(14, 18, 25, 255));
    avaBg.setOutlineThickness(1.5f);
    avaBg.setOutlineColor(isBlackTurn ? sf::Color(90, 170, 255) : sf::Color(50, 65, 90));
    window.draw(avaBg);

    if (daTaiAvatarTextures && nhanVatDen >= 0 && nhanVatDen < 5 && avatarTextures[nhanVatDen].getSize().x > 0) {
        sf::Sprite spAva(avatarTextures[nhanVatDen]);
        spAva.setPosition(avaX + 5.0f, avaY + 5.0f);
        spAva.setScale(100.0f / static_cast<float>(avatarTextures[nhanVatDen].getSize().x),
                       100.0f / static_cast<float>(avatarTextures[nhanVatDen].getSize().y));
        window.draw(spAva);
    }

    // Name, Title, Turn Badge
    sf::Text bName;
    bName.setFont(font);
    bName.setCharacterSize(22);
    bName.setStyle(sf::Text::Bold);
    bName.setString(toUtf8(tenNguoiChoiDen));
    bName.setFillColor(sf::Color(220, 240, 255));
    bName.setPosition(px + 152.0f, py + 70.0f);
    window.draw(bName);

    sf::Text bChar;
    bChar.setFont(font);
    bChar.setCharacterSize(15);
    bChar.setStyle(sf::Text::Bold);
    if (cheDoChoi == CheDoChoi::VOI_MAY) {
        std::string diffStr = (aiDifficulty == DoKho::DE) ? (tiengViet ? "Mức độ: Dễ" : "Diff: Easy")
                            : (aiDifficulty == DoKho::KHO) ? (tiengViet ? "Mức độ: Khó" : "Diff: Hard")
                            : (tiengViet ? "Mức độ: Trung bình" : "Diff: Medium");
        bChar.setString(toUtf8("[" + diffStr + "]"));
    } else {
        bChar.setString(toUtf8("[" + Loc::getCharName(nhanVatDen) + "]"));
    }
    bChar.setFillColor(sf::Color(165, 200, 240));
    bChar.setPosition(px + 152.0f, py + 104.0f);
    window.draw(bChar);

    sf::Text bBadge;
    bBadge.setFont(font);
    bBadge.setCharacterSize(16);
    bBadge.setStyle(sf::Text::Bold);
    bBadge.setString(toUtf8(isBlackTurn ? (aiDangSuyNghi ? (tiengViet ? "[ AI ĐANG NGHĨ... ]" : "[ AI THINKING... ]")
                                                         : (tiengViet ? "[ ĐẾN LƯỢT ĐI ]" : "[ YOUR TURN ]"))
                                        : (tiengViet ? "[ Chờ đối thủ ]" : "[ Waiting ]")));
    bBadge.setFillColor(isBlackTurn ? sf::Color(100, 210, 255) : sf::Color(170, 160, 160));
    bBadge.setPosition(px + 152.0f, py + 138.0f);
    window.draw(bBadge);

    // 4. Timer Box
    sf::RectangleShape timeBox(sf::Vector2f(pw - 30.0f, 60.0f));
    timeBox.setPosition(px + 15.0f, py + 198.0f);
    timeBox.setFillColor(sf::Color(26, 18, 14, 230));
    timeBox.setOutlineThickness(1.0f);
    timeBox.setOutlineColor(sf::Color(100, 70, 40));
    window.draw(timeBox);

    sf::Text tTimeLbl;
    tTimeLbl.setFont(font);
    tTimeLbl.setCharacterSize(15);
    tTimeLbl.setStyle(sf::Text::Bold);
    tTimeLbl.setString(toUtf8(tiengViet ? "THỜI GIAN CÒN LẠI:" : "TIME REMAINING:"));
    tTimeLbl.setFillColor(sf::Color(210, 190, 160));
    tTimeLbl.setPosition(px + 30.0f, py + 218.0f);
    window.draw(tTimeLbl);

    sf::Text tTimeVal;
    tTimeVal.setFont(font);
    tTimeVal.setCharacterSize(30);
    tTimeVal.setStyle(sf::Text::Bold);
    tTimeVal.setString(toUtf8(formatTime(thoiGianConDen)));
    tTimeVal.setFillColor(isBlackTurn ? sf::Color(255, 220, 90) : sf::Color(200, 200, 200));
    sf::FloatRect tvb = tTimeVal.getLocalBounds();
    tTimeVal.setOrigin(tvb.left + tvb.width, tvb.top + tvb.height / 2.0f);
    tTimeVal.setPosition(px + pw - 35.0f, py + 228.0f);
    window.draw(tTimeVal);

    // Captured pieces
    sf::RectangleShape capBox(sf::Vector2f(pw - 30.0f, 95.0f));
    capBox.setPosition(px + 15.0f, py + 268.0f);
    capBox.setFillColor(sf::Color(26, 18, 14, 230));
    capBox.setOutlineThickness(1.0f);
    capBox.setOutlineColor(sf::Color(100, 70, 40));
    window.draw(capBox);

    sf::Text capTitle;
    capTitle.setFont(font);
    capTitle.setCharacterSize(14);
    capTitle.setStyle(sf::Text::Bold);
    capTitle.setString(toUtf8(tiengViet ? "QUÂN ĐÃ BẮT ĐƯỢC (TÙ BINH):" : "CAPTURED PIECES:"));
    capTitle.setFillColor(sf::Color(200, 180, 150));
    capTitle.setPosition(px + 25.0f, py + 276.0f);
    window.draw(capTitle);

    int capCount = 0;
    float startChipX = px + 28.0f;
    float startChipY = py + 302.0f;
    for (const auto& q : banCo.getCacQuan()) {
        if (q && q->getDaBiAn() && q->getMau() == Mau::DO) {
            float chipR = 14.0f;
            sf::CircleShape chip(chipR);
            chip.setPosition(startChipX + (capCount % 12) * 33.0f, startChipY + (capCount / 12) * 30.0f);
            chip.setFillColor(sf::Color(55, 25, 25));
            chip.setOutlineThickness(1.5f);
            chip.setOutlineColor(sf::Color(160, 60, 60));
            window.draw(chip);

            sf::Text chipTxt;
            chipTxt.setFont(font);
            std::string code = layKyHieuQuanTheoStyle(q->layTen(), Mau::DO);
            if (code.size() > 2) code = code.substr(0, 2);
            chipTxt.setString(toUtf8(code));
            chipTxt.setCharacterSize(11);
            chipTxt.setStyle(sf::Text::Bold);
            chipTxt.setFillColor(sf::Color(255, 140, 140));
            sf::FloatRect ctb = chipTxt.getLocalBounds();
            chipTxt.setOrigin(ctb.left + ctb.width / 2.0f, ctb.top + ctb.height / 2.0f);
            chipTxt.setPosition(chip.getPosition().x + chipR, chip.getPosition().y + chipR);
            window.draw(chipTxt);

            capCount++;
            if (capCount >= 24) break;
        }
    }
    if (capCount == 0) {
        sf::Text noCap;
        noCap.setFont(font);
        noCap.setCharacterSize(14);
        noCap.setStyle(sf::Text::Bold);
        noCap.setString(toUtf8(tiengViet ? "[ Chưa bắt được quân nào ]" : "[ No pieces captured ]"));
        noCap.setFillColor(sf::Color(170, 155, 140));
        noCap.setPosition(px + 30.0f, py + 312.0f);
        window.draw(noCap);
    }

    // 6. Move History Box
    float histH = 405.0f;
    sf::RectangleShape histBox(sf::Vector2f(pw - 30.0f, histH));
    histBox.setPosition(px + 15.0f, py + 373.0f);
    histBox.setFillColor(sf::Color(26, 18, 14, 230));
    histBox.setOutlineThickness(1.0f);
    histBox.setOutlineColor(sf::Color(100, 70, 40));
    window.draw(histBox);

    sf::Text histTitle;
    histTitle.setFont(font);
    histTitle.setCharacterSize(14);
    histTitle.setStyle(sf::Text::Bold);
    histTitle.setString(toUtf8(tiengViet ? "📜 LỊCH SỬ NƯỚC ĐI" : "📜 MOVE HISTORY"));
    histTitle.setFillColor(sf::Color(240, 215, 120));
    histTitle.setPosition(px + 25.0f, py + 382.0f);
    window.draw(histTitle);

    sf::RectangleShape innerList(sf::Vector2f(pw - 50.0f, 350.0f));
    innerList.setPosition(px + 25.0f, py + 408.0f);
    innerList.setFillColor(sf::Color(18, 12, 9, 230));
    innerList.setOutlineThickness(1.0f);
    innerList.setOutlineColor(sf::Color(70, 50, 30));
    window.draw(innerList);

    const auto& lichSu = banCo.getLichSuNuocDi();
    size_t count = lichSu.size();
    size_t maxDisplay = 14;
    size_t start = (count > maxDisplay) ? (count - maxDisplay) : 0;
    float textY = py + 416.0f;

    if (lichSu.empty()) {
        sf::Text emptyT;
        emptyT.setFont(font);
        emptyT.setCharacterSize(15);
        emptyT.setStyle(sf::Text::Bold);
        emptyT.setString(toUtf8(Loc::get(LocKey::NO_MOVES_YET)));
        emptyT.setFillColor(sf::Color(170, 155, 140));
        emptyT.setPosition(px + 45.0f, textY + 20.0f);
        window.draw(emptyT);
    } else {
        for (size_t i = start; i < count; ++i) {
            if ((i - start) % 2 == 1) {
                sf::RectangleShape strip(sf::Vector2f(pw - 54.0f, 22.0f));
                strip.setPosition(px + 27.0f, textY - 2.0f);
                strip.setFillColor(sf::Color(255, 255, 255, 8));
                window.draw(strip);
            }

            sf::Text ndText;
            ndText.setFont(font);
            ndText.setCharacterSize(15);
            ndText.setStyle(sf::Text::Bold);
            std::string line = std::to_string(i + 1) + ". " + banCo.layMoTaNuocDi(lichSu[i]);
            ndText.setString(toUtf8(line));
            ndText.setFillColor((lichSu[i].mauQuan == Mau::DO) ? sf::Color(255, 140, 140) : sf::Color(160, 215, 255));
            ndText.setPosition(px + 36.0f, textY);
            window.draw(ndText);
            textY += 23.0f;
        }
    }
}

void GameManager::vePopupKetThuc() {
    if (!daPhatAmKetThuc) {
        phatAmThanhKetThuc();
    }

    // Dark overlay
    sf::RectangleShape overlay(sf::Vector2f(1920.0f, 1080.0f));
    overlay.setFillColor(sf::Color(0, 0, 0, 195));
    window.draw(overlay);

    const float dw = 760.0f;
    const float dh = 520.0f;
    const float dx = (1920.0f - dw) / 2.0f;
    const float dy = (1080.0f - dh) / 2.0f;

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

    // 2. Bóng đổ ngoài bảng thông báo
    sf::RectangleShape shadow(sf::Vector2f(dw + 10.0f, dh + 10.0f));
    shadow.setPosition(dx - 5.0f, dy - 5.0f);
    shadow.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(shadow);

    // 3. Khung hộp thông báo chính
    sf::RectangleShape dialog(sf::Vector2f(dw, dh));
    dialog.setPosition(dx, dy);
    dialog.setFillColor(sf::Color(32, 23, 18, 252));
    dialog.setOutlineThickness(3.5f);
    dialog.setOutlineColor(borderColor);
    window.draw(dialog);

    // 4. Viền chỉ vàng mảnh bên trong
    sf::RectangleShape inner(sf::Vector2f(dw - 14.0f, dh - 14.0f));
    inner.setPosition(dx + 7.0f, dy + 7.0f);
    inner.setFillColor(sf::Color(22, 16, 12, 230));
    inner.setOutlineThickness(1.2f);
    inner.setOutlineColor(sf::Color(140, 100, 55, 180));
    window.draw(inner);

    // 5. Hiệu ứng 4 góc bracket hoàng gia
    ve4GocLuaChon(window, sf::Vector2f(dx, dy), sf::Vector2f(dw, dh), 3.5f, 2.2f);

    // Result badge
    const float badgeW = 460.0f;
    const float badgeH = 44.0f;
    const float badgeX = dx + (dw - badgeW) / 2.0f;
    const float badgeY = dy + 22.0f;

    sf::RectangleShape badgeBox(sf::Vector2f(badgeW, badgeH));
    badgeBox.setPosition(badgeX, badgeY);
    badgeBox.setFillColor(badgeBgColor);
    badgeBox.setOutlineThickness(1.8f);
    badgeBox.setOutlineColor(borderColor);
    window.draw(badgeBox);

    sf::Text tBadge;
    tBadge.setFont(fTitle);
    tBadge.setCharacterSize(22);
    tBadge.setStyle(sf::Text::Bold);
    tBadge.setString(toUtf8(badgeStr));
    tBadge.setFillColor(titleColor);
    sf::FloatRect bb = tBadge.getLocalBounds();
    tBadge.setOrigin(bb.left + bb.width / 2.0f, bb.top + bb.height / 2.0f);
    tBadge.setPosition(badgeX + badgeW / 2.0f, badgeY + badgeH / 2.0f);
    window.draw(tBadge);

    // 7. Dòng phụ đề thông điệp kết quả
    sf::Text tSub;
    tSub.setFont(fBodyBold);
    tSub.setCharacterSize(16);
    tSub.setStyle(sf::Text::Bold);
    tSub.setString(toUtf8(subHeaderStr));
    tSub.setFillColor(sf::Color(245, 230, 200));
    sf::FloatRect sb = tSub.getLocalBounds();
    tSub.setOrigin(sb.left + sb.width / 2.0f, sb.top + sb.height / 2.0f);
    tSub.setPosition(dx + dw / 2.0f, dy + 84.0f);
    window.draw(tSub);

    // Match statistics
    const float statW = dw - 70.0f;
    const float statH = 228.0f;
    const float statX = dx + 35.0f;
    const float statY = dy + 112.0f;

    sf::RectangleShape statBox(sf::Vector2f(statW, statH));
    statBox.setPosition(statX, statY);
    statBox.setFillColor(sf::Color(16, 11, 8, 235));
    statBox.setOutlineThickness(1.0f);
    statBox.setOutlineColor(sf::Color(75, 52, 35));
    window.draw(statBox);

    // Tiêu đề bảng thống kê
    sf::Text sHeader;
    sHeader.setFont(fBodyBold);
    sHeader.setCharacterSize(14);
    sHeader.setStyle(sf::Text::Bold);
    sHeader.setString(toUtf8(tiengViet ? "--- TỔNG KẾT VÁN ĐẤU ---" : "--- MATCH REPORT ---"));
    sHeader.setFillColor(sf::Color(210, 175, 105));
    sf::FloatRect shb = sHeader.getLocalBounds();
    sHeader.setOrigin(shb.left + shb.width / 2.0f, shb.top + shb.height / 2.0f);
    sHeader.setPosition(statX + statW / 2.0f, statY + 18.0f);
    window.draw(sHeader);

    // Đường kẻ phân cách
    sf::RectangleShape line(sf::Vector2f(statW - 36.0f, 1.0f));
    line.setPosition(statX + 18.0f, statY + 32.0f);
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

    float curRowY = statY + 44.0f;
    for (size_t r = 0; r < rows.size(); ++r) {
        if (r % 2 == 1) {
            sf::RectangleShape rowStrip(sf::Vector2f(statW - 16.0f, 26.0f));
            rowStrip.setPosition(statX + 8.0f, curRowY - 2.0f);
            rowStrip.setFillColor(sf::Color(255, 255, 255, 6));
            window.draw(rowStrip);
        }

        sf::Text tLbl;
        tLbl.setFont(fBodyBold);
        tLbl.setCharacterSize(15);
        tLbl.setStyle(sf::Text::Bold);
        tLbl.setString(toUtf8(rows[r].label));
        tLbl.setFillColor(sf::Color(215, 195, 170));
        tLbl.setPosition(statX + 24.0f, curRowY + 2.0f);
        window.draw(tLbl);

        sf::Text tVal;
        tVal.setFont(fBodyBold);
        tVal.setCharacterSize(15);
        tVal.setStyle(sf::Text::Bold);
        tVal.setString(toUtf8(rows[r].val));
        tVal.setFillColor(rows[r].valCol);
        sf::FloatRect vb = tVal.getLocalBounds();
        tVal.setPosition(statX + statW - 24.0f - vb.width, curRowY + 2.0f);
        window.draw(tVal);

        curRowY += 34.0f;
    }

    // Action buttons
    for (auto& btn : popupButtons) {
        btn->draw(window);
    }

    // Shortcut prompts
    sf::Text tHint;
    tHint.setFont(fBodyBold);
    tHint.setCharacterSize(14);
    tHint.setStyle(sf::Text::Bold);
    tHint.setString(toUtf8(tiengViet ? "[ Phím Enter: Bắt đầu ván mới  |  Phím ESC: Về Menu chính ]"
                                     : "[ Enter: Play Again  |  ESC: Main Menu ]"));
    tHint.setFillColor(sf::Color(220, 190, 150));
    sf::FloatRect hb = tHint.getLocalBounds();
    tHint.setOrigin(hb.left + hb.width / 2.0f, hb.top + hb.height / 2.0f);
    tHint.setPosition(dx + dw / 2.0f, dy + dh - 16.0f);
    window.draw(tHint);
}

void GameManager::veMenuPopUpInGame() {
    sf::RectangleShape overlay(sf::Vector2f(1920.0f, 1080.0f));
    overlay.setFillColor(sf::Color(0, 0, 0, 195));
    window.draw(overlay);

    const float dw = 580.0f;
    const float dh = 600.0f;
    const float dx = (1920.0f - dw) / 2.0f;
    const float dy = (1080.0f - dh) / 2.0f;

    // Outer shadow / glow
    sf::RectangleShape shadowBox(sf::Vector2f(dw + 8.0f, dh + 8.0f));
    shadowBox.setPosition(dx - 4.0f, dy - 4.0f);
    shadowBox.setFillColor(sf::Color(0, 0, 0, 140));
    window.draw(shadowBox);

    // Modal background
    sf::RectangleShape dialog(sf::Vector2f(dw, dh));
    dialog.setPosition(dx, dy);
    dialog.setFillColor(sf::Color(35, 26, 20, 252));
    dialog.setOutlineThickness(3.0f);
    dialog.setOutlineColor(sf::Color(220, 180, 80));
    window.draw(dialog);

    // Inner subtle outline
    sf::RectangleShape innerBorder(sf::Vector2f(dw - 12.0f, dh - 12.0f));
    innerBorder.setPosition(dx + 6.0f, dy + 6.0f);
    innerBorder.setFillColor(sf::Color(24, 18, 14, 230));
    innerBorder.setOutlineThickness(1.0f);
    innerBorder.setOutlineColor(sf::Color(130, 95, 50, 180));
    window.draw(innerBorder);

    // 4 decorative corners
    ve4GocLuaChon(window, sf::Vector2f(dx, dy), sf::Vector2f(dw, dh), 3.0f, 2.0f);

    // Header title
    sf::Text tTitle;
    tTitle.setFont(font);
    tTitle.setCharacterSize(28);
    tTitle.setStyle(sf::Text::Bold);
    tTitle.setString(toUtf8(Loc::get(LocKey::PAUSE_MENU_TITLE)));
    tTitle.setFillColor(sf::Color(255, 220, 100));
    sf::FloatRect tb = tTitle.getLocalBounds();
    tTitle.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
    tTitle.setPosition(1920.0f / 2.0f, dy + 38.0f);
    window.draw(tTitle);

    // Subtitle
    sf::Text tSub;
    tSub.setFont(font);
    tSub.setCharacterSize(15);
    tSub.setStyle(sf::Text::Bold);
    tSub.setString(toUtf8(Loc::get(LocKey::MATCH_PAUSED_SUBTITLE)));
    tSub.setFillColor(sf::Color(240, 220, 180));
    sf::FloatRect sb = tSub.getLocalBounds();
    tSub.setOrigin(sb.left + sb.width / 2.0f, sb.top + sb.height / 2.0f);
    tSub.setPosition(1920.0f / 2.0f, dy + 68.0f);
    window.draw(tSub);

    // Decorative divider line
    sf::RectangleShape line(sf::Vector2f(dw - 60.0f, 2.0f));
    line.setPosition(dx + 30.0f, dy + 88.0f);
    line.setFillColor(sf::Color(160, 115, 60));
    window.draw(line);

    // Draw popup menu buttons
    for (auto& btn : inGameMenuButtons) {
        btn->draw(window);
    }
}

void GameManager::veToast() {
    if (toastTimer <= 0.0f || toastMessage.empty()) return;

    sf::Text t;
    t.setFont(font);
    t.setCharacterSize(18);
    t.setString(toUtf8(toastMessage));
    t.setFillColor(sf::Color::White);

    sf::FloatRect tb = t.getLocalBounds();
    float tw = tb.width + 36.0f;
    float th = 42.0f;
    float tx = (1920.0f - tw) / 2.0f;
    float ty = 1080.0f - 65.0f;

    sf::RectangleShape bg(sf::Vector2f(tw, th));
    bg.setPosition(tx, ty);
    bg.setFillColor(sf::Color(16, 22, 32, 235));
    bg.setOutlineThickness(1.5f);
    bg.setOutlineColor(sf::Color(80, 160, 240));

    t.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
    t.setPosition(tx + tw / 2.0f, ty + th / 2.0f);

    window.draw(bg);
    window.draw(t);
}

    // Screen renderers
void GameManager::veMenuChinh() {
    
    
    
    bool isVietnamese = (Loc::getLanguage() == Language::TIENG_VIET);
    menuChinh.draw(window, isVietnamese);
}

void GameManager::vePlaySubmenu() {
    sf::Text title;
    title.setFont(font);
    title.setString(toUtf8(Loc::get(LocKey::PLAY_SUB_TITLE)));
    title.setCharacterSize(52);
    title.setFillColor(sf::Color(240, 190, 70));
    title.setStyle(sf::Text::Bold);
    
    sf::FloatRect tb = title.getLocalBounds();
    title.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
    title.setPosition(1920.0f / 2.0f, 110.0f);
    window.draw(title);

    sf::RectangleShape goldLine(sf::Vector2f(320.0f, 3.0f));
    goldLine.setPosition((1920.0f - 320.0f) / 2.0f, 150.0f);
    goldLine.setFillColor(sf::Color(240, 190, 70));
    window.draw(goldLine);

    menuPlaySub.draw(window);
}

    // Play setup

void GameManager::khoiTaoPlaySetupButtons() {
    playSetupButtons.clear();
    auto& mAssets = getMenuAssets();
    bool tiengViet = (Loc::getLanguage() == Language::TIENG_VIET);
    sf::Font& f = mAssets.vnFontLoaded ? mAssets.vnFont : font;
    sf::Font& fBold = mAssets.fontBoldLoaded ? mAssets.fontBold : fontBold;

    // Left page
    const float lBtnX = 535.0f;
    const float lBtnW = 380.0f;
    const float lBtnH = 50.0f;
    const float lStartBtnY = 320.0f;
    const float lSpacing = 72.0f;

    const std::vector<std::pair<std::string, const sf::Texture*>> modes = {
        { tiengViet ? "Đấu Với Máy (PvAI)" : "Player vs AI", &mAssets.iconSettings },
        { tiengViet ? "Hai Người Chơi (PvP)" : "Two Players (PvP)", &mAssets.iconPlay },
        { tiengViet ? "Xem Máy Đấu (Demo)" : "Bot vs Bot (Demo)", &mAssets.iconInfo }
    };

    for (int i = 0; i < 3; ++i) {
        float by = lStartBtnY + i * lSpacing;
        auto btn = std::make_unique<Button>(lBtnX, by, lBtnW, lBtnH, modes[i].first, f, [this, i]() {
            soundManager.play(SoundType::CLICK);
            playSetupMode = i;
            khoiTaoPlaySetupButtons();
        });
        btn->setSelected(playSetupMode == i);
        btn->setCharacterSize(19);
        btn->setShapeColor(sf::Color(242, 230, 212, (playSetupMode == i) ? 255 : 210));
        btn->setOutline(1.2f, (playSetupMode == i) ? sf::Color(190, 125, 60) : sf::Color(175, 140, 100, 160));
        btn->setColors(
            (playSetupMode == i) ? sf::Color(185, 25, 20) : sf::Color(55, 30, 15),
            sf::Color(185, 25, 20),
            sf::Color(130, 20, 15)
        );
        btn->setIcon(modes[i].second);
        playSetupButtons.push_back(std::move(btn));
    }

    // 2. RIGHT PAGE CONTROLS
    const float rCtrlX = 1195.0f;
    const float rCtrlW = 184.0f;
    const float rArrowW = 28.0f;
    const float rRowH = 32.0f;

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
        return tiengViet ? "Trúc Xanh" : "Warm Bamboo";
    };

    auto getTimerLabel = [this, tiengViet]() -> std::string {
        if (thoiGianVanDau == 0.0f) return tiengViet ? "Không Giới Hạn" : "Unlimited";
        if (thoiGianVanDau == 300.0f) return tiengViet ? "5 Phút / Bên" : "5 Mins";
        if (thoiGianVanDau == 600.0f) return tiengViet ? "10 Phút / Bên" : "10 Mins";
        if (thoiGianVanDau == 900.0f) return tiengViet ? "15 Phút / Bên" : "15 Mins";
        return tiengViet ? "30 Phút / Bên" : "30 Mins";
    };

    // MODE 0: PvAI
    if (playSetupMode == 0) {
        inputAiPlayerName->setPosition(1045.0f, 318.0f);
        inputAiPlayerName->setSize(140.0f, 34.0f);
        inputAiPlayerName->setParchmentStyle(true);

        // Avatar selector
        auto avaPrev = std::make_unique<Button>(1195.0f, 318.0f, 26.0f, 34.0f, "<", f, [this]() {
            soundManager.play(SoundType::CLICK);
            aiPlayerCharIndex = (aiPlayerCharIndex + Loc::getCharCount() - 1) % Loc::getCharCount();
            khoiTaoPlaySetupButtons();
        });
        avaPrev->setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        avaPrev->setShapeColor(sf::Color(242, 230, 212, 220));
        avaPrev->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(avaPrev));

        auto avaVal = std::make_unique<Button>(1224.0f, 318.0f, 155.0f, 34.0f, Loc::getCharName(aiPlayerCharIndex), f, [this]() {
            soundManager.play(SoundType::CLICK);
            aiPlayerCharIndex = (aiPlayerCharIndex + 1) % Loc::getCharCount();
            khoiTaoPlaySetupButtons();
        });
        avaVal->setCharacterSize(14);
        avaVal->setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        avaVal->setShapeColor(sf::Color(242, 230, 212, 220));
        avaVal->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(avaVal));

        auto avaNext = std::make_unique<Button>(1382.0f, 318.0f, 26.0f, 34.0f, ">", f, [this]() {
            soundManager.play(SoundType::CLICK);
            aiPlayerCharIndex = (aiPlayerCharIndex + 1) % Loc::getCharCount();
            khoiTaoPlaySetupButtons();
        });
        avaNext->setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        avaNext->setShapeColor(sf::Color(242, 230, 212, 220));
        avaNext->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(avaNext));

        // AI Difficulty (Y = 362)
        auto getDiffLabel = [this, tiengViet]() -> std::string {
            if (aiDifficulty == DoKho::DE) return tiengViet ? "Dễ" : "Easy";
            if (aiDifficulty == DoKho::TRUNG_BINH) return tiengViet ? "Trung Bình" : "Medium";
            return tiengViet ? "Khó" : "Hard";
        };

        float yDiff = 362.0f;
        auto diffPrev = std::make_unique<Button>(rCtrlX, yDiff, rArrowW, rRowH, "<", f, [this]() {
            soundManager.play(SoundType::CLICK);
            if (aiDifficulty == DoKho::KHO) aiDifficulty = DoKho::TRUNG_BINH;
            else if (aiDifficulty == DoKho::TRUNG_BINH) aiDifficulty = DoKho::DE;
            else aiDifficulty = DoKho::KHO;
            khoiTaoPlaySetupButtons();
        });
        diffPrev->setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        diffPrev->setShapeColor(sf::Color(242, 230, 212, 220));
        diffPrev->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(diffPrev));

        auto diffVal = std::make_unique<Button>(rCtrlX + rArrowW + 3.0f, yDiff, rCtrlW, rRowH, getDiffLabel(), f, [this]() {
            soundManager.play(SoundType::CLICK);
            if (aiDifficulty == DoKho::DE) aiDifficulty = DoKho::TRUNG_BINH;
            else if (aiDifficulty == DoKho::TRUNG_BINH) aiDifficulty = DoKho::KHO;
            else aiDifficulty = DoKho::DE;
            khoiTaoPlaySetupButtons();
        });
        diffVal->setCharacterSize(14);
        diffVal->setColors(sf::Color(160, 30, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        diffVal->setShapeColor(sf::Color(248, 238, 224, 230));
        diffVal->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(diffVal));

        auto diffNext = std::make_unique<Button>(rCtrlX + rArrowW + 3.0f + rCtrlW + 3.0f, yDiff, rArrowW, rRowH, ">", f, [this]() {
            soundManager.play(SoundType::CLICK);
            if (aiDifficulty == DoKho::DE) aiDifficulty = DoKho::TRUNG_BINH;
            else if (aiDifficulty == DoKho::TRUNG_BINH) aiDifficulty = DoKho::KHO;
            else aiDifficulty = DoKho::DE;
            khoiTaoPlaySetupButtons();
        });
        diffNext->setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        diffNext->setShapeColor(sf::Color(242, 230, 212, 220));
        diffNext->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(diffNext));

        // Side choice (Y = 402)
        auto getSideLabel = [this, tiengViet]() -> std::string {
            if (aiFirstChoice == 0) return tiengViet ? "Người Chơi (Đỏ)" : "Player (Red)";
            if (aiFirstChoice == 1) return tiengViet ? "Máy Đi Trước (Đỏ)" : "AI First (Red)";
            return tiengViet ? "Ngẫu Nhiên" : "Random Side";
        };

        float ySide = 402.0f;
        auto sidePrev = std::make_unique<Button>(rCtrlX, ySide, rArrowW, rRowH, "<", f, [this]() {
            soundManager.play(SoundType::CLICK);
            aiFirstChoice = (aiFirstChoice + 2) % 3;
            khoiTaoPlaySetupButtons();
        });
        sidePrev->setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        sidePrev->setShapeColor(sf::Color(242, 230, 212, 220));
        sidePrev->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(sidePrev));

        auto sideVal = std::make_unique<Button>(rCtrlX + rArrowW + 3.0f, ySide, rCtrlW, rRowH, getSideLabel(), f, [this]() {
            soundManager.play(SoundType::CLICK);
            aiFirstChoice = (aiFirstChoice + 1) % 3;
            khoiTaoPlaySetupButtons();
        });
        sideVal->setCharacterSize(14);
        sideVal->setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        sideVal->setShapeColor(sf::Color(248, 238, 224, 230));
        sideVal->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(sideVal));

        auto sideNext = std::make_unique<Button>(rCtrlX + rArrowW + 3.0f + rCtrlW + 3.0f, ySide, rArrowW, rRowH, ">", f, [this]() {
            soundManager.play(SoundType::CLICK);
            aiFirstChoice = (aiFirstChoice + 1) % 3;
            khoiTaoPlaySetupButtons();
        });
        sideNext->setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        sideNext->setShapeColor(sf::Color(242, 230, 212, 220));
        sideNext->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(sideNext));

        // Piece Style (Y = 442)
        float yPiece = 442.0f;
        auto piecePrev = std::make_unique<Button>(rCtrlX, yPiece, rArrowW, rRowH, "<", f, [this]() {
            soundManager.play(SoundType::CLICK);
            int cur = static_cast<int>(pieceStyle);
            pieceStyle = static_cast<PieceStyle>((cur + 3) % 4);
            khoiTaoPlaySetupButtons();
        });
        piecePrev->setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        piecePrev->setShapeColor(sf::Color(242, 230, 212, 220));
        piecePrev->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(piecePrev));

        auto pieceVal = std::make_unique<Button>(rCtrlX + rArrowW + 3.0f, yPiece, rCtrlW, rRowH, getPieceStyleLabel(), f, [this]() {
            soundManager.play(SoundType::CLICK);
            int cur = static_cast<int>(pieceStyle);
            pieceStyle = static_cast<PieceStyle>((cur + 1) % 4);
            khoiTaoPlaySetupButtons();
        });
        pieceVal->setCharacterSize(14);
        pieceVal->setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        pieceVal->setShapeColor(sf::Color(248, 238, 224, 230));
        pieceVal->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(pieceVal));

        auto pieceNext = std::make_unique<Button>(rCtrlX + rArrowW + 3.0f + rCtrlW + 3.0f, yPiece, rArrowW, rRowH, ">", f, [this]() {
            soundManager.play(SoundType::CLICK);
            int cur = static_cast<int>(pieceStyle);
            pieceStyle = static_cast<PieceStyle>((cur + 1) % 4);
            khoiTaoPlaySetupButtons();
        });
        pieceNext->setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        pieceNext->setShapeColor(sf::Color(242, 230, 212, 220));
        pieceNext->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(pieceNext));

        // Board theme
        float yBoard = 536.0f;
        auto boardPrev = std::make_unique<Button>(rCtrlX, yBoard, rArrowW, rRowH, "<", f, [this]() {
            soundManager.play(SoundType::CLICK);
            int cur = static_cast<int>(boardTheme);
            boardTheme = static_cast<BoardTheme>((cur + 3) % 4);
            khoiTaoPlaySetupButtons();
        });
        boardPrev->setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        boardPrev->setShapeColor(sf::Color(242, 230, 212, 220));
        boardPrev->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(boardPrev));

        auto boardVal = std::make_unique<Button>(rCtrlX + rArrowW + 3.0f, yBoard, rCtrlW, rRowH, getBoardThemeLabel(), f, [this]() {
            soundManager.play(SoundType::CLICK);
            int cur = static_cast<int>(boardTheme);
            boardTheme = static_cast<BoardTheme>((cur + 1) % 4);
            khoiTaoPlaySetupButtons();
        });
        boardVal->setCharacterSize(14);
        boardVal->setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        boardVal->setShapeColor(sf::Color(248, 238, 224, 230));
        boardVal->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(boardVal));

        auto boardNext = std::make_unique<Button>(rCtrlX + rArrowW + 3.0f + rCtrlW + 3.0f, yBoard, rArrowW, rRowH, ">", f, [this]() {
            soundManager.play(SoundType::CLICK);
            int cur = static_cast<int>(boardTheme);
            boardTheme = static_cast<BoardTheme>((cur + 1) % 4);
            khoiTaoPlaySetupButtons();
        });
        boardNext->setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        boardNext->setShapeColor(sf::Color(242, 230, 212, 220));
        boardNext->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(boardNext));

        // Match Timer (Y = 576)
        float yTimer = 576.0f;
        auto timerPrev = std::make_unique<Button>(rCtrlX, yTimer, rArrowW, rRowH, "<", f, [this]() {
            soundManager.play(SoundType::CLICK);
            if (thoiGianVanDau == 0.0f) thoiGianVanDau = 1800.0f;
            else if (thoiGianVanDau == 300.0f) thoiGianVanDau = 0.0f;
            else if (thoiGianVanDau == 600.0f) thoiGianVanDau = 300.0f;
            else if (thoiGianVanDau == 900.0f) thoiGianVanDau = 600.0f;
            else thoiGianVanDau = 900.0f;
            khoiTaoPlaySetupButtons();
        });
        timerPrev->setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        timerPrev->setShapeColor(sf::Color(242, 230, 212, 220));
        timerPrev->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(timerPrev));

        auto timerVal = std::make_unique<Button>(rCtrlX + rArrowW + 3.0f, yTimer, rCtrlW, rRowH, getTimerLabel(), f, [this]() {
            soundManager.play(SoundType::CLICK);
            if (thoiGianVanDau == 0.0f) thoiGianVanDau = 300.0f;
            else if (thoiGianVanDau == 300.0f) thoiGianVanDau = 600.0f;
            else if (thoiGianVanDau == 600.0f) thoiGianVanDau = 900.0f;
            else if (thoiGianVanDau == 900.0f) thoiGianVanDau = 1800.0f;
            else thoiGianVanDau = 0.0f;
            khoiTaoPlaySetupButtons();
        });
        timerVal->setCharacterSize(14);
        timerVal->setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        timerVal->setShapeColor(sf::Color(248, 238, 224, 230));
        timerVal->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(timerVal));

        auto timerNext = std::make_unique<Button>(rCtrlX + rArrowW + 3.0f + rCtrlW + 3.0f, yTimer, rArrowW, rRowH, ">", f, [this]() {
            soundManager.play(SoundType::CLICK);
            if (thoiGianVanDau == 0.0f) thoiGianVanDau = 300.0f;
            else if (thoiGianVanDau == 300.0f) thoiGianVanDau = 600.0f;
            else if (thoiGianVanDau == 600.0f) thoiGianVanDau = 900.0f;
            else if (thoiGianVanDau == 900.0f) thoiGianVanDau = 1800.0f;
            else thoiGianVanDau = 0.0f;
            khoiTaoPlaySetupButtons();
        });
        timerNext->setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        timerNext->setShapeColor(sf::Color(242, 230, 212, 220));
        timerNext->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(timerNext));

        // King Aura (Y = 616)
        float yAura = 616.0f;
        auto auraPrev = std::make_unique<Button>(rCtrlX, yAura, rArrowW, rRowH, "<", f, [this]() {
            soundManager.play(SoundType::CLICK);
            p1Aura = static_cast<PieceAura>((static_cast<int>(p1Aura) + 4) % 5);
            khoiTaoPlaySetupButtons();
        });
        auraPrev->setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        auraPrev->setShapeColor(sf::Color(242, 230, 212, 220));
        auraPrev->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(auraPrev));

        auto auraVal = std::make_unique<Button>(rCtrlX + rArrowW + 3.0f, yAura, rCtrlW, rRowH, layTenPieceAura(p1Aura), f, [this]() {
            soundManager.play(SoundType::CLICK);
            p1Aura = static_cast<PieceAura>((static_cast<int>(p1Aura) + 1) % 5);
            khoiTaoPlaySetupButtons();
        });
        auraVal->setCharacterSize(14);
        auraVal->setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        auraVal->setShapeColor(sf::Color(248, 238, 224, 230));
        auraVal->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(auraVal));

        auto auraNext = std::make_unique<Button>(rCtrlX + rArrowW + 3.0f + rCtrlW + 3.0f, yAura, rArrowW, rRowH, ">", f, [this]() {
            soundManager.play(SoundType::CLICK);
            p1Aura = static_cast<PieceAura>((static_cast<int>(p1Aura) + 1) % 5);
            khoiTaoPlaySetupButtons();
        });
        auraNext->setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        auraNext->setShapeColor(sf::Color(242, 230, 212, 220));
        auraNext->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(auraNext));

        // Start Match Button (Y = 660)
        auto startBtn = std::make_unique<Button>(1045.0f, 660.0f, 390.0f, 48.0f,
            tiengViet ? "⚔  BẮT ĐẦU VÁN ĐẤU  ⚔" : "⚔  START MATCH  ⚔", f, [this]() {
            soundManager.play(SoundType::CLICK);
            batDauPvai();
        });
        startBtn->setTextures(&mAssets.frameNormal, &mAssets.frameHover);
        startBtn->setCharacterSize(18);
        startBtn->setColors(sf::Color(180, 25, 20), sf::Color(215, 35, 25), sf::Color(120, 20, 15));
        playSetupButtons.push_back(std::move(startBtn));
    }

    // MODE 1: PvP
    else if (playSetupMode == 1) {
        // Player 1 (Red)
        inputP1Name->setPosition(1045.0f, 310.0f);
        inputP1Name->setSize(140.0f, 32.0f);
        inputP1Name->setParchmentStyle(true);

        auto p1AvaPrev = std::make_unique<Button>(1195.0f, 310.0f, 26.0f, 32.0f, "<", f, [this]() {
            soundManager.play(SoundType::CLICK);
            p1CharIndex = (p1CharIndex + Loc::getCharCount() - 1) % Loc::getCharCount();
            khoiTaoPlaySetupButtons();
        });
        p1AvaPrev->setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        p1AvaPrev->setShapeColor(sf::Color(242, 230, 212, 220));
        p1AvaPrev->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(p1AvaPrev));

        auto p1AvaVal = std::make_unique<Button>(1224.0f, 310.0f, 155.0f, 32.0f, Loc::getCharName(p1CharIndex), f, [this]() {
            soundManager.play(SoundType::CLICK);
            p1CharIndex = (p1CharIndex + 1) % Loc::getCharCount();
            khoiTaoPlaySetupButtons();
        });
        p1AvaVal->setCharacterSize(14);
        p1AvaVal->setColors(sf::Color(170, 30, 20), sf::Color(200, 35, 25), sf::Color(120, 20, 15));
        p1AvaVal->setShapeColor(sf::Color(242, 230, 212, 220));
        p1AvaVal->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(p1AvaVal));

        auto p1AvaNext = std::make_unique<Button>(1382.0f, 310.0f, 26.0f, 32.0f, ">", f, [this]() {
            soundManager.play(SoundType::CLICK);
            p1CharIndex = (p1CharIndex + 1) % Loc::getCharCount();
            khoiTaoPlaySetupButtons();
        });
        p1AvaNext->setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        p1AvaNext->setShapeColor(sf::Color(242, 230, 212, 220));
        p1AvaNext->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(p1AvaNext));

        // Player 2 (Black)
        inputP2Name->setPosition(1045.0f, 368.0f);
        inputP2Name->setSize(140.0f, 32.0f);
        inputP2Name->setParchmentStyle(true);

        auto p2AvaPrev = std::make_unique<Button>(1195.0f, 368.0f, 26.0f, 32.0f, "<", f, [this]() {
            soundManager.play(SoundType::CLICK);
            p2CharIndex = (p2CharIndex + Loc::getCharCount() - 1) % Loc::getCharCount();
            khoiTaoPlaySetupButtons();
        });
        p2AvaPrev->setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        p2AvaPrev->setShapeColor(sf::Color(242, 230, 212, 220));
        p2AvaPrev->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(p2AvaPrev));

        auto p2AvaVal = std::make_unique<Button>(1224.0f, 368.0f, 155.0f, 32.0f, Loc::getCharName(p2CharIndex), f, [this]() {
            soundManager.play(SoundType::CLICK);
            p2CharIndex = (p2CharIndex + 1) % Loc::getCharCount();
            khoiTaoPlaySetupButtons();
        });
        p2AvaVal->setCharacterSize(14);
        p2AvaVal->setColors(sf::Color(35, 60, 120), sf::Color(50, 80, 150), sf::Color(20, 40, 90));
        p2AvaVal->setShapeColor(sf::Color(242, 230, 212, 220));
        p2AvaVal->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(p2AvaVal));

        auto p2AvaNext = std::make_unique<Button>(1382.0f, 368.0f, 26.0f, 32.0f, ">", f, [this]() {
            soundManager.play(SoundType::CLICK);
            p2CharIndex = (p2CharIndex + 1) % Loc::getCharCount();
            khoiTaoPlaySetupButtons();
        });
        p2AvaNext->setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        p2AvaNext->setShapeColor(sf::Color(242, 230, 212, 220));
        p2AvaNext->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(p2AvaNext));

        // Toss Coin (Y = 416)
        auto tossBtn = std::make_unique<Button>(1045.0f, 416.0f, 390.0f, 34.0f,
            tiengViet ? "🎲  Tung đồng xu chọn bên đi trước" : "🎲  Toss Coin For First Move", f, [this, tiengViet]() {
            soundManager.play(SoundType::CAPTURE);
            bool p1Red = (rand() % 2 == 0);
            pvpFirstPlayer = p1Red ? 1 : 2;
            std::string n1 = inputP1Name->getValue().empty() ? "Player 1" : inputP1Name->getValue();
            std::string n2 = inputP2Name->getValue().empty() ? "Player 2" : inputP2Name->getValue();
            pvpRollBanner = p1Red
                ? (tiengViet ? ("▶ " + n1 + " nhận quân ĐỎ và đi trước!") : ("▶ " + n1 + " plays RED and moves first!"))
                : (tiengViet ? ("▶ " + n2 + " nhận quân ĐỎ và đi trước!") : ("▶ " + n2 + " plays RED and moves first!"));
        });
        tossBtn->setCharacterSize(14);
        tossBtn->setColors(sf::Color(135, 80, 25), sf::Color(180, 110, 35), sf::Color(100, 60, 20));
        tossBtn->setShapeColor(sf::Color(250, 240, 225, 230));
        tossBtn->setOutline(1.2f, sf::Color(190, 140, 80));
        playSetupButtons.push_back(std::move(tossBtn));

        // Piece Style (Y = 444)
        float yPiece = 444.0f;
        auto piecePrev = std::make_unique<Button>(rCtrlX, yPiece, rArrowW, rRowH, "<", f, [this]() {
            soundManager.play(SoundType::CLICK);
            int cur = static_cast<int>(pieceStyle);
            pieceStyle = static_cast<PieceStyle>((cur + 3) % 4);
            khoiTaoPlaySetupButtons();
        });
        piecePrev->setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        piecePrev->setShapeColor(sf::Color(242, 230, 212, 220));
        piecePrev->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(piecePrev));

        auto pieceVal = std::make_unique<Button>(rCtrlX + rArrowW + 3.0f, yPiece, rCtrlW, rRowH, getPieceStyleLabel(), f, [this]() {
            soundManager.play(SoundType::CLICK);
            int cur = static_cast<int>(pieceStyle);
            pieceStyle = static_cast<PieceStyle>((cur + 1) % 4);
            khoiTaoPlaySetupButtons();
        });
        pieceVal->setCharacterSize(14);
        pieceVal->setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        pieceVal->setShapeColor(sf::Color(248, 238, 224, 230));
        pieceVal->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(pieceVal));

        auto pieceNext = std::make_unique<Button>(rCtrlX + rArrowW + 3.0f + rCtrlW + 3.0f, yPiece, rArrowW, rRowH, ">", f, [this]() {
            soundManager.play(SoundType::CLICK);
            int cur = static_cast<int>(pieceStyle);
            pieceStyle = static_cast<PieceStyle>((cur + 1) % 4);
            khoiTaoPlaySetupButtons();
        });
        pieceNext->setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        pieceNext->setShapeColor(sf::Color(242, 230, 212, 220));
        pieceNext->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(pieceNext));

        // Board theme
        float yBoard = 536.0f;
        auto boardPrev = std::make_unique<Button>(rCtrlX, yBoard, rArrowW, rRowH, "<", f, [this]() {
            soundManager.play(SoundType::CLICK);
            int cur = static_cast<int>(boardTheme);
            boardTheme = static_cast<BoardTheme>((cur + 3) % 4);
            khoiTaoPlaySetupButtons();
        });
        boardPrev->setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        boardPrev->setShapeColor(sf::Color(242, 230, 212, 220));
        boardPrev->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(boardPrev));

        auto boardVal = std::make_unique<Button>(rCtrlX + rArrowW + 3.0f, yBoard, rCtrlW, rRowH, getBoardThemeLabel(), f, [this]() {
            soundManager.play(SoundType::CLICK);
            int cur = static_cast<int>(boardTheme);
            boardTheme = static_cast<BoardTheme>((cur + 1) % 4);
            khoiTaoPlaySetupButtons();
        });
        boardVal->setCharacterSize(14);
        boardVal->setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        boardVal->setShapeColor(sf::Color(248, 238, 224, 230));
        boardVal->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(boardVal));

        auto boardNext = std::make_unique<Button>(rCtrlX + rArrowW + 3.0f + rCtrlW + 3.0f, yBoard, rArrowW, rRowH, ">", f, [this]() {
            soundManager.play(SoundType::CLICK);
            int cur = static_cast<int>(boardTheme);
            boardTheme = static_cast<BoardTheme>((cur + 1) % 4);
            khoiTaoPlaySetupButtons();
        });
        boardNext->setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        boardNext->setShapeColor(sf::Color(242, 230, 212, 220));
        boardNext->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(boardNext));

        // Match Timer (Y = 576)
        float yTimer = 576.0f;
        auto timerPrev = std::make_unique<Button>(rCtrlX, yTimer, rArrowW, rRowH, "<", f, [this]() {
            soundManager.play(SoundType::CLICK);
            if (thoiGianVanDau == 0.0f) thoiGianVanDau = 1800.0f;
            else if (thoiGianVanDau == 300.0f) thoiGianVanDau = 0.0f;
            else if (thoiGianVanDau == 600.0f) thoiGianVanDau = 300.0f;
            else if (thoiGianVanDau == 900.0f) thoiGianVanDau = 600.0f;
            else thoiGianVanDau = 900.0f;
            khoiTaoPlaySetupButtons();
        });
        timerPrev->setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        timerPrev->setShapeColor(sf::Color(242, 230, 212, 220));
        timerPrev->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(timerPrev));

        auto timerVal = std::make_unique<Button>(rCtrlX + rArrowW + 3.0f, yTimer, rCtrlW, rRowH, getTimerLabel(), f, [this]() {
            soundManager.play(SoundType::CLICK);
            if (thoiGianVanDau == 0.0f) thoiGianVanDau = 300.0f;
            else if (thoiGianVanDau == 300.0f) thoiGianVanDau = 600.0f;
            else if (thoiGianVanDau == 600.0f) thoiGianVanDau = 900.0f;
            else if (thoiGianVanDau == 900.0f) thoiGianVanDau = 1800.0f;
            else thoiGianVanDau = 0.0f;
            khoiTaoPlaySetupButtons();
        });
        timerVal->setCharacterSize(14);
        timerVal->setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        timerVal->setShapeColor(sf::Color(248, 238, 224, 230));
        timerVal->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(timerVal));

        auto timerNext = std::make_unique<Button>(rCtrlX + rArrowW + 3.0f + rCtrlW + 3.0f, yTimer, rArrowW, rRowH, ">", f, [this]() {
            soundManager.play(SoundType::CLICK);
            if (thoiGianVanDau == 0.0f) thoiGianVanDau = 300.0f;
            else if (thoiGianVanDau == 300.0f) thoiGianVanDau = 600.0f;
            else if (thoiGianVanDau == 600.0f) thoiGianVanDau = 900.0f;
            else if (thoiGianVanDau == 900.0f) thoiGianVanDau = 1800.0f;
            else thoiGianVanDau = 0.0f;
            khoiTaoPlaySetupButtons();
        });
        timerNext->setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        timerNext->setShapeColor(sf::Color(242, 230, 212, 220));
        timerNext->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(timerNext));

        // King Aura (Y = 616)
        float yAura = 616.0f;
        auto auraPrev = std::make_unique<Button>(rCtrlX, yAura, rArrowW, rRowH, "<", f, [this]() {
            soundManager.play(SoundType::CLICK);
            p1Aura = static_cast<PieceAura>((static_cast<int>(p1Aura) + 4) % 5);
            khoiTaoPlaySetupButtons();
        });
        auraPrev->setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        auraPrev->setShapeColor(sf::Color(242, 230, 212, 220));
        auraPrev->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(auraPrev));

        auto auraVal = std::make_unique<Button>(rCtrlX + rArrowW + 3.0f, yAura, rCtrlW, rRowH, layTenPieceAura(p1Aura), f, [this]() {
            soundManager.play(SoundType::CLICK);
            p1Aura = static_cast<PieceAura>((static_cast<int>(p1Aura) + 1) % 5);
            khoiTaoPlaySetupButtons();
        });
        auraVal->setCharacterSize(14);
        auraVal->setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        auraVal->setShapeColor(sf::Color(248, 238, 224, 230));
        auraVal->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(auraVal));

        auto auraNext = std::make_unique<Button>(rCtrlX + rArrowW + 3.0f + rCtrlW + 3.0f, yAura, rArrowW, rRowH, ">", f, [this]() {
            soundManager.play(SoundType::CLICK);
            p1Aura = static_cast<PieceAura>((static_cast<int>(p1Aura) + 1) % 5);
            khoiTaoPlaySetupButtons();
        });
        auraNext->setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        auraNext->setShapeColor(sf::Color(242, 230, 212, 220));
        auraNext->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(auraNext));

        // Start Match Button (Y = 660)
        auto startBtn = std::make_unique<Button>(1045.0f, 660.0f, 390.0f, 48.0f,
            tiengViet ? "⚔  BẮT ĐẦU VÁN ĐẤU  ⚔" : "⚔  START MATCH  ⚔", f, [this]() {
            soundManager.play(SoundType::CLICK);
            batDauPvp();
        });
        startBtn->setTextures(&mAssets.frameNormal, &mAssets.frameHover);
        startBtn->setCharacterSize(18);
        startBtn->setColors(sf::Color(180, 25, 20), sf::Color(215, 35, 25), sf::Color(120, 20, 15));
        playSetupButtons.push_back(std::move(startBtn));
    }

    // MODE 2: Demo
    else if (playSetupMode == 2) {
        // Piece Style (Y = 430)
        float yPiece = 430.0f;
        auto piecePrev = std::make_unique<Button>(rCtrlX, yPiece, rArrowW, rRowH, "<", f, [this]() {
            soundManager.play(SoundType::CLICK);
            int cur = static_cast<int>(pieceStyle);
            pieceStyle = static_cast<PieceStyle>((cur + 3) % 4);
            khoiTaoPlaySetupButtons();
        });
        piecePrev->setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        piecePrev->setShapeColor(sf::Color(242, 230, 212, 220));
        piecePrev->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(piecePrev));

        auto pieceVal = std::make_unique<Button>(rCtrlX + rArrowW + 3.0f, yPiece, rCtrlW, rRowH, getPieceStyleLabel(), f, [this]() {
            soundManager.play(SoundType::CLICK);
            int cur = static_cast<int>(pieceStyle);
            pieceStyle = static_cast<PieceStyle>((cur + 1) % 4);
            khoiTaoPlaySetupButtons();
        });
        pieceVal->setCharacterSize(14);
        pieceVal->setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        pieceVal->setShapeColor(sf::Color(248, 238, 224, 230));
        pieceVal->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(pieceVal));

        auto pieceNext = std::make_unique<Button>(rCtrlX + rArrowW + 3.0f + rCtrlW + 3.0f, yPiece, rArrowW, rRowH, ">", f, [this]() {
            soundManager.play(SoundType::CLICK);
            int cur = static_cast<int>(pieceStyle);
            pieceStyle = static_cast<PieceStyle>((cur + 1) % 4);
            khoiTaoPlaySetupButtons();
        });
        pieceNext->setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        pieceNext->setShapeColor(sf::Color(242, 230, 212, 220));
        pieceNext->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(pieceNext));

        // Board theme
        float yBoard = 530.0f;
        auto boardPrev = std::make_unique<Button>(rCtrlX, yBoard, rArrowW, rRowH, "<", f, [this]() {
            soundManager.play(SoundType::CLICK);
            int cur = static_cast<int>(boardTheme);
            boardTheme = static_cast<BoardTheme>((cur + 3) % 4);
            khoiTaoPlaySetupButtons();
        });
        boardPrev->setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        boardPrev->setShapeColor(sf::Color(242, 230, 212, 220));
        boardPrev->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(boardPrev));

        auto boardVal = std::make_unique<Button>(rCtrlX + rArrowW + 3.0f, yBoard, rCtrlW, rRowH, getBoardThemeLabel(), f, [this]() {
            soundManager.play(SoundType::CLICK);
            int cur = static_cast<int>(boardTheme);
            boardTheme = static_cast<BoardTheme>((cur + 1) % 4);
            khoiTaoPlaySetupButtons();
        });
        boardVal->setCharacterSize(14);
        boardVal->setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        boardVal->setShapeColor(sf::Color(248, 238, 224, 230));
        boardVal->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(boardVal));

        auto boardNext = std::make_unique<Button>(rCtrlX + rArrowW + 3.0f + rCtrlW + 3.0f, yBoard, rArrowW, rRowH, ">", f, [this]() {
            soundManager.play(SoundType::CLICK);
            int cur = static_cast<int>(boardTheme);
            boardTheme = static_cast<BoardTheme>((cur + 1) % 4);
            khoiTaoPlaySetupButtons();
        });
        boardNext->setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
        boardNext->setShapeColor(sf::Color(242, 230, 212, 220));
        boardNext->setOutline(1.0f, sf::Color(175, 140, 100));
        playSetupButtons.push_back(std::move(boardNext));

        // Start Demo Button (Y = 610)
        auto startBtn = std::make_unique<Button>(1045.0f, 610.0f, 390.0f, 50.0f,
            tiengViet ? "⚔  BẮT ĐẦU XEM MÔ PHỎNG  ⚔" : "⚔  START SIMULATION  ⚔", f, [this, tiengViet]() {
            soundManager.play(SoundType::CLICK);
            tenNguoiChoiDo = tiengViet ? "Quân Đỏ (Xe & Voi)" : "Red (Chariot & Elephant)";
            tenNguoiChoiDen = tiengViet ? "Quân Đen (Xe & Voi)" : "Black (Chariot & Elephant)";
            nhanVatDo = 0;
            nhanVatDen = 0;
            batDauTroChoiMoi(CheDoChoi::DEMO_2XE_2VOI);
        });
        startBtn->setTextures(&mAssets.frameNormal, &mAssets.frameHover);
        startBtn->setCharacterSize(17);
        startBtn->setColors(sf::Color(180, 25, 20), sf::Color(215, 35, 25), sf::Color(120, 20, 15));
        playSetupButtons.push_back(std::move(startBtn));
    }
}

void GameManager::vePieceStylePreview(sf::RenderWindow& win, float x, float y, float w, float h) {
    auto& assets = getMenuAssets();
    sf::Font& f = assets.vnFontLoaded ? assets.vnFont : font;
    bool vi = (Loc::getLanguage() == Language::TIENG_VIET);

    // 1. Khung hộp parchment cổ điển
    sf::RectangleShape box(sf::Vector2f(w, h));
    box.setPosition(x, y);
    box.setFillColor(sf::Color(252, 244, 230, 245));
    box.setOutlineThickness(1.2f);
    box.setOutlineColor(sf::Color(185, 140, 85));
    win.draw(box);

    // 2. Nhãn hiển thị bên trái
    sf::Text lbl;
    lbl.setFont(f);
    lbl.setString(toUtf8(vi ? "Xem trước quân cờ:" : "Piece Preview:"));
    lbl.setCharacterSize(14);
    lbl.setStyle(sf::Text::Bold);
    lbl.setFillColor(sf::Color(140, 30, 20));
    lbl.setPosition(x + 14.0f, y + (h - 18.0f) / 2.0f);
    win.draw(lbl);

    // 3. Hai quân cờ Tướng mẫu (Đỏ & Đen)
    float r = std::min(19.0f, (h - 8.0f) / 2.0f);
    sf::Vector2f posRed(x + w * 0.65f, y + h / 2.0f);
    sf::Vector2f posBlack(x + w * 0.87f, y + h / 2.0f);

    // Hào quang vương nếu có
    veAuraQuan(posRed, r, p1Aura, true);
    veAuraQuan(posBlack, r, p2Aura, true);

    // --- Tướng Đỏ ---
    const sf::Texture* texRed = (pieceStyle == PieceStyle::REALISTIC_WOOD) ? layTextureQuan("Tuong", Mau::DO) : nullptr;
    if (texRed != nullptr && texRed->getSize().x > 0) {
        sf::CircleShape shadow(r - 1.0f);
        shadow.setOrigin(r - 1.0f, r - 1.0f);
        shadow.setPosition(posRed.x + 1.5f, posRed.y + 2.0f);
        shadow.setFillColor(sf::Color(0, 0, 0, 90));
        win.draw(shadow);

        sf::Sprite sp(*texRed);
        sf::Vector2u sz = texRed->getSize();
        sp.setOrigin(sz.x / 2.0f, sz.y / 2.0f);
        float sc = (r * 2.0f) / static_cast<float>(sz.x);
        sp.setScale(sc, sc);
        sp.setPosition(posRed);
        win.draw(sp);
    } else {
        sf::CircleShape outer(r + 2.0f);
        outer.setOrigin(r + 2.0f, r + 2.0f);
        outer.setPosition(posRed);
        outer.setFillColor(sf::Color(165, 30, 20));
        win.draw(outer);

        sf::CircleShape piece(r);
        piece.setOrigin(r, r);
        piece.setPosition(posRed);
        piece.setFillColor(sf::Color(252, 242, 222));
        win.draw(piece);

        sf::Text sym;
        sym.setFont(f);
        std::string s = layKyHieuQuanTheoStyle("Tuong", Mau::DO);
        sym.setString(toUtf8(s));
        sym.setCharacterSize(s.length() > 2 ? 11 : 13);
        sym.setStyle(sf::Text::Bold);
        sym.setFillColor(sf::Color(175, 25, 20));
        sf::FloatRect sb = sym.getLocalBounds();
        sym.setOrigin(sb.left + sb.width / 2.0f, sb.top + sb.height / 2.0f);
        sym.setPosition(posRed.x, posRed.y - 1.0f);
        win.draw(sym);
    }

    // --- Tướng Đen ---
    const sf::Texture* texBlack = (pieceStyle == PieceStyle::REALISTIC_WOOD) ? layTextureQuan("Tuong", Mau::DEN) : nullptr;
    if (texBlack != nullptr && texBlack->getSize().x > 0) {
        sf::CircleShape shadow(r - 1.0f);
        shadow.setOrigin(r - 1.0f, r - 1.0f);
        shadow.setPosition(posBlack.x + 1.5f, posBlack.y + 2.0f);
        shadow.setFillColor(sf::Color(0, 0, 0, 90));
        win.draw(shadow);

        sf::Sprite sp(*texBlack);
        sf::Vector2u sz = texBlack->getSize();
        sp.setOrigin(sz.x / 2.0f, sz.y / 2.0f);
        float sc = (r * 2.0f) / static_cast<float>(sz.x);
        sp.setScale(sc, sc);
        sp.setPosition(posBlack);
        win.draw(sp);
    } else {
        sf::CircleShape outer(r + 2.0f);
        outer.setOrigin(r + 2.0f, r + 2.0f);
        outer.setPosition(posBlack);
        outer.setFillColor(sf::Color(35, 55, 75));
        win.draw(outer);

        sf::CircleShape piece(r);
        piece.setOrigin(r, r);
        piece.setPosition(posBlack);
        piece.setFillColor(sf::Color(252, 242, 222));
        win.draw(piece);

        sf::Text sym;
        sym.setFont(f);
        std::string s = layKyHieuQuanTheoStyle("Tuong", Mau::DEN);
        sym.setString(toUtf8(s));
        sym.setCharacterSize(s.length() > 2 ? 11 : 13);
        sym.setStyle(sf::Text::Bold);
        sym.setFillColor(sf::Color(25, 45, 65));
        sf::FloatRect sb = sym.getLocalBounds();
        sym.setOrigin(sb.left + sb.width / 2.0f, sb.top + sb.height / 2.0f);
        sym.setPosition(posBlack.x, posBlack.y - 1.0f);
        win.draw(sym);
    }
}

void GameManager::vePlaySetup() {
    auto& assets = getMenuAssets();
    bool tiengViet = (Loc::getLanguage() == Language::TIENG_VIET);
    sf::Font& f = assets.vnFontLoaded ? assets.vnFont : font;
    sf::Font& fBold = assets.fontBoldLoaded ? assets.fontBold : fontBold;

    // Background vignette
    sf::RectangleShape vignette(sf::Vector2f(1920.0f, 1080.0f));
    vignette.setFillColor(sf::Color(10, 8, 15, 65));
    window.draw(vignette);

    // 2. Open Book
    const float bookX = 445.0f;
    const float bookY = 190.0f;
    const float scale = 4.6f;

    if (assets.bookCover.getSize().x > 0) {
        sf::Sprite spCover(assets.bookCover);
        spCover.setPosition(bookX, bookY);
        spCover.setScale(scale, scale);
        window.draw(spCover);
    }
    if (assets.bookPageLeft.getSize().x > 0) {
        sf::Sprite spLeft(assets.bookPageLeft);
        spLeft.setPosition(bookX + 8.0f * scale, bookY + 6.5f * scale);
        spLeft.setScale(scale, scale);
        window.draw(spLeft);
    }
    if (assets.bookPageRight.getSize().x > 0) {
        sf::Sprite spRight(assets.bookPageRight);
        spRight.setPosition(bookX + (8.0f + 104.0f + 8.0f) * scale, bookY + 6.5f * scale);
        spRight.setScale(scale, scale);
        window.draw(spRight);
    }

    sf::RectangleShape spineOpen(sf::Vector2f(20.0f, 736.0f));
    spineOpen.setOrigin(10.0f, 0.0f);
    spineOpen.setPosition(960.0f, bookY);
    spineOpen.setFillColor(sf::Color(0, 0, 0, 40));
    window.draw(spineOpen);

    if (assets.marker.getSize().x > 0) {
        sf::Sprite spMarker(assets.marker);
        spMarker.setPosition(482.0f, 186.0f);
        spMarker.setScale(3.4f, 3.4f);
        window.draw(spMarker);
    }

    // 3. Return Bookmark Tab (Top-Left)
    sf::Vector2f mCoords = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    sf::Vector2i mousePos(static_cast<int>(std::round(mCoords.x)), static_cast<int>(std::round(mCoords.y)));
    sf::FloatRect backTabRect(482.0f, 186.0f, 68.0f, 48.0f);
    bool hoverBackTab = backTabRect.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    if (assets.iconArrow.getSize().x > 0) {
        sf::Sprite spArrow(assets.iconArrow);
        sf::Vector2u asz = assets.iconArrow.getSize();
        spArrow.setOrigin(asz.x / 2.0f, asz.y / 2.0f);
        spArrow.setPosition(516.0f, 210.0f);
        spArrow.setScale(-1.8f, 1.8f); // Đảo chiều sprite để chỉ về bên TRÁI (<-)
        spArrow.setColor(hoverBackTab ? sf::Color(220, 35, 25) : sf::Color(90, 50, 30, 230));
        window.draw(spArrow);
    } else {
        sf::Text backArrow;
        backArrow.setFont(f);
        backArrow.setString(toUtf8("◄"));
        backArrow.setCharacterSize(18);
        backArrow.setStyle(sf::Text::Bold);
        backArrow.setFillColor(hoverBackTab ? sf::Color(210, 35, 25) : sf::Color(85, 50, 30, 220));
        sf::FloatRect bab = backArrow.getLocalBounds();
        backArrow.setOrigin(bab.left + bab.width / 2.0f, bab.top + bab.height / 2.0f);
        backArrow.setPosition(516.0f, 210.0f);
        window.draw(backArrow);
    }

    if (hoverBackTab) {
        sf::RectangleShape tabHighlight(sf::Vector2f(68.0f, 48.0f));
        tabHighlight.setPosition(482.0f, 186.0f);
        tabHighlight.setFillColor(sf::Color(255, 235, 170, 45));
        tabHighlight.setOutlineThickness(2.0f);
        tabHighlight.setOutlineColor(sf::Color(220, 185, 80));
        window.draw(tabHighlight);


        sf::RectangleShape tipBg(sf::Vector2f(105.0f, 26.0f));
        tipBg.setPosition(465.0f, 150.0f);
        tipBg.setFillColor(sf::Color(25, 18, 14, 230));
        tipBg.setOutlineThickness(1.0f);
        tipBg.setOutlineColor(sf::Color(220, 180, 80));
        window.draw(tipBg);

        sf::Text tipText;
        tipText.setFont(f);
        tipText.setString(toUtf8(tiengViet ? "Quay về menu" : "Back to Menu"));
        tipText.setCharacterSize(12);
        tipText.setStyle(sf::Text::Bold);
        tipText.setFillColor(sf::Color(255, 225, 120));
        sf::FloatRect ttb = tipText.getLocalBounds();
        tipText.setOrigin(ttb.left + ttb.width / 2.0f, ttb.top + ttb.height / 2.0f);
        tipText.setPosition(516.0f, 163.0f);
        window.draw(tipText);
    }

    // Top edge tabs
    const sf::Texture* modeTabIcons[3] = { &assets.iconPlay, &assets.iconLoad, &assets.iconInfo };
    for (int i = 0; i < 3; ++i) {
        float tx = 560.0f + i * 55.0f;
        sf::FloatRect tRect(tx, 185.0f, 48.0f, 38.0f);
        bool isCur = (playSetupMode == i);
        bool isHov = tRect.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

        if (isCur && assets.frameHover.getSize().x > 0) {
            sf::Sprite fH(assets.frameHover);
            fH.setPosition(tx - 3.0f, 182.0f);
            fH.setScale(54.0f / static_cast<float>(assets.frameHover.getSize().x),
                        44.0f / static_cast<float>(assets.frameHover.getSize().y));
            window.draw(fH);
        } else if (isHov) {
            sf::RectangleShape hBox(sf::Vector2f(48.0f, 38.0f));
            hBox.setPosition(tx, 185.0f);
            hBox.setFillColor(sf::Color(255, 235, 170, 60));
            window.draw(hBox);
        }

        if (modeTabIcons[i] && modeTabIcons[i]->getSize().x > 0) {
            sf::Sprite ic(*modeTabIcons[i]);
            float icScale = 22.0f / static_cast<float>(modeTabIcons[i]->getSize().y);
            ic.setScale(icScale, icScale);
            ic.setPosition(tx + 13.0f, 193.0f);
            if (isCur) ic.setColor(sf::Color(190, 35, 25));
            window.draw(ic);
        }
    }

    // 4. Left Page Header
    sf::Text leftTitle;
    leftTitle.setFont(f);
    leftTitle.setString(toUtf8(tiengViet ? "CHẾ ĐỘ CHƠI" : "GAME MODES"));
    leftTitle.setCharacterSize(20);
    leftTitle.setStyle(sf::Text::Bold);
    leftTitle.setFillColor(sf::Color(65, 38, 20));
    sf::FloatRect ltb = leftTitle.getLocalBounds();
    leftTitle.setOrigin(ltb.left + ltb.width / 2.0f, ltb.top + ltb.height / 2.0f);
    leftTitle.setPosition(725.0f, 260.0f);
    window.draw(leftTitle);

    if (assets.decLine.getSize().x > 0) {
        sf::Sprite lineL(assets.decLine);
        lineL.setPosition(605.0f, 278.0f);
        lineL.setScale(240.0f / static_cast<float>(assets.decLine.getSize().x), 2.0f);
        window.draw(lineL);
    }

    // Selection frame on active mode bar
    const float lBtnX = 535.0f;
    const float lBtnW = 380.0f;
    const float lBtnH = 50.0f;
    const float lStartBtnY = 320.0f;
    const float lSpacing = 72.0f;
    float activeY = lStartBtnY + playSetupMode * lSpacing;

    if (assets.frameHover.getSize().x > 0) {
        sf::Sprite fHover(assets.frameHover);
        fHover.setPosition(lBtnX - 3.0f, activeY - 3.0f);
        fHover.setScale((lBtnW + 6.0f) / static_cast<float>(assets.frameHover.getSize().x),
                        (lBtnH + 6.0f) / static_cast<float>(assets.frameHover.getSize().y));
        window.draw(fHover);
    }
    ve4GocLuaChon(window, sf::Vector2f(lBtnX, activeY), sf::Vector2f(lBtnW, lBtnH), 2.0f, 1.8f);

    // Left Page Footer
    sf::Text fLText;
    fLText.setFont(fBold);
    fLText.setString(toUtf8(tiengViet ? "W / S: Chọn chế độ  |  A: Menu chính" : "W / S: Select Mode  |  A: Main Menu"));
    fLText.setCharacterSize(14);
    fLText.setStyle(sf::Text::Bold);
    fLText.setFillColor(sf::Color(80, 45, 25));
    fLText.setPosition(535.0f, 695.0f);
    window.draw(fLText);

    // 5. Right Page Header
    std::string rightTitleStr;
    std::string rightSubStr;
    if (playSetupMode == 0) {
        rightTitleStr = tiengViet ? "ĐẤU VỚI MÁY (PvAI)" : "PLAYER VS AI";
        rightSubStr = tiengViet ? "Thử sức với AI Minimax & Alpha-Beta" : "Challenge Minimax & Alpha-Beta AI";
    } else if (playSetupMode == 1) {
        rightTitleStr = tiengViet ? "HAI NGƯỜI CHƠI (PvP)" : "TWO PLAYERS (PvP)";
        rightSubStr = tiengViet ? "So tài trực tiếp trên cùng máy tính" : "Two players compete on the same device";
    } else {
        rightTitleStr = tiengViet ? "XEM MÁY ĐẤU (DEMO)" : "BOT VS BOT DEMO";
        rightSubStr = tiengViet ? "Mô phỏng tự động ván cờ tàn mẫu" : "Auto simulation of classic endgame";
    }

    sf::Text rTitle;
    rTitle.setFont(fBold);
    rTitle.setString(toUtf8(rightTitleStr));
    rTitle.setCharacterSize(20);
    rTitle.setStyle(sf::Text::Bold);
    rTitle.setFillColor(sf::Color(65, 38, 20));
    sf::FloatRect rtb = rTitle.getLocalBounds();
    rTitle.setOrigin(rtb.left + rtb.width / 2.0f, rtb.top + rtb.height / 2.0f);
    rTitle.setPosition(1240.0f, 250.0f);
    window.draw(rTitle);

    sf::Text rSub;
    rSub.setFont(fBold);
    rSub.setString(toUtf8(rightSubStr));
    rSub.setCharacterSize(14);
    rSub.setStyle(sf::Text::Bold);
    rSub.setFillColor(sf::Color(140, 30, 20));
    sf::FloatRect rsb = rSub.getLocalBounds();
    rSub.setOrigin(rsb.left + rsb.width / 2.0f, rsb.top + rsb.height / 2.0f);
    rSub.setPosition(1240.0f, 275.0f);
    window.draw(rSub);

    if (assets.decLine.getSize().x > 0) {
        sf::Sprite lineR(assets.decLine);
        lineR.setPosition(1120.0f, 288.0f);
        lineR.setScale(240.0f / static_cast<float>(assets.decLine.getSize().x), 2.0f);
        window.draw(lineR);
    }

    // Right Page Specific Drawing
    if (playSetupMode == 0) {
        sf::Text lblName;
        lblName.setFont(f);
        lblName.setString(toUtf8(tiengViet ? "Kỳ thủ & Avatar:" : "Player & Avatar:"));
        lblName.setCharacterSize(14);
        lblName.setStyle(sf::Text::Bold);
        lblName.setFillColor(sf::Color(150, 30, 20));
        lblName.setPosition(1045.0f, 298.0f);
        window.draw(lblName);

        inputAiPlayerName->draw(window);

        // Avatar Box
        sf::RectangleShape avaBox(sf::Vector2f(38.0f, 38.0f));
        avaBox.setPosition(1415.0f, 316.0f);
        avaBox.setFillColor(sf::Color(250, 240, 225));
        avaBox.setOutlineThickness(1.5f);
        avaBox.setOutlineColor(sf::Color(185, 135, 75));
        window.draw(avaBox);

        if (daTaiAvatarTextures && aiPlayerCharIndex >= 0 && aiPlayerCharIndex < 5 && avatarTextures[aiPlayerCharIndex].getSize().x > 0) {
            sf::Sprite spAva(avatarTextures[aiPlayerCharIndex]);
            spAva.setPosition(1418.0f, 319.0f);
            spAva.setScale(32.0f / static_cast<float>(avatarTextures[aiPlayerCharIndex].getSize().x),
                           32.0f / static_cast<float>(avatarTextures[aiPlayerCharIndex].getSize().y));
            window.draw(spAva);
        }

        const std::vector<std::string> rowLabels = {
            tiengViet ? "Độ khó AI:" : "AI Difficulty:",
            tiengViet ? "Lượt đi đầu:" : "Turn Order:",
            tiengViet ? "Kiểu quân cờ:" : "Piece Style:",
            tiengViet ? "Màu bàn cờ:" : "Board Theme:",
            tiengViet ? "Thời gian ván:" : "Match Timer:",
            tiengViet ? "Hào quang vương:" : "King Aura:"
        };
        const float rowYs[] = { 362.0f, 402.0f, 442.0f, 536.0f, 576.0f, 616.0f };
        for (int i = 0; i < 6; ++i) {
            sf::Text lbl;
            lbl.setFont(f);
            lbl.setString(toUtf8(rowLabels[i]));
            lbl.setCharacterSize(14);
            lbl.setStyle(sf::Text::Bold);
            lbl.setFillColor(sf::Color(75, 45, 25));
            lbl.setPosition(1045.0f, rowYs[i] + 7.0f);
            window.draw(lbl);
        }

        // Piece preview
        vePieceStylePreview(window, 1045.0f, 480.0f, 390.0f, 48.0f);
    }
    else if (playSetupMode == 1) {
        // P1 Label
        sf::Text lblP1;
        lblP1.setFont(fBold);
        lblP1.setString(toUtf8(tiengViet ? "Kỳ thủ 1 (Quân Đỏ):" : "Player 1 (Red):"));
        lblP1.setCharacterSize(14);
        lblP1.setStyle(sf::Text::Bold);
        lblP1.setFillColor(sf::Color(170, 30, 20));
        lblP1.setPosition(1045.0f, 292.0f);
        window.draw(lblP1);

        inputP1Name->draw(window);

        // P1 Avatar Box
        sf::RectangleShape avaBox1(sf::Vector2f(34.0f, 34.0f));
        avaBox1.setPosition(1415.0f, 308.0f);
        avaBox1.setFillColor(sf::Color(250, 240, 225));
        avaBox1.setOutlineThickness(1.5f);
        avaBox1.setOutlineColor(sf::Color(190, 40, 30));
        window.draw(avaBox1);

        if (daTaiAvatarTextures && p1CharIndex >= 0 && p1CharIndex < 5 && avatarTextures[p1CharIndex].getSize().x > 0) {
            sf::Sprite spAva(avatarTextures[p1CharIndex]);
            spAva.setPosition(1417.0f, 310.0f);
            spAva.setScale(30.0f / static_cast<float>(avatarTextures[p1CharIndex].getSize().x),
                           30.0f / static_cast<float>(avatarTextures[p1CharIndex].getSize().y));
            window.draw(spAva);
        }

        // P2 Label
        sf::Text lblP2;
        lblP2.setFont(fBold);
        lblP2.setString(toUtf8(tiengViet ? "Kỳ thủ 2 (Quân Đen):" : "Player 2 (Black):"));
        lblP2.setCharacterSize(14);
        lblP2.setStyle(sf::Text::Bold);
        lblP2.setFillColor(sf::Color(35, 60, 120));
        lblP2.setPosition(1045.0f, 350.0f);
        window.draw(lblP2);

        inputP2Name->draw(window);

        // P2 Avatar Box
        sf::RectangleShape avaBox2(sf::Vector2f(34.0f, 34.0f));
        avaBox2.setPosition(1415.0f, 366.0f);
        avaBox2.setFillColor(sf::Color(250, 240, 225));
        avaBox2.setOutlineThickness(1.5f);
        avaBox2.setOutlineColor(sf::Color(50, 80, 140));
        window.draw(avaBox2);

        if (daTaiAvatarTextures && p2CharIndex >= 0 && p2CharIndex < 5 && avatarTextures[p2CharIndex].getSize().x > 0) {
            sf::Sprite spAva(avatarTextures[p2CharIndex]);
            spAva.setPosition(1417.0f, 368.0f);
            spAva.setScale(30.0f / static_cast<float>(avatarTextures[p2CharIndex].getSize().x),
                           30.0f / static_cast<float>(avatarTextures[p2CharIndex].getSize().y));
            window.draw(spAva);
        }

        // Roll banner text
        if (!pvpRollBanner.empty()) {
            sf::Text bText;
            bText.setFont(f);
            bText.setString(toUtf8(pvpRollBanner));
            bText.setCharacterSize(13);
            bText.setStyle(sf::Text::Bold);
            bText.setFillColor(sf::Color(180, 35, 25));
            sf::FloatRect btb = bText.getLocalBounds();
            bText.setOrigin(btb.left + btb.width / 2.0f, btb.top + btb.height / 2.0f);
            bText.setPosition(1240.0f, 456.0f);
            window.draw(bText);
        }

        const std::vector<std::string> rowLabels = {
            tiengViet ? "Kiểu quân cờ:" : "Piece Style:",
            tiengViet ? "Màu bàn cờ:" : "Board Theme:",
            tiengViet ? "Thời gian ván:" : "Match Timer:",
            tiengViet ? "Hào quang vương:" : "King Aura:"
        };
        const float rowYs[] = { 444.0f, 536.0f, 576.0f, 616.0f };
        for (int i = 0; i < 4; ++i) {
            sf::Text lbl;
            lbl.setFont(f);
            lbl.setString(toUtf8(rowLabels[i]));
            lbl.setCharacterSize(14);
            lbl.setStyle(sf::Text::Bold);
            lbl.setFillColor(sf::Color(75, 45, 25));
            lbl.setPosition(1045.0f, rowYs[i] + 7.0f);
            window.draw(lbl);
        }

        // Piece preview
        vePieceStylePreview(window, 1045.0f, 482.0f, 390.0f, 46.0f);
    }
    else if (playSetupMode == 2) {
        sf::RectangleShape infoBox(sf::Vector2f(390.0f, 115.0f));
        infoBox.setPosition(1045.0f, 305.0f);
        infoBox.setFillColor(sf::Color(250, 242, 228, 230));
        infoBox.setOutlineThickness(1.5f);
        infoBox.setOutlineColor(sf::Color(190, 140, 80));
        window.draw(infoBox);

        sf::Text demoDesc;
        demoDesc.setFont(fBold);
        demoDesc.setCharacterSize(14);
        demoDesc.setStyle(sf::Text::Bold);
        demoDesc.setFillColor(sf::Color(45, 22, 10));
        demoDesc.setString(toUtf8(tiengViet
            ? "Mô phỏng tự động ván đấu cờ tàn:\n"
              "• Bên Đỏ: AI Minimax (2 Xe 2 Voi)\n"
              "• Bên Đen: AI Alpha-Beta (2 Xe 2 Voi)\n"
              "• Tự động tính toán các nước đi tinh tế\n"
              "• Thích hợp học hỏi chiến thuật đối tàn cuộc"
            : "Automatic endgame battle simulation:\n"
              "• Red: Minimax AI (2 Chariots 2 Elephants)\n"
              "• Black: Alpha-Beta AI (2 Chariots 2 Elephants)\n"
              "• High-level tactical moves demonstrated\n"
              "• Perfect for learning endgame strategies"
        ));
        demoDesc.setPosition(1058.0f, 314.0f);
        window.draw(demoDesc);

        sf::Text lblPiece;
        lblPiece.setFont(f);
        lblPiece.setString(toUtf8(tiengViet ? "Kiểu quân cờ:" : "Piece Style:"));
        lblPiece.setCharacterSize(14);
        lblPiece.setStyle(sf::Text::Bold);
        lblPiece.setFillColor(sf::Color(75, 45, 25));
        lblPiece.setPosition(1045.0f, 437.0f);
        window.draw(lblPiece);

        // Piece preview
        vePieceStylePreview(window, 1045.0f, 472.0f, 390.0f, 48.0f);

        sf::Text lblBoard;
        lblBoard.setFont(f);
        lblBoard.setString(toUtf8(tiengViet ? "Màu bàn cờ:" : "Board Theme:"));
        lblBoard.setCharacterSize(14);
        lblBoard.setStyle(sf::Text::Bold);
        lblBoard.setFillColor(sf::Color(75, 45, 25));
        lblBoard.setPosition(1045.0f, 537.0f);
        window.draw(lblBoard);
    }

    // Draw buttons
    for (auto& btn : playSetupButtons) {
        btn->draw(window);
    }

    // Right Page Footer
    sf::Text fRText;
    fRText.setFont(fBold);
    fRText.setString(toUtf8(tiengViet ? "Enter: Bắt đầu ván đấu       ESC: Quay về menu" : "Enter: Start Game       ESC: Back to Menu"));
    fRText.setCharacterSize(14);
    fRText.setStyle(sf::Text::Bold);
    fRText.setFillColor(sf::Color(80, 45, 25));
    fRText.setPosition(1055.0f, 726.0f);
    window.draw(fRText);
}

void GameManager::xuLyClickPlaySetup(const sf::Vector2i& viTri) {
    // 1. Return Tab (Top-Left)
    sf::FloatRect backTabRect(482.0f, 186.0f, 68.0f, 48.0f);
    if (backTabRect.contains(static_cast<float>(viTri.x), static_cast<float>(viTri.y))) {
        soundManager.play(SoundType::CLICK);
        trangThai = TrangThai::MENU_CHINH;
        menuChinh.triggerOpenAnimation(MenuSection::PLAY);
        return;
    }

    // 2. Top Edge Tabs: [PvAI] [PvP] [Demo]
    for (int i = 0; i < 3; ++i) {
        if (sf::FloatRect(560.0f + i * 55.0f, 185.0f, 48.0f, 38.0f).contains(static_cast<float>(viTri.x), static_cast<float>(viTri.y))) {
            soundManager.play(SoundType::CLICK);
            playSetupMode = i;
            khoiTaoPlaySetupButtons();
            return;
        }
    }

    // 3. Text Inputs
    if (playSetupMode == 0) {
        inputAiPlayerName->handleClick(viTri);
    } else if (playSetupMode == 1) {
        inputP1Name->handleClick(viTri);
        inputP2Name->handleClick(viTri);
    }

    // 4. Buttons
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
    title.setPosition(1920.0f / 2.0f, 60.0f);
    window.draw(title);

    // Left Column: Player 1 Card
    sf::RectangleShape p1Card(sf::Vector2f(440.0f, 255.0f));
    p1Card.setPosition(100.0f, 115.0f);
    p1Card.setFillColor(sf::Color(24, 30, 44));
    p1Card.setOutlineThickness(2.0f);
    p1Card.setOutlineColor(sf::Color(70, 90, 125));
    window.draw(p1Card);

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
    sf::RectangleShape p2Card(sf::Vector2f(440.0f, 255.0f));
    p2Card.setPosition(660.0f, 115.0f);
    p2Card.setFillColor(sf::Color(24, 30, 44));
    p2Card.setOutlineThickness(2.0f);
    p2Card.setOutlineColor(sf::Color(70, 90, 125));
    window.draw(p2Card);

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
    bannerText.setPosition(1920.0f / 2.0f, 545.0f);
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
    title.setPosition(1920.0f / 2.0f, 60.0f);
    window.draw(title);

    // Left Column: Player
    sf::RectangleShape pCard(sf::Vector2f(440.0f, 255.0f));
    pCard.setPosition(100.0f, 115.0f);
    pCard.setFillColor(sf::Color(24, 30, 44));
    pCard.setOutlineThickness(2.0f);
    pCard.setOutlineColor(sf::Color(70, 90, 125));
    window.draw(pCard);

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
    sf::RectangleShape aiCard(sf::Vector2f(440.0f, 255.0f));
    aiCard.setPosition(660.0f, 115.0f);
    aiCard.setFillColor(sf::Color(24, 30, 44));
    aiCard.setOutlineThickness(2.0f);
    aiCard.setOutlineColor(sf::Color(70, 90, 125));
    window.draw(aiCard);

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
    bannerText.setPosition(1920.0f / 2.0f, 575.0f);
    window.draw(bannerText);

    menuPvaiSetup.draw(window);
}

void GameManager::veLoadMenu() {
    sf::Text title;
    title.setFont(font);
    title.setString(Loc::get(LocKey::LOAD_TITLE));
    title.setCharacterSize(48);
    title.setFillColor(sf::Color(240, 190, 70));
    title.setStyle(sf::Text::Bold);
    
    sf::FloatRect tb = title.getLocalBounds();
    title.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
    title.setPosition(1920.0f / 2.0f, 85.0f);
    window.draw(title);

    for (int i = 0; i < 3; ++i) {
        float sy = 160.0f + i * 145.0f;
        sf::RectangleShape card(sf::Vector2f(900.0f, 125.0f));
        card.setPosition(150.0f, sy);
        
        bool isSel = (selectedSlot == i);
        card.setFillColor(isSel ? sf::Color(32, 42, 62) : sf::Color(22, 28, 40));
        card.setOutlineThickness(isSel ? 3.0f : 1.5f);
        card.setOutlineColor(isSel ? sf::Color(240, 190, 70) : sf::Color(55, 70, 95));
        window.draw(card);

        sf::Text slotHeader;
        slotHeader.setFont(font);
        slotHeader.setCharacterSize(22);
        slotHeader.setStyle(sf::Text::Bold);
        slotHeader.setString(Loc::get(LocKey::SLOT_LABEL) + std::to_string(i + 1));
        slotHeader.setFillColor(isSel ? sf::Color(255, 215, 100) : sf::Color(180, 205, 235));
        slotHeader.setPosition(180.0f, sy + 18.0f);
        window.draw(slotHeader);

        const SaveSlotInfo& info = slotInfos[i];
        if (info.tonTai) {
            sf::Text det1;
            det1.setFont(font);
            det1.setCharacterSize(16);
            std::string d1 = "Mode: " + info.tenCheDo + "  |  Date: " + info.thoiGian;
            det1.setString(d1);
            det1.setFillColor(sf::Color(220, 230, 245));
            det1.setPosition(180.0f, sy + 54.0f);
            window.draw(det1);

            sf::Text det2;
            det2.setFont(font);
            det2.setCharacterSize(16);
            std::string d2 = "Red: " + info.tenDo + "  vs  Black: " + info.tenDen + 
                             "  |  Moves: " + std::to_string(info.soNuoc) +
                             "  |  Turn: " + ((info.luot == Mau::DO) ? "RED" : "BLACK");
            det2.setString(d2);
            det2.setFillColor(sf::Color(160, 185, 215));
            det2.setPosition(180.0f, sy + 82.0f);
            window.draw(det2);
        } else {
            sf::Text emp;
            emp.setFont(font);
            emp.setCharacterSize(17);
            emp.setString(Loc::get(LocKey::EMPTY_SLOT));
            emp.setFillColor(sf::Color(130, 145, 165));
            emp.setPosition(180.0f, sy + 60.0f);
            window.draw(emp);
        }
    }

    menuLoad.draw(window);
}

void GameManager::veSettingsMenu() {
    // 1. Header Title
    sf::Text title;
    title.setFont(font);
    title.setString(Loc::get(LocKey::SETTINGS_TITLE));
    title.setCharacterSize(44);
    title.setFillColor(sf::Color(240, 190, 70));
    title.setStyle(sf::Text::Bold);
    
    sf::FloatRect tb = title.getLocalBounds();
    title.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
    title.setPosition(1920.0f / 2.0f, 55.0f);
    window.draw(title);

    // Decorative golden bar
    sf::RectangleShape goldLine(sf::Vector2f(320.0f, 3.0f));
    goldLine.setPosition((1920.0f - 320.0f) / 2.0f, 88.0f);
    goldLine.setFillColor(sf::Color(240, 190, 70));
    window.draw(goldLine);

    // 2. Tab Navigation Bar
    for (size_t i = 0; i < settingsTabButtons.size(); ++i) {
        bool isActive = (static_cast<int>(activeSettingsTab) == static_cast<int>(i));
        if (isActive) {
            settingsTabButtons[i]->setColors(sf::Color(180, 135, 30), sf::Color(210, 160, 40), sf::Color(150, 110, 20));
        } else {
            settingsTabButtons[i]->setColors(sf::Color(32, 40, 56), sf::Color(45, 58, 80), sf::Color(25, 32, 44));
        }
        settingsTabButtons[i]->draw(window);
    }

   
    // 3. Central Main Dashboard Panel
    float panelX = 100.0f;
    float panelY = 195.0f;
    float panelW = 1000.0f;
    float panelH = 490.0f;

    static sf::Texture texSetting;
    static bool thuTaiSetting = false;
    static bool taiSettingXong = false;

    if (!thuTaiSetting) {
        // Quét tìm file ở mọi ngóc ngách có thể
        const std::vector<std::string> thuMuc = { "assets/", "../assets/", "../../assets/", "resources/textures/", "" };
        for (const auto& tm : thuMuc) {
            if (texSetting.loadFromFile(tm + "setting_background.png")) {
                taiSettingXong = true;
                texSetting.setSmooth(true);
                break;
            }
        }
        thuTaiSetting = true;
    }

    if (taiSettingXong) {
        sf::Sprite spSetting(texSetting);
        spSetting.setPosition(panelX, panelY);
        spSetting.setScale(panelW / texSetting.getSize().x, panelH / texSetting.getSize().y);
        window.draw(spSetting);
    }
    else {
        sf::RectangleShape mainCard(sf::Vector2f(panelW, panelH));
        mainCard.setPosition(panelX, panelY);
        // Thay màu nền thành nâu gỗ tối
        mainCard.setFillColor(sf::Color(60, 40, 20));
        mainCard.setOutlineThickness(2.0f);
        // Thay màu viền thành nâu gỗ sáng
        mainCard.setOutlineColor(sf::Color(100, 70, 35));
        window.draw(mainCard);
    }

    

    // 4. Render Active Tab Content
    if (activeSettingsTab == SettingsTab::AUDIO) {
        if (sliderMaster) sliderMaster->draw(window);
        if (settingsAudioButtons.size() >= 2) {
            settingsAudioButtons[0]->draw(window); // SFX Toggle
        }
        if (sliderSfx) sliderSfx->draw(window);
        if (settingsAudioButtons.size() >= 2) {
            settingsAudioButtons[1]->draw(window); // BGM Toggle
        }
        if (sliderBgm) sliderBgm->draw(window);

        // Interactive Soundboard
        sf::Text sbTitle;
        sbTitle.setFont(font);
        sbTitle.setCharacterSize(14);
        sbTitle.setStyle(sf::Text::Bold);
        sbTitle.setString((Loc::getLanguage() == Language::TIENG_VIET) ? "THU NGHIEM AM THANH TRUC TIEP:" : "INTERACTIVE SOUNDBOARD TEST:");
        sbTitle.setFillColor(sf::Color(240, 190, 70));
        sbTitle.setPosition(140.0f, 528.0f);
        window.draw(sbTitle);

        for (auto& btn : soundboardButtons) btn->draw(window);

        // Right side info/visualizer card
        float rcX = 640.0f;
        float rcY = 220.0f;
        float rcW = 420.0f;
        float rcH = 350.0f;

        sf::RectangleShape infoBox(sf::Vector2f(rcW, rcH));
        infoBox.setPosition(rcX, rcY);

        // Card background
        infoBox.setFillColor(sf::Color(85, 55, 35));

        infoBox.setOutlineThickness(2.0f); // Tăng độ viền lên 2.0f cho rõ nét

        // Viền: Vàng gỗ sáng rực rỡ
        infoBox.setOutlineColor(sf::Color(160, 105, 50));

        window.draw(infoBox);

        sf::Text cardTitle;
        cardTitle.setFont(font);
        cardTitle.setCharacterSize(20);
        cardTitle.setStyle(sf::Text::Bold);
        cardTitle.setString("AUDIO SYSTEM PREVIEW");
        cardTitle.setFillColor(sf::Color(240, 190, 70));
        cardTitle.setPosition(rcX + 25.0f, rcY + 20.0f);
        window.draw(cardTitle);

        std::string audioDetails = 
            "- MASTER VOLUME: " + std::to_string(static_cast<int>(soundManager.getMasterVolume())) + "%\n"
            "- SFX SYSTEM: " + (soundManager.getSfxBat() ? "ONLINE" : "MUTED") + " (" + std::to_string(static_cast<int>(soundManager.getSfxVolume())) + "%)\n"
            "- BGM AMBIENT: " + (soundManager.getBgmBat() ? "PLAYING" : "MUTED") + " (" + std::to_string(static_cast<int>(soundManager.getBgmVolume())) + "%)\n\n"
            "High-fidelity recorded sound effects & \n"
            "Guzheng audio synthesis with real-time sliders.\n\n"
            "Click any soundboard button on the left to test!";

        sf::Text audioDesc;
        audioDesc.setFont(font);
        audioDesc.setCharacterSize(15);
        audioDesc.setString(audioDetails);
        audioDesc.setFillColor(sf::Color(210, 225, 245));
        audioDesc.setPosition(rcX + 25.0f, rcY + 60.0f);
        window.draw(audioDesc);

        // Equalizer visual bars
        float barStartX = rcX + 25.0f;
        float barY = rcY + 280.0f;
        for (int b = 0; b < 10; ++b) {
            float bh = 15.0f + (b % 4) * 12.0f;
            sf::RectangleShape eqBar(sf::Vector2f(22.0f, bh));
            eqBar.setPosition(barStartX + b * 36.0f, barY - bh);
            eqBar.setFillColor((b < static_cast<int>(soundManager.getMasterVolume() / 10.0f)) ? sf::Color(100, 200, 140) : sf::Color(50, 65, 85));
            window.draw(eqBar);
        }
    }
    else if (activeSettingsTab == SettingsTab::GRAPHICS) {
        for (auto& btn : settingsGraphicsButtons) btn->draw(window);
        if (sliderAtmosphere) sliderAtmosphere->draw(window);

        // Theme preview
        float rcX = 640.0f;
        float rcY = 220.0f;
        float rcW = 420.0f;
        float rcH = 350.0f;

        sf::RectangleShape infoBox(sf::Vector2f(rcW, rcH));
        infoBox.setPosition(rcX, rcY);

        // Card background
        infoBox.setFillColor(sf::Color(85, 55, 35));

        infoBox.setOutlineThickness(2.0f); // Tăng độ viền lên 2.0f cho rõ nét

        // Viền: Vàng gỗ sáng rực rỡ
        infoBox.setOutlineColor(sf::Color(160, 105, 50));

        window.draw(infoBox);

        sf::Text cardTitle;
        cardTitle.setFont(font);
        cardTitle.setCharacterSize(20);
        cardTitle.setStyle(sf::Text::Bold);
        cardTitle.setString("LIVE THEME PREVIEW");
        cardTitle.setFillColor(sf::Color(240, 190, 70));
        cardTitle.setPosition(rcX + 25.0f, rcY + 20.0f);
        window.draw(cardTitle);

        // Mini preview board
        float prevW = 200.0f;
        float prevH = 140.0f;
        float prevX = rcX + 110.0f;
        float prevY = rcY + 65.0f;

        sf::Color prevBg = sf::Color(228, 192, 138);
        sf::Color prevBorder = sf::Color(90, 50, 25);
        if (boardTheme == BoardTheme::IMPERIAL_JADE) {
            prevBg = sf::Color(190, 222, 200);
            prevBorder = sf::Color(30, 70, 50);
        } else if (boardTheme == BoardTheme::MIDNIGHT_INK) {
            prevBg = sf::Color(32, 38, 50);
            prevBorder = sf::Color(18, 22, 30);
        } else if (boardTheme == BoardTheme::WARM_BAMBOO) {
            prevBg = sf::Color(242, 225, 190);
            prevBorder = sf::Color(115, 75, 40);
        }

        sf::RectangleShape prevBoard(sf::Vector2f(prevW, prevH));
        prevBoard.setPosition(prevX, prevY);
        prevBoard.setFillColor(prevBg);
        prevBoard.setOutlineThickness(3.0f);
        prevBoard.setOutlineColor(prevBorder);
        window.draw(prevBoard);

        // Cross line on preview
        sf::Vertex pvLineH[] = {
            sf::Vertex(sf::Vector2f(prevX, prevY + prevH / 2.0f), prevBorder),
            sf::Vertex(sf::Vector2f(prevX + prevW, prevY + prevH / 2.0f), prevBorder)
        };
        sf::Vertex pvLineV[] = {
            sf::Vertex(sf::Vector2f(prevX + prevW / 2.0f, prevY), prevBorder),
            sf::Vertex(sf::Vector2f(prevX + prevW / 2.0f, prevY + prevH), prevBorder)
        };
        window.draw(pvLineH, 2, sf::Lines);
        window.draw(pvLineV, 2, sf::Lines);

        // Preview pieces
        auto drawPreviewPiece = [this](float x, float y, const std::string& name, Mau m) {
            float r = 24.0f;
            float targetDiameter = r * 2.0f;
            const sf::Texture* tex = nullptr;
            if (pieceStyle == PieceStyle::REALISTIC_WOOD) {
                tex = layTextureQuan(name, m);
            }

            if (tex != nullptr) {
                sf::CircleShape shadow(r - 1.0f);
                shadow.setOrigin(r - 1.0f, r - 1.0f);
                shadow.setPosition(x + 2.0f, y + 2.5f);
                shadow.setFillColor(sf::Color(0, 0, 0, 100));
                window.draw(shadow);

                sf::Sprite sprite(*tex);
                sf::Vector2u texSize = tex->getSize();
                sprite.setOrigin(texSize.x / 2.0f, texSize.y / 2.0f);
                float scale = targetDiameter / static_cast<float>(texSize.x);
                sprite.setScale(scale, scale);
                sprite.setPosition(x, y);
                window.draw(sprite);
            } else {
                sf::CircleShape outer(r + 2.0f);
                outer.setOrigin(r + 2.0f, r + 2.0f);
                outer.setPosition(x, y);
                outer.setFillColor((m == Mau::DO) ? sf::Color(140, 30, 20) : sf::Color(30, 40, 50));
                window.draw(outer);

                sf::CircleShape body(r);
                body.setOrigin(r, r);
                body.setPosition(x, y);
                body.setFillColor(sf::Color(250, 238, 215));
                body.setOutlineThickness(2.0f);
                body.setOutlineColor((m == Mau::DO) ? sf::Color(180, 40, 30) : sf::Color(40, 50, 65));
                window.draw(body);

                std::string label = layKyHieuQuanTheoStyle(name, m);
                sf::Text t;
                t.setFont(font);
                t.setString(label);
                t.setCharacterSize((label.size() > 2) ? 15 : 18);
                t.setStyle(sf::Text::Bold);
                t.setFillColor((m == Mau::DO) ? sf::Color(190, 25, 20) : sf::Color(20, 25, 35));
                sf::FloatRect tb = t.getLocalBounds();
                t.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
                t.setPosition(x, y);
                window.draw(t);
            }
        };

        drawPreviewPiece(prevX + 55.0f, prevY + prevH / 2.0f, "Xe", Mau::DO);
        drawPreviewPiece(prevX + 145.0f, prevY + prevH / 2.0f, "Ma", Mau::DEN);

        sf::Text prevDesc;
        prevDesc.setFont(font);
        prevDesc.setCharacterSize(14);
        prevDesc.setString(
            "Move Hints: " + std::string(hienGoiY ? "ENABLED" : "DISABLED") + "\n"
            "Last Move Highlight: " + std::string(hienNuocDiCuoi ? "ENABLED" : "DISABLED") + "\n"
            "Board Coordinates: " + std::string(hienToaDo ? "1-9, A-J ON" : "HIDDEN") + "\n"
            "Display Mode: " + std::string(fullscreenMode ? "FULLSCREEN" : "WINDOWED")
        );
        prevDesc.setFillColor(sf::Color(200, 215, 235));
        prevDesc.setPosition(rcX + 25.0f, rcY + 230.0f);
        window.draw(prevDesc);
    }
    else if (activeSettingsTab == SettingsTab::GAMEPLAY) {
        for (auto& btn : settingsGameplayButtons) btn->draw(window);

        // Right side info card
        
        float rcX = 640.0f;
        float rcY = 220.0f;
        float rcW = 420.0f;
        float rcH = 350.0f;

        sf::RectangleShape infoBox(sf::Vector2f(rcW, rcH));
        infoBox.setPosition(rcX, rcY);

        // Nền: Nâu sồi sáng vừa
        infoBox.setFillColor(sf::Color(85, 55, 35));
        infoBox.setOutlineThickness(2.0f);
        // Viền: Vàng gỗ sáng rực rỡ
        infoBox.setOutlineColor(sf::Color(160, 105, 50));

        window.draw(infoBox);

        sf::Text cardTitle;
        cardTitle.setFont(font);
        cardTitle.setCharacterSize(20);
        cardTitle.setStyle(sf::Text::Bold);
        cardTitle.setString("GAMEPLAY & AI ENGINE");
        cardTitle.setFillColor(sf::Color(240, 190, 70));
        cardTitle.setPosition(rcX + 25.0f, rcY + 20.0f);
        window.draw(cardTitle);

        std::string gameDetails = 
            std::string("- AI ENGINE: Minimax with Alpha-Beta Pruning\n") +
            "  Integrated Piece-Square Positional Tables & Move Ordering.\n\n" +
            "- SIMULATION DELAY: " + (aiThinkRealistic ? "450ms (Natural pacing)" : "Instant (Immediate)") + "\n\n" +
            "- CHECK WARNING ALARM: " + (checkAlarmSound ? "Audio & Visual Flash" : "Silent") + "\n\n" +
            "- LANGUAGE: " + ((Loc::getLanguage() == Language::TIENG_VIET) ? "Tieng Viet" : "English") + "\n\n" +
            "All changes apply seamlessly in-game.";

        sf::Text gameDesc;
        gameDesc.setFont(font);
        gameDesc.setCharacterSize(15);
        gameDesc.setString(gameDetails);
        gameDesc.setFillColor(sf::Color(210, 225, 245));
        gameDesc.setPosition(rcX + 25.0f, rcY + 65.0f);
        window.draw(gameDesc);
    }
    else if (activeSettingsTab == SettingsTab::CONTROLS) {
        for (auto& btn : settingsControlButtons) btn->draw(window);

        // Right side info card
        float rcX = 660.0f;
        float rcY = 210.0f;
        float rcW = 400.0f;
        float rcH = 370.0f;

        sf::RectangleShape infoBox(sf::Vector2f(rcW, rcH));
        infoBox.setPosition(rcX, rcY);

        // Nền: Nâu sồi sáng vừa
        infoBox.setFillColor(sf::Color(85, 55, 35));
        infoBox.setOutlineThickness(2.0f);
        // Viền: Vàng gỗ sáng rực rỡ
        infoBox.setOutlineColor(sf::Color(160, 105, 50));

        window.draw(infoBox);

        sf::Text cardTitle;
        cardTitle.setFont(font);
        cardTitle.setCharacterSize(20);
        cardTitle.setStyle(sf::Text::Bold);
        cardTitle.setString("KEY REBINDING GUIDE");
        cardTitle.setFillColor(sf::Color(240, 190, 70));
        cardTitle.setPosition(rcX + 25.0f, rcY + 20.0f);
        window.draw(cardTitle);

        std::string ctrlDetails = 
            "1. Click on any action on the left.\n"
            "2. When button turns yellow with\n"
            "   '[ Press any key... ]',\n"
            "   press ANY key on your keyboard.\n"
            "3. Press [Esc] to cancel without change.\n\n"
            "CURRENT PRESET: " + keyConfig.getPresetName() + "\n\n"
            "MOUSE NAVIGATION:\n"
            "Direct clicking on the board squares\n"
            "is always supported simultaneously.";

        sf::Text ctrlDesc;
        ctrlDesc.setFont(font);
        ctrlDesc.setCharacterSize(15);
        ctrlDesc.setString(ctrlDetails);
        ctrlDesc.setFillColor(sf::Color(210, 225, 245));
        ctrlDesc.setPosition(rcX + 25.0f, rcY + 65.0f);
        window.draw(ctrlDesc);

        if (rebindingAction != KeyAction::COUNT) {
            sf::RectangleShape listeningBox(sf::Vector2f(rcW - 40.0f, 50.0f));
            listeningBox.setPosition(rcX + 20.0f, rcY + 295.0f);
            listeningBox.setFillColor(sf::Color(180, 130, 20));
            listeningBox.setOutlineThickness(2.0f);
            listeningBox.setOutlineColor(sf::Color(255, 230, 80));
            window.draw(listeningBox);

            sf::Text listenT;
            listenT.setFont(font);
            listenT.setCharacterSize(16);
            listenT.setStyle(sf::Text::Bold);
            listenT.setString(">>> LISTENING FOR KEYPRESS <<<");
            listenT.setFillColor(sf::Color::White);
            sf::FloatRect ltb = listenT.getLocalBounds();
            listenT.setOrigin(ltb.left + ltb.width / 2.0f, ltb.top + ltb.height / 2.0f);
            listenT.setPosition(rcX + rcW / 2.0f, rcY + 320.0f);
            window.draw(listenT);
        }
    }

    // Bottom buttons
    for (auto& btn : settingsBottomButtons) {
        btn->draw(window);
    }
}

void GameManager::veKeybindingMenu() {
    // Controls tab
    activeSettingsTab = SettingsTab::CONTROLS;
    veSettingsMenu();
}

void GameManager::veIntroMenu() {
    sf::Text title;
    title.setFont(font);
    title.setString(Loc::get(LocKey::INTRO_TITLE));
    title.setCharacterSize(42);
    title.setFillColor(sf::Color(240, 190, 70));
    title.setStyle(sf::Text::Bold);
    
    sf::FloatRect tb = title.getLocalBounds();
    title.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
    title.setPosition(1920.0f / 2.0f, 65.0f);
    window.draw(title);

    sf::RectangleShape box(sf::Vector2f(960.0f, 545.0f));
    box.setPosition((1920.0f - 960.0f) / 2.0f, 120.0f);
    box.setFillColor(sf::Color(24, 30, 44));
    box.setOutlineThickness(2.0f);
    box.setOutlineColor(sf::Color(65, 80, 115));
    window.draw(box);

    std::string text = (Loc::getLanguage() == Language::TIENG_VIET)
        ? "- GIOI THIEU DO AN:\n"
          "  Do an Lap trinh Huong doi tuong (OOP) - Truong Dai hoc Khoa hoc Tu nhien (HCMUS)\n"
          "  Game Co Tuong hien thuc bang C++17 va thu vien do hoa SFML.\n\n"
          "- KIEN TRUC OOP & DESIGN PATTERNS:\n"
          "  * Da hinh (Polymorphism): Lop truu tuong QuanCo (kiemTraNuocDi, layGiaTri, layDiemViTri, laTuong)\n"
          "  * Cac lop con ke thua: Xe, Voi, Phao, Ma, Tuong, Si, Tot\n"
          "  * Design Patterns: Factory Method (QuanCoFactory), Prototype (clone Deep Copy), RAII Move Guard.\n\n"
          "- LUAT DI CHUYEN CAC QUAN CO:\n"
          "  1. XE: Di ngang/doc khong gioi han, khong duoc nhay qua quan khac.\n"
          "  2. VOI: Di cheo dung 2 o, khong duoc qua song, khong bi chan mat Voi.\n"
          "  3. PHAO: Di ngang/doc tu do; an quan phai co dung 1 quan lam ngoi de nhay.\n"
          "  4. MA: Di hinh chu nhat 1x2 hoac 2x1 (hinh chu Nhat), bi can chan Ma.\n"
          "  5. TUONG: Di ngang/doc 1 o trong Cung Cuu Cung; 2 Tuong khong nhin thang nhau.\n"
          "  6. SI: Di cheo 1 o trong pham vi Cung Cuu Cung.\n"
          "  7. TOT: Di thang 1 buoc; sau khi qua song duoc di ngang hoac di thang 1 buoc. Khong di lui.\n\n"
          "- DIEU KHIEN: Phim W, A, S, D di chuyen con tro, Enter de chon/di, Esc de bo chon, Q thoat."
        : "- PROJECT INTRODUCTION:\n"
          "  Object-Oriented Programming (OOP) Project - VNU-HCM University of Science (HCMUS)\n"
          "  Chinese Chess (Xiangqi) fully developed in C++17 and SFML graphics library.\n\n"
          "- OOP ARCHITECTURE & DESIGN PATTERNS:\n"
          "  * Polymorphism: Abstract QuanCo base class (kiemTraNuocDi, layGiaTri, layDiemViTri, laTuong).\n"
          "  * Derived Piece Classes: Xe, Voi, Phao, Ma, Tuong, Si, Tot.\n"
          "  * Design Patterns: Factory Method (QuanCoFactory), Prototype (Deep Copy clone), RAII Move Guard.\n\n"
          "- PIECE MOVEMENT RULES:\n"
          "  1. CHARIOT (Xe): Moves orthogonally any number of spaces; blocked by obstacles.\n"
          "  2. ELEPHANT (Voi): Moves diagonally exactly 2 squares; cannot cross river; blocked by eye.\n"
          "  3. CANNON (Phao): Moves orthogonally; captures by jumping over exactly 1 piece.\n"
          "  4. HORSE (Ma): Moves 1 step orthogonally then 1 step diagonally; blocked by hobbling.\n"
          "  5. GENERAL (Tuong): Moves 1 step orthogonally within Palace; Flying Generals forbidden.\n"
          "  6. ADVISOR (Si): Moves 1 step diagonally within Palace.\n"
          "  7. SOLDIER (Tot): 1 step forward before river; 1 step forward/horizontal after river. Never backwards.\n\n"
          "- CONTROLS: W, A, S, D to move cursor, Enter to select/move, Esc to cancel, Q to quit.";

    sf::Text content;
    content.setFont(font);
    content.setString(text);
    content.setCharacterSize(15);
    content.setStyle(sf::Text::Bold);
    content.setFillColor(sf::Color(245, 248, 255));
    content.setPosition((1920.0f - 960.0f) / 2.0f + 30.0f, 140.0f);
    window.draw(content);

    menuIntro.draw(window);
}
