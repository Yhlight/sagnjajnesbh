#!/bin/bash

# Create a release package with all necessary files

RELEASE_DIR="chtl_antlr_release"
VERSION="1.0.0"

echo "Creating CHTL ANTLR release package v$VERSION..."

# Clean previous release
rm -rf $RELEASE_DIR
mkdir -p $RELEASE_DIR

# Copy headers
echo "Copying headers..."
mkdir -p $RELEASE_DIR/include/chtl
cp -r src/chtl/interfaces $RELEASE_DIR/include/chtl/
cp -r src/chtl/css $RELEASE_DIR/include/chtl/
cp -r src/chtl/javascript $RELEASE_DIR/include/chtl/

# Copy single header version
mkdir -p $RELEASE_DIR/single_header
cp single_header/chtl_antlr_parsers.h $RELEASE_DIR/single_header/

# Copy prebuilt libraries
echo "Copying prebuilt libraries..."
mkdir -p $RELEASE_DIR/lib/windows/x64/Release
mkdir -p $RELEASE_DIR/lib/windows/x64/Debug
cp -r prebuilt/windows/x64/* $RELEASE_DIR/lib/windows/x64/

# Copy build scripts
echo "Copying build scripts..."
mkdir -p $RELEASE_DIR/build_scripts
cp build_all_windows.bat $RELEASE_DIR/build_scripts/
cp build_antlr_libs.bat $RELEASE_DIR/build_scripts/
cp download_prebuilt.bat $RELEASE_DIR/build_scripts/

# Copy examples
echo "Copying examples..."
mkdir -p $RELEASE_DIR/examples
cp -r example_windows_project/* $RELEASE_DIR/examples/
cp single_header/example.cpp $RELEASE_DIR/examples/single_header_example.cpp

# Copy documentation
echo "Copying documentation..."
cp README_ANTLR_Windows.md $RELEASE_DIR/
cp README_prebuilt.md $RELEASE_DIR/

# Create main README
cat > $RELEASE_DIR/README.md << EOF
# CHTL ANTLR Parsers Release v$VERSION

This package contains CSS and JavaScript parsers for the CHTL compiler.

## Contents

- \`include/\` - Header files for using the libraries
- \`lib/\` - Prebuilt static libraries for Windows
- \`single_header/\` - Single header version (stub implementation)
- \`examples/\` - Example code
- \`build_scripts/\` - Scripts to build from source

## Quick Start

### Option 1: Use Prebuilt Libraries (Windows)

\`\`\`cpp
#include "chtl/css/CSSCompiler.h"
#include "chtl/javascript/JavaScriptCompiler.h"

// Link with lib/windows/x64/Release/chtl_antlr_all.lib
\`\`\`

### Option 2: Single Header (Development)

\`\`\`cpp
#include "single_header/chtl_antlr_parsers.h"
// No linking required, but limited functionality
\`\`\`

## Requirements

- C++17 compiler
- Windows: Visual Studio 2019 or later
- Linux/macOS: Build from source required

## License

ANTLR runtime is under BSD license.
CHTL components are under the project license.
EOF

# Create archive
echo "Creating archive..."
tar -czf chtl_antlr_v${VERSION}.tar.gz $RELEASE_DIR/
zip -r chtl_antlr_v${VERSION}.zip $RELEASE_DIR/

echo "Release package created:"
echo "  - chtl_antlr_v${VERSION}.tar.gz"
echo "  - chtl_antlr_v${VERSION}.zip"
echo "  - $RELEASE_DIR/ (directory)"

# Create file list
echo -e "\nPackage contents:"
find $RELEASE_DIR -type f | sort