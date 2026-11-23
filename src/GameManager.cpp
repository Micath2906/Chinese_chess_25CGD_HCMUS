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
#include "AssetManager.h"

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
      aiThinkRealistic(true),
      checkAlarmSound(true),
      pvpSetupStep(1),
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
      hienPopupXinHoa(false),
      nguoiXinHoa(Mau::DO),
      victoryParticlesActive(false),
      isRandomRolling(false),
      rollFinished(false),
      rollTimer(0.0f),
      rollAngle(0.0f),
      rollWinner(0),
      viTriNuocDiSai(-1, -1),
      thoiGianNuocDiSai(0.0f),
      lyDoNuocDiSai(""),
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

    // Load fonts via AssetManager
    auto& am = AssetManager::get();
    fontVnBold = am.getBoldFont(true);
    fontEnBold = am.getBoldFont(false);
    fontVn     = am.getFont(true, false);
    fontEn     = am.getFont(false, false);
    fontHan    = am.font(FontId::NOM_NA_TONG);

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
    inputP1Name = std::make_unique<TextInput>(510.0f, 218.0f, 410.0f, 38.0f, font, "Player 1", 14);
    inputP1Name->setValue("Player 1");
    inputP1Name->setParchmentStyle(true);

    inputP2Name = std::make_unique<TextInput>(510.0f, 218.0f, 410.0f, 38.0f, font, "Player 2", 14);
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
        pvpSetupStep = 1;
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
        pvpSetupStep = 1;
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
    inGameRightButtons.clear();
    inGameMenuButtons.clear();

    // 1. In-game Left Panel Action Buttons (Player 1 / Red)
    // Row 1 (Y=308): Draw & Resign for Player 1 (Red)
    auto btnDraw = std::make_unique<Button>(35.0f, 308.0f, 115.0f, 38.0f, Loc::get(LocKey::DRAW_OFFER), font, [this]() {
        thucHienXinHoa(Mau::DO);
    });
    btnDraw->setCharacterSize(14);
    btnDraw->setColors(sf::Color(45, 52, 65, 240), sf::Color(65, 80, 105, 255), sf::Color(35, 40, 52, 255));
    inGameButtons.push_back(std::move(btnDraw));

    auto btnResign = std::make_unique<Button>(160.0f, 308.0f, 115.0f, 38.0f, Loc::get(LocKey::SURRENDER), font, [this]() {
        thucHienDauHang(Mau::DO);
    });
    btnResign->setCharacterSize(14);
    btnResign->setColors(sf::Color(75, 32, 32, 240), sf::Color(110, 45, 45, 255), sf::Color(55, 22, 22, 255));
    inGameButtons.push_back(std::move(btnResign));

    // Help '?' button (42x38 at 35, 716)
    auto btnHelp = std::make_unique<Button>(35.0f, 716.0f, 42.0f, 38.0f, "?", fontBold, [this]() {
        soundManager.play(SoundType::CLICK);
        hienToast(Loc::getLanguage() == Language::TIENG_VIET 
            ? "[WASD/Mũi tên]: Chọn ô • [Enter/Space]: Đi cờ • [ESC]: Menu" 
            : "[WASD/Arrows]: Select • [Enter/Space]: Move • [ESC]: Menu");
    });
    btnHelp->setCharacterSize(16);
    btnHelp->setColors(sf::Color(65, 55, 35, 240), sf::Color(105, 90, 55, 255), sf::Color(45, 38, 25, 255));
    inGameButtons.push_back(std::move(btnHelp));

    // Match Menu [ESC] button at X = 82.0f, Y = 716.0f, W = 193.0f, H = 38.0f
    auto btnOpen = std::make_unique<Button>(82.0f, 716.0f, 193.0f, 38.0f, Loc::get(LocKey::BTN_GAME_MENU), font, [this]() {
        moMenuPopUp();
    });
    btnOpen->setCharacterSize(14);
    btnOpen->setColors(sf::Color(45, 70, 115, 250), sf::Color(65, 100, 160, 255), sf::Color(30, 50, 85, 255));
    inGameButtons.push_back(std::move(btnOpen));

    // 2. In-game Right Panel Action Buttons (Player 2 / Black)
    // Row 1 (Y=308): Draw & Resign for Player 2 (Black)
    auto btnDrawP2 = std::make_unique<Button>(925.0f, 308.0f, 115.0f, 38.0f, Loc::get(LocKey::DRAW_OFFER), font, [this]() {
        thucHienXinHoa(Mau::DEN);
    });
    btnDrawP2->setCharacterSize(14);
    btnDrawP2->setColors(sf::Color(45, 52, 65, 240), sf::Color(65, 80, 105, 255), sf::Color(35, 40, 52, 255));
    inGameRightButtons.push_back(std::move(btnDrawP2));

    auto btnResignP2 = std::make_unique<Button>(1050.0f, 308.0f, 115.0f, 38.0f, Loc::get(LocKey::SURRENDER), font, [this]() {
        thucHienDauHang(Mau::DEN);
    });
    btnResignP2->setCharacterSize(14);
    btnResignP2->setColors(sf::Color(75, 32, 32, 240), sf::Color(110, 45, 45, 255), sf::Color(55, 22, 22, 255));
    inGameRightButtons.push_back(std::move(btnResignP2));

    // 3. Draw Offer Dialog Buttons
    bool vi = (Loc::getLanguage() == Language::TIENG_VIET);
    btnAcceptDraw = std::make_unique<Button>(410.0f, 430.0f, 170.0f, 44.0f,
        vi ? "Đồng Ý (Hòa)" : "Accept Draw", font, [this]() {
        chapNhanHoa();
    });
    btnAcceptDraw->setCharacterSize(15);
    btnAcceptDraw->setColors(sf::Color(45, 95, 65, 250), sf::Color(65, 135, 95, 255), sf::Color(35, 75, 50, 255));

    btnDeclineDraw = std::make_unique<Button>(620.0f, 430.0f, 170.0f, 44.0f,
        vi ? "Từ Chối" : "Decline", font, [this]() {
        tuChoiHoa();
    });
    btnDeclineDraw->setCharacterSize(15);
    btnDeclineDraw->setColors(sf::Color(95, 45, 45, 250), sf::Color(135, 65, 65, 255), sf::Color(75, 35, 35, 255));

    // 2. Pause Modal Buttons (Stage 4, Modal centered at 400, 170, W=400, H=460)
    // Row 0 (Y=245): Large Resume button (320 x 44px, X=440)
    auto btnResume = std::make_unique<Button>(440.0f, 245.0f, 320.0f, 44.0f, Loc::get(LocKey::RESUME), font, [this]() {
        dongMenuPopUp();
    });
    btnResume->setColors(sf::Color(62, 42, 26, 245), sf::Color(96, 68, 40, 255), sf::Color(45, 30, 18, 255));
    inGameMenuButtons.push_back(std::move(btnResume));

    // Row 1 (Y=300): New Game (440, 300, 155, 40) & Draw Offer (605, 300, 155, 40)
    auto btnNew = std::make_unique<Button>(440.0f, 300.0f, 155.0f, 40.0f, Loc::get(LocKey::NEW_GAME), font, [this]() {
        dongMenuPopUp();
        batDauTroChoiMoi(cheDoChoi);
    });
    btnNew->setColors(sf::Color(48, 32, 22, 240), sf::Color(76, 54, 36, 255), sf::Color(35, 22, 15, 255));
    inGameMenuButtons.push_back(std::move(btnNew));

    auto btnDrawOffer = std::make_unique<Button>(605.0f, 300.0f, 155.0f, 40.0f, Loc::get(LocKey::DRAW_OFFER), font, [this]() {
        dongMenuPopUp();
        thucHienXinHoa();
    });
    btnDrawOffer->setColors(sf::Color(48, 32, 22, 240), sf::Color(76, 54, 36, 255), sf::Color(35, 22, 15, 255));
    inGameMenuButtons.push_back(std::move(btnDrawOffer));

    // Row 2 (Y=350): Save Game (440, 350, 155, 40) & Settings (605, 350, 155, 40)
    auto btnSave = std::make_unique<Button>(440.0f, 350.0f, 155.0f, 40.0f, Loc::get(LocKey::SAVE_GAME), font, [this]() {
        dongMenuPopUp();
        thucHienLuuGame();
    });
    btnSave->setColors(sf::Color(48, 32, 22, 240), sf::Color(76, 54, 36, 255), sf::Color(35, 22, 15, 255));
    inGameMenuButtons.push_back(std::move(btnSave));

    auto btnSettings = std::make_unique<Button>(605.0f, 350.0f, 155.0f, 40.0f, Loc::get(LocKey::MENU_SETTINGS), font, [this]() {
        dongMenuPopUp();
        trangThaiTruocSettings = TrangThai::DANG_CHOI;
        matchMenuTuDangChoi = true;
        trangThai = TrangThai::MENU_CHINH;
        menuChinh.triggerOpenAnimation(MenuSection::SETTINGS);
    });
    btnSettings->setColors(sf::Color(48, 32, 22, 240), sf::Color(76, 54, 36, 255), sf::Color(35, 22, 15, 255));
    inGameMenuButtons.push_back(std::move(btnSettings));

    // Row 3 (Y=400): Main Menu (440, 400, 320, 44)
    auto btnQuit = std::make_unique<Button>(440.0f, 400.0f, 320.0f, 44.0f, Loc::get(LocKey::BACK_MENU), font, [this]() {
        dongMenuPopUp();
        matchMenuTuDangChoi = false;
        trangThai = TrangThai::MENU_CHINH;
        menuChinh.triggerOpenAnimation(MenuSection::PLAY);
    });
    btnQuit->setColors(sf::Color(72, 26, 22, 240), sf::Color(102, 38, 30, 255), sf::Color(48, 18, 14, 255));
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
            else if (key == "p1Aura" || key == "p2Aura") {
                // Deprecated: silently ignored for backward compatibility
            }
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
    if (n1.empty()) n1 = (Loc::getLanguage() == Language::TIENG_VIET ? "Người chơi 1" : "Player 1");
    std::string n2 = inputP2Name->getValue();
    if (n2.empty()) n2 = (Loc::getLanguage() == Language::TIENG_VIET ? "Người chơi 2" : "Player 2");

    if (pvpFirstPlayer == 1) {
        tenNguoiChoiDo = n1;
        nhanVatDo = p1CharIndex;
        tenNguoiChoiDen = n2;
        nhanVatDen = p2CharIndex;
    } else {
        tenNguoiChoiDo = n2;
        nhanVatDo = p2CharIndex;
        tenNguoiChoiDen = n1;
        nhanVatDen = p1CharIndex;
    }

    batDauTroChoiMoi(CheDoChoi::HAI_NGUOI);
    std::string msg = (Loc::getLanguage() == Language::TIENG_VIET)
        ? (tenNguoiChoiDo + " cầm quân ĐỎ đi trước!")
        : (tenNguoiChoiDo + " plays RED and moves first!");
    hienToast(msg);
}

