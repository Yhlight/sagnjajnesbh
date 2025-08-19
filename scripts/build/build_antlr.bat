@echo off
REM ANTLR4 Build Script for CHTL Compiler
REM Downloads, builds, and integrates ANTLR4 C++ runtime

setlocal EnableDelayedExpansion

REM Get script directory and project root
set "SCRIPT_DIR=%~dp0"
set "PROJECT_ROOT=%SCRIPT_DIR%..\..\"

echo [INFO] Starting ANTLR4 Build and Integration
echo [INFO] Project root: %PROJECT_ROOT%

cd /d "%PROJECT_ROOT%"

REM Configuration
set "ANTLR_VERSION=4.13.1"
set "ANTLR_URL=https://github.com/antlr/antlr4/archive/%ANTLR_VERSION%.tar.gz"
set "ANTLR_JAR_URL=https://www.antlr.org/download/antlr-%ANTLR_VERSION%-complete.jar"
set "EXTERNAL_DIR=%PROJECT_ROOT%external"
set "ANTLR_DIR=%EXTERNAL_DIR%\antlr4"
set "ANTLR_BUILD_DIR=%ANTLR_DIR%\build"
set "ANTLR_INSTALL_DIR=%EXTERNAL_DIR%\antlr4-install"

REM Check for required tools
echo [INFO] Checking build dependencies...

where cmake >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] CMake is not installed or not in PATH
    exit /b 1
)

REM Try to setup Visual Studio environment if not already setup
if not defined VSINSTALLDIR (
    echo [INFO] Visual Studio environment not detected, attempting setup...
    if exist "%SCRIPT_DIR%..\integration\setup_windows_env.bat" (
        call "%SCRIPT_DIR%..\integration\setup_windows_env.bat"
        if %errorlevel% neq 0 (
            echo [WARNING] Could not setup Visual Studio environment automatically
        )
    )
)

REM Check for Visual Studio or MinGW
set "BUILD_SYSTEM="

REM First check for MSBuild (Visual Studio Build Tools)
where msbuild >nul 2>&1
if %errorlevel%==0 (
    set "BUILD_SYSTEM=msbuild"
    echo [INFO] Found MSBuild
    goto :build_system_found
)

REM Check for Visual Studio Developer Command Prompt
if defined VSINSTALLDIR (
    echo [INFO] Visual Studio environment detected: %VSINSTALLDIR%
    set "BUILD_SYSTEM=msbuild"
    goto :build_system_found
)

REM Check for cl.exe (MSVC compiler)
where cl >nul 2>&1
if %errorlevel%==0 (
    echo [INFO] Found MSVC compiler, attempting to find MSBuild...
    REM Try to find MSBuild in common locations
    for %%p in (
        "%ProgramFiles%\Microsoft Visual Studio\2022\*\MSBuild\Current\Bin\MSBuild.exe"
        "%ProgramFiles%\Microsoft Visual Studio\2019\*\MSBuild\Current\Bin\MSBuild.exe"
        "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\*\MSBuild\Current\Bin\MSBuild.exe"
        "%ProgramFiles(x86)%\MSBuild\*\Bin\MSBuild.exe"
    ) do (
        if exist "%%p" (
            set "BUILD_SYSTEM=msbuild"
            echo [INFO] Found MSBuild at: %%p
            goto :build_system_found
        )
    )
)

REM Check for MinGW
where mingw32-make >nul 2>&1
if %errorlevel%==0 (
    set "BUILD_SYSTEM=mingw"
    echo [INFO] Found MinGW
    goto :build_system_found
)

REM Check for MSYS2/Cygwin Make
where make >nul 2>&1
if %errorlevel%==0 (
    set "BUILD_SYSTEM=make"
    echo [INFO] Found Make
    goto :build_system_found
)

REM Check for Ninja
where ninja >nul 2>&1
if %errorlevel%==0 (
    set "BUILD_SYSTEM=ninja"
    echo [INFO] Found Ninja
    goto :build_system_found
)

