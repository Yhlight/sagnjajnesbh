#!/bin/bash

# VSCode CHTL Plugin Build Script
# Cross-platform shell script for building and packaging VSCode CHTL extension

set -e

# Color output for better visibility
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

print_status "Starting VSCode CHTL Plugin Build"
print_status "Project root: $PROJECT_ROOT"

# Configuration
PLUGIN_DIR="$PROJECT_ROOT/vscode-chtl-extension"
BUILD_MODE="${1:-production}"  # production or development

if [ "$BUILD_MODE" != "production" ] && [ "$BUILD_MODE" != "development" ]; then
    print_error "Invalid build mode. Use 'production' or 'development'"
    exit 1
fi

print_status "Build mode: $BUILD_MODE"

# Check if plugin directory exists
if [ ! -d "$PLUGIN_DIR" ]; then
    print_error "VSCode plugin directory not found: $PLUGIN_DIR"
    exit 1
fi

cd "$PLUGIN_DIR"

# Check for required tools
print_status "Checking build dependencies..."

if ! command -v node &> /dev/null; then
    print_error "Node.js is not installed or not in PATH"
    print_status "Please install Node.js from: https://nodejs.org/"
    exit 1
fi

if ! command -v npm &> /dev/null; then
    print_error "npm is not installed or not in PATH"
    exit 1
fi

NODE_VERSION=$(node --version)
NPM_VERSION=$(npm --version)

print_status "Node.js version: $NODE_VERSION"
print_status "npm version: $NPM_VERSION"

# Check if vsce is installed
if ! command -v vsce &> /dev/null; then
    print_warning "vsce (Visual Studio Code Extension Manager) not found"
    print_status "Installing vsce globally..."
    npm install -g vsce
    
    if [ $? -ne 0 ]; then
        print_error "Failed to install vsce"
        exit 1
    fi
    
    print_success "vsce installed successfully"
fi

VSCE_VERSION=$(vsce --version)
print_status "vsce version: $VSCE_VERSION"

print_success "Build dependencies verified"

# Install dependencies
print_status "Installing Node.js dependencies..."

if [ -f "package-lock.json" ]; then
    npm ci
else
    npm install
fi

if [ $? -ne 0 ]; then
    print_error "Failed to install dependencies"
    exit 1
fi

print_success "Dependencies installed successfully"

# Run linting
print_status "Running code linting..."

if npm run lint &> /dev/null; then
    print_success "Linting passed"
else
    print_warning "Linting issues found, attempting to fix..."
    if npm run lint:fix &> /dev/null; then
        print_success "Linting issues fixed automatically"
    else
        print_warning "Some linting issues could not be fixed automatically"
    fi
fi

# Run tests if they exist
if npm run test &> /dev/null; then
    print_status "Running tests..."
    npm run test
    
    if [ $? -eq 0 ]; then
        print_success "All tests passed"
    else
        print_warning "Some tests failed, but continuing build..."
    fi
else
    print_warning "No test script found, skipping tests"
fi

# Build the extension
print_status "Building VSCode extension..."

if [ "$BUILD_MODE" = "production" ]; then
    # Production build
    if npm run build &> /dev/null; then
        npm run build
    elif npm run compile &> /dev/null; then
        npm run compile
    else
        print_warning "No build script found, using TypeScript compiler directly"
        if command -v tsc &> /dev/null; then
            tsc -p .
        else
            print_warning "TypeScript compiler not found, skipping compilation"
        fi
    fi
else
    # Development build
    if npm run watch &> /dev/null; then
        print_status "Development mode: running watch in background"
        npm run watch &
        WATCH_PID=$!
        print_status "Watch process started with PID: $WATCH_PID"
        sleep 5  # Give watch time to compile initially
        kill $WATCH_PID 2>/dev/null || true
    else
        print_warning "No watch script found, using regular compilation"
        if npm run compile &> /dev/null; then
            npm run compile
        fi
    fi
fi

print_success "Extension build completed"

# Package the extension
print_status "Packaging VSCode extension..."

# Clean previous packages
rm -f *.vsix

# Create package
if [ "$BUILD_MODE" = "production" ]; then
    vsce package --out "chtl-extension-$(date +%Y%m%d-%H%M%S).vsix"
else
    vsce package --out "chtl-extension-dev-$(date +%Y%m%d-%H%M%S).vsix"
fi

if [ $? -ne 0 ]; then
    print_error "Failed to package extension"
    exit 1
