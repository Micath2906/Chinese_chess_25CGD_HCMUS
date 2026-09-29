#include "SoundManager.h"
#include <algorithm>
#include <cstdlib>

SoundManager::SoundManager()
    : amThanhBat(true), amLuong(80.0f) {
}

void SoundManager::khoiTao() {
    sinhAmThanh();
    setAmLuong(amLuong);
}

void SoundManager::sinhAmThanh() {
    const unsigned int sampleRate = 44100;
    const double PI = 3.14159265358979323846;

    // Helper lambda to make sound buffer
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

    // 1. Move: Crisp wooden piece click (wood tap)
    makeBuffer(0.08, [&](double t, size_t, size_t) {
        double decay = std::exp(-t * 60.0);
        double freq = 500.0 - t * 2500.0;
        if (freq < 160.0) freq = 160.0;
        return std::sin(2.0 * PI * freq * t) * decay;
    }, bufferMove);
    soundMove.setBuffer(bufferMove);

    // 2. Capture: Heavy wooden impact with resonance
    makeBuffer(0.18, [&](double t, size_t i, size_t) {
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

    // 4. Victory: Triumphant fanfare arpeggio (C5 - E5 - G5 - C6)
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

    // 5. Defeat: Descending somber notes (A4 - F4 - D4)
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
}

void SoundManager::play(SoundType type) {
    if (!amThanhBat) return;

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

void SoundManager::setAmThanhBat(bool bat) {
    amThanhBat = bat;
    if (!amThanhBat) {
        soundMove.stop();
        soundCapture.stop();
        soundCheck.stop();
        soundVictory.stop();
        soundDefeat.stop();
        soundClick.stop();
    }
}

void SoundManager::setAmLuong(float volume) {
    amLuong = std::clamp(volume, 0.0f, 100.0f);
    soundMove.setVolume(amLuong);
    soundCapture.setVolume(amLuong);
    soundCheck.setVolume(amLuong);
    soundVictory.setVolume(amLuong);
    soundDefeat.setVolume(amLuong);
    soundClick.setVolume(amLuong);
}
