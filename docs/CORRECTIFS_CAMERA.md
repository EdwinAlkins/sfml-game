# Correctifs Appliqués - Caméra et Thread Safety

## 📅 Date : 13 mars 2026

---

## 🎯 Problèmes Corrigés

### 1. **Crash Box2D à la destruction** ❌ → ✅

**Problème** : Le monde Box2D était détruit avant les GameObjects, causant un crash car les corps Box2D référençaient un monde inexistant.

```cpp
// AVANT (GameWorld.cpp)
GameWorld::~GameWorld()
{
    b2DestroyWorld(worldId);  // ← Crash ! Les corps existent encore
    gameObjects.clear();
}

// APRÈS
GameWorld::~GameWorld()
{
    // 1. Détruire les GameObjects (et leurs corps Box2D)
    gameObjects.clear();
    
    // 2. Puis détruire le monde Box2D
    b2DestroyWorld(worldId);
}
```

---

### 2. **Zoom de la caméra reset lors des déplacements/rotations** ❌ → ✅

**Problème** : La caméra utilisait `view.zoom()` de SFML (cumulatif) et `updateView()` qui resetait tout avec `setSize()`.

**Solution** : Réécriture complète du système de zoom avec un facteur multiplicatif.

```cpp
// AVANT (Camera.cpp)
void Camera::setZoom(float zoom)
{
    this->zoom += zoom;
    view.zoom(1.0f / (1.0f + zoom));  // ← Cumulatif + incompatible avec updateView()
}

void Camera::updateView()
{
    view.setCenter(center);
    view.setSize(size);  // ← Reset le zoom !
    view.setRotation(rotationAngle);
}

// APRÈS
class Camera {
    sf::Vector2f baseSize;    // Taille de base (sans zoom)
    float zoomFactor;         // Facteur de zoom (1.0 = pas de zoom)
};

void Camera::updateView()
{
    sf::Vector2f effectiveSize = baseSize * zoomFactor;  // ← Zoom intégré
    view.setCenter(center);
    view.setSize(effectiveSize);
    view.setRotation(rotationAngle);
}

void Camera::setZoom(float zoom)
{
    zoomFactor += zoom;
    zoomFactor = std::clamp(zoomFactor, 0.1f, 10.0f);  // ← Clamp
    updateView();  // ← UpdateView n'affecte plus le zoom
}
```

---

### 3. **Caméra non appliquée au rendu** ❌ → ✅

**Problème** : Le view de la caméra n'était jamais appliqué à la fenêtre.

```cpp
// AVANT (GameManager.cpp)
void GameManager::render()
{
    window.clear(sf::Color::Black);
    this->gameWorld->render(window);  // ← Utilise le view par défaut
    window.display();
}

// APRÈS
void GameManager::render()
{
    window.clear(sf::Color::Black);
    camera->apply(window);  // ← Applique le view de la caméra
    this->gameWorld->render(window);
    window.display();
}
```

---

### 4. **Initialisation caméra avec taille incorrecte** ❌ → ✅

**Problème** : La caméra était initialisée avec des constantes (1920x1080) au lieu de la taille réelle de la fenêtre.

```cpp
// AVANT
camera = std::make_unique<Camera>();
camera->init();  // ← Utilise SCREEN_WIDTH x SCREEN_HEIGHT

// APRÈS
camera = std::make_unique<Camera>(
    sf::Vector2f(window.getSize().x / 2.0f, window.getSize().y / 2.0f),
    sf::Vector2f(window.getSize().x, window.getSize().y)
);
```

---

### 5. **Thread Safety insuffisante** ❌ → ✅

**Problème** : `gameObjects` était lu par `culling()` sans protection mutex.

```cpp
// AVANT (GameWorld.hpp)
class GameWorld {
    std::vector<std::unique_ptr<GameObjectBase>> gameObjects;
    mutable std::mutex cullingMutex;  // ← Un seul mutex
};

// APRÈS
class GameWorld {
    std::vector<std::unique_ptr<GameObjectBase>> gameObjects;
    mutable std::mutex gameObjectsMutex;  // ← Protège gameObjects
    mutable std::mutex cullingMutex;      // ← Protège visibleGameObjects
};

// Toutes les méthodes accédant à gameObjects sont maintenant protégées
void GameWorld::culling(const Camera& camera)
{
    {
        std::lock_guard<std::mutex> lock(gameObjectsMutex);
        // Lecture protégée de gameObjects
    }
    std::lock_guard<std::mutex> lock(cullingMutex);
    // Écriture protégée de visibleGameObjects
}
```

---

### 6. **Gestion des threads incorrecte** ❌ → ✅

**Problème** : Utilisation de `&&` au lieu de `||` pour vérifier si les threads sont joinable.

```cpp
// AVANT
if (instanceGameThread.joinable() && instanceCullingThread.joinable())
{
    instanceGameThread.join();
    instanceCullingThread.join();
}

// APRÈS
if (instanceGameThread.joinable())
{
    instanceGameThread.join();
}
if (instanceCullingThread.joinable())
{
    instanceCullingThread.join();
}
```

