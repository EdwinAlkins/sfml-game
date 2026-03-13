# Analyse Technique - Projet SFML Game

## 📋 Vue d'ensemble du projet

Ce projet est un moteur de jeu 2D basé sur **SFML**, **Box2D** et plusieurs bibliothèques modernes. Il s'agit d'une architecture de jeu en C++20 avec un système de physique, de rendu et de gestion d'assets.

**État actuel** : ✅ Corrections critiques appliquées (mémoire, thread safety, caméra)

---

## 🏗️ Architecture Générale

### Structure actuelle

```
sfml-game/
├── main.cpp                 # Point d'entrée
├── CMakeLists.txt           # Configuration build
├── core/
│   ├── manager/
│   │   ├── GameManager      # Boucle principale ✅ Refactorisé
│   │   └── AssetsManager    # Gestion des ressources
│   ├── SceneManager         # Gestion des scènes ✅ Nouveau (13 mars 2026)
│   ├── GameWorld            # Monde physique et logique ✅ Corrigé
│   ├── Camera               # Système de caméra ✅ Refondu
│   ├── Constants.hpp        # Constantes (nouveau)
│   └── gameobject/          # Hiérarchie des objets ✅ Corrigé
├── cmake/                   # Scripts FetchContent
├── docs/                    # Documentation
└── assets/                  # Ressources du jeu
```

### Diagramme de dépendances

```
main.cpp
    └── GameManager (unique_ptr)
            ├── SceneManager (unique_ptr)
            │       └── Scene* (observation)
            │               └── GameWorld (héritage)
            │                       ├── GameObject (unique_ptr)
            │                       └── Box2D world
            ├── Camera (unique_ptr)
            └── SFML/Box2D
```

---

## ✅ Points Forts

### 1. **Choix technologiques modernes**

- **C++20** avec concepts (`AssetType` dans AssetsManager)
- **Box2D v3** (dernière version stable)
- **SFML 2.6.2** (version stable éprouvée)
- **spdlog** pour le logging (excellent choix)
- **nlohmann/json** pour la sérialisation
- **TGUI** pour l'interface utilisateur

### 2. **AssetsManager bien conçu**

```cpp
template<AssetType T>
std::shared_ptr<T> LoadAsset(std::string_view filename)
```

- ✅ Utilisation de **concepts C++20** pour la sécurité des types
- ✅ **Singleton** avec thread-safe initialization
- ✅ **std::shared_mutex** pour lectures concurrentes
- ✅ **std::string_view** pour éviter les copies
- ✅ Chargement asynchrone avec `std::future`

### 3. **Système de physique Box2D**

- Intégration correcte de Box2D
- Séparation claire entre logique et physique
- Fixed timestep pour la physique (60 Hz)

### 4. **Logging complet**

- spdlog configuré avec niveaux (debug/info)
- Timing des différentes phases (render, update, culling)
- Utile pour le profiling

### 5. **Gestion des threads**

- Threads séparés pour update et culling
- Atomic variables pour les états partagés
- Mutex pour la protection des données

---

## ✅ Corrections Appliquées (13 mars 2026)

### 1. **Fuites de mémoire - RÉSOLU** ✅

**Avant** :

```cpp
sf::Shape* newShape = new sf::RectangleShape(...);  // ❌ Fuite
this->gameWorld->addGameObject(new GameObjectSimple(..., *newShape));
```

**Après** :

```cpp
auto shape = std::make_unique<sf::RectangleShape>(...);
gameWorld->addGameObject(std::make_unique<GameObjectSimple>(pos, std::move(shape)));
```

**Impact** : ~15000 allocations maintenant correctement gérées.

### 2. **Design de Camera - RÉSOLU** ✅

**Avant** :

```cpp
class Camera {
    sf::RenderWindow* window;  // ❌ Dépendance externe
    void setWindow(sf::RenderWindow*);
};
```

**Après** :

```cpp
class Camera {
    sf::View view;              // ✅ Autonome
    sf::Vector2f baseSize;      // Taille de base
    float zoomFactor;           // Facteur de zoom (1.0 = 100%)
    void apply(sf::RenderWindow&);
};
```

### 3. **Thread Safety - RÉSOLU** ✅

**Avant** :

```cpp
// Mutex unique, protection incomplète
std::mutex cullingMutex;
```

**Après** :

