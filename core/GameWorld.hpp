
#ifndef GAMEWORLD_HPP
#define GAMEWORLD_HPP

#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>
#include <vector>
#include <atomic>
#include <spdlog/spdlog.h>
#include "GameObject.hpp"


bool isInView(const GameObject& obj, const sf::View& view);

class GameWorld
{
    public:
        GameWorld();
        ~GameWorld();

        void updateLogic(float localDeltaTime);
        void updatePhysics(float localDeltaTime);
        void render(sf::RenderWindow& window);

        void culling(const sf::View* camera);

        void addGameObject(GameObject* gameObject);
        std::vector<GameObject*> getGameObjects();

        b2WorldId* getWorldId();

    private:
        b2WorldDef def;
        b2WorldId worldId;
        std::vector<GameObject*> gameObjects;
        std::vector<GameObject*> visibleGameObjectsA, visibleGameObjectsB;

        std::mutex cullingMutex;
        std::atomic<bool> hasNewData;
};

#endif // GAMEWORLD_HPP