void GameManager::batDauPvai() {
    std::string n = inputAiPlayerName->getValue();
    if (n.empty()) n = (Loc::getLanguage() == Language::TIENG_VIET ? "Người chơi" : "Player");

    Mau chosenSide = Mau::DO;
    if (aiFirstChoice == 0) chosenSide = Mau::DO;
    else if (aiFirstChoice == 1) chosenSide = Mau::DEN;
    else chosenSide = (turnRandomizer.flip() == TurnResult::PLAYER1_RED) ? Mau::DO : Mau::DEN;

    aiPlayerSide = chosenSide;
    
    std::string aiName = "AI (" + Loc::get((aiDifficulty == DoKho::DE) ? LocKey::DIFF_EASY : ((aiDifficulty == DoKho::TRUNG_BINH) ? LocKey::DIFF_MED : LocKey::DIFF_HARD)) + ")";

    if (chosenSide == Mau::DO) {
        tenNguoiChoiDo = n;
        nhanVatDo = aiPlayerCharIndex;
        tenNguoiChoiDen = aiName;
        nhanVatDen = 0;
    } else {
        tenNguoiChoiDo = aiName;
        nhanVatDo = 0;
        tenNguoiChoiDen = n;
        nhanVatDen = aiPlayerCharIndex;
    }

    batDauTroChoiMoi(CheDoChoi::VOI_MAY);
    std::string msg = (Loc::getLanguage() == Language::TIENG_VIET)
        ? (tenNguoiChoiDo + " cầm quân ĐỎ đi trước!")
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
    hienPopupXinHoa = false;
    victoryParticlesActive = false;
    victoryParticles.clear();
    khoiTaoNutTrongGame();
    
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
        if (thoiGianNuocDiSai > 0.0f) {
            thoiGianNuocDiSai -= dt;
        }
        if (trangThai == TrangThai::RANDOM_LUOT) {
            capNhatRandomRoll(dt);
        }
        if (victoryParticlesActive) {
            capNhatVictoryParticles(dt);
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
    else if (trangThai == TrangThai::RANDOM_LUOT) {
        if (!isRandomRolling) {
            if (!rollFinished) {
                if (btnRoll) btnRoll->update(mousePos);
            } else {
                if (btnStartFromRoll) btnStartFromRoll->update(mousePos);
                if (btnReroll) btnReroll->update(mousePos);
            }
            if (btnBackFromRoll) btnBackFromRoll->update(mousePos);
        }
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
        } else if (hienPopupXinHoa) {
            if (btnAcceptDraw) btnAcceptDraw->update(mousePos);
            if (btnDeclineDraw) btnDeclineDraw->update(mousePos);
        } else if (hienMenuPopUp) {
            for (auto& btn : inGameMenuButtons) btn->update(mousePos);
        } else {
            for (auto& btn : inGameButtons) btn->update(mousePos);
            for (auto& btn : inGameRightButtons) btn->update(mousePos);
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
            else if (trangThai == TrangThai::RANDOM_LUOT) {
                if (!isRandomRolling) {
                    if (!rollFinished) {
                        if (btnRoll) btnRoll->update(mousePos);
                    } else {
                        if (btnStartFromRoll) btnStartFromRoll->update(mousePos);
                        if (btnReroll) btnReroll->update(mousePos);
                    }
                    if (btnBackFromRoll) btnBackFromRoll->update(mousePos);
                }
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
                } else if (hienPopupXinHoa) {
                    if (btnAcceptDraw) btnAcceptDraw->update(mousePos);
                    if (btnDeclineDraw) btnDeclineDraw->update(mousePos);
                } else if (hienMenuPopUp) {
                    for (auto& btn : inGameMenuButtons) btn->update(mousePos);
                } else {
                    for (auto& btn : inGameButtons) btn->update(mousePos);
                    for (auto& btn : inGameRightButtons) btn->update(mousePos);
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
            if (pvpSetupStep == 1) {
                inputP1Name->handleTextEntered(unicode);
            } else {
                inputP2Name->handleTextEntered(unicode);
            }
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
    else if (trangThai == TrangThai::RANDOM_LUOT) {
        if (!isRandomRolling) {
            float dx = static_cast<float>(viTri.x) - 600.0f;
            float dy = static_cast<float>(viTri.y) - 320.0f;
            bool clickCoin = (dx * dx + dy * dy <= 70.0f * 70.0f);

            if (!rollFinished) {
                if (clickCoin || (btnRoll && btnRoll->contains(viTri))) {
                    batDauRandomRoll();
                }
            } else {
                if (clickCoin || (btnStartFromRoll && btnStartFromRoll->contains(viTri))) {
                    if (btnStartFromRoll) btnStartFromRoll->handleClick(viTri);
                }
            }
            if (btnBackFromRoll && btnBackFromRoll->contains(viTri)) {
                btnBackFromRoll->handleClick(viTri);
            }
        }
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

        if (hienPopupXinHoa) {
            if (btnAcceptDraw && btnAcceptDraw->contains(viTri)) {
                btnAcceptDraw->handleClick(viTri);
                return;
            }
            if (btnDeclineDraw && btnDeclineDraw->contains(viTri)) {
                btnDeclineDraw->handleClick(viTri);
                return;
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

        if (thoiGianNuocDiSai > 0.0f) {
            thoiGianNuocDiSai = 0.0f;
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

        for (auto& btn : inGameRightButtons) {
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
            if (playSetupMode == 0) {
                bool needsRoll = (aiFirstChoice == 2);
                cheDoChoi = CheDoChoi::VOI_MAY;
                if (needsRoll) {
                    khoiTaoRandomLuotScene();
                    trangThai = TrangThai::RANDOM_LUOT;
                } else {
                    batDauPvai();
                }
            } else if (playSetupMode == 1) {
                cheDoChoi = CheDoChoi::HAI_NGUOI;
                khoiTaoRandomLuotScene();
                trangThai = TrangThai::RANDOM_LUOT;
            } else {
                nhanVatDo = 0; nhanVatDen = 0;
                batDauTroChoiMoi(CheDoChoi::DEMO_2XE_2VOI);
            }
            return;
        }
        return;
    }

    if (trangThai == TrangThai::RANDOM_LUOT) {
        if (key == sf::Keyboard::Escape) {
            soundManager.play(SoundType::CLICK);
            khoiTaoPlaySetupButtons();
            trangThai = (cheDoChoi == CheDoChoi::HAI_NGUOI) ? TrangThai::PVP_SETUP : TrangThai::PVAI_SETUP;
            return;
        }
        if (key == sf::Keyboard::Return || key == sf::Keyboard::Space) {
            if (!isRandomRolling) {
                if (!rollFinished) {
                    batDauRandomRoll();
                } else {
                    soundManager.play(SoundType::CLICK);
                    if (cheDoChoi == CheDoChoi::HAI_NGUOI) {
                        pvpFirstPlayer = (rollWinner == 0) ? 1 : 2;
                        batDauPvp();
                    } else if (cheDoChoi == CheDoChoi::VOI_MAY) {
                        aiFirstChoice = (rollWinner == 0) ? 0 : 1;
                        batDauPvai();
                    }
                }
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
            khoiTaoVictoryParticles();
        }
    } else {
        thoiGianConDen -= dt;
        if (thoiGianConDen <= 0.0f) {
            thoiGianConDen = 0.0f;
            banCo.setKetThuc(true);
            banCo.setNguoiThang(Mau::DO);
            lyDoKetThuc = (Loc::getLanguage() == Language::TIENG_VIET) ? "Quan Den het gio!" : "Black time out!";
            soundManager.play(SoundType::VICTORY);
            khoiTaoVictoryParticles();
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

                bool biChieu = banCo.kiemTraChieu(banCo.getLuotChoi());
                if (banCo.getKetThuc()) {
                    if (banCo.getNguoiThang() == Mau::HOA) {
                        if (banCo.isHoaDoLapLai()) {
                            lyDoKetThuc = (Loc::getLanguage() == Language::TIENG_VIET)
                                ? "Hòa cờ do lặp lại nước đi 3 lần!"
                                : "Draw by threefold repetition rule!";
                        } else {
                            lyDoKetThuc = (Loc::getLanguage() == Language::TIENG_VIET)
                                ? "Không còn nước đi hợp lệ (Hòa cờ)."
                                : "No legal moves left (Draw).";
                        }
                        soundManager.play(SoundType::CLICK);
                    } else if (banCo.getNguoiThang() == aiPlayerSide) {
                        lyDoKetThuc = (Loc::getLanguage() == Language::TIENG_VIET)
                            ? "Bạn đã chiếu bí máy AI!"
                            : "You checkmated the AI!";
                        soundManager.play(SoundType::VICTORY);
                        khoiTaoVictoryParticles();
                    } else {
                        lyDoKetThuc = (Loc::getLanguage() == Language::TIENG_VIET)
                            ? "Máy AI đã chiếu bí bạn!"
                            : "AI checkmated you!";
                        soundManager.play(SoundType::DEFEAT);
                    }
                } else if (biChieu) {
                    if (checkAlarmSound) soundManager.play(SoundType::CHECK);
                }
            }
        } else {
            banCo.setKetThuc(true);
            if (banCo.kiemTraChieu(aiMau)) {
                banCo.setNguoiThang(aiPlayerSide);
                lyDoKetThuc = (Loc::getLanguage() == Language::TIENG_VIET)
                    ? "Bạn đã chiếu bí máy AI!"
                    : "You checkmated the AI!";
                soundManager.play(SoundType::VICTORY);
                khoiTaoVictoryParticles();
            } else {
                banCo.setNguoiThang(Mau::HOA);
                lyDoKetThuc = (Loc::getLanguage() == Language::TIENG_VIET)
                    ? "Máy AI không còn nước đi hợp lệ (Hòa cờ)."
                    : "AI has no legal moves left (Draw).";
                soundManager.play(SoundType::CLICK);
            }
        }
    }
}

void GameManager::chonQuan(int hang, int cot) {
    auto q = banCo.timQuan(hang, cot);
    if (q) {
        if (q->getMau() == banCo.getLuotChoi()) {
            quanDangChon = q;
            soundManager.play(SoundType::CLICK);
            tinhCacNuocDiHopLe();
        } else {
            viTriNuocDiSai = sf::Vector2i(hang, cot);
            thoiGianNuocDiSai = 1.6f;
            lyDoNuocDiSai = (Loc::getLanguage() == Language::TIENG_VIET)
                ? "Không thể chọn quân cờ của đối phương!"
                : "Cannot select opponent's piece!";
            soundManager.play(SoundType::ILLEGAL);
        }
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
            viTriNuocDiSai = sf::Vector2i(-1, -1);
            thoiGianNuocDiSai = 0.0f;

            if (anQuan) soundManager.play(SoundType::CAPTURE);
            else soundManager.play(SoundType::MOVE);

            bool biChieu = banCo.kiemTraChieu(banCo.getLuotChoi());
            if (banCo.getKetThuc()) {
                if (banCo.getNguoiThang() == Mau::HOA) {
                    if (banCo.isHoaDoLapLai()) {
                        lyDoKetThuc = (Loc::getLanguage() == Language::TIENG_VIET)
                            ? "Hòa cờ do lặp lại nước đi 3 lần!"
                            : "Draw by threefold repetition rule!";
                    } else {
                        lyDoKetThuc = (Loc::getLanguage() == Language::TIENG_VIET)
                            ? "Không còn nước đi hợp lệ (Hòa cờ theo luật)."
                            : "No legal moves left (Draw).";
                    }
                    soundManager.play(SoundType::CLICK);
                } else {
                    lyDoKetThuc = (Loc::getLanguage() == Language::TIENG_VIET)
                        ? "Chiếu bí đối phương!"
                        : "Checkmate!";
                    soundManager.play(SoundType::VICTORY);
                    khoiTaoVictoryParticles();
                }
            } else if (biChieu) {
                if (checkAlarmSound) soundManager.play(SoundType::CHECK);
            }
        }
    } else {
        std::string lyDo = xacDinhLyDoNuocDiSai(quanDangChon, hang, cot);
        viTriNuocDiSai = sf::Vector2i(hang, cot);
        thoiGianNuocDiSai = 2.0f;
        lyDoNuocDiSai = lyDo;
        soundManager.play(SoundType::ILLEGAL);
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
    // Undo removed as requested
}

void GameManager::thucHienDiTiep() {
    // Redo removed as requested
}

void GameManager::thucHienDauHang() {
    thucHienDauHang(banCo.getLuotChoi());
}

void GameManager::thucHienDauHang(Mau mau) {
    if (banCo.getKetThuc()) return;
    banCo.dauHang(mau);
    std::string tenNguoi = (mau == Mau::DO) ? tenNguoiChoiDo : tenNguoiChoiDen;
    bool vi = (Loc::getLanguage() == Language::TIENG_VIET);
    if (mau == Mau::DO) {
        lyDoKetThuc = vi ? (tenNguoi + " (Quân Đỏ) đã xin đầu hàng.") : (tenNguoi + " (Red) has resigned.");
    } else {
        lyDoKetThuc = vi ? (tenNguoi + " (Quân Đen) đã xin đầu hàng.") : (tenNguoi + " (Black) has resigned.");
    }
    soundManager.play(SoundType::DEFEAT);
    khoiTaoVictoryParticles();
}

void GameManager::thucHienXinHoa() {
    thucHienXinHoa(banCo.getLuotChoi());
}

void GameManager::thucHienXinHoa(Mau mau) {
    if (banCo.getKetThuc()) return;
    nguoiXinHoa = mau;
    hienPopupXinHoa = true;
    soundManager.play(SoundType::CLICK);
}

void GameManager::chapNhanHoa() {
    hienPopupXinHoa = false;
    banCo.xinHoa();
    bool vi = (Loc::getLanguage() == Language::TIENG_VIET);
    lyDoKetThuc = vi ? "Hai bên đã đồng thuận hòa cờ." : "Both players agreed to a draw.";
    soundManager.play(SoundType::CLICK);
    khoiTaoVictoryParticles();
}

void GameManager::tuChoiHoa() {
    hienPopupXinHoa = false;
    soundManager.play(SoundType::CLICK);
    bool vi = (Loc::getLanguage() == Language::TIENG_VIET);
    hienToast(vi ? "Lời đề nghị hòa cờ đã bị từ chối!" : "Draw offer was declined!");
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

    auto& am = AssetManager::get();

    textureBackground = am.texture(TextureId::BACKGROUND_PAVILION);
    texturePlayBg     = am.texture(TextureId::BACKGROUND_PAVILION);
    textureBanCo      = am.texture(TextureId::BOARD);

    // Board parameters (centered on 1200x800)
    kichThuocO = 68.0f;
    offsetX    = 328.0f;
    offsetY    = 110.0f;

    // Load pieces
    const std::vector<std::string> pieceTypes = { "Tuong", "Si", "Voi", "Ma", "Xe", "Phao", "Tot" };
    for (const auto& t : pieceTypes) {
        textureQuanDo[t]  = am.getPieceTexture(t, Mau::DO);
        textureQuanDen[t] = am.getPieceTexture(t, Mau::DEN);
    }

    taiAvatarTextures();
    daTaiTextures = true;
}

void GameManager::taiAvatarTextures() {
    if (daTaiAvatarTextures) return;
    auto& am = AssetManager::get();
    for (int i = 0; i < 5; ++i) {
        avatarTextures[i] = am.getAvatarTexture(i);
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
    else if (trangThai == TrangThai::RANDOM_LUOT) {
        veRandomLuot();
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
            veVictoryParticles();
            vePopupKetThuc();
        } else if (hienPopupXinHoa) {
            vePopupXinHoa();
        } else if (hienMenuPopUp) {
            veMenuPopUpInGame();
        }

        if (thoiGianNuocDiSai > 0.0f) {
            vePopupNuocDiSai();
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

    // 3. Invalid Move Visual Feedback on the board
    if (thoiGianNuocDiSai > 0.0f && viTriNuocDiSai.x != -1) {
        sf::Vector2f badPos = chuyenDoiToaDoBanCoThanhManHinh(viTriNuocDiSai.x, viTriNuocDiSai.y);
        float pulse = 0.5f + 0.5f * std::sin(bannerPulseClock.getElapsedTime().asSeconds() * 12.0f);
        
        // Red warning circle
        sf::CircleShape badCircle(28.0f);
        badCircle.setOrigin(28.0f, 28.0f);
        badCircle.setPosition(badPos);
        badCircle.setFillColor(sf::Color(230, 30, 30, 90));
        badCircle.setOutlineThickness(3.0f + pulse * 1.5f);
        badCircle.setOutlineColor(sf::Color(255, 50, 50, static_cast<sf::Uint8>(200 + pulse * 55)));
        window.draw(badCircle);

        // Bold red 'X' mark
        float arm = 14.0f;
        sf::Vertex xMark[] = {
            sf::Vertex(sf::Vector2f(badPos.x - arm, badPos.y - arm), sf::Color(255, 240, 240)),
            sf::Vertex(sf::Vector2f(badPos.x + arm, badPos.y + arm), sf::Color(255, 240, 240)),
            sf::Vertex(sf::Vector2f(badPos.x + arm, badPos.y - arm), sf::Color(255, 240, 240)),
            sf::Vertex(sf::Vector2f(badPos.x - arm, badPos.y + arm), sf::Color(255, 240, 240))
        };
        window.draw(xMark, 4, sf::Lines);
    }

    // 4. King under CHECK Visual Effect (Dramatic pulsing aura & shockwave)
    if (!banCo.getKetThuc() && banCo.kiemTraChieu(banCo.getLuotChoi())) {
        for (const auto& q : banCo.getCacQuan()) {
            if (q && !q->getDaBiAn() && q->getMau() == banCo.getLuotChoi() && q->laTuong()) {
                sf::Vector2f kPos = chuyenDoiToaDoBanCoThanhManHinh(q->getHang(), q->getCot());
                float pulse = 0.5f + 0.5f * std::sin(bannerPulseClock.getElapsedTime().asSeconds() * 8.0f);
                
                // Pulsing red danger glow
                float ringR = 34.0f + pulse * 6.0f;
                sf::CircleShape checkRing(ringR);
                checkRing.setOrigin(ringR, ringR);
                checkRing.setPosition(kPos);
                checkRing.setFillColor(sf::Color(255, 30, 20, static_cast<sf::Uint8>(60 + pulse * 70)));
                checkRing.setOutlineThickness(2.5f);
                checkRing.setOutlineColor(sf::Color(255, 60, 40, static_cast<sf::Uint8>(180 + pulse * 75)));
                window.draw(checkRing);

                // Expanding shockwave ring
                float wavePulse = std::fmod(bannerPulseClock.getElapsedTime().asSeconds() * 1.5f, 1.0f);
                float waveR = 28.0f + wavePulse * 24.0f;
                sf::CircleShape wave(waveR);
                wave.setOrigin(waveR, waveR);
                wave.setPosition(kPos);
                wave.setFillColor(sf::Color::Transparent);
                wave.setOutlineThickness(2.0f);
                wave.setOutlineColor(sf::Color(255, 50, 40, static_cast<sf::Uint8>((1.0f - wavePulse) * 210)));
                window.draw(wave);
                break;
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
    bool checkAlert = !banCo.getKetThuc() && banCo.kiemTraChieu(banCo.getLuotChoi());
    bool tiengViet = (Loc::getLanguage() == Language::TIENG_VIET);
    bool isRed = (banCo.getLuotChoi() == Mau::DO);
    float pulse = 0.5f + 0.5f * std::sin(bannerPulseClock.getElapsedTime().asSeconds() * 8.0f);

    float bw = 460.0f;
    float bh = 46.0f;
    float bx = (1200.0f - bw) / 2.0f;
    float by = 20.0f;

    if (banCo.getKetThuc()) {
        sf::RectangleShape banner(sf::Vector2f(bw, bh));
        banner.setPosition(bx, by);
        banner.setOutlineThickness(2.0f);
        if (banCo.getNguoiThang() != Mau::HOA) {
            banner.setFillColor(sf::Color(140, 25, 25, static_cast<sf::Uint8>(210 + pulse * 45)));
            banner.setOutlineColor(sf::Color(255, 215, 0));
            std::string winner = (banCo.getNguoiThang() == Mau::DO) ? tenNguoiChoiDo : tenNguoiChoiDen;
            std::string winnerSide = (banCo.getNguoiThang() == Mau::DO) ? (tiengViet ? "BÊN ĐỎ" : "RED") : (tiengViet ? "BÊN ĐEN" : "BLACK");
            window.draw(banner);
            ve4GocLuaChon(window, sf::Vector2f(bx, by), sf::Vector2f(bw, bh), 2.0f, 1.5f);

            sf::Text t;
            t.setFont(fontBold);
            t.setCharacterSize(16);
            t.setStyle(sf::Text::Bold);
            t.setString(toUtf8(tiengViet ? ("CHIẾU BÍ! " + winner + " (" + winnerSide + ") CHIẾN THẮNG!") : ("CHECKMATE! " + winner + " (" + winnerSide + ") WINS!")));
            t.setFillColor(sf::Color(255, 230, 100));
            sf::FloatRect tb = t.getLocalBounds();
            t.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));
            t.setPosition(std::round(1200.0f / 2.0f), std::round(by + bh / 2.0f));
            window.draw(t);
        } else {
            banner.setFillColor(sf::Color(40, 50, 75, 230));
            banner.setOutlineColor(sf::Color(160, 200, 240));
            window.draw(banner);
            ve4GocLuaChon(window, sf::Vector2f(bx, by), sf::Vector2f(bw, bh), 2.0f, 1.5f);

            sf::Text t;
            t.setFont(fontBold);
            t.setCharacterSize(16);
            t.setStyle(sf::Text::Bold);
            t.setString(toUtf8(tiengViet ? "HÒA CỜ! HAI BÊN BẤT PHÂN THẮNG BẠI" : "DRAW MATCH! NO WINNER"));
            t.setFillColor(sf::Color(255, 230, 140));
            sf::FloatRect tb = t.getLocalBounds();
            t.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));
            t.setPosition(std::round(1200.0f / 2.0f), std::round(by + bh / 2.0f));
            window.draw(t);
        }
    } else if (checkAlert) {
        sf::RectangleShape banner(sf::Vector2f(bw, bh));
        banner.setPosition(bx, by);
        banner.setFillColor(sf::Color(160, 20, 20, static_cast<sf::Uint8>(190 + pulse * 65)));
        banner.setOutlineThickness(2.5f);
        banner.setOutlineColor(sf::Color(255, 220, 60));
        window.draw(banner);

        ve4GocLuaChon(window, sf::Vector2f(bx, by), sf::Vector2f(bw, bh), 2.0f, 1.5f);

        sf::Text t;
        t.setFont(fontBold);
        t.setCharacterSize(16);
        t.setStyle(sf::Text::Bold);
        std::string sideStr = (banCo.getLuotChoi() == Mau::DO) ? (tiengViet ? "BÊN ĐỎ ĐANG BỊ CHIẾU TƯỚNG!" : "RED KING IS UNDER CHECK!")
                                                              : (tiengViet ? "BÊN ĐEN ĐANG BỊ CHIẾU TƯỚNG!" : "BLACK KING IS UNDER CHECK!");
        t.setString(toUtf8("[!]  " + sideStr + "  [!]"));
        t.setFillColor(sf::Color(255, 245, 120));
        sf::FloatRect tb = t.getLocalBounds();
        t.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));
        t.setPosition(std::round(1200.0f / 2.0f), std::round(by + bh / 2.0f));
        window.draw(t);
    } else {
        std::string turnStr = isRed ? Loc::get(LocKey::TURN_RED) : Loc::get(LocKey::TURN_BLACK);

        FantasyUI::get().drawRibbonTitle(window, 1200.0f / 2.0f, 20.0f, 260.0f, 46.0f,
                                         turnStr, font, 16,
                                         isRed ? sf::Color(255, 215, 140) : sf::Color(255, 230, 140),
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
    float ph = 190.0f;
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

    // 1. Player 1 Card (Red) - EBONY_WOOD (X=35, Y=110, W=240, H=190)
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

    // Player Name at Y = 170
    sf::Text rName;
    rName.setFont(fontBold);
    rName.setCharacterSize(15);
    rName.setStyle(sf::Text::Bold);
    rName.setString(toUtf8(tenNguoiChoiDo));
    rName.setFillColor(sf::Color(255, 230, 140));
    sf::FloatRect nb = rName.getLocalBounds();
    rName.setOrigin(std::round(nb.left + nb.width / 2.0f), std::round(nb.top));
    rName.setPosition(std::round(px + pw / 2.0f), 170.0f);
    window.draw(rName);

    // Digital Countdown Timer in JADE_INSET slot at Y = 232
    float timerSlotY = py + 122.0f;
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

    // 2. Action buttons for Player 1 (Draw, Resign)
    for (auto& btn : inGameButtons) {
        btn->draw(window);
    }

    // 3. Captured Pieces Tray for Red (displays Black pieces captured by Red)
    std::string capTitleRed = tiengViet ? "QUÂN ĐÃ BẮT (ĐỎ)" : "CAPTURED PIECES (RED)";
    veKhayQuanBiAn(px, 356.0f, pw, 140.0f, Mau::DEN, capTitleRed);

    // 4. Match Status Panel at X=35.0f, Y=508.0f, W=240.0f, H=247.0f (Symmetrical to Move History)
    float infoX = px;
    float infoY = 508.0f;
    float infoW = pw;
    float infoH = 247.0f;

    FantasyUI::get().drawPanel(window, sf::FloatRect(infoX, infoY, infoW, infoH), OrientalPanelStyle::EBONY_WOOD);

    sf::RectangleShape infoInset(sf::Vector2f(infoW - 18.0f, infoH - 18.0f));
    infoInset.setPosition(infoX + 9.0f, infoY + 9.0f);
    infoInset.setFillColor(sf::Color(32, 20, 15));
    infoInset.setOutlineThickness(1.0f);
    infoInset.setOutlineColor(sf::Color(140, 102, 48, 175));
    window.draw(infoInset);

    // Header: "THÔNG TIN TRẬN ĐẤU" / "MATCH INFO"
    sf::Text titleInfoShadow;
    titleInfoShadow.setFont(fontBold);
    titleInfoShadow.setCharacterSize(14);
    titleInfoShadow.setStyle(sf::Text::Bold);
    titleInfoShadow.setString(toUtf8(tiengViet ? "THÔNG TIN TRẬN ĐẤU" : "MATCH INFO"));
    titleInfoShadow.setFillColor(sf::Color(0, 0, 0, 140));
    sf::FloatRect tib = titleInfoShadow.getLocalBounds();
    titleInfoShadow.setOrigin(std::round(tib.left + tib.width / 2.0f), std::round(tib.top));
    titleInfoShadow.setPosition(std::round(infoX + infoW / 2.0f) + 1.0f, infoY + 12.0f);
    window.draw(titleInfoShadow);

    sf::Text titleInfo = titleInfoShadow;
    titleInfo.setFillColor(sf::Color(235, 198, 118));
    titleInfo.setPosition(std::round(infoX + infoW / 2.0f), infoY + 11.0f);
    window.draw(titleInfo);

    sf::RectangleShape divInfo(sf::Vector2f(infoW - 36.0f, 1.5f));
    divInfo.setPosition(infoX + 18.0f, infoY + 32.0f);
    divInfo.setFillColor(sf::Color(175, 132, 62, 190));
    window.draw(divInfo);

    // Calculate match stats
    std::string sMode = (cheDoChoi == CheDoChoi::HAI_NGUOI)
        ? (tiengViet ? "2 Người (PvP)" : "2 Players (PvP)")
        : (tiengViet ? "Đấu Máy (AI)" : "vs Computer (AI)");
    int currentRound = static_cast<int>(banCo.getLichSuNuocDi().size() / 2) + 1;
    std::string sRound = std::to_string(currentRound);
    std::string sTurn = isRedTurn
        ? (tiengViet ? "Bên Đỏ" : "Red Side")
        : (tiengViet ? "Bên Đen" : "Black Side");
    sf::Color turnColor = isRedTurn ? sf::Color(245, 120, 110) : sf::Color(135, 185, 245);

    bool inCheck = banCo.kiemTraChieuTuong();
    std::string sStatus;
    sf::Color statusColor;
    if (banCo.isHoaDoLapLai()) {
        sStatus = tiengViet ? "Hòa (Lặp 3 nước)" : "Draw (Repetition)";
        statusColor = sf::Color(255, 205, 90);
    } else if (inCheck) {
        sStatus = tiengViet ? "CHIẾU TƯỚNG!" : "CHECK!";
        statusColor = sf::Color(255, 75, 75);
    } else {
        sStatus = tiengViet ? "Bình thường" : "Normal";
        statusColor = sf::Color(145, 215, 145);
    }

    int redCap = 0, blackCap = 0;
    for (const auto& q : banCo.getCacQuan()) {
        if (q && q->getDaBiAn()) {
            if (q->getMau() == Mau::DEN) redCap++;
            else blackCap++;
        }
    }

    struct InfoRow {
        std::string label;
        std::string val;
        sf::Color valColor;
    };
    std::vector<InfoRow> rows = {
        { tiengViet ? "Chế độ:" : "Mode:", sMode, sf::Color(255, 230, 140) },
        { tiengViet ? "Hiệp đấu:" : "Round:", sRound, sf::Color(255, 230, 140) },
        { tiengViet ? "Lượt đi:" : "Turn:", sTurn, turnColor },
        { tiengViet ? "Tình trạng:" : "Status:", sStatus, statusColor },
        { tiengViet ? "Đã bắt:" : "Captured:", (tiengViet ? "Đỏ " : "Red ") + std::to_string(redCap) + (tiengViet ? " | Đen " : " | Black ") + std::to_string(blackCap), sf::Color(255, 230, 140) }
    };

    float rowY = infoY + 44.0f;
    for (size_t i = 0; i < rows.size(); ++i) {
        if (i % 2 == 1) {
            sf::RectangleShape strip(sf::Vector2f(infoW - 24.0f, 32.0f));
            strip.setPosition(infoX + 12.0f, rowY - 4.0f);
            strip.setFillColor(sf::Color(255, 236, 196, 10));
            window.draw(strip);
        }

        sf::Text lText;
        lText.setFont(fontBold);
        lText.setCharacterSize(13);
        lText.setStyle(sf::Text::Bold);
        lText.setString(toUtf8(rows[i].label));
        lText.setFillColor(sf::Color(220, 200, 165));
        lText.setPosition(infoX + 16.0f, rowY);
        window.draw(lText);

        sf::Text vText;
        vText.setFont(fontBold);
        vText.setCharacterSize(14);
        vText.setStyle(sf::Text::Bold);
        vText.setString(toUtf8(rows[i].val));
        vText.setFillColor(rows[i].valColor);
        sf::FloatRect vtb = vText.getLocalBounds();
        float maxValW = 125.0f;
        if (vtb.width > maxValW) {
            vText.setScale(maxValW / vtb.width, 1.0f);
        }
        vText.setPosition(infoX + infoW - 16.0f - std::min(vtb.width, maxValW), rowY);
        window.draw(vText);

        rowY += 38.0f;
    }
}

void GameManager::veKhayQuanBiAn(float x, float y, float w, float h, Mau mauQuanBiAn, const std::string& tieuDe) {
    FantasyUI::get().drawPanel(window, sf::FloatRect(x, y, w, h), OrientalPanelStyle::JADE_INSET);

    sf::Text capTitle;
    capTitle.setFont(fontBold);
    capTitle.setCharacterSize(13);
    capTitle.setStyle(sf::Text::Bold);
    capTitle.setString(toUtf8(tieuDe));
    capTitle.setFillColor(sf::Color(225, 200, 155));
    sf::FloatRect ctbTitle = capTitle.getLocalBounds();
    capTitle.setOrigin(std::round(ctbTitle.left + ctbTitle.width / 2.0f), std::round(ctbTitle.top));
    capTitle.setPosition(std::round(x + w / 2.0f), y + 7.0f);
    window.draw(capTitle);

    int capCount = 0;
    float startChipX = x + 12.0f;
    float startChipY = y + 26.0f;
    float chipR = 13.0f;
    float chipStepX = 36.0f;
    float chipStepY = 32.0f;

    for (const auto& q : banCo.getCacQuan()) {
        if (!q || !q->getDaBiAn() || q->getMau() != mauQuanBiAn) continue;

        float px = startChipX + (capCount % 6) * chipStepX;
        float py = startChipY + (capCount / 6) * chipStepY;
        bool isRed = (q->getMau() == Mau::DO);

        const sf::Texture* tex = (pieceStyle == PieceStyle::REALISTIC_WOOD) 
            ? layTextureQuan(q->layTen(), q->getMau()) : nullptr;

        if (tex != nullptr) {
            sf::CircleShape sh(chipR);
            sh.setOrigin(chipR, chipR);
            sh.setPosition(px + chipR + 1.0f, py + chipR + 1.5f);
            sh.setFillColor(sf::Color(10, 6, 4, 120));
            window.draw(sh);

            sf::Sprite sp(*tex);
            sf::Vector2u sz = tex->getSize();
            sp.setOrigin(sz.x / 2.0f, sz.y / 2.0f);
            float scale = (chipR * 2.0f) / static_cast<float>(sz.x);
            sp.setScale(scale, scale);
            sp.setPosition(px + chipR, py + chipR);
            window.draw(sp);
        } else {
            sf::CircleShape sh(chipR);
            sh.setOrigin(chipR, chipR);
            sh.setPosition(px + chipR + 1.0f, py + chipR + 1.5f);
            sh.setFillColor(sf::Color(10, 6, 4, 120));
            window.draw(sh);

            sf::CircleShape chip(chipR);
            chip.setOrigin(chipR, chipR);
            chip.setPosition(px + chipR, py + chipR);
            chip.setFillColor(isRed ? sf::Color(245, 230, 210) : sf::Color(35, 42, 52));
            chip.setOutlineThickness(1.2f);
            chip.setOutlineColor(isRed ? sf::Color(180, 40, 25) : sf::Color(80, 110, 145));
            window.draw(chip);

            sf::Text chipTxt;
            chipTxt.setFont(fontBold);
            std::string code = layKyHieuQuanTheoStyle(q->layTen(), q->getMau());
            chipTxt.setString(toUtf8(code));
            chipTxt.setCharacterSize(code.size() > 2 ? 8 : 10);
            chipTxt.setStyle(sf::Text::Bold);
            chipTxt.setFillColor(isRed ? sf::Color(170, 25, 20) : sf::Color(180, 210, 245));
            sf::FloatRect ctb = chipTxt.getLocalBounds();
            chipTxt.setOrigin(std::round(ctb.left + ctb.width / 2.0f), std::round(ctb.top + ctb.height / 2.0f));
            chipTxt.setPosition(std::round(px + chipR), std::round(py + chipR - 0.5f));
            window.draw(chipTxt);
        }

        capCount++;
        if (capCount >= 16) break;
    }

    if (capCount == 0) {
        sf::Text noCap;
        noCap.setFont(fontBold);
        noCap.setCharacterSize(16);
        noCap.setStyle(sf::Text::Bold);
        noCap.setString(toUtf8(Loc::getLanguage() == Language::TIENG_VIET ? "[ Chưa có quân bị bắt ]" : "[ No captured pieces ]"));
        noCap.setFillColor(sf::Color(255, 235, 175));
        noCap.setOutlineThickness(1.0f);
        noCap.setOutlineColor(sf::Color(20, 14, 8, 220));
        sf::FloatRect ncb = noCap.getLocalBounds();
        noCap.setOrigin(std::round(ncb.left + ncb.width / 2.0f), std::round(ncb.top + ncb.height / 2.0f));
        noCap.setPosition(std::round(x + w / 2.0f), std::round(y + h / 2.0f + 6.0f));
        window.draw(noCap);
    }
}

void GameManager::veThanhBenPhai() {
    float px = 925.0f;
    float py = 110.0f;
    float pw = 240.0f;
    float ph = 190.0f;
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

    // 1. Player 2 Card (Black) - EBONY_WOOD (X=925, Y=110, W=240, H=190)
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

    // Player Name at Y = 170
    sf::Text bName;
    bName.setFont(fontBold);
    bName.setCharacterSize(15);
    bName.setStyle(sf::Text::Bold);
    bName.setString(toUtf8(tenNguoiChoiDen));
    bName.setFillColor(sf::Color(255, 230, 140));
    sf::FloatRect nb = bName.getLocalBounds();
    bName.setOrigin(std::round(nb.left + nb.width / 2.0f), std::round(nb.top));
    bName.setPosition(std::round(px + pw / 2.0f), 170.0f);
    window.draw(bName);

    // Digital Countdown Timer in JADE_INSET slot at Y = 232
    float timerSlotY = py + 122.0f;
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
                          : sf::Color(255, 230, 140));
    sf::FloatRect tb = timerTxt.getLocalBounds();
    timerTxt.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));
    timerTxt.setPosition(std::round(timerSlotX + timerSlotW / 2.0f), std::round(timerSlotY + timerSlotH / 2.0f));
    window.draw(timerTxt);

    // 2. Action buttons for Player 2 (Draw, Resign)
    for (auto& btn : inGameRightButtons) {
        btn->draw(window);
    }

    // 3. Captured Pieces Tray for Black (displays Red pieces captured by Black)
    std::string capTitleBlack = tiengViet ? "QUÂN ĐÃ BẮT (ĐEN)" : "CAPTURED PIECES (BLACK)";
    veKhayQuanBiAn(px, 356.0f, pw, 140.0f, Mau::DO, capTitleBlack);

    // 4. Move History Box (KỲ PHỔ) at X=925.0f, Y=508.0f, W=240.0f, H=247.0f
    float histX = 925.0f;
    float histY = 508.0f;
    float histW = 240.0f;
    float histH = 247.0f;

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
    titleHistShadow.setCharacterSize(14);
    titleHistShadow.setStyle(sf::Text::Bold);
    titleHistShadow.setString(toUtf8(tiengViet ? "KỲ PHỔ" : "MOVE HISTORY"));
    titleHistShadow.setFillColor(sf::Color(0, 0, 0, 140));
    sf::FloatRect thb = titleHistShadow.getLocalBounds();
    titleHistShadow.setOrigin(std::round(thb.left + thb.width / 2.0f), std::round(thb.top));
    titleHistShadow.setPosition(std::round(histX + histW / 2.0f) + 1.0f, histY + 12.0f);
    window.draw(titleHistShadow);

    sf::Text titleHist = titleHistShadow;
    titleHist.setFillColor(sf::Color(235, 198, 118));
    titleHist.setPosition(std::round(histX + histW / 2.0f), histY + 11.0f);
    window.draw(titleHist);

    sf::RectangleShape div(sf::Vector2f(histW - 36.0f, 1.5f));
    div.setPosition(histX + 18.0f, histY + 32.0f);
    div.setFillColor(sf::Color(175, 132, 62, 190));
    window.draw(div);

    const auto& lichSu = banCo.getLichSuNuocDi();
    if (lichSu.empty()) {
        sf::Text emptyT;
        emptyT.setFont(font);
        emptyT.setCharacterSize(13);
        emptyT.setStyle(sf::Text::Bold);
        emptyT.setString(toUtf8(Loc::get(LocKey::NO_MOVES_YET)));
        emptyT.setFillColor(sf::Color(180, 158, 125));
        sf::FloatRect etb = emptyT.getLocalBounds();
        emptyT.setOrigin(std::round(etb.left + etb.width / 2.0f), std::round(etb.top + etb.height / 2.0f));
        emptyT.setPosition(std::round(histX + histW / 2.0f), std::round(histY + histH / 2.0f + 8.0f));
        window.draw(emptyT);
    } else {
        float col0X = histX + 13.0f;
        float col1X = histX + 37.0f;
        float col2X = histX + 132.0f;
        float headerY = histY + 36.0f;

        // Table Headers
        sf::Text hCol0;
        hCol0.setFont(fontBold);
        hCol0.setCharacterSize(13);
        hCol0.setStyle(sf::Text::Bold);
        hCol0.setString("#");
        hCol0.setFillColor(sf::Color(220, 195, 150));
        hCol0.setPosition(col0X, headerY);
        window.draw(hCol0);

        sf::Text hCol1;
        hCol1.setFont(fontBold);
        hCol1.setCharacterSize(13);
        hCol1.setStyle(sf::Text::Bold);
        hCol1.setString(toUtf8(tiengViet ? "Bên Đỏ" : "Red"));
        hCol1.setFillColor(sf::Color(255, 145, 135));
        hCol1.setPosition(col1X, headerY);
        window.draw(hCol1);

        sf::Text hCol2;
        hCol2.setFont(fontBold);
        hCol2.setCharacterSize(13);
        hCol2.setStyle(sf::Text::Bold);
        hCol2.setString(toUtf8(tiengViet ? "Bên Đen" : "Black"));
        hCol2.setFillColor(sf::Color(155, 205, 255));
        hCol2.setPosition(col2X, headerY);
        window.draw(hCol2);

        sf::RectangleShape divTable(sf::Vector2f(histW - 26.0f, 1.0f));
        divTable.setPosition(histX + 13.0f, headerY + 17.0f);
        divTable.setFillColor(sf::Color(140, 102, 48, 120));
        window.draw(divTable);

        // Calculate rounds
        size_t totalMoves = lichSu.size();
        size_t totalRounds = (totalMoves + 1) / 2;
        size_t maxDisplay = 8;
        size_t startRound = (totalRounds > maxDisplay) ? (totalRounds - maxDisplay) : 0;
        float rowY = headerY + 22.0f;

        for (size_t r = startRound; r < totalRounds; ++r) {
            if ((r - startRound) % 2 == 1) {
                sf::RectangleShape strip(sf::Vector2f(histW - 24.0f, 20.0f));
                strip.setPosition(histX + 12.0f, rowY - 1.0f);
                strip.setFillColor(sf::Color(255, 236, 196, 12));
                window.draw(strip);
            }

            // Col 0: Round number
            sf::Text rNum;
            rNum.setFont(fontBold);
            rNum.setCharacterSize(13);
            rNum.setStyle(sf::Text::Bold);
            rNum.setString(std::to_string(r + 1));
            rNum.setFillColor(sf::Color(210, 190, 155));
            rNum.setPosition(col0X, rowY);
            window.draw(rNum);

            // Col 1: Red move
            size_t redIdx = r * 2;
            if (redIdx < totalMoves) {
                std::string redDesc = banCo.layMoTaNuocDiNgan(lichSu[redIdx], tiengViet);
                sf::Text rTxt;
                rTxt.setFont(fontBold);
                rTxt.setCharacterSize(13);
                rTxt.setStyle(sf::Text::Bold);
                rTxt.setString(toUtf8(redDesc));
                rTxt.setFillColor(sf::Color(255, 150, 140));
                sf::FloatRect tb = rTxt.getLocalBounds();
                float maxColW = 90.0f;
                if (tb.width > maxColW) {
                    rTxt.setScale(maxColW / tb.width, 1.0f);
                }
                rTxt.setPosition(col1X, rowY);
                window.draw(rTxt);
            }

            // Col 2: Black move
            size_t blackIdx = r * 2 + 1;
            if (blackIdx < totalMoves) {
                std::string blackDesc = banCo.layMoTaNuocDiNgan(lichSu[blackIdx], tiengViet);
                sf::Text bTxt;
                bTxt.setFont(fontBold);
                bTxt.setCharacterSize(13);
                bTxt.setStyle(sf::Text::Bold);
                bTxt.setString(toUtf8(blackDesc));
                bTxt.setFillColor(sf::Color(165, 210, 255));
                sf::FloatRect tb = bTxt.getLocalBounds();
                float maxColW = 90.0f;
                if (tb.width > maxColW) {
                    bTxt.setScale(maxColW / tb.width, 1.0f);
                }
                bTxt.setPosition(col2X, rowY);
                window.draw(bTxt);
            }

            rowY += 21.0f;
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

    std::string winnerSideStr;
    if (winner == Mau::DO) {
        winnerSideStr = tiengViet ? "BÊN ĐỎ (RED)" : "RED SIDE";
    } else if (winner == Mau::DEN) {
        winnerSideStr = tiengViet ? "BÊN ĐEN (BLACK)" : "BLACK SIDE";
    }

    if (outcome == MatchOutcome::WIN) {
        if (winner == Mau::DO) {
            borderColor = sf::Color(255, 120, 100);
            badgeBgColor = sf::Color(140, 30, 25, 235);
            titleColor = sf::Color(255, 230, 130);
            badgeStr = tiengViet ? "[ CHIẾN THẮNG: BÊN ĐỎ ]" : "[ VICTORY: RED SIDE ]";
        } else if (winner == Mau::DEN) {
            borderColor = sf::Color(90, 160, 255);
            badgeBgColor = sf::Color(25, 45, 95, 235);
            titleColor = sf::Color(255, 230, 140);
            badgeStr = tiengViet ? "[ CHIẾN THẮNG: BÊN ĐEN ]" : "[ VICTORY: BLACK SIDE ]";
        } else {
            borderColor = sf::Color(225, 185, 60);
            badgeBgColor = sf::Color(32, 95, 55, 230);
            titleColor = sf::Color(255, 225, 95);
            badgeStr = tiengViet ? "[ CHIẾN THẮNG QUANG VINH ]" : "[ GLORIOUS VICTORY ]";
        }

        if (cheDoChoi == CheDoChoi::VOI_MAY) {
            subHeaderStr = tiengViet
                ? ("Chúc mừng! " + winnerSideStr + " đã xuất sắc chiến thắng AI!")
                : ("Outstanding! " + winnerSideStr + " has conquered the AI!");
        } else if (cheDoChoi == CheDoChoi::HAI_NGUOI) {
            std::string tenThang = (winner == Mau::DO) ? tenNguoiChoiDo : tenNguoiChoiDen;
            subHeaderStr = winnerSideStr + " - " + tenThang + (tiengViet ? " ĐÃ GIÀNH CHIẾN THẮNG!" : " HAS WON THE MATCH!");
        } else {
            std::string tenThang = (winner == Mau::DO) ? tenNguoiChoiDo : tenNguoiChoiDen;
            subHeaderStr = winnerSideStr + " (" + tenThang + ")" + (tiengViet ? " thắng thế trong trận diễn tập!" : " won the simulation!");
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
        titleColor = sf::Color(255, 230, 140);         // Vàng nhạt match Match Paused
        badgeStr = tiengViet ? "[ BẤT PHÂN THẮNG BẠI ]" : "[ HONORABLE DRAW ]";
        subHeaderStr = tiengViet
            ? "Cục diện cân bằng tuyệt đối, hai bên cùng ký biên bản hòa kỳ!"
            : "Equal strength displayed! The game concluded in a noble draw!";
    }

    // 3. Khung hộp thông báo chính
    FantasyUI::get().drawPanel(window, sf::FloatRect(dx, dy, dw, dh), FantasyPanelStyle::ORNATE_WOOD, borderColor);

    // Result badge: ornate fantasy ribbon banner
    const float badgeW = 440.0f;
    FantasyUI::get().drawRibbonTitle(window, dx + dw / 2.0f, dy + 16.0f, badgeW, 44.0f,
                                     badgeStr, fTitle, 19, titleColor, borderColor);

    // 7. Dòng phụ đề thông điệp kết quả
    sf::Text tSub;
    tSub.setFont(fBodyBold);
    tSub.setCharacterSize(16);
    tSub.setStyle(sf::Text::Bold);
    tSub.setString(toUtf8(subHeaderStr));
    tSub.setFillColor(sf::Color(255, 235, 150));
    sf::FloatRect sb = tSub.getLocalBounds();
    tSub.setOrigin(sb.left + sb.width / 2.0f, sb.top + sb.height / 2.0f);
    tSub.setPosition(dx + dw / 2.0f, dy + 76.0f);
    window.draw(tSub);

    // Match statistics
    const float statW = dw - 60.0f;
    const float statH = 205.0f;
    const float statX = dx + 30.0f;
    const float statY = dy + 98.0f;

    FantasyUI::get().drawPanel(window, sf::FloatRect(statX, statY, statW, statH), FantasyPanelStyle::WOOD_SLOT);

    // Tiêu đề bảng thống kê
    sf::Text sHeader;
    sHeader.setFont(fBodyBold);
    sHeader.setCharacterSize(15);
    sHeader.setStyle(sf::Text::Bold);
    sHeader.setString(toUtf8(tiengViet ? "--- TỔNG KẾT VÁN ĐẤU ---" : "--- MATCH REPORT ---"));
    sHeader.setFillColor(sf::Color(230, 195, 125));
    sf::FloatRect shb = sHeader.getLocalBounds();
    sHeader.setOrigin(shb.left + shb.width / 2.0f, shb.top + shb.height / 2.0f);
    sHeader.setPosition(statX + statW / 2.0f, statY + 15.0f);
    window.draw(sHeader);

    // Đường kẻ phân cách
    sf::RectangleShape line(sf::Vector2f(statW - 36.0f, 1.0f));
    line.setPosition(statX + 18.0f, statY + 26.0f);
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

    std::string winnerVal;
    sf::Color winnerValCol;
    if (winner == Mau::DO) {
        winnerVal = (tiengViet ? "BÊN ĐỎ (" : "RED SIDE (") + tenNguoiChoiDo + ")";
        winnerValCol = sf::Color(255, 120, 100);
    } else if (winner == Mau::DEN) {
        winnerVal = (tiengViet ? "BÊN ĐEN (" : "BLACK SIDE (") + tenNguoiChoiDen + ")";
        winnerValCol = sf::Color(120, 180, 255);
    } else {
        winnerVal = tiengViet ? "Hòa cờ" : "Draw";
        winnerValCol = sf::Color(255, 230, 140);
    }

    std::vector<StatRow> rows = {
        { tiengViet ? "Bên chiến thắng:" : "Winner Side:", winnerVal, winnerValCol },
        { tiengViet ? "Chế độ chơi:" : "Game Mode:", modeStr, sf::Color(255, 230, 140) },
        { tiengViet ? "Nguyên nhân kết thúc:" : "Outcome Reason:", reasonStr, sf::Color(255, 195, 100) },
        { tiengViet ? "Tổng số nước đi:" : "Total Moves:",
          std::to_string(movesCount) + (tiengViet ? " nước (" : " moves (") + std::to_string(roundsCount) + (tiengViet ? " hiệp)" : " rounds)"),
          sf::Color(255, 230, 140) },
        { tiengViet ? "Thời gian bên Đỏ còn:" : "Red Remaining Time:", formatTime(thoiGianConDo), sf::Color(255, 130, 120) },
        { tiengViet ? "Thời gian bên Đen còn:" : "Black Remaining Time:", formatTime(thoiGianConDen), sf::Color(135, 190, 255) }
    };

    float curRowY = statY + 33.0f;
    for (size_t r = 0; r < rows.size(); ++r) {
        if (r % 2 == 1) {
            sf::RectangleShape rowStrip(sf::Vector2f(statW - 16.0f, 24.0f));
            rowStrip.setPosition(statX + 8.0f, curRowY - 2.0f);
            rowStrip.setFillColor(sf::Color(255, 255, 255, 6));
            window.draw(rowStrip);
        }

        sf::Text tLbl;
        tLbl.setFont(fBodyBold);
        tLbl.setCharacterSize(14);
        tLbl.setStyle(sf::Text::Bold);
        tLbl.setString(toUtf8(rows[r].label));
        tLbl.setFillColor(sf::Color(240, 220, 160));
        tLbl.setPosition(statX + 20.0f, curRowY + 1.0f);
        window.draw(tLbl);

        sf::Text tVal;
        tVal.setFont(fBodyBold);
        tVal.setCharacterSize(14);
        tVal.setStyle(sf::Text::Bold);
        tVal.setString(toUtf8(rows[r].val));
        tVal.setFillColor(rows[r].valCol);
        sf::FloatRect vb = tVal.getLocalBounds();
        tVal.setPosition(statX + statW - 20.0f - vb.width, curRowY + 1.0f);
        window.draw(tVal);

        curRowY += 28.0f;
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
    tHint.setFillColor(sf::Color(235, 210, 155));
    sf::FloatRect hb = tHint.getLocalBounds();
    tHint.setOrigin(hb.left + hb.width / 2.0f, hb.top + hb.height / 2.0f);
    tHint.setPosition(dx + dw / 2.0f, dy + dh - 14.0f);
    window.draw(tHint);
}

void GameManager::veMenuPopUpInGame() {
    FantasyUI::get().drawFullDimOverlay(window, sf::Color(0, 0, 0, 160));

    // Modal W = 400.0f, H = 390.0f, centered at X = 400.0f, Y = 170.0f
    const float dw = 400.0f;
    const float dh = 390.0f;
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

    // 4. Y = 458: Hint text "Nhấn [ESC] để quay lại ngay"
    bool tiengViet = (Loc::getLanguage() == Language::TIENG_VIET);
    sf::Text tHint;
    tHint.setFont(fontBold);
    tHint.setCharacterSize(14);
    tHint.setStyle(sf::Text::Bold);
    tHint.setString(toUtf8(tiengViet ? "Nhấn [ESC] để quay lại ngay" : "Press [ESC] to quick resume"));
    tHint.setFillColor(sf::Color(240, 215, 155));
    sf::FloatRect hb = tHint.getLocalBounds();
    tHint.setOrigin(std::round(hb.left + hb.width / 2.0f), std::round(hb.top + hb.height / 2.0f));
    tHint.setPosition(1200.0f / 2.0f, 458.0f);
    window.draw(tHint);

    // 5. Y = 485: Match indicator slot tray in lower space
    float slotY = 485.0f;
    float slotH = 46.0f;
    float slotW = 320.0f;
    float slotX = 440.0f;

    sf::RectangleShape slotTray(sf::Vector2f(slotW, slotH));
    slotTray.setPosition(slotX, slotY);
    slotTray.setFillColor(sf::Color(24, 16, 12, 235));
    slotTray.setOutlineThickness(1.5f);
    slotTray.setOutlineColor(sf::Color(165, 125, 70, 200));
    window.draw(slotTray);
    ve4GocLuaChon(window, sf::Vector2f(slotX, slotY), sf::Vector2f(slotW, slotH), 1.0f, 1.2f);

    bool isRedTurn = (banCo.getLuotChoi() == Mau::DO);
    sf::Text tStatus;
    tStatus.setFont(fontBold);
    tStatus.setCharacterSize(16);
    tStatus.setStyle(sf::Text::Bold);
    std::string turnStr = isRedTurn ? (tiengViet ? "Lượt đi: Bên Đỏ" : "Turn: Red") : (tiengViet ? "Lượt đi: Bên Đen" : "Turn: Black");
    std::string s1 = turnStr + (tiengViet ? "  •  Nước đi: " : "  •  Move: ") + std::to_string(banCo.getLichSuNuocDi().size() + 1);
    tStatus.setString(toUtf8(s1));
    tStatus.setFillColor(isRedTurn ? sf::Color(255, 215, 150) : sf::Color(235, 225, 175));
    sf::FloatRect sb = tStatus.getLocalBounds();
    tStatus.setOrigin(std::round(sb.left + sb.width / 2.0f), std::round(sb.top + sb.height / 2.0f));
    tStatus.setPosition(1200.0f / 2.0f, std::round(slotY + slotH / 2.0f - 1.0f));

    sf::Text shadow = tStatus;
    shadow.setPosition(std::round(tStatus.getPosition().x + 1.2f), std::round(tStatus.getPosition().y + 1.2f));
    shadow.setFillColor(sf::Color(10, 6, 4, 230));
    window.draw(shadow);
    window.draw(tStatus);
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

// Implementation of khoiTaoPlaySetupButtons, vePieceStylePreview, vePlaySetup, xuLyClickPlaySetup

void GameManager::khoiTaoPlaySetupButtons() {
    playSetupButtons.clear();
    auto& mAssets = getMenuAssets();
    bool tiengViet = (Loc::getLanguage() == Language::TIENG_VIET);
    sf::Font& f = mAssets.vnFontLoaded ? mAssets.vnFont : font;
    sf::Font& fBold = mAssets.fontBoldLoaded ? mAssets.fontBold : fontBold;

    // 1. Top Horizontal Mode Tab Bar inside Dashboard (X=160, Y=120, W=880, H=560)
    // Symmetrical 3 tabs: W=256, H=40, Step=284 (Tab 0: 188, Tab 1: 472, Tab 2: 756 -> ends at 1012)
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

    const float tabStartX = 188.0f;
    const float tabY = 140.0f;
    const float tabW = 256.0f;
    const float tabH = 40.0f;
    const float tabStep = 284.0f;

    for (size_t i = 0; i < tabSpecs.size(); ++i) {
        int targetMode = tabSpecs[i].modeId;
        float tx = tabStartX + static_cast<float>(i) * tabStep;
        auto tabBtn = std::make_unique<Button>(tx, tabY, tabW, tabH, tabSpecs[i].label, fBold, [this, targetMode]() {
            soundManager.play(SoundType::CLICK);
            playSetupMode = targetMode;
            khoiTaoPlaySetupButtons();
        });
        bool active = (playSetupMode == targetMode);
        tabBtn->setSelected(active);
        tabBtn->setCharacterSize(15);
        tabBtn->setColors(
            active ? sf::Color(255, 226, 128) : sf::Color(242, 232, 206),
            sf::Color(255, 234, 150),
            sf::Color(255, 250, 224)
        );
        tabBtn->setIcon(tabSpecs[i].icon);
        playSetupButtons.push_back(std::move(tabBtn));
    }

    // Helpers for cycles
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

    // Standardized selector cycle helper: < (arrW) [ Val (valWidth) ] > (arrW)
    auto addCycle = [&](float x, float y, float valWidth, const std::string& valText,
                        std::function<void()> onPrev, std::function<void()> onNext) {
        const float arrW = 28.0f;
        const float h = 34.0f;
        auto bPrev = std::make_unique<Button>(x, y, arrW, h, "<", fBold, std::move(onPrev));
        bPrev->setCharacterSize(15);
        bPrev->setColors(sf::Color(242, 232, 206), sf::Color(255, 224, 128), sf::Color(255, 250, 220));
        playSetupButtons.push_back(std::move(bPrev));

        auto bVal = std::make_unique<Button>(x + arrW + 2.0f, y, valWidth, h, valText, fBold, onNext);
        bVal->setCharacterSize(14);
        bVal->setColors(sf::Color(255, 232, 165), sf::Color(255, 244, 195), sf::Color(255, 250, 224));
        playSetupButtons.push_back(std::move(bVal));

        auto bNext = std::make_unique<Button>(x + arrW + 2.0f + valWidth + 2.0f, y, arrW, h, ">", fBold, std::move(onNext));
        bNext->setCharacterSize(15);
        bNext->setColors(sf::Color(242, 232, 206), sf::Color(255, 224, 128), sf::Color(255, 250, 220));
        playSetupButtons.push_back(std::move(bNext));
    };

    // Avatar cycle: arrW=28, valW=120, arrW=28 -> total = 28 + 2 + 120 + 2 + 28 = 180.0f (exact match with input box width 180.0f!)
    auto addAvatarCycle = [&](float x, float y, int& charIdx) {
        addCycle(x, y, 120.0f, Loc::getCharName(charIdx),
            [this, &charIdx]() {
                soundManager.play(SoundType::CLICK);
                charIdx = (charIdx + Loc::getCharCount() - 1) % Loc::getCharCount();
                khoiTaoPlaySetupButtons();
            },
            [this, &charIdx]() {
                soundManager.play(SoundType::CLICK);
                charIdx = (charIdx + 1) % Loc::getCharCount();
                khoiTaoPlaySetupButtons();
            });
    };

    if (playSetupMode == 1) {
        // --- 1. Two-player PvP Setup ---
        // Player 1 Card: left=188, input at X=300, Y=244, W=180, H=34 (ends at 480, avatar centered at 534)
        inputP1Name->setPosition(300.0f, 244.0f);
        inputP1Name->setSize(180.0f, 34.0f);
        inputP1Name->setParchmentStyle(true);
        addAvatarCycle(300.0f, 296.0f, p1CharIndex);

        // Player 2 Card: left=612, input at X=724, Y=244, W=180, H=34 (ends at 904, avatar centered at 958)
        inputP2Name->setPosition(724.0f, 244.0f);
        inputP2Name->setSize(180.0f, 34.0f);
        inputP2Name->setParchmentStyle(true);
        addAvatarCycle(724.0f, 296.0f, p2CharIndex);

        // Settings 2-Column Grid:
        // Left Column (Timer & Board Theme):
        // Row 1 (Y = 422): Match Timer at (X=336, valW=140, total=200, ends at 536)
        addCycle(336.0f, 422.0f, 140.0f, getTimerLabel(),
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

        // Row 2 (Y = 468): Board Theme at (X=336, valW=140, total=200, ends at 536)
        addCycle(336.0f, 468.0f, 140.0f, getBoardThemeLabel(),
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

        // Right Column:
        // Row 1 (Y = 422): Piece Style at (X=744, valW=140, total=200, ends at 944)
        addCycle(744.0f, 422.0f, 140.0f, getPieceStyleLabel(),
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
        // Row 2 is Piece Sample preview drawn procedurally in vePieceStylePreview!

    } else if (playSetupMode == 0) {
        // --- 2. Player vs AI Setup ---
        // Human Player (Left Card): input at X=300, Y=244, W=180, H=34
        inputAiPlayerName->setPosition(300.0f, 244.0f);
        inputAiPlayerName->setSize(180.0f, 34.0f);
        inputAiPlayerName->setParchmentStyle(true);
        addAvatarCycle(300.0f, 296.0f, aiPlayerCharIndex);

        // AI Engine (Right Card): X=724, W=180, H=34 (valW=120)
        auto getDiffLabel = [this, tiengViet]() -> std::string {
            if (aiDifficulty == DoKho::DE) return tiengViet ? "Dễ" : "Easy";
            if (aiDifficulty == DoKho::TRUNG_BINH) return tiengViet ? "Trung Bình" : "Medium";
            return tiengViet ? "Khó" : "Hard";
        };
        addCycle(724.0f, 244.0f, 120.0f, getDiffLabel(),
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
            if (aiFirstChoice == 0) return tiengViet ? "Người (Đỏ)" : "Player (Red)";
            if (aiFirstChoice == 1) return tiengViet ? "Máy (Đỏ)" : "AI First (Red)";
            return tiengViet ? "Ngẫu Nhiên" : "Random Side";
        };
        addCycle(724.0f, 296.0f, 120.0f, getSideLabel(),
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

        // Settings 2-Column Grid (exact same as PvP):
        // Left Column:
        addCycle(336.0f, 422.0f, 140.0f, getTimerLabel(),
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

        addCycle(336.0f, 468.0f, 140.0f, getBoardThemeLabel(),
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

        // Right Column:
        addCycle(744.0f, 422.0f, 140.0f, getPieceStyleLabel(),
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

    } else {
        // --- 3. Demo Match Setup ---
        // Left Column: Piece Style at X=336, Y=452
        addCycle(336.0f, 452.0f, 140.0f, getPieceStyleLabel(),
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

        // Right Column: Board Theme at X=744, Y=452
        addCycle(744.0f, 452.0f, 140.0f, getBoardThemeLabel(),
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
    }

    // 3. Bottom Action Bar (TASK 05, 07)
    // Shared baseline Y = 614.0f, H = 46.0f
    const float btmY = 614.0f;
    const float btmH = 46.0f;

    auto backBtn = std::make_unique<Button>(188.0f, btmY, 140.0f, btmH,
        tiengViet ? "Quay Lại" : "Back", fBold, [this]() {
        soundManager.play(SoundType::CLICK);
        trangThai = TrangThai::MENU_CHINH;
        menuChinh.triggerOpenAnimation(MenuSection::PLAY);
    });
    backBtn->setCharacterSize(16);
    backBtn->setColors(sf::Color(242, 232, 206), sf::Color(255, 224, 128), sf::Color(255, 250, 220));
    playSetupButtons.push_back(std::move(backBtn));

    auto trimStr = [](std::string s) {
        while (!s.empty() && (s.front() == ' ' || s.front() == '	')) s.erase(s.begin());
        while (!s.empty() && (s.back() == ' ' || s.back() == '	')) s.pop_back();
        return s;
    };

    const float rightMargin = 1012.0f;
    const float actW = 268.0f;
    const float actX = rightMargin - actW; // 744.0f

    if (playSetupMode == 1) {
        auto rollBtn = std::make_unique<Button>(actX, btmY, actW, btmH,
            tiengViet ? "Tung đồng xu >>" : "Roll Turn Order >>", fBold, [this, trimStr, tiengViet]() {
            soundManager.play(SoundType::CLICK);
            std::string n1 = trimStr(inputP1Name->getValue());
            std::string n2 = trimStr(inputP2Name->getValue());
            if (n1.empty()) n1 = tiengViet ? "Người chơi 1" : "Player 1";
            if (n2.empty()) n2 = tiengViet ? "Người chơi 2" : "Player 2";
            if (n1 == n2) {
                n1 += " (1)";
                n2 += " (2)";
            }
            inputP1Name->setValue(n1);
            inputP2Name->setValue(n2);

            cheDoChoi = CheDoChoi::HAI_NGUOI;
            khoiTaoRandomLuotScene();
            trangThai = TrangThai::RANDOM_LUOT;
        });
        rollBtn->setCharacterSize(16);
        rollBtn->setSelected(true);
        rollBtn->setColors(sf::Color(255, 236, 175), sf::Color(255, 248, 210), sf::Color(255, 255, 235));
        playSetupButtons.push_back(std::move(rollBtn));

    } else if (playSetupMode == 0) {
        bool needsRoll = (aiFirstChoice == 2);
        std::string actLabel = needsRoll
            ? (tiengViet ? "Tung đồng xu >>" : "Roll Turn Order >>")
            : (tiengViet ? "Vào trận đấu >>" : "Start Match >>");

        auto actBtn = std::make_unique<Button>(actX, btmY, actW, btmH, actLabel, fBold, [this, trimStr, tiengViet, needsRoll]() {
            soundManager.play(SoundType::CLICK);
            std::string n = trimStr(inputAiPlayerName->getValue());
            if (n.empty()) n = tiengViet ? "Người chơi" : "Player";
            inputAiPlayerName->setValue(n);

            cheDoChoi = CheDoChoi::VOI_MAY;
            if (needsRoll) {
                khoiTaoRandomLuotScene();
                trangThai = TrangThai::RANDOM_LUOT;
            } else {
                batDauPvai();
            }
        });
        actBtn->setCharacterSize(16);
        actBtn->setSelected(true);
        actBtn->setColors(sf::Color(255, 236, 175), sf::Color(255, 248, 210), sf::Color(255, 255, 235));
        playSetupButtons.push_back(std::move(actBtn));

    } else {
        auto actBtn = std::make_unique<Button>(actX, btmY, actW, btmH,
            tiengViet ? "Bắt đầu mô phỏng >>" : "Start Demo >>", fBold, [this, tiengViet]() {
            soundManager.play(SoundType::CLICK);
            tenNguoiChoiDo = tiengViet ? "Quân Đỏ (Xe & Voi)" : "Red (Chariot & Elephant)";
            tenNguoiChoiDen = tiengViet ? "Quân Đen (Xe & Voi)" : "Black (Chariot & Elephant)";
            nhanVatDo = 0;
            nhanVatDen = 0;
            batDauTroChoiMoi(CheDoChoi::DEMO_2XE_2VOI);
        });
        actBtn->setCharacterSize(16);
        actBtn->setSelected(true);
        actBtn->setColors(sf::Color(255, 236, 175), sf::Color(255, 248, 210), sf::Color(255, 255, 235));
        playSetupButtons.push_back(std::move(actBtn));
    }
}

void GameManager::vePieceStylePreview(sf::RenderWindow& win, float x, float y, float w, float h) {
    auto& assets = getMenuAssets();
    sf::Font& fBold = assets.fontBoldLoaded ? assets.fontBold : fontBold;
    bool vi = (Loc::getLanguage() == Language::TIENG_VIET);

    // Subtle Aged Xuan inset box
    sf::RectangleShape box(sf::Vector2f(w, h));
    box.setPosition(x, y);
    box.setFillColor(sf::Color(220, 204, 172));
    box.setOutlineThickness(1.2f);
    box.setOutlineColor(sf::Color(65, 42, 22));
    win.draw(box);

    sf::Text lbl;
    lbl.setFont(fBold);
    lbl.setString(toUtf8(vi ? "Mẫu quân:" : "Piece Sample:"));
    lbl.setCharacterSize(14);
    lbl.setStyle(sf::Text::Bold);
    lbl.setFillColor(sf::Color(36, 20, 12));
    sf::FloatRect lb = lbl.getLocalBounds();
    lbl.setOrigin(0.0f, std::round(lb.top + lb.height / 2.0f));
    lbl.setPosition(x + 12.0f, std::round(y + h / 2.0f));
    win.draw(lbl);

    float r = std::min(13.0f, (h - 8.0f) / 2.0f);
    // Two sample pieces evenly spaced on the right
    sf::Vector2f posRed(std::round(x + w * 0.58f), std::round(y + h / 2.0f));
    sf::Vector2f posBlack(std::round(x + w * 0.83f), std::round(y + h / 2.0f));

    auto drawSamplePiece = [&](sf::Vector2f pos, Mau side) {
        bool isRed = (side == Mau::DO);
        const sf::Texture* tex = (pieceStyle == PieceStyle::REALISTIC_WOOD) ? layTextureQuan("Tuong", side) : nullptr;

        sf::CircleShape shadow(r);
        shadow.setOrigin(r, r);
        shadow.setPosition(pos.x + 1.2f, pos.y + 1.8f);
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
            sf::CircleShape outer(r + 1.5f);
            outer.setOrigin(r + 1.5f, r + 1.5f);
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
            sym.setFont(fBold);
            std::string s = layKyHieuQuanTheoStyle("Tuong", side);
            sym.setString(toUtf8(s));
            sym.setCharacterSize(s.length() > 2 ? 10 : 12);
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

void GameManager::veBoardThemePreview(sf::RenderWindow& win, float x, float y, float w, float h) {
    auto& assets = getMenuAssets();
    sf::Font& fBold = assets.fontBoldLoaded ? assets.fontBold : fontBold;
    sf::Font& fVN = assets.vnFontLoaded ? assets.vnFont : fBold;
    bool vi = (Loc::getLanguage() == Language::TIENG_VIET);

    // 1. Subtle Aged Xuan inset container
    sf::RectangleShape box(sf::Vector2f(w, h));
    box.setPosition(x, y);
    box.setFillColor(sf::Color(220, 204, 172));
    box.setOutlineThickness(1.2f);
    box.setOutlineColor(sf::Color(65, 42, 22));
    win.draw(box);

    // 2. Left Label: "Mẫu bàn cờ:" / "Board Sample:"
    sf::Text lbl;
    lbl.setFont(fBold);
    lbl.setString(toUtf8(vi ? "Mẫu bàn cờ:" : "Board Sample:"));
    lbl.setCharacterSize(14);
    lbl.setStyle(sf::Text::Bold);
    lbl.setFillColor(sf::Color(36, 20, 12));
    sf::FloatRect lb = lbl.getLocalBounds();
    lbl.setOrigin(0.0f, std::round(lb.top + lb.height / 2.0f));
    lbl.setPosition(x + 12.0f, std::round(y + h / 2.0f));
    win.draw(lbl);

    // 3. Mini Board Canvas to the right of the label
    float canvasX = x + 130.0f;
    float canvasY = y + 7.0f;
    float canvasW = w - 142.0f;
    float canvasH = h - 14.0f;

    sf::Color boardBgCol(220, 185, 135);
    sf::Color borderCol(70, 40, 20);
    sf::Color lineCol(70, 40, 20);
    sf::Color riverCol(70, 40, 20, 220);

    if (boardTheme == BoardTheme::IMPERIAL_JADE) {
        boardBgCol = sf::Color(168, 194, 162);
        borderCol = sf::Color(35, 58, 42);
        lineCol = sf::Color(35, 58, 42);
        riverCol = sf::Color(30, 52, 36, 225);
    } else if (boardTheme == BoardTheme::MIDNIGHT_INK) {
        boardBgCol = sf::Color(34, 40, 52);
        borderCol = sf::Color(150, 118, 62);
        lineCol = sf::Color(125, 150, 182);
        riverCol = sf::Color(208, 178, 116, 225);
    } else if (boardTheme == BoardTheme::WARM_BAMBOO) {
        boardBgCol = sf::Color(210, 175, 125);
        borderCol = sf::Color(68, 38, 18);
        lineCol = sf::Color(68, 38, 18);
        riverCol = sf::Color(68, 38, 18, 220);
    }

    // Mini board background
    sf::RectangleShape boardSurface(sf::Vector2f(canvasW, canvasH));
    boardSurface.setPosition(canvasX, canvasY);
    boardSurface.setFillColor(boardBgCol);
    boardSurface.setOutlineThickness(1.5f);
    boardSurface.setOutlineColor(borderCol);
    win.draw(boardSurface);

    // Inner rim
    sf::RectangleShape innerBorder(sf::Vector2f(canvasW - 6.0f, canvasH - 6.0f));
    innerBorder.setPosition(canvasX + 3.0f, canvasY + 3.0f);
    innerBorder.setFillColor(sf::Color::Transparent);
    innerBorder.setOutlineThickness(1.0f);
    innerBorder.setOutlineColor(sf::Color(lineCol.r, lineCol.g, lineCol.b, 140));
    win.draw(innerBorder);

    // Draw horizontal grid lines across the canvas
    float lineY1 = canvasY + 9.0f;
    float lineY2 = canvasY + canvasH / 2.0f;
    float lineY3 = canvasY + canvasH - 9.0f;

    auto drawHLine = [&](float ly) {
        sf::RectangleShape hl(sf::Vector2f(canvasW - 12.0f, 1.0f));
        hl.setPosition(canvasX + 6.0f, ly);
        hl.setFillColor(sf::Color(lineCol.r, lineCol.g, lineCol.b, 170));
        win.draw(hl);
    };
    drawHLine(lineY1);
    drawHLine(lineY2);
    drawHLine(lineY3);

    // Draw vertical column grid lines
    int numCols = 11;
    float colSpacing = (canvasW - 20.0f) / static_cast<float>(numCols - 1);
    for (int c = 0; c < numCols; ++c) {
        float cx = canvasX + 10.0f + static_cast<float>(c) * colSpacing;
        sf::RectangleShape vlTop(sf::Vector2f(1.0f, canvasH / 2.0f - 4.0f));
        vlTop.setPosition(cx, canvasY + 4.0f);
        vlTop.setFillColor(sf::Color(lineCol.r, lineCol.g, lineCol.b, 130));
        win.draw(vlTop);

        sf::RectangleShape vlBtm(sf::Vector2f(1.0f, canvasH / 2.0f - 4.0f));
        vlBtm.setPosition(cx, canvasY + canvasH / 2.0f);
        vlBtm.setFillColor(sf::Color(lineCol.r, lineCol.g, lineCol.b, 130));
        win.draw(vlBtm);
    }

    // Traditional Xiangqi River Calligraphy in the center
    sf::Text riverText;
    riverText.setFont(fVN);
    riverText.setCharacterSize(14);
    riverText.setStyle(sf::Text::Bold);
    riverText.setString(toUtf8("楚  河              漢  界"));
    riverText.setFillColor(riverCol);
    sf::FloatRect rtb = riverText.getLocalBounds();
    riverText.setOrigin(std::round(rtb.left + rtb.width / 2.0f), std::round(rtb.top + rtb.height / 2.0f));
    riverText.setPosition(std::round(canvasX + canvasW / 2.0f), std::round(canvasY + canvasH / 2.0f - 1.0f));
    win.draw(riverText);
}

void GameManager::vePlaySetup() {
    auto& assets = getMenuAssets();
    bool tiengViet = (Loc::getLanguage() == Language::TIENG_VIET);
    sf::Font& f = assets.vnFontLoaded ? assets.vnFont : font;
    sf::Font& fBold = assets.fontBoldLoaded ? assets.fontBold : fontBold;

    // 1. Subtle Fullscreen Dim Backdrop over Atmospheric Courtyard (1200x800)
    FantasyUI::get().drawFullDimOverlay(window, sf::Color(10, 8, 12, 75));

    // Top Antique Silk Ribbon Title (Hierarchy Level 1 - Title Case)
    FantasyUI::get().drawRibbonTitle(window, 600.0f, 38.0f, 380.0f, 56.0f,
        tiengViet ? "Thiết Lập Trận Đấu" : "Match Setup", fBold, 20);

    // 2. Unified Oriental Ebony Wood Dashboard Frame (880x560 at 160, 120)
    FantasyUI::get().drawPanel(window, sf::FloatRect(160.0f, 120.0f, 880.0f, 560.0f), OrientalPanelStyle::EBONY_WOOD);

    // Active Mode Tab Gold Corner Highlight (Tab 0=PvP, Tab 1=PvAI, Tab 2=Demo)
    int activeVisualTab = (playSetupMode == 1) ? 0 : ((playSetupMode == 0) ? 1 : 2);
    ve4GocLuaChon(window,
        sf::Vector2f(188.0f + static_cast<float>(activeVisualTab) * 284.0f, 140.0f),
        sf::Vector2f(256.0f, 40.0f),
        1.5f, 1.2f);

    auto drawSubCard = [this](sf::FloatRect rect, sf::Color fillCol, sf::Color borderCol) {
        sf::RectangleShape card(sf::Vector2f(rect.width, rect.height));
        card.setPosition(rect.left, rect.top);
        card.setFillColor(fillCol);
        card.setOutlineThickness(1.0f);
        card.setOutlineColor(borderCol);
        window.draw(card);
    };

    if (playSetupMode == 1) {
        // --- 1. TWO-PLAYER PVP: SYMMETRICAL SIDE-BY-SIDE PANELS ---
        // Left Card: Player 1 (Red Side) [188, 196, 400, 164]
        drawSubCard(sf::FloatRect(188.0f, 196.0f, 400.0f, 164.0f),
                    sf::Color(28, 18, 16, 240), sf::Color(140, 55, 45, 175));

        // P1 Header ribbon strip (Title Case)
        sf::RectangleShape h1(sf::Vector2f(384.0f, 28.0f));
        h1.setPosition(196.0f, 204.0f);
        h1.setFillColor(sf::Color(80, 24, 24, 220));
        h1.setOutlineThickness(1.0f);
        h1.setOutlineColor(sf::Color(160, 55, 50, 170));
        window.draw(h1);

        sf::Text t1;
        t1.setFont(fBold);
        t1.setCharacterSize(15);
        t1.setStyle(sf::Text::Bold);
        t1.setString(toUtf8(tiengViet ? "[ Kỳ Thủ 1 - Quân Đỏ ]" : "[ Player 1 - Red ]"));
        t1.setFillColor(sf::Color(255, 230, 224));
        sf::FloatRect tb = t1.getLocalBounds();
        t1.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));
        t1.setPosition(388.0f, 218.0f);
        window.draw(t1);

        // Row 1: Name Label & Input (Vertically centered at Y = 261)
        sf::Text lName1;
        lName1.setFont(fBold);
        lName1.setString(toUtf8(tiengViet ? "Tên:" : "Name:"));
        lName1.setCharacterSize(15);
        lName1.setStyle(sf::Text::Bold);
        lName1.setFillColor(sf::Color(255, 235, 165));
        sf::FloatRect nb1 = lName1.getLocalBounds();
        lName1.setOrigin(0.0f, std::round(nb1.top + nb1.height / 2.0f));
        lName1.setPosition(204.0f, 261.0f);
        window.draw(lName1);

        inputP1Name->draw(window);

        // Row 2: Avatar Label & Cycle (Vertically centered at Y = 313)
        sf::Text lAva1;
        lAva1.setFont(fBold);
        lAva1.setString(toUtf8(tiengViet ? "Quân diện:" : "Avatar:"));
        lAva1.setCharacterSize(15);
        lAva1.setStyle(sf::Text::Bold);
        lAva1.setFillColor(sf::Color(255, 235, 165));
        sf::FloatRect ab1 = lAva1.getLocalBounds();
        lAva1.setOrigin(0.0f, std::round(ab1.top + ab1.height / 2.0f));
        lAva1.setPosition(204.0f, 313.0f);
        window.draw(lAva1);

        // Portrait preview for P1 (Centered at X = 534, Y = 287)
        veAvatarPortrait(sf::Vector2f(534.0f, 287.0f), 23.0f, p1CharIndex, Mau::DO, false);

        // Right Card: Player 2 (Black Side) [612, 196, 400, 164]
        drawSubCard(sf::FloatRect(612.0f, 196.0f, 400.0f, 164.0f),
                    sf::Color(18, 22, 30, 240), sf::Color(55, 85, 135, 175));

        // P2 Header ribbon strip (Title Case)
        sf::RectangleShape h2(sf::Vector2f(384.0f, 28.0f));
        h2.setPosition(620.0f, 204.0f);
        h2.setFillColor(sf::Color(24, 38, 58, 220));
        h2.setOutlineThickness(1.0f);
        h2.setOutlineColor(sf::Color(55, 90, 145, 170));
        window.draw(h2);

        sf::Text t2;
        t2.setFont(fBold);
        t2.setCharacterSize(15);
        t2.setStyle(sf::Text::Bold);
        t2.setString(toUtf8(tiengViet ? "[ Kỳ Thủ 2 - Quân Đen ]" : "[ Player 2 - Black ]"));
        t2.setFillColor(sf::Color(215, 235, 255));
        tb = t2.getLocalBounds();
        t2.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));
        t2.setPosition(812.0f, 218.0f);
        window.draw(t2);

        // Row 1: Name Label & Input (Vertically centered at Y = 261)
        sf::Text lName2;
        lName2.setFont(fBold);
        lName2.setString(toUtf8(tiengViet ? "Tên:" : "Name:"));
        lName2.setCharacterSize(15);
        lName2.setStyle(sf::Text::Bold);
        lName2.setFillColor(sf::Color(255, 235, 165));
        sf::FloatRect nb2 = lName2.getLocalBounds();
        lName2.setOrigin(0.0f, std::round(nb2.top + nb2.height / 2.0f));
        lName2.setPosition(628.0f, 261.0f);
        window.draw(lName2);

        inputP2Name->draw(window);

        // Row 2: Avatar Label & Cycle (Vertically centered at Y = 313)
        sf::Text lAva2;
        lAva2.setFont(fBold);
        lAva2.setString(toUtf8(tiengViet ? "Quân diện:" : "Avatar:"));
        lAva2.setCharacterSize(15);
        lAva2.setStyle(sf::Text::Bold);
        lAva2.setFillColor(sf::Color(255, 235, 165));
        sf::FloatRect ab2 = lAva2.getLocalBounds();
        lAva2.setOrigin(0.0f, std::round(ab2.top + ab2.height / 2.0f));
        lAva2.setPosition(628.0f, 313.0f);
        window.draw(lAva2);

        // Portrait preview for P2 (Centered at X = 958, Y = 287)
        veAvatarPortrait(sf::Vector2f(958.0f, 287.0f), 23.0f, p2CharIndex, Mau::DEN, false);

        // --- Shared Match & Theme Configuration ---
        drawSubCard(sf::FloatRect(188.0f, 376.0f, 824.0f, 218.0f),
                    sf::Color(25, 19, 15, 242), sf::Color(120, 88, 48, 175));

        sf::Text rTitle;
        rTitle.setFont(fBold);
        rTitle.setCharacterSize(16);
        rTitle.setStyle(sf::Text::Bold);
        rTitle.setString(toUtf8(tiengViet ? "Cấu Hình Bàn Cờ & Thiết Lập Trận Đấu" : "Board Themes & Match Settings"));
        rTitle.setFillColor(sf::Color(255, 225, 120));
        rTitle.setPosition(208.0f, 388.0f);
        window.draw(rTitle);

        // Left Column: Match Timer (Row 1) & Board Theme (Row 2) (Vertically centered)
        sf::Text lTimer;
        lTimer.setFont(fBold);
        lTimer.setCharacterSize(15);
        lTimer.setStyle(sf::Text::Bold);
        lTimer.setString(toUtf8(tiengViet ? "Thời gian ván:" : "Match Timer:"));
        lTimer.setFillColor(sf::Color(245, 230, 185));
        sf::FloatRect tbTimer = lTimer.getLocalBounds();
        lTimer.setOrigin(0.0f, std::round(tbTimer.top + tbTimer.height / 2.0f));
        lTimer.setPosition(208.0f, 439.0f);
        window.draw(lTimer);

        sf::Text lBoard;
        lBoard.setFont(fBold);
        lBoard.setCharacterSize(15);
        lBoard.setStyle(sf::Text::Bold);
        lBoard.setString(toUtf8(tiengViet ? "Màu bàn cờ:" : "Board Theme:"));
        lBoard.setFillColor(sf::Color(245, 230, 185));
        sf::FloatRect tbBoard = lBoard.getLocalBounds();
        lBoard.setOrigin(0.0f, std::round(tbBoard.top + tbBoard.height / 2.0f));
        lBoard.setPosition(208.0f, 485.0f);
        window.draw(lBoard);

        // Right Column: Piece Style (Row 1) & Piece Sample (Row 2)
        sf::Text lStyle;
        lStyle.setFont(fBold);
        lStyle.setCharacterSize(15);
        lStyle.setStyle(sf::Text::Bold);
        lStyle.setString(toUtf8(tiengViet ? "Kiểu quân cờ:" : "Piece Style:"));
        lStyle.setFillColor(sf::Color(245, 230, 185));
        sf::FloatRect tbStyle = lStyle.getLocalBounds();
        lStyle.setOrigin(0.0f, std::round(tbStyle.top + tbStyle.height / 2.0f));
        lStyle.setPosition(612.0f, 439.0f);
        window.draw(lStyle);

        // Piece Style Live Preview box
        vePieceStylePreview(window, 612.0f, 468.0f, 332.0f, 34.0f);

        // Board Theme Sample Canvas (Replaces the old instruction text box)
        veBoardThemePreview(window, 204.0f, 516.0f, 792.0f, 58.0f);

    } else if (playSetupMode == 0) {
        // --- 2. PLAYER VS AI SETUP ---
        // Left Card: Human Player [188, 196, 400, 164]
        drawSubCard(sf::FloatRect(188.0f, 196.0f, 400.0f, 164.0f),
                    sf::Color(28, 20, 16, 240), sf::Color(140, 95, 45, 175));

        sf::RectangleShape h1(sf::Vector2f(384.0f, 28.0f));
        h1.setPosition(196.0f, 204.0f);
        h1.setFillColor(sf::Color(75, 45, 22, 220));
        h1.setOutlineThickness(1.0f);
        h1.setOutlineColor(sf::Color(185, 125, 50, 170));
        window.draw(h1);

        sf::Text t1;
        t1.setFont(fBold);
        t1.setCharacterSize(15);
        t1.setStyle(sf::Text::Bold);
        t1.setString(toUtf8(tiengViet ? "[ Kỳ Thủ (Người Chơi) ]" : "[ Human Player ]"));
        t1.setFillColor(sf::Color(255, 235, 185));
        sf::FloatRect tb = t1.getLocalBounds();
        t1.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));
        t1.setPosition(388.0f, 218.0f);
        window.draw(t1);

        sf::Text lName1;
        lName1.setFont(fBold);
        lName1.setString(toUtf8(tiengViet ? "Tên:" : "Name:"));
        lName1.setCharacterSize(15);
        lName1.setStyle(sf::Text::Bold);
        lName1.setFillColor(sf::Color(255, 235, 165));
        sf::FloatRect nb1 = lName1.getLocalBounds();
        lName1.setOrigin(0.0f, std::round(nb1.top + nb1.height / 2.0f));
        lName1.setPosition(204.0f, 261.0f);
        window.draw(lName1);

        inputAiPlayerName->draw(window);

        sf::Text lAva1;
        lAva1.setFont(fBold);
        lAva1.setString(toUtf8(tiengViet ? "Quân diện:" : "Avatar:"));
        lAva1.setCharacterSize(15);
        lAva1.setStyle(sf::Text::Bold);
        lAva1.setFillColor(sf::Color(255, 235, 165));
        sf::FloatRect ab1 = lAva1.getLocalBounds();
        lAva1.setOrigin(0.0f, std::round(ab1.top + ab1.height / 2.0f));
        lAva1.setPosition(204.0f, 313.0f);
        window.draw(lAva1);

        veAvatarPortrait(sf::Vector2f(534.0f, 287.0f), 23.0f, aiPlayerCharIndex, Mau::DO, false);

        // Right Card: AI Opponent [612, 196, 400, 164]
        drawSubCard(sf::FloatRect(612.0f, 196.0f, 400.0f, 164.0f),
                    sf::Color(18, 24, 32, 240), sf::Color(55, 85, 135, 175));

        sf::RectangleShape h2(sf::Vector2f(384.0f, 28.0f));
        h2.setPosition(620.0f, 204.0f);
        h2.setFillColor(sf::Color(22, 38, 58, 220));
        h2.setOutlineThickness(1.0f);
        h2.setOutlineColor(sf::Color(55, 90, 145, 170));
        window.draw(h2);

        sf::Text t2;
        t2.setFont(fBold);
        t2.setCharacterSize(15);
        t2.setStyle(sf::Text::Bold);
        t2.setString(toUtf8(tiengViet ? "[ Đối Thủ Máy (AI Engine) ]" : "[ AI Opponent ]"));
        t2.setFillColor(sf::Color(215, 235, 255));
        tb = t2.getLocalBounds();
        t2.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));
        t2.setPosition(812.0f, 218.0f);
        window.draw(t2);

        // Row 1: AI Difficulty (Vertically centered at Y = 261)
        sf::Text lDiff;
        lDiff.setFont(fBold);
        lDiff.setString(toUtf8(tiengViet ? "Độ khó AI:" : "Difficulty:"));
        lDiff.setCharacterSize(15);
        lDiff.setStyle(sf::Text::Bold);
        lDiff.setFillColor(sf::Color(255, 235, 165));
        sf::FloatRect db = lDiff.getLocalBounds();
        lDiff.setOrigin(0.0f, std::round(db.top + db.height / 2.0f));
        lDiff.setPosition(628.0f, 261.0f);
        window.draw(lDiff);

        // Row 2: AI First Move (Vertically centered at Y = 313)
        sf::Text lSide;
        lSide.setFont(fBold);
        lSide.setString(toUtf8(tiengViet ? "Bên đi đầu:" : "First Move:"));
        lSide.setCharacterSize(15);
        lSide.setStyle(sf::Text::Bold);
        lSide.setFillColor(sf::Color(255, 235, 165));
        sf::FloatRect sb = lSide.getLocalBounds();
        lSide.setOrigin(0.0f, std::round(sb.top + sb.height / 2.0f));
        lSide.setPosition(628.0f, 313.0f);
        window.draw(lSide);

        veAvatarPortrait(sf::Vector2f(958.0f, 287.0f), 23.0f, 0, Mau::DEN, false);

        // Settings (Grid 2-column)
        drawSubCard(sf::FloatRect(188.0f, 376.0f, 824.0f, 218.0f),
                    sf::Color(25, 19, 15, 242), sf::Color(120, 88, 48, 175));

        sf::Text rTitle;
        rTitle.setFont(fBold);
        rTitle.setCharacterSize(16);
        rTitle.setStyle(sf::Text::Bold);
        rTitle.setString(toUtf8(tiengViet ? "Cấu Hình Bàn Cờ & Thiết Lập Trận Đấu" : "Board Themes & Match Settings"));
        rTitle.setFillColor(sf::Color(255, 225, 120));
        rTitle.setPosition(208.0f, 388.0f);
        window.draw(rTitle);

        sf::Text lTimer;
        lTimer.setFont(fBold);
        lTimer.setCharacterSize(15);
        lTimer.setStyle(sf::Text::Bold);
        lTimer.setString(toUtf8(tiengViet ? "Thời gian ván:" : "Match Timer:"));
        lTimer.setFillColor(sf::Color(245, 230, 185));
        sf::FloatRect tbTimer = lTimer.getLocalBounds();
        lTimer.setOrigin(0.0f, std::round(tbTimer.top + tbTimer.height / 2.0f));
        lTimer.setPosition(208.0f, 439.0f);
        window.draw(lTimer);

        sf::Text lBoard;
        lBoard.setFont(fBold);
        lBoard.setCharacterSize(15);
        lBoard.setStyle(sf::Text::Bold);
        lBoard.setString(toUtf8(tiengViet ? "Màu bàn cờ:" : "Board Theme:"));
        lBoard.setFillColor(sf::Color(245, 230, 185));
        sf::FloatRect tbBoard = lBoard.getLocalBounds();
        lBoard.setOrigin(0.0f, std::round(tbBoard.top + tbBoard.height / 2.0f));
        lBoard.setPosition(208.0f, 485.0f);
        window.draw(lBoard);

        sf::Text lStyle;
        lStyle.setFont(fBold);
        lStyle.setCharacterSize(15);
        lStyle.setStyle(sf::Text::Bold);
        lStyle.setString(toUtf8(tiengViet ? "Kiểu quân cờ:" : "Piece Style:"));
        lStyle.setFillColor(sf::Color(245, 230, 185));
        sf::FloatRect tbStyle = lStyle.getLocalBounds();
        lStyle.setOrigin(0.0f, std::round(tbStyle.top + tbStyle.height / 2.0f));
        lStyle.setPosition(612.0f, 439.0f);
        window.draw(lStyle);

        vePieceStylePreview(window, 612.0f, 468.0f, 332.0f, 34.0f);

        // Board Theme Sample Canvas (Replaces the old instruction text box)
        veBoardThemePreview(window, 204.0f, 516.0f, 792.0f, 58.0f);

    } else {
        // --- 3. DEMO SIMULATION SETUP ---
        drawSubCard(sf::FloatRect(188.0f, 196.0f, 824.0f, 198.0f),
                    sf::Color(25, 19, 15, 242), sf::Color(120, 88, 48, 175));

        sf::Text dTitle;
        dTitle.setFont(fBold);
        dTitle.setCharacterSize(16);
        dTitle.setStyle(sf::Text::Bold);
        dTitle.setString(toUtf8(tiengViet ? "Mô Phỏng Ván Cờ Tàn Tự Động (Minimax vs Alpha-Beta)" : "Automated Endgame Simulation (Minimax vs Alpha-Beta)"));
        dTitle.setFillColor(sf::Color(255, 225, 120));
        dTitle.setPosition(208.0f, 210.0f);
        window.draw(dTitle);

        sf::Text dDesc;
        dDesc.setFont(fBold);
        dDesc.setCharacterSize(14);
        dDesc.setStyle(sf::Text::Bold);
        dDesc.setString(toUtf8(tiengViet
            ? "Mô phỏng trận thế cờ tàn kinh điển Song Xe Song Voi tự động giữa 2 thuật toán AI:\n"
              "  * Quân Đỏ: Minimax AI kết hợp bảng đánh giá vị trí chiến lược.\n"
              "  * Quân Đen: Alpha-Beta Pruning 4 lớp với thuật toán cắt tỉa nhánh tối ưu.\n"
              "Người chơi có thể theo dõi cách máy AI phối hợp tấn công và phòng thủ."
            : "Automatic demonstration of a classic Double-Chariot Double-Elephant endgame:\n"
              "  * Red Side: Minimax search with positional piece-square evaluation.\n"
              "  * Black Side: 4-ply Alpha-Beta Pruning with aggressive tactical cutoffs.\n"
              "Observe and study tactical masteries and defenses between competing algorithms."));
        dDesc.setFillColor(sf::Color(245, 230, 185));
        dDesc.setLineSpacing(1.3f);
        dDesc.setPosition(208.0f, 248.0f);
        window.draw(dDesc);

        // Themes
        drawSubCard(sf::FloatRect(188.0f, 408.0f, 824.0f, 186.0f),
                    sf::Color(25, 19, 15, 242), sf::Color(120, 88, 48, 175));

        sf::Text dThemeTitle;
        dThemeTitle.setFont(fBold);
        dThemeTitle.setCharacterSize(16);
        dThemeTitle.setStyle(sf::Text::Bold);
        dThemeTitle.setString(toUtf8(tiengViet ? "Cấu Hình Bàn Cờ & Kiểu Quân" : "Board Themes & Piece Styles"));
        dThemeTitle.setFillColor(sf::Color(255, 225, 120));
        dThemeTitle.setPosition(208.0f, 418.0f);
        window.draw(dThemeTitle);

        sf::Text lStyle;
        lStyle.setFont(fBold);
        lStyle.setCharacterSize(15);
        lStyle.setStyle(sf::Text::Bold);
        lStyle.setString(toUtf8(tiengViet ? "Kiểu quân cờ:" : "Piece Style:"));
        lStyle.setFillColor(sf::Color(245, 230, 185));
        lStyle.setPosition(208.0f, 460.0f);
        window.draw(lStyle);

        sf::Text lBoard;
        lBoard.setFont(fBold);
        lBoard.setCharacterSize(15);
        lBoard.setStyle(sf::Text::Bold);
        lBoard.setString(toUtf8(tiengViet ? "Màu bàn cờ:" : "Board Theme:"));
        lBoard.setFillColor(sf::Color(245, 230, 185));
        lBoard.setPosition(612.0f, 460.0f);
        window.draw(lBoard);

        vePieceStylePreview(window, 208.0f, 506.0f, 384.0f, 44.0f);
        veBoardThemePreview(window, 612.0f, 506.0f, 384.0f, 44.0f);
    }

    // 4. Interactive Action Buttons
    for (auto& btn : playSetupButtons) {
        btn->draw(window);
    }

    // Minimalist Footer Hint Strip over dark backdrop (Y=722)
    sf::Text footer;
    footer.setFont(fBold);
    footer.setString(toUtf8(tiengViet
        ? "[W/S] Đổi chế độ  |  [A/D] Thời gian  |  [Enter] Bắt đầu  |  [ESC] Quay lại"
        : "[W/S] Switch Mode  |  [A/D] Timer  |  [Enter] Start  |  [ESC] Back"));
    footer.setCharacterSize(14);
    footer.setStyle(sf::Text::Bold);
    footer.setFillColor(sf::Color(240, 215, 140));
    sf::FloatRect fb = footer.getLocalBounds();
    footer.setOrigin(std::round(fb.left + fb.width / 2.0f), std::round(fb.top + fb.height / 2.0f));
    footer.setPosition(600.0f, 722.0f);
    window.draw(footer);
}

void GameManager::xuLyClickPlaySetup(const sf::Vector2i& viTri) {
    if (playSetupMode == 0) {
        inputAiPlayerName->handleClick(viTri);
    } else if (playSetupMode == 1) {
        inputP1Name->handleClick(viTri);
        inputP2Name->handleClick(viTri);
    }

    for (size_t i = 0; i < playSetupButtons.size(); ++i) {
        if (playSetupButtons[i]->contains(viTri)) {
            playSetupButtons[i]->handleClick(viTri);
            break;
        }
    }
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
            det1.setFillColor(isSel ? sf::Color(35, 18, 10) : sf::Color(255, 230, 140));
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
            isActive ? sf::Color(255, 248, 175) : sf::Color(255, 230, 140),
            sf::Color(255, 248, 175),
            sf::Color(255, 250, 190)
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
        btn->setColors(sf::Color(255, 230, 140), sf::Color(255, 248, 175), sf::Color(255, 250, 190));
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

std::string GameManager::xacDinhLyDoNuocDiSai(std::shared_ptr<QuanCo> quan, int hMoi, int cMoi) {
    bool vi = (Loc::getLanguage() == Language::TIENG_VIET);
    if (!quan) return vi ? "Chưa chọn quân cờ nào!" : "No piece selected!";

    if (!banCo.trongBanCo(hMoi, cMoi)) {
        return vi ? "Ô nằm ngoài phạm vi bàn cờ!" : "Position outside board!";
    }

    int h = quan->getHang();
    int c = quan->getCot();
    if (hMoi == h && cMoi == c) {
        return vi ? "Quân cờ đang ở chính ô này!" : "Piece is already here!";
    }

    auto quanDich = banCo.timQuan(hMoi, cMoi);
    if (quanDich && quanDich->getMau() == quan->getMau()) {
        return vi ? "Không thể ăn quân của chính phe mình!" : "Cannot capture your own piece!";
    }

    int dh = std::abs(hMoi - h);
    int dc = std::abs(cMoi - c);
    std::string ten = quan->layTen();

    // 1. Kiểm tra quy tắc di chuyển cơ bản của từng quân
    if (ten == "Ma") {
        if (!((dh == 2 && dc == 1) || (dh == 1 && dc == 2))) {
            return vi ? "Mã phải đi chữ Nhật (2 ô dọc 1 ô ngang hoặc 2 ô ngang 1 ô dọc)!" 
                      : "Knight must move in an L-shape (2 by 1)!";
        }
        int hangChan = (dh == 2) ? (h + (hMoi - h) / 2) : h;
        int cotChan = (dh == 2) ? c : (c + (cMoi - c) / 2);
        if (banCo.coQuanTai(hangChan, cotChan)) {
            return vi ? "Mã bị cản chân! Có quân chặn tại điểm cản của Mã." 
                      : "Knight's path is blocked by an adjacent piece!";
        }
    }
    else if (ten == "Voi") {
        if (quan->getMau() == Mau::DO && hMoi < 5) {
            return vi ? "Tượng không được phép qua sông sang phần sân đối phương!" 
                      : "Elephant cannot cross the river!";
        }
        if (quan->getMau() == Mau::DEN && hMoi > 4) {
            return vi ? "Tượng không được phép qua sông sang phần sân đối phương!" 
                      : "Elephant cannot cross the river!";
        }
        if (!(dh == 2 && dc == 2)) {
            return vi ? "Tượng phải đi đường chéo đúng 2 ô (chữ Điền)!" 
                      : "Elephant must move diagonally 2 squares!";
        }
        int hangChan = h + (hMoi - h) / 2;
        int cotChan = c + (cMoi - c) / 2;
        if (banCo.coQuanTai(hangChan, cotChan)) {
            return vi ? "Tượng bị cản mắt! Có quân chặn giữa đường chéo." 
                      : "Elephant's eye is blocked by a piece in the center!";
        }
    }
    else if (ten == "Si") {
        if (cMoi < 3 || cMoi > 5) {
            return vi ? "Sĩ không được đi ra ngoài Cung Tướng!" 
                      : "Advisor cannot leave the palace!";
        }
        if (quan->getMau() == Mau::DO && (hMoi < 7 || hMoi > 9)) {
            return vi ? "Sĩ Đỏ chỉ được di chuyển trong Cung Tướng (hàng 7-9)!" 
                      : "Red Advisor cannot leave the palace!";
        }
        if (quan->getMau() == Mau::DEN && (hMoi < 0 || hMoi > 2)) {
            return vi ? "Sĩ Đen chỉ được di chuyển trong Cung Tướng (hàng 0-2)!" 
                      : "Black Advisor cannot leave the palace!";
        }
        if (!(dh == 1 && dc == 1)) {
            return vi ? "Sĩ chỉ được đi chéo đúng 1 ô trong Cung Tướng!" 
                      : "Advisor only moves 1 diagonal step in palace!";
        }
    }
    else if (ten == "Tuong") {
        if (cMoi < 3 || cMoi > 5) {
            return vi ? "Tướng không được đi ra ngoài Cung Tướng!" 
                      : "General cannot leave the palace!";
        }
        if (quan->getMau() == Mau::DO && (hMoi < 7 || hMoi > 9)) {
            return vi ? "Tướng Đỏ không được rời khỏi Cung Tướng!" 
                      : "Red General cannot leave the palace!";
        }
        if (quan->getMau() == Mau::DEN && (hMoi < 0 || hMoi > 2)) {
            return vi ? "Tướng Đen không được rời khỏi Cung Tướng!" 
                      : "Black General cannot leave the palace!";
        }
        if (!((dh == 1 && dc == 0) || (dh == 0 && dc == 1))) {
            return vi ? "Tướng chỉ được đi ngang hoặc dọc đúng 1 ô trong Cung!" 
                      : "General moves 1 step horizontally or vertically!";
        }
    }
    else if (ten == "Xe") {
        if (dh != 0 && dc != 0) {
            return vi ? "Xe chỉ được di chuyển theo đường thẳng ngang hoặc dọc!" 
                      : "Chariot must move in straight lines!";
        }
        if (banCo.demQuanTrenDuong(h, c, hMoi, cMoi) > 0) {
            return vi ? "Xe bị chặn đường! Đường đi của Xe phải hoàn toàn thông suốt." 
                      : "Chariot path is blocked by other pieces!";
        }
    }
    else if (ten == "Phao") {
        if (dh != 0 && dc != 0) {
            return vi ? "Pháo chỉ được di chuyển theo đường thẳng ngang hoặc dọc!" 
                      : "Cannon must move in straight lines!";
        }
        int dem = banCo.demQuanTrenDuong(h, c, hMoi, cMoi);
        if (quanDich == nullptr) {
            if (dem > 0) return vi ? "Pháo đi không ăn quân thì đường đi phải thông suốt!" 
                                   : "Cannon cannot jump without capturing!";
        } else {
            if (dem != 1) return vi ? "Pháo muốn ăn quân đối phương phải có đúng 1 quân làm ngòi cản!" 
                                    : "Cannon needs exactly 1 screen piece to capture!";
        }
    }
    else if (ten == "Tot") {
        if (quan->getMau() == Mau::DO) {
            if (hMoi > h) return vi ? "Tốt không bao giờ được đi lùi!" : "Soldier cannot move backwards!";
            if (h >= 5 && dc != 0) return vi ? "Tốt chưa qua sông chỉ được tiến thẳng, không đi ngang!" 
                                             : "Soldier cannot move sideways before crossing river!";
            if (dh + dc != 1) return vi ? "Tốt chỉ được đi đúng 1 ô mỗi nước!" : "Soldier moves only 1 step!";
        } else {
            if (hMoi < h) return vi ? "Tốt không bao giờ được đi lùi!" : "Soldier cannot move backwards!";
            if (h <= 4 && dc != 0) return vi ? "Tốt chưa qua sông chỉ được tiến thẳng, không đi ngang!" 
                                             : "Soldier cannot move sideways before crossing river!";
            if (dh + dc != 1) return vi ? "Tốt chỉ được đi đúng 1 ô mỗi nước!" : "Soldier moves only 1 step!";
        }
    }

    // 2. Kiểm tra nước đi không bảo vệ được Tướng hoặc lộ mặt Tướng
    if (banCo.kiemTraChieuTuong() || banCo.kiemTraChieu(quan->getMau())) {
        return vi ? "TƯỚNG ĐANG BỊ CHIẾU! Phải đi nước hóa giải thế chiếu." 
                  : "KING UNDER CHECK! You must protect the King.";
    }
    return vi ? "NƯỚC ĐI PHẠM QUY: Làm lộ mặt Tướng hoặc khiến Tướng bị chiếu!" 
              : "ILLEGAL MOVE: Exposes King to check or flying generals!";
}

void GameManager::veAvatarPortrait(sf::Vector2f center, float radius, int charIndex, Mau mau, bool activeGlow) {
    if (activeGlow) {
        float pulse = 0.5f + 0.5f * std::sin(bannerPulseClock.getElapsedTime().asSeconds() * 6.0f);
        float glowR = radius + 6.0f + pulse * 4.0f;
        sf::CircleShape glow(glowR);
        glow.setOrigin(glowR, glowR);
        glow.setPosition(center);
        sf::Color glowColor = (mau == Mau::DO) ? sf::Color(255, 90, 70, 110) : sf::Color(70, 160, 255, 110);
        glow.setFillColor(glowColor);
        window.draw(glow);
    }

    sf::CircleShape disc(radius);
    disc.setOrigin(radius, radius);
    disc.setPosition(center);

    sf::Color innerBg = (mau == Mau::DO) ? sf::Color(52, 24, 20) : sf::Color(22, 28, 38);
    sf::Color borderCol = (mau == Mau::DO) ? sf::Color(215, 65, 55) : sf::Color(70, 130, 205);
    disc.setFillColor(innerBg);
    disc.setOutlineThickness(2.5f);
    disc.setOutlineColor(borderCol);
    window.draw(disc);

    sf::CircleShape innerRing(radius - 3.5f);
    innerRing.setOrigin(radius - 3.5f, radius - 3.5f);
    innerRing.setPosition(center);
    innerRing.setFillColor(sf::Color::Transparent);
    innerRing.setOutlineThickness(1.0f);
    innerRing.setOutlineColor(sf::Color(borderCol.r, borderCol.g, borderCol.b, 140));
    window.draw(innerRing);

    const sf::Texture& pieceTex = AssetManager::get().getAvatarTexture(charIndex, mau);
    if (pieceTex.getSize().x > 0) {
        sf::Sprite sp(pieceTex);
        float scale = (radius * 1.6f) / static_cast<float>(pieceTex.getSize().y);
        sp.setScale(scale, scale);
        float sw = static_cast<float>(pieceTex.getSize().x) * scale;
        float sh = static_cast<float>(pieceTex.getSize().y) * scale;
        sp.setPosition(center.x - sw / 2.0f, center.y - sh / 2.0f);
        window.draw(sp);
    } else {
        std::string sigil;
        switch (charIndex % 5) {
            case 0: sigil = (Loc::getLanguage() == Language::TIENG_VIET) ? "TƯỚNG" : "GEN"; break;
            case 1: sigil = (Loc::getLanguage() == Language::TIENG_VIET) ? "SƯ" : "STR"; break;
            case 2: sigil = (Loc::getLanguage() == Language::TIENG_VIET) ? "VƯƠNG" : "GM"; break;
            case 3: sigil = (Loc::getLanguage() == Language::TIENG_VIET) ? "HIỆP" : "KNT"; break;
            case 4: default: sigil = (Loc::getLanguage() == Language::TIENG_VIET) ? "SINH" : "SCH"; break;
        }
        sf::Text txt;
        txt.setFont(fontBold);
        txt.setCharacterSize(static_cast<unsigned int>(radius * 0.42f));
        txt.setStyle(sf::Text::Bold);
        txt.setString(toUtf8(sigil));
        txt.setFillColor(sf::Color(255, 230, 160));
        sf::FloatRect tb = txt.getLocalBounds();
        txt.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));
        txt.setPosition(center.x, center.y - 1.0f);
        window.draw(txt);
    }
}

void GameManager::khoiTaoRandomLuotScene() {
    isRandomRolling = false;
    rollFinished = false;
    rollTimer = 0.0f;
    rollAngle = 0.0f;
    rollWinner = 0;

    auto& assets = getMenuAssets();
    sf::Font& fBold = assets.fontBoldLoaded ? assets.fontBold : fontBold;

    btnRoll = std::make_unique<Button>(
        460.0f, 650.0f, 280.0f, 52.0f,
        Loc::get(LocKey::RANDOM_BTN_ROLL), fBold,
        [this]() {
            batDauRandomRoll();
        }
    );
    btnRoll->setCharacterSize(18);
    btnRoll->setColors(sf::Color(190, 45, 45), sf::Color(225, 65, 65), sf::Color(140, 30, 30));

    btnStartFromRoll = std::make_unique<Button>(
        460.0f, 650.0f, 280.0f, 52.0f,
        Loc::get(LocKey::RANDOM_BTN_START), fBold,
        [this]() {
            soundManager.play(SoundType::CLICK);
            if (cheDoChoi == CheDoChoi::HAI_NGUOI) {
                pvpFirstPlayer = (rollWinner == 0) ? 1 : 2;
                batDauPvp();
            } else if (cheDoChoi == CheDoChoi::VOI_MAY) {
                aiFirstChoice = (rollWinner == 0) ? 0 : 1;
                batDauPvai();
            }
        }
    );
    btnStartFromRoll->setCharacterSize(18);
    btnStartFromRoll->setColors(sf::Color(40, 130, 60), sf::Color(55, 170, 80), sf::Color(30, 95, 45));

    btnReroll.reset();

    btnBackFromRoll = std::make_unique<Button>(
        40.0f, 35.0f, 150.0f, 42.0f,
        Loc::get(LocKey::BACK), fBold,
        [this]() {
            soundManager.play(SoundType::CLICK);
            khoiTaoPlaySetupButtons();
            trangThai = (cheDoChoi == CheDoChoi::HAI_NGUOI) ? TrangThai::PVP_SETUP : TrangThai::PVAI_SETUP;
        }
    );
    btnBackFromRoll->setCharacterSize(16);
}

void GameManager::batDauRandomRoll() {
    soundManager.play(SoundType::CLICK);
    isRandomRolling = true;
    rollFinished = false;
    rollTimer = 1.2f;
    rollAngle = 0.0f;
    rollWinner = (turnRandomizer.flip() == TurnResult::PLAYER1_RED) ? 0 : 1;
}

void GameManager::capNhatRandomRoll(float dt) {
    if (isRandomRolling) {
        rollTimer -= dt;
        rollAngle += 960.0f * dt;
        if (rollTimer <= 0.0f) {
            rollTimer = 0.0f;
            rollAngle = 0.0f;
            isRandomRolling = false;
            rollFinished = true;
            soundManager.play(SoundType::CAPTURE);
        }
    }
}

void GameManager::veRandomLuot() {
    FantasyUI::get().drawFullDimOverlay(window, sf::Color(10, 8, 12, 85));

    auto& assets = getMenuAssets();
    bool vi = (Loc::getLanguage() == Language::TIENG_VIET);
    sf::Font& fBold = assets.fontBoldLoaded ? assets.fontBold : fontBold;

    // 1. Header Title & Subtitle (Centered strictly at logical center X = 600.0f)
    sf::Text title;
    title.setFont(fBold);
    title.setCharacterSize(36);
    title.setStyle(sf::Text::Bold);
    title.setString(toUtf8(Loc::get(LocKey::RANDOM_TITLE)));
    title.setFillColor(sf::Color(255, 222, 105));
    sf::FloatRect tb = title.getLocalBounds();
    title.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));

    sf::Text titleShadow = title;
    titleShadow.setFillColor(sf::Color(0, 0, 0, 180));
    titleShadow.setPosition(602.0f, 50.0f);
    window.draw(titleShadow);

    title.setPosition(600.0f, 48.0f);
    window.draw(title);

    sf::Text subtitle;
    subtitle.setFont(fBold);
    subtitle.setCharacterSize(16);
    subtitle.setStyle(sf::Text::Bold);
    subtitle.setString(toUtf8(Loc::get(LocKey::RANDOM_SUBTITLE)));
    subtitle.setFillColor(sf::Color(255, 235, 150));
    tb = subtitle.getLocalBounds();
    subtitle.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));

    sf::Text subtitleShadow = subtitle;
    subtitleShadow.setFillColor(sf::Color(0, 0, 0, 160));
    subtitleShadow.setPosition(601.0f, 91.0f);
    window.draw(subtitleShadow);

    subtitle.setPosition(600.0f, 90.0f);
    window.draw(subtitle);

    // Determine Names and Avatars
    std::string p1Name, p2Name;
    int p1Char = 0, p2Char = 1;

    if (cheDoChoi == CheDoChoi::HAI_NGUOI) {
        p1Name = inputP1Name->getValue();
        if (p1Name.empty()) p1Name = (Loc::getLanguage() == Language::TIENG_VIET ? "Người chơi 1" : "Player 1");
        p1Char = p1CharIndex;

        p2Name = inputP2Name->getValue();
        if (p2Name.empty()) p2Name = (Loc::getLanguage() == Language::TIENG_VIET ? "Người chơi 2" : "Player 2");
        p2Char = p2CharIndex;
    } else {
        p1Name = inputAiPlayerName->getValue();
        if (p1Name.empty()) p1Name = (Loc::getLanguage() == Language::TIENG_VIET ? "Người chơi" : "Player");
        p1Char = aiPlayerCharIndex;

        p2Name = "AI (" + Loc::get((aiDifficulty == DoKho::DE) ? LocKey::DIFF_EASY : ((aiDifficulty == DoKho::TRUNG_BINH) ? LocKey::DIFF_MED : LocKey::DIFF_HARD)) + ")";
        p2Char = 0;
    }

    // 2. Left Player Card (Framed with Oriental Ebony Panel)
    sf::FloatRect cardRect1(80.0f, 130.0f, 360.0f, 480.0f);
    bool p1IsRed = rollFinished && (rollWinner == 0);
    sf::Color tint1 = p1IsRed ? sf::Color(255, 215, 215) : sf::Color(215, 235, 255);
    FantasyUI::get().drawPanel(window, cardRect1, OrientalPanelStyle::EBONY_WOOD, tint1);

    sf::RectangleShape h1(sf::Vector2f(340.0f, 38.0f));
    h1.setPosition(90.0f, 140.0f);
    h1.setFillColor(p1IsRed ? sf::Color(85, 25, 25, 220) : sf::Color(32, 45, 68, 220));
    h1.setOutlineThickness(1.5f);
    h1.setOutlineColor(p1IsRed ? sf::Color(230, 80, 80) : sf::Color(80, 130, 200));
    window.draw(h1);

    sf::Text t1;
    t1.setFont(fBold);
    t1.setCharacterSize(16);
    t1.setStyle(sf::Text::Bold);
    t1.setString(toUtf8((cheDoChoi == CheDoChoi::HAI_NGUOI) ? "[ " + Loc::get(LocKey::P1_LABEL) + " ]" : "[ " + Loc::get(LocKey::PLAYER_LABEL) + " ]"));
    t1.setFillColor(p1IsRed ? sf::Color(255, 210, 210) : sf::Color(190, 225, 255));
    tb = t1.getLocalBounds();
    t1.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));

    sf::Text t1Shadow = t1;
    t1Shadow.setFillColor(sf::Color(0, 0, 0, 180));
    t1Shadow.setPosition(261.0f, 160.0f);
    window.draw(t1Shadow);

    t1.setPosition(260.0f, 159.0f);
    window.draw(t1);

    // Left Avatar Portrait
    sf::Vector2f ava1Pos(260.0f, 260.0f);
    Mau m1 = rollFinished ? (p1IsRed ? Mau::DO : Mau::DEN) : Mau::DO;
    veAvatarPortrait(ava1Pos, 54.0f, p1Char, m1, p1IsRed);

    sf::Text np1;
    np1.setFont(fBold);
    np1.setCharacterSize(22);
    np1.setStyle(sf::Text::Bold);
    np1.setString(toUtf8(p1Name));
    np1.setFillColor(p1IsRed ? sf::Color(255, 140, 140) : sf::Color(255, 235, 150));
    tb = np1.getLocalBounds();
    np1.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));

    sf::Text np1Shadow = np1;
    np1Shadow.setFillColor(sf::Color(0, 0, 0, 180));
    np1Shadow.setPosition(261.0f, 350.0f);
    window.draw(np1Shadow);

    np1.setPosition(260.0f, 349.0f);
    window.draw(np1);

    sf::Text rp1;
    rp1.setFont(fBold);
    rp1.setCharacterSize(17);
    rp1.setStyle(sf::Text::Bold);
    rp1.setString(toUtf8("[" + Loc::getCharName(p1Char) + "]"));
    rp1.setFillColor(sf::Color(255, 222, 120));
    tb = rp1.getLocalBounds();
    rp1.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));

    sf::Text rp1Shadow = rp1;
    rp1Shadow.setFillColor(sf::Color(0, 0, 0, 180));
    rp1Shadow.setPosition(261.0f, 388.0f);
    window.draw(rp1Shadow);

    rp1.setPosition(260.0f, 387.0f);
    window.draw(rp1);

    // Left Result Status
    sf::RectangleShape stBox1(sf::Vector2f(300.0f, 46.0f));
    stBox1.setPosition(110.0f, 460.0f);
    if (rollFinished) {
        if (p1IsRed) {
            stBox1.setFillColor(sf::Color(135, 25, 25));
            stBox1.setOutlineThickness(2.0f);
            stBox1.setOutlineColor(sf::Color(255, 215, 0));
        } else {
            stBox1.setFillColor(sf::Color(25, 45, 80));
            stBox1.setOutlineThickness(1.5f);
            stBox1.setOutlineColor(sf::Color(110, 175, 255));
        }
    } else {
        stBox1.setFillColor(sf::Color(20, 25, 36));
        stBox1.setOutlineThickness(1.0f);
        stBox1.setOutlineColor(sf::Color(70, 85, 110));
    }
    window.draw(stBox1);

    sf::Text stTxt1;
    stTxt1.setFont(fBold);
    stTxt1.setCharacterSize(16);
    stTxt1.setStyle(sf::Text::Bold);
    if (isRandomRolling) {
        stTxt1.setString(toUtf8((Loc::getLanguage() == Language::TIENG_VIET) ? "Đang quay đồng xu..." : "Rolling turn..."));
        stTxt1.setFillColor(sf::Color(255, 235, 150));
    } else if (rollFinished) {
        if (p1IsRed) {
            stTxt1.setString(toUtf8((Loc::getLanguage() == Language::TIENG_VIET) ? "• Quân Đỏ (Đi trước) •" : "• Red Side (Moves first) •"));
            stTxt1.setFillColor(sf::Color(255, 235, 110));
        } else {
            stTxt1.setString(toUtf8((Loc::getLanguage() == Language::TIENG_VIET) ? "Quân Đen (Đi sau)" : "Black Side (Moves second)"));
            stTxt1.setFillColor(sf::Color(180, 225, 255));
        }
    } else {
        stTxt1.setString(toUtf8((Loc::getLanguage() == Language::TIENG_VIET) ? "(Sẵn sàng tung lượt)" : "(Ready to roll)"));
        stTxt1.setFillColor(sf::Color(180, 195, 215));
    }
    tb = stTxt1.getLocalBounds();
    stTxt1.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));

    sf::Text stTxt1Shadow = stTxt1;
    stTxt1Shadow.setFillColor(sf::Color(0, 0, 0, 180));
    stTxt1Shadow.setPosition(261.0f, 484.0f);
    window.draw(stTxt1Shadow);

    stTxt1.setPosition(260.0f, 483.0f);
    window.draw(stTxt1);

    // 3. Right Player Card (Framed with Oriental Ebony Panel)
    sf::FloatRect cardRect2(760.0f, 130.0f, 360.0f, 480.0f);
    bool p2IsRed = rollFinished && (rollWinner == 1);
    sf::Color tint2 = p2IsRed ? sf::Color(255, 215, 215) : sf::Color(215, 235, 255);
    FantasyUI::get().drawPanel(window, cardRect2, OrientalPanelStyle::EBONY_WOOD, tint2);

    sf::RectangleShape h2(sf::Vector2f(340.0f, 38.0f));
    h2.setPosition(770.0f, 140.0f);
    h2.setFillColor(p2IsRed ? sf::Color(85, 25, 25, 220) : sf::Color(32, 45, 68, 220));
    h2.setOutlineThickness(1.5f);
    h2.setOutlineColor(p2IsRed ? sf::Color(230, 80, 80) : sf::Color(80, 130, 200));
    window.draw(h2);

    sf::Text t2;
    t2.setFont(fBold);
    t2.setCharacterSize(16);
    t2.setStyle(sf::Text::Bold);
    t2.setString(toUtf8((cheDoChoi == CheDoChoi::HAI_NGUOI) ? "[ " + Loc::get(LocKey::P2_LABEL) + " ]" : "[ AI Engine ]"));
    t2.setFillColor(p2IsRed ? sf::Color(255, 210, 210) : sf::Color(190, 225, 255));
    tb = t2.getLocalBounds();
    t2.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));

    sf::Text t2Shadow = t2;
    t2Shadow.setFillColor(sf::Color(0, 0, 0, 180));
    t2Shadow.setPosition(941.0f, 160.0f);
    window.draw(t2Shadow);

    t2.setPosition(940.0f, 159.0f);
    window.draw(t2);

    // Right Avatar Portrait
    sf::Vector2f ava2Pos(940.0f, 260.0f);
    Mau m2 = rollFinished ? (p2IsRed ? Mau::DO : Mau::DEN) : Mau::DEN;
    veAvatarPortrait(ava2Pos, 54.0f, p2Char, m2, p2IsRed);

    sf::Text np2;
    np2.setFont(fBold);
    np2.setCharacterSize(22);
    np2.setStyle(sf::Text::Bold);
    np2.setString(toUtf8(p2Name));
    np2.setFillColor(p2IsRed ? sf::Color(255, 140, 140) : sf::Color(235, 245, 255));
    tb = np2.getLocalBounds();
    np2.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));

    sf::Text np2Shadow = np2;
    np2Shadow.setFillColor(sf::Color(0, 0, 0, 180));
    np2Shadow.setPosition(941.0f, 350.0f);
    window.draw(np2Shadow);

    np2.setPosition(940.0f, 349.0f);
    window.draw(np2);

    sf::Text rp2;
    rp2.setFont(fBold);
    rp2.setCharacterSize(17);
    rp2.setStyle(sf::Text::Bold);
    rp2.setString(toUtf8("[" + Loc::getCharName(p2Char) + "]"));
    rp2.setFillColor(sf::Color(255, 222, 120));
    tb = rp2.getLocalBounds();
    rp2.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));

    sf::Text rp2Shadow = rp2;
    rp2Shadow.setFillColor(sf::Color(0, 0, 0, 180));
    rp2Shadow.setPosition(941.0f, 388.0f);
    window.draw(rp2Shadow);

    rp2.setPosition(940.0f, 387.0f);
    window.draw(rp2);

    // Right Result Status
    sf::RectangleShape stBox2(sf::Vector2f(300.0f, 46.0f));
    stBox2.setPosition(790.0f, 460.0f);
    if (rollFinished) {
        if (p2IsRed) {
            stBox2.setFillColor(sf::Color(135, 25, 25));
            stBox2.setOutlineThickness(2.0f);
            stBox2.setOutlineColor(sf::Color(255, 215, 0));
        } else {
            stBox2.setFillColor(sf::Color(25, 45, 80));
            stBox2.setOutlineThickness(1.5f);
            stBox2.setOutlineColor(sf::Color(110, 175, 255));
        }
    } else {
        stBox2.setFillColor(sf::Color(20, 25, 36));
        stBox2.setOutlineThickness(1.0f);
        stBox2.setOutlineColor(sf::Color(70, 85, 110));
    }
    window.draw(stBox2);

    sf::Text stTxt2;
    stTxt2.setFont(fBold);
    stTxt2.setCharacterSize(16);
    stTxt2.setStyle(sf::Text::Bold);
    if (isRandomRolling) {
        stTxt2.setString(toUtf8((Loc::getLanguage() == Language::TIENG_VIET) ? "Đang quay đồng xu..." : "Rolling turn..."));
        stTxt2.setFillColor(sf::Color(255, 235, 150));
    } else if (rollFinished) {
        if (p2IsRed) {
            stTxt2.setString(toUtf8((Loc::getLanguage() == Language::TIENG_VIET) ? "• Quân Đỏ (Đi trước) •" : "• Red Side (Moves first) •"));
            stTxt2.setFillColor(sf::Color(255, 235, 110));
        } else {
            stTxt2.setString(toUtf8((Loc::getLanguage() == Language::TIENG_VIET) ? "Quân Đen (Đi sau)" : "Black Side (Moves second)"));
            stTxt2.setFillColor(sf::Color(180, 225, 255));
        }
    } else {
        stTxt2.setString(toUtf8((Loc::getLanguage() == Language::TIENG_VIET) ? "(Sẵn sàng tung lượt)" : "(Ready to roll)"));
        stTxt2.setFillColor(sf::Color(180, 195, 215));
    }
    tb = stTxt2.getLocalBounds();
    stTxt2.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));

    sf::Text stTxt2Shadow = stTxt2;
    stTxt2Shadow.setFillColor(sf::Color(0, 0, 0, 180));
    stTxt2Shadow.setPosition(941.0f, 484.0f);
    window.draw(stTxt2Shadow);

    stTxt2.setPosition(940.0f, 483.0f);
    window.draw(stTxt2);

    // 4. Center Coin / Dice Roll Zone
    sf::Vector2f coinPos(600.0f, 320.0f);
    float coinBaseR = 60.0f;
    float cosVal = std::cos(rollAngle * 0.0174533f);
    float coinScaleX = isRandomRolling ? std::max(0.08f, std::abs(cosVal)) : 1.0f;

    bool showRedSide = isRandomRolling ? (cosVal >= 0.0f) : (rollWinner == 0);

    // Decorative Coin Label Badge
    sf::Text coinBadge;
    coinBadge.setFont(fBold);
    coinBadge.setCharacterSize(16);
    coinBadge.setStyle(sf::Text::Bold);
    coinBadge.setString(toUtf8(vi ? "[ Đồng xu lượt đi ]" : "[ Turn Coin ]"));
    coinBadge.setFillColor(sf::Color(255, 230, 140));
    tb = coinBadge.getLocalBounds();
    coinBadge.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));

    sf::Text badgeShadow = coinBadge;
    badgeShadow.setFillColor(sf::Color(0, 0, 0, 180));
    badgeShadow.setPosition(coinPos.x + 1.0f, coinPos.y - 82.0f + 1.0f);
    window.draw(badgeShadow);

    coinBadge.setPosition(coinPos.x, coinPos.y - 82.0f);
    window.draw(coinBadge);

    // Coin Drop Shadow
    sf::CircleShape coinShadow(coinBaseR);
    coinShadow.setOrigin(coinBaseR, coinBaseR);
    coinShadow.setPosition(coinPos.x + 2.0f, coinPos.y + 4.0f);
    coinShadow.setScale(coinScaleX, 1.0f);
    coinShadow.setFillColor(sf::Color(0, 0, 0, 95));
    window.draw(coinShadow);

    // Main Coin Medallion
    sf::CircleShape coin(coinBaseR);
    coin.setOrigin(coinBaseR, coinBaseR);
    coin.setPosition(coinPos);
    coin.setScale(coinScaleX, 1.0f);
    coin.setFillColor(showRedSide ? sf::Color(175, 30, 30) : sf::Color(30, 45, 68));
    coin.setOutlineThickness(3.5f);
    coin.setOutlineColor(sf::Color(255, 215, 0));
    window.draw(coin);

    sf::CircleShape coinInner(coinBaseR - 6.0f);
    coinInner.setOrigin(coinBaseR - 6.0f, coinBaseR - 6.0f);
    coinInner.setPosition(coinPos);
    coinInner.setScale(coinScaleX, 1.0f);
    coinInner.setFillColor(sf::Color::Transparent);
    coinInner.setOutlineThickness(1.5f);
    coinInner.setOutlineColor(sf::Color(255, 235, 120, 180));
    window.draw(coinInner);

    sf::Text coinTxt;
    coinTxt.setFont(fBold);
    coinTxt.setStyle(sf::Text::Bold);
    if (vi) {
        coinTxt.setCharacterSize(34);
        coinTxt.setString(toUtf8(showRedSide ? "ĐỎ" : "ĐEN"));
    } else {
        coinTxt.setCharacterSize(showRedSide ? 28 : 24);
        coinTxt.setString(showRedSide ? "RED" : "BLACK");
    }
    coinTxt.setScale(coinScaleX, 1.0f);
    coinTxt.setFillColor(sf::Color(255, 240, 140));
    tb = coinTxt.getLocalBounds();
    coinTxt.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));

    sf::Text coinTxtShadow = coinTxt;
    coinTxtShadow.setFillColor(sf::Color(0, 0, 0, 180));
    coinTxtShadow.setPosition(coinPos.x + 1.0f, coinPos.y - 2.0f + 1.0f);
    window.draw(coinTxtShadow);

    coinTxt.setPosition(coinPos.x, coinPos.y - 2.0f);
    window.draw(coinTxt);

    // Dynamic result subtitle below coin
    sf::Text coinResultHint;
    coinResultHint.setFont(fBold);
    coinResultHint.setCharacterSize(16);
    coinResultHint.setStyle(sf::Text::Bold);
    if (isRandomRolling) {
        coinResultHint.setString(toUtf8(vi ? "Đang tung đồng xu..." : "Rolling..."));
        coinResultHint.setFillColor(sf::Color(255, 235, 150));
    } else if (rollFinished) {
        if (cheDoChoi == CheDoChoi::HAI_NGUOI) {
            if (rollWinner == 0) {
                coinResultHint.setString(toUtf8(vi ? "-> Người chơi 1 đi trước (Đỏ)" : "-> Player 1 moves first (Red)"));
            } else {
                coinResultHint.setString(toUtf8(vi ? "-> Người chơi 2 đi trước (Đỏ)" : "-> Player 2 moves first (Red)"));
            }
        } else {
            if (rollWinner == 0) {
                coinResultHint.setString(toUtf8(vi ? "-> Bạn đi trước (Đỏ)" : "-> You move first (Red)"));
            } else {
                coinResultHint.setString(toUtf8(vi ? "-> Máy đi trước (Đỏ)" : "-> AI moves first (Red)"));
            }
        }
        coinResultHint.setFillColor(sf::Color(255, 235, 140));
    } else {
        coinResultHint.setString(toUtf8(vi ? "Bấm để tung lượt đi" : "Click to roll turn"));
        coinResultHint.setFillColor(sf::Color(200, 215, 235));
    }
    tb = coinResultHint.getLocalBounds();
    coinResultHint.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));

    sf::Text hintShadow = coinResultHint;
    hintShadow.setFillColor(sf::Color(0, 0, 0, 180));
    hintShadow.setPosition(coinPos.x + 1.0f, coinPos.y + 82.0f + 1.0f);
    window.draw(hintShadow);

    coinResultHint.setPosition(coinPos.x, coinPos.y + 82.0f);
    window.draw(coinResultHint);

    // 5. Action Buttons
    if (!isRandomRolling) {
        if (!rollFinished) {
            if (btnRoll) btnRoll->draw(window);
        } else {
            if (btnStartFromRoll) btnStartFromRoll->draw(window);
        }
    }
    if (btnBackFromRoll) btnBackFromRoll->draw(window);
}