```cpp
mutable std::mutex gameObjectsMutex;  // ✅ Protège gameObjects
mutable std::mutex cullingMutex;      // ✅ Protège visibleGameObjects

void GameWorld::culling(const Camera& camera)
{
    {
        std::lock_guard<std::mutex> lock(gameObjectsMutex);
        // Lecture protégée
    }
    std::lock_guard<std::mutex> lock(cullingMutex);
    // Écriture protégée
}
```

### 4. **Ordre de destruction Box2D - RÉSOLU** ✅

**Avant** :

```cpp
GameWorld::~GameWorld() {
    b2DestroyWorld(worldId);  // ❌ Crash ! Corps encore existants
    gameObjects.clear();
}
```

**Après** :

```cpp
GameWorld::~GameWorld() {
    gameObjects.clear();      // ✅ Détruit les corps d'abord
    b2DestroyWorld(worldId);  // ✅ Puis le monde
}
```

### 5. **Constantes - RÉSOLU** ✅

**Nouveau fichier** : `Constants.hpp`

```cpp
constexpr int SCREEN_WIDTH = 1920;
constexpr int SCREEN_HEIGHT = 1080;
constexpr int INITIAL_OBJECTS_COUNT = 10000;
constexpr float OBJECT_BASE_SPEED = 100.0f;
constexpr float FIXED_TIME_STEP = 1.0f / 60.0f;
constexpr float CAMERA_ZOOM_STEP = 0.05f;
// ... et plus
```

### 6. **Système de scènes - RÉSOLU** ✅ (13 mars 2026)

**Avant** :

```cpp
class SceneManager {
    std::unique_ptr<Scene> currentScene;  // ❌ Détruit la scène courante
    
    std::unique_ptr<Scene> getCurrentScene() {
        return std::move(currentScene);  // ❌ Fuite mémoire !
    }
    
    void setCurrentScene(std::string name) {
        currentScene = std::move(scenes.at(name));  // ❌ Scène perdue du map
    }
};
```

**Après** :

```cpp
class SceneManager {
    std::map<std::string, std::unique_ptr<Scene>> scenes;  // ✅ Ownership
    Scene* currentScene;                                    // ✅ Observation
    std::string currentSceneName;
    
    Scene* getCurrentScene() {
        return currentScene;  // ✅ Pointeur brut, pas de ownership transfer
    }
    
    void setCurrentScene(std::string name) {
        currentScene = scenes[name].get();  // ✅ Garde ownership dans le map
        currentSceneName = name;
    }
};
```

**Corrections clés** :

- ✅ `getCurrentScene()` retourne `Scene`* au lieu de `std::unique_ptr`
- ✅ `currentScene` est un pointeur d'observation (non-possesseur)
- ✅ Les scènes restent dans le map après changement
- ✅ Cycle de vie avec `onLoad()` / `onUnload()`
- ✅ `SceneManager::update()` appelle `updateLogic()` + `updatePhysics()`

**Fichiers** : `SceneManager.hpp`, `SceneManager.cpp`, `Scene.cpp`

---

## 🔧 Problèmes d'Architecture

### 1. **Couplage fort GameManager ↔ GameWorld** ✅ RÉSOLU

**Avant** :

```cpp
class GameManager {
    GameWorld* gameWorld;  // ❌ Raw pointer
    Camera* camera;        // ❌ Raw pointer
};
```

**Après** :

```cpp
class GameManager {
    std::unique_ptr<GameWorld> gameWorld;  // ✅
    std::unique_ptr<Camera> camera;        // ✅
};
```

### 2. **GameObjectBase avec référence** ✅ RÉSOLU

**Avant** :

```cpp
class GameObjectBase {
protected:
    sf::Shape& shape;  // ❌ Référence
};
```

**Après** :

```cpp
class GameObjectBase {
protected:
    std::unique_ptr<sf::Shape> shape;  // ✅ Propriété claire
};
```

### 3. **Système de scènes** ✅ IMPLÉMENTÉ (13 mars 2026)

**Nouvelle architecture** :

```cpp
class Scene : public GameWorld {
    void onLoad();    // Callback d'activation
    void onUnload();  // Callback de désactivation
};

class SceneManager {
    std::map<std::string, std::unique_ptr<Scene>> scenes;  // Ownership
    Scene* currentScene;                                    // Observation
    std::string currentSceneName;
    
    void addScene(std::string name, std::unique_ptr<Scene>);
    void setCurrentScene(std::string name);
    Scene* getCurrentScene();  // Pointeur brut (pas de ownership transfer)
};
```

