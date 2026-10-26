#include "Menu.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <sstream>

static bool loadTexSafe(sf::Texture& tex, const std::string& path, bool smooth = false) {
    const std::vector<std::string> prefixes = { "", "../", "../../", "../../../", "../../../../" };
    for (const auto& p : prefixes) {
        if (tex.loadFromFile(p + path)) {
            tex.setSmooth(smooth);
            return true;
        }
    }
    return false;
}

static bool loadFontSafe(sf::Font& font, const std::string& path) {
    const std::vector<std::string> prefixes = { "", "../", "../../", "../../../", "../../../../" };
    for (const auto& p : prefixes) {
        if (font.loadFromFile(p + path)) {
            return true;
        }
    }
    return false;
}

MenuAssets::MenuAssets() {
    // Background
    loadTexSafe(cleanBg, "assets/background.png", true) ||
    loadTexSafe(cleanBg, "resources/textures/background.png", true);

    // Book assets
    const std::string bookDir = "assets/Complete_UI_Book_Styles_Pack_Free/Complete_UI_Book_Styles_Pack_Free_v1.0/01_TravelBookLite/Sprites/";
    
    bookLoaded = loadTexSafe(bookCover, bookDir + "UI_TravelBook_BookCover01a.png", false) &&
                 loadTexSafe(bookPageLeft, bookDir + "UI_TravelBook_BookPageLeft01a.png", false) &&
                 loadTexSafe(bookPageRight, bookDir + "UI_TravelBook_BookPageRight01a.png", false);

    loadTexSafe(frameNormal, bookDir + "UI_TravelBook_Frame01a.png", false);
    loadTexSafe(frameHover, bookDir + "UI_TravelBook_FrameSelect01a.png", false);
    loadTexSafe(framePressed, bookDir + "UI_TravelBook_FrameSelect01b.png", false);
    loadTexSafe(marker, bookDir + "UI_TravelBook_Marker01a.png", false);
    loadTexSafe(decLine, bookDir + "UI_TravelBook_Line01a.png", false);

    // Menu icons
    loadTexSafe(iconPlay, bookDir + "UI_TravelBook_IconPlay01a.png", false);
    loadTexSafe(iconLoad, bookDir + "UI_TravelBook_IconRestart01a.png", false);
    loadTexSafe(iconSettings, bookDir + "UI_TravelBook_IconGear01a.png", false);
    loadTexSafe(iconInfo, bookDir + "UI_TravelBook_IconStar01a.png", false);
    loadTexSafe(iconExit, bookDir + "UI_TravelBook_IconCross01a.png", false);
    loadTexSafe(iconArrow, bookDir + "UI_TravelBook_IconArrow01a.png", false);

    // Controls
    loadTexSafe(bar, bookDir + "UI_TravelBook_Bar01a.png", false);
    loadTexSafe(fill, bookDir + "UI_TravelBook_Fill01a.png", false);
    loadTexSafe(handle, bookDir + "UI_TravelBook_Handle03a.png", false);
    loadTexSafe(buttonValue, bookDir + "UI_TravelBook_ButtonValue01a.png", false);
    loadTexSafe(toggleOn, bookDir + "UI_TravelBook_Toggle03a.png", false);
    loadTexSafe(toggleOff, bookDir + "UI_TravelBook_Toggle03b.png", false);
    loadTexSafe(padLB, bookDir + "UI_TravelBook_GamepadLB01a.png", false);
    loadTexSafe(padRB, bookDir + "UI_TravelBook_GamepadRB01a.png", false);
    loadTexSafe(padL, bookDir + "UI_TravelBook_GamepadL01a.png", false);
    loadTexSafe(padB, bookDir + "UI_TravelBook_GamepadB01a.png", false);
    loadTexSafe(padA, bookDir + "UI_TravelBook_GamepadA01a.png", false);
    loadTexSafe(slotBg, bookDir + "UI_TravelBook_Slot01a.png", false);

    // Cursor and background
    loadTexSafe(cursorNormal, bookDir + "UI_TravelBook_Cursor01c.png", false);
    loadTexSafe(cursorPressed, bookDir + "UI_TravelBook_Cursor01d.png", false);
    loadTexSafe(selectCorners, bookDir + "UI_TravelBook_Select01a.png", false);
    loadTexSafe(playBg, "assets/background play.jpg", true) ||
    loadTexSafe(playBg, "resources/textures/background play.jpg", true);

    // Load fonts
    fontVnLoaded = loadFontSafe(fontVn, "assets/font/ViaodaLibre-Regular.ttf") ||
                   loadFontSafe(fontVn, "resources/fonts/ViaodaLibre-Regular.ttf") ||
                   loadFontSafe(fontVn, "assets/font/NomNaTong-Regular.ttf") ||
                   loadFontSafe(fontVn, "resources/fonts/NomNaTong-Regular.ttf") ||
                   loadFontSafe(fontVn, "resources/fonts/arial.ttf") ||
                   loadFontSafe(fontVn, "C:/Windows/Fonts/arial.ttf") ||
                   loadFontSafe(fontVn, "C:/Windows/Fonts/segoeui.ttf") ||
                   loadFontSafe(fontVn, "C:/Windows/Fonts/tahoma.ttf");

    fontVnBoldLoaded = loadFontSafe(fontVnBold, "assets/font/ViaodaLibre-Regular.ttf") ||
                       loadFontSafe(fontVnBold, "resources/fonts/ViaodaLibre-Regular.ttf") ||
                       loadFontSafe(fontVnBold, "C:/Windows/Fonts/arialbd.ttf") ||
                       loadFontSafe(fontVnBold, "resources/fonts/arial.ttf") ||
                       loadFontSafe(fontVnBold, "C:/Windows/Fonts/arial.ttf");

    fontEnLoaded = loadFontSafe(fontEn, "assets/font/ViaodaLibre-Regular.ttf") ||
                   loadFontSafe(fontEn, "resources/fonts/ViaodaLibre-Regular.ttf") ||
                   loadFontSafe(fontEn, "assets/font/PixeloidSans.ttf") ||
                   loadFontSafe(fontEn, "resources/fonts/PixeloidSans.ttf") ||
                   loadFontSafe(fontEn, "resources/fonts/arial.ttf");

    fontEnBoldLoaded = loadFontSafe(fontEnBold, "assets/font/ViaodaLibre-Regular.ttf") ||
                       loadFontSafe(fontEnBold, "resources/fonts/ViaodaLibre-Regular.ttf") ||
                       loadFontSafe(fontEnBold, "assets/font/PixeloidSansBold.ttf") ||
                       loadFontSafe(fontEnBold, "resources/fonts/PixeloidSansBold.ttf") ||
                       loadFontSafe(fontEnBold, "assets/font/PixeloidSans.ttf");

    // Font aliases
    vnFont = fontVn;
    vnFontLoaded = fontVnLoaded;
    fontBold = fontEnBoldLoaded ? fontEnBold : fontVn;
    fontBoldLoaded = fontEnBoldLoaded || fontVnLoaded;
    globalFont = fontBold;
    fontLoaded = fontBoldLoaded;
}

MenuAssets& getMenuAssets() {
    static MenuAssets assets;
    return assets;
}

sf::Font& getMenuFont(bool tiengViet, bool bold) {
    auto& assets = getMenuAssets();
    if (tiengViet) {
        if (bold && assets.fontVnBoldLoaded) return assets.fontVnBold;
        if (assets.fontVnLoaded) return assets.fontVn;
    }
    if (bold && assets.fontEnBoldLoaded) return assets.fontEnBold;
    if (assets.fontEnLoaded) return assets.fontEn;
    if (assets.fontVnLoaded) return assets.fontVn;
    return assets.fontEn;
}

void veNenMenuGiaoDien(sf::RenderWindow& window) {
    auto& assets = getMenuAssets();
    if (assets.cleanBg.getSize().x > 0) {
        sf::Sprite sp(assets.cleanBg);
        sf::Vector2f viewSz = window.getView().getSize();
        sf::Vector2u texSz = assets.cleanBg.getSize();
        sp.setPosition(0.0f, 0.0f);
        sp.setScale(viewSz.x / static_cast<float>(texSz.x), viewSz.y / static_cast<float>(texSz.y));
        window.draw(sp);
    }
}

void ve4GocLuaChon(sf::RenderWindow& window, sf::Vector2f pos, sf::Vector2f sz, float pad, float scale) {
    auto& assets = getMenuAssets();
    if (assets.selectCorners.getSize().x > 0) {
        sf::Sprite sp(assets.selectCorners);
        sp.setScale(scale, scale);
        sp.setColor(sf::Color(220, 160, 50));
        const float cw = 7.0f * scale;
        const float ch = 7.0f * scale;

        // Top-left
        sp.setTextureRect(sf::IntRect(0, 0, 7, 7));
        sp.setPosition(pos.x - pad, pos.y - pad);
        window.draw(sp);

        // Top-right
        sp.setTextureRect(sf::IntRect(23, 0, 7, 7));
        sp.setPosition(pos.x + sz.x + pad - cw, pos.y - pad);
        window.draw(sp);

        // Bottom-left
        sp.setTextureRect(sf::IntRect(0, 23, 7, 7));
        sp.setPosition(pos.x - pad, pos.y + sz.y + pad - ch);
        window.draw(sp);

        // Bottom-right
        sp.setTextureRect(sf::IntRect(23, 23, 7, 7));
        sp.setPosition(pos.x + sz.x + pad - cw, pos.y + sz.y + pad - ch);
        window.draw(sp);
    } else {
        const float len = 10.0f;
        const float th = 2.0f;
        sf::Color col(220, 160, 50);

        sf::RectangleShape h1(sf::Vector2f(len, th)), v1(sf::Vector2f(th, len));
        h1.setPosition(pos.x - pad, pos.y - pad); h1.setFillColor(col); window.draw(h1);
        v1.setPosition(pos.x - pad, pos.y - pad); v1.setFillColor(col); window.draw(v1);

        sf::RectangleShape h2(sf::Vector2f(len, th)), v2(sf::Vector2f(th, len));
        h2.setPosition(pos.x + sz.x + pad - len, pos.y - pad); h2.setFillColor(col); window.draw(h2);
        v2.setPosition(pos.x + sz.x + pad - th, pos.y - pad); v2.setFillColor(col); window.draw(v2);

        sf::RectangleShape h3(sf::Vector2f(len, th)), v3(sf::Vector2f(th, len));
        h3.setPosition(pos.x - pad, pos.y + sz.y + pad - th); h3.setFillColor(col); window.draw(h3);
        v3.setPosition(pos.x - pad, pos.y + sz.y + pad - len); v3.setFillColor(col); window.draw(v3);

        sf::RectangleShape h4(sf::Vector2f(len, th)), v4(sf::Vector2f(th, len));
        h4.setPosition(pos.x + sz.x + pad - len, pos.y + sz.y + pad - th); h4.setFillColor(col); window.draw(h4);
        v4.setPosition(pos.x + sz.x + pad - th, pos.y + sz.y + pad - len); v4.setFillColor(col); window.draw(v4);
    }
}

