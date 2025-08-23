#!/bin/bash

set -e

echo "==============================================="
echo "CHTL CMOD Package Script for Linux"
echo "==============================================="

if [ $# -eq 0 ]; then
    echo "Usage: $0 <module_path>"
    echo "Example: $0 Chtholly/CMOD/Core"
    exit 1
fi

# Set directories
MODULE_PATH="$1"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SOURCE_DIR="$(cd "${SCRIPT_DIR}/../.." && pwd)"
DIST_DIR="${SOURCE_DIR}/dist/modules"

# Check if module path exists
if [ ! -d "${SOURCE_DIR}/${MODULE_PATH}" ]; then
    echo "Module path not found: ${MODULE_PATH}"
    exit 1
fi

# Get module name from path
MODULE_NAME="$(basename "${MODULE_PATH}")"

# Create dist directory
mkdir -p "${DIST_DIR}"

# Create temporary packaging directory
TEMP_DIR="/tmp/chtl_cmod_package_$$"
mkdir -p "${TEMP_DIR}"

echo "Packaging CMOD module: ${MODULE_NAME}"

# Copy module structure according to CHTL grammar specification
cp -r "${SOURCE_DIR}/${MODULE_PATH}/src" "${TEMP_DIR}/${MODULE_NAME}/"
cp -r "${SOURCE_DIR}/${MODULE_PATH}/info" "${TEMP_DIR}/${MODULE_NAME}/"

# Check if required files exist (according to CHTL grammar)
if [ ! -f "${TEMP_DIR}/${MODULE_NAME}/src/${MODULE_NAME}.chtl" ]; then
    echo "Warning: Main source file ${MODULE_NAME}.chtl not found in src directory"
fi

if [ ! -f "${TEMP_DIR}/${MODULE_NAME}/info/${MODULE_NAME}.chtl" ]; then
    echo "Warning: Info file ${MODULE_NAME}.chtl not found in info directory"
fi

# Create .cmod package (zip format)
PACKAGE_FILE="${DIST_DIR}/${MODULE_NAME}.cmod"

# Change to temp directory and create zip
cd "${TEMP_DIR}"
zip -r "${PACKAGE_FILE}" "${MODULE_NAME}" > /dev/null

# Clean up
rm -rf "${TEMP_DIR}"

echo ""
echo "==============================================="
echo "CMOD package created successfully!"
echo "Package: ${PACKAGE_FILE}"
echo "==============================================="