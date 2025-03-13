# Collect all source files from current directory
# file(GLOB_RECURSE CURRENT_DIR_SOURCES "*.cpp")
# file(GLOB_RECURSE CURRENT_DIR_SOURCES "*.hpp")

add_library(core 
  GameManager.cpp GameManager.hpp
  GameWorld.cpp GameWorld.hpp
  GameObject.cpp GameObject.hpp
  Camera.cpp Camera.hpp
  )

target_link_libraries(core PUBLIC tgui spdlog::spdlog box2d nlohmann_json::nlohmann_json sfml-graphics sfml-window sfml-system fmt eigen)