REM If nothing found, provide helpful error message
echo [ERROR] No suitable build system found
echo [INFO] Please install one of the following:
echo [INFO] 1. Visual Studio 2019/2022 with C++ workload
echo [INFO] 2. Visual Studio Build Tools
echo [INFO] 3. MinGW-w64
echo [INFO] 4. MSYS2 with development tools
echo [INFO] 
echo [INFO] For Visual Studio, ensure you're running from:
echo [INFO] - Developer Command Prompt for VS
echo [INFO] - Developer PowerShell for VS
echo [INFO] - Or run vcvarsall.bat first
exit /b 1

:build_system_found

REM Check for download tool
set "DOWNLOAD_TOOL="
where curl >nul 2>&1
if %errorlevel%==0 (
    set "DOWNLOAD_TOOL=curl"
) else (
    where powershell >nul 2>&1
    if %errorlevel%==0 (
        set "DOWNLOAD_TOOL=powershell"
    ) else (
        echo [ERROR] No suitable download tool found (curl or PowerShell)
        exit /b 1
    )
)

REM Check for Java (required for ANTLR JAR)
set "JAVA_AVAILABLE=false"
where java >nul 2>&1
if %errorlevel%==0 (
    set "JAVA_AVAILABLE=true"
    for /f "tokens=3" %%i in ('java -version 2^>^&1 ^| findstr /i version') do (
        set "JAVA_VERSION=%%i"
        set "JAVA_VERSION=!JAVA_VERSION:"=!"
        echo [INFO] Java version: !JAVA_VERSION!
    )
) else (
    echo [WARNING] Java not found - ANTLR JAR will not be available for grammar generation
)

echo [SUCCESS] Build dependencies verified
echo [INFO] Using build system: !BUILD_SYSTEM!
echo [INFO] Using download tool: !DOWNLOAD_TOOL!

REM Create external directory
echo [INFO] Creating external directory: %EXTERNAL_DIR%
if not exist "%EXTERNAL_DIR%" mkdir "%EXTERNAL_DIR%"
cd /d "%EXTERNAL_DIR%"

REM Download ANTLR4 source if not exists
if not exist "%ANTLR_DIR%" (
    echo [INFO] Downloading ANTLR4 v%ANTLR_VERSION%...
    
    if "!DOWNLOAD_TOOL!"=="curl" (
        curl -L -o "antlr4-%ANTLR_VERSION%.tar.gz" "%ANTLR_URL%"
    ) else (
        powershell -Command "Invoke-WebRequest -Uri '%ANTLR_URL%' -OutFile 'antlr4-%ANTLR_VERSION%.tar.gz'"
    )
    
    if %errorlevel% neq 0 (
        echo [ERROR] Failed to download ANTLR4 source
        exit /b 1
    )
    
    echo [INFO] Extracting ANTLR4 source...
    REM Use PowerShell to extract tar.gz on Windows
    powershell -Command "Add-Type -AssemblyName System.IO.Compression.FileSystem; [System.IO.Compression.ZipFile]::ExtractToDirectory('antlr4-%ANTLR_VERSION%.tar.gz', '.')" 2>nul || (
        REM Fallback: try with 7zip if available
        where 7z >nul 2>&1
        if %errorlevel%==0 (
            7z x "antlr4-%ANTLR_VERSION%.tar.gz" -so | 7z x -si -ttar
            ren "antlr4-%ANTLR_VERSION%" "antlr4"
        ) else (
            echo [ERROR] Cannot extract tar.gz file. Please install 7-Zip or use WSL.
            exit /b 1
        )
    )
    
    del "antlr4-%ANTLR_VERSION%.tar.gz"
    echo [SUCCESS] ANTLR4 source downloaded and extracted
) else (
    echo [INFO] ANTLR4 source already exists, skipping download
)

REM Download ANTLR4 JAR if Java is available
if "!JAVA_AVAILABLE!"=="true" (
    set "ANTLR_JAR=%EXTERNAL_DIR%\antlr-%ANTLR_VERSION%-complete.jar"
    if not exist "!ANTLR_JAR!" (
        echo [INFO] Downloading ANTLR4 JAR...
        
        if "!DOWNLOAD_TOOL!"=="curl" (
            curl -L -o "!ANTLR_JAR!" "%ANTLR_JAR_URL%"
        ) else (
            powershell -Command "Invoke-WebRequest -Uri '%ANTLR_JAR_URL%' -OutFile '!ANTLR_JAR!'"
        )
        
        if %errorlevel%==0 (
            echo [SUCCESS] ANTLR4 JAR downloaded
        ) else (
            echo [WARNING] Failed to download ANTLR4 JAR
        )
    ) else (
        echo [INFO] ANTLR4 JAR already exists
    )
)

