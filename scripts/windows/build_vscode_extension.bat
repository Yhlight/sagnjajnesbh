@echo off
REM CHTL VSCode Extension Build Script for Windows
REM This script only builds the VSCode extension without compiler dependencies

echo Building CHTL VSCode Extension on Windows...

REM Check if Node.js is available
node --version >nul 2>&1
if %errorlevel% neq 0 (
    echo Error: Node.js is not installed or not in PATH
    echo Please install Node.js from: https://nodejs.org/
    pause
    exit /b 1
)

REM Check if npm is available
npm --version >nul 2>&1
if %errorlevel% neq 0 (
    echo Error: npm is not available
    pause
    exit /b 1
)

REM Navigate to VSCode extension directory
if not exist "vscode-chtl-extension" (
    echo Error: VSCode extension directory not found
    pause
    exit /b 1
)

cd vscode-chtl-extension

REM Install dependencies
echo Installing extension dependencies...
npm install

if %errorlevel% neq 0 (
    echo Error: npm install failed
    cd ..
    pause
    exit /b 1
)

REM Install vsce if not present
echo Checking for vsce...
npx vsce --version >nul 2>&1
if %errorlevel% neq 0 (
    echo Installing vsce...
    npm install -g vsce
)

REM Compile TypeScript
echo Compiling TypeScript source...
npm run compile

if %errorlevel% neq 0 (
    echo Error: TypeScript compilation failed
    cd ..
    pause
    exit /b 1
)

REM Create distribution directory
if not exist "..\dist\vscode" mkdir ..\dist\vscode

REM Package the extension
echo Packaging VSCode extension...
npx vsce package --out ..\dist\vscode\chtl-extension.vsix

if %errorlevel% neq 0 (
    echo Error: Extension packaging failed
    cd ..
    pause
    exit /b 1
)

cd ..

echo.
echo VSCode extension build completed successfully!
echo Extension package available at: dist\vscode\chtl-extension.vsix
echo.
echo To install the extension:
echo   code --install-extension dist\vscode\chtl-extension.vsix
echo.

pause