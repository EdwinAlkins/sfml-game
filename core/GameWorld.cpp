#include "GameWorld.hpp"

bool isInView(const GameObjectBase& obj, const sf::FloatRect& viewBounds)
{
    /*
    sf::Vector2f viewCenter = view.getCenter();
    sf::Vector2f viewSize = view.getSize();

    float viewRotation = view.getRotation() * (3.14159265f / 180.0f); // Convert to radians
    float cosRotation = std::cos(viewRotation);
    float sinRotation = std::sin(viewRotation);

    // Calculate the four corners of the view
    sf::Vector2f halfSize = viewSize / 2.0f;
    
    // Unrotated corners relative to center
    sf::Vector2f topLeft(-halfSize.x, -halfSize.y);
    sf::Vector2f topRight(halfSize.x, -halfSize.y);
    sf::Vector2f bottomLeft(-halfSize.x, halfSize.y);
    sf::Vector2f bottomRight(halfSize.x, halfSize.y);
    
    // Apply rotation to corners
    sf::Vector2f rotatedTopLeft(
        topLeft.x * cosRotation - topLeft.y * sinRotation,
        topLeft.x * sinRotation + topLeft.y * cosRotation
    );
    sf::Vector2f rotatedTopRight(
        topRight.x * cosRotation - topRight.y * sinRotation,
        topRight.x * sinRotation + topRight.y * cosRotation
    );
    sf::Vector2f rotatedBottomLeft(
        bottomLeft.x * cosRotation - bottomLeft.y * sinRotation,
        bottomLeft.x * sinRotation + bottomLeft.y * cosRotation
    );
    sf::Vector2f rotatedBottomRight(
        bottomRight.x * cosRotation - bottomRight.y * sinRotation,
        bottomRight.x * sinRotation + bottomRight.y * cosRotation
    );
    
    // Translate corners to world space
    rotatedTopLeft += viewCenter;
    rotatedTopRight += viewCenter;
    rotatedBottomLeft += viewCenter;
    rotatedBottomRight += viewCenter;
    
    // Find the min and max coordinates to create an AABB that contains the rotated view
    float minX = std::min({rotatedTopLeft.x, rotatedTopRight.x, rotatedBottomLeft.x, rotatedBottomRight.x});
    float maxX = std::max({rotatedTopLeft.x, rotatedTopRight.x, rotatedBottomLeft.x, rotatedBottomRight.x});
    float minY = std::min({rotatedTopLeft.y, rotatedTopRight.y, rotatedBottomLeft.y, rotatedBottomRight.y});
    float maxY = std::max({rotatedTopLeft.y, rotatedTopRight.y, rotatedBottomLeft.y, rotatedBottomRight.y});
    */
    
    /*
    sf::Vector2f viewCenter = view.getCenter();
    sf::Vector2f viewSize = view.getSize();

    float viewRotation = view.getRotation() * (3.14159265f / 180.0f); // Convert to radians
    
    // Create rotation matrix using Eigen
    Eigen::Matrix2f rotationMatrix;
    rotationMatrix << std::cos(viewRotation), -std::sin(viewRotation),
                      std::sin(viewRotation),  std::cos(viewRotation);
    
    // Calculate the four corners of the view
    sf::Vector2f halfSize = viewSize / 2.0f;
    
    // Create corner vectors
    Eigen::Vector2f corners[4] = {
        Eigen::Vector2f(-halfSize.x, -halfSize.y), // topLeft
        Eigen::Vector2f(halfSize.x, -halfSize.y),  // topRight
        Eigen::Vector2f(-halfSize.x, halfSize.y),  // bottomLeft
        Eigen::Vector2f(halfSize.x, halfSize.y)    // bottomRight
    };
    
    // Apply rotation to all corners at once using Eigen
    Eigen::Vector2f rotatedCorners[4];
    for (int i = 0; i < 4; i++) {
        rotatedCorners[i] = rotationMatrix * corners[i];
    }
    
    // Translate corners to world space and find min/max
    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();
    
    for (int i = 0; i < 4; i++) {
        float worldX = rotatedCorners[i].x() + viewCenter.x;
        float worldY = rotatedCorners[i].y() + viewCenter.y;
        
        minX = std::min(minX, worldX);
        maxX = std::max(maxX, worldX);
        minY = std::min(minY, worldY);
        maxY = std::max(maxY, worldY);
    }
    */
    

    // Create an axis-aligned bounding box that contains the rotated view
    // sf::FloatRect viewBounds(minX, minY, maxX - minX, maxY - minY);
    
    sf::FloatRect objBounds = obj.getBounds();
    return viewBounds.intersects(objBounds);
}

GameWorld::GameWorld()
{
    def = b2DefaultWorldDef();
    b2Vec2 gravity = {0.0f, 0.0f};
    def.gravity = gravity;
    def.enableSleep = false;
    worldId = b2CreateWorld(&def);  
}

GameWorld::~GameWorld()
{
    b2DestroyWorld(worldId);
    spdlog::info("GameWorld destructor");

    for (GameObjectBase* gameObject : this->gameObjects)
    {
        delete gameObject;
    }
    spdlog::info("GameWorld destructor: Deleted {} game objects", this->gameObjects.size());
    this->gameObjects.clear();
    spdlog::info("GameWorld destructor: Cleared game objects");

    for (GameObjectBase* gameObject : this->visibleGameObjectsA)
    {
        delete gameObject;
    }
    spdlog::info("GameWorld destructor: Deleted {} visible game objects", this->visibleGameObjectsA.size());
    this->visibleGameObjectsA.clear();

    for (GameObjectBase* gameObject : this->visibleGameObjectsB)
    {
        delete gameObject;
    }
    spdlog::info("GameWorld destructor: Deleted {} visible game objects", this->visibleGameObjectsB.size());
    this->visibleGameObjectsB.clear();
}

void GameWorld::updateLogic(float localDeltaTime)
{
    for (GameObjectBase* gameObject : this->gameObjects)
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
    for (GameObjectBase* gameObject : this->visibleGameObjectsB)
    {
        gameObject->render(window);
    }
    // for (GameObjectBase* gameObject : this->gameObjects)
    // {
    //     gameObject->render(window);
    // }
}

void GameWorld::addGameObject(GameObjectBase* gameObject)
{
    this->gameObjects.push_back(gameObject);
}

std::vector<GameObjectBase*> GameWorld::getGameObjects()
{
    return gameObjects;
}

b2WorldId* GameWorld::getWorldId()
{
    return &worldId;
}

void GameWorld::culling(Camera& camera)
{
    sf::FloatRect viewBounds = camera.getViewBounds();
    std::vector<GameObjectBase*> newVisibleObject;
    for (const auto& obj : gameObjects)
    {
        if(isInView(*obj,viewBounds))
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