REM Build ANTLR4 C++ runtime
echo [INFO] Building ANTLR4 C++ runtime...

REM Clean previous build
if exist "%ANTLR_BUILD_DIR%" (
    echo [INFO] Cleaning previous ANTLR4 build...
    rmdir /s /q "%ANTLR_BUILD_DIR%"
)

if exist "%ANTLR_INSTALL_DIR%" (
    echo [INFO] Cleaning previous ANTLR4 installation...
    rmdir /s /q "%ANTLR_INSTALL_DIR%"
)

mkdir "%ANTLR_BUILD_DIR%"
mkdir "%ANTLR_INSTALL_DIR%"

cd /d "%ANTLR_BUILD_DIR%"

REM Configure ANTLR4 C++ runtime
echo [INFO] Configuring ANTLR4 C++ runtime...

if "!BUILD_SYSTEM!"=="msbuild" (
    cmake -DCMAKE_BUILD_TYPE=Release ^
          -DCMAKE_INSTALL_PREFIX="%ANTLR_INSTALL_DIR%" ^
          -DANTLR_BUILD_CPP_TESTS=OFF ^
          -DANTLR_BUILD_SHARED=ON ^
          -DANTLR_BUILD_STATIC=ON ^
          "%ANTLR_DIR%\runtime\Cpp"
) else if "!BUILD_SYSTEM!"=="mingw" (
    cmake -G "MinGW Makefiles" ^
          -DCMAKE_BUILD_TYPE=Release ^
          -DCMAKE_INSTALL_PREFIX="%ANTLR_INSTALL_DIR%" ^
          -DANTLR_BUILD_CPP_TESTS=OFF ^
          -DANTLR_BUILD_SHARED=ON ^
          -DANTLR_BUILD_STATIC=ON ^
          "%ANTLR_DIR%\runtime\Cpp"
) else (
    cmake -G "Unix Makefiles" ^
          -DCMAKE_BUILD_TYPE=Release ^
          -DCMAKE_INSTALL_PREFIX="%ANTLR_INSTALL_DIR%" ^
          -DANTLR_BUILD_CPP_TESTS=OFF ^
          -DANTLR_BUILD_SHARED=ON ^
          -DANTLR_BUILD_STATIC=ON ^
          "%ANTLR_DIR%\runtime\Cpp"
)

if %errorlevel% neq 0 (
    echo [ERROR] ANTLR4 CMake configuration failed
    exit /b 1
)

echo [SUCCESS] ANTLR4 CMake configuration completed

REM Build ANTLR4 C++ runtime
echo [INFO] Building ANTLR4 C++ runtime...

if "!BUILD_SYSTEM!"=="msbuild" (
    msbuild antlr4-runtime.sln /p:Configuration=Release /m
) else if "!BUILD_SYSTEM!"=="mingw" (
    mingw32-make -j%NUMBER_OF_PROCESSORS%
) else (
    make -j%NUMBER_OF_PROCESSORS%
)

if %errorlevel% neq 0 (
    echo [ERROR] ANTLR4 build failed
    exit /b 1
)

echo [SUCCESS] ANTLR4 C++ runtime build completed

REM Install ANTLR4 C++ runtime
echo [INFO] Installing ANTLR4 C++ runtime...

if "!BUILD_SYSTEM!"=="msbuild" (
    msbuild INSTALL.vcxproj /p:Configuration=Release
) else if "!BUILD_SYSTEM!"=="mingw" (
    mingw32-make install
) else (
    make install
)

if %errorlevel% neq 0 (
    echo [ERROR] ANTLR4 installation failed
    exit /b 1
)

echo [SUCCESS] ANTLR4 C++ runtime installed to: %ANTLR_INSTALL_DIR%

REM Create ANTLR4 configuration for CHTL project
echo [INFO] Creating ANTLR4 configuration for CHTL project...

set "ANTLR_CONFIG_FILE=%PROJECT_ROOT%cmake\FindANTLR4.cmake"
if not exist "%PROJECT_ROOT%cmake" mkdir "%PROJECT_ROOT%cmake"

