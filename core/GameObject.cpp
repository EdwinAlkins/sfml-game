
#include "GameObject.hpp"

GameObject::GameObject(b2WorldId* worldId, sf::Vector2f position)
{
    spdlog::info("GameObject created at position: {} {}", position.x, position.y);
    this->worldId = worldId;
    bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2BodyType::b2_dynamicBody;
    bodyDef.position = b2Vec2(position.x, position.y);
    this->bodyId = b2CreateBody(*worldId, &bodyDef);
    b2Body_SetLinearVelocity(this->bodyId, b2Vec2(100, 0));
    b2Body_SetAngularVelocity(this->bodyId, 100);

    float width = 100;
    float height = 100;
        
    shape.setSize(sf::Vector2f(width, height)); // Échelle 10x pour affichage
    shape.setOrigin(width / 2, height / 2);
    shape.setFillColor(sf::Color::Green);

}

GameObject::~GameObject()
{
    b2DestroyBody(this->bodyId);
}

void GameObject::updateLogic(float localDeltaTime)
{

}

void GameObject::render(sf::RenderWindow& window)
{
    b2Vec2 position = b2Body_GetPosition(this->bodyId);
    this->shape.setPosition(position.x, position.y);
    window.draw(this->shape);
}
