@echo off
REM CHTL Compiler Debug Build Script
REM Cross-platform batch script for building CHTL compiler in debug mode

setlocal EnableDelayedExpansion

REM Get script directory and project root
set "SCRIPT_DIR=%~dp0"
set "PROJECT_ROOT=%SCRIPT_DIR%..\..\"

echo [INFO] Starting CHTL Compiler Debug Build
echo [INFO] Project root: %PROJECT_ROOT%

cd /d "%PROJECT_ROOT%"

REM Check for required tools
echo [INFO] Checking build dependencies...

where cmake >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] CMake is not installed or not in PATH
    exit /b 1
)

REM Check for Visual Studio or MinGW
set "BUILD_SYSTEM="

REM First check for MSBuild (Visual Studio Build Tools)
where msbuild >nul 2>&1
if %errorlevel%==0 (
    set "BUILD_SYSTEM=msbuild"
    echo [INFO] Found MSBuild
    goto :build_system_found
)

REM Check for Visual Studio Developer Command Prompt
if defined VSINSTALLDIR (
    echo [INFO] Visual Studio environment detected: %VSINSTALLDIR%
    set "BUILD_SYSTEM=msbuild"
    goto :build_system_found
)

REM Check for cl.exe (MSVC compiler)
where cl >nul 2>&1
if %errorlevel%==0 (
    echo [INFO] Found MSVC compiler, attempting to find MSBuild...
    REM Try to find MSBuild in common locations
    for %%p in (
        "%ProgramFiles%\Microsoft Visual Studio\2022\*\MSBuild\Current\Bin\MSBuild.exe"
        "%ProgramFiles%\Microsoft Visual Studio\2019\*\MSBuild\Current\Bin\MSBuild.exe"
        "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\*\MSBuild\Current\Bin\MSBuild.exe"
        "%ProgramFiles(x86)%\MSBuild\*\Bin\MSBuild.exe"
    ) do (
        if exist "%%p" (
            set "BUILD_SYSTEM=msbuild"
            echo [INFO] Found MSBuild at: %%p
            goto :build_system_found
        )
    )
)

REM Check for MinGW
where mingw32-make >nul 2>&1
if %errorlevel%==0 (
    set "BUILD_SYSTEM=mingw"
    echo [INFO] Found MinGW
    goto :build_system_found
)

REM Check for MSYS2/Cygwin Make
where make >nul 2>&1
if %errorlevel%==0 (
    set "BUILD_SYSTEM=make"
    echo [INFO] Found Make
    goto :build_system_found
)

REM Check for Ninja
where ninja >nul 2>&1
if %errorlevel%==0 (
    set "BUILD_SYSTEM=ninja"
    echo [INFO] Found Ninja
    goto :build_system_found
)

REM If nothing found, provide helpful error message
echo [ERROR] No suitable build system found
echo [INFO] Please install one of the following:
echo [INFO] 1. Visual Studio 2019/2022 with C++ workload
echo [INFO] 2. Visual Studio Build Tools
echo [INFO] 3. MinGW-w64
echo [INFO] 4. MSYS2 with development tools
echo [INFO] 
echo [INFO] For Visual Studio, ensure you're running from:
echo [INFO] - Developer Command Prompt for VS
echo [INFO] - Developer PowerShell for VS
echo [INFO] - Or run vcvarsall.bat first
exit /b 1

:build_system_found

echo [SUCCESS] Build dependencies verified
echo [INFO] Using build system: !BUILD_SYSTEM!

REM Create build directory
set "BUILD_DIR=%PROJECT_ROOT%build-debug"
echo [INFO] Creating build directory: %BUILD_DIR%

if exist "%BUILD_DIR%" (
    echo [WARNING] Debug build directory exists, cleaning...
    rmdir /s /q "%BUILD_DIR%"
)

mkdir "%BUILD_DIR%"
cd /d "%BUILD_DIR%"

REM Configure CMake for debug build
echo [INFO] Configuring CMake for debug build...

if "!BUILD_SYSTEM!"=="msbuild" (
    cmake -DCMAKE_BUILD_TYPE=Debug ^
          -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ^
          -DBUILD_TESTING=ON ^
          -DENABLE_DEBUG_SYMBOLS=ON ^
          -DENABLE_SANITIZERS=OFF ^
          "%PROJECT_ROOT%"
) else if "!BUILD_SYSTEM!"=="mingw" (
    cmake -G "MinGW Makefiles" ^
          -DCMAKE_BUILD_TYPE=Debug ^
          -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ^
          -DBUILD_TESTING=ON ^
          -DENABLE_DEBUG_SYMBOLS=ON ^
          -DENABLE_SANITIZERS=OFF ^
          "%PROJECT_ROOT%"
) else (
    cmake -G "Unix Makefiles" ^
          -DCMAKE_BUILD_TYPE=Debug ^
          -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ^
          -DBUILD_TESTING=ON ^
          -DENABLE_DEBUG_SYMBOLS=ON ^
          -DENABLE_SANITIZERS=OFF ^
          "%PROJECT_ROOT%"
)

if %errorlevel% neq 0 (
    echo [ERROR] CMake configuration failed
    exit /b 1
)

echo [SUCCESS] CMake configuration completed

REM Build the project
echo [INFO] Building CHTL compiler in debug mode...

if "!BUILD_SYSTEM!"=="msbuild" (
    msbuild CHTL.sln /p:Configuration=Debug /m
) else if "!BUILD_SYSTEM!"=="mingw" (
    mingw32-make -j%NUMBER_OF_PROCESSORS%
) else (
    make -j%NUMBER_OF_PROCESSORS%
)

if %errorlevel% neq 0 (
    echo [ERROR] Build failed
    exit /b 1
)

echo [SUCCESS] CHTL compiler debug build completed successfully

REM Check if executables were created
echo [INFO] Verifying build output...

if exist "%BUILD_DIR%\chtl_compiler.exe" (
    echo [SUCCESS] Found executable: chtl_compiler.exe
) else if exist "%BUILD_DIR%\src\chtl_compiler.exe" (
    echo [SUCCESS] Found executable: src\chtl_compiler.exe
) else if exist "%BUILD_DIR%\Debug\chtl_compiler.exe" (
    echo [SUCCESS] Found executable: Debug\chtl_compiler.exe
) else (
    echo [WARNING] Executable not found: chtl_compiler.exe
)

if exist "%BUILD_DIR%\chtl_test.exe" (
    echo [SUCCESS] Found executable: chtl_test.exe
) else if exist "%BUILD_DIR%\src\chtl_test.exe" (
    echo [SUCCESS] Found executable: src\chtl_test.exe
) else if exist "%BUILD_DIR%\Debug\chtl_test.exe" (
    echo [SUCCESS] Found executable: Debug\chtl_test.exe
) else (
    echo [WARNING] Executable not found: chtl_test.exe
)

REM Copy compile_commands.json to project root for IDE support
if exist "%BUILD_DIR%\compile_commands.json" (
    copy "%BUILD_DIR%\compile_commands.json" "%PROJECT_ROOT%\" >nul
    echo [SUCCESS] Copied compile_commands.json for IDE support
)

echo [SUCCESS] Debug build process completed
echo [INFO] Build artifacts are in: %BUILD_DIR%
echo [INFO] To run tests: cd %BUILD_DIR% ^&^& ctest

exit /b 0