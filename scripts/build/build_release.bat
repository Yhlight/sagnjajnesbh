@echo off
REM CHTL Compiler Release Build Script
REM Standard CMake-based build for Windows (supports MinGW, MSVC, Clang)

setlocal EnableDelayedExpansion

REM Get script directory and project root
set "SCRIPT_DIR=%~dp0"
set "PROJECT_ROOT=%SCRIPT_DIR%..\..\"
for %%i in ("%PROJECT_ROOT%") do set "PROJECT_ROOT=%%~fi\"

echo [INFO] Starting CHTL Compiler Release Build
echo [INFO] Project root: %PROJECT_ROOT%

cd /d "%PROJECT_ROOT%"

REM Check for required tools
echo [INFO] Checking build dependencies...

where cmake >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] CMake is not installed or not in PATH
    echo [INFO] Please install CMake from: https://cmake.org/download/
    exit /b 1
)

echo [SUCCESS] CMake found

REM Check for C++ compiler (any will do)
set "COMPILER_FOUND=false"

where g++ >nul 2>&1
if %errorlevel%==0 (
    echo [SUCCESS] Found G++ (MinGW/MSYS2)
    set "COMPILER_FOUND=true"
    set "COMPILER_TYPE=MinGW"
)

where clang++ >nul 2>&1
if %errorlevel%==0 (
    echo [SUCCESS] Found Clang++
    set "COMPILER_FOUND=true"
    set "COMPILER_TYPE=Clang"
)

where cl >nul 2>&1
if %errorlevel%==0 (
    echo [SUCCESS] Found MSVC (cl.exe)
    set "COMPILER_FOUND=true"
    set "COMPILER_TYPE=MSVC"
)

if "!COMPILER_FOUND!"=="false" (
    echo [ERROR] No C++ compiler found
    echo [INFO] Please install one of the following:
    echo [INFO] 1. MinGW-w64 (recommended for VSCode)
    echo [INFO] 2. MSYS2 with development tools
    echo [INFO] 3. Visual Studio Build Tools
    echo [INFO] 4. Clang/LLVM
    exit /b 1
)

echo [SUCCESS] Using !COMPILER_TYPE! compiler

REM Create build directory
set "BUILD_DIR=%PROJECT_ROOT%build-release"
echo [INFO] Creating build directory: %BUILD_DIR%

if exist "%BUILD_DIR%" (
    echo [INFO] Cleaning existing build directory...
    rmdir /s /q "%BUILD_DIR%"
)

mkdir "%BUILD_DIR%"
cd /d "%BUILD_DIR%"

REM Configure with CMake (let CMake detect the best generator)
echo [INFO] Configuring with CMake...

REM Let CMake choose the appropriate generator automatically
cmake -DCMAKE_BUILD_TYPE=Release ^
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ^
      "%PROJECT_ROOT%"

if %errorlevel% neq 0 (
    echo [ERROR] CMake configuration failed
    echo [INFO] This might be due to missing dependencies or compiler issues
    echo [INFO] Check the error messages above for details
    exit /b 1
)

echo [SUCCESS] CMake configuration completed

REM Build the project using CMake (universal approach)
echo [INFO] Building CHTL compiler in release mode...

cmake --build . --config Release --parallel %NUMBER_OF_PROCESSORS%

if %errorlevel% neq 0 (
    echo [ERROR] Build failed
    echo [INFO] Check the error messages above for details
    exit /b 1
)

echo [SUCCESS] CHTL compiler release build completed successfully

REM Find and verify executables
echo [INFO] Verifying build output...

set "EXE_FOUND=false"
for /r . %%f in (chtl_compiler.exe chtl.exe) do (
    if exist "%%f" (
        echo [SUCCESS] Found executable: %%f
        for %%s in ("%%f") do echo [INFO] Size: %%~zs bytes
        set "EXE_FOUND=true"
    )
)

if "!EXE_FOUND!"=="false" (
    echo [WARNING] Executables not found in expected locations
    echo [INFO] Searching for any CHTL executables...
    for /r . %%f in (chtl*.exe) do (
        echo [INFO] Found: %%f
        set "EXE_FOUND=true"
    )
)

REM Create distribution directory
set "DIST_DIR=%PROJECT_ROOT%dist"
echo [INFO] Creating distribution directory: %DIST_DIR%

if exist "%DIST_DIR%" (
    rmdir /s /q "%DIST_DIR%"
)

mkdir "%DIST_DIR%\bin"
mkdir "%DIST_DIR%\lib"
mkdir "%DIST_DIR%\docs"
mkdir "%DIST_DIR%\module"

REM Copy executables to distribution
for /r . %%f in (chtl_compiler.exe chtl.exe) do (
    if exist "%%f" (
        copy "%%f" "%DIST_DIR%\bin\" >nul
        echo [SUCCESS] Copied %%~nxf to distribution
    )
)

REM Copy libraries to distribution
for /r . %%f in (*.lib *.a) do (
    if exist "%%f" (
        copy "%%f" "%DIST_DIR%\lib\" >nul 2>&1
    )
)

REM Copy documentation
if exist "%PROJECT_ROOT%docs" (
    xcopy "%PROJECT_ROOT%docs\*" "%DIST_DIR%\docs\" /E /I /Q >nul 2>&1
    echo [SUCCESS] Copied documentation to distribution
)

REM Copy modules
if exist "%PROJECT_ROOT%module" (
    xcopy "%PROJECT_ROOT%module\*" "%DIST_DIR%\module\" /E /I /Q >nul 2>&1
    echo [SUCCESS] Copied modules to distribution
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
echo [INFO] For VSCode: Open the project folder and use CMake extension

REM Create archive if available
where 7z >nul 2>&1
if %errorlevel%==0 (
    set "ARCHIVE_NAME=chtl-compiler-%date:~0,4%%date:~5,2%%date:~8,2%-%time:~0,2%%time:~3,2%%time:~6,2%.zip"
    set "ARCHIVE_NAME=!ARCHIVE_NAME: =0!"
    set "ARCHIVE_NAME=!ARCHIVE_NAME::=!"
    cd /d "%DIST_DIR%"
    7z a "%PROJECT_ROOT%!ARCHIVE_NAME!" * >nul
    echo [SUCCESS] Created distribution archive: !ARCHIVE_NAME!
)

exit /b 0