---

### 7. **Perte de l'état de la caméra en fullscreen** ❌ → ✅

**Problème** : Le switch fullscreen recréait la caméra, perdant zoom et rotation.

```cpp
// AVANT
if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F)
{
    fullscreenMode = !fullscreenMode;
    window.close();
    init();  // ← Perte de l'état de la caméra
}

// APRÈS
if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F)
{
    fullscreenMode = !fullscreenMode;
    
    // Sauvegarder l'état de la caméra
    sf::Vector2f camCenter = camera->getCenter();
    float camZoom = camera->getZoom();
    float camRotation = camera->getRotation();
    
    window.close();
    init();
    
    // Restaurer l'état
    camera->setCenter(camCenter);
    camera->setRotation(camRotation);
    camera->setZoom(camZoom - 1.0f);
}
```

---

## 📁 Fichiers Modifiés

| Fichier | Modifications |
|---------|---------------|
| `core/Camera.hpp` | Refonte complète (zoomFactor, baseSize) |
| `core/Camera.cpp` | Réécriture de updateView(), setZoom(), etc. |
| `core/GameWorld.hpp` | Ajout gameObjectsMutex |
| `core/GameWorld.cpp` | Thread safety + ordre de destruction |
| `core/manager/GameManager.hpp` | Nettoyage déclarations |
| `core/manager/GameManager.cpp` | Caméra appliquée, threads, fullscreen |
| `core/Constants.hpp` | **Nouveau** - Toutes les constantes |

---

## ✅ Tests à Effectuer

### Caméra
- [ ] Zoom avec la molette (avant/arrière)
- [ ] Déplacement avec flèches directionnelles
- [ ] Rotation avec A/E
- [ ] Reset avec R
- [ ] Switch fullscreen avec F (préserve zoom/rotation)
- [ ] Combinaison zoom + rotation + déplacement

### Threads
- [ ] Toggle threads avec T (plusieurs fois)
- [ ] Pause avec P
- [ ] Quitter pendant que les threads tournent
- [ ] Perte/regain de focus

### Physics
- [ ] Les objets physiques tombent/interagissent
- [ ] Pas de crash à la fermeture
- [ ] Pas de fuite mémoire (valgrind)

---

## 🔧 Nouvelles Constantes (Constants.hpp)

```cpp
// Screen
constexpr int SCREEN_WIDTH = 1920;
constexpr int SCREEN_HEIGHT = 1080;

// Game
constexpr int INITIAL_OBJECTS_COUNT = 10000;
constexpr int INITIAL_PHYSICS_OBJECTS_COUNT = 5000;
constexpr float OBJECT_BASE_SPEED = 100.0f;

// Physics
constexpr float FIXED_TIME_STEP = 1.0f / 60.0f;
constexpr float MAX_FRAME_TIME = 0.25f;

// Camera
constexpr float CAMERA_ZOOM_STEP = 0.05f;
constexpr float CAMERA_MOVE_STEP = 10.0f;
constexpr float CAMERA_ROTATION_STEP = 10.0f;

// Threading
constexpr int CULLING_THREAD_SLEEP_MS = 16;
constexpr int MAIN_THREAD_SLEEP_MS = 1;
```

---

## 🎮 Contrôles

| Touche | Action |
|--------|--------|
| ↑ ↓ ← → | Déplacer la caméra |
| A | Rotation horaire |
| E | Rotation anti-horaire |
| Molette | Zoom in/out |
| R | Reset caméra (zoom + rotation) |
| F | Toggle fullscreen |
| P | Pause |
| T | Toggle threads |
| D | Toggle debug logs |
| Échap | Quitter |

---

## 📊 Architecture Caméra (Nouvelle)

```
┌─────────────────────────────────────┐
│              Camera                 │
├─────────────────────────────────────┤
│ - baseSize: Vector2f     (1920x1080)│
│ - zoomFactor: float      (1.0 = 100%)│
│ - center: Vector2f       (position) │
│ - rotationAngle: float   (degrés)   │
├─────────────────────────────────────┤
│ - view: sf::View                    │
│   └─ Taille effective = baseSize × zoomFactor │
└─────────────────────────────────────┘
```

**Avantages** :
- Zoom persistant à travers les updates
- Pas d'effet cumulatif incontrôlé
- Clamp pour éviter les valeurs invalides (0.1x à 10x)
- Compatible avec rotation et déplacement

---

## ⚠️ Points de Vigilance

1. **Zoom cumulatif** : Chaque appel à `setZoom()` ajoute au facteur. Pour un zoom absolu, utiliser `resetZoom()` puis ajuster.

2. **Thread culling** : Le culling tourne à ~60 FPS (16ms de sleep). Si le jeu ralentit, augmenter `CULLING_THREAD_SLEEP_MS`.

3. **Fullscreen** : La caméra est recentrée sur la nouvelle fenêtre. Le centre est conservé mais peut être hors écran si les résolutions diffèrent.

---

*Document généré le 13 mars 2026*
*Correctifs validés par compilation*
