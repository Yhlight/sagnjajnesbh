@echo off
REM ANTLR 4 Cross-Platform Build Script for Windows
REM Builds ANTLR C++ runtime for Windows platform

setlocal enabledelayedexpansion

REM Configuration
set "SCRIPT_DIR=%~dp0"
set "ANTLR_ROOT=%SCRIPT_DIR%.."
set "SOURCE_DIR=%ANTLR_ROOT%\source\current"
set "PRECOMPILED_DIR=%ANTLR_ROOT%\precompiled"
set "BUILD_DIR=%ANTLR_ROOT%\build"

echo 🔨 ANTLR 4 Cross-Platform Build Script for Windows
echo ANTLR Root: %ANTLR_ROOT%
echo Source Directory: %SOURCE_DIR%
echo Precompiled Directory: %PRECOMPILED_DIR%
echo.

REM Function definitions (using labels and goto)
goto :main

:print_section
echo === %~1 ===
goto :eof

:print_success
echo ✅ %~1
goto :eof

:print_error
echo ❌ %~1
goto :eof

:print_warning
echo ⚠️ %~1
goto :eof

:print_info
echo ℹ️ %~1
goto :eof

:show_usage
echo Usage: %0 [OPTIONS]
echo.
echo Options:
echo   -h, --help              Show this help message
echo   --clean                 Clean previous builds
echo   --debug                 Build debug version
echo   --static                Build static libraries only
echo   --shared                Build shared libraries only
echo.
echo Examples:
echo   %0                      # Build release version
echo   %0 --clean --debug      # Clean debug build
echo.
goto :eof

:check_build_prerequisites
call :print_section "Build Prerequisites Check"

REM Check CMake
where cmake >nul 2>&1
if errorlevel 1 (
    call :print_error "CMake not found"
    exit /b 1
) else (
    for /f "tokens=3" %%i in ('cmake --version ^| findstr /R "cmake"') do set CMAKE_VERSION=%%i
    call :print_success "CMake: !CMAKE_VERSION!"
)

REM Check Visual Studio or MinGW
where cl >nul 2>&1
if not errorlevel 1 (
    call :print_success "MSVC compiler found"
    set "COMPILER=MSVC"
) else (
    where g++ >nul 2>&1
    if not errorlevel 1 (
        call :print_success "GCC compiler found"
        set "COMPILER=GCC"
    ) else (
        call :print_error "No C++ compiler found (Visual Studio or MinGW required)"
        exit /b 1
    )
)

REM Check source availability
if not exist "%SOURCE_DIR%" (
    call :print_error "ANTLR source not found. Run download_antlr.sh first"
    exit /b 1
) else (
    call :print_success "ANTLR source: available"
)

goto :eof

:build_antlr_windows
call :print_section "Building ANTLR for Windows"

set "PLATFORM_DIR=%PRECOMPILED_DIR%\windows-x64"
set "BUILD_PLATFORM_DIR=%BUILD_DIR%\windows-%BUILD_TYPE%"

if not exist "%BUILD_PLATFORM_DIR%" mkdir "%BUILD_PLATFORM_DIR%"
if not exist "%PLATFORM_DIR%" mkdir "%PLATFORM_DIR%"

REM Navigate to ANTLR C++ runtime source
set "CPP_RUNTIME_DIR=%SOURCE_DIR%\runtime\Cpp"

if not exist "%CPP_RUNTIME_DIR%" (
    call :print_error "ANTLR C++ runtime source not found: %CPP_RUNTIME_DIR%"
    exit /b 1
)

cd /d "%BUILD_PLATFORM_DIR%"

call :print_info "Configuring ANTLR C++ runtime build..."

