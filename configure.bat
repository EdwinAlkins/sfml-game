@echo off
echo Configuring SFML project with MinGW...

REM Add MinGW to PATH
set PATH=%PATH%;C:\Program Files\CodeBlocks\MinGW\bin

REM Clean build directory
if exist build rmdir /s /q build
mkdir build
cd build

REM Configure with CMake
echo Configuring with CMake (this will download and build SFML automatically)...
cmake .. -G "MinGW Makefiles"

echo.
if %ERRORLEVEL% == 0 (
    echo Configuration successful! You can now build the project with:
    echo cd build
    echo cmake --build .
    
    REM Build the project
    echo.
    echo Building the project (this may take a few minutes)...
    cmake --build .
    
    if %ERRORLEVEL% == 0 (
        echo.
        echo Build successful! You can run the application with:
        echo SFMLProject.exe
    ) else (
        echo.
        echo Build failed. Please check the error messages above.
    )
) else (
    echo Configuration failed. Please check the error messages above.
)

pause 