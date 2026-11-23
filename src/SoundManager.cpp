#include "SoundManager.h"
#include "AssetManager.h"
#include <algorithm>
#include <cstdlib>
#include <string>

SoundManager::SoundManager()
    : amThanhBat(true),
    sfxBat(true),
    bgmBat(true),
    masterVolume(80.0f),
    sfxVolume(80.0f),
    bgmVolume(50.0f),
    currentTrack(BgmTrack::MENU),
    menuMusicLoaded(false),
    matchMusicLoaded(false) {
}

SoundManager::~SoundManager() = default;

void SoundManager::khoiTao() {
    sinhAmThanh(); // Fallback synthetic sounds in case audio file is unavailable

    auto& am = AssetManager::get();

    // Move sound
    bufferMove = am.sound(SoundId::MOVE);
    if (bufferMove.getSampleCount() > 0) {
        soundMove.setBuffer(bufferMove);
    }

    // Capture sound
    bufferCapture = am.sound(SoundId::CAPTURE);
    if (bufferCapture.getSampleCount() > 0) {
        soundCapture.setBuffer(bufferCapture);
    }

    // 1. Menu BGM: Lotus Pond - Loop (ambient background for menus, setup, title, etc.)
    musicMenu = std::make_unique<sf::Music>();
    std::string pathLotus = am.getMusicPath(MusicId::LOTUS_POND);
    if (!pathLotus.empty() && musicMenu->openFromFile(pathLotus)) {
        musicMenu->setLoop(true);
        menuMusicLoaded = true;
    }

    // 2. Match BGM: Dragon Dance - Loop (dramatic martial music when playing chess)
    musicMatch = std::make_unique<sf::Music>();
    std::string pathDragon = am.getMusicPath(MusicId::DRAGON_DANCE);
    if (!pathDragon.empty() && musicMatch->openFromFile(pathDragon)) {
        musicMatch->setLoop(true);
        matchMusicLoaded = true;
    }

    // Fallback synthetic BGM in case WAV streams couldn't be loaded
    if (!menuMusicLoaded && !matchMusicLoaded) {
        std::string bgmPath = am.getMusicPath(MusicId::BGM_MAIN);
        if (!bgmPath.empty() && bufferBgm.loadFromFile(bgmPath)) {
            soundBgm.setBuffer(bufferBgm);
            soundBgm.setLoop(true);
        }
    }

    // Check sound
    bufferCheck = am.sound(SoundId::CHECK);
    if (bufferCheck.getSampleCount() > 0) {
        soundCheck.setBuffer(bufferCheck);
    }

    // Victory sound
    bufferVictory = am.sound(SoundId::WIN);
    if (bufferVictory.getSampleCount() > 0) {
        soundVictory.setBuffer(bufferVictory);
    }

    // Click / Select sound
    bufferClick = am.sound(SoundId::SELECT);
    if (bufferClick.getSampleCount() > 0) {
        soundClick.setBuffer(bufferClick);
    }

    // Custom Defeat sound
    bufferDefeat = am.sound(SoundId::LOSE);
    if (bufferDefeat.getSampleCount() > 0) {
        soundDefeat.setBuffer(bufferDefeat);
    }

    // Custom Wrong / Illegal move sound
    bufferIllegal = am.sound(SoundId::WRONG);
    if (bufferIllegal.getSampleCount() > 0) {
        soundIllegal.setBuffer(bufferIllegal);
    }

    currentTrack = BgmTrack::MENU;
    capNhatAmLuong();
    if (amThanhBat && bgmBat) {
        playBgm(BgmTrack::MENU);
    }
}

void SoundManager::capNhatAmLuong() {
    float effectiveSfx = (amThanhBat && sfxBat) ? (masterVolume * (sfxVolume / 100.0f)) : 0.0f;
    float effectiveBgm = (amThanhBat && bgmBat) ? (masterVolume * (bgmVolume / 100.0f)) : 0.0f;

    soundMove.setVolume(effectiveSfx);
    soundCapture.setVolume(effectiveSfx);
    soundCheck.setVolume(effectiveSfx);
    soundVictory.setVolume(effectiveSfx);
    soundDefeat.setVolume(effectiveSfx);
    soundClick.setVolume(effectiveSfx);
    soundIllegal.setVolume(effectiveSfx);
    soundBgm.setVolume(effectiveBgm);

    if (musicMenu) {
        musicMenu->setVolume(effectiveBgm);
    }
    if (musicMatch) {
        musicMatch->setVolume(effectiveBgm);
    }
}

