cmake_minimum_required(VERSION 3.14)

# Set compilers before project() call
if(WIN32)
    set(CMAKE_C_COMPILER "C:/Program Files/CodeBlocks/MinGW/bin/gcc.exe")
    set(CMAKE_CXX_COMPILER "C:/Program Files/CodeBlocks/MinGW/bin/g++.exe")
endif()
if(UNIX)
    set(CMAKE_C_COMPILER "gcc")
    set(CMAKE_CXX_COMPILER "g++")
endif()

project(SFMLProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_SYSTEM_NAME MATCHES "CYGWIN" OR CMAKE_SYSTEM_NAME MATCHES "MSYS" OR CMAKE_SYSTEM_NAME MATCHES "MINGW")
    set(CMAKE_CXX_EXTENSIONS ON)
endif ()

find_package(Threads REQUIRED)
message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})

# Enable all warnings
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
endif()

# Enable optimizations
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")


# Add the cmake directory to the CMAKE_MODULE_PATH
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Include the add_sfml.cmake file
include(add_sfml)
include(add_spdlog)
include(add_fmt)
include(add_box2d)
include(add_json)
include(add_tgui)

# Add executable
add_executable(${PROJECT_NAME} main.cpp)


# Include subdirectories that contain source code
# Replace these with your actual subdirectories
add_subdirectory(core)
# add_subdirectory(entities)
# add_subdirectory(states)
# add_subdirectory(utils)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    # ${CMAKE_CURRENT_SOURCE_DIR}/entities
    # ${CMAKE_CURRENT_SOURCE_DIR}/states
    # ${CMAKE_CURRENT_SOURCE_DIR}/utils
)

target_link_libraries(${PROJECT_NAME} core)

# set(SPDLOG_DIR ${CMAKE_SOURCE_DIR})
# include_directories(${SPDLOG_DIR})

# Link SFML libraries
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-system)
target_link_libraries(${PROJECT_NAME} spdlog box2d nlohmann_json::nlohmann_json fmt)
# Link TGUI library
target_link_libraries(${PROJECT_NAME} tgui)

if (UNIX)
    target_link_libraries(${PROJECT_NAME} pthread)
endif (UNIX)
# Copy SFML DLLs to output directory on Windows
if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:sfml-graphics>
            $<TARGET_FILE:sfml-window>
            $<TARGET_FILE:sfml-system>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
            $<TARGET_FILE:box2d>
            $<TARGET_FILE:spdlog>
            $<TARGET_FILE:fmt>
            $<TARGET_FILE:nlohmann_json::nlohmann_json>
            $<TARGET_FILE:tgui::tgui>
            $<TARGET_FILE:pthread>
    )
endif()
