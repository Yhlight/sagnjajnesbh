@echo off
REM CHTL CMOD Module Packaging Script for Windows
REM Packages CMOD modules according to CHTL syntax documentation

if "%~1"=="" (
    echo Usage: pack_cmod.bat ^<module_directory^>
    echo.
    echo Example: pack_cmod.bat Chtholly
    echo.
    echo CMOD Structure Requirements:
    echo   ModuleName/
    echo     src/
    echo       ModuleName.chtl  ^(main module^)
    echo       Other.chtl       ^(optional additional files^)
    echo     info/
    echo       ModuleName.chtl  ^(module information^)
    echo.
    pause
    exit /b 1
)

set MODULE_DIR=%~1
set MODULE_NAME=%~1

echo Packaging CMOD module: %MODULE_NAME%

REM Check if module directory exists
if not exist "%MODULE_DIR%" (
    echo Error: Module directory '%MODULE_DIR%' not found
    pause
    exit /b 1
)

REM Validate CMOD structure according to CHTL syntax documentation
echo Validating CMOD structure...

if not exist "%MODULE_DIR%\src" (
    echo Error: Missing 'src' directory in module
    echo Required CMOD structure: %MODULE_NAME%/src/
    pause
    exit /b 1
)

if not exist "%MODULE_DIR%\info" (
    echo Error: Missing 'info' directory in module
    echo Required CMOD structure: %MODULE_NAME%/info/
    pause
    exit /b 1
)

if not exist "%MODULE_DIR%\info\%MODULE_NAME%.chtl" (
    echo Error: Missing module info file: %MODULE_NAME%/info/%MODULE_NAME%.chtl
    echo This file must contain [Info] configuration block
    pause
    exit /b 1
)

REM Check for main module file (optional according to documentation)
if exist "%MODULE_DIR%\src\%MODULE_NAME%.chtl" (
    echo Found main module file: %MODULE_NAME%.chtl
) else (
    echo Warning: Main module file not found, checking for submodules...
    dir /b "%MODULE_DIR%\src\*.chtl" >nul 2>&1
    if %errorlevel% neq 0 (
        echo Error: No CHTL files found in src directory
        pause
        exit /b 1
    )
)

REM Create packages directory
if not exist "packages" mkdir packages

REM Create temporary directory for packaging
set TEMP_DIR=temp_%MODULE_NAME%_%RANDOM%
mkdir %TEMP_DIR%

REM Copy module structure
echo Copying module files...
xcopy "%MODULE_DIR%" "%TEMP_DIR%\%MODULE_NAME%" /E /I /Q

REM Create .cmod package (ZIP format)
echo Creating CMOD package...
cd %TEMP_DIR%
powershell -command "Compress-Archive -Path '%MODULE_NAME%' -DestinationPath '..\packages\%MODULE_NAME%.cmod' -Force"
cd ..

REM Clean up
rmdir /s /q %TEMP_DIR%

echo.
echo CMOD package created successfully!
echo Package: packages\%MODULE_NAME%.cmod
echo.
echo Package contents validate according to CHTL syntax documentation:
echo - Required: src/ directory with CHTL files
echo - Required: info/%MODULE_NAME%.chtl with [Info] block
echo - Structure: %MODULE_NAME%/src/ and %MODULE_NAME%/info/
echo.

pause