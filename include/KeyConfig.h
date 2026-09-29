#pragma once
#include <SFML/Window/Keyboard.hpp>
#include <string>

enum class KeyAction {
    UP = 0,
    DOWN = 1,
    LEFT = 2,
    RIGHT = 3,
    SELECT = 4,
    DESELECT = 5,
    QUIT = 6,
    COUNT = 7
};

enum class KeyPreset {
    WASD,
    ARROWS,
    IJKL,
    CUSTOM
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

    void setActionKey(KeyAction action, sf::Keyboard::Key key) {
        currentPreset = KeyPreset::CUSTOM;
        switch (action) {
            case KeyAction::UP: keyUp = key; break;
            case KeyAction::DOWN: keyDown = key; break;
            case KeyAction::LEFT: keyLeft = key; break;
            case KeyAction::RIGHT: keyRight = key; break;
            case KeyAction::SELECT: keySelect = key; break;
            case KeyAction::DESELECT: keyDeselect = key; break;
            case KeyAction::QUIT: keyQuit = key; break;
            default: break;
        }
    }

    sf::Keyboard::Key getActionKey(KeyAction action) const {
        switch (action) {
            case KeyAction::UP: return keyUp;
            case KeyAction::DOWN: return keyDown;
            case KeyAction::LEFT: return keyLeft;
            case KeyAction::RIGHT: return keyRight;
            case KeyAction::SELECT: return keySelect;
            case KeyAction::DESELECT: return keyDeselect;
            case KeyAction::QUIT: return keyQuit;
            default: return sf::Keyboard::Unknown;
        }
    }

    std::string getPresetName() const {
        if (currentPreset == KeyPreset::WASD) return "W, A, S, D (Default)";
        if (currentPreset == KeyPreset::ARROWS) return "Arrow Keys";
        if (currentPreset == KeyPreset::IJKL) return "I, J, K, L";
        return "Custom";
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
        if (key >= sf::Keyboard::A && key <= sf::Keyboard::Z) {
            return std::string(1, 'A' + (key - sf::Keyboard::A));
        }
        if (key >= sf::Keyboard::Num0 && key <= sf::Keyboard::Num9) {
            return std::string(1, '0' + (key - sf::Keyboard::Num0));
        }
        if (key >= sf::Keyboard::Numpad0 && key <= sf::Keyboard::Numpad9) {
            return "Num " + std::string(1, '0' + (key - sf::Keyboard::Numpad0));
        }

        switch (key) {
            case sf::Keyboard::Up: return "Up";
            case sf::Keyboard::Down: return "Down";
            case sf::Keyboard::Left: return "Left";
            case sf::Keyboard::Right: return "Right";
            case sf::Keyboard::Return: return "Enter";
            case sf::Keyboard::Space: return "Space";
            case sf::Keyboard::Escape: return "Esc";
            case sf::Keyboard::BackSpace: return "Backspace";
            case sf::Keyboard::Tab: return "Tab";
            case sf::Keyboard::LShift: return "LShift";
            case sf::Keyboard::RShift: return "RShift";
            case sf::Keyboard::LControl: return "LCtrl";
            case sf::Keyboard::RControl: return "RCtrl";
            case sf::Keyboard::LAlt: return "LAlt";
            case sf::Keyboard::RAlt: return "RAlt";
            case sf::Keyboard::Home: return "Home";
            case sf::Keyboard::End: return "End";
            case sf::Keyboard::PageUp: return "PgUp";
            case sf::Keyboard::PageDown: return "PgDn";
            case sf::Keyboard::Insert: return "Insert";
            case sf::Keyboard::Delete: return "Delete";
            default: return "Key";
        }
    }
};
