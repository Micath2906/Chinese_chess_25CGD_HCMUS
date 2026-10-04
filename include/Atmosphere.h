#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <cmath>
#include <cstdlib>
#include <algorithm>

enum class AtmosphereStyle {
    BAMBOO_LEAVES,  // La truc roi
    PEACH_BLOSSOMS, // Canh hoa dao roi
    GOLDEN_SPARKS   // Kim sa tinh quang
};

struct Particle {
    float x, y;
    float vx, vy;
    float rotation;
    float rotSpeed;
    float scale;
    float swayAmp;
    float swaySpeed;
    float swayPhase;
    float alpha;
};

class AtmosphereSystem {
private:
    std::vector<Particle> particles;
    AtmosphereStyle style;
    float density; // 0.0f - 100.0f
    float windowW, windowH;

public:
    AtmosphereSystem(float w = 1200.0f, float h = 800.0f)
        : style(AtmosphereStyle::BAMBOO_LEAVES), density(60.0f), windowW(w), windowH(h) {
        khoiTaoParticles();
    }

    void setStyle(AtmosphereStyle s) {
        style = s;
        khoiTaoParticles();
    }
    AtmosphereStyle getStyle() const { return style; }

    void cycleStyle() {
        int s = (static_cast<int>(style) + 1) % 3;
        setStyle(static_cast<AtmosphereStyle>(s));
    }

    std::string getStyleName(bool vi) const {
        switch (style) {
            case AtmosphereStyle::BAMBOO_LEAVES:
                return vi ? "Truc Lam Diep Lac (La Truc)" : "Falling Bamboo Leaves";
            case AtmosphereStyle::PEACH_BLOSSOMS:
                return vi ? "Dao Hoa Phieu Lac (Hoa Dao)" : "Peach Blossoms";
            case AtmosphereStyle::GOLDEN_SPARKS:
                return vi ? "Kim Sa Tinh Quang (Bui Vang)" : "Golden Starlight";
        }
        return "";
    }

    void setDensity(float d) {
        density = std::clamp(d, 0.0f, 100.0f);
        khoiTaoParticles();
    }
    float getDensity() const { return density; }

    void update(float dt) {
        if (density <= 0.0f) return;
        for (auto& p : particles) {
            p.swayPhase += p.swaySpeed * dt;
            p.x += (p.vx + std::sin(p.swayPhase) * p.swayAmp) * dt;
            p.y += p.vy * dt;
            p.rotation += p.rotSpeed * dt;

            // Wrap around
            if (p.y > windowH + 30.0f) {
                p.y = -30.0f;
                p.x = static_cast<float>(rand() % static_cast<int>(windowW));
            }
            if (p.x < -40.0f) p.x = windowW + 20.0f;
            if (p.x > windowW + 40.0f) p.x = -20.0f;
        }
    }

    void draw(sf::RenderWindow& window) {
        if (density <= 0.0f) return;

        for (const auto& p : particles) {
            sf::Transform transform;
            transform.translate(p.x, p.y);
            transform.rotate(p.rotation);
            transform.scale(p.scale, p.scale);

            if (style == AtmosphereStyle::BAMBOO_LEAVES) {
                // Bamboo leaf: slender curved polygon
                sf::ConvexShape leaf(6);
                leaf.setPoint(0, sf::Vector2f(0.0f, -14.0f));
                leaf.setPoint(1, sf::Vector2f(4.0f, -4.0f));
                leaf.setPoint(2, sf::Vector2f(3.0f, 8.0f));
                leaf.setPoint(3, sf::Vector2f(0.0f, 15.0f));
                leaf.setPoint(4, sf::Vector2f(-3.0f, 8.0f));
                leaf.setPoint(5, sf::Vector2f(-4.0f, -4.0f));

                leaf.setFillColor(sf::Color(80, 160, 75, static_cast<sf::Uint8>(p.alpha)));
                leaf.setOutlineThickness(0.8f);
                leaf.setOutlineColor(sf::Color(120, 200, 110, static_cast<sf::Uint8>(p.alpha * 0.9f)));
                window.draw(leaf, transform);
            }
            else if (style == AtmosphereStyle::PEACH_BLOSSOMS) {
                // Peach blossom petal
                sf::ConvexShape petal(6);
                petal.setPoint(0, sf::Vector2f(0.0f, -10.0f));
                petal.setPoint(1, sf::Vector2f(6.0f, -4.0f));
                petal.setPoint(2, sf::Vector2f(5.0f, 6.0f));
                petal.setPoint(3, sf::Vector2f(0.0f, 11.0f));
                petal.setPoint(4, sf::Vector2f(-5.0f, 6.0f));
                petal.setPoint(5, sf::Vector2f(-6.0f, -4.0f));

                petal.setFillColor(sf::Color(255, 185, 200, static_cast<sf::Uint8>(p.alpha)));
                petal.setOutlineThickness(0.8f);
                petal.setOutlineColor(sf::Color(255, 140, 170, static_cast<sf::Uint8>(p.alpha * 0.9f)));
                window.draw(petal, transform);
            }
            else if (style == AtmosphereStyle::GOLDEN_SPARKS) {
                // Golden star particle
                sf::ConvexShape star(8);
                star.setPoint(0, sf::Vector2f(0.0f, -8.0f));
                star.setPoint(1, sf::Vector2f(2.5f, -2.5f));
                star.setPoint(2, sf::Vector2f(8.0f, 0.0f));
                star.setPoint(3, sf::Vector2f(2.5f, 2.5f));
                star.setPoint(4, sf::Vector2f(0.0f, 8.0f));
                star.setPoint(5, sf::Vector2f(-2.5f, 2.5f));
                star.setPoint(6, sf::Vector2f(-8.0f, 0.0f));
                star.setPoint(7, sf::Vector2f(-2.5f, -2.5f));

                star.setFillColor(sf::Color(255, 220, 80, static_cast<sf::Uint8>(p.alpha)));
                window.draw(star, transform);
            }
        }
    }

private:
    void khoiTaoParticles() {
        particles.clear();
        if (density <= 0.0f) return;

        int count = static_cast<int>(std::round((density / 100.0f) * 35.0f));
        if (count < 2) count = 2;

        particles.resize(count);
        for (auto& p : particles) {
            p.x = static_cast<float>(rand() % static_cast<int>(windowW));
            p.y = static_cast<float>(rand() % static_cast<int>(windowH));
            p.vx = -15.0f + static_cast<float>(rand() % 30);
            p.vy = 28.0f + static_cast<float>(rand() % 40);
            p.rotation = static_cast<float>(rand() % 360);
            p.rotSpeed = -45.0f + static_cast<float>(rand() % 90);
            p.scale = 0.7f + static_cast<float>(rand() % 70) / 100.0f;
            p.swayAmp = 15.0f + static_cast<float>(rand() % 25);
            p.swaySpeed = 1.0f + static_cast<float>(rand() % 20) / 10.0f;
            p.swayPhase = static_cast<float>(rand() % 360);
            p.alpha = 150.0f + static_cast<float>(rand() % 95);
        }
    }
};
