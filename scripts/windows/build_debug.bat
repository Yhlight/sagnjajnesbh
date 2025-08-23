@echo off
REM CHTL Compiler Debug Build Script for Windows

echo Building CHTL Compiler (Debug Mode) on Windows...

cmake --version >nul 2>&1
if %errorlevel% neq 0 (
    echo Error: CMake is not installed or not in PATH
    pause
    exit /b 1
)

if not exist "build\debug" mkdir build\debug
cd build\debug

cmake -DCMAKE_BUILD_TYPE=Debug ^
      -DCMAKE_CXX_STANDARD=17 ^
      -DENABLE_TESTING=ON ^
      -DENABLE_DEBUG_SYMBOLS=ON ^
      ..\..\

cmake --build . --config Debug --parallel

if not exist "..\..\dist\windows\debug" mkdir ..\..\dist\windows\debug
copy Debug\*.exe ..\..\dist\windows\debug\ >nul 2>&1
copy Debug\*.dll ..\..\dist\windows\debug\ >nul 2>&1
copy Debug\*.pdb ..\..\dist\windows\debug\ >nul 2>&1

cd ..\..
echo Debug build completed successfully!
pause