@echo off
REM CJMOD Packaging Script
REM Cross-platform batch script for packaging CHTL CJMOD modules

setlocal EnableDelayedExpansion

REM Show usage information
if "%~1"=="-h" goto :show_usage
if "%~1"=="--help" goto :show_usage
if "%~1"=="" goto :show_usage

REM Parse command line arguments
set "OUTPUT_DIR=.\packages"
set "COMPILE=false"
set "VERBOSE=false"
set "MODULE_DIR="

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

if "%~1" NEQ "" (
    if "!MODULE_DIR!"=="" (
        set "MODULE_DIR=%~1"
        shift
        goto :parse_args
    ) else (
        echo [ERROR] Multiple module directories specified
        goto :show_usage
    )
)

:args_done

REM Validate arguments
if "!MODULE_DIR!"=="" (
    echo [ERROR] Module directory not specified
    goto :show_usage
)

REM Get script directory and project root
set "SCRIPT_DIR=%~dp0"
set "PROJECT_ROOT=%SCRIPT_DIR%..\..\"

echo [INFO] Starting CJMOD Packaging
echo [INFO] Project root: %PROJECT_ROOT%
echo [INFO] Module directory: !MODULE_DIR!
echo [INFO] Output directory: !OUTPUT_DIR!
echo [INFO] Compile sources: !COMPILE!

REM Convert to absolute paths
if not "!MODULE_DIR:~1,1!"==":" (
    set "MODULE_DIR=%PROJECT_ROOT%!MODULE_DIR!"
)

if not "!OUTPUT_DIR:~1,1!"==":" (
    set "OUTPUT_DIR=%PROJECT_ROOT%!OUTPUT_DIR!"
)

REM Validate module directory
if not exist "!MODULE_DIR!" (
    echo [ERROR] Module directory does not exist: !MODULE_DIR!
    exit /b 1
)

REM Get module name from directory
for %%f in ("!MODULE_DIR!") do set "MODULE_NAME=%%~nxf"
echo [INFO] Module name: !MODULE_NAME!

REM Validate CJMOD structure
echo [INFO] Validating CJMOD structure...

set "STRUCTURE_VALID=true"

if not exist "!MODULE_DIR!\src" (
    echo [ERROR] Missing required directory: src
    set "STRUCTURE_VALID=false"
)

if not exist "!MODULE_DIR!\info" (
    echo [ERROR] Missing required directory: info
    set "STRUCTURE_VALID=false"
)

if "!STRUCTURE_VALID!"=="false" (
    echo [ERROR] CJMOD structure should be:
    echo [ERROR]   !MODULE_NAME!/
    echo [ERROR]   ├── src/
    echo [ERROR]   │   ├── *.cpp (C++ source files)
    echo [ERROR]   │   ├── *.h (C++ header files)
    echo [ERROR]   │   └── CMakeLists.txt (optional)
    echo [ERROR]   └── info/
    echo [ERROR]       └── !MODULE_NAME!.chtl
    exit /b 1
)

REM Check for info file (must match module name)
set "INFO_FILE=!MODULE_DIR!\info\!MODULE_NAME!.chtl"
if not exist "!INFO_FILE!" (
    echo [ERROR] Info file not found: !INFO_FILE!
    echo [ERROR] The info file must have the same name as the module directory
    exit /b 1
)

REM Check for C++ source files
set "CPP_COUNT=0"
set "H_COUNT=0"

for %%f in ("!MODULE_DIR!\src\*.cpp") do (
    if exist "%%f" (
        set /a CPP_COUNT+=1
        if "!VERBOSE!"=="true" echo [INFO] Found C++ source: %%~nxf
    )
)

for %%f in ("!MODULE_DIR!\src\*.h" "!MODULE_DIR!\src\*.hpp") do (
    if exist "%%f" (
        set /a H_COUNT+=1
        if "!VERBOSE!"=="true" echo [INFO] Found header: %%~nxf
    )
)

if !CPP_COUNT! equ 0 (
    echo [ERROR] No C++ source files found in !MODULE_DIR!\src\
    echo [ERROR] CJMOD modules require C++ implementation files
    exit /b 1
)

echo [SUCCESS] Found !CPP_COUNT! C++ source file(s) and !H_COUNT! header file(s)

REM Validate info file format
echo [INFO] Validating info file format...

findstr /B /C:"[Info]" "!INFO_FILE!" >nul
if %errorlevel% neq 0 (
    echo [ERROR] Info file missing [Info] section: !INFO_FILE!
    exit /b 1
)

REM Extract module info
set "MODULE_INFO_NAME="
set "MODULE_VERSION="
set "MODULE_DESCRIPTION="

