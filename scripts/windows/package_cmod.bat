@echo off
setlocal EnableDelayedExpansion

echo ===============================================
echo CHTL CMOD Package Script for Windows
echo ===============================================

if "%~1"=="" (
    echo Usage: %0 ^<module_path^>
    echo Example: %0 Chtholly\CMOD\Core
    pause
    exit /b 1
)

:: Set directories
set MODULE_PATH=%~1
set SOURCE_DIR=%~dp0..\..
set DIST_DIR=%SOURCE_DIR%\dist\modules

:: Check if module path exists
if not exist "%SOURCE_DIR%\%MODULE_PATH%" (
    echo Module path not found: %MODULE_PATH%
    pause
    exit /b 1
)

:: Get module name from path
for %%F in ("%MODULE_PATH%") do set MODULE_NAME=%%~nxF

:: Create dist directory
if not exist "%DIST_DIR%" (
    mkdir "%DIST_DIR%"
)

:: Create temporary packaging directory
set TEMP_DIR=%TEMP%\chtl_cmod_package_%RANDOM%
mkdir "%TEMP_DIR%"

echo Packaging CMOD module: %MODULE_NAME%

:: Copy module structure according to CHTL grammar specification
xcopy "%SOURCE_DIR%\%MODULE_PATH%\src" "%TEMP_DIR%\%MODULE_NAME%\src\" /E /I /Y >nul
xcopy "%SOURCE_DIR%\%MODULE_PATH%\info" "%TEMP_DIR%\%MODULE_NAME%\info\" /E /I /Y >nul

:: Check if required files exist (according to CHTL grammar)
if not exist "%TEMP_DIR%\%MODULE_NAME%\src\%MODULE_NAME%.chtl" (
    echo Warning: Main source file %MODULE_NAME%.chtl not found in src directory
)

if not exist "%TEMP_DIR%\%MODULE_NAME%\info\%MODULE_NAME%.chtl" (
    echo Warning: Info file %MODULE_NAME%.chtl not found in info directory
)

:: Create .cmod package (zip format)
set PACKAGE_FILE=%DIST_DIR%\%MODULE_NAME%.cmod

:: Use PowerShell to create zip package
powershell -Command "Compress-Archive -Path '%TEMP_DIR%\*' -DestinationPath '%PACKAGE_FILE%' -Force"
if errorlevel 1 (
    echo Failed to create CMOD package!
    rmdir /S /Q "%TEMP_DIR%"
    pause
    exit /b 1
)

:: Clean up
rmdir /S /Q "%TEMP_DIR%"

echo.
echo ===============================================
echo CMOD package created successfully!
echo Package: %PACKAGE_FILE%
echo ===============================================
pause