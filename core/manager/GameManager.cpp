#include "GameManager.hpp"

GameManager::GameManager()
    : running(true)
    , minimized(false)
    , fullscreenMode(false)
    , paused(false)
    , pausedByFocus(false)
    , debug(DEBUG_DEFAULT)
    , useThread(true)
    , gameWorld(std::make_unique<GameWorld>())
    , camera(std::make_unique<Camera>())
{
    if (debug)
    {
        spdlog::set_level(spdlog::level::debug);
    }
    else
    {
        spdlog::set_level(spdlog::level::info);
    }
    spdlog::info("GameManager constructor");
    
    int nbObjectsSimple = INITIAL_OBJECTS_COUNT;
    sf::Color color = sf::Color(128, 128, 128);
    
    for (int i = 0; i < nbObjectsSimple; i++)
    {
        auto shape = std::make_unique<sf::RectangleShape>(sf::Vector2f(rand() % 10, rand() % 10));
        shape->setFillColor(color);
        gameWorld->addGameObject(std::make_unique<GameObjectSimple>(
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
        gameWorld->addGameObject(std::make_unique<GameObjectSimpleBody>(
            gameWorld->getWorldId(),
            sf::Vector2f(rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT), 
            std::move(shape)
        ));
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
        spdlog::debug("GameManager run: frame rate {}", 1.0f / deltaTime.asSeconds());
        
        auto endRun = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(endRun - startRun);
        spdlog::debug("GameManager run: total time {} ms", duration.count() / 1000.0f);
        std::this_thread::sleep_for(std::chrono::milliseconds(MAIN_THREAD_SLEEP_MS));
    }
}


void GameManager::update(float localDeltaTime)
{
    handleLogic(localDeltaTime);
    handlePhysics(localDeltaTime);
}


void GameManager::threadUpdate()
{
    float accumulator = 0.0f;
    auto lastFrameTime = std::chrono::high_resolution_clock::now();

    while (running && useThread)
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaSeconds = std::chrono::duration<float>(currentTime - lastFrameTime).count();
        lastFrameTime = currentTime;

        deltaSeconds = std::min(deltaSeconds, MAX_FRAME_TIME);
        accumulator += deltaSeconds;

        bool updatedThisFrame = false;
        while (accumulator >= FIXED_TIME_STEP && running && useThread)
        {
            if (!paused && !pausedByFocus)
            {
                auto start = std::chrono::high_resolution_clock::now();
                update(FIXED_TIME_STEP);
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                spdlog::debug("GameManager threadUpdate: update time {} ms pausedByFocus={}", 
                             duration.count() / 1000.0f, static_cast<bool>(pausedByFocus));
                updatedThisFrame = true;
            }
            accumulator -= FIXED_TIME_STEP;
        }

        if (!updatedThisFrame)
        {
            float timeToNextUpdate = FIXED_TIME_STEP - accumulator;
            if (timeToNextUpdate > 0.001f)
            {
                std::this_thread::sleep_for(std::chrono::duration<float>(timeToNextUpdate * 0.9f));
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(MAIN_THREAD_SLEEP_MS));
    }
}

void GameManager::cullingThread()
{
    while (running && useThread)
    {
        if(!pausedByFocus && !paused){
            auto start = std::chrono::high_resolution_clock::now();
            gameWorld->culling(*camera);
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            spdlog::debug("GameManager cullingThread: culling time {} ms pausedByFocus={}", 
                         duration.count() / 1000.0f, static_cast<bool>(pausedByFocus));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(CULLING_THREAD_SLEEP_MS));
    }
}

void GameManager::render()
{
    window.clear(sf::Color::Black);
    camera->apply(window);  // Appliquer le view de la caméra
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
        
        // Sauvegarder l'état de la caméra
        sf::Vector2f camCenter = camera->getCenter();
        float camZoom = camera->getZoom();
        float camRotation = camera->getRotation();
        
        window.close();
        init();
        
        // Restaurer l'état de la caméra
        camera->setCenter(camCenter);
        camera->setRotation(camRotation);
        // Le zoom est relatif, donc on ajuste
        float zoomDiff = camZoom - 1.0f;
        if (zoomDiff != 0) {
            camera->setZoom(zoomDiff);
        }
    }
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P)
    {
        paused = !paused;
        spdlog::info("GameManager handleInput: paused {}", static_cast<bool>(paused));
    }
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
        window.setFramerateLimit(FRAMERATE_LIMIT);
        window.setVerticalSyncEnabled(VSYNC_DEFAULT);
        spdlog::info("GameManager handleInput: gained focus {}", static_cast<bool>(pausedByFocus));
    }

    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::T)
    {
        useThread = !useThread;
        spdlog::info("GameManager handleInput: useThread {}", static_cast<bool>(useThread));
        if (useThread)
        {
            instanceGameThread = std::thread(&GameManager::threadUpdate, this);
            instanceCullingThread = std::thread(&GameManager::cullingThread, this);
        }
        else
        {
            if (instanceGameThread.joinable())
            {
                instanceGameThread.join();
            }
            if (instanceCullingThread.joinable())
            {
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
        this->camera->move(sf::Vector2f(0, -CAMERA_MOVE_STEP));
    }
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Down){
        this->camera->move(sf::Vector2f(0, CAMERA_MOVE_STEP));
    }
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Left){
        this->camera->move(sf::Vector2f(-CAMERA_MOVE_STEP, 0));
    }
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Right){
        this->camera->move(sf::Vector2f(CAMERA_MOVE_STEP, 0));
    }
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::A){
        this->camera->setRotation(CAMERA_ROTATION_STEP);
    }
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::E){
        this->camera->setRotation(-CAMERA_ROTATION_STEP);
    }
    if (event.type == sf::Event::MouseWheelScrolled)
    {
      if (event.mouseWheelScroll.delta > 0)
      {
        this->camera->setZoom(CAMERA_ZOOM_STEP);
      }
      else if (event.mouseWheelScroll.delta < 0)
      {
        this->camera->setZoom(-CAMERA_ZOOM_STEP);
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


void GameManager::handleCleanup()
{
}


void GameManager::init()
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = ANTIALIASING_LEVEL;
    settings.sRgbCapable = true;
    settings.majorVersion = OPENGL_MAJOR_VERSION;
    settings.minorVersion = OPENGL_MINOR_VERSION;
    settings.depthBits = DEPTH_BITS;
    settings.stencilBits = STENCIL_BITS;

    if (this->fullscreenMode)
    {
        window.create(sf::VideoMode::getDesktopMode(), "SFML Window", sf::Style::Fullscreen, settings);
    }
    else
    {
        window.create(sf::VideoMode::getDesktopMode(), "SFML Window", sf::Style::Default, settings);
    }
    window.setVerticalSyncEnabled(VSYNC_DEFAULT);
    window.setFramerateLimit(FRAMERATE_LIMIT);

    if (settings.antialiasingLevel > 0) {
        spdlog::info("Anti-aliasing is supported");
    } else {
        spdlog::info("Anti-aliasing is not supported");
    }

    // Initialiser la caméra avec la taille de la fenêtre
    camera = std::make_unique<Camera>(
        sf::Vector2f(window.getSize().x / 2.0f, window.getSize().y / 2.0f),
        sf::Vector2f(window.getSize().x, window.getSize().y)
    );
    camera->init();

    spdlog::info("GameManager init: window created");
}

void GameManager::start()
{
    init();
    running = true;
    if (useThread)
    {
        spdlog::info("GameManager start: using thread");
        instanceGameThread = std::thread(&GameManager::threadUpdate, this);
        instanceCullingThread = std::thread(&GameManager::cullingThread, this);
    }
    run();
}

void GameManager::cleanup()
{
    spdlog::info("GameManager cleanup");
    running = false;
    
    // Attendre que les threads se terminent
    if (instanceGameThread.joinable())
    {
        instanceGameThread.join();
    }
    if (instanceCullingThread.joinable())
    {
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
