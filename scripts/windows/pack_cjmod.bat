@echo off
REM CHTL CJMOD Module Packaging Script for Windows
REM Packages CJMOD modules according to CHTL syntax documentation

if "%~1"=="" (
    echo Usage: pack_cjmod.bat ^<cjmod_directory^>
    echo.
    echo Example: pack_cjmod.bat ChthollyExtension
    echo.
    echo CJMOD Structure Requirements:
    echo   CJmodName/
    echo     src/
    echo       xxx.cpp xxx.h    ^(C++ source files^)
    echo     info/
    echo       CJmodName.chtl   ^(module information^)
    echo.
    echo For submodules:
    echo   CJmodName/
    echo     SubModule1/
    echo       src/ xxx.cpp xxx.h
    echo       info/ SubModule1.chtl
    echo     SubModule2/
    echo       src/ xxx.cpp xxx.h
    echo       info/ SubModule2.chtl
    echo.
    pause
    exit /b 1
)

set CJMOD_DIR=%~1
set CJMOD_NAME=%~1

echo Packaging CJMOD module: %CJMOD_NAME%

REM Check if CJMOD directory exists
if not exist "%CJMOD_DIR%" (
    echo Error: CJMOD directory '%CJMOD_DIR%' not found
    pause
    exit /b 1
)

REM Validate CJMOD structure according to CHTL syntax documentation
echo Validating CJMOD structure...

REM Check for main module structure
if exist "%CJMOD_DIR%\src" (
    echo Found main CJMOD structure
    
    if not exist "%CJMOD_DIR%\info" (
        echo Error: Missing 'info' directory in CJMOD
        echo Required CJMOD structure: %CJMOD_NAME%/info/
        pause
        exit /b 1
    )
    
    if not exist "%CJMOD_DIR%\info\%CJMOD_NAME%.chtl" (
        echo Error: Missing CJMOD info file: %CJMOD_NAME%/info/%CJMOD_NAME%.chtl
        echo This file must contain [Info] configuration block
        pause
        exit /b 1
    )
    
    REM Check for C++ source files
    dir /b "%CJMOD_DIR%\src\*.cpp" "%CJMOD_DIR%\src\*.h" >nul 2>&1
    if %errorlevel% neq 0 (
        echo Error: No C++ source files found in src directory
        echo CJMOD requires .cpp and .h files for C++ implementation
        pause
        exit /b 1
    )
    
    echo Main CJMOD structure validated
) else (
    echo No main src directory found, checking for submodule structure...
    
    REM Check for submodule structure
    set SUBMODULE_FOUND=0
    for /d %%i in ("%CJMOD_DIR%\*") do (
        if exist "%%i\src" (
            set SUBMODULE_FOUND=1
            echo Found submodule: %%~ni
            
            if not exist "%%i\info" (
                echo Error: Missing 'info' directory in submodule %%~ni
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
                echo Error: No C++ source files in submodule %%~ni
                pause
                exit /b 1
            )
        )
    )
    
    if %SUBMODULE_FOUND%==0 (
        echo Error: No valid CJMOD structure found
        echo Please ensure proper CJMOD structure according to CHTL documentation
        pause
        exit /b 1
    )
)

REM Create packages directory
if not exist "packages" mkdir packages

REM Create temporary directory for packaging
set TEMP_DIR=temp_%CJMOD_NAME%_%RANDOM%
mkdir %TEMP_DIR%

REM Copy CJMOD structure
echo Copying CJMOD files...
xcopy "%CJMOD_DIR%" "%TEMP_DIR%\%CJMOD_NAME%" /E /I /Q

REM Create .cjmod package (ZIP format)
echo Creating CJMOD package...
cd %TEMP_DIR%
powershell -command "Compress-Archive -Path '%CJMOD_NAME%' -DestinationPath '..\packages\%CJMOD_NAME%.cjmod' -Force"
cd ..

REM Clean up
rmdir /s /q %TEMP_DIR%

echo.
echo CJMOD package created successfully!
echo Package: packages\%CJMOD_NAME%.cjmod
echo.
echo Package contents validate according to CHTL syntax documentation:
echo - Required: C++ source files ^(.cpp, .h^) for CHTL JS syntax extension
echo - Required: info/ModuleName.chtl with [Info] block
echo - Purpose: Extends CHTL JS syntax through C++ implementation
echo.

pause