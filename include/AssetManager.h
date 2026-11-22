#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include "AssetPaths.h"
#include "BanCo.h" // For Mau enum

/**
 * ============================================================================
 * Design Patterns Applied:
 * 1. Singleton (Meyers Singleton): Exactly one instance coordinates all resources.
 * 2. Facade: Exposes a unified, clean interface for textures, fonts, and audio.
 * 3. Flyweight (ResourceCache<T, Id>): Ensures each resource is loaded at most
 *    once into memory in a std::unique_ptr, handed out via const reference.
 * 4. Policy-based Loader: Encapsulates image transparency masking, smoothing,
 *    and format handling inside dedicated loader routines.
 * ============================================================================
 */

template <typename Resource, typename Id>
class ResourceCache {
private:
    std::unordered_map<Id, std::unique_ptr<Resource>> m_cache;
    Resource m_fallback;

public:
    ResourceCache() = default;
    ~ResourceCache() = default;

    ResourceCache(const ResourceCache&) = delete;
    ResourceCache& operator=(const ResourceCache&) = delete;
    ResourceCache(ResourceCache&&) noexcept = default;
    ResourceCache& operator=(ResourceCache&&) noexcept = default;

    bool has(Id id) const {
        return m_cache.find(id) != m_cache.end();
    }

    const Resource& get(Id id) const {
        auto it = m_cache.find(id);
        if (it != m_cache.end() && it->second) {
            return *(it->second);
        }
        return m_fallback;
    }

    Resource* getPtr(Id id) {
        auto it = m_cache.find(id);
        if (it != m_cache.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    void insert(Id id, std::unique_ptr<Resource> resource) {
        m_cache[id] = std::move(resource);
    }

    void clear() {
        m_cache.clear();
    }

    size_t size() const {
        return m_cache.size();
    }
};

class AssetManager {
private:
    AssetManager();
    ~AssetManager() = default;

    std::string m_assetsRoot;
    bool m_initialized = false;

    ResourceCache<sf::Texture, TextureId> m_textures;
    ResourceCache<sf::Font, FontId> m_fonts;
    ResourceCache<sf::SoundBuffer, SoundId> m_sounds;

    // System font fallbacks (cached for crisp bold/vietnamese legibility)
    sf::Font m_sysBoldFont;
    bool m_sysBoldLoaded = false;
    sf::Font m_sysRegularFont;
    bool m_sysRegularLoaded = false;

    // Internal loader helpers
    bool loadTexture(TextureId id);
    bool loadFont(FontId id);
    bool loadSound(SoundId id);

    void detectAssetsRoot(const std::string& customRoot = "");

public:
    // Meyers Singleton
    static AssetManager& get();

    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;
    AssetManager(AssetManager&&) = delete;
    AssetManager& operator=(AssetManager&&) = delete;

    // Lifecycle
    bool init(const std::string& customRoot = "");
    bool isInitialized() const { return m_initialized; }
    const std::string& getAssetsRoot() const { return m_assetsRoot; }
    std::string resolvePath(const std::string& relativePath) const;

    // Validation
    bool validateAll(std::vector<std::string>& missing);

    // Texture Access
    const sf::Texture& texture(TextureId id);
    const sf::Texture& getPieceTexture(const std::string& tenQuan, Mau mau);
    const sf::Texture& getAvatarTexture(int index);

    // Font Access
    const sf::Font& font(FontId id);
    const sf::Font& getFont(bool tiengViet, bool bold = false);
    const sf::Font& getBoldFont(bool tiengViet = true);

    // Sound Buffer Access
    const sf::SoundBuffer& sound(SoundId id);

    // Music streaming path
    std::string getMusicPath(MusicId id);
};
