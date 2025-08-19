@echo off
REM Complete CHTL Build Script
REM Integrated build script for compiler, VSCode plugin, and modules

setlocal EnableDelayedExpansion

REM Show usage information
if "%~1"=="-h" goto :show_usage
if "%~1"=="--help" goto :show_usage

REM Parse command line arguments
set "BUILD_COMPILER=true"
set "BUILD_VSCODE=true"
set "BUILD_MODULES=true"
set "BUILD_MODE=debug"
set "COMPILE_CJMOD=false"
set "CLEAN_BUILD=false"
set "PARALLEL_JOBS="
set "VERBOSE=false"

:parse_args
if "%~1"=="" goto :args_done

if "%~1"=="--compiler" (
    set "BUILD_COMPILER=true"
    set "BUILD_VSCODE=false"
    set "BUILD_MODULES=false"
    shift
    goto :parse_args
)

if "%~1"=="--vscode" (
    set "BUILD_COMPILER=false"
    set "BUILD_VSCODE=true"
    set "BUILD_MODULES=false"
    shift
    goto :parse_args
)

if "%~1"=="--modules" (
    set "BUILD_COMPILER=false"
    set "BUILD_VSCODE=false"
    set "BUILD_MODULES=true"
    shift
    goto :parse_args
)

if "%~1"=="--all" (
    set "BUILD_COMPILER=true"
    set "BUILD_VSCODE=true"
    set "BUILD_MODULES=true"
    shift
    goto :parse_args
)

if "%~1"=="--debug" (
    set "BUILD_MODE=debug"
    shift
    goto :parse_args
)

if "%~1"=="--release" (
    set "BUILD_MODE=release"
    shift
    goto :parse_args
)

if "%~1"=="--compile-cjmod" (
    set "COMPILE_CJMOD=true"
    shift
    goto :parse_args
)

if "%~1"=="--clean" (
    set "CLEAN_BUILD=true"
    shift
    goto :parse_args
)

if "%~1"=="--skip-compiler" (
    set "BUILD_COMPILER=false"
    shift
    goto :parse_args
)

if "%~1"=="--skip-vscode" (
    set "BUILD_VSCODE=false"
    shift
    goto :parse_args
)



if "%~1"=="--skip-modules" (
    set "BUILD_MODULES=false"
    shift
    goto :parse_args
)

if "%~1"=="-j" (
    set "PARALLEL_JOBS=%~2"
    shift
    shift
    goto :parse_args
)

if "%~1"=="--jobs" (
    set "PARALLEL_JOBS=%~2"
    shift
    shift
    goto :parse_args
)

if "%~1"=="-v" (
    set "VERBOSE=true"
    shift
    goto :parse_args
)

if "%~1"=="--verbose" (
    set "VERBOSE=true"
    shift
    goto :parse_args
)

shift
goto :parse_args

:args_done

REM Get script directory and project root
set "SCRIPT_DIR=%~dp0"
set "PROJECT_ROOT=%SCRIPT_DIR%..\..\"
REM Normalize path
for %%i in ("%PROJECT_ROOT%") do set "PROJECT_ROOT=%%~fi\"

echo [INFO] Starting Complete CHTL Build Process
echo [INFO] Project root: %PROJECT_ROOT%
echo [INFO] Build mode: !BUILD_MODE!
echo [INFO] Components to build:
echo [INFO]   - Compiler: !BUILD_COMPILER!
echo [INFO]   - VSCode Plugin: !BUILD_VSCODE!
echo [INFO]   - Modules: !BUILD_MODULES!
echo [INFO] Clean build: !CLEAN_BUILD!
echo [INFO] Compile CJMOD: !COMPILE_CJMOD!

cd /d "%PROJECT_ROOT%"

REM Determine parallel jobs
if "!PARALLEL_JOBS!"=="" (
    set "PARALLEL_JOBS=%NUMBER_OF_PROCESSORS%"
)
echo [INFO] Parallel jobs: !PARALLEL_JOBS!

