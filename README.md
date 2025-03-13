# SFML Project

A simple SFML application that displays a green circle on a black background.

## Prerequisites

- CMake 3.14 or higher
- C++20 compatible compiler
- Git (for FetchContent to download SFML)
- Internet connection (for the first build to download SFML)

## Building the Project

This project uses CMake's FetchContent module to automatically download and build SFML, so you don't need to install SFML separately.

### Windows with MinGW

1. Make sure you have MinGW installed (this project is configured to use CodeBlocks MinGW).
2. Run the provided batch file:
   ```
   configure.bat
   ```
   
   This will:
   - Create a build directory
   - Configure the project with CMake
   - Build the project
   - Copy necessary DLLs

3. Alternatively, you can do it manually:
   ```
   mkdir build
   cd build
   cmake .. -G "MinGW Makefiles"
   cmake --build .
   ```

### Windows with Visual Studio

1. Open a command prompt in the project directory.
2. Create a build directory and navigate to it:
   ```
   mkdir build
   cd build
   ```
3. Generate Visual Studio solution:
   ```
   cmake .. -G "Visual Studio 16 2019" -A x64
   ```
   (Replace with your Visual Studio version if different)
4. Open the generated solution in Visual Studio and build it, or build from command line:
   ```
   cmake --build . --config Release
   ```

### Linux

1. Create a build directory and navigate to it:
   ```
   mkdir build
   cd build
   ```
2. Generate Makefiles:
   ```
   cmake ..
   ```
3. Build the project:
   ```
   cmake --build .
   ```

## Running the Application

After building, you can find the executable in the build directory (or in build/Release on Windows with Visual Studio).

Run the application:
- Windows: `SFMLProject.exe`
- Linux: `./SFMLProject`

## Controls

- Press the Escape key or click the close button to exit the application. 