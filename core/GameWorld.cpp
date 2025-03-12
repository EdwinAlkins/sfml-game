#include "GameWorld.hpp"

bool isInView(const GameObject& obj, const sf::View& view)
{
    sf::FloatRect viewBounds(
        view.getCenter().x - view.getSize().x / 2, 
        view.getCenter().y - view.getSize().y / 2, 
        view.getSize().x, 
        view.getSize().y
    );
    return viewBounds.intersects(obj.getBounds());
}

GameWorld::GameWorld()
{
    def = b2DefaultWorldDef();
    b2Vec2 gravity = {0.0f, 0.0f};
    def.gravity = gravity;
    worldId = b2CreateWorld(&def);  
}

GameWorld::~GameWorld()
{
    b2DestroyWorld(worldId);
    spdlog::info("GameWorld destructor");

    for (GameObject* gameObject : this->gameObjects)
    {
        delete gameObject;
    }
    spdlog::info("GameWorld destructor: Deleted {} game objects", this->gameObjects.size());
    this->gameObjects.clear();
    spdlog::info("GameWorld destructor: Cleared game objects");

    for (GameObject* gameObject : this->visibleGameObjectsA)
    {
        delete gameObject;
    }
    spdlog::info("GameWorld destructor: Deleted {} visible game objects", this->visibleGameObjectsA.size());
    this->visibleGameObjectsA.clear();

    for (GameObject* gameObject : this->visibleGameObjectsB)
    {
        delete gameObject;
    }
    spdlog::info("GameWorld destructor: Deleted {} visible game objects", this->visibleGameObjectsB.size());
    this->visibleGameObjectsB.clear();
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
    if(hasNewData.load(std::memory_order_acquire))
    {
        std::lock_guard<std::mutex> lock(cullingMutex);
        std::swap(visibleGameObjectsB, visibleGameObjectsA);
        hasNewData.store(false, std::memory_order_release);

    }
    spdlog::debug("GameWorld render: Rendering {} objects", visibleGameObjectsB.size());
    for (GameObject* gameObject : this->visibleGameObjectsB)
    {
        gameObject->render(window);
    }
}

void GameWorld::addGameObject(GameObject* gameObject)
{
    this->gameObjects.push_back(gameObject);
}

std::vector<GameObject*> GameWorld::getGameObjects()
{
    return gameObjects;
}

b2WorldId* GameWorld::getWorldId()
{
    return &worldId;
}

void GameWorld::culling(const sf::View* camera)
{
    std::vector<GameObject*> newVisibleObject;
    for (const auto& obj : gameObjects)
    {
        if(isInView(*obj,*camera))
        {
            newVisibleObject.push_back(obj);
        }
    }
    {
        std::lock_guard<std::mutex> lock(cullingMutex);
        std::swap(visibleGameObjectsA, newVisibleObject);
        hasNewData.store(true, std::memory_order_release);
    }   
}