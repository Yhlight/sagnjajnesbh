@echo off
REM CHTL Compiler Release Build Script
REM Cross-platform batch script for building CHTL compiler in release mode

setlocal EnableDelayedExpansion

REM Get script directory and project root
set "SCRIPT_DIR=%~dp0"
set "PROJECT_ROOT=%SCRIPT_DIR%..\..\"

echo [INFO] Starting CHTL Compiler Release Build
echo [INFO] Project root: %PROJECT_ROOT%

cd /d "%PROJECT_ROOT%"

REM Check for required tools
echo [INFO] Checking build dependencies...

where cmake >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] CMake is not installed or not in PATH
    exit /b 1
)

REM Try to setup Visual Studio environment if not already setup
if not defined VSINSTALLDIR (
    echo [INFO] Visual Studio environment not detected, attempting setup...
    if exist "%SCRIPT_DIR%..\integration\setup_windows_env.bat" (
        call "%SCRIPT_DIR%..\integration\setup_windows_env.bat"
        if %errorlevel% neq 0 (
            echo [WARNING] Could not setup Visual Studio environment automatically
        )
    )
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
set "BUILD_DIR=%PROJECT_ROOT%build-release"
echo [INFO] Creating build directory: %BUILD_DIR%

if exist "%BUILD_DIR%" (
    echo [WARNING] Release build directory exists, cleaning...
    rmdir /s /q "%BUILD_DIR%"
)

mkdir "%BUILD_DIR%"
cd /d "%BUILD_DIR%"

REM Configure CMake for release build
echo [INFO] Configuring CMake for release build...

if "!BUILD_SYSTEM!"=="msbuild" (
    cmake -DCMAKE_BUILD_TYPE=Release ^
          -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ^
          -DBUILD_TESTING=OFF ^
          -DENABLE_DEBUG_SYMBOLS=OFF ^
          -DENABLE_OPTIMIZATIONS=ON ^
          -DENABLE_LTO=ON ^
          -DSTRIP_SYMBOLS=ON ^
          "%PROJECT_ROOT%"
) else if "!BUILD_SYSTEM!"=="mingw" (
    cmake -G "MinGW Makefiles" ^
          -DCMAKE_BUILD_TYPE=Release ^
          -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ^
          -DBUILD_TESTING=OFF ^
          -DENABLE_DEBUG_SYMBOLS=OFF ^
          -DENABLE_OPTIMIZATIONS=ON ^
          -DENABLE_LTO=ON ^
          -DSTRIP_SYMBOLS=ON ^
          "%PROJECT_ROOT%"
) else (
    cmake -G "Unix Makefiles" ^
          -DCMAKE_BUILD_TYPE=Release ^
          -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ^
          -DBUILD_TESTING=OFF ^
          -DENABLE_DEBUG_SYMBOLS=OFF ^
          -DENABLE_OPTIMIZATIONS=ON ^
          -DENABLE_LTO=ON ^
          -DSTRIP_SYMBOLS=ON ^
          "%PROJECT_ROOT%"
)

if %errorlevel% neq 0 (
    echo [ERROR] CMake configuration failed
    exit /b 1
)

echo [SUCCESS] CMake configuration completed

REM Build the project
echo [INFO] Building CHTL compiler in release mode...

if "!BUILD_SYSTEM!"=="msbuild" (
    msbuild CHTL.sln /p:Configuration=Release /p:Platform=x64 /m
) else if "!BUILD_SYSTEM!"=="mingw" (
    mingw32-make -j%NUMBER_OF_PROCESSORS%
) else (
    make -j%NUMBER_OF_PROCESSORS%
)

if %errorlevel% neq 0 (
    echo [ERROR] Build failed
    exit /b 1
)

echo [SUCCESS] CHTL compiler release build completed successfully

REM Check if executables were created
echo [INFO] Verifying build output...

set "FOUND_EXE=0"
if exist "%BUILD_DIR%\chtl_compiler.exe" (
    echo [SUCCESS] Found executable: chtl_compiler.exe
    set "FOUND_EXE=1"
    set "EXE_PATH=%BUILD_DIR%\chtl_compiler.exe"
) else if exist "%BUILD_DIR%\src\chtl_compiler.exe" (
    echo [SUCCESS] Found executable: src\chtl_compiler.exe
    set "FOUND_EXE=1"
    set "EXE_PATH=%BUILD_DIR%\src\chtl_compiler.exe"
) else if exist "%BUILD_DIR%\Release\chtl_compiler.exe" (
    echo [SUCCESS] Found executable: Release\chtl_compiler.exe
    set "FOUND_EXE=1"
    set "EXE_PATH=%BUILD_DIR%\Release\chtl_compiler.exe"
) else (
    echo [WARNING] Executable not found: chtl_compiler.exe
)

REM Create distribution directory
set "DIST_DIR=%PROJECT_ROOT%dist"
echo [INFO] Creating distribution directory: %DIST_DIR%

if exist "%DIST_DIR%" (
    rmdir /s /q "%DIST_DIR%"
)

mkdir "%DIST_DIR%\bin"
mkdir "%DIST_DIR%\module"
mkdir "%DIST_DIR%\docs"

REM Copy binaries to distribution
if !FOUND_EXE!==1 (
    copy "!EXE_PATH!" "%DIST_DIR%\bin\" >nul
    echo [SUCCESS] Copied chtl_compiler.exe to distribution
)

REM Copy modules if they exist
if exist "%PROJECT_ROOT%module" (
    xcopy "%PROJECT_ROOT%module\*" "%DIST_DIR%\module\" /E /I /Q >nul 2>&1
    echo [SUCCESS] Copied modules to distribution
)

REM Copy documentation
if exist "%PROJECT_ROOT%docs" (
    xcopy "%PROJECT_ROOT%docs\*" "%DIST_DIR%\docs\" /E /I /Q >nul 2>&1
    echo [SUCCESS] Copied documentation to distribution
)

REM Copy license and readme
if exist "%PROJECT_ROOT%LICENSE" (
    copy "%PROJECT_ROOT%LICENSE" "%DIST_DIR%\" >nul
)

if exist "%PROJECT_ROOT%README.md" (
    copy "%PROJECT_ROOT%README.md" "%DIST_DIR%\" >nul
)

echo [SUCCESS] Release build process completed
echo [INFO] Build artifacts are in: %BUILD_DIR%
echo [INFO] Distribution package is in: %DIST_DIR%

REM Create archive if 7zip or WinRAR is available
where 7z >nul 2>&1
if %errorlevel%==0 (
    set "ARCHIVE_NAME=chtl-compiler-%date:~0,4%%date:~5,2%%date:~8,2%-%time:~0,2%%time:~3,2%%time:~6,2%.zip"
    set "ARCHIVE_NAME=!ARCHIVE_NAME: =0!"
    cd /d "%DIST_DIR%"
    7z a "%PROJECT_ROOT%!ARCHIVE_NAME!" * >nul
    echo [SUCCESS] Created distribution archive: !ARCHIVE_NAME!
) else (
    where winrar >nul 2>&1
    if %errorlevel%==0 (
        set "ARCHIVE_NAME=chtl-compiler-%date:~0,4%%date:~5,2%%date:~8,2%-%time:~0,2%%time:~3,2%%time:~6,2%.rar"
        set "ARCHIVE_NAME=!ARCHIVE_NAME: =0!"
        cd /d "%DIST_DIR%"
        winrar a "%PROJECT_ROOT%!ARCHIVE_NAME!" * >nul
        echo [SUCCESS] Created distribution archive: !ARCHIVE_NAME!
    )
)

exit /b 0