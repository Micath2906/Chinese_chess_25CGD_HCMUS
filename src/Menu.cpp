#include "Menu.h"
#include <iostream>
#include <vector>

static bool loadTexSafe(sf::Texture& tex, const std::string& path) {
    const std::vector<std::string> prefixes = { "", "../", "../../", "../../../", "../../../../" };
    for (const auto& p : prefixes) {
        if (tex.loadFromFile(p + path)) {
            tex.setSmooth(true);
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

struct MenuAssets {
    sf::Texture cleanBg;          // Nền sạch background.png
    sf::Texture titleEn;          // Title tiếng Anh chinese_chess.png
    sf::Texture titleVi;          // Title tiếng Việt co_tuong.png
    sf::Texture btnNormal;        // Nút xanh lá thường
    sf::Texture btnHover;         // Hover
    sf::Texture boardWood;        // Medieval Fantasy UI board
    sf::Font globalFont;          // Font zpix
    bool fontLoaded = false;

    MenuAssets() {
        loadTexSafe(cleanBg, "assets/background.png") || loadTexSafe(cleanBg, "resources/textures/background.png");
        loadTexSafe(titleEn, "assets/chinese_chess_2.png") || loadTexSafe(titleEn, "resources/textures/chinese_chess_2.png");
        loadTexSafe(titleVi, "assets/co_tuong_2.png") || loadTexSafe(titleVi, "resources/textures/co_tuong_2.png");

        // Đã trả lại nút xanh lá như bạn yêu cầu
        loadTexSafe(btnNormal, "assets/button_menu_nor2.png");
        loadTexSafe(btnHover, "assets/button_menu_hol2.png");
        loadTexSafe(boardWood, "assets/panel_brown_window.png");

        // Nạp font zpix.ttf
        fontLoaded = loadFontSafe(globalFont, "assets/font/zpix.ttf") ||
            loadFontSafe(globalFont, "resources/font/zpix.ttf") ||
            loadFontSafe(globalFont, "assets/zpix.ttf");
    }
};

static MenuAssets& getMenuAssets() {
    static MenuAssets assets;
    return assets;
}

void veNenMenuGiaoDien(sf::RenderWindow& window) {
    auto& assets = getMenuAssets();
    if (assets.cleanBg.getSize().x > 0) {
        sf::Sprite sp(assets.cleanBg);
        sf::Vector2u winSz = window.getSize();
        sf::Vector2u texSz = assets.cleanBg.getSize();
        sp.setScale(static_cast<float>(winSz.x) / texSz.x, static_cast<float>(winSz.y) / texSz.y);
        window.draw(sp);
    }
}

// ============================================================
// Button Implementation
// ============================================================
void veBangGo(sf::RenderWindow& window, float x, float y, float w, float h) {
    auto& assets = getMenuAssets();
    if (assets.boardWood.getSize().x == 0) {
        sf::RectangleShape fallback(sf::Vector2f(w, h));
        fallback.setPosition(x, y);
        fallback.setFillColor(sf::Color(55, 38, 22, 235));
        fallback.setOutlineThickness(2.0f);
        fallback.setOutlineColor(sf::Color(140, 95, 45));
        window.draw(fallback);
        return;
    }

    const float c = 14.0f;
    const float tw = static_cast<float>(assets.boardWood.getSize().x);
    const float th = static_cast<float>(assets.boardWood.getSize().y);

    sf::Sprite sp(assets.boardWood);

    // 1. Top-Left
    sp.setTextureRect(sf::IntRect(0, 0, static_cast<int>(c), static_cast<int>(c)));
    sp.setPosition(x, y);
    sp.setScale(1.0f, 1.0f);
    window.draw(sp);

    // 2. Top-Right
    sp.setTextureRect(sf::IntRect(static_cast<int>(tw - c), 0, static_cast<int>(c), static_cast<int>(c)));
    sp.setPosition(x + w - c, y);
    sp.setScale(1.0f, 1.0f);
    window.draw(sp);

    // 3. Bottom-Left
    sp.setTextureRect(sf::IntRect(0, static_cast<int>(th - c), static_cast<int>(c), static_cast<int>(c)));
    sp.setPosition(x, y + h - c);
    sp.setScale(1.0f, 1.0f);
    window.draw(sp);

    // 4. Bottom-Right
    sp.setTextureRect(sf::IntRect(static_cast<int>(tw - c), static_cast<int>(th - c), static_cast<int>(c), static_cast<int>(c)));
    sp.setPosition(x + w - c, y + h - c);
    sp.setScale(1.0f, 1.0f);
    window.draw(sp);

    // 5. Top edge
    if (w > 2 * c) {
        sp.setTextureRect(sf::IntRect(static_cast<int>(c), 0, static_cast<int>(tw - 2 * c), static_cast<int>(c)));
        sp.setPosition(x + c, y);
        sp.setScale((w - 2 * c) / (tw - 2 * c), 1.0f);
        window.draw(sp);
    }

    // 6. Bottom edge
    if (w > 2 * c) {
        sp.setTextureRect(sf::IntRect(static_cast<int>(c), static_cast<int>(th - c), static_cast<int>(tw - 2 * c), static_cast<int>(c)));
        sp.setPosition(x + c, y + h - c);
        sp.setScale((w - 2 * c) / (tw - 2 * c), 1.0f);
        window.draw(sp);
    }

    // 7. Left edge
    if (h > 2 * c) {
        sp.setTextureRect(sf::IntRect(0, static_cast<int>(c), static_cast<int>(c), static_cast<int>(th - 2 * c)));
        sp.setPosition(x, y + c);
        sp.setScale(1.0f, (h - 2 * c) / (th - 2 * c));
        window.draw(sp);
    }

    // 8. Right edge
    if (h > 2 * c) {
        sp.setTextureRect(sf::IntRect(static_cast<int>(tw - c), static_cast<int>(c), static_cast<int>(c), static_cast<int>(th - 2 * c)));
        sp.setPosition(x + w - c, y + c);
        sp.setScale(1.0f, (h - 2 * c) / (th - 2 * c));
        window.draw(sp);
    }

    // 9. Center
    if (w > 2 * c && h > 2 * c) {
        sp.setTextureRect(sf::IntRect(static_cast<int>(c), static_cast<int>(c), static_cast<int>(tw - 2 * c), static_cast<int>(th - 2 * c)));
        sp.setPosition(x + c, y + c);
        sp.setScale((w - 2 * c) / (tw - 2 * c), (h - 2 * c) / (th - 2 * c));
        window.draw(sp);
    }
}

void Button::canGiuaText() {
    sf::FloatRect textBounds = text.getLocalBounds();
    // Chú ý: Cộng thêm textBounds.top và left để bù sai số của font pixel
    text.setOrigin(textBounds.left + textBounds.width / 2.0f,
        textBounds.top + textBounds.height / 2.0f);

    // Đặt vào tâm nút (thêm offset Y nếu viền dưới nút của bạn dày hơn viền trên)
    // Nếu chữ vẫn hơi cao/thấp, bạn có thể chỉnh "- 2.0f" hoặc "+ 2.0f" ở tọa độ Y
    text.setPosition(shape.getPosition().x + shape.getSize().x / 2.0f,
        shape.getPosition().y + shape.getSize().y / 2.0f + 5.0f);
}

Button::Button(float x, float y, float width, float height,
    const std::string& label, sf::Font& font,
    std::function<void()> onClick)
    : callback(onClick), hovered(false), enabled(true) {

    auto& assets = getMenuAssets();
    texNormal = &assets.btnNormal;
    texHover = &assets.btnHover;

    shape.setSize(sf::Vector2f(width, height));
    shape.setPosition(x, y);

    colorNormal = sf::Color(65, 105, 165);
    colorHover = sf::Color(90, 140, 205);
    colorPressed = sf::Color(45, 80, 135);
    colorDisabled = sf::Color(70, 70, 70);

    shape.setFillColor(colorNormal);
    shape.setOutlineThickness(0.0f);

    if (assets.fontLoaded) {
        text.setFont(assets.globalFont);
    }
    else {
        text.setFont(font);
    }

    text.setString(label);
    text.setCharacterSize(22);
    text.setStyle(sf::Text::Bold);
    text.setFillColor(sf::Color::White);

    canGiuaText();
}

void Button::setPosition(float x, float y) {
    shape.setPosition(x, y);
    canGiuaText();
}

void Button::setLabel(const std::string& label) {
    text.setString(label);
    canGiuaText();
}

void Button::setCharacterSize(unsigned int size) {
    text.setCharacterSize(size);
    canGiuaText();
}

void Button::setColors(sf::Color normal, sf::Color hover, sf::Color pressed) {
    colorNormal = normal; colorHover = hover; colorPressed = pressed;
    if (enabled) shape.setFillColor(hovered ? colorHover : colorNormal);
}

void Button::update(const sf::Vector2i& mousePos) {
    if (!enabled) {
        shape.setFillColor(colorDisabled);
        text.setFillColor(sf::Color(140, 140, 140));
        return;
    }
    hovered = contains(mousePos);
    if (hovered) {
        shape.setFillColor(colorHover);
        text.setFillColor(sf::Color(255, 240, 150));
    }
    else {
        shape.setFillColor(colorNormal);
        text.setFillColor(sf::Color::White);
    }
}

void Button::handleClick(const sf::Vector2i& mousePos) {
    if (!enabled) return;
    if (contains(mousePos) && callback) {
        shape.setFillColor(colorPressed);
        callback();
    }
}

void Button::draw(sf::RenderWindow& window) {
    const sf::Texture* activeTex = (hovered && texHover && texHover->getSize().x > 0) ? texHover : texNormal;

    if (activeTex && activeTex->getSize().x > 0 && enabled) {
        sf::Sprite sp(*activeTex);
        sf::Vector2f pos = shape.getPosition();
        sf::Vector2f sz = shape.getSize();
        sf::Vector2u texSz = activeTex->getSize();

        sp.setPosition(pos);
        sp.setScale(sz.x / static_cast<float>(texSz.x), sz.y / static_cast<float>(texSz.y));
        window.draw(sp);
    }
    else {
        window.draw(shape);
    }
    window.draw(text);
}

bool Button::contains(const sf::Vector2i& point) const {
    return shape.getGlobalBounds().contains(static_cast<float>(point.x), static_cast<float>(point.y));
}

// ============================================================
// Menu Implementation
// ============================================================
Menu::Menu() {
    auto& assets = getMenuAssets();
    if (assets.fontLoaded) {
        font = assets.globalFont;
    }
    title.setFont(font);
    title.setCharacterSize(44);
    title.setFillColor(sf::Color::White);
    title.setStyle(sf::Text::Bold);
}

bool Menu::loadFont(const std::string& fontPath) {
    return loadFontSafe(font, fontPath);
}

void Menu::themButton(float x, float y, float width, float height,
    const std::string& label, std::function<void()> onClick) {
    buttons.emplace_back(x, y, width, height, label, font, onClick);
}

void Menu::update(const sf::Vector2i& mousePos) {
    for (auto& button : buttons) button.update(mousePos);
}

void Menu::handleClick(const sf::Vector2i& mousePos) {
    for (auto& button : buttons) button.handleClick(mousePos);
}

void Menu::draw(sf::RenderWindow& window, bool tiengViet) {
    auto& assets = getMenuAssets();
    sf::Vector2u winSz = window.getSize();

    // 1. Vẽ nền sạch
    if (assets.cleanBg.getSize().x > 0) {
        sf::Sprite bgSp(assets.cleanBg);
        sf::Vector2u texSz = assets.cleanBg.getSize();
        bgSp.setScale(static_cast<float>(winSz.x) / texSz.x, static_cast<float>(winSz.y) / texSz.y);
        window.draw(bgSp);
    }

    // 2. Nếu là Menu Chính -> Vẽ Title theo ngôn ngữ (file đã được căn chuẩn)
    if (laMenuChinh) {
        const sf::Texture* titleTex = tiengViet ? &assets.titleVi : &assets.titleEn;
        if (titleTex && titleTex->getSize().x > 0) {
            sf::Sprite titleSp(*titleTex);
            sf::FloatRect b = titleSp.getLocalBounds();

            // Lấy tâm ở chính giữa bề ngang, đỉnh ở trên cùng
            titleSp.setOrigin(b.width / 2.0f, 0.0f);

            // Đặt cố định ở giữa màn hình ngang, và cách mép trên 10% (hoặc bạn có thể để 20.0f nếu muốn bám sát lên đỉnh)
            titleSp.setPosition(winSz.x / 2.0f, winSz.y * 0.10f);

            window.draw(titleSp);
        }
    }

    // 3. Vẽ nút
    for (auto& button : buttons) {
        button.draw(window);
    }
}