void SoundManager::sinhAmThanh() {
    const unsigned int sampleRate = 44100;
    const double PI = 3.14159265358979323846;

    auto makeBuffer = [&](double duration, auto genFunc, sf::SoundBuffer& buffer) {
        size_t totalSamples = static_cast<size_t>(duration * sampleRate);
        std::vector<sf::Int16> samples(totalSamples);
        for (size_t i = 0; i < totalSamples; ++i) {
            double t = static_cast<double>(i) / sampleRate;
            double v = genFunc(t, i, totalSamples);
            if (v > 1.0) v = 1.0;
            if (v < -1.0) v = -1.0;
            samples[i] = static_cast<sf::Int16>(v * 32767.0);
        }
        buffer.loadFromSamples(samples.data(), samples.size(), 1, sampleRate);
        };

    // 1. Move fallback
    makeBuffer(0.08, [&](double t, size_t, size_t) {
        double decay = std::exp(-t * 60.0);
        double freq = 500.0 - t * 2500.0;
        if (freq < 160.0) freq = 160.0;
        return std::sin(2.0 * PI * freq * t) * decay;
        }, bufferMove);
    soundMove.setBuffer(bufferMove);

    // 2. Capture fallback
    makeBuffer(0.18, [&](double t, size_t, size_t) {
        double decay = std::exp(-t * 30.0);
        double tone1 = std::sin(2.0 * PI * 240.0 * t);
        double tone2 = std::sin(2.0 * PI * 480.0 * t) * 0.4;
        double click = (t < 0.015) ? (((rand() % 1000) / 500.0 - 1.0) * 0.4) : 0.0;
        return (tone1 * 0.6 + tone2 * 0.3 + click) * decay;
        }, bufferCapture);
    soundCapture.setBuffer(bufferCapture);

    // Check sound
    makeBuffer(0.38, [&](double t, size_t, size_t) {
        double env1 = std::exp(-t * 14.0);
        double s1 = std::sin(2.0 * PI * 700.0 * t) * env1 * 0.45;
        double s2 = 0.0;
        if (t > 0.12) {
            double t2 = t - 0.12;
            double env2 = std::exp(-t2 * 12.0);
            s2 = (std::sin(2.0 * PI * 980.0 * t2) + 0.3 * std::sin(2.0 * PI * 1960.0 * t2)) * env2 * 0.55;
        }
        return s1 + s2;
        }, bufferCheck);
    soundCheck.setBuffer(bufferCheck);

    // Victory sound
    makeBuffer(0.9, [&](double t, size_t, size_t) {
        double notes[] = { 523.25, 659.25, 783.99, 1046.50 };
        double seg = 0.18;
        int idx = static_cast<int>(t / seg);
        if (idx > 3) idx = 3;
        double tLocal = t - (idx * seg);
        double env = std::exp(-tLocal * 7.0);
        return (std::sin(2.0 * PI * notes[idx] * t) + 0.2 * std::sin(2.0 * PI * notes[idx] * 2.0 * t)) * env * 0.5;
        }, bufferVictory);
    soundVictory.setBuffer(bufferVictory);

    // Defeat sound: Deep martial gong resonance with sombre descending harmonic cadence
    makeBuffer(1.4, [&](double t, size_t, size_t) {
        double gong = (std::sin(2.0 * PI * 110.0 * t) * 0.45 * std::exp(-t * 2.2)
                     + std::sin(2.0 * PI * 220.0 * t) * 0.25 * std::exp(-t * 3.5)
                     + std::sin(2.0 * PI * 330.0 * t) * 0.15 * std::exp(-t * 4.8));
        double notes[] = { 440.0, 370.0, 311.13, 261.63 };
        double seg = 0.32;
        int idx = static_cast<int>(t / seg);
        if (idx > 3) idx = 3;
        double tLocal = t - (idx * seg);
        double env = std::exp(-tLocal * 4.0);
        double melody = std::sin(2.0 * PI * notes[idx] * t) * env * 0.35;
        return gong + melody;
    }, bufferDefeat);
    soundDefeat.setBuffer(bufferDefeat);

    // Try loading custom user defeat audio file if present
    auto& amInst = AssetManager::get();
    std::string loseFiles[] = {
        amInst.resolvePath("audio/sfx/lose.wav"),
        amInst.resolvePath("audio/sfx/lose.ogg"),
        amInst.resolvePath("audio/sfx/lose.mp3")
    };
    for (const auto& lf : loseFiles) {
        if (!lf.empty() && bufferDefeat.loadFromFile(lf)) {
            soundDefeat.setBuffer(bufferDefeat);
            break;
        }
    }

    // Click sound
    makeBuffer(0.04, [&](double t, size_t, size_t) {
        double decay = std::exp(-t * 90.0);
        return std::sin(2.0 * PI * 1100.0 * t) * decay * 0.5;
        }, bufferClick);
    soundClick.setBuffer(bufferClick);

    // Illegal move: Warning low double buzzer
    makeBuffer(0.18, [&](double t, size_t, size_t) {
        double decay = std::exp(-std::fmod(t, 0.09) * 35.0);
        double buzzer = (std::sin(2.0 * PI * 140.0 * t) > 0 ? 1.0 : -1.0) * 0.35;
        return buzzer * decay;
    }, bufferIllegal);
    soundIllegal.setBuffer(bufferIllegal);

    // Try loading custom user wrong / illegal audio file if present
    std::string wrongFiles[] = {
        amInst.resolvePath("audio/sfx/wrong.mp3"),
        amInst.resolvePath("audio/sfx/wrong.wav"),
        amInst.resolvePath("audio/sfx/wrong.ogg"),
        amInst.resolvePath("audio/sfx/illegal.mp3"),
        amInst.resolvePath("audio/sfx/illegal.wav"),
        amInst.resolvePath("audio/sfx/illegal.ogg")
    };
    for (const auto& wf : wrongFiles) {
        if (!wf.empty() && bufferIllegal.loadFromFile(wf)) {
            soundIllegal.setBuffer(bufferIllegal);
            break;
        }
    }

    // BGM fallback
    struct NoteEvent {
        double startTime;
        double freq;
        double gain;
    };
    std::vector<NoteEvent> bgmNotes = {
        {0.0, 261.63, 0.40},   // C4
        {0.1, 392.00, 0.35},   // G4
        {1.4, 329.63, 0.35},   // E4
        {2.8, 440.00, 0.35},   // A4
        {4.2, 293.66, 0.38},   // D4
        {4.3, 440.00, 0.30},   // A4
        {5.6, 392.00, 0.35},   // G4
        {7.0, 523.25, 0.35},   // C5
        {8.4, 329.63, 0.35},   // E4
        {8.5, 523.25, 0.30},   // C5
        {9.8, 587.33, 0.32},   // D5
        {11.2, 392.00, 0.35},  // G4
        {11.3, 659.25, 0.30},  // E5
        {12.6, 261.63, 0.40},  // C4
        {12.7, 392.00, 0.35}   // G4
    };

    makeBuffer(14.0, [&](double t, size_t, size_t) {
        double val = 0.0;
        val += std::sin(2.0 * PI * 130.81 * t) * 0.08;
        val += std::sin(2.0 * PI * 196.00 * t) * 0.05;

        for (const auto& ne : bgmNotes) {
            if (t >= ne.startTime) {
                double dt = t - ne.startTime;
                if (dt < 2.5) {
                    double decay = std::exp(-dt * 2.2);
                    double harmonic1 = std::sin(2.0 * PI * ne.freq * dt);
                    double harmonic2 = std::sin(2.0 * PI * (ne.freq * 2.0) * dt) * 0.25;
                    double harmonic3 = std::sin(2.0 * PI * (ne.freq * 3.0) * dt) * 0.10;
                    val += (harmonic1 + harmonic2 + harmonic3) * decay * ne.gain;
                }
            }
        }
        return val * 0.45;
        }, bufferBgm);

    soundBgm.setBuffer(bufferBgm);
    soundBgm.setLoop(true);
}

