#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <functional>

// Hàm toàn cục để vẽ nền sạch cho mọi màn hình phụ
void veNenMenuGiaoDien(sf::RenderWindow& window);
void veBangGo(sf::RenderWindow& window, float x, float y, float w, float h);

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

    const sf::Texture* texNormal;
    const sf::Texture* texHover;

    // Hàm căn giữa text tuyệt đối
    void canGiuaText();

public:
    Button(float x, float y, float width, float height,
        const std::string& label, sf::Font& font,
        std::function<void()> onClick);

    void setPosition(float x, float y);
    void setLabel(const std::string& label);
    void setCharacterSize(unsigned int size);
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
    bool laMenuChinh = false;

    Menu();
    bool loadFont(const std::string& fontPath);
    void setFont(const sf::Font& f) { font = f; title.setFont(font); }
    void setTitle(const std::string& t) { title.setString(t); }

    void themButton(float x, float y, float width, float height,
        const std::string& label, std::function<void()> onClick);
    void xoaTatCaButton() { buttons.clear(); }
    Button* layButton(size_t index) { return (index < buttons.size()) ? &buttons[index] : nullptr; }

    void update(const sf::Vector2i& mousePos);
    void handleClick(const sf::Vector2i& mousePos);
    // Thêm tham số tiengViet để đổi Title
    void draw(sf::RenderWindow& window, bool tiengViet = false);
};
