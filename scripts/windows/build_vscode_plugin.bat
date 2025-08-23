@echo off
setlocal EnableDelayedExpansion

echo ===============================================
echo CHTL VSCode Plugin Build Script for Windows
echo ===============================================

:: Set directories
set SOURCE_DIR=%~dp0..\..
set PLUGIN_DIR=%SOURCE_DIR%\vscode-chtl-extension
set DIST_DIR=%SOURCE_DIR%\dist

:: Check if plugin directory exists
if not exist "%PLUGIN_DIR%" (
    echo VSCode extension directory not found: %PLUGIN_DIR%
    pause
    exit /b 1
)

:: Change to plugin directory
cd /d "%PLUGIN_DIR%"

:: Check if Node.js is installed
where node >nul 2>&1
if errorlevel 1 (
    echo Node.js is not installed or not in PATH!
    pause
    exit /b 1
)

:: Check if npm is installed
where npm >nul 2>&1
if errorlevel 1 (
    echo npm is not installed or not in PATH!
    pause
    exit /b 1
)

:: Install dependencies
echo Installing dependencies...
npm install
if errorlevel 1 (
    echo Failed to install dependencies!
    pause
    exit /b 1
)

:: Build the extension
echo Building VSCode extension...
npm run compile
if errorlevel 1 (
    echo Failed to build extension!
    pause
    exit /b 1
)

:: Package the extension using vsce
echo Packaging extension...
npx vsce package --out "%DIST_DIR%"
if errorlevel 1 (
    echo Failed to package extension!
    pause
    exit /b 1
)

echo.
echo ===============================================
echo VSCode plugin build completed successfully!
echo Package location: %DIST_DIR%
echo ===============================================
pause