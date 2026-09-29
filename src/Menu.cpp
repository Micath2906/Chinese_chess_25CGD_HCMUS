#include "Menu.h"
#include <iostream>

// ============================================================
// Button Implementation
// ============================================================

Button::Button(float x, float y, float width, float height,
               const std::string& label, sf::Font& font,
               std::function<void()> onClick)
    : callback(onClick), hovered(false), enabled(true) {
    
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
    text.setCharacterSize(20);
    text.setFillColor(sf::Color::White);
    
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setOrigin(textBounds.left + textBounds.width / 2.0f,
                   textBounds.top + textBounds.height / 2.0f);
    text.setPosition(x + width / 2.0f, y + height / 2.0f);
}

void Button::setPosition(float x, float y) {
    shape.setPosition(x, y);
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setOrigin(textBounds.left + textBounds.width / 2.0f,
                   textBounds.top + textBounds.height / 2.0f);
    text.setPosition(x + shape.getSize().x / 2.0f, 
                     y + shape.getSize().y / 2.0f);
}

void Button::setLabel(const std::string& label) {
    text.setString(label);
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setOrigin(textBounds.left + textBounds.width / 2.0f,
                   textBounds.top + textBounds.height / 2.0f);
    text.setPosition(shape.getPosition().x + shape.getSize().x / 2.0f,
                     shape.getPosition().y + shape.getSize().y / 2.0f);
}

void Button::setCharacterSize(unsigned int size) {
    text.setCharacterSize(size);
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setOrigin(textBounds.left + textBounds.width / 2.0f,
                   textBounds.top + textBounds.height / 2.0f);
    text.setPosition(shape.getPosition().x + shape.getSize().x / 2.0f,
                     shape.getPosition().y + shape.getSize().y / 2.0f);
}

void Button::setColors(sf::Color normal, sf::Color hover, sf::Color pressed) {
    colorNormal = normal;
    colorHover = hover;
    colorPressed = pressed;
    if (enabled) {
        shape.setFillColor(hovered ? colorHover : colorNormal);
    }
}

void Button::update(const sf::Vector2i& mousePos) {
    if (!enabled) {
        shape.setFillColor(colorDisabled);
        text.setFillColor(sf::Color(140, 140, 140));
        return;
    }
    
    text.setFillColor(sf::Color::White);
    hovered = contains(mousePos);
    
    if (hovered) {
        shape.setFillColor(colorHover);
    } else {
        shape.setFillColor(colorNormal);
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
    window.draw(shape);
    window.draw(text);
}

bool Button::contains(const sf::Vector2i& point) const {
    return shape.getGlobalBounds().contains(
        static_cast<float>(point.x), 
        static_cast<float>(point.y)
    );
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
    if (!font.loadFromFile(fontPath)) {
        std::cerr << "Khong the load font: " << fontPath << std::endl;
        return false;
    }
    title.setFont(font);
    return true;
}

void Menu::themButton(float x, float y, float width, float height,
                      const std::string& label, std::function<void()> onClick) {
    buttons.emplace_back(x, y, width, height, label, font, onClick);
}

void Menu::update(const sf::Vector2i& mousePos) {
    for (auto& button : buttons) {
        button.update(mousePos);
    }
}

void Menu::handleClick(const sf::Vector2i& mousePos) {
    for (auto& button : buttons) {
        button.handleClick(mousePos);
    }
}

void Menu::draw(sf::RenderWindow& window) {
    window.draw(title);
    for (auto& button : buttons) {
        button.draw(window);
    }
}
