#!/bin/bash

# ANTLR 4 Cross-Platform Build Script
# Builds ANTLR C++ runtime for multiple platforms
# Creates precompiled packages for distribution

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
ANTLR_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
SOURCE_DIR="$ANTLR_ROOT/source/current"
PRECOMPILED_DIR="$ANTLR_ROOT/precompiled"
BUILD_DIR="$ANTLR_ROOT/build"

echo -e "${PURPLE}🔨 ANTLR 4 Cross-Platform Build Script${NC}"
echo -e "${BLUE}ANTLR Root: $ANTLR_ROOT${NC}"
echo -e "${BLUE}Source Directory: $SOURCE_DIR${NC}"
echo -e "${BLUE}Precompiled Directory: $PRECOMPILED_DIR${NC}"
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

# Usage information
show_usage() {
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -h, --help              Show this help message"
    echo "  -p, --platform PLATFORM Target platform (linux|macos|windows|all)"
    echo "  -j, --jobs JOBS         Number of parallel jobs (default: auto)"
    echo "  --clean                 Clean previous builds"
    echo "  --debug                 Build debug version"
    echo "  --static                Build static libraries only"
    echo "  --shared                Build shared libraries only"
    echo ""
    echo "Platforms:"
    echo "  linux    - Linux x86_64"
    echo "  macos    - macOS x86_64"
    echo "  windows  - Windows x86_64"
    echo "  all      - All supported platforms"
    echo ""
    echo "Examples:"
    echo "  $0 --platform linux    # Build for Linux only"
    echo "  $0 --platform all      # Build for all platforms"
    echo "  $0 --clean --debug     # Clean debug build"
}

# Platform detection
detect_platform() {
    case "$(uname -s)" in
        Linux*)     echo "linux";;
        Darwin*)    echo "macos";;
        CYGWIN*)    echo "windows";;
        MINGW*)     echo "windows";;
        MSYS*)      echo "windows";;
        *)          echo "unknown";;
    esac
}

# Check build prerequisites
check_build_prerequisites() {
    local platform="$1"
    
    print_section "Build Prerequisites Check"
    print_info "Target platform: $platform"
    
    # Check CMake
    if command -v cmake >/dev/null 2>&1; then
        local cmake_version=$(cmake --version | head -n1 | grep -o '[0-9]\+\.[0-9]\+\.[0-9]\+')
        print_success "CMake: $cmake_version"
        
        # Check minimum version
        if [ "$(printf '%s\n' "3.16" "$cmake_version" | sort -V | head -n1)" = "3.16" ]; then
            print_success "CMake version requirement met (>= 3.16)"
        else
            print_error "CMake version too old (need >= 3.16, found $cmake_version)"
            return 1
        fi
    else
        print_error "CMake not found"
        return 1
    fi
    
    # Check C++ compiler
    case "$platform" in
        "linux"|"macos")
            if command -v g++ >/dev/null 2>&1; then
                local gcc_version=$(g++ --version | head -n1 | grep -o '[0-9]\+\.[0-9]\+\.[0-9]\+' | head -1)
                print_success "GCC: $gcc_version"
            elif command -v clang++ >/dev/null 2>&1; then
                local clang_version=$(clang++ --version | head -n1 | grep -o '[0-9]\+\.[0-9]\+\.[0-9]\+' | head -1)
                print_success "Clang: $clang_version"
            else
                print_error "No C++ compiler found (GCC or Clang required)"
                return 1
            fi
            ;;
        "windows")
            print_info "Windows build requires Visual Studio or MinGW"
            ;;
    esac
    
    # Check Make
    if command -v make >/dev/null 2>&1; then
        print_success "Make: available"
    else
        print_error "Make not found"
        return 1
    fi
    
    # Check source availability
    if [ ! -d "$SOURCE_DIR" ]; then
        print_error "ANTLR source not found. Run download_antlr.sh first"
        return 1
    else
        print_success "ANTLR source: available"
    fi
}

