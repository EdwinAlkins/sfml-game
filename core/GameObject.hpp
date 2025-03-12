#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>
#include <spdlog/spdlog.h>


class GameObject
{
    public:
        GameObject(b2WorldId* worldId, sf::Vector2f position, sf::Shape &shape);
        ~GameObject();

        void updateLogic(float localDeltaTime);
        // void updatePhysics(float localDeltaTime);
        void render(sf::RenderWindow& window);

        sf::FloatRect getBounds() const;

    private:
        b2BodyId bodyId;
        b2BodyDef bodyDef;
        b2WorldId* worldId;
        sf::Shape& shape;
};
#endif // GAMEOBJECT_HPP
