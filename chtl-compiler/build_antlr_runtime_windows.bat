@echo off
setlocal EnableDelayedExpansion

:: Build ANTLR4 C++ Runtime for Windows

echo Building ANTLR4 C++ Runtime for Windows...

:: Get current directory
set SCRIPT_DIR=%~dp0
cd /d "%SCRIPT_DIR%"

:: Check if CMake is available
where cmake >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo Error: CMake not found in PATH
    echo Please install CMake from https://cmake.org/
    exit /b 1
)

:: Navigate to ANTLR runtime directory
cd third_party\antlr4-runtime

:: Create build directory
if not exist "build" mkdir "build"
cd build

:: Configure with CMake
echo Configuring ANTLR4 runtime...
cmake .. -G "Visual Studio 17 2022" -A x64 ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DANTLR4_INSTALL=OFF ^
    -DBUILD_SHARED_LIBS=OFF ^
    -DBUILD_TESTS=OFF

if %ERRORLEVEL% NEQ 0 (
    echo Error: CMake configuration failed
    echo Trying with Visual Studio 16 2019...
    cmake .. -G "Visual Studio 16 2019" -A x64 ^
        -DCMAKE_BUILD_TYPE=Release ^
        -DANTLR4_INSTALL=OFF ^
        -DBUILD_SHARED_LIBS=OFF ^
        -DBUILD_TESTS=OFF
)

if %ERRORLEVEL% NEQ 0 (
    echo Error: CMake configuration failed
    exit /b 1
)

:: Build
echo Building ANTLR4 runtime...
cmake --build . --config Release

if %ERRORLEVEL% NEQ 0 (
    echo Error: Build failed
    exit /b 1
)

:: Return to original directory
cd "%SCRIPT_DIR%"

echo.
echo ANTLR4 runtime built successfully!
echo Library location: third_party\antlr4-runtime\build\runtime\Release\antlr4-runtime.lib