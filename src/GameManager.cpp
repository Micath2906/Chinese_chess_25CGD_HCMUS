#include "GameManager.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>

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
    : window(sf::VideoMode(1200, 800), "Co Tuong - Chinese Chess (HCMUS OOP)", sf::Style::Close | sf::Style::Titlebar),
      trangThai(TrangThai::MENU_CHINH),
      cheDoChoi(CheDoChoi::HAI_NGUOI),
      rebindingAction(KeyAction::COUNT),
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
      offsetX(65.0f),
      offsetY(60.0f),
      thoiGianVanDau(600.0f), // 10 minutes default
      hienGoiY(true),
      thoiGianConDo(600.0f),
      thoiGianConDen(600.0f),
      aiDangSuyNghi(false),
      thoiGianChoAI(0.0f),
      toastTimer(0.0f),
      lyDoKetThuc("") {
    
    window.setFramerateLimit(60);
    khoiTao();
}

void GameManager::khoiTao() {
    if (!font.loadFromFile("resources/fonts/arial.ttf")) {
        std::cerr << "Khong the load font tai resources/fonts/arial.ttf" << std::endl;
    }
    
    soundManager.khoiTao();
    
    // Create text inputs
    inputP1Name = std::make_unique<TextInput>(150.0f, 215.0f, 320.0f, 42.0f, font, "Player 1", 14);
    inputP1Name->setValue("Player 1");

    inputP2Name = std::make_unique<TextInput>(730.0f, 215.0f, 320.0f, 42.0f, font, "Player 2", 14);
    inputP2Name->setValue("Player 2");

    inputAiPlayerName = std::make_unique<TextInput>(150.0f, 215.0f, 320.0f, 42.0f, font, "Player", 14);
    inputAiPlayerName->setValue("Player");

    slotInfos.resize(3);
    capNhatThongTinSlots();

    khoiTaoTatCaMenu();
    khoiTaoNutTrongGame();
    khoiTaoNutPopup();
}

