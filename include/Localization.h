#pragma once
#include <string>
#include <vector>

enum class Language {
    TIENG_VIET,
    ENGLISH
};

enum class LocKey {
    // App Header
    APP_TITLE,
    APP_SUBTITLE,

    // Main Menu
    MENU_PLAY,
    MENU_LOAD,
    MENU_SETTINGS,
    MENU_INTRO,
    MENU_EXIT,

    // Play Submenu
    PLAY_SUB_TITLE,
    PLAY_PVP,
    PLAY_PVAI,
    PLAY_DEMO,
    BACK,

    // PvP Setup
    SETUP_PVP_TITLE,
    P1_LABEL,
    P2_LABEL,
    ENTER_NAME_P1,
    ENTER_NAME_P2,
    CHOOSE_CHAR,
    TOSS_COIN,
    FIRST_MOVE_INFO,
    START_GAME,

    // PvAI Setup
    SETUP_PVAI_TITLE,
    PLAYER_LABEL,
    AI_LABEL,
    ENTER_NAME_PLAYER,
    AI_DIFFICULTY,
    DIFF_EASY,
    DIFF_MED,
    DIFF_HARD,
    WHO_GOES_FIRST,
    PLAYER_FIRST,
    AI_FIRST,
    RANDOM_FIRST,

    // Load Menu
    LOAD_TITLE,
    SAVED_GAMES,
    SLOT_LABEL,
    EMPTY_SLOT,
    LOAD_BUTTON,
    DELETE_BUTTON,

    // Settings Menu
    SETTINGS_TITLE,
    SOUND_FX,
    SOUND_VOLUME,
    KEY_BINDINGS_MENU,
    LANGUAGE_LABEL,
    MATCH_TIMER,
    HINT_MOVES,
    ON,
    OFF,
    UNLIMITED,
    MINUTES,

    // Keybindings Menu
    KEYBINDING_TITLE,
    CURRENT_PRESET,
    PRESET_WASD_DESC,
    PRESET_ARROWS_DESC,
    PRESET_IJKL_DESC,
    SWITCH_PRESET,

    // Introduction Menu
    INTRO_TITLE,

    // In Game UI
    TURN_RED,
    TURN_BLACK,
    CHECK_ALERT,
    MOVE_HISTORY,
    NO_MOVES_YET,
    TIME_RED,
    TIME_BLACK,
    UNDO,
    REDO,
    NEW_GAME,
    DRAW_OFFER,
    SURRENDER,
    SAVE_GAME,
    LOAD_GAME,
    BACK_MENU,

    // End Game Popup
    GAME_OVER,
    RED_WINS,
    BLACK_WINS,
    DRAW_MATCH,
    PLAY_AGAIN
};

class Loc {
private:
    static Language currentLang;

public:
    static void setLanguage(Language lang) { currentLang = lang; }
    static Language getLanguage() { return currentLang; }
    static void toggleLanguage() {
        currentLang = (currentLang == Language::TIENG_VIET) ? Language::ENGLISH : Language::TIENG_VIET;
    }

    static std::string get(LocKey key);
    static std::string getCharName(int index);
    static std::string getCharDesc(int index);
    static int getCharCount() { return 5; }
};
