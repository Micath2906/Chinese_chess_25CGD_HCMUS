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
    CLICK,
    ILLEGAL,
    WRONG = ILLEGAL
};

enum class BgmTrack {
    MENU,  // Lotus Pond - Loop (everything: title, menus, settings, setups)
    MATCH  // Dragon Dance - Loop (when playing chess in match)
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
    sf::SoundBuffer bufferIllegal;
    sf::SoundBuffer bufferBgm;

    sf::Sound soundMove;
    sf::Sound soundCapture;
    sf::Sound soundCheck;
    sf::Sound soundVictory;
    sf::Sound soundDefeat;
    sf::Sound soundClick;
    sf::Sound soundIllegal;
    sf::Sound soundBgm;

    std::unique_ptr<sf::Music> musicMenu;   // Lotus Pond - Loop
    std::unique_ptr<sf::Music> musicMatch;  // Dragon Dance - Loop
    BgmTrack currentTrack;
    bool menuMusicLoaded;
    bool matchMusicLoaded;

    void sinhAmThanh();
    void capNhatAmLuong();

public:
    SoundManager();
    ~SoundManager();

    void khoiTao();
    void play(SoundType type);
    void playBgm(BgmTrack track = BgmTrack::MENU);
    void switchToTrack(BgmTrack track);
    void stopBgm();
    void testAudio();

    BgmTrack getCurrentTrack() const { return currentTrack; }

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
