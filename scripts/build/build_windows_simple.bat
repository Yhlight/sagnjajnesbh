@echo off
REM Simplified Windows Build Script for CHTL
REM Handles Windows-specific build requirements

setlocal EnableDelayedExpansion

set "SCRIPT_DIR=%~dp0"
set "PROJECT_ROOT=%SCRIPT_DIR%..\..\"
for %%i in ("%PROJECT_ROOT%") do set "PROJECT_ROOT=%%~fi\"

echo [INFO] CHTL Windows Build Script
echo [INFO] Project root: %PROJECT_ROOT%

cd /d "%PROJECT_ROOT%"

REM Check for CMake
where cmake >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] CMake not found. Please install CMake from https://cmake.org/
    exit /b 1
)

echo [SUCCESS] CMake found

REM Check for Visual Studio environment
set "VS_READY=false"

REM Method 1: Check if already in VS environment
if defined VSINSTALLDIR (
    echo [SUCCESS] Visual Studio environment already active
    set "VS_READY=true"
    goto :vs_check_done
)

REM Method 2: Try to find and setup VS environment
echo [INFO] Searching for Visual Studio installation...

REM Look for VS 2022
for /f "tokens=*" %%i in ('dir /b "%ProgramFiles%\Microsoft Visual Studio\2022\*" 2^>nul') do (
    set "VS_PATH=%ProgramFiles%\Microsoft Visual Studio\2022\%%i"
    if exist "!VS_PATH!\VC\Auxiliary\Build\vcvarsall.bat" (
        echo [INFO] Found Visual Studio 2022 %%i
        echo [INFO] Setting up environment...
        call "!VS_PATH!\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul 2>&1
        if !errorlevel!==0 (
            set "VS_READY=true"
            echo [SUCCESS] Visual Studio 2022 environment activated
            goto :vs_check_done
        )
    )
)

REM Look for VS 2019
for /f "tokens=*" %%i in ('dir /b "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\*" 2^>nul') do (
    set "VS_PATH=%ProgramFiles(x86)%\Microsoft Visual Studio\2019\%%i"
    if exist "!VS_PATH!\VC\Auxiliary\Build\vcvarsall.bat" (
        echo [INFO] Found Visual Studio 2019 %%i
        echo [INFO] Setting up environment...
        call "!VS_PATH!\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul 2>&1
        if !errorlevel!==0 (
            set "VS_READY=true"
            echo [SUCCESS] Visual Studio 2019 environment activated
            goto :vs_check_done
        )
    )
)

:vs_check_done

if "!VS_READY!"=="false" (
    echo [ERROR] Visual Studio environment not available
    echo [INFO] Please install Visual Studio 2019 or 2022 with C++ workload
    echo [INFO] Or run this script from "Developer Command Prompt for VS"
    echo [INFO] 
    echo [INFO] Download from: https://visualstudio.microsoft.com/downloads/
    exit /b 1
)

REM Verify tools are available
where cl >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] MSVC compiler (cl.exe) not found
    exit /b 1
)

where msbuild >nul 2>&1
if %errorlevel%==0 (
    echo [SUCCESS] MSBuild available
    set "BUILD_TOOL=msbuild"
) else (
    echo [WARNING] MSBuild not found, will use cmake --build
    set "BUILD_TOOL=cmake"
)

echo [SUCCESS] Build environment ready

REM Determine build type
set "BUILD_TYPE=%~1"
if "!BUILD_TYPE!"=="" set "BUILD_TYPE=Debug"

if not "!BUILD_TYPE!"=="Debug" if not "!BUILD_TYPE!"=="Release" (
    echo [ERROR] Invalid build type. Use Debug or Release
    exit /b 1
)

echo [INFO] Build type: !BUILD_TYPE!

REM Create build directory
set "BUILD_DIR=%PROJECT_ROOT%build-windows-!BUILD_TYPE!"
echo [INFO] Build directory: !BUILD_DIR!

if exist "!BUILD_DIR!" (
    echo [INFO] Cleaning existing build directory...
    rmdir /s /q "!BUILD_DIR!"
)

mkdir "!BUILD_DIR!"
cd /d "!BUILD_DIR!"

REM Configure with CMake
echo [INFO] Configuring with CMake...

cmake -DCMAKE_BUILD_TYPE=!BUILD_TYPE! ^
      -DCMAKE_GENERATOR_PLATFORM=x64 ^
      -DBUILD_TESTING=ON ^
      "%PROJECT_ROOT%"

if %errorlevel% neq 0 (
    echo [ERROR] CMake configuration failed
    exit /b 1
)

echo [SUCCESS] CMake configuration completed

REM Build the project
echo [INFO] Building CHTL compiler...

if "!BUILD_TOOL!"=="msbuild" (
    REM Find the solution file
    for %%f in (*.sln) do (
        echo [INFO] Building solution: %%f
        msbuild "%%f" /p:Configuration=!BUILD_TYPE! /p:Platform=x64 /m /verbosity:minimal
        goto :build_done
    )
    echo [WARNING] No solution file found, using cmake --build
    cmake --build . --config !BUILD_TYPE! --parallel %NUMBER_OF_PROCESSORS%
) else (
    cmake --build . --config !BUILD_TYPE! --parallel %NUMBER_OF_PROCESSORS%
)

:build_done

if %errorlevel% neq 0 (
    echo [ERROR] Build failed
    exit /b 1
)

echo [SUCCESS] Build completed successfully

REM Verify output
echo [INFO] Verifying build output...

set "EXE_FOUND=false"
for %%d in ("." "src" "Debug" "Release" "bin") do (
    if exist "%%d\chtl_compiler.exe" (
        echo [SUCCESS] Found chtl_compiler.exe in %%d\
        set "EXE_FOUND=true"
    )
    if exist "%%d\chtl.exe" (
        echo [SUCCESS] Found chtl.exe in %%d\
        set "EXE_FOUND=true"
    )
)

if "!EXE_FOUND!"=="false" (
    echo [WARNING] Executable not found in expected locations
    echo [INFO] Searching entire build directory...
    for /r . %%f in (chtl*.exe) do (
        echo [INFO] Found: %%f
        set "EXE_FOUND=true"
    )
)

if "!EXE_FOUND!"=="true" (
    echo [SUCCESS] CHTL compiler build verification passed
) else (
    echo [WARNING] Could not verify executable creation
)

echo [INFO] Build artifacts location: !BUILD_DIR!

exit /b 0