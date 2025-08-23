@echo off
REM CHTL Unified Build Script for Windows
REM Builds compiler, packages modules, and creates IDE extension with all components

echo =============================================================
echo CHTL Unified Build Process
echo =============================================================
echo This script will:
echo 1. Build CHTL Compiler (Release)
echo 2. Package available CMOD modules
echo 3. Package available CJMOD modules  
echo 4. Build VSCode extension with integrated components
echo =============================================================
echo.

REM Step 1: Build CHTL Compiler (Release)
echo [Step 1/4] Building CHTL Compiler...
call scripts\windows\build_release.bat
if %errorlevel% neq 0 (
    echo Error: Compiler build failed
    pause
    exit /b 1
)

echo.
echo [Step 2/4] Packaging CMOD modules...

REM Step 2: Auto-package CMOD modules
if exist "module" (
    for /d %%i in ("module\*") do (
        if exist "%%i\src" (
            if exist "%%i\info" (
                echo Packaging CMOD: %%~ni
                call scripts\windows\pack_cmod.bat "%%i"
            )
        )
    )
    echo CMOD packaging completed
) else (
    echo No module directory found, skipping CMOD packaging
)

echo.
echo [Step 3/4] Packaging CJMOD modules...

REM Step 3: Auto-package CJMOD modules from various locations
set CJMOD_FOUND=0

REM Check module directory for CJMOD structures
if exist "module" (
    for /d %%i in ("module\*") do (
        REM Check for C++ files to identify CJMOD
        dir /b "%%i\src\*.cpp" "%%i\src\*.h" >nul 2>&1
        if not errorlevel 1 (
            if exist "%%i\info" (
                echo Packaging CJMOD: %%~ni
                call scripts\windows\pack_cjmod.bat "%%i"
                set CJMOD_FOUND=1
            )
        )
    )
)

REM Check examples directory for CJMOD modules
if exist "examples" (
    for /d %%i in ("examples\*") do (
        dir /b "%%i\src\*.cpp" "%%i\src\*.h" >nul 2>&1
        if not errorlevel 1 (
            if exist "%%i\info" (
                echo Packaging example CJMOD: %%~ni
                call scripts\windows\pack_cjmod.bat "%%i"
                set CJMOD_FOUND=1
            )
        )
    )
)

if %CJMOD_FOUND%==0 (
    echo No CJMOD modules found for packaging
) else (
    echo CJMOD packaging completed
)

echo.
echo [Step 4/4] Building VSCode extension with integrated components...

REM Step 4: Prepare VSCode extension with all components
if not exist "vscode-chtl-extension\resources" mkdir vscode-chtl-extension\resources

REM Copy compiler binaries to extension resources
if exist "dist\windows" (
    echo Copying compiler binaries to extension...
    if not exist "vscode-chtl-extension\resources\compiler" mkdir vscode-chtl-extension\resources\compiler
    xcopy "dist\windows\*" "vscode-chtl-extension\resources\compiler\" /Y /Q
)

REM Copy packaged modules to extension resources
if exist "packages" (
    echo Copying packaged modules to extension...
    if not exist "vscode-chtl-extension\resources\modules" mkdir vscode-chtl-extension\resources\modules
    xcopy "packages\*" "vscode-chtl-extension\resources\modules\" /Y /Q
)

REM Copy official modules (Chtholly and Yuigahama) if they exist
if exist "module\Chtholly" (
    echo Including Chtholly module...
    if not exist "vscode-chtl-extension\resources\official" mkdir vscode-chtl-extension\resources\official
    xcopy "module\Chtholly" "vscode-chtl-extension\resources\official\Chtholly\" /E /I /Y /Q
)

if exist "module\Yuigahama" (
    echo Including Yuigahama module...
    if not exist "vscode-chtl-extension\resources\official" mkdir vscode-chtl-extension\resources\official
    xcopy "module\Yuigahama" "vscode-chtl-extension\resources\official\Yuigahama\" /E /I /Y /Q
)

REM Build the complete VSCode extension
call scripts\windows\build_vscode_extension.bat
if %errorlevel% neq 0 (
    echo Error: VSCode extension build failed
    pause
    exit /b 1
)

echo.
echo =============================================================
echo CHTL Unified Build Completed Successfully!
echo =============================================================
echo.
echo Build Results:
echo - Compiler binaries: dist\windows\
echo - Packaged modules: packages\
echo - VSCode extension: dist\vscode\chtl-extension.vsix
echo.
echo The VSCode extension includes:
echo - Integrated CHTL compiler
echo - All packaged CMOD/CJMOD modules
echo - Official Chtholly and Yuigahama modules (if available)
echo - Complete language support and IDE features
echo.
echo Installation:
echo   code --install-extension dist\vscode\chtl-extension.vsix
echo.
echo =============================================================

pause