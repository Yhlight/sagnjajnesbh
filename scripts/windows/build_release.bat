@echo off
REM CHTL Compiler Release Build Script for Windows
REM This script builds the CHTL compiler in Release mode

echo Building CHTL Compiler (Release Mode) on Windows...

REM Check if CMake is available
cmake --version >nul 2>&1
if %errorlevel% neq 0 (
    echo Error: CMake is not installed or not in PATH
    pause
    exit /b 1
)

REM Check if we have a C++ compiler
where cl >nul 2>&1
if %errorlevel% neq 0 (
    echo Error: MSVC compiler not found. Please run from Developer Command Prompt
    pause
    exit /b 1
)

REM Create build directory
if not exist "build\release" mkdir build\release
cd build\release

REM Configure with CMake (Release mode)
echo Configuring build system...
cmake -DCMAKE_BUILD_TYPE=Release ^
      -DCMAKE_CXX_STANDARD=17 ^
      -DENABLE_TESTING=OFF ^
      -DENABLE_EXAMPLES=ON ^
      ..\..\

if %errorlevel% neq 0 (
    echo Error: CMake configuration failed
    cd ..\..
    pause
    exit /b 1
)

REM Build the project
echo Building CHTL Compiler...
cmake --build . --config Release --parallel

if %errorlevel% neq 0 (
    echo Error: Build failed
    cd ..\..
    pause
    exit /b 1
)

REM Create distribution directory
if not exist "..\..\dist\windows" mkdir ..\..\dist\windows

REM Copy executables
echo Copying release binaries...
copy Release\*.exe ..\..\dist\windows\ >nul 2>&1
copy Release\*.dll ..\..\dist\windows\ >nul 2>&1

REM Copy ANTLR runtime if exists
if exist "Release\antlr4-runtime.dll" copy Release\antlr4-runtime.dll ..\..\dist\windows\

cd ..\..

echo.
echo Build completed successfully!
echo Release binaries are available in: dist\windows\
echo.

pause