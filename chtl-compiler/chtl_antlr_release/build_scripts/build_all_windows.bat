@echo off
setlocal EnableDelayedExpansion

:: Complete build script for CHTL with ANTLR parsers on Windows

echo ========================================
echo Building CHTL Compiler Suite for Windows
echo ========================================

:: Get current directory
set SCRIPT_DIR=%~dp0
cd /d "%SCRIPT_DIR%"

:: Step 1: Download ANTLR if not present
if not exist "third_party\antlr-4.13.1-complete.jar" (
    echo Downloading ANTLR...
    powershell -Command "Invoke-WebRequest -Uri 'https://www.antlr.org/download/antlr-4.13.1-complete.jar' -OutFile 'third_party\antlr-4.13.1-complete.jar'"
)

:: Step 2: Download ANTLR C++ Runtime if not present
if not exist "third_party\antlr4-runtime" (
    echo Downloading ANTLR C++ Runtime...
    powershell -Command "Invoke-WebRequest -Uri 'https://www.antlr.org/download/antlr4-cpp-runtime-4.13.1-source.zip' -OutFile 'third_party\antlr4-runtime.zip'"
    powershell -Command "Expand-Archive -Path 'third_party\antlr4-runtime.zip' -DestinationPath 'third_party\antlr4-runtime' -Force"
    del third_party\antlr4-runtime.zip
)

:: Step 3: Build ANTLR Runtime
echo.
echo Building ANTLR Runtime...
call build_antlr_runtime_windows.bat
if %ERRORLEVEL% NEQ 0 (
    echo Error: Failed to build ANTLR runtime
    exit /b 1
)

:: Step 4: Generate parsers
echo.
echo Generating CSS and JavaScript parsers...

:: Check if Java is available
where java >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo Error: Java not found in PATH
    echo Please install Java to generate parsers
    exit /b 1
)

:: Generate CSS parser
cd third_party
if not exist "generated\css3" mkdir "generated\css3"
java -jar antlr-4.13.1-complete.jar -Dlanguage=Cpp -visitor -package css3 -o generated\css3 grammars\css3\css3Lexer.g4 grammars\css3\css3Parser.g4

:: Generate JavaScript parser
if not exist "generated\javascript" mkdir "generated\javascript"
java -jar antlr-4.13.1-complete.jar -Dlanguage=Cpp -visitor -package javascript -o generated\javascript grammars\javascript\JavaScriptLexer.g4 grammars\javascript\JavaScriptParser.g4

cd ..

:: Step 5: Build CHTL libraries
echo.
echo Building CHTL libraries...
call build_antlr_libs.bat
if %ERRORLEVEL% NEQ 0 (
    echo Error: Failed to build CHTL libraries
    exit /b 1
)

:: Step 6: Build example (optional)
echo.
echo Building example application...
if exist "build\antlr" (
    cd build\antlr
    cl.exe /std:c++17 /EHsc /MD ^
        /I..\..\third_party\antlr4-runtime\runtime\src ^
        /I..\..\src ^
        ..\..\examples\use_antlr_parsers.cpp ^
        /link chtl_css.lib chtl_javascript.lib antlr4-runtime.lib ^
        /out:antlr_example.exe
    cd ..\..
)

echo.
echo ========================================
echo Build completed successfully!
echo ========================================
echo.
echo Libraries created:
echo   - build\antlr\chtl_css.lib
echo   - build\antlr\chtl_javascript.lib
echo   - build\antlr\chtl_antlr_all.lib
echo.
echo Example: build\antlr\antlr_example.exe
echo.