void GameManager::khoiTaoTatCaMenu() {
    float bw = 340.0f;
    float bh = 48.0f;
    float cx = (window.getSize().x - bw) / 2.0f;
    float sp = 62.0f;

    // ==========================================
    // 1. Main Menu (No numbering!)
    // ==========================================
    menuChinh.setFont(font);
    menuChinh.xoaTatCaButton();
    float startY = 230.0f;

    // Play
    menuChinh.themButton(cx, startY, bw, bh, Loc::get(LocKey::MENU_PLAY), [this]() {
        soundManager.play(SoundType::CLICK);
        trangThai = TrangThai::PLAY_SUBMENU;
    });
    // Load
    menuChinh.themButton(cx, startY + sp, bw, bh, Loc::get(LocKey::MENU_LOAD), [this]() {
        soundManager.play(SoundType::CLICK);
        capNhatThongTinSlots();
        trangThai = TrangThai::LOAD_MENU;
    });
    // Settings
    menuChinh.themButton(cx, startY + sp * 2, bw, bh, Loc::get(LocKey::MENU_SETTINGS), [this]() {
        soundManager.play(SoundType::CLICK);
        trangThai = TrangThai::SETTINGS_MENU;
    });
    // Introduction
    menuChinh.themButton(cx, startY + sp * 3, bw, bh, Loc::get(LocKey::MENU_INTRO), [this]() {
        soundManager.play(SoundType::CLICK);
        trangThai = TrangThai::INTRODUCTION_MENU;
    });
    // Exit
    menuChinh.themButton(cx, startY + sp * 4, bw, bh, Loc::get(LocKey::MENU_EXIT), [this]() {
        soundManager.play(SoundType::CLICK);
        window.close();
    });

    // ==========================================
    // 2. Play Submenu
    // ==========================================
    menuPlaySub.setFont(font);
    menuPlaySub.xoaTatCaButton();
    float psubY = 235.0f;

    // Player vs Player
    menuPlaySub.themButton(cx, psubY, bw, bh, Loc::get(LocKey::PLAY_PVP), [this]() {
        soundManager.play(SoundType::CLICK);
        pvpRollBanner = "";
        trangThai = TrangThai::PVP_SETUP;
    });
    // Player vs AI
    menuPlaySub.themButton(cx, psubY + sp, bw, bh, Loc::get(LocKey::PLAY_PVAI), [this]() {
        soundManager.play(SoundType::CLICK);
        aiRollBanner = "";
        trangThai = TrangThai::PVAI_SETUP;
    });
    // Demo Mode (2 Chariots & 2 Elephants)
    menuPlaySub.themButton(cx, psubY + sp * 2, bw, bh, Loc::get(LocKey::PLAY_DEMO), [this]() {
        soundManager.play(SoundType::CLICK);
        tenNguoiChoiDo = (Loc::getLanguage() == Language::TIENG_VIET) ? "Quan Do (Xe & Voi)" : "Red (Chariot & Elephant)";
        tenNguoiChoiDen = (Loc::getLanguage() == Language::TIENG_VIET) ? "Quan Den (Xe & Voi)" : "Black (Chariot & Elephant)";
        nhanVatDo = 0;
        nhanVatDen = 0;
        batDauTroChoiMoi(CheDoChoi::DEMO_2XE_2VOI);
    });
    // Back
    menuPlaySub.themButton(cx, psubY + sp * 3, bw, bh, Loc::get(LocKey::BACK), [this]() {
        soundManager.play(SoundType::CLICK);
        trangThai = TrangThai::MENU_CHINH;
    });

    // ==========================================
    // 3. PvP Setup Menu (With Match Timer)
    // ==========================================
    menuPvpSetup.setFont(font);
    menuPvpSetup.xoaTatCaButton();
    
    // P1 Char selector buttons
    menuPvpSetup.themButton(150.0f, 305.0f, 45.0f, 42.0f, "<", [this]() {
        soundManager.play(SoundType::CLICK);
        p1CharIndex = (p1CharIndex + Loc::getCharCount() - 1) % Loc::getCharCount();
        capNhatNhanNut();
    });
    menuPvpSetup.themButton(205.0f, 305.0f, 210.0f, 42.0f, Loc::getCharName(p1CharIndex), [this]() {
        soundManager.play(SoundType::CLICK);
        p1CharIndex = (p1CharIndex + 1) % Loc::getCharCount();
        capNhatNhanNut();
    });
    menuPvpSetup.themButton(425.0f, 305.0f, 45.0f, 42.0f, ">", [this]() {
        soundManager.play(SoundType::CLICK);
        p1CharIndex = (p1CharIndex + 1) % Loc::getCharCount();
        capNhatNhanNut();
    });

    // P2 Char selector buttons
    menuPvpSetup.themButton(730.0f, 305.0f, 45.0f, 42.0f, "<", [this]() {
        soundManager.play(SoundType::CLICK);
        p2CharIndex = (p2CharIndex + Loc::getCharCount() - 1) % Loc::getCharCount();
        capNhatNhanNut();
    });
    menuPvpSetup.themButton(785.0f, 305.0f, 210.0f, 42.0f, Loc::getCharName(p2CharIndex), [this]() {
        soundManager.play(SoundType::CLICK);
        p2CharIndex = (p2CharIndex + 1) % Loc::getCharCount();
        capNhatNhanNut();
    });
    menuPvpSetup.themButton(1005.0f, 305.0f, 45.0f, 42.0f, ">", [this]() {
        soundManager.play(SoundType::CLICK);
        p2CharIndex = (p2CharIndex + 1) % Loc::getCharCount();
        capNhatNhanNut();
    });

    // Match Timer selector directly in Setup
    auto getTimerStr = [this]() {
        if (thoiGianVanDau <= 0.0f) return Loc::get(LocKey::MATCH_TIMER_LABEL) + Loc::get(LocKey::TIME_UNLIMITED);
        if (thoiGianVanDau == 300.0f) return Loc::get(LocKey::MATCH_TIMER_LABEL) + Loc::get(LocKey::TIME_5M);
        if (thoiGianVanDau == 600.0f) return Loc::get(LocKey::MATCH_TIMER_LABEL) + Loc::get(LocKey::TIME_10M);
        return Loc::get(LocKey::MATCH_TIMER_LABEL) + Loc::get(LocKey::TIME_15M);
    };

    menuPvpSetup.themButton(360.0f, 410.0f, 480.0f, 46.0f, getTimerStr(), [this, getTimerStr]() {
        soundManager.play(SoundType::CLICK);
        if (thoiGianVanDau == 300.0f) thoiGianVanDau = 600.0f;
        else if (thoiGianVanDau == 600.0f) thoiGianVanDau = 900.0f;
        else if (thoiGianVanDau == 900.0f) thoiGianVanDau = 0.0f;
        else thoiGianVanDau = 300.0f;
        if (auto b = menuPvpSetup.layButton(6)) b->setLabel(getTimerStr());
    });

    // Center Random Roll Button
    menuPvpSetup.themButton(360.0f, 468.0f, 480.0f, 48.0f, Loc::get(LocKey::TOSS_COIN), [this]() {
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

    // Bottom Start & Back
    menuPvpSetup.themButton(340.0f, 650.0f, 240.0f, 52.0f, Loc::get(LocKey::START_GAME), [this]() {
        soundManager.play(SoundType::CLICK);
        batDauPvp();
    });
    menuPvpSetup.themButton(620.0f, 650.0f, 240.0f, 52.0f, Loc::get(LocKey::BACK), [this]() {
        soundManager.play(SoundType::CLICK);
        trangThai = TrangThai::PLAY_SUBMENU;
    });

    // ==========================================
    // 4. PvAI Setup Menu (With Match Timer)
    // ==========================================
    menuPvaiSetup.setFont(font);
    menuPvaiSetup.xoaTatCaButton();

    // Player Char buttons
    menuPvaiSetup.themButton(150.0f, 305.0f, 45.0f, 42.0f, "<", [this]() {
        soundManager.play(SoundType::CLICK);
        aiPlayerCharIndex = (aiPlayerCharIndex + Loc::getCharCount() - 1) % Loc::getCharCount();
        capNhatNhanNut();
    });
    menuPvaiSetup.themButton(205.0f, 305.0f, 210.0f, 42.0f, Loc::getCharName(aiPlayerCharIndex), [this]() {
        soundManager.play(SoundType::CLICK);
        aiPlayerCharIndex = (aiPlayerCharIndex + 1) % Loc::getCharCount();
        capNhatNhanNut();
    });
    menuPvaiSetup.themButton(425.0f, 305.0f, 45.0f, 42.0f, ">", [this]() {
        soundManager.play(SoundType::CLICK);
        aiPlayerCharIndex = (aiPlayerCharIndex + 1) % Loc::getCharCount();
        capNhatNhanNut();
    });

    // AI Difficulty Button
    auto getDiffLabel = [this]() {
        LocKey k = LocKey::DIFF_MED;
        if (aiDifficulty == DoKho::DE) k = LocKey::DIFF_EASY;
        else if (aiDifficulty == DoKho::KHO) k = LocKey::DIFF_HARD;
        return Loc::get(LocKey::AI_DIFFICULTY) + Loc::get(k);
    };

    menuPvaiSetup.themButton(730.0f, 215.0f, 320.0f, 42.0f, getDiffLabel(), [this, getDiffLabel]() {
        soundManager.play(SoundType::CLICK);
        if (aiDifficulty == DoKho::DE) aiDifficulty = DoKho::TRUNG_BINH;
        else if (aiDifficulty == DoKho::TRUNG_BINH) aiDifficulty = DoKho::KHO;
        else aiDifficulty = DoKho::DE;
        if (auto b = menuPvaiSetup.layButton(3)) b->setLabel(getDiffLabel());
    });

    // Match Timer selector directly in PvAI Setup
    menuPvaiSetup.themButton(360.0f, 395.0f, 480.0f, 44.0f, getTimerStr(), [this, getTimerStr]() {
        soundManager.play(SoundType::CLICK);
        if (thoiGianVanDau == 300.0f) thoiGianVanDau = 600.0f;
        else if (thoiGianVanDau == 600.0f) thoiGianVanDau = 900.0f;
        else if (thoiGianVanDau == 900.0f) thoiGianVanDau = 0.0f;
        else thoiGianVanDau = 300.0f;
        if (auto b = menuPvaiSetup.layButton(4)) b->setLabel(getTimerStr());
    });

    // Who goes first toggle
    auto getFirstChoiceLabel = [this]() {
        std::string modeStr;
        if (aiFirstChoice == 0) modeStr = Loc::get(LocKey::PLAYER_FIRST);
        else if (aiFirstChoice == 1) modeStr = Loc::get(LocKey::AI_FIRST);
        else modeStr = Loc::get(LocKey::RANDOM_FIRST);
        return Loc::get(LocKey::WHO_GOES_FIRST) + modeStr;
    };

    menuPvaiSetup.themButton(360.0f, 450.0f, 480.0f, 44.0f, getFirstChoiceLabel(), [this, getFirstChoiceLabel]() {
        soundManager.play(SoundType::CLICK);
        aiFirstChoice = (aiFirstChoice + 1) % 3;
        if (auto b = menuPvaiSetup.layButton(5)) b->setLabel(getFirstChoiceLabel());
    });

    // Random roll button
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

    // Bottom Start & Back
    menuPvaiSetup.themButton(340.0f, 650.0f, 240.0f, 52.0f, Loc::get(LocKey::START_GAME), [this]() {
        soundManager.play(SoundType::CLICK);
        batDauPvai();
    });
    menuPvaiSetup.themButton(620.0f, 650.0f, 240.0f, 52.0f, Loc::get(LocKey::BACK), [this]() {
        soundManager.play(SoundType::CLICK);
        trangThai = TrangThai::PLAY_SUBMENU;
    });

    // ==========================================
    // 5. Load Game Menu
    // ==========================================
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
        trangThai = TrangThai::MENU_CHINH;
    });

    // ==========================================
    // 6. Settings Menu (Clean without Match Timer)
    // ==========================================
    menuSettings.setFont(font);
    menuSettings.xoaTatCaButton();
    float setY = 190.0f;
    float setSp = 65.0f;

    // Sound FX
    menuSettings.themButton(cx, setY, bw, bh, 
        Loc::get(LocKey::SOUND_FX) + (soundManager.getAmThanhBat() ? Loc::get(LocKey::ON) : Loc::get(LocKey::OFF)), [this]() {
        bool bat = !soundManager.getAmThanhBat();
        soundManager.setAmThanhBat(bat);
        capNhatNhanNut();
        soundManager.play(SoundType::CLICK);
    });

    // Volume
    menuSettings.themButton(cx, setY + setSp, bw, bh,
        Loc::get(LocKey::SOUND_VOLUME) + std::to_string(static_cast<int>(soundManager.getAmLuong())) + "%", [this]() {
        float cur = soundManager.getAmLuong();
        float next = (cur >= 100.0f) ? 25.0f : (cur + 25.0f);
        soundManager.setAmLuong(next);
        capNhatNhanNut();
        soundManager.play(SoundType::CLICK);
    });

    // Custom Key Bindings
    menuSettings.themButton(cx, setY + setSp * 2, bw, bh, Loc::get(LocKey::KEY_BINDINGS_MENU), [this]() {
        soundManager.play(SoundType::CLICK);
        rebindingAction = KeyAction::COUNT;
        trangThai = TrangThai::KEYBINDING_MENU;
    });

    // Language toggle
    menuSettings.themButton(cx, setY + setSp * 3, bw, bh, Loc::get(LocKey::LANGUAGE_LABEL), [this]() {
        Loc::toggleLanguage();
        capNhatNhanNut();
        soundManager.play(SoundType::CLICK);
    });

    // Move hints
    menuSettings.themButton(cx, setY + setSp * 4, bw, bh,
        Loc::get(LocKey::HINT_MOVES) + (hienGoiY ? Loc::get(LocKey::ON) : Loc::get(LocKey::OFF)), [this]() {
        hienGoiY = !hienGoiY;
        capNhatNhanNut();
        soundManager.play(SoundType::CLICK);
    });

    // Back
    menuSettings.themButton(cx, setY + setSp * 5 + 15.0f, bw, bh, Loc::get(LocKey::BACK), [this]() {
        soundManager.play(SoundType::CLICK);
        trangThai = TrangThai::MENU_CHINH;
    });

    // ==========================================
    // 7. Interactive Keybinding Menu
    // ==========================================
    menuKeybinding.setFont(font);
    menuKeybinding.xoaTatCaButton();
    float kbX = 390.0f;
    float kbW = 420.0f;
    float kbH = 40.0f;
    float kbY = 160.0f;
    float kbSp = 46.0f;

    // Up
    menuKeybinding.themButton(kbX, kbY, kbW, kbH, "", [this]() {
        rebindingAction = KeyAction::UP;
        capNhatNhanNut();
        soundManager.play(SoundType::CLICK);
    });
    // Down
    menuKeybinding.themButton(kbX, kbY + kbSp, kbW, kbH, "", [this]() {
        rebindingAction = KeyAction::DOWN;
        capNhatNhanNut();
        soundManager.play(SoundType::CLICK);
    });
    // Left
    menuKeybinding.themButton(kbX, kbY + kbSp * 2, kbW, kbH, "", [this]() {
        rebindingAction = KeyAction::LEFT;
        capNhatNhanNut();
        soundManager.play(SoundType::CLICK);
    });
    // Right
    menuKeybinding.themButton(kbX, kbY + kbSp * 3, kbW, kbH, "", [this]() {
        rebindingAction = KeyAction::RIGHT;
        capNhatNhanNut();
        soundManager.play(SoundType::CLICK);
    });
    // Select
    menuKeybinding.themButton(kbX, kbY + kbSp * 4, kbW, kbH, "", [this]() {
        rebindingAction = KeyAction::SELECT;
        capNhatNhanNut();
        soundManager.play(SoundType::CLICK);
    });
    // Deselect
    menuKeybinding.themButton(kbX, kbY + kbSp * 5, kbW, kbH, "", [this]() {
        rebindingAction = KeyAction::DESELECT;
        capNhatNhanNut();
        soundManager.play(SoundType::CLICK);
    });
    // Quit
    menuKeybinding.themButton(kbX, kbY + kbSp * 6, kbW, kbH, "", [this]() {
        rebindingAction = KeyAction::QUIT;
        capNhatNhanNut();
        soundManager.play(SoundType::CLICK);
    });

    // Presets & Reset
    float preY = kbY + kbSp * 7 + 25.0f;
    float preW = 210.0f;
    menuKeybinding.themButton(240.0f, preY, preW, 46.0f, Loc::get(LocKey::PRESET_WASD_BTN), [this]() {
        keyConfig.setPreset(KeyPreset::WASD);
        rebindingAction = KeyAction::COUNT;
        capNhatNhanNut();
        soundManager.play(SoundType::CLICK);
    });
    menuKeybinding.themButton(495.0f, preY, preW, 46.0f, Loc::get(LocKey::PRESET_ARROWS_BTN), [this]() {
        keyConfig.setPreset(KeyPreset::ARROWS);
        rebindingAction = KeyAction::COUNT;
        capNhatNhanNut();
        soundManager.play(SoundType::CLICK);
    });
    menuKeybinding.themButton(750.0f, preY, preW, 46.0f, Loc::get(LocKey::KEY_RESET_DEFAULT), [this]() {
        keyConfig.setPreset(KeyPreset::WASD);
        rebindingAction = KeyAction::COUNT;
        capNhatNhanNut();
        soundManager.play(SoundType::CLICK);
    });

    // Back to Settings
    menuKeybinding.themButton((window.getSize().x - 220.0f) / 2.0f, preY + 68.0f, 220.0f, 48.0f, Loc::get(LocKey::BACK), [this]() {
        rebindingAction = KeyAction::COUNT;
        soundManager.play(SoundType::CLICK);
        trangThai = TrangThai::SETTINGS_MENU;
    });

    // ==========================================
    // 8. Introduction Menu
    // ==========================================
    menuIntro.setFont(font);
    menuIntro.xoaTatCaButton();
    menuIntro.themButton((window.getSize().x - 220.0f) / 2.0f, 715.0f, 220.0f, 48.0f, Loc::get(LocKey::BACK), [this]() {
        soundManager.play(SoundType::CLICK);
        trangThai = TrangThai::MENU_CHINH;
    });

    capNhatNhanNut();
}

