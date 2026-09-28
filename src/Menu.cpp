#include "Menu.h"
#include <iostream>

// ============================================================
// Button Implementation
// ============================================================

Button::Button(float x, float y, float width, float height,
               const std::string& label, sf::Font& font,
               std::function<void()> onClick)
    : callback(onClick), hovered(false) {
    
    shape.setSize(sf::Vector2f(width, height));
    shape.setPosition(x, y);
    
    colorNormal = sf::Color(70, 130, 180);
    colorHover = sf::Color(100, 160, 210);
    colorPressed = sf::Color(50, 100, 150);
    
    shape.setFillColor(colorNormal);
    shape.setOutlineThickness(3);
    shape.setOutlineColor(sf::Color::White);
    
    text.setFont(font);
    text.setString(label);
    text.setCharacterSize(24);
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

void Button::update(const sf::Vector2i& mousePos) {
    hovered = contains(mousePos);
    
    if (hovered) {
        shape.setFillColor(colorHover);
    } else {
        shape.setFillColor(colorNormal);
    }
}

void Button::handleClick(const sf::Vector2i& mousePos) {
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
    title.setCharacterSize(48);
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
