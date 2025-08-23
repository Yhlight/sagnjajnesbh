@echo off
setlocal EnableDelayedExpansion

echo ===============================================
echo CHTL Unified Package Script for Windows
echo ===============================================

if "%~1"=="" (
    echo Usage: %0 ^<module_path^>
    echo Example: %0 Chtholly\CMOD\Core
    echo Example: %0 Chtholly\CJMOD\printMylove
    pause
    exit /b 1
)

:: Set directories
set MODULE_PATH=%~1
set SOURCE_DIR=%~dp0..\..
set SCRIPT_DIR=%~dp0

:: Check if module path exists
if not exist "%SOURCE_DIR%\%MODULE_PATH%" (
    echo Module path not found: %MODULE_PATH%
    pause
    exit /b 1
)

:: Determine module type based on path
echo %MODULE_PATH% | findstr /i "CMOD" >nul
if %errorlevel%==0 (
    set MODULE_TYPE=CMOD
    goto :package
)

echo %MODULE_PATH% | findstr /i "CJMOD" >nul
if %errorlevel%==0 (
    set MODULE_TYPE=CJMOD
    goto :package
)

:: Auto-detect module type by examining contents
echo Auto-detecting module type...
if exist "%SOURCE_DIR%\%MODULE_PATH%\src\*.cpp" (
    set MODULE_TYPE=CJMOD
) else if exist "%SOURCE_DIR%\%MODULE_PATH%\src\*.h" (
    set MODULE_TYPE=CJMOD
) else (
    set MODULE_TYPE=CMOD
)

:package
echo Detected module type: !MODULE_TYPE!

if "!MODULE_TYPE!"=="CMOD" (
    echo Calling CMOD package script...
    call "%SCRIPT_DIR%package_cmod.bat" "%MODULE_PATH%"
) else (
    echo Calling CJMOD package script...
    call "%SCRIPT_DIR%package_cjmod.bat" "%MODULE_PATH%"
)

echo.
echo ===============================================
echo Unified packaging completed!
echo ===============================================
pause