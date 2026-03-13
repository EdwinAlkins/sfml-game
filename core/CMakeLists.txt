# ==========================
# GameObject
# ==========================
file(GLOB_RECURSE gameobject_sources "gameobject/*.cpp")
file(GLOB_RECURSE gameobject_headers "gameobject/*.hpp")

add_library(gameobject ${gameobject_sources} ${gameobject_headers})

target_include_directories(gameobject PUBLIC 
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/gameobject
)
target_link_libraries(gameobject PUBLIC tgui spdlog::spdlog box2d sfml-graphics sfml-window sfml-system)

# ==========================
# Core
# ==========================
add_library(core 
  GameWorld.cpp GameWorld.hpp
  Camera.cpp Camera.hpp
  )

target_include_directories(core PUBLIC 
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(core PUBLIC gameobject tgui spdlog::spdlog box2d nlohmann_json::nlohmann_json sfml-graphics sfml-window sfml-system fmt eigen cppfs)

# ==========================
# Manager
# ==========================
file(GLOB_RECURSE manager_sources "manager/*.cpp")
file(GLOB_RECURSE manager_headers "manager/*.hpp")

add_library(manager ${manager_sources} ${manager_headers})

target_include_directories(manager PUBLIC 
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/manager
)
target_link_libraries(manager PUBLIC core gameobject tgui spdlog::spdlog box2d nlohmann_json::nlohmann_json sfml-graphics sfml-window sfml-system fmt eigen)