# Build ANTLR for specific platform
build_antlr_platform() {
    local platform="$1"
    local build_type="$2"
    local jobs="$3"
    
    print_section "Building ANTLR for $platform"
    
    local platform_dir="$PRECOMPILED_DIR/$platform-x64"
    local build_platform_dir="$BUILD_DIR/$platform-$build_type"
    
    mkdir -p "$build_platform_dir" "$platform_dir"
    
    # Navigate to ANTLR C++ runtime source
    local cpp_runtime_dir="$SOURCE_DIR/runtime/Cpp"
    
    if [ ! -d "$cpp_runtime_dir" ]; then
        print_error "ANTLR C++ runtime source not found: $cpp_runtime_dir"
        return 1
    fi
    
    cd "$build_platform_dir"
    
    print_info "Configuring ANTLR C++ runtime build..."
    
    # Platform-specific CMake configuration
    case "$platform" in
        "linux")
            cmake "$cpp_runtime_dir" \
                -DCMAKE_BUILD_TYPE="$build_type" \
                -DCMAKE_INSTALL_PREFIX="$platform_dir" \
                -DCMAKE_CXX_STANDARD=17 \
                -DWITH_STATIC_CRT=OFF \
                -DDISABLE_WARNINGS=ON
            ;;
        "macos")
            cmake "$cpp_runtime_dir" \
                -DCMAKE_BUILD_TYPE="$build_type" \
                -DCMAKE_INSTALL_PREFIX="$platform_dir" \
                -DCMAKE_CXX_STANDARD=17 \
                -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 \
                -DWITH_STATIC_CRT=OFF
            ;;
        "windows")
            cmake "$cpp_runtime_dir" \
                -DCMAKE_BUILD_TYPE="$build_type" \
                -DCMAKE_INSTALL_PREFIX="$platform_dir" \
                -DCMAKE_CXX_STANDARD=17 \
                -DWITH_STATIC_CRT=ON \
                -G "Visual Studio 16 2019" -A x64
            ;;
    esac
    
    if [ $? -eq 0 ]; then
        print_success "CMake configuration completed"
    else
        print_error "CMake configuration failed"
        return 1
    fi
    
    print_info "Building ANTLR C++ runtime..."
    
    # Build
    case "$platform" in
        "linux"|"macos")
            make -j"$jobs"
            make install
            ;;
        "windows")
            cmake --build . --config "$build_type" --parallel "$jobs"
            cmake --install . --config "$build_type"
            ;;
    esac
    
    if [ $? -eq 0 ]; then
        print_success "ANTLR build completed for $platform"
        
        # Verify build results
        if [ -d "$platform_dir/include" ] && [ -d "$platform_dir/lib" ]; then
            local header_count=$(find "$platform_dir/include" -name "*.h" | wc -l)
            local lib_count=$(find "$platform_dir/lib" -name "*.a" -o -name "*.so" -o -name "*.dylib" -o -name "*.lib" | wc -l)
            
            print_success "Build verification:"
            print_info "  Headers: $header_count files"
            print_info "  Libraries: $lib_count files"
            
            return 0
        else
            print_error "Build verification failed - missing output files"
            return 1
        fi
    else
        print_error "ANTLR build failed for $platform"
        return 1
    fi
}

# Build for all platforms
build_all_platforms() {
    local build_type="$1"
    local jobs="$2"
    
    local current_platform=$(detect_platform)
    print_info "Current platform: $current_platform"
    
    local platforms=("linux" "macos" "windows")
    local built=0
    local failed=0
    
    for platform in "${platforms[@]}"; do
        if [ "$platform" = "$current_platform" ]; then
            print_info "Building for native platform: $platform"
            if build_antlr_platform "$platform" "$build_type" "$jobs"; then
                ((built++))
            else
                ((failed++))
            fi
        else
            print_warning "Cross-compilation for $platform not implemented yet"
            print_info "To build for $platform, run this script on a $platform system"
        fi
    done
    
    print_section "Multi-Platform Build Summary"
    print_info "Built platforms: $built"
    print_info "Failed platforms: $failed"
    
    if [ $built -gt 0 ]; then
        print_success "At least one platform built successfully"
        return 0
    else
        print_error "No platforms built successfully"
        return 1
    fi
}

# Main function
main() {
    local target_platform="$(detect_platform)"
    local build_type="Release"
    local jobs="$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)"
    local clean=false
    local static_only=false
    local shared_only=false
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                show_usage
                exit 0
                ;;
            -p|--platform)
                target_platform="$2"
                shift 2
                ;;
            -j|--jobs)
                jobs="$2"
                shift 2
                ;;
            --clean)
                clean=true
                shift
                ;;
            --debug)
                build_type="Debug"
                shift
                ;;
            --static)
                static_only=true
                shift
                ;;
            --shared)
                shared_only=true
                shift
                ;;
            -*)
                print_error "Unknown option: $1"
                show_usage
                exit 1
                ;;
            *)
                print_error "Unexpected argument: $1"
                show_usage
                exit 1
                ;;
        esac
    done
    
    # Clean if requested
    if [ "$clean" = true ]; then
        print_section "Cleaning Previous Builds"
        rm -rf "$BUILD_DIR" "$PRECOMPILED_DIR"
        print_success "Previous builds cleaned"
    fi
    
    # Check prerequisites
    if ! check_build_prerequisites "$target_platform"; then
        exit 1
    fi
    
    print_info "Build configuration:"
    print_info "  Platform: $target_platform"
    print_info "  Build type: $build_type"
    print_info "  Parallel jobs: $jobs"
    
    # Build based on platform
    if [ "$target_platform" = "all" ]; then
        build_all_platforms "$build_type" "$jobs"
    else
        build_antlr_platform "$target_platform" "$build_type" "$jobs"
    fi
    
    if [ $? -eq 0 ]; then
        print_section "ANTLR Build Complete"
        print_success "ANTLR C++ runtime build completed successfully"
        
        echo ""
        print_info "📁 Build outputs:"
        find "$PRECOMPILED_DIR" -name "*.a" -o -name "*.so" -o -name "*.dylib" -o -name "*.lib" 2>/dev/null | head -5
        
        echo ""
        print_success "🎉 ANTLR cross-platform build ready!"
        print_info "Next: Run package_antlr.sh to create distribution packages"
    else
        print_error "ANTLR build failed"
        exit 1
    fi
}

# Execute main function
main "$@"