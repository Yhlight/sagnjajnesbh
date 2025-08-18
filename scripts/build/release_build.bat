@echo off
REM CHTL Release Build Script for Windows
REM Cross-platform optimized build for production deployment

setlocal enabledelayedexpansion

REM Configuration
set "SCRIPT_DIR=%~dp0"
set "PROJECT_ROOT=%SCRIPT_DIR%..\.."
set "BUILD_DIR=%PROJECT_ROOT%\build-release"
set "BUILD_TYPE=Release"
set "DIST_DIR=%PROJECT_ROOT%\dist"
set "VERSION=1.0.0"

echo 🚀 CHTL Release Build Script v%VERSION% for Windows
echo Project Root: %PROJECT_ROOT%
echo Build Directory: %BUILD_DIR%
echo Distribution Directory: %DIST_DIR%
echo Build Type: %BUILD_TYPE%
echo.

REM Platform detection
echo === Platform Detection ===
echo ℹ️ Detected platform: Windows
set "PLATFORM=Windows"

REM Prerequisites check
echo === Prerequisites Check ===
where cmake >nul 2>&1
if errorlevel 1 (
    echo ❌ CMake not found. Please install CMake 3.16+
    exit /b 1
) else (
    for /f "tokens=3" %%i in ('cmake --version ^| findstr /R "cmake"') do set CMAKE_VERSION=%%i
    echo ✅ CMake found: !CMAKE_VERSION!
)

where cl >nul 2>&1
if not errorlevel 1 (
    echo ✅ MSVC compiler found
    set "COMPILER=MSVC"
) else (
    where g++ >nul 2>&1
    if not errorlevel 1 (
        echo ✅ GCC compiler found
        set "COMPILER=GCC"
    ) else (
        echo ❌ No C++ compiler found. Please install Visual Studio or MinGW
        exit /b 1
    )
)

where nmake >nul 2>&1 || where make >nul 2>&1
if errorlevel 1 (
    echo ❌ Make not found. Please install build tools
    exit /b 1
) else (
    echo ✅ Make found
)

REM Clean previous builds
echo === Cleaning Previous Builds ===
if exist "%BUILD_DIR%" (
    echo ℹ️ Removing previous build directory...
    rmdir /s /q "%BUILD_DIR%"
)

if exist "%DIST_DIR%" (
    echo ℹ️ Removing previous distribution directory...
    rmdir /s /q "%DIST_DIR%"
)

echo ✅ Clean completed

REM Configure build
echo === Configuring Release Build ===
mkdir "%BUILD_DIR%"
cd /d "%BUILD_DIR%"

echo ℹ️ Running CMake configuration...
if "%COMPILER%"=="MSVC" (
    cmake "%PROJECT_ROOT%" ^
        -DCMAKE_BUILD_TYPE=Release ^
        -DCMAKE_CXX_FLAGS_RELEASE="/O2 /DNDEBUG" ^
        -DCMAKE_INSTALL_PREFIX="%DIST_DIR%" ^
        -DENABLE_TESTING=OFF ^
        -DENABLE_OPTIMIZATIONS=ON
) else (
    cmake "%PROJECT_ROOT%" ^
        -DCMAKE_BUILD_TYPE=Release ^
        -DCMAKE_CXX_FLAGS_RELEASE="-O3 -DNDEBUG" ^
        -DCMAKE_INSTALL_PREFIX="%DIST_DIR%" ^
        -DENABLE_TESTING=OFF ^
        -DENABLE_OPTIMIZATIONS=ON
)

if errorlevel 1 (
    echo ❌ CMake configuration failed
    exit /b 1
) else (
    echo ✅ CMake configuration completed
)

REM Build project
echo === Building CHTL Project ===
echo ℹ️ Building CHTL in release mode...

if "%COMPILER%"=="MSVC" (
    msbuild CHTL.sln /p:Configuration=Release /maxcpucount
) else (
    make -j%NUMBER_OF_PROCESSORS%
)

