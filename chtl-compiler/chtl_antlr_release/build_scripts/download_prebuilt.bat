@echo off
setlocal EnableDelayedExpansion

:: Download prebuilt ANTLR libraries for Windows

echo Downloading prebuilt ANTLR libraries for Windows...

:: GitHub repository information (update with your actual repo)
set GITHUB_REPO=yourusername/chtl-compiler
set BRANCH=main

:: Create directory
if not exist "prebuilt\windows\x64" mkdir "prebuilt\windows\x64"

:: Download function using PowerShell
echo Downloading from GitHub...

:: Release libraries
powershell -Command "& {
    $baseUrl = 'https://github.com/%GITHUB_REPO%/raw/%BRANCH%/prebuilt/windows/x64/Release/'
    $libs = @('chtl_css.lib', 'chtl_javascript.lib', 'antlr4-runtime.lib', 'chtl_antlr_all.lib')
    
    New-Item -ItemType Directory -Force -Path 'prebuilt\windows\x64\Release' | Out-Null
    
    foreach ($lib in $libs) {
        Write-Host \"Downloading $lib...\"
        try {
            Invoke-WebRequest -Uri \"$baseUrl$lib\" -OutFile \"prebuilt\windows\x64\Release\$lib\"
        } catch {
            Write-Host \"Warning: Failed to download $lib\"
        }
    }
}"

:: Debug libraries
powershell -Command "& {
    $baseUrl = 'https://github.com/%GITHUB_REPO%/raw/%BRANCH%/prebuilt/windows/x64/Debug/'
    $libs = @('chtl_css_d.lib', 'chtl_javascript_d.lib', 'antlr4-runtime_d.lib', 'chtl_antlr_all_d.lib')
    
    New-Item -ItemType Directory -Force -Path 'prebuilt\windows\x64\Debug' | Out-Null
    
    foreach ($lib in $libs) {
        Write-Host \"Downloading $lib...\"
        try {
            Invoke-WebRequest -Uri \"$baseUrl$lib\" -OutFile \"prebuilt\windows\x64\Debug\$lib\"
        } catch {
            Write-Host \"Warning: Failed to download $lib\"
        }
    }
}"

echo.
echo Download complete! Libraries are in prebuilt\windows\x64\
echo.
echo To use in your project:
echo 1. Add to Library Directories: prebuilt\windows\x64\$(Configuration)
echo 2. Link with: chtl_antlr_all.lib (Release) or chtl_antlr_all_d.lib (Debug)
echo.