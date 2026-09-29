#include "Localization.h"

Language Loc::currentLang = Language::TIENG_VIET;

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
            return vi ? "1. CHOI CO (PLAY)" : "1. PLAY";
        case LocKey::MENU_LOAD:
            return vi ? "2. TAI VAN CO (LOAD)" : "2. LOAD";
        case LocKey::MENU_SETTINGS:
            return vi ? "3. CAI DAT (SETTING)" : "3. SETTINGS";
        case LocKey::MENU_INTRO:
            return vi ? "4. GIOI THIEU (INTRODUCTION)" : "4. INTRODUCTION";
        case LocKey::MENU_EXIT:
            return vi ? "5. THOAT (EXIT)" : "5. EXIT";

        // Play Submenu
        case LocKey::PLAY_SUB_TITLE:
            return vi ? "CHON CHE DO CHOI" : "SELECT GAME MODE";
        case LocKey::PLAY_PVP:
            return vi ? "HAI NGUOI CHOI (Player vs Player)" : "PLAYER VS PLAYER";
        case LocKey::PLAY_PVAI:
            return vi ? "DAU VOI MAY (Player vs AI)" : "PLAYER VS AI";
        case LocKey::PLAY_DEMO:
            return vi ? "CHE DO DEMO (2 Xe & 2 Voi)" : "DEMO MODE (2 Chariots & 2 Elephants)";
        case LocKey::BACK:
            return vi ? "QUAY LAI (BACK)" : "BACK";

        // PvP Setup
        case LocKey::SETUP_PVP_TITLE:
            return vi ? "THIET LAP: HAI NGUOI CHOI" : "SETUP: PLAYER VS PLAYER";
        case LocKey::P1_LABEL:
            return vi ? "NGUOI CHOI 1" : "PLAYER 1";
        case LocKey::P2_LABEL:
            return vi ? "NGUOI CHOI 2" : "PLAYER 2";
        case LocKey::ENTER_NAME_P1:
            return vi ? "Nhap ten nguoi choi 1..." : "Enter Player 1 name...";
        case LocKey::ENTER_NAME_P2:
            return vi ? "Nhap ten nguoi choi 2..." : "Enter Player 2 name...";
        case LocKey::CHOOSE_CHAR:
            return vi ? "Nhan vat: " : "Character: ";
        case LocKey::TOSS_COIN:
            return vi ? "TUNG DONG XU (RANDOM AI DI TRUOC)" : "ROLL RANDOM WHO GOES FIRST";
        case LocKey::FIRST_MOVE_INFO:
            return vi ? "PHE DI TRUOC: QUAN DO" : "FIRST MOVE: RED PIECES";
        case LocKey::START_GAME:
            return vi ? "BAT DAU VAN DAU (START)" : "START GAME";

        // PvAI Setup
        case LocKey::SETUP_PVAI_TITLE:
            return vi ? "THIET LAP: DAU VOI MAY" : "SETUP: PLAYER VS AI";
        case LocKey::PLAYER_LABEL:
            return vi ? "NGUOI CHOI" : "PLAYER";
        case LocKey::AI_LABEL:
            return vi ? "MAY AI" : "AI ENGINE";
        case LocKey::ENTER_NAME_PLAYER:
            return vi ? "Nhap ten nguoi choi..." : "Enter player name...";
        case LocKey::AI_DIFFICULTY:
            return vi ? "DO KHO MAY (AI): " : "AI DIFFICULTY: ";
        case LocKey::DIFF_EASY:
            return vi ? "De (Easy)" : "Easy";
        case LocKey::DIFF_MED:
            return vi ? "Trung Binh (Medium)" : "Medium";
        case LocKey::DIFF_HARD:
            return vi ? "Kho (Hard)" : "Hard";
        case LocKey::WHO_GOES_FIRST:
            return vi ? "AI DI TRUOC: " : "WHO GOES FIRST: ";
        case LocKey::PLAYER_FIRST:
            return vi ? "Ban di truoc (Quan Do)" : "You first (Red pieces)";
        case LocKey::AI_FIRST:
            return vi ? "May di truoc (Quan Do)" : "AI first (Red pieces)";
        case LocKey::RANDOM_FIRST:
            return vi ? "Ngau nhien (Random)" : "Random";

        // Load Menu
        case LocKey::LOAD_TITLE:
            return vi ? "CAC VAN DAU DA LUU" : "SAVED GAMES";
        case LocKey::SAVED_GAMES:
            return vi ? "DANH SACH FILE LUU" : "SAVED GAMES LIST";
        case LocKey::SLOT_LABEL:
            return vi ? "O LUU SO " : "SAVE SLOT ";
        case LocKey::EMPTY_SLOT:
            return vi ? "(O luu trong - Chua co du lieu)" : "(Empty save slot)";
        case LocKey::LOAD_BUTTON:
            return vi ? "TAI VAN DAU NAY" : "LOAD THIS GAME";
        case LocKey::DELETE_BUTTON:
            return vi ? "XOA BAN LUU" : "DELETE SAVE";

        // Settings Menu
        case LocKey::SETTINGS_TITLE:
            return vi ? "CAI DAT TRO CHOI" : "GAME SETTINGS";
        case LocKey::SOUND_FX:
            return vi ? "AM THANH: " : "SOUND EFFECTS: ";
        case LocKey::SOUND_VOLUME:
            return vi ? "AM LUONG: " : "VOLUME: ";
        case LocKey::KEY_BINDINGS_MENU:
            return vi ? "CAI DAT PHIM DIEU KHIEN" : "SETTING KEY BINDINGS";
        case LocKey::LANGUAGE_LABEL:
            return vi ? "NGON NGU: TIENG VIET" : "LANGUAGE: ENGLISH";
        case LocKey::MATCH_TIMER:
            return vi ? "THOI GIAN VAN: " : "MATCH TIME: ";
        case LocKey::HINT_MOVES:
            return vi ? "GOI Y NUOC DI: " : "MOVE HINTS: ";
        case LocKey::ON:
            return vi ? "BAT" : "ON";
        case LocKey::OFF:
            return vi ? "TAT" : "OFF";
        case LocKey::UNLIMITED:
            return vi ? "VO HAN" : "UNLIMITED";
        case LocKey::MINUTES:
            return vi ? " PHUT" : " MINS";

        // Keybindings Menu
        case LocKey::KEYBINDING_TITLE:
            return vi ? "CAI DAT PHIM DIEU KHIEN" : "KEY BINDINGS CONFIGURATION";
        case LocKey::CURRENT_PRESET:
            return vi ? "BO PHIM HIEN TAI: " : "CURRENT KEY PRESET: ";
        case LocKey::PRESET_WASD_DESC:
            return vi ? "[W, A, S, D] Di chuyen  |  [Enter / Space] Chon/Di  |  [Esc] Bo chon  |  [Q] Thoat"
                      : "[W, A, S, D] Move cursor  |  [Enter / Space] Select/Move  |  [Esc] Deselect  |  [Q] Quit";
        case LocKey::PRESET_ARROWS_DESC:
            return vi ? "[Mui ten] Di chuyen  |  [Space / Enter] Chon/Di  |  [Esc] Bo chon  |  [Q] Thoat"
                      : "[Arrow keys] Move cursor  |  [Space / Enter] Select/Move  |  [Esc] Deselect  |  [Q] Quit";
        case LocKey::PRESET_IJKL_DESC:
            return vi ? "[I, J, K, L] Di chuyen  |  [Enter / Space] Chon/Di  |  [Esc] Bo chon  |  [Q] Thoat"
                      : "[I, J, K, L] Move cursor  |  [Enter / Space] Select/Move  |  [Esc] Deselect  |  [Q] Quit";
        case LocKey::SWITCH_PRESET:
            return vi ? "DOI BO PHIM (PRESET)" : "SWITCH KEY PRESET";

        // Introduction Menu
        case LocKey::INTRO_TITLE:
            return vi ? "GIOI THIEU & LUAT CHOI CO TUONG" : "INTRODUCTION & CHINESE CHESS RULES";

        // In Game UI
        case LocKey::TURN_RED:
            return vi ? "LUOT DI: QUAN DO" : "TURN: RED (FIRST)";
        case LocKey::TURN_BLACK:
            return vi ? "LUOT DI: QUAN DEN" : "TURN: BLACK";
        case LocKey::CHECK_ALERT:
            return vi ? "!!! CHIEU TUONG !!!" : "!!! CHECK !!!";
        case LocKey::MOVE_HISTORY:
            return vi ? "LICH SU NUOC DI:" : "MOVE HISTORY:";
        case LocKey::NO_MOVES_YET:
            return vi ? "(Chua co nuoc di nao)" : "(No moves yet)";
        case LocKey::TIME_RED:
            return vi ? "DO: " : "RED: ";
        case LocKey::TIME_BLACK:
            return vi ? "DEN: " : "BLACK: ";
        case LocKey::UNDO:
            return vi ? "HOAN TAC (Undo)" : "UNDO";
        case LocKey::REDO:
            return vi ? "DI TIEP (Redo)" : "REDO";
        case LocKey::NEW_GAME:
            return vi ? "VAN MOI" : "NEW GAME";
        case LocKey::DRAW_OFFER:
            return vi ? "XIN HOA" : "OFFER DRAW";
        case LocKey::SURRENDER:
            return vi ? "DAU HANG" : "RESIGN";
        case LocKey::SAVE_GAME:
            return vi ? "LUU GAME" : "SAVE GAME";
        case LocKey::LOAD_GAME:
            return vi ? "TAI GAME" : "LOAD GAME";
        case LocKey::BACK_MENU:
            return vi ? "VE MENU" : "MAIN MENU";

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
            return vi ? "CHOI LAI" : "PLAY AGAIN";
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
