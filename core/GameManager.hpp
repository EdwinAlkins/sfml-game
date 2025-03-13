#ifndef GAMEMANAGER_HPP
#define GAMEMANAGER_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <spdlog/spdlog.h>
#include <thread>
#include <atomic>
#include <chrono>

#include "SFMLFormatters.hpp"
#include "GameObject.hpp"
#include "GameWorld.hpp"
#include "Camera.hpp"

class GameManager
{
    public:
        GameManager();
        ~GameManager();

        void run();
        void update(float localDeltaTime);
        void threadUpdate();
        void cullingThread();
        void render();
        void handleEvents();
        void handleInput(sf::Event event);
        void handlePhysics(float localDeltaTime);
        void handleLogic(float localDeltaTime);
        // void handleNetwork(float localDeltaTime);
        // void handleDebug(float localDeltaTime);
        void handleCleanup();

        void init();
        void start();
        void cleanup();

        sf::RenderWindow& getWindow();
        sf::Clock& getClock();
        sf::Time& getDeltaTime();

    private:
        sf::RenderWindow window;
        sf::Clock clock;
        sf::Time deltaTime;

        std::atomic<bool> running;
        std::atomic<bool> pacreateWindowused;
        std::atomic<bool> minimized;
        std::atomic<bool> fullscreenMode;
        // std::atomic<bool> vsync;
        std::atomic<bool> debug;
        std::atomic<bool> paused;
        std::atomic<bool> pausedByFocus;
        std::mutex pausedByFocusMutex;
        

        std::thread instanceGameThread;
        std::thread instanceCullingThread;
        std::atomic<bool> useThread;

        GameWorld* gameWorld;
        Camera* camera;

};

#endif // GAMEMANAGER_HPP
