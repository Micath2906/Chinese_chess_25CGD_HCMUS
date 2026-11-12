#include "Menu.h"
#include "FantasyUI.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <filesystem>

static bool loadTexSafe(sf::Texture& tex, const std::string& path, bool smooth = false, bool maskWhite = true) {
    const std::vector<std::string> prefixes = { "", "../", "../../", "../../../", "../../../../" };
    std::error_code ec;
    for (const auto& p : prefixes) {
        std::string fullPath = p + path;
        if (!std::filesystem::exists(std::filesystem::u8path(fullPath), ec)) {
            continue;
        }
        sf::Image img;
        if (img.loadFromFile(fullPath)) {
            if (maskWhite) {
                bool hasAlpha = false;
                sf::Vector2u sz = img.getSize();
                for (unsigned int y = 0; y < sz.y && !hasAlpha; ++y) {
                    for (unsigned int x = 0; x < sz.x; ++x) {
                        if (img.getPixel(x, y).a < 250) {
                            hasAlpha = true;
                            break;
                        }
                    }
                }
                if (!hasAlpha) {
                    img.createMaskFromColor(sf::Color::White);
                }
            }
            if (tex.loadFromImage(img)) {
                tex.setSmooth(smooth);
                return true;
            }
        }
    }
    return false;
}

static bool loadFontSafe(sf::Font& font, const std::string& path) {
    const std::vector<std::string> prefixes = { "", "../", "../../", "../../../", "../../../../" };
    std::error_code ec;
    for (const auto& p : prefixes) {
        std::string fullPath = p + path;
        if (std::filesystem::exists(std::filesystem::u8path(fullPath), ec) &&
            font.loadFromFile(fullPath)) {
            return true;
        }
    }
    return false;
}

