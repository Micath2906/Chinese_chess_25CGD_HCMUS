#pragma once
#include <string_view>
#include <array>

/**
 * ============================================================================
 * Design Pattern: Flyweight Metadata / Centralized Asset Registry
 * ============================================================================
 * AssetPaths.h defines strongly-typed enumeration identifiers and maps each
 * asset to its single canonical relative path inside the assets/ directory.
 * No hardcoded file path strings should exist anywhere else in game logic.
 */

enum class TextureId {
    BACKGROUND_PAVILION,
    BOARD,
    ORIENTAL_SHEET,

    // Pieces - Red (Quân Đỏ)
    PIECE_R_TUONG,
    PIECE_R_SI,
    PIECE_R_VOI,
    PIECE_R_MA,
    PIECE_R_XE,
    PIECE_R_PHAO,
    PIECE_R_TOT,

    // Pieces - Black (Quân Đen)
    PIECE_B_TUONG,
    PIECE_B_SI,
    PIECE_B_VOI,
    PIECE_B_MA,
    PIECE_B_XE,
    PIECE_B_PHAO,
    PIECE_B_TOT,

    // TravelBook UI Sprites
    TB_BAR_01A,
    TB_BUTTON_VALUE_01A,
    TB_CURSOR_01C,
    TB_CURSOR_01D,
    TB_FILL_01A,
    TB_FRAME_01A,
    TB_FRAME_SELECT_01A,
    TB_FRAME_SELECT_01B,
    TB_GAMEPAD_A_01A,
    TB_GAMEPAD_B_01A,
    TB_GAMEPAD_L_01A,
    TB_GAMEPAD_LB_01A,
    TB_GAMEPAD_RB_01A,
    TB_HANDLE_03A,
    TB_ICON_ARROW_01A,
    TB_ICON_CROSS_01A,
    TB_ICON_GEAR_01A,
    TB_ICON_PLAY_01A,
    TB_ICON_RESTART_01A,
    TB_ICON_STAR_01A,
    TB_LINE_01A,
    TB_MARKER_01A,
    TB_SELECT_01A,
    TB_SLOT_01A,
    TB_TOGGLE_03A,
    TB_TOGGLE_03B,

    COUNT
};

enum class FontId {
    NOM_NA_TONG,
    PIXELOID_SANS,
    PIXELOID_SANS_BOLD,
    VIAODA_LIBRE,
    COUNT
};

enum class SoundId {
    MOVE,
    CAPTURE,
    CHECK,
    WIN,
    SELECT,
    COUNT
};

enum class MusicId {
    BGM_MAIN,
    COUNT
};

enum class TextureLoadPolicy {
    DEFAULT,
    SMOOTH,
    PIXEL_ART,
    MASK_WHITE_SMOOTH
};

struct TextureAssetDef {
    TextureId id;
    const char* relativePath;
    TextureLoadPolicy policy;
};

struct FontAssetDef {
    FontId id;
    const char* relativePath;
};

struct SoundAssetDef {
    SoundId id;
    const char* relativePath;
};

struct MusicAssetDef {
    MusicId id;
    const char* relativePath;
};