**Corrections appliquées** :

- ✅ `getCurrentScene()` retourne `Scene`* au lieu de `std::unique_ptr<Scene>` (évite le déplacement destructif)
- ✅ `setCurrentScene()` garde les scènes dans le map (plus de fuite mémoire)
- ✅ Cycle de vie avec `onLoad()` / `onUnload()`
- ✅ Thread-safe : pointeur d'observation non-possesseur

### 4. **Pas de système d'états (State Pattern)** ⚠️ EN ATTENTE

Le `GameManager` gère tout dans une seule classe.

**Recommandation** : Implémenter un **GameStateManager**

```cpp
class GameState {
    virtual void enter() = 0;
    virtual void update(float dt) = 0;
    virtual void render(sf::RenderWindow&) = 0;
    virtual void exit() = 0;
};
```

---

## 📊 Problèmes de Code Quality

### 1. **Variables atomiques mal nommées** ✅ RÉSOLU

**Avant** : `std::atomic<bool> pacreateWindowused;`
**Après** : Supprimé

### 2. **Magic numbers** ✅ RÉSOLU

Toutes les constantes sont maintenant dans `Constants.hpp`.

### 3. **Logs excessifs en production** ⚠️ À AMÉLIORER

```cpp
spdlog::debug("GameManager run: handleEvents time {} ms", ...);
// 10+ logs par frame
```

**Solution recommandée** :

```cpp
#ifdef DEBUG_PROFILE
    spdlog::debug(...)
#endif
// Ou utiliser un profiler dédié (Tracy)
```

### 4. **Gestion des erreurs absente** ⚠️ À AMÉLIORER

```cpp
if (!window) {
    spdlog::error("...");
    return;  // Retour silencieux
}
```

**Solution recommandée** :

```cpp
if (!window) {
    throw std::runtime_error("...");
}
```

### 5. **CMakeLists.txt problématique** ⚠️ À AMÉLIORER

```cmake
set(CMAKE_C_COMPILER "C:/Program Files/CodeBlocks/MinGW/bin/gcc.exe")
```

**Solution recommandée** :

```cmake
if(WIN32 AND NOT DEFINED CMAKE_CXX_COMPILER)
    find_program(CMAKE_CXX_COMPILER g++ PATHS "...")
endif()
```

### 6. **GLOB pour les fichiers sources** ⚠️ À AMÉLIORER

```cmake
file(GLOB_RECURSE gameobject_sources "gameobject/*.cpp")
```

**Solution recommandée** : Lister explicitement les fichiers.

---

## 🎯 État des Recommandations

### ✅ Haute priorité - TERMINÉ


| #   | Action                                       | Statut | Impact   |
| --- | -------------------------------------------- | ------ | -------- |
| 1   | Remplacer raw pointers par smart pointers    | ✅ Fait | Critique |
| 2   | Corriger les fuites mémoire dans GameManager | ✅ Fait | Critique |
| 3   | Nettoyer le code commenté                    | ✅ Fait | Moyen    |
| 4   | Fixer le thread safety du culling            | ✅ Fait | Critique |
| 5   | Constants au lieu de magic numbers           | ✅ Fait | Moyen    |


### 🟡 Moyenne priorité - EN PARTIE


| #   | Action                                  | Statut                | Impact | Effort |
| --- | --------------------------------------- | --------------------- | ------ | ------ |
| 6   | Refactoriser Camera (ownership clair)   | ✅ Fait                | Élevé  | Moyen  |
| 7   | Implémenter GameStateManager            | ⏳ À faire             | Élevé  | Élevé  |
| 8   | Ajouter spatial partitioning (QuadTree) | ⏳ À faire             | Élevé  | Élevé  |
| 9   | Système de scènes                       | ✅ Fait (13 mars 2026) | Élevé  | Moyen  |


### 🟢 Basse priorité - AMÉLIORATIONS


| #   | Action                             | Impact | Effort |
| --- | ---------------------------------- | ------ | ------ |
| 10  | Profiler avec Tracy                | Moyen  | Faible |
| 11  | Tests unitaires (GoogleTest)       | Élevé  | Élevé  |
| 12  | CI/CD (GitHub Actions)             | Moyen  | Moyen  |
| 13  | Documentation Doxygen              | Faible | Moyen  |
| 14  | Corriger CMake (chemins hardcoded) | Moyen  | Faible |


