@echo off
setlocal EnableDelayedExpansion

:: Build ANTLR-based CSS and JavaScript static libraries for Windows

echo Building ANTLR-based parsers for Windows...

:: Get current directory
set SCRIPT_DIR=%~dp0
cd /d "%SCRIPT_DIR%"

:: Create build directory
if not exist "build\antlr" mkdir "build\antlr"

:: Set compiler (adjust path if needed)
set CXX=cl.exe
set AR=lib.exe

:: Check if Visual Studio environment is set
where cl >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo Error: Visual Studio compiler not found in PATH
    echo Please run this script from Visual Studio Developer Command Prompt
    exit /b 1
)

:: Compile CSS parser library
echo Building CSS parser...
cd build\antlr

:: Compile CSS parser files
%CXX% /c /std:c++17 /EHsc /MD ^
    /I..\..\third_party\antlr4-runtime\runtime\src ^
    /I..\..\third_party\generated ^
    ..\..\third_party\generated\css3\*.cpp ^
    ..\..\src\chtl\css\CSSCompiler.cpp

if %ERRORLEVEL% NEQ 0 (
    echo Error: Failed to compile CSS parser
    exit /b 1
)

:: Create CSS static library
%AR% /OUT:chtl_css.lib *.obj
del *.obj

:: Compile JavaScript parser library
echo Building JavaScript parser...
%CXX% /c /std:c++17 /EHsc /MD ^
    /I..\..\third_party\antlr4-runtime\runtime\src ^
    /I..\..\third_party\generated ^
    ..\..\third_party\generated\javascript\*.cpp ^
    ..\..\src\chtl\javascript\JavaScriptCompiler.cpp

if %ERRORLEVEL% NEQ 0 (
    echo Error: Failed to compile JavaScript parser
    exit /b 1
)

:: Create JavaScript static library
%AR% /OUT:chtl_javascript.lib *.obj
del *.obj

:: Copy ANTLR runtime library
if exist "..\..\third_party\antlr4-runtime\build\runtime\Release\antlr4-runtime.lib" (
    copy "..\..\third_party\antlr4-runtime\build\runtime\Release\antlr4-runtime.lib" .
) else if exist "..\..\third_party\antlr4-runtime\build\runtime\Debug\antlr4-runtime.lib" (
    copy "..\..\third_party\antlr4-runtime\build\runtime\Debug\antlr4-runtime.lib" .
) else (
    echo Warning: ANTLR runtime library not found. Please build it first.
)

echo.
echo Static libraries created:
echo   - build\antlr\chtl_css.lib
echo   - build\antlr\chtl_javascript.lib
echo   - build\antlr\antlr4-runtime.lib

:: Create combined static library
echo Creating combined library...
%AR% /OUT:chtl_antlr_all.lib chtl_css.lib chtl_javascript.lib antlr4-runtime.lib

echo Combined library created: build\antlr\chtl_antlr_all.lib

cd ..\..
echo Done!