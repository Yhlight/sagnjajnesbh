@echo off
REM Unified Packaging Script
REM Cross-platform batch script for packaging all CHTL modules and components

setlocal EnableDelayedExpansion

REM Show usage information
if "%~1"=="-h" goto :show_usage
if "%~1"=="--help" goto :show_usage

REM Parse command line arguments
set "OUTPUT_DIR=.\packages"
set "COMPILE=false"
set "MODULE_SOURCE_DIR=src\Module"
set "VERBOSE=false"
set "CMOD_ONLY=false"
set "CJMOD_ONLY=false"

:parse_args
if "%~1"=="" goto :args_done

if "%~1"=="-o" (
    set "OUTPUT_DIR=%~2"
    shift
    shift
    goto :parse_args
)

if "%~1"=="--output" (
    set "OUTPUT_DIR=%~2"
    shift
    shift
    goto :parse_args
)

if "%~1"=="-c" (
    set "COMPILE=true"
    shift
    goto :parse_args
)

if "%~1"=="--compile" (
    set "COMPILE=true"
    shift
    goto :parse_args
)

if "%~1"=="-m" (
    set "MODULE_SOURCE_DIR=%~2"
    shift
    shift
    goto :parse_args
)

if "%~1"=="--modules" (
    set "MODULE_SOURCE_DIR=%~2"
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

if "%~1"=="--cmod-only" (
    set "CMOD_ONLY=true"
    shift
    goto :parse_args
)

if "%~1"=="--cjmod-only" (
    set "CJMOD_ONLY=true"
    shift
    goto :parse_args
)

shift
goto :parse_args

:args_done

REM Validate conflicting options
if "!CMOD_ONLY!"=="true" if "!CJMOD_ONLY!"=="true" (
    echo [ERROR] Cannot specify both --cmod-only and --cjmod-only
    exit /b 1
)

REM Get script directory and project root
set "SCRIPT_DIR=%~dp0"
set "PROJECT_ROOT=%SCRIPT_DIR%..\..\"

echo [INFO] Starting Unified Packaging Process
echo [INFO] Project root: %PROJECT_ROOT%
echo [INFO] Module source directory: !MODULE_SOURCE_DIR!
echo [INFO] Output directory: !OUTPUT_DIR!
echo [INFO] Compile CJMOD: !COMPILE!
echo [INFO] CMOD only: !CMOD_ONLY!
echo [INFO] CJMOD only: !CJMOD_ONLY!

REM Convert to absolute paths
if not "!MODULE_SOURCE_DIR:~1,1!"==":" (
    set "MODULE_SOURCE_DIR=%PROJECT_ROOT%!MODULE_SOURCE_DIR!"
)

if not "!OUTPUT_DIR:~1,1!"==":" (
    set "OUTPUT_DIR=%PROJECT_ROOT%!OUTPUT_DIR!"
)

REM Check if module source directory exists
if not exist "!MODULE_SOURCE_DIR!" (
    echo [ERROR] Module source directory does not exist: !MODULE_SOURCE_DIR!
    exit /b 1
)

REM Create output directory
if not exist "!OUTPUT_DIR!" mkdir "!OUTPUT_DIR!"

REM Initialize counters
set "CMOD_COUNT=0"
set "CJMOD_COUNT=0"
set "CMOD_SUCCESS=0"
set "CJMOD_SUCCESS=0"
set "CMOD_FAILED=0"
set "CJMOD_FAILED=0"

REM Find CMOD modules
if not "!CJMOD_ONLY!"=="true" (
    echo [INFO] Scanning for CMOD modules...
    
    REM Check for different case variants
    for %%v in (CMOD Cmod cmod) do (
        if exist "!MODULE_SOURCE_DIR!\%%v" (
            for /d %%m in ("!MODULE_SOURCE_DIR!\%%v\*") do (
                if exist "%%m\src" if exist "%%m\info" (
                    set /a CMOD_COUNT+=1
                    if "!VERBOSE!"=="true" (
                        echo [INFO] Found CMOD module: %%~nxm
                    )
                )
            )
        )
    )
)

REM Find CJMOD modules
if not "!CMOD_ONLY!"=="true" (
    echo [INFO] Scanning for CJMOD modules...
    
    REM Check for different case variants
    for %%v in (CJMOD CJmod cjmod) do (
        if exist "!MODULE_SOURCE_DIR!\%%v" (
            for /d %%m in ("!MODULE_SOURCE_DIR!\%%v\*") do (
                if exist "%%m\src" if exist "%%m\info" (
                    REM Check for C++ files
                    set "HAS_CPP=false"
                    for %%f in ("%%m\src\*.cpp") do (
                        if exist "%%f" (
                            set "HAS_CPP=true"
                            goto :cpp_found
                        )
                    )
                    :cpp_found
                    if "!HAS_CPP!"=="true" (
                        set /a CJMOD_COUNT+=1
                        if "!VERBOSE!"=="true" (
                            echo [INFO] Found CJMOD module: %%~nxm
                        )
                    )
                )
            )
        )
    )
)

echo [INFO] Found !CMOD_COUNT! CMOD module(s) and !CJMOD_COUNT! CJMOD module(s)

if !CMOD_COUNT! equ 0 if !CJMOD_COUNT! equ 0 (
    echo [WARNING] No modules found to package
    echo [INFO] Expected structure:
    echo [INFO]   !MODULE_SOURCE_DIR!\
    echo [INFO]   ├── CMOD\ (or Cmod\, cmod\)
    echo [INFO]   │   └── ModuleName\
    echo [INFO]   │       ├── src\
    echo [INFO]   │       └── info\
    echo [INFO]   └── CJMOD\ (or CJmod\, cjmod\)
    echo [INFO]       └── ModuleName\
    echo [INFO]           ├── src\ (with .cpp files)
    echo [INFO]           └── info\
    exit /b 0
)

REM Package CMOD modules
if !CMOD_COUNT! gtr 0 (
    echo [INFO] Packaging CMOD modules...
    
    for %%v in (CMOD Cmod cmod) do (
        if exist "!MODULE_SOURCE_DIR!\%%v" (
            for /d %%m in ("!MODULE_SOURCE_DIR!\%%v\*") do (
                if exist "%%m\src" if exist "%%m\info" (
                    set "MODULE_NAME=%%~nxm"
                    echo [INFO] Packaging CMOD module: !MODULE_NAME!
                    
                    REM Build command arguments
                    set "PACK_ARGS=-o "!OUTPUT_DIR!""
                    if "!VERBOSE!"=="true" (
                        set "PACK_ARGS=!PACK_ARGS! --verbose"
                    )
                    set "PACK_ARGS=!PACK_ARGS! "%%m""
                    
                    REM Run CMOD packaging script
                    call "%SCRIPT_DIR%pack_cmod.bat" !PACK_ARGS!
                    if %errorlevel%==0 (
                        set /a CMOD_SUCCESS+=1
                        echo [SUCCESS] CMOD module packaged: !MODULE_NAME!
                    ) else (
                        set /a CMOD_FAILED+=1
                        echo [ERROR] Failed to package CMOD module: !MODULE_NAME!
                    )
                )
            )
        )
    )
)

REM Package CJMOD modules
if !CJMOD_COUNT! gtr 0 (
    echo [INFO] Packaging CJMOD modules...
    
    for %%v in (CJMOD CJmod cjmod) do (
        if exist "!MODULE_SOURCE_DIR!\%%v" (
            for /d %%m in ("!MODULE_SOURCE_DIR!\%%v\*") do (
                if exist "%%m\src" if exist "%%m\info" (
                    REM Check for C++ files
                    set "HAS_CPP=false"
                    for %%f in ("%%m\src\*.cpp") do (
                        if exist "%%f" (
                            set "HAS_CPP=true"
                            goto :cpp_found2
                        )
                    )
                    :cpp_found2
                    
                    if "!HAS_CPP!"=="true" (
                        set "MODULE_NAME=%%~nxm"
                        echo [INFO] Packaging CJMOD module: !MODULE_NAME!
                        
                        REM Build command arguments
                        set "PACK_ARGS=-o "!OUTPUT_DIR!""
                        if "!COMPILE!"=="true" (
                            set "PACK_ARGS=!PACK_ARGS! --compile"
                        )
                        if "!VERBOSE!"=="true" (
                            set "PACK_ARGS=!PACK_ARGS! --verbose"
                        )
                        set "PACK_ARGS=!PACK_ARGS! "%%m""
                        
                        REM Run CJMOD packaging script
                        call "%SCRIPT_DIR%pack_cjmod.bat" !PACK_ARGS!
                        if %errorlevel%==0 (
                            set /a CJMOD_SUCCESS+=1
                            echo [SUCCESS] CJMOD module packaged: !MODULE_NAME!
                        ) else (
                            set /a CJMOD_FAILED+=1
                            echo [ERROR] Failed to package CJMOD module: !MODULE_NAME!
                        )
                    )
                )
            )
        )
    )
)

REM Generate summary report
set /a TOTAL_SUCCESS=!CMOD_SUCCESS! + !CJMOD_SUCCESS!
set /a TOTAL_FAILED=!CMOD_FAILED! + !CJMOD_FAILED!
set /a TOTAL_MODULES=!CMOD_COUNT! + !CJMOD_COUNT!

echo [INFO] Packaging Summary:
echo [INFO] ==================
echo [INFO] Total modules found: !TOTAL_MODULES!
echo [INFO] CMOD modules: !CMOD_COUNT! (Success: !CMOD_SUCCESS!, Failed: !CMOD_FAILED!)
echo [INFO] CJMOD modules: !CJMOD_COUNT! (Success: !CJMOD_SUCCESS!, Failed: !CJMOD_FAILED!)
echo [INFO] Overall success rate: !TOTAL_SUCCESS!/!TOTAL_MODULES!

REM Create package index
set "PACKAGE_INDEX=!OUTPUT_DIR!\package-index.txt"
(
echo CHTL Module Package Index
echo =========================
echo Generated: %date% %time%
echo.
echo Summary:
echo   Total Packages: !TOTAL_SUCCESS!
echo   CMOD Packages: !CMOD_SUCCESS!
echo   CJMOD Packages: !CJMOD_SUCCESS!
echo   Failed: !TOTAL_FAILED!
echo.
echo Packages:
) > "!PACKAGE_INDEX!"

REM List all package files
for %%f in ("!OUTPUT_DIR!\*.cmod" "!OUTPUT_DIR!\*.cjmod") do (
    if exist "%%f" (
        for %%s in ("%%f") do (
            echo   - %%~nxf (%%~zs bytes) >> "!PACKAGE_INDEX!"
        )
    )
)

echo [SUCCESS] Package index created: !PACKAGE_INDEX!

REM Create installation script
set "INSTALL_SCRIPT=!OUTPUT_DIR!\install-all-modules.bat"
(
echo @echo off
echo REM CHTL Module Installation Script
echo.
echo setlocal EnableDelayedExpansion
echo.
echo set "SCRIPT_DIR=%%~dp0"
echo.
echo echo Installing all CHTL modules...
echo.
echo where chtl_compiler ^>nul 2^>^&1
echo if %%errorlevel%% neq 0 (
echo     echo ERROR: CHTL compiler not found in PATH
echo     echo Please ensure CHTL compiler is installed and available
echo     exit /b 1
echo ^)
echo.
echo set "INSTALLED=0"
echo set "FAILED=0"
echo.
echo for %%%%f in ("%%SCRIPT_DIR%%*.cmod" "%%SCRIPT_DIR%%*.cjmod"^) do (
echo     if exist "%%%%f" (
echo         set "PACKAGE_NAME=%%%%~nxf"
echo         echo Installing: ^^!PACKAGE_NAME^^!
echo         
echo         if "%%%%~xf"==".cmod" (
echo             chtl_compiler --unpack-cmod "%%%%f"
echo             if %%errorlevel%%==0 (
echo                 set /a INSTALLED+=1
echo                 echo SUCCESS: ^^!PACKAGE_NAME^^! installed
echo             ^) else (
echo                 set /a FAILED+=1
echo                 echo ERROR: Failed to install ^^!PACKAGE_NAME^^!
echo             ^)
echo         ^) else if "%%%%~xf"==".cjmod" (
echo             chtl_compiler --unpack-cjmod "%%%%f"
echo             if %%errorlevel%%==0 (
echo                 set /a INSTALLED+=1
echo                 echo SUCCESS: ^^!PACKAGE_NAME^^! installed
echo             ^) else (
echo                 set /a FAILED+=1
echo                 echo ERROR: Failed to install ^^!PACKAGE_NAME^^!
echo             ^)
echo         ^)
echo     ^)
echo ^)
echo.
echo echo Installation complete: ^^!INSTALLED^^! installed, ^^!FAILED^^! failed
) > "!INSTALL_SCRIPT!"

echo [SUCCESS] Installation script created: !INSTALL_SCRIPT!

REM Final status
if !TOTAL_FAILED! equ 0 (
    echo [SUCCESS] All modules packaged successfully!
    exit /b 0
) else (
    echo [WARNING] !TOTAL_FAILED! module(s) failed to package
    exit /b 1
)

:show_usage
echo Usage: %~nx0 [OPTIONS]
echo.
echo Options:
echo   -o, --output DIR     Output directory for packages (default: .\packages)
echo   -c, --compile        Compile C++ sources for CJMOD modules
echo   -m, --modules DIR    Module source directory (default: src\Module)
echo   -v, --verbose        Enable verbose output
echo   --cmod-only         Package only CMOD modules
echo   --cjmod-only        Package only CJMOD modules
echo   -h, --help          Show this help message
echo.
echo Examples:
echo   %~nx0                              # Package all modules
echo   %~nx0 -o \tmp\packages --compile   # Package all with compilation
echo   %~nx0 --cmod-only -v               # Package only CMOD modules verbosely
exit /b 0