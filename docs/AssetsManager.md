# Gestionnaire d'Assets en C++20/23 avec SFML & nlohmann\_json

## Introduction

Ce document explique en détail le fonctionnement d'un gestionnaire d'assets en **C++20/23**, utilisant **SFML** pour la gestion graphique et **nlohmann\_json** pour la gestion des fichiers JSON. Il repose sur des concepts modernes comme :

- **Smart Pointers (**``**)** : Gestion automatique de la mémoire
- **Templates & Concepts** : Code générique et sécurisé
- **Multithreading (**``**, **``**)** : Chargement optimisé
- **Optimisation (**``**, **``**)** : Éviter les copies inutiles

## Fonctionnalités principales

- **Chargement générique d'assets** : Textures, polices, sons, JSON
- **Chargement asynchrone** pour éviter de bloquer le thread principal
- **Gestion des spritesheets & animations**
- **Accès optimisé aux assets** grâce à `std::unordered_map`
- **Protection multi-threading** pour éviter les conflits

## Explication du Code

### 1. Singleton AssetManager

Le `AssetManager` est un singleton : une seule instance globale est créée et accessible via `AssetManager::Instance()`. Cela garantit que tous les assets sont partagés à travers le programme.

```cpp
class AssetManager {
public:
    static AssetManager& Instance() {
        static AssetManager instance;
        return instance;
    }
    // ...
};
```

### 2. Utilisation de `std::shared_ptr` (Smart Pointer)

Nous utilisons `std::shared_ptr` pour stocker et partager les assets, ce qui évite les fuites mémoire.

```cpp
std::unordered_map<std::string, std::shared_ptr<sf::Texture>> textures;
```

### 3. Définition du `AssetType` avec `concept`

Le concept `AssetType` permet de **restreindre les types d'assets autorisés**. Cela empêche l'utilisation accidentelle d'un type non supporté et améliore la lisibilité du code.

```cpp
template <typename T>
concept AssetType = std::same_as<T, sf::Texture> ||
                    std::same_as<T, sf::Font> ||
                    std::same_as<T, sf::SoundBuffer> ||
                    std::same_as<T, nlohmann::json>;
```

Avec ce `concept`, si un développeur tente d'utiliser un type non supporté avec `LoadAsset<T>`, une erreur de compilation claire sera générée.

### 4. Chargement générique avec `if constexpr`

Grâce aux **templates**, notre fonction `LoadAsset<T>` peut charger différents types d'assets. `if constexpr` permet de spécialiser le comportement en fonction du type d'asset.

```cpp
template<AssetType T>
std::shared_ptr<T> LoadAsset(std::string_view filename) {
    if constexpr (std::same_as<T, sf::Texture> || std::same_as<T, sf::Font>) {
        if (!asset->loadFromFile(std::string(filename))) {
            throw std::runtime_error("Erreur de chargement");
        }
    } else if constexpr (std::same_as<T, nlohmann::json>) {
        std::ifstream file(std::string(filename));
        file >> *asset;
    }
    return asset;
}
```

### 5. Utilisation de `std::string_view` pour l'optimisation

Au lieu de passer `std::string`, on utilise `std::string_view`, qui évite les copies inutiles de chaînes de caractères.

```cpp
std::shared_ptr<T> LoadAsset(std::string_view filename);
```

### 6. Gestion multi-thread avec `std::shared_mutex`

Nous utilisons `std::shared_mutex` pour permettre **plusieurs lectures simultanées**, tout en bloquant l'accès en écriture.

```cpp
mutable std::shared_mutex mutex;
std::unique_lock lock(mutex); // Pour écriture
std::shared_lock lock(mutex); // Pour lecture
```

### 7. Chargement asynchrone avec `std::async`

La fonction `LoadAssetAsync` permet de charger les assets en **background**, sans bloquer le thread principal.

```cpp
template<AssetType T>
std::future<std::shared_ptr<T>> LoadAssetAsync(std::string_view filename) {
    return std::async(std::launch::async, [this, filename]() {
        return LoadAsset<T>(filename);
    });
}
```

### 8. Chargement d'une animation avec `std::vector<std::shared_ptr<sf::Texture>>`

Nous chargeons une **succession d'images** depuis une spritesheet pour les animations.

```cpp
std::vector<std::shared_ptr<sf::Texture>> LoadAnimationFrames(
    std::string_view filename, sf::Vector2i frameSize, int frameCount) {
    std::vector<std::shared_ptr<sf::Texture>> frames;
    auto texture = LoadAsset<sf::Texture>(filename);
    for (int i = 0; i < frameCount; ++i) {
        sf::IntRect rect(i * frameSize.x, 0, frameSize.x, frameSize.y);
        auto frameTexture = std::make_shared<sf::Texture>();
        frameTexture->loadFromImage(texture->copyToImage(), rect);
        frames.emplace_back(frameTexture);
    }
    return frames;
}
```

## Exemple d'utilisation

```cpp
int main() {
    auto texture = AssetManager::Instance().LoadAsset<sf::Texture>("assets/player.png");
    auto futureFont = AssetManager::Instance().LoadAssetAsync<sf::Font>("assets/font.ttf");
    auto walkFrames = AssetManager::Instance().LoadAnimationFrames("assets/spritesheet.png", {64, 64}, 6);
    auto font = futureFont.get();
    return 0;
}
```

## Avantages et Optimisations

✅ `` → Gestion mémoire automatique\
✅ `` → Accès multi-thread sécurisé\
✅ `` → Chargement non-bloquant\
✅ `` → Réduction des copies\
✅ `` → Génération de code optimisé\
✅ **Gestion des spritesheets** pour l'animation

## Améliorations possibles

🔹 Ajouter un cache pour éviter le recalcul des sous-textures\
🔹 Charger les animations via un fichier JSON\
🔹 Optimiser la gestion mémoire pour les assets lourds

Ce gestionnaire offre une base **robuste, optimisée et flexible** pour la gestion d'assets dans un moteur de jeu en **C++ moderne** ! 🚀

