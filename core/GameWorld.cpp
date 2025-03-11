
#include "GameWorld.hpp"

GameWorld::GameWorld()
{
    def = b2DefaultWorldDef();
    worldId = b2CreateWorld(&def);
}

GameWorld::~GameWorld()
{
    b2DestroyWorld(worldId);
}

void GameWorld::updateLogic(float localDeltaTime)
{
    for (GameObject* gameObject : this->gameObjects)
    {
        gameObject->updateLogic(localDeltaTime);
    }
}

void GameWorld::updatePhysics(float localDeltaTime)
{
    b2World_Step(worldId, localDeltaTime, 1);
}

void GameWorld::render(sf::RenderWindow& window)
{
    for (GameObject* gameObject : this->gameObjects)
    {
        gameObject->render(window);
    }
}

void GameWorld::addGameObject(GameObject* gameObject)
{
    this->gameObjects.push_back(gameObject);
}

b2WorldId* GameWorld::getWorldId()
{
    return &worldId;
}
