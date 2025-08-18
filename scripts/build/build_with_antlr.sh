#!/bin/bash

# CHTL Build with ANTLR Integration Script
# Builds CHTL with cross-platform ANTLR support
# Automatically handles ANTLR dependencies and integration

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
ANTLR_DIR="$PROJECT_ROOT/external/antlr4-cross-platform"
BUILD_DIR="$PROJECT_ROOT/build-with-antlr"

echo -e "${PURPLE}🔗 CHTL Build with ANTLR Integration${NC}"
echo -e "${BLUE}Project Root: $PROJECT_ROOT${NC}"
echo -e "${BLUE}ANTLR Directory: $ANTLR_DIR${NC}"
echo -e "${BLUE}Build Directory: $BUILD_DIR${NC}"
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
    echo "  --auto-download         Automatically download ANTLR if missing"
    echo "  --auto-build            Automatically build ANTLR if missing"
    echo "  --use-precompiled       Use precompiled ANTLR packages"
    echo "  --clean                 Clean previous builds"
    echo "  --debug                 Build in debug mode"
    echo "  --test                  Run tests after build"
    echo ""
    echo "Build Pipeline:"
    echo "  1. 🔍 Check ANTLR availability"
    echo "  2. 📥 Download ANTLR (if needed)"
    echo "  3. 🔨 Build ANTLR runtime (if needed)"
    echo "  4. 🏗️ Build CHTL with ANTLR integration"
    echo "  5. 🧪 Run integration tests (optional)"
    echo ""
    echo "Examples:"
    echo "  $0                      # Basic build with existing ANTLR"
    echo "  $0 --auto-download      # Auto-download ANTLR if missing"
    echo "  $0 --auto-build         # Auto-build ANTLR if missing"
    echo "  $0 --use-precompiled    # Use precompiled packages"
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

# Check ANTLR availability
check_antlr_availability() {
    print_section "Checking ANTLR Availability"
    
    local platform=$(detect_platform)
    local platform_dir="$ANTLR_DIR/precompiled/$platform-x64"
    
    print_info "Target platform: $platform"
    
    # Check precompiled ANTLR
    if [ -d "$platform_dir/include" ] && [ -d "$platform_dir/lib" ]; then
        local header_count=$(find "$platform_dir/include" -name "*.h" | wc -l)
        local lib_count=$(find "$platform_dir/lib" -name "*.a" -o -name "*.so" -o -name "*.dylib" -o -name "*.lib" | wc -l)
        
        if [ $header_count -gt 0 ] && [ $lib_count -gt 0 ]; then
            print_success "Precompiled ANTLR found for $platform"
            print_info "  Headers: $header_count files"
            print_info "  Libraries: $lib_count files"
            return 0
        fi
    fi
    
    # Check existing ANTLR installation
    if [ -d "$PROJECT_ROOT/external/antlr4-install" ]; then
        print_success "Existing ANTLR installation found"
        return 0
    fi
    
    print_warning "ANTLR not found for $platform"
    return 1
}

# Setup ANTLR if missing
setup_antlr() {
    local auto_download="$1"
    local auto_build="$2"
    
    if check_antlr_availability; then
        return 0
    fi
    
    print_section "Setting Up ANTLR"
    
    if [ "$auto_download" = true ]; then
        print_info "Auto-downloading ANTLR..."
        if [ -f "$ANTLR_DIR/scripts/download_antlr.sh" ]; then
            "$ANTLR_DIR/scripts/download_antlr.sh"
        else
            print_error "ANTLR download script not found"
            return 1
        fi
    fi
    
    if [ "$auto_build" = true ]; then
        print_info "Auto-building ANTLR..."
        local platform=$(detect_platform)
        
        if [ -f "$ANTLR_DIR/scripts/build_antlr.sh" ]; then
            "$ANTLR_DIR/scripts/build_antlr.sh" --platform "$platform"
        else
            print_error "ANTLR build script not found"
            return 1
        fi
    fi
    
    # Final check
    if check_antlr_availability; then
        print_success "ANTLR setup completed"
        return 0
    else
        print_error "ANTLR setup failed"
        print_info "Please manually download and build ANTLR:"
        print_info "  1. cd external/antlr4-cross-platform"
        print_info "  2. ./scripts/download_antlr.sh"
        print_info "  3. ./scripts/build_antlr.sh"
        return 1
    fi
}

# Configure CHTL build with ANTLR
configure_chtl_build() {
    local build_type="$1"
    local use_precompiled="$2"
    
    print_section "Configuring CHTL Build with ANTLR"
    
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    local platform=$(detect_platform)
    local cmake_args=""
    
    if [ "$use_precompiled" = true ] && [ -d "$ANTLR_DIR/precompiled/$platform-x64" ]; then
        print_info "Using precompiled ANTLR for $platform"
        
        cmake_args="$cmake_args -DANTLR4_ROOT=$ANTLR_DIR/precompiled/$platform-x64"
        cmake_args="$cmake_args -DANTLR4_INCLUDE_DIR=$ANTLR_DIR/precompiled/$platform-x64/include"
        cmake_args="$cmake_args -DANTLR4_LIBRARY_DIR=$ANTLR_DIR/precompiled/$platform-x64/lib"
    elif [ -d "$PROJECT_ROOT/external/antlr4-install" ]; then
        print_info "Using existing ANTLR installation"
        
        cmake_args="$cmake_args -DANTLR4_ROOT=$PROJECT_ROOT/external/antlr4-install"
    else
        print_error "No ANTLR installation found"
        return 1
    fi
    
    print_info "Running CMake configuration..."
    cmake "$PROJECT_ROOT" \
        -DCMAKE_BUILD_TYPE="$build_type" \
        -DWITH_ANTLR4=ON \
        $cmake_args
    
    if [ $? -eq 0 ]; then
        print_success "CHTL with ANTLR configuration completed"
        return 0
    else
        print_error "CHTL with ANTLR configuration failed"
        return 1
    fi
}

