#ifndef SCENEMANAGER_HPP
#define SCENEMANAGER_HPP

#include <vector>
#include <memory>
#include <string>
#include <map>
#include "GameObject.hpp"
#include "GameWorld.hpp"

// ============================================
// Scene
// ============================================
class Scene : public GameWorld {
    public:
        Scene();
        ~Scene();

        void onLoad();
        void onUnload();
};

// ============================================
// SceneManager
// ============================================
class SceneManager {
    private:
        std::map<std::string, std::unique_ptr<Scene>> scenes;
        Scene* currentScene;  // Pointeur non-possesseur pour éviter les problèmes de ownership
        std::string currentSceneName;
    public:
        SceneManager();
        ~SceneManager();

        void addScene(std::string name, std::unique_ptr<Scene> scene);
        void removeScene(std::string name);
        void setCurrentScene(std::string name);
        Scene* getCurrentScene();  // Retourne un pointeur, pas un unique_ptr
        std::string getCurrentSceneName() const;

        void update(float localDeltaTime);
        void render(sf::RenderWindow& window);
};

#endif // SCENEMANAGER_HPP