void GameManager::capNhatNhanNut() {
    // 1. Menu Chinh
    if (auto b = menuChinh.layButton(0)) b->setLabel(Loc::get(LocKey::MENU_PLAY));
    if (auto b = menuChinh.layButton(1)) b->setLabel(Loc::get(LocKey::MENU_LOAD));
    if (auto b = menuChinh.layButton(2)) b->setLabel(Loc::get(LocKey::MENU_SETTINGS));
    if (auto b = menuChinh.layButton(3)) b->setLabel(Loc::get(LocKey::MENU_INTRO));
    if (auto b = menuChinh.layButton(4)) b->setLabel(Loc::get(LocKey::MENU_EXIT));

    // 2. Play Submenu
    if (auto b = menuPlaySub.layButton(0)) b->setLabel(Loc::get(LocKey::PLAY_PVP));
    if (auto b = menuPlaySub.layButton(1)) b->setLabel(Loc::get(LocKey::PLAY_PVAI));
    if (auto b = menuPlaySub.layButton(2)) b->setLabel(Loc::get(LocKey::PLAY_DEMO));
    if (auto b = menuPlaySub.layButton(3)) b->setLabel(Loc::get(LocKey::BACK));

    // Helper for timer
    auto getTimerStr = [this]() {
        if (thoiGianVanDau <= 0.0f) return Loc::get(LocKey::MATCH_TIMER_LABEL) + Loc::get(LocKey::TIME_UNLIMITED);
        if (thoiGianVanDau == 300.0f) return Loc::get(LocKey::MATCH_TIMER_LABEL) + Loc::get(LocKey::TIME_5M);
        if (thoiGianVanDau == 600.0f) return Loc::get(LocKey::MATCH_TIMER_LABEL) + Loc::get(LocKey::TIME_10M);
        return Loc::get(LocKey::MATCH_TIMER_LABEL) + Loc::get(LocKey::TIME_15M);
    };

    // 3. PvP Setup
    if (auto b = menuPvpSetup.layButton(1)) b->setLabel(Loc::getCharName(p1CharIndex));
    if (auto b = menuPvpSetup.layButton(4)) b->setLabel(Loc::getCharName(p2CharIndex));
    if (auto b = menuPvpSetup.layButton(6)) b->setLabel(getTimerStr());
    if (auto b = menuPvpSetup.layButton(7)) b->setLabel(Loc::get(LocKey::TOSS_COIN));
    if (auto b = menuPvpSetup.layButton(8)) b->setLabel(Loc::get(LocKey::START_GAME));
    if (auto b = menuPvpSetup.layButton(9)) b->setLabel(Loc::get(LocKey::BACK));

    // 4. PvAI Setup
    if (auto b = menuPvaiSetup.layButton(1)) b->setLabel(Loc::getCharName(aiPlayerCharIndex));
    if (auto b = menuPvaiSetup.layButton(3)) {
        LocKey k = LocKey::DIFF_MED;
        if (aiDifficulty == DoKho::DE) k = LocKey::DIFF_EASY;
        else if (aiDifficulty == DoKho::KHO) k = LocKey::DIFF_HARD;
        b->setLabel(Loc::get(LocKey::AI_DIFFICULTY) + Loc::get(k));
    }
    if (auto b = menuPvaiSetup.layButton(4)) b->setLabel(getTimerStr());
    if (auto b = menuPvaiSetup.layButton(5)) {
        std::string modeStr;
        if (aiFirstChoice == 0) modeStr = Loc::get(LocKey::PLAYER_FIRST);
        else if (aiFirstChoice == 1) modeStr = Loc::get(LocKey::AI_FIRST);
        else modeStr = Loc::get(LocKey::RANDOM_FIRST);
        b->setLabel(Loc::get(LocKey::WHO_GOES_FIRST) + modeStr);
    }
    if (auto b = menuPvaiSetup.layButton(6)) b->setLabel(Loc::get(LocKey::TOSS_COIN));
    if (auto b = menuPvaiSetup.layButton(7)) b->setLabel(Loc::get(LocKey::START_GAME));
    if (auto b = menuPvaiSetup.layButton(8)) b->setLabel(Loc::get(LocKey::BACK));

    // 5. Load Menu
    if (auto b = menuLoad.layButton(0)) b->setLabel(Loc::get(LocKey::LOAD_BUTTON));
    if (auto b = menuLoad.layButton(1)) b->setLabel(Loc::get(LocKey::DELETE_BUTTON));
    if (auto b = menuLoad.layButton(2)) b->setLabel(Loc::get(LocKey::BACK));

    // 6. Settings Menu
    if (auto b = menuSettings.layButton(0)) {
        b->setLabel(Loc::get(LocKey::SOUND_FX) + (soundManager.getAmThanhBat() ? Loc::get(LocKey::ON) : Loc::get(LocKey::OFF)));
    }
    if (auto b = menuSettings.layButton(1)) {
        b->setLabel(Loc::get(LocKey::SOUND_VOLUME) + std::to_string(static_cast<int>(soundManager.getAmLuong())) + "%");
    }
    if (auto b = menuSettings.layButton(2)) {
        b->setLabel(Loc::get(LocKey::KEY_BINDINGS_MENU));
    }
    if (auto b = menuSettings.layButton(3)) {
        b->setLabel(Loc::get(LocKey::LANGUAGE_LABEL));
    }
    if (auto b = menuSettings.layButton(4)) {
        b->setLabel(Loc::get(LocKey::HINT_MOVES) + (hienGoiY ? Loc::get(LocKey::ON) : Loc::get(LocKey::OFF)));
    }
    if (auto b = menuSettings.layButton(5)) {
        b->setLabel(Loc::get(LocKey::BACK));
    }

    // 7. Keybindings interactive labels
    auto formatKeyBtn = [this](KeyAction action, LocKey nameKey) {
        if (rebindingAction == action) {
            return Loc::get(LocKey::KEY_PRESS_PROMPT);
        }
        return Loc::get(nameKey) + ":   [ " + KeyConfig::getKeyName(keyConfig.getActionKey(action)) + " ]";
    };

    if (auto b = menuKeybinding.layButton(0)) b->setLabel(formatKeyBtn(KeyAction::UP, LocKey::KEY_ACTION_UP));
    if (auto b = menuKeybinding.layButton(1)) b->setLabel(formatKeyBtn(KeyAction::DOWN, LocKey::KEY_ACTION_DOWN));
    if (auto b = menuKeybinding.layButton(2)) b->setLabel(formatKeyBtn(KeyAction::LEFT, LocKey::KEY_ACTION_LEFT));
    if (auto b = menuKeybinding.layButton(3)) b->setLabel(formatKeyBtn(KeyAction::RIGHT, LocKey::KEY_ACTION_RIGHT));
    if (auto b = menuKeybinding.layButton(4)) b->setLabel(formatKeyBtn(KeyAction::SELECT, LocKey::KEY_ACTION_SELECT));
    if (auto b = menuKeybinding.layButton(5)) b->setLabel(formatKeyBtn(KeyAction::DESELECT, LocKey::KEY_ACTION_DESELECT));
    if (auto b = menuKeybinding.layButton(6)) b->setLabel(formatKeyBtn(KeyAction::QUIT, LocKey::KEY_ACTION_QUIT));

    if (auto b = menuKeybinding.layButton(7)) b->setLabel(Loc::get(LocKey::PRESET_WASD_BTN));
    if (auto b = menuKeybinding.layButton(8)) b->setLabel(Loc::get(LocKey::PRESET_ARROWS_BTN));
    if (auto b = menuKeybinding.layButton(9)) b->setLabel(Loc::get(LocKey::KEY_RESET_DEFAULT));
    if (auto b = menuKeybinding.layButton(10)) b->setLabel(Loc::get(LocKey::BACK));

    // 8. Intro
    if (auto b = menuIntro.layButton(0)) b->setLabel(Loc::get(LocKey::BACK));

    // In-game buttons
    khoiTaoNutTrongGame();
    khoiTaoNutPopup();
}

