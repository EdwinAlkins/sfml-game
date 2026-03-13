#include "AssetsManager.hpp"


// template<AssetType T>
// std::shared_ptr<T> AssetsManager::LoadAsset(std::string_view filename) {
//     std::unique_lock lock(mutex); // Empêche accès concurrentiel en écriture

//     auto& assetMap = getAssetMap<T>();

//     // Recherche optimisée avec string_view
//     if (auto it = assetMap.find(filename); it != assetMap.end()) {
//         return it->second;
//     }

//     // Chargement de l'asset
//     auto asset = std::make_shared<T>();
//     if constexpr (std::same_as<T, sf::Texture> || std::same_as<T, sf::Font> || std::same_as<T, sf::SoundBuffer>) {
//         if (!asset->loadFromFile(std::string(filename))) {
//             throw std::runtime_error("Failed to load asset: " + std::string(filename));
//         }
//     } else if constexpr (std::same_as<T, nlohmann::json>) {
//         std::ifstream file(std::string(filename));
//         if (!file) {
//             throw std::runtime_error("Failed to load JSON: " + std::string(filename));
//         }
//         file >> *asset;
//     }

//     assetMap.emplace(filename, asset);
//     return asset;
// }

// template<AssetType T>
// std::future<std::shared_ptr<T>> AssetsManager::LoadAssetAsync(std::string_view filename) {
//     return std::async(std::launch::async, [this, filename]() {
//         return LoadAsset<T>(filename);
//     });
// }

// std::vector<std::shared_ptr<sf::Texture>> AssetsManager::LoadAnimationFrames(
//     std::string_view filename, sf::Vector2i frameSize, int frameCount, int startX = 0, int startY = 0) {

//     std::vector<std::shared_ptr<sf::Texture>> frames;
//     auto texture = LoadAsset<sf::Texture>(filename);

//     for (int i = 0; i < frameCount; ++i) {
//         sf::IntRect rect(startX + i * frameSize.x, startY, frameSize.x, frameSize.y);
//         frames.emplace_back(std::make_shared<sf::Texture>(*texture, rect));
//     }

//     return frames;
// }

// template<AssetType T>
// auto& AssetsManager::getAssetMap() {
//     if constexpr (std::same_as<T, sf::Texture>) return textures;
//     else if constexpr (std::same_as<T, sf::Font>) return fonts;
//     else if constexpr (std::same_as<T, sf::SoundBuffer>) return sounds;
//     else if constexpr (std::same_as<T, nlohmann::json>) return jsonData;
// }