void SoundManager::play(SoundType type) {
    if (!amThanhBat || !sfxBat) return;

    switch (type) {
    case SoundType::MOVE:
        soundMove.play();
        break;
    case SoundType::CAPTURE:
        soundCapture.play();
        break;
    case SoundType::CHECK:
        soundCheck.play();
        break;
    case SoundType::VICTORY:
        soundVictory.play();
        break;
    case SoundType::DEFEAT:
        soundDefeat.play();
        break;
    case SoundType::CLICK:
        soundClick.play();
        break;
    case SoundType::ILLEGAL:
        soundIllegal.play();
        break;
    }
}

void SoundManager::switchToTrack(BgmTrack track) {
    if (currentTrack == track) {
        if (amThanhBat && bgmBat) {
            sf::Music* cur = (track == BgmTrack::MENU) ? musicMenu.get() : musicMatch.get();
            if (cur && cur->getStatus() == sf::Music::Stopped) {
                cur->play();
            } else if (!cur && soundBgm.getStatus() == sf::Sound::Stopped) {
                soundBgm.play();
            }
        }
        return;
    }

    currentTrack = track;
    capNhatAmLuong();

    if (track == BgmTrack::MENU) {
        if (musicMatch && musicMatch->getStatus() == sf::Music::Playing) {
            musicMatch->stop();
        }
        if (musicMenu && menuMusicLoaded) {
            if (amThanhBat && bgmBat && musicMenu->getStatus() != sf::Music::Playing) {
                musicMenu->play();
            }
        } else if (soundBgm.getStatus() != sf::Sound::Playing && amThanhBat && bgmBat) {
            soundBgm.play();
        }
    } else { // MATCH
        if (musicMenu && musicMenu->getStatus() == sf::Music::Playing) {
            musicMenu->stop();
        }
        if (musicMatch && matchMusicLoaded) {
            if (amThanhBat && bgmBat && musicMatch->getStatus() != sf::Music::Playing) {
                musicMatch->play();
            }
        } else if (soundBgm.getStatus() != sf::Sound::Playing && amThanhBat && bgmBat) {
            soundBgm.play();
        }
    }
}

