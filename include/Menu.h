#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <functional>
#include "Localization.h"

// Forward declaration
struct SaveSlotInfo;

// Draw background
void veNenMenuGiaoDien(sf::RenderWindow& window);

enum class MenuSection {
    NONE = -1,
    PLAY = 0,
    LOAD = 1,
    SETTINGS = 2,
    INFO = 3,
    EXIT = 4
};

struct MenuAssets {
    sf::Texture cleanBg;          // assets/background.png
    sf::Texture bookCover;        // UI_TravelBook_BookCover01a.png
    sf::Texture bookPageLeft;     // UI_TravelBook_BookPageLeft01a.png
    sf::Texture bookPageRight;    // UI_TravelBook_BookPageRight01a.png
    sf::Texture frameNormal;      // UI_TravelBook_Frame01a.png
    sf::Texture frameHover;       // UI_TravelBook_FrameSelect01a.png
    sf::Texture framePressed;     // UI_TravelBook_FrameSelect01b.png
    sf::Texture marker;           // UI_TravelBook_Marker01a.png
    sf::Texture decLine;          // UI_TravelBook_Line01a.png

    // Menu icons
    sf::Texture iconPlay;         // UI_TravelBook_IconPlay01a.png
    sf::Texture iconLoad;         // UI_TravelBook_IconRestart01a.png
    sf::Texture iconSettings;     // UI_TravelBook_IconGear01a.png
    sf::Texture iconInfo;         // UI_TravelBook_IconStar01a.png
    sf::Texture iconExit;         // UI_TravelBook_IconCross01a.png
    sf::Texture iconArrow;        // UI_TravelBook_IconArrow01a.png

    // Controls
    sf::Texture bar;              // UI_TravelBook_Bar01a.png
    sf::Texture fill;             // UI_TravelBook_Fill01a.png
    sf::Texture handle;           // UI_TravelBook_Handle03a.png
    sf::Texture buttonValue;      // UI_TravelBook_ButtonValue01a.png
    sf::Texture toggleOn;         // UI_TravelBook_Toggle03a.png
    sf::Texture toggleOff;        // UI_TravelBook_Toggle03b.png
    sf::Texture padLB;            // UI_TravelBook_GamepadLB01a.png
    sf::Texture padRB;            // UI_TravelBook_GamepadRB01a.png
    sf::Texture padL;             // UI_TravelBook_GamepadL01a.png
    sf::Texture padB;             // UI_TravelBook_GamepadB01a.png
    sf::Texture padA;             // UI_TravelBook_GamepadA01a.png
    sf::Texture slotBg;           // UI_TravelBook_Slot01a.png

    // Cursor and background
    sf::Texture cursorNormal;     // UI_TravelBook_Cursor01c.png
    sf::Texture cursorPressed;    // UI_TravelBook_Cursor01d.png
    sf::Texture selectCorners;    // UI_TravelBook_Select01a.png
    sf::Texture playBg;           // assets/background play.jpg

    sf::Font fontEn;              // PixeloidSans.ttf (Regular)
    sf::Font fontEnBold;          // PixeloidSansBold.ttf (Bold)
    sf::Font fontVn;              // arial.ttf / segoeui.ttf (Full Vietnamese Unicode)
    sf::Font fontVnBold;          // arialbd.ttf / arial.ttf (Full Vietnamese Unicode Bold)
    bool fontEnLoaded = false;
    bool fontEnBoldLoaded = false;
    bool fontVnLoaded = false;
    bool fontVnBoldLoaded = false;

    sf::Font vnFont;              // Compatibility
    sf::Font globalFont;          // Compatibility
    sf::Font fontBold;            // Compatibility
    bool vnFontLoaded = false;
    bool fontLoaded = false;
    bool fontBoldLoaded = false;
    bool bookLoaded = false;

    MenuAssets();
};

MenuAssets& getMenuAssets();
sf::Font& getMenuFont(bool tiengViet, bool bold = false);

// Draw selection corners
void ve4GocLuaChon(sf::RenderWindow& window, sf::Vector2f pos, sf::Vector2f sz, float pad = 3.0f, float scale = 2.0f);

class Button {
private:
    sf::RectangleShape shape;
    sf::Text text;
    std::function<void()> callback;
    bool hovered;
    bool enabled;
    bool selected;
    unsigned int baseCharSize;

    sf::Color colorNormal;
    sf::Color colorHover;
    sf::Color colorPressed;
    sf::Color colorDisabled;

    const sf::Texture* texNormal;
    const sf::Texture* texHover;
    const sf::Texture* texIcon;

    void canGiuaText();

public:
    Button(float x, float y, float width, float height,
        const std::string& label, sf::Font& font,
        std::function<void()> onClick);

    void setPosition(float x, float y);
    void setLabel(const std::string& label);
    void setCharacterSize(unsigned int size);
    void setFont(const sf::Font& f);
    void setColors(sf::Color normal, sf::Color hover, sf::Color pressed);
    void setEnabled(bool en) { enabled = en; }
    bool isEnabled() const { return enabled; }
    void setSelected(bool sel) {
        selected = sel;
        if (selected || hovered) {
            text.setFillColor(colorHover);
            text.setStyle(sf::Text::Bold);
        } else {
            text.setFillColor(colorNormal);
            text.setStyle(sf::Text::Bold);
        }
        canGiuaText();
    }
    bool isSelected() const { return selected; }
    void setIcon(const sf::Texture* icon);
    const sf::Texture* getIcon() const { return texIcon; }
    void setTextures(const sf::Texture* normal, const sf::Texture* hover) {
        texNormal = normal;
        texHover = hover;
    }
    void setShapeColor(sf::Color c) { shape.setFillColor(c); }
    void setOutline(float th, sf::Color c) { shape.setOutlineThickness(th); shape.setOutlineColor(c); }

