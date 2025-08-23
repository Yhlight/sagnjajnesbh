@echo off
setlocal EnableDelayedExpansion

echo ===============================================
echo CHTL Unified Build Script for Windows
echo ===============================================
echo This script will:
echo 1. Build CHTL Compiler (Release)
echo 2. Auto-package all modules
echo 3. Build VSCode extension with embedded components
echo ===============================================

set SOURCE_DIR=%~dp0..\..
set SCRIPT_DIR=%~dp0

:: Step 1: Build CHTL Compiler (Release)
echo.
echo [1/3] Building CHTL Compiler (Release)...
call "%SCRIPT_DIR%build_compiler_release.bat"
if errorlevel 1 (
    echo Failed to build CHTL Compiler!
    pause
    exit /b 1
)

:: Step 2: Auto-package all modules
echo.
echo [2/3] Auto-packaging modules...

:: Package Chtholly modules (CMOD + CJMOD)
echo Packaging Chtholly modules...
if exist "%SOURCE_DIR%\Chtholly\CMOD" (
    for /d %%D in ("%SOURCE_DIR%\Chtholly\CMOD\*") do (
        echo Packaging CMOD: %%~nD
        call "%SCRIPT_DIR%package_cmod.bat" "Chtholly\CMOD\%%~nD"
    )
)

if exist "%SOURCE_DIR%\Chtholly\CJMOD" (
    for /d %%D in ("%SOURCE_DIR%\Chtholly\CJMOD\*") do (
        echo Packaging CJMOD: %%~nD
        call "%SCRIPT_DIR%package_cjmod.bat" "Chtholly\CJMOD\%%~nD"
    )
)

:: Package Yuigahama modules (CMOD only)
echo Packaging Yuigahama modules...
if exist "%SOURCE_DIR%\Yuigahama\CMOD" (
    for /d %%D in ("%SOURCE_DIR%\Yuigahama\CMOD\*") do (
        echo Packaging CMOD: %%~nD
        call "%SCRIPT_DIR%package_cmod.bat" "Yuigahama\CMOD\%%~nD"
    )
)

:: Step 3: Prepare VSCode extension with embedded components
echo.
echo [3/3] Preparing VSCode extension...

:: Create extension assets directory
set EXTENSION_ASSETS=%SOURCE_DIR%\vscode-chtl-extension\assets
if not exist "%EXTENSION_ASSETS%" (
    mkdir "%EXTENSION_ASSETS%"
)

:: Copy compiler executable to extension
if not exist "%EXTENSION_ASSETS%\bin" (
    mkdir "%EXTENSION_ASSETS%\bin"
)
copy "%SOURCE_DIR%\bin\chtl-compiler.exe" "%EXTENSION_ASSETS%\bin\" >nul

:: Copy packaged modules to extension
if not exist "%EXTENSION_ASSETS%\modules" (
    mkdir "%EXTENSION_ASSETS%\modules"
)
if exist "%SOURCE_DIR%\dist\modules\*.cmod" (
    copy "%SOURCE_DIR%\dist\modules\*.cmod" "%EXTENSION_ASSETS%\modules\" >nul
)
if exist "%SOURCE_DIR%\dist\modules\*.cjmod" (
    copy "%SOURCE_DIR%\dist\modules\*.cjmod" "%EXTENSION_ASSETS%\modules\" >nul
)

:: Build VSCode extension
echo Building VSCode extension with embedded components...
call "%SCRIPT_DIR%build_vscode_plugin.bat"
if errorlevel 1 (
    echo Failed to build VSCode extension!
    pause
    exit /b 1
)

echo.
echo ===============================================
echo Unified build completed successfully!
echo.
echo Components built:
echo - CHTL Compiler: %SOURCE_DIR%\bin\chtl-compiler.exe
echo - Module packages: %SOURCE_DIR%\dist\modules\
echo - VSCode extension: %SOURCE_DIR%\dist\
echo ===============================================
pause