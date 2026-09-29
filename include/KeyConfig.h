#pragma once
#include <SFML/Window/Keyboard.hpp>
#include <string>

enum class KeyPreset {
    WASD,
    ARROWS,
    IJKL
};

struct KeyConfig {
    sf::Keyboard::Key keyUp;
    sf::Keyboard::Key keyDown;
    sf::Keyboard::Key keyLeft;
    sf::Keyboard::Key keyRight;
    sf::Keyboard::Key keySelect;
    sf::Keyboard::Key keyDeselect;
    sf::Keyboard::Key keyQuit;
    sf::Keyboard::Key keyUndo;
    sf::Keyboard::Key keyRedo;
    KeyPreset currentPreset;

    KeyConfig() {
        setPreset(KeyPreset::WASD);
    }

    void setPreset(KeyPreset preset) {
        currentPreset = preset;
        if (preset == KeyPreset::WASD) {
            keyUp = sf::Keyboard::W;
            keyDown = sf::Keyboard::S;
            keyLeft = sf::Keyboard::A;
            keyRight = sf::Keyboard::D;
            keySelect = sf::Keyboard::Return;
            keyDeselect = sf::Keyboard::Escape;
            keyQuit = sf::Keyboard::Q;
            keyUndo = sf::Keyboard::Z;
            keyRedo = sf::Keyboard::Y;
        } else if (preset == KeyPreset::ARROWS) {
            keyUp = sf::Keyboard::Up;
            keyDown = sf::Keyboard::Down;
            keyLeft = sf::Keyboard::Left;
            keyRight = sf::Keyboard::Right;
            keySelect = sf::Keyboard::Space;
            keyDeselect = sf::Keyboard::Escape;
            keyQuit = sf::Keyboard::Q;
            keyUndo = sf::Keyboard::Z;
            keyRedo = sf::Keyboard::Y;
        } else if (preset == KeyPreset::IJKL) {
            keyUp = sf::Keyboard::I;
            keyDown = sf::Keyboard::K;
            keyLeft = sf::Keyboard::J;
            keyRight = sf::Keyboard::L;
            keySelect = sf::Keyboard::Return;
            keyDeselect = sf::Keyboard::Escape;
            keyQuit = sf::Keyboard::Q;
            keyUndo = sf::Keyboard::Z;
            keyRedo = sf::Keyboard::Y;
        }
    }

    void cyclePreset() {
        if (currentPreset == KeyPreset::WASD) setPreset(KeyPreset::ARROWS);
        else if (currentPreset == KeyPreset::ARROWS) setPreset(KeyPreset::IJKL);
        else setPreset(KeyPreset::WASD);
    }

    std::string getPresetName() const {
        if (currentPreset == KeyPreset::WASD) return "W, A, S, D (Default)";
        if (currentPreset == KeyPreset::ARROWS) return "Arrow Keys (Mui ten)";
        return "I, J, K, L";
    }

    bool isUpKey(sf::Keyboard::Key key) const {
        return key == keyUp || key == sf::Keyboard::Up;
    }
    bool isDownKey(sf::Keyboard::Key key) const {
        return key == keyDown || key == sf::Keyboard::Down;
    }
    bool isLeftKey(sf::Keyboard::Key key) const {
        return key == keyLeft || key == sf::Keyboard::Left;
    }
    bool isRightKey(sf::Keyboard::Key key) const {
        return key == keyRight || key == sf::Keyboard::Right;
    }
    bool isSelectKey(sf::Keyboard::Key key) const {
        return key == keySelect || key == sf::Keyboard::Return || key == sf::Keyboard::Space;
    }
    bool isDeselectKey(sf::Keyboard::Key key) const {
        return key == keyDeselect || key == sf::Keyboard::Escape;
    }
    bool isQuitKey(sf::Keyboard::Key key) const {
        return key == keyQuit;
    }

    static std::string getKeyName(sf::Keyboard::Key key) {
        switch (key) {
            case sf::Keyboard::W: return "W";
            case sf::Keyboard::A: return "A";
            case sf::Keyboard::S: return "S";
            case sf::Keyboard::D: return "D";
            case sf::Keyboard::Up: return "Up";
            case sf::Keyboard::Down: return "Down";
            case sf::Keyboard::Left: return "Left";
            case sf::Keyboard::Right: return "Right";
            case sf::Keyboard::Return: return "Enter";
            case sf::Keyboard::Space: return "Space";
            case sf::Keyboard::Escape: return "Esc";
            case sf::Keyboard::Q: return "Q";
            case sf::Keyboard::Z: return "Z";
            case sf::Keyboard::Y: return "Y";
            case sf::Keyboard::I: return "I";
            case sf::Keyboard::J: return "J";
            case sf::Keyboard::K: return "K";
            case sf::Keyboard::L: return "L";
            default: return "Key";
        }
    }
};
