include(FetchContent)

FetchContent_Declare(
  box2d
  GIT_REPOSITORY https://github.com/erincatto/box2d.git
  GIT_TAG        v3.0.0
)

# Définir les options avant d'appeler FetchContent_MakeAvailable
set(BOX2D_BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(BOX2D_BUILD_TESTBED OFF CACHE BOOL "" FORCE)
# set(BOX2D_BUILD_DOCS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(box2d)

# Optionnel : Pour s'assurer que Box2D utilise les bons paramètres CMake
if (NOT TARGET box2d)
    add_subdirectory(${box2d_SOURCE_DIR} ${box2d_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()