void GameManager::vePopupXinHoa() {
    FantasyUI::get().drawFullDimOverlay(window, sf::Color(0, 0, 0, 185));

    const float dw = 480.0f;
    const float dh = 270.0f;
    const float dx = (1200.0f - dw) / 2.0f;
    const float dy = (800.0f - dh) / 2.0f;

    bool vi = (Loc::getLanguage() == Language::TIENG_VIET);
    sf::Font& fTitle = getMenuFont(vi, true);
    sf::Font& fBody = getMenuFont(vi, true);

    // Modal background
    FantasyUI::get().drawPanel(window, sf::FloatRect(dx, dy, dw, dh), FantasyPanelStyle::ORNATE_WOOD, sf::Color(140, 185, 230));

    // Ribbon Title
    std::string titleStr = vi ? "[ ĐỀ NGHỊ HÒA CỜ ]" : "[ OFFER A DRAW ]";
    FantasyUI::get().drawRibbonTitle(window, dx + dw / 2.0f, dy + 18.0f, 320.0f, 42.0f,
                                     titleStr, fTitle, 16, sf::Color(215, 235, 255), sf::Color(130, 180, 230));

    // Content: who is asking
    std::string senderName = (nguoiXinHoa == Mau::DO) ? tenNguoiChoiDo : tenNguoiChoiDen;
    std::string senderSide = (nguoiXinHoa == Mau::DO) ? (vi ? "Bên Đỏ" : "Red Side") : (vi ? "Bên Đen" : "Black Side");

    sf::Text tMsg1;
    tMsg1.setFont(fBody);
    tMsg1.setCharacterSize(17);
    tMsg1.setStyle(sf::Text::Bold);
    tMsg1.setString(toUtf8(senderName + " (" + senderSide + ") " + (vi ? "muốn cầu hòa ván cờ này." : "proposes an honorable draw.")));
    tMsg1.setFillColor(sf::Color(255, 240, 190));
    tMsg1.setOutlineThickness(0.8f);
    tMsg1.setOutlineColor(sf::Color(20, 15, 10, 220));
    sf::FloatRect b1 = tMsg1.getLocalBounds();
    tMsg1.setOrigin(std::round(b1.left + b1.width / 2.0f), std::round(b1.top + b1.height / 2.0f));
    tMsg1.setPosition(dx + dw / 2.0f, dy + 88.0f);
    window.draw(tMsg1);

    sf::Text tMsg2;
    tMsg2.setFont(fBody);
    tMsg2.setCharacterSize(18);
    tMsg2.setStyle(sf::Text::Bold);
    tMsg2.setString(toUtf8(vi ? "Bạn có đồng ý kết thúc trận đấu với tỷ số hòa?" : "Do you agree to conclude this match as a draw?"));
    tMsg2.setFillColor(sf::Color(255, 245, 215));
    tMsg2.setOutlineThickness(0.8f);
    tMsg2.setOutlineColor(sf::Color(20, 15, 10, 220));
    sf::FloatRect b2 = tMsg2.getLocalBounds();
    tMsg2.setOrigin(std::round(b2.left + b2.width / 2.0f), std::round(b2.top + b2.height / 2.0f));
    tMsg2.setPosition(dx + dw / 2.0f, dy + 120.0f);
    window.draw(tMsg2);

    // Decorative line
    sf::RectangleShape divLine(sf::Vector2f(dw - 80.0f, 1.5f));
    divLine.setPosition(dx + 40.0f, dy + 155.0f);
    divLine.setFillColor(sf::Color(100, 80, 60, 180));
    window.draw(divLine);

    // Action buttons
    if (btnAcceptDraw) btnAcceptDraw->draw(window);
    if (btnDeclineDraw) btnDeclineDraw->draw(window);
}

