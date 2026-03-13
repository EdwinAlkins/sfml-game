#include "GameWorld.hpp"

bool isInView(const GameObjectBase& obj, const sf::FloatRect& viewBounds)
{
    sf::FloatRect objBounds = obj.getBounds();
    return viewBounds.intersects(objBounds);
}

GameWorld::GameWorld()
{
    def = b2DefaultWorldDef();
    b2Vec2 gravity = {GRAVITY_X, GRAVITY_Y};
    def.gravity = gravity;
    def.enableSleep = true;
    worldId = b2CreateWorld(&def);
}

GameWorld::~GameWorld()
{
    spdlog::info("GameWorld destructor");

    // Détruire les GameObjects EN PREMIER (ils contiennent des corps Box2D)
    {
        std::lock_guard<std::mutex> lock(gameObjectsMutex);
        gameObjects.clear();
    }
    {
        std::lock_guard<std::mutex> lock(cullingMutex);
        visibleGameObjects.clear();
    }

    spdlog::info("GameWorld destructor: Cleared all game objects");

    // Puis détruire le monde Box2D
    b2DestroyWorld(worldId);

    spdlog::info("GameWorld destructor: World destroyed");
}

void GameWorld::updateLogic(float localDeltaTime)
{
    std::lock_guard<std::mutex> lock(gameObjectsMutex);
    for (const auto& gameObject : gameObjects)
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
    std::lock_guard<std::mutex> lock(cullingMutex);
    for (GameObjectBase* gameObject : visibleGameObjects)
    {
        gameObject->render(window);
    }
}

void GameWorld::addGameObject(std::unique_ptr<GameObjectBase> gameObject)
{
    std::lock_guard<std::mutex> lock(gameObjectsMutex);
    gameObjects.push_back(std::move(gameObject));
}

void GameWorld::removeGameObject(std::unique_ptr<GameObjectBase> gameObject)
{
    std::lock_guard<std::mutex> lock(gameObjectsMutex);
    gameObjects.erase(std::remove_if(gameObjects.begin(), gameObjects.end(), [&gameObject](const auto& obj) {
        return obj.get() == gameObject.get();
    }), gameObjects.end());
}

const std::vector<std::unique_ptr<GameObjectBase>>& GameWorld::getGameObjects() const
{
    return gameObjects;
}

b2WorldId* GameWorld::getWorldId()
{
    return &worldId;
}

void GameWorld::culling(const Camera& camera)
{
    sf::FloatRect viewBounds = camera.getViewBounds();
    std::vector<GameObjectBase*> newVisibleObjects;
    
    {
        std::lock_guard<std::mutex> lock(gameObjectsMutex);
        newVisibleObjects.reserve(gameObjects.size());

        for (const auto& obj : gameObjects)
        {
            if (isInView(*obj, viewBounds))
            {
                newVisibleObjects.push_back(obj.get());
            }
        }
    }

    std::lock_guard<std::mutex> lock(cullingMutex);
    visibleGameObjects = std::move(newVisibleObjects);
}
