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
    bool amThanhBat;      // Master audio toggle
    bool sfxBat;          // SFX toggle
    bool bgmBat;          // BGM toggle
    float masterVolume;   // 0.0f - 100.0f
    float sfxVolume;      // 0.0f - 100.0f
    float bgmVolume;      // 0.0f - 100.0f

    sf::SoundBuffer bufferMove;
    sf::SoundBuffer bufferCapture;
    sf::SoundBuffer bufferCheck;
    sf::SoundBuffer bufferVictory;
    sf::SoundBuffer bufferDefeat;
    sf::SoundBuffer bufferClick;
    sf::SoundBuffer bufferBgm;

    sf::Sound soundMove;
    sf::Sound soundCapture;
    sf::Sound soundCheck;
    sf::Sound soundVictory;
    sf::Sound soundDefeat;
    sf::Sound soundClick;
    sf::Sound soundBgm;

    void sinhAmThanh();
    void capNhatAmLuong();

public:
    SoundManager();

    void khoiTao();
    void play(SoundType type);
    void playBgm();
    void stopBgm();
    void testAudio();

    // Master
    void setAmThanhBat(bool bat);
    bool getAmThanhBat() const { return amThanhBat; }
    void setMasterVolume(float volume);
    float getMasterVolume() const { return masterVolume; }
    float getAmLuong() const { return masterVolume; } // Backward compatibility
    void setAmLuong(float v) { setMasterVolume(v); }

    // SFX
    void setSfxBat(bool bat);
    bool getSfxBat() const { return sfxBat; }
    void setSfxVolume(float volume);
    float getSfxVolume() const { return sfxVolume; }

    // BGM
    void setBgmBat(bool bat);
    bool getBgmBat() const { return bgmBat; }
    void setBgmVolume(float volume);
    float getBgmVolume() const { return bgmVolume; }
};
