#pragma once
#include <SFML/Audio.hpp>
#include <vector>
#include <cmath>
#include <memory>

enum class SoundType {
    MOVE,
    CAPTURE,
    CHECK,
    VICTORY,
    DEFEAT,
    CLICK
};

class SoundManager {
private:
    bool amThanhBat;
    float amLuong; // 0.0f - 100.0f

    sf::SoundBuffer bufferMove;
    sf::SoundBuffer bufferCapture;
    sf::SoundBuffer bufferCheck;
    sf::SoundBuffer bufferVictory;
    sf::SoundBuffer bufferDefeat;
    sf::SoundBuffer bufferClick;

    sf::Sound soundMove;
    sf::Sound soundCapture;
    sf::Sound soundCheck;
    sf::Sound soundVictory;
    sf::Sound soundDefeat;
    sf::Sound soundClick;

    void sinhAmThanh();

public:
    SoundManager();

    void khoiTao();
    void play(SoundType type);

    void setAmThanhBat(bool bat);
    bool getAmThanhBat() const { return amThanhBat; }

    void setAmLuong(float volume);
    float getAmLuong() const { return amLuong; }
};
