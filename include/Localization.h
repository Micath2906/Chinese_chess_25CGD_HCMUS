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

    // Match Setup
    SETUP_PVP_TITLE,
    SETUP_PVAI_TITLE,
    P1_LABEL,
    P2_LABEL,
    PLAYER_LABEL,
    AI_LABEL,
    ENTER_NAME_P1,
    ENTER_NAME_P2,
    ENTER_NAME_PLAYER,
    CHOOSE_CHAR,
    MATCH_TIMER_LABEL,
    TIME_5M,
    TIME_10M,
    TIME_15M,
    TIME_UNLIMITED,
    TOSS_COIN,
    FIRST_MOVE_INFO,
    START_GAME,

    // AI options
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

    // Settings Menu Commercial Dashboard
    SETTINGS_TITLE,
    TAB_AUDIO,
    TAB_GRAPHICS,
    TAB_GAMEPLAY,
    TAB_CONTROLS,

    // Audio Settings
    SETTING_MASTER_VOL,
    SETTING_SFX_TOGGLE,
    SETTING_SFX_VOL,
    SETTING_BGM_TOGGLE,
    SETTING_BGM_VOL,
    SETTING_TEST_AUDIO,

    // Graphics Settings
    SETTING_BOARD_THEME,
    THEME_WOOD,
    THEME_JADE,
    THEME_DARK,
    THEME_BAMBOO,
    SETTING_PIECE_STYLE,
    PIECE_VIETNAMESE,
    PIECE_SHORT,
    PIECE_INTL,
    SETTING_MOVE_HINTS,
    SETTING_LAST_MOVE,
    SETTING_COORDINATES,
    SETTING_WINDOW_MODE,
    MODE_WINDOWED,
    MODE_FULLSCREEN,

    // Gameplay Settings
    SETTING_LANG,
    SETTING_AI_THINK,
    AI_THINK_REALISTIC,
    AI_THINK_INSTANT,
    SETTING_CHECK_ALARM,

    // Common & Actions
    ON,
    OFF,
    BTN_APPLY_SAVE,
    BTN_RESTORE_DEFAULTS,

    // Keybindings Menu
    KEYBINDING_TITLE,
    KEYBINDING_SUBTITLE,
    KEY_ACTION_UP,
    KEY_ACTION_DOWN,
    KEY_ACTION_LEFT,
    KEY_ACTION_RIGHT,
    KEY_ACTION_SELECT,
    KEY_ACTION_DESELECT,
    KEY_ACTION_QUIT,
    KEY_PRESS_PROMPT,
    KEY_RESET_DEFAULT,
    PRESET_WASD_BTN,
    PRESET_ARROWS_BTN,

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