# Build CHTL with ANTLR
build_chtl() {
    local jobs="$1"
    
    print_section "Building CHTL with ANTLR Integration"
    
    cd "$BUILD_DIR"
    
    print_info "Building with $jobs parallel jobs..."
    make -j"$jobs"
    
    if [ $? -eq 0 ]; then
        print_success "CHTL with ANTLR build completed"
        
        # Verify key components
        local test_binaries=("ComprehensiveTestRunner" "SimpleTestRunner")
        for binary in "${test_binaries[@]}"; do
            if [ -f "bin/$binary" ]; then
                print_success "  $binary: ✅ Built"
            else
                print_warning "  $binary: ❌ Missing"
            fi
        done
        
        return 0
    else
        print_error "CHTL with ANTLR build failed"
        return 1
    fi
}

# Run integration tests
run_integration_tests() {
    print_section "Running ANTLR Integration Tests"
    
    cd "$BUILD_DIR"
    
    # Test basic ANTLR functionality
    if [ -f "bin/test_antlr_basic" ]; then
        print_info "Running basic ANTLR tests..."
        ./bin/test_antlr_basic > "$BUILD_DIR/antlr_basic_test.log" 2>&1
        
        if [ $? -eq 0 ]; then
            print_success "Basic ANTLR tests passed"
        else
            print_warning "Basic ANTLR tests failed, check log: antlr_basic_test.log"
        fi
    fi
    
    # Test compiler integration
    if [ -f "bin/test_antlr_compilers" ]; then
        print_info "Running ANTLR compiler integration tests..."
        ./bin/test_antlr_compilers > "$BUILD_DIR/antlr_compilers_test.log" 2>&1
        
        if [ $? -eq 0 ]; then
            print_success "ANTLR compiler integration tests passed"
        else
            print_warning "ANTLR compiler integration tests failed, check log: antlr_compilers_test.log"
        fi
    fi
    
    # Run comprehensive tests
    if [ -f "bin/ComprehensiveTestRunner" ]; then
        print_info "Running comprehensive CHTL tests..."
        ./bin/ComprehensiveTestRunner > "$BUILD_DIR/comprehensive_test.log" 2>&1
        
        local success_rate=$(grep "成功率:" "$BUILD_DIR/comprehensive_test.log" | tail -1 | grep -o '[0-9.]*%' || echo "Unknown")
        print_info "Comprehensive test success rate: $success_rate"
        
        if [ "$success_rate" = "100.0%" ]; then
            print_success "All comprehensive tests passed!"
        else
            print_warning "Some comprehensive tests failed"
        fi
    fi
}

# Main function
main() {
    local auto_download=false
    local auto_build=false
    local use_precompiled=false
    local clean=false
    local build_type="Release"
    local run_tests=false
    local jobs="$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)"
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                show_usage
                exit 0
                ;;
            --auto-download)
                auto_download=true
                shift
                ;;
            --auto-build)
                auto_build=true
                shift
                ;;
            --use-precompiled)
                use_precompiled=true
                shift
                ;;
            --clean)
                clean=true
                shift
                ;;
            --debug)
                build_type="Debug"
                shift
                ;;
            --test)
                run_tests=true
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
        rm -rf "$BUILD_DIR"
        print_success "Previous builds cleaned"
    fi
    
    # Setup ANTLR
    if ! setup_antlr "$auto_download" "$auto_build"; then
        exit 1
    fi
    
    # Configure and build CHTL
    if configure_chtl_build "$build_type" "$use_precompiled"; then
        if build_chtl "$jobs"; then
            # Run tests if requested
            if [ "$run_tests" = true ]; then
                run_integration_tests
            fi
            
            print_section "CHTL with ANTLR Build Summary"
            print_success "CHTL with ANTLR integration build completed"
            
            echo ""
            print_info "📊 Build Results:"
            echo "  🔧 Build Type: $build_type"
            echo "  🌍 Platform: $(detect_platform)"
            echo "  📁 Build Directory: $BUILD_DIR"
            echo "  🧪 Tests: $([ "$run_tests" = true ] && echo "Executed" || echo "Skipped")"
            
            echo ""
            print_info "🎯 Available Executables:"
            find "$BUILD_DIR/bin" -type f -executable 2>/dev/null | while read -r exe; do
                echo "  🔧 $(basename "$exe")"
            done
            
            echo ""
            print_success "🎉 CHTL with ANTLR integration ready!"
            
        else
            exit 1
        fi
    else
        exit 1
    fi
}

# Execute main function
main "$@"