REM Track build results
set "BUILD_RESULTS="
set "BUILD_TIMES="
set "TOTAL_SUCCESS=0"
set "TOTAL_FAILED=0"

REM Clean build directories if requested
if "!CLEAN_BUILD!"=="true" (
    echo [INFO] Cleaning build directories...
    
    if exist "build-debug" rmdir /s /q "build-debug"
    if exist "build-release" rmdir /s /q "build-release"
    if exist "build" rmdir /s /q "build"
    if exist "packages" rmdir /s /q "packages"
    if exist "dist" rmdir /s /q "dist"

    
    if exist "vscode-chtl-extension\node_modules" rmdir /s /q "vscode-chtl-extension\node_modules"
    if exist "vscode-chtl-extension\out" rmdir /s /q "vscode-chtl-extension\out"
    
    REM Clean VSCode extension packages
    del /q "vscode-chtl-extension\*.vsix" >nul 2>&1
    
    echo [SUCCESS] Build directories cleaned
)

REM Note: ANTLR is pre-built and available in external/antlr4-cross-platform/
echo [INFO] Using pre-built ANTLR from external/antlr4-cross-platform/

REM Build compiler if requested
if "!BUILD_COMPILER!"=="true" (
    echo [INFO] Building Compiler...
    set "START_TIME=!TIME!"
    
    if "!BUILD_MODE!"=="debug" (
        set "COMPILER_SCRIPT=%PROJECT_ROOT%scripts\build\build_debug.bat"
    ) else (
        set "COMPILER_SCRIPT=%PROJECT_ROOT%scripts\build\build_release.bat"
    )
    
    if "!VERBOSE!"=="true" (
        echo [INFO] Running: !COMPILER_SCRIPT!
    )
    
    call "!COMPILER_SCRIPT!"
    if %errorlevel%==0 (
        set "BUILD_RESULTS=!BUILD_RESULTS!Compiler:SUCCESS;"
        set /a TOTAL_SUCCESS+=1
        echo [SUCCESS] Compiler build completed
    ) else (
        set "BUILD_RESULTS=!BUILD_RESULTS!Compiler:FAILED;"
        set /a TOTAL_FAILED+=1
        echo [ERROR] Compiler build failed - aborting
        goto :build_summary
    )
)

REM Build VSCode plugin if requested
if "!BUILD_VSCODE!"=="true" (
    echo [INFO] Building VSCode Plugin...
    set "START_TIME=!TIME!"
    
    if "!BUILD_MODE!"=="debug" (
        set "VSCODE_ARGS=development"
    ) else (
        set "VSCODE_ARGS=production"
    )
    
    if "!VERBOSE!"=="true" (
        echo [INFO] Running: %PROJECT_ROOT%scripts\build\build_vscode_plugin.bat !VSCODE_ARGS!
    )
    
    call "%PROJECT_ROOT%scripts\build\build_vscode_plugin.bat" !VSCODE_ARGS!
    if %errorlevel%==0 (
        set "BUILD_RESULTS=!BUILD_RESULTS!VSCode:SUCCESS;"
        set /a TOTAL_SUCCESS+=1
        echo [SUCCESS] VSCode plugin build completed
    ) else (
        set "BUILD_RESULTS=!BUILD_RESULTS!VSCode:FAILED;"
        set /a TOTAL_FAILED+=1
        echo [ERROR] VSCode plugin build failed
        REM Continue as this doesn't affect core functionality
    )
)

