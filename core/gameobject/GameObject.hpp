#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>
#include <spdlog/spdlog.h>
#include <memory>


class GameObjectBase
{
    public:
        explicit GameObjectBase(std::unique_ptr<sf::Shape> shape);
        virtual ~GameObjectBase() = default;

        virtual void updateLogic(float localDeltaTime) = 0;
        virtual void render(sf::RenderWindow& window) = 0;
        virtual sf::FloatRect getBounds() const;

    protected:
        std::unique_ptr<sf::Shape> shape;
};

class GameObjectSimple : public GameObjectBase
{
    public:
        GameObjectSimple(sf::Vector2f position, std::unique_ptr<sf::Shape> shape);
        ~GameObjectSimple() override = default;

        void updateLogic(float localDeltaTime) override;
        void render(sf::RenderWindow& window) override;

    private:
        sf::Vector2f position;
        sf::Vector2f velocity;
};

class GameObjectSimpleBody : public GameObjectBase
{
    public:
        GameObjectSimpleBody(b2WorldId* worldId, sf::Vector2f position, std::unique_ptr<sf::Shape> shape);
        ~GameObjectSimpleBody() override;

        void updateLogic(float localDeltaTime) override;
        void render(sf::RenderWindow& window) override;

    protected:
        b2BodyId bodyId;
        b2BodyDef bodyDef;
};
#endif // GAMEOBJECT_HPP
