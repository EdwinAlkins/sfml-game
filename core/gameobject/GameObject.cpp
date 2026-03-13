#include "GameObject.hpp"
#include "Constants.hpp"

// ============================================
// GameObjectBase
// ============================================

GameObjectBase::GameObjectBase(std::unique_ptr<sf::Shape> shape)
    : shape(std::move(shape))
{
}

sf::FloatRect GameObjectBase::getBounds() const
{
    return shape->getGlobalBounds();
}

// ============================================
// GameObjectSimpleBody
// ============================================

GameObjectSimpleBody::GameObjectSimpleBody(b2WorldId* worldId, sf::Vector2f position, std::unique_ptr<sf::Shape> shape)
    : GameObjectBase(std::move(shape))
{
    bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2BodyType::b2_dynamicBody;
    bodyDef.position = b2Vec2(position.x, position.y);

    float speed = OBJECT_BASE_SPEED;
    float xVel = (rand() % 100) / 100.0f * speed - speed / 2.0f;
    float yVel = (rand() % 100) / 100.0f * speed - speed / 2.0f;

    this->bodyId = b2CreateBody(*worldId, &bodyDef);
    b2Body_SetLinearVelocity(this->bodyId, b2Vec2(xVel, yVel));

    b2Polygon box = b2MakeBox(
        this->shape->getGlobalBounds().width / 2.0f, 
        this->shape->getGlobalBounds().height / 2.0f
    );
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;
    shapeDef.friction = 0.1f;
    b2CreatePolygonShape(bodyId, &shapeDef, &box);
}

GameObjectSimpleBody::~GameObjectSimpleBody()
{
    b2DestroyBody(this->bodyId);
}

void GameObjectSimpleBody::updateLogic(float /*localDeltaTime*/)
{
    // La physique est gérée par Box2D dans updatePhysics
}

void GameObjectSimpleBody::render(sf::RenderWindow& window)
{
    b2Vec2 position = b2Body_GetPosition(this->bodyId);
    this->shape->setPosition(position.x, position.y);
    window.draw(*this->shape);
}

// ============================================
// GameObjectSimple
// ============================================

GameObjectSimple::GameObjectSimple(sf::Vector2f position, std::unique_ptr<sf::Shape> shape)
    : GameObjectBase(std::move(shape))
    , position(position)
{
    this->shape->setPosition(this->position);
    float speed = OBJECT_BASE_SPEED;
    float xVel = (rand() % 100) / 100.0f * speed - speed / 2.0f;
    float yVel = (rand() % 100) / 100.0f * speed - speed / 2.0f;
    this->velocity = sf::Vector2f(xVel, yVel);
}

void GameObjectSimple::updateLogic(float /*localDeltaTime*/)
{
    // Logique simple sans physique Box2D
}

void GameObjectSimple::render(sf::RenderWindow& window)
{
    window.draw(*this->shape);
}
