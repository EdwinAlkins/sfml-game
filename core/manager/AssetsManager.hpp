#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <memory>
#include <future>
#include <shared_mutex>
#include <fstream>
#include <iostream>
#include <vector>
#include <concepts>

// 🔹 Concept pour restreindre les types d'assets autorisés
template <typename T>
concept AssetType = std::same_as<T, sf::Texture> ||
                    std::same_as<T, sf::Font> ||
                    std::same_as<T, sf::SoundBuffer> ||
                    std::same_as<T, nlohmann::json>;

class AssetsManager {
public:
    static AssetsManager& Instance() {
        static AssetsManager instance;
        return instance;
    }

    // 🔹 Chargement générique d'un asset avec std::string_view (accès optimisé)
    template<AssetType T>
    std::shared_ptr<T> LoadAsset(std::string_view filename) {
        std::unique_lock lock(mutex); // Empêche accès concurrentiel en écriture

        auto& assetMap = getAssetMap<T>();

        // Recherche optimisée avec string_view
        if (auto it = assetMap.find(filename); it != assetMap.end()) {
            return it->second;
        }

        // Chargement de l'asset
        auto asset = std::make_shared<T>();
        if constexpr (std::same_as<T, sf::Texture> || std::same_as<T, sf::Font> || std::same_as<T, sf::SoundBuffer>) {
            if (!asset->loadFromFile(std::string(filename))) {
                throw std::runtime_error("Failed to load asset: " + std::string(filename));
            }
        } else if constexpr (std::same_as<T, nlohmann::json>) {
            std::ifstream file{std::string(filename)};
            if (!file) {
                throw std::runtime_error("Failed to load JSON: " + std::string(filename));
            }
            file >> *asset;
        }

        assetMap.emplace(filename, asset);
        return asset;
    }

    // 🔹 Chargement asynchrone
    template<AssetType T>
    std::future<std::shared_ptr<T>> LoadAssetAsync(std::string_view filename) {
        return std::async(std::launch::async, [this, filename]() {
            return LoadAsset<T>(filename);
        });
    }

    // 🔹 Chargement d'une animation (retourne std::span pour éviter copies inutiles)
    std::vector<std::shared_ptr<sf::Texture>> LoadAnimationFrames(
        std::string_view filename, sf::Vector2i frameSize, int frameCount, int startX = 0, int startY = 0) {

        std::vector<std::shared_ptr<sf::Texture>> frames;
        auto texture = LoadAsset<sf::Texture>(filename);

        for (int i = 0; i < frameCount; ++i) {
            sf::IntRect rect(startX + i * frameSize.x, startY, frameSize.x, frameSize.y);
            auto frameTexture = std::make_shared<sf::Texture>();
            frameTexture->loadFromImage(texture->copyToImage(), rect);
            frames.emplace_back(frameTexture);
        }

        return frames;
    }

private:
    std::unordered_map<std::string_view, std::shared_ptr<sf::Texture>> textures;
    std::unordered_map<std::string_view, std::shared_ptr<sf::Font>> fonts;
    std::unordered_map<std::string_view, std::shared_ptr<sf::SoundBuffer>> sounds;
    std::unordered_map<std::string_view, std::shared_ptr<nlohmann::json>> jsonData;
    mutable std::shared_mutex mutex; // 🔹 Permet plusieurs lectures simultanées

    AssetsManager() = default;

    // 🔹 Retourne la bonne map en fonction du type (avec if constexpr)
    template<AssetType T>
    auto& getAssetMap() {
        if constexpr (std::same_as<T, sf::Texture>) return textures;
        else if constexpr (std::same_as<T, sf::Font>) return fonts;
        else if constexpr (std::same_as<T, sf::SoundBuffer>) return sounds;
        else if constexpr (std::same_as<T, nlohmann::json>) return jsonData;
    }
};
