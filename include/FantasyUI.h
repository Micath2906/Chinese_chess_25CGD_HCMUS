#pragma once
#include <SFML/Graphics.hpp>
#include <string>

enum class FantasyPanelStyle {
    ORNATE_WOOD,      // Rich carved dark wood with bronze rivets & metal corners
    WOOD_SLOT,        // Deep beveled dark oak recessed panel / card
    SQUARE_FRAME,     // Ornate square frame (for avatars, badges)
    PARCHMENT_CARD,   // Warm aged parchment with delicate gold border
    PARCHMENT_RIBBON  // Carved scroll ribbon for section banners
};

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
                   FantasyPanelStyle style = FantasyPanelStyle::ORNATE_WOOD,
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

    // Incense stick countdown timer (replaces drawDragonTimer)
    void drawDragonTimer(sf::RenderWindow& window,
                         const sf::FloatRect& dstRect,
                         float remainingSeconds,
                         float totalSeconds,
                         const sf::Font& font,
                         bool isLowTime = false);

    void drawRibbonTitle(sf::RenderWindow& window,
                         float centerX, float topY, float width, float height,
                         const std::string& title,
                         const sf::Font& font,
                         unsigned int charSize = 16,
                         const sf::Color& textColor = sf::Color(255, 235, 180),
                         const sf::Color& tint = sf::Color::White);

    // Textures
    sf::Texture texMasterSpritesheet; // assets/xiangqi_master_spritesheet.png
    sf::Texture texMedieval;          // Alias for backward compatibility
    sf::Texture texFantasy;
    sf::Texture texDragonHpBar;

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