// Button
void Button::canGiuaText() {
    float btnW = shape.getSize().x;
    float btnH = shape.getSize().y;
    float posX = shape.getPosition().x;
    float posY = shape.getPosition().y;

    bool hasIcon = (texIcon && texIcon->getSize().x > 0);
    float iconAreaW = 0.0f;
    if (hasIcon) {
        float targetH = btnH * 0.52f;
        float iconScale = targetH / static_cast<float>(texIcon->getSize().y);
        float iconW = static_cast<float>(texIcon->getSize().x) * iconScale;
        iconAreaW = 12.0f + iconW + 8.0f; // 12px margin trái + icon width + 8px gap
    }

    float availW = btnW - iconAreaW - 12.0f; // 12px margin phải
    if (availW < 20.0f) availW = 20.0f;

    // Adjust font size
    sf::FloatRect tb = text.getLocalBounds();
    unsigned int curSize = text.getCharacterSize();
    while (tb.width > availW && curSize > 10) {
        curSize--;
        text.setCharacterSize(curSize);
        tb = text.getLocalBounds();
    }

    text.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);

    float centerX = posX + iconAreaW + availW / 2.0f;
    float centerY = posY + btnH / 2.0f + 1.0f;
    text.setPosition(centerX, centerY);
}

Button::Button(float x, float y, float width, float height,
    const std::string& label, sf::Font& font,
    std::function<void()> onClick)
    : callback(onClick), hovered(false), enabled(true), selected(false), texIcon(nullptr) {

    auto& assets = getMenuAssets();
    texNormal = (assets.frameNormal.getSize().x > 0) ? &assets.frameNormal : nullptr;
    texHover = (assets.frameHover.getSize().x > 0) ? &assets.frameHover : nullptr;

    shape.setSize(sf::Vector2f(width, height));
    shape.setPosition(x, y);

    // Button colors
    colorNormal = sf::Color(55, 30, 15);
    colorHover = sf::Color(185, 25, 20);
    colorPressed = sf::Color(130, 20, 15);
    colorDisabled = sf::Color(130, 120, 110);

    shape.setFillColor(sf::Color(238, 226, 210, 230));
    shape.setOutlineThickness(1.2f);
    shape.setOutlineColor(sf::Color(175, 140, 100, 180));

    bool tiengViet = (Loc::getLanguage() == Language::TIENG_VIET);
    text.setFont(getMenuFont(tiengViet, true));
    text.setString(toUtf8(label));
    text.setCharacterSize(18);
    text.setStyle(sf::Text::Bold);
    text.setFillColor(colorNormal);

    canGiuaText();
}

void Button::setPosition(float x, float y) {
    if (shape.getPosition().x == x && shape.getPosition().y == y) return;
    shape.setPosition(x, y);
    canGiuaText();
}

void Button::setFont(const sf::Font& f) {
    text.setFont(f);
    canGiuaText();
}

void Button::setLabel(const std::string& label) {
    sf::String newStr = toUtf8(label);
    if (text.getString() == newStr) return;
    bool tiengViet = (Loc::getLanguage() == Language::TIENG_VIET);
    text.setFont(getMenuFont(tiengViet, true));
    text.setString(newStr);
    canGiuaText();
}

void Button::setCharacterSize(unsigned int size) {
    if (text.getCharacterSize() == size) return;
    text.setCharacterSize(size);
    canGiuaText();
}

void Button::setColors(sf::Color normal, sf::Color hover, sf::Color pressed) {
    colorNormal = normal;
    colorHover = hover;
    colorPressed = pressed;
    if (enabled) {
        text.setFillColor(hovered ? colorHover : (selected ? colorPressed : colorNormal));
    }
}

void Button::setIcon(const sf::Texture* icon) {
    texIcon = icon;
    canGiuaText();
}

void Button::update(const sf::Vector2i& mousePos) {
    if (!enabled) {
        text.setFillColor(colorDisabled);
        hovered = false;
        return;
    }
    bool wasHovered = hovered;
    hovered = contains(mousePos);
    if (hovered || selected) {
        text.setFillColor(colorHover);
        text.setStyle(sf::Text::Bold);
    } else {
        text.setFillColor(colorNormal);
        text.setStyle(sf::Text::Bold);
    }
    if (wasHovered != hovered) {
        canGiuaText();
    }
}

void Button::handleClick(const sf::Vector2i& mousePos) {
    if (!enabled) return;
    if (contains(mousePos) && callback) {
        text.setFillColor(colorPressed);
        callback();
    }
}

void Button::draw(sf::RenderWindow& window) {
    auto& assets = getMenuAssets();
    bool isHighlighted = (hovered || selected) && enabled;

    const sf::Texture* activeTex = texNormal;
    if (isHighlighted && texHover && texHover->getSize().x > 0) {
        activeTex = texHover;
    }

    sf::Vector2f pos = shape.getPosition();
    sf::Vector2f sz = shape.getSize();

    // 1. Draw solid button background with highlighted frame
    sf::RectangleShape drawShape(sz);
    drawShape.setPosition(pos);
    if (isHighlighted) {
        drawShape.setFillColor(sf::Color(255, 245, 230, 255));
        drawShape.setOutlineThickness(2.0f);
        drawShape.setOutlineColor(sf::Color(210, 140, 50, 245));
    } else {
        drawShape.setFillColor(sf::Color(238, 226, 210, 230));
        drawShape.setOutlineThickness(1.2f);
        drawShape.setOutlineColor(sf::Color(175, 140, 100, 180));
    }
    window.draw(drawShape);

    // 2. Overlay texture frame if available
    if (activeTex && activeTex->getSize().x > 0 && enabled) {
        sf::Sprite sp(*activeTex);
        sf::Vector2u texSz = activeTex->getSize();
        float hoverExpand = isHighlighted ? 2.0f : 0.0f;
        sp.setPosition(pos.x - hoverExpand, pos.y - hoverExpand);
        sp.setScale((sz.x + hoverExpand * 2.0f) / static_cast<float>(texSz.x),
                    (sz.y + hoverExpand * 2.0f) / static_cast<float>(texSz.y));
        if (isHighlighted) {
            sp.setColor(sf::Color(255, 245, 225));
        }
        window.draw(sp);
    }

    // 3. Draw icon
    if (texIcon && texIcon->getSize().x > 0 && enabled) {
        sf::Sprite iconSp(*texIcon);
        sf::Vector2u iSz = texIcon->getSize();
        float targetH = shape.getSize().y * 0.54f;
        float iconScale = targetH / static_cast<float>(iSz.y);
        iconSp.setScale(iconScale, iconScale);
        float iconW = static_cast<float>(iSz.x) * iconScale;
        float iconH = static_cast<float>(iSz.y) * iconScale;

        iconSp.setPosition(shape.getPosition().x + 14.0f,
                           shape.getPosition().y + (shape.getSize().y - iconH) / 2.0f);
        if (isHighlighted) {
            iconSp.setColor(sf::Color(190, 35, 25));
        } else {
            iconSp.setColor(sf::Color(85, 55, 30));
        }
        window.draw(iconSp);
    }

    // 4. Draw text
    window.draw(text);

    // 5. 4 Corner brackets on hover or selected
    if (isHighlighted) {
        ve4GocLuaChon(window, shape.getPosition(), shape.getSize(), 2.0f, 1.8f);
    }
}

bool Button::contains(const sf::Vector2i& point) const {
    return shape.getGlobalBounds().contains(static_cast<float>(point.x), static_cast<float>(point.y));
}

// Menu
Menu::Menu()
    : activeSection(MenuSection::NONE),
      pendingSection(MenuSection::PLAY),
      bookProgress(0.0f),
      isBookOpening(false),
      isBookClosing(false),
      isBookClosed(true),
      settingsSubTab(0),
      infoSubTab(0),
      currentSlot(0),
      draggingSlider(-1) {

    auto& assets = getMenuAssets();
    if (assets.vnFontLoaded) {
        font = assets.vnFont;
    } else if (assets.fontLoaded) {
        font = assets.globalFont;
    }

    title.setFont(font);
    title.setCharacterSize(44);
    title.setFillColor(sf::Color::White);
    title.setStyle(sf::Text::Bold);
    animClock.restart();
}

bool Menu::loadFont(const std::string& fontPath) {
    return loadFontSafe(font, fontPath);
}

void Menu::setFont(const sf::Font& f) {
    font = f;
    title.setFont(font);
}

void Menu::setTitle(const std::string& t) {
    title.setString(toUtf8(t));
}

void Menu::themButton(float x, float y, float width, float height,
    const std::string& label, std::function<void()> onClick) {
    buttons.emplace_back(x, y, width, height, label, font, onClick);
}

void Menu::setSection(MenuSection sec) {
    activeSection = sec;
    rightPageIndex = 0;
    capNhatNutTrangPhai();
}

void Menu::triggerOpenAnimation(MenuSection targetSection) {
    pendingSection = targetSection;
    activeSection = targetSection;
    bookProgress = 0.0f;
    isBookOpening = true;
    isBookClosing = false;
    isBookClosed = false;
    animClock.restart();
    capNhatNutTrangPhai();
}

void Menu::triggerCloseAnimation() {
    isBookClosing = true;
    isBookOpening = false;
    animClock.restart();
}

namespace {
int timNutGanNhatTheoHuong(int curIdx, int dirX, int dirY, const std::vector<Button>& btns) {
    if (btns.empty()) return -1;
    if (curIdx < 0 || curIdx >= static_cast<int>(btns.size())) curIdx = 0;

    sf::Vector2f curPos = btns[curIdx].getPosition();
    sf::Vector2f curSz = btns[curIdx].getSize();
    float curCx = curPos.x + curSz.x / 2.0f;
    float curCy = curPos.y + curSz.y / 2.0f;

    int bestIdx = -1;
    float bestScore = 1e9f;

    for (int i = 0; i < static_cast<int>(btns.size()); ++i) {
        if (i == curIdx) continue;
        if (!btns[i].isEnabled()) continue;

        sf::Vector2f pos = btns[i].getPosition();
        sf::Vector2f sz = btns[i].getSize();
        float cx = pos.x + sz.x / 2.0f;
        float cy = pos.y + sz.y / 2.0f;

        float dx = cx - curCx;
        float dy = cy - curCy;

        if (dirX > 0 && dx > 25.0f) { // Sang phải
            float score = dx + std::abs(dy) * 2.5f;
            if (score < bestScore) { bestScore = score; bestIdx = i; }
        } else if (dirX < 0 && dx < -25.0f) { // Sang trái
            float score = -dx + std::abs(dy) * 2.5f;
            if (score < bestScore) { bestScore = score; bestIdx = i; }
        } else if (dirY > 0 && dy > 18.0f) { // Xuống dưới
            float score = dy + std::abs(dx) * 1.5f;
            if (score < bestScore) { bestScore = score; bestIdx = i; }
        } else if (dirY < 0 && dy < -18.0f) { // Lên trên
            float score = -dy + std::abs(dx) * 1.5f;
            if (score < bestScore) { bestScore = score; bestIdx = i; }
        }
    }
    return bestIdx;
}
} // namespace

void Menu::navigateLeft() {
    keyboardNavActive = true;
    if (isBookClosed || bookProgress <= 0.0f) return;
    if (!isFullyOpen()) return;

    if (currentFocus == MenuFocus::SUB_MENU) {
        int nextIdx = timNutGanNhatTheoHuong(rightPageIndex, -1, 0, rightPageButtons);
        if (nextIdx != -1) {
            rightPageIndex = nextIdx;
        } else {
            // Return to main menu
            currentFocus = MenuFocus::MAIN_MENU;
        }
    }
}

