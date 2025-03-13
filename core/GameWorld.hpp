
#ifndef GAMEWORLD_HPP
#define GAMEWORLD_HPP

#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>
#include <vector>
#include <atomic>
#include <spdlog/spdlog.h>
#include "GameObject.hpp"
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

        void culling(Camera& camera);

        void addGameObject(GameObjectBase* gameObject);
        std::vector<GameObjectBase*> getGameObjects();

        b2WorldId* getWorldId();

    private:
        b2WorldDef def;
        b2WorldId worldId;
        std::vector<GameObjectBase*> gameObjects;
        std::vector<GameObjectBase*> visibleGameObjectsA, visibleGameObjectsB;

        std::mutex cullingMutex;
        std::atomic<bool> hasNewData;
};

#endif // GAMEWORLD_HPP
