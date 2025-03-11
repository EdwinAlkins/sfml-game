
#ifndef GAMEWORLD_HPP
#define GAMEWORLD_HPP

#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>
#include "GameObject.hpp"


class GameWorld
{
    public:
        GameWorld();
        ~GameWorld();

        void updateLogic(float localDeltaTime);
        void updatePhysics(float localDeltaTime);
        void render(sf::RenderWindow& window);

        void addGameObject(GameObject* gameObject);

        b2WorldId* getWorldId();

    private:
        b2WorldDef def;
        b2WorldId worldId;
        std::vector<GameObject*> gameObjects;
};

#endif // GAMEWORLD_HPP
