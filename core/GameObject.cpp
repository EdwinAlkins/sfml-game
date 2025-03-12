#include "GameObject.hpp"

GameObject::GameObject(b2WorldId* worldId, sf::Vector2f position, sf::Shape &shape)
: shape(shape)
{
    spdlog::info("GameObject created at position: {} {}", position.x, position.y);
    this->worldId = worldId;
    bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2BodyType::b2_dynamicBody;
    bodyDef.position = b2Vec2(position.x, position.y);

    float speed = 100.0f;
    float xVel = (rand() % 100) / 100.0f * speed - speed / 2.0f;
    float yVel = (rand() % 100) / 100.0f * speed - speed / 2.0f;

    this->bodyId = b2CreateBody(*worldId, &bodyDef);
    b2Body_SetLinearVelocity(this->bodyId, b2Vec2(xVel, yVel));
    // b2Body_SetAngularVelocity(this->bodyId, 0);

    b2Polygon box = b2MakeBox(shape.getGlobalBounds().width / 2, shape.getGlobalBounds().height / 2);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;
    shapeDef.friction = 0.3f;
    b2ShapeId shapeId = b2CreatePolygonShape(bodyId, &shapeDef, &box);
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

sf::FloatRect GameObject::getBounds() const
{
    return this->shape.getGlobalBounds();
}