namespace AssetRegistry {

constexpr std::array<TextureAssetDef, static_cast<size_t>(TextureId::COUNT)> TEXTURES = {{
    { TextureId::BACKGROUND_PAVILION, "textures/backgrounds/xiangqi_background_pavilion.png", TextureLoadPolicy::SMOOTH },
    { TextureId::BOARD,               "textures/board/board.png",                              TextureLoadPolicy::SMOOTH },
    { TextureId::ORIENTAL_SHEET,      "textures/ui/xiangqi_oriental_sheet.png",                TextureLoadPolicy::PIXEL_ART },

    // Red pieces
    { TextureId::PIECE_R_TUONG,       "textures/pieces/r_tuong.png",                           TextureLoadPolicy::MASK_WHITE_SMOOTH },
    { TextureId::PIECE_R_SI,          "textures/pieces/r_si.png",                              TextureLoadPolicy::MASK_WHITE_SMOOTH },
    { TextureId::PIECE_R_VOI,         "textures/pieces/r_voi.png",                             TextureLoadPolicy::MASK_WHITE_SMOOTH },
    { TextureId::PIECE_R_MA,          "textures/pieces/r_ma.png",                              TextureLoadPolicy::MASK_WHITE_SMOOTH },
    { TextureId::PIECE_R_XE,          "textures/pieces/r_xe.png",                              TextureLoadPolicy::MASK_WHITE_SMOOTH },
    { TextureId::PIECE_R_PHAO,        "textures/pieces/r_phao.png",                            TextureLoadPolicy::MASK_WHITE_SMOOTH },
    { TextureId::PIECE_R_TOT,         "textures/pieces/r_tot.png",                             TextureLoadPolicy::MASK_WHITE_SMOOTH },

    // Black pieces
    { TextureId::PIECE_B_TUONG,       "textures/pieces/b_tuong.png",                           TextureLoadPolicy::MASK_WHITE_SMOOTH },
    { TextureId::PIECE_B_SI,          "textures/pieces/b_si.png",                              TextureLoadPolicy::MASK_WHITE_SMOOTH },
    { TextureId::PIECE_B_VOI,         "textures/pieces/b_voi.png",                             TextureLoadPolicy::MASK_WHITE_SMOOTH },
    { TextureId::PIECE_B_MA,          "textures/pieces/b_ma.png",                              TextureLoadPolicy::MASK_WHITE_SMOOTH },
    { TextureId::PIECE_B_XE,          "textures/pieces/b_xe.png",                              TextureLoadPolicy::MASK_WHITE_SMOOTH },
    { TextureId::PIECE_B_PHAO,        "textures/pieces/b_phao.png",                            TextureLoadPolicy::MASK_WHITE_SMOOTH },
    { TextureId::PIECE_B_TOT,         "textures/pieces/b_tot.png",                             TextureLoadPolicy::MASK_WHITE_SMOOTH },

    // TravelBook UI Sprites
    { TextureId::TB_BAR_01A,          "textures/ui/travelbook/UI_TravelBook_Bar01a.png",         TextureLoadPolicy::PIXEL_ART },
    { TextureId::TB_BUTTON_VALUE_01A, "textures/ui/travelbook/UI_TravelBook_ButtonValue01a.png", TextureLoadPolicy::PIXEL_ART },
    { TextureId::TB_CURSOR_01C,       "textures/ui/travelbook/UI_TravelBook_Cursor01c.png",      TextureLoadPolicy::PIXEL_ART },
    { TextureId::TB_CURSOR_01D,       "textures/ui/travelbook/UI_TravelBook_Cursor01d.png",      TextureLoadPolicy::PIXEL_ART },
    { TextureId::TB_FILL_01A,         "textures/ui/travelbook/UI_TravelBook_Fill01a.png",        TextureLoadPolicy::PIXEL_ART },
    { TextureId::TB_FRAME_01A,        "textures/ui/travelbook/UI_TravelBook_Frame01a.png",       TextureLoadPolicy::PIXEL_ART },
    { TextureId::TB_FRAME_SELECT_01A, "textures/ui/travelbook/UI_TravelBook_FrameSelect01a.png", TextureLoadPolicy::PIXEL_ART },
    { TextureId::TB_FRAME_SELECT_01B, "textures/ui/travelbook/UI_TravelBook_FrameSelect01b.png", TextureLoadPolicy::PIXEL_ART },
    { TextureId::TB_GAMEPAD_A_01A,    "textures/ui/travelbook/UI_TravelBook_GamepadA01a.png",   TextureLoadPolicy::PIXEL_ART },
    { TextureId::TB_GAMEPAD_B_01A,    "textures/ui/travelbook/UI_TravelBook_GamepadB01a.png",   TextureLoadPolicy::PIXEL_ART },
    { TextureId::TB_GAMEPAD_L_01A,    "textures/ui/travelbook/UI_TravelBook_GamepadL01a.png",   TextureLoadPolicy::PIXEL_ART },
    { TextureId::TB_GAMEPAD_LB_01A,   "textures/ui/travelbook/UI_TravelBook_GamepadLB01a.png",  TextureLoadPolicy::PIXEL_ART },
    { TextureId::TB_GAMEPAD_RB_01A,   "textures/ui/travelbook/UI_TravelBook_GamepadRB01a.png",  TextureLoadPolicy::PIXEL_ART },
    { TextureId::TB_HANDLE_03A,       "textures/ui/travelbook/UI_TravelBook_Handle03a.png",      TextureLoadPolicy::PIXEL_ART },
    { TextureId::TB_ICON_ARROW_01A,   "textures/ui/travelbook/UI_TravelBook_IconArrow01a.png",  TextureLoadPolicy::PIXEL_ART },
    { TextureId::TB_ICON_CROSS_01A,   "textures/ui/travelbook/UI_TravelBook_IconCross01a.png",  TextureLoadPolicy::PIXEL_ART },
    { TextureId::TB_ICON_GEAR_01A,    "textures/ui/travelbook/UI_TravelBook_IconGear01a.png",   TextureLoadPolicy::PIXEL_ART },
    { TextureId::TB_ICON_PLAY_01A,    "textures/ui/travelbook/UI_TravelBook_IconPlay01a.png",   TextureLoadPolicy::PIXEL_ART },
    { TextureId::TB_ICON_RESTART_01A, "textures/ui/travelbook/UI_TravelBook_IconRestart01a.png",TextureLoadPolicy::PIXEL_ART },
    { TextureId::TB_ICON_STAR_01A,    "textures/ui/travelbook/UI_TravelBook_IconStar01a.png",   TextureLoadPolicy::PIXEL_ART },
    { TextureId::TB_LINE_01A,         "textures/ui/travelbook/UI_TravelBook_Line01a.png",        TextureLoadPolicy::PIXEL_ART },
    { TextureId::TB_MARKER_01A,       "textures/ui/travelbook/UI_TravelBook_Marker01a.png",      TextureLoadPolicy::PIXEL_ART },
    { TextureId::TB_SELECT_01A,       "textures/ui/travelbook/UI_TravelBook_Select01a.png",      TextureLoadPolicy::PIXEL_ART },
    { TextureId::TB_SLOT_01A,         "textures/ui/travelbook/UI_TravelBook_Slot01a.png",        TextureLoadPolicy::PIXEL_ART },
    { TextureId::TB_TOGGLE_03A,       "textures/ui/travelbook/UI_TravelBook_Toggle03a.png",      TextureLoadPolicy::PIXEL_ART },
    { TextureId::TB_TOGGLE_03B,       "textures/ui/travelbook/UI_TravelBook_Toggle03b.png",      TextureLoadPolicy::PIXEL_ART }
}};

constexpr std::array<FontAssetDef, static_cast<size_t>(FontId::COUNT)> FONTS = {{
    { FontId::NOM_NA_TONG,        "fonts/NomNaTong-Regular.ttf" },
    { FontId::PIXELOID_SANS,      "fonts/PixeloidSans.ttf" },
    { FontId::PIXELOID_SANS_BOLD, "fonts/PixeloidSansBold.ttf" },
    { FontId::VIAODA_LIBRE,       "fonts/ViaodaLibre-Regular.ttf" }
}};

constexpr std::array<SoundAssetDef, static_cast<size_t>(SoundId::COUNT)> SOUNDS = {{
    { SoundId::MOVE,              "audio/sfx/move.ogg" },
    { SoundId::CAPTURE,           "audio/sfx/capture.ogg" },
    { SoundId::CHECK,             "audio/sfx/check.wav" },
    { SoundId::WIN,               "audio/sfx/win.wav" },
    { SoundId::SELECT,            "audio/sfx/select.wav" }
}};

constexpr std::array<MusicAssetDef, static_cast<size_t>(MusicId::COUNT)> MUSIC = {{
    { MusicId::BGM_MAIN,          "audio/music/golden_gleam.ogg" }
}};

} // namespace AssetRegistry
