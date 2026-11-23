#include "AssetManager.h"
#include <filesystem>
#include <iostream>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#endif

namespace fs = std::filesystem;

AssetManager::AssetManager() {
    detectAssetsRoot();
}

AssetManager& AssetManager::get() {
    static AssetManager instance;
    return instance;
}

void AssetManager::detectAssetsRoot(const std::string& customRoot) {
    std::vector<fs::path> candidates;

    if (!customRoot.empty()) {
        candidates.push_back(fs::u8path(customRoot));
    }

    // Common development / deployment relative paths
    candidates.push_back(fs::u8path("assets"));
    candidates.push_back(fs::u8path("../assets"));
    candidates.push_back(fs::u8path("../../assets"));

#ifdef _WIN32
    // Resolve relative to running executable path on Windows
    wchar_t exePathBuf[MAX_PATH];
    if (GetModuleFileNameW(nullptr, exePathBuf, MAX_PATH) > 0) {
        fs::path exeDir = fs::path(exePathBuf).parent_path();
        candidates.push_back(exeDir / "assets");
        candidates.push_back(exeDir / ".." / "assets");
    }
#endif

    const std::string probeFile = "textures/backgrounds/xiangqi_background_pavilion.png";

    m_assetsRoot = "assets"; // default fallback
    for (const auto& c : candidates) {
        std::error_code ec;
        if (fs::exists(c / probeFile, ec)) {
            m_assetsRoot = c.lexically_normal().generic_string();
            break;
        }
    }
}

bool AssetManager::init(const std::string& customRoot) {
    detectAssetsRoot(customRoot);

    // Cache Windows system bold/regular fonts for crisp, clean UI typography
    m_sysBoldLoaded = m_sysBoldFont.loadFromFile("C:/Windows/Fonts/arialbd.ttf") ||
                      m_sysBoldFont.loadFromFile("C:/Windows/Fonts/segoeuib.ttf");

    m_sysRegularLoaded = m_sysRegularFont.loadFromFile("C:/Windows/Fonts/arial.ttf") ||
                         m_sysRegularFont.loadFromFile("C:/Windows/Fonts/segoeui.ttf");

    m_initialized = true;
    return true;
}

std::string AssetManager::resolvePath(const std::string& relativePath) const {
    fs::path root = fs::u8path(m_assetsRoot);
    fs::path rel = fs::u8path(relativePath);
    return (root / rel).lexically_normal().generic_string();
}

bool AssetManager::validateAll(std::vector<std::string>& missing) {
    missing.clear();

    for (const auto& def : AssetRegistry::TEXTURES) {
        std::string fullPath = resolvePath(def.relativePath);
        std::error_code ec;
        if (!fs::exists(fs::u8path(fullPath), ec)) {
            missing.push_back("[Texture] " + std::string(def.relativePath) + " -> " + fullPath);
        }
    }

    for (const auto& def : AssetRegistry::FONTS) {
        std::string fullPath = resolvePath(def.relativePath);
        std::error_code ec;
        if (!fs::exists(fs::u8path(fullPath), ec)) {
            missing.push_back("[Font] " + std::string(def.relativePath) + " -> " + fullPath);
        }
    }

    for (const auto& def : AssetRegistry::SOUNDS) {
        std::string fullPath = resolvePath(def.relativePath);
        std::error_code ec;
        if (!fs::exists(fs::u8path(fullPath), ec)) {
            missing.push_back("[Sound] " + std::string(def.relativePath) + " -> " + fullPath);
        }
    }

    for (const auto& def : AssetRegistry::MUSIC) {
        std::string fullPath = resolvePath(def.relativePath);
        std::error_code ec;
        if (!fs::exists(fs::u8path(fullPath), ec)) {
            missing.push_back("[Music] " + std::string(def.relativePath) + " -> " + fullPath);
        }
    }

    return missing.empty();
}

bool AssetManager::loadTexture(TextureId id) {
    size_t idx = static_cast<size_t>(id);
    if (idx >= AssetRegistry::TEXTURES.size()) {
        std::cerr << "[AssetManager] Invalid TextureId index: " << idx << std::endl;
        return false;
    }

    const auto& def = AssetRegistry::TEXTURES[idx];
    std::string fullPath = resolvePath(def.relativePath);

    std::error_code ec;
    if (!fs::exists(fs::u8path(fullPath), ec)) {
        std::cerr << "[AssetManager] Texture file not found: " << fullPath << std::endl;
        return false;
    }

    auto tex = std::make_unique<sf::Texture>();

    if (def.policy == TextureLoadPolicy::MASK_WHITE_SMOOTH) {
        sf::Image img;
        if (!img.loadFromFile(fullPath)) {
            std::cerr << "[AssetManager] Failed to load image for mask: " << fullPath << std::endl;
            return false;
        }

        // Detect if alpha channel is already present
        bool hasAlpha = false;
        sf::Vector2u sz = img.getSize();
        for (unsigned int y = 0; y < sz.y && !hasAlpha; ++y) {
            for (unsigned int x = 0; x < sz.x; ++x) {
                if (img.getPixel(x, y).a < 250) {
                    hasAlpha = true;
                    break;
                }
            }
        }

        if (!hasAlpha) {
            img.createMaskFromColor(sf::Color::White);
        }

        if (!tex->loadFromImage(img)) {
            std::cerr << "[AssetManager] Failed to create texture from masked image: " << fullPath << std::endl;
            return false;
        }
        tex->setSmooth(true);
    } else {
        if (!tex->loadFromFile(fullPath)) {
            std::cerr << "[AssetManager] Failed to load texture: " << fullPath << std::endl;
            return false;
        }
        if (def.policy == TextureLoadPolicy::SMOOTH) {
            tex->setSmooth(true);
        } else if (def.policy == TextureLoadPolicy::PIXEL_ART) {
            tex->setSmooth(false);
        }
    }

    m_textures.insert(id, std::move(tex));
    return true;
}

