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
    camera = new Camera();

    int height = 1080;
    int width = 1920;
    int nbObjectsSimple = 50000;
    for (int i = 0; i < nbObjectsSimple; i++)
    {
        sf::Shape* newShape = new sf::RectangleShape(sf::Vector2f(2.0f, 2.0f));
        newShape->setFillColor(sf::Color::Blue);
        // this->gameWorld->addGameObject(new GameObjectSimpleBody(this->gameWorld->getWorldId(), sf::Vector2f(rand() % width, rand() % height), *newShape));
        this->gameWorld->addGameObject(new GameObjectSimple(this->gameWorld->getWorldId(), sf::Vector2f(rand() % (width * 2) - width/2, rand() % (height * 2) - height/2), *newShape));
    }
    int nbObjectsSimpleBody = 10000;
    for (int i = 0; i < nbObjectsSimpleBody; i++)
    {
        sf::Shape* newShape = new sf::RectangleShape(sf::Vector2f(5.0f, 5.0f));
        newShape->setFillColor(sf::Color::White);
        this->gameWorld->addGameObject(new GameObjectSimpleBody(this->gameWorld->getWorldId(), sf::Vector2f(rand() % width, rand() % height), *newShape));
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
            gameWorld->culling(*camera);
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
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
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
    const float fixedTimeStep = 1.0f / 60.0f; // 60 ticks per second
    float accumulator = 0.0f;
    auto lastFrameTime = std::chrono::high_resolution_clock::now();
    
    while (running && useThread)
    {
        // Calculate actual elapsed time since last frame
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaSeconds = std::chrono::duration<float>(currentTime - lastFrameTime).count();
        lastFrameTime = currentTime;
        
        // Cap the frame time to avoid spiral of death
        const float maxFrameTime = 0.25f; // Maximum 250ms per frame
        deltaSeconds = std::min(deltaSeconds, maxFrameTime);
        
        // Add to accumulator
        accumulator += deltaSeconds;
        
        // Run as many fixed updates as needed to catch up
        bool updatedThisFrame = false;
        while (accumulator >= fixedTimeStep && running && useThread)
        {
            if (!paused && !pausedByFocus)
            {
                auto start = std::chrono::high_resolution_clock::now();
                update(fixedTimeStep);
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                spdlog::debug("GameManager threadUpdate: update time {} ms pausedByFocus={}", duration.count() / 1000.0f, static_cast<bool>(pausedByFocus));
                updatedThisFrame = true;
            }
            accumulator -= fixedTimeStep;
        }
        
        // If we didn't need to update this frame, sleep to save CPU
        if (!updatedThisFrame)
        {
            // Calculate time until next update is needed
            float timeToNextUpdate = fixedTimeStep - accumulator;
            if (timeToNextUpdate > 0.001f) // Only sleep if we have at least 1ms to wait
            {
                std::this_thread::sleep_for(std::chrono::duration<float>(timeToNextUpdate * 0.9f)); // Sleep for 90% of wait time to avoid oversleeping
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void GameManager::cullingThread()
{
    while (running && useThread)
    {
        if(!pausedByFocus){
            auto start = std::chrono::high_resolution_clock::now();
            gameWorld->culling(*camera);
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            spdlog::debug("GameManager cullingThread: culling time {} ms pausedByFocus={}", duration.count() / 1000.0f, static_cast<bool>(pausedByFocus));
        }
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
        pausedByFocus = true;
        window.setFramerateLimit(1);
        window.setVerticalSyncEnabled(false);
        spdlog::info("GameManager handleInput: lost focus {}", static_cast<bool>(pausedByFocus));
    }
    if (event.type == sf::Event::GainedFocus)   
    {
        pausedByFocus = false;
        window.setFramerateLimit(60);
        window.setVerticalSyncEnabled(true);
        spdlog::info("GameManager handleInput: gained focus {}", static_cast<bool>(pausedByFocus));
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
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Up){
        this->camera->move(sf::Vector2f(0, -10));
    }
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Down){
        this->camera->move(sf::Vector2f(0, 10));
    }
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Left){
        this->camera->move(sf::Vector2f(-10, 0));
    }
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Right){
        this->camera->move(sf::Vector2f(10, 0));
    }
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::A){
        this->camera->setRotation(10);
    }
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::E){
        this->camera->setRotation(-10);
    }
    if (event.type == sf::Event::MouseWheelScrolled)
    {
      if (event.mouseWheelScroll.delta > 0)
      {
        this->camera->setZoom(0.05f);
      }
      else if (event.mouseWheelScroll.delta < 0)
      {
        this->camera->setZoom(-0.05f);
      }
    }
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R){
        this->camera->resetRotation();
        this->camera->resetZoom();
    }
}   


void GameManager::handlePhysics(float localDeltaTime)
{
    auto start = std::chrono::high_resolution_clock::now();
    this->gameWorld->updatePhysics(localDeltaTime);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    spdlog::debug("GameManager handlePhysics: updatePhysics time {} ms", duration.count() / 1000.0f);
}   


void GameManager::handleLogic(float localDeltaTime)
{
    auto start = std::chrono::high_resolution_clock::now();
    this->gameWorld->updateLogic(localDeltaTime);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    spdlog::debug("GameManager handleLogic: updateLogic time {} ms", duration.count() / 1000.0f);
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

    camera->setWindow(&window);
    camera->init();

    spdlog::info("GameManager init: window created");
    // gui.setTarget(window);
}

void GameManager::start()
{
    init();
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