REM Configure based on compiler
if "%COMPILER%"=="MSVC" (
    cmake "%CPP_RUNTIME_DIR%" ^
        -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
        -DCMAKE_INSTALL_PREFIX="%PLATFORM_DIR%" ^
        -DCMAKE_CXX_STANDARD=17 ^
        -DWITH_STATIC_CRT=ON ^
        -G "Visual Studio 16 2019" -A x64
) else (
    cmake "%CPP_RUNTIME_DIR%" ^
        -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
        -DCMAKE_INSTALL_PREFIX="%PLATFORM_DIR%" ^
        -DCMAKE_CXX_STANDARD=17 ^
        -DWITH_STATIC_CRT=OFF ^
        -G "MinGW Makefiles"
)

if errorlevel 1 (
    call :print_error "CMake configuration failed"
    exit /b 1
) else (
    call :print_success "CMake configuration completed"
)

call :print_info "Building ANTLR C++ runtime..."

REM Build
if "%COMPILER%"=="MSVC" (
    cmake --build . --config %BUILD_TYPE% --parallel %NUMBER_OF_PROCESSORS%
    cmake --install . --config %BUILD_TYPE%
) else (
    make -j%NUMBER_OF_PROCESSORS%
    make install
)

if errorlevel 1 (
    call :print_error "ANTLR build failed for Windows"
    exit /b 1
) else (
    call :print_success "ANTLR build completed for Windows"
    
    REM Verify build results
    if exist "%PLATFORM_DIR%\include" if exist "%PLATFORM_DIR%\lib" (
        for /f %%i in ('dir /b /s "%PLATFORM_DIR%\include\*.h" 2^>nul ^| find /c /v ""') do set HEADER_COUNT=%%i
        for /f %%i in ('dir /b /s "%PLATFORM_DIR%\lib\*.lib" "%PLATFORM_DIR%\lib\*.a" 2^>nul ^| find /c /v ""') do set LIB_COUNT=%%i
        
        call :print_success "Build verification:"
        call :print_info "  Headers: !HEADER_COUNT! files"
        call :print_info "  Libraries: !LIB_COUNT! files"
    ) else (
        call :print_error "Build verification failed - missing output files"
        exit /b 1
    )
)

goto :eof

:main
REM Default values
set "BUILD_TYPE=Release"
set "CLEAN=false"

REM Parse command line arguments
:parse_args
if "%~1"=="" goto :end_parse
if "%~1"=="-h" goto :show_usage_and_exit
if "%~1"=="--help" goto :show_usage_and_exit
if "%~1"=="--clean" (
    set "CLEAN=true"
    shift
    goto :parse_args
)
if "%~1"=="--debug" (
    set "BUILD_TYPE=Debug"
    shift
    goto :parse_args
)
REM Add more argument parsing as needed
shift
goto :parse_args

:show_usage_and_exit
call :show_usage
exit /b 0

:end_parse

REM Clean if requested
if "%CLEAN%"=="true" (
    call :print_section "Cleaning Previous Builds"
    if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"
    if exist "%PRECOMPILED_DIR%\windows-x64" rmdir /s /q "%PRECOMPILED_DIR%\windows-x64"
    call :print_success "Previous builds cleaned"
)

REM Check prerequisites
call :check_build_prerequisites
if errorlevel 1 exit /b 1

call :print_info "Build configuration:"
call :print_info "  Platform: Windows"
call :print_info "  Build type: %BUILD_TYPE%"
call :print_info "  Compiler: %COMPILER%"

REM Build ANTLR
call :build_antlr_windows
if errorlevel 1 exit /b 1

call :print_section "ANTLR Windows Build Complete"
call :print_success "ANTLR C++ runtime build completed successfully"

echo.
call :print_info "📁 Build outputs:"
if exist "%PRECOMPILED_DIR%\windows-x64\lib" (
    dir /b "%PRECOMPILED_DIR%\windows-x64\lib\*.lib" "%PRECOMPILED_DIR%\windows-x64\lib\*.a" 2>nul | findstr /v /c:"File Not Found"
)

echo.
call :print_success "🎉 ANTLR Windows build ready!"
call :print_info "Next: Run package_antlr.sh to create distribution packages"

endlocal
exit /b 0