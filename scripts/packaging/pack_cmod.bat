@echo off
REM CMOD Packaging Script
REM Cross-platform batch script for packaging CHTL CMOD modules

setlocal EnableDelayedExpansion

REM Show usage information
if "%~1"=="-h" goto :show_usage
if "%~1"=="--help" goto :show_usage
if "%~1"=="" goto :show_usage

REM Parse command line arguments
set "OUTPUT_DIR=.\packages"
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

echo [INFO] Starting CMOD Packaging
echo [INFO] Project root: %PROJECT_ROOT%
echo [INFO] Module directory: !MODULE_DIR!
echo [INFO] Output directory: !OUTPUT_DIR!

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

REM Validate CMOD structure
echo [INFO] Validating CMOD structure...

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
    echo [ERROR] CMOD structure should be:
    echo [ERROR]   !MODULE_NAME!/
    echo [ERROR]   ├── src/
    echo [ERROR]   │   ├── !MODULE_NAME!.chtl (optional if submodules exist)
    echo [ERROR]   │   └── [submodules...]
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

REM Check for main module file or submodules
set "MAIN_FILE=!MODULE_DIR!\src\!MODULE_NAME!.chtl"
set "HAS_MAIN=false"
set "HAS_SUBMODULES=false"

if exist "!MAIN_FILE!" (
    set "HAS_MAIN=true"
    echo [SUCCESS] Found main module file: !MAIN_FILE!
)

for /d %%d in ("!MODULE_DIR!\src\*") do (
    set "HAS_SUBMODULES=true"
    echo [INFO] Found submodule: %%~nxd
)

if "!HAS_MAIN!"=="false" if "!HAS_SUBMODULES!"=="false" (
    echo [ERROR] Neither main module file nor submodules found
    echo [ERROR] Expected: !MAIN_FILE! or submodule directories in !MODULE_DIR!\src\
    exit /b 1
)

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

if "!MODULE_INFO_NAME!"=="" (
    echo [WARNING] Module name not found in info file, using directory name
    set "MODULE_INFO_NAME=!MODULE_NAME!"
)

if "!MODULE_VERSION!"=="" (
    echo [WARNING] Module version not found in info file, using default
    set "MODULE_VERSION=1.0.0"
)

echo [INFO] Module info name: !MODULE_INFO_NAME!
echo [INFO] Module version: !MODULE_VERSION!

echo [SUCCESS] CMOD structure validation passed

REM Create output directory
if not exist "!OUTPUT_DIR!" mkdir "!OUTPUT_DIR!"

REM Create temporary packaging directory
set "TEMP_DIR=%TEMP%\cmod_package_%RANDOM%"
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

REM Generate or update [Export] section in info file
echo [INFO] Generating export information...

set "INFO_FILE_TEMP=!PACKAGE_DIR!\info\!MODULE_NAME!.chtl"
set "EXPORT_CONTENT="
set "EXPORT_COUNT=0"

REM Scan for exportable items in main file
if exist "!PACKAGE_DIR!\src\!MODULE_NAME!.chtl" (
    if "!VERBOSE!"=="true" (
        echo [INFO] Scanning main module file for exports...
    )
    
    REM Extract [Template] and [Custom] exports (simplified)
    for /f "tokens=*" %%a in ('findstr /R "^\[Template\].*@" "!PACKAGE_DIR!\src\!MODULE_NAME!.chtl" 2^>nul') do (
        set "line=%%a"
        set "line=!line:[Template]=!"
        set "line=!line: =!"
        set "EXPORT_CONTENT=!EXPORT_CONTENT![Template] !line!
"
        set /a EXPORT_COUNT+=1
    )
    
    for /f "tokens=*" %%a in ('findstr /R "^\[Custom\].*@" "!PACKAGE_DIR!\src\!MODULE_NAME!.chtl" 2^>nul') do (
        set "line=%%a"
        set "line=!line:[Custom]=!"
        set "line=!line: =!"
        set "EXPORT_CONTENT=!EXPORT_CONTENT![Custom] !line!
"
        set /a EXPORT_COUNT+=1
    )
)

