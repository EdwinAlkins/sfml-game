#include "GameObject.hpp"

GameObjectSimpleBody::GameObjectSimpleBody(b2WorldId* worldId, sf::Vector2f position, sf::Shape &shape)
: GameObjectBase(worldId, shape)
{
    // spdlog::info("GameObjectSimpleBody created at position: {} {}", position.x, position.y);
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
    shapeDef.friction = 0.1f;
    b2CreatePolygonShape(bodyId, &shapeDef, &box);
}

GameObjectSimpleBody::~GameObjectSimpleBody()
{
    b2DestroyBody(this->bodyId);
}

void GameObjectSimpleBody::updateLogic(float localDeltaTime)
{

}

void GameObjectSimpleBody::render(sf::RenderWindow& window)
{
    b2Vec2 position = b2Body_GetPosition(this->bodyId);
    this->shape.setPosition(position.x, position.y);
    window.draw(this->shape);
}




GameObjectSimple::GameObjectSimple(b2WorldId* worldId, sf::Vector2f position, sf::Shape &shape)
: GameObjectBase(worldId, shape)
{
    this->position = position;
    this->shape.setPosition(this->position);
    float speed = 100.0f;
    float xVel = (rand() % 100) / 100.0f * speed - speed / 2.0f;
    float yVel = (rand() % 100) / 100.0f * speed - speed / 2.0f;
    this->velocity = sf::Vector2f(xVel, yVel);
}

GameObjectSimple::~GameObjectSimple()
{

}

void GameObjectSimple::updateLogic(float localDeltaTime)
{
    // this->shape.move(this->velocity * localDeltaTime);
    // this->position = this->shape.getPosition();
}

void GameObjectSimple::render(sf::RenderWindow& window)
{
    window.draw(this->shape);
}
