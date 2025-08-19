#!/bin/bash

# Complete CHTL Build Script
# Integrated build script for compiler, VSCode plugin, and modules

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

# Show usage information
show_usage() {
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Build Components:"
    echo "  --compiler          Build CHTL compiler (default: enabled)"
    echo "  --vscode            Build VSCode plugin (default: enabled)"
    echo "  --modules           Package all modules (default: enabled)"
    echo "  --all               Build all components (default)"
    echo ""
    echo "Build Options:"
    echo "  --debug             Build in debug mode (default)"
    echo "  --release           Build in release mode"
    echo "  --compile-cjmod     Compile C++ sources for CJMOD modules"
    echo "  --clean             Clean build directories before building"
    echo ""
    echo "Control Options:"
    echo "  --skip-compiler     Skip compiler build"
    echo "  --skip-vscode       Skip VSCode plugin build"
    echo "  --skip-modules      Skip module packaging"
    echo "  -j, --jobs N        Number of parallel build jobs (default: auto)"
    echo "  -v, --verbose       Enable verbose output"
    echo "  -h, --help         Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0                          # Build everything in debug mode"
    echo "  $0 --release --clean        # Clean release build of everything"
    echo "  $0 --compiler --skip-vscode # Build only compiler"
    echo "  $0 --vscode --debug         # Build only VSCode plugin in debug"
}

# Parse command line arguments
BUILD_COMPILER=true
BUILD_VSCODE=true
BUILD_MODULES=true
BUILD_MODE="debug"
COMPILE_CJMOD=false
CLEAN_BUILD=false
PARALLEL_JOBS=""
VERBOSE=false

while [[ $# -gt 0 ]]; do
    case $1 in
        --compiler)
            BUILD_COMPILER=true
            BUILD_VSCODE=false
            BUILD_MODULES=false
            shift
            ;;
        --vscode)
            BUILD_COMPILER=false
            BUILD_VSCODE=true
            BUILD_MODULES=false
            shift
            ;;
        --modules)
            BUILD_COMPILER=false
            BUILD_VSCODE=false
            BUILD_MODULES=true
            shift
            ;;
        --all)
            BUILD_COMPILER=true
            BUILD_VSCODE=true
            BUILD_MODULES=true
            shift
            ;;
        --debug)
            BUILD_MODE="debug"
            shift
            ;;
        --release)
            BUILD_MODE="release"
            shift
            ;;
        --compile-cjmod)
            COMPILE_CJMOD=true
            shift
            ;;
        --clean)
            CLEAN_BUILD=true
            shift
            ;;
        --skip-compiler)
            BUILD_COMPILER=false
            shift
            ;;
        --skip-vscode)
            BUILD_VSCODE=false
            shift
            ;;
        --skip-modules)
            BUILD_MODULES=false
            shift
            ;;
        -j|--jobs)
            PARALLEL_JOBS="$2"
            shift 2
            ;;
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        -h|--help)
            show_usage
            exit 0
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

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

print_status "Starting Complete CHTL Build Process"
print_status "Project root: $PROJECT_ROOT"
print_status "Build mode: $BUILD_MODE"
print_status "Components to build:"
print_status "  - Compiler: $BUILD_COMPILER"
print_status "  - VSCode Plugin: $BUILD_VSCODE"
print_status "  - Modules: $BUILD_MODULES"
print_status "Clean build: $CLEAN_BUILD"
print_status "Compile CJMOD: $COMPILE_CJMOD"

cd "$PROJECT_ROOT"

# Determine parallel jobs
if [ -z "$PARALLEL_JOBS" ]; then
    PARALLEL_JOBS=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
fi
print_status "Parallel jobs: $PARALLEL_JOBS"

# Track build results
BUILD_RESULTS=()
BUILD_TIMES=()

# Function to record build result
record_build_result() {
    local component="$1"
    local result="$2"
    local duration="$3"
    
    BUILD_RESULTS+=("$component:$result")
    BUILD_TIMES+=("$component:$duration")
}

