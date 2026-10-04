#include "Localization.h"

// Default language is English
Language Loc::currentLang = Language::ENGLISH;

std::string Loc::get(LocKey key) {
    bool vi = (currentLang == Language::TIENG_VIET);

    switch (key) {
        // App Header
        case LocKey::APP_TITLE:
            return vi ? "CO TUONG" : "CHINESE CHESS";
        case LocKey::APP_SUBTITLE:
            return vi ? "DO AN MON OOP - HCMUS" : "OOP PROJECT - HCMUS";

        // Main Menu
        case LocKey::MENU_PLAY:
            return vi ? "CHOI CO" : "PLAY";
        case LocKey::MENU_LOAD:
            return vi ? "TAI VAN CO" : "LOAD";
        case LocKey::MENU_SETTINGS:
            return vi ? "CAI DAT" : "SETTINGS";
        case LocKey::MENU_INTRO:
            return vi ? "GIOI THIEU" : "INTRODUCTION";
        case LocKey::MENU_EXIT:
            return vi ? "THOAT" : "EXIT";

        // Play Submenu
        case LocKey::PLAY_SUB_TITLE:
            return vi ? "CHON CHE DO CHOI" : "SELECT GAME MODE";
        case LocKey::PLAY_PVP:
            return vi ? "Hai nguoi choi (PvP)" : "Player vs Player";
        case LocKey::PLAY_PVAI:
            return vi ? "Dau voi may (PvAI)" : "Player vs AI";
        case LocKey::PLAY_DEMO:
            return vi ? "Che do Demo (2 Xe & 2 Voi)" : "Demo Mode (2 Chariots & 2 Elephants)";
        case LocKey::BACK:
            return vi ? "Quay lai" : "Back";

        // Match Setup
        case LocKey::SETUP_PVP_TITLE:
            return vi ? "THIET LAP: HAI NGUOI CHOI" : "SETUP: PLAYER VS PLAYER";
        case LocKey::SETUP_PVAI_TITLE:
            return vi ? "THIET LAP: DAU VOI MAY" : "SETUP: PLAYER VS AI";
        case LocKey::P1_LABEL:
            return vi ? "NGUOI CHOI 1" : "PLAYER 1";
        case LocKey::P2_LABEL:
            return vi ? "NGUOI CHOI 2" : "PLAYER 2";
        case LocKey::PLAYER_LABEL:
            return vi ? "NGUOI CHOI" : "PLAYER";
        case LocKey::AI_LABEL:
            return vi ? "MAY AI" : "AI ENGINE";
        case LocKey::ENTER_NAME_P1:
            return vi ? "Ten nguoi choi 1:" : "Player 1 name:";
        case LocKey::ENTER_NAME_P2:
            return vi ? "Ten nguoi choi 2:" : "Player 2 name:";
        case LocKey::ENTER_NAME_PLAYER:
            return vi ? "Ten nguoi choi:" : "Player name:";
        case LocKey::CHOOSE_CHAR:
            return vi ? "Nhan vat: " : "Character: ";
        case LocKey::MATCH_TIMER_LABEL:
            return vi ? "Thoi gian moi ben: " : "Match Time: ";
        case LocKey::TIME_5M:
            return vi ? "5 Phut" : "5 Mins";
        case LocKey::TIME_10M:
            return vi ? "10 Phut" : "10 Mins";
        case LocKey::TIME_15M:
            return vi ? "15 Phut" : "15 Mins";
        case LocKey::TIME_UNLIMITED:
            return vi ? "Vo han" : "Unlimited";
        case LocKey::TOSS_COIN:
            return vi ? "Tung dong xu chon ai di truoc" : "Roll Random First Move";
        case LocKey::FIRST_MOVE_INFO:
            return vi ? "Phe di truoc: Quan Do" : "First move: Red side";
        case LocKey::START_GAME:
            return vi ? "BAT DAU VAN DAU" : "START GAME";

        // AI options
        case LocKey::AI_DIFFICULTY:
            return vi ? "Do kho may: " : "AI Difficulty: ";
        case LocKey::DIFF_EASY:
            return vi ? "De" : "Easy";
        case LocKey::DIFF_MED:
            return vi ? "Vua" : "Medium";
        case LocKey::DIFF_HARD:
            return vi ? "Kho" : "Hard";
        case LocKey::WHO_GOES_FIRST:
            return vi ? "Ai di truoc: " : "First Move: ";
        case LocKey::PLAYER_FIRST:
            return vi ? "Ban di truoc (Quan Do)" : "You first (Red)";
        case LocKey::AI_FIRST:
            return vi ? "May di truoc (Quan Do)" : "AI first (Red)";
        case LocKey::RANDOM_FIRST:
            return vi ? "Ngau nhien" : "Random";

        // Load Menu
        case LocKey::LOAD_TITLE:
            return vi ? "CAC VAN DAU DA LUU" : "SAVED GAMES";
        case LocKey::SAVED_GAMES:
            return vi ? "Danh sach file luu" : "Saved Games List";
        case LocKey::SLOT_LABEL:
            return vi ? "O luu " : "Save Slot ";
        case LocKey::EMPTY_SLOT:
            return vi ? "(O luu trong - Chua co du lieu)" : "(Empty save slot)";
        case LocKey::LOAD_BUTTON:
            return vi ? "Tai van dau" : "Load Game";
        case LocKey::DELETE_BUTTON:
            return vi ? "Xoa ban luu" : "Delete Save";

        // Settings Menu
        case LocKey::SETTINGS_TITLE:
            return vi ? "CAI DAT HE THONG" : "SYSTEM SETTINGS";
        case LocKey::TAB_AUDIO:
            return vi ? "AM THANH" : "AUDIO";
        case LocKey::TAB_GRAPHICS:
            return vi ? "DO HOA & BAN CO" : "GRAPHICS";
        case LocKey::TAB_GAMEPLAY:
            return vi ? "TRO CHOI" : "GAMEPLAY";
        case LocKey::TAB_CONTROLS:
            return vi ? "DIEU KHIEN" : "CONTROLS";

        // Audio Settings
        case LocKey::SETTING_MASTER_VOL:
            return vi ? "Am luong tong (Master): " : "Master Volume: ";
        case LocKey::SETTING_SFX_TOGGLE:
            return vi ? "Hieu ung am thanh (SFX): " : "Sound Effects (SFX): ";
        case LocKey::SETTING_SFX_VOL:
            return vi ? "Am luong hieu ung (SFX): " : "SFX Volume: ";
        case LocKey::SETTING_BGM_TOGGLE:
            return vi ? "Nhac nen co trang (BGM): " : "Background Music (BGM): ";
        case LocKey::SETTING_BGM_VOL:
            return vi ? "Am luong nhac nen (BGM): " : "BGM Volume: ";
        case LocKey::SETTING_TEST_AUDIO:
            return vi ? "Nghe thu am thanh" : "Test Sound";

        // Graphics Settings
        case LocKey::SETTING_BOARD_THEME:
            return vi ? "Giao dien ban co: " : "Board Theme: ";
        case LocKey::THEME_WOOD:
            return vi ? "Go Co Dien" : "Classic Wood";
        case LocKey::THEME_JADE:
            return vi ? "Ngoc Bich Hoang Gia" : "Imperial Jade";
        case LocKey::THEME_DARK:
            return vi ? "Huyen Thach (Dark Mode)" : "Midnight Obsidian";
        case LocKey::THEME_BAMBOO:
            return vi ? "Giay Truc Tram" : "Warm Bamboo";

        case LocKey::SETTING_PIECE_STYLE:
            return vi ? "Kieu ky hieu quan co: " : "Piece Style: ";
        case LocKey::PIECE_REALISTIC:
            return vi ? "Quan go truyen thong (Sprite)" : "Traditional Wood (Sprites)";
        case LocKey::PIECE_VIETNAMESE:
            return vi ? "Tieng Viet (Xe, Ma, Voi...)" : "Full Names (Xe, Ma, Voi...)";
        case LocKey::PIECE_SHORT:
            return vi ? "Ký hieu tat (X, M, V...)" : "Short Badges (X, M, V...)";
        case LocKey::PIECE_INTL:
            return vi ? "Quoc te (R, H, E...)" : "International (R, H, E...)";

        case LocKey::SETTING_MOVE_HINTS:
            return vi ? "Goi y nuoc di hop le: " : "Valid Move Hints: ";
        case LocKey::SETTING_LAST_MOVE:
            return vi ? "Danh dau nuoc vua di: " : "Last Move Highlight: ";
        case LocKey::SETTING_COORDINATES:
            return vi ? "Toa do mep ban co (1-9, A-J): " : "Board Coordinates (1-9, A-J): ";
        case LocKey::SETTING_WINDOW_MODE:
            return vi ? "Che do man hinh: " : "Display Mode: ";
        case LocKey::MODE_WINDOWED:
            return vi ? "Cua so (1200x800)" : "Windowed (1200x800)";
        case LocKey::MODE_FULLSCREEN:
            return vi ? "Toan man hinh" : "Fullscreen";

        // Gameplay Settings
        case LocKey::SETTING_LANG:
            return vi ? "Ngon ngu game: " : "Game Language: ";
        case LocKey::SETTING_AI_THINK:
            return vi ? "Mo phong AI suy nghi: " : "AI Thinking Delay: ";
        case LocKey::AI_THINK_REALISTIC:
            return vi ? "Chan thuc (450ms)" : "Realistic (450ms)";
        case LocKey::AI_THINK_INSTANT:
            return vi ? "Tuc thi (0ms)" : "Instant (0ms)";
        case LocKey::SETTING_CHECK_ALARM:
            return vi ? "Bao dong chieu tuong: " : "Check Warning Alarm: ";

        // Common & Actions
        case LocKey::ON:
            return vi ? "Bat" : "ON";
        case LocKey::OFF:
            return vi ? "Tat" : "OFF";
        case LocKey::BTN_APPLY_SAVE:
            return vi ? "Ap dung & Luu" : "Apply & Save";
        case LocKey::BTN_RESTORE_DEFAULTS:
            return vi ? "Khoi phuc mac dinh" : "Restore Defaults";

        // Keybindings Menu
        case LocKey::KEYBINDING_TITLE:
            return vi ? "CAI DAT PHIM DIEU KHIEN" : "CUSTOM KEY BINDINGS";
        case LocKey::KEYBINDING_SUBTITLE:
            return vi ? "Click vao hanh dong ben duoi de tuy y doi phim theo y muon:"
                      : "Click on any action below to customize its key binding:";
        case LocKey::KEY_ACTION_UP:
            return vi ? "Di chuyen Len" : "Move Up";
        case LocKey::KEY_ACTION_DOWN:
            return vi ? "Di chuyen Xuong" : "Move Down";
        case LocKey::KEY_ACTION_LEFT:
            return vi ? "Di chuyen Trai" : "Move Left";
        case LocKey::KEY_ACTION_RIGHT:
            return vi ? "Di chuyen Phai" : "Move Right";
        case LocKey::KEY_ACTION_SELECT:
            return vi ? "Chon / Ha co" : "Select / Move";
        case LocKey::KEY_ACTION_DESELECT:
            return vi ? "Huy chon / Menu" : "Deselect / Cancel";
        case LocKey::KEY_ACTION_QUIT:
            return vi ? "Thoat nhanh" : "Quick Quit";
        case LocKey::KEY_PRESS_PROMPT:
            return vi ? "[ Nhan phim bat ky... ]" : "[ Press any key... ]";
        case LocKey::KEY_RESET_DEFAULT:
            return vi ? "Dat lai mac dinh" : "Reset Defaults";
        case LocKey::PRESET_WASD_BTN:
            return "WASD Preset";
        case LocKey::PRESET_ARROWS_BTN:
            return vi ? "Bo phim Mui ten" : "Arrow Keys Preset";

        // Introduction Menu
        case LocKey::INTRO_TITLE:
            return vi ? "GIOI THIEU & LUAT CHOI CO TUONG" : "INTRODUCTION & RULES";

        // In Game UI
        case LocKey::TURN_RED:
            return vi ? "Luot di: Quan Do" : "Turn: Red";
        case LocKey::TURN_BLACK:
            return vi ? "Luot di: Quan Den" : "Turn: Black";
        case LocKey::CHECK_ALERT:
            return vi ? "!!! CHIEU TUONG !!!" : "!!! CHECK !!!";
        case LocKey::MOVE_HISTORY:
            return vi ? "Lich su nuoc di:" : "Move History:";
        case LocKey::NO_MOVES_YET:
            return vi ? "(Chua co nuoc di nao)" : "(No moves yet)";
        case LocKey::TIME_RED:
            return vi ? "Do: " : "Red: ";
        case LocKey::TIME_BLACK:
            return vi ? "Den: " : "Black: ";
        case LocKey::UNDO:
            return vi ? "Hoan tac" : "Undo";
        case LocKey::REDO:
            return vi ? "Di tiep" : "Redo";
        case LocKey::NEW_GAME:
            return vi ? "Van moi" : "New Game";
        case LocKey::DRAW_OFFER:
            return vi ? "Xin hoa" : "Offer Draw";
        case LocKey::SURRENDER:
            return vi ? "Dau hang" : "Resign";
        case LocKey::SAVE_GAME:
            return vi ? "Luu game" : "Save Game";
        case LocKey::LOAD_GAME:
            return vi ? "Tai game" : "Load Game";
        case LocKey::BACK_MENU:
            return vi ? "Ve Menu" : "Main Menu";

        // End Game Popup
        case LocKey::GAME_OVER:
            return vi ? "KET THUC VAN CO" : "GAME OVER";
        case LocKey::RED_WINS:
            return vi ? "QUAN DO CHIEN THANG!" : "RED WINS!";
        case LocKey::BLACK_WINS:
            return vi ? "QUAN DEN CHIEN THANG!" : "BLACK WINS!";
        case LocKey::DRAW_MATCH:
            return vi ? "HAI BEN HOA CO!" : "GAME DRAW!";
        case LocKey::PLAY_AGAIN:
            return vi ? "Choi lai" : "Play Again";
    }

    return "";
}