void GameManager::vePopupNuocDiSai() {
    float alphaRatio = (thoiGianNuocDiSai < 0.4f) ? (thoiGianNuocDiSai / 0.4f) : 1.0f;
    if (alphaRatio < 0.0f) alphaRatio = 0.0f;
    sf::Uint8 alpha = static_cast<sf::Uint8>(255.0f * alphaRatio);
    if (alpha == 0) return;

    FantasyUI::get().drawFullDimOverlay(window, sf::Color(0, 0, 0, static_cast<sf::Uint8>(110 * alphaRatio)));

    const float dw = 480.0f;
    const float dh = 210.0f;
    const float dx = (1200.0f - dw) / 2.0f;
    const float dy = (800.0f - dh) / 2.0f;

    bool vi = (Loc::getLanguage() == Language::TIENG_VIET);
    sf::Font& fTitle = getMenuFont(vi, true);
    sf::Font& fBody = getMenuFont(vi, true);

    // Modal background with pulse border
    float pulse = 0.5f + 0.5f * std::sin(bannerPulseClock.getElapsedTime().asSeconds() * 10.0f);
    sf::Color borderColor(255, 60, 50, static_cast<sf::Uint8>(alpha * (0.8f + 0.2f * pulse)));
    FantasyUI::get().drawPanel(window, sf::FloatRect(dx, dy, dw, dh), FantasyPanelStyle::ORNATE_WOOD, borderColor);

    // Ribbon Title
    std::string titleStr = vi ? "[ ! ] NƯỚC ĐI KHÔNG HỢP LỆ [ ! ]" : "[ ! ] ILLEGAL MOVE [ ! ]";
    FantasyUI::get().drawRibbonTitle(window, dx + dw / 2.0f, dy + 16.0f, 360.0f, 40.0f,
                                     titleStr, fTitle, 16, sf::Color(255, 230, 200, alpha), borderColor);

    // Warning message
    sf::Text tReason;
    tReason.setFont(fBody);
    tReason.setCharacterSize(18);
    tReason.setStyle(sf::Text::Bold);
    tReason.setString(toUtf8(lyDoNuocDiSai));
    tReason.setFillColor(sf::Color(255, 240, 180, alpha));
    tReason.setOutlineThickness(1.0f);
    tReason.setOutlineColor(sf::Color(20, 12, 8, alpha));
    sf::FloatRect rb = tReason.getLocalBounds();
    tReason.setOrigin(std::round(rb.left + rb.width / 2.0f), std::round(rb.top + rb.height / 2.0f));
    tReason.setPosition(dx + dw / 2.0f, dy + 95.0f);
    window.draw(tReason);

    // Dismiss hint
    sf::Text tHint;
    tHint.setFont(fBody);
    tHint.setCharacterSize(14);
    tHint.setStyle(sf::Text::Bold);
    tHint.setString(toUtf8(vi ? "(Nhấp chuột bất kỳ đâu để tiếp tục)" : "(Click anywhere to dismiss)"));
    tHint.setFillColor(sf::Color(225, 205, 160, static_cast<sf::Uint8>(alpha * 0.95f)));
    sf::FloatRect hb = tHint.getLocalBounds();
    tHint.setOrigin(std::round(hb.left + hb.width / 2.0f), std::round(hb.top + hb.height / 2.0f));
    tHint.setPosition(dx + dw / 2.0f, dy + dh - 26.0f);
    window.draw(tHint);
}

