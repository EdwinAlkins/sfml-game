include(FetchContent)

FetchContent_Declare(
  tgui
  GIT_REPOSITORY https://github.com/texus/TGUI.git
  GIT_TAG        v1.8.0
)

# Set TGUI backend before making it available
# Options: SFML_GRAPHICS, SFML_OPENGL3, SDL_RENDERER, SDL_OPENGL3, etc.
set(TGUI_BACKEND "SFML_GRAPHICS" CACHE STRING "Backend to use with TGUI")

# Enable debugging to see what's happening
# set(CMAKE_VERBOSE_MAKEFILE ON)
message(STATUS "Setting TGUI_BACKEND to: ${TGUI_BACKEND}")

FetchContent_MakeAvailable(tgui)

# Make sure TGUI was found properly
if(NOT TARGET tgui)
  message(FATAL_ERROR "TGUI target not found after FetchContent_MakeAvailable")
endif()
