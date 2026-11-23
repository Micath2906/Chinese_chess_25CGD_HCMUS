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

static inline float lerpVal(float a, float b, float t) {
    return a + t * (b - a);
}

GameManager::GameManager() 
    : window(sf::VideoMode(1200, 800), "Co Tuong - Chinese Chess (HCMUS OOP)", sf::Style::Default),
      trangThai(TrangThai::TITLE_SCREEN),
      cheDoChoi(CheDoChoi::HAI_NGUOI),
      rebindingAction(KeyAction::COUNT),
      activeSettingsTab(SettingsTab::SOUND),
      settingsDetailOpen(false),
      settingsSlideProgress(0.0f),
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
      defeatParticlesActive(false),
      defeatVignetteTimer(0.0f),
      screenShakeTimer(0.0f),
      screenShakeIntensity(0.0f),
      isRandomRolling(false),
      rollFinished(false),
      rollTimer(0.0f),
      rollAngle(0.0f),
      rollWinner(0),
      viTriNuocDiSai(-1, -1),
      thoiGianNuocDiSai(0.0f),
      lyDoNuocDiSai(""),
      viTriQuanRung(-1, -1),
      thoiGianRungQuan(0.0f),
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
    menuChinh.themButton(0, 0, 240.0f, 44.0f, isVi ? "CHƠI" : "PLAY", [this]() {
        soundManager.play(SoundType::CLICK);
        menuChinh.setSection(MenuSection::PLAY);
    });
    menuChinh.themButton(0, 0, 240.0f, 44.0f, isVi ? "TẢI VÁN ĐẤU" : "LOAD GAME", [this]() {
        soundManager.play(SoundType::CLICK);
        capNhatThongTinSlots();
        menuChinh.setSection(MenuSection::LOAD);
    });
    menuChinh.themButton(0, 0, 240.0f, 44.0f, Loc::get(LocKey::MENU_SETTINGS), [this]() {
        soundManager.play(SoundType::CLICK);
        menuChinh.setSection(MenuSection::SETTINGS);
    });
    menuChinh.themButton(0, 0, 240.0f, 44.0f, isVi ? "THÔNG TIN" : "INFO & RULES", [this]() {
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
    menuChinh.onOpenSettings = [this]() {
        soundManager.play(SoundType::CLICK);
        menuChinh.setSection(MenuSection::SETTINGS);
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
    menuPlaySub.themButton(cx, psubY + sp * 2, bw, bh, Loc::get(LocKey::BACK), [this]() {
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
    menuIntro.setFont(fontBold);
    menuIntro.xoaTatCaButton();
    menuIntro.themButton(490.0f, 624.0f, 220.0f, 44.0f, Loc::get(LocKey::BACK), [this]() {
        soundManager.play(SoundType::CLICK);
        trangThai = trangThaiTruocIntro;
    });

    capNhatNhanNut();
}

void GameManager::capNhatNhanNut() {
    // 1. Menu Chinh (5 mục)
    bool isVi = (Loc::getLanguage() == Language::TIENG_VIET);
    if (auto b = menuChinh.layButton(0)) b->setLabel(isVi ? "CHƠI" : "PLAY");
    if (auto b = menuChinh.layButton(1)) b->setLabel(isVi ? "TẢI VÁN ĐẤU" : "LOAD GAME");
    if (auto b = menuChinh.layButton(2)) b->setLabel(Loc::get(LocKey::MENU_SETTINGS));
    if (auto b = menuChinh.layButton(3)) b->setLabel(isVi ? "THÔNG TIN" : "INFO & RULES");
    if (auto b = menuChinh.layButton(4)) b->setLabel(Loc::get(LocKey::MENU_EXIT));
    menuChinh.capNhatNutTrangPhai();

    // 2. Settings Dashboard Tabs & Buttons
    if (settingsTabButtons.size() >= 4) {
        settingsTabButtons[0]->setLabel(isVi ? "ÂM THANH" : "SOUND");
        settingsTabButtons[1]->setLabel(isVi ? "LỐI CHƠI" : "GAMEPLAY");
        settingsTabButtons[2]->setLabel(isVi ? "NGÔN NGỮ" : "LANGUAGE");
        settingsTabButtons[3]->setLabel(isVi ? "ĐIỀU KHIỂN" : "CONTROLS");
    }
    if (settingsCloseBtn) settingsCloseBtn->setLabel(isVi ? "ĐÓNG" : "CLOSE");
    if (settingsCollapseBtn) settingsCollapseBtn->setLabel(isVi ? "Quay lại" : "Back");

    if (sliderMaster) sliderMaster->setTitle(Loc::get(LocKey::SETTING_MASTER_VOL));
    if (sliderSfx) sliderSfx->setTitle(Loc::get(LocKey::SETTING_SFX_VOL));
    if (sliderBgm) sliderBgm->setTitle(Loc::get(LocKey::SETTING_BGM_VOL));

    if (settingsAudioButtons.size() >= 2) {
        bool sfxOn = soundManager.getSfxBat();
        bool bgmOn = soundManager.getBgmBat();
        settingsAudioButtons[0]->setLabel(
            (isVi ? "Âm hiệu ứng (SFX):  " : "Sound FX (SFX):  ") +
            std::string(sfxOn ? (isVi ? "[ BẬT ]" : "[ ON ]") : (isVi ? "[ TẮT ]" : "[ OFF ]"))
        );
        settingsAudioButtons[0]->setSelected(sfxOn);
        settingsAudioButtons[1]->setLabel(
            (isVi ? "Nhạc nền (BGM):  " : "Music (BGM):  ") +
            std::string(bgmOn ? (isVi ? "[ BẬT ]" : "[ ON ]") : (isVi ? "[ TẮT ]" : "[ OFF ]"))
        );
        settingsAudioButtons[1]->setSelected(bgmOn);
    }

    if (settingsGameplayButtons.size() >= 6) {
        settingsGameplayButtons[0]->setLabel((isVi ? "Tốc độ AI: " : "AI Pacing: ") + std::string(aiThinkRealistic ? (isVi ? "< Tự nhiên >" : "< Natural >") : (isVi ? "< Tức thì >" : "< Instant >")));
        settingsGameplayButtons[0]->setSelected(aiThinkRealistic);

        settingsGameplayButtons[1]->setLabel((isVi ? "Báo chiếu: " : "Check Alarm: ") + std::string(checkAlarmSound ? (isVi ? "[ BẬT ]" : "[ ON ]") : (isVi ? "[ TẮT ]" : "[ OFF ]")));
        settingsGameplayButtons[1]->setSelected(checkAlarmSound);

        settingsGameplayButtons[2]->setLabel((isVi ? "Gợi ý nước: " : "Move Hints: ") + std::string(hienGoiY ? (isVi ? "[ BẬT ]" : "[ ON ]") : (isVi ? "[ TẮT ]" : "[ OFF ]")));
        settingsGameplayButtons[2]->setSelected(hienGoiY);

        settingsGameplayButtons[3]->setLabel((isVi ? "Nước vừa đi: " : "Last Move: ") + std::string(hienNuocDiCuoi ? (isVi ? "[ BẬT ]" : "[ ON ]") : (isVi ? "[ TẮT ]" : "[ OFF ]")));
        settingsGameplayButtons[3]->setSelected(hienNuocDiCuoi);

        settingsGameplayButtons[4]->setLabel((isVi ? "Tọa độ bàn: " : "Coordinates: ") + std::string(hienToaDo ? (isVi ? "[ BẬT ]" : "[ ON ]") : (isVi ? "[ TẮT ]" : "[ OFF ]")));
        settingsGameplayButtons[4]->setSelected(hienToaDo);

        settingsGameplayButtons[5]->setLabel((isVi ? "Hiển thị: " : "Display: ") + std::string(fullscreenMode ? (isVi ? "< Toàn Màn Hình >" : "< Fullscreen >") : (isVi ? "< Cửa Sổ >" : "< Windowed >")));
        settingsGameplayButtons[5]->setSelected(fullscreenMode);
    }

    if (settingsLanguageButtons.size() >= 2) {
        settingsLanguageButtons[0]->setLabel(isVi ? "TIẾNG VIỆT" : "VIETNAMESE");
        settingsLanguageButtons[1]->setLabel(isVi ? "ENGLISH" : "ENGLISH");
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

    if (settingsControlButtons.size() >= 10) {
        settingsControlButtons[7]->setSelected(keyConfig.currentPreset == KeyPreset::WASD);
        settingsControlButtons[8]->setSelected(keyConfig.currentPreset == KeyPreset::ARROWS);
        settingsControlButtons[9]->setSelected(keyConfig.currentPreset == KeyPreset::IJKL);
    }

    if (settingsBottomButtons.size() >= 2) {
        settingsBottomButtons[0]->setLabel(Loc::get(LocKey::BTN_APPLY_SAVE));
        settingsBottomButtons[0]->setSelected(true);
        settingsBottomButtons[1]->setLabel(Loc::get(LocKey::BTN_RESTORE_DEFAULTS));
        settingsBottomButtons[1]->setSelected(false);
    }

    if (auto b = menuIntro.layButton(0)) {
        b->setLabel(Loc::get(LocKey::BACK));
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
    khoiTaoNutTrongGame();
    soundManager.play(SoundType::CLICK);
}

void GameManager::dongMenuPopUp() {
    hienMenuPopUp = false;
    soundManager.play(SoundType::CLICK);
}

void GameManager::moMenuCaiDat(TrangThai trangThaiTruoc) {
    trangThaiTruocSettings = trangThaiTruoc;
    trangThai = TrangThai::SETTINGS_MENU;
    settingsDetailOpen = false;
    settingsSlideProgress = 0.0f;
    rebindingAction = KeyAction::COUNT;
    khoiTaoDashboardSettings();
    capNhatViTriNutCaiDat();
}

void GameManager::capNhatCaiDat(float dt) {
    float target = settingsDetailOpen ? 1.0f : 0.0f;
    float speed = 5.2f;
    if (std::abs(settingsSlideProgress - target) > 0.001f) {
        if (settingsSlideProgress < target) {
            settingsSlideProgress = std::min(target, settingsSlideProgress + dt * speed);
        } else {
            settingsSlideProgress = std::max(target, settingsSlideProgress - dt * speed);
        }
        capNhatViTriNutCaiDat();
    } else {
        if (settingsSlideProgress != target) {
            settingsSlideProgress = target;
            capNhatViTriNutCaiDat();
        }
    }
}

void GameManager::capNhatViTriNutCaiDat() {
    float t = settingsSlideProgress * settingsSlideProgress * (3.0f - 2.0f * settingsSlideProgress);

    // Left/Center Panel Geometry (Symmetrical & Regal)
    float panelX = lerpVal(360.0f, 110.0f, t);
    float panelY = lerpVal(130.0f, 85.0f, t);
    float panelW = lerpVal(480.0f, 250.0f, t);
    float panelH = lerpVal(540.0f, 635.0f, t);

    // 4 Category Buttons
    float btnW = lerpVal(380.0f, 224.0f, t);
    float btnH = 58.0f;
    for (size_t i = 0; i < settingsTabButtons.size() && i < 4; ++i) {
        float bx = lerpVal(410.0f, 123.0f, t);
        float by = lerpVal(195.0f + i * 72.0f, 145.0f + i * 68.0f, t);
        settingsTabButtons[i]->setPosition(bx, by);
        settingsTabButtons[i]->setSize(btnW, btnH);
    }

    if (settingsCloseBtn) {
        settingsCloseBtn->setPosition(panelX + (panelW - 220.0f) / 2.0f, panelY + panelH - 62.0f);
        settingsCloseBtn->setSize(220.0f, 44.0f);
    }
    if (settingsCollapseBtn) {
        settingsCollapseBtn->setPosition(123.0f, 645.0f);
        settingsCollapseBtn->setSize(224.0f, 44.0f);
    }

    // Right Panel Geometry (X = 380, W = 710, Margin = 110 on both sides)
    float rightPanelX = lerpVal(1220.0f, 380.0f, t);
    float rightPanelY = 85.0f;
    float offX = rightPanelX - 380.0f;

    // Reposition controls inside right panel
    // Tab 1: Sound - Evenly distributed vertical spacing without monitor void
    float sW = 540.0f;
    float sX = 380.0f + (710.0f - sW) / 2.0f + offX; // 465.0f + offX
    if (sliderMaster) {
        sliderMaster->setPosition(sX, 180.0f);
        sliderMaster->setWidth(sW);
    }
    if (settingsAudioButtons.size() >= 2) {
        settingsAudioButtons[0]->setPosition(sX, 228.0f);
        settingsAudioButtons[0]->setSize(sW, 42.0f);
    }
    if (sliderSfx) {
        sliderSfx->setPosition(sX, 312.0f);
        sliderSfx->setWidth(sW);
    }
    if (settingsAudioButtons.size() >= 2) {
        settingsAudioButtons[1]->setPosition(sX, 360.0f);
        settingsAudioButtons[1]->setSize(sW, 42.0f);
    }
    if (sliderBgm) {
        sliderBgm->setPosition(sX, 444.0f);
        sliderBgm->setWidth(sW);
    }

    float sbY = 534.0f;
    float sbW = 80.0f;
    float sbGap = 12.0f;
    for (size_t i = 0; i < soundboardButtons.size(); ++i) {
        soundboardButtons[i]->setPosition(sX + i * (sbW + sbGap), sbY);
        soundboardButtons[i]->setSize(sbW, 44.0f);
    }

    // Tab 2: Gameplay (Balanced columns & proper clearances)
    float gpCol1X = 415.0f + offX;
    float gpCol2X = 735.0f + offX;
    float gpW = 310.0f;
    float gpH = 42.0f;
    float gpRow0 = 196.0f;
    float gpRow1 = 258.0f;
    float gpRow2 = 320.0f;
    float gpRow3 = 382.0f;
    if (settingsGameplayButtons.size() >= 4) {
        settingsGameplayButtons[0]->setPosition(gpCol1X, gpRow0);
        settingsGameplayButtons[0]->setSize(gpW, gpH);
        settingsGameplayButtons[1]->setPosition(gpCol1X, gpRow1);
        settingsGameplayButtons[1]->setSize(gpW, gpH);
        settingsGameplayButtons[2]->setPosition(gpCol1X, gpRow2);
        settingsGameplayButtons[2]->setSize(gpW, gpH);
        settingsGameplayButtons[3]->setPosition(gpCol1X, gpRow3);
        settingsGameplayButtons[3]->setSize(gpW, gpH);
    }
    if (settingsGameplayButtons.size() >= 6) {
        settingsGameplayButtons[4]->setPosition(gpCol2X, gpRow0);
        settingsGameplayButtons[4]->setSize(gpW, gpH);
        settingsGameplayButtons[5]->setPosition(gpCol2X, gpRow1);
        settingsGameplayButtons[5]->setSize(gpW, gpH);
    }

    // Tab 3: Language (Full-height Imperial Decree Parchment Cards)
    if (settingsLanguageButtons.size() >= 2) {
        float langCardW = 330.0f;
        float langCardH = 345.0f;
        float langCardY = 175.0f;
        settingsLanguageButtons[0]->setPosition(397.0f + offX, langCardY);
        settingsLanguageButtons[0]->setSize(langCardW, langCardH);
        settingsLanguageButtons[1]->setPosition(743.0f + offX, langCardY);
        settingsLanguageButtons[1]->setSize(langCardW, langCardH);
    }

    // Tab 4: Controls (3 Bamboo Presets + 7 Keybinding Slots)
    float pW = 98.0f;
    float pGap = 8.0f;
    float pX = 415.0f + offX;
    if (settingsControlButtons.size() >= 10) {
        settingsControlButtons[7]->setPosition(pX, 184.0f);
        settingsControlButtons[7]->setSize(pW, 40.0f);
        settingsControlButtons[8]->setPosition(pX + pW + pGap, 184.0f);
        settingsControlButtons[8]->setSize(pW, 40.0f);
        settingsControlButtons[9]->setPosition(pX + (pW + pGap) * 2.0f, 184.0f);
        settingsControlButtons[9]->setSize(pW, 40.0f);

        float kCol1X = 415.0f + offX;
        float kCol2X = 735.0f + offX;
        float kW = 310.0f;
        float kH = 40.0f;
        float kStep = 50.0f;
        for (int a = 0; a < 4; ++a) {
            settingsControlButtons[a]->setPosition(kCol1X, 274.0f + a * kStep);
            settingsControlButtons[a]->setSize(kW, kH);
        }
        for (int a = 4; a < 7; ++a) {
            settingsControlButtons[a]->setPosition(kCol2X, 274.0f + (a - 4) * kStep);
            settingsControlButtons[a]->setSize(kW, kH);
        }
    }

    // Bottom action buttons in right panel (Centered at bottom baseline Y = 645)
    if (settingsBottomButtons.size() >= 2) {
        float bY = 645.0f;
        float bW = 220.0f;
        float bGap = 20.0f;
        float bStartX = 380.0f + (710.0f - (2.0f * bW + bGap)) / 2.0f + offX;
        settingsBottomButtons[0]->setPosition(bStartX, bY);
        settingsBottomButtons[0]->setSize(bW, 44.0f);
        settingsBottomButtons[1]->setPosition(bStartX + bW + bGap, bY);
        settingsBottomButtons[1]->setSize(bW, 44.0f);
    }
}

void GameManager::khoiTaoDashboardSettings() {
    settingsTabButtons.clear();
    settingsAudioButtons.clear();
    settingsGraphicsButtons.clear();
    settingsGameplayButtons.clear();
    settingsLanguageButtons.clear();
    settingsControlButtons.clear();
    settingsBottomButtons.clear();
    soundboardButtons.clear();

    bool isVi = (Loc::getLanguage() == Language::TIENG_VIET);
    auto& mAssets = getMenuAssets();
    sf::Font& f = mAssets.vnFontLoaded ? mAssets.vnFont : font;
    sf::Font& fBold = mAssets.fontBoldLoaded ? mAssets.fontBold : fontBold;

    // 1. Four Category Tabs: Sound, Gameplay, Language, Controls
    settingsTabButtons.push_back(std::make_unique<Button>(0.0f, 0.0f, 380.0f, 58.0f, isVi ? "ÂM THANH" : "SOUND", fBold, [this]() {
        soundManager.play(SoundType::CLICK);
        activeSettingsTab = SettingsTab::SOUND;
        settingsDetailOpen = true;
        rebindingAction = KeyAction::COUNT;
    }));
    settingsTabButtons.push_back(std::make_unique<Button>(0.0f, 0.0f, 380.0f, 58.0f, isVi ? "LỐI CHƠI" : "GAMEPLAY", fBold, [this]() {
        soundManager.play(SoundType::CLICK);
        activeSettingsTab = SettingsTab::GAMEPLAY;
        settingsDetailOpen = true;
        rebindingAction = KeyAction::COUNT;
    }));
    settingsTabButtons.push_back(std::make_unique<Button>(0.0f, 0.0f, 380.0f, 58.0f, isVi ? "NGÔN NGỮ" : "LANGUAGE", fBold, [this]() {
        soundManager.play(SoundType::CLICK);
        activeSettingsTab = SettingsTab::LANGUAGE;
        settingsDetailOpen = true;
        rebindingAction = KeyAction::COUNT;
    }));
    settingsTabButtons.push_back(std::make_unique<Button>(0.0f, 0.0f, 380.0f, 58.0f, isVi ? "ĐIỀU KHIỂN" : "CONTROLS", fBold, [this]() {
        soundManager.play(SoundType::CLICK);
        activeSettingsTab = SettingsTab::CONTROLS;
        settingsDetailOpen = true;
        rebindingAction = KeyAction::COUNT;
    }));

    for (auto& btn : settingsTabButtons) {
        btn->setCharacterSize(16);
        btn->setColors(sf::Color(250, 235, 180), sf::Color(255, 245, 190), sf::Color(255, 250, 210));
    }

    // Close button (when centered popup)
    settingsCloseBtn = std::make_unique<Button>(0.0f, 0.0f, 220.0f, 44.0f, isVi ? "ĐÓNG" : "CLOSE", fBold, [this]() {
        soundManager.play(SoundType::CLICK);
        trangThai = trangThaiTruocSettings;
        if (trangThai == TrangThai::DANG_CHOI) {
            hienMenuPopUp = true;
        }
    });
    settingsCloseBtn->setCharacterSize(15);
    settingsCloseBtn->setColors(sf::Color(240, 220, 180), sf::Color(255, 240, 190), sf::Color(255, 250, 210));

    // Collapse button (when slid to left)
    settingsCollapseBtn = std::make_unique<Button>(0.0f, 0.0f, 224.0f, 44.0f, isVi ? "Quay lại" : "Back", fBold, [this]() {
        soundManager.play(SoundType::CLICK);
        settingsDetailOpen = false;
        rebindingAction = KeyAction::COUNT;
    });
    settingsCollapseBtn->setCharacterSize(15);
    settingsCollapseBtn->setColors(sf::Color(240, 220, 180), sf::Color(255, 240, 190), sf::Color(255, 250, 210));

    // 2. Sound Controls (Symmetrical 540px Width)
    const float audioCtrlW = 540.0f;
    sliderMaster = std::make_unique<Slider>(0.0f, 0.0f, audioCtrlW, 18.0f, 0.0f, 100.0f,
        soundManager.getMasterVolume(), Loc::get(LocKey::SETTING_MASTER_VOL), fBold,
        [this](float val) { soundManager.setMasterVolume(val); });

    settingsAudioButtons.push_back(std::make_unique<Button>(0.0f, 0.0f, audioCtrlW, 40.0f, "", fBold, [this]() {
        soundManager.setSfxBat(!soundManager.getSfxBat());
        soundManager.play(SoundType::CLICK);
        capNhatNhanNut();
    }));
    settingsAudioButtons.back()->setCharacterSize(14);

    sliderSfx = std::make_unique<Slider>(0.0f, 0.0f, audioCtrlW, 18.0f, 0.0f, 100.0f,
        soundManager.getSfxVolume(), Loc::get(LocKey::SETTING_SFX_VOL), fBold,
        [this](float val) { soundManager.setSfxVolume(val); });

    settingsAudioButtons.push_back(std::make_unique<Button>(0.0f, 0.0f, audioCtrlW, 40.0f, "", fBold, [this]() {
        soundManager.setBgmBat(!soundManager.getBgmBat());
        soundManager.play(SoundType::CLICK);
        capNhatNhanNut();
    }));
    settingsAudioButtons.back()->setCharacterSize(14);

    sliderBgm = std::make_unique<Slider>(0.0f, 0.0f, audioCtrlW, 18.0f, 0.0f, 100.0f,
        soundManager.getBgmVolume(), Loc::get(LocKey::SETTING_BGM_VOL), fBold,
        [this](float val) { soundManager.setBgmVolume(val); });

    // Soundboard Test Buttons (6 buttons evenly distributed across 540px)
    const float sbW = 80.0f;
    soundboardButtons.push_back(std::make_unique<Button>(0.0f, 0.0f, sbW, 40.0f, isVi ? "Nước Đi" : "Move", fBold, [this]() { soundManager.play(SoundType::MOVE); }));
    soundboardButtons.push_back(std::make_unique<Button>(0.0f, 0.0f, sbW, 40.0f, isVi ? "Ăn Quân" : "Capture", fBold, [this]() { soundManager.play(SoundType::CAPTURE); }));
    soundboardButtons.push_back(std::make_unique<Button>(0.0f, 0.0f, sbW, 40.0f, isVi ? "Chiếu" : "Check", fBold, [this]() { soundManager.play(SoundType::CHECK); }));
    soundboardButtons.push_back(std::make_unique<Button>(0.0f, 0.0f, sbW, 40.0f, isVi ? "Thắng" : "Win", fBold, [this]() { soundManager.play(SoundType::VICTORY); }));
    soundboardButtons.push_back(std::make_unique<Button>(0.0f, 0.0f, sbW, 40.0f, isVi ? "Đi Sai" : "Wrong", fBold, [this]() { soundManager.play(SoundType::WRONG); }));
    soundboardButtons.push_back(std::make_unique<Button>(0.0f, 0.0f, sbW, 40.0f, isVi ? "Nhấp" : "Click", fBold, [this]() { soundManager.play(SoundType::CLICK); }));
    for (auto& sb : soundboardButtons) sb->setCharacterSize(12);

    // 3. Gameplay Controls (7 buttons)
    settingsGameplayButtons.push_back(std::make_unique<Button>(0.0f, 0.0f, 315.0f, 42.0f, "", fBold, [this]() {
        aiThinkRealistic = !aiThinkRealistic;
        soundManager.play(SoundType::CLICK);
        capNhatNhanNut();
    }));
    settingsGameplayButtons.push_back(std::make_unique<Button>(0.0f, 0.0f, 315.0f, 42.0f, "", fBold, [this]() {
        checkAlarmSound = !checkAlarmSound;
        soundManager.play(SoundType::CLICK);
        capNhatNhanNut();
    }));
    settingsGameplayButtons.push_back(std::make_unique<Button>(0.0f, 0.0f, 315.0f, 42.0f, "", fBold, [this]() {
        hienGoiY = !hienGoiY;
        soundManager.play(SoundType::CLICK);
        capNhatNhanNut();
    }));
    settingsGameplayButtons.push_back(std::make_unique<Button>(0.0f, 0.0f, 315.0f, 42.0f, "", fBold, [this]() {
        hienNuocDiCuoi = !hienNuocDiCuoi;
        soundManager.play(SoundType::CLICK);
        capNhatNhanNut();
    }));
    settingsGameplayButtons.push_back(std::make_unique<Button>(0.0f, 0.0f, 315.0f, 42.0f, "", fBold, [this]() {
        hienToaDo = !hienToaDo;
        soundManager.play(SoundType::CLICK);
        capNhatNhanNut();
    }));
    settingsGameplayButtons.push_back(std::make_unique<Button>(0.0f, 0.0f, 315.0f, 42.0f, "", fBold, [this]() {
        fullscreenMode = !fullscreenMode;
        apDungCheDoManHinh();
        soundManager.play(SoundType::CLICK);
        capNhatNhanNut();
    }));
    for (auto& gb : settingsGameplayButtons) gb->setCharacterSize(13);

    // 4. Language Selection Buttons (2 cards)
    settingsLanguageButtons.push_back(std::make_unique<Button>(0.0f, 0.0f, 330.0f, 345.0f, "", fBold, [this]() {
        Loc::setLanguage(Language::TIENG_VIET);
        capNhatFontTheoNgonNgu();
        capNhatNhanNut();
        soundManager.play(SoundType::CLICK);
        hienToast("Đã chọn ngôn ngữ: Tiếng Việt");
    }));
    settingsLanguageButtons.push_back(std::make_unique<Button>(0.0f, 0.0f, 330.0f, 345.0f, "", fBold, [this]() {
        Loc::setLanguage(Language::ENGLISH);
        capNhatFontTheoNgonNgu();
        capNhatNhanNut();
        soundManager.play(SoundType::CLICK);
        hienToast("Language set to: English");
    }));

    // 5. Controls Tab (7 action keybinds + 3 presets)
    for (int a = 0; a < 7; ++a) {
        KeyAction act = static_cast<KeyAction>(a);
        settingsControlButtons.push_back(std::make_unique<Button>(0.0f, 0.0f, 315.0f, 38.0f, "", fBold, [this, act]() {
            rebindingAction = act;
            capNhatNhanNut();
            soundManager.play(SoundType::CLICK);
        }));
        settingsControlButtons.back()->setCharacterSize(13);
    }
    // Presets
    settingsControlButtons.push_back(std::make_unique<Button>(0.0f, 0.0f, 98.0f, 38.0f, "WASD", fBold, [this]() {
        keyConfig.setPreset(KeyPreset::WASD);
        rebindingAction = KeyAction::COUNT;
        capNhatNhanNut();
        soundManager.play(SoundType::CLICK);
    }));
    settingsControlButtons.push_back(std::make_unique<Button>(0.0f, 0.0f, 98.0f, 38.0f, "Arrows", fBold, [this]() {
        keyConfig.setPreset(KeyPreset::ARROWS);
        rebindingAction = KeyAction::COUNT;
        capNhatNhanNut();
        soundManager.play(SoundType::CLICK);
    }));
    settingsControlButtons.push_back(std::make_unique<Button>(0.0f, 0.0f, 98.0f, 38.0f, "IJKL", fBold, [this]() {
        keyConfig.setPreset(KeyPreset::IJKL);
        rebindingAction = KeyAction::COUNT;
        capNhatNhanNut();
        soundManager.play(SoundType::CLICK);
    }));
    for (size_t i = 7; i < settingsControlButtons.size(); ++i) {
        settingsControlButtons[i]->setCharacterSize(13);
    }

    // 6. Bottom action buttons in right panel
    settingsBottomButtons.push_back(std::make_unique<Button>(0.0f, 0.0f, 220.0f, 44.0f, Loc::get(LocKey::BTN_APPLY_SAVE), fBold, [this]() {
        luuCaiDat();
        soundManager.play(SoundType::VICTORY);
        hienToast((Loc::getLanguage() == Language::TIENG_VIET) ? "Đã lưu cài đặt!" : "Settings saved!");
    }));
    settingsBottomButtons.push_back(std::make_unique<Button>(0.0f, 0.0f, 220.0f, 44.0f, Loc::get(LocKey::BTN_RESTORE_DEFAULTS), fBold, [this]() {
        khoiPhucCaiDatMacDinh();
        soundManager.play(SoundType::CLICK);
        hienToast((Loc::getLanguage() == Language::TIENG_VIET) ? "Đã khôi phục mặc định." : "Restored defaults.");
    }));
    for (auto& btn : settingsBottomButtons) {
        btn->setCharacterSize(14);
        btn->setColors(sf::Color(240, 220, 180), sf::Color(255, 240, 190), sf::Color(255, 250, 210));
    }

    capNhatNhanNut();
    capNhatViTriNutCaiDat();
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

    // 2. Pause Modal Buttons (Modal centered at X=380, Y=222, W=440, H=356)
    sf::Font& fBtn = getMenuFont(vi, true);

    const float btnX = 425.0f;
    const float btnW = 350.0f;
    const float splitW = 170.0f;
    const float splitGap = 10.0f;
    const float splitX2 = btnX + splitW + splitGap; // 605.0f

    // Row 0 (Y=324): Large Primary Resume button (350 x 42px)
    auto btnResume = std::make_unique<Button>(btnX, 324.0f, btnW, 42.0f, Loc::get(LocKey::RESUME), fBtn, [this]() {
        dongMenuPopUp();
    });
    btnResume->setCharacterSize(15);
    btnResume->setColors(sf::Color(255, 235, 160), sf::Color(255, 255, 225), sf::Color(230, 195, 90));
    inGameMenuButtons.push_back(std::move(btnResume));

    // Row 1 (Y=374): New Game (170 x 38px) & Offer Draw (170 x 38px)
    auto btnNew = std::make_unique<Button>(btnX, 374.0f, splitW, 38.0f, Loc::get(LocKey::NEW_GAME), fBtn, [this]() {
        dongMenuPopUp();
        batDauTroChoiMoi(cheDoChoi);
    });
    btnNew->setCharacterSize(14);
    btnNew->setColors(sf::Color(250, 225, 155), sf::Color(255, 255, 220), sf::Color(220, 185, 90));
    inGameMenuButtons.push_back(std::move(btnNew));

    auto btnDrawOffer = std::make_unique<Button>(splitX2, 374.0f, splitW, 38.0f, Loc::get(LocKey::DRAW_OFFER), fBtn, [this]() {
        dongMenuPopUp();
        thucHienXinHoa();
    });
    btnDrawOffer->setCharacterSize(14);
    btnDrawOffer->setColors(sf::Color(250, 225, 155), sf::Color(255, 255, 220), sf::Color(220, 185, 90));
    inGameMenuButtons.push_back(std::move(btnDrawOffer));

    // Row 2 (Y=420): Save Game (170 x 38px) & Settings (170 x 38px)
    auto btnSave = std::make_unique<Button>(btnX, 420.0f, splitW, 38.0f, Loc::get(LocKey::SAVE_GAME), fBtn, [this]() {
        dongMenuPopUp();
        thucHienLuuGame();
    });
    btnSave->setCharacterSize(14);
    btnSave->setColors(sf::Color(250, 225, 155), sf::Color(255, 255, 220), sf::Color(220, 185, 90));
    inGameMenuButtons.push_back(std::move(btnSave));

    auto btnSettings = std::make_unique<Button>(splitX2, 420.0f, splitW, 38.0f, vi ? "Cài Đặt" : "Settings", fBtn, [this]() {
        dongMenuPopUp();
        moMenuCaiDat(TrangThai::DANG_CHOI);
        soundManager.play(SoundType::CLICK);
    });
    btnSettings->setCharacterSize(14);
    btnSettings->setColors(sf::Color(250, 225, 155), sf::Color(255, 255, 220), sf::Color(220, 185, 90));
    inGameMenuButtons.push_back(std::move(btnSettings));

    // Row 3 (Y=466): Main Menu (350 x 42px)
    auto btnQuit = std::make_unique<Button>(btnX, 466.0f, btnW, 42.0f, Loc::get(LocKey::BACK_MENU), fBtn, [this]() {
        dongMenuPopUp();
        matchMenuTuDangChoi = false;
        quayLaiMenu();
    });
    btnQuit->setCharacterSize(15);
    btnQuit->setColors(sf::Color(255, 210, 180), sf::Color(255, 240, 225), sf::Color(220, 140, 120));
    inGameMenuButtons.push_back(std::move(btnQuit));
}

void GameManager::khoiTaoNutPopup() {
    popupButtons.clear();
    const float dw = 600.0f;
    const float dh = 450.0f;
    const float dx = (1200.0f - dw) / 2.0f;
    const float dy = (800.0f - dh) / 2.0f;
    const float pw = 168.0f;
    const float ph = 44.0f;
    const float py = dy + dh - 64.0f;

    bool tiengViet = (Loc::getLanguage() == Language::TIENG_VIET);
    sf::Font& fBtn = getMenuFont(tiengViet, true);

    auto& assets = getMenuAssets();

    // 1. Play Again
    std::string againLabel = Loc::get(LocKey::PLAY_AGAIN) + " (Enter)";
    auto btnAgain = std::make_unique<Button>(dx + 25.0f, py, pw, ph, againLabel, fBtn, [this]() {
        soundManager.play(SoundType::CLICK);
        batDauTroChoiMoi(cheDoChoi);
    });
    btnAgain->setColors(sf::Color(35, 125, 75, 245), sf::Color(45, 165, 95, 255), sf::Color(25, 95, 55, 255));
    if (assets.iconPlay.getSize().x > 0) btnAgain->setIcon(&assets.iconPlay);
    popupButtons.push_back(std::move(btnAgain));

    // 2. View Board (Xem bàn cờ)
    std::string viewLabel = tiengViet ? "Xem bàn cờ (V)" : "View Board (V)";
    auto btnView = std::make_unique<Button>(dx + 216.0f, py, pw, ph, viewLabel, fBtn, [this]() {
        soundManager.play(SoundType::CLICK);
        xemBanCoSauKetThuc = true;
    });
    btnView->setColors(sf::Color(45, 95, 140, 245), sf::Color(60, 125, 180, 255), sf::Color(30, 65, 100, 255));
    popupButtons.push_back(std::move(btnView));

    // 3. Back to Main Menu
    std::string backLabel = Loc::get(LocKey::BACK_MENU) + " (ESC)";
    auto btnBack = std::make_unique<Button>(dx + 407.0f, py, pw, ph, backLabel, fBtn, [this]() {
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

    if (screenShakeTimer > 0.0f) {
        float shakeFactor = screenShakeIntensity * (screenShakeTimer / 0.45f);
        float sx = (static_cast<float>(rand() % 100) / 50.0f - 1.0f) * shakeFactor;
        float sy = (static_cast<float>(rand() % 100) / 50.0f - 1.0f) * shakeFactor;
        view.setCenter(baseWidth / 2.0f + sx, baseHeight / 2.0f + sy);
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
    defeatParticlesActive = false;
    defeatParticles.clear();
    screenShakeTimer = 0.0f;
    viTriQuanRung = sf::Vector2i(-1, -1);
    thoiGianRungQuan = 0.0f;
    khoiTaoNutTrongGame();
    
    thoiGianConDo = thoiGianVanDau;
    thoiGianConDen = thoiGianVanDau;
    dtClock.restart();
    
    aiDangSuyNghi = false;
    thoiGianChoAI = 0.0f;
    lyDoKetThuc = "";
    daPhatAmKetThuc = false;
    xemBanCoSauKetThuc = false;
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
        xemBanCoSauKetThuc = false;
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

        // Music track switching:
        // Dragon Dance - Loop when playing chess; Lotus Pond - Loop for everything else
        bool inMatch = (trangThai == TrangThai::DANG_CHOI) ||
                       (trangThai == TrangThai::SETTINGS_MENU && trangThaiTruocSettings == TrangThai::DANG_CHOI) ||
                       (trangThai == TrangThai::LOAD_MENU && trangThaiTruocLoad == TrangThai::DANG_CHOI);
        soundManager.switchToTrack(inMatch ? BgmTrack::MATCH : BgmTrack::MENU);
        
        if (trangThai == TrangThai::DANG_CHOI && !banCo.getKetThuc() && !hienMenuPopUp) {
            // Immediate checkmate or stalemate safeguard
            if (!banCo.coNuocDiHopLe(banCo.getLuotChoi())) {
                banCo.setKetThuc(true);
                Mau benThang = (banCo.getLuotChoi() == Mau::DO) ? Mau::DEN : Mau::DO;
                banCo.setNguoiThang(benThang);
                bool biChieu = banCo.kiemTraChieu(banCo.getLuotChoi()) || banCo.kiemTraChieuTuong();
                bool tiengViet = (Loc::getLanguage() == Language::TIENG_VIET);
                bool playerLostToAi = (cheDoChoi == CheDoChoi::VOI_MAY && benThang != aiPlayerSide);
                if (playerLostToAi) {
                    lyDoKetThuc = tiengViet ? "Máy AI đã chiếu bí bạn!" : "AI has checkmated you!";
                    soundManager.play(SoundType::DEFEAT);
                    khoiTaoDefeatParticles();
                    kichHoatRungManHinh(0.45f, 7.0f);
                } else if (biChieu) {
                    lyDoKetThuc = tiengViet ? "Chiếu bí đối phương!" : "Checkmate!";
                    soundManager.play(SoundType::VICTORY);
                    khoiTaoVictoryParticles();
                } else {
                    lyDoKetThuc = tiengViet ? "Đối phương hết nước đi (Khốn bí)!" : "Opponent has no legal moves (Stalemate)!";
                    soundManager.play(SoundType::VICTORY);
                    khoiTaoVictoryParticles();
                }
                khoiTaoNutPopup();
            } else {
                capNhatDongHo(dt);
                if (cheDoChoi == CheDoChoi::VOI_MAY) {
                    xuLyLuotAI(dt);
                }
            }
        }
        
        if (toastTimer > 0.0f) {
            toastTimer -= dt;
        }
        if (thoiGianNuocDiSai > 0.0f) {
            thoiGianNuocDiSai -= dt;
        }
        if (thoiGianRungQuan > 0.0f) {
            thoiGianRungQuan -= dt;
            if (thoiGianRungQuan < 0.0f) thoiGianRungQuan = 0.0f;
        }
        if (trangThai == TrangThai::RANDOM_LUOT) {
            capNhatRandomRoll(dt);
        }
        if (victoryParticlesActive) {
            capNhatVictoryParticles(dt);
        }
        if (defeatParticlesActive) {
            capNhatDefeatParticles(dt);
        }
        if (screenShakeTimer > 0.0f) {
            screenShakeTimer -= dt;
            if (screenShakeTimer < 0.0f) screenShakeTimer = 0.0f;
        }
        if (trangThai == TrangThai::SETTINGS_MENU) {
            capNhatCaiDat(dt);
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
    if (trangThai == TrangThai::TITLE_SCREEN) capNhatHoverTitleScreen(mousePos);
    else if (trangThai == TrangThai::MENU_CHINH) menuChinh.update(mousePos);
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
        if (!settingsDetailOpen || settingsSlideProgress < 0.2f) {
            if (settingsCloseBtn) settingsCloseBtn->update(mousePos);
        }
        if (settingsDetailOpen && settingsSlideProgress > 0.8f) {
            if (settingsCollapseBtn) settingsCollapseBtn->update(mousePos);
            for (auto& btn : settingsBottomButtons) btn->update(mousePos);
            if (activeSettingsTab == SettingsTab::SOUND) {
                for (auto& btn : settingsAudioButtons) btn->update(mousePos);
                for (auto& btn : soundboardButtons) btn->update(mousePos);
            } else if (activeSettingsTab == SettingsTab::GAMEPLAY) {
                for (auto& btn : settingsGameplayButtons) btn->update(mousePos);
            } else if (activeSettingsTab == SettingsTab::LANGUAGE) {
                for (auto& btn : settingsLanguageButtons) btn->update(mousePos);
            } else if (activeSettingsTab == SettingsTab::CONTROLS) {
                for (auto& btn : settingsControlButtons) btn->update(mousePos);
            }
        }
    }
    else if (trangThai == TrangThai::INTRODUCTION_MENU) menuIntro.update(mousePos);
    else if (trangThai == TrangThai::DANG_CHOI) {
        if (banCo.getKetThuc()) {
            if (!xemBanCoSauKetThuc) {
                for (auto& btn : popupButtons) btn->update(mousePos);
            }
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
            if (settingsDetailOpen && settingsSlideProgress > 0.8f) {
                if (activeSettingsTab == SettingsTab::SOUND) {
                    if (sliderMaster) sliderMaster->handleEvent(event, mousePos);
                    if (sliderSfx) sliderSfx->handleEvent(event, mousePos);
                    if (sliderBgm) sliderBgm->handleEvent(event, mousePos);
                }
            }
        }
        
        if (event.type == sf::Event::MouseMoved) {
            sf::Vector2f mc = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
            mousePos = sf::Vector2i(static_cast<int>(std::round(mc.x)), static_cast<int>(std::round(mc.y)));
            if (trangThai == TrangThai::TITLE_SCREEN) capNhatHoverTitleScreen(mousePos);
            else if (trangThai == TrangThai::MENU_CHINH) menuChinh.update(mousePos);
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
                if (!settingsDetailOpen || settingsSlideProgress < 0.2f) {
                    if (settingsCloseBtn) settingsCloseBtn->update(mousePos);
                }
                if (settingsDetailOpen && settingsSlideProgress > 0.8f) {
                    if (settingsCollapseBtn) settingsCollapseBtn->update(mousePos);
                    for (auto& btn : settingsBottomButtons) btn->update(mousePos);
                    if (activeSettingsTab == SettingsTab::SOUND) {
                        for (auto& btn : settingsAudioButtons) btn->update(mousePos);
                        for (auto& btn : soundboardButtons) btn->update(mousePos);
                    } else if (activeSettingsTab == SettingsTab::GAMEPLAY) {
                        for (auto& btn : settingsGameplayButtons) btn->update(mousePos);
                    } else if (activeSettingsTab == SettingsTab::LANGUAGE) {
                        for (auto& btn : settingsLanguageButtons) btn->update(mousePos);
                    } else if (activeSettingsTab == SettingsTab::CONTROLS) {
                        for (auto& btn : settingsControlButtons) btn->update(mousePos);
                    }
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
    if (trangThai == TrangThai::TITLE_SCREEN) {
        xuLyClickTitleScreen(viTri);
    }
    else if (trangThai == TrangThai::MENU_CHINH) {
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
        for (auto& btn : settingsTabButtons) {
            if (btn->contains(viTri)) {
                btn->handleClick(viTri);
                return;
            }
        }
        if (!settingsDetailOpen || settingsSlideProgress < 0.2f) {
            if (settingsCloseBtn && settingsCloseBtn->contains(viTri)) {
                settingsCloseBtn->handleClick(viTri);
                return;
            }
        }
        if (settingsDetailOpen && settingsSlideProgress > 0.8f) {
            if (settingsCollapseBtn && settingsCollapseBtn->contains(viTri)) {
                settingsCollapseBtn->handleClick(viTri);
                return;
            }
            for (auto& btn : settingsBottomButtons) {
                if (btn->contains(viTri)) {
                    btn->handleClick(viTri);
                    return;
                }
            }
            if (activeSettingsTab == SettingsTab::SOUND) {
                for (auto& btn : settingsAudioButtons) {
                    if (btn->contains(viTri)) { btn->handleClick(viTri); return; }
                }
                for (auto& btn : soundboardButtons) {
                    if (btn->contains(viTri)) { btn->handleClick(viTri); return; }
                }
            } else if (activeSettingsTab == SettingsTab::GAMEPLAY) {
                for (auto& btn : settingsGameplayButtons) {
                    if (btn->contains(viTri)) { btn->handleClick(viTri); return; }
                }
            } else if (activeSettingsTab == SettingsTab::LANGUAGE) {
                for (auto& btn : settingsLanguageButtons) {
                    if (btn->contains(viTri)) { btn->handleClick(viTri); return; }
                }
            } else if (activeSettingsTab == SettingsTab::CONTROLS) {
                for (auto& btn : settingsControlButtons) {
                    if (btn->contains(viTri)) { btn->handleClick(viTri); return; }
                }
            }
        }
    }
    else if (trangThai == TrangThai::INTRODUCTION_MENU) {
        menuIntro.handleClick(viTri);
    }
    else if (trangThai == TrangThai::DANG_CHOI) {
        if (banCo.getKetThuc()) {
            if (xemBanCoSauKetThuc) {
                xemBanCoSauKetThuc = false;
                soundManager.play(SoundType::CLICK);
                return;
            }
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
            bool clickedInside = false;
            for (auto& btn : inGameMenuButtons) {
                if (btn->contains(viTri)) {
                    btn->handleClick(viTri);
                    clickedInside = true;
                    break;
                }
            }
            if (!clickedInside) {
                sf::FloatRect modalRect(380.0f, 222.0f, 440.0f, 356.0f);
                if (!modalRect.contains(static_cast<float>(viTri.x), static_cast<float>(viTri.y))) {
                    dongMenuPopUp();
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

    if (trangThai == TrangThai::TITLE_SCREEN) {
        if (key == sf::Keyboard::M) {
            soundManager.setBgmBat(!soundManager.getBgmBat());
            soundManager.play(SoundType::CLICK);
            luuCaiDat();
            bool vi = (Loc::getLanguage() == Language::TIENG_VIET);
            hienToast(soundManager.getBgmBat() ? (vi ? "Nhạc nền: BẬT" : "BGM: ON") 
                                               : (vi ? "Nhạc nền: TẮT" : "BGM: OFF"));
            return;
        }
        if (key == sf::Keyboard::I) {
            soundManager.play(SoundType::CLICK);
            trangThaiTruocIntro = TrangThai::TITLE_SCREEN;
            trangThai = TrangThai::INTRODUCTION_MENU;
            return;
        }

        if (titleSubMenu == TitleSubMenu::MAIN_START) {
            if (key == sf::Keyboard::S) {
                soundManager.play(SoundType::CLICK);
                moMenuCaiDat(TrangThai::TITLE_SCREEN);
                return;
            }
            if (key == sf::Keyboard::Return || key == sf::Keyboard::Space) {
                soundManager.play(SoundType::CLICK);
                titleSubMenu = TitleSubMenu::PLAY_LOAD;
                titleMenuIndex = 0;
                return;
            }
            if (key == sf::Keyboard::Escape) {
                soundManager.play(SoundType::CLICK);
                luuCaiDat();
                window.close();
                return;
            }
        } else if (titleSubMenu == TitleSubMenu::PLAY_LOAD) {
            if (key == sf::Keyboard::W || key == sf::Keyboard::Up) {
                soundManager.play(SoundType::CLICK);
                titleMenuIndex = (titleMenuIndex + 2) % 3;
                return;
            }
            if (key == sf::Keyboard::S || key == sf::Keyboard::Down) {
                soundManager.play(SoundType::CLICK);
                titleMenuIndex = (titleMenuIndex + 1) % 3;
                return;
            }
            if (key == sf::Keyboard::Return || key == sf::Keyboard::Space) {
                soundManager.play(SoundType::CLICK);
                if (titleMenuIndex == 0) {
                    titleSubMenu = TitleSubMenu::MODE_SELECT;
                    titleMenuIndex = 0;
                } else if (titleMenuIndex == 1) {
                    trangThaiTruocLoad = TrangThai::TITLE_SCREEN;
                    capNhatThongTinSlots();
                    trangThai = TrangThai::LOAD_MENU;
                } else {
                    titleSubMenu = TitleSubMenu::MAIN_START;
                    titleMenuIndex = 0;
                }
                return;
            }
            if (key == sf::Keyboard::Escape) {
                soundManager.play(SoundType::CLICK);
                titleSubMenu = TitleSubMenu::MAIN_START;
                titleMenuIndex = 0;
                return;
            }
        } else if (titleSubMenu == TitleSubMenu::MODE_SELECT) {
            if (key == sf::Keyboard::W || key == sf::Keyboard::Up) {
                soundManager.play(SoundType::CLICK);
                titleMenuIndex = (titleMenuIndex + 2) % 3;
                return;
            }
            if (key == sf::Keyboard::S || key == sf::Keyboard::Down) {
                soundManager.play(SoundType::CLICK);
                titleMenuIndex = (titleMenuIndex + 1) % 3;
                return;
            }
            if (key == sf::Keyboard::Return || key == sf::Keyboard::Space) {
                soundManager.play(SoundType::CLICK);
                if (titleMenuIndex == 0) {
                    pvpRollBanner = "";
                    pvpSetupStep = 1;
                    playSetupMode = 1;
                    khoiTaoPlaySetupButtons();
                    trangThai = TrangThai::PVP_SETUP;
                } else if (titleMenuIndex == 1) {
                    aiRollBanner = "";
                    playSetupMode = 0;
                    khoiTaoPlaySetupButtons();
                    trangThai = TrangThai::PVAI_SETUP;
                } else {
                    titleSubMenu = TitleSubMenu::PLAY_LOAD;
                    titleMenuIndex = 0;
                }
                return;
            }
            if (key == sf::Keyboard::Escape) {
                soundManager.play(SoundType::CLICK);
                titleSubMenu = TitleSubMenu::PLAY_LOAD;
                titleMenuIndex = 0;
                return;
            }
        }
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
        if (key == sf::Keyboard::M) {
            soundManager.setBgmBat(!soundManager.getBgmBat());
            soundManager.play(SoundType::CLICK);
            luuCaiDat();
            bool vi = (Loc::getLanguage() == Language::TIENG_VIET);
            hienToast(soundManager.getBgmBat() ? (vi ? "Nhạc nền: BẬT" : "BGM: ON") 
                                               : (vi ? "Nhạc nền: TẮT" : "BGM: OFF"));
            menuChinh.capNhatNutTrangPhai();
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
            } else {
                trangThai = TrangThai::TITLE_SCREEN;
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

        if (key == sf::Keyboard::W || key == sf::Keyboard::Up || key == sf::Keyboard::Tab) {
            soundManager.play(SoundType::CLICK);
            playSetupMode = (playSetupMode == 1) ? 0 : 1;
            khoiTaoPlaySetupButtons();
            return;
        }
        if (key == sf::Keyboard::S || key == sf::Keyboard::Down) {
            soundManager.play(SoundType::CLICK);
            playSetupMode = (playSetupMode == 1) ? 0 : 1;
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
            trangThai = TrangThai::TITLE_SCREEN;
            titleSubMenu = TitleSubMenu::MODE_SELECT;
            titleMenuIndex = (playSetupMode == 1 ? 0 : 1);
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
        if (trangThai == TrangThai::PLAY_SUBMENU) {
            rebindingAction = KeyAction::COUNT;
            trangThai = TrangThai::MENU_CHINH;
            return;
        }
        if (trangThai == TrangThai::INTRODUCTION_MENU) {
            rebindingAction = KeyAction::COUNT;
            trangThai = trangThaiTruocIntro;
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
            if (settingsDetailOpen) {
                settingsDetailOpen = false;
                soundManager.play(SoundType::CLICK);
            } else {
                trangThai = trangThaiTruocSettings;
                if (trangThai == TrangThai::DANG_CHOI) {
                    hienMenuPopUp = true;
                }
            }
            return;
        }
        if (trangThai == TrangThai::PVP_SETUP || trangThai == TrangThai::PVAI_SETUP) {
            soundManager.play(SoundType::CLICK);
            trangThai = TrangThai::TITLE_SCREEN;
            titleSubMenu = TitleSubMenu::MODE_SELECT;
            titleMenuIndex = 0;
            return;
        }
        if (trangThai == TrangThai::DANG_CHOI) {
            if (banCo.getKetThuc()) {
                if (xemBanCoSauKetThuc) {
                    xemBanCoSauKetThuc = false;
                    soundManager.play(SoundType::CLICK);
                    return;
                }
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
            if (xemBanCoSauKetThuc) {
                xemBanCoSauKetThuc = false;
                soundManager.play(SoundType::CLICK);
                return;
            }
            if (key == sf::Keyboard::V) {
                xemBanCoSauKetThuc = true;
                soundManager.play(SoundType::CLICK);
                return;
            }
            if (key == sf::Keyboard::Return || key == sf::Keyboard::Space) {
                soundManager.play(SoundType::CLICK);
                batDauTroChoiMoi(cheDoChoi);
                return;
            }
            return;
        }
        if (hienMenuPopUp) {
            if (key == sf::Keyboard::Return || key == sf::Keyboard::Space) {
                dongMenuPopUp();
                return;
            }
            return;
        }

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
            bool playerLostToAi = (cheDoChoi == CheDoChoi::VOI_MAY && aiPlayerSide == Mau::DO);
            bool vi = (Loc::getLanguage() == Language::TIENG_VIET);
            lyDoKetThuc = vi ? (playerLostToAi ? "Bạn đã hết thời gian suy nghĩ!" : "Quân Đỏ hết giờ!")
                             : (playerLostToAi ? "You ran out of time!" : "Red time out!");
            if (playerLostToAi) {
                soundManager.play(SoundType::DEFEAT);
                khoiTaoDefeatParticles();
                kichHoatRungManHinh(0.4f, 6.0f);
            } else {
                soundManager.play(SoundType::VICTORY);
                khoiTaoVictoryParticles();
            }
            khoiTaoNutPopup();
        }
    } else {
        thoiGianConDen -= dt;
        if (thoiGianConDen <= 0.0f) {
            thoiGianConDen = 0.0f;
            banCo.setKetThuc(true);
            banCo.setNguoiThang(Mau::DO);
            bool playerLostToAi = (cheDoChoi == CheDoChoi::VOI_MAY && aiPlayerSide == Mau::DEN);
            bool vi = (Loc::getLanguage() == Language::TIENG_VIET);
            lyDoKetThuc = vi ? (playerLostToAi ? "Bạn đã hết thời gian suy nghĩ!" : "Quân Đen hết giờ!")
                             : (playerLostToAi ? "You ran out of time!" : "Black time out!");
            if (playerLostToAi) {
                soundManager.play(SoundType::DEFEAT);
                khoiTaoDefeatParticles();
                kichHoatRungManHinh(0.4f, 6.0f);
            } else {
                soundManager.play(SoundType::VICTORY);
                khoiTaoVictoryParticles();
            }
            khoiTaoNutPopup();
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

                bool biChieu = banCo.kiemTraChieu(banCo.getLuotChoi()) || banCo.kiemTraChieuTuong();
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
                        khoiTaoDefeatParticles();
                        kichHoatRungManHinh(0.45f, 7.0f);
                    }
                    khoiTaoNutPopup();
                } else if (biChieu) {
                    if (checkAlarmSound) soundManager.play(SoundType::CHECK);
                }
            }
        } else {
            banCo.setKetThuc(true);
            banCo.setNguoiThang(aiPlayerSide);
            if (banCo.kiemTraChieu(aiMau) || banCo.kiemTraChieuTuong()) {
                lyDoKetThuc = (Loc::getLanguage() == Language::TIENG_VIET)
                    ? "Bạn đã chiếu bí máy AI!"
                    : "You checkmated the AI!";
            } else {
                lyDoKetThuc = (Loc::getLanguage() == Language::TIENG_VIET)
                    ? "Máy AI hết nước đi (Khốn bí)!"
                    : "AI has no legal moves (Stalemate)!";
            }
            soundManager.play(SoundType::VICTORY);
            khoiTaoVictoryParticles();
            khoiTaoNutPopup();
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
            thoiGianNuocDiSai = 1.2f;
            lyDoNuocDiSai = (Loc::getLanguage() == Language::TIENG_VIET)
                ? "Không thể chọn quân của đối phương!"
                : "Cannot select opponent's piece!";
            viTriQuanRung = sf::Vector2i(hang, cot);
            thoiGianRungQuan = 0.35f;
            soundManager.play(SoundType::WRONG);
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

            bool biChieu = banCo.kiemTraChieu(banCo.getLuotChoi()) || banCo.kiemTraChieuTuong();
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
                    bool biChieuMauThua = banCo.kiemTraChieu(banCo.getLuotChoi()) || banCo.kiemTraChieuTuong();
                    bool playerLostToAi = (cheDoChoi == CheDoChoi::VOI_MAY && banCo.getNguoiThang() != aiPlayerSide);
                    if (playerLostToAi) {
                        lyDoKetThuc = (Loc::getLanguage() == Language::TIENG_VIET)
                            ? "Máy AI đã chiếu bí bạn!"
                            : "AI checkmated you!";
                        soundManager.play(SoundType::DEFEAT);
                        khoiTaoDefeatParticles();
                        kichHoatRungManHinh(0.45f, 7.0f);
                    } else if (biChieuMauThua) {
                        lyDoKetThuc = (Loc::getLanguage() == Language::TIENG_VIET)
                            ? "Chiếu bí đối phương!"
                            : "Checkmate!";
                        soundManager.play(SoundType::VICTORY);
                        khoiTaoVictoryParticles();
                    } else {
                        lyDoKetThuc = (Loc::getLanguage() == Language::TIENG_VIET)
                            ? "Đối phương hết nước đi (Khốn bí)!"
                            : "Opponent has no legal moves (Stalemate)!";
                        soundManager.play(SoundType::VICTORY);
                        khoiTaoVictoryParticles();
                    }
                }
                khoiTaoNutPopup();
            } else if (biChieu) {
                if (checkAlarmSound) soundManager.play(SoundType::CHECK);
            }
        } else {
            viTriNuocDiSai = sf::Vector2i(hang, cot);
            thoiGianNuocDiSai = 1.3f;
            lyDoNuocDiSai = (Loc::getLanguage() == Language::TIENG_VIET)
                ? "Nước đi không hợp lệ!"
                : "Illegal move!";
            if (quanDangChon) {
                viTriQuanRung = sf::Vector2i(quanDangChon->getHang(), quanDangChon->getCot());
                thoiGianRungQuan = 0.35f;
            }
            soundManager.play(SoundType::WRONG);
        }
    } else {
        std::string lyDo = xacDinhLyDoNuocDiSai(quanDangChon, hang, cot);
        viTriNuocDiSai = sf::Vector2i(hang, cot);
        thoiGianNuocDiSai = 1.3f;
        lyDoNuocDiSai = lyDo;
        if (quanDangChon) {
            viTriQuanRung = sf::Vector2i(quanDangChon->getHang(), quanDangChon->getCot());
            thoiGianRungQuan = 0.35f;
        }
        soundManager.play(SoundType::WRONG);
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
    bool playerLostToAi = (cheDoChoi == CheDoChoi::VOI_MAY && mau == aiPlayerSide);
    if (playerLostToAi) {
        soundManager.play(SoundType::DEFEAT);
        khoiTaoDefeatParticles();
        kichHoatRungManHinh(0.35f, 5.0f);
    } else {
        soundManager.play(SoundType::VICTORY);
        khoiTaoVictoryParticles();
    }
    khoiTaoNutPopup();
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
    khoiTaoNutPopup();
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
    trangThai = TrangThai::TITLE_SCREEN;
    titleSubMenu = TitleSubMenu::MODE_SELECT;
    titleMenuIndex = 0;
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
            const sf::Texture& titleBg = AssetManager::get().texture(TextureId::TITLE_BACKGROUND);
            if (titleBg.getSize().x > 0) {
                sf::Sprite bgSprite(titleBg);
                float tw = static_cast<float>(titleBg.getSize().x);
                float th = static_cast<float>(titleBg.getSize().y);
                float coverScale = std::max(winW / tw, winH / th);
                bgSprite.setOrigin(tw / 2.0f, th / 2.0f);
                bgSprite.setPosition(winW / 2.0f, winH / 2.0f);
                bgSprite.setScale(coverScale, coverScale);
                window.draw(bgSprite);
            }
        } else if (trangThai == TrangThai::TITLE_SCREEN ||
                   trangThai == TrangThai::PVP_SETUP ||
                   trangThai == TrangThai::PVAI_SETUP ||
                   trangThai == TrangThai::RANDOM_LUOT ||
                   trangThai == TrangThai::SETTINGS_MENU ||
                   trangThai == TrangThai::INTRODUCTION_MENU ||
                   trangThai == TrangThai::LOAD_MENU) {
            const sf::Texture& titleBg = AssetManager::get().texture(TextureId::TITLE_BACKGROUND);
            if (titleBg.getSize().x > 0) {
                sf::Sprite bgSprite(titleBg);
                float tw = static_cast<float>(titleBg.getSize().x);
                float th = static_cast<float>(titleBg.getSize().y);
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
    if (trangThai == TrangThai::TITLE_SCREEN ||
        trangThai == TrangThai::PVP_SETUP ||
        trangThai == TrangThai::PVAI_SETUP ||
        trangThai == TrangThai::RANDOM_LUOT ||
        trangThai == TrangThai::SETTINGS_MENU ||
        trangThai == TrangThai::INTRODUCTION_MENU ||
        trangThai == TrangThai::LOAD_MENU ||
        trangThai == TrangThai::DANG_CHOI) {
        const sf::Texture& titleBg = AssetManager::get().texture(TextureId::TITLE_BACKGROUND);
        if (titleBg.getSize().x > 0) {
            sf::Sprite bgSprite(titleBg);
            float tw = static_cast<float>(titleBg.getSize().x);
            float th = static_cast<float>(titleBg.getSize().y);
            float scale = std::max(1200.0f / tw, 800.0f / th);
            bgSprite.setOrigin(tw / 2.0f, th / 2.0f);
            bgSprite.setPosition(600.0f, 400.0f);
            bgSprite.setScale(scale, scale);
            window.draw(bgSprite);
        }
        if (trangThai == TrangThai::DANG_CHOI) {
            FantasyUI::get().drawFullDimOverlay(window, sf::Color(10, 8, 12, 120));
        }
    } else {
        atmosphere.draw(window);
    }

    if (trangThai == TrangThai::TITLE_SCREEN) {
        veTitleScreen();
    }
    else if (trangThai == TrangThai::MENU_CHINH) {
        if (menuChinh.isClosed() && !matchMenuTuDangChoi) {
            trangThai = TrangThai::TITLE_SCREEN;
            veTitleScreen();
        } else {
            veMenuChinh();
        }
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
            veDefeatParticles();
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
        if (thoiGianRungQuan > 0.0f && viTriQuanRung.x == quanDangChon->getHang() && viTriQuanRung.y == quanDangChon->getCot()) {
            float shakeOffset = std::sin(thoiGianRungQuan * 48.0f) * 7.0f * (thoiGianRungQuan / 0.35f);
            pos.x += shakeOffset;
        }
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

    // Helper lambdas for check and checkmate rendering
    bool tiengViet = (Loc::getLanguage() == Language::TIENG_VIET);
    float pulse = 0.5f + 0.5f * std::sin(bannerPulseClock.getElapsedTime().asSeconds() * 8.0f);

    auto veTiaChieu = [&](sf::Vector2f pFrom, sf::Vector2f pTo, bool isCheckmate) {
        sf::Vector2f diff = pTo - pFrom;
        float dist = std::hypot(diff.x, diff.y);
        if (dist <= 30.0f) return;

        sf::Vector2f dir = diff / dist;
        sf::Vector2f normal(-dir.y, dir.x);
        sf::Vector2f pStart = pFrom + dir * 29.0f;
        sf::Vector2f pEnd = pTo - dir * 29.0f;
        float beamLen = dist - 58.0f;
        if (beamLen <= 2.0f) return;

        float angleDeg = std::atan2(dir.y, dir.x) * 180.0f / 3.14159265f;

        // 1. Outer radiant glow beam
        float beamThick = isCheckmate ? 5.5f : 4.2f;
        sf::RectangleShape outerBeam(sf::Vector2f(beamLen, beamThick));
        outerBeam.setOrigin(0.0f, beamThick / 2.0f);
        outerBeam.setPosition(pStart);
        outerBeam.setRotation(angleDeg);
        sf::Color outerCol = isCheckmate ? sf::Color(255, 60, 30, static_cast<sf::Uint8>(170 + pulse * 75))
                                         : sf::Color(255, 110, 30, static_cast<sf::Uint8>(150 + pulse * 65));
        outerBeam.setFillColor(outerCol);
        window.draw(outerBeam);

        // 2. Inner blazing core line
        sf::RectangleShape innerBeam(sf::Vector2f(beamLen, 2.0f));
        innerBeam.setOrigin(0.0f, 1.0f);
        innerBeam.setPosition(pStart);
        innerBeam.setRotation(angleDeg);
        innerBeam.setFillColor(sf::Color(255, 245, 170, 245));
        window.draw(innerBeam);

        // 3. Arrowhead pointing at the General
        sf::ConvexShape arrow(3);
        arrow.setPoint(0, pEnd);
        arrow.setPoint(1, pEnd - dir * 14.0f + normal * 7.5f);
        arrow.setPoint(2, pEnd - dir * 14.0f - normal * 7.5f);
        arrow.setFillColor(isCheckmate ? sf::Color(255, 220, 80) : sf::Color(255, 180, 60));
        arrow.setOutlineThickness(1.5f);
        arrow.setOutlineColor(isCheckmate ? sf::Color(255, 40, 30) : sf::Color(210, 50, 20));
        window.draw(arrow);

        // 4. Energy pulse particle travelling continuously along the beam
        float tPulse = std::fmod(bannerPulseClock.getElapsedTime().asSeconds() * 2.2f, 1.0f);
        sf::Vector2f dotPos = pStart + dir * (beamLen * tPulse);
        sf::CircleShape dot(isCheckmate ? 4.5f : 3.8f);
        dot.setOrigin(dot.getRadius(), dot.getRadius());
        dot.setPosition(dotPos);
        dot.setFillColor(sf::Color(255, 255, 230, 250));
        window.draw(dot);
    };

    auto veHuyHieuQuan = [&](sf::Vector2f pos, const std::string& text, sf::Color bgColor, sf::Color outlineColor, sf::Color textColor, bool placeAbove) {
        sf::Font& f = getMenuFont(tiengViet, true);
        sf::Text t;
        t.setFont(f);
        t.setCharacterSize(11);
        t.setStyle(sf::Text::Bold);
        t.setString(toUtf8(text));
        t.setFillColor(textColor);
        sf::FloatRect tb = t.getLocalBounds();

        float pw = std::max(tb.width + 14.0f, 68.0f);
        float ph = 18.0f;
        float py = placeAbove ? (pos.y - 37.0f) : (pos.y + 37.0f);

        sf::RectangleShape pill(sf::Vector2f(pw, ph));
        pill.setOrigin(pw / 2.0f, ph / 2.0f);
        pill.setPosition(pos.x, py);
        pill.setFillColor(bgColor);
        pill.setOutlineThickness(1.2f);
        pill.setOutlineColor(outlineColor);
        window.draw(pill);

        t.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));
        t.setPosition(std::round(pos.x), std::round(py));
        window.draw(t);
    };

    // 4. King under CHECK Visual Effect (Dramatic pulsing aura, shockwave, checking pieces & attack beams)
    if (!banCo.getKetThuc() && (banCo.kiemTraChieu(banCo.getLuotChoi()) || banCo.kiemTraChieuTuong())) {
        std::shared_ptr<QuanCo> tuongBiChieu = nullptr;
        for (const auto& q : banCo.getCacQuan()) {
            if (q && !q->getDaBiAn() && q->getMau() == banCo.getLuotChoi() && q->laTuong()) {
                tuongBiChieu = q;
                break;
            }
        }

        if (tuongBiChieu) {
            sf::Vector2f kPos = chuyenDoiToaDoBanCoThanhManHinh(tuongBiChieu->getHang(), tuongBiChieu->getCot());

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

            // Highlight all pieces actively delivering check
            auto danhSachChieu = banCo.timCacQuanDangChieu(banCo.getLuotChoi());
            for (const auto& qAtk : danhSachChieu) {
                if (!qAtk || qAtk->getDaBiAn()) continue;
                sf::Vector2f atkPos = chuyenDoiToaDoBanCoThanhManHinh(qAtk->getHang(), qAtk->getCot());

                // Pulsing orange-gold aura on checking piece
                float aRingR = 32.0f + pulse * 4.0f;
                sf::CircleShape atkRing(aRingR);
                atkRing.setOrigin(aRingR, aRingR);
                atkRing.setPosition(atkPos);
                atkRing.setFillColor(sf::Color(255, 140, 20, static_cast<sf::Uint8>(50 + pulse * 60)));
                atkRing.setOutlineThickness(2.2f);
                atkRing.setOutlineColor(sf::Color(255, 185, 40, static_cast<sf::Uint8>(190 + pulse * 65)));
                window.draw(atkRing);

                ve4GocLuaChon(window, sf::Vector2f(atkPos.x - 30.0f, atkPos.y - 30.0f), sf::Vector2f(60.0f, 60.0f), 1.0f, 1.1f);
            }

            // Check if king has no legal escape moves
            bool tuongCoNuocChay = false;
            for (int r = 0; r < BanCo::getSOHANG(); ++r) {
                for (int c = 0; c < BanCo::getSOCOT(); ++c) {
                    if (banCo.kiemTraNuocDiHopLe(tuongBiChieu->getHang(), tuongBiChieu->getCot(), r, c)) {
                        tuongCoNuocChay = true;
                        break;
                    }
                }
                if (tuongCoNuocChay) break;
            }

            // If king cannot move and move hints are ON: highlight friendly pieces that can rescue the king!
            if (!tuongCoNuocChay && hienGoiY) {
                for (const auto& qSave : banCo.getCacQuan()) {
                    if (qSave && !qSave->getDaBiAn() && qSave->getMau() == banCo.getLuotChoi() && !qSave->laTuong()) {
                        bool coNuocGiai = false;
                        for (int r = 0; r < BanCo::getSOHANG(); ++r) {
                            for (int c = 0; c < BanCo::getSOCOT(); ++c) {
                                if (banCo.kiemTraNuocDiHopLe(qSave->getHang(), qSave->getCot(), r, c)) {
                                    coNuocGiai = true;
                                    break;
                                }
                            }
                            if (coNuocGiai) break;
                        }

                        if (coNuocGiai) {
                            sf::Vector2f savePos = chuyenDoiToaDoBanCoThanhManHinh(qSave->getHang(), qSave->getCot());
                            sf::CircleShape saveRing(31.0f);
                            saveRing.setOrigin(31.0f, 31.0f);
                            saveRing.setPosition(savePos);
                            saveRing.setFillColor(sf::Color(40, 200, 110, static_cast<sf::Uint8>(40 + pulse * 45)));
                            saveRing.setOutlineThickness(2.0f);
                            saveRing.setOutlineColor(sf::Color(65, 235, 140, static_cast<sf::Uint8>(170 + pulse * 70)));
                            window.draw(saveRing);
                        }
                    }
                }
            }
        }
    }

    // 5. Post-Match CHECKMATE / VICTORY Highlights:
    // Highlight the checkmating pieces, the defeated General, and attack trajectories
    if (banCo.getKetThuc() && banCo.getNguoiThang() != Mau::HOA) {
        Mau mauThang = banCo.getNguoiThang();
        Mau mauThua = (mauThang == Mau::DO) ? Mau::DEN : Mau::DO;

        std::shared_ptr<QuanCo> tuongThua = nullptr;
        for (const auto& q : banCo.getCacQuan()) {
            if (q && !q->getDaBiAn() && q->getMau() == mauThua && q->laTuong()) {
                tuongThua = q;
                break;
            }
        }

        if (tuongThua) {
            sf::Vector2f kPos = chuyenDoiToaDoBanCoThanhManHinh(tuongThua->getHang(), tuongThua->getCot());

            // 1. Defeated General: Pulsing crimson danger ring
            float ringR = 35.0f + pulse * 6.0f;
            sf::CircleShape defeatedRing(ringR);
            defeatedRing.setOrigin(ringR, ringR);
            defeatedRing.setPosition(kPos);
            defeatedRing.setFillColor(sf::Color(255, 30, 20, static_cast<sf::Uint8>(80 + pulse * 65)));
            defeatedRing.setOutlineThickness(3.0f);
            defeatedRing.setOutlineColor(sf::Color(255, 60, 40, static_cast<sf::Uint8>(200 + pulse * 55)));
            window.draw(defeatedRing);

            // Expanding shockwaves around fallen general
            float wave1 = std::fmod(bannerPulseClock.getElapsedTime().asSeconds() * 1.4f, 1.0f);
            float wave2 = std::fmod(bannerPulseClock.getElapsedTime().asSeconds() * 1.4f + 0.5f, 1.0f);
            auto drawWave = [&](float wp) {
                float wr = 28.0f + wp * 28.0f;
                sf::CircleShape w(wr);
                w.setOrigin(wr, wr);
                w.setPosition(kPos);
                w.setFillColor(sf::Color::Transparent);
                w.setOutlineThickness(2.0f);
                w.setOutlineColor(sf::Color(255, 50, 40, static_cast<sf::Uint8>((1.0f - wp) * 220)));
                window.draw(w);
            };
            drawWave(wave1);
            drawWave(wave2);

            // Crimson corner brackets on General
            ve4GocLuaChon(window, sf::Vector2f(kPos.x - 32.0f, kPos.y - 32.0f), sf::Vector2f(64.0f, 64.0f), 2.0f, 1.4f);

            // Check if there are active checkmating attackers
            auto danhSachChieu = banCo.timCacQuanDangChieu(mauThua);

            // Badge on General
            bool placeAboveK = (tuongThua->getHang() > 4);
            if (!danhSachChieu.empty()) {
                veHuyHieuQuan(kPos, tiengViet ? "BỊ CHIẾU BÍ" : "CHECKMATED",
                             sf::Color(145, 18, 18, 240), sf::Color(255, 75, 65), sf::Color(255, 235, 140), placeAboveK);
            } else {
                veHuyHieuQuan(kPos, tiengViet ? "BỊ KHỐN BÍ" : "STALEMATED",
                             sf::Color(140, 75, 15, 240), sf::Color(245, 150, 40), sf::Color(255, 240, 160), placeAboveK);
            }

            // 2. Checkmating Pieces: Radiant imperial gold aura, 4 corner brackets, badges & attack beams!
            for (const auto& qAtk : danhSachChieu) {
                if (!qAtk || qAtk->getDaBiAn()) continue;
                sf::Vector2f atkPos = chuyenDoiToaDoBanCoThanhManHinh(qAtk->getHang(), qAtk->getCot());

                // Glowing radiant imperial gold halo
                float haloR = 34.0f + pulse * 5.0f;
                sf::CircleShape halo(haloR);
                halo.setOrigin(haloR, haloR);
                halo.setPosition(atkPos);
                halo.setFillColor(sf::Color(255, 195, 25, static_cast<sf::Uint8>(80 + pulse * 65)));
                halo.setOutlineThickness(3.0f);
                halo.setOutlineColor(sf::Color(255, 225, 75, static_cast<sf::Uint8>(210 + pulse * 45)));
                window.draw(halo);

                // Gold 4 corner brackets
                ve4GocLuaChon(window, sf::Vector2f(atkPos.x - 32.0f, atkPos.y - 32.0f), sf::Vector2f(64.0f, 64.0f), 2.0f, 1.4f);

                // Badge on checking piece
                bool placeAboveAtk = (qAtk->getHang() > 4);
                veHuyHieuQuan(atkPos, tiengViet ? "CHIẾU BÍ" : "CHECKMATE",
                             sf::Color(145, 95, 15, 240), sf::Color(255, 215, 60), sf::Color(255, 250, 190), placeAboveAtk);
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
        if (thoiGianRungQuan > 0.0f && viTriQuanRung.x == q->getHang() && viTriQuanRung.y == q->getCot()) {
            float shakeOffset = std::sin(thoiGianRungQuan * 48.0f) * 7.0f * (thoiGianRungQuan / 0.35f);
            rawPos.x += shakeOffset;
        }
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
    bool checkAlert = !banCo.getKetThuc() && (banCo.kiemTraChieu(banCo.getLuotChoi()) || banCo.kiemTraChieuTuong());
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
        std::string sideStr;
        if (banCo.getLuotChoi() == Mau::DO) {
            sideStr = tiengViet ? "BÊN ĐỎ ĐANG BỊ CHIẾU TƯỚNG!" : "RED KING IS UNDER CHECK!";
        } else {
            sideStr = tiengViet ? "BÊN ĐEN ĐANG BỊ CHIẾU TƯỚNG!" : "BLACK KING IS UNDER CHECK!";
        }

        sf::Text t;
        t.setFont(fontBold);
        t.setCharacterSize(15);
        t.setStyle(sf::Text::Bold);
        t.setString(toUtf8("[!]  " + sideStr + "  [!]"));
        t.setFillColor(sf::Color(255, 245, 120));
        sf::FloatRect tb = t.getLocalBounds();

        float bannerW = std::max(480.0f, tb.width + 60.0f);
        float bannerX = (1200.0f - bannerW) / 2.0f;

        sf::RectangleShape banner(sf::Vector2f(bannerW, bh));
        banner.setPosition(bannerX, by);
        banner.setFillColor(sf::Color(160, 20, 20, static_cast<sf::Uint8>(190 + pulse * 65)));
        banner.setOutlineThickness(2.5f);
        banner.setOutlineColor(sf::Color(255, 220, 60));
        window.draw(banner);

        ve4GocLuaChon(window, sf::Vector2f(bannerX, by), sf::Vector2f(bannerW, bh), 2.0f, 1.5f);

        t.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));
        t.setPosition(std::round(1200.0f / 2.0f), std::round(by + bh / 2.0f));
        window.draw(t);
    } else {
        float tw = 320.0f;
        float th = 42.0f;
        float tx = (1200.0f - tw) / 2.0f;
        float ty = 22.0f;

        sf::RectangleShape banner(sf::Vector2f(tw, th));
        banner.setPosition(tx, ty);
        banner.setFillColor(isRed ? sf::Color(140, 24, 18, 235) : sf::Color(24, 38, 64, 235));
        banner.setOutlineThickness(1.8f);
        banner.setOutlineColor(isRed ? sf::Color(245, 215, 120) : sf::Color(140, 195, 255));
        window.draw(banner);

        ve4GocLuaChon(window, sf::Vector2f(tx, ty), sf::Vector2f(tw, th), 1.8f, 1.2f);

        std::string turnStr = isRed
            ? (tiengViet ? "• LƯỢT ĐI: BÊN ĐỎ (TIÊN) •" : "• TURN: RED SIDE •")
            : (tiengViet ? "• LƯỢT ĐI: BÊN ĐEN (HẬU) •" : "• TURN: BLACK SIDE •");

        sf::Text tShadow;
        tShadow.setFont(fontBold);
        tShadow.setCharacterSize(15);
        tShadow.setStyle(sf::Text::Bold);
        tShadow.setString(toUtf8(turnStr));
        tShadow.setFillColor(sf::Color(0, 0, 0, 180));
        sf::FloatRect tb = tShadow.getLocalBounds();
        tShadow.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));
        tShadow.setPosition(std::round(1200.0f / 2.0f) + 1.0f, std::round(ty + th / 2.0f) + 1.0f);
        window.draw(tShadow);

        sf::Text t = tShadow;
        t.setFillColor(isRed ? sf::Color(255, 240, 190) : sf::Color(220, 240, 255));
        t.setPosition(std::round(1200.0f / 2.0f), std::round(ty + th / 2.0f));
        window.draw(t);
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

    // 1. Player 1 Card (Red) - EBONY_WOOD (X=35, Y=110, W=240, H=192)
    FantasyUI::get().drawPanel(window, sf::FloatRect(px, py, pw, 192.0f), OrientalPanelStyle::EBONY_WOOD);

    // Header strip: Red lacquer
    sf::RectangleShape h1(sf::Vector2f(pw - 18.0f, 26.0f));
    h1.setPosition(px + 9.0f, py + 9.0f);
    h1.setFillColor(isRedTurn ? sf::Color(135, 26, 20, 240) : sf::Color(65, 20, 18, 200));
    h1.setOutlineThickness(1.0f);
    h1.setOutlineColor(isRedTurn ? sf::Color(245, 215, 120) : sf::Color(140, 50, 45));
    window.draw(h1);

    sf::Text tHead1;
    tHead1.setFont(fontBold);
    tHead1.setCharacterSize(13);
    tHead1.setStyle(sf::Text::Bold);
    tHead1.setString(toUtf8(tiengViet ? "[ BÊN ĐỎ - TIÊN ]" : "[ RED - FIRST ]"));
    tHead1.setFillColor(isRedTurn ? sf::Color(255, 240, 200) : sf::Color(220, 180, 175));
    sf::FloatRect thb1 = tHead1.getLocalBounds();
    tHead1.setOrigin(std::round(thb1.left + thb1.width / 2.0f), std::round(thb1.top + thb1.height / 2.0f));
    tHead1.setPosition(std::round(px + pw / 2.0f), py + 22.0f);
    window.draw(tHead1);

    // Left circular avatar portrait with active glow when Red's turn
    veAvatarPortrait(sf::Vector2f(px + 46.0f, py + 84.0f), 26.0f, nhanVatDo, Mau::DO, isRedTurn);

    // Right of avatar: Player Name & Role
    sf::Text rName;
    rName.setFont(fontBold);
    rName.setCharacterSize(15);
    rName.setStyle(sf::Text::Bold);
    rName.setString(toUtf8(tenNguoiChoiDo));
    rName.setFillColor(sf::Color(255, 235, 160));
    sf::FloatRect nb = rName.getLocalBounds();
    rName.setOrigin(0.0f, std::round(nb.top + nb.height / 2.0f));
    rName.setPosition(px + 84.0f, py + 68.0f);

    sf::Text rNameShadow = rName;
    rNameShadow.setFillColor(sf::Color(0, 0, 0, 180));
    rNameShadow.setPosition(px + 85.0f, py + 69.0f);
    window.draw(rNameShadow);
    window.draw(rName);

    sf::Text rSide;
    rSide.setFont(fontBold);
    rSide.setCharacterSize(12);
    rSide.setStyle(sf::Text::Bold);
    rSide.setString(toUtf8(tiengViet ? "Quân Đỏ" : "Red Pieces"));
    rSide.setFillColor(isRedTurn ? sf::Color(255, 180, 170) : sf::Color(180, 150, 140));
    rSide.setPosition(px + 84.0f, py + 88.0f);
    window.draw(rSide);

    // Digital Countdown Timer in JADE_INSET slot at Y = py + 134.0f
    float timerSlotY = py + 134.0f;
    float timerSlotW = pw - 24.0f;
    float timerSlotX = px + 12.0f;
    float timerSlotH = 44.0f;
    FantasyUI::get().drawPanel(window, sf::FloatRect(timerSlotX, timerSlotY, timerSlotW, timerSlotH), OrientalPanelStyle::JADE_INSET);

    sf::Text timerLbl;
    timerLbl.setFont(fontBold);
    timerLbl.setCharacterSize(12);
    timerLbl.setStyle(sf::Text::Bold);
    timerLbl.setString(toUtf8(tiengViet ? "THỜI GIAN:" : "TIME:"));
    timerLbl.setFillColor(sf::Color(220, 195, 135));
    timerLbl.setPosition(timerSlotX + 12.0f, timerSlotY + 14.0f);
    window.draw(timerLbl);

    sf::Text timerTxt;
    timerTxt.setFont(fontBold);
    timerTxt.setCharacterSize(18);
    timerTxt.setStyle(sf::Text::Bold);
    timerTxt.setString(toUtf8(formatTime(thoiGianConDo)));
    timerTxt.setFillColor(isRedTurn && thoiGianConDo < 30.0f && thoiGianConDo > 0.0f
                          ? sf::Color(255, 70, 60)
                          : sf::Color(255, 230, 140));
    sf::FloatRect tb = timerTxt.getLocalBounds();
    timerTxt.setOrigin(std::round(tb.left + tb.width), std::round(tb.top + tb.height / 2.0f));
    timerTxt.setPosition(timerSlotX + timerSlotW - 14.0f, std::round(timerSlotY + timerSlotH / 2.0f));
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

    // 1. Player 2 Card (Black) - EBONY_WOOD (X=925, Y=110, W=240, H=192)
    FantasyUI::get().drawPanel(window, sf::FloatRect(px, py, pw, 192.0f), OrientalPanelStyle::EBONY_WOOD);

    // Header strip: Dark indigo / navy lacquer
    sf::RectangleShape h2(sf::Vector2f(pw - 18.0f, 26.0f));
    h2.setPosition(px + 9.0f, py + 9.0f);
    h2.setFillColor(isBlackTurn ? sf::Color(24, 42, 70, 240) : sf::Color(18, 28, 44, 200));
    h2.setOutlineThickness(1.0f);
    h2.setOutlineColor(isBlackTurn ? sf::Color(140, 195, 255) : sf::Color(45, 70, 105));
    window.draw(h2);

    sf::Text tHead2;
    tHead2.setFont(fontBold);
    tHead2.setCharacterSize(13);
    tHead2.setStyle(sf::Text::Bold);
    tHead2.setString(toUtf8(tiengViet
        ? (cheDoChoi == CheDoChoi::VOI_MAY ? "[ ĐỐI THỦ MÁY (AI) ]" : "[ BÊN ĐEN - HẬU ]")
        : (cheDoChoi == CheDoChoi::VOI_MAY ? "[ AI OPPONENT ]" : "[ BLACK - SECOND ]")));
    tHead2.setFillColor(isBlackTurn ? sf::Color(220, 240, 255) : sf::Color(170, 190, 210));
    sf::FloatRect thb2 = tHead2.getLocalBounds();
    tHead2.setOrigin(std::round(thb2.left + thb2.width / 2.0f), std::round(thb2.top + thb2.height / 2.0f));
    tHead2.setPosition(std::round(px + pw / 2.0f), py + 22.0f);
    window.draw(tHead2);

    // Left circular avatar portrait with active glow when Black's turn
    veAvatarPortrait(sf::Vector2f(px + 46.0f, py + 84.0f), 26.0f, nhanVatDen, Mau::DEN, isBlackTurn);

    // Right of avatar: Player Name & Role
    sf::Text bName;
    bName.setFont(fontBold);
    bName.setCharacterSize(15);
    bName.setStyle(sf::Text::Bold);
    bName.setString(toUtf8(tenNguoiChoiDen));
    bName.setFillColor(sf::Color(255, 235, 160));
    sf::FloatRect nb = bName.getLocalBounds();
    bName.setOrigin(0.0f, std::round(nb.top + nb.height / 2.0f));
    bName.setPosition(px + 84.0f, py + 68.0f);

    sf::Text bNameShadow = bName;
    bNameShadow.setFillColor(sf::Color(0, 0, 0, 180));
    bNameShadow.setPosition(px + 85.0f, py + 69.0f);
    window.draw(bNameShadow);
    window.draw(bName);

    sf::Text bSide;
    bSide.setFont(fontBold);
    bSide.setCharacterSize(12);
    bSide.setStyle(sf::Text::Bold);
    if (cheDoChoi == CheDoChoi::VOI_MAY) {
        bSide.setString(toUtf8("AI (" + Loc::get((aiDifficulty == DoKho::DE) ? LocKey::DIFF_EASY : ((aiDifficulty == DoKho::TRUNG_BINH) ? LocKey::DIFF_MED : LocKey::DIFF_HARD)) + ")"));
    } else {
        bSide.setString(toUtf8(tiengViet ? "Quân Đen" : "Black Pieces"));
    }
    bSide.setFillColor(isBlackTurn ? sf::Color(170, 210, 255) : sf::Color(140, 165, 195));
    bSide.setPosition(px + 84.0f, py + 88.0f);
    window.draw(bSide);

    // Digital Countdown Timer in JADE_INSET slot at Y = py + 134.0f
    float timerSlotY = py + 134.0f;
    float timerSlotW = pw - 24.0f;
    float timerSlotX = px + 12.0f;
    float timerSlotH = 44.0f;
    FantasyUI::get().drawPanel(window, sf::FloatRect(timerSlotX, timerSlotY, timerSlotW, timerSlotH), OrientalPanelStyle::JADE_INSET);

    sf::Text timerLbl;
    timerLbl.setFont(fontBold);
    timerLbl.setCharacterSize(12);
    timerLbl.setStyle(sf::Text::Bold);
    timerLbl.setString(toUtf8(tiengViet ? "THỜI GIAN:" : "TIME:"));
    timerLbl.setFillColor(sf::Color(220, 195, 135));
    timerLbl.setPosition(timerSlotX + 12.0f, timerSlotY + 14.0f);
    window.draw(timerLbl);

    sf::Text timerTxt;
    timerTxt.setFont(fontBold);
    timerTxt.setCharacterSize(18);
    timerTxt.setStyle(sf::Text::Bold);
    timerTxt.setString(toUtf8(formatTime(thoiGianConDen)));
    timerTxt.setFillColor(isBlackTurn && thoiGianConDen < 30.0f && thoiGianConDen > 0.0f
                          ? sf::Color(255, 70, 60)
                          : sf::Color(255, 230, 140));
    sf::FloatRect tb = timerTxt.getLocalBounds();
    timerTxt.setOrigin(std::round(tb.left + tb.width), std::round(tb.top + tb.height / 2.0f));
    timerTxt.setPosition(timerSlotX + timerSlotW - 14.0f, std::round(timerSlotY + timerSlotH / 2.0f));
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

void GameManager::veBannerXemBanCo() {
    bool tiengViet = (Loc::getLanguage() == Language::TIENG_VIET);
    float pulse = 0.5f + 0.5f * std::sin(bannerPulseClock.getElapsedTime().asSeconds() * 6.0f);

    float bw = 760.0f;
    float bh = 44.0f;
    float bx = (1200.0f - bw) / 2.0f;
    float by = 746.0f;

    // Dark wood background with golden border
    sf::RectangleShape plaque(sf::Vector2f(bw, bh));
    plaque.setPosition(bx, by);
    plaque.setFillColor(sf::Color(18, 12, 10, static_cast<sf::Uint8>(230 + pulse * 25)));
    plaque.setOutlineThickness(1.8f);
    plaque.setOutlineColor(sf::Color(240, 195, 75, static_cast<sf::Uint8>(200 + pulse * 55)));
    window.draw(plaque);

    ve4GocLuaChon(window, sf::Vector2f(bx, by), sf::Vector2f(bw, bh), 2.0f, 1.2f);

    sf::Font& f = getMenuFont(tiengViet, true);
    sf::Text t;
    t.setFont(f);
    t.setCharacterSize(14);
    t.setStyle(sf::Text::Bold);
    t.setString(toUtf8(tiengViet
        ? "[!] ĐANG XEM BÀN CỜ - NHẤP CHUỘT HOẶC NHẤN PHÍM BẤT KỲ / ESC ĐỂ MỞ LẠI BẢNG TỔNG KẾT [!]"
        : "[!] VIEWING BOARD - CLICK ANYWHERE OR PRESS ANY KEY / ESC TO RE-OPEN MATCH REPORT [!]"));
    t.setFillColor(sf::Color(255, 235, 140));
    sf::FloatRect tb = t.getLocalBounds();
    t.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));
    t.setPosition(std::round(bx + bw / 2.0f), std::round(by + bh / 2.0f));
    window.draw(t);
}

void GameManager::vePopupKetThuc() {
    if (!daPhatAmKetThuc) {
        phatAmThanhKetThuc();
    }

    if (xemBanCoSauKetThuc) {
        veBannerXemBanCo();
        return;
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
    tHint.setString(toUtf8(tiengViet ? "[ Enter: Chơi lại  |  V: Xem bàn cờ  |  ESC: Về Menu chính ]"
                                     : "[ Enter: Play Again  |  V: View Board  |  ESC: Main Menu ]"));
    tHint.setFillColor(sf::Color(235, 210, 155));
    sf::FloatRect hb = tHint.getLocalBounds();
    tHint.setOrigin(hb.left + hb.width / 2.0f, hb.top + hb.height / 2.0f);
    tHint.setPosition(dx + dw / 2.0f, dy + dh - 14.0f);
    window.draw(tHint);
}

void GameManager::veMenuPopUpInGame() {
    FantasyUI::get().drawFullDimOverlay(window, sf::Color(10, 8, 12, 180));

    // Modal centered at X=380, Y=222, W=440, H=356 (Strictly <= 480px, 0 dead space)
    const float dw = 440.0f;
    const float dh = 356.0f;
    const float dx = (1200.0f - dw) / 2.0f; // 380.0f
    const float dy = (800.0f - dh) / 2.0f;  // 222.0f

    // 1. Modal background - Oriental Ebony Wood with Inset Velvet
    FantasyUI::get().drawPanel(window, sf::FloatRect(dx, dy, dw, dh), OrientalPanelStyle::EBONY_WOOD);

    sf::RectangleShape inner(sf::Vector2f(dw - 20.0f, dh - 20.0f));
    inner.setPosition(dx + 10.0f, dy + 10.0f);
    inner.setFillColor(sf::Color(22, 16, 12, 230));
    inner.setOutlineThickness(1.2f);
    inner.setOutlineColor(sf::Color(145, 105, 50, 160));
    window.draw(inner);

    ve4GocLuaChon(window, sf::Vector2f(dx + 10.0f, dy + 10.0f), sf::Vector2f(dw - 20.0f, dh - 20.0f), 2.0f, 1.2f);

    bool tiengViet = (Loc::getLanguage() == Language::TIENG_VIET);
    sf::Font& fBold = getMenuFont(tiengViet, true);

    // 2. Header Tablet - Lacquer Red Banner with Gold Cloud Corners (W=300, H=36 at Y=238)
    float tw = 300.0f;
    float th = 36.0f;
    float tx = (1200.0f - tw) / 2.0f;
    float ty = dy + 16.0f;

    sf::RectangleShape titlePlate(sf::Vector2f(tw, th));
    titlePlate.setPosition(tx, ty);
    titlePlate.setFillColor(sf::Color(140, 24, 18, 240));
    titlePlate.setOutlineThickness(1.8f);
    titlePlate.setOutlineColor(sf::Color(225, 180, 80));
    window.draw(titlePlate);

    ve4GocLuaChon(window, sf::Vector2f(tx, ty), sf::Vector2f(tw, th), 1.5f, 1.1f);

    sf::Text tTitle;
    tTitle.setFont(fBold);
    tTitle.setCharacterSize(16);
    tTitle.setStyle(sf::Text::Bold);
    tTitle.setString(toUtf8(tiengViet ? "• TẠM DỪNG TRẬN ĐẤU •" : "• MATCH PAUSED •"));
    tTitle.setFillColor(sf::Color(255, 240, 190));
    sf::FloatRect tb = tTitle.getLocalBounds();
    tTitle.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));

    sf::Text tTitleShadow = tTitle;
    tTitleShadow.setFillColor(sf::Color(10, 4, 4, 220));
    tTitleShadow.setPosition(std::round(1200.0f / 2.0f) + 1.2f, std::round(ty + th / 2.0f) + 1.2f);
    window.draw(tTitleShadow);

    tTitle.setPosition(std::round(1200.0f / 2.0f), std::round(ty + th / 2.0f));
    window.draw(tTitle);

    // 3. Match Status Inset Badge (Y=282, H=30, W=350)
    float sw = 350.0f;
    float sh = 30.0f;
    float sx = (1200.0f - sw) / 2.0f;
    float sy = dy + 60.0f;

    sf::RectangleShape statusPlate(sf::Vector2f(sw, sh));
    statusPlate.setPosition(sx, sy);
    statusPlate.setFillColor(sf::Color(18, 24, 22, 235));
    statusPlate.setOutlineThickness(1.2f);
    statusPlate.setOutlineColor(sf::Color(70, 115, 85, 180));
    window.draw(statusPlate);

    ve4GocLuaChon(window, sf::Vector2f(sx, sy), sf::Vector2f(sw, sh), 1.0f, 1.0f);

    bool isRed = (banCo.getLuotChoi() == Mau::DO);
    float discRadius = 9.0f;
    sf::Vector2f discCenter(sx + 20.0f, sy + sh / 2.0f);

    sf::CircleShape disc(discRadius);
    disc.setOrigin(discRadius, discRadius);
    disc.setPosition(discCenter);
    disc.setFillColor(isRed ? sf::Color(165, 25, 20) : sf::Color(26, 32, 42));
    disc.setOutlineThickness(1.2f);
    disc.setOutlineColor(isRed ? sf::Color(245, 215, 120) : sf::Color(170, 200, 240));
    window.draw(disc);

    sf::Text discSymbol;
    discSymbol.setFont(fBold);
    discSymbol.setCharacterSize(10);
    discSymbol.setStyle(sf::Text::Bold);
    discSymbol.setString(toUtf8(isRed ? "帥" : "將"));
    discSymbol.setFillColor(isRed ? sf::Color(255, 240, 190) : sf::Color(230, 245, 255));
    sf::FloatRect dsb = discSymbol.getLocalBounds();
    discSymbol.setOrigin(std::round(dsb.left + dsb.width / 2.0f), std::round(dsb.top + dsb.height / 2.0f));
    discSymbol.setPosition(discCenter.x, discCenter.y - 1.0f);
    window.draw(discSymbol);

    std::string turnName = isRed
        ? (tiengViet ? "Lượt đi: Bên Đỏ (Tiên)" : "Turn: Red (1st)")
        : (tiengViet ? "Lượt đi: Bên Đen (Hậu)" : "Turn: Black (2nd)");
    int moveCount = static_cast<int>(banCo.getLichSuNuocDi().size() + 1);
    std::string moveStr = (tiengViet ? "  •  Nước thứ: " : "  •  Move: ") + std::to_string(moveCount);

    sf::Text tStatus;
    tStatus.setFont(fBold);
    tStatus.setCharacterSize(13);
    tStatus.setStyle(sf::Text::Bold);
    tStatus.setString(toUtf8(turnName + moveStr));
    tStatus.setFillColor(isRed ? sf::Color(255, 225, 160) : sf::Color(215, 235, 255));
    sf::FloatRect stb = tStatus.getLocalBounds();
    tStatus.setOrigin(0.0f, std::round(stb.top + stb.height / 2.0f));
    tStatus.setPosition(sx + 38.0f, std::round(sy + sh / 2.0f - 1.0f));

    sf::Text tStatusShadow = tStatus;
    tStatusShadow.setFillColor(sf::Color(0, 0, 0, 200));
    tStatusShadow.setPosition(tStatus.getPosition().x + 1.0f, tStatus.getPosition().y + 1.0f);
    window.draw(tStatusShadow);
    window.draw(tStatus);

    // 4. Draw interactive Pause Menu Buttons
    for (auto& btn : inGameMenuButtons) {
        btn->draw(window);
    }

    // 5. Minimal Footer Prompt (Y = dy + dh - 20 = 558)
    sf::Text tHint;
    tHint.setFont(fBold);
    tHint.setCharacterSize(13);
    tHint.setStyle(sf::Text::Bold);
    tHint.setString(toUtf8(tiengViet
        ? "[ESC] hoặc click ngoài để tiếp tục"
        : "Press [ESC] or click outside to resume"));
    tHint.setFillColor(sf::Color(210, 185, 135, 220));
    sf::FloatRect hb = tHint.getLocalBounds();
    tHint.setOrigin(std::round(hb.left + hb.width / 2.0f), std::round(hb.top + hb.height / 2.0f));
    tHint.setPosition(std::round(1200.0f / 2.0f), std::round(dy + dh - 20.0f));
    window.draw(tHint);
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

void GameManager::veDongHoCat(sf::RenderWindow& win, sf::Vector2f center, float h) {
    auto& am = AssetManager::get();
    const sf::Texture& hgTex = am.texture(TextureId::CHINESE_HOURGLASS);
    if (hgTex.getSize().x > 0) {
        sf::Sprite sp(hgTex);
        float tw = static_cast<float>(hgTex.getSize().x);
        float th = static_cast<float>(hgTex.getSize().y);
        sp.setOrigin(tw / 2.0f, th / 2.0f);
        float sc = h / th;
        sp.setScale(sc, sc);
        sp.setPosition(center.x, center.y);
        win.draw(sp);
        return;
    }

    float w = h * 0.52f;
    float topY = center.y - h / 2.0f;
    float btmY = center.y + h / 2.0f;
    float leftX = center.x - w / 2.0f;
    float rightX = center.x + w / 2.0f;

    // 1. Shadow beneath the hourglass
    sf::CircleShape baseShadow(w * 0.45f);
    baseShadow.setScale(1.0f, 0.35f);
    baseShadow.setOrigin(w * 0.45f, w * 0.45f);
    baseShadow.setPosition(center.x, btmY + 6.0f);
    baseShadow.setFillColor(sf::Color(0, 0, 0, 90));
    win.draw(baseShadow);

    // 2. Glass Bulb Body (Upper & Lower Bulb)
    sf::ConvexShape upperGlass(4);
    upperGlass.setPoint(0, sf::Vector2f(leftX + 6.0f, topY + 8.0f));
    upperGlass.setPoint(1, sf::Vector2f(rightX - 6.0f, topY + 8.0f));
    upperGlass.setPoint(2, sf::Vector2f(center.x + 4.0f, center.y));
    upperGlass.setPoint(3, sf::Vector2f(center.x - 4.0f, center.y));
    upperGlass.setFillColor(sf::Color(210, 235, 255, 45));
    upperGlass.setOutlineThickness(1.5f);
    upperGlass.setOutlineColor(sf::Color(170, 210, 245, 140));
    win.draw(upperGlass);

    sf::ConvexShape lowerGlass(4);
    lowerGlass.setPoint(0, sf::Vector2f(center.x - 4.0f, center.y));
    lowerGlass.setPoint(1, sf::Vector2f(center.x + 4.0f, center.y));
    lowerGlass.setPoint(2, sf::Vector2f(rightX - 6.0f, btmY - 8.0f));
    lowerGlass.setPoint(3, sf::Vector2f(leftX + 6.0f, btmY - 8.0f));
    lowerGlass.setFillColor(sf::Color(210, 235, 255, 45));
    lowerGlass.setOutlineThickness(1.5f);
    lowerGlass.setOutlineColor(sf::Color(170, 210, 245, 140));
    win.draw(lowerGlass);

    // 3. Golden Sand
    sf::ConvexShape upperSand(4);
    float sandTop = topY + 20.0f;
    float upperSandW = (w - 20.0f) * 0.72f;
    upperSand.setPoint(0, sf::Vector2f(center.x - upperSandW, sandTop));
    upperSand.setPoint(1, sf::Vector2f(center.x + upperSandW, sandTop));
    upperSand.setPoint(2, sf::Vector2f(center.x + 3.0f, center.y - 1.0f));
    upperSand.setPoint(3, sf::Vector2f(center.x - 3.0f, center.y - 1.0f));
    upperSand.setFillColor(sf::Color(235, 185, 80));
    win.draw(upperSand);

    sf::RectangleShape sandStream(sf::Vector2f(2.0f, h * 0.28f));
    sandStream.setPosition(center.x - 1.0f, center.y - 1.0f);
    sandStream.setFillColor(sf::Color(255, 215, 110));
    win.draw(sandStream);

    sf::ConvexShape lowerSand(3);
    float moundTop = btmY - 24.0f;
    lowerSand.setPoint(0, sf::Vector2f(center.x, moundTop));
    lowerSand.setPoint(1, sf::Vector2f(rightX - 8.0f, btmY - 9.0f));
    lowerSand.setPoint(2, sf::Vector2f(leftX + 8.0f, btmY - 9.0f));
    lowerSand.setFillColor(sf::Color(220, 170, 70));
    win.draw(lowerSand);

    // 4. Wooden Top & Bottom Plates
    auto drawPlate = [&](float py, bool isTop) {
        sf::RectangleShape plate(sf::Vector2f(w + 4.0f, 10.0f));
        plate.setPosition(leftX - 2.0f, py);
        plate.setFillColor(sf::Color(80, 36, 18));
        plate.setOutlineThickness(1.2f);
        plate.setOutlineColor(sf::Color(215, 175, 80));
        win.draw(plate);

        sf::RectangleShape rim(sf::Vector2f(w - 6.0f, 3.0f));
        rim.setPosition(leftX + 3.0f, isTop ? py + 1.0f : py + 6.0f);
        rim.setFillColor(sf::Color(245, 210, 110, 200));
        win.draw(rim);

        sf::CircleShape knob(3.5f);
        knob.setOrigin(3.5f, 3.5f);
        knob.setPosition(center.x, isTop ? py : py + 10.0f);
        knob.setFillColor(sf::Color(215, 175, 80));
        win.draw(knob);
    };
    drawPlate(topY, true);
    drawPlate(btmY - 10.0f, false);

    // 5. Wooden Pillars
    auto drawPillar = [&](float px) {
        sf::RectangleShape pil(sf::Vector2f(4.0f, h - 16.0f));
        pil.setPosition(px - 2.0f, topY + 8.0f);
        pil.setFillColor(sf::Color(95, 45, 22));
        pil.setOutlineThickness(0.8f);
        pil.setOutlineColor(sf::Color(190, 140, 60));
        win.draw(pil);

        sf::RectangleShape hl(sf::Vector2f(1.0f, h - 18.0f));
        hl.setPosition(px - 1.0f, topY + 9.0f);
        hl.setFillColor(sf::Color(240, 200, 110, 140));
        win.draw(hl);
    };
    drawPillar(leftX + 2.0f);
    drawPillar(rightX - 2.0f);

    // 6. Glass Highlights
    sf::RectangleShape shine1(sf::Vector2f(2.0f, h * 0.32f));
    shine1.setPosition(leftX + 9.0f, topY + 12.0f);
    shine1.setFillColor(sf::Color(255, 255, 255, 90));
    win.draw(shine1);

    sf::RectangleShape shine2(sf::Vector2f(2.0f, h * 0.28f));
    shine2.setPosition(leftX + 9.0f, center.y + 8.0f);
    shine2.setFillColor(sf::Color(255, 255, 255, 90));
    win.draw(shine2);
}

void GameManager::khoiTaoPlaySetupButtons() {
    playSetupButtons.clear();
    bool tiengViet = (Loc::getLanguage() == Language::TIENG_VIET);
    sf::Font& fBold = getMenuFont(tiengViet, true);

    auto getTimerLabel = [this, tiengViet]() -> std::string {
        if (thoiGianVanDau == 0.0f) return tiengViet ? "Không Giới Hạn" : "Unlimited";
        if (thoiGianVanDau == 300.0f) return tiengViet ? "5 Phút / Ván" : "5 Mins / Side";
        if (thoiGianVanDau == 600.0f) return tiengViet ? "10 Phút / Ván" : "10 Mins / Side";
        if (thoiGianVanDau == 900.0f) return tiengViet ? "15 Phút / Ván" : "15 Mins / Side";
        return tiengViet ? "30 Phút / Ván" : "30 Mins / Side";
    };

    auto getBoardThemeLabel = [this, tiengViet]() -> std::string {
        if (boardTheme == BoardTheme::IMPERIAL_JADE) return tiengViet ? "Ngọc Bích" : "Imperial Jade";
        if (boardTheme == BoardTheme::MIDNIGHT_INK) return tiengViet ? "Mực Đêm" : "Midnight Ink";
        if (boardTheme == BoardTheme::WARM_BAMBOO) return tiengViet ? "Trúc Ấm" : "Warm Bamboo";
        return tiengViet ? "Gỗ Cổ Điển" : "Classic Wood";
    };

    auto getPieceStyleLabel = [this, tiengViet]() -> std::string {
        if (pieceStyle == PieceStyle::VIETNAMESE) return tiengViet ? "Việt Hóa" : "Vietnamese";
        if (pieceStyle == PieceStyle::SHORT_CODE) return tiengViet ? "Ký Hiệu" : "Short Code";
        if (pieceStyle == PieceStyle::INTERNATIONAL) return tiengViet ? "Quốc Tế" : "International";
        return tiengViet ? "Truyền Thống" : "Traditional";
    };

    auto addCycle = [&](float x, float y, float valWidth, const std::string& valText,
                        std::function<void()> onPrev, std::function<void()> onNext) {
        const float arrW = 28.0f;
        const float h = 34.0f;
        auto bPrev = std::make_unique<Button>(x, y, arrW, h, "<", fBold, std::move(onPrev));
        bPrev->setCharacterSize(15);
        bPrev->setColors(sf::Color(242, 232, 206), sf::Color(255, 224, 128), sf::Color(255, 250, 220));
        playSetupButtons.push_back(std::move(bPrev));

        auto bVal = std::make_unique<Button>(x + arrW + 2.0f, y, valWidth, h, valText, fBold, onNext);
        bVal->setCharacterSize(13);
        bVal->setColors(sf::Color(255, 232, 165), sf::Color(255, 244, 195), sf::Color(255, 250, 224));
        playSetupButtons.push_back(std::move(bVal));

        auto bNext = std::make_unique<Button>(x + arrW + 2.0f + valWidth + 2.0f, y, arrW, h, ">", fBold, std::move(onNext));
        bNext->setCharacterSize(15);
        bNext->setColors(sf::Color(242, 232, 206), sf::Color(255, 224, 128), sf::Color(255, 250, 220));
        playSetupButtons.push_back(std::move(bNext));
    };

    const float frameX = 120.0f;
    const float frameY = 75.0f;
    const float cardY = frameY + 18.0f;  // 93.0f
    const float card1X = frameX + 22.0f; // 142.0f
    const float card2X = 614.0f;
    const float secY = 284.0f;
    const float colCycleY = secY + 54.0f; // 338.0f

    if (playSetupMode == 1) {
        // --- 1. Two-Player PvP Setup ---
        // Player 1 Card (Left): input at (208, 141, 216, 34)
        inputP1Name->setPosition(card1X + 66.0f, cardY + 48.0f);
        inputP1Name->setSize(216.0f, 34.0f);
        inputP1Name->setParchmentStyle(true);

        std::string p1Role = (pvpFirstPlayer == 1) ? (tiengViet ? "BÊN ĐỎ" : "RED SIDE") : (tiengViet ? "BÊN ĐEN" : "BLACK SIDE");
        addCycle(card1X + 66.0f, cardY + 98.0f, 156.0f, p1Role,
            [this]() {
                soundManager.play(SoundType::CLICK);
                pvpFirstPlayer = (pvpFirstPlayer == 1) ? 2 : 1;
                khoiTaoPlaySetupButtons();
            },
            [this]() {
                soundManager.play(SoundType::CLICK);
                pvpFirstPlayer = (pvpFirstPlayer == 1) ? 2 : 1;
                khoiTaoPlaySetupButtons();
            });

        // Player 1 Avatar cycle arrows below circular portrait (centered at 514, 177)
        auto bP1AvaPrev = std::make_unique<Button>(474.0f, cardY + 138.0f, 36.0f, 26.0f, "<", fBold, [this]() {
            soundManager.play(SoundType::CLICK);
            p1CharIndex = (p1CharIndex + Loc::getCharCount() - 1) % Loc::getCharCount();
            khoiTaoPlaySetupButtons();
        });
        bP1AvaPrev->setCharacterSize(14);
        bP1AvaPrev->setColors(sf::Color(242, 232, 206), sf::Color(255, 224, 128), sf::Color(255, 250, 220));
        playSetupButtons.push_back(std::move(bP1AvaPrev));

        auto bP1AvaNext = std::make_unique<Button>(518.0f, cardY + 138.0f, 36.0f, 26.0f, ">", fBold, [this]() {
            soundManager.play(SoundType::CLICK);
            p1CharIndex = (p1CharIndex + 1) % Loc::getCharCount();
            khoiTaoPlaySetupButtons();
        });
        bP1AvaNext->setCharacterSize(14);
        bP1AvaNext->setColors(sf::Color(242, 232, 206), sf::Color(255, 224, 128), sf::Color(255, 250, 220));
        playSetupButtons.push_back(std::move(bP1AvaNext));

        // Player 2 Card (Right): input at (680, 141, 216, 34)
        inputP2Name->setPosition(card2X + 66.0f, cardY + 48.0f);
        inputP2Name->setSize(216.0f, 34.0f);
        inputP2Name->setParchmentStyle(true);

        std::string p2Role = (pvpFirstPlayer == 1) ? (tiengViet ? "BÊN ĐEN" : "BLACK SIDE") : (tiengViet ? "BÊN ĐỎ" : "RED SIDE");
        addCycle(card2X + 66.0f, cardY + 98.0f, 156.0f, p2Role,
            [this]() {
                soundManager.play(SoundType::CLICK);
                pvpFirstPlayer = (pvpFirstPlayer == 1) ? 2 : 1;
                khoiTaoPlaySetupButtons();
            },
            [this]() {
                soundManager.play(SoundType::CLICK);
                pvpFirstPlayer = (pvpFirstPlayer == 1) ? 2 : 1;
                khoiTaoPlaySetupButtons();
            });

        auto bP2AvaPrev = std::make_unique<Button>(946.0f, cardY + 138.0f, 36.0f, 26.0f, "<", fBold, [this]() {
            soundManager.play(SoundType::CLICK);
            p2CharIndex = (p2CharIndex + Loc::getCharCount() - 1) % Loc::getCharCount();
            khoiTaoPlaySetupButtons();
        });
        bP2AvaPrev->setCharacterSize(14);
        bP2AvaPrev->setColors(sf::Color(242, 232, 206), sf::Color(255, 224, 128), sf::Color(255, 250, 220));
        playSetupButtons.push_back(std::move(bP2AvaPrev));

        auto bP2AvaNext = std::make_unique<Button>(990.0f, cardY + 138.0f, 36.0f, 26.0f, ">", fBold, [this]() {
            soundManager.play(SoundType::CLICK);
            p2CharIndex = (p2CharIndex + 1) % Loc::getCharCount();
            khoiTaoPlaySetupButtons();
        });
        bP2AvaNext->setCharacterSize(14);
        bP2AvaNext->setColors(sf::Color(242, 232, 206), sf::Color(255, 224, 128), sf::Color(255, 250, 220));
        playSetupButtons.push_back(std::move(bP2AvaNext));

    } else {
        // --- 2. Player vs AI Setup ---
        inputAiPlayerName->setPosition(card1X + 66.0f, cardY + 48.0f);
        inputAiPlayerName->setSize(216.0f, 34.0f);
        inputAiPlayerName->setParchmentStyle(true);

        auto getSideLabel = [this, tiengViet]() -> std::string {
            if (aiFirstChoice == 0) return tiengViet ? "BÊN ĐỎ (ĐI TIÊN)" : "RED (FIRST)";
            if (aiFirstChoice == 1) return tiengViet ? "BÊN ĐEN (ĐI HẬU)" : "BLACK (SECOND)";
            return tiengViet ? "NGẪU NHIÊN" : "RANDOM SIDE";
        };
        addCycle(card1X + 66.0f, cardY + 98.0f, 156.0f, getSideLabel(),
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

        auto bAiAvaPrev = std::make_unique<Button>(474.0f, cardY + 138.0f, 36.0f, 26.0f, "<", fBold, [this]() {
            soundManager.play(SoundType::CLICK);
            aiPlayerCharIndex = (aiPlayerCharIndex + Loc::getCharCount() - 1) % Loc::getCharCount();
            khoiTaoPlaySetupButtons();
        });
        bAiAvaPrev->setCharacterSize(14);
        bAiAvaPrev->setColors(sf::Color(242, 232, 206), sf::Color(255, 224, 128), sf::Color(255, 250, 220));
        playSetupButtons.push_back(std::move(bAiAvaPrev));

        auto bAiAvaNext = std::make_unique<Button>(518.0f, cardY + 138.0f, 36.0f, 26.0f, ">", fBold, [this]() {
            soundManager.play(SoundType::CLICK);
            aiPlayerCharIndex = (aiPlayerCharIndex + 1) % Loc::getCharCount();
            khoiTaoPlaySetupButtons();
        });
        bAiAvaNext->setCharacterSize(14);
        bAiAvaNext->setColors(sf::Color(242, 232, 206), sf::Color(255, 224, 128), sf::Color(255, 250, 220));
        playSetupButtons.push_back(std::move(bAiAvaNext));

        // Right Card (AI Opponent):
        auto getDiffLabel = [this, tiengViet]() -> std::string {
            if (aiDifficulty == DoKho::DE) return tiengViet ? "DỄ" : "EASY";
            if (aiDifficulty == DoKho::TRUNG_BINH) return tiengViet ? "TRUNG BÌNH" : "MEDIUM";
            return tiengViet ? "KHÓ" : "HARD";
        };
        addCycle(card2X + 66.0f, cardY + 48.0f, 156.0f, getDiffLabel(),
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

        std::string aiSideStr = (aiFirstChoice == 0) ? (tiengViet ? "ĐI HẬU (ĐEN)" : "BLACK (2ND)")
                             : ((aiFirstChoice == 1) ? (tiengViet ? "ĐI TIÊN (ĐỎ)" : "RED (1ST)")
                                                     : (tiengViet ? "NGẪU NHIÊN" : "RANDOM"));
        addCycle(card2X + 66.0f, cardY + 98.0f, 156.0f, aiSideStr,
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
    }

    // --- 4 Column Cycles in "TÙY CHỌN TRẬN ĐẤU" ---
    // Col 1: Match Timer (col1X = 158.0f)
    addCycle(158.0f + 6.0f, colCycleY, 136.0f, getTimerLabel(),
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

    // Col 2: Rules (col2X = 382.0f)
    addCycle(382.0f + 6.0f, colCycleY, 136.0f, tiengViet ? "CỜ TƯỚNG CƠ BẢN" : "STANDARD RULES",
        [this]() { soundManager.play(SoundType::CLICK); },
        [this]() { soundManager.play(SoundType::CLICK); });

    // Col 3: Board Theme (col3X = 606.0f)
    addCycle(606.0f + 6.0f, colCycleY, 136.0f, getBoardThemeLabel(),
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

    // Col 4: Piece Style (col4X = 830.0f)
    addCycle(830.0f + 6.0f, colCycleY, 136.0f, getPieceStyleLabel(),
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

    // --- Bottom Action Buttons ---
    const float btmY = 600.0f;
    const float btmH = 46.0f;
    const float btmBtnW = 160.0f;
    const float nextBtnX = 1058.0f - btmBtnW; // 898.0f, symmetrically balanced with Back button at 142.0f

    auto backBtn = std::make_unique<Button>(142.0f, btmY, btmBtnW, btmH,
        tiengViet ? "Quay lại" : "Back", fBold, [this]() {
        soundManager.play(SoundType::CLICK);
        trangThai = TrangThai::TITLE_SCREEN;
        titleSubMenu = TitleSubMenu::MODE_SELECT;
        titleMenuIndex = 0;
    });
    backBtn->setCharacterSize(16);
    backBtn->setColors(sf::Color(242, 232, 206), sf::Color(255, 224, 128), sf::Color(255, 250, 220));
    playSetupButtons.push_back(std::move(backBtn));

    auto trimStr = [](std::string s) {
        while (!s.empty() && (s.front() == ' ' || s.front() == '\t')) s.erase(s.begin());
        while (!s.empty() && (s.back() == ' ' || s.back() == '\t')) s.pop_back();
        return s;
    };

    if (playSetupMode == 1) {
        auto nextBtn = std::make_unique<Button>(nextBtnX, btmY, btmBtnW, btmH,
            tiengViet ? "Tiếp theo" : "Next", fBold, [this, trimStr, tiengViet]() {
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
        nextBtn->setCharacterSize(16);
        nextBtn->setSelected(true);
        nextBtn->setColors(sf::Color(255, 236, 175), sf::Color(255, 248, 210), sf::Color(255, 255, 235));
        playSetupButtons.push_back(std::move(nextBtn));
    } else {
        bool needsRoll = (aiFirstChoice == 2);
        std::string actLabel = needsRoll
            ? (tiengViet ? "Tiếp theo" : "Next")
            : (tiengViet ? "Vào trận đấu" : "Start Match");

        auto actBtn = std::make_unique<Button>(nextBtnX, btmY, btmBtnW, btmH, actLabel, fBold, [this, trimStr, tiengViet, needsRoll]() {
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
    }
}

void GameManager::vePieceStylePreview(sf::RenderWindow& win, float x, float y, float w, float h) {
    auto& assets = getMenuAssets();
    sf::Font& fBold = assets.fontBoldLoaded ? assets.fontBold : fontBold;
    bool vi = (Loc::getLanguage() == Language::TIENG_VIET);

    sf::RectangleShape box(sf::Vector2f(w, h));
    box.setPosition(x, y);
    box.setFillColor(sf::Color(20, 16, 12, 230));
    box.setOutlineThickness(1.2f);
    box.setOutlineColor(sf::Color(115, 85, 45, 160));
    win.draw(box);
    ve4GocLuaChon(win, sf::Vector2f(x, y), sf::Vector2f(w, h), 1.0f, 1.0f);

    float r = 28.0f;
    float centerY = y + h * 0.44f;
    sf::Vector2f posRed(x + w * 0.32f, centerY);
    sf::Vector2f posBlack(x + w * 0.68f, centerY);

    auto drawSamplePiece = [&](sf::Vector2f pos, Mau side) {
        bool isRed = (side == Mau::DO);
        const sf::Texture* tex = (pieceStyle == PieceStyle::REALISTIC_WOOD) ? layTextureQuan("Tuong", side) : nullptr;
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
            sf::CircleShape shadow(r);
            shadow.setOrigin(r, r);
            shadow.setPosition(pos.x + 1.5f, pos.y + 2.0f);
            shadow.setFillColor(sf::Color(0, 0, 0, 140));
            win.draw(shadow);

            sf::CircleShape piece(r);
            piece.setOrigin(r, r);
            piece.setPosition(pos);

            sf::Color pieceFill(248, 236, 216);
            sf::Color pieceBorder = isRed ? sf::Color(180, 50, 40) : sf::Color(45, 95, 65);
            sf::Color textCol = isRed ? sf::Color(175, 25, 20) : sf::Color(22, 34, 48);

            piece.setFillColor(pieceFill);
            piece.setOutlineThickness(2.2f);
            piece.setOutlineColor(pieceBorder);
            win.draw(piece);

            sf::CircleShape innerRing(r - 3.5f);
            innerRing.setOrigin(r - 3.5f, r - 3.5f);
            innerRing.setPosition(pos);
            innerRing.setFillColor(sf::Color::Transparent);
            innerRing.setOutlineThickness(1.0f);
            innerRing.setOutlineColor(sf::Color(pieceBorder.r, pieceBorder.g, pieceBorder.b, 150));
            win.draw(innerRing);

            sf::Text sym;
            sym.setFont(fBold);
            std::string s;
            if (pieceStyle == PieceStyle::VIETNAMESE) {
                s = "Tướng";
            } else if (pieceStyle == PieceStyle::SHORT_CODE) {
                s = "TG";
            } else if (pieceStyle == PieceStyle::INTERNATIONAL) {
                s = "K";
            } else {
                s = isRed ? "帥" : "將";
            }
            sym.setString(toUtf8(s));
            sym.setCharacterSize((pieceStyle == PieceStyle::VIETNAMESE) ? 14 : 22);
            sym.setStyle(sf::Text::Bold);
            sym.setFillColor(textCol);
            sf::FloatRect sb = sym.getLocalBounds();
            sym.setOrigin(std::round(sb.left + sb.width / 2.0f), std::round(sb.top + sb.height / 2.0f));
            sym.setPosition(pos.x, pos.y - 1.0f);
            win.draw(sym);
        }
    };

    drawSamplePiece(posRed, Mau::DO);
    drawSamplePiece(posBlack, Mau::DEN);

    sf::Text nameLbl;
    nameLbl.setFont(fBold);
    std::string styleName;
    if (pieceStyle == PieceStyle::REALISTIC_WOOD) styleName = vi ? "Gỗ Truyền Thống" : "Classic Wood";
    else if (pieceStyle == PieceStyle::VIETNAMESE) styleName = vi ? "Việt Hóa" : "Vietnamese";
    else if (pieceStyle == PieceStyle::SHORT_CODE) styleName = vi ? "Ký Hiệu Rút Gọn" : "Short Code";
    else styleName = vi ? "Quốc Tế" : "International";

    nameLbl.setString(toUtf8(styleName));
    nameLbl.setCharacterSize(13);
    nameLbl.setStyle(sf::Text::Bold);
    nameLbl.setFillColor(sf::Color(245, 225, 160));
    sf::FloatRect nlb = nameLbl.getLocalBounds();
    nameLbl.setOrigin(std::round(nlb.left + nlb.width / 2.0f), std::round(nlb.top + nlb.height / 2.0f));
    nameLbl.setPosition(std::round(x + w / 2.0f), std::round(y + h - 20.0f));
    win.draw(nameLbl);
}

void GameManager::veBoardThemePreview(sf::RenderWindow& win, float x, float y, float w, float h) {
    auto& assets = getMenuAssets();
    sf::Font& fBold = assets.fontBoldLoaded ? assets.fontBold : fontBold;
    bool vi = (Loc::getLanguage() == Language::TIENG_VIET);

    sf::RectangleShape box(sf::Vector2f(w, h));
    box.setPosition(x, y);
    box.setFillColor(sf::Color(20, 16, 12, 230));
    box.setOutlineThickness(1.2f);
    box.setOutlineColor(sf::Color(115, 85, 45, 160));
    win.draw(box);
    ve4GocLuaChon(win, sf::Vector2f(x, y), sf::Vector2f(w, h), 1.0f, 1.0f);

    float canvasH = h - 24.0f;
    float canvasW = std::round(canvasH * 0.92f);
    float canvasX = std::round(x + (w - canvasW) / 2.0f);
    float canvasY = y + 12.0f;

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

    sf::RectangleShape boardSurface(sf::Vector2f(canvasW, canvasH));
    boardSurface.setPosition(canvasX, canvasY);
    boardSurface.setFillColor(boardBgCol);
    boardSurface.setOutlineThickness(1.5f);
    boardSurface.setOutlineColor(borderCol);
    win.draw(boardSurface);

    sf::RectangleShape innerBorder(sf::Vector2f(canvasW - 4.0f, canvasH - 4.0f));
    innerBorder.setPosition(canvasX + 2.0f, canvasY + 2.0f);
    innerBorder.setFillColor(sf::Color::Transparent);
    innerBorder.setOutlineThickness(1.0f);
    innerBorder.setOutlineColor(sf::Color(lineCol.r, lineCol.g, lineCol.b, 120));
    win.draw(innerBorder);

    for (float gy = canvasY + 6.0f; gy < canvasY + canvasH - 4.0f; gy += 8.0f) {
        sf::RectangleShape gr(sf::Vector2f(canvasW - 6.0f, 1.0f));
        gr.setPosition(canvasX + 3.0f, std::round(gy));
        gr.setFillColor(sf::Color(lineCol.r, lineCol.g, lineCol.b, 25));
        win.draw(gr);
    }

    float riverTop = canvasY + canvasH / 2.0f - 7.0f;
    float riverBtm = canvasY + canvasH / 2.0f + 7.0f;
    sf::RectangleShape riverZone(sf::Vector2f(canvasW - 6.0f, riverBtm - riverTop));
    riverZone.setPosition(canvasX + 3.0f, riverTop);
    riverZone.setFillColor(sf::Color(lineCol.r, lineCol.g, lineCol.b, 20));
    win.draw(riverZone);

    float rowsY[] = {
        canvasY + 3.0f,
        canvasY + (riverTop - canvasY) * 0.5f + 1.5f,
        riverTop,
        riverBtm,
        riverBtm + (canvasY + canvasH - riverBtm) * 0.5f - 1.5f,
        canvasY + canvasH - 3.0f
    };
    for (float ry : rowsY) {
        sf::RectangleShape hl(sf::Vector2f(canvasW - 6.0f, 1.0f));
        hl.setPosition(canvasX + 3.0f, std::round(ry));
        hl.setFillColor(sf::Color(lineCol.r, lineCol.g, lineCol.b, 170));
        win.draw(hl);
    }

    int numCols = 9;
    float colSpacing = (canvasW - 14.0f) / static_cast<float>(numCols - 1);
    for (int c = 0; c < numCols; ++c) {
        float cx = canvasX + 7.0f + static_cast<float>(c) * colSpacing;
        if (c == 0 || c == numCols - 1) {
            sf::RectangleShape vl(sf::Vector2f(1.0f, canvasH - 6.0f));
            vl.setPosition(cx, canvasY + 3.0f);
            vl.setFillColor(sf::Color(lineCol.r, lineCol.g, lineCol.b, 180));
            win.draw(vl);
        } else {
            sf::RectangleShape vlTop(sf::Vector2f(1.0f, riverTop - (canvasY + 3.0f)));
            vlTop.setPosition(cx, canvasY + 3.0f);
            vlTop.setFillColor(sf::Color(lineCol.r, lineCol.g, lineCol.b, 130));
            win.draw(vlTop);

            sf::RectangleShape vlBtm(sf::Vector2f(1.0f, (canvasY + canvasH - 3.0f) - riverBtm));
            vlBtm.setPosition(cx, riverBtm);
            vlBtm.setFillColor(sf::Color(lineCol.r, lineCol.g, lineCol.b, 130));
            win.draw(vlBtm);
        }
    }

    float pX1 = canvasX + 7.0f + 3.0f * colSpacing;
    float pX2 = canvasX + 7.0f + 5.0f * colSpacing;
    auto drawDiag = [&](sf::Vector2f p1, sf::Vector2f p2) {
        sf::Vector2f diff = p2 - p1;
        float len = std::hypot(diff.x, diff.y);
        if (len > 0.001f) {
            sf::RectangleShape seg(sf::Vector2f(len, 1.0f));
            seg.setPosition(p1);
            seg.setRotation(std::atan2(diff.y, diff.x) * 180.0f / 3.14159265f);
            seg.setFillColor(sf::Color(lineCol.r, lineCol.g, lineCol.b, 110));
            win.draw(seg);
        }
    };
    drawDiag(sf::Vector2f(pX1, rowsY[0]), sf::Vector2f(pX2, rowsY[2]));
    drawDiag(sf::Vector2f(pX2, rowsY[0]), sf::Vector2f(pX1, rowsY[2]));
    drawDiag(sf::Vector2f(pX1, rowsY[3]), sf::Vector2f(pX2, rowsY[5]));
    drawDiag(sf::Vector2f(pX2, rowsY[3]), sf::Vector2f(pX1, rowsY[5]));

    sf::Text riverText;
    riverText.setFont(fBold);
    riverText.setCharacterSize(10);
    riverText.setStyle(sf::Text::Bold);
    riverText.setString(toUtf8(vi ? "— SỞ HÀ • HÁN GIỚI —" : "— CHU RIVER • HAN BORDER —"));
    riverText.setFillColor(riverCol);
    sf::FloatRect rtb = riverText.getLocalBounds();
    riverText.setOrigin(std::round(rtb.left + rtb.width / 2.0f), std::round(rtb.top + rtb.height / 2.0f));
    riverText.setPosition(std::round(canvasX + canvasW / 2.0f), std::round(canvasY + canvasH / 2.0f - 0.5f));
    win.draw(riverText);

    float dotR = 5.0f;
    sf::CircleShape dotBlack(dotR);
    dotBlack.setOrigin(dotR, dotR);
    dotBlack.setPosition(canvasX + 7.0f + 4.0f * colSpacing, rowsY[0]);
    dotBlack.setFillColor(sf::Color(30, 36, 48));
    dotBlack.setOutlineThickness(1.0f);
    dotBlack.setOutlineColor(sf::Color(190, 215, 240));
    win.draw(dotBlack);

    sf::CircleShape dotRed(dotR);
    dotRed.setOrigin(dotR, dotR);
    dotRed.setPosition(canvasX + 7.0f + 4.0f * colSpacing, rowsY[5]);
    dotRed.setFillColor(sf::Color(175, 25, 20));
    dotRed.setOutlineThickness(1.0f);
    dotRed.setOutlineColor(sf::Color(255, 225, 120));
    win.draw(dotRed);
}

void GameManager::vePlaySetup() {
    auto& assets = getMenuAssets();
    bool tiengViet = (Loc::getLanguage() == Language::TIENG_VIET);
    sf::Font& fBold = assets.fontBoldLoaded ? assets.fontBold : fontBold;

    // 1. Fullscreen Dim Backdrop over High-Res Landscape
    FantasyUI::get().drawFullDimOverlay(window, sf::Color(10, 8, 12, 175));

    // 2. Main Oriental Dashboard Frame (960x650 at X=120, Y=75) — NO TOP SCROLL BANNER TITLE!
    const float frameX = 120.0f;
    const float frameY = 75.0f;
    const float frameW = 960.0f;
    const float frameH = 650.0f;

    FantasyUI::get().drawPanel(window, sf::FloatRect(frameX, frameY, frameW, frameH), OrientalPanelStyle::EBONY_WOOD);

    sf::RectangleShape inner(sf::Vector2f(frameW - 16.0f, frameH - 16.0f));
    inner.setPosition(frameX + 8.0f, frameY + 8.0f);
    inner.setFillColor(sf::Color(22, 16, 12, 235));
    inner.setOutlineThickness(1.2f);
    inner.setOutlineColor(sf::Color(140, 102, 50, 160));
    window.draw(inner);

    ve4GocLuaChon(window, sf::Vector2f(frameX + 8.0f, frameY + 8.0f), sf::Vector2f(frameW - 16.0f, frameH - 16.0f), 2.0f, 1.2f);

    auto drawSubCard = [this](sf::FloatRect rect, sf::Color tintCol) {
        FantasyUI::get().drawPanel(window, rect, OrientalPanelStyle::EBONY_WOOD, tintCol);
    };

    const float cardY = frameY + 18.0f;  // 93.0f
    const float cardH = 175.0f;
    const float cardW = 444.0f;
    const float card1X = frameX + 22.0f; // 142.0f
    const float card2X = 614.0f;

    if (playSetupMode == 1) {
        // --- 1. Two-Player PvP Setup ---
        // Left Card: Player 1 (Red Side) [142, 93, 444, 175]
        drawSubCard(sf::FloatRect(card1X, cardY, cardW, cardH), sf::Color(255, 220, 220));

        sf::RectangleShape h1(sf::Vector2f(cardW - 16.0f, 28.0f));
        h1.setPosition(card1X + 8.0f, cardY + 8.0f);
        h1.setFillColor(sf::Color(80, 24, 24, 220));
        h1.setOutlineThickness(1.0f);
        h1.setOutlineColor(sf::Color(160, 55, 50, 170));
        window.draw(h1);

        sf::Text t1;
        t1.setFont(fBold);
        t1.setCharacterSize(15);
        t1.setStyle(sf::Text::Bold);
        t1.setString(toUtf8(tiengViet ? "[ NGƯỜI CHƠI 1 ]" : "[ PLAYER 1 ]"));
        t1.setFillColor(sf::Color(255, 230, 224));
        sf::FloatRect tb = t1.getLocalBounds();
        t1.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));
        t1.setPosition(card1X + cardW / 2.0f, cardY + 22.0f);
        window.draw(t1);

        // Name
        sf::Text lName1;
        lName1.setFont(fBold);
        lName1.setString(toUtf8(tiengViet ? "TÊN:" : "NAME:"));
        lName1.setCharacterSize(14);
        lName1.setStyle(sf::Text::Bold);
        lName1.setFillColor(sf::Color(255, 235, 165));
        sf::FloatRect nb1 = lName1.getLocalBounds();
        lName1.setOrigin(0.0f, std::round(nb1.top + nb1.height / 2.0f));
        lName1.setPosition(card1X + 16.0f, cardY + 65.0f);
        window.draw(lName1);

        inputP1Name->draw(window);

        // Role / Side
        sf::Text lRole1;
        lRole1.setFont(fBold);
        lRole1.setString(toUtf8(tiengViet ? "VAI TRÒ:" : "ROLE:"));
        lRole1.setCharacterSize(14);
        lRole1.setStyle(sf::Text::Bold);
        lRole1.setFillColor(sf::Color(255, 235, 165));
        sf::FloatRect rb1 = lRole1.getLocalBounds();
        lRole1.setOrigin(0.0f, std::round(rb1.top + rb1.height / 2.0f));
        lRole1.setPosition(card1X + 16.0f, cardY + 115.0f);
        window.draw(lRole1);

        // P1 Avatar Portrait (Circular with red/gold rim)
        Mau p1Mau = (pvpFirstPlayer == 1) ? Mau::DO : Mau::DEN;
        veAvatarPortrait(sf::Vector2f(514.0f, cardY + 84.0f), 32.0f, p1CharIndex, p1Mau, false);

        // Right Card: Player 2 (Black Side) [614, 93, 444, 175]
        drawSubCard(sf::FloatRect(card2X, cardY, cardW, cardH), sf::Color(215, 230, 255));

        sf::RectangleShape h2(sf::Vector2f(cardW - 16.0f, 28.0f));
        h2.setPosition(card2X + 8.0f, cardY + 8.0f);
        h2.setFillColor(sf::Color(24, 38, 58, 220));
        h2.setOutlineThickness(1.0f);
        h2.setOutlineColor(sf::Color(55, 90, 145, 170));
        window.draw(h2);

        sf::Text t2;
        t2.setFont(fBold);
        t2.setCharacterSize(15);
        t2.setStyle(sf::Text::Bold);
        t2.setString(toUtf8(tiengViet ? "[ NGƯỜI CHƠI 2 ]" : "[ PLAYER 2 ]"));
        t2.setFillColor(sf::Color(215, 235, 255));
        tb = t2.getLocalBounds();
        t2.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));
        t2.setPosition(card2X + cardW / 2.0f, cardY + 22.0f);
        window.draw(t2);

        sf::Text lName2;
        lName2.setFont(fBold);
        lName2.setString(toUtf8(tiengViet ? "TÊN:" : "NAME:"));
        lName2.setCharacterSize(14);
        lName2.setStyle(sf::Text::Bold);
        lName2.setFillColor(sf::Color(255, 235, 165));
        sf::FloatRect nb2 = lName2.getLocalBounds();
        lName2.setOrigin(0.0f, std::round(nb2.top + nb2.height / 2.0f));
        lName2.setPosition(card2X + 16.0f, cardY + 65.0f);
        window.draw(lName2);

        inputP2Name->draw(window);

        sf::Text lRole2;
        lRole2.setFont(fBold);
        lRole2.setString(toUtf8(tiengViet ? "VAI TRÒ:" : "ROLE:"));
        lRole2.setCharacterSize(14);
        lRole2.setStyle(sf::Text::Bold);
        lRole2.setFillColor(sf::Color(255, 235, 165));
        sf::FloatRect rb2 = lRole2.getLocalBounds();
        lRole2.setOrigin(0.0f, std::round(rb2.top + rb2.height / 2.0f));
        lRole2.setPosition(card2X + 16.0f, cardY + 115.0f);
        window.draw(lRole2);

        Mau p2Mau = (pvpFirstPlayer == 1) ? Mau::DEN : Mau::DO;
        veAvatarPortrait(sf::Vector2f(986.0f, cardY + 84.0f), 32.0f, p2CharIndex, p2Mau, false);

    } else {
        // --- 2. Player vs AI Setup ---
        // Left Card: Human Player [142, 93, 444, 175]
        drawSubCard(sf::FloatRect(card1X, cardY, cardW, cardH), sf::Color(255, 225, 205));

        sf::RectangleShape h1(sf::Vector2f(cardW - 16.0f, 28.0f));
        h1.setPosition(card1X + 8.0f, cardY + 8.0f);
        h1.setFillColor(sf::Color(80, 24, 24, 220));
        h1.setOutlineThickness(1.0f);
        h1.setOutlineColor(sf::Color(160, 55, 50, 170));
        window.draw(h1);

        sf::Text t1;
        t1.setFont(fBold);
        t1.setCharacterSize(15);
        t1.setStyle(sf::Text::Bold);
        t1.setString(toUtf8(tiengViet ? "[ KỲ THỦ (NGƯỜI CHƠI) ]" : "[ HUMAN PLAYER ]"));
        t1.setFillColor(sf::Color(255, 230, 224));
        sf::FloatRect tb = t1.getLocalBounds();
        t1.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));
        t1.setPosition(card1X + cardW / 2.0f, cardY + 22.0f);
        window.draw(t1);

        sf::Text lName1;
        lName1.setFont(fBold);
        lName1.setString(toUtf8(tiengViet ? "TÊN:" : "NAME:"));
        lName1.setCharacterSize(14);
        lName1.setStyle(sf::Text::Bold);
        lName1.setFillColor(sf::Color(255, 235, 165));
        sf::FloatRect nb1 = lName1.getLocalBounds();
        lName1.setOrigin(0.0f, std::round(nb1.top + nb1.height / 2.0f));
        lName1.setPosition(card1X + 16.0f, cardY + 65.0f);
        window.draw(lName1);

        inputAiPlayerName->draw(window);

        sf::Text lRole1;
        lRole1.setFont(fBold);
        lRole1.setString(toUtf8(tiengViet ? "VAI TRÒ:" : "ROLE:"));
        lRole1.setCharacterSize(14);
        lRole1.setStyle(sf::Text::Bold);
        lRole1.setFillColor(sf::Color(255, 235, 165));
        sf::FloatRect rb1 = lRole1.getLocalBounds();
        lRole1.setOrigin(0.0f, std::round(rb1.top + rb1.height / 2.0f));
        lRole1.setPosition(card1X + 16.0f, cardY + 115.0f);
        window.draw(lRole1);

        Mau humanMau = (aiFirstChoice == 1) ? Mau::DEN : Mau::DO;
        veAvatarPortrait(sf::Vector2f(514.0f, cardY + 84.0f), 32.0f, aiPlayerCharIndex, humanMau, false);

        // Right Card: AI Opponent [614, 93, 444, 175]
        drawSubCard(sf::FloatRect(card2X, cardY, cardW, cardH), sf::Color(215, 230, 255));

        sf::RectangleShape h2(sf::Vector2f(cardW - 16.0f, 28.0f));
        h2.setPosition(card2X + 8.0f, cardY + 8.0f);
        h2.setFillColor(sf::Color(24, 38, 58, 220));
        h2.setOutlineThickness(1.0f);
        h2.setOutlineColor(sf::Color(55, 90, 145, 170));
        window.draw(h2);

        sf::Text t2;
        t2.setFont(fBold);
        t2.setCharacterSize(15);
        t2.setStyle(sf::Text::Bold);
        t2.setString(toUtf8(tiengViet ? "[ ĐỐI THỦ MÁY (AI ENGINE) ]" : "[ AI OPPONENT ]"));
        t2.setFillColor(sf::Color(215, 235, 255));
        tb = t2.getLocalBounds();
        t2.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));
        t2.setPosition(card2X + cardW / 2.0f, cardY + 22.0f);
        window.draw(t2);

        sf::Text lDiff;
        lDiff.setFont(fBold);
        lDiff.setString(toUtf8(tiengViet ? "ĐỘ KHÓ:" : "DIFF:"));
        lDiff.setCharacterSize(14);
        lDiff.setStyle(sf::Text::Bold);
        lDiff.setFillColor(sf::Color(255, 235, 165));
        sf::FloatRect db = lDiff.getLocalBounds();
        lDiff.setOrigin(0.0f, std::round(db.top + db.height / 2.0f));
        lDiff.setPosition(card2X + 16.0f, cardY + 65.0f);
        window.draw(lDiff);

        sf::Text lFirst;
        lFirst.setFont(fBold);
        lFirst.setString(toUtf8(tiengViet ? "ĐI TRƯỚC:" : "SIDE:"));
        lFirst.setCharacterSize(14);
        lFirst.setStyle(sf::Text::Bold);
        lFirst.setFillColor(sf::Color(255, 235, 165));
        sf::FloatRect fb = lFirst.getLocalBounds();
        lFirst.setOrigin(0.0f, std::round(fb.top + fb.height / 2.0f));
        lFirst.setPosition(card2X + 16.0f, cardY + 115.0f);
        window.draw(lFirst);

        Mau aiMau = (aiFirstChoice == 1) ? Mau::DO : Mau::DEN;
        veAvatarPortrait(sf::Vector2f(986.0f, cardY + 84.0f), 32.0f, 0, aiMau, false);
    }

    // --- 3. Match Options Panel: "TÙY CHỌN TRẬN ĐẤU" (916x300 at 142, 284) ---
    const float secX = 142.0f;
    const float secY = 284.0f;
    const float secW = 916.0f;
    const float secH = 300.0f;

    FantasyUI::get().drawPanel(window, sf::FloatRect(secX, secY, secW, secH), OrientalPanelStyle::EBONY_WOOD);
    sf::RectangleShape secInset(sf::Vector2f(secW - 8.0f, secH - 8.0f));
    secInset.setPosition(secX + 4.0f, secY + 4.0f);
    secInset.setFillColor(sf::Color(22, 16, 12, 230));
    secInset.setOutlineThickness(1.0f);
    secInset.setOutlineColor(sf::Color(135, 95, 45, 140));
    window.draw(secInset);
    ve4GocLuaChon(window, sf::Vector2f(secX + 4.0f, secY + 4.0f), sf::Vector2f(secW - 8.0f, secH - 8.0f), 1.0f, 1.0f);

    // Section Header Title
    sf::Text rTitle;
    rTitle.setFont(fBold);
    rTitle.setCharacterSize(15);
    rTitle.setStyle(sf::Text::Bold);
    rTitle.setString(toUtf8(tiengViet ? "TÙY CHỌN TRẬN ĐẤU" : "MATCH SETTINGS"));
    rTitle.setFillColor(sf::Color(255, 225, 120));
    rTitle.setPosition(secX + 16.0f, secY + 12.0f);
    window.draw(rTitle);

    // 4 Column Labels
    const float colW = 210.0f;
    const float col1X = 158.0f;
    const float col2X = 382.0f;
    const float col3X = 606.0f;
    const float col4X = 830.0f;
    const float colHeaderY = secY + 38.0f;

    auto drawColHeader = [&](float cx, const std::string& title) {
        sf::Text th;
        th.setFont(fBold);
        th.setCharacterSize(13);
        th.setStyle(sf::Text::Bold);
        th.setString(toUtf8(title));
        th.setFillColor(sf::Color(245, 230, 185));
        th.setPosition(cx + 6.0f, colHeaderY);
        window.draw(th);
    };

    drawColHeader(col1X, tiengViet ? "THỜI GIAN MỖI BÊN:" : "TIME PER SIDE:");
    drawColHeader(col2X, tiengViet ? "LUẬT CHƠI:" : "MATCH RULES:");
    drawColHeader(col3X, tiengViet ? "CHỦ ĐỀ BÀN CỜ:" : "BOARD THEME:");
    drawColHeader(col4X, tiengViet ? "KIỂU QUÂN CỜ:" : "PIECE STYLE:");

    const float boxY = secY + 104.0f; // 388.0f
    const float boxH = 180.0f;

    // --- Column 1 Display Box: Antique Hourglass ---
    sf::RectangleShape b1(sf::Vector2f(colW, boxH));
    b1.setPosition(col1X, boxY);
    b1.setFillColor(sf::Color(20, 16, 12, 230));
    b1.setOutlineThickness(1.2f);
    b1.setOutlineColor(sf::Color(115, 85, 45, 160));
    window.draw(b1);
    ve4GocLuaChon(window, sf::Vector2f(col1X, boxY), sf::Vector2f(colW, boxH), 1.0f, 1.0f);

    veDongHoCat(window, sf::Vector2f(col1X + colW / 2.0f, boxY + boxH * 0.44f), 108.0f);

    sf::Text timeStatusLbl;
    timeStatusLbl.setFont(fBold);
    timeStatusLbl.setCharacterSize(12);
    timeStatusLbl.setStyle(sf::Text::Bold);
    std::string timeStr = (thoiGianVanDau == 0.0f)
        ? (tiengViet ? "Không Giới Hạn" : "Unlimited Time")
        : (tiengViet ? "Đếm ngược theo lượt" : "Turn countdown");
    timeStatusLbl.setString(toUtf8(timeStr));
    timeStatusLbl.setFillColor(sf::Color(245, 225, 160));
    sf::FloatRect tsb = timeStatusLbl.getLocalBounds();
    timeStatusLbl.setOrigin(std::round(tsb.left + tsb.width / 2.0f), std::round(tsb.top + tsb.height / 2.0f));
    timeStatusLbl.setPosition(std::round(col1X + colW / 2.0f), std::round(boxY + boxH - 20.0f));
    window.draw(timeStatusLbl);

    // --- Column 2 Display Box: 3 Rule Checkboxes ---
    sf::RectangleShape b2(sf::Vector2f(colW, boxH));
    b2.setPosition(col2X, boxY);
    b2.setFillColor(sf::Color(20, 16, 12, 230));
    b2.setOutlineThickness(1.2f);
    b2.setOutlineColor(sf::Color(115, 85, 45, 160));
    window.draw(b2);
    ve4GocLuaChon(window, sf::Vector2f(col2X, boxY), sf::Vector2f(colW, boxH), 1.0f, 1.0f);

    auto drawCheckItem = [&](float py, const std::string& line1, const std::string& line2) {
        auto& am = AssetManager::get();
        const sf::Texture& cbTex = am.texture(TextureId::CHECKBOX_CHECKED);
        if (cbTex.getSize().x > 0) {
            sf::Sprite cbSp(cbTex);
            float cs = 18.0f / static_cast<float>(cbTex.getSize().x);
            cbSp.setScale(cs, cs);
            cbSp.setPosition(col2X + 10.0f, py + 1.0f);
            window.draw(cbSp);
        } else {
            // Fallback checkbox square
            sf::RectangleShape cb(sf::Vector2f(14.0f, 14.0f));
            cb.setPosition(col2X + 12.0f, py + 2.0f);
            cb.setFillColor(sf::Color(18, 32, 22, 240));
            cb.setOutlineThickness(1.2f);
            cb.setOutlineColor(sf::Color(215, 175, 80));
            window.draw(cb);

            sf::Text chk;
            chk.setFont(fBold);
            chk.setCharacterSize(12);
            chk.setStyle(sf::Text::Bold);
            chk.setString(toUtf8("✓"));
            chk.setFillColor(sf::Color(90, 220, 130));
            sf::FloatRect cbBounds = chk.getLocalBounds();
            chk.setOrigin(std::round(cbBounds.left + cbBounds.width / 2.0f), std::round(cbBounds.top + cbBounds.height / 2.0f));
            chk.setPosition(col2X + 19.0f, py + 8.0f);
            window.draw(chk);
        }

        sf::Text txt1;
        txt1.setFont(fBold);
        txt1.setCharacterSize(13);
        txt1.setStyle(sf::Text::Bold);
        txt1.setString(toUtf8(line1));
        txt1.setFillColor(sf::Color(255, 240, 200));
        txt1.setPosition(col2X + 34.0f, py - 2.0f);
        window.draw(txt1);

        if (!line2.empty()) {
            sf::Text txt2;
            txt2.setFont(fBold);
            txt2.setCharacterSize(12);
            txt2.setStyle(sf::Text::Bold);
            txt2.setString(toUtf8(line2));
            txt2.setFillColor(sf::Color(215, 195, 155));
            txt2.setPosition(col2X + 34.0f, py + 16.0f);
            window.draw(txt2);
        }
    };

    drawCheckItem(boxY + 22.0f,
                  tiengViet ? "Đi theo luật tiêu chuẩn" : "Standard move rules",
                  tiengViet ? "(Mã nhật, Tượng điền...)" : "(Standard movements)");
    drawCheckItem(boxY + 74.0f,
                  tiengViet ? "Không cộng giờ suy nghĩ" : "No move increment",
                  tiengViet ? "(Cố định mỗi lượt)" : "(Fixed clock per side)");
    drawCheckItem(boxY + 126.0f,
                  tiengViet ? "Hòa theo luật cơ bản" : "Standard draw rules",
                  tiengViet ? "(Đồng thuận hoặc 60 nước)" : "(Agreed or 60 moves)");

    // --- Column 3 Display Box: Board Theme Preview ---
    veBoardThemePreview(window, col3X, boxY, colW, boxH);

    // --- Column 4 Display Box: Piece Style Preview ---
    vePieceStylePreview(window, col4X, boxY, colW, boxH);

    // 4. Interactive Action Buttons
    for (auto& btn : playSetupButtons) {
        btn->draw(window);
    }

    // 5. Minimalist Footer Prompt
    sf::Text footer;
    footer.setFont(fBold);
    footer.setString(toUtf8(tiengViet
        ? "[Enter / Phím Space] Tiếp tục  •  [ESC] Quay lại menu"
        : "[Enter / Space] Continue  •  [ESC] Back to menu"));
    footer.setCharacterSize(13);
    footer.setStyle(sf::Text::Bold);
    footer.setFillColor(sf::Color(210, 185, 135, 200));
    sf::FloatRect fb = footer.getLocalBounds();
    footer.setOrigin(std::round(fb.left + fb.width / 2.0f), std::round(fb.top + fb.height / 2.0f));
    footer.setPosition(600.0f, 744.0f);
    window.draw(footer);
}

void GameManager::xuLyClickPlaySetup(const sf::Vector2i& viTri) {
    if (playSetupMode == 0) {
        inputAiPlayerName->handleClick(viTri);
    } else if (playSetupMode == 1) {
        inputP1Name->handleClick(viTri);
        inputP2Name->handleClick(viTri);
    }

    // Direct click on avatars to cycle character
    float r = 36.0f;
    float p1AvaX = 514.0f;
    float p1AvaY = 177.0f;
    float d1 = std::hypot(static_cast<float>(viTri.x) - p1AvaX, static_cast<float>(viTri.y) - p1AvaY);
    if (d1 <= r) {
        soundManager.play(SoundType::CLICK);
        if (playSetupMode == 1) {
            p1CharIndex = (p1CharIndex + 1) % Loc::getCharCount();
        } else {
            aiPlayerCharIndex = (aiPlayerCharIndex + 1) % Loc::getCharCount();
        }
        khoiTaoPlaySetupButtons();
        return;
    }

    if (playSetupMode == 1) {
        float p2AvaX = 986.0f;
        float p2AvaY = 177.0f;
        float d2 = std::hypot(static_cast<float>(viTri.x) - p2AvaX, static_cast<float>(viTri.y) - p2AvaY);
        if (d2 <= r) {
            soundManager.play(SoundType::CLICK);
            p2CharIndex = (p2CharIndex + 1) % Loc::getCharCount();
            khoiTaoPlaySetupButtons();
            return;
        }
    }

    for (size_t i = 0; i < playSetupButtons.size(); ++i) {
        if (playSetupButtons[i]->contains(viTri)) {
            playSetupButtons[i]->handleClick(viTri);
            break;
        }
    }
}


void GameManager::veLoadMenu() {
    FantasyUI::get().drawFullDimOverlay(window, sf::Color(10, 8, 12, 185));
    bool tiengViet = (Loc::getLanguage() == Language::TIENG_VIET);
    auto& am = AssetManager::get();

    // 1. Center Modal Panel (510px height, centered vertically at Y = 105)
    FantasyUI::get().drawPanel(window, sf::FloatRect(126.0f, 105.0f, 948.0f, 510.0f), OrientalPanelStyle::EBONY_WOOD);

    for (int i = 0; i < 3; ++i) {
        float sy = 125.0f + i * 154.0f;
        sf::FloatRect cardRect(150.0f, sy, 900.0f, 138.0f);
        bool isSel = (selectedSlot == i);

        FantasyUI::get().drawPanel(window, cardRect,
            isSel ? OrientalPanelStyle::PARCHMENT_SCROLL : OrientalPanelStyle::EBONY_WOOD);
        if (isSel) {
            ve4GocLuaChon(window, sf::Vector2f(150.0f, sy), sf::Vector2f(900.0f, 138.0f), 2.0f, 2.0f);
        }

        // Slot Number Badge
        sf::Text slotHeader;
        slotHeader.setFont(fontBold);
        slotHeader.setCharacterSize(20);
        slotHeader.setStyle(sf::Text::Bold);
        slotHeader.setString(toUtf8(Loc::get(LocKey::SLOT_LABEL) + std::to_string(i + 1)));
        slotHeader.setFillColor(isSel ? sf::Color(165, 25, 20) : sf::Color(245, 215, 120));
        slotHeader.setPosition(180.0f, sy + 16.0f);
        window.draw(slotHeader);

        const SaveSlotInfo& info = slotInfos[i];
        if (info.tonTai) {
            // Mode Badge
            bool isPvp = (info.tenCheDo.find("Hai") != std::string::npos || info.tenCheDo.find("Two") != std::string::npos || info.tenCheDo.find("PvP") != std::string::npos);
            sf::RectangleShape modeBadge(sf::Vector2f(180.0f, 26.0f));
            modeBadge.setPosition(330.0f, sy + 16.0f);
            modeBadge.setFillColor(isPvp ? sf::Color(115, 28, 22, 230) : sf::Color(22, 45, 80, 230));
            modeBadge.setOutlineThickness(1.0f);
            modeBadge.setOutlineColor(isSel ? sf::Color(165, 30, 25) : sf::Color(215, 175, 75));
            window.draw(modeBadge);

            sf::Text modeTxt;
            modeTxt.setFont(fontBold);
            modeTxt.setCharacterSize(12);
            modeTxt.setStyle(sf::Text::Bold);
            modeTxt.setString(toUtf8(info.tenCheDo));
            modeTxt.setFillColor(sf::Color(255, 245, 210));
            sf::FloatRect mb = modeTxt.getLocalBounds();
            modeTxt.setOrigin(std::round(mb.left + mb.width / 2.0f), std::round(mb.top + mb.height / 2.0f));
            modeTxt.setPosition(330.0f + 90.0f, sy + 16.0f + 13.0f);
            window.draw(modeTxt);

            // Date / Time Info
            sf::Text detDate;
            detDate.setFont(fontBold);
            detDate.setCharacterSize(14);
            detDate.setStyle(sf::Text::Bold);
            detDate.setString(toUtf8((tiengViet ? "Thời gian: " : "Saved on: ") + info.thoiGian));
            detDate.setFillColor(isSel ? sf::Color(80, 45, 25) : sf::Color(220, 195, 150));
            detDate.setPosition(535.0f, sy + 20.0f);
            window.draw(detDate);

            // Player Matchup Details
            sf::CircleShape redDot(6.0f);
            redDot.setFillColor(sf::Color(215, 45, 35));
            redDot.setOutlineThickness(1.0f);
            redDot.setOutlineColor(sf::Color(255, 220, 120));
            redDot.setPosition(180.0f, sy + 58.0f);
            window.draw(redDot);

            sf::Text rName;
            rName.setFont(fontBold);
            rName.setCharacterSize(15);
            rName.setStyle(sf::Text::Bold);
            rName.setString(toUtf8((tiengViet ? "Đỏ: " : "Red: ") + info.tenDo));
            rName.setFillColor(isSel ? sf::Color(135, 25, 20) : sf::Color(255, 225, 135));
            rName.setPosition(200.0f, sy + 52.0f);
            window.draw(rName);

            sf::CircleShape blackDot(6.0f);
            blackDot.setFillColor(sf::Color(35, 65, 115));
            blackDot.setOutlineThickness(1.0f);
            blackDot.setOutlineColor(sf::Color(255, 220, 120));
            blackDot.setPosition(400.0f, sy + 58.0f);
            window.draw(blackDot);

            sf::Text bName;
            bName.setFont(fontBold);
            bName.setCharacterSize(15);
            bName.setStyle(sf::Text::Bold);
            bName.setString(toUtf8((tiengViet ? "Đen: " : "Black: ") + info.tenDen));
            bName.setFillColor(isSel ? sf::Color(25, 45, 80) : sf::Color(200, 225, 255));
            bName.setPosition(420.0f, sy + 52.0f);
            window.draw(bName);

            // Move count
            sf::Text detStats;
            detStats.setFont(fontBold);
            detStats.setCharacterSize(14);
            detStats.setStyle(sf::Text::Bold);
            std::string statsStr = (tiengViet ? "Đã đi: " : "Total Moves: ") + std::to_string(info.soNuoc) + (tiengViet ? " nước" : " moves");
            detStats.setString(toUtf8(statsStr));
            detStats.setFillColor(isSel ? sf::Color(90, 55, 30) : sf::Color(235, 210, 165));
            detStats.setPosition(180.0f, sy + 92.0f);
            window.draw(detStats);

            // Turn Indicator Plaque
            bool isRedTurn = (info.luot == Mau::DO);
            sf::RectangleShape turnBadge(sf::Vector2f(210.0f, 32.0f));
            turnBadge.setPosition(710.0f, sy + 52.0f);
            turnBadge.setFillColor(isRedTurn ? sf::Color(130, 28, 22, 240) : sf::Color(28, 52, 95, 240));
            turnBadge.setOutlineThickness(1.5f);
            turnBadge.setOutlineColor(sf::Color(255, 215, 95));
            window.draw(turnBadge);
            ve4GocLuaChon(window, sf::Vector2f(710.0f, sy + 52.0f), sf::Vector2f(210.0f, 32.0f), 1.0f, 1.0f);

            sf::Text turnTxt;
            turnTxt.setFont(fontBold);
            turnTxt.setCharacterSize(13);
            turnTxt.setStyle(sf::Text::Bold);
            std::string turnLabel = isRedTurn ? (tiengViet ? "LƯỢT ĐI: BÊN ĐỎ" : "CURRENT: RED SIDE")
                                              : (tiengViet ? "LƯỢT ĐI: BÊN ĐEN" : "CURRENT: BLACK SIDE");
            turnTxt.setString(toUtf8(turnLabel));
            turnTxt.setFillColor(sf::Color(255, 245, 205));
            sf::FloatRect tb = turnTxt.getLocalBounds();
            turnTxt.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));
            turnTxt.setPosition(710.0f + 105.0f, sy + 52.0f + 16.0f);
            window.draw(turnTxt);
        } else {
            sf::Text emp;
            emp.setFont(fontBold);
            emp.setCharacterSize(16);
            emp.setStyle(sf::Text::Bold);
            emp.setString(toUtf8(tiengViet ? "Ô Nhớ Trống  (Chưa có dữ liệu ván đấu)" : "Empty Save Slot  (No saved game data)"));
            emp.setFillColor(isSel ? sf::Color(125, 80, 45) : sf::Color(170, 145, 115));
            emp.setPosition(180.0f, sy + 64.0f);
            window.draw(emp);
        }
    }

    menuLoad.draw(window);
}

void GameManager::veSettingsMenu() {
    bool tiengViet = (Loc::getLanguage() == Language::TIENG_VIET);
    auto& mAssets = getMenuAssets();
    sf::Font& fTitle = tiengViet ? fontVnBold : fontEnBold;
    sf::Font& f = mAssets.vnFontLoaded ? mAssets.vnFont : font;
    sf::Font& fBold = mAssets.fontBoldLoaded ? mAssets.fontBold : fontBold;
    auto& am = AssetManager::get();

    // 1. Subtle Dim Backdrop over High-Res Landscape
    FantasyUI::get().drawFullDimOverlay(window, sf::Color(10, 8, 12, 190));

    // Smoothstep transition progress
    float t = settingsSlideProgress * settingsSlideProgress * (3.0f - 2.0f * settingsSlideProgress);

    // 2. Left / Center Panel Geometry (Smooth Master-Detail Sliding Drawer)
    float panelX = lerpVal(360.0f, 110.0f, t);
    float panelY = lerpVal(130.0f, 85.0f, t);
    float panelW = lerpVal(480.0f, 250.0f, t);
    float panelH = lerpVal(540.0f, 635.0f, t);

    // Draw Left/Center Panel
    FantasyUI::get().drawPanel(window, sf::FloatRect(panelX, panelY, panelW, panelH), OrientalPanelStyle::EBONY_WOOD);

    sf::RectangleShape leftInner(sf::Vector2f(panelW - 16.0f, panelH - 16.0f));
    leftInner.setPosition(panelX + 8.0f, panelY + 8.0f);
    leftInner.setFillColor(sf::Color(22, 16, 12, 235));
    leftInner.setOutlineThickness(1.2f);
    leftInner.setOutlineColor(sf::Color(160, 120, 55, 170));
    window.draw(leftInner);
    ve4GocLuaChon(window, sf::Vector2f(panelX + 8.0f, panelY + 8.0f), sf::Vector2f(panelW - 16.0f, panelH - 16.0f), 1.2f, 1.2f);

    // Header inside Left/Center panel (Clean titles without glyph squares)
    if (t < 0.5f) {
        sf::Text titleText;
        titleText.setFont(fTitle);
        titleText.setString(toUtf8(tiengViet ? "CHỌN MỤC THIẾT LẬP" : "SELECT CATEGORY"));
        titleText.setCharacterSize(16);
        titleText.setStyle(sf::Text::Bold);
        titleText.setFillColor(sf::Color(255, 225, 130));
        sf::FloatRect tb = titleText.getLocalBounds();
        titleText.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));
        titleText.setPosition(panelX + panelW / 2.0f, panelY + 28.0f);
        window.draw(titleText);

        sf::RectangleShape sep(sf::Vector2f(panelW - 60.0f, 1.5f));
        sep.setPosition(panelX + 30.0f, panelY + 44.0f);
        sep.setFillColor(sf::Color(170, 130, 60, 180));
        window.draw(sep);
    } else {
        sf::Text catTitle;
        catTitle.setFont(fTitle);
        catTitle.setString(toUtf8(tiengViet ? "THIẾT LẬP" : "SETTINGS"));
        catTitle.setCharacterSize(17);
        catTitle.setStyle(sf::Text::Bold);
        catTitle.setFillColor(sf::Color(255, 225, 130));
        sf::FloatRect cb = catTitle.getLocalBounds();
        catTitle.setOrigin(std::round(cb.left + cb.width / 2.0f), std::round(cb.top + cb.height / 2.0f));
        catTitle.setPosition(panelX + panelW / 2.0f, panelY + 28.0f);
        window.draw(catTitle);

        sf::RectangleShape sep(sf::Vector2f(panelW - 40.0f, 1.5f));
        sep.setPosition(panelX + 20.0f, panelY + 44.0f);
        sep.setFillColor(sf::Color(170, 130, 60, 180));
        window.draw(sep);
    }

    // Draw the 4 Category Buttons with authentic Chinese pixel art icons
    const sf::Texture& btnTex = am.texture(TextureId::TITLE_BTN_FRAME);
    const TextureId categoryIconIds[4] = {
        TextureId::ICON_TAB_SOUND,
        TextureId::ICON_TAB_GAMEPLAY,
        TextureId::ICON_TAB_LANGUAGE,
        TextureId::ICON_TAB_CONTROLS
    };

    for (size_t i = 0; i < settingsTabButtons.size() && i < 4; ++i) {
        bool isActive = (static_cast<int>(activeSettingsTab) == static_cast<int>(i));
        sf::Vector2f pos = settingsTabButtons[i]->getPosition();
        sf::Vector2f sz = settingsTabButtons[i]->getSize();
        bool isHovered = settingsTabButtons[i]->isHovered();

        const sf::Texture& iconTex = am.texture(categoryIconIds[i]);

        std::string catName;
        if (i == 0) catName = tiengViet ? "ÂM THANH" : "SOUND";
        else if (i == 1) catName = tiengViet ? "LỐI CHƠI" : "GAMEPLAY";
        else if (i == 2) catName = tiengViet ? "NGÔN NGỮ" : "LANGUAGE";
        else if (i == 3) catName = tiengViet ? "ĐIỀU KHIỂN" : "CONTROLS";

        if (t < 0.5f) {
            // Centered Popup Mode: Draw authentic vermilion lacquer plaque button (title_btn_frame.png)
            if (btnTex.getSize().x > 0) {
                sf::Sprite btnSprite(btnTex);
                float bw = static_cast<float>(btnTex.getSize().x);
                float bh = static_cast<float>(btnTex.getSize().y);
                btnSprite.setOrigin(bw / 2.0f, bh / 2.0f);
                btnSprite.setScale(sz.x / bw, sz.y / bh);
                btnSprite.setPosition(pos.x + sz.x / 2.0f, pos.y + sz.y / 2.0f);

                if (isHovered) {
                    sf::RectangleShape glow(sf::Vector2f(sz.x + 10.0f, sz.y + 6.0f));
                    glow.setOrigin((sz.x + 10.0f) / 2.0f, (sz.y + 6.0f) / 2.0f);
                    glow.setPosition(pos.x + sz.x / 2.0f, pos.y + sz.y / 2.0f);
                    glow.setFillColor(sf::Color(255, 215, 80, 45));
                    glow.setOutlineThickness(1.5f);
                    glow.setOutlineColor(sf::Color(255, 230, 120, 190));
                    window.draw(glow);
                    btnSprite.setColor(sf::Color(255, 248, 225));
                } else {
                    btnSprite.setColor(sf::Color(255, 255, 255));
                }
                window.draw(btnSprite);
            } else {
                settingsTabButtons[i]->draw(window);
            }

            // Category text and icon side by side (clean layout)
            sf::Text catTxt;
            catTxt.setFont(fTitle);
            catTxt.setString(toUtf8(catName));
            catTxt.setCharacterSize(19);
            catTxt.setStyle(sf::Text::Bold);
            sf::FloatRect ctb = catTxt.getLocalBounds();
            catTxt.setOrigin(0.0f, std::round(ctb.top + ctb.height / 2.0f));

            float iconSize = 26.0f;
            float iconGap = 12.0f;
            float totalW = iconSize + iconGap + ctb.width;
            float startX = pos.x + (sz.x - totalW) / 2.0f;

            // Draw Pixel Art Icon
            if (iconTex.getSize().x > 0) {
                sf::Sprite iconSp(iconTex);
                float sc = iconSize / static_cast<float>(iconTex.getSize().x);
                iconSp.setScale(sc, sc);
                iconSp.setPosition(startX, pos.y + (sz.y - iconSize) / 2.0f);
                window.draw(iconSp);
            }

            // Text Shadow
            catTxt.setFillColor(sf::Color(45, 12, 8, 230));
            catTxt.setOutlineThickness(1.2f);
            catTxt.setOutlineColor(sf::Color(45, 12, 8, 230));
            catTxt.setPosition(startX + iconSize + iconGap + 1.0f, pos.y + sz.y / 2.0f + 1.0f);
            window.draw(catTxt);

            // Text Foreground
            if (isHovered) {
                catTxt.setFillColor(sf::Color(255, 255, 225));
                catTxt.setOutlineThickness(1.5f);
                catTxt.setOutlineColor(sf::Color(100, 25, 15, 255));
            } else {
                catTxt.setFillColor(sf::Color(250, 230, 165));
                catTxt.setOutlineThickness(1.2f);
                catTxt.setOutlineColor(sf::Color(75, 18, 12, 240));
            }
            catTxt.setPosition(startX + iconSize + iconGap, pos.y + sz.y / 2.0f);
            window.draw(catTxt);

            if (isHovered) {
                ve4GocLuaChon(window, pos, sz, 1.5f, 1.2f);
            }
        } else {
            // Slid Left Drawer Column: Vertical master navigation plaques
            sf::RectangleShape tabPlaque(sz);
            tabPlaque.setPosition(pos);
            if (isActive) {
                tabPlaque.setFillColor(sf::Color(85, 24, 20, 245)); // Vermilion lacquer
                tabPlaque.setOutlineThickness(2.0f);
                tabPlaque.setOutlineColor(sf::Color(255, 215, 95)); // Gold border
            } else if (isHovered) {
                tabPlaque.setFillColor(sf::Color(48, 32, 22, 250)); // Amber wood
                tabPlaque.setOutlineThickness(1.5f);
                tabPlaque.setOutlineColor(sf::Color(225, 180, 80));
            } else {
                tabPlaque.setFillColor(sf::Color(28, 20, 15, 235)); // Deep ebony
                tabPlaque.setOutlineThickness(1.2f);
                tabPlaque.setOutlineColor(sf::Color(150, 115, 60, 160));
            }
            window.draw(tabPlaque);

            if (isActive || isHovered) {
                ve4GocLuaChon(window, pos, sz, 1.2f, 1.2f);
            }

            // Draw Pixel Art Icon on Left
            float iconSize = 24.0f;
            if (iconTex.getSize().x > 0) {
                sf::Sprite iconSp(iconTex);
                float sc = iconSize / static_cast<float>(iconTex.getSize().x);
                iconSp.setScale(sc, sc);
                iconSp.setPosition(pos.x + 12.0f, pos.y + (sz.y - iconSize) / 2.0f);
                window.draw(iconSp);
            }

            // Tab text
            sf::Text tabTxt;
            tabTxt.setFont(fTitle);
            tabTxt.setString(toUtf8(catName));
            tabTxt.setCharacterSize(15);
            tabTxt.setStyle(sf::Text::Bold);
            sf::FloatRect ttb = tabTxt.getLocalBounds();
            tabTxt.setOrigin(0.0f, std::round(ttb.top + ttb.height / 2.0f));
            tabTxt.setPosition(pos.x + 44.0f, pos.y + sz.y / 2.0f);
            tabTxt.setFillColor(isActive ? sf::Color(255, 248, 195) : (isHovered ? sf::Color(255, 235, 165) : sf::Color(230, 210, 165)));
            window.draw(tabTxt);

            if (isActive) {
                // Golden Right Arrow Triangle (Native SFML shape, no unicode glyph)
                sf::ConvexShape arrow(3);
                arrow.setPoint(0, sf::Vector2f(pos.x + sz.x - 22.0f, pos.y + sz.y / 2.0f - 6.0f));
                arrow.setPoint(1, sf::Vector2f(pos.x + sz.x - 14.0f, pos.y + sz.y / 2.0f));
                arrow.setPoint(2, sf::Vector2f(pos.x + sz.x - 22.0f, pos.y + sz.y / 2.0f + 6.0f));
                arrow.setFillColor(sf::Color(255, 215, 95));
                window.draw(arrow);
            }
        }
    }

    // Close button (when centered popup)
    if (t < 0.3f && settingsCloseBtn) {
        settingsCloseBtn->draw(window);
        sf::Text closeHint;
        closeHint.setFont(f);
        closeHint.setString(toUtf8(tiengViet ? "[ESC / Đóng] Quay lại  •  [Nhấp chuột] Chọn mục" : "[ESC / Close] Back  •  [Click] Select tab"));
        closeHint.setCharacterSize(12);
        closeHint.setFillColor(sf::Color(200, 175, 125, 200));
        sf::FloatRect chb = closeHint.getLocalBounds();
        closeHint.setOrigin(std::round(chb.left + chb.width / 2.0f), std::round(chb.top + chb.height / 2.0f));
        closeHint.setPosition(panelX + panelW / 2.0f, panelY + panelH - 12.0f);
        window.draw(closeHint);
    }

    // Collapse button (when slid to left)
    if (t > 0.7f && settingsCollapseBtn) {
        settingsCollapseBtn->draw(window);
    }

    // 3. Right Panel (Phần Tùy Chỉnh)
    if (t > 0.05f) {
        float rightPanelX = lerpVal(1220.0f, 380.0f, t);
        float rightPanelY = 85.0f;
        float rightW = 710.0f;
        float rightH = 635.0f;
        float offX = rightPanelX - 380.0f;

        FantasyUI::get().drawPanel(window, sf::FloatRect(rightPanelX, rightPanelY, rightW, rightH), OrientalPanelStyle::EBONY_WOOD);

        sf::RectangleShape rightInner(sf::Vector2f(rightW - 16.0f, rightH - 16.0f));
        rightInner.setPosition(rightPanelX + 8.0f, rightPanelY + 8.0f);
        rightInner.setFillColor(sf::Color(20, 15, 11, 235));
        rightInner.setOutlineThickness(1.2f);
        rightInner.setOutlineColor(sf::Color(160, 120, 55, 170));
        window.draw(rightInner);
        ve4GocLuaChon(window, sf::Vector2f(rightPanelX + 8.0f, rightPanelY + 8.0f), sf::Vector2f(rightW - 16.0f, rightH - 16.0f), 1.2f, 1.2f);

        // Header Plaque across top of Right Panel (Clean, no glyph boxes)
        sf::RectangleShape headPlaque(sf::Vector2f(rightW - 40.0f, 40.0f));
        headPlaque.setPosition(rightPanelX + 20.0f, rightPanelY + 16.0f);
        headPlaque.setFillColor(sf::Color(65, 18, 14, 225));
        headPlaque.setOutlineThickness(1.2f);
        headPlaque.setOutlineColor(sf::Color(195, 145, 65, 200));
        window.draw(headPlaque);
        ve4GocLuaChon(window, sf::Vector2f(rightPanelX + 20.0f, rightPanelY + 16.0f), sf::Vector2f(rightW - 40.0f, 40.0f), 1.0f, 1.0f);

        std::string headText;
        if (activeSettingsTab == SettingsTab::SOUND) headText = tiengViet ? "HỆ THỐNG ÂM THANH" : "AUDIO SYSTEM SETTINGS";
        else if (activeSettingsTab == SettingsTab::GAMEPLAY) headText = tiengViet ? "LỐI CHƠI & HIỂN THỊ" : "GAMEPLAY & DISPLAY SETTINGS";
        else if (activeSettingsTab == SettingsTab::LANGUAGE) headText = tiengViet ? "TÙY CHỌN NGÔN NGỮ" : "LANGUAGE SELECTION";
        else if (activeSettingsTab == SettingsTab::CONTROLS) headText = tiengViet ? "THIẾT LẬP ĐIỀU KHIỂN" : "KEYBOARD & CONTROLS";

        sf::Text ht;
        ht.setFont(fTitle);
        ht.setString(toUtf8(headText));
        ht.setCharacterSize(18);
        ht.setStyle(sf::Text::Bold);
        ht.setFillColor(sf::Color(255, 230, 140));
        sf::FloatRect htb = ht.getLocalBounds();
        ht.setOrigin(std::round(htb.left + htb.width / 2.0f), std::round(htb.top + htb.height / 2.0f));
        ht.setPosition(rightPanelX + rightW / 2.0f, rightPanelY + 36.0f);
        window.draw(ht);

        // Draw Content based on activeSettingsTab
        if (activeSettingsTab == SettingsTab::SOUND) {
            // Symmetrical full-width audio sliders & buttons (Task 3: Sound monitor removed)
            if (sliderMaster) sliderMaster->draw(window);
            if (settingsAudioButtons.size() >= 1) settingsAudioButtons[0]->draw(window);
            if (sliderSfx) sliderSfx->draw(window);
            if (settingsAudioButtons.size() >= 2) settingsAudioButtons[1]->draw(window);
            if (sliderBgm) sliderBgm->draw(window);

            float sW = 540.0f;
            float sX = 380.0f + (710.0f - sW) / 2.0f + offX;
            sf::Text sbTitle;
            sbTitle.setFont(fBold);
            sbTitle.setCharacterSize(14);
            sbTitle.setStyle(sf::Text::Bold);
            sbTitle.setString(toUtf8(tiengViet ? "THỬ NGHIỆM ÂM THANH" : "SOUNDBOARD AUDIO TEST"));
            sbTitle.setFillColor(sf::Color(255, 220, 130));
            sbTitle.setPosition(sX, 504.0f);
            window.draw(sbTitle);

            for (auto& btn : soundboardButtons) btn->draw(window);

        } else if (activeSettingsTab == SettingsTab::GAMEPLAY) {
            for (auto& btn : settingsGameplayButtons) btn->draw(window);

            float gpCol1X = 415.0f + offX;
            float gpCol2X = 735.0f + offX;

            sf::Text col1Head;
            col1Head.setFont(fBold);
            col1Head.setString(toUtf8(tiengViet ? "QUY TẮC & ĐỐI THỦ AI" : "RULES & AI BEHAVIOR"));
            col1Head.setCharacterSize(13);
            col1Head.setFillColor(sf::Color(255, 220, 120));
            col1Head.setPosition(gpCol1X, 166.0f);
            window.draw(col1Head);

            sf::Text col2Head;
            col2Head.setFont(fBold);
            col2Head.setString(toUtf8(tiengViet ? "HIỂN THỊ & GIAO DIỆN" : "DISPLAY & INTERFACE"));
            col2Head.setCharacterSize(13);
            col2Head.setFillColor(sf::Color(255, 220, 120));
            col2Head.setPosition(gpCol2X, 166.0f);
            window.draw(col2Head);

            // Balanced Gameplay Guide Box (Fills lower panel void elegantly)
            float gpGuideX = 415.0f + offX;
            float gpGuideY = 460.0f;
            float gpGuideW = 630.0f;
            float gpGuideH = 116.0f;

            sf::RectangleShape gpGuideBg(sf::Vector2f(gpGuideW, gpGuideH));
            gpGuideBg.setPosition(gpGuideX, gpGuideY);
            gpGuideBg.setFillColor(sf::Color(24, 18, 13, 235));
            gpGuideBg.setOutlineThickness(1.2f);
            gpGuideBg.setOutlineColor(sf::Color(150, 115, 60, 160));
            window.draw(gpGuideBg);
            ve4GocLuaChon(window, sf::Vector2f(gpGuideX, gpGuideY), sf::Vector2f(gpGuideW, gpGuideH), 1.0f, 1.0f);

            sf::Text gpGuideTitle;
            gpGuideTitle.setFont(fBold);
            gpGuideTitle.setString(toUtf8(tiengViet ? "HƯỚNG DẪN TÙY CHỌN TRẬN ĐẤU" : "GAMEPLAY & DISPLAY GUIDE"));
            gpGuideTitle.setCharacterSize(13);
            gpGuideTitle.setStyle(sf::Text::Bold);
            gpGuideTitle.setFillColor(sf::Color(255, 220, 120));
            gpGuideTitle.setPosition(gpGuideX + 16.0f, gpGuideY + 12.0f);
            window.draw(gpGuideTitle);

            sf::RectangleShape gpGuideDiv(sf::Vector2f(gpGuideW - 32.0f, 1.0f));
            gpGuideDiv.setPosition(gpGuideX + 16.0f, gpGuideY + 32.0f);
            gpGuideDiv.setFillColor(sf::Color(150, 115, 60, 140));
            window.draw(gpGuideDiv);

            sf::Text gpGuideContent;
            gpGuideContent.setFont(fBold);
            gpGuideContent.setString(toUtf8(tiengViet
                ? "• Hiển thị: Bật/tắt chế độ toàn màn hình hoặc cửa sổ, hiển thị hệ tọa độ 1-9 & A-J.\n"
                  "• Trợ giúp nước đi: Bật/tắt gợi ý nước đi hợp lệ, vòng sáng nước đi cuối và cảnh báo Chiếu.\n"
                  "• Nhịp điệu AI: Tùy biến tốc độ suy nghĩ của máy (Tự nhiên hoặc Phản hồi tức thì)."
                : "• Display: Switch between Fullscreen and Windowed mode, toggle coordinates.\n"
                  "• Assists: Toggle valid move dots, last move highlight, and king check alarms.\n"
                  "• AI Pacing: Switch between natural deliberate timing and instant responses."));
            gpGuideContent.setCharacterSize(13);
            gpGuideContent.setStyle(sf::Text::Bold);
            gpGuideContent.setLineSpacing(1.28f);
            gpGuideContent.setFillColor(sf::Color(235, 220, 190));
            gpGuideContent.setPosition(gpGuideX + 16.0f, gpGuideY + 40.0f);
            window.draw(gpGuideContent);

        } else if (activeSettingsTab == SettingsTab::LANGUAGE) {
            float card1X = 397.0f + offX;
            float card2X = 743.0f + offX;
            float cardY = 175.0f;
            float cardW = 330.0f;
            float cardH = 345.0f;

            auto drawLangCardDecor = [&](float cx, bool isCurrent, const std::string& title, const std::string& sub,
                                        const std::vector<std::string>& descBullets,
                                        const std::string& status, sf::Color headerTint) {
                sf::RectangleShape cardBg(sf::Vector2f(cardW, cardH));
                cardBg.setPosition(cx, cardY);
                cardBg.setFillColor(isCurrent ? sf::Color(32, 22, 16, 245) : sf::Color(18, 14, 10, 230));
                cardBg.setOutlineThickness(isCurrent ? 2.0f : 1.2f);
                cardBg.setOutlineColor(isCurrent ? sf::Color(255, 215, 95) : sf::Color(150, 115, 60, 160));
                window.draw(cardBg);

                ve4GocLuaChon(window, sf::Vector2f(cx, cardY), sf::Vector2f(cardW, cardH), 1.2f, 1.2f);

                // Header Band
                sf::RectangleShape hBand(sf::Vector2f(cardW - 16.0f, 36.0f));
                hBand.setPosition(cx + 8.0f, cardY + 8.0f);
                hBand.setFillColor(headerTint);
                hBand.setOutlineThickness(1.0f);
                hBand.setOutlineColor(sf::Color(190, 140, 60, 180));
                window.draw(hBand);

                sf::Text t;
                t.setFont(fTitle);
                t.setString(toUtf8(title));
                t.setCharacterSize(16);
                t.setStyle(sf::Text::Bold);
                t.setFillColor(isCurrent ? sf::Color(255, 245, 205) : sf::Color(240, 225, 185));
                sf::FloatRect tb = t.getLocalBounds();
                t.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));
                t.setPosition(cx + cardW / 2.0f, cardY + 26.0f);
                window.draw(t);

                sf::Text st;
                st.setFont(fBold);
                st.setString(toUtf8(sub));
                st.setCharacterSize(13);
                st.setStyle(sf::Text::Bold);
                st.setFillColor(sf::Color(235, 195, 115));
                sf::FloatRect stb = st.getLocalBounds();
                st.setOrigin(std::round(stb.left + stb.width / 2.0f), std::round(stb.top + stb.height / 2.0f));
                st.setPosition(cx + cardW / 2.0f, cardY + 58.0f);
                window.draw(st);

                sf::RectangleShape div(sf::Vector2f(cardW - 40.0f, 1.0f));
                div.setPosition(cx + 20.0f, cardY + 76.0f);
                div.setFillColor(sf::Color(165, 125, 60, 160));
                window.draw(div);

                // Word-wrapped Bullet Points (Strict boundary guard: never overflows card borders)
                const float textPadX = 18.0f;
                const float maxTextW = cardW - textPadX * 2.0f;
                const unsigned int descCharSize = 13;

                std::string fullDesc;
                for (size_t i = 0; i < descBullets.size(); ++i) {
                    std::string item = descBullets[i];
                    std::istringstream iss(item);
                    std::string word;
                    std::string curLine;
                    std::string wrappedBullet;
                    while (iss >> word) {
                        std::string test = curLine.empty() ? word : (curLine + " " + word);
                        sf::Text probe;
                        probe.setFont(fBold);
                        probe.setCharacterSize(descCharSize);
                        probe.setStyle(sf::Text::Bold);
                        probe.setString(toUtf8(test));
                        if (probe.getLocalBounds().width > maxTextW && !curLine.empty()) {
                            if (!wrappedBullet.empty()) wrappedBullet += "\n";
                            wrappedBullet += curLine;
                            curLine = "  " + word;
                        } else {
                            curLine = test;
                        }
                    }
                    if (!curLine.empty()) {
                        if (!wrappedBullet.empty()) wrappedBullet += "\n";
                        wrappedBullet += curLine;
                    }

                    if (!fullDesc.empty()) fullDesc += "\n";
                    fullDesc += wrappedBullet;
                }

                sf::Text d;
                d.setFont(fBold);
                d.setString(toUtf8(fullDesc));
                d.setCharacterSize(descCharSize);
                d.setStyle(sf::Text::Bold);
                d.setLineSpacing(1.32f);
                d.setFillColor(sf::Color(245, 235, 205));
                d.setPosition(cx + textPadX, cardY + 90.0f);
                window.draw(d);

                // Imperial Seal Status Badge
                sf::RectangleShape badge(sf::Vector2f(cardW - 40.0f, 38.0f));
                badge.setPosition(cx + 20.0f, cardY + cardH - 54.0f);
                badge.setFillColor(isCurrent ? sf::Color(85, 24, 20, 245) : sf::Color(45, 32, 22, 220));
                badge.setOutlineThickness(1.2f);
                badge.setOutlineColor(isCurrent ? sf::Color(255, 215, 95) : sf::Color(160, 120, 60));
                window.draw(badge);

                sf::Text statText;
                statText.setFont(fBold);
                statText.setString(toUtf8(status));
                statText.setCharacterSize(14);
                statText.setStyle(sf::Text::Bold);
                statText.setFillColor(isCurrent ? sf::Color(255, 245, 200) : sf::Color(225, 205, 165));
                sf::FloatRect bgb = statText.getLocalBounds();
                statText.setOrigin(std::round(bgb.left + bgb.width / 2.0f), std::round(bgb.top + bgb.height / 2.0f));
                statText.setPosition(cx + cardW / 2.0f, cardY + cardH - 35.0f);
                window.draw(statText);
            };

            std::vector<std::string> viBullets = {
                "• Giao diện chữ tiếng Việt chuẩn xác.",
                "• Quân: Tướng, Sĩ, Tượng, Xe, Pháo, Mã, Tốt.",
                "• Phông chữ cổ phong thuần Việt hiển thị rõ nét."
            };
            drawLangCardDecor(card1X, tiengViet,
                "TIẾNG VIỆT", "Cờ Tướng Thuần Việt",
                viBullets,
                tiengViet ? "ĐANG SỬ DỤNG" : "NHẤN ĐỂ CHỌN",
                sf::Color(75, 22, 18, 230));

            std::vector<std::string> enBullets = {
                "• Standard international English interface.",
                "• Pieces: General, Advisor, Elephant, Rook, etc.",
                "• Crisp pixel & vector typography."
            };
            drawLangCardDecor(card2X, !tiengViet,
                "ENGLISH", "International Xiangqi",
                enBullets,
                !tiengViet ? "CURRENTLY ACTIVE" : "CLICK TO SELECT",
                sf::Color(22, 42, 65, 230));

            sf::Text langHint;
            langHint.setFont(fBold);
            langHint.setString(toUtf8(tiengViet
                ? "Nhấp vào thẻ ngôn ngữ tương ứng để thay đổi giao diện tức thì."
                : "Click either card to switch the interface language immediately."));
            langHint.setCharacterSize(14);
            langHint.setStyle(sf::Text::Bold);
            langHint.setFillColor(sf::Color(255, 230, 160));
            sf::FloatRect lhb = langHint.getLocalBounds();
            langHint.setOrigin(std::round(lhb.left + lhb.width / 2.0f), std::round(lhb.top + lhb.height / 2.0f));
            langHint.setPosition(rightPanelX + rightW / 2.0f, cardY + cardH + 28.0f);
            window.draw(langHint);

        } else if (activeSettingsTab == SettingsTab::CONTROLS) {
            float kCol1X = 415.0f + offX;
            float kCol2X = 735.0f + offX;

            sf::Text preTitle;
            preTitle.setFont(fBold);
            preTitle.setString(toUtf8(tiengViet ? "BỘ PHÍM SẴN CÓ" : "QUICK PRESETS"));
            preTitle.setCharacterSize(13);
            preTitle.setFillColor(sf::Color(255, 220, 120));
            preTitle.setPosition(kCol1X, 158.0f);
            window.draw(preTitle);

            sf::Text actTitle;
            actTitle.setFont(fBold);
            actTitle.setString(toUtf8(tiengViet ? "TÙY BIẾN PHÍM BẤM (Nhấp vào để đổi phím)" : "CUSTOM KEY BINDINGS (Click to rebind)"));
            actTitle.setCharacterSize(13);
            actTitle.setFillColor(sf::Color(255, 220, 120));
            actTitle.setPosition(kCol1X, 244.0f);
            window.draw(actTitle);

            for (auto& btn : settingsControlButtons) btn->draw(window);

            if (rebindingAction != KeyAction::COUNT) {
                sf::RectangleShape listenBox(sf::Vector2f(rightW - 60.0f, 56.0f));
                listenBox.setPosition(rightPanelX + 30.0f, 492.0f);
                listenBox.setFillColor(sf::Color(165, 25, 20, 240));
                listenBox.setOutlineThickness(1.8f);
                listenBox.setOutlineColor(sf::Color(255, 215, 95));
                window.draw(listenBox);
                ve4GocLuaChon(window, sf::Vector2f(rightPanelX + 30.0f, 492.0f), sf::Vector2f(rightW - 60.0f, 56.0f), 1.2f, 1.2f);

                sf::Text listenT;
                listenT.setFont(fBold);
                listenT.setString(toUtf8(tiengViet ? ">>> ĐANG CHỜ NHẤN PHÍM MỚI (Nhấn ESC để hủy) <<<" : ">>> LISTENING FOR NEW KEY (Press ESC to cancel) <<<"));
                listenT.setCharacterSize(14);
                listenT.setStyle(sf::Text::Bold);
                listenT.setFillColor(sf::Color(255, 245, 190));
                sf::FloatRect ltb = listenT.getLocalBounds();
                listenT.setOrigin(std::round(ltb.left + ltb.width / 2.0f), std::round(ltb.top + ltb.height / 2.0f));
                listenT.setPosition(rightPanelX + rightW / 2.0f, 520.0f);
                window.draw(listenT);
            } else {
                // Balanced Controls & Mouse Guide Box
                float ctrlGuideX = 415.0f + offX;
                float ctrlGuideY = 490.0f;
                float ctrlGuideW = 630.0f;
                float ctrlGuideH = 88.0f;

                sf::RectangleShape ctrlGuideBg(sf::Vector2f(ctrlGuideW, ctrlGuideH));
                ctrlGuideBg.setPosition(ctrlGuideX, ctrlGuideY);
                ctrlGuideBg.setFillColor(sf::Color(24, 18, 13, 235));
                ctrlGuideBg.setOutlineThickness(1.2f);
                ctrlGuideBg.setOutlineColor(sf::Color(150, 115, 60, 160));
                window.draw(ctrlGuideBg);
                ve4GocLuaChon(window, sf::Vector2f(ctrlGuideX, ctrlGuideY), sf::Vector2f(ctrlGuideW, ctrlGuideH), 1.0f, 1.0f);

                sf::Text ctrlGuideTitle;
                ctrlGuideTitle.setFont(fBold);
                ctrlGuideTitle.setString(toUtf8(tiengViet ? "HƯỚNG DẪN THAO TÁC & PHÍM TẮT" : "MOUSE CONTROLS & QUICK SHORTCUTS"));
                ctrlGuideTitle.setCharacterSize(13);
                ctrlGuideTitle.setStyle(sf::Text::Bold);
                ctrlGuideTitle.setFillColor(sf::Color(255, 220, 120));
                ctrlGuideTitle.setPosition(ctrlGuideX + 16.0f, ctrlGuideY + 12.0f);
                window.draw(ctrlGuideTitle);

                sf::RectangleShape ctrlGuideDiv(sf::Vector2f(ctrlGuideW - 32.0f, 1.0f));
                ctrlGuideDiv.setPosition(ctrlGuideX + 16.0f, ctrlGuideY + 30.0f);
                ctrlGuideDiv.setFillColor(sf::Color(150, 115, 60, 140));
                window.draw(ctrlGuideDiv);

                sf::Text ctrlGuideContent;
                ctrlGuideContent.setFont(fBold);
                ctrlGuideContent.setString(toUtf8(tiengViet
                    ? "• Chuột: Nhấp chuột trái để chọn quân cờ và điểm đến. Nhấp chuột phải để hủy chọn.\n"
                      "• Phím tắt nhanh: U (Đi lại / Undo)  •  ESC (Hủy chọn / Bảng tạm dừng)  •  Q (Thoát nhanh)."
                    : "• Mouse: Left click to select piece and target square. Right click to deselect.\n"
                      "• Quick Hotkeys: U (Undo last move)  •  ESC (Deselect / Pause menu)  •  Q (Quick Quit)."));
                ctrlGuideContent.setCharacterSize(13);
                ctrlGuideContent.setStyle(sf::Text::Bold);
                ctrlGuideContent.setLineSpacing(1.28f);
                ctrlGuideContent.setFillColor(sf::Color(235, 220, 190));
                ctrlGuideContent.setPosition(ctrlGuideX + 16.0f, ctrlGuideY + 38.0f);
                window.draw(ctrlGuideContent);
            }
        }

        // Draw Bottom Action Buttons inside Right Panel
        for (auto& btn : settingsBottomButtons) {
            btn->draw(window);
        }
    }
}

void GameManager::veKeybindingMenu() {
    activeSettingsTab = SettingsTab::CONTROLS;
    settingsDetailOpen = true;
    settingsSlideProgress = 1.0f;
    capNhatViTriNutCaiDat();
    veSettingsMenu();
}

void GameManager::veIntroMenu() {
    bool tiengViet = (Loc::getLanguage() == Language::TIENG_VIET);

    // 1. Full Dim Backdrop (Unified with Title & Setup)
    FantasyUI::get().drawFullDimOverlay(window, sf::Color(10, 8, 12, 175));

    // Top Royal Scroll Banner Title
    auto& am = AssetManager::get();
    const sf::Texture& bannerTex = am.texture(TextureId::TITLE_SCROLL_BANNER);
    if (bannerTex.getSize().x > 0) {
        sf::Sprite bannerSprite(bannerTex);
        float bw = static_cast<float>(bannerTex.getSize().x);
        float bh = static_cast<float>(bannerTex.getSize().y);
        bannerSprite.setOrigin(bw / 2.0f, bh / 2.0f);
        float bScale = 0.86f;
        bannerSprite.setScale(bScale, bScale);
        bannerSprite.setPosition(600.0f, 62.0f);
        window.draw(bannerSprite);

        sf::Text titleText;
        titleText.setFont(tiengViet ? fontVnBold : fontEnBold);
        titleText.setString(toUtf8(tiengViet ? "Thông Tin & Hướng Dẫn" : "Game Info & Rules"));
        titleText.setCharacterSize(tiengViet ? 25 : 23);
        titleText.setStyle(sf::Text::Bold);
        sf::FloatRect tb = titleText.getLocalBounds();
        titleText.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));

        // Lacquer drop shadow
        titleText.setFillColor(sf::Color(65, 16, 10, 200));
        titleText.setOutlineThickness(1.8f);
        titleText.setOutlineColor(sf::Color(65, 16, 10, 200));
        titleText.setPosition(601.0f, 63.0f);
        window.draw(titleText);

        // Crimson with antique gold outline
        titleText.setFillColor(sf::Color(156, 24, 18));
        titleText.setOutlineThickness(2.2f);
        titleText.setOutlineColor(sf::Color(245, 215, 120));
        titleText.setPosition(600.0f, 62.0f);
        window.draw(titleText);
    }

    // 2. Outer Oriental Ebony Wood Frame (1000x576 at X=100, Y=112)
    FantasyUI::get().drawPanel(window, sf::FloatRect(100.0f, 112.0f, 1000.0f, 576.0f), OrientalPanelStyle::EBONY_WOOD);

    const sf::Color inkDark(35, 18, 10);

    const float colW = 472.0f;
    const float colH = 478.0f;
    const float col1X = 118.0f;
    const float col2X = 610.0f;
    const float colY = 132.0f;
    const float padX = 18.0f;
    const float contentW = colW - padX * 2.0f; // 436.0f
    const float col1TextX = col1X + padX;      // 136.0f
    const float col2TextX = col2X + padX;      // 628.0f
    const unsigned int contentCharSize = tiengViet ? 13 : 12;

    auto drawXuanColumnHeader = [&](float centerX, float topY, const std::string& headerText, float divStartX, float divWidth) {
        sf::Text sh;
        sh.setFont(fontBold);
        sh.setString(toUtf8(headerText));
        sh.setCharacterSize(17);
        sh.setStyle(sf::Text::Bold);
        sh.setFillColor(sf::Color(0, 0, 0, 100));
        sf::FloatRect b = sh.getLocalBounds();
        sh.setOrigin(std::round(b.left + b.width / 2.0f), std::round(b.top + b.height / 2.0f));
        sh.setPosition(centerX + 1.0f, topY + 1.0f);
        window.draw(sh);

        sf::Text tx = sh;
        tx.setFillColor(sf::Color(175, 25, 20));
        tx.setPosition(centerX, topY);
        window.draw(tx);

        sf::RectangleShape div(sf::Vector2f(divWidth, 2.0f));
        div.setPosition(divStartX, topY + 18.0f);
        div.setFillColor(sf::Color(135, 92, 42));
        window.draw(div);
    };

    // 3. Left Symmetrical Column: OOP Project & Design Patterns
    FantasyUI::get().drawPanel(window, sf::FloatRect(col1X, colY, colW, colH), OrientalPanelStyle::PARCHMENT_SCROLL);
    drawXuanColumnHeader(col1X + colW / 2.0f, 154.0f, tiengViet ? "ĐỒ ÁN OOP & THIẾT KẾ" : "OOP PROJECT & DESIGN", col1TextX, contentW);

    std::string oopText = tiengViet
        ? "• TRƯỜNG ĐH KHOA HỌC TỰ NHIÊN (HCMUS)\n"
          "  Đồ án Lập trình Hướng đối tượng - C++17 / SFML.\n\n"
          "• ĐA HÌNH & TRỪU TƯỢNG (Polymorphism):\n"
          "  Lớp thuần ảo QuanCo định nghĩa giao diện chung:\n"
          "  kiemTraNuocDi(), layGiaTri(), layDiemViTri().\n\n"
          "• KẾ THỪA (Inheritance) & FACTORY:\n"
          "  7 quân cờ: Xe, Voi, Phao, Ma, Tuong, Si, Tot.\n"
          "  QuanCoFactory chuẩn hóa khởi tạo thực thể.\n\n"
          "• MẪU PROTOTYPE (Deep Copy):\n"
          "  Hiện thực clone() sao chép sâu trạng thái bàn cờ,\n"
          "  hỗ trợ tìm kiếm mô phỏng nước đi hiệu năng cao.\n\n"
          "• TRÍ TUỆ NHÂN TẠO (AI Engine):\n"
          "  Thuật toán Minimax + cắt tỉa Alpha-Beta,\n"
          "  kết hợp bảng điểm vị trí và Move Ordering."
        : "• VNU-HCM UNIVERSITY OF SCIENCE (HCMUS)\n"
          "  Object-Oriented Programming - C++17 / SFML.\n\n"
          "• POLYMORPHISM & ABSTRACTION:\n"
          "  Abstract QuanCo defines core polymorphic APIs:\n"
          "  kiemTraNuocDi(), layGiaTri(), layDiemViTri().\n\n"
          "• INHERITANCE & FACTORY PATTERN:\n"
          "  7 pieces: Chariot, Elephant, Cannon, Horse,\n"
          "  General, Advisor, Soldier via QuanCoFactory.\n\n"
          "• PROTOTYPE PATTERN (Deep Copy):\n"
          "  Robust clone() for zero-leak board copying,\n"
          "  supporting search and move simulations.\n\n"
          "• ARTIFICIAL INTELLIGENCE (AI Engine):\n"
          "  Minimax search with Alpha-Beta pruning,\n"
          "  Piece-Square Tables and move ordering.";

    sf::Text oopContent;
    oopContent.setFont(fontBold);
    oopContent.setString(toUtf8(oopText));
    oopContent.setCharacterSize(contentCharSize);
    oopContent.setStyle(sf::Text::Bold);
    oopContent.setLineSpacing(1.22f);
    oopContent.setFillColor(inkDark);
    oopContent.setPosition(col1TextX, 186.0f);
    window.draw(oopContent);

    // 4. Right Symmetrical Column: 7 Piece Rules & Controls
    FantasyUI::get().drawPanel(window, sf::FloatRect(col2X, colY, colW, colH), OrientalPanelStyle::PARCHMENT_SCROLL);
    drawXuanColumnHeader(col2X + colW / 2.0f, 154.0f, tiengViet ? "LUẬT 7 QUÂN CỜ & ĐIỀU KHIỂN" : "7 PIECES RULES & CONTROLS", col2TextX, contentW);

    std::string rulesText = tiengViet
        ? "1. XE: Đi ngang/dọc tùy ý; bị cản bởi quân cờ.\n"
          "2. PHÁO: Đi như Xe; ăn quân phải nhảy qua 1 ngòi.\n"
          "3. MÃ: Đi chữ Nhật (2x1); cản chân tại mắt Mã.\n"
          "4. TƯỢNG: Đi chéo 2 ô; không qua sông; cản mắt.\n"
          "5. SĨ: Đi chéo 1 ô bên trong Cung Cửu Cung.\n"
          "6. TƯỚNG: Đi 1 ô ngang/dọc trong Cung;\n"
          "   Hai Tướng không đối mặt trực tiếp trên cùng cột.\n"
          "7. TỐT: Đi thẳng 1 ô; qua sông đi thêm ngang, không lùi.\n\n"
          "• THAO TÁC TRẬN ĐẤU:\n"
          "  - Chuột: Nhấp chọn quân cờ và ô đích hợp lệ.\n"
          "  - Bàn phím: W/A/S/D di chuyển, Enter để chọn/đi.\n"
          "  - Phím tắt: ESC (Hủy / Tạm dừng), U (Đi lại / Undo)."
        : "1. CHARIOT: Moves orthogonally any distance.\n"
          "2. CANNON: Moves like Chariot; jumps 1 piece to capture.\n"
          "3. HORSE: Moves in L-shape (2x1); hobbled at foot / eye.\n"
          "4. ELEPHANT: Diagonal 2 squares; own side; eye-blocked.\n"
          "5. ADVISOR: Diagonal 1 square within Palace.\n"
          "6. GENERAL: 1 step in Palace (horizontal / vertical);\n"
          "   Cannot face enemy General directly on open file.\n"
          "7. SOLDIER: 1 forward; moves sideways across river.\n\n"
          "• MATCH CONTROLS:\n"
          "  - Mouse: Left click to select piece & destination.\n"
          "  - Keyboard: W/A/S/D move cursor, Enter to select.\n"
          "  - Hotkeys: ESC (Deselect / Pause), U (Undo move).";

    sf::Text rulesContent;
    rulesContent.setFont(fontBold);
    rulesContent.setString(toUtf8(rulesText));
    rulesContent.setCharacterSize(contentCharSize);
    rulesContent.setStyle(sf::Text::Bold);
    rulesContent.setLineSpacing(1.22f);
    rulesContent.setFillColor(inkDark);
    rulesContent.setPosition(col2TextX, 186.0f);
    window.draw(rulesContent);

    // 5. Centered Bottom Action Button (X=490, Y=624, W=220, H=44)
    if (auto b = menuIntro.layButton(0)) {
        b->setPosition(490.0f, 624.0f);
        b->setSize(220.0f, 44.0f);
        b->setColors(sf::Color(255, 230, 140), sf::Color(255, 248, 175), sf::Color(255, 250, 190));
    }
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
        return vi ? "Tướng đang bị chiếu! Cần giải chiếu." 
                  : "King is under check! Protect the King.";
    }
    return vi ? "Làm lộ mặt Tướng hoặc khiến Tướng bị chiếu!" 
              : "Exposes King to check or flying generals!";
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
        float scale = (radius * 1.95f) / static_cast<float>(pieceTex.getSize().y);
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
    float alphaRatio = (thoiGianNuocDiSai < 0.35f) ? (thoiGianNuocDiSai / 0.35f) : 1.0f;
    if (alphaRatio < 0.0f) alphaRatio = 0.0f;
    sf::Uint8 alpha = static_cast<sf::Uint8>(255.0f * alphaRatio);
    if (alpha == 0) return;

    bool vi = (Loc::getLanguage() == Language::TIENG_VIET);
    sf::Font& fTitle = getMenuFont(vi, true);
    sf::Font& fBody = getMenuFont(vi, true);

    const float dw = 520.0f;
    const float dh = 86.0f;
    const float dx = (1200.0f - dw) / 2.0f;
    const float dy = 160.0f;

    // Elegant compact floating oriental panel with pulsing crimson-gold border (no full screen dimming)
    float pulse = 0.5f + 0.5f * std::sin(bannerPulseClock.getElapsedTime().asSeconds() * 10.0f);
    sf::Color borderColor(255, 60, 50, static_cast<sf::Uint8>(alpha * (0.8f + 0.2f * pulse)));
    FantasyUI::get().drawPanel(window, sf::FloatRect(dx, dy, dw, dh), FantasyPanelStyle::ORNATE_WOOD, borderColor);

    // Ribbon Title
    std::string titleStr = vi ? "[ ! ] NƯỚC ĐI KHÔNG HỢP LỆ [ ! ]" : "[ ! ] ILLEGAL MOVE [ ! ]";
    FantasyUI::get().drawRibbonTitle(window, dx + dw / 2.0f, dy + 14.0f, 320.0f, 32.0f,
                                     titleStr, fTitle, 14, sf::Color(255, 230, 200, alpha), borderColor);

    // Warning message (concise, centered, never overflows)
    sf::Text tReason;
    tReason.setFont(fBody);
    tReason.setCharacterSize(14);
    tReason.setStyle(sf::Text::Bold);
    tReason.setString(toUtf8(lyDoNuocDiSai));
    tReason.setFillColor(sf::Color(255, 240, 180, alpha));
    tReason.setOutlineThickness(1.0f);
    tReason.setOutlineColor(sf::Color(20, 12, 8, alpha));
    sf::FloatRect rb = tReason.getLocalBounds();
    tReason.setOrigin(std::round(rb.left + rb.width / 2.0f), std::round(rb.top + rb.height / 2.0f));
    tReason.setPosition(dx + dw / 2.0f, dy + 58.0f);
    window.draw(tReason);
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

void GameManager::kichHoatRungManHinh(float thoiGian, float cuongDo) {
    screenShakeTimer = thoiGian;
    screenShakeIntensity = cuongDo;
}

void GameManager::khoiTaoDefeatParticles() {
    defeatParticles.clear();
    defeatParticlesActive = true;
    defeatVignetteTimer = 0.0f;

    sf::Color palette[] = {
        sf::Color(230, 60, 20),   // Vivid ember red
        sf::Color(255, 120, 30),  // Bright orange spark
        sf::Color(180, 40, 25),   // Deep crimson cinder
        sf::Color(140, 20, 20),   // Blood ruby ember
        sf::Color(70, 60, 65),    // Dark ash flake
        sf::Color(45, 40, 42),    // Charcoal flake
        sf::Color(100, 85, 80),   // Warm grey smoke flake
        sf::Color(255, 180, 50)   // Golden cinder
    };
    size_t numColors = sizeof(palette) / sizeof(palette[0]);

    for (int i = 0; i < 95; ++i) {
        DefeatParticle p;
        p.pos.x = static_cast<float>(rand() % 1200);
        p.pos.y = -20.0f - static_cast<float>(rand() % 500);
        p.vel.x = static_cast<float>((rand() % 80) - 40) * 0.8f;
        p.vel.y = 50.0f + static_cast<float>(rand() % 110);
        p.rot = static_cast<float>(rand() % 360);
        p.rotSpeed = static_cast<float>((rand() % 160) - 80) * 1.2f;
        p.size = 4.0f + static_cast<float>(rand() % 7);
        p.baseColor = palette[rand() % numColors];
        p.alpha = 140.0f + static_cast<float>(rand() % 115);
        p.flickerSpeed = 3.0f + static_cast<float>(rand() % 6);
        p.flickerPhase = static_cast<float>(rand() % 360) * 0.0174533f;
        defeatParticles.push_back(p);
    }
}

void GameManager::capNhatDefeatParticles(float dt) {
    if (!defeatParticlesActive) return;
    defeatVignetteTimer += dt;

    for (auto& p : defeatParticles) {
        p.pos += p.vel * dt;
        p.rot += p.rotSpeed * dt;
        p.flickerPhase += p.flickerSpeed * dt;
        p.vel.x += std::sin(p.pos.y * 0.025f + p.flickerPhase) * 15.0f * dt;

        if (p.pos.y > 830.0f) {
            p.pos.y = -20.0f - static_cast<float>(rand() % 80);
            p.pos.x = static_cast<float>(rand() % 1200);
            p.vel.y = 50.0f + static_cast<float>(rand() % 110);
        }
    }
}

void GameManager::veDefeatParticles() {
    if (!defeatParticlesActive) return;

    // 1. Sombre fullscreen dark crimson/charcoal vignette border
    float pulse = 0.8f + 0.2f * std::sin(defeatVignetteTimer * 2.2f);
    sf::Uint8 vigAlpha = static_cast<sf::Uint8>(std::min(255.0f, 130.0f * pulse));

    // Top border glow
    sf::Vertex topQuad[] = {
        sf::Vertex(sf::Vector2f(0.0f, 0.0f), sf::Color(80, 10, 10, vigAlpha)),
        sf::Vertex(sf::Vector2f(1200.0f, 0.0f), sf::Color(80, 10, 10, vigAlpha)),
        sf::Vertex(sf::Vector2f(1200.0f, 90.0f), sf::Color(40, 5, 5, 0)),
        sf::Vertex(sf::Vector2f(0.0f, 90.0f), sf::Color(40, 5, 5, 0))
    };
    window.draw(topQuad, 4, sf::Quads);

    // Bottom border glow
    sf::Vertex btmQuad[] = {
        sf::Vertex(sf::Vector2f(0.0f, 710.0f), sf::Color(40, 5, 5, 0)),
        sf::Vertex(sf::Vector2f(1200.0f, 710.0f), sf::Color(40, 5, 5, 0)),
        sf::Vertex(sf::Vector2f(1200.0f, 800.0f), sf::Color(80, 10, 10, vigAlpha)),
        sf::Vertex(sf::Vector2f(0.0f, 800.0f), sf::Color(80, 10, 10, vigAlpha))
    };
    window.draw(btmQuad, 4, sf::Quads);

    // Left border glow
    sf::Vertex leftQuad[] = {
        sf::Vertex(sf::Vector2f(0.0f, 0.0f), sf::Color(80, 10, 10, vigAlpha)),
        sf::Vertex(sf::Vector2f(110.0f, 0.0f), sf::Color(40, 5, 5, 0)),
        sf::Vertex(sf::Vector2f(110.0f, 800.0f), sf::Color(40, 5, 5, 0)),
        sf::Vertex(sf::Vector2f(0.0f, 800.0f), sf::Color(80, 10, 10, vigAlpha))
    };
    window.draw(leftQuad, 4, sf::Quads);

    // Right border glow
    sf::Vertex rightQuad[] = {
        sf::Vertex(sf::Vector2f(1090.0f, 0.0f), sf::Color(40, 5, 5, 0)),
        sf::Vertex(sf::Vector2f(1200.0f, 0.0f), sf::Color(80, 10, 10, vigAlpha)),
        sf::Vertex(sf::Vector2f(1200.0f, 800.0f), sf::Color(80, 10, 10, vigAlpha)),
        sf::Vertex(sf::Vector2f(1090.0f, 800.0f), sf::Color(40, 5, 5, 0))
    };
    window.draw(rightQuad, 4, sf::Quads);

    // 2. Render falling embers and ash particles
    for (const auto& p : defeatParticles) {
        float fVal = 0.65f + 0.35f * std::sin(p.flickerPhase);
        sf::Uint8 a = static_cast<sf::Uint8>(std::min(255.0f, std::max(0.0f, p.alpha * fVal)));
        sf::Color c = p.baseColor;
        c.a = a;

        sf::RectangleShape shape(sf::Vector2f(p.size, p.size * 0.7f));
        shape.setOrigin(p.size / 2.0f, (p.size * 0.7f) / 2.0f);
        shape.setPosition(p.pos);
        shape.setRotation(p.rot);
        shape.setFillColor(c);
        window.draw(shape);
    }
}

void GameManager::capNhatHoverTitleScreen(const sf::Vector2i& viTri) {
    float mx = static_cast<float>(viTri.x);
    float my = static_cast<float>(viTri.y);
    titleBtnSettingsHovered = titleSettingsRect.contains(mx, my);
    titleBtnSoundHovered    = titleSoundRect.contains(mx, my);
    titleBtnInfoHovered     = titleInfoRect.contains(mx, my);

    if (titleSubMenu == TitleSubMenu::MAIN_START) {
        titleBtnStartHovered = titleStartRect.contains(mx, my);
        if (titleBtnStartHovered) titleMenuIndex = 0;
    } else if (titleSubMenu == TitleSubMenu::PLAY_LOAD) {
        titleBtnPlayHovered = titleBtnPlayRect.contains(mx, my);
        titleBtnLoadHovered = titleBtnLoadRect.contains(mx, my);
        titleBtnBackHovered = titleBtnBackRect.contains(mx, my);
        if (titleBtnPlayHovered) titleMenuIndex = 0;
        else if (titleBtnLoadHovered) titleMenuIndex = 1;
        else if (titleBtnBackHovered) titleMenuIndex = 2;
    } else if (titleSubMenu == TitleSubMenu::MODE_SELECT) {
        titleBtnPvpHovered      = titleBtnPvpRect.contains(mx, my);
        titleBtnPvaiHovered     = titleBtnPvaiRect.contains(mx, my);
        titleBtnModeBackHovered = titleBtnModeBackRect.contains(mx, my);
        if (titleBtnPvpHovered) titleMenuIndex = 0;
        else if (titleBtnPvaiHovered) titleMenuIndex = 1;
        else if (titleBtnModeBackHovered) titleMenuIndex = 2;
    }
}

void GameManager::xuLyClickTitleScreen(const sf::Vector2i& viTri) {
    float x = static_cast<float>(viTri.x);
    float y = static_cast<float>(viTri.y);

    // 1. Bottom Corner Action Buttons
    if (titleSettingsRect.contains(x, y)) {
        soundManager.play(SoundType::CLICK);
        moMenuCaiDat(TrangThai::TITLE_SCREEN);
        return;
    }

    if (titleSoundRect.contains(x, y)) {
        soundManager.setBgmBat(!soundManager.getBgmBat());
        soundManager.play(SoundType::CLICK);
        luuCaiDat();
        bool vi = (Loc::getLanguage() == Language::TIENG_VIET);
        hienToast(soundManager.getBgmBat() ? (vi ? "Nhạc nền: BẬT" : "BGM: ON") 
                                           : (vi ? "Nhạc nền: TẮT" : "BGM: OFF"));
        return;
    }

    if (titleInfoRect.contains(x, y)) {
        soundManager.play(SoundType::CLICK);
        trangThaiTruocIntro = TrangThai::TITLE_SCREEN;
        trangThai = TrangThai::INTRODUCTION_MENU;
        return;
    }

    // 2. Central Action Buttons by Current Submenu
    if (titleSubMenu == TitleSubMenu::MAIN_START) {
        if (titleStartRect.contains(x, y)) {
            soundManager.play(SoundType::CLICK);
            titleSubMenu = TitleSubMenu::PLAY_LOAD;
            titleMenuIndex = 0;
            return;
        }
    } else if (titleSubMenu == TitleSubMenu::PLAY_LOAD) {
        if (titleBtnPlayRect.contains(x, y)) {
            soundManager.play(SoundType::CLICK);
            titleSubMenu = TitleSubMenu::MODE_SELECT;
            titleMenuIndex = 0;
            return;
        }
        if (titleBtnLoadRect.contains(x, y)) {
            soundManager.play(SoundType::CLICK);
            trangThaiTruocLoad = TrangThai::TITLE_SCREEN;
            capNhatThongTinSlots();
            trangThai = TrangThai::LOAD_MENU;
            return;
        }
        if (titleBtnBackRect.contains(x, y)) {
            soundManager.play(SoundType::CLICK);
            titleSubMenu = TitleSubMenu::MAIN_START;
            titleMenuIndex = 0;
            return;
        }
    } else if (titleSubMenu == TitleSubMenu::MODE_SELECT) {
        if (titleBtnPvpRect.contains(x, y)) {
            soundManager.play(SoundType::CLICK);
            pvpRollBanner = "";
            pvpSetupStep = 1;
            playSetupMode = 1;
            khoiTaoPlaySetupButtons();
            trangThai = TrangThai::PVP_SETUP;
            return;
        }
        if (titleBtnPvaiRect.contains(x, y)) {
            soundManager.play(SoundType::CLICK);
            aiRollBanner = "";
            playSetupMode = 0;
            khoiTaoPlaySetupButtons();
            trangThai = TrangThai::PVAI_SETUP;
            return;
        }
        if (titleBtnModeBackRect.contains(x, y)) {
            soundManager.play(SoundType::CLICK);
            titleSubMenu = TitleSubMenu::PLAY_LOAD;
            titleMenuIndex = 0;
            return;
        }
    }
}

void GameManager::veTitleScreen() {
    auto& am = AssetManager::get();
    bool isVi = (Loc::getLanguage() == Language::TIENG_VIET);

    // 1. Royal Scroll Banner (Cuộn Thư Hoàng Gia) at top center (Static, sharp, no pulse)
    const sf::Texture& bannerTex = am.texture(TextureId::TITLE_SCROLL_BANNER);
    if (bannerTex.getSize().x > 0) {
        sf::Sprite bannerSprite(bannerTex);
        float bw = static_cast<float>(bannerTex.getSize().x);
        float bh = static_cast<float>(bannerTex.getSize().y);
        bannerSprite.setOrigin(bw / 2.0f, bh / 2.0f);

        float baseScale = 1.18f;
        bannerSprite.setScale(baseScale, baseScale);

        float bannerCenterY = 125.0f;
        bannerSprite.setPosition(600.0f, bannerCenterY);
        window.draw(bannerSprite);

        // Title text inside banner rendered with sf::Text
        sf::Text titleText;
        titleText.setFont(isVi ? fontVnBold : fontEnBold);
        titleText.setString(toUtf8(isVi ? "Cờ Tướng" : "Chinese Chess"));
        titleText.setCharacterSize(isVi ? 54 : 44);
        titleText.setStyle(sf::Text::Bold);

        sf::FloatRect tb = titleText.getLocalBounds();
        titleText.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));

        // Soft lacquer drop-shadow
        titleText.setFillColor(sf::Color(65, 16, 10, 200));
        titleText.setOutlineThickness(2.2f);
        titleText.setOutlineColor(sf::Color(65, 16, 10, 200));
        titleText.setPosition(602.0f, bannerCenterY + 1.0f);
        window.draw(titleText);

        // Main text: Imperial crimson with antique gold outline
        titleText.setFillColor(sf::Color(156, 24, 18));
        titleText.setOutlineThickness(2.8f);
        titleText.setOutlineColor(sf::Color(245, 215, 120));
        titleText.setPosition(600.0f, bannerCenterY - 1.0f);
        window.draw(titleText);
    }

    // 2. Lacquer Red Button Renderer for Submenus
    const sf::Texture& btnTex = am.texture(TextureId::TITLE_BTN_FRAME);

    auto drawTitleButton = [&](sf::Vector2f center, float scale, const std::string& labelText, bool hovered, sf::FloatRect& outRect) {
        if (btnTex.getSize().x == 0) return;
        sf::Sprite btnSprite(btnTex);
        float bw = static_cast<float>(btnTex.getSize().x);
        float bh = static_cast<float>(btnTex.getSize().y);
        btnSprite.setOrigin(bw / 2.0f, bh / 2.0f);
        btnSprite.setScale(scale, scale);
        btnSprite.setPosition(center);

        float rw = bw * scale;
        float rh = bh * scale;
        outRect = sf::FloatRect(center.x - rw / 2.0f, center.y - rh / 2.0f, rw, rh);

        // Golden glow when hovered or selected
        if (hovered) {
            sf::RectangleShape glow(sf::Vector2f(rw + 12.0f, rh + 8.0f));
            glow.setOrigin((rw + 12.0f) / 2.0f, (rh + 8.0f) / 2.0f);
            glow.setPosition(center);
            glow.setFillColor(sf::Color(255, 215, 80, 50));
            glow.setOutlineThickness(2.0f);
            glow.setOutlineColor(sf::Color(255, 230, 120, 200));
            window.draw(glow);

            btnSprite.setColor(sf::Color(255, 245, 220));
        } else {
            btnSprite.setColor(sf::Color(255, 255, 255));
        }

        window.draw(btnSprite);

        // Text inside button rendered using sf::Text
        sf::Text btnText;
        btnText.setFont(isVi ? fontVnBold : fontEnBold);
        btnText.setString(toUtf8(labelText));
        int charSize = static_cast<int>(std::round(22.0f * (scale / 1.15f)));
        if (!isVi) charSize += 2;
        if (labelText.length() > 20) charSize = std::max(16, charSize - 3);
        else if (labelText.length() > 14) charSize = std::max(17, charSize - 1);
        btnText.setCharacterSize(charSize);
        btnText.setStyle(sf::Text::Bold);

        sf::FloatRect bb = btnText.getLocalBounds();
        while (bb.width > (rw - 38.0f) && charSize > 14) {
            charSize--;
            btnText.setCharacterSize(charSize);
            bb = btnText.getLocalBounds();
        }
        btnText.setOrigin(std::round(bb.left + bb.width / 2.0f), std::round(bb.top + bb.height / 2.0f));

        // Drop-shadow
        btnText.setFillColor(sf::Color(45, 12, 8, 230));
        btnText.setOutlineThickness(1.2f);
        btnText.setOutlineColor(sf::Color(45, 12, 8, 230));
        btnText.setPosition(center.x + 1.0f, center.y + 1.0f);
        window.draw(btnText);

        // Main text with hover responsive palette
        if (hovered) {
            btnText.setFillColor(sf::Color(255, 255, 225));
            btnText.setOutlineThickness(1.5f);
            btnText.setOutlineColor(sf::Color(100, 25, 15, 255));
        } else {
            btnText.setFillColor(sf::Color(250, 230, 165));
            btnText.setOutlineThickness(1.2f);
            btnText.setOutlineColor(sf::Color(75, 18, 12, 240));
        }
        btnText.setPosition(center);
        window.draw(btnText);
    };

    if (titleSubMenu == TitleSubMenu::MAIN_START) {
        drawTitleButton(sf::Vector2f(600.0f, 338.0f), 1.22f,
                        isVi ? "Bắt đầu" : "Start",
                        titleBtnStartHovered || titleMenuIndex == 0,
                        titleStartRect);
    } else if (titleSubMenu == TitleSubMenu::PLAY_LOAD) {
        drawTitleButton(sf::Vector2f(600.0f, 275.0f), 1.18f,
                        isVi ? "Chơi" : "Play",
                        titleBtnPlayHovered || titleMenuIndex == 0,
                        titleBtnPlayRect);
        drawTitleButton(sf::Vector2f(600.0f, 355.0f), 1.18f,
                        isVi ? "Tải ván đấu" : "Load Game",
                        titleBtnLoadHovered || titleMenuIndex == 1,
                        titleBtnLoadRect);
        drawTitleButton(sf::Vector2f(600.0f, 435.0f), 1.08f,
                        isVi ? "Quay lại" : "Back",
                        titleBtnBackHovered || titleMenuIndex == 2,
                        titleBtnBackRect);
    } else if (titleSubMenu == TitleSubMenu::MODE_SELECT) {
        drawTitleButton(sf::Vector2f(600.0f, 280.0f), 1.18f,
                        isVi ? "Đấu 2 Người (PvP)" : "Two Players (PvP)",
                        titleBtnPvpHovered || titleMenuIndex == 0,
                        titleBtnPvpRect);
        drawTitleButton(sf::Vector2f(600.0f, 360.0f), 1.18f,
                        isVi ? "Đấu Với Máy (AI)" : "Play vs AI",
                        titleBtnPvaiHovered || titleMenuIndex == 1,
                        titleBtnPvaiRect);
        drawTitleButton(sf::Vector2f(600.0f, 440.0f), 1.08f,
                        isVi ? "Quay lại" : "Back",
                        titleBtnModeBackHovered || titleMenuIndex == 2,
                        titleBtnModeBackRect);
    }

    // 3. Corner Action Buttons (Settings, Sound, Info)
    auto drawCircularButton = [this](sf::Vector2f center, float radius, bool hovered, sf::FloatRect& outRect) {
        outRect = sf::FloatRect(center.x - radius, center.y - radius, radius * 2.0f, radius * 2.0f);
        float r = hovered ? (radius * 1.08f) : radius;

        if (hovered) {
            sf::CircleShape glow(r + 3.0f);
            glow.setOrigin(r + 3.0f, r + 3.0f);
            glow.setPosition(center);
            glow.setFillColor(sf::Color(255, 215, 80, 45));
            window.draw(glow);
        }

        sf::CircleShape circle(r);
        circle.setOrigin(r, r);
        circle.setPosition(center);
        circle.setFillColor(hovered ? sf::Color(45, 34, 25, 245) : sf::Color(22, 18, 14, 220));
        circle.setOutlineThickness(hovered ? 2.5f : 2.0f);
        circle.setOutlineColor(hovered ? sf::Color(255, 225, 110, 255) : sf::Color(195, 155, 70, 230));
        window.draw(circle);
    };

    // Button 1: Settings (Bottom-Left: 52, 735)
    drawCircularButton(sf::Vector2f(52.0f, 735.0f), 24.0f, titleBtnSettingsHovered, titleSettingsRect);
    auto& mAssets = getMenuAssets();
    if (mAssets.iconSettings.getSize().x > 0) {
        sf::Sprite gearSp(mAssets.iconSettings);
        float gw = static_cast<float>(mAssets.iconSettings.getSize().x);
        float gh = static_cast<float>(mAssets.iconSettings.getSize().y);
        gearSp.setOrigin(gw / 2.0f, gh / 2.0f);
        gearSp.setPosition(52.0f, 735.0f);
        float gScale = (titleBtnSettingsHovered ? 1.9f : 1.75f);
        gearSp.setScale(gScale, gScale);
        gearSp.setColor(titleBtnSettingsHovered ? sf::Color(255, 235, 140) : sf::Color(245, 235, 215));
        window.draw(gearSp);
    }

    // Button 2: Sound (Bottom-Left: 116, 735) - Quick BGM Toggle
    drawCircularButton(sf::Vector2f(116.0f, 735.0f), 24.0f, titleBtnSoundHovered, titleSoundRect);
    bool bgmOn = soundManager.getBgmBat();
    {
        sf::Color iconCol = titleBtnSoundHovered ? sf::Color(255, 235, 140) : sf::Color(245, 235, 215);
        if (!bgmOn) iconCol = sf::Color(180, 160, 150, 180);

        // Speaker box (body)
        sf::RectangleShape spkBox(sf::Vector2f(6.0f, 10.0f));
        spkBox.setPosition(108.0f, 730.0f);
        spkBox.setFillColor(iconCol);
        window.draw(spkBox);

        // Speaker cone
        sf::ConvexShape spkCone;
        spkCone.setPointCount(4);
        spkCone.setPoint(0, sf::Vector2f(114.0f, 730.0f));
        spkCone.setPoint(1, sf::Vector2f(121.0f, 725.0f));
        spkCone.setPoint(2, sf::Vector2f(121.0f, 745.0f));
        spkCone.setPoint(3, sf::Vector2f(114.0f, 740.0f));
        spkCone.setFillColor(iconCol);
        window.draw(spkCone);

        if (bgmOn) {
            // Sound wave 1
            sf::RectangleShape w1a(sf::Vector2f(2.0f, 6.0f));
            w1a.setPosition(125.0f, 729.0f);
            w1a.setRotation(25.0f);
            w1a.setFillColor(iconCol);
            window.draw(w1a);

            sf::RectangleShape w1b(sf::Vector2f(2.0f, 6.0f));
            w1b.setPosition(125.0f, 736.0f);
            w1b.setRotation(-25.0f);
            w1b.setFillColor(iconCol);
            window.draw(w1b);

            // Sound wave 2
            sf::RectangleShape w2a(sf::Vector2f(2.0f, 8.0f));
            w2a.setPosition(129.0f, 727.0f);
            w2a.setRotation(25.0f);
            w2a.setFillColor(iconCol);
            window.draw(w2a);

            sf::RectangleShape w2b(sf::Vector2f(2.0f, 8.0f));
            w2b.setPosition(129.0f, 737.0f);
            w2b.setRotation(-25.0f);
            w2b.setFillColor(iconCol);
            window.draw(w2b);
        } else {
            // Mute red diagonal slash
            sf::RectangleShape slash(sf::Vector2f(28.0f, 2.5f));
            slash.setOrigin(14.0f, 1.25f);
            slash.setPosition(116.0f, 735.0f);
            slash.setRotation(-40.0f);
            slash.setFillColor(sf::Color(220, 50, 40));
            window.draw(slash);
        }
    }

    // Button 3: Info (Bottom-Right: 1148, 735)
    drawCircularButton(sf::Vector2f(1148.0f, 735.0f), 24.0f, titleBtnInfoHovered, titleInfoRect);
    {
        sf::Text infoText;
        infoText.setFont(fontBold);
        infoText.setString("i");
        infoText.setCharacterSize(titleBtnInfoHovered ? 26 : 24);
        infoText.setStyle(sf::Text::Bold | sf::Text::Italic);
        infoText.setFillColor(titleBtnInfoHovered ? sf::Color(255, 235, 140) : sf::Color(245, 235, 215));
        sf::FloatRect ib = infoText.getLocalBounds();
        infoText.setOrigin(std::round(ib.left + ib.width / 2.0f), std::round(ib.top + ib.height / 2.0f));
        infoText.setPosition(1148.0f, 734.0f);
        window.draw(infoText);
    }

    // Tooltips on hover for Corner Buttons
    if (titleBtnSettingsHovered) {
        std::string tipStr = isVi ? "Cài đặt [S]" : "Settings [S]";
        sf::Text tipTxt;
        tipTxt.setFont(isVi ? fontVnBold : fontEnBold);
        tipTxt.setString(toUtf8(tipStr));
        tipTxt.setCharacterSize(13);
        tipTxt.setFillColor(sf::Color(255, 235, 140));
        sf::FloatRect tb = tipTxt.getLocalBounds();
        tipTxt.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));
        tipTxt.setPosition(52.0f, 692.0f);

        sf::RectangleShape tipBg(sf::Vector2f(tb.width + 16.0f, 24.0f));
        tipBg.setOrigin(std::round((tb.width + 16.0f) / 2.0f), 12.0f);
        tipBg.setPosition(52.0f, 692.0f);
        tipBg.setFillColor(sf::Color(22, 15, 10, 235));
        tipBg.setOutlineThickness(1.0f);
        tipBg.setOutlineColor(sf::Color(210, 170, 75));

        window.draw(tipBg);
        window.draw(tipTxt);
    }

    if (titleBtnSoundHovered) {
        std::string tipStr = isVi ? "Nhạc nền [M]" : "Music BGM [M]";
        sf::Text tipTxt;
        tipTxt.setFont(isVi ? fontVnBold : fontEnBold);
        tipTxt.setString(toUtf8(tipStr));
        tipTxt.setCharacterSize(13);
        tipTxt.setFillColor(sf::Color(255, 235, 140));
        sf::FloatRect tb = tipTxt.getLocalBounds();
        tipTxt.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));
        tipTxt.setPosition(116.0f, 692.0f);

        sf::RectangleShape tipBg(sf::Vector2f(tb.width + 16.0f, 24.0f));
        tipBg.setOrigin(std::round((tb.width + 16.0f) / 2.0f), 12.0f);
        tipBg.setPosition(116.0f, 692.0f);
        tipBg.setFillColor(sf::Color(22, 15, 10, 235));
        tipBg.setOutlineThickness(1.0f);
        tipBg.setOutlineColor(sf::Color(210, 170, 75));

        window.draw(tipBg);
        window.draw(tipTxt);
    }

    if (titleBtnInfoHovered) {
        std::string tipStr = isVi ? "Thông tin [I]" : "Info [I]";
        sf::Text tipTxt;
        tipTxt.setFont(isVi ? fontVnBold : fontEnBold);
        tipTxt.setString(toUtf8(tipStr));
        tipTxt.setCharacterSize(13);
        tipTxt.setFillColor(sf::Color(255, 235, 140));
        sf::FloatRect tb = tipTxt.getLocalBounds();
        tipTxt.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));
        tipTxt.setPosition(1148.0f, 692.0f);

        sf::RectangleShape tipBg(sf::Vector2f(tb.width + 16.0f, 24.0f));
        tipBg.setOrigin(std::round((tb.width + 16.0f) / 2.0f), 12.0f);
        tipBg.setPosition(1148.0f, 692.0f);
        tipBg.setFillColor(sf::Color(22, 15, 10, 235));
        tipBg.setOutlineThickness(1.0f);
        tipBg.setOutlineColor(sf::Color(210, 170, 75));

        window.draw(tipBg);
        window.draw(tipTxt);
    }
}


