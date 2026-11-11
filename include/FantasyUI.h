#pragma once
#include <SFML/Graphics.hpp>
#include <string>

enum class OrientalPanelStyle {
    EBONY_WOOD,       // Gỗ mun viền đồng mây (thay cho ORNATE_WOOD)
    PARCHMENT_SCROLL, // Giấy ngà vân mây (thay cho PARCHMENT_CARD)
    JADE_INSET,       // Khảm ngọc bích chìm

    // Backward compatibility aliases
    ORNATE_WOOD = EBONY_WOOD,
    PARCHMENT_CARD = PARCHMENT_SCROLL,
    WOOD_SLOT = JADE_INSET,
    SQUARE_FRAME = EBONY_WOOD,
    PARCHMENT_RIBBON = PARCHMENT_SCROLL
};

using FantasyPanelStyle = OrientalPanelStyle;

enum class FantasyButtonStyle {
    DEFAULT,          // Large carved wood button (118x48 slice)
    SLIM,             // Slim carved button (118x32 slice)
    SQUARE            // Square icon button (56x56 slice)
};

class FantasyUI {
public:
    static FantasyUI& get();

    bool init();
    bool isLoaded() const { return loaded; }

    // Core 9-slice primitive using hardware-accelerated vertex array
    void draw9Slice(sf::RenderWindow& window,
                    const sf::Texture& texture,
                    const sf::IntRect& srcRect,
                    int inLeft, int inTop, int inRight, int inBottom,
                    const sf::FloatRect& dstRect,
                    const sf::Color& tint = sf::Color::White);

    // High-level UI Components
    void drawPanel(sf::RenderWindow& window,
                   const sf::FloatRect& dstRect,
                   OrientalPanelStyle style = OrientalPanelStyle::EBONY_WOOD,
                   const sf::Color& tint = sf::Color::White);

    void drawButton(sf::RenderWindow& window,
                    const sf::FloatRect& dstRect,
                    const std::string& label,
                    const sf::Font& font,
                    unsigned int charSize,
                    bool isHovered = false,
                    bool isSelected = false,
                    bool isEnabled = true,
                    const sf::Texture* icon = nullptr,
                    FantasyButtonStyle style = FantasyButtonStyle::DEFAULT);

    // Incense stick countdown timer (Stage 2.3)
    void drawIncenseTimer(sf::RenderWindow& window,
                          const sf::FloatRect& rect,
                          float remainingSeconds,
                          float totalSeconds);

    void drawDragonTimer(sf::RenderWindow& window,
                         const sf::FloatRect& rect,
                         float remainingSeconds,
                         float totalSeconds,
                         const sf::Font& font,
                         bool isLowTime = false) {
        (void)font; (void)isLowTime;
        drawIncenseTimer(window, rect, remainingSeconds, totalSeconds);
    }

    void drawRibbonTitle(sf::RenderWindow& window,
                         float centerX, float topY, float width, float height,
                         const std::string& title,
                         const sf::Font& font,
                         unsigned int charSize = 16,
                         const sf::Color& textColor = sf::Color(255, 235, 180),
                         const sf::Color& tint = sf::Color::White);

    // Textures
    sf::Texture texMasterSpritesheet; // assets/xiangqi_oriental_sheet.png
    sf::Texture texMedieval;          // Alias for backward compatibility
    sf::Texture texFantasy;

    // Oriental Texture Atlas Rects (Stage 1.1)
    sf::IntRect rcPanelEbony;
    sf::IntRect rcParchment;
    sf::IntRect rcRibbonHeader;
    sf::IntRect rcBtnNormal;
    sf::IntRect rcBtnHover;
    sf::IntRect rcBtnPressed;
    sf::IntRect rcIncenseStick;
    sf::IntRect rcIconUndo;
    sf::IntRect rcIconRedo;
    sf::IntRect rcIconDraw;
    sf::IntRect rcIconResign;

private:
    FantasyUI();
    bool loaded = false;

    // Slicing definitions
    sf::IntRect rcBtnNorm;
    sf::IntRect rcBtnHov;
    sf::IntRect rcBtnSlimNorm;
    sf::IntRect rcBtnSlimHov;
    sf::IntRect rcSqNorm;
    sf::IntRect rcSqHov;

    sf::IntRect rcPanelOrnate;
    sf::IntRect rcSlotWood;
    sf::IntRect rcRibbon;
};