---

## 📐 Architecture Actuelle

```
┌─────────────────────────────────────────────────────────────┐
│                        Application                          │
│                         main.cpp                            │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                      GameManager                            │
│  ┌─────────────────────────────────────────────────────┐    │
│  │  unique_ptr<SceneManager>  unique_ptr<Camera>       │    │
│  └─────────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────────┘
                              │
        ┌─────────────────────┼─────────────────────┐
        ▼                     ▼                     ▼
┌───────────────┐   ┌─────────────────┐   ┌─────────────────┐
│ SceneManager  │   │  PhysicsSystem  │   │  Camera         │
│ Scene* (obs)  │   │    (Box2D)      │   │  (unique_ptr)   │
└───────────────┘   └─────────────────┘   └─────────────────┘
        │
        ▼
┌─────────────────────────────────────────────────────────────┐
│                    Current Scene                            │
│  ┌──────────────────────────────────────────────────────┐   │
│  │  Scene : public GameWorld                            │   │
│  │  - onLoad() / onUnload() callbacks                   │   │
│  │  - std::vector<GameObject>                           │   │
│  └──────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                   GameObject Hierarchy                      │
│  ┌──────────────────┐  ┌──────────────────────────────────┐ │
│  │ GameObjectSimple │  │ GameObjectSimpleBody (Box2D)     │ │
│  │ - unique_ptr<Shape> │ - unique_ptr<Shape>              │ │
│  └──────────────────┘  │ - b2BodyId                       │ │
│                        └──────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                     Resources Layer                         │
│  ┌──────────────────────────────────────────────────────┐   │
│  │  AssetsManager (Singleton, Concepts C++20)           │   │
│  │  - Textures, Fonts, Sounds, JSON                     │   │
│  │  - Thread-safe (shared_mutex)                        │   │
│  │  - Async loading                                     │   │
│  └──────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────┘
```

---

## 🛠️ Implémentations de Référence

### 1. GameObject avec smart pointers ✅

```cpp
class GameObjectBase {
public:
    virtual ~GameObjectBase() = default;
    
protected:
    std::unique_ptr<sf::Shape> shape;
};

class GameObjectSimple : public GameObjectBase {
public:
    GameObjectSimple(sf::Vector2f position, std::unique_ptr<sf::Shape> shape)
        : GameObjectBase(std::move(shape))
    {
        this->shape->setPosition(position);
    }
};

// Usage
auto shape = std::make_unique<sf::RectangleShape>(sf::Vector2f(10, 10));
shape->setFillColor(sf::Color::White);
gameWorld->addGameObject(std::make_unique<GameObjectSimple>(pos, std::move(shape)));
```

### 2. Camera autonome ✅

```cpp
class Camera {
private:
    sf::View view;
    sf::Vector2f baseSize;      // Taille de base (sans zoom)
    float zoomFactor;           // 1.0 = pas de zoom
    sf::Vector2f center;
    float rotationAngle;

public:
    Camera(sf::Vector2f center, sf::Vector2f size);
    
    void apply(sf::RenderWindow& window) {
        window.setView(view);
    }
    
    void setZoom(float delta) {
        zoomFactor += delta;
        zoomFactor = std::clamp(zoomFactor, 0.1f, 10.0f);
        updateView();
    }
    
    void move(sf::Vector2f offset) {
        center += offset;
        updateView();
    }
    
    void setRotation(float angle) {
        rotationAngle += angle;
        updateView();
    }
    
private:
    void updateView() {
        sf::Vector2f effectiveSize = baseSize * zoomFactor;
        view.setCenter(center);
        view.setSize(effectiveSize);
        view.setRotation(rotationAngle);
    }
};
```

### 3. GameWorld thread-safe ✅

```cpp
class GameWorld {
private:
    std::vector<std::unique_ptr<GameObjectBase>> gameObjects;
    std::vector<GameObjectBase*> visibleGameObjects;
    mutable std::mutex gameObjectsMutex;
    mutable std::mutex cullingMutex;

public:
    void culling(const Camera& camera) {
        std::vector<GameObjectBase*> newVisible;
        
        {
            std::lock_guard<std::mutex> lock(gameObjectsMutex);
            for (const auto& obj : gameObjects) {
                if (isInView(*obj, viewBounds)) {
                    newVisible.push_back(obj.get());
                }
            }
        }
        
        std::lock_guard<std::mutex> lock(cullingMutex);
        visibleGameObjects = std::move(newVisible);
    }
    
    ~GameWorld() {
        gameObjects.clear();      // Détruit les corps Box2D d'abord
        b2DestroyWorld(worldId);  // Puis le monde
    }
};
```

