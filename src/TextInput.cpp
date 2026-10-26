#include "TextInput.h"
#include "Localization.h"
#include <cmath>

TextInput::TextInput(float x, float y, float w, float h, sf::Font& font,
                     const std::string& placeholder, size_t maxLen)
    : value(""),
      placeholder(placeholder),
      focused(false),
      maxLength(maxLen),
      parchmentStyle(false) {
    
    box.setPosition(x, y);
    box.setSize(sf::Vector2f(w, h));
    box.setFillColor(sf::Color(25, 30, 42));
    box.setOutlineThickness(1.5f);
    box.setOutlineColor(sf::Color(70, 85, 110));

    text.setFont(font);
    text.setCharacterSize(18);
    text.setStyle(sf::Text::Bold);
    text.setFillColor(sf::Color::White);
    text.setPosition(x + 12.0f, y + (h - 22.0f) / 2.0f);

    placeholderText.setFont(font);
    placeholderText.setCharacterSize(18);
    placeholderText.setStyle(sf::Text::Bold);
    placeholderText.setFillColor(sf::Color(160, 175, 195));
    placeholderText.setString(toUtf8(placeholder));
    placeholderText.setPosition(x + 12.0f, y + (h - 22.0f) / 2.0f);
}

void TextInput::setPosition(float x, float y) {
    box.setPosition(x, y);
    float h = box.getSize().y;
    text.setPosition(x + 12.0f, y + (h - 22.0f) / 2.0f);
    placeholderText.setPosition(x + 12.0f, y + (h - 22.0f) / 2.0f);
}

void TextInput::setSize(float w, float h) {
    box.setSize(sf::Vector2f(w, h));
    float x = box.getPosition().x;
    float y = box.getPosition().y;
    text.setPosition(x + 12.0f, y + (h - 22.0f) / 2.0f);
    placeholderText.setPosition(x + 12.0f, y + (h - 22.0f) / 2.0f);
}

void TextInput::setPlaceholder(const std::string& p) {
    placeholder = p;
    placeholderText.setString(toUtf8(p));
}

void TextInput::setValue(const std::string& v) {
    value = v;
    if (value.size() > maxLength) {
        value = value.substr(0, maxLength);
    }
}

bool TextInput::handleClick(const sf::Vector2i& mousePos) {
    sf::FloatRect bounds = box.getGlobalBounds();
    bool inside = bounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
    focused = inside;
    if (focused) {
        blinkClock.restart();
    }
    return inside;
}

void TextInput::handleTextEntered(sf::Uint32 unicode) {
    if (!focused) return;

    if (unicode == 8) { // Backspace
        if (!value.empty()) {
            value.pop_back();
        }
    } else if (unicode == 13) { // Enter
        focused = false;
    } else if (unicode >= 32 && unicode < 127) { // Printable ASCII
        if (value.size() < maxLength) {
            value += static_cast<char>(unicode);
        }
    }
}

void TextInput::draw(sf::RenderWindow& window) {
    if (parchmentStyle) {
        if (focused) {
            box.setOutlineColor(sf::Color(195, 35, 25));
            box.setFillColor(sf::Color(255, 248, 235));
            text.setFillColor(sf::Color(50, 25, 15));
        } else {
            box.setOutlineColor(sf::Color(170, 140, 105));
            box.setFillColor(sf::Color(245, 235, 220));
            text.setFillColor(sf::Color(55, 30, 18));
            placeholderText.setFillColor(sf::Color(140, 115, 90));
        }
    } else {
        if (focused) {
            box.setOutlineColor(sf::Color(240, 190, 70));
            box.setFillColor(sf::Color(35, 42, 58));
        } else {
            box.setOutlineColor(sf::Color(70, 85, 110));
            box.setFillColor(sf::Color(25, 30, 42));
        }
    }

    window.draw(box);

    if (value.empty() && !focused) {
        window.draw(placeholderText);
    } else {
        std::string displayStr = value;
        if (focused) {
            // Blinking cursor every 500ms
            float t = blinkClock.getElapsedTime().asSeconds();
            if (std::fmod(t, 1.0f) < 0.5f) {
                displayStr += "|";
            }
        }
        text.setString(toUtf8(displayStr));
        window.draw(text);
    }
}
