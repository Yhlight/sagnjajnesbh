@echo off
REM CHTL Hybrid CMOD+CJMOD Module Packaging Script for Windows
REM Packages hybrid modules with both CMOD and CJMOD components according to CHTL syntax documentation

if "%~1"=="" (
    echo Usage: pack_hybrid.bat ^<hybrid_module_directory^>
    echo.
    echo Example: pack_hybrid.bat Chtholly
    echo.
    echo Hybrid Module Structure Requirements:
    echo   HybridModuleName/
    echo     CMOD/^|Cmod/^|cmod/
    echo       SubModule1/
    echo         src/ *.chtl files
    echo         info/ SubModule1.chtl
    echo       SubModule2/
    echo         src/ *.chtl files  
    echo         info/ SubModule2.chtl
    echo     CJMOD/^|CJmod/^|cjmod/
    echo       SubModule1/
    echo         src/ *.cpp *.h files
    echo         info/ SubModule1.chtl
    echo.
    echo Note: Hybrid modules are packaged as .cmod format but contain both CMOD and CJMOD components
    echo.
    pause
    exit /b 1
)

set HYBRID_DIR=%~1
set HYBRID_NAME=%~1

echo Packaging Hybrid CMOD+CJMOD module: %HYBRID_NAME%

REM Check if hybrid directory exists
if not exist "%HYBRID_DIR%" (
    echo Error: Hybrid module directory '%HYBRID_DIR%' not found
    pause
    exit /b 1
)

REM Validate hybrid structure according to CHTL syntax documentation
echo Validating Hybrid module structure...

set CMOD_DIR=
set CJMOD_DIR=

REM Find CMOD directory (case-insensitive)
if exist "%HYBRID_DIR%\CMOD" set CMOD_DIR=%HYBRID_DIR%\CMOD
if exist "%HYBRID_DIR%\Cmod" set CMOD_DIR=%HYBRID_DIR%\Cmod
if exist "%HYBRID_DIR%\cmod" set CMOD_DIR=%HYBRID_DIR%\cmod

REM Find CJMOD directory (case-insensitive)
if exist "%HYBRID_DIR%\CJMOD" set CJMOD_DIR=%HYBRID_DIR%\CJMOD
if exist "%HYBRID_DIR%\CJmod" set CJMOD_DIR=%HYBRID_DIR%\CJmod
if exist "%HYBRID_DIR%\cjmod" set CJMOD_DIR=%HYBRID_DIR%\cjmod

if "%CMOD_DIR%"=="" (
    echo Error: Missing CMOD directory in hybrid module
    echo Required: %HYBRID_NAME%/CMOD/ or %HYBRID_NAME%/Cmod/ or %HYBRID_NAME%/cmod/
    pause
    exit /b 1
)

if "%CJMOD_DIR%"=="" (
    echo Error: Missing CJMOD directory in hybrid module  
    echo Required: %HYBRID_NAME%/CJMOD/ or %HYBRID_NAME%/CJmod/ or %HYBRID_NAME%/cjmod/
    pause
    exit /b 1
)

echo Found CMOD directory: %CMOD_DIR%
echo Found CJMOD directory: %CJMOD_DIR%

REM Validate CMOD submodules
echo Validating CMOD submodules...
set CMOD_VALID=0
for /d %%i in ("%CMOD_DIR%\*") do (
    echo Checking CMOD submodule: %%~ni
    
    if not exist "%%i\src" (
        echo Error: Missing src directory in CMOD submodule %%~ni
        pause
        exit /b 1
    )
    
    if not exist "%%i\info" (
        echo Error: Missing info directory in CMOD submodule %%~ni
        pause
        exit /b 1
    )
    
    if not exist "%%i\info\%%~ni.chtl" (
        echo Error: Missing info file: %%~ni/info/%%~ni.chtl
        pause
        exit /b 1
    )
    
    dir /b "%%i\src\*.chtl" >nul 2>&1
    if %errorlevel% neq 0 (
        echo Error: No CHTL files found in CMOD submodule %%~ni
        pause
        exit /b 1
    )
    
    set CMOD_VALID=1
    echo CMOD submodule %%~ni validated
)

if %CMOD_VALID%==0 (
    echo Error: No valid CMOD submodules found
    pause
    exit /b 1
)

REM Validate CJMOD submodules
echo Validating CJMOD submodules...
set CJMOD_VALID=0
for /d %%i in ("%CJMOD_DIR%\*") do (
    echo Checking CJMOD submodule: %%~ni
    
    if not exist "%%i\src" (
        echo Error: Missing src directory in CJMOD submodule %%~ni
        pause
        exit /b 1
    )
    
    if not exist "%%i\info" (
        echo Error: Missing info directory in CJMOD submodule %%~ni
        pause
        exit /b 1
    )
    
    if not exist "%%i\info\%%~ni.chtl" (
        echo Error: Missing info file: %%~ni/info/%%~ni.chtl
        pause
        exit /b 1
    )
    
    dir /b "%%i\src\*.cpp" "%%i\src\*.h" >nul 2>&1
    if %errorlevel% neq 0 (
        echo Error: No C++ files found in CJMOD submodule %%~ni
        pause
        exit /b 1
    )
    
    set CJMOD_VALID=1
    echo CJMOD submodule %%~ni validated
)

if %CJMOD_VALID%==0 (
    echo Error: No valid CJMOD submodules found
    pause
    exit /b 1
)

REM Create packages directory
if not exist "packages" mkdir packages

REM Create temporary directory for packaging
set TEMP_DIR=temp_%HYBRID_NAME%_%RANDOM%
mkdir %TEMP_DIR%

REM Copy hybrid module structure
echo Copying hybrid module files...
xcopy "%HYBRID_DIR%" "%TEMP_DIR%\%HYBRID_NAME%" /E /I /Q

REM Create .cmod package (hybrid modules use .cmod format)
echo Creating Hybrid CMOD+CJMOD package...
cd %TEMP_DIR%
powershell -command "Compress-Archive -Path '%HYBRID_NAME%' -DestinationPath '..\packages\%HYBRID_NAME%.cmod' -Force"
cd ..

REM Clean up
rmdir /s /q %TEMP_DIR%

echo.
echo Hybrid CMOD+CJMOD package created successfully!
echo Package: packages\%HYBRID_NAME%.cmod
echo.
echo Package contents validate according to CHTL syntax documentation:
echo - Contains both CMOD and CJMOD components in separate subdirectories
echo - CMOD: Provides reusable CHTL components
echo - CJMOD: Extends CHTL JS syntax through C++ implementation
echo - Imported separately: [Import] @Chtl for CMOD, [Import] @CJmod for CJMOD
echo - Not recommended to use @Chtl for both CMOD and CJMOD simultaneously
echo.

pause