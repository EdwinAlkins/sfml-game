#include "SceneManager.hpp"

// ============================================
// Scene
// ============================================
Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::onLoad()
{
    // Called when the scene is loaded/activated
}

void Scene::onUnload()
{
    // Called when the scene is unloaded/deactivated
}

// ============================================
// SceneManager
// ============================================
SceneManager::SceneManager()
    : currentScene(nullptr)
    , currentSceneName("")
{
}

SceneManager::~SceneManager()
{
    scenes.clear();
    currentScene = nullptr;
}

void SceneManager::addScene(std::string name, std::unique_ptr<Scene> scene)
{
    scenes[name] = std::move(scene);
}

void SceneManager::removeScene(std::string name)
{
    if (scenes.find(name) != scenes.end())
    {
        if (currentScene == scenes[name].get())
        {
            currentScene = nullptr;
            currentSceneName = "";
        }
        scenes.erase(name);
    }
}

void SceneManager::setCurrentScene(std::string name)
{
    if (scenes.find(name) == scenes.end())
    {
        spdlog::error("SceneManager::setCurrentScene: Scene {} not found", name);
        return;
    }
    
    // Unload previous scene if exists
    if (currentScene != nullptr && currentSceneName != name)
    {
        currentScene->onUnload();
    }
    
    // Set new current scene (just a pointer, ownership stays in the map)
    currentScene = scenes[name].get();
    currentSceneName = name;
    
    // Load new scene
    if (currentScene != nullptr)
    {
        currentScene->onLoad();
    }
}

Scene* SceneManager::getCurrentScene()
{
    return currentScene;
}

std::string SceneManager::getCurrentSceneName() const
{
    return currentSceneName;
}

void SceneManager::update(float localDeltaTime)
{
    if (currentScene != nullptr)
    {
        currentScene->updateLogic(localDeltaTime);
        currentScene->updatePhysics(localDeltaTime);
    }
}

void SceneManager::render(sf::RenderWindow& window)
{
    if (currentScene != nullptr)
    {
        currentScene->render(window);
    }
}
