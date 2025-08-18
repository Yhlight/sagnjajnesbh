@echo off
REM CHTL CJMOD Packaging Script for Windows
REM Cross-platform CJMOD module packaging utility

setlocal enabledelayedexpansion

REM Configuration
set "SCRIPT_DIR=%~dp0"
set "PROJECT_ROOT=%SCRIPT_DIR%..\.."

echo ⚙️ CHTL CJMOD Packaging Script for Windows
echo Project Root: %PROJECT_ROOT%
echo.

REM Check if module path is provided
if "%~1"=="" (
    echo ❌ Module path is required
    echo.
    echo Usage: %0 [module_path] [output_dir]
    echo.
    echo Examples:
    echo   %0 src\Module\Chtholly\CJMOD\Chtholly
    echo   %0 src\Module\MyModule\CJMOD\Component dist
    echo.
    echo CJMOD Structure Requirements:
    echo   ModuleName\
    echo   ├── src\
    echo   │   ├── *.cpp           # C++ source files
    echo   │   └── *.h             # C++ header files
    echo   └── ModuleName.chtl     # Info file (same name as folder)
    echo.
    echo Two-Name Standard: Folder name and info file name must be identical
    echo Note: CJMOD info files do NOT have [Export] sections
    exit /b 1
)

set "MODULE_PATH=%~1"
set "OUTPUT_DIR=%~2"
if "%OUTPUT_DIR%"=="" set "OUTPUT_DIR=.\dist"

REM Get module name
for %%i in ("%MODULE_PATH%") do set "MODULE_NAME=%%~ni"

echo ℹ️ Processing CJMOD module: %MODULE_NAME%
echo ℹ️ Module path: %MODULE_PATH%
echo ℹ️ Output directory: %OUTPUT_DIR%
echo.

REM Validate CJMOD structure
echo === Validating CJMOD Structure ===

if not exist "%MODULE_PATH%" (
    echo ❌ Module directory does not exist: %MODULE_PATH%
    exit /b 1
)

if not exist "%MODULE_PATH%\src" (
    echo ❌ Missing src\ directory
    exit /b 1
) else (
    echo ✅ src\ directory found
    
    REM Check for C++ source files
    dir /b "%MODULE_PATH%\src\*.cpp" >nul 2>&1
    if errorlevel 1 (
        echo ⚠️ No C++ source files (.cpp) found in src\
    ) else (
        for /f %%i in ('dir /b "%MODULE_PATH%\src\*.cpp" 2^>nul ^| find /c /v ""') do set CPP_COUNT=%%i
        echo ✅ Found !CPP_COUNT! C++ source files
    )
    
    dir /b "%MODULE_PATH%\src\*.h" >nul 2>&1
    if errorlevel 1 (
        echo ⚠️ No C++ header files (.h) found in src\
    ) else (
        for /f %%i in ('dir /b "%MODULE_PATH%\src\*.h" 2^>nul ^| find /c /v ""') do set H_COUNT=%%i
        echo ✅ Found !H_COUNT! C++ header files
    )
)

REM Check two-name standard
set "INFO_FILE=%MODULE_PATH%\%MODULE_NAME%.chtl"

if not exist "%INFO_FILE%" (
    echo ❌ Missing info file: %INFO_FILE%
    echo ℹ️ Two-name standard requires: %MODULE_NAME%.chtl
    exit /b 1
) else (
    echo ✅ Info file found: %MODULE_NAME%.chtl
)

REM Validate info file content (CJMOD specific)
findstr /C:"[Info]" "%INFO_FILE%" >nul
if errorlevel 1 (
    echo ❌ Info file missing [Info] section
    exit /b 1
) else (
    echo ✅ Info file contains [Info] section
)

REM CJMOD should NOT have [Export] section
findstr /C:"[Export]" "%INFO_FILE%" >nul
if not errorlevel 1 (
    echo ❌ CJMOD info file should NOT contain [Export] section
    echo ℹ️ CJMOD extends CHTL JS via C++ interfaces, no export needed
    exit /b 1
) else (
    echo ✅ Info file correctly has no [Export] section
)

echo ✅ CJMOD structure validation passed
echo.

REM Create output directory
if not exist "%OUTPUT_DIR%" mkdir "%OUTPUT_DIR%"

REM Package CJMOD
echo === Packaging CJMOD Module ===
set "PACKAGE_NAME=%MODULE_NAME%.cjmod"
set "PACKAGE_PATH=%OUTPUT_DIR%\%PACKAGE_NAME%"

echo ℹ️ Creating CJMOD package: %PACKAGE_NAME%

REM Use PowerShell to create ZIP
powershell -Command "Compress-Archive -Path '%MODULE_PATH%\*' -DestinationPath '%PACKAGE_PATH%' -Force"

if errorlevel 1 (
    echo ❌ Failed to create CJMOD package
    exit /b 1
) else (
    echo ✅ CJMOD package created successfully
)

REM Show package info
for %%i in ("%PACKAGE_PATH%") do set "PACKAGE_SIZE=%%~zi"
echo ℹ️ Package size: !PACKAGE_SIZE! bytes

REM Generate CJMOD manifest
echo === Generating CJMOD Package Manifest ===
set "MANIFEST_PATH=%OUTPUT_DIR%\%MODULE_NAME%.cjmod.manifest"

echo # CHTL CJMOD Package Manifest > "%MANIFEST_PATH%"
echo # Generated on: %date% %time% >> "%MANIFEST_PATH%"
echo. >> "%MANIFEST_PATH%"
echo [Package] >> "%MANIFEST_PATH%"
echo Name: %MODULE_NAME% >> "%MANIFEST_PATH%"
echo Type: CJMOD >> "%MANIFEST_PATH%"
echo Created: %date% %time% >> "%MANIFEST_PATH%"
echo Platform: Windows >> "%MANIFEST_PATH%"
echo SourceIncluded: true >> "%MANIFEST_PATH%"
echo. >> "%MANIFEST_PATH%"
echo [Structure] >> "%MANIFEST_PATH%"
echo Root: %MODULE_NAME%\ >> "%MANIFEST_PATH%"
echo Info: %MODULE_NAME%.chtl >> "%MANIFEST_PATH%"
echo SourceDir: src\ >> "%MANIFEST_PATH%"
echo. >> "%MANIFEST_PATH%"
echo [Validation] >> "%MANIFEST_PATH%"
echo TwoNameStandard: Verified >> "%MANIFEST_PATH%"
echo InfoSection: Present >> "%MANIFEST_PATH%"
echo ExportSection: Absent (CJMOD standard) >> "%MANIFEST_PATH%"
echo. >> "%MANIFEST_PATH%"
echo [CJMOD Features] >> "%MANIFEST_PATH%"
echo CHTLJSExtension: Enabled >> "%MANIFEST_PATH%"
echo OfficialPrefix: chtl:: >> "%MANIFEST_PATH%"
echo CppInterface: Available >> "%MANIFEST_PATH%"
echo GlobalVariables: Supported >> "%MANIFEST_PATH%"

echo ✅ CJMOD manifest created: %MODULE_NAME%.cjmod.manifest

echo.
echo === CJMOD Packaging Complete ===
echo ✅ CJMOD module packaged successfully
echo ℹ️ Package location: %PACKAGE_PATH%
echo ℹ️ Manifest location: %MANIFEST_PATH%
echo.
echo 🎉 CJMOD packaging completed!
echo ℹ️ CJMOD extends CHTL JS functionality via C++ interfaces
echo ℹ️ Supports official chtl:: module prefix

endlocal
exit /b 0