@echo off
REM CHTL CMOD Packaging Script for Windows
REM Cross-platform CMOD module packaging utility

setlocal enabledelayedexpansion

REM Configuration
set "SCRIPT_DIR=%~dp0"
set "PROJECT_ROOT=%SCRIPT_DIR%..\.."

echo 📦 CHTL CMOD Packaging Script for Windows
echo Project Root: %PROJECT_ROOT%
echo.

REM Check if module path is provided
if "%~1"=="" (
    echo ❌ Module path is required
    echo.
    echo Usage: %0 [module_path] [output_dir]
    echo.
    echo Examples:
    echo   %0 src\Module\Chtholly\CMOD\Accordion
    echo   %0 src\Module\Yuigahama\CMOD\MusicPlayer dist
    echo.
    exit /b 1
)

set "MODULE_PATH=%~1"
set "OUTPUT_DIR=%~2"
if "%OUTPUT_DIR%"=="" set "OUTPUT_DIR=.\dist"

REM Get module name
for %%i in ("%MODULE_PATH%") do set "MODULE_NAME=%%~ni"

echo ℹ️ Processing module: %MODULE_NAME%
echo ℹ️ Module path: %MODULE_PATH%
echo ℹ️ Output directory: %OUTPUT_DIR%
echo.

REM Validate CMOD structure
echo === Validating CMOD Structure ===

if not exist "%MODULE_PATH%" (
    echo ❌ Module directory does not exist: %MODULE_PATH%
    exit /b 1
)

if not exist "%MODULE_PATH%\src" (
    echo ❌ Missing src\ directory
    exit /b 1
) else (
    echo ✅ src\ directory found
)

if not exist "%MODULE_PATH%\info" (
    echo ❌ Missing info\ directory
    exit /b 1
) else (
    echo ✅ info\ directory found
)

REM Check three-name standard
set "SRC_FILE=%MODULE_PATH%\src\%MODULE_NAME%.chtl"
set "INFO_FILE=%MODULE_PATH%\info\%MODULE_NAME%.chtl"

if not exist "%SRC_FILE%" (
    echo ❌ Missing main source file: %SRC_FILE%
    echo ℹ️ Three-name standard requires: src\%MODULE_NAME%.chtl
    exit /b 1
) else (
    echo ✅ Main source file found: %MODULE_NAME%.chtl
)

if not exist "%INFO_FILE%" (
    echo ❌ Missing info file: %INFO_FILE%
    echo ℹ️ Three-name standard requires: info\%MODULE_NAME%.chtl
    exit /b 1
) else (
    echo ✅ Info file found: %MODULE_NAME%.chtl
)

REM Validate info file content
findstr /C:"[Info]" "%INFO_FILE%" >nul
if errorlevel 1 (
    echo ❌ Info file missing [Info] section
    exit /b 1
) else (
    echo ✅ Info file contains [Info] section
)

findstr /C:"[Export]" "%INFO_FILE%" >nul
if errorlevel 1 (
    echo ⚠️ Info file missing [Export] section (can be auto-generated)
) else (
    echo ✅ Info file contains [Export] section
)

echo ✅ CMOD structure validation passed
echo.

REM Create output directory
if not exist "%OUTPUT_DIR%" mkdir "%OUTPUT_DIR%"

REM Package CMOD
echo === Packaging CMOD Module ===
set "PACKAGE_NAME=%MODULE_NAME%.cmod"
set "PACKAGE_PATH=%OUTPUT_DIR%\%PACKAGE_NAME%"

echo ℹ️ Creating CMOD package: %PACKAGE_NAME%

REM Use PowerShell to create ZIP (available on Windows 10+)
powershell -Command "Compress-Archive -Path '%MODULE_PATH%\*' -DestinationPath '%PACKAGE_PATH%' -Force"

if errorlevel 1 (
    echo ❌ Failed to create CMOD package
    exit /b 1
) else (
    echo ✅ CMOD package created successfully
)

REM Show package info
for %%i in ("%PACKAGE_PATH%") do set "PACKAGE_SIZE=%%~zi"
echo ℹ️ Package size: !PACKAGE_SIZE! bytes

REM Generate manifest
echo === Generating Package Manifest ===
set "MANIFEST_PATH=%OUTPUT_DIR%\%MODULE_NAME%.manifest"

echo # CHTL CMOD Package Manifest > "%MANIFEST_PATH%"
echo # Generated on: %date% %time% >> "%MANIFEST_PATH%"
echo. >> "%MANIFEST_PATH%"
echo [Package] >> "%MANIFEST_PATH%"
echo Name: %MODULE_NAME% >> "%MANIFEST_PATH%"
echo Type: CMOD >> "%MANIFEST_PATH%"
echo Created: %date% %time% >> "%MANIFEST_PATH%"
echo Platform: Windows >> "%MANIFEST_PATH%"
echo. >> "%MANIFEST_PATH%"
echo [Structure] >> "%MANIFEST_PATH%"
echo Root: %MODULE_NAME%/ >> "%MANIFEST_PATH%"
echo Source: src/%MODULE_NAME%.chtl >> "%MANIFEST_PATH%"
echo Info: info/%MODULE_NAME%.chtl >> "%MANIFEST_PATH%"
echo. >> "%MANIFEST_PATH%"
echo [Validation] >> "%MANIFEST_PATH%"
echo ThreeNameStandard: Verified >> "%MANIFEST_PATH%"
echo InfoSection: Present >> "%MANIFEST_PATH%"

echo ✅ Manifest created: %MODULE_NAME%.manifest

echo.
echo === CMOD Packaging Complete ===
echo ✅ CMOD module packaged successfully
echo ℹ️ Package location: %PACKAGE_PATH%
echo ℹ️ Manifest location: %MANIFEST_PATH%
echo.
echo 🎉 CMOD packaging completed!

endlocal
exit /b 0