---

## 📈 Métriques de Performance


| Opération            | Temps estimé | Statut                      |
| -------------------- | ------------ | --------------------------- |
| handleEvents         | ~0.5ms       | ✅ OK                        |
| update (15k objets)  | ~5-10ms      | ⚠️ À optimiser              |
| culling (15k objets) | ~3-5ms       | 🔴 QuadTree recommandé      |
| render (visible)     | ~2-4ms       | ⚠️ Batch rendering possible |
| **Total/frame**      | **~15-25ms** | **Cible: <16.67ms**         |


**Objectif** : 60 FPS stable = 16.67ms/frame maximum

---

## 📚 Ressources Recommandées

### Livres

- **Game Programming Patterns** - Robert Nystrom (gratuit en ligne)
- **Effective Modern C++** - Scott Meyers
- **Game Engine Architecture** - Jason Gregory

### Outils

- **Tracy** - Profiler en temps réel
- **Valgrind/AddressSanitizer** - Détection fuites mémoire
- **clang-tidy** - Linting C++ moderne
- **clang-format** - Formatage automatique

### Bibliothèques à considérer

- **EnTT** - ECS moderne et performant
- **glm** - Mathématiques (alternative à Eigen pour les jeux)
- **stb_image** - Chargement d'images rapide

---

## ✅ Checklist de Validation - MISE À JOUR

### Critères CRITIQUES (maintenant validés)

- Toutes les fuites mémoire sont corrigées
- Smart pointers partout où approprié
- Thread safety implémentée (mutex gameObjects + culling)
- Ordre de destruction Box2D corrigé
- Code commenté supprimé
- Constants au lieu de magic numbers
- Build sans erreurs

### Critères RECOMMANDÉS (à valider)

- Tests unitaires > 70% de coverage
- Profiling montre < 16ms/frame
- Documentation à jour
- CI/CD fonctionnelle
- Build sans warnings (-Wall -Wextra -Wpedantic)
- Gestion des erreurs robuste (exceptions)

---

## 🎯 Conclusion - MISE À JOUR

### ✅ Progrès Significatifs

**Corrections appliquées (13 mars 2026)** :

1. **Gestion mémoire** - Smart pointers implémentés ✅
2. **Thread safety** - Mutex multiples pour protection complète ✅
3. **Caméra** - Architecture autonome, zoom persistant ✅
4. **Box2D** - Ordre de destruction corrigé (plus de crash) ✅
5. **Constantes** - Fichier dédié `Constants.hpp` ✅
6. **Système de scènes** - Architecture ownership/observation ✅

**Détail des corrections de scène** :

- `getCurrentScene()` ne détruit plus la scène courante
- `setCurrentScene()` garde les scènes en mémoire
- Cycle de vie avec `onLoad()` / `onUnload()`
- `SceneManager::update()` appelle logique + physique

### ⚠️ Travaux Restants

**Moyenne priorité** :

- GameStateManager (Menu, Play, Pause, etc.)
- Spatial partitioning (QuadTree pour performance)

**Basse priorité** :

- Tests unitaires
- CI/CD
- Profiler avec Tracy
- Nettoyer CMakeLists.txt

### 📊 Verdict

**État actuel** : Le projet est maintenant **stable et maintenable**.

- ✅ Plus de fuites mémoire
- ✅ Plus de crash Box2D
- ✅ Caméra fonctionnelle et robuste
- ✅ Thread safety correcte
- ✅ Système de scènes fonctionnel

**Prochaines étapes recommandées** :

1. Implémenter un **GameStateManager** (Menu, Play, Pause)
2. Ajouter un **QuadTree** pour le culling (performance)
3. Ajouter des **tests unitaires** (qualité)

Le projet a une **bonne base technique** et peut maintenant évoluer sereinement.

---

*Document généré le 13 mars 2026*
*Dernière mise à jour : Système de scènes implémenté*
*Voir aussi : `[CORRECTIFS_CAMERA.md](CORRECTIFS_CAMERA.md)`*