#include "SceneManager.hpp"

std::unique_ptr<Scene> createMainScene()
{
    auto scene = std::make_unique<Scene>();

    int nbObjectsSimple = INITIAL_OBJECTS_COUNT;
    sf::Color color = sf::Color(128, 128, 128);

    for (int i = 0; i < nbObjectsSimple; i++)
    {
        auto shape = std::make_unique<sf::RectangleShape>(sf::Vector2f(rand() % 10, rand() % 10));
        shape->setFillColor(color);
        scene->addGameObject(std::make_unique<GameObjectSimple>(
            sf::Vector2f(rand() % (SCREEN_WIDTH * 2) - SCREEN_WIDTH/2,
                        rand() % (SCREEN_HEIGHT * 2) - SCREEN_HEIGHT/2),
            std::move(shape)
        ));
    }

    int nbObjectsSimpleBody = INITIAL_PHYSICS_OBJECTS_COUNT;
    for (int i = 0; i < nbObjectsSimpleBody; i++)
    {
        auto shape = std::make_unique<sf::RectangleShape>(sf::Vector2f(5.0f, 5.0f));
        shape->setFillColor(sf::Color::White);
        scene->addGameObject(std::make_unique<GameObjectSimpleBody>(
            scene->getWorldId(),
            sf::Vector2f(rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT),
            std::move(shape)
        ));
    }
    return scene;
}