const sf::Texture& AssetManager::texture(TextureId id) {
    if (!m_textures.has(id)) {
        loadTexture(id);
    }
    return m_textures.get(id);
}

const sf::Texture& AssetManager::getPieceTexture(const std::string& tenQuan, Mau mau) {
    TextureId id = TextureId::PIECE_R_TUONG;

    if (mau == Mau::DO) {
        if (tenQuan == "Tuong") id = TextureId::PIECE_R_TUONG;
        else if (tenQuan == "Si") id = TextureId::PIECE_R_SI;
        else if (tenQuan == "Voi") id = TextureId::PIECE_R_VOI;
        else if (tenQuan == "Ma") id = TextureId::PIECE_R_MA;
        else if (tenQuan == "Xe") id = TextureId::PIECE_R_XE;
        else if (tenQuan == "Phao") id = TextureId::PIECE_R_PHAO;
        else if (tenQuan == "Tot") id = TextureId::PIECE_R_TOT;
    } else {
        if (tenQuan == "Tuong") id = TextureId::PIECE_B_TUONG;
        else if (tenQuan == "Si") id = TextureId::PIECE_B_SI;
        else if (tenQuan == "Voi") id = TextureId::PIECE_B_VOI;
        else if (tenQuan == "Ma") id = TextureId::PIECE_B_MA;
        else if (tenQuan == "Xe") id = TextureId::PIECE_B_XE;
        else if (tenQuan == "Phao") id = TextureId::PIECE_B_PHAO;
        else if (tenQuan == "Tot") id = TextureId::PIECE_B_TOT;
    }

    return texture(id);
}

const sf::Texture& AssetManager::getAvatarTexture(int index, Mau mau) {
    (void)mau;
    int clamped = std::clamp(index, 0, 4);
    TextureId id = static_cast<TextureId>(static_cast<int>(TextureId::AVATAR_0) + clamped);
    return texture(id);
}

bool AssetManager::loadFont(FontId id) {
    size_t idx = static_cast<size_t>(id);
    if (idx >= AssetRegistry::FONTS.size()) {
        std::cerr << "[AssetManager] Invalid FontId index: " << idx << std::endl;
        return false;
    }

    const auto& def = AssetRegistry::FONTS[idx];
    std::string fullPath = resolvePath(def.relativePath);

    std::error_code ec;
    if (!fs::exists(fs::u8path(fullPath), ec)) {
        std::cerr << "[AssetManager] Font file not found: " << fullPath << std::endl;
        return false;
    }

    auto f = std::make_unique<sf::Font>();
    if (!f->loadFromFile(fullPath)) {
        std::cerr << "[AssetManager] Failed to load font: " << fullPath << std::endl;
        return false;
    }

    m_fonts.insert(id, std::move(f));
    return true;
}

const sf::Font& AssetManager::font(FontId id) {
    if (!m_fonts.has(id)) {
        loadFont(id);
    }
    return m_fonts.get(id);
}

const sf::Font& AssetManager::getFont(bool tiengViet, bool bold) {
    if (bold) {
        if (m_sysBoldLoaded) {
            return m_sysBoldFont;
        }
        if (!tiengViet) {
            return font(FontId::PIXELOID_SANS_BOLD);
        }
        return font(FontId::NOM_NA_TONG);
    }

    if (tiengViet) {
        if (m_sysRegularLoaded) {
            return m_sysRegularFont;
        }
        return font(FontId::NOM_NA_TONG);
    }

    return font(FontId::PIXELOID_SANS);
}

const sf::Font& AssetManager::getBoldFont(bool tiengViet) {
    return getFont(tiengViet, true);
}

bool AssetManager::loadSound(SoundId id) {
    size_t idx = static_cast<size_t>(id);
    if (idx >= AssetRegistry::SOUNDS.size()) {
        std::cerr << "[AssetManager] Invalid SoundId index: " << idx << std::endl;
        return false;
    }

    const auto& def = AssetRegistry::SOUNDS[idx];
    std::string fullPath = resolvePath(def.relativePath);

    std::error_code ec;
    if (!fs::exists(fs::u8path(fullPath), ec)) {
        std::cerr << "[AssetManager] Sound file not found: " << fullPath << std::endl;
        return false;
    }

    auto buf = std::make_unique<sf::SoundBuffer>();
    if (!buf->loadFromFile(fullPath)) {
        std::cerr << "[AssetManager] Failed to load sound buffer: " << fullPath << std::endl;
        return false;
    }

    m_sounds.insert(id, std::move(buf));
    return true;
}

const sf::SoundBuffer& AssetManager::sound(SoundId id) {
    if (!m_sounds.has(id)) {
        loadSound(id);
    }
    return m_sounds.get(id);
}

std::string AssetManager::getMusicPath(MusicId id) {
    size_t idx = static_cast<size_t>(id);
    if (idx < AssetRegistry::MUSIC.size()) {
        return resolvePath(AssetRegistry::MUSIC[idx].relativePath);
    }
    return "";
}
