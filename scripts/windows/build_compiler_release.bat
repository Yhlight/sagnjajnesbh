@echo off
setlocal EnableDelayedExpansion

echo ===============================================
echo CHTL Compiler Release Build Script for Windows
echo ===============================================

:: Set build directory
set BUILD_DIR=%~dp0..\..\build\release
set SOURCE_DIR=%~dp0..\..

:: Create build directory
if not exist "%BUILD_DIR%" (
    mkdir "%BUILD_DIR%"
)

:: Change to build directory
cd /d "%BUILD_DIR%"

:: Configure with CMake
echo Configuring CMake for Release build...
cmake -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release "%SOURCE_DIR%"
if errorlevel 1 (
    echo CMake configuration failed!
    pause
    exit /b 1
)

:: Build the project
echo Building CHTL Compiler (Release)...
cmake --build . --config Release --parallel
if errorlevel 1 (
    echo Build failed!
    pause
    exit /b 1
)

:: Copy executable to bin directory
set BIN_DIR=%SOURCE_DIR%\bin
if not exist "%BIN_DIR%" (
    mkdir "%BIN_DIR%"
)

copy "Release\chtl-compiler.exe" "%BIN_DIR%\" >nul
if errorlevel 1 (
    echo Failed to copy executable!
    pause
    exit /b 1
)

echo.
echo ===============================================
echo Build completed successfully!
echo Executable: %BIN_DIR%\chtl-compiler.exe
echo ===============================================
pause