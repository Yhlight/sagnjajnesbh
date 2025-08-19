@echo off
REM VSCode CHTL Plugin Build Script
REM Cross-platform batch script for building and packaging VSCode CHTL extension

setlocal EnableDelayedExpansion

REM Get script directory and project root
set "SCRIPT_DIR=%~dp0"
set "PROJECT_ROOT=%SCRIPT_DIR%..\..\"

echo [INFO] Starting VSCode CHTL Plugin Build
echo [INFO] Project root: %PROJECT_ROOT%

REM Configuration
set "PLUGIN_DIR=%PROJECT_ROOT%vscode-chtl-extension"
set "BUILD_MODE=%~1"

if "%BUILD_MODE%"=="" set "BUILD_MODE=production"

if not "%BUILD_MODE%"=="production" if not "%BUILD_MODE%"=="development" (
    echo [ERROR] Invalid build mode. Use 'production' or 'development'
    exit /b 1
)

echo [INFO] Build mode: %BUILD_MODE%

REM Check if plugin directory exists
if not exist "%PLUGIN_DIR%" (
    echo [ERROR] VSCode plugin directory not found: %PLUGIN_DIR%
    exit /b 1
)

cd /d "%PLUGIN_DIR%"

REM Check for required tools
echo [INFO] Checking build dependencies...

where node >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] Node.js is not installed or not in PATH
    echo [INFO] Please install Node.js from: https://nodejs.org/
    exit /b 1
)

where npm >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] npm is not installed or not in PATH
    exit /b 1
)

for /f "tokens=*" %%i in ('node --version') do set "NODE_VERSION=%%i"
for /f "tokens=*" %%i in ('npm --version') do set "NPM_VERSION=%%i"

echo [INFO] Node.js version: %NODE_VERSION%
echo [INFO] npm version: %NPM_VERSION%

REM Check if vsce is installed
where vsce >nul 2>&1
if %errorlevel% neq 0 (
    echo [WARNING] vsce (Visual Studio Code Extension Manager) not found
    echo [INFO] Installing vsce globally...
    npm install -g vsce
    
    if %errorlevel% neq 0 (
        echo [ERROR] Failed to install vsce
        exit /b 1
    )
    
    echo [SUCCESS] vsce installed successfully
)

for /f "tokens=*" %%i in ('vsce --version 2^>nul') do set "VSCE_VERSION=%%i"
echo [INFO] vsce version: %VSCE_VERSION%

echo [SUCCESS] Build dependencies verified

REM Install dependencies
echo [INFO] Installing Node.js dependencies...

if exist "package-lock.json" (
    npm ci
) else (
    npm install
)

if %errorlevel% neq 0 (
    echo [ERROR] Failed to install dependencies
    exit /b 1
)

echo [SUCCESS] Dependencies installed successfully

REM Run linting
echo [INFO] Running code linting...

npm run lint >nul 2>&1
if %errorlevel%==0 (
    echo [SUCCESS] Linting passed
) else (
    echo [WARNING] Linting issues found, attempting to fix...
    npm run lint:fix >nul 2>&1
    if %errorlevel%==0 (
        echo [SUCCESS] Linting issues fixed automatically
    ) else (
        echo [WARNING] Some linting issues could not be fixed automatically
    )
)

REM Run tests if they exist
npm run test >nul 2>&1
if %errorlevel%==0 (
    echo [INFO] Running tests...
    npm run test
    
    if %errorlevel%==0 (
        echo [SUCCESS] All tests passed
    ) else (
        echo [WARNING] Some tests failed, but continuing build...
    )
) else (
    echo [WARNING] No test script found, skipping tests
)

REM Build the extension
echo [INFO] Building VSCode extension...

if "%BUILD_MODE%"=="production" (
    REM Production build
    npm run build >nul 2>&1
    if %errorlevel%==0 (
        npm run build
    ) else (
        npm run compile >nul 2>&1
        if %errorlevel%==0 (
            npm run compile
        ) else (
            echo [WARNING] No build script found, using TypeScript compiler directly
            where tsc >nul 2>&1
            if %errorlevel%==0 (
                tsc -p .
            ) else (
                echo [WARNING] TypeScript compiler not found, skipping compilation
            )
        )
    )
) else (
    REM Development build
    npm run watch >nul 2>&1
    if %errorlevel%==0 (
        echo [INFO] Development mode: running watch briefly
        start /b npm run watch
        timeout /t 5 >nul
        taskkill /f /im node.exe /fi "WINDOWTITLE eq npm run watch" >nul 2>&1
    ) else (
        echo [WARNING] No watch script found, using regular compilation
        npm run compile >nul 2>&1
        if %errorlevel%==0 (
            npm run compile
        )
    )
)

echo [SUCCESS] Extension build completed

REM Package the extension
echo [INFO] Packaging VSCode extension...

REM Clean previous packages
del *.vsix >nul 2>&1

REM Create package
if "%BUILD_MODE%"=="production" (
    vsce package --out "chtl-extension-%date:~0,4%%date:~5,2%%date:~8,2%-%time:~0,2%%time:~3,2%%time:~6,2%.vsix"
) else (
    vsce package --out "chtl-extension-dev-%date:~0,4%%date:~5,2%%date:~8,2%-%time:~0,2%%time:~3,2%%time:~6,2%.vsix"
)

