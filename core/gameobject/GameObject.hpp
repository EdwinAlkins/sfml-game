#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>
#include <spdlog/spdlog.h>


class GameObjectBase
{
    public:
        GameObjectBase(b2WorldId* worldId, sf::Shape &shape) : worldId(worldId), shape(shape) {}
        virtual ~GameObjectBase() {}

        virtual void updateLogic(float localDeltaTime) = 0;
        virtual void render(sf::RenderWindow& window) = 0;
        virtual sf::FloatRect getBounds() const { return shape.getGlobalBounds(); }

    protected:
        b2WorldId* worldId;
        sf::Shape& shape;
};

class GameObjectSimple : public GameObjectBase
{
    public:
        GameObjectSimple(b2WorldId* worldId, sf::Vector2f position, sf::Shape &shape);
        ~GameObjectSimple();

        void updateLogic(float localDeltaTime);
        void render(sf::RenderWindow& window);
    
    private:
        sf::Vector2f position;
        sf::Vector2f velocity;
};

class GameObjectSimpleBody : public GameObjectBase
{
    public:
        GameObjectSimpleBody(b2WorldId* worldId, sf::Vector2f position, sf::Shape &shape);
        ~GameObjectSimpleBody();

        void updateLogic(float localDeltaTime);
        // void updatePhysics(float localDeltaTime);
        void render(sf::RenderWindow& window);

    protected:
        b2BodyId bodyId;
        b2BodyDef bodyDef;
        b2WorldId* worldId;
};
#endif // GAMEOBJECT_HPP
