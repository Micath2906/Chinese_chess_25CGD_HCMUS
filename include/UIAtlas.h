#pragma once
#include <SFML/Graphics/Rect.hpp>

/**
 * @file UIAtlas.h
 * @brief Định nghĩa tọa độ cắt vùng (Texture Atlas Regions) cho giao diện Oriental Fantasy.
 * 
 * Áp dụng nguyên lý Single Source of Truth cho toàn bộ sprite UI trong xiangqi_oriental_sheet.png,
 * loại bỏ hoàn toàn magic numbers rải rác trong mã nguồn.
 */

namespace UIAtlas {

struct AtlasRegion {
    int x;
    int y;
    int w;
    int h;

    inline sf::IntRect toIntRect() const {
        return sf::IntRect(x, y, w, h);
    }
};

// 9-Slice Panels & Backgrounds
constexpr AtlasRegion PanelEbony    = { 0,   0,   192, 192 }; // Khung gỗ mun viền đồng mây
constexpr AtlasRegion Parchment     = { 200, 0,   160, 160 }; // Nền giấy xuyến chỉ
constexpr AtlasRegion RibbonHeader  = { 370, 0,   260, 54  }; // Dải ruy-băng tiêu đề

// 9-Slice Carved Wood Buttons
constexpr AtlasRegion ButtonNormal  = { 0,   200, 220, 50  }; // Nút trạng thái bình thường
constexpr AtlasRegion ButtonHover   = { 0,   260, 220, 50  }; // Nút hover (sáng vàng viền ngọc)
constexpr AtlasRegion ButtonPressed = { 0,   320, 220, 50  }; // Nút khi nhấn

// Action Icons (48x48)
constexpr AtlasRegion IconUndo      = { 230, 200, 48,  48  }; // Đi lại
constexpr AtlasRegion IconRedo      = { 285, 200, 48,  48  }; // Đi tiếp
constexpr AtlasRegion IconDraw      = { 340, 200, 48,  48  }; // Xin hòa
constexpr AtlasRegion IconResign    = { 395, 200, 48,  48  }; // Đầu hàng

// Gameplay Timers
constexpr AtlasRegion IncenseStick  = { 0,   380, 320, 24  }; // Thân nhang đồng hồ

} // namespace UIAtlas