void Menu::navigateRight() {
    keyboardNavActive = true;
    if (isBookClosed || bookProgress <= 0.0f) return;
    if (!isFullyOpen()) return;

    if (currentFocus == MenuFocus::MAIN_MENU) {
        currentFocus = MenuFocus::SUB_MENU;
        if (rightPageIndex < 0 || rightPageIndex >= static_cast<int>(rightPageButtons.size())) {
            rightPageIndex = 0;
        }
    } else {
        int nextIdx = timNutGanNhatTheoHuong(rightPageIndex, 1, 0, rightPageButtons);
        if (nextIdx != -1) {
            rightPageIndex = nextIdx;
        }
    }
}

void Menu::navigateUp() {
    keyboardNavActive = true;
    // Navigate up
    if (isBookClosed || bookProgress <= 0.0f) {
        coverIndex = (coverIndex - 1 + 5) % 5;
        return;
    }
    if (!isFullyOpen()) return;

    if (currentFocus == MenuFocus::MAIN_MENU) {
        int cur = static_cast<int>(activeSection);
        setSection(static_cast<MenuSection>((cur - 1 + 5) % 5));
    } else {
        int nextIdx = timNutGanNhatTheoHuong(rightPageIndex, 0, -1, rightPageButtons);
        if (nextIdx != -1) {
            rightPageIndex = nextIdx;
        }
    }
}

void Menu::navigateDown() {
    keyboardNavActive = true;
    // Navigate down
    if (isBookClosed || bookProgress <= 0.0f) {
        coverIndex = (coverIndex + 1) % 5;
        return;
    }
    if (!isFullyOpen()) return;

    if (currentFocus == MenuFocus::MAIN_MENU) {
        int cur = static_cast<int>(activeSection);
        setSection(static_cast<MenuSection>((cur + 1) % 5));
    } else {
        int nextIdx = timNutGanNhatTheoHuong(rightPageIndex, 0, 1, rightPageButtons);
        if (nextIdx != -1) {
            rightPageIndex = nextIdx;
        }
    }
}

void Menu::activateCurrent() {
    keyboardNavActive = true;
    // Open section
    if (isBookClosed || bookProgress <= 0.0f) {
        triggerOpenAnimation(static_cast<MenuSection>(coverIndex));
        return;
    }
    if (!isFullyOpen()) {
        triggerOpenAnimation(MenuSection::PLAY);
        return;
    }
    if (currentFocus == MenuFocus::MAIN_MENU) {
        currentFocus = MenuFocus::SUB_MENU;
        rightPageIndex = 0;
    } else {
        if (rightPageIndex >= 0 && rightPageIndex < static_cast<int>(rightPageButtons.size())) {
            rightPageButtons[rightPageIndex].triggerClick();
        }
    }
}

void Menu::capNhatNutTrangPhai() {
    rightPageButtons.clear();
    auto& assets = getMenuAssets();
    bool tiengViet = (Loc::getLanguage() == Language::TIENG_VIET);
    sf::Font& f = getMenuFont(tiengViet, true);

    const float rx = 1042.0f;
    const float rw = 390.0f;

    // Play section
    if (activeSection == MenuSection::PLAY) {
        // PvP button
        rightPageButtons.emplace_back(rx, 320.0f, rw, 64.0f, tiengViet ? "Đấu hai người (PvP)" : "Two Players (PvP)", f, [this]() {
            if (onPlayPvp) onPlayPvp();
        });
        rightPageButtons.back().setIcon(&assets.iconPlay);
        rightPageButtons.back().setCharacterSize(19);

        // PvAI button
        rightPageButtons.emplace_back(rx, 410.0f, rw, 64.0f, tiengViet ? "Đấu với máy (PvAI)" : "Play vs AI (PvAI)", f, [this]() {
            if (onPlayPvai) onPlayPvai();
        });
        rightPageButtons.back().setIcon(&assets.iconLoad);
        rightPageButtons.back().setCharacterSize(19);

        // Demo button
        rightPageButtons.emplace_back(rx, 500.0f, rw, 64.0f, tiengViet ? "Xem máy đánh (Demo)" : "Bot vs Bot (Demo)", f, [this]() {
            if (onPlayDemo) onPlayDemo();
        });
        rightPageButtons.back().setIcon(&assets.iconInfo);
        rightPageButtons.back().setCharacterSize(19);
    }
    // Load section
    else if (activeSection == MenuSection::LOAD) {
        // Save slots
        for (int i = 0; i < 3; ++i) {
            float slotY = 315.0f + i * 90.0f;
            rightPageButtons.emplace_back(rx, slotY, rw, 78.0f, "", f, [this, i]() {
                currentSlot = i;
                if (onSelectSlot) onSelectSlot(i);
                capNhatNutTrangPhai();
            });
            if (currentSlot == i) {
                rightPageButtons.back().setSelected(true);
            }
        }

        // Load game button
        rightPageButtons.emplace_back(rx, 595.0f, 188.0f, 48.0f, tiengViet ? "Vào ván đấu" : "Load Game", f, [this]() {
            if (onLoadSlot) onLoadSlot(currentSlot);
        });
        rightPageButtons.back().setIcon(&assets.iconPlay);
        rightPageButtons.back().setCharacterSize(16);

        // Delete save button
        rightPageButtons.emplace_back(rx + 202.0f, 595.0f, 188.0f, 48.0f, tiengViet ? "Xóa bản lưu" : "Delete Save", f, [this]() {
            if (onDeleteSlot) onDeleteSlot(currentSlot);
            capNhatNutTrangPhai();
        });
        rightPageButtons.back().setIcon(&assets.iconExit);
        rightPageButtons.back().setCharacterSize(16);
    }
    // Settings section
    else if (activeSection == MenuSection::SETTINGS) {
        // Sub-tabs
        const std::vector<std::string> tabNames = tiengViet
            ? std::vector<std::string>{ "Âm thanh", "Đồ họa", "Trò chơi", "Phím" }
            : std::vector<std::string>{ "Audio", "Graphics", "Gameplay", "Controls" };
        for (int i = 0; i < 4; ++i) {
            float tx = rx + i * 98.0f;
            rightPageButtons.emplace_back(tx, 292.0f, 94.0f, 32.0f, tabNames[i], f, [this, i]() {
                settingsSubTab = i;
                capNhatNutTrangPhai();
            });
            rightPageButtons.back().setCharacterSize(15);
            if (settingsSubTab == i) {
                rightPageButtons.back().setSelected(true);
            }
        }

        // Tab content
        if (settingsSubTab == 0) { // Âm thanh (Audio)
            // Test sound
            rightPageButtons.emplace_back(rx, 570.0f, rw, 40.0f, tiengViet ? "Nghe thử tiếng gõ cờ" : "Test Chess Click Sound", f, [this]() {
                if (onTestSound) onTestSound();
            });
        }
        else if (settingsSubTab == 1) { // Đồ họa
            // Board theme
            std::string themeStr = getSettingStatusStr ? getSettingStatusStr(3) : (tiengViet ? "Gỗ Cổ Điển" : "Classic Wood");
            rightPageButtons.emplace_back(rx, 340.0f, rw, 44.0f, (tiengViet ? "Bàn cờ: " : "Board: ") + themeStr, f, [this]() {
                if (onToggleBoardTheme) onToggleBoardTheme();
                capNhatNutTrangPhai();
            });

            // Atmosphere
            std::string atmStr = getSettingStatusStr ? getSettingStatusStr(5) : (tiengViet ? "Lá Trúc Xanh" : "Bamboo Leaves");
            rightPageButtons.emplace_back(rx, 410.0f, rw, 44.0f, (tiengViet ? "Khí quyển: " : "Atmosphere: ") + atmStr, f, [this]() {
                if (onToggleAtmosphere) onToggleAtmosphere();
                capNhatNutTrangPhai();
            });

            // Window mode
            std::string winStr = getSettingStatusStr ? getSettingStatusStr(6) : (tiengViet ? "Toàn màn hình" : "Fullscreen");
            rightPageButtons.emplace_back(rx, 480.0f, rw, 44.0f, (tiengViet ? "Màn hình: " : "Display: ") + winStr, f, [this]() {
                if (onToggleWindowMode) onToggleWindowMode();
                capNhatNutTrangPhai();
            });
        }
        else if (settingsSubTab == 2) { // Trò chơi
            // Language
            std::string langStr = getSettingStatusStr ? getSettingStatusStr(7) : (tiengViet ? "Tiếng Việt" : "English");
            rightPageButtons.emplace_back(rx, 340.0f, rw, 42.0f, (tiengViet ? "Ngôn ngữ: " : "Language: ") + langStr, f, [this]() {
                if (onToggleLang) onToggleLang();
                capNhatNutTrangPhai();
            });
            // AI delay
            std::string aiStr = getSettingStatusStr ? getSettingStatusStr(8) : (tiengViet ? "Chân thực (450ms)" : "Realistic (450ms)");
            rightPageButtons.emplace_back(rx, 395.0f, rw, 42.0f, (tiengViet ? "Mô phỏng AI: " : "AI Thinking: ") + aiStr, f, [this]() {
                if (onToggleAiDelay) onToggleAiDelay();
                capNhatNutTrangPhai();
            });
            // Check alarm
            std::string chkStr = getSettingStatusStr ? getSettingStatusStr(9) : (tiengViet ? "BẬT" : "ON");
            rightPageButtons.emplace_back(rx, 450.0f, rw, 42.0f, (tiengViet ? "Báo chiếu tướng: " : "Check Alarm: ") + chkStr, f, [this]() {
                if (onToggleCheckAlarm) onToggleCheckAlarm();
                capNhatNutTrangPhai();
            });
        }
        else if (settingsSubTab == 3) { // Phím
            rightPageButtons.emplace_back(rx, 540.0f, 190.0f, 44.0f, tiengViet ? "Bộ phím WASD" : "WASD Preset", f, [this]() {
                if (onPresetWasd) onPresetWasd();
            });
            rightPageButtons.emplace_back(rx + 200.0f, 540.0f, 190.0f, 44.0f, tiengViet ? "Bộ phím Mũi tên" : "Arrow Keys Preset", f, [this]() {
                if (onPresetArrows) onPresetArrows();
            });
        }

        // Action buttons
        rightPageButtons.emplace_back(rx, 624.0f, 188.0f, 44.0f, tiengViet ? "Áp dụng & Lưu" : "Apply & Save", f, [this]() {
            if (onApplySettings) onApplySettings();
        });
        rightPageButtons.back().setCharacterSize(16);
        rightPageButtons.emplace_back(rx + 202.0f, 624.0f, 188.0f, 44.0f, tiengViet ? "Khôi phục" : "Restore", f, [this]() {
            if (onRestoreDefaults) onRestoreDefaults();
            capNhatNutTrangPhai();
        });
        rightPageButtons.back().setCharacterSize(16);

        // Button styling
        for (auto& btn : rightPageButtons) {
            btn.setColors(sf::Color(65, 38, 20), sf::Color(190, 35, 25), sf::Color(120, 20, 15));
            btn.setShapeColor(sf::Color(248, 238, 224, 230));
            btn.setOutline(1.0f, sf::Color(175, 140, 100));
        }
    }
    // Info section
    else if (activeSection == MenuSection::INFO) {
        // Info tabs
        rightPageButtons.emplace_back(rx, 292.0f, 188.0f, 32.0f, tiengViet ? "Kiến trúc OOP" : "OOP Architecture", f, [this]() {
            infoSubTab = 0;
            capNhatNutTrangPhai();
        });
        rightPageButtons.back().setCharacterSize(15);
        if (infoSubTab == 0) rightPageButtons.back().setSelected(true);

        rightPageButtons.emplace_back(rx + 202.0f, 292.0f, 188.0f, 32.0f, tiengViet ? "Luật 7 Quân Cờ" : "7 Pieces Rules", f, [this]() {
            infoSubTab = 1;
            capNhatNutTrangPhai();
        });
        rightPageButtons.back().setCharacterSize(15);
        if (infoSubTab == 1) rightPageButtons.back().setSelected(true);
    }
    // Exit section
    else if (activeSection == MenuSection::EXIT) {
        // Confirm exit
        rightPageButtons.emplace_back(rx, 520.0f, 188.0f, 48.0f, tiengViet ? "Thoát Game" : "Exit Game", f, [this]() {
            if (onConfirmExit) onConfirmExit();
        });
        rightPageButtons.back().setIcon(&assets.iconExit);
        rightPageButtons.back().setCharacterSize(16);

        // Cancel exit
        rightPageButtons.emplace_back(rx + 202.0f, 520.0f, 188.0f, 48.0f, tiengViet ? "Ở lại chơi" : "Stay & Play", f, [this]() {
            if (onReturnToGame) onReturnToGame();
            else setSection(MenuSection::PLAY);
        });
        rightPageButtons.back().setIcon(&assets.iconPlay);
        rightPageButtons.back().setCharacterSize(16);
    }
}

