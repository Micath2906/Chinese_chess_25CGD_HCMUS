#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <functional>

class Button {
private:
    sf::RectangleShape shape;
    sf::Text text;
    std::function<void()> callback;
    bool hovered;
    bool enabled;
    
    sf::Color colorNormal;
    sf::Color colorHover;
    sf::Color colorPressed;
    sf::Color colorDisabled;
    
public:
    Button(float x, float y, float width, float height, 
           const std::string& label, sf::Font& font,
           std::function<void()> onClick);
    
    void setPosition(float x, float y);
    void setLabel(const std::string& label);
    void setColors(sf::Color normal, sf::Color hover, sf::Color pressed);
    void setEnabled(bool en) { enabled = en; }
    bool isEnabled() const { return enabled; }

    void update(const sf::Vector2i& mousePos);
    void handleClick(const sf::Vector2i& mousePos);
    void draw(sf::RenderWindow& window);
    
    bool contains(const sf::Vector2i& point) const;
};

class Menu {
private:
    sf::Font font;
    sf::Text title;
    std::vector<Button> buttons;
    
public:
    Menu();
    bool loadFont(const std::string& fontPath);
    void setFont(const sf::Font& f) { font = f; title.setFont(font); }
    
    void themButton(float x, float y, float width, float height,
                    const std::string& label, std::function<void()> onClick);
    void xoaTatCaButton() { buttons.clear(); }
    Button* layButton(size_t index) { return (index < buttons.size()) ? &buttons[index] : nullptr; }
    
    void update(const sf::Vector2i& mousePos);
    void handleClick(const sf::Vector2i& mousePos);
    void draw(sf::RenderWindow& window);
    
    sf::Font& getFont() { return font; }
};
