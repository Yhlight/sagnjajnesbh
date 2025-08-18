#!/bin/bash

# CHTL Release Build Script
# Cross-platform optimized build for production deployment
# Supports: Linux, macOS, Windows (via WSL/MSYS2)

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build-release"
BUILD_TYPE="Release"
DIST_DIR="$PROJECT_ROOT/dist"
VERSION="1.0.0"

echo -e "${PURPLE}🚀 CHTL Release Build Script v$VERSION${NC}"
echo -e "${BLUE}Project Root: $PROJECT_ROOT${NC}"
echo -e "${BLUE}Build Directory: $BUILD_DIR${NC}"
echo -e "${BLUE}Distribution Directory: $DIST_DIR${NC}"
echo -e "${BLUE}Build Type: $BUILD_TYPE${NC}"
echo ""

# Function definitions
print_section() {
    echo -e "${CYAN}=== $1 ===${NC}"
}

print_success() {
    echo -e "${GREEN}✅ $1${NC}"
}

print_error() {
    echo -e "${RED}❌ $1${NC}"
}

print_warning() {
    echo -e "${YELLOW}⚠️ $1${NC}"
}

print_info() {
    echo -e "${BLUE}ℹ️ $1${NC}"
}

# Platform detection
detect_platform() {
    case "$(uname -s)" in
        Linux*)     PLATFORM=Linux;;
        Darwin*)    PLATFORM=macOS;;
        CYGWIN*)    PLATFORM=Windows;;
        MINGW*)     PLATFORM=Windows;;
        MSYS*)      PLATFORM=Windows;;
        *)          PLATFORM=Unknown;;
    esac
    
    print_info "Detected platform: $PLATFORM"
}

# Prerequisites check
check_prerequisites() {
    print_section "Prerequisites Check"
    
    # Check CMake
    if command -v cmake >/dev/null 2>&1; then
        CMAKE_VERSION=$(cmake --version | head -n1 | cut -d' ' -f3)
        print_success "CMake found: $CMAKE_VERSION"
    else
        print_error "CMake not found. Please install CMake 3.16+"
        exit 1
    fi
    
    # Check C++ compiler
    if command -v g++ >/dev/null 2>&1; then
        GCC_VERSION=$(g++ --version | head -n1)
        print_success "GCC found: $GCC_VERSION"
    elif command -v clang++ >/dev/null 2>&1; then
        CLANG_VERSION=$(clang++ --version | head -n1)
        print_success "Clang found: $CLANG_VERSION"
    else
        print_error "No C++ compiler found. Please install GCC or Clang"
        exit 1
    fi
    
    # Check make
    if command -v make >/dev/null 2>&1; then
        print_success "Make found"
    else
        print_error "Make not found. Please install build tools"
        exit 1
    fi
}

# Clean previous builds
clean_build() {
    print_section "Cleaning Previous Builds"
    
    if [ -d "$BUILD_DIR" ]; then
        print_info "Removing previous build directory..."
        rm -rf "$BUILD_DIR"
    fi
    
    if [ -d "$DIST_DIR" ]; then
        print_info "Removing previous distribution directory..."
        rm -rf "$DIST_DIR"
    fi
    
    print_success "Clean completed"
}

# Configure build
configure_build() {
    print_section "Configuring Release Build"
    
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    print_info "Running CMake configuration..."
    cmake "$PROJECT_ROOT" \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_CXX_FLAGS_RELEASE="-O3 -DNDEBUG -march=native" \
        -DCMAKE_INSTALL_PREFIX="$DIST_DIR" \
        -DENABLE_TESTING=OFF \
        -DENABLE_OPTIMIZATIONS=ON
    
    if [ $? -eq 0 ]; then
        print_success "CMake configuration completed"
    else
        print_error "CMake configuration failed"
        exit 1
    fi
}

# Build project
build_project() {
    print_section "Building CHTL Project"
    
    cd "$BUILD_DIR"
    
    # Determine number of cores for parallel build
    if [ "$PLATFORM" = "Linux" ] || [ "$PLATFORM" = "macOS" ]; then
        CORES=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
    else
        CORES=4
    fi
    
    print_info "Building with $CORES parallel jobs..."
    make -j$CORES
    
    if [ $? -eq 0 ]; then
        print_success "Build completed successfully"
    else
        print_error "Build failed"
        exit 1
    fi
}