void Menu::update(const sf::Vector2i& mousePos) {
    bool mouseMoved = (mousePos != mousePosCache);
    mousePosCache = mousePos;

    // Drag slider
    if (draggingSlider != -1 && isFullyOpen() && activeSection == MenuSection::SETTINGS && settingsSubTab == 0) {
        float val = std::clamp((static_cast<float>(mousePos.x) - 1202.0f) / 135.0f * 100.0f, 0.0f, 100.0f);
        if (draggingSlider == 0 && onSetMasterVolume) onSetMasterVolume(val);
        else if (draggingSlider == 1 && onSetBgmVolume) onSetBgmVolume(val);
        else if (draggingSlider == 2 && onSetSfxVolume) onSetSfxVolume(val);
    }

    if (laMenuChinh) {
        float dt = animClock.restart().asSeconds();
        if (dt > 0.1f) dt = 0.1f;

        if (isBookOpening) {
            bookProgress += dt * 2.5f; // Open animation
            if (bookProgress >= 1.0f) {
                bookProgress = 1.0f;
                isBookOpening = false;
                isBookClosed = false;
            }
        } else if (isBookClosing) {
            bookProgress -= dt * 2.5f; // Close animation
            if (bookProgress <= 0.0f) {
                bookProgress = 0.0f;
                isBookClosing = false;
                isBookClosed = true;
            }
        }
    }

    if (!laMenuChinh) {
        for (auto& button : buttons) button.update(mousePos);
        return;
    }

    // Update cover buttons
    if (isBookClosed) {
        for (size_t i = 0; i < buttons.size() && i < 5; ++i) {
            buttons[i].update(mousePos);
            if (buttons[i].isHovered()) {
                coverIndex = static_cast<int>(i);
                keyboardNavActive = false;
            }
            buttons[i].setSelected(keyboardNavActive && static_cast<int>(i) == coverIndex);
        }
    }
    // Update page buttons
    else if (bookProgress >= 0.7f) {
        for (size_t i = 0; i < buttons.size(); ++i) {
            buttons[i].update(mousePos);
            if (buttons[i].isHovered()) {
                currentFocus = MenuFocus::MAIN_MENU;
                keyboardNavActive = false;
            }
            buttons[i].setSelected(currentFocus == MenuFocus::MAIN_MENU && static_cast<int>(activeSection) == static_cast<int>(i));
        }

        int hoveredSubIdx = -1;
        for (size_t i = 0; i < rightPageButtons.size(); ++i) {
            rightPageButtons[i].update(mousePos);
            if (rightPageButtons[i].isHovered()) {
                hoveredSubIdx = static_cast<int>(i);
            }
        }

        if (hoveredSubIdx != -1) {
            currentFocus = MenuFocus::SUB_MENU;
            rightPageIndex = hoveredSubIdx;
            keyboardNavActive = false;
        } else if (mouseMoved) {
            // Clear hover
            keyboardNavActive = false;
        }

        for (size_t i = 0; i < rightPageButtons.size(); ++i) {
            bool isKbd = (keyboardNavActive && currentFocus == MenuFocus::SUB_MENU && static_cast<int>(i) == rightPageIndex);
            rightPageButtons[i].setSelected(isKbd);
        }
    }
}

void Menu::handleMouseDown(const sf::Vector2i& mousePos) {
    if (!isFullyOpen()) return;
    if (activeSection == MenuSection::SETTINGS && settingsSubTab == 0) {
        // Master slider
        if (mousePos.x >= 1192 && mousePos.x <= 1347 && mousePos.y >= 344 && mousePos.y <= 370) {
            draggingSlider = 0;
            float val = std::clamp((static_cast<float>(mousePos.x) - 1202.0f) / 135.0f * 100.0f, 0.0f, 100.0f);
            if (onSetMasterVolume) onSetMasterVolume(val);
            return;
        }
        // BGM slider
        if (mousePos.x >= 1192 && mousePos.x <= 1347 && mousePos.y >= 404 && mousePos.y <= 430) {
            draggingSlider = 1;
            float val = std::clamp((static_cast<float>(mousePos.x) - 1202.0f) / 135.0f * 100.0f, 0.0f, 100.0f);
            if (onSetBgmVolume) onSetBgmVolume(val);
            return;
        }
        // SFX slider
        if (mousePos.x >= 1192 && mousePos.x <= 1347 && mousePos.y >= 464 && mousePos.y <= 490) {
            draggingSlider = 2;
            float val = std::clamp((static_cast<float>(mousePos.x) - 1202.0f) / 135.0f * 100.0f, 0.0f, 100.0f);
            if (onSetSfxVolume) onSetSfxVolume(val);
            return;
        }
    }
}

void Menu::handleMouseUp(const sf::Vector2i& /*mousePos*/) {
    draggingSlider = -1;
}

void Menu::handleClick(const sf::Vector2i& mousePos) {
    keyboardNavActive = false;
    if (laMenuChinh) {
        if (isBookOpening) {
            bookProgress = 1.0f;
            isBookOpening = false;
            isBookClosed = false;
            return;
        }
        if (isBookClosing) {
            bookProgress = 0.0f;
            isBookClosing = false;
            isBookClosed = true;
            return;
        }

        // Closed book click
        if (isBookClosed) {
            for (size_t i = 0; i < buttons.size() && i < 5; ++i) {
                if (buttons[i].contains(mousePos)) {
                    triggerOpenAnimation(static_cast<MenuSection>(i));
                    return;
                }
            }

            // Open book
            float coverW = 112.0f * 4.6f;
            float cx = (1920.0f - coverW) / 2.0f;
            sf::FloatRect closedRect(cx, 190.0f, coverW, 736.0f);
            if (closedRect.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                triggerOpenAnimation(MenuSection::PLAY);
                return;
            }
            return;
        }

        // Open book
        // Back tab
        sf::FloatRect backTabRect(482.0f, 186.0f, 68.0f, 48.0f);
        if (backTabRect.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            if (onReturnToGame) {
                onReturnToGame();
                return;
            }
            triggerCloseAnimation();
            return;
        }

        // Top tabs
        for (int i = 0; i < 5; ++i) {
            float tabX = 560.0f + i * 55.0f;
            if (sf::FloatRect(tabX, 185.0f, 48.0f, 40.0f).contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                currentFocus = MenuFocus::MAIN_MENU;
                setSection(static_cast<MenuSection>(i));
                return;
            }
        }

        // Left page buttons
        for (int i = 0; i < 5; ++i) {
            float by = 310.0f + i * 66.0f;
            if (sf::FloatRect(535.0f, by, 380.0f, 50.0f).contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                currentFocus = MenuFocus::MAIN_MENU;
                setSection(static_cast<MenuSection>(i));
                return;
            }
        }

        // Audio controls
        if (activeSection == MenuSection::SETTINGS && settingsSubTab == 0) {
            currentFocus = MenuFocus::SUB_MENU;
            // Master Toggle Switch
            if (sf::FloatRect(1392.0f, 340.0f, 54.0f, 34.0f).contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                if (onToggleMasterSound) onToggleMasterSound();
                return;
            }
            // BGM Toggle Switch
            if (sf::FloatRect(1392.0f, 400.0f, 54.0f, 34.0f).contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                if (onToggleBgm) onToggleBgm();
                return;
            }
            // SFX Toggle Switch
            if (sf::FloatRect(1392.0f, 460.0f, 54.0f, 34.0f).contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                if (onToggleSfx) onToggleSfx();
                return;
            }
            // Check alarm Toggle Switch
            if (sf::FloatRect(1392.0f, 520.0f, 54.0f, 34.0f).contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                if (onToggleCheckAlarm) onToggleCheckAlarm();
                return;
            }
            // Click trực tiếp trên Slider Master
            if (sf::FloatRect(1192.0f, 344.0f, 150.0f, 26.0f).contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                float val = std::clamp((static_cast<float>(mousePos.x) - 1202.0f) / 135.0f * 100.0f, 0.0f, 100.0f);
                if (onSetMasterVolume) onSetMasterVolume(val);
                return;
            }
            // Click trực tiếp trên Slider BGM
            if (sf::FloatRect(1192.0f, 404.0f, 150.0f, 26.0f).contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                float val = std::clamp((static_cast<float>(mousePos.x) - 1202.0f) / 135.0f * 100.0f, 0.0f, 100.0f);
                if (onSetBgmVolume) onSetBgmVolume(val);
                return;
            }
            // Click trực tiếp trên Slider SFX
            if (sf::FloatRect(1192.0f, 464.0f, 150.0f, 26.0f).contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                float val = std::clamp((static_cast<float>(mousePos.x) - 1202.0f) / 135.0f * 100.0f, 0.0f, 100.0f);
                if (onSetSfxVolume) onSetSfxVolume(val);
                return;
            }
        }

        // Right page buttons
        for (size_t i = 0; i < rightPageButtons.size(); ++i) {
            if (rightPageButtons[i].contains(mousePos)) {
                currentFocus = MenuFocus::SUB_MENU;
                rightPageIndex = static_cast<int>(i);
                rightPageButtons[i].handleClick(mousePos);
                break;
            }
        }
        return;
    }

    for (auto& button : buttons) {
        button.handleClick(mousePos);
    }
}

