#include "Menu.h"
#include <iostream>
#include <vector>

// Hàm hỗ trợ tìm file an toàn qua nhiều cấp thư mục
static bool loadTexSafe(sf::Texture& tex, const std::string& path) {
    const std::vector<std::string> prefixes = { "", "../", "../../", "../../../", "../../../../" };
    for (const auto& p : prefixes) {
        if (tex.loadFromFile(p + path)) return true;
    }
    return false;
}

// Hàm quản lý tài nguyên an toàn (tránh lỗi Static Initialization Order)
struct MenuAssets {
    sf::Texture menuBg;
    sf::Sprite menuSprite;
    sf::Texture btnNormal;
    sf::Texture btnHover;
    bool loaded = false;

    MenuAssets() {
        // Nạp ảnh nền menu chính
        if (loadTexSafe(menuBg, "resources/textures/first look.png") || loadTexSafe(menuBg, "assets/first look.png")) {
            menuBg.setSmooth(true);
            menuSprite.setTexture(menuBg);
        }

        // Nạp ảnh nút bấm
        loadTexSafe(btnNormal, "resources/textures/play_nhat.png") || loadTexSafe(btnNormal, "assets/play_nhat.png");
        loadTexSafe(btnHover, "resources/textures/play_dam.png") || loadTexSafe(btnHover, "assets/play_dam.png");

        btnNormal.setSmooth(true);
        btnHover.setSmooth(true);
        loaded = true;
    }
};

static MenuAssets& getMenuAssets() {
    static MenuAssets assets;
    return assets;
}

// Hàm vẽ nền màn hình phụ
void veNenMenuGiaoDien(sf::RenderWindow& window) {
    auto& assets = getMenuAssets();
    if (assets.menuBg.getSize().x > 0) {
        sf::Vector2u winSz = window.getSize();
        sf::Vector2u texSz = assets.menuBg.getSize();
        assets.menuSprite.setScale(static_cast<float>(winSz.x) / texSz.x, static_cast<float>(winSz.y) / texSz.y);
        window.draw(assets.menuSprite);
    }
}

// ============================================================
// Button Implementation
// ============================================================
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
    shape.setOutlineThickness(2.0f);
    shape.setOutlineColor(sf::Color(220, 220, 220, 180));

    text.setFont(font);
    text.setString(label);
    text.setCharacterSize(22);
    text.setStyle(sf::Text::Bold);
    text.setFillColor(sf::Color::White);

    sf::FloatRect textBounds = text.getLocalBounds();
    text.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
    text.setPosition(x + width / 2.0f, y + height / 2.0f);
}

void Button::setPosition(float x, float y) {
    shape.setPosition(x, y);
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
    text.setPosition(x + shape.getSize().x / 2.0f, y + shape.getSize().y / 2.0f);
}

void Button::setLabel(const std::string& label) {
    text.setString(label);
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
    text.setPosition(shape.getPosition().x + shape.getSize().x / 2.0f, shape.getPosition().y + shape.getSize().y / 2.0f);
}

void Button::setCharacterSize(unsigned int size) {
    text.setCharacterSize(size);
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
    text.setPosition(shape.getPosition().x + shape.getSize().x / 2.0f, shape.getPosition().y + shape.getSize().y / 2.0f);
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
    title.setCharacterSize(44);
    title.setFillColor(sf::Color::White);
    title.setStyle(sf::Text::Bold);
}

bool Menu::loadFont(const std::string& fontPath) {
    if (!font.loadFromFile(fontPath)) return false;
    title.setFont(font);
    return true;
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

void Menu::draw(sf::RenderWindow& window) {
    auto& assets = getMenuAssets();
    if (assets.menuBg.getSize().x > 0) {
        sf::Vector2u winSz = window.getSize();
        sf::Vector2u texSz = assets.menuBg.getSize();
        assets.menuSprite.setScale(static_cast<float>(winSz.x) / texSz.x, static_cast<float>(winSz.y) / texSz.y);
        window.draw(assets.menuSprite);
    }

    for (auto& button : buttons) {
        button.draw(window);
    }
}