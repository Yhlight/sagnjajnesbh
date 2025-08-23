#!/bin/bash
# CHTL VSCode Extension Build Script for Linux
# This script only builds the VSCode extension without compiler dependencies

set -e

echo "Building CHTL VSCode Extension on Linux..."

# Check if Node.js is available
if ! command -v node &> /dev/null; then
    echo "Error: Node.js is not installed"
    echo "Please install Node.js: sudo apt install nodejs npm (Ubuntu/Debian)"
    exit 1
fi

# Check if npm is available
if ! command -v npm &> /dev/null; then
    echo "Error: npm is not available"
    echo "Please install npm: sudo apt install npm"
    exit 1
fi

# Navigate to VSCode extension directory
if [[ ! -d "vscode-chtl-extension" ]]; then
    echo "Error: VSCode extension directory not found"
    exit 1
fi

cd vscode-chtl-extension

# Install dependencies
echo "Installing extension dependencies..."
npm install

# Install vsce globally if not present
if ! command -v vsce &> /dev/null; then
    echo "Installing vsce..."
    npm install -g vsce
fi

# Compile TypeScript
echo "Compiling TypeScript source..."
npm run compile

# Create distribution directory
mkdir -p ../dist/vscode

# Package the extension
echo "Packaging VSCode extension..."
npx vsce package --out ../dist/vscode/chtl-extension.vsix

cd ..

echo ""
echo "VSCode extension build completed successfully!"
echo "Extension package available at: dist/vscode/chtl-extension.vsix"
echo ""
echo "To install the extension:"
echo "  code --install-extension dist/vscode/chtl-extension.vsix"
echo ""