void Menu::draw(sf::RenderWindow& window, bool tiengViet) {
    auto& assets = getMenuAssets();
    sf::Font& f = getMenuFont(tiengViet, true);
    sf::Font& fRegular = getMenuFont(tiengViet, false);
    sf::Font& fBold = f;

    // Background toàn màn hình
    veNenMenuGiaoDien(window);

    if (!laMenuChinh) {
        for (auto& button : buttons) {
            button.draw(window);
        }
        return;
    }

    // Book menu

    // Dark overlay
    sf::RectangleShape darkVignette(sf::Vector2f(1920.0f, 1080.0f));
    darkVignette.setFillColor(sf::Color(10, 8, 15, 65));
    window.draw(darkVignette);

    // Title above book
    const float titleW = 720.0f;
    const float titleH = 88.0f;
    const float titleX = (1920.0f - titleW) / 2.0f; // 600px
    const float titleY = 55.0f;                     // Title position

    // Title frame
    if (assets.frameNormal.getSize().x > 0) {
        sf::Sprite tFrame(assets.frameNormal);
        sf::Vector2u fs = assets.frameNormal.getSize();
        tFrame.setPosition(titleX, titleY);
        tFrame.setScale(titleW / static_cast<float>(fs.x), titleH / static_cast<float>(fs.y));
        window.draw(tFrame);
    } else {
        sf::RectangleShape tPlate(sf::Vector2f(titleW, titleH));
        tPlate.setPosition(titleX, titleY);
        tPlate.setFillColor(sf::Color(25, 18, 14, 235));
        tPlate.setOutlineThickness(2.5f);
        tPlate.setOutlineColor(sf::Color(230, 185, 75));
        window.draw(tPlate);
    }

    // Title text
    sf::Text gameTitleShadow;
    gameTitleShadow.setFont(fBold);
    gameTitleShadow.setString(toUtf8(tiengViet ? "CỜ TƯỚNG" : "CHINESE CHESS"));
    gameTitleShadow.setCharacterSize(42);
    gameTitleShadow.setStyle(sf::Text::Bold);
    gameTitleShadow.setFillColor(sf::Color(15, 8, 5, 200));
    sf::FloatRect gtsb = gameTitleShadow.getLocalBounds();
    gameTitleShadow.setOrigin(gtsb.left + gtsb.width / 2.0f, gtsb.top + gtsb.height / 2.0f);
    gameTitleShadow.setPosition(962.0f, titleY + 34.0f);
    window.draw(gameTitleShadow);

    sf::Text gameTitle;
    gameTitle.setFont(fBold);
    gameTitle.setString(toUtf8(tiengViet ? "CỜ TƯỚNG" : "CHINESE CHESS"));
    gameTitle.setCharacterSize(42);
    gameTitle.setStyle(sf::Text::Bold);
    gameTitle.setFillColor(sf::Color(255, 220, 100)); // Màu vàng hoàng gia
    sf::FloatRect gtb = gameTitle.getLocalBounds();
    gameTitle.setOrigin(gtb.left + gtb.width / 2.0f, gtb.top + gtb.height / 2.0f);
    gameTitle.setPosition(960.0f, titleY + 32.0f);
    window.draw(gameTitle);

    // Subtitle
    sf::Text gameSub;
    gameSub.setFont(f);
    gameSub.setString(toUtf8(tiengViet ? "TƯỢNG KỲ KỲ PHỔ • ĐỒ ÁN OOP HCMUS 2026" : "ANCIENT XIANGQI • HCMUS OOP 2026"));
    gameSub.setCharacterSize(16);
    gameSub.setStyle(sf::Text::Bold);
    gameSub.setFillColor(sf::Color(240, 200, 155));
    sf::FloatRect gsbb = gameSub.getLocalBounds();
    gameSub.setOrigin(gsbb.left + gsbb.width / 2.0f, gsbb.top + gsbb.height / 2.0f);
    gameSub.setPosition(960.0f, titleY + 66.0f);
    window.draw(gameSub);

    // Draw book
    const float scale = 4.6f;
    const float bookY = 190.0f;

    // Book shadow
    float shadowW = 1040.0f * bookProgress + 530.0f * (1.0f - bookProgress);
    float shadowX = (1920.0f - shadowW) / 2.0f;
    sf::RectangleShape shadow(sf::Vector2f(shadowW, 736.0f));
    shadow.setPosition(shadowX + 12.0f, bookY + 12.0f);
    shadow.setFillColor(sf::Color(0, 0, 0, 95));
    window.draw(shadow);

    // Closed book
    if (isBookClosed && bookProgress <= 0.0f) {
        float coverW = 112.0f * scale; // 515.2px
        float cx = (1920.0f - coverW) / 2.0f; // 702.4px
        float cy = bookY;

        // Book cover
        if (assets.bookCover.getSize().x > 0) {
            sf::Sprite closedCover(assets.bookCover);
            closedCover.setTextureRect(sf::IntRect(112, 0, 112, 160));
            closedCover.setPosition(cx, cy);
            closedCover.setScale(scale, scale);
            window.draw(closedCover);
        }

        // Cover buttons
        float btnW = 370.0f;
        float btnH = 58.0f;
        float btnX = cx + (coverW - btnW) / 2.0f;
        float startBtnY = cy + 125.0f;
        float btnSpacing = 76.0f;

        const std::vector<std::string> coverBtnLabels = {
            tiengViet ? "CHƠI MỚI" : "PLAY",
            tiengViet ? "TIẾP TỤC" : "LOAD",
            tiengViet ? "CÀI ĐẶT" : "SETTINGS",
            tiengViet ? "THÔNG TIN" : "INFORMATION",
            tiengViet ? "THOÁT" : "EXIT"
        };

        for (size_t i = 0; i < buttons.size() && i < 5; ++i) {
            buttons[i].setPosition(btnX, startBtnY + i * btnSpacing);
            buttons[i].setLabel(coverBtnLabels[i]);
            buttons[i].setSelected(keyboardNavActive && static_cast<int>(i) == coverIndex);
            buttons[i].draw(window);
        }

        // Control prompts
        sf::Text fCoverText;
        fCoverText.setFont(fBold);
        fCoverText.setString(toUtf8(tiengViet ? "W / S: Di chuyển chọn mục  •  Enter: Mở sách" : "W / S: Navigate  •  Enter: Open Book"));
        fCoverText.setCharacterSize(15);
        fCoverText.setStyle(sf::Text::Bold);
        fCoverText.setFillColor(sf::Color(255, 230, 180));
        sf::FloatRect ctb = fCoverText.getLocalBounds();
        fCoverText.setOrigin(ctb.left + ctb.width / 2.0f, ctb.top + ctb.height / 2.0f);
        fCoverText.setPosition(cx + coverW / 2.0f, cy + 540.0f);
        window.draw(fCoverText);
    }
    // Book animation
    else if (isBookOpening || isBookClosing) {
        float t = 1.0f - std::pow(1.0f - bookProgress, 3.0f);
        float cx = 960.0f;
        float openHalfW = 112.0f * scale * t;

        // Left cover
        if (assets.bookCover.getSize().x > 0) {
            sf::Sprite leftCover(assets.bookCover);
            leftCover.setTextureRect(sf::IntRect(0, 0, 112, 160));
            leftCover.setPosition(cx - openHalfW, bookY);
            leftCover.setScale((openHalfW / 112.0f), scale);
            window.draw(leftCover);
        }

        // Right cover
        if (assets.bookCover.getSize().x > 0) {
            sf::Sprite rightCover(assets.bookCover);
            rightCover.setTextureRect(sf::IntRect(112, 0, 112, 160));
            rightCover.setPosition(cx, bookY);
            rightCover.setScale((openHalfW / 112.0f), scale);
            window.draw(rightCover);
        }

        // Open pages
        if (t > 0.08f) {
            if (assets.bookPageLeft.getSize().x > 0) {
                sf::Sprite spLeft(assets.bookPageLeft);
                float pw = 104.0f * scale * t;
                spLeft.setPosition(cx - pw, bookY + 6.5f * scale);
                spLeft.setScale((pw / 104.0f), scale);
                window.draw(spLeft);
            }
            if (assets.bookPageRight.getSize().x > 0) {
                sf::Sprite spRight(assets.bookPageRight);
                float pw = 104.0f * scale * t;
                spRight.setPosition(cx, bookY + 6.5f * scale);
                spRight.setScale((pw / 104.0f), scale);
                window.draw(spRight);
            }
        }

        // Book spine
        sf::RectangleShape spine(sf::Vector2f(28.0f * t, 736.0f));
        spine.setOrigin(14.0f * t, 0.0f);
        spine.setPosition(cx, bookY);
        spine.setFillColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>(130.0f * (1.0f - t))));
        window.draw(spine);
    }
    // Open book
    else {
        const float bookX = 445.0f; // Centered

        // Cover background
        if (assets.bookCover.getSize().x > 0) {
            sf::Sprite spCover(assets.bookCover);
            spCover.setPosition(bookX, bookY);
            spCover.setScale(scale, scale);
            window.draw(spCover);
        }

        // Left page
        if (assets.bookPageLeft.getSize().x > 0) {
            sf::Sprite spLeft(assets.bookPageLeft);
            spLeft.setPosition(bookX + 8.0f * scale, bookY + 6.5f * scale); // (481.8, 219.9)
            spLeft.setScale(scale, scale);
            window.draw(spLeft);
        }

        // Right page
        if (assets.bookPageRight.getSize().x > 0) {
            sf::Sprite spRight(assets.bookPageRight);
            spRight.setPosition(bookX + (8.0f + 104.0f + 8.0f) * scale, bookY + 6.5f * scale); // (997.8, 219.9)
            spRight.setScale(scale, scale);
            window.draw(spRight);
        }

        // Book spine
        sf::RectangleShape spineOpen(sf::Vector2f(20.0f, 736.0f));
        spineOpen.setOrigin(10.0f, 0.0f);
        spineOpen.setPosition(960.0f, bookY);
        spineOpen.setFillColor(sf::Color(0, 0, 0, 40));
        window.draw(spineOpen);

        // Ribbon
        if (assets.marker.getSize().x > 0) {
            sf::Sprite spMarker(assets.marker);
            spMarker.setPosition(482.0f, 186.0f);
            spMarker.setScale(3.4f, 3.4f);
            window.draw(spMarker);
        }

    // Top tabs
        // Back tab
        sf::FloatRect backTabRect(482.0f, 186.0f, 68.0f, 48.0f);
        bool hoverBackTab = backTabRect.contains(static_cast<float>(mousePosCache.x), static_cast<float>(mousePosCache.y));

        if (assets.iconArrow.getSize().x > 0) {
            sf::Sprite spArrow(assets.iconArrow);
            sf::Vector2u asz = assets.iconArrow.getSize();
            spArrow.setOrigin(asz.x / 2.0f, asz.y / 2.0f);
            spArrow.setPosition(516.0f, 210.0f);
            spArrow.setScale(-1.8f, 1.8f); // Đảo chiều sprite để mũi tên chỉ về bên TRÁI (<-)
            spArrow.setColor(hoverBackTab ? sf::Color(220, 35, 25) : sf::Color(90, 50, 30, 230));
            window.draw(spArrow);
        } else {
            sf::Text backArrow;
            backArrow.setFont(f);
            backArrow.setString(toUtf8("◄"));
            backArrow.setCharacterSize(18);
            backArrow.setStyle(sf::Text::Bold);
            backArrow.setFillColor(hoverBackTab ? sf::Color(210, 35, 25) : sf::Color(85, 50, 30, 220));
            sf::FloatRect bab = backArrow.getLocalBounds();
            backArrow.setOrigin(bab.left + bab.width / 2.0f, bab.top + bab.height / 2.0f);
            backArrow.setPosition(516.0f, 210.0f);
            window.draw(backArrow);
        }

        if (hoverBackTab) {
            sf::RectangleShape tabHighlight(sf::Vector2f(68.0f, 48.0f));
            tabHighlight.setPosition(482.0f, 186.0f);
            tabHighlight.setFillColor(sf::Color(255, 235, 170, 45));
            tabHighlight.setOutlineThickness(2.0f);
            tabHighlight.setOutlineColor(sf::Color(220, 185, 80));
            window.draw(tabHighlight);


            sf::RectangleShape tipBg(sf::Vector2f(105.0f, 26.0f));
            tipBg.setPosition(465.0f, 150.0f);
            tipBg.setFillColor(sf::Color(25, 18, 14, 230));
            tipBg.setOutlineThickness(1.0f);
            tipBg.setOutlineColor(sf::Color(220, 180, 80));
            window.draw(tipBg);

            sf::Text tipText;
            tipText.setFont(fBold);
            tipText.setString(toUtf8(tiengViet ? "Quay về bìa" : "Back to Cover"));
            tipText.setCharacterSize(14);
            tipText.setStyle(sf::Text::Bold);
            tipText.setFillColor(sf::Color(255, 230, 120));
            sf::FloatRect ttb = tipText.getLocalBounds();
            tipText.setOrigin(ttb.left + ttb.width / 2.0f, ttb.top + ttb.height / 2.0f);
            tipText.setPosition(516.0f, 163.0f);
            window.draw(tipText);
        }

        // Section tabs
        const sf::Texture* topTabIcons[5] = {
            &assets.iconPlay, &assets.iconLoad, &assets.iconSettings, &assets.iconInfo, &assets.iconExit
        };

        for (int i = 0; i < 5; ++i) {
            float tabX = 560.0f + i * 55.0f;
            bool isTabActive = (static_cast<int>(activeSection) == i);
            float tabY = isTabActive ? 186.0f : 192.0f;
            float tabW = 48.0f;
            float tabH = isTabActive ? 36.0f : 30.0f;

            // Nền tab
            sf::RectangleShape tabBox(sf::Vector2f(tabW, tabH));
            tabBox.setPosition(tabX, tabY);
            tabBox.setFillColor(isTabActive ? sf::Color(252, 242, 226) : sf::Color(230, 215, 192, 220));
            tabBox.setOutlineThickness(1.0f);
            tabBox.setOutlineColor(isTabActive ? sf::Color(190, 140, 75) : sf::Color(165, 135, 100));
            window.draw(tabBox);

            // Active tab frame
            if (isTabActive && assets.frameHover.getSize().x > 0) {
                sf::Sprite fHover(assets.frameHover);
                fHover.setPosition(tabX - 2.0f, tabY - 2.0f);
                fHover.setScale((tabW + 4.0f) / static_cast<float>(assets.frameHover.getSize().x),
                                (tabH + 4.0f) / static_cast<float>(assets.frameHover.getSize().y));
                window.draw(fHover);
            }

            // Hover nếu tab chưa active
            bool isHov = sf::FloatRect(tabX, 185.0f, tabW, 40.0f).contains(static_cast<float>(mousePosCache.x), static_cast<float>(mousePosCache.y));
            if (!isTabActive && isHov) {
                sf::RectangleShape hBox(sf::Vector2f(tabW, tabH));
                hBox.setPosition(tabX, tabY);
                hBox.setFillColor(sf::Color(255, 235, 170, 70));
                window.draw(hBox);
            }

            // Icon trong tab
            if (topTabIcons[i] && topTabIcons[i]->getSize().x > 0) {
                sf::Sprite ic(*topTabIcons[i]);
                float icScale = 20.0f / static_cast<float>(topTabIcons[i]->getSize().y);
                ic.setScale(icScale, icScale);
                float icW = static_cast<float>(topTabIcons[i]->getSize().x) * icScale;
                ic.setPosition(tabX + (tabW - icW) / 2.0f, tabY + 6.0f);
                if (isTabActive) ic.setColor(sf::Color(185, 35, 25));
                window.draw(ic);
            }
        }

    // Left page
        // Left page title
        if (assets.frameNormal.getSize().x > 0) {
            sf::Sprite lHeader(assets.frameNormal);
            lHeader.setPosition(535.0f, 235.0f);
            lHeader.setScale(380.0f / static_cast<float>(assets.frameNormal.getSize().x),
                             50.0f / static_cast<float>(assets.frameNormal.getSize().y));
            window.draw(lHeader);
        }

        sf::Text lTitle;
        lTitle.setFont(fBold);
        lTitle.setString(toUtf8(tiengViet ? "DANH MỤC CHÍNH" : "MAIN MENU"));
        lTitle.setCharacterSize(21);
        lTitle.setStyle(sf::Text::Bold);
        lTitle.setFillColor(sf::Color(140, 25, 20));
        sf::FloatRect ltb = lTitle.getLocalBounds();
        lTitle.setOrigin(ltb.left + ltb.width / 2.0f, ltb.top + ltb.height / 2.0f);
        lTitle.setPosition(535.0f + 190.0f, 260.0f);
        window.draw(lTitle);

        if (assets.decLine.getSize().x > 0) {
            sf::Sprite lineL(assets.decLine);
            lineL.setPosition(540.0f, 292.0f);
            lineL.setScale(370.0f / static_cast<float>(assets.decLine.getSize().x), 2.8f);
            window.draw(lineL);
        }

        // Category buttons
        const std::vector<std::string> navLabels = {
            tiengViet ? "Chơi Mới" : "Play",
            tiengViet ? "Tiếp Tục" : "Load Game",
            tiengViet ? "Cài Đặt" : "Settings",
            tiengViet ? "Thông Tin" : "Information",
            tiengViet ? "Thoát Game" : "Exit"
        };

        const float lBtnX = 535.0f;
        const float lBtnW = 380.0f;
        const float lBtnH = 48.0f;
        const float lStartBtnY = 310.0f;
        const float lSpacing = 66.0f;

        for (int i = 0; i < 5; ++i) {
            float by = lStartBtnY + i * lSpacing;
            bool isCurrent = (static_cast<int>(activeSection) == i);
            bool isHover = sf::FloatRect(lBtnX, by, lBtnW, lBtnH).contains(static_cast<float>(mousePosCache.x), static_cast<float>(mousePosCache.y));
            bool isHighlight = isHover || isCurrent;

            // Bar background & frame outline
            sf::RectangleShape barShape(sf::Vector2f(lBtnW, lBtnH));
            barShape.setPosition(lBtnX, by);

            if (isHighlight) {
                barShape.setFillColor(sf::Color(255, 245, 230, 255));
                barShape.setOutlineThickness(2.0f);
                barShape.setOutlineColor(sf::Color(210, 140, 50, 245));
            } else {
                barShape.setFillColor(sf::Color(240, 228, 210, 220));
                barShape.setOutlineThickness(1.2f);
                barShape.setOutlineColor(sf::Color(175, 140, 100, 180));
            }
            window.draw(barShape);

            // Selection frame
            if (isHighlight && assets.frameHover.getSize().x > 0) {
                sf::Sprite fHover(assets.frameHover);
                fHover.setPosition(lBtnX - 3.0f, by - 3.0f);
                fHover.setScale((lBtnW + 6.0f) / static_cast<float>(assets.frameHover.getSize().x),
                                (lBtnH + 6.0f) / static_cast<float>(assets.frameHover.getSize().y));
                window.draw(fHover);
            }

            // 4 Corner brackets on highlight
            if (isHighlight) {
                ve4GocLuaChon(window, sf::Vector2f(lBtnX, by), sf::Vector2f(lBtnW, lBtnH), 2.0f, 1.8f);
            }

            // Icon
            if (topTabIcons[i] && topTabIcons[i]->getSize().x > 0) {
                sf::Sprite ic(*topTabIcons[i]);
                float icH = 26.0f;
                float icScale = icH / static_cast<float>(topTabIcons[i]->getSize().y);
                ic.setScale(icScale, icScale);
                ic.setPosition(lBtnX + 16.0f, by + (lBtnH - icH) / 2.0f);
                if (isHighlight) ic.setColor(sf::Color(190, 35, 25));
                else ic.setColor(sf::Color(85, 55, 30));
                window.draw(ic);
            }

            // Item label
            sf::Text bText;
            bText.setFont(fBold);
            bText.setString(toUtf8(navLabels[i]));
            bText.setCharacterSize(20);
            bText.setStyle(sf::Text::Bold);
            if (isHighlight) bText.setFillColor(sf::Color(185, 25, 20));
            else bText.setFillColor(sf::Color(55, 30, 15));
            bText.setPosition(lBtnX + 56.0f, by + 10.0f);
            window.draw(bText);
        }

        // Control prompts
        sf::Text fLText;
        fLText.setFont(fBold);
        fLText.setString(toUtf8(tiengViet ? "W/S: Lên/Xuống  |  A/D: Đổi Menu" : "W/S: Up/Down  |  A/D: Switch Menu"));
        fLText.setCharacterSize(14);
        fLText.setStyle(sf::Text::Bold);
        fLText.setFillColor(sf::Color(80, 45, 25));
        fLText.setPosition(535.0f, 695.0f);
        window.draw(fLText);

    // Right page
        const float rHeaderX = 1042.0f;
        const float rHeaderW = 390.0f;

        // Right page title
        if (assets.frameNormal.getSize().x > 0) {
            sf::Sprite rHeader(assets.frameNormal);
            rHeader.setPosition(rHeaderX, 235.0f);
            rHeader.setScale(rHeaderW / static_cast<float>(assets.frameNormal.getSize().x),
                             50.0f / static_cast<float>(assets.frameNormal.getSize().y));
            window.draw(rHeader);
        }

        std::string rightTitleStr;
        if (activeSection == MenuSection::PLAY) rightTitleStr = tiengViet ? "CHỌN CHẾ ĐỘ CHƠI" : "SELECT GAME MODE";
        else if (activeSection == MenuSection::LOAD) rightTitleStr = tiengViet ? "CÁC VÁN ĐẤU ĐÃ LƯU" : "SAVED GAMES";
        else if (activeSection == MenuSection::SETTINGS) rightTitleStr = tiengViet ? "CÀI ĐẶT HỆ THỐNG" : "SYSTEM SETTINGS";
        else if (activeSection == MenuSection::INFO) rightTitleStr = tiengViet ? "THÔNG TIN & LUẬT CHƠI" : "INFORMATION & RULES";
        else if (activeSection == MenuSection::EXIT) rightTitleStr = tiengViet ? "XÁC NHẬN THOÁT GAME" : "CONFIRM EXIT";

        sf::Text rTitle;
        rTitle.setFont(fBold);
        rTitle.setString(toUtf8(rightTitleStr));
        rTitle.setCharacterSize(21);
        rTitle.setStyle(sf::Text::Bold);
        rTitle.setFillColor(sf::Color(140, 25, 20));
        sf::FloatRect rtb = rTitle.getLocalBounds();
        rTitle.setOrigin(rtb.left + rtb.width / 2.0f, rtb.top + rtb.height / 2.0f);
        rTitle.setPosition(rHeaderX + rHeaderW / 2.0f, 260.0f);
        window.draw(rTitle);

        if (assets.decLine.getSize().x > 0) {
            sf::Sprite lineR(assets.decLine);
            lineR.setPosition(rHeaderX, 292.0f);
            lineR.setScale(rHeaderW / static_cast<float>(assets.decLine.getSize().x), 2.8f);
            window.draw(lineR);
        }

        // Load section
        if (activeSection == MenuSection::LOAD) {
            for (int i = 0; i < 3; ++i) {
                float slotY = 315.0f + i * 90.0f;
                std::string headerStr = (tiengViet ? "Ô LƯU " : "SAVE SLOT ") + std::to_string(i + 1);
                std::string descStr = getSlotInfoStr ? getSlotInfoStr(i, 0) : (tiengViet ? "(Ô lưu trống - Chưa có dữ liệu)" : "(Empty slot)");
                std::string subStr = getSlotInfoStr ? getSlotInfoStr(i, 1) : "";

                sf::Text sHeader;
                sHeader.setFont(fBold);
                sHeader.setString(toUtf8(headerStr));
                sHeader.setCharacterSize(16);
                sHeader.setStyle(sf::Text::Bold);
                sHeader.setFillColor(currentSlot == i ? sf::Color(185, 25, 20) : sf::Color(55, 28, 14));
                sHeader.setPosition(rHeaderX + 18.0f, slotY + 8.0f);
                window.draw(sHeader);

                sf::Text sDesc;
                sDesc.setFont(fBold);
                sDesc.setString(toUtf8(descStr));
                sDesc.setCharacterSize(15);
                sDesc.setStyle(sf::Text::Bold);
                sDesc.setFillColor(descStr.find("(") != std::string::npos ? sf::Color(115, 80, 50) : sf::Color(45, 22, 10));
                sDesc.setPosition(rHeaderX + 18.0f, slotY + 32.0f);
                window.draw(sDesc);

                if (!subStr.empty()) {
                    sf::Text sSub;
                    sSub.setFont(fBold);
                    sSub.setString(toUtf8(subStr));
                    sSub.setCharacterSize(14);
                    sSub.setStyle(sf::Text::Bold);
                    sSub.setFillColor(sf::Color(85, 45, 25));
                    sSub.setPosition(rHeaderX + 18.0f, slotY + 54.0f);
                    window.draw(sSub);
                }
            }
        }
        // Settings section
        else if (activeSection == MenuSection::SETTINGS) {
            if (settingsSubTab == 0) { // Âm thanh (Audio)
                const std::vector<std::string> audioLabels = {
                    tiengViet ? "Âm lượng tổng" : "All Volumes",
                    tiengViet ? "Nhạc nền (BGM)" : "Music",
                    tiengViet ? "Hiệu ứng (SFX)" : "Sound Effects"
                };

                float curVols[3] = {
                    getMasterVolume ? getMasterVolume() : 80.0f,
                    getBgmVolume ? getBgmVolume() : 50.0f,
                    getSfxVolume ? getSfxVolume() : 80.0f
                };

                bool curToggles[3] = {
                    (getMasterBat ? getMasterBat() : true) && curVols[0] > 0.0f,
                    getBgmBat ? getBgmBat() : true,
                    getSfxBat ? getSfxBat() : true
                };

                // Sliders
                for (int i = 0; i < 3; ++i) {
                    float rowY = 345.0f + i * 60.0f;

                    // Label bên trái
                    sf::Text aLabel;
                    aLabel.setFont(fBold);
                    aLabel.setString(toUtf8(audioLabels[i]));
                    aLabel.setCharacterSize(16);
                    aLabel.setStyle(sf::Text::Bold);
                    aLabel.setFillColor(sf::Color(50, 25, 12));
                    aLabel.setPosition(rHeaderX + 6.0f, rowY + 3.0f);
                    window.draw(aLabel);

                    // Slider Bar Track
                    float barX = 1202.0f;
                    float barY = rowY + 8.0f;
                    float barW = 135.0f;
                    float barH = 12.0f;

                    if (assets.bar.getSize().x > 0) {
                        sf::Sprite spBar(assets.bar);
                        spBar.setPosition(barX, barY);
                        spBar.setScale(barW / static_cast<float>(assets.bar.getSize().x),
                                       barH / static_cast<float>(assets.bar.getSize().y));
                        window.draw(spBar);
                    } else {
                        sf::RectangleShape rBar(sf::Vector2f(barW, barH));
                        rBar.setPosition(barX, barY);
                        rBar.setFillColor(sf::Color(140, 100, 65));
                        window.draw(rBar);
                    }

                    // Slider Progress Fill
                    float fillW = std::max(0.0f, (barW - 4.0f) * (curVols[i] / 100.0f));
                    if (fillW > 2.0f) {
                        if (assets.fill.getSize().x > 0) {
                            sf::Sprite spFill(assets.fill);
                            spFill.setPosition(barX + 2.0f, barY + 2.0f);
                            spFill.setScale(fillW / static_cast<float>(assets.fill.getSize().x),
                                           (barH - 4.0f) / static_cast<float>(assets.fill.getSize().y));
                            window.draw(spFill);
                        } else {
                            sf::RectangleShape rFill(sf::Vector2f(fillW, barH - 4.0f));
                            rFill.setPosition(barX + 2.0f, barY + 2.0f);
                            rFill.setFillColor(sf::Color(190, 45, 35));
                            window.draw(rFill);
                        }
                    }

                    // Slider Thumb Knob Handle
                    float handleX = barX + barW * (curVols[i] / 100.0f) - 8.0f;
                    float handleY = barY - 6.0f;
                    if (assets.handle.getSize().x > 0) {
                        sf::Sprite spHandle(assets.handle);
                        spHandle.setPosition(handleX, handleY);
                        spHandle.setScale(16.0f / static_cast<float>(assets.handle.getSize().x),
                                          24.0f / static_cast<float>(assets.handle.getSize().y));
                        window.draw(spHandle);
                    } else {
                        sf::CircleShape cHandle(8.0f);
                        cHandle.setPosition(handleX, handleY);
                        cHandle.setFillColor(sf::Color(230, 205, 140));
                        cHandle.setOutlineThickness(1.5f);
                        cHandle.setOutlineColor(sf::Color(90, 50, 25));
                        window.draw(cHandle);
                    }

                    // Value Badge Pill
                    float badgeX = 1347.0f;
                    float badgeY = rowY + 1.0f;
                    float badgeW = 44.0f;
                    float badgeH = 26.0f;

                    sf::RectangleShape rBadge(sf::Vector2f(badgeW, badgeH));
                    rBadge.setPosition(badgeX, badgeY);
                    rBadge.setFillColor(sf::Color(246, 236, 220));
                    rBadge.setOutlineThickness(1.5f);
                    rBadge.setOutlineColor(sf::Color(170, 125, 75));
                    window.draw(rBadge);

                    sf::Text valText;
                    valText.setFont(f);
                    valText.setString(std::to_string(static_cast<int>(curVols[i])));
                    valText.setCharacterSize(15);
                    valText.setStyle(sf::Text::Bold);
                    valText.setFillColor(sf::Color(55, 25, 10));
                    sf::FloatRect vtb = valText.getLocalBounds();
                    valText.setOrigin(vtb.left + vtb.width / 2.0f, vtb.top + vtb.height / 2.0f);
                    valText.setPosition(badgeX + badgeW / 2.0f, badgeY + badgeH / 2.0f);
                    window.draw(valText);

                    // Toggle Switch Button
                    float toggleX = 1401.0f;
                    float toggleY = rowY + 1.0f;
                    float toggleW = 42.0f;
                    float toggleH = 26.0f;

                    const sf::Texture& tTex = curToggles[i] ? assets.toggleOn : assets.toggleOff;
                    if (tTex.getSize().x > 0) {
                        sf::Sprite spToggle(tTex);
                        spToggle.setPosition(toggleX, toggleY);
                        spToggle.setScale(toggleW / static_cast<float>(tTex.getSize().x),
                                          toggleH / static_cast<float>(tTex.getSize().y));
                        window.draw(spToggle);
                    } else {
                        sf::RectangleShape rToggle(sf::Vector2f(toggleW, toggleH));
                        rToggle.setPosition(toggleX, toggleY);
                        rToggle.setFillColor(curToggles[i] ? sf::Color(80, 160, 70) : sf::Color(130, 120, 110));
                        window.draw(rToggle);
                    }
                }

                // Check alarm
                float chkRowY = 525.0f;
                sf::Text chkLabel;
                chkLabel.setFont(fBold);
                chkLabel.setString(toUtf8(tiengViet ? "Báo chiếu tướng" : "Check Alarm"));
                chkLabel.setCharacterSize(16);
                chkLabel.setStyle(sf::Text::Bold);
                chkLabel.setFillColor(sf::Color(50, 25, 12));
                chkLabel.setPosition(rHeaderX + 6.0f, chkRowY + 3.0f);
                window.draw(chkLabel);

                std::string chkStr = getSettingStatusStr ? getSettingStatusStr(9) : "BẬT";
                bool chkOn = (chkStr == "BẬT" || chkStr == "ON");

                // Badge BẬT/TẮT
                float chkBadgeX = 1347.0f;
                float chkBadgeY = chkRowY + 1.0f;
                sf::RectangleShape rChkBadge(sf::Vector2f(44.0f, 26.0f));
                rChkBadge.setPosition(chkBadgeX, chkBadgeY);
                rChkBadge.setFillColor(sf::Color(246, 236, 220));
                rChkBadge.setOutlineThickness(1.5f);
                rChkBadge.setOutlineColor(sf::Color(170, 125, 75));
                window.draw(rChkBadge);

                sf::Text chkValText;
                chkValText.setFont(f);
                chkValText.setString(toUtf8(chkStr));
                chkValText.setCharacterSize(14);
                chkValText.setStyle(sf::Text::Bold);
                chkValText.setFillColor(chkOn ? sf::Color(185, 25, 20) : sf::Color(115, 85, 55));
                sf::FloatRect ctb = chkValText.getLocalBounds();
                chkValText.setOrigin(ctb.left + ctb.width / 2.0f, ctb.top + ctb.height / 2.0f);
                chkValText.setPosition(chkBadgeX + 22.0f, chkBadgeY + 13.0f);
                window.draw(chkValText);

                // Toggle switch chuông báo
                float chkToggleX = 1401.0f;
                const sf::Texture& chkTex = chkOn ? assets.toggleOn : assets.toggleOff;
                if (chkTex.getSize().x > 0) {
                    sf::Sprite spToggle(chkTex);
                    spToggle.setPosition(chkToggleX, chkBadgeY);
                    spToggle.setScale(42.0f / static_cast<float>(chkTex.getSize().x),
                                      26.0f / static_cast<float>(chkTex.getSize().y));
                    window.draw(spToggle);
                }
            }
            else if (settingsSubTab == 1) { // Đồ họa (Graphics)
                // Piece style note
            }
            else if (settingsSubTab == 3) { // Phím (Controls)
                sf::Text hSec;
                hSec.setFont(fBold);
                hSec.setString(toUtf8(tiengViet ? "CÁC PHÍM ĐIỀU KHIỂN BÀN CỜ:" : "BOARD CONTROLS:"));
                hSec.setCharacterSize(15);
                hSec.setStyle(sf::Text::Bold);
                hSec.setFillColor(sf::Color(160, 25, 20));
                hSec.setPosition(rHeaderX + 12.0f, 338.0f);
                window.draw(hSec);

                if (assets.decLine.getSize().x > 0) {
                    sf::Sprite lineDiv(assets.decLine);
                    lineDiv.setPosition(rHeaderX + 10.0f, 366.0f);
                    lineDiv.setScale((rHeaderW - 20.0f) / static_cast<float>(assets.decLine.getSize().x), 1.6f);
                    window.draw(lineDiv);
                }

                const std::vector<std::string> ctrlItems = tiengViet ? std::vector<std::string>{
                    "• Di chuyển con trỏ: Phím W/A/S/D hoặc 4 Mũi Tên",
                    "• Chọn / Hạ quân cờ: Phím [Space] hoặc [Enter]",
                    "• Hủy chọn / Menu: Phím [ESC] hoặc [Backspace]",
                    "• Hoàn tác nước đi: Phím [Ctrl + Z]"
                } : std::vector<std::string>{
                    "• Move Cursor: W/A/S/D or Arrow Keys",
                    "• Select / Move Piece: [Space] or [Enter]",
                    "• Deselect / Menu: [ESC] or [Backspace]",
                    "• Undo Move: [Ctrl + Z]"
                };

                for (size_t i = 0; i < ctrlItems.size(); ++i) {
                    sf::Text cTxt;
                    cTxt.setFont(fBold);
                    cTxt.setString(toUtf8(ctrlItems[i]));
                    cTxt.setCharacterSize(14);
                    cTxt.setStyle(sf::Text::Bold);
                    cTxt.setFillColor(sf::Color(45, 22, 10));
                    cTxt.setPosition(rHeaderX + 12.0f, 382.0f + static_cast<float>(i) * 32.0f);
                    window.draw(cTxt);
                }

                sf::Text promptTxt;
                promptTxt.setFont(fBold);
                promptTxt.setString(toUtf8(tiengViet ? "Chọn bộ phím nhanh bên dưới:" : "Select quick preset below:"));
                promptTxt.setCharacterSize(14);
                promptTxt.setStyle(sf::Text::Bold);
                promptTxt.setFillColor(sf::Color(140, 30, 20));
                promptTxt.setPosition(rHeaderX + 12.0f, 514.0f);
                window.draw(promptTxt);
            }
        }
        // Info section
        else if (activeSection == MenuSection::INFO) {
            if (infoSubTab == 0) {
                sf::Text h1;
                h1.setFont(fBold);
                h1.setString(toUtf8(tiengViet ? "DỰ ÁN LẬP TRÌNH HƯỚNG ĐỐI TƯỢNG (OOP)" : "OBJECT-ORIENTED PROGRAMMING (OOP)"));
                h1.setCharacterSize(15);
                h1.setStyle(sf::Text::Bold);
                h1.setFillColor(sf::Color(160, 25, 20));
                h1.setPosition(rHeaderX + 12.0f, 340.0f);
                window.draw(h1);

                sf::Text h2;
                h2.setFont(fBold);
                h2.setString(toUtf8(tiengViet ? "Trường ĐH Khoa học Tự nhiên - ĐHQG-HCM (2026)" : "VNU-HCM University of Science (HCMUS 2026)"));
                h2.setCharacterSize(14);
                h2.setStyle(sf::Text::Bold);
                h2.setFillColor(sf::Color(80, 45, 25));
                h2.setPosition(rHeaderX + 12.0f, 366.0f);
                window.draw(h2);

                if (assets.decLine.getSize().x > 0) {
                    sf::Sprite lineDiv(assets.decLine);
                    lineDiv.setPosition(rHeaderX + 10.0f, 395.0f);
                    lineDiv.setScale((rHeaderW - 20.0f) / static_cast<float>(assets.decLine.getSize().x), 1.6f);
                    window.draw(lineDiv);
                }

                sf::Text hSec;
                hSec.setFont(fBold);
                hSec.setString(toUtf8(tiengViet ? "CÁC MẪU THIẾT KẾ (DESIGN PATTERNS):" : "DESIGN PATTERNS APPLIED:"));
                hSec.setCharacterSize(14);
                hSec.setStyle(sf::Text::Bold);
                hSec.setFillColor(sf::Color(140, 30, 20));
                hSec.setPosition(rHeaderX + 12.0f, 410.0f);
                window.draw(hSec);

                const std::vector<std::string> patterns = tiengViet ? std::vector<std::string>{
                    "1. State Pattern: Quản lý ván đấu & menu.",
                    "2. Command Pattern: Lưu nước đi, Undo / Redo.",
                    "3. Strategy Pattern: Thuật toán AI Minimax.",
                    "4. Factory Pattern: Khởi tạo đa hình 7 quân cờ.",
                    "5. Observer & Singleton: Quản lý âm thanh & cài đặt."
                } : std::vector<std::string>{
                    "1. State Pattern: Game & menu state flow.",
                    "2. Command Pattern: Move history, Undo / Redo.",
                    "3. Strategy Pattern: Minimax & Alpha-Beta AI.",
                    "4. Factory Pattern: Polymorphic 7 piece types.",
                    "5. Observer & Singleton: Audio & Settings."
                };

                for (size_t i = 0; i < patterns.size(); ++i) {
                    sf::Text pTxt;
                    pTxt.setFont(fBold);
                    pTxt.setString(toUtf8(patterns[i]));
                    pTxt.setCharacterSize(14);
                    pTxt.setStyle(sf::Text::Bold);
                    pTxt.setFillColor(sf::Color(45, 22, 10));
                    pTxt.setPosition(rHeaderX + 12.0f, 442.0f + static_cast<float>(i) * 33.0f);
                    window.draw(pTxt);
                }
            }
            else {
                sf::Text hSec;
                hSec.setFont(fBold);
                hSec.setString(toUtf8(tiengViet ? "QUY TẮC DI CHUYỂN 7 QUÂN CỜ:" : "RULES FOR 7 PIECE TYPES:"));
                hSec.setCharacterSize(15);
                hSec.setStyle(sf::Text::Bold);
                hSec.setFillColor(sf::Color(160, 25, 20));
                hSec.setPosition(rHeaderX + 12.0f, 340.0f);
                window.draw(hSec);

                if (assets.decLine.getSize().x > 0) {
                    sf::Sprite lineDiv(assets.decLine);
                    lineDiv.setPosition(rHeaderX + 10.0f, 370.0f);
                    lineDiv.setScale((rHeaderW - 20.0f) / static_cast<float>(assets.decLine.getSize().x), 1.6f);
                    window.draw(lineDiv);
                }

                const std::vector<std::string> rules = tiengViet ? std::vector<std::string>{
                    "• TƯỚNG: Đi 1 ô ngang/dọc trong Cung; không đối mặt.",
                    "• SĨ: Đi chéo 1 ô trong Cung để hộ vệ Tướng.",
                    "• TƯỢNG: Đi chéo 2 ô (không qua sông); có cản mắt.",
                    "• XE: Đi ngang/dọc không hạn chế khi đường trống.",
                    "• PHÁO: Đi như Xe; ăn quân phải nhảy qua 1 quân.",
                    "• MÃ: Đi chữ Nhật (1 thẳng 1 chéo); có cản chân.",
                    "• TỐT: Chưa qua sông đi thẳng; qua sông đi thêm ngang."
                } : std::vector<std::string>{
                    "• GENERAL: 1 step orthogonal in Palace. No facing.",
                    "• ADVISOR: 1 step diagonally within Palace.",
                    "• ELEPHANT: 2 steps diagonally, no crossing river.",
                    "• CHARIOT: Orthogonal move. Strongest piece.",
                    "• CANNON: Moves like Chariot; captures by jumping.",
                    "• HORSE: Moves in L-shape; can be blocked.",
                    "• SOLDIER: 1 step forward; sideways after river."
                };

                for (size_t i = 0; i < rules.size(); ++i) {
                    sf::Text rTxt;
                    rTxt.setFont(fBold);
                    rTxt.setString(toUtf8(rules[i]));
                    rTxt.setCharacterSize(14);
                    rTxt.setStyle(sf::Text::Bold);
                    rTxt.setFillColor(sf::Color(45, 22, 10));
                    rTxt.setPosition(rHeaderX + 12.0f, 386.0f + static_cast<float>(i) * 34.0f);
                    window.draw(rTxt);
                }
            }
        }
        // Exit section
        else if (activeSection == MenuSection::EXIT) {
            sf::CircleShape exitRing(48.0f);
            exitRing.setFillColor(sf::Color(230, 205, 165));
            exitRing.setOutlineThickness(3.0f);
            exitRing.setOutlineColor(sf::Color(190, 40, 30));
            exitRing.setOrigin(48.0f, 48.0f);
            exitRing.setPosition(rHeaderX + rHeaderW / 2.0f, 360.0f);
            window.draw(exitRing);

            if (assets.iconExit.getSize().x > 0) {
                sf::Sprite ic(assets.iconExit);
                sf::Vector2u isz = assets.iconExit.getSize();
                ic.setOrigin(isz.x / 2.0f, isz.y / 2.0f);
                ic.setPosition(rHeaderX + rHeaderW / 2.0f, 360.0f);
                ic.setScale(1.8f, 1.8f);
                window.draw(ic);
            }

            sf::Text eTxt;
            eTxt.setFont(fBold);
            std::string q = tiengViet
                ? "Bạn có chắc chắn muốn thoát game?\nMọi thiết lập đã lưu sẽ được bảo toàn."
                : "Are you sure you want to exit?\nAll saved settings will be preserved.";
            eTxt.setString(toUtf8(q));
            eTxt.setCharacterSize(17);
            eTxt.setStyle(sf::Text::Bold);
            eTxt.setFillColor(sf::Color(55, 25, 12));
            eTxt.setLineSpacing(1.35f);
            sf::FloatRect etb = eTxt.getLocalBounds();
            eTxt.setOrigin(etb.left + etb.width / 2.0f, etb.top + etb.height / 2.0f);
            eTxt.setPosition(rHeaderX + rHeaderW / 2.0f, 445.0f);
            window.draw(eTxt);
        }

        // Draw buttons
        for (size_t i = 0; i < rightPageButtons.size(); ++i) {
            rightPageButtons[i].draw(window);
        }

        // Control prompts
        sf::Text fRText;
        fRText.setFont(fBold);
        fRText.setString(toUtf8(tiengViet ? "W/S: Lên/Xuống  |  A/D: Sang Trái/Phải  |  Enter: Chọn" : "W/S: Up/Down  |  A/D: Left/Right  |  Enter: Select"));
        fRText.setCharacterSize(14);
        fRText.setStyle(sf::Text::Bold);
        fRText.setFillColor(sf::Color(80, 45, 25));
        fRText.setPosition(1020.0f, 695.0f);
        window.draw(fRText);
    }
}