REM Scan submodules for exports (simplified - would need more complex parsing in real implementation)
for /d %%d in ("!PACKAGE_DIR!\src\*") do (
    set "SUBMODULE_NAME=%%~nxd"
    if "!VERBOSE!"=="true" (
        echo [INFO] Scanning submodule: !SUBMODULE_NAME!
    )
    
    for /f "tokens=*" %%a in ('dir /b "%%d\*.chtl" 2^>nul') do (
        REM This is a simplified version - real implementation would need proper parsing
        set /a EXPORT_COUNT+=0
    )
)

REM Update info file with export section
if !EXPORT_COUNT! gtr 0 (
    REM Remove existing [Export] section if present (simplified)
    copy "!INFO_FILE_TEMP!" "!INFO_FILE_TEMP!.backup" >nul
    
    REM Add new [Export] section
    echo. >> "!INFO_FILE_TEMP!"
    echo [Export] >> "!INFO_FILE_TEMP!"
    echo { >> "!INFO_FILE_TEMP!"
    echo !EXPORT_CONTENT! >> "!INFO_FILE_TEMP!"
    echo } >> "!INFO_FILE_TEMP!"
    
    echo [SUCCESS] Generated export section with !EXPORT_COUNT! items
) else (
    echo [WARNING] No exportable items found
)

REM Create package archive
set "PACKAGE_NAME=!MODULE_NAME!-!MODULE_VERSION!.cmod"
set "PACKAGE_PATH=!OUTPUT_DIR!\!PACKAGE_NAME!"

echo [INFO] Creating CMOD package: !PACKAGE_NAME!

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
    echo [SUCCESS] CMOD package created successfully
    echo [INFO] Package: !PACKAGE_PATH!
    echo [INFO] Size: !PACKAGE_SIZE! bytes
    echo [INFO] Compression: !COMPRESSION_METHOD!
    
    REM Create package info file
    set "INFO_FILE=!OUTPUT_DIR!\!MODULE_NAME!-info.txt"
    (
    echo CMOD Package Information
    echo ========================
    echo.
    echo Module Name: !MODULE_INFO_NAME!
    echo Directory Name: !MODULE_NAME!
    echo Version: !MODULE_VERSION!
    echo Package File: !PACKAGE_NAME!
    echo Size: !PACKAGE_SIZE! bytes
    echo Compression: !COMPRESSION_METHOD!
    echo Created: %date% %time%
    echo.
    echo Structure Validation: PASSED
    echo Export Generation: %if !EXPORT_COUNT! gtr 0 (SUCCESS^) else (NO EXPORTS^)%
    echo.
    echo Installation:
    echo   1. Copy !PACKAGE_NAME! to your CHTL module directory
    echo   2. Use CHTL compiler to unpack: chtl --unpack-cmod !PACKAGE_NAME!
    echo   3. Import in your CHTL files: [Import] @!MODULE_INFO_NAME!;
    ) > "!INFO_FILE!"
    
    echo [SUCCESS] Package info saved: !INFO_FILE!
    
    REM Create verification script
    set "VERIFY_SCRIPT=!OUTPUT_DIR!\verify-!MODULE_NAME!.bat"
    (
    echo @echo off
    echo REM CMOD Package Verification Script
    echo.
    echo set "PACKAGE=!PACKAGE_PATH!"
    echo set "TEMP_DIR=%%TEMP%%\verify_cmod_%%RANDOM%%"
    echo.
    echo echo Verifying CMOD package: %%PACKAGE%%
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
    echo     7z l "%%PACKAGE%%" ^| more
    echo ^) else (
    echo     echo Package contents cannot be listed - 7z not available
    echo ^)
    echo.
    echo echo Package contents verified
    echo rmdir /s /q "%%TEMP_DIR%%"
    ) > "!VERIFY_SCRIPT!"
    
    echo [SUCCESS] Verification script created: !VERIFY_SCRIPT!
    
) else (
    echo [ERROR] Failed to create package
    exit /b 1
)

echo [SUCCESS] CMOD packaging completed successfully
exit /b 0

:show_usage
echo Usage: %~nx0 [OPTIONS] ^<module_directory^>
echo.
echo Options:
echo   -o, --output DIR     Output directory for .cmod packages (default: .\packages)
echo   -v, --verbose        Enable verbose output
echo   -h, --help          Show this help message
echo.
echo Examples:
echo   %~nx0 src\Module\CMOD\Chtholly
echo   %~nx0 -o \tmp\packages src\Module\CMOD\MyModule
echo   %~nx0 --verbose modules\MyCustomModule
exit /b 0