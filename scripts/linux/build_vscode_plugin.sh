#!/bin/bash

set -e

echo "==============================================="
echo "CHTL VSCode Plugin Build Script for Linux"
echo "==============================================="

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SOURCE_DIR="$(cd "${SCRIPT_DIR}/../.." && pwd)"
PLUGIN_DIR="${SOURCE_DIR}/vscode-chtl-extension"
DIST_DIR="${SOURCE_DIR}/dist"

# Check if plugin directory exists
if [ ! -d "${PLUGIN_DIR}" ]; then
    echo "VSCode extension directory not found: ${PLUGIN_DIR}"
    exit 1
fi

# Change to plugin directory
cd "${PLUGIN_DIR}"

# Check if Node.js is installed
if ! command -v node &> /dev/null; then
    echo "Node.js is not installed or not in PATH!"
    exit 1
fi

# Check if npm is installed
if ! command -v npm &> /dev/null; then
    echo "npm is not installed or not in PATH!"
    exit 1
fi

# Install dependencies
echo "Installing dependencies..."
npm install

# Build the extension
echo "Building VSCode extension..."
npm run compile

# Create dist directory
mkdir -p "${DIST_DIR}"

# Package the extension using vsce
echo "Packaging extension..."
npx vsce package --out "${DIST_DIR}"

echo ""
echo "==============================================="
echo "VSCode plugin build completed successfully!"
echo "Package location: ${DIST_DIR}"
echo "==============================================="