for /f "tokens=2 delims==" %%a in ('findstr /C:"name" "!INFO_FILE!" 2^>nul') do (
    set "temp=%%a"
    set "temp=!temp: =!"
    set "temp=!temp:;=!"
    set "temp=!temp:"=!"
    set "temp=!temp:'=!"
    set "MODULE_INFO_NAME=!temp!"
    goto :name_found
)
:name_found

for /f "tokens=2 delims==" %%a in ('findstr /C:"version" "!INFO_FILE!" 2^>nul') do (
    set "temp=%%a"
    set "temp=!temp: =!"
    set "temp=!temp:;=!"
    set "temp=!temp:"=!"
    set "temp=!temp:'=!"
    set "MODULE_VERSION=!temp!"
    goto :version_found
)
:version_found

for /f "tokens=2 delims==" %%a in ('findstr /C:"description" "!INFO_FILE!" 2^>nul') do (
    set "temp=%%a"
    set "temp=!temp:;=!"
    set "temp=!temp:"=!"
    set "temp=!temp:'=!"
    set "MODULE_DESCRIPTION=!temp!"
    goto :description_found
)
:description_found

if "!MODULE_INFO_NAME!"=="" (
    echo [WARNING] Module name not found in info file, using directory name
    set "MODULE_INFO_NAME=!MODULE_NAME!"
)

if "!MODULE_VERSION!"=="" (
    echo [WARNING] Module version not found in info file, using default
    set "MODULE_VERSION=1.0.0"
)

if "!MODULE_DESCRIPTION!"=="" (
    set "MODULE_DESCRIPTION=CJMOD extension module"
)

echo [INFO] Module info name: !MODULE_INFO_NAME!
echo [INFO] Module version: !MODULE_VERSION!
echo [INFO] Module description: !MODULE_DESCRIPTION!

echo [SUCCESS] CJMOD structure validation passed

REM Check for compilation requirements if compile flag is set
if "!COMPILE!"=="true" (
    echo [INFO] Checking compilation requirements...
    
    where cmake >nul 2>&1
    if %errorlevel% neq 0 (
        echo [ERROR] CMake is required for compilation but not found
        exit /b 1
    )
    
    where msbuild >nul 2>&1
    if %errorlevel% neq 0 (
        where mingw32-make >nul 2>&1
        if %errorlevel% neq 0 (
            where make >nul 2>&1
            if %errorlevel% neq 0 (
                echo [ERROR] Build system (MSBuild, MinGW, or Make) is required for compilation but not found
                exit /b 1
            )
        )
    )
    
    REM Check for C++ compiler
    where cl >nul 2>&1
    if %errorlevel% neq 0 (
        where g++ >nul 2>&1
        if %errorlevel% neq 0 (
            where clang++ >nul 2>&1
            if %errorlevel% neq 0 (
                echo [ERROR] C++ compiler (MSVC, g++, or clang++) is required for compilation but not found
                exit /b 1
            )
        )
    )
    
    echo [SUCCESS] Compilation requirements verified
)

REM Create output directory
if not exist "!OUTPUT_DIR!" mkdir "!OUTPUT_DIR!"

REM Create temporary packaging directory
set "TEMP_DIR=%TEMP%\cjmod_package_%RANDOM%"
set "PACKAGE_DIR=!TEMP_DIR!\!MODULE_NAME!"

if "!VERBOSE!"=="true" (
    echo [INFO] Using temporary directory: !TEMP_DIR!
)

REM Copy module structure to temporary directory
echo [INFO] Preparing module for packaging...

mkdir "!PACKAGE_DIR!"
xcopy "!MODULE_DIR!\*" "!PACKAGE_DIR!\" /E /I /Q >nul

if "!VERBOSE!"=="true" (
    echo [INFO] Module structure copied to: !PACKAGE_DIR!
)