void GameManager::khoiTaoNutTrongGame() {
    inGameButtons.clear();
    float bx = 680.0f;
    float by = 470.0f;
    float bw = 220.0f;
    float bh = 45.0f;
    float gapX = 245.0f;
    float gapY = 55.0f;

    // Row 1: Undo & Redo
    inGameButtons.push_back(std::make_unique<Button>(bx, by, bw, bh, Loc::get(LocKey::UNDO), font, [this]() {
        thucHienHoanTac();
    }));
    inGameButtons.push_back(std::make_unique<Button>(bx + gapX, by, bw, bh, Loc::get(LocKey::REDO), font, [this]() {
        thucHienDiTiep();
    }));

    // Row 2: New game & Offer draw
    inGameButtons.push_back(std::make_unique<Button>(bx, by + gapY, bw, bh, Loc::get(LocKey::NEW_GAME), font, [this]() {
        batDauTroChoiMoi(cheDoChoi);
    }));
    inGameButtons.push_back(std::make_unique<Button>(bx + gapX, by + gapY, bw, bh, Loc::get(LocKey::DRAW_OFFER), font, [this]() {
        thucHienXinHoa();
    }));

    // Row 3: Resign & Save
    inGameButtons.push_back(std::make_unique<Button>(bx, by + gapY * 2, bw, bh, Loc::get(LocKey::SURRENDER), font, [this]() {
        thucHienDauHang();
    }));
    inGameButtons.push_back(std::make_unique<Button>(bx + gapX, by + gapY * 2, bw, bh, Loc::get(LocKey::SAVE_GAME), font, [this]() {
        thucHienLuuGame();
    }));

    // Row 4: Load & Menu
    inGameButtons.push_back(std::make_unique<Button>(bx, by + gapY * 3, bw, bh, Loc::get(LocKey::LOAD_GAME), font, [this]() {
        thucHienLoadGame();
    }));
    inGameButtons.push_back(std::make_unique<Button>(bx + gapX, by + gapY * 3, bw, bh, Loc::get(LocKey::BACK_MENU), font, [this]() {
        quayLaiMenu();
    }));
}

void GameManager::khoiTaoNutPopup() {
    popupButtons.clear();
    float pw = 200.0f;
    float ph = 50.0f;
    float cx = window.getSize().x / 2.0f;
    float py = 450.0f;

    popupButtons.push_back(std::make_unique<Button>(cx - pw - 20.0f, py, pw, ph, Loc::get(LocKey::PLAY_AGAIN), font, [this]() {
        soundManager.play(SoundType::CLICK);
        batDauTroChoiMoi(cheDoChoi);
    }));

    popupButtons.push_back(std::make_unique<Button>(cx + 20.0f, py, pw, ph, Loc::get(LocKey::BACK_MENU), font, [this]() {
        soundManager.play(SoundType::CLICK);
        quayLaiMenu();
    }));
}

void GameManager::batDauPvp() {
    std::string n1 = inputP1Name->getValue();
    if (n1.empty()) n1 = "Player 1";
    std::string n2 = inputP2Name->getValue();
    if (n2.empty()) n2 = "Player 2";

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
        tenNguoiChoiDen = aiName;
        nhanVatDen = 1;
    } else {
        tenNguoiChoiDo = aiName;
        nhanVatDo = 1;
        tenNguoiChoiDen = n;
        nhanVatDen = aiPlayerCharIndex;
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
    
    thoiGianConDo = thoiGianVanDau;
    thoiGianConDen = thoiGianVanDau;
    dtClock.restart();
    
    aiDangSuyNghi = false;
    thoiGianChoAI = 0.0f;
    lyDoKetThuc = "";
    toastTimer = 0.0f;

    soundManager.play(SoundType::MOVE);
}

// ==========================================
// Save/Load System
// ==========================================
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
        aiDangSuyNghi = false;
        thoiGianChoAI = 0.0f;
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

// ==========================================
// In-game Logic
// ==========================================
void GameManager::chay() {
    sf::Clock clock;
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        
        xuLySuKien();
        
        if (trangThai == TrangThai::DANG_CHOI && !banCo.getKetThuc()) {
            capNhatDongHo(dt);
            if (cheDoChoi == CheDoChoi::VOI_MAY) {
                xuLyLuotAI(dt);
            }
        }
        
        if (toastTimer > 0.0f) {
            toastTimer -= dt;
        }
        
        ve();
    }
}

void GameManager::xuLySuKien() {
    sf::Event event;
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    
    // Update button hovers
    if (trangThai == TrangThai::MENU_CHINH) menuChinh.update(mousePos);
    else if (trangThai == TrangThai::PLAY_SUBMENU) menuPlaySub.update(mousePos);
    else if (trangThai == TrangThai::PVP_SETUP) menuPvpSetup.update(mousePos);
    else if (trangThai == TrangThai::PVAI_SETUP) menuPvaiSetup.update(mousePos);
    else if (trangThai == TrangThai::LOAD_MENU) menuLoad.update(mousePos);
    else if (trangThai == TrangThai::SETTINGS_MENU) menuSettings.update(mousePos);
    else if (trangThai == TrangThai::KEYBINDING_MENU) menuKeybinding.update(mousePos);
    else if (trangThai == TrangThai::INTRODUCTION_MENU) menuIntro.update(mousePos);
    else if (trangThai == TrangThai::DANG_CHOI) {
        if (banCo.getKetThuc()) {
            for (auto& btn : popupButtons) btn->update(mousePos);
        } else {
            for (auto& btn : inGameButtons) btn->update(mousePos);
        }
    }

    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        else if (event.type == sf::Event::TextEntered) {
            xuLyTextEntered(event.text.unicode);
        }
        else if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                xuLyClickChuot(mousePos);
            }
        }
        else if (event.type == sf::Event::KeyPressed) {
            xuLyBanPhim(event.key.code, event.key.control);
        }
    }
}

void GameManager::xuLyTextEntered(sf::Uint32 unicode) {
    if (trangThai == TrangThai::PVP_SETUP) {
        inputP1Name->handleTextEntered(unicode);
        inputP2Name->handleTextEntered(unicode);
    } else if (trangThai == TrangThai::PVAI_SETUP) {
        inputAiPlayerName->handleTextEntered(unicode);
    }
}

