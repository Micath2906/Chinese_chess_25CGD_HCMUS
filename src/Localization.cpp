#include "Localization.h"

// Default language is Vietnamese
Language Loc::currentLang = Language::TIENG_VIET;

std::string Loc::get(LocKey key) {
    bool vi = (currentLang == Language::TIENG_VIET);

    switch (key) {
        // App Header
        case LocKey::APP_TITLE:
            return vi ? "CỜ TƯỚNG" : "CHINESE CHESS";
        case LocKey::APP_SUBTITLE:
            return vi ? "ĐỒ ÁN MÔN HỌC OOP - HCMUS" : "OOP PROJECT - HCMUS";

        // Main Menu
        case LocKey::MENU_PLAY:
            return vi ? "CHƠI MỚI" : "PLAY";
        case LocKey::MENU_LOAD:
            return vi ? "TIẾP TỤC" : "LOAD";
        case LocKey::MENU_SETTINGS:
            return vi ? "CÀI ĐẶT" : "SETTINGS";
        case LocKey::MENU_INTRO:
            return vi ? "THÔNG TIN" : "INTRODUCTION";
        case LocKey::MENU_EXIT:
            return vi ? "THOÁT" : "EXIT";

        // Play Submenu
        case LocKey::PLAY_SUB_TITLE:
            return vi ? "CHỌN CHẾ ĐỘ CHƠI" : "SELECT GAME MODE";
        case LocKey::PLAY_PVP:
            return vi ? "Đấu hai người (PvP)" : "Player vs Player";
        case LocKey::PLAY_PVAI:
            return vi ? "Đấu với máy (PvAI)" : "Player vs AI";
        case LocKey::PLAY_DEMO:
            return vi ? "Xem máy đánh (Demo)" : "Demo Mode";
        case LocKey::BACK:
            return vi ? "Quay lại" : "Back";

        // Match Setup
        case LocKey::SETUP_PVP_TITLE:
            return vi ? "THIẾT LẬP: HAI NGƯỜI CHƠI" : "SETUP: PLAYER VS PLAYER";
        case LocKey::SETUP_PVAI_TITLE:
            return vi ? "THIẾT LẬP: ĐẤU VỚI MÁY" : "SETUP: PLAYER VS AI";
        case LocKey::P1_LABEL:
            return vi ? "NGƯỜI CHƠI 1" : "PLAYER 1";
        case LocKey::P2_LABEL:
            return vi ? "NGƯỜI CHƠI 2" : "PLAYER 2";
        case LocKey::PLAYER_LABEL:
            return vi ? "NGƯỜI CHƠI" : "PLAYER";
        case LocKey::AI_LABEL:
            return vi ? "MÁY AI" : "AI ENGINE";
        case LocKey::ENTER_NAME_P1:
            return vi ? "Tên người chơi 1:" : "Player 1 name:";
        case LocKey::ENTER_NAME_P2:
            return vi ? "Tên người chơi 2:" : "Player 2 name:";
        case LocKey::ENTER_NAME_PLAYER:
            return vi ? "Tên người chơi:" : "Player name:";
        case LocKey::CHOOSE_CHAR:
            return vi ? "Nhân vật: " : "Character: ";
        case LocKey::MATCH_TIMER_LABEL:
            return vi ? "Thời gian mỗi bên: " : "Match Time: ";
        case LocKey::TIME_5M:
            return vi ? "5 Phút" : "5 Mins";
        case LocKey::TIME_10M:
            return vi ? "10 Phút" : "10 Mins";
        case LocKey::TIME_15M:
            return vi ? "15 Phút" : "15 Mins";
        case LocKey::TIME_UNLIMITED:
            return vi ? "Vô hạn" : "Unlimited";
        case LocKey::TOSS_COIN:
            return vi ? "Tung đồng xu chọn bên đi trước" : "Roll Random First Move";
        case LocKey::FIRST_MOVE_INFO:
            return vi ? "Phe đi trước: Quân Đỏ" : "First move: Red side";
        case LocKey::START_GAME:
            return vi ? "BẮT ĐẦU VÁN ĐẤU" : "START GAME";

        // AI options
        case LocKey::AI_DIFFICULTY:
            return vi ? "Độ khó máy: " : "AI Difficulty: ";
        case LocKey::DIFF_EASY:
            return vi ? "Dễ" : "Easy";
        case LocKey::DIFF_MED:
            return vi ? "Vừa" : "Medium";
        case LocKey::DIFF_HARD:
            return vi ? "Khó" : "Hard";
        case LocKey::WHO_GOES_FIRST:
            return vi ? "Ai đi trước: " : "First Move: ";
        case LocKey::PLAYER_FIRST:
            return vi ? "Bạn đi trước (Quân Đỏ)" : "You first (Red)";
        case LocKey::AI_FIRST:
            return vi ? "Máy đi trước (Quân Đỏ)" : "AI first (Red)";
        case LocKey::RANDOM_FIRST:
            return vi ? "Ngẫu nhiên" : "Random";

        // Load Menu
        case LocKey::LOAD_TITLE:
            return vi ? "CÁC VÁN ĐẤU ĐÃ LƯU" : "SAVED GAMES";
        case LocKey::SAVED_GAMES:
            return vi ? "Danh sách ván đã lưu" : "Saved Games List";
        case LocKey::SLOT_LABEL:
            return vi ? "Ô lưu " : "Save Slot ";
        case LocKey::EMPTY_SLOT:
            return vi ? "(Ô lưu trống - Chưa có dữ liệu)" : "(Empty save slot)";
        case LocKey::LOAD_BUTTON:
            return vi ? "Vào ván đấu" : "Load Game";
        case LocKey::DELETE_BUTTON:
            return vi ? "Xóa bản lưu" : "Delete Save";

        // Settings Menu
        case LocKey::SETTINGS_TITLE:
            return vi ? "CÀI ĐẶT HỆ THỐNG" : "SYSTEM SETTINGS";
        case LocKey::TAB_AUDIO:
            return vi ? "ÂM THANH" : "AUDIO";
        case LocKey::TAB_GRAPHICS:
            return vi ? "ĐỒ HỌA & BÀN CỜ" : "GRAPHICS";
        case LocKey::TAB_GAMEPLAY:
            return vi ? "TRÒ CHƠI" : "GAMEPLAY";
        case LocKey::TAB_CONTROLS:
            return vi ? "ĐIỀU KHIỂN" : "CONTROLS";

        // Audio Settings
        case LocKey::SETTING_MASTER_VOL:
            return vi ? "Âm lượng tổng (Master): " : "Master Volume: ";
        case LocKey::SETTING_SFX_TOGGLE:
            return vi ? "Hiệu ứng âm thanh (SFX): " : "Sound Effects (SFX): ";
        case LocKey::SETTING_SFX_VOL:
            return vi ? "Âm lượng hiệu ứng (SFX): " : "SFX Volume: ";
        case LocKey::SETTING_BGM_TOGGLE:
            return vi ? "Nhạc nền cổ trang (BGM): " : "Background Music (BGM): ";
        case LocKey::SETTING_BGM_VOL:
            return vi ? "Âm lượng nhạc nền (BGM): " : "BGM Volume: ";
        case LocKey::SETTING_TEST_AUDIO:
            return vi ? "Nghe thử âm thanh" : "Test Sound";

        // Graphics Settings
        case LocKey::SETTING_BOARD_THEME:
            return vi ? "Giao diện bàn cờ: " : "Board Theme: ";
        case LocKey::THEME_WOOD:
            return vi ? "Gỗ Cổ Điển" : "Classic Wood";
        case LocKey::THEME_JADE:
            return vi ? "Ngọc Bích Hoàng Gia" : "Imperial Jade";
        case LocKey::THEME_DARK:
            return vi ? "Huyền Thạch (Dark Mode)" : "Midnight Obsidian";
        case LocKey::THEME_BAMBOO:
            return vi ? "Giấy Trúc Trầm" : "Warm Bamboo";

        case LocKey::SETTING_PIECE_STYLE:
            return vi ? "Kiểu ký hiệu quân cờ: " : "Piece Style: ";
        case LocKey::PIECE_REALISTIC:
            return vi ? "Quân gỗ truyền thống (Hình ảnh)" : "Traditional Wood (Sprites)";
        case LocKey::PIECE_VIETNAMESE:
            return vi ? "Tiếng Việt (Xe, Mã, Voi...)" : "Full Names (Xe, Mã, Voi...)";
        case LocKey::PIECE_SHORT:
            return vi ? "Ký hiệu tắt (X, M, V...)" : "Short Badges (X, M, V...)";
        case LocKey::PIECE_INTL:
            return vi ? "Quốc tế (R, H, E...)" : "International (R, H, E...)";

        case LocKey::SETTING_MOVE_HINTS:
            return vi ? "Gợi ý nước đi hợp lệ: " : "Valid Move Hints: ";
        case LocKey::SETTING_LAST_MOVE:
            return vi ? "Đánh dấu nước vừa đi: " : "Last Move Highlight: ";
        case LocKey::SETTING_COORDINATES:
            return vi ? "Tọa độ mép bàn cờ (1-9, A-J): " : "Board Coordinates (1-9, A-J): ";
        case LocKey::SETTING_WINDOW_MODE:
            return vi ? "Chế độ màn hình: " : "Display Mode: ";
        case LocKey::MODE_WINDOWED:
            return vi ? "Cửa sổ (1200x800)" : "Windowed (1200x800)";
        case LocKey::MODE_FULLSCREEN:
            return vi ? "Toàn màn hình" : "Fullscreen";
        case LocKey::SETTING_ATMOSPHERE_STYLE:
            return vi ? "Hiệu ứng khí quyển: " : "Atmospheric VFX: ";
        case LocKey::SETTING_ATMOSPHERE_DENSITY:
            return vi ? "Mật độ hạt khí quyển" : "Atmospheric Particle Density";
        case LocKey::ATMOSPHERE_LEAVES:
            return vi ? "Lá Trúc Xanh (Bamboo)" : "Bamboo Leaves";
        case LocKey::ATMOSPHERE_PETALS:
            return vi ? "Cánh Đào Hồng (Peach)" : "Peach Blossoms";
        case LocKey::ATMOSPHERE_SPARKS:
            return vi ? "Tàn Lửa Vàng (Sparks)" : "Golden Sparks";
        case LocKey::PIECE_AURA_LABEL:
            return vi ? "Hào quang quân cờ: " : "Piece Aura: ";
        case LocKey::AURA_CLASSIC:
            return vi ? "Vành Gỗ Cổ Điển" : "Classic Wood Trim";
        case LocKey::AURA_GOLD:
            return vi ? "Hoàng Kim Hào Quang" : "Imperial Gold Aura";
        case LocKey::AURA_FLAME:
            return vi ? "Xích Long Viêm (Hỏa)" : "Dragon Flame Aura";
        case LocKey::AURA_THUNDER:
            return vi ? "Lam Lôi Phong (Ngọc)" : "Thunder Jade Aura";
        case LocKey::AURA_VOID:
            return vi ? "Hắc Diệu Ma Thạch" : "Shadow Void Aura";

        // Gameplay Settings
        case LocKey::SETTING_LANG:
            return vi ? "Ngôn ngữ game: " : "Game Language: ";
        case LocKey::SETTING_AI_THINK:
            return vi ? "Mô phỏng AI suy nghĩ: " : "AI Thinking Delay: ";
        case LocKey::AI_THINK_REALISTIC:
            return vi ? "Chân thực (450ms)" : "Realistic (450ms)";
        case LocKey::AI_THINK_INSTANT:
            return vi ? "Tức thì (0ms)" : "Instant (0ms)";
        case LocKey::SETTING_CHECK_ALARM:
            return vi ? "Báo động chiếu tướng: " : "Check Warning Alarm: ";

        // Common & Actions
        case LocKey::ON:
            return vi ? "Bật" : "ON";
        case LocKey::OFF:
            return vi ? "Tắt" : "OFF";
        case LocKey::BTN_APPLY_SAVE:
            return vi ? "Áp dụng & Lưu" : "Apply & Save";
        case LocKey::BTN_RESTORE_DEFAULTS:
            return vi ? "Khôi phục mặc định" : "Restore Defaults";

        // Keybindings Menu
        case LocKey::KEYBINDING_TITLE:
            return vi ? "CÀI ĐẶT PHÍM ĐIỀU KHIỂN" : "CUSTOM KEY BINDINGS";
        case LocKey::KEYBINDING_SUBTITLE:
            return vi ? "Nhấp vào hành động bên dưới để tùy ý đổi phím theo ý muốn:"
                      : "Click on any action below to customize its key binding:";
        case LocKey::KEY_ACTION_UP:
            return vi ? "Di chuyển Lên" : "Move Up";
        case LocKey::KEY_ACTION_DOWN:
            return vi ? "Di chuyển Xuống" : "Move Down";
        case LocKey::KEY_ACTION_LEFT:
            return vi ? "Di chuyển Trái" : "Move Left";
        case LocKey::KEY_ACTION_RIGHT:
            return vi ? "Di chuyển Phải" : "Move Right";
        case LocKey::KEY_ACTION_SELECT:
            return vi ? "Chọn / Hạ cờ" : "Select / Move";
        case LocKey::KEY_ACTION_DESELECT:
            return vi ? "Hủy chọn / Menu" : "Deselect / Cancel";
        case LocKey::KEY_ACTION_QUIT:
            return vi ? "Thoát nhanh" : "Quick Quit";
        case LocKey::KEY_PRESS_PROMPT:
            return vi ? "[ Nhấn phím bất kỳ... ]" : "[ Press any key... ]";
        case LocKey::KEY_RESET_DEFAULT:
            return vi ? "Đặt lại mặc định" : "Reset Defaults";
        case LocKey::PRESET_WASD_BTN:
            return vi ? "Bộ phím WASD" : "WASD Preset";
        case LocKey::PRESET_ARROWS_BTN:
            return vi ? "Bộ phím Mũi tên" : "Arrow Keys Preset";

        // Introduction Menu
        case LocKey::INTRO_TITLE:
            return vi ? "GIỚI THIỆU & LUẬT CHƠI CỜ TƯỚNG" : "INTRODUCTION & RULES";

        // In Game UI
        case LocKey::TURN_RED:
            return vi ? "Lượt đi: Quân Đỏ" : "Turn: Red";
        case LocKey::TURN_BLACK:
            return vi ? "Lượt đi: Quân Đen" : "Turn: Black";
        case LocKey::CHECK_ALERT:
            return vi ? "!!! CHIẾU TƯỚNG !!!" : "!!! CHECK !!!";
        case LocKey::MOVE_HISTORY:
            return vi ? "Lịch sử nước đi:" : "Move History:";
        case LocKey::NO_MOVES_YET:
            return vi ? "(Chưa có nước đi nào)" : "(No moves yet)";
        case LocKey::TIME_RED:
            return vi ? "Đỏ: " : "Red: ";
        case LocKey::TIME_BLACK:
            return vi ? "Đen: " : "Black: ";
        case LocKey::UNDO:
            return vi ? "Hoàn tác" : "Undo";
        case LocKey::REDO:
            return vi ? "Đi tiếp" : "Redo";
        case LocKey::NEW_GAME:
            return vi ? "Ván mới" : "New Game";
        case LocKey::DRAW_OFFER:
            return vi ? "Xin hòa" : "Offer Draw";
        case LocKey::SURRENDER:
            return vi ? "Đầu hàng" : "Resign";
        case LocKey::SAVE_GAME:
            return vi ? "Lưu ván đấu" : "Save Game";
        case LocKey::LOAD_GAME:
            return vi ? "Tải ván đấu" : "Load Game";
        case LocKey::BACK_MENU:
            return vi ? "Về Menu" : "Main Menu";

        // In Game Pause Popup Menu
        case LocKey::PAUSE_MENU_TITLE:
            return vi ? "MENU TRẬN ĐẤU" : "MATCH PAUSED";
        case LocKey::MATCH_PAUSED_SUBTITLE:
            return vi ? "(Trận đấu tạm dừng - Nhấn [ESC] hoặc bấm để tiếp tục)" : "(Game Paused - Press [ESC] or click to resume)";
        case LocKey::RESUME:
            return vi ? "Tiếp tục chơi" : "Resume Game";
        case LocKey::BTN_GAME_MENU:
            return vi ? "Menu trận đấu [ESC]" : "Match Menu [ESC]";

        // End Game Popup
        case LocKey::GAME_OVER:
            return vi ? "KẾT THÚC VÁN CỜ" : "GAME OVER";
        case LocKey::RED_WINS:
            return vi ? "QUÂN ĐỎ CHIẾN THẮNG!" : "RED WINS!";
        case LocKey::BLACK_WINS:
            return vi ? "QUÂN ĐEN CHIẾN THẮNG!" : "BLACK WINS!";
        case LocKey::DRAW_MATCH:
            return vi ? "HAI BÊN HÒA CỜ!" : "GAME DRAW!";
        case LocKey::PLAY_AGAIN:
            return vi ? "Chơi lại" : "Play Again";
    }

    return "";
}