# Run tests
run_tests() {
    print_section "Running Release Tests"
    
    cd "$BUILD_DIR"
    
    if [ -f "bin/ComprehensiveTestRunner" ]; then
        print_info "Running comprehensive syntax tests..."
        ./bin/ComprehensiveTestRunner > "$PROJECT_ROOT/test-reports/release_test_$(date +%Y%m%d_%H%M%S).log" 2>&1
        
        if [ $? -eq 0 ]; then
            print_success "All tests passed"
        else
            print_warning "Some tests failed, check logs for details"
        fi
    else
        print_warning "Test runner not found, skipping tests"
    fi
}

# Create distribution
create_distribution() {
    print_section "Creating Distribution Package"
    
    mkdir -p "$DIST_DIR"/{bin,lib,include,modules,docs,examples}
    
    # Copy binaries
    if [ -d "$BUILD_DIR/bin" ]; then
        cp -r "$BUILD_DIR/bin"/* "$DIST_DIR/bin/" 2>/dev/null || true
        print_success "Binaries copied"
    fi
    
    # Copy libraries
    if [ -d "$BUILD_DIR/lib" ]; then
        cp -r "$BUILD_DIR/lib"/* "$DIST_DIR/lib/" 2>/dev/null || true
        print_success "Libraries copied"
    fi
    
    # Copy headers
    cp -r "$PROJECT_ROOT/src/Shared"/* "$DIST_DIR/include/" 2>/dev/null || true
    print_success "Headers copied"
    
    # Copy modules
    if [ -d "$PROJECT_ROOT/src/Module" ]; then
        cp -r "$PROJECT_ROOT/src/Module"/* "$DIST_DIR/modules/" 2>/dev/null || true
        print_success "Modules copied"
    fi
    
    # Copy documentation
    cp "$PROJECT_ROOT/README.md" "$DIST_DIR/docs/" 2>/dev/null || true
    cp "$PROJECT_ROOT/CHTL语法文档.md" "$DIST_DIR/docs/" 2>/dev/null || true
    cp "$PROJECT_ROOT/RELEASE_NOTES.md" "$DIST_DIR/docs/" 2>/dev/null || true
    print_success "Documentation copied"
    
    # Create version info
    cat > "$DIST_DIR/VERSION" << EOF
CHTL Release Build
Version: $VERSION
Build Date: $(date)
Platform: $PLATFORM
Build Type: $BUILD_TYPE
EOF
    
    print_success "Distribution package created"
}

# Create archive
create_archive() {
    print_section "Creating Release Archive"
    
    cd "$PROJECT_ROOT"
    ARCHIVE_NAME="chtl-v$VERSION-$PLATFORM-$(date +%Y%m%d)"
    
    if command -v tar >/dev/null 2>&1; then
        tar -czf "$ARCHIVE_NAME.tar.gz" -C dist .
        print_success "Archive created: $ARCHIVE_NAME.tar.gz"
    fi
    
    if command -v zip >/dev/null 2>&1; then
        cd dist && zip -r "../$ARCHIVE_NAME.zip" . && cd ..
        print_success "ZIP archive created: $ARCHIVE_NAME.zip"
    fi
}

# Main execution
main() {
    detect_platform
    check_prerequisites
    clean_build
    configure_build
    build_project
    run_tests
    create_distribution
    create_archive
    
    print_section "Release Build Summary"
    print_success "CHTL release build completed successfully"
    print_info "Distribution location: $DIST_DIR"
    print_info "Archive location: $PROJECT_ROOT/chtl-v$VERSION-$PLATFORM-$(date +%Y%m%d)*"
    
    echo ""
    print_info "Release package contents:"
    echo "  📁 bin/         - Executable files"
    echo "  📁 lib/         - Library files"
    echo "  📁 include/     - Header files"
    echo "  📁 modules/     - Official modules (Chtholly, Yuigahama)"
    echo "  📁 docs/        - Documentation"
    echo "  📄 VERSION      - Build information"
    
    echo ""
    print_success "🎉 CHTL v$VERSION release build ready for distribution!"
}

# Execute main function
main "$@"