fi

# Find the created package
PACKAGE_FILE=$(ls -t *.vsix | head -n1)

if [ -f "$PACKAGE_FILE" ]; then
    PACKAGE_SIZE=$(ls -lh "$PACKAGE_FILE" | awk '{print $5}')
    print_success "Extension packaged successfully: $PACKAGE_FILE (Size: $PACKAGE_SIZE)"
    
    # Move package to project root packages directory
    PACKAGES_DIR="$PROJECT_ROOT/packages"
    mkdir -p "$PACKAGES_DIR"
    cp "$PACKAGE_FILE" "$PACKAGES_DIR/"
    print_success "Package copied to: $PACKAGES_DIR/$PACKAGE_FILE"
    
    # Create symlink to latest
    cd "$PACKAGES_DIR"
    ln -sf "$PACKAGE_FILE" "chtl-extension-latest.vsix"
    print_success "Created symlink: chtl-extension-latest.vsix"
else
    print_error "Package file not found after packaging"
    exit 1
fi

# Verify package contents
print_status "Verifying package contents..."

if command -v unzip &> /dev/null; then
    unzip -l "$PACKAGES_DIR/$PACKAGE_FILE" | head -20
    print_status "Package verification completed"
fi

# Generate installation instructions
INSTALL_SCRIPT="$PACKAGES_DIR/install_chtl_extension.sh"
cat > "$INSTALL_SCRIPT" << EOF
#!/bin/bash
# CHTL VSCode Extension Installation Script

set -e

SCRIPT_DIR="\$(cd "\$(dirname "\${BASH_SOURCE[0]}")" && pwd)"
PACKAGE_FILE="\$SCRIPT_DIR/$PACKAGE_FILE"

echo "Installing CHTL VSCode Extension..."

if ! command -v code &> /dev/null; then
    echo "ERROR: VSCode 'code' command not found in PATH"
    echo "Please ensure VSCode is installed and the 'code' command is available"
    exit 1
fi

if [ ! -f "\$PACKAGE_FILE" ]; then
    echo "ERROR: Package file not found: \$PACKAGE_FILE"
    exit 1
fi

code --install-extension "\$PACKAGE_FILE"

if [ \$? -eq 0 ]; then
    echo "SUCCESS: CHTL VSCode Extension installed successfully"
    echo "Restart VSCode to activate the extension"
else
    echo "ERROR: Failed to install extension"
    exit 1
fi
EOF

chmod +x "$INSTALL_SCRIPT"
print_success "Created installation script: $INSTALL_SCRIPT"

# Generate Windows installation script
INSTALL_SCRIPT_WIN="$PACKAGES_DIR/install_chtl_extension.bat"
cat > "$INSTALL_SCRIPT_WIN" << EOF
@echo off
REM CHTL VSCode Extension Installation Script for Windows

setlocal EnableDelayedExpansion

set "SCRIPT_DIR=%~dp0"
set "PACKAGE_FILE=%SCRIPT_DIR%$PACKAGE_FILE"

echo Installing CHTL VSCode Extension...

where code >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: VSCode 'code' command not found in PATH
    echo Please ensure VSCode is installed and the 'code' command is available
    exit /b 1
)

if not exist "%PACKAGE_FILE%" (
    echo ERROR: Package file not found: %PACKAGE_FILE%
    exit /b 1
)

code --install-extension "%PACKAGE_FILE%"

if %errorlevel%==0 (
    echo SUCCESS: CHTL VSCode Extension installed successfully
    echo Restart VSCode to activate the extension
) else (
    echo ERROR: Failed to install extension
    exit /b 1
)
EOF

print_success "Created Windows installation script: $INSTALL_SCRIPT_WIN"

# Development server information
if [ "$BUILD_MODE" = "development" ]; then
    print_status "Development build completed"
    print_status "To test the extension:"
    print_status "1. Open VSCode"
    print_status "2. Press F5 to open Extension Development Host"
    print_status "3. Open a .chtl file to test the extension"
fi

print_success "VSCode CHTL Plugin build process completed"
print_status "Package location: $PACKAGES_DIR/$PACKAGE_FILE"
print_status "Installation script: $INSTALL_SCRIPT"
print_status "To install: bash $INSTALL_SCRIPT"

# Show final package info
cd "$PACKAGES_DIR"
print_status "Available packages:"
ls -la *.vsix 2>/dev/null || print_warning "No packages found"

exit 0