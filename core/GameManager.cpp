#include "GameManager.hpp"

GameManager::GameManager()
{
    spdlog::info("GameManager constructor");
    running = true;
    paused = false;
    fullscreenMode = false;
    useThread = true;
    debug = true;
    pausedByFocus = false;
    this->gameWorld = new GameWorld();

    int height = 1080;
    int width = 1920;
    int nbObjects = 20000;
    for (int i = 0; i < nbObjects; i++)
    {
        sf::Shape* newShape = new sf::RectangleShape(sf::Vector2f(5.0f, 5.0f));
        this->gameWorld->addGameObject(new GameObject(this->gameWorld->getWorldId(), sf::Vector2f(rand() % width, rand() % height), *newShape));
    }
}

GameManager::~GameManager()
{
    spdlog::info("GameManager destructor");
    cleanup();
}

void GameManager::run()
{
    spdlog::info("GameManager run");
    init();
    while (running)
    {
        auto startRun = std::chrono::high_resolution_clock::now();
        deltaTime = clock.restart();
        auto start = std::chrono::high_resolution_clock::now();
        handleEvents();
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        spdlog::debug("GameManager run: handleEvents time {} ms", duration.count() / 1000.0f);
        if (!useThread && !paused && !pausedByFocus)
        {
            auto start = std::chrono::high_resolution_clock::now();
            update(deltaTime.asSeconds());
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            spdlog::debug("GameManager run: update time {} ms", duration.count() / 1000.0f);
            start = std::chrono::high_resolution_clock::now();
            gameWorld->culling(&window.getView());
            end = std::chrono::high_resolution_clock::now();
            duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            spdlog::debug("GameManager run: culling time {} ms", duration.count() / 1000.0f);
        }
        start = std::chrono::high_resolution_clock::now();
        render();
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        spdlog::debug("GameManager run: render time {} ms", duration.count() / 1000.0f);
        // display the frame rate
        spdlog::debug("GameManager run: frame rate {}", 1.0f / deltaTime.asSeconds());
        auto endRun = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(endRun - startRun);
        spdlog::debug("GameManager run: total time {} ms", duration.count() / 1000.0f);
    }
    cleanup();
}


void GameManager::update(float localDeltaTime)
{
    handleLogic(localDeltaTime);
    handlePhysics(localDeltaTime);
    // handleNetwork(localDeltaTime);
    // handleDebug(localDeltaTime);
}


void GameManager::threadUpdate()
{
    auto lastTime = std::chrono::high_resolution_clock::now();
    const float fixedTimeStep = 1.0f / 60.0f; // 60 ticks per second
    float accumulator = 0.0f;
    
    while (running && useThread)
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> frameTime = currentTime - lastTime;
        lastTime = currentTime;
        
        // Cap the frame time to avoid spiral of death
        const float maxFrameTime = 0.25f;
        float deltaSeconds = std::min(frameTime.count(), maxFrameTime);
        
        accumulator += deltaSeconds;
        
        // Run fixed updates as needed
        while (accumulator >= fixedTimeStep)
        {
            if (!paused && !pausedByFocus)
            {
                auto start = std::chrono::high_resolution_clock::now();
                update(fixedTimeStep);
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                spdlog::debug("GameManager threadUpdate: update time {} ms", duration.count() / 1000.0f);
            }
            accumulator -= fixedTimeStep;
        }
        
        // Calculate sleep time to maintain 60Hz if possible
        auto endTime = std::chrono::high_resolution_clock::now();
        auto processingTime = std::chrono::duration<float>(endTime - currentTime).count();
        auto sleepTime = std::max(0.0f, fixedTimeStep - processingTime);

        spdlog::debug("GameManager threadUpdate: sleepTime {} ms", sleepTime * 1000.0f);
        
        if (sleepTime > 0)
        {
            std::this_thread::sleep_for(std::chrono::duration<float>(sleepTime));
        }
    }
}

void GameManager::cullingThread()
{
    while (running && useThread)
    {
        auto start = std::chrono::high_resolution_clock::now();
        gameWorld->culling(&window.getView());
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        spdlog::debug("GameManager cullingThread: culling time {} ms", duration.count() / 1000.0f);
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}

void GameManager::render()
{
    // Clear the window with black color
    window.clear(sf::Color::Black);
    
    this->gameWorld->render(window);
    
    window.display();
}


void GameManager::handleEvents()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            running = false;
        }
        handleInput(event);
    }
}