REM Package modules if requested
if "!BUILD_MODULES!"=="true" (
    echo [INFO] Packaging Modules...
    set "START_TIME=!TIME!"
    
    set "MODULE_ARGS=-o packages"
    
    if "!COMPILE_CJMOD!"=="true" (
        set "MODULE_ARGS=!MODULE_ARGS! --compile"
    )
    
    if "!VERBOSE!"=="true" (
        set "MODULE_ARGS=!MODULE_ARGS! --verbose"
    )
    
    if "!VERBOSE!"=="true" (
        echo [INFO] Running: %PROJECT_ROOT%scripts\packaging\pack_all.bat !MODULE_ARGS!
    )
    
    call "%PROJECT_ROOT%scripts\packaging\pack_all.bat" !MODULE_ARGS!
    if %errorlevel%==0 (
        set "BUILD_RESULTS=!BUILD_RESULTS!Modules:SUCCESS;"
        set /a TOTAL_SUCCESS+=1
        echo [SUCCESS] Module packaging completed
    ) else (
        set "BUILD_RESULTS=!BUILD_RESULTS!Modules:FAILED;"
        set /a TOTAL_FAILED+=1
        echo [ERROR] Module packaging failed
        REM Continue as modules are optional
    )
)

:build_summary

REM Generate build report
echo [INFO] Generating build report...

set "REPORT_FILE=%PROJECT_ROOT%build-report-%date:~0,4%%date:~5,2%%date:~8,2%-%time:~0,2%%time:~3,2%%time:~6,2%.txt"
set "REPORT_FILE=!REPORT_FILE: =0!"
set "REPORT_FILE=!REPORT_FILE::=!"

(
echo CHTL Complete Build Report
echo ==========================
echo Generated: %date% %time%
echo Build Mode: !BUILD_MODE!
echo Project Root: %PROJECT_ROOT%
echo.
echo Build Configuration:
echo   - ANTLR: !BUILD_ANTLR!
echo   - Compiler: !BUILD_COMPILER!
echo   - VSCode Plugin: !BUILD_VSCODE!
echo   - Modules: !BUILD_MODULES!
echo   - Clean Build: !CLEAN_BUILD!
echo   - Compile CJMOD: !COMPILE_CJMOD!
echo   - Parallel Jobs: !PARALLEL_JOBS!
echo.
echo Build Results:
) > "!REPORT_FILE!"

REM Add results to report
for %%r in (!BUILD_RESULTS!) do (
    set "result=%%r"
    set "result=!result:;= !"
    for /f "tokens=1,2 delims=:" %%a in ("!result!") do (
        echo   - %%a: %%b >> "!REPORT_FILE!"
    )
)

(
echo.
echo Summary:
echo   - Total Components: %if !TOTAL_SUCCESS! gtr 0 (!TOTAL_SUCCESS!^) else (0^)% + %if !TOTAL_FAILED! gtr 0 (!TOTAL_FAILED!^) else (0^)%
echo   - Successful: !TOTAL_SUCCESS!
echo   - Failed: !TOTAL_FAILED!
echo.
echo Build Artifacts:
) >> "!REPORT_FILE!"

REM List build artifacts
if exist "build-!BUILD_MODE!" (
    echo   - Compiler: build-!BUILD_MODE!\ >> "!REPORT_FILE!"
    if exist "build-!BUILD_MODE!\chtl_compiler.exe" (
        for %%f in ("build-!BUILD_MODE!\chtl_compiler.exe") do (
            echo     * chtl_compiler.exe (%%~zf bytes) >> "!REPORT_FILE!"
        )
    )
)

if exist "packages" (
    echo   - VSCode Extension: packages\ >> "!REPORT_FILE!"
    for %%f in ("packages\*.vsix") do (
        if exist "%%f" (
            echo     * %%~nxf (%%~zf bytes) >> "!REPORT_FILE!"
        )
    )
    
    echo   - Module Packages: packages\ >> "!REPORT_FILE!"
    for %%f in ("packages\*.cmod" "packages\*.cjmod") do (
        if exist "%%f" (
            echo     * %%~nxf (%%~zf bytes) >> "!REPORT_FILE!"
        )
    )
)

if exist "dist" (
    echo   - Distribution: dist\ >> "!REPORT_FILE!"
)