void SoundManager::playBgm(BgmTrack track) {
    switchToTrack(track);
}

void SoundManager::stopBgm() {
    if (musicMenu) musicMenu->stop();
    if (musicMatch) musicMatch->stop();
    soundBgm.stop();
}

void SoundManager::testAudio() {
    play(SoundType::CHECK);
}

void SoundManager::setAmThanhBat(bool bat) {
    amThanhBat = bat;
    capNhatAmLuong();
    if (!amThanhBat) {
        if (musicMenu && musicMenu->getStatus() == sf::Music::Playing) musicMenu->pause();
        if (musicMatch && musicMatch->getStatus() == sf::Music::Playing) musicMatch->pause();
        soundBgm.pause();
    } else if (bgmBat) {
        sf::Music* cur = (currentTrack == BgmTrack::MENU) ? musicMenu.get() : musicMatch.get();
        if (cur && cur->getStatus() != sf::Music::Playing) {
            cur->play();
        } else if (!cur && soundBgm.getStatus() != sf::Sound::Playing) {
            soundBgm.play();
        }
    }
}

void SoundManager::setMasterVolume(float volume) {
    masterVolume = std::clamp(volume, 0.0f, 100.0f);
    capNhatAmLuong();
}

void SoundManager::setSfxBat(bool bat) {
    sfxBat = bat;
    capNhatAmLuong();
}

void SoundManager::setSfxVolume(float volume) {
    sfxVolume = std::clamp(volume, 0.0f, 100.0f);
    capNhatAmLuong();
}

void SoundManager::setBgmBat(bool bat) {
    bgmBat = bat;
    capNhatAmLuong();
    if (bgmBat && amThanhBat) {
        sf::Music* cur = (currentTrack == BgmTrack::MENU) ? musicMenu.get() : musicMatch.get();
        if (cur && cur->getStatus() != sf::Music::Playing) {
            cur->play();
        } else if (!cur && soundBgm.getStatus() != sf::Sound::Playing) {
            soundBgm.play();
        }
    } else {
        if (musicMenu && musicMenu->getStatus() == sf::Music::Playing) musicMenu->pause();
        if (musicMatch && musicMatch->getStatus() == sf::Music::Playing) musicMatch->pause();
        soundBgm.pause();
    }
}

void SoundManager::setBgmVolume(float volume) {
    bgmVolume = std::clamp(volume, 0.0f, 100.0f);
    capNhatAmLuong();
}