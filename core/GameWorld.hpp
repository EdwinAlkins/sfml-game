#ifndef GAMEWORLD_HPP
#define GAMEWORLD_HPP

#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>
#include <vector>
#include <atomic>
#include <mutex>
#include <memory>
#include <spdlog/spdlog.h>
#include "gameobject/GameObject.hpp"
#include "Camera.hpp"


bool isInView(const GameObjectBase& obj, const sf::FloatRect& viewBounds);

class GameWorld
{
    public:
        GameWorld();
        ~GameWorld();

        void updateLogic(float localDeltaTime);
        void updatePhysics(float localDeltaTime);
        void render(sf::RenderWindow& window);

        void culling(const Camera& camera);

        void addGameObject(std::unique_ptr<GameObjectBase> gameObject);
        const std::vector<std::unique_ptr<GameObjectBase>>& getGameObjects() const;

        b2WorldId* getWorldId();

    private:
        b2WorldDef def;
        b2WorldId worldId;
        std::vector<std::unique_ptr<GameObjectBase>> gameObjects;
        std::vector<GameObjectBase*> visibleGameObjects;

        mutable std::mutex gameObjectsMutex;  // Protège gameObjects
        mutable std::mutex cullingMutex;      // Protège visibleGameObjects
};

#endif // GAMEWORLD_HPP