REM Compile C++ sources if requested
if "!COMPILE!"=="true" (
    echo [INFO] Compiling C++ sources...
    
    set "BUILD_DIR=!PACKAGE_DIR!\build"
    mkdir "!BUILD_DIR!"
    cd /d "!BUILD_DIR!"
    
    REM Create CMakeLists.txt if it doesn't exist
    if not exist "!PACKAGE_DIR!\src\CMakeLists.txt" (
        echo [INFO] Creating CMakeLists.txt for compilation...
        
        (
        echo cmake_minimum_required(VERSION 3.10^)
        echo project(!MODULE_INFO_NAME!^)
        echo.
        echo set(CMAKE_CXX_STANDARD 17^)
        echo set(CMAKE_CXX_STANDARD_REQUIRED ON^)
        echo.
        echo # Find all source files
        echo file(GLOB_RECURSE SOURCES "*.cpp"^)
        echo file(GLOB_RECURSE HEADERS "*.h" "*.hpp"^)
        echo.
        echo # Create shared library
        echo add_library(${PROJECT_NAME} SHARED ${SOURCES}^)
        echo.
        echo # Set output directory
        echo set_target_properties(${PROJECT_NAME} PROPERTIES
        echo     LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/../lib"
        echo     RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/../lib"
        echo ^)
        echo.
        echo # Include directories
        echo target_include_directories(${PROJECT_NAME} PRIVATE .^)
        echo.
        echo # Install targets
        echo install(TARGETS ${PROJECT_NAME}
        echo     LIBRARY DESTINATION lib
        echo     RUNTIME DESTINATION lib
        echo ^)
        ) > "!PACKAGE_DIR!\src\CMakeLists.txt"
    )
    
    REM Configure and build
    cmake -DCMAKE_BUILD_TYPE=Release "!PACKAGE_DIR!\src"
    
    if %errorlevel% neq 0 (
        echo [ERROR] CMake configuration failed
        rmdir /s /q "!TEMP_DIR!"
        exit /b 1
    )
    
    REM Build using appropriate system
    where msbuild >nul 2>&1
    if %errorlevel%==0 (
        msbuild !MODULE_INFO_NAME!.sln /p:Configuration=Release /m
    ) else (
        where mingw32-make >nul 2>&1
        if %errorlevel%==0 (
            mingw32-make -j%NUMBER_OF_PROCESSORS%
        ) else (
            make -j%NUMBER_OF_PROCESSORS%
        )
    )
    
    if %errorlevel% neq 0 (
        echo [ERROR] Compilation failed
        rmdir /s /q "!TEMP_DIR!"
        exit /b 1
    )
    
    echo [SUCCESS] Compilation completed successfully
    
    REM Clean up build artifacts but keep libraries
    cd /d "!PACKAGE_DIR!"
    if exist "lib" (
        echo [INFO] Compiled libraries available in lib\ directory
        dir lib\
    )
    
    REM Remove build directory to save space
    rmdir /s /q "!BUILD_DIR!"
)

REM Analyze C++ sources for function information (for documentation only)
echo [INFO] Analyzing C++ sources for documentation...

set "EXPORT_COUNT=0"
set "EXPORT_FUNCTIONS="

REM This is for documentation purposes only - CJMOD uses C++ export mechanisms
for %%f in ("!PACKAGE_DIR!\src\*.cpp") do (
    if exist "%%f" (
        for /f "tokens=*" %%l in ('findstr /R "^void.*(" "%%f" 2^>nul') do (
            set "line=%%l"
            REM Extract function name (simplified)
            for /f "tokens=2" %%n in ("!line!") do (
                if not "%%n"=="main" (
                    set "EXPORT_FUNCTIONS=!EXPORT_FUNCTIONS! %%n"
                    set /a EXPORT_COUNT+=1
                )
            )
        )
    )
)

REM Note: CJMOD does NOT use [Export] syntax - it uses C++ export mechanisms
REM The info file should only contain [Info] section for CJMOD modules
echo [INFO] CJMOD modules use C++ export mechanisms, not CHTL [Export] syntax

if !EXPORT_COUNT! gtr 0 (
    echo [SUCCESS] Found !EXPORT_COUNT! C++ function(s) for documentation
) else (
    echo [WARNING] No C++ functions found for documentation
)

REM Create package archive
set "PACKAGE_NAME=!MODULE_NAME!-!MODULE_VERSION!.cjmod"
set "PACKAGE_PATH=!OUTPUT_DIR!\!PACKAGE_NAME!"

echo [INFO] Creating CJMOD package: !PACKAGE_NAME!

cd /d "!TEMP_DIR!"

REM Use available compression tool
set "COMPRESSION_METHOD="
where 7z >nul 2>&1
if %errorlevel%==0 (
    7z a -tzip "!PACKAGE_PATH!" "!MODULE_NAME!" >nul
    set "COMPRESSION_METHOD=7zip"
    goto :compression_done
)

where tar >nul 2>&1
if %errorlevel%==0 (
    tar -czf "!PACKAGE_PATH!" "!MODULE_NAME!"
    set "COMPRESSION_METHOD=tar+gzip"
    goto :compression_done
)

where powershell >nul 2>&1
if %errorlevel%==0 (
    powershell -Command "Compress-Archive -Path '!MODULE_NAME!' -DestinationPath '!PACKAGE_PATH!'" >nul
    set "COMPRESSION_METHOD=powershell"
    goto :compression_done
)

echo [ERROR] No suitable compression tool found (7z, tar, or PowerShell required)
rmdir /s /q "!TEMP_DIR!"
exit /b 1

:compression_done

REM Cleanup temporary directory
rmdir /s /q "!TEMP_DIR!"