    void update(const sf::Vector2i& mousePos);
    void handleClick(const sf::Vector2i& mousePos);
    void draw(sf::RenderWindow& window);
    bool contains(const sf::Vector2i& point) const;

    sf::Vector2f getPosition() const { return shape.getPosition(); }
    sf::Vector2f getSize() const { return shape.getSize(); }
    std::string getLabel() const { return text.getString(); }
    bool isHovered() const { return hovered; }
    void setHovered(bool hov) {
        hovered = hov;
        text.setFillColor((selected || hovered) ? colorHover : colorNormal);
    }
    void triggerClick() { if (enabled && callback) callback(); }
};

class Menu {
private:
    sf::Font font;
    sf::Text title;
    std::vector<Button> buttons;

    // Book animation
    MenuSection activeSection;
    MenuSection pendingSection;
    float bookProgress; // 0.0f (closed) to 1.0f (fully open)
    bool isBookOpening;
    bool isBookClosing;
    bool isBookClosed;
    sf::Clock animClock;

    // Sub-tabs
    int settingsSubTab; // 0: Audio, 1: Graphics, 2: Gameplay, 3: Controls
    int infoSubTab;     // 0: Architecture, 1: Piece Rules
    int currentSlot;    // 0, 1, 2
    int draggingSlider; // -1: none, 0: master, 1: bgm, 2: sfx

    // Right page buttons
    std::vector<Button> rightPageButtons;
    sf::Vector2i mousePosCache;

    void khoiTaoCacNutBia();
    void khoiTaoCacNutTrangPhai();

public:
    bool laMenuChinh = false;

    // Callbacks
    std::function<void()> onPlayPvp;
    std::function<void()> onPlayPvai;
    std::function<void()> onPlayDemo;

    std::function<void(int slot)> onSelectSlot;
    std::function<void(int slot)> onLoadSlot;
    std::function<void(int slot)> onDeleteSlot;

    std::function<void()> onToggleSound;
    std::function<void(float delta)> onMasterVolStep;
    std::function<void()> onToggleSfx;
    std::function<void()> onToggleBgm;
    std::function<void()> onTestSound;
    std::function<void()> onToggleBoardTheme;
    std::function<void()> onTogglePieceStyle;
    std::function<void()> onToggleWindowMode;
    std::function<void()> onToggleAtmosphere;
    std::function<void()> onToggleLang;
    std::function<void()> onToggleAiDelay;
    std::function<void()> onToggleCheckAlarm;
    std::function<void()> onApplySettings;
    std::function<void()> onRestoreDefaults;
    std::function<void()> onPresetWasd;
    std::function<void()> onPresetArrows;

    std::function<void()> onConfirmExit;
    std::function<void()> onReturnToGame;

    // Audio callbacks
    std::function<void(float vol)> onSetMasterVolume;
    std::function<void(float vol)> onSetBgmVolume;
    std::function<void(float vol)> onSetSfxVolume;
    std::function<float()> getMasterVolume;
    std::function<float()> getBgmVolume;
    std::function<float()> getSfxVolume;
    std::function<bool()> getMasterBat;
    std::function<bool()> getSfxBat;
    std::function<bool()> getBgmBat;
    std::function<void()> onToggleMasterSound;

    // Status getters
    std::function<std::string(int id)> getSettingStatusStr;
    std::function<std::string(int slot, int infoType)> getSlotInfoStr;
    std::function<void(sf::RenderWindow& window, float x, float y, float w, float h)> onDrawPiecePreview;

    Menu();
    bool loadFont(const std::string& fontPath);
    void setFont(const sf::Font& f);
    void setTitle(const std::string& t);

    void themButton(float x, float y, float width, float height,
        const std::string& label, std::function<void()> onClick);
    void xoaTatCaButton() { buttons.clear(); }
    Button* layButton(size_t index) { return (index < buttons.size()) ? &buttons[index] : nullptr; }
    size_t getButtonCount() const { return buttons.size(); }

    void triggerOpenAnimation(MenuSection targetSection = MenuSection::PLAY);
    void triggerCloseAnimation();
    void closeBook() { triggerCloseAnimation(); }

    enum class MenuFocus { MAIN_MENU, SUB_MENU };
    MenuFocus currentFocus = MenuFocus::MAIN_MENU;
    bool keyboardNavActive = false;
    int rightPageIndex = 0;
    int coverIndex = 0;

    void navigateUp();
    void navigateDown();
    void navigateLeft();
    void navigateRight();
    void activateCurrent();
    void switchToMainMenu() { currentFocus = MenuFocus::MAIN_MENU; }
    void switchToSubMenu() { currentFocus = MenuFocus::SUB_MENU; rightPageIndex = 0; }
    MenuFocus getFocus() const { return currentFocus; }
    bool isOpening() const { return isBookOpening; }
    bool isClosing() const { return isBookClosing; }
    bool isFullyOpen() const { return bookProgress >= 1.0f && !isBookClosed; }
    bool isClosed() const { return isBookClosed; }
    float getBookProgress() const { return bookProgress; }

    void setSection(MenuSection sec);
    MenuSection getSection() const { return activeSection; }

    void capNhatNutTrangPhai();
    void update(const sf::Vector2i& mousePos);
    void handleClick(const sf::Vector2i& mousePos);
    void handleMouseDown(const sf::Vector2i& mousePos);
    void handleMouseUp(const sf::Vector2i& mousePos);
    void draw(sf::RenderWindow& window, bool tiengViet = true);
};
