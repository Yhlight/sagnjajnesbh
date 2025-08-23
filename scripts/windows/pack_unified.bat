@echo off
REM CHTL Unified Module Packaging Script for Windows
REM Automatically detects and packages CMOD, CJMOD, or Hybrid modules according to CHTL syntax documentation

if "%~1"=="" (
    echo Usage: pack_unified.bat ^<module_directory^>
    echo.
    echo This script automatically detects module type and packages accordingly:
    echo   - CMOD: Contains src/*.chtl files only
    echo   - CJMOD: Contains src/*.cpp and *.h files only
    echo   - Hybrid CMOD+CJMOD: Contains both CMOD and CJMOD subdirectories
    echo.
    echo Examples:
    echo   pack_unified.bat MyModule         (auto-detect type)
    echo   pack_unified.bat Chtholly         (hybrid module)
    echo.
    echo Module Structure Support:
    echo.
    echo Regular CMOD:
    echo   ModuleName/
    echo     src/ *.chtl files
    echo     info/ ModuleName.chtl
    echo.
    echo Regular CJMOD:
    echo   ModuleName/
    echo     src/ *.cpp *.h files  
    echo     info/ ModuleName.chtl
    echo.
    echo Hybrid CMOD+CJMOD (Mixed):
    echo   ModuleName/
    echo     CMOD/^|Cmod/^|cmod/
    echo       SubModule/
    echo         src/ *.chtl files
    echo         info/ SubModule.chtl
    echo     CJMOD/^|CJmod/^|cjmod/
    echo       SubModule/
    echo         src/ *.cpp *.h files
    echo         info/ SubModule.chtl
    echo.
    pause
    exit /b 1
)

set MODULE_DIR=%~1
set MODULE_NAME=%~1

echo CHTL Unified Module Packaging Tool
echo ==================================
echo Analyzing module: %MODULE_NAME%
echo.

REM Check if module directory exists
if not exist "%MODULE_DIR%" (
    echo Error: Module directory '%MODULE_DIR%' not found
    pause
    exit /b 1
)

REM Detect module type according to CHTL syntax documentation
echo Detecting module structure...

set MODULE_TYPE=UNKNOWN
set HAS_CMOD_SUBDIR=0
set HAS_CJMOD_SUBDIR=0
set HAS_DIRECT_SRC=0
set HAS_CHTL_FILES=0
set HAS_CPP_FILES=0

REM Check for hybrid structure (CMOD + CJMOD subdirectories)
if exist "%MODULE_DIR%\CMOD" set HAS_CMOD_SUBDIR=1
if exist "%MODULE_DIR%\Cmod" set HAS_CMOD_SUBDIR=1  
if exist "%MODULE_DIR%\cmod" set HAS_CMOD_SUBDIR=1

if exist "%MODULE_DIR%\CJMOD" set HAS_CJMOD_SUBDIR=1
if exist "%MODULE_DIR%\CJmod" set HAS_CJMOD_SUBDIR=1
if exist "%MODULE_DIR%\cjmod" set HAS_CJMOD_SUBDIR=1

REM Check for direct src directory
if exist "%MODULE_DIR%\src" (
    set HAS_DIRECT_SRC=1
    
    REM Check content types
    dir /b "%MODULE_DIR%\src\*.chtl" >nul 2>&1
    if %errorlevel%==0 set HAS_CHTL_FILES=1
    
    dir /b "%MODULE_DIR%\src\*.cpp" "%MODULE_DIR%\src\*.h" >nul 2>&1  
    if %errorlevel%==0 set HAS_CPP_FILES=1
)

REM Determine module type based on structure analysis
if %HAS_CMOD_SUBDIR%==1 (
    if %HAS_CJMOD_SUBDIR%==1 (
        set MODULE_TYPE=HYBRID
        echo ^> Detected: Hybrid CMOD+CJMOD module structure
    ) else (
        set MODULE_TYPE=CMOD
        echo ^> Detected: CMOD-only module with subdirectory structure
    )
) else if %HAS_CJMOD_SUBDIR%==1 (
    set MODULE_TYPE=CJMOD  
    echo ^> Detected: CJMOD-only module with subdirectory structure
) else if %HAS_DIRECT_SRC%==1 (
    if %HAS_CHTL_FILES%==1 (
        if %HAS_CPP_FILES%==1 (
            echo Warning: Direct src contains both CHTL and C++ files
            echo This is not recommended. Consider using hybrid structure.
            set MODULE_TYPE=CMOD
            echo ^> Treating as: CMOD module (CHTL files take priority)
        ) else (
            set MODULE_TYPE=CMOD
            echo ^> Detected: Standard CMOD module
        )
    ) else if %HAS_CPP_FILES%==1 (
        set MODULE_TYPE=CJMOD
        echo ^> Detected: Standard CJMOD module
    ) else (
        echo Error: No valid content found in src directory
        pause
        exit /b 1
    )
) else (
    echo Error: No valid module structure detected
    echo Please ensure your module follows CHTL syntax documentation structure
    pause
    exit /b 1
)

echo.
echo Module Type: %MODULE_TYPE%
echo.

REM Package according to detected type
if "%MODULE_TYPE%"=="HYBRID" (
    echo Packaging as Hybrid CMOD+CJMOD module...
    echo Note: Hybrid modules are packaged as .cmod format but contain both types
    call "%~dp0pack_hybrid.bat" "%MODULE_DIR%"
) else if "%MODULE_TYPE%"=="CMOD" (
    echo Packaging as CMOD module...
    call "%~dp0pack_cmod.bat" "%MODULE_DIR%"
) else if "%MODULE_TYPE%"=="CJMOD" (
    echo Packaging as CJMOD module...
    call "%~dp0pack_cjmod.bat" "%MODULE_DIR%"
) else (
    echo Error: Unable to determine packaging method
    pause
    exit /b 1
)

echo.
echo Unified packaging completed!
echo Module %MODULE_NAME% has been packaged as %MODULE_TYPE% type.
echo.

pause