REM Verify package was created
if exist "!PACKAGE_PATH!" (
    for %%f in ("!PACKAGE_PATH!") do set "PACKAGE_SIZE=%%~zf"
    echo [SUCCESS] CJMOD package created successfully
    echo [INFO] Package: !PACKAGE_PATH!
    echo [INFO] Size: !PACKAGE_SIZE! bytes
    echo [INFO] Compression: !COMPRESSION_METHOD!
    
    REM Create package info file
    set "INFO_FILE=!OUTPUT_DIR!\!MODULE_NAME!-info.txt"
    (
    echo CJMOD Package Information
    echo =========================
    echo.
    echo Module Name: !MODULE_INFO_NAME!
    echo Directory Name: !MODULE_NAME!
    echo Version: !MODULE_VERSION!
    echo Description: !MODULE_DESCRIPTION!
    echo Package File: !PACKAGE_NAME!
    echo Size: !PACKAGE_SIZE! bytes
    echo Compression: !COMPRESSION_METHOD!
    echo Compiled: %if "!COMPILE!"=="true" (YES^) else (NO^)%
    echo Created: %date% %time%
    echo.
    echo C++ Sources: !CPP_COUNT!
    echo Header Files: !H_COUNT!
    echo Documented Functions: !EXPORT_COUNT!
    echo.
    echo Structure Validation: PASSED
    echo Function Analysis: %if !EXPORT_COUNT! gtr 0 (SUCCESS^) else (NO FUNCTIONS FOUND^)%
    echo.
    echo Installation:
    echo   1. Copy !PACKAGE_NAME! to your CHTL module directory
    echo   2. Use CHTL compiler to unpack: chtl --unpack-cjmod !PACKAGE_NAME!
    echo   3. The module will be compiled and linked automatically
    echo   4. Import in your CHTL files: [Import] @CJmod from !MODULE_INFO_NAME!
    echo.
    echo Note: CJMOD uses C++ export mechanisms (extern "C", __declspec(dllexport)^)
    echo       Functions available: !EXPORT_FUNCTIONS!
    echo.
    echo Requirements:
    echo   - C++ compiler (MSVC, g++, or clang++^)
    echo   - CMake (for building^)
    echo   - CHTL JS runtime with CJMOD support
    ) > "!INFO_FILE!"
    
    echo [SUCCESS] Package info saved: !INFO_FILE!
    
    REM Create verification script
    set "VERIFY_SCRIPT=!OUTPUT_DIR!\verify-!MODULE_NAME!.bat"
    (
    echo @echo off
    echo REM CJMOD Package Verification Script
    echo.
    echo set "PACKAGE=!PACKAGE_PATH!"
    echo set "TEMP_DIR=%%TEMP%%\verify_cjmod_%%RANDOM%%"
    echo.
    echo echo Verifying CJMOD package: %%PACKAGE%%
    echo.
    echo if not exist "%%PACKAGE%%" (
    echo     echo ERROR: Package file not found
    echo     exit /b 1
    echo ^)
    echo.
    echo mkdir "%%TEMP_DIR%%"
    echo cd /d "%%TEMP_DIR%%"
    echo.
    echo where 7z ^>nul 2^>^&1
    echo if %%errorlevel%%==0 (
    echo     echo Package contents:
    echo     7z l "%%PACKAGE%%" ^| more
    echo     echo.
    echo     echo C++ source files:
    echo     7z l "%%PACKAGE%%" ^| findstr "\.cpp"
    echo     echo.
    echo     echo Header files:
    echo     7z l "%%PACKAGE%%" ^| findstr "\.h"
    echo ^) else (
    echo     echo Package contents cannot be listed - 7z not available
    echo ^)
    echo.
    echo echo Package verification completed
    echo rmdir /s /q "%%TEMP_DIR%%"
    ) > "!VERIFY_SCRIPT!"
    
    echo [SUCCESS] Verification script created: !VERIFY_SCRIPT!
    
) else (
    echo [ERROR] Failed to create package
    exit /b 1
)

echo [SUCCESS] CJMOD packaging completed successfully

if !EXPORT_COUNT! gtr 0 (
    echo [INFO] Documented C++ functions: !EXPORT_FUNCTIONS!
    echo [INFO] Note: CJMOD uses C++ export mechanisms, not CHTL [Export] syntax
)

exit /b 0

:show_usage
echo Usage: %~nx0 [OPTIONS] ^<module_directory^>
echo.
echo Options:
echo   -o, --output DIR     Output directory for .cjmod packages (default: .\packages)
echo   -c, --compile        Compile C++ sources before packaging
echo   -v, --verbose        Enable verbose output
echo   -h, --help          Show this help message
echo.
echo Examples:
echo   %~nx0 src\Module\CJMOD\Chtholly
echo   %~nx0 -o \tmp\packages --compile src\Module\CJMOD\MyModule
echo   %~nx0 --verbose modules\MyCustomCJModule
exit /b 0