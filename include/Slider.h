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
    void setWidth(float w) { trackWidth = w; }
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
        // Draw title drop shadow & text with proper UTF-8 decoding
        sf::String utf8Title = sf::String::fromUtf8(title.begin(), title.end());

        sf::Text labelShadow;
        labelShadow.setFont(font);
        labelShadow.setCharacterSize(14);
        labelShadow.setStyle(sf::Text::Bold);
        labelShadow.setFillColor(sf::Color(25, 12, 8, 220));
        labelShadow.setString(utf8Title);
        labelShadow.setPosition(posX + 1.0f, posY - 22.0f);
        window.draw(labelShadow);

        sf::Text labelText;
        labelText.setFont(font);
        labelText.setCharacterSize(14);
        labelText.setStyle(sf::Text::Bold);
        labelText.setFillColor(sf::Color(250, 230, 165));
        labelText.setString(utf8Title);
        labelText.setPosition(posX, posY - 23.0f);
        window.draw(labelText);

        // Draw value
        std::ostringstream ss;
        ss << static_cast<int>(std::round(currentValue)) << "%";
        sf::Text valShadow;
        valShadow.setFont(font);
        valShadow.setCharacterSize(14);
        valShadow.setStyle(sf::Text::Bold);
        valShadow.setFillColor(sf::Color(25, 12, 8, 220));
        valShadow.setString(ss.str());
        sf::FloatRect vb = valShadow.getLocalBounds();
        valShadow.setPosition(posX + trackWidth - vb.width + 1.0f, posY - 22.0f);
        window.draw(valShadow);

        sf::Text valText;
        valText.setFont(font);
        valText.setCharacterSize(14);
        valText.setStyle(sf::Text::Bold);
        valText.setFillColor(sf::Color(255, 215, 95)); // Imperial Gold
        valText.setString(ss.str());
        valText.setPosition(posX + trackWidth - vb.width, posY - 23.0f);
        window.draw(valText);

        // Draw track (Carved Rosewood lacquer trough)
        sf::RectangleShape trackBg(sf::Vector2f(trackWidth, trackHeight));
        trackBg.setPosition(posX, posY);
        trackBg.setFillColor(sf::Color(18, 14, 10, 245));
        trackBg.setOutlineThickness(1.2f);
        trackBg.setOutlineColor(sf::Color(150, 115, 55, 190));
        window.draw(trackBg);

        // Draw progress fill (Amber Gold / Vermilion lacquer)
        float progressFraction = (maxValue > minValue) ? ((currentValue - minValue) / (maxValue - minValue)) : 0.0f;
        progressFraction = std::clamp(progressFraction, 0.0f, 1.0f);
        float fillW = trackWidth * progressFraction;

        if (fillW > 0.0f) {
            sf::RectangleShape fillTrack(sf::Vector2f(fillW, trackHeight));
            fillTrack.setPosition(posX, posY);
            fillTrack.setFillColor(sf::Color(215, 155, 45, 240)); // Amber gold fill
            window.draw(fillTrack);

            // Subtle highlight line on top edge of fill
            sf::RectangleShape fillShine(sf::Vector2f(fillW, 2.0f));
            fillShine.setPosition(posX, posY);
            fillShine.setFillColor(sf::Color(255, 225, 120, 170));
            window.draw(fillShine);
        }

        // Draw knob (Polished Imperial Jade Bead)
        float knobRadius = trackHeight * 0.72f;
        float knobX = posX + fillW;
        float knobY = posY + trackHeight / 2.0f;

        // Jade glow when dragging
        if (isDragging) {
            sf::CircleShape glow(knobRadius + 5.0f);
            glow.setOrigin(knobRadius + 5.0f, knobRadius + 5.0f);
            glow.setPosition(knobX, knobY);
            glow.setFillColor(sf::Color(80, 225, 130, 85));
            window.draw(glow);
        }

        // Jade knob body (Imperial Jade Green)
        sf::CircleShape knob(knobRadius);
        knob.setOrigin(knobRadius, knobRadius);
        knob.setPosition(knobX, knobY);
        knob.setFillColor(sf::Color(32, 115, 68));
        knob.setOutlineThickness(2.0f);
        knob.setOutlineColor(sf::Color(245, 215, 100)); // Gold rim
        window.draw(knob);

        // Jade translucent gleam
        sf::CircleShape knobGleam(knobRadius * 0.45f);
        knobGleam.setOrigin(knobRadius * 0.45f, knobRadius * 0.45f);
        knobGleam.setPosition(knobX - knobRadius * 0.22f, knobY - knobRadius * 0.22f);
        knobGleam.setFillColor(sf::Color(90, 210, 145, 140));
        window.draw(knobGleam);

        // Knob center stud (Gold/bronze bead)
        sf::CircleShape knobCenter(knobRadius * 0.30f);
        knobCenter.setOrigin(knobRadius * 0.30f, knobRadius * 0.30f);
        knobCenter.setPosition(knobX, knobY);
        knobCenter.setFillColor(sf::Color(255, 235, 140));
        knobCenter.setOutlineThickness(1.0f);
        knobCenter.setOutlineColor(sf::Color(165, 115, 30));
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