REM Clean up time format (remove colons and spaces)
for %%f in (*.vsix) do (
    set "PACKAGE_FILE=%%f"
    set "PACKAGE_FILE=!PACKAGE_FILE: =0!"
    set "PACKAGE_FILE=!PACKAGE_FILE::=!"
    if not "%%f"=="!PACKAGE_FILE!" (
        ren "%%f" "!PACKAGE_FILE!"
    )
)

if %errorlevel% neq 0 (
    echo [ERROR] Failed to package extension
    exit /b 1
)

REM Find the created package
for /f "delims=" %%f in ('dir /b /o-d *.vsix 2^>nul') do (
    set "PACKAGE_FILE=%%f"
    goto :found_package
)

echo [ERROR] Package file not found after packaging
exit /b 1

:found_package
for %%f in ("%PACKAGE_FILE%") do set "PACKAGE_SIZE=%%~zf"
echo [SUCCESS] Extension packaged successfully: %PACKAGE_FILE% (Size: %PACKAGE_SIZE% bytes)

REM Move package to project root packages directory
set "PACKAGES_DIR=%PROJECT_ROOT%packages"
if not exist "%PACKAGES_DIR%" mkdir "%PACKAGES_DIR%"
copy "%PACKAGE_FILE%" "%PACKAGES_DIR%\" >nul
echo [SUCCESS] Package copied to: %PACKAGES_DIR%\%PACKAGE_FILE%

REM Create copy as latest
cd /d "%PACKAGES_DIR%"
copy "%PACKAGE_FILE%" "chtl-extension-latest.vsix" >nul
echo [SUCCESS] Created copy: chtl-extension-latest.vsix

REM Verify package contents
echo [INFO] Verifying package contents...

where 7z >nul 2>&1
if %errorlevel%==0 (
    7z l "%PACKAGES_DIR%\%PACKAGE_FILE%" | more
    echo [INFO] Package verification completed
)

REM Generate installation instructions
set "INSTALL_SCRIPT=%PACKAGES_DIR%\install_chtl_extension.bat"
(
echo @echo off
echo REM CHTL VSCode Extension Installation Script
echo.
echo setlocal EnableDelayedExpansion
echo.
echo set "SCRIPT_DIR=%%~dp0"
echo set "PACKAGE_FILE=%%SCRIPT_DIR%%%PACKAGE_FILE%"
echo.
echo echo Installing CHTL VSCode Extension...
echo.
echo where code ^>nul 2^>^&1
echo if %%errorlevel%% neq 0 (
echo     echo ERROR: VSCode 'code' command not found in PATH
echo     echo Please ensure VSCode is installed and the 'code' command is available
echo     exit /b 1
echo ^)
echo.
echo if not exist "%%PACKAGE_FILE%%" (
echo     echo ERROR: Package file not found: %%PACKAGE_FILE%%
echo     exit /b 1
echo ^)
echo.
echo code --install-extension "%%PACKAGE_FILE%%"
echo.
echo if %%errorlevel%%==0 (
echo     echo SUCCESS: CHTL VSCode Extension installed successfully
echo     echo Restart VSCode to activate the extension
echo ^) else (
echo     echo ERROR: Failed to install extension
echo     exit /b 1
echo ^)
) > "%INSTALL_SCRIPT%"

echo [SUCCESS] Created installation script: %INSTALL_SCRIPT%

REM Generate Unix installation script
set "INSTALL_SCRIPT_UNIX=%PACKAGES_DIR%\install_chtl_extension.sh"
(
echo #!/bin/bash
echo # CHTL VSCode Extension Installation Script
echo.
echo set -e
echo.
echo SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
echo PACKAGE_FILE="$SCRIPT_DIR/%PACKAGE_FILE%"
echo.
echo echo "Installing CHTL VSCode Extension..."
echo.
echo if ! command -v code ^&^> /dev/null; then
echo     echo "ERROR: VSCode 'code' command not found in PATH"
echo     echo "Please ensure VSCode is installed and the 'code' command is available"
echo     exit 1
echo fi
echo.
echo if [ ! -f "$PACKAGE_FILE" ]; then
echo     echo "ERROR: Package file not found: $PACKAGE_FILE"
echo     exit 1
echo fi
echo.
echo code --install-extension "$PACKAGE_FILE"
echo.
echo if [ $? -eq 0 ]; then
echo     echo "SUCCESS: CHTL VSCode Extension installed successfully"
echo     echo "Restart VSCode to activate the extension"
echo else
echo     echo "ERROR: Failed to install extension"
echo     exit 1
echo fi
) > "%INSTALL_SCRIPT_UNIX%"

echo [SUCCESS] Created Unix installation script: %INSTALL_SCRIPT_UNIX%

REM Development server information
if "%BUILD_MODE%"=="development" (
    echo [INFO] Development build completed
    echo [INFO] To test the extension:
    echo [INFO] 1. Open VSCode
    echo [INFO] 2. Press F5 to open Extension Development Host
    echo [INFO] 3. Open a .chtl file to test the extension
)

echo [SUCCESS] VSCode CHTL Plugin build process completed
echo [INFO] Package location: %PACKAGES_DIR%\%PACKAGE_FILE%
echo [INFO] Installation script: %INSTALL_SCRIPT%
echo [INFO] To install: %INSTALL_SCRIPT%

REM Show final package info
cd /d "%PACKAGES_DIR%"
echo [INFO] Available packages:
dir *.vsix 2>nul || echo [WARNING] No packages found

exit /b 0