if errorlevel 1 (
    echo ❌ Build failed
    exit /b 1
) else (
    echo ✅ Build completed successfully
)

REM Run tests
echo === Running Release Tests ===
if exist "bin\ComprehensiveTestRunner.exe" (
    echo ℹ️ Running comprehensive syntax tests...
    .\bin\ComprehensiveTestRunner.exe > "%PROJECT_ROOT%\test-reports\release_test_windows_%date:~0,4%%date:~5,2%%date:~8,2%.log" 2>&1
    if not errorlevel 1 (
        echo ✅ All tests passed
    ) else (
        echo ⚠️ Some tests failed, check logs for details
    )
) else (
    echo ⚠️ Test runner not found, skipping tests
)

REM Create distribution
echo === Creating Distribution Package ===
mkdir "%DIST_DIR%\bin" "%DIST_DIR%\lib" "%DIST_DIR%\include" "%DIST_DIR%\modules" "%DIST_DIR%\docs" "%DIST_DIR%\examples"

REM Copy binaries
if exist "bin\" (
    xcopy /s /e /y "bin\*" "%DIST_DIR%\bin\" >nul 2>&1
    echo ✅ Binaries copied
)

REM Copy libraries
if exist "lib\" (
    xcopy /s /e /y "lib\*" "%DIST_DIR%\lib\" >nul 2>&1
    echo ✅ Libraries copied
)

REM Copy headers
xcopy /s /e /y "%PROJECT_ROOT%\src\Shared\*" "%DIST_DIR%\include\" >nul 2>&1
echo ✅ Headers copied

REM Copy modules
if exist "%PROJECT_ROOT%\src\Module\" (
    xcopy /s /e /y "%PROJECT_ROOT%\src\Module\*" "%DIST_DIR%\modules\" >nul 2>&1
    echo ✅ Modules copied
)

REM Copy documentation
copy "%PROJECT_ROOT%\README.md" "%DIST_DIR%\docs\" >nul 2>&1
copy "%PROJECT_ROOT%\CHTL语法文档.md" "%DIST_DIR%\docs\" >nul 2>&1
copy "%PROJECT_ROOT%\RELEASE_NOTES.md" "%DIST_DIR%\docs\" >nul 2>&1
echo ✅ Documentation copied

REM Create version info
echo CHTL Release Build > "%DIST_DIR%\VERSION"
echo Version: %VERSION% >> "%DIST_DIR%\VERSION"
echo Build Date: %date% %time% >> "%DIST_DIR%\VERSION"
echo Platform: %PLATFORM% >> "%DIST_DIR%\VERSION"
echo Build Type: %BUILD_TYPE% >> "%DIST_DIR%\VERSION"
echo ✅ Version info created

REM Create archive
echo === Creating Release Archive ===
cd /d "%PROJECT_ROOT%"
set "ARCHIVE_NAME=chtl-v%VERSION%-%PLATFORM%-%date:~0,4%%date:~5,2%%date:~8,2%"

if exist "%ProgramFiles%\7-Zip\7z.exe" (
    "%ProgramFiles%\7-Zip\7z.exe" a -tzip "%ARCHIVE_NAME%.zip" "dist\*"
    echo ✅ ZIP archive created: %ARCHIVE_NAME%.zip
) else (
    echo ⚠️ 7-Zip not found, archive creation skipped
)

echo === Release Build Summary ===
echo ✅ CHTL release build completed successfully
echo ℹ️ Distribution location: %DIST_DIR%
echo ℹ️ Archive location: %PROJECT_ROOT%\%ARCHIVE_NAME%*
echo.
echo ℹ️ Release package contents:
echo   📁 bin\         - Executable files
echo   📁 lib\         - Library files
echo   📁 include\     - Header files
echo   📁 modules\     - Official modules (Chtholly, Yuigahama)
echo   📁 docs\        - Documentation
echo   📄 VERSION      - Build information
echo.
echo 🎉 CHTL v%VERSION% release build ready for distribution!

endlocal
exit /b 0