(
echo # FindANTLR4.cmake - Find ANTLR4 C++ runtime
echo # This module defines:
echo #  ANTLR4_FOUND - True if ANTLR4 is found
echo #  ANTLR4_INCLUDE_DIRS - Include directories for ANTLR4
echo #  ANTLR4_LIBRARIES - Libraries to link against
echo #  ANTLR4_JAR - Path to ANTLR4 JAR file (if available^)
echo.
echo set(ANTLR4_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../external/antlr4-install"^)
echo.
echo # Find include directory
echo find_path(ANTLR4_INCLUDE_DIR
echo     NAMES antlr4-runtime.h
echo     PATHS ${ANTLR4_ROOT_DIR}/include
echo     PATH_SUFFIXES antlr4-runtime
echo     NO_DEFAULT_PATH
echo ^)
echo.
echo # Find library
echo find_library(ANTLR4_LIBRARY
echo     NAMES antlr4-runtime antlr4-runtime-static
echo     PATHS ${ANTLR4_ROOT_DIR}/lib ${ANTLR4_ROOT_DIR}/lib64
echo     NO_DEFAULT_PATH
echo ^)
echo.
echo # Find JAR file
echo find_file(ANTLR4_JAR
echo     NAMES antlr-*-complete.jar
echo     PATHS ${CMAKE_CURRENT_LIST_DIR}/../external
echo     NO_DEFAULT_PATH
echo ^)
echo.
echo # Handle standard arguments
echo include(FindPackageHandleStandardArgs^)
echo find_package_handle_standard_args(ANTLR4
echo     REQUIRED_VARS ANTLR4_LIBRARY ANTLR4_INCLUDE_DIR
echo     VERSION_VAR ANTLR4_VERSION
echo ^)
echo.
echo if(ANTLR4_FOUND^)
echo     set(ANTLR4_INCLUDE_DIRS ${ANTLR4_INCLUDE_DIR}^)
echo     set(ANTLR4_LIBRARIES ${ANTLR4_LIBRARY}^)
echo     
echo     # Create imported target
echo     if(NOT TARGET antlr4-runtime^)
echo         add_library(antlr4-runtime UNKNOWN IMPORTED^)
echo         set_target_properties(antlr4-runtime PROPERTIES
echo             IMPORTED_LOCATION ${ANTLR4_LIBRARY}
echo             INTERFACE_INCLUDE_DIRECTORIES ${ANTLR4_INCLUDE_DIR}
echo         ^)
echo     endif(^)
echo endif(^)
echo.
echo mark_as_advanced(ANTLR4_INCLUDE_DIR ANTLR4_LIBRARY ANTLR4_JAR^)
) > "%ANTLR_CONFIG_FILE%"

echo [SUCCESS] Created ANTLR4 CMake configuration

REM Create grammar directory and example grammars
set "GRAMMAR_DIR=%PROJECT_ROOT%grammars"
if not exist "%GRAMMAR_DIR%" mkdir "%GRAMMAR_DIR%"

REM Create example CSS grammar
(
echo grammar CSS;
echo.
echo // CSS Grammar for CHTL Compiler
echo stylesheet
echo     : (charset ^| imports ^| rule^)*
echo     ;
echo.
echo charset
echo     : CHARSET_SYM STRING SEMICOLON
echo     ;
echo.
echo imports
echo     : IMPORT_SYM (STRING ^| URI^) mediaList? SEMICOLON
echo     ;
echo.
echo rule
echo     : selector (COMMA selector^)* LBRACE declaration* RBRACE
echo     ;
echo.
echo selector
echo     : simpleSelector (combinator simpleSelector^)*
echo     ;
echo.
echo simpleSelector
echo     : (HASH ^| DOT IDENT ^| IDENT ^| STAR^) (DOT IDENT ^| HASH ^| LBRACKET IDENT (EQUALS ^| CONTAINS ^| BEGINS ^| ENDS^)? STRING? RBRACKET^)*
echo     ;
echo.
echo combinator
echo     : GREATER ^| PLUS ^| TILDE ^| WS
echo     ;
echo.
echo declaration
echo     : property COLON value (IMPORTANT_SYM^)? SEMICOLON?
echo     ;
echo.
echo property
echo     : IDENT
echo     ;
echo.
echo value
echo     : (IDENT ^| STRING ^| NUMBER ^| HASH ^| URI ^| FUNCTION^)*
echo     ;
echo.
echo mediaList
echo     : IDENT (COMMA IDENT^)*
echo     ;
echo.
echo // Lexer rules
echo CHARSET_SYM : '@charset' ;
echo IMPORT_SYM  : '@import' ;
echo IMPORTANT_SYM : '!important' ;
echo.
echo LBRACE      : '{' ;
echo RBRACE      : '}' ;
echo LBRACKET    : '[' ;
echo RBRACKET    : ']' ;
echo LPAREN      : '(' ;
echo RPAREN      : '^)' ;
echo SEMICOLON   : ';' ;
echo COLON       : ':' ;
echo COMMA       : ',' ;
echo DOT         : '.' ;
echo HASH        : '#' [a-fA-F0-9]+ ;
echo STAR        : '*' ;
echo GREATER     : '^>' ;
echo PLUS        : '+' ;
echo TILDE       : '~' ;
echo EQUALS      : '=' ;
echo CONTAINS    : '*=' ;
echo BEGINS      : '^=' ;
echo ENDS        : '$=' ;
echo.
echo IDENT       : [a-zA-Z_-] [a-zA-Z0-9_-]* ;
echo STRING      : '"' (~["\r\n]^)* '"' ^| '\'' (~['\r\n]^)* '\'' ;
echo NUMBER      : [0-9]+ ('.' [0-9]+^)? [a-zA-Z%%]* ;
echo URI         : 'url(^)' (~[]^)^)* '^)' ;
echo FUNCTION    : IDENT '(^)' ;
echo.
echo WS          : [ \t\r\n]+ -^> skip ;
echo COMMENT     : '/*' .*? '*/' -^> skip ;
) > "%GRAMMAR_DIR%\CSS.g4"

echo [SUCCESS] Created example CSS grammar

REM Create helper script for grammar generation
if "!JAVA_AVAILABLE!"=="true" if exist "!ANTLR_JAR!" (
    set "GRAMMAR_SCRIPT=%PROJECT_ROOT%scripts\build\generate_grammars.bat"
    (
    echo @echo off
    echo REM Grammar Generation Script for CHTL Compiler
    echo.
    echo setlocal EnableDelayedExpansion
    echo.
    echo set "SCRIPT_DIR=%%~dp0"
    echo set "PROJECT_ROOT=%%SCRIPT_DIR%%..\..\\"
    echo set "ANTLR_JAR=!ANTLR_JAR!"
    echo set "GRAMMAR_DIR=%%PROJECT_ROOT%%grammars"
    echo set "OUTPUT_DIR=%%PROJECT_ROOT%%src\Generated"
    echo.
    echo echo [INFO] Generating C++ code from ANTLR grammars...
    echo.
    echo if not exist "%%OUTPUT_DIR%%" mkdir "%%OUTPUT_DIR%%"
    echo.
    echo for %%%%g in ("%%GRAMMAR_DIR%%\*.g4"^) do (
    echo     if exist "%%%%g" (
    echo         echo [INFO] Processing: %%%%~nxg
    echo         java -jar "%%ANTLR_JAR%%" -Dlanguage=Cpp -o "%%OUTPUT_DIR%%" "%%%%g"
    echo     ^)
    echo ^)
    echo.
    echo echo [SUCCESS] Grammar generation completed
    ) > "!GRAMMAR_SCRIPT!"
    
    echo [SUCCESS] Created grammar generation script: !GRAMMAR_SCRIPT!
)

echo [SUCCESS] ANTLR4 build and integration completed successfully
echo [INFO] ANTLR4 installation directory: %ANTLR_INSTALL_DIR%
echo [INFO] ANTLR4 CMake configuration: %ANTLR_CONFIG_FILE%
echo [INFO] Grammar directory: %GRAMMAR_DIR%

if "!JAVA_AVAILABLE!"=="true" (
    echo [INFO] ANTLR4 JAR: !ANTLR_JAR!
    echo [INFO] Grammar generation script: scripts\build\generate_grammars.bat
)

echo [INFO] To use ANTLR4 in your CMakeLists.txt, add:
echo     find_package(ANTLR4 REQUIRED^)
echo     target_link_libraries(your_target antlr4-runtime^)

exit /b 0