cmake_minimum_required(VERSION 3.14)

# Set compilers before project() call
set(CMAKE_C_COMPILER "C:/Program Files/CodeBlocks/MinGW/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "C:/Program Files/CodeBlocks/MinGW/bin/g++.exe")

project(SFMLProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include FetchContent module
include(FetchContent)

# Configure SFML options before fetching
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_GRAPHICS ON CACHE BOOL "" FORCE)
set(SFML_BUILD_WINDOW ON CACHE BOOL "" FORCE)
set(SFML_BUILD_SYSTEM ON CACHE BOOL "" FORCE)

# Fetch SFML
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.0  # You can specify a specific version/tag
)
FetchContent_MakeAvailable(SFML)

# Add executable
add_executable(${PROJECT_NAME} src/main.cpp)

# Link SFML libraries
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-system)

# Copy SFML DLLs to output directory on Windows
if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:sfml-graphics>
            $<TARGET_FILE:sfml-window>
            $<TARGET_FILE:sfml-system>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif() 