@echo off
REM Windows Environment Setup Script for CHTL
REM Detects and configures Visual Studio environment

setlocal EnableDelayedExpansion

echo [INFO] CHTL Windows Environment Setup
echo [INFO] ================================

set "VS_FOUND=false"
set "VS_VERSION="
set "VS_EDITION="
set "VS_PATH="

REM Check if already in VS environment
if defined VSINSTALLDIR (
    echo [SUCCESS] Already in Visual Studio environment
    echo [INFO] VS Install Dir: %VSINSTALLDIR%
    if defined VCINSTALLDIR (
        echo [INFO] VC Install Dir: %VCINSTALLDIR%
    )
    set "VS_FOUND=true"
    goto :environment_ready
)

echo [INFO] Searching for Visual Studio installations...

REM Check for Visual Studio 2022
for /f "tokens=*" %%i in ('dir /b "%ProgramFiles%\Microsoft Visual Studio\2022\*" 2^>nul') do (
    set "VS_CANDIDATE=%ProgramFiles%\Microsoft Visual Studio\2022\%%i"
    if exist "!VS_CANDIDATE!\VC\Auxiliary\Build\vcvarsall.bat" (
        set "VS_FOUND=true"
        set "VS_VERSION=2022"
        set "VS_EDITION=%%i"
        set "VS_PATH=!VS_CANDIDATE!"
        echo [SUCCESS] Found Visual Studio 2022 %%i at: !VS_CANDIDATE!
        goto :setup_environment
    )
)

REM Check for Visual Studio 2019
for /f "tokens=*" %%i in ('dir /b "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\*" 2^>nul') do (
    set "VS_CANDIDATE=%ProgramFiles(x86)%\Microsoft Visual Studio\2019\%%i"
    if exist "!VS_CANDIDATE!\VC\Auxiliary\Build\vcvarsall.bat" (
        set "VS_FOUND=true"
        set "VS_VERSION=2019"
        set "VS_EDITION=%%i"
        set "VS_PATH=!VS_CANDIDATE!"
        echo [SUCCESS] Found Visual Studio 2019 %%i at: !VS_CANDIDATE!
        goto :setup_environment
    )
)

REM Check for Build Tools
for /f "tokens=*" %%i in ('dir /b "%ProgramFiles(x86)%\Microsoft Visual Studio\*\BuildTools" 2^>nul') do (
    set "VS_CANDIDATE=%ProgramFiles(x86)%\Microsoft Visual Studio\%%i\BuildTools"
    if exist "!VS_CANDIDATE!\VC\Auxiliary\Build\vcvarsall.bat" (
        set "VS_FOUND=true"
        set "VS_VERSION=%%i"
        set "VS_EDITION=BuildTools"
        set "VS_PATH=!VS_CANDIDATE!"
        echo [SUCCESS] Found Visual Studio Build Tools %%i at: !VS_CANDIDATE!
        goto :setup_environment
    )
)

if "!VS_FOUND!"=="false" (
    echo [ERROR] Visual Studio not found
    echo [INFO] Please install one of the following:
    echo [INFO] 1. Visual Studio 2022 Community/Professional/Enterprise
    echo [INFO] 2. Visual Studio 2019 Community/Professional/Enterprise  
    echo [INFO] 3. Visual Studio Build Tools 2022
    echo [INFO] 4. Visual Studio Build Tools 2019
    echo [INFO]
    echo [INFO] Download from: https://visualstudio.microsoft.com/downloads/
    echo [INFO] Make sure to include "C++ build tools" workload
    exit /b 1
)

:setup_environment
echo [INFO] Setting up Visual Studio !VS_VERSION! !VS_EDITION! environment...

REM Determine architecture
set "ARCH=x64"
if "%PROCESSOR_ARCHITECTURE%"=="AMD64" (
    set "ARCH=x64"
) else if "%PROCESSOR_ARCHITECTURE%"=="x86" (
    set "ARCH=x86"
) else (
    echo [WARNING] Unknown architecture: %PROCESSOR_ARCHITECTURE%, using x64
    set "ARCH=x64"
)

echo [INFO] Target architecture: !ARCH!

REM Setup Visual Studio environment
set "VCVARS_BAT=!VS_PATH!\VC\Auxiliary\Build\vcvarsall.bat"

if not exist "!VCVARS_BAT!" (
    echo [ERROR] vcvarsall.bat not found at: !VCVARS_BAT!
    exit /b 1
)

echo [INFO] Running: "!VCVARS_BAT!" !ARCH!
call "!VCVARS_BAT!" !ARCH! >nul

if %errorlevel% neq 0 (
    echo [ERROR] Failed to setup Visual Studio environment
    exit /b 1
)

:environment_ready
echo [SUCCESS] Visual Studio environment configured

REM Verify tools are available
echo [INFO] Verifying build tools...

where cl >nul 2>&1
if %errorlevel%==0 (
    echo [SUCCESS] MSVC compiler (cl.exe) available
    for /f "tokens=*" %%v in ('cl 2^>^&1 ^| findstr "Version"') do (
        echo [INFO] %%v
    )
) else (
    echo [ERROR] MSVC compiler not found after environment setup
    exit /b 1
)

where msbuild >nul 2>&1
if %errorlevel%==0 (
    echo [SUCCESS] MSBuild available
    for /f "tokens=*" %%v in ('msbuild /version /nologo 2^>^&1 ^| findstr /R "[0-9]"') do (
        echo [INFO] MSBuild version: %%v
        goto :msbuild_version_found
    )
    :msbuild_version_found
) else (
    echo [WARNING] MSBuild not found, but cl.exe is available
    echo [INFO] CMake should be able to generate build files
)

where cmake >nul 2>&1
if %errorlevel%==0 (
    echo [SUCCESS] CMake available
    for /f "tokens=3" %%v in ('cmake --version 2^>^&1 ^| findstr "version"') do (
        echo [INFO] CMake version: %%v
    )
) else (
    echo [ERROR] CMake not found - please install CMake
    exit /b 1
)

REM Check for optional tools
where ninja >nul 2>&1
if %errorlevel%==0 (
    echo [SUCCESS] Ninja build system available
)

where git >nul 2>&1
if %errorlevel%==0 (
    echo [SUCCESS] Git available
)

echo [SUCCESS] Windows environment setup completed
echo [INFO] Environment is ready for CHTL compilation
echo [INFO] You can now run CHTL build scripts

REM Create environment activation script for future use
set "ENV_SCRIPT=%~dp0..\..\activate_vs_env.bat"
(
echo @echo off
echo REM CHTL Visual Studio Environment Activation
echo.
echo if defined VSINSTALLDIR (
echo     echo Visual Studio environment already active
echo     goto :end
echo ^)
echo.
echo echo Activating Visual Studio environment...
echo call "!VCVARS_BAT!" !ARCH! ^>nul
echo.
echo if %%errorlevel%%==0 (
echo     echo Visual Studio environment activated
echo ^) else (
echo     echo Failed to activate Visual Studio environment
echo     exit /b 1
echo ^)
echo.
echo :end
) > "%ENV_SCRIPT%"

echo [SUCCESS] Created environment activation script: %ENV_SCRIPT%
echo [INFO] Use 'activate_vs_env.bat' to setup VS environment in future sessions

exit /b 0