void GameManager::handleInput(sf::Event event)
{
    if (event.type == sf::Event::KeyPressed)
    {
        spdlog::info("GameManager handleInput: Key pressed {}", static_cast<int>(event.key.code));
    }
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
    {
        running = false;
    }
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F)
    {
        fullscreenMode = !fullscreenMode;
        window.close();
        init();
    }
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P)
    {
        paused = !paused;
        spdlog::info("GameManager handleInput: paused {}", static_cast<bool>(paused));
    }
    // if not focused, pause the game
    if (event.type == sf::Event::LostFocus)
    {
        spdlog::info("GameManager handleInput: lost focus");
        window.setFramerateLimit(1);
        window.setVerticalSyncEnabled(false);
        pausedByFocus = true;
    }
    if (event.type == sf::Event::GainedFocus)   
    {
        spdlog::info("GameManager handleInput: gained focus");
        window.setFramerateLimit(60);
        window.setVerticalSyncEnabled(true);
        pausedByFocus = false;
    }
    
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::T)
    {
        useThread = !useThread;
        spdlog::info("GameManager handleInput: useThread {}", static_cast<bool>(useThread));
        if (useThread)
        {
            instanceGameThread = std::thread(&GameManager::threadUpdate, this);
            instanceCullingThread = std::thread(&GameManager::cullingThread,this);
        }
        else
        {
            if (instanceGameThread.joinable() && instanceCullingThread.joinable())
            {
                instanceGameThread.join();
                instanceCullingThread.join();
            }
        }
    }
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::D)
    {
        debug = !debug;
        spdlog::info("GameManager handleInput: debug {}", static_cast<bool>(debug));
        if (debug)
        {
            spdlog::set_level(spdlog::level::debug);
        }
        else
        {
            spdlog::set_level(spdlog::level::info);
        }
    }
}   


void GameManager::handlePhysics(float localDeltaTime)
{
    this->gameWorld->updatePhysics(localDeltaTime);
}   


void GameManager::handleLogic(float localDeltaTime)
{
    this->gameWorld->updateLogic(localDeltaTime);
}   


// void GameManager::handleNetwork(float localDeltaTime)
// {

// }   


// void GameManager::handleDebug(float localDeltaTime)
// {

// }   


void GameManager::handleCleanup()
{
    
}   


void GameManager::init()
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 4; // 8 niveaux d'anti-aliasing
    // settings.sRgbCapable = true; // Activer le rendu sRGB
    // settings.majorVersion = 4; // Version majeure de l'OpenGL
    // settings.minorVersion = 6; // Version mineure de l'OpenGL
    // settings.attributeFlags = sf::ContextSettings::Core; // Utiliser le profil core
    settings.depthBits = 24; // Profondeur de 24 bits
    settings.stencilBits = 8; // 8 bits de stencil

    if (this->fullscreenMode)
    {
        window.create(sf::VideoMode::getDesktopMode(), "SFML Window", sf::Style::Fullscreen, settings);
    }
    else
    {
        window.create(sf::VideoMode::getDesktopMode(), "SFML Window", sf::Style::Default, settings);
    }
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    // Vérifiez si l'anti-aliasing est supporté
    if (settings.antialiasingLevel > 0) {
        spdlog::info("Anti-aliasing is supported");
    } else {
        spdlog::info("Anti-aliasing is not supported");
    }

    spdlog::info("GameManager init: window created");
    // gui.setTarget(window);
}

void GameManager::start()
{
    running = true;
    if (useThread)
    {
        spdlog::info("GameManager start: using thread");
        instanceGameThread = std::thread(&GameManager::threadUpdate, this);
        instanceCullingThread = std::thread(&GameManager::cullingThread,this);
    }
    run();
}

void GameManager::cleanup()
{
    spdlog::info("GameManager cleanup");
    running = false;
    if (useThread && instanceGameThread.joinable() && instanceCullingThread.joinable())
    {
        instanceGameThread.join();
        instanceCullingThread.join();
    }
}


sf::RenderWindow& GameManager::getWindow()
{
    return window;
}


sf::Clock& GameManager::getClock()
{
    return clock;
}


sf::Time& GameManager::getDeltaTime()
{
    return deltaTime;
}