std::string Loc::getCharName(int index) {
    bool vi = (currentLang == Language::TIENG_VIET);
    switch (index % 5) {
        case 0: return vi ? "Tuong Quan" : "General";
        case 1: return vi ? "Quan Su" : "Strategist";
        case 2: return vi ? "Ky Vuong" : "Grandmaster";
        case 3: return vi ? "Hiep Khach" : "Knight";
        case 4: return vi ? "Thu Sinh" : "Scholar";
        default: return vi ? "Ky Thu" : "Player";
    }
}

std::string Loc::getCharDesc(int index) {
    bool vi = (currentLang == Language::TIENG_VIET);
    switch (index % 5) {
        case 0: return vi ? "Tien phong dung manh, phong cach cong pha" : "Brave vanguard, aggressive offensive style";
        case 1: return vi ? "Muu luoc sau xa, tinh toan tung nuoc co" : "Deep tactical thinker, meticulous positional planner";
        case 2: return vi ? "Ky nghe uyen tham, cong thu toan dien" : "Legendary master, perfectly balanced play";
        case 3: return vi ? "Linh hoat bien hoa, phan cong bat ngo" : "Agile and flexible, swift counter-attacks";
        case 4: return vi ? "Phong thai ung dung, can trong vung chac" : "Composed and wise, rock-solid defense";
        default: return "";
    }
}
