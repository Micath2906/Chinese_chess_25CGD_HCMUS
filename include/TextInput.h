#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class TextInput {
private:
    sf::RectangleShape box;
    sf::Text text;
    sf::Text placeholderText;
    std::string value;
    std::string placeholder;
    bool focused;
    size_t maxLength;
    sf::Clock blinkClock;
    
public:
    TextInput(float x, float y, float w, float h, sf::Font& font,
              const std::string& placeholder = "", size_t maxLen = 16);

    void setPosition(float x, float y);
    void setSize(float w, float h);
    void setPlaceholder(const std::string& p);
    void setValue(const std::string& v);
    std::string getValue() const { return value; }

    bool isFocused() const { return focused; }
    void setFocused(bool f) { focused = f; }

    bool handleClick(const sf::Vector2i& mousePos);
    void handleTextEntered(sf::Uint32 unicode);
    void draw(sf::RenderWindow& window);
};
