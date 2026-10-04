#include "SoundManager.h"
#include <algorithm>
#include <cstdlib>
#include <vector>

SoundManager::SoundManager()
    : amThanhBat(true),
      sfxBat(true),
      bgmBat(true),
      masterVolume(80.0f),
      sfxVolume(80.0f),
      bgmVolume(50.0f) {
}

void SoundManager::khoiTao() {
    sinhAmThanh(); // Generate synthesized sounds first as 100% reliable fallback

    // Attempt to load high-fidelity recorded sound assets if available
    sf::SoundBuffer tempBuf;
    if (tempBuf.loadFromFile("resources/sounds/move.wav")) {
        bufferMove = tempBuf;
        soundMove.setBuffer(bufferMove);
    }
    if (tempBuf.loadFromFile("resources/sounds/capture.wav")) {
        bufferCapture = tempBuf;
        soundCapture.setBuffer(bufferCapture);
    }
    if (tempBuf.loadFromFile("resources/sounds/check.wav")) {
        bufferCheck = tempBuf;
        soundCheck.setBuffer(bufferCheck);
    }
    if (tempBuf.loadFromFile("resources/sounds/win.wav")) {
        bufferVictory = tempBuf;
        soundVictory.setBuffer(bufferVictory);
    }
    if (tempBuf.loadFromFile("resources/sounds/select.wav")) {
        bufferClick = tempBuf;
        soundClick.setBuffer(bufferClick);
    }

    capNhatAmLuong();
    if (amThanhBat && bgmBat) {
        playBgm();
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
    soundBgm.setVolume(effectiveBgm);
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

    // 1. Move: Crisp wooden piece click
    makeBuffer(0.08, [&](double t, size_t, size_t) {
        double decay = std::exp(-t * 60.0);
        double freq = 500.0 - t * 2500.0;
        if (freq < 160.0) freq = 160.0;
        return std::sin(2.0 * PI * freq * t) * decay;
    }, bufferMove);
    soundMove.setBuffer(bufferMove);

    // 2. Capture: Heavy wooden impact with resonance
    makeBuffer(0.18, [&](double t, size_t, size_t) {
        double decay = std::exp(-t * 30.0);
        double tone1 = std::sin(2.0 * PI * 240.0 * t);
        double tone2 = std::sin(2.0 * PI * 480.0 * t) * 0.4;
        double click = (t < 0.015) ? (((rand() % 1000) / 500.0 - 1.0) * 0.4) : 0.0;
        return (tone1 * 0.6 + tone2 * 0.3 + click) * decay;
    }, bufferCapture);
    soundCapture.setBuffer(bufferCapture);

    // 3. Check: Warning chime (two clear ascending bell tones)
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

    // 4. Victory: Triumphant fanfare arpeggio
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

    // 5. Defeat: Descending somber notes
    makeBuffer(0.8, [&](double t, size_t, size_t) {
        double notes[] = { 440.0, 349.23, 293.66 };
        double seg = 0.24;
        int idx = static_cast<int>(t / seg);
        if (idx > 2) idx = 2;
        double tLocal = t - (idx * seg);
        double env = std::exp(-tLocal * 5.0);
        return std::sin(2.0 * PI * notes[idx] * t) * env * 0.45;
    }, bufferDefeat);
    soundDefeat.setBuffer(bufferDefeat);

    // 6. Click: UI button click
    makeBuffer(0.04, [&](double t, size_t, size_t) {
        double decay = std::exp(-t * 90.0);
        return std::sin(2.0 * PI * 1100.0 * t) * decay * 0.5;
    }, bufferClick);
    soundClick.setBuffer(bufferClick);

    // 7. Ambient BGM: 14-second Chinese Pentatonic Guzheng loop
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
        // Warm sub-bass drone
        val += std::sin(2.0 * PI * 130.81 * t) * 0.08;
        val += std::sin(2.0 * PI * 196.00 * t) * 0.05;

        // Plucked notes
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
    }
}

void SoundManager::playBgm() {
    if (amThanhBat && bgmBat) {
        if (soundBgm.getStatus() != sf::Sound::Playing) {
            soundBgm.play();
        }
    }
}

void SoundManager::stopBgm() {
    soundBgm.stop();
}

void SoundManager::testAudio() {
    play(SoundType::CHECK);
}

void SoundManager::setAmThanhBat(bool bat) {
    amThanhBat = bat;
    capNhatAmLuong();
    if (!amThanhBat) {
        soundBgm.pause();
    } else if (bgmBat) {
        playBgm();
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
        playBgm();
    } else {
        soundBgm.pause();
    }
}

void SoundManager::setBgmVolume(float volume) {
    bgmVolume = std::clamp(volume, 0.0f, 100.0f);
    capNhatAmLuong();
}