void GameManager::khoiTaoVictoryParticles() {
    victoryParticles.clear();
    victoryParticlesActive = true;
    const sf::Color colors[] = {
        sf::Color(255, 215, 0),    // Gold
        sf::Color(255, 60, 50),    // Crimson
        sf::Color(60, 220, 120),   // Jade green
        sf::Color(80, 170, 255),   // Azure
        sf::Color(255, 175, 40),   // Amber
        sf::Color(255, 250, 230),  // Ivory white
        sf::Color(230, 80, 210)    // Orchid
    };
    size_t numColors = sizeof(colors) / sizeof(colors[0]);

    for (int i = 0; i < 140; ++i) {
        VictoryParticle p;
        p.pos.x = static_cast<float>(rand() % 1200);
        p.pos.y = -20.0f - static_cast<float>(rand() % 400);
        p.vel.x = static_cast<float>((rand() % 120) - 60) * 1.5f;
        p.vel.y = 80.0f + static_cast<float>(rand() % 160);
        p.rot = static_cast<float>(rand() % 360);
        p.rotSpeed = static_cast<float>((rand() % 240) - 120) * 1.5f;
        p.size = 6.0f + static_cast<float>(rand() % 8);
        p.color = colors[rand() % numColors];
        victoryParticles.push_back(p);
    }
}

void GameManager::capNhatVictoryParticles(float dt) {
    if (!victoryParticlesActive) return;

    for (auto& p : victoryParticles) {
        p.pos += p.vel * dt;
        p.rot += p.rotSpeed * dt;
        p.vel.x += std::sin(p.pos.y * 0.04f) * 20.0f * dt;

        if (p.pos.y > 830.0f) {
            p.pos.y = -20.0f - static_cast<float>(rand() % 100);
            p.pos.x = static_cast<float>(rand() % 1200);
            p.vel.y = 80.0f + static_cast<float>(rand() % 160);
        }
    }
}

void GameManager::veVictoryParticles() {
    if (!victoryParticlesActive) return;

    for (const auto& p : victoryParticles) {
        sf::RectangleShape shape(sf::Vector2f(p.size, p.size * 0.6f));
        shape.setOrigin(p.size / 2.0f, (p.size * 0.6f) / 2.0f);
        shape.setPosition(p.pos);
        shape.setRotation(p.rot);
        shape.setFillColor(p.color);
        window.draw(shape);
    }
}