echo [SUCCESS] Build report saved: !REPORT_FILE!

REM Create quick install script
set "INSTALL_SCRIPT=%PROJECT_ROOT%quick-install.bat"
(
echo @echo off
echo REM CHTL Quick Installation Script
echo.
echo setlocal EnableDelayedExpansion
echo.
echo set "SCRIPT_DIR=%%~dp0"
echo.
echo echo CHTL Quick Installation
echo echo ======================
echo.
echo REM Install compiler
echo if exist "%%SCRIPT_DIR%%dist" (
echo     echo Installing CHTL compiler...
echo     echo Please manually copy files from dist\bin\ to your PATH
echo ^)
echo.
echo REM Install VSCode extension
echo if exist "%%SCRIPT_DIR%%packages" (
echo     for %%%%f in ("%%SCRIPT_DIR%%packages\*.vsix"^) do (
echo         if exist "%%%%f" (
echo             where code ^>nul 2^>^&1
echo             if %%errorlevel%%==0 (
echo                 echo Installing VSCode extension...
echo                 code --install-extension "%%%%f"
echo                 echo VSCode extension installed
echo             ^) else (
echo                 echo VSCode not found in PATH - please install extension manually
echo                 echo Extension file: %%%%f
echo             ^)
echo             goto :extension_done
echo         ^)
echo     ^)
echo     :extension_done
echo ^)
echo.
echo REM Install modules
echo if exist "%%SCRIPT_DIR%%packages\install-all-modules.bat" (
echo     echo Installing CHTL modules...
echo     call "%%SCRIPT_DIR%%packages\install-all-modules.bat"
echo ^)
echo.
echo echo Installation completed!
echo echo Restart VSCode to activate the extension
) > "!INSTALL_SCRIPT!"

echo [SUCCESS] Quick install script created: !INSTALL_SCRIPT!

REM Final summary
echo [INFO] Build Summary:
echo [INFO] ==============
set /a TOTAL_COMPONENTS=!TOTAL_SUCCESS! + !TOTAL_FAILED!
echo [INFO] Total components: !TOTAL_COMPONENTS!
echo [INFO] Successful: !TOTAL_SUCCESS!
echo [INFO] Failed: !TOTAL_FAILED!

if !TOTAL_FAILED! equ 0 (
    echo [SUCCESS] All components built successfully!
    echo [INFO] Next steps:
    echo [INFO] 1. Run 'quick-install.bat' to install everything
    echo [INFO] 2. Check build-report-*.txt for detailed information
    echo [INFO] 3. Test the compiler with: chtl_compiler --version
    exit /b 0
) else (
    echo [WARNING] !TOTAL_FAILED! component(s) failed to build
    echo [INFO] Check build-report-*.txt for details
    exit /b 1
)

:show_usage
echo Usage: %~nx0 [OPTIONS]
echo.
echo Build Components:
echo   --compiler          Build CHTL compiler (default: enabled)
echo   --vscode            Build VSCode plugin (default: enabled)
echo   --modules           Package all modules (default: enabled)
echo   --all               Build all components (default)
echo.
echo Build Options:
echo   --debug             Build in debug mode (default)
echo   --release           Build in release mode
echo   --compile-cjmod     Compile C++ sources for CJMOD modules
echo   --clean             Clean build directories before building
echo.
echo Control Options:
echo   --skip-compiler     Skip compiler build
echo   --skip-vscode       Skip VSCode plugin build
echo   --skip-modules      Skip module packaging
echo   -j, --jobs N        Number of parallel build jobs (default: auto)
echo   -v, --verbose       Enable verbose output
echo   -h, --help         Show this help message
echo.
echo Examples:
echo   %~nx0                          # Build everything in debug mode
echo   %~nx0 --release --clean        # Clean release build of everything
echo   %~nx0 --compiler --skip-vscode # Build only compiler
echo   %~nx0 --vscode --debug         # Build only VSCode plugin in debug
exit /b 0