std::string Loc::getCharName(int index) {
    bool vi = (currentLang == Language::TIENG_VIET);
    switch (index % 5) {
        case 0: return vi ? "Tướng Quân" : "General";
        case 1: return vi ? "Quân Sư" : "Strategist";
        case 2: return vi ? "Kỳ Vương" : "Grandmaster";
        case 3: return vi ? "Hiệp Khách" : "Knight";
        case 4: return vi ? "Thư Sinh" : "Scholar";
        default: return vi ? "Kỳ Thủ" : "Player";
    }
}

std::string Loc::getCharDesc(int index) {
    bool vi = (currentLang == Language::TIENG_VIET);
    switch (index % 5) {
        case 0: return vi ? "Tiên phong dũng mãnh, phong cách công phá vũ bão" : "Brave vanguard, aggressive offensive style";
        case 1: return vi ? "Mưu lược sâu xa, tính toán từng nước cờ chặt chẽ" : "Deep tactical thinker, meticulous positional planner";
        case 2: return vi ? "Kỳ nghệ uyên thâm, công thủ toàn diện kỳ tài" : "Legendary master, perfectly balanced play";
        case 3: return vi ? "Linh hoạt biến hóa, phản công bất ngờ xuất quỷ nhập thần" : "Agile and flexible, swift counter-attacks";
        case 4: return vi ? "Phong thái ung dung, cẩn trọng phòng ngự vững như bàn thạch" : "Composed and wise, rock-solid defense";
        default: return "";
    }
}