void GameManager::xuLyClickChuot(const sf::Vector2i& viTri) {
    if (trangThai == TrangThai::MENU_CHINH) {
        menuChinh.handleClick(viTri);
    }
    else if (trangThai == TrangThai::PLAY_SUBMENU) {
        menuPlaySub.handleClick(viTri);
    }
    else if (trangThai == TrangThai::PVP_SETUP) {
        inputP1Name->handleClick(viTri);
        inputP2Name->handleClick(viTri);
        menuPvpSetup.handleClick(viTri);
    }
    else if (trangThai == TrangThai::PVAI_SETUP) {
        inputAiPlayerName->handleClick(viTri);
        menuPvaiSetup.handleClick(viTri);
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
        menuSettings.handleClick(viTri);
    }
    else if (trangThai == TrangThai::KEYBINDING_MENU) {
        menuKeybinding.handleClick(viTri);
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

        for (auto& btn : inGameButtons) {
            btn->handleClick(viTri);
        }

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
    // 1. If currently in interactive key rebinding mode
    if (trangThai == TrangThai::KEYBINDING_MENU && rebindingAction != KeyAction::COUNT) {
        if (key != sf::Keyboard::Escape) {
            keyConfig.setActionKey(rebindingAction, key);
            soundManager.play(SoundType::CLICK);
        }
        rebindingAction = KeyAction::COUNT;
        capNhatNhanNut();
        return;
    }

    // 2. Global Quick Quit (only if not rebinding)
    if (keyConfig.isQuitKey(key)) {
        soundManager.play(SoundType::CLICK);
        window.close();
        return;
    }

    if (trangThai == TrangThai::MENU_CHINH) {
        return;
    }

    // 3. Deselect or Return back
    if (keyConfig.isDeselectKey(key)) {
        soundManager.play(SoundType::CLICK);
        if (trangThai == TrangThai::PLAY_SUBMENU || trangThai == TrangThai::LOAD_MENU || 
            trangThai == TrangThai::SETTINGS_MENU || trangThai == TrangThai::INTRODUCTION_MENU) {
            trangThai = TrangThai::MENU_CHINH;
            return;
        }
        if (trangThai == TrangThai::PVP_SETUP || trangThai == TrangThai::PVAI_SETUP) {
            trangThai = TrangThai::PLAY_SUBMENU;
            return;
        }
        if (trangThai == TrangThai::KEYBINDING_MENU) {
            rebindingAction = KeyAction::COUNT;
            trangThai = TrangThai::SETTINGS_MENU;
            return;
        }
        if (trangThai == TrangThai::DANG_CHOI) {
            if (quanDangChon != nullptr) {
                quanDangChon = nullptr;
                cacNuocDiHopLe.clear();
            } else {
                quayLaiMenu();
            }
            return;
        }
    }

    // 4. In game navigation and shortcuts
    if (trangThai == TrangThai::DANG_CHOI) {
        if (banCo.getKetThuc()) return;

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
        thoiGianChoAI = 0.45f;
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
                    soundManager.play(SoundType::CHECK);
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
                soundManager.play(SoundType::CHECK);
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
    trangThai = TrangThai::MENU_CHINH;
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

// ==========================================
// Rendering Pipeline
// ==========================================
void GameManager::ve() {
    // Elegant deep slate background
    window.clear(sf::Color(14, 18, 26));

    if (trangThai == TrangThai::MENU_CHINH) {
        veMenuChinh();
    } else if (trangThai == TrangThai::PLAY_SUBMENU) {
        vePlaySubmenu();
    } else if (trangThai == TrangThai::PVP_SETUP) {
        vePvpSetup();
    } else if (trangThai == TrangThai::PVAI_SETUP) {
        vePvaiSetup();
    } else if (trangThai == TrangThai::LOAD_MENU) {
        veLoadMenu();
    } else if (trangThai == TrangThai::SETTINGS_MENU) {
        veSettingsMenu();
    } else if (trangThai == TrangThai::KEYBINDING_MENU) {
        veKeybindingMenu();
    } else if (trangThai == TrangThai::INTRODUCTION_MENU) {
        veIntroMenu();
    } else if (trangThai == TrangThai::DANG_CHOI) {
        veBanCoTruyenThong();
        veHighlights();
        veCacQuanCo();
        veConTroBanPhim();
        veThanhBenPhai();

        if (banCo.getKetThuc()) {
            vePopupKetThuc();
        }
    }

    veToast();
    window.display();
}

void GameManager::veBanCoTruyenThong() {
    float boardW = (BanCo::getSOCOT() - 1) * kichThuocO + 60.0f;
    float boardH = (BanCo::getSOHANG() - 1) * kichThuocO + 60.0f;
    float bx = offsetX - 30.0f;
    float by = offsetY - 30.0f;

    sf::RectangleShape boardBg(sf::Vector2f(boardW, boardH));
    boardBg.setPosition(bx, by);
    boardBg.setFillColor(sf::Color(228, 192, 138));
    boardBg.setOutlineThickness(4.0f);
    boardBg.setOutlineColor(sf::Color(90, 50, 25));
    window.draw(boardBg);

    sf::Color lineColor(90, 50, 25);
    for (int r = 0; r < BanCo::getSOHANG(); ++r) {
        sf::Vertex line[] = {
            sf::Vertex(chuyenDoiToaDoBanCoThanhManHinh(r, 0), lineColor),
            sf::Vertex(chuyenDoiToaDoBanCoThanhManHinh(r, BanCo::getSOCOT() - 1), lineColor)
        };
        window.draw(line, 2, sf::Lines);
    }

    for (int c = 0; c < BanCo::getSOCOT(); ++c) {
        if (c == 0 || c == BanCo::getSOCOT() - 1) {
            sf::Vertex line[] = {
                sf::Vertex(chuyenDoiToaDoBanCoThanhManHinh(0, c), lineColor),
                sf::Vertex(chuyenDoiToaDoBanCoThanhManHinh(BanCo::getSOHANG() - 1, c), lineColor)
            };
            window.draw(line, 2, sf::Lines);
        } else {
            sf::Vertex lineTop[] = {
                sf::Vertex(chuyenDoiToaDoBanCoThanhManHinh(0, c), lineColor),
                sf::Vertex(chuyenDoiToaDoBanCoThanhManHinh(4, c), lineColor)
            };
            window.draw(lineTop, 2, sf::Lines);

            sf::Vertex lineBot[] = {
                sf::Vertex(chuyenDoiToaDoBanCoThanhManHinh(5, c), lineColor),
                sf::Vertex(chuyenDoiToaDoBanCoThanhManHinh(9, c), lineColor)
            };
            window.draw(lineBot, 2, sf::Lines);
        }
    }

    // Palaces diagonals
    sf::Vertex palace1[] = {
        sf::Vertex(chuyenDoiToaDoBanCoThanhManHinh(0, 3), lineColor),
        sf::Vertex(chuyenDoiToaDoBanCoThanhManHinh(2, 5), lineColor)
    };
    sf::Vertex palace2[] = {
        sf::Vertex(chuyenDoiToaDoBanCoThanhManHinh(0, 5), lineColor),
        sf::Vertex(chuyenDoiToaDoBanCoThanhManHinh(2, 3), lineColor)
    };
    sf::Vertex palace3[] = {
        sf::Vertex(chuyenDoiToaDoBanCoThanhManHinh(7, 3), lineColor),
        sf::Vertex(chuyenDoiToaDoBanCoThanhManHinh(9, 5), lineColor)
    };
    sf::Vertex palace4[] = {
        sf::Vertex(chuyenDoiToaDoBanCoThanhManHinh(7, 5), lineColor),
        sf::Vertex(chuyenDoiToaDoBanCoThanhManHinh(9, 3), lineColor)
    };
    window.draw(palace1, 2, sf::Lines);
    window.draw(palace2, 2, sf::Lines);
    window.draw(palace3, 2, sf::Lines);
    window.draw(palace4, 2, sf::Lines);

    // River label
    sf::Text riverText;
    riverText.setFont(font);
    riverText.setCharacterSize(22);
    riverText.setStyle(sf::Text::Bold);
    riverText.setString("SO HA               HAN GIOI");
    riverText.setFillColor(sf::Color(140, 75, 35, 180));
    sf::FloatRect rtb = riverText.getLocalBounds();
    riverText.setOrigin(rtb.left + rtb.width / 2.0f, rtb.top + rtb.height / 2.0f);
    riverText.setPosition(offsetX + 4 * kichThuocO, offsetY + 4.5f * kichThuocO);
    window.draw(riverText);
}

void GameManager::veHighlights() {
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

    if (quanDangChon) {
        sf::Vector2f pos = chuyenDoiToaDoBanCoThanhManHinh(quanDangChon->getHang(), quanDangChon->getCot());
        sf::CircleShape selCircle(30.0f);
        selCircle.setOrigin(30.0f, 30.0f);
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
                    sf::CircleShape capCircle(26.0f);
                    capCircle.setOrigin(26.0f, 26.0f);
                    capCircle.setPosition(destPos);
                    capCircle.setFillColor(sf::Color(230, 40, 40, 120));
                    capCircle.setOutlineThickness(3.0f);
                    capCircle.setOutlineColor(sf::Color(250, 80, 80));
                    window.draw(capCircle);
                } else {
                    sf::CircleShape dot(9.0f);
                    dot.setOrigin(9.0f, 9.0f);
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
    float r = 26.0f;
    for (const auto& q : banCo.getCacQuan()) {
        if (!q || q->getDaBiAn()) continue;
        sf::Vector2f pos = chuyenDoiToaDoBanCoThanhManHinh(q->getHang(), q->getCot());

        sf::CircleShape outer(r + 3.0f);
        outer.setOrigin(r + 3.0f, r + 3.0f);
        outer.setPosition(pos);
        outer.setFillColor((q->getMau() == Mau::DO) ? sf::Color(140, 30, 20) : sf::Color(30, 40, 50));
        window.draw(outer);

        sf::CircleShape piece(r);
        piece.setOrigin(r, r);
        piece.setPosition(pos);
        piece.setFillColor(sf::Color(250, 238, 215));
        piece.setOutlineThickness(2.0f);
        piece.setOutlineColor((q->getMau() == Mau::DO) ? sf::Color(180, 40, 30) : sf::Color(40, 50, 65));
        window.draw(piece);

        sf::CircleShape inner(r - 4.0f);
        inner.setOrigin(r - 4.0f, r - 4.0f);
        inner.setPosition(pos);
        inner.setFillColor(sf::Color::Transparent);
        inner.setOutlineThickness(1.2f);
        inner.setOutlineColor((q->getMau() == Mau::DO) ? sf::Color(200, 50, 40, 180) : sf::Color(60, 75, 95, 180));
        window.draw(inner);

        sf::Text t;
        t.setFont(font);
        t.setString(q->layTen());
        t.setCharacterSize(20);
        t.setStyle(sf::Text::Bold);
        t.setFillColor((q->getMau() == Mau::DO) ? sf::Color(190, 25, 20) : sf::Color(20, 25, 35));

        sf::FloatRect tb = t.getLocalBounds();
        t.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
        t.setPosition(pos);
        window.draw(t);
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

void GameManager::veThanhBenPhai() {
    float px = 670.0f;
    float py = 30.0f;
    float pw = 490.0f;
    float ph = 730.0f;

    sf::RectangleShape panel(sf::Vector2f(pw, ph));
    panel.setPosition(px, py);
    panel.setFillColor(sf::Color(22, 28, 40));
    panel.setOutlineThickness(2.0f);
    panel.setOutlineColor(sf::Color(55, 68, 92));
    window.draw(panel);

    // Red Player Profile Box
    float boxW = 225.0f;
    float boxH = 90.0f;
    sf::RectangleShape redBox(sf::Vector2f(boxW, boxH));
    redBox.setPosition(px + 15.0f, py + 15.0f);
    redBox.setFillColor(sf::Color(38, 25, 25));
    redBox.setOutlineThickness((banCo.getLuotChoi() == Mau::DO) ? 2.5f : 1.0f);
    redBox.setOutlineColor((banCo.getLuotChoi() == Mau::DO) ? sf::Color(240, 80, 80) : sf::Color(80, 50, 50));
    window.draw(redBox);

    sf::Text rName;
    rName.setFont(font);
    rName.setCharacterSize(17);
    rName.setStyle(sf::Text::Bold);
    rName.setString(tenNguoiChoiDo);
    rName.setFillColor(sf::Color(255, 120, 120));
    rName.setPosition(px + 25.0f, py + 22.0f);
    window.draw(rName);

    sf::Text rChar;
    rChar.setFont(font);
    rChar.setCharacterSize(14);
    rChar.setString("[" + Loc::getCharName(nhanVatDo) + "]");
    rChar.setFillColor(sf::Color(200, 180, 180));
    rChar.setPosition(px + 25.0f, py + 46.0f);
    window.draw(rChar);

    auto formatTime = [](float t) {
        if (t <= 0.0f) return std::string("--:--");
        int m = static_cast<int>(t / 60.0f);
        int s = static_cast<int>(t) % 60;
        char buf[16];
        snprintf(buf, sizeof(buf), "%02d:%02d", m, s);
        return std::string(buf);
    };

    sf::Text rTime;
    rTime.setFont(font);
    rTime.setCharacterSize(16);
    rTime.setStyle(sf::Text::Bold);
    rTime.setString(Loc::get(LocKey::TIME_RED) + formatTime(thoiGianConDo));
    rTime.setFillColor((banCo.getLuotChoi() == Mau::DO) ? sf::Color(255, 200, 100) : sf::Color(160, 140, 140));
    rTime.setPosition(px + 25.0f, py + 70.0f);
    window.draw(rTime);

    // Black Player Profile Box
    sf::RectangleShape blackBox(sf::Vector2f(boxW, boxH));
    blackBox.setPosition(px + 250.0f, py + 15.0f);
    blackBox.setFillColor(sf::Color(20, 28, 42));
    blackBox.setOutlineThickness((banCo.getLuotChoi() == Mau::DEN) ? 2.5f : 1.0f);
    blackBox.setOutlineColor((banCo.getLuotChoi() == Mau::DEN) ? sf::Color(100, 180, 255) : sf::Color(50, 70, 95));
    window.draw(blackBox);

    sf::Text bName;
    bName.setFont(font);
    bName.setCharacterSize(17);
    bName.setStyle(sf::Text::Bold);
    bName.setString(tenNguoiChoiDen);
    bName.setFillColor(sf::Color(140, 200, 255));
    bName.setPosition(px + 260.0f, py + 22.0f);
    window.draw(bName);

    sf::Text bChar;
    bChar.setFont(font);
    bChar.setCharacterSize(14);
    bChar.setString("[" + Loc::getCharName(nhanVatDen) + "]");
    bChar.setFillColor(sf::Color(170, 190, 210));
    bChar.setPosition(px + 260.0f, py + 46.0f);
    window.draw(bChar);

    sf::Text bTime;
    bTime.setFont(font);
    bTime.setCharacterSize(16);
    bTime.setStyle(sf::Text::Bold);
    bTime.setString(Loc::get(LocKey::TIME_BLACK) + formatTime(thoiGianConDen));
    bTime.setFillColor((banCo.getLuotChoi() == Mau::DEN) ? sf::Color(255, 200, 100) : sf::Color(140, 155, 170));
    bTime.setPosition(px + 260.0f, py + 70.0f);
    window.draw(bTime);

    // Status / Check alert banner
    float statusY = py + 115.0f;
    sf::RectangleShape turnBox(sf::Vector2f(pw - 30.0f, 44.0f));
    turnBox.setPosition(px + 15.0f, statusY);
    bool checkAlert = banCo.kiemTraChieuTuong();
    turnBox.setFillColor(checkAlert ? sf::Color(130, 20, 20) : sf::Color(28, 36, 52));
    turnBox.setOutlineThickness(1.5f);
    turnBox.setOutlineColor(checkAlert ? sf::Color(255, 70, 70) : sf::Color(70, 90, 125));
    window.draw(turnBox);

    sf::Text turnText;
    turnText.setFont(font);
    turnText.setCharacterSize(18);
    turnText.setStyle(sf::Text::Bold);

    if (checkAlert) {
        turnText.setString(Loc::get(LocKey::CHECK_ALERT));
        turnText.setFillColor(sf::Color(255, 230, 120));
    } else {
        turnText.setString((banCo.getLuotChoi() == Mau::DO) ? Loc::get(LocKey::TURN_RED) : Loc::get(LocKey::TURN_BLACK));
        turnText.setFillColor((banCo.getLuotChoi() == Mau::DO) ? sf::Color(255, 140, 140) : sf::Color(150, 210, 255));
    }

    sf::FloatRect ttb = turnText.getLocalBounds();
    turnText.setOrigin(ttb.left + ttb.width / 2.0f, ttb.top + ttb.height / 2.0f);
    turnText.setPosition(px + pw / 2.0f, statusY + 22.0f);
    window.draw(turnText);

    // Key guide box
    float keyY = statusY + 54.0f;
    sf::RectangleShape keyBox(sf::Vector2f(pw - 30.0f, 52.0f));
    keyBox.setPosition(px + 15.0f, keyY);
    keyBox.setFillColor(sf::Color(18, 24, 34));
    keyBox.setOutlineThickness(1.0f);
    keyBox.setOutlineColor(sf::Color(55, 75, 105));
    window.draw(keyBox);

    sf::Text keyText;
    keyText.setFont(font);
    keyText.setCharacterSize(13);
    std::string keyInfo = 
        "[ " + KeyConfig::getKeyName(keyConfig.keyUp) + "," + 
        KeyConfig::getKeyName(keyConfig.keyLeft) + "," + 
        KeyConfig::getKeyName(keyConfig.keyDown) + "," + 
        KeyConfig::getKeyName(keyConfig.keyRight) + " ] Move  |  [ " + 
        KeyConfig::getKeyName(keyConfig.keySelect) + " ] Select  |  [ " + 
        KeyConfig::getKeyName(keyConfig.keyDeselect) + " ] Cancel\n" +
        "Preset: " + keyConfig.getPresetName() + "  |  Mouse click supported";
    keyText.setString(keyInfo);
    keyText.setFillColor(sf::Color(240, 220, 150));
    keyText.setPosition(px + 22.0f, keyY + 8.0f);
    window.draw(keyText);

    // Move history
    float histY = keyY + 62.0f;
    sf::Text histTitle;
    histTitle.setFont(font);
    histTitle.setCharacterSize(16);
    histTitle.setStyle(sf::Text::Bold);
    histTitle.setString(Loc::get(LocKey::MOVE_HISTORY));
    histTitle.setFillColor(sf::Color(200, 215, 235));
    histTitle.setPosition(px + 18.0f, histY);
    window.draw(histTitle);

    sf::RectangleShape histBox(sf::Vector2f(pw - 30.0f, 100.0f));
    histBox.setPosition(px + 15.0f, histY + 24.0f);
    histBox.setFillColor(sf::Color(18, 22, 32));
    histBox.setOutlineThickness(1.0f);
    histBox.setOutlineColor(sf::Color(50, 60, 80));
    window.draw(histBox);

    const auto& lichSu = banCo.getLichSuNuocDi();
    size_t count = lichSu.size();
    size_t start = (count > 4) ? (count - 4) : 0;
    float textY = histY + 30.0f;

    if (lichSu.empty()) {
        sf::Text emptyT;
        emptyT.setFont(font);
        emptyT.setCharacterSize(14);
        emptyT.setString(Loc::get(LocKey::NO_MOVES_YET));
        emptyT.setFillColor(sf::Color(120, 130, 145));
        emptyT.setPosition(px + 30.0f, textY + 28.0f);
        window.draw(emptyT);
    } else {
        for (size_t i = start; i < count; ++i) {
            sf::Text ndText;
            ndText.setFont(font);
            ndText.setCharacterSize(14);
            std::string line = std::to_string(i + 1) + ". " + banCo.layMoTaNuocDi(lichSu[i]);
            ndText.setString(line);
            ndText.setFillColor((lichSu[i].mauQuan == Mau::DO) ? sf::Color(240, 120, 120) : sf::Color(170, 200, 230));
            ndText.setPosition(px + 26.0f, textY);
            window.draw(ndText);
            textY += 21.0f;
        }
    }

    // In-game buttons
    for (auto& btn : inGameButtons) {
        btn->draw(window);
    }
}

void GameManager::vePopupKetThuc() {
    sf::RectangleShape overlay(sf::Vector2f(window.getSize().x, window.getSize().y));
    overlay.setFillColor(sf::Color(0, 0, 0, 175));
    window.draw(overlay);

    float dw = 520.0f;
    float dh = 300.0f;
    float dx = (window.getSize().x - dw) / 2.0f;
    float dy = (window.getSize().y - dh) / 2.0f;

    sf::RectangleShape dialog(sf::Vector2f(dw, dh));
    dialog.setPosition(dx, dy);
    dialog.setFillColor(sf::Color(28, 34, 48));
    dialog.setOutlineThickness(3.0f);
    dialog.setOutlineColor(sf::Color(240, 190, 70));
    window.draw(dialog);

    sf::Text tTitle;
    tTitle.setFont(font);
    tTitle.setCharacterSize(34);
    tTitle.setStyle(sf::Text::Bold);
    tTitle.setString(Loc::get(LocKey::GAME_OVER));
    tTitle.setFillColor(sf::Color(240, 190, 70));
    sf::FloatRect tb = tTitle.getLocalBounds();
    tTitle.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
    tTitle.setPosition(window.getSize().x / 2.0f, dy + 50.0f);
    window.draw(tTitle);

    sf::Text tResult;
    tResult.setFont(font);
    tResult.setCharacterSize(26);
    tResult.setStyle(sf::Text::Bold);

    if (banCo.getNguoiThang() == Mau::DO) {
        tResult.setString(tenNguoiChoiDo + " (RED) WINS!");
        tResult.setFillColor(sf::Color(240, 80, 80));
    } else if (banCo.getNguoiThang() == Mau::DEN) {
        tResult.setString(tenNguoiChoiDen + " (BLACK) WINS!");
        tResult.setFillColor(sf::Color(100, 180, 255));
    } else {
        tResult.setString(Loc::get(LocKey::DRAW_MATCH));
        tResult.setFillColor(sf::Color(220, 220, 220));
    }

    tb = tResult.getLocalBounds();
    tResult.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
    tResult.setPosition(window.getSize().x / 2.0f, dy + 110.0f);
    window.draw(tResult);

    if (!lyDoKetThuc.empty()) {
        sf::Text tReason;
        tReason.setFont(font);
        tReason.setCharacterSize(18);
        tReason.setString(lyDoKetThuc);
        tReason.setFillColor(sf::Color(190, 200, 215));
        tb = tReason.getLocalBounds();
        tReason.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
        tReason.setPosition(window.getSize().x / 2.0f, dy + 155.0f);
        window.draw(tReason);
    }

    for (auto& btn : popupButtons) {
        btn->draw(window);
    }
}

void GameManager::veToast() {
    if (toastTimer <= 0.0f || toastMessage.empty()) return;

    sf::Text t;
    t.setFont(font);
    t.setCharacterSize(18);
    t.setString(toastMessage);
    t.setFillColor(sf::Color::White);

    sf::FloatRect tb = t.getLocalBounds();
    float tw = tb.width + 36.0f;
    float th = 42.0f;
    float tx = (window.getSize().x - tw) / 2.0f;
    float ty = window.getSize().y - 65.0f;

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

// ==========================================
// Screen Renderers
// ==========================================
void GameManager::veMenuChinh() {
    sf::Text title;
    title.setFont(font);
    title.setString(Loc::get(LocKey::APP_TITLE));
    title.setCharacterSize(64);
    title.setFillColor(sf::Color(240, 190, 70));
    title.setStyle(sf::Text::Bold);
    
    sf::FloatRect tb = title.getLocalBounds();
    title.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
    title.setPosition(window.getSize().x / 2.0f, 90.0f);
    window.draw(title);

    // Decorative golden bar
    sf::RectangleShape goldLine(sf::Vector2f(380.0f, 3.0f));
    goldLine.setPosition((window.getSize().x - 380.0f) / 2.0f, 130.0f);
    goldLine.setFillColor(sf::Color(240, 190, 70));
    window.draw(goldLine);

    sf::Text sub;
    sub.setFont(font);
    sub.setString(Loc::get(LocKey::APP_SUBTITLE));
    sub.setCharacterSize(18);
    sub.setFillColor(sf::Color(165, 185, 210));
    tb = sub.getLocalBounds();
    sub.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
    sub.setPosition(window.getSize().x / 2.0f, 155.0f);
    window.draw(sub);
    
    menuChinh.draw(window);
}

void GameManager::vePlaySubmenu() {
    sf::Text title;
    title.setFont(font);
    title.setString(Loc::get(LocKey::PLAY_SUB_TITLE));
    title.setCharacterSize(52);
    title.setFillColor(sf::Color(240, 190, 70));
    title.setStyle(sf::Text::Bold);
    
    sf::FloatRect tb = title.getLocalBounds();
    title.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
    title.setPosition(window.getSize().x / 2.0f, 110.0f);
    window.draw(title);

    sf::RectangleShape goldLine(sf::Vector2f(320.0f, 3.0f));
    goldLine.setPosition((window.getSize().x - 320.0f) / 2.0f, 150.0f);
    goldLine.setFillColor(sf::Color(240, 190, 70));
    window.draw(goldLine);

    menuPlaySub.draw(window);
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
    title.setPosition(window.getSize().x / 2.0f, 60.0f);
    window.draw(title);

    // Left Column: Player 1 Card
    sf::RectangleShape p1Card(sf::Vector2f(440.0f, 250.0f));
    p1Card.setPosition(100.0f, 120.0f);
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
    p1Label.setPosition(150.0f, 138.0f);
    window.draw(p1Label);

    sf::Text p1NamePrompt;
    p1NamePrompt.setFont(font);
    p1NamePrompt.setCharacterSize(16);
    p1NamePrompt.setString(Loc::get(LocKey::ENTER_NAME_P1));
    p1NamePrompt.setFillColor(sf::Color(180, 195, 215));
    p1NamePrompt.setPosition(150.0f, 180.0f);
    window.draw(p1NamePrompt);

    inputP1Name->draw(window);

    sf::Text p1CharPrompt;
    p1CharPrompt.setFont(font);
    p1CharPrompt.setCharacterSize(16);
    p1CharPrompt.setString(Loc::get(LocKey::CHOOSE_CHAR));
    p1CharPrompt.setFillColor(sf::Color(180, 195, 215));
    p1CharPrompt.setPosition(150.0f, 275.0f);
    window.draw(p1CharPrompt);

    // Right Column: Player 2 Card
    sf::RectangleShape p2Card(sf::Vector2f(440.0f, 250.0f));
    p2Card.setPosition(660.0f, 120.0f);
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
    p2Label.setPosition(730.0f, 138.0f);
    window.draw(p2Label);

    sf::Text p2NamePrompt;
    p2NamePrompt.setFont(font);
    p2NamePrompt.setCharacterSize(16);
    p2NamePrompt.setString(Loc::get(LocKey::ENTER_NAME_P2));
    p2NamePrompt.setFillColor(sf::Color(180, 195, 215));
    p2NamePrompt.setPosition(730.0f, 180.0f);
    window.draw(p2NamePrompt);

    inputP2Name->draw(window);

    sf::Text p2CharPrompt;
    p2CharPrompt.setFont(font);
    p2CharPrompt.setCharacterSize(16);
    p2CharPrompt.setString(Loc::get(LocKey::CHOOSE_CHAR));
    p2CharPrompt.setFillColor(sf::Color(180, 195, 215));
    p2CharPrompt.setPosition(730.0f, 275.0f);
    window.draw(p2CharPrompt);

    // Center Match Controls Card (Timer + Random Roll)
    sf::RectangleShape rollCard(sf::Vector2f(600.0f, 220.0f));
    rollCard.setPosition(300.0f, 395.0f);
    rollCard.setFillColor(sf::Color(22, 28, 40));
    rollCard.setOutlineThickness(1.5f);
    rollCard.setOutlineColor(sf::Color(65, 80, 110));
    window.draw(rollCard);

    sf::Text bannerText;
    bannerText.setFont(font);
    bannerText.setCharacterSize(17);
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
    bannerText.setPosition(window.getSize().x / 2.0f, 545.0f);
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
    title.setPosition(window.getSize().x / 2.0f, 60.0f);
    window.draw(title);

    // Left Column: Player
    sf::RectangleShape pCard(sf::Vector2f(440.0f, 240.0f));
    pCard.setPosition(100.0f, 120.0f);
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
    pLabel.setPosition(150.0f, 138.0f);
    window.draw(pLabel);

    sf::Text pNamePrompt;
    pNamePrompt.setFont(font);
    pNamePrompt.setCharacterSize(16);
    pNamePrompt.setString(Loc::get(LocKey::ENTER_NAME_PLAYER));
    pNamePrompt.setFillColor(sf::Color(180, 195, 215));
    pNamePrompt.setPosition(150.0f, 180.0f);
    window.draw(pNamePrompt);

    inputAiPlayerName->draw(window);

    sf::Text pCharPrompt;
    pCharPrompt.setFont(font);
    pCharPrompt.setCharacterSize(16);
    pCharPrompt.setString(Loc::get(LocKey::CHOOSE_CHAR));
    pCharPrompt.setFillColor(sf::Color(180, 195, 215));
    pCharPrompt.setPosition(150.0f, 275.0f);
    window.draw(pCharPrompt);

    // Right Column: AI Engine
    sf::RectangleShape aiCard(sf::Vector2f(440.0f, 240.0f));
    aiCard.setPosition(660.0f, 120.0f);
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
    aiLabel.setPosition(730.0f, 138.0f);
    window.draw(aiLabel);

    sf::Text aiDiffPrompt;
    aiDiffPrompt.setFont(font);
    aiDiffPrompt.setCharacterSize(16);
    aiDiffPrompt.setString((Loc::getLanguage() == Language::TIENG_VIET) ? "Chon cap do thu thach:" : "Select challenge level:");
    aiDiffPrompt.setFillColor(sf::Color(180, 195, 215));
    aiDiffPrompt.setPosition(730.0f, 180.0f);
    window.draw(aiDiffPrompt);

    sf::Text aiDesc;
    aiDesc.setFont(font);
    aiDesc.setCharacterSize(14);
    std::string aiDescStr = (aiDifficulty == DoKho::DE) 
        ? ((Loc::getLanguage() == Language::TIENG_VIET) ? "Cap do De: Nuoc di co ban, thich hop luyen tap." : "Easy mode: Basic play, great for training.")
        : ((aiDifficulty == DoKho::TRUNG_BINH) 
            ? ((Loc::getLanguage() == Language::TIENG_VIET) ? "Cap do Vua: Minimax 2 lop + Danh gia the tran." : "Medium mode: Minimax 2-ply + Position evaluation.")
            : ((Loc::getLanguage() == Language::TIENG_VIET) ? "Cap do Kho: Alpha-Beta 4 lop + Sap xep nuoc di." : "Hard mode: Alpha-Beta 4-ply + Move ordering."));
    aiDesc.setString(aiDescStr);
    aiDesc.setFillColor(sf::Color(160, 180, 205));
    aiDesc.setPosition(730.0f, 280.0f);
    window.draw(aiDesc);

    // Center Match Controls Card (Timer + Who goes first + Random Roll)
    sf::RectangleShape rollCard(sf::Vector2f(600.0f, 240.0f));
    rollCard.setPosition(300.0f, 380.0f);
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
    bannerText.setPosition(window.getSize().x / 2.0f, 575.0f);
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
    title.setPosition(window.getSize().x / 2.0f, 85.0f);
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
    sf::Text title;
    title.setFont(font);
    title.setString(Loc::get(LocKey::SETTINGS_TITLE));
    title.setCharacterSize(48);
    title.setFillColor(sf::Color(240, 190, 70));
    title.setStyle(sf::Text::Bold);
    
    sf::FloatRect tb = title.getLocalBounds();
    title.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
    title.setPosition(window.getSize().x / 2.0f, 90.0f);
    window.draw(title);

    sf::RectangleShape goldLine(sf::Vector2f(280.0f, 3.0f));
    goldLine.setPosition((window.getSize().x - 280.0f) / 2.0f, 135.0f);
    goldLine.setFillColor(sf::Color(240, 190, 70));
    window.draw(goldLine);

    menuSettings.draw(window);
}

void GameManager::veKeybindingMenu() {
    sf::Text title;
    title.setFont(font);
    title.setString(Loc::get(LocKey::KEYBINDING_TITLE));
    title.setCharacterSize(44);
    title.setFillColor(sf::Color(240, 190, 70));
    title.setStyle(sf::Text::Bold);
    
    sf::FloatRect tb = title.getLocalBounds();
    title.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
    title.setPosition(window.getSize().x / 2.0f, 65.0f);
    window.draw(title);

    sf::Text sub;
    sub.setFont(font);
    sub.setString(Loc::get(LocKey::KEYBINDING_SUBTITLE));
    sub.setCharacterSize(17);
    sub.setFillColor(sf::Color(170, 190, 215));
    tb = sub.getLocalBounds();
    sub.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
    sub.setPosition(window.getSize().x / 2.0f, 115.0f);
    window.draw(sub);

    // Box container around key actions
    sf::RectangleShape box(sf::Vector2f(560.0f, 370.0f));
    box.setPosition((window.getSize().x - 560.0f) / 2.0f, 145.0f);
    box.setFillColor(sf::Color(22, 28, 40));
    box.setOutlineThickness(1.5f);
    box.setOutlineColor(sf::Color(60, 80, 115));
    window.draw(box);

    if (rebindingAction != KeyAction::COUNT) {
        sf::Text prompt;
        prompt.setFont(font);
        prompt.setCharacterSize(16);
        prompt.setString((Loc::getLanguage() == Language::TIENG_VIET)
            ? ">>> Dang cho ban nhan phim moi (Nhan Esc de huy) <<<"
            : ">>> Listening for new keypress (Press Esc to cancel) <<<");
        prompt.setFillColor(sf::Color(255, 220, 80));
        tb = prompt.getLocalBounds();
        prompt.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
        prompt.setPosition(window.getSize().x / 2.0f, 532.0f);
        window.draw(prompt);
    }

    menuKeybinding.draw(window);
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
    title.setPosition(window.getSize().x / 2.0f, 65.0f);
    window.draw(title);

    sf::RectangleShape box(sf::Vector2f(960.0f, 545.0f));
    box.setPosition((window.getSize().x - 960.0f) / 2.0f, 120.0f);
    box.setFillColor(sf::Color(24, 30, 44));
    box.setOutlineThickness(2.0f);
    box.setOutlineColor(sf::Color(65, 80, 115));
    window.draw(box);

    std::string text = (Loc::getLanguage() == Language::TIENG_VIET)
        ? "- GIOI THIEU DO AN:\n"
          "  Do an Lap trinh Huong doi tuong (OOP) - Truong Dai hoc Khoa hoc Tu nhien (HCMUS)\n"
          "  Game Co Tuong hien thuc bang C++ va thu vien do hoa SFML.\n\n"
          "- THIET KE HUONG DOI TUONG (OOP):\n"
          "  * Lop co so truu tuong QuanCo voi phuong thuc thuan ao: kiemTraNuocDi(...)\n"
          "  * Cac lop con ke thua da hinh: Xe, Voi, Phao, Ma, Tuong, Si, Tot\n"
          "  * Tuan thu dung yeu cau: Ban co 10 hang 9 cot; 2 Xe & 2 Voi; WASD, Enter, Esc, Q.\n\n"
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
          "  Chinese Chess (Xiangqi) fully developed in C++ and SFML graphics library.\n\n"
          "- OOP ARCHITECTURE:\n"
          "  * Abstract Base Class: QuanCo with pure virtual kiemTraNuocDi(...) method.\n"
          "  * Polymorphic Derived Classes: Xe, Voi, Phao, Ma, Tuong, Si, Tot.\n"
          "  * Core milestone rules satisfied: 10 rows x 9 cols; 2 Chariots & 2 Elephants; WASD, Enter, Esc, Q.\n\n"
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
    content.setFillColor(sf::Color(225, 235, 245));
    content.setPosition((window.getSize().x - 960.0f) / 2.0f + 30.0f, 140.0f);
    window.draw(content);

    menuIntro.draw(window);
}
