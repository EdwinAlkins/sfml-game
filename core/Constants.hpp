#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <SFML/System/Vector2.hpp>

// Screen dimensions
constexpr int SCREEN_WIDTH = 1920;
constexpr int SCREEN_HEIGHT = 1080;

// Game constants
constexpr int INITIAL_OBJECTS_COUNT = 10000;
constexpr int INITIAL_PHYSICS_OBJECTS_COUNT = 5000;
constexpr float OBJECT_BASE_SPEED = 100.0f;

// Physics constants
constexpr float FIXED_TIME_STEP = 1.0f / 60.0f;  // 60 ticks per second
constexpr float MAX_FRAME_TIME = 0.25f;           // Maximum 250ms per frame
constexpr float GRAVITY_X = 0.0f;
constexpr float GRAVITY_Y = 0.0f;

// Camera constants
constexpr float CAMERA_ZOOM_STEP = 0.05f;
constexpr float CAMERA_MOVE_STEP = 10.0f;
constexpr float CAMERA_ROTATION_STEP = 10.0f;

// Threading constants
constexpr int CULLING_THREAD_SLEEP_MS = 16;
constexpr int MAIN_THREAD_SLEEP_MS = 1;

// Debug constants
constexpr bool DEBUG_DEFAULT = false;
constexpr bool VSYNC_DEFAULT = true;
constexpr int FRAMERATE_LIMIT = 60;

// OpenGL context settings
constexpr int OPENGL_MAJOR_VERSION = 3;
constexpr int OPENGL_MINOR_VERSION = 6;
constexpr int DEPTH_BITS = 24;
constexpr int STENCIL_BITS = 8;
constexpr int ANTIALIASING_LEVEL = 4;

#endif // CONSTANTS_HPP