MenuAssets::MenuAssets() {
    // Background
    loadTexSafe(cleanBg, "assets/xiangqi_background_pavilion.png", true, false) ||
    loadTexSafe(cleanBg, "resources/textures/background.png", true, false);

    // Legacy TravelBook cover/page textures removed; scroll/ebony 9-slice panels are drawn via FantasyUI.
    // Remaining UI icons/toggles still load from the sprite pack directory.
    const std::string bookDir = "assets/Complete_UI_Book_Styles_Pack_Free/Complete_UI_Book_Styles_Pack_Free_v1.0/01_TravelBookLite/Sprites/";

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
    loadTexSafe(playBg, "assets/xiangqi_background_pavilion.png", true, false) ||
    loadTexSafe(playBg, "resources/textures/background play.jpg", true, false);

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
        float coverScale = std::max(viewSz.x / static_cast<float>(texSz.x),
                                    viewSz.y / static_cast<float>(texSz.y));
        sp.setOrigin(static_cast<float>(texSz.x) / 2.0f, static_cast<float>(texSz.y) / 2.0f);
        sp.setPosition(viewSz.x / 2.0f, viewSz.y / 2.0f);
        sp.setScale(coverScale, coverScale);
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
    sf::FloatRect bounds = text.getLocalBounds();
    unsigned int curSize = text.getCharacterSize();
    float availW = shape.getSize().x - 14.0f;
    while (bounds.width > availW && curSize > 10) {
        curSize--;
        text.setCharacterSize(curSize);
        bounds = text.getLocalBounds();
    }

    text.setOrigin(std::round(bounds.left + bounds.width / 2.0f),
                   std::round(bounds.top + bounds.height / 2.0f));
    text.setPosition(std::round(shape.getPosition().x + shape.getSize().x / 2.0f),
                     std::round(shape.getPosition().y + shape.getSize().y / 2.0f));
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

void Button::setSize(float w, float h) {
    if (shape.getSize().x == w && shape.getSize().y == h) return;
    shape.setSize(sf::Vector2f(w, h));
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
    bool isHighlighted = (hovered || selected) && enabled;
    sf::Vector2f pos = shape.getPosition();
    sf::Vector2f sz = shape.getSize();

    auto& fui = FantasyUI::get();
    if (fui.isLoaded() && fui.texFantasy.getSize().x > 0) {
        sf::IntRect srcRect = isHighlighted ? fui.rcBtnHover : fui.rcBtnNormal;
        sf::FloatRect dstRect(pos.x, pos.y, sz.x, sz.y);
        sf::Color tint = !enabled ? sf::Color(120, 120, 120, 200) : sf::Color::White;
        fui.draw9Slice(window, fui.texFantasy, srcRect, 18, 10, 18, 10, dstRect, tint);
    } else {
        // 1. Draw rich dark rosewood plaque base
        sf::RectangleShape drawShape(sz);
        drawShape.setPosition(pos);

        if (!enabled) {
            drawShape.setFillColor(sf::Color(35, 28, 24, 210));
            drawShape.setOutlineThickness(1.2f);
            drawShape.setOutlineColor(sf::Color(90, 75, 65, 180));
        } else if (isHighlighted) {
            drawShape.setFillColor(sf::Color(65, 42, 25, 255));
            drawShape.setOutlineThickness(2.0f);
            drawShape.setOutlineColor(sf::Color(235, 185, 75, 255));
        } else {
            drawShape.setFillColor(sf::Color(42, 28, 20, 245));
            drawShape.setOutlineThickness(1.5f);
            drawShape.setOutlineColor(sf::Color(160, 115, 65, 220));
        }
        window.draw(drawShape);
    }

    // 2. Corner brackets on highlight
    if (isHighlighted) {
        ve4GocLuaChon(window, pos, sz, 2.0f, 1.8f);
    }

    // 3. Draw icon if attached
    if (texIcon && texIcon->getSize().x > 0 && enabled) {
        sf::Sprite iconSp(*texIcon);
        sf::Vector2u iSz = texIcon->getSize();
        float targetH = sz.y * 0.52f;
        float iconScale = targetH / static_cast<float>(iSz.y);
        iconSp.setScale(iconScale, iconScale);
        float iconH = static_cast<float>(iSz.y) * iconScale;

        iconSp.setPosition(pos.x + 12.0f, pos.y + (sz.y - iconH) / 2.0f);
        if (isHighlighted) {
            iconSp.setColor(sf::Color(255, 235, 150));
        } else {
            iconSp.setColor(sf::Color(210, 185, 140));
        }
        window.draw(iconSp);
    }

    // 4. Draw text with geometric std::round centering
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(std::round(bounds.left + bounds.width / 2.0f),
                   std::round(bounds.top + bounds.height / 2.0f));
    text.setPosition(std::round(pos.x + sz.x / 2.0f),
                     std::round(pos.y + sz.y / 2.0f));

    sf::Text drawText = text;
    if (!enabled) {
        drawText.setFillColor(sf::Color(130, 120, 110));
    } else if (isHighlighted) {
        drawText.setFillColor(sf::Color(255, 245, 140));
        drawText.setStyle(sf::Text::Bold);
    } else {
        drawText.setFillColor(sf::Color(245, 230, 205));
        drawText.setStyle(sf::Text::Bold);
    }

    // Carved shadow
    sf::Text shadow = drawText;
    shadow.setPosition(std::round(drawText.getPosition().x + 1.2f), std::round(drawText.getPosition().y + 1.2f));
    shadow.setFillColor(sf::Color(12, 6, 3, 230));
    window.draw(shadow);
    window.draw(drawText);
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

    // Symmetrical Right Panel at (640, 160, 300, 480) -> inner (rx = 665, rw = 250)
    const float rx = 665.0f;
    const float rw = 250.0f;

    // Play section
    if (activeSection == MenuSection::PLAY) {
        const float playX = 670.0f;
        const float playW = 240.0f;
        const float playH = 44.0f;
        const float playY = 266.0f;
        const float stepY = 56.0f;

        rightPageButtons.emplace_back(playX, playY, playW, playH,
            tiengViet ? "Đấu 2 Người (PvP)" : "Two Players (PvP)", f, [this]() {
            if (onPlayPvp) onPlayPvp();
        });
        rightPageButtons.back().setIcon(&assets.iconPlay);
        rightPageButtons.back().setCharacterSize(15);

        rightPageButtons.emplace_back(playX, playY + stepY, playW, playH,
            tiengViet ? "Đấu Với Máy (PvAI)" : "Player vs AI", f, [this]() {
            if (onPlayPvai) onPlayPvai();
        });
        rightPageButtons.back().setIcon(&assets.iconLoad);
        rightPageButtons.back().setCharacterSize(15);

        rightPageButtons.emplace_back(playX, playY + stepY * 2.0f, playW, playH,
            tiengViet ? "Xem Máy Đấu (Demo)" : "Demo Match", f, [this]() {
            if (onPlayDemo) onPlayDemo();
        });
        rightPageButtons.back().setIcon(&assets.iconInfo);
        rightPageButtons.back().setCharacterSize(15);
    }
    // Load section
    else if (activeSection == MenuSection::LOAD) {
        for (int i = 0; i < 3; ++i) {
            float slotY = 252.0f + static_cast<float>(i) * 74.0f;
            rightPageButtons.emplace_back(rx, slotY, rw, 66.0f, "", f, [this, i]() {
                currentSlot = i;
                if (onSelectSlot) onSelectSlot(i);
                capNhatNutTrangPhai();
            });
            if (currentSlot == i) {
                rightPageButtons.back().setSelected(true);
            }
        }

        rightPageButtons.emplace_back(rx, 496.0f, 120.0f, 44.0f, tiengViet ? "Vào Ván" : "Load Game", f, [this]() {
            if (onLoadSlot) onLoadSlot(currentSlot);
        });
        rightPageButtons.back().setCharacterSize(14);

        rightPageButtons.emplace_back(rx + 130.0f, 496.0f, 120.0f, 44.0f, tiengViet ? "Xóa Lưu" : "Delete", f, [this]() {
            if (onDeleteSlot) onDeleteSlot(currentSlot);
            capNhatNutTrangPhai();
        });
        rightPageButtons.back().setCharacterSize(14);
    }
    // Settings section
    else if (activeSection == MenuSection::SETTINGS) {
        const std::vector<std::string> tabNames = tiengViet
            ? std::vector<std::string>{ "Âm thanh", "Đồ họa", "Trò chơi", "Phím" }
            : std::vector<std::string>{ "Audio", "Graphics", "Game", "Keys" };
        for (int i = 0; i < 4; ++i) {
            float tx = 660.0f + static_cast<float>(i) * 66.0f;
            rightPageButtons.emplace_back(tx, 248.0f, 62.0f, 32.0f, tabNames[i], f, [this, i]() {
                settingsSubTab = i;
                capNhatNutTrangPhai();
            });
            rightPageButtons.back().setCharacterSize(12);
            if (settingsSubTab == i) {
                rightPageButtons.back().setSelected(true);
            }
        }

        if (settingsSubTab == 0) { // Audio
            rightPageButtons.emplace_back(670.0f, 492.0f, 240.0f, 38.0f,
                tiengViet ? "Nghe Thử Tiếng Cờ" : "Test Chess Sound", f, [this]() {
                if (onTestSound) onTestSound();
            });
            rightPageButtons.back().setCharacterSize(13);
        } else if (settingsSubTab == 1) { // Graphics
            std::string themeStr = getSettingStatusStr ? getSettingStatusStr(3) : (tiengViet ? "Gỗ Cổ" : "Classic");
            rightPageButtons.emplace_back(670.0f, 304.0f, 240.0f, 42.0f,
                (tiengViet ? "Bàn: " : "Board: ") + themeStr, f, [this]() {
                if (onToggleBoardTheme) onToggleBoardTheme();
                capNhatNutTrangPhai();
            });
            rightPageButtons.back().setCharacterSize(13);

            std::string atmStr = getSettingStatusStr ? getSettingStatusStr(5) : (tiengViet ? "Lá Trúc" : "Bamboo");
            rightPageButtons.emplace_back(670.0f, 360.0f, 240.0f, 42.0f,
                (tiengViet ? "Hiệu ứng: " : "FX: ") + atmStr, f, [this]() {
                if (onToggleAtmosphere) onToggleAtmosphere();
                capNhatNutTrangPhai();
            });
            rightPageButtons.back().setCharacterSize(13);

            std::string winStr = getSettingStatusStr ? getSettingStatusStr(6) : (tiengViet ? "Toàn MH" : "Full");
            rightPageButtons.emplace_back(670.0f, 416.0f, 240.0f, 42.0f,
                (tiengViet ? "Màn hình: " : "Mode: ") + winStr, f, [this]() {
                if (onToggleWindowMode) onToggleWindowMode();
                capNhatNutTrangPhai();
            });
            rightPageButtons.back().setCharacterSize(13);
        } else if (settingsSubTab == 2) { // Gameplay
            std::string langStr = getSettingStatusStr ? getSettingStatusStr(7) : (tiengViet ? "Tiếng Việt" : "English");
            rightPageButtons.emplace_back(670.0f, 304.0f, 240.0f, 42.0f,
                (tiengViet ? "Ngôn ngữ: " : "Lang: ") + langStr, f, [this]() {
                if (onToggleLang) onToggleLang();
                capNhatNutTrangPhai();
            });
            rightPageButtons.back().setCharacterSize(13);

            std::string aiStr = getSettingStatusStr ? getSettingStatusStr(8) : (tiengViet ? "450ms" : "450ms");
            rightPageButtons.emplace_back(670.0f, 360.0f, 240.0f, 42.0f,
                (tiengViet ? "Độ trễ AI: " : "AI Delay: ") + aiStr, f, [this]() {
                if (onToggleAiDelay) onToggleAiDelay();
                capNhatNutTrangPhai();
            });
            rightPageButtons.back().setCharacterSize(13);

            std::string chkStr = getSettingStatusStr ? getSettingStatusStr(9) : (tiengViet ? "BẬT" : "ON");
            rightPageButtons.emplace_back(670.0f, 416.0f, 240.0f, 42.0f,
                (tiengViet ? "Báo chiếu: " : "Check: ") + chkStr, f, [this]() {
                if (onToggleCheckAlarm) onToggleCheckAlarm();
                capNhatNutTrangPhai();
            });
            rightPageButtons.back().setCharacterSize(13);
        } else if (settingsSubTab == 3) { // Controls
            rightPageButtons.emplace_back(668.0f, 492.0f, 118.0f, 38.0f, tiengViet ? "Phím WASD" : "WASD Keys", f, [this]() {
                if (onPresetWasd) onPresetWasd();
            });
            rightPageButtons.back().setCharacterSize(12);
            rightPageButtons.emplace_back(794.0f, 492.0f, 118.0f, 38.0f, tiengViet ? "Mũi tên" : "Arrow Keys", f, [this]() {
                if (onPresetArrows) onPresetArrows();
            });
            rightPageButtons.back().setCharacterSize(12);
        }

        rightPageButtons.emplace_back(rx, 552.0f, 120.0f, 44.0f, tiengViet ? "Áp dụng" : "Apply", f, [this]() {
            if (onApplySettings) onApplySettings();
        });
        rightPageButtons.back().setCharacterSize(13);

        rightPageButtons.emplace_back(rx + 130.0f, 552.0f, 120.0f, 44.0f, tiengViet ? "Mặc định" : "Restore", f, [this]() {
            if (onRestoreDefaults) onRestoreDefaults();
            capNhatNutTrangPhai();
        });
        rightPageButtons.back().setCharacterSize(13);
    }
    // Info section
    else if (activeSection == MenuSection::INFO) {
        rightPageButtons.emplace_back(rx, 248.0f, 120.0f, 34.0f, tiengViet ? "Kiến trúc" : "OOP Design", f, [this]() {
            infoSubTab = 0;
            capNhatNutTrangPhai();
        });
        rightPageButtons.back().setCharacterSize(13);
        if (infoSubTab == 0) rightPageButtons.back().setSelected(true);

        rightPageButtons.emplace_back(rx + 130.0f, 248.0f, 120.0f, 34.0f, tiengViet ? "Luật 7 Quân" : "Piece Rules", f, [this]() {
            infoSubTab = 1;
            capNhatNutTrangPhai();
        });
        rightPageButtons.back().setCharacterSize(13);
        if (infoSubTab == 1) rightPageButtons.back().setSelected(true);
    }
    // Exit section
    else if (activeSection == MenuSection::EXIT) {
        rightPageButtons.emplace_back(rx, 500.0f, 120.0f, 44.0f, tiengViet ? "Thoát" : "Exit", f, [this]() {
            if (onConfirmExit) onConfirmExit();
        });
        rightPageButtons.back().setCharacterSize(14);

        rightPageButtons.emplace_back(rx + 130.0f, 500.0f, 120.0f, 44.0f, tiengViet ? "Ở Lại" : "Stay", f, [this]() {
            if (onReturnToGame) onReturnToGame();
            else setSection(MenuSection::PLAY);
        });
        rightPageButtons.back().setCharacterSize(14);
    }
}

void Menu::update(const sf::Vector2i& mousePos) {
    bool mouseMoved = (mousePos != mousePosCache);
    mousePosCache = mousePos;

    // Drag slider inside 1200x800 right panel (barX = 666, barW = 148)
    if (draggingSlider != -1 && isFullyOpen() && activeSection == MenuSection::SETTINGS && settingsSubTab == 0) {
        float val = std::clamp((static_cast<float>(mousePos.x) - 666.0f) / 148.0f * 100.0f, 0.0f, 100.0f);
        if (draggingSlider == 0 && onSetMasterVolume) onSetMasterVolume(val);
        else if (draggingSlider == 1 && onSetBgmVolume) onSetBgmVolume(val);
        else if (draggingSlider == 2 && onSetSfxVolume) onSetSfxVolume(val);
    }

    if (laMenuChinh) {
        float dt = animClock.restart().asSeconds();
        if (dt > 0.1f) dt = 0.1f;

        if (isBookOpening) {
            bookProgress += dt * 3.2f; // Smooth twin-panel unfold animation
            if (bookProgress >= 1.0f) {
                bookProgress = 1.0f;
                isBookOpening = false;
                isBookClosed = false;
            }
        } else if (isBookClosing) {
            bookProgress -= dt * 3.2f; // Smooth twin-panel collapse animation
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

    // Update centered panel buttons
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
    // Update twin open panels
    else if (bookProgress >= 0.7f) {
        for (size_t i = 0; i < buttons.size() && i < 5; ++i) {
            buttons[i].update(mousePos);
            if (buttons[i].isHovered()) {
                currentFocus = MenuFocus::MAIN_MENU;
                keyboardNavActive = false;
            }
            bool isActiveSec = (static_cast<int>(activeSection) == static_cast<int>(i));
            buttons[i].setSelected(isActiveSec && (currentFocus == MenuFocus::MAIN_MENU || keyboardNavActive));
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
            keyboardNavActive = false;
        }

        for (size_t i = 0; i < rightPageButtons.size(); ++i) {
            bool isTabSelected =
                (activeSection == MenuSection::SETTINGS && static_cast<int>(i) == settingsSubTab) ||
                (activeSection == MenuSection::INFO && static_cast<int>(i) == infoSubTab) ||
                (activeSection == MenuSection::LOAD && static_cast<int>(i) == currentSlot);
            bool isKbd = (keyboardNavActive && currentFocus == MenuFocus::SUB_MENU && static_cast<int>(i) == rightPageIndex);
            rightPageButtons[i].setSelected(isTabSelected || isKbd);
        }
    }
}

void Menu::handleMouseDown(const sf::Vector2i& mousePos) {
    if (!isFullyOpen()) return;
    if (activeSection == MenuSection::SETTINGS && settingsSubTab == 0) {
        for (int i = 0; i < 3; ++i) {
            float rowY = 298.0f + static_cast<float>(i) * 52.0f;
            if (mousePos.x >= 660 && mousePos.x <= 820 &&
                mousePos.y >= static_cast<int>(rowY + 18.0f) &&
                mousePos.y <= static_cast<int>(rowY + 42.0f)) {
                draggingSlider = i;
                float val = std::clamp((static_cast<float>(mousePos.x) - 666.0f) / 148.0f * 100.0f, 0.0f, 100.0f);
                if (i == 0 && onSetMasterVolume) onSetMasterVolume(val);
                else if (i == 1 && onSetBgmVolume) onSetBgmVolume(val);
                else if (i == 2 && onSetSfxVolume) onSetSfxVolume(val);
                return;
            }
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

        // Single centered panel click (450, 160, 300, 480)
        if (isBookClosed) {
            for (size_t i = 0; i < buttons.size() && i < 5; ++i) {
                if (buttons[i].contains(mousePos)) {
                    triggerOpenAnimation(static_cast<MenuSection>(i));
                    return;
                }
            }

            sf::FloatRect closedRect(450.0f, 160.0f, 300.0f, 480.0f);
            if (closedRect.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                triggerOpenAnimation(MenuSection::PLAY);
                return;
            }
            return;
        }

        // Twin panels open: collapse/back button top-left of left panel (260, 160, 300, 480)
        sf::FloatRect backBtnRect(272.0f, 174.0f, 28.0f, 28.0f);
        if (backBtnRect.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            if (onReturnToGame) {
                onReturnToGame();
                return;
            }
            triggerCloseAnimation();
            return;
        }

        // Left panel menu buttons
        for (size_t i = 0; i < buttons.size() && i < 5; ++i) {
            if (buttons[i].contains(mousePos)) {
                currentFocus = MenuFocus::MAIN_MENU;
                setSection(static_cast<MenuSection>(i));
                return;
            }
        }

        // Audio controls inside right panel
        if (activeSection == MenuSection::SETTINGS && settingsSubTab == 0) {
            currentFocus = MenuFocus::SUB_MENU;
            for (int i = 0; i < 3; ++i) {
                float rowY = 298.0f + static_cast<float>(i) * 52.0f;
                if (sf::FloatRect(866.0f, rowY - 2.0f, 48.0f, 28.0f).contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    if (i == 0 && onToggleMasterSound) onToggleMasterSound();
                    else if (i == 1 && onToggleBgm) onToggleBgm();
                    else if (i == 2 && onToggleSfx) onToggleSfx();
                    return;
                }
                if (sf::FloatRect(660.0f, rowY + 18.0f, 160.0f, 24.0f).contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    float val = std::clamp((static_cast<float>(mousePos.x) - 666.0f) / 148.0f * 100.0f, 0.0f, 100.0f);
                    if (i == 0 && onSetMasterVolume) onSetMasterVolume(val);
                    else if (i == 1 && onSetBgmVolume) onSetBgmVolume(val);
                    else if (i == 2 && onSetSfxVolume) onSetSfxVolume(val);
                    return;
                }
            }
            if (sf::FloatRect(866.0f, 454.0f, 48.0f, 28.0f).contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                if (onToggleCheckAlarm) onToggleCheckAlarm();
                return;
            }
        }

        // Right panel buttons
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
    sf::Font& fBold = f;
    auto& fui = FantasyUI::get();

    if (!laMenuChinh) {
        for (auto& button : buttons) {
            button.draw(window);
        }
        return;
    }

    // Dusk atmospheric vignette covering 100% of physical window pixels
    fui.drawFullDimOverlay(window, sf::Color(10, 8, 14, 48));

    // 1. Hanging Oriental Signboard Header: X = 400, Y = 60, W = 400, H = 60
    const float titleX = 400.0f;
    const float titleY = 60.0f;
    const float titleW = 400.0f;
    const float titleH = 60.0f;

    // Brass hanging chains suspending signboard from top beam
    const sf::Color brassChainCol(175, 138, 62, 225);
    const sf::Color chainShadowCol(20, 12, 6, 180);
    for (float cxChain : { 446.0f, 754.0f }) {
        sf::RectangleShape chainShadow(sf::Vector2f(4.0f, titleY + 4.0f));
        chainShadow.setPosition(cxChain - 1.0f, 0.0f);
        chainShadow.setFillColor(chainShadowCol);
        window.draw(chainShadow);

        sf::RectangleShape chain(sf::Vector2f(2.5f, titleY + 4.0f));
        chain.setPosition(cxChain, 0.0f);
        chain.setFillColor(brassChainCol);
        window.draw(chain);

        sf::CircleShape stud(4.0f);
        stud.setOrigin(4.0f, 4.0f);
        stud.setPosition(cxChain + 1.25f, titleY + 6.0f);
        stud.setFillColor(sf::Color(225, 185, 95));
        window.draw(stud);
    }

    // Signboard shadow + frame
    sf::RectangleShape signShadow(sf::Vector2f(titleW, titleH));
    signShadow.setPosition(titleX + 6.0f, titleY + 6.0f);
    signShadow.setFillColor(sf::Color(0, 0, 0, 115));
    window.draw(signShadow);

    if (fui.isLoaded()) {
        fui.drawPanel(window, sf::FloatRect(titleX, titleY, titleW, titleH), OrientalPanelStyle::EBONY_WOOD);
    } else {
        sf::RectangleShape tPlate(sf::Vector2f(titleW, titleH));
        tPlate.setPosition(titleX, titleY);
        tPlate.setFillColor(sf::Color(28, 20, 14, 240));
        tPlate.setOutlineThickness(2.0f);
        tPlate.setOutlineColor(sf::Color(201, 154, 62));
        window.draw(tPlate);
    }

    // Signboard centered gold calligraphy title
    std::string mainTitleStr = tiengViet ? "CỜ TƯỚNG" : "CHINESE CHESS";
    sf::Text gameTitleShadow;
    gameTitleShadow.setFont(fBold);
    gameTitleShadow.setString(toUtf8(mainTitleStr));
    gameTitleShadow.setCharacterSize(30);
    gameTitleShadow.setStyle(sf::Text::Bold);
    gameTitleShadow.setFillColor(sf::Color(12, 6, 4, 230));
    sf::FloatRect gtsb = gameTitleShadow.getLocalBounds();
    gameTitleShadow.setOrigin(std::round(gtsb.left + gtsb.width / 2.0f), std::round(gtsb.top + gtsb.height / 2.0f));
    gameTitleShadow.setPosition(std::round(titleX + titleW / 2.0f + 1.5f), std::round(titleY + titleH / 2.0f + 1.5f));
    window.draw(gameTitleShadow);

    sf::Text gameTitle;
    gameTitle.setFont(fBold);
    gameTitle.setString(toUtf8(mainTitleStr));
    gameTitle.setCharacterSize(30);
    gameTitle.setStyle(sf::Text::Bold);
    gameTitle.setFillColor(sf::Color(255, 222, 105));
    sf::FloatRect gtb = gameTitle.getLocalBounds();
    gameTitle.setOrigin(std::round(gtb.left + gtb.width / 2.0f), std::round(gtb.top + gtb.height / 2.0f));
    gameTitle.setPosition(std::round(titleX + titleW / 2.0f), std::round(titleY + titleH / 2.0f));
    window.draw(gameTitle);

    // Subtle dynasty subtitle below signboard (above Y=160 panels)
    std::string subTitleStr = tiengViet ? "TƯỢNG KỲ KỲ PHỔ  •  ĐỒ ÁN OOP HCMUS" : "ANCIENT XIANGQI  •  SCROLL PAVILION";
    sf::Text gameSubShadow;
    gameSubShadow.setFont(f);
    gameSubShadow.setString(toUtf8(subTitleStr));
    gameSubShadow.setCharacterSize(13);
    gameSubShadow.setStyle(sf::Text::Bold);
    gameSubShadow.setFillColor(sf::Color(10, 6, 4, 230));
    sf::FloatRect gssb = gameSubShadow.getLocalBounds();
    gameSubShadow.setOrigin(std::round(gssb.left + gssb.width / 2.0f), std::round(gssb.top + gssb.height / 2.0f));
    gameSubShadow.setPosition(601.0f, 137.0f);
    window.draw(gameSubShadow);

    sf::Text gameSub = gameSubShadow;
    gameSub.setFillColor(sf::Color(242, 214, 155));
    gameSub.setPosition(600.0f, 136.0f);
    window.draw(gameSub);

    const float panelY = 160.0f;
    const float panelW = 300.0f;
    const float panelH = 480.0f;
    const float btnW = 240.0f;
    const float btnH = 44.0f;
    const float startBtnY = 266.0f;
    const float btnSpacing = 56.0f;

    const std::vector<std::string> mainBtnLabels = {
        tiengViet ? "CHƠI MỚI" : "Play",
        tiengViet ? "TIẾP TỤC" : "Load Game",
        tiengViet ? "CÀI ĐẶT" : "Settings",
        tiengViet ? "THÔNG TIN" : "Rules & Info",
        tiengViet ? "THOÁT" : "Exit"
    };

    auto drawEbonyOrFallback = [&](sf::FloatRect rect, OrientalPanelStyle style) {
        if (fui.isLoaded() || style == OrientalPanelStyle::PARCHMENT_SCROLL) {
            fui.drawPanel(window, rect, style);
        } else {
            sf::RectangleShape fallback(sf::Vector2f(rect.width, rect.height));
            fallback.setPosition(rect.left, rect.top);
            fallback.setFillColor(sf::Color(28, 20, 14, 242));
            fallback.setOutlineThickness(2.0f);
            fallback.setOutlineColor(sf::Color(201, 154, 62));
            window.draw(fallback);
        }
    };

    auto drawParchmentHeader = [&](const std::string& headerStr, float x, float y, unsigned int size, bool centered) {
        sf::Text txt;
        txt.setFont(fBold);
        txt.setString(toUtf8(headerStr));
        txt.setCharacterSize(size);
        txt.setStyle(sf::Text::Bold);
        if (centered) {
            sf::FloatRect tb = txt.getLocalBounds();
            txt.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));
        }

        sf::Text shadow = txt;
        shadow.setPosition(std::round(x + 1.0f), std::round(y + 1.0f));
        shadow.setFillColor(sf::Color(0, 0, 0, 100));
        window.draw(shadow);

        txt.setPosition(std::round(x), std::round(y));
        txt.setFillColor(sf::Color(175, 25, 20));
        window.draw(txt);
    };

    auto drawLeftPanelContents = [&](float px, bool activeState) {
        if (fui.isLoaded()) {
            fui.drawRibbonTitle(window, px + panelW / 2.0f + (activeState ? 6.0f : 0.0f), panelY + 28.0f,
                                activeState ? 216.0f : 236.0f, 44.0f,
                                tiengViet ? "DANH MỤC" : "MAIN MENU",
                                fBold, 17, sf::Color(255, 240, 210), sf::Color(190, 50, 35));
        } else {
            sf::Text hdr;
            hdr.setFont(fBold);
            hdr.setString(toUtf8(tiengViet ? "DANH MỤC" : "MAIN MENU"));
            hdr.setCharacterSize(18);
            hdr.setStyle(sf::Text::Bold);
            hdr.setFillColor(sf::Color(255, 222, 120));
            sf::FloatRect hb = hdr.getLocalBounds();
            hdr.setOrigin(std::round(hb.left + hb.width / 2.0f), std::round(hb.top + hb.height / 2.0f));
            hdr.setPosition(std::round(px + panelW / 2.0f), panelY + 50.0f);
            window.draw(hdr);
        }

        sf::RectangleShape sepTop(sf::Vector2f(236.0f, 1.5f));
        sepTop.setPosition(px + 32.0f, panelY + 86.0f);
        sepTop.setFillColor(sf::Color(190, 150, 75, 160));
        window.draw(sepTop);

        float btnX = px + (panelW - btnW) / 2.0f;
        for (size_t i = 0; i < buttons.size() && i < 5; ++i) {
            buttons[i].setSize(btnW, btnH);
            buttons[i].setPosition(btnX, startBtnY + static_cast<float>(i) * btnSpacing);
            buttons[i].setLabel(mainBtnLabels[i]);
            buttons[i].setCharacterSize(16);
            if (!activeState) {
                buttons[i].setSelected(keyboardNavActive && static_cast<int>(i) == coverIndex);
            } else {
                bool isActiveSec = (static_cast<int>(activeSection) == static_cast<int>(i));
                buttons[i].setSelected(isActiveSec && (currentFocus == MenuFocus::MAIN_MENU || keyboardNavActive));
            }
            buttons[i].draw(window);
        }

        sf::RectangleShape sepBot(sf::Vector2f(236.0f, 1.5f));
        sepBot.setPosition(px + 32.0f, 558.0f);
        sepBot.setFillColor(sf::Color(190, 150, 75, 140));
        window.draw(sepBot);

        sf::Text hintTxt;
        hintTxt.setFont(f);
        hintTxt.setString(toUtf8(activeState
            ? (tiengViet ? "[A/D] Chuyển bảng" : "[A/D] Switch Panel")
            : (tiengViet ? "[Enter] Mở Bảng" : "[Enter] Unfold Menu")));
        hintTxt.setCharacterSize(13);
        hintTxt.setFillColor(sf::Color(225, 200, 150));
        sf::FloatRect htb = hintTxt.getLocalBounds();
        hintTxt.setOrigin(std::round(htb.left + htb.width / 2.0f), std::round(htb.top + htb.height / 2.0f));
        hintTxt.setPosition(std::round(px + panelW / 2.0f), 592.0f);
        window.draw(hintTxt);
    };

    // 2. Closed State: Single Vertical Panel Centered (X = 450, Y = 160, W = 300, H = 480)
    if (isBookClosed && bookProgress <= 0.0f) {
        const float centerX = 450.0f;

        sf::RectangleShape shadow(sf::Vector2f(panelW, panelH));
        shadow.setPosition(centerX + 8.0f, panelY + 8.0f);
        shadow.setFillColor(sf::Color(0, 0, 0, 110));
        window.draw(shadow);

        drawEbonyOrFallback(sf::FloatRect(centerX, panelY, panelW, panelH), OrientalPanelStyle::EBONY_WOOD);
        drawLeftPanelContents(centerX, false);
    }
    // 3. Animating Twin Panels Unfold / Collapse (450 -> 260 & 640)
    else if (isBookOpening || isBookClosing || bookProgress < 0.7f) {
        float t = 1.0f - std::pow(1.0f - std::clamp(bookProgress, 0.0f, 1.0f), 3.0f);
        float leftX = 450.0f - 190.0f * t;
        float rightX = 450.0f + 190.0f * t;

        // Right panel unfolds symmetrically from behind
        sf::RectangleShape shadowR(sf::Vector2f(panelW, panelH));
        shadowR.setPosition(rightX + 8.0f, panelY + 8.0f);
        shadowR.setFillColor(sf::Color(0, 0, 0, 105));
        window.draw(shadowR);
        drawEbonyOrFallback(sf::FloatRect(rightX, panelY, panelW, panelH), OrientalPanelStyle::EBONY_WOOD);

        // Left panel slides to X = 260
        sf::RectangleShape shadowL(sf::Vector2f(panelW, panelH));
        shadowL.setPosition(leftX + 8.0f, panelY + 8.0f);
        shadowL.setFillColor(sf::Color(0, 0, 0, 110));
        window.draw(shadowL);
        drawEbonyOrFallback(sf::FloatRect(leftX, panelY, panelW, panelH), OrientalPanelStyle::EBONY_WOOD);
        drawLeftPanelContents(leftX, false);
    }
    // 4. Expanded Symmetrical Twin Panels:
    //    Left  = (260, 160, 300, 480)
    //    Right = (640, 160, 300, 480)
    else {
        const float leftX = 260.0f;
        const float rightX = 640.0f;

        // Shadows
        sf::RectangleShape shadowL(sf::Vector2f(panelW, panelH));
        shadowL.setPosition(leftX + 8.0f, panelY + 8.0f);
        shadowL.setFillColor(sf::Color(0, 0, 0, 110));
        window.draw(shadowL);

        sf::RectangleShape shadowR(sf::Vector2f(panelW, panelH));
        shadowR.setPosition(rightX + 8.0f, panelY + 8.0f);
        shadowR.setFillColor(sf::Color(0, 0, 0, 110));
        window.draw(shadowR);

        // Twin ornate ebony wood panels
        drawEbonyOrFallback(sf::FloatRect(leftX, panelY, panelW, panelH), OrientalPanelStyle::EBONY_WOOD);
        drawEbonyOrFallback(sf::FloatRect(rightX, panelY, panelW, panelH), OrientalPanelStyle::EBONY_WOOD);

        // Left Panel Back/Collapse icon at (272, 174, 28, 28)
        sf::FloatRect backBtnRect(272.0f, 174.0f, 28.0f, 28.0f);
        bool hoverBack = backBtnRect.contains(static_cast<float>(mousePosCache.x), static_cast<float>(mousePosCache.y));
        sf::RectangleShape backBox(sf::Vector2f(28.0f, 28.0f));
        backBox.setPosition(272.0f, 174.0f);
        backBox.setFillColor(hoverBack ? sf::Color(70, 46, 26, 240) : sf::Color(38, 25, 16, 220));
        backBox.setOutlineThickness(1.5f);
        backBox.setOutlineColor(hoverBack ? sf::Color(255, 215, 100) : sf::Color(180, 140, 65));
        window.draw(backBox);

        if (assets.iconArrow.getSize().x > 0) {
            sf::Sprite spArrow(assets.iconArrow);
            sf::Vector2u asz = assets.iconArrow.getSize();
            spArrow.setOrigin(asz.x / 2.0f, asz.y / 2.0f);
            spArrow.setPosition(286.0f, 188.0f);
            spArrow.setScale(-1.2f, 1.2f);
            spArrow.setColor(hoverBack ? sf::Color(255, 225, 110) : sf::Color(225, 190, 130));
            window.draw(spArrow);
        } else {
            sf::Text backGlyph;
            backGlyph.setFont(fBold);
            backGlyph.setString(toUtf8("<"));
            backGlyph.setCharacterSize(16);
            backGlyph.setFillColor(hoverBack ? sf::Color(255, 225, 110) : sf::Color(225, 190, 130));
            sf::FloatRect bgb = backGlyph.getLocalBounds();
            backGlyph.setOrigin(std::round(bgb.left + bgb.width / 2.0f), std::round(bgb.top + bgb.height / 2.0f));
            backGlyph.setPosition(286.0f, 188.0f);
            window.draw(backGlyph);
        }

        if (hoverBack) {
            sf::RectangleShape tipBg(sf::Vector2f(106.0f, 24.0f));
            tipBg.setPosition(260.0f, 134.0f);
            tipBg.setFillColor(sf::Color(22, 15, 10, 235));
            tipBg.setOutlineThickness(1.0f);
            tipBg.setOutlineColor(sf::Color(210, 170, 75));
            window.draw(tipBg);

            sf::Text tipTxt;
            tipTxt.setFont(fBold);
            tipTxt.setString(toUtf8(tiengViet ? "Thu gọn [ESC]" : "Collapse [ESC]"));
            tipTxt.setCharacterSize(12);
            tipTxt.setFillColor(sf::Color(255, 228, 125));
            sf::FloatRect ttb = tipTxt.getLocalBounds();
            tipTxt.setOrigin(std::round(ttb.left + ttb.width / 2.0f), std::round(ttb.top + ttb.height / 2.0f));
            tipTxt.setPosition(313.0f, 146.0f);
            window.draw(tipTxt);
        }

        // Render Left Panel buttons
        drawLeftPanelContents(leftX, true);

        // Right Panel Title Ribbon
        std::string rightTitleStr;
        if (activeSection == MenuSection::PLAY) rightTitleStr = tiengViet ? "CHẾ ĐỘ CHƠI" : "GAME MODES";
        else if (activeSection == MenuSection::LOAD) rightTitleStr = tiengViet ? "VÁN ĐÃ LƯU" : "SAVED GAMES";
        else if (activeSection == MenuSection::SETTINGS) rightTitleStr = tiengViet ? "CÀI ĐẶT" : "SETTINGS";
        else if (activeSection == MenuSection::INFO) rightTitleStr = tiengViet ? "THÔNG TIN" : "INFO & RULES";
        else if (activeSection == MenuSection::EXIT) rightTitleStr = tiengViet ? "THOÁT GAME" : "EXIT GAME";

        if (fui.isLoaded()) {
            fui.drawRibbonTitle(window, rightX + panelW / 2.0f, panelY + 28.0f, 236.0f, 44.0f,
                                rightTitleStr, fBold, 16, sf::Color(255, 240, 210), sf::Color(190, 50, 35));
        } else {
            sf::Text rTitle;
            rTitle.setFont(fBold);
            rTitle.setString(toUtf8(rightTitleStr));
            rTitle.setCharacterSize(18);
            rTitle.setStyle(sf::Text::Bold);
            rTitle.setFillColor(sf::Color(255, 222, 120));
            sf::FloatRect rtb = rTitle.getLocalBounds();
            rTitle.setOrigin(std::round(rtb.left + rtb.width / 2.0f), std::round(rtb.top + rtb.height / 2.0f));
            rTitle.setPosition(std::round(rightX + panelW / 2.0f), panelY + 50.0f);
            window.draw(rTitle);
        }

        const float rx = 665.0f;
        const float rw = 250.0f;
        const float rightCenterX = rightX + panelW / 2.0f; // 790.0f

        if (activeSection == MenuSection::PLAY) {
            sf::RectangleShape sepR(sf::Vector2f(236.0f, 1.5f));
            sepR.setPosition(rightX + 32.0f, panelY + 86.0f);
            sepR.setFillColor(sf::Color(190, 150, 75, 160));
            window.draw(sepR);

            for (auto& btn : rightPageButtons) {
                btn.draw(window);
            }

            // Mode preview card below the 3 cards (eliminates dead space)
            drawEbonyOrFallback(sf::FloatRect(652.0f, 442.0f, 276.0f, 178.0f), OrientalPanelStyle::PARCHMENT_SCROLL);

            int previewMode = 0;
            for (size_t i = 0; i < rightPageButtons.size() && i < 3; ++i) {
                if (rightPageButtons[i].isHovered() || (keyboardNavActive && currentFocus == MenuFocus::SUB_MENU && static_cast<int>(i) == rightPageIndex)) {
                    previewMode = static_cast<int>(i);
                    break;
                }
            }

            std::string mHead, mDesc;
            if (previewMode == 0) {
                mHead = tiengViet ? "ĐẤU ĐỐI KHÁNG" : "TWO PLAYERS (PvP)";
                mDesc = tiengViet
                    ? "Hai kỳ thủ luân phiên đánh\nquân Đỏ và Đen trên cùng máy.\nHỗ trợ Undo/Redo & lưu ván."
                    : "Challenge a friend locally\non Red and Black turns.\nIncludes timer & history.";
            } else if (previewMode == 1) {
                mHead = tiengViet ? "ĐẤU VỚI AI" : "PLAYER VS AI";
                mDesc = tiengViet
                    ? "Thi đấu cùng AI Minimax.\nTùy chọn phe Đỏ/Đen, cấp độ\nvà thời gian suy nghĩ."
                    : "Battle our Minimax engine.\nChoose your side, timer\nand AI difficulty level.";
            } else {
                mHead = tiengViet ? "MÁY ĐẤU DEMO" : "DEMO MATCH";
                mDesc = tiengViet
                    ? "Xem hai kỳ thủ AI tự động\nđối đầu và phân tích các\nthế trận kinh điển."
                    : "Watch two AI engines play\nautonomously in real time.\nStudy classic endgames.";
            }

            drawParchmentHeader(mHead, rightCenterX, 468.0f, 20, true);

            sf::RectangleShape pLine(sf::Vector2f(226.0f, 2.0f));
            pLine.setPosition(rightCenterX - 113.0f, 490.0f);
            pLine.setFillColor(sf::Color(45, 25, 15, 160));
            window.draw(pLine);

            sf::Text mdTxt;
            mdTxt.setFont(fBold);
            mdTxt.setString(toUtf8(mDesc));
            mdTxt.setCharacterSize(16);
            mdTxt.setStyle(sf::Text::Bold);
            mdTxt.setFillColor(sf::Color(35, 18, 10));
            mdTxt.setLineSpacing(1.22f);
            sf::FloatRect mdb = mdTxt.getLocalBounds();
            mdTxt.setOrigin(std::round(mdb.left + mdb.width / 2.0f), std::round(mdb.top + mdb.height / 2.0f));
            mdTxt.setPosition(rightCenterX, 554.0f);
            window.draw(mdTxt);
        }
        else if (activeSection == MenuSection::LOAD) {
            // Draw Jade card buttons first, then overlay bright gold/ivory slot text
            for (auto& btn : rightPageButtons) {
                btn.draw(window);
            }

            for (int i = 0; i < 3; ++i) {
                float slotY = 252.0f + static_cast<float>(i) * 74.0f;
                std::string headerStr = (tiengViet ? "Ô LƯU " : "SAVE SLOT ") + std::to_string(i + 1);
                std::string descStr = getSlotInfoStr ? getSlotInfoStr(i, 0) : (tiengViet ? "(Ô lưu trống)" : "(Empty slot)");
                std::string subStr = getSlotInfoStr ? getSlotInfoStr(i, 1) : "";

                sf::Text sHeader;
                sHeader.setFont(fBold);
                sHeader.setString(toUtf8(headerStr));
                sHeader.setCharacterSize(15);
                sHeader.setStyle(sf::Text::Bold);
                sHeader.setFillColor(currentSlot == i ? sf::Color(255, 225, 105) : sf::Color(235, 200, 125));
                sHeader.setPosition(rx + 12.0f, slotY + 5.0f);
                window.draw(sHeader);

                sf::Text sDesc;
                sDesc.setFont(fBold);
                sDesc.setString(toUtf8(descStr));
                sDesc.setCharacterSize(14);
                sDesc.setStyle(sf::Text::Bold);
                sDesc.setFillColor(descStr.find("(") != std::string::npos ? sf::Color(200, 195, 175) : sf::Color(245, 236, 215));
                sDesc.setPosition(rx + 12.0f, slotY + 24.0f);
                window.draw(sDesc);

                if (!subStr.empty()) {
                    sf::Text sSub;
                    sSub.setFont(fBold);
                    sSub.setString(toUtf8(subStr));
                    sSub.setCharacterSize(13);
                    sSub.setStyle(sf::Text::Bold);
                    sSub.setFillColor(sf::Color(205, 225, 205));
                    sSub.setPosition(rx + 12.0f, slotY + 43.0f);
                    window.draw(sSub);
                }
            }

            sf::Text lHint;
            lHint.setFont(fBold);
            lHint.setString(toUtf8(tiengViet ? "Chọn ô lưu rồi bấm Vào Ván" : "Pick a slot then click Load"));
            lHint.setCharacterSize(14);
            lHint.setStyle(sf::Text::Bold);
            lHint.setFillColor(sf::Color(235, 210, 160));
            sf::FloatRect lhb = lHint.getLocalBounds();
            lHint.setOrigin(std::round(lhb.left + lhb.width / 2.0f), std::round(lhb.top + lhb.height / 2.0f));
            lHint.setPosition(rightCenterX, 584.0f);
            window.draw(lHint);
        }
        else if (activeSection == MenuSection::SETTINGS) {
            // Aged Xuan Parchment inset for active settings tab content
            drawEbonyOrFallback(sf::FloatRect(652.0f, 286.0f, 276.0f, 254.0f), OrientalPanelStyle::PARCHMENT_SCROLL);

            if (settingsSubTab == 0) { // Audio
                const std::vector<std::string> audioLabels = {
                    tiengViet ? "Âm tổng" : "Master Vol",
                    tiengViet ? "Nhạc nền" : "Music BGM",
                    tiengViet ? "Hiệu ứng" : "Sound FX"
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

                for (int i = 0; i < 3; ++i) {
                    float rowY = 298.0f + static_cast<float>(i) * 52.0f;

                    sf::Text aLabel;
                    aLabel.setFont(fBold);
                    aLabel.setString(toUtf8(audioLabels[i]));
                    aLabel.setCharacterSize(16);
                    aLabel.setStyle(sf::Text::Bold);
                    aLabel.setFillColor(sf::Color(35, 18, 10));
                    aLabel.setPosition(664.0f, rowY - 1.0f);
                    window.draw(aLabel);

                    // Value Badge
                    float badgeX = 824.0f;
                    float badgeY = rowY;
                    float badgeW = 38.0f;
                    float badgeH = 22.0f;
                    sf::RectangleShape rBadge(sf::Vector2f(badgeW, badgeH));
                    rBadge.setPosition(badgeX, badgeY);
                    rBadge.setFillColor(sf::Color(246, 236, 215));
                    rBadge.setOutlineThickness(1.5f);
                    rBadge.setOutlineColor(sf::Color(45, 25, 15));
                    window.draw(rBadge);

                    sf::Text valText;
                    valText.setFont(fBold);
                    valText.setString(std::to_string(static_cast<int>(curVols[i])));
                    valText.setCharacterSize(14);
                    valText.setStyle(sf::Text::Bold);
                    valText.setFillColor(sf::Color(35, 18, 10));
                    sf::FloatRect vtb = valText.getLocalBounds();
                    valText.setOrigin(std::round(vtb.left + vtb.width / 2.0f), std::round(vtb.top + vtb.height / 2.0f));
                    valText.setPosition(badgeX + badgeW / 2.0f, badgeY + badgeH / 2.0f);
                    window.draw(valText);

                    // Toggle Switch
                    float toggleX = 870.0f;
                    float toggleY = rowY;
                    float toggleW = 40.0f;
                    float toggleH = 22.0f;
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
                        rToggle.setFillColor(curToggles[i] ? sf::Color(70, 155, 65) : sf::Color(135, 120, 105));
                        window.draw(rToggle);
                    }

                    // Slider Bar Track cleanly beneath label
                    float barX = 664.0f;
                    float barY = rowY + 26.0f;
                    float barW = 152.0f;
                    float barH = 10.0f;
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
                            rFill.setFillColor(sf::Color(175, 25, 20));
                            window.draw(rFill);
                        }
                    }

                    float handleX = barX + barW * (curVols[i] / 100.0f) - 7.0f;
                    float handleY = barY - 5.0f;
                    if (assets.handle.getSize().x > 0) {
                        sf::Sprite spHandle(assets.handle);
                        spHandle.setPosition(handleX, handleY);
                        spHandle.setScale(14.0f / static_cast<float>(assets.handle.getSize().x),
                                          20.0f / static_cast<float>(assets.handle.getSize().y));
                        window.draw(spHandle);
                    } else {
                        sf::CircleShape cHandle(7.0f);
                        cHandle.setPosition(handleX, handleY);
                        cHandle.setFillColor(sf::Color(230, 205, 140));
                        cHandle.setOutlineThickness(1.5f);
                        cHandle.setOutlineColor(sf::Color(45, 25, 15));
                        window.draw(cHandle);
                    }
                }

                // Check alarm row
                float chkRowY = 456.0f;
                sf::Text chkLabel;
                chkLabel.setFont(fBold);
                chkLabel.setString(toUtf8(tiengViet ? "Báo chiếu" : "Check Alert"));
                chkLabel.setCharacterSize(16);
                chkLabel.setStyle(sf::Text::Bold);
                chkLabel.setFillColor(sf::Color(35, 18, 10));
                chkLabel.setPosition(664.0f, chkRowY);
                window.draw(chkLabel);

                std::string chkStr = getSettingStatusStr ? getSettingStatusStr(9) : (tiengViet ? "BẬT" : "ON");
                bool chkOn = (chkStr == "BẬT" || chkStr == "ON");

                sf::RectangleShape rChkBadge(sf::Vector2f(40.0f, 22.0f));
                rChkBadge.setPosition(822.0f, chkRowY);
                rChkBadge.setFillColor(sf::Color(246, 236, 215));
                rChkBadge.setOutlineThickness(1.5f);
                rChkBadge.setOutlineColor(sf::Color(45, 25, 15));
                window.draw(rChkBadge);

                sf::Text chkValText;
                chkValText.setFont(fBold);
                chkValText.setString(toUtf8(chkStr));
                chkValText.setCharacterSize(14);
                chkValText.setStyle(sf::Text::Bold);
                chkValText.setFillColor(chkOn ? sf::Color(175, 25, 20) : sf::Color(35, 18, 10));
                sf::FloatRect ctb = chkValText.getLocalBounds();
                chkValText.setOrigin(std::round(ctb.left + ctb.width / 2.0f), std::round(ctb.top + ctb.height / 2.0f));
                chkValText.setPosition(842.0f, chkRowY + 11.0f);
                window.draw(chkValText);

                const sf::Texture& chkTex = chkOn ? assets.toggleOn : assets.toggleOff;
                if (chkTex.getSize().x > 0) {
                    sf::Sprite spToggle(chkTex);
                    spToggle.setPosition(870.0f, chkRowY);
                    spToggle.setScale(40.0f / static_cast<float>(chkTex.getSize().x),
                                      22.0f / static_cast<float>(chkTex.getSize().y));
                    window.draw(spToggle);
                }
            }
            else if (settingsSubTab == 1) { // Graphics
                sf::Text gNote;
                gNote.setFont(fBold);
                gNote.setString(toUtf8(tiengViet ? "Phong cách Á Đông\n16-bit Cổ Phong" : "16-bit Oriental\nPavilion Aesthetic"));
                gNote.setCharacterSize(16);
                gNote.setStyle(sf::Text::Bold);
                gNote.setFillColor(sf::Color(35, 18, 10));
                gNote.setLineSpacing(1.25f);
                sf::FloatRect gnb = gNote.getLocalBounds();
                gNote.setOrigin(std::round(gnb.left + gnb.width / 2.0f), std::round(gnb.top + gnb.height / 2.0f));
                gNote.setPosition(rightCenterX, 502.0f);
                window.draw(gNote);
            }
            else if (settingsSubTab == 3) { // Controls
                drawParchmentHeader(tiengViet ? "ĐIỀU KHIỂN" : "BOARD CONTROLS", 666.0f, 300.0f, 20, false);

                sf::RectangleShape div(sf::Vector2f(248.0f, 2.0f));
                div.setPosition(666.0f, 328.0f);
                div.setFillColor(sf::Color(45, 25, 15, 150));
                window.draw(div);

                const std::vector<std::string> ctrlItems = tiengViet ? std::vector<std::string>{
                    "• Di chuyển: WASD / Mũi tên",
                    "• Chọn / Hạ cờ: Enter / Space",
                    "• Hủy / Quay lại: ESC",
                    "• Hoàn tác: Ctrl + Z"
                } : std::vector<std::string>{
                    "• Move: WASD / Arrows",
                    "• Select / Drop: Enter / Space",
                    "• Deselect / Back: ESC",
                    "• Undo Move: Ctrl + Z"
                };

                for (size_t i = 0; i < ctrlItems.size(); ++i) {
                    sf::Text cTxt;
                    cTxt.setFont(fBold);
                    cTxt.setString(toUtf8(ctrlItems[i]));
                    cTxt.setCharacterSize(16);
                    cTxt.setStyle(sf::Text::Bold);
                    cTxt.setFillColor(sf::Color(35, 18, 10));
                    cTxt.setPosition(666.0f, 340.0f + static_cast<float>(i) * 29.0f);
                    window.draw(cTxt);
                }

                sf::Text promptTxt;
                promptTxt.setFont(fBold);
                promptTxt.setString(toUtf8(tiengViet ? "Chọn bộ phím nhanh:" : "Select key preset:"));
                promptTxt.setCharacterSize(16);
                promptTxt.setStyle(sf::Text::Bold);
                promptTxt.setFillColor(sf::Color(175, 25, 20));
                promptTxt.setPosition(666.0f, 464.0f);
                window.draw(promptTxt);
            }

            for (auto& btn : rightPageButtons) {
                btn.draw(window);
            }
        }
        else if (activeSection == MenuSection::INFO) {
            drawEbonyOrFallback(sf::FloatRect(652.0f, 288.0f, 276.0f, 326.0f), OrientalPanelStyle::PARCHMENT_SCROLL);

            if (infoSubTab == 0) {
                drawParchmentHeader(tiengViet ? "ĐỒ ÁN OOP 2026" : "OOP PROJECT 2026", 666.0f, 300.0f, 20, false);

                sf::Text h2;
                h2.setFont(fBold);
                h2.setString(toUtf8(tiengViet ? "ĐH KHTN ĐHQG-HCM" : "HCMUS - VNUHCM"));
                h2.setCharacterSize(16);
                h2.setStyle(sf::Text::Bold);
                h2.setFillColor(sf::Color(35, 18, 10));
                h2.setPosition(666.0f, 326.0f);
                window.draw(h2);

                sf::RectangleShape div(sf::Vector2f(248.0f, 2.0f));
                div.setPosition(666.0f, 354.0f);
                div.setFillColor(sf::Color(45, 25, 15, 150));
                window.draw(div);

                drawParchmentHeader(tiengViet ? "MẪU THIẾT KẾ:" : "DESIGN PATTERNS:", 666.0f, 364.0f, 18, false);

                const std::vector<std::string> patterns = tiengViet ? std::vector<std::string>{
                    "1. State: Luồng ván & menu",
                    "2. Command: Undo / Redo",
                    "3. Strategy: AI Alpha-Beta",
                    "4. Factory: 7 loại quân cờ",
                    "5. Singleton: Âm & Cài đặt"
                } : std::vector<std::string>{
                    "1. State: Menu & game flow",
                    "2. Command: Undo / Redo",
                    "3. Strategy: AI Alpha-Beta",
                    "4. Factory: 7 piece classes",
                    "5. Singleton: Audio/Config"
                };

                for (size_t i = 0; i < patterns.size(); ++i) {
                    sf::Text pTxt;
                    pTxt.setFont(fBold);
                    pTxt.setString(toUtf8(patterns[i]));
                    pTxt.setCharacterSize(16);
                    pTxt.setStyle(sf::Text::Bold);
                    pTxt.setFillColor(sf::Color(35, 18, 10));
                    pTxt.setPosition(666.0f, 392.0f + static_cast<float>(i) * 32.0f);
                    window.draw(pTxt);
                }

                sf::Text techTxt;
                techTxt.setFont(fBold);
                techTxt.setString(toUtf8("C++17  •  SFML 2.6  •  OOP"));
                techTxt.setCharacterSize(15);
                techTxt.setStyle(sf::Text::Bold);
                techTxt.setFillColor(sf::Color(115, 45, 20));
                sf::FloatRect tb = techTxt.getLocalBounds();
                techTxt.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));
                techTxt.setPosition(rightCenterX, 586.0f);
                window.draw(techTxt);
            } else {
                drawParchmentHeader(tiengViet ? "LUẬT 7 QUÂN CỜ" : "XIANGQI RULES", 666.0f, 298.0f, 20, false);

                sf::RectangleShape div(sf::Vector2f(248.0f, 2.0f));
                div.setPosition(666.0f, 326.0f);
                div.setFillColor(sf::Color(45, 25, 15, 150));
                window.draw(div);

                const std::vector<std::string> rules = tiengViet ? std::vector<std::string>{
                    "• TƯỚNG: 1 ô Cung; cấm lộ mặt.",
                    "• SĨ: Chéo 1 ô trong Cung.",
                    "• TƯỢNG: Chéo 2 ô, không sang sông.",
                    "• XE: Ngang dọc khi trống.",
                    "• PHÁO: Nhảy qua 1 ngòi để ăn.",
                    "• MÃ: Chữ L; bị cản chân.",
                    "• TỐT: Tiến 1; qua sông đi ngang."
                } : std::vector<std::string>{
                    "• KING: 1 step in Palace.",
                    "• ADVISOR: 1 diag in Palace.",
                    "• ELEPHANT: 2 diag, own side.",
                    "• CHARIOT: Straight lines.",
                    "• CANNON: Jump 1 to capture.",
                    "• HORSE: L-move; blockable.",
                    "• SOLDIER: Fwd; side past river."
                };

                for (size_t i = 0; i < rules.size(); ++i) {
                    sf::Text rTxt;
                    rTxt.setFont(fBold);
                    rTxt.setString(toUtf8(rules[i]));
                    rTxt.setCharacterSize(16);
                    rTxt.setStyle(sf::Text::Bold);
                    rTxt.setFillColor(sf::Color(35, 18, 10));
                    rTxt.setPosition(666.0f, 340.0f + static_cast<float>(i) * 35.0f);
                    window.draw(rTxt);
                }
            }

            for (auto& btn : rightPageButtons) {
                btn.draw(window);
            }
        }
        else if (activeSection == MenuSection::EXIT) {
            drawEbonyOrFallback(sf::FloatRect(652.0f, 250.0f, 276.0f, 236.0f), OrientalPanelStyle::PARCHMENT_SCROLL);

            sf::CircleShape exitRing(34.0f);
            exitRing.setFillColor(sf::Color(225, 198, 158));
            exitRing.setOutlineThickness(2.5f);
            exitRing.setOutlineColor(sf::Color(175, 25, 20));
            exitRing.setOrigin(34.0f, 34.0f);
            exitRing.setPosition(rightCenterX, 316.0f);
            window.draw(exitRing);

            if (assets.iconExit.getSize().x > 0) {
                sf::Sprite ic(assets.iconExit);
                sf::Vector2u isz = assets.iconExit.getSize();
                ic.setOrigin(isz.x / 2.0f, isz.y / 2.0f);
                ic.setPosition(rightCenterX, 316.0f);
                ic.setScale(1.4f, 1.4f);
                window.draw(ic);
            }

            sf::Text eTxt;
            eTxt.setFont(fBold);
            std::string q = tiengViet
                ? "Bạn chắc chắn muốn thoát?\nThiết lập đã lưu sẽ được giữ."
                : "Are you sure you want to exit?\nSaved settings will be kept.";
            eTxt.setString(toUtf8(q));
            eTxt.setCharacterSize(16);
            eTxt.setStyle(sf::Text::Bold);
            eTxt.setFillColor(sf::Color(35, 18, 10));
            eTxt.setLineSpacing(1.28f);
            sf::FloatRect etb = eTxt.getLocalBounds();
            eTxt.setOrigin(std::round(etb.left + etb.width / 2.0f), std::round(etb.top + etb.height / 2.0f));
            eTxt.setPosition(rightCenterX, 418.0f);
            window.draw(eTxt);

            for (auto& btn : rightPageButtons) {
                btn.draw(window);
            }
        }
    }

    // 5. Minimalist Dark Footer Strip (1200x800)
    sf::RectangleShape footerBar(sf::Vector2f(1200.0f, 40.0f));
    footerBar.setPosition(0.0f, 760.0f);
    footerBar.setFillColor(sf::Color(12, 8, 6, 205));
    window.draw(footerBar);

    sf::RectangleShape footerLine(sf::Vector2f(1200.0f, 1.5f));
    footerLine.setPosition(0.0f, 760.0f);
    footerLine.setFillColor(sf::Color(185, 145, 65, 180));
    window.draw(footerLine);

    sf::Text footerTxt;
    footerTxt.setFont(fBold);
    std::string footerPrompt = (isBookClosed || bookProgress < 0.7f)
        ? (tiengViet ? "[W/S] Di chuyển   |   [Enter] Chọn   |   [ESC] Quay lại" : "[W/S] Navigate   |   [Enter] Select   |   [ESC] Back")
        : (tiengViet ? "[W/S] Lên/Xuống   |   [A/D] Đổi bảng   |   [Enter] Chọn   |   [ESC] Thu gọn" : "[W/S] Navigate   |   [A/D] Switch Panel   |   [Enter] Select   |   [ESC] Back");
    footerTxt.setString(toUtf8(footerPrompt));
    footerTxt.setCharacterSize(14);
    footerTxt.setStyle(sf::Text::Bold);
    footerTxt.setFillColor(sf::Color(240, 215, 155));
    sf::FloatRect ftb = footerTxt.getLocalBounds();
    footerTxt.setOrigin(std::round(ftb.left + ftb.width / 2.0f), std::round(ftb.top + ftb.height / 2.0f));
    footerTxt.setPosition(600.0f, 780.0f);
    window.draw(footerTxt);
}
