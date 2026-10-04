#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>
#include <algorithm>
#include <cmath>
#include <sstream>

class Slider {
private:
    float posX, posY;
    float trackWidth, trackHeight;
    float minValue, maxValue;
    float currentValue;
    bool isDragging;
    std::string title;
    sf::Font& font;
    std::function<void(float)> onValueChanged;

public:
    Slider(float x, float y, float w, float h, float minVal, float maxVal, float initialVal,
           const std::string& label, sf::Font& f, std::function<void(float)> callback = nullptr)
        : posX(x), posY(y), trackWidth(w), trackHeight(h),
          minValue(minVal), maxValue(maxVal), currentValue(initialVal),
          isDragging(false), title(label), font(f), onValueChanged(callback) {
        clampValue();
    }

    void setPosition(float x, float y) { posX = x; posY = y; }
    void setTitle(const std::string& t) { title = t; }
    void setLabel(const std::string& l) { title = l; }
    std::string getTitle() const { return title; }

    void setValue(float val) {
        currentValue = val;
        clampValue();
    }
    float getValue() const { return currentValue; }

    void handleEvent(const sf::Event& event, const sf::Vector2i& mousePos) {
        sf::FloatRect bounds(posX - 10.0f, posY - 10.0f, trackWidth + 20.0f, trackHeight + 20.0f);
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            if (bounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                isDragging = true;
                updateFromMouse(mousePos.x);
            }
        } else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
            isDragging = false;
        } else if (event.type == sf::Event::MouseMoved) {
            if (isDragging) {
                updateFromMouse(mousePos.x);
            }
        }
    }

    void draw(sf::RenderWindow& window) {
        // 1. Draw Title
        sf::Text labelText;
        labelText.setFont(font);
        labelText.setCharacterSize(16);
        labelText.setStyle(sf::Text::Bold);
        labelText.setFillColor(sf::Color(220, 235, 250));
        labelText.setString(title);
        labelText.setPosition(posX, posY - 24.0f);
        window.draw(labelText);

        // 2. Draw Value Percentage on the right
        std::ostringstream ss;
        ss << static_cast<int>(std::round(currentValue)) << "%";
        sf::Text valText;
        valText.setFont(font);
        valText.setCharacterSize(16);
        valText.setStyle(sf::Text::Bold);
        valText.setFillColor(sf::Color(240, 190, 70)); // Gold
        valText.setString(ss.str());
        sf::FloatRect vb = valText.getLocalBounds();
        valText.setPosition(posX + trackWidth - vb.width, posY - 24.0f);
        window.draw(valText);

        // 3. Draw Track Background
        sf::RectangleShape trackBg(sf::Vector2f(trackWidth, trackHeight));
        trackBg.setPosition(posX, posY);
        trackBg.setFillColor(sf::Color(22, 28, 40));
        trackBg.setOutlineThickness(1.5f);
        trackBg.setOutlineColor(sf::Color(65, 85, 120));
        window.draw(trackBg);

        // 4. Draw Filled Progress Bar (Active track)
        float progressFraction = (maxValue > minValue) ? ((currentValue - minValue) / (maxValue - minValue)) : 0.0f;
        progressFraction = std::clamp(progressFraction, 0.0f, 1.0f);
        float fillW = trackWidth * progressFraction;

        if (fillW > 0.0f) {
            sf::RectangleShape fillTrack(sf::Vector2f(fillW, trackHeight));
            fillTrack.setPosition(posX, posY);
            fillTrack.setFillColor(sf::Color(240, 190, 70)); // Gold bar
            window.draw(fillTrack);
        }

        // 5. Draw Thumb Knob
        float knobRadius = trackHeight * 0.9f;
        float knobX = posX + fillW;
        float knobY = posY + trackHeight / 2.0f;

        // Outer glow when dragging
        if (isDragging) {
            sf::CircleShape glow(knobRadius + 5.0f);
            glow.setOrigin(knobRadius + 5.0f, knobRadius + 5.0f);
            glow.setPosition(knobX, knobY);
            glow.setFillColor(sf::Color(240, 190, 70, 90));
            window.draw(glow);
        }

        // Knob body
        sf::CircleShape knob(knobRadius);
        knob.setOrigin(knobRadius, knobRadius);
        knob.setPosition(knobX, knobY);
        knob.setFillColor(sf::Color(255, 235, 170));
        knob.setOutlineThickness(2.5f);
        knob.setOutlineColor(sf::Color(200, 140, 30));
        window.draw(knob);

        // Knob center dot
        sf::CircleShape knobCenter(knobRadius * 0.35f);
        knobCenter.setOrigin(knobRadius * 0.35f, knobRadius * 0.35f);
        knobCenter.setPosition(knobX, knobY);
        knobCenter.setFillColor(sf::Color(160, 95, 15));
        window.draw(knobCenter);
    }

private:
    void clampValue() {
        if (currentValue < minValue) currentValue = minValue;
        if (currentValue > maxValue) currentValue = maxValue;
    }

    void updateFromMouse(int mouseX) {
        float relX = static_cast<float>(mouseX) - posX;
        float fraction = relX / trackWidth;
        fraction = std::clamp(fraction, 0.0f, 1.0f);
        currentValue = minValue + fraction * (maxValue - minValue);
        clampValue();
        if (onValueChanged) {
            onValueChanged(currentValue);
        }
    }
};
