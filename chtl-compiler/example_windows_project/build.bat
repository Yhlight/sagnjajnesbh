@echo off
setlocal EnableDelayedExpansion

:: Build example project using prebuilt libraries

echo Building example project...

:: Check for Visual Studio
where cl >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo Error: Visual Studio compiler not found
    echo Please run from Visual Studio Developer Command Prompt
    exit /b 1
)

:: Create build directory
if not exist "build" mkdir "build"

:: Compile
echo Compiling...
cl.exe /std:c++17 /EHsc /MD /O2 ^
    /I"..\src" ^
    /I"..\third_party\antlr4-runtime\runtime\src" ^
    main.cpp ^
    /link /LIBPATH:"..\prebuilt\windows\x64\Release" ^
    chtl_antlr_all.lib ^
    /out:build\example.exe

if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    exit /b 1
)

echo.
echo Build successful!
echo Executable: build\example.exe
echo.
echo Running example...
echo.

build\example.exe