# Function to run build step
run_build_step() {
    local component="$1"
    local script="$2"
    shift 2
    local args=("$@")
    
    print_status "Building $component..."
    local start_time=$(date +%s)
    
    if [ "$VERBOSE" = true ]; then
        print_status "Running: $script ${args[*]}"
    fi
    
    if "$script" "${args[@]}"; then
        local end_time=$(date +%s)
        local duration=$((end_time - start_time))
        record_build_result "$component" "SUCCESS" "${duration}s"
        print_success "$component build completed in ${duration}s"
        return 0
    else
        local end_time=$(date +%s)
        local duration=$((end_time - start_time))
        record_build_result "$component" "FAILED" "${duration}s"
        print_error "$component build failed after ${duration}s"
        return 1
    fi
}

# Clean build directories if requested
if [ "$CLEAN_BUILD" = true ]; then
    print_status "Cleaning build directories..."
    
    rm -rf build-debug build-release build packages dist
    
    if [ -d "vscode-chtl-extension/node_modules" ]; then
        rm -rf vscode-chtl-extension/node_modules
    fi
    
    if [ -d "vscode-chtl-extension/out" ]; then
        rm -rf vscode-chtl-extension/out
    fi
    
    # Clean VSCode extension packages
    rm -f vscode-chtl-extension/*.vsix
    
    print_success "Build directories cleaned"
fi

# Note: ANTLR is pre-built and available in external/antlr4-cross-platform/
print_status "Using pre-built ANTLR from external/antlr4-cross-platform/"

# Build compiler if requested
if [ "$BUILD_COMPILER" = true ]; then
    COMPILER_SCRIPT=""
    if [ "$BUILD_MODE" = "debug" ]; then
        COMPILER_SCRIPT="$SCRIPT_DIR/../build/build_debug.sh"
    else
        COMPILER_SCRIPT="$SCRIPT_DIR/../build/build_release.sh"
    fi
    
    run_build_step "Compiler" "$COMPILER_SCRIPT" || {
        print_error "Compiler build failed - aborting"
        exit 1
    }
fi

# Build VSCode plugin if requested
if [ "$BUILD_VSCODE" = true ]; then
    VSCODE_ARGS=()
    if [ "$BUILD_MODE" = "debug" ]; then
        VSCODE_ARGS+=("development")
    else
        VSCODE_ARGS+=("production")
    fi
    
    run_build_step "VSCode Plugin" "$SCRIPT_DIR/../build/build_vscode_plugin.sh" "${VSCODE_ARGS[@]}" || {
        print_error "VSCode plugin build failed"
        # Continue as this doesn't affect core functionality
    }
fi

# Package modules if requested
if [ "$BUILD_MODULES" = true ]; then
    MODULE_ARGS=()
    MODULE_ARGS+=("-o" "packages")
    
    if [ "$COMPILE_CJMOD" = true ]; then
        MODULE_ARGS+=("--compile")
    fi
    
    if [ "$VERBOSE" = true ]; then
        MODULE_ARGS+=("--verbose")
    fi
    
    run_build_step "Modules" "$SCRIPT_DIR/../packaging/pack_all.sh" "${MODULE_ARGS[@]}" || {
        print_error "Module packaging failed"
        # Continue as modules are optional
    }
fi

# Generate build report
print_status "Generating build report..."

REPORT_FILE="$PROJECT_ROOT/build-report-$(date +%Y%m%d-%H%M%S).txt"
cat > "$REPORT_FILE" << EOF
CHTL Complete Build Report
==========================
Generated: $(date)
Build Mode: $BUILD_MODE
Project Root: $PROJECT_ROOT

Build Configuration:
  - ANTLR: $BUILD_ANTLR
  - Compiler: $BUILD_COMPILER
  - VSCode Plugin: $BUILD_VSCODE
  - Modules: $BUILD_MODULES
  - Clean Build: $CLEAN_BUILD
  - Compile CJMOD: $COMPILE_CJMOD
  - Parallel Jobs: $PARALLEL_JOBS

Build Results:
EOF

# Add results to report
TOTAL_SUCCESS=0
TOTAL_FAILED=0

for result in "${BUILD_RESULTS[@]}"; do
    IFS=':' read -r component status <<< "$result"
    
    # Find corresponding time
    for time_entry in "${BUILD_TIMES[@]}"; do
        IFS=':' read -r time_component duration <<< "$time_entry"
        if [ "$time_component" = "$component" ]; then
            echo "  - $component: $status ($duration)" >> "$REPORT_FILE"
            break
        fi
    done
    
    if [ "$status" = "SUCCESS" ]; then
        ((TOTAL_SUCCESS++))
    else
        ((TOTAL_FAILED++))
    fi
done

cat >> "$REPORT_FILE" << EOF

Summary:
  - Total Components: $((TOTAL_SUCCESS + TOTAL_FAILED))
  - Successful: $TOTAL_SUCCESS
  - Failed: $TOTAL_FAILED
  - Success Rate: $(( TOTAL_SUCCESS * 100 / (TOTAL_SUCCESS + TOTAL_FAILED) ))%

Build Artifacts:
EOF

# List build artifacts
if [ -d "build-$BUILD_MODE" ]; then
    echo "  - Compiler: build-$BUILD_MODE/" >> "$REPORT_FILE"
    if [ -f "build-$BUILD_MODE/chtl_compiler" ]; then
        SIZE=$(ls -lh "build-$BUILD_MODE/chtl_compiler" | awk '{print $5}')
        echo "    * chtl_compiler ($SIZE)" >> "$REPORT_FILE"
    fi
fi

if [ -d "packages" ]; then
    echo "  - VSCode Extension: packages/" >> "$REPORT_FILE"
    for vsix in packages/*.vsix; do
        if [ -f "$vsix" ]; then
            SIZE=$(ls -lh "$vsix" | awk '{print $5}')
            echo "    * $(basename "$vsix") ($SIZE)" >> "$REPORT_FILE"
        fi
    done
    
    echo "  - Module Packages: packages/" >> "$REPORT_FILE"
    for pkg in packages/*.cmod packages/*.cjmod; do
        if [ -f "$pkg" ]; then
            SIZE=$(ls -lh "$pkg" | awk '{print $5}')
            echo "    * $(basename "$pkg") ($SIZE)" >> "$REPORT_FILE"
        fi
    done
fi

if [ -d "dist" ]; then
    echo "  - Distribution: dist/" >> "$REPORT_FILE"
fi

print_success "Build report saved: $REPORT_FILE"

# Create quick install script
INSTALL_SCRIPT="$PROJECT_ROOT/quick-install.sh"
cat > "$INSTALL_SCRIPT" << 'EOF'
#!/bin/bash
# CHTL Quick Installation Script

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "CHTL Quick Installation"
echo "======================"

# Install compiler
if [ -d "$SCRIPT_DIR/dist" ]; then
    echo "Installing CHTL compiler..."
    if [ -w "/usr/local/bin" ]; then
        sudo cp "$SCRIPT_DIR/dist/bin/"* /usr/local/bin/ 2>/dev/null || true
        echo "Compiler installed to /usr/local/bin"
    else
        echo "Please manually copy files from dist/bin/ to your PATH"
    fi
fi

# Install VSCode extension
if [ -d "$SCRIPT_DIR/packages" ]; then
    VSIX_FILE=$(ls "$SCRIPT_DIR/packages/"*.vsix 2>/dev/null | head -n1)
    if [ -f "$VSIX_FILE" ] && command -v code &> /dev/null; then
        echo "Installing VSCode extension..."
        code --install-extension "$VSIX_FILE"
        echo "VSCode extension installed"
    fi
fi

# Install modules
if [ -f "$SCRIPT_DIR/packages/install-all-modules.sh" ]; then
    echo "Installing CHTL modules..."
    bash "$SCRIPT_DIR/packages/install-all-modules.sh"
fi

echo "Installation completed!"
echo "Restart VSCode to activate the extension"
EOF

chmod +x "$INSTALL_SCRIPT"
print_success "Quick install script created: $INSTALL_SCRIPT"

# Final summary
print_status "Build Summary:"
print_status "=============="
print_status "Total components: $((TOTAL_SUCCESS + TOTAL_FAILED))"
print_status "Successful: $TOTAL_SUCCESS"
print_status "Failed: $TOTAL_FAILED"

if [ $TOTAL_FAILED -eq 0 ]; then
    print_success "All components built successfully!"
    print_status "Next steps:"
    print_status "1. Run './quick-install.sh' to install everything"
    print_status "2. Check build-report-*.txt for detailed information"
    print_status "3. Test the compiler with: chtl_compiler --version"
    exit 0
else
    print_warning "$TOTAL_FAILED component(s) failed to build"
    print_status "Check build-report-*.txt for details"
    exit 1
fi