#include <SFML/Graphics.hpp>
#include <spdlog/spdlog.h>
#include <box2d/box2d.h>
#include <nlohmann/json.hpp>

#include "core/GameManager.hpp"

int main()
{
    spdlog::set_level(spdlog::level::debug);

    GameManager gameManager;
    gameManager.start();
    
    return 0;
}
