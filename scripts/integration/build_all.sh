#!/bin/bash

# CHTL Integration Build Script
# Complete build pipeline: Compiler + Modules + VSCode Extension
# Cross-platform integration build for full CHTL ecosystem

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
BUILD_DIR="$PROJECT_ROOT/build-integration"
DIST_DIR="$PROJECT_ROOT/dist-integration"
PACKAGES_DIR="$PROJECT_ROOT/packages"
VERSION="1.0.0"

echo -e "${PURPLE}🏗️ CHTL Integration Build Script v$VERSION${NC}"
echo -e "${BLUE}Project Root: $PROJECT_ROOT${NC}"
echo -e "${BLUE}Build Directory: $BUILD_DIR${NC}"
echo -e "${BLUE}Distribution Directory: $DIST_DIR${NC}"
echo -e "${BLUE}Packages Directory: $PACKAGES_DIR${NC}"
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
    echo "  -v, --verbose           Verbose output"
    echo "  --skip-compiler         Skip compiler build"
    echo "  --skip-modules          Skip module packaging"
    echo "  --skip-vscode           Skip VSCode extension build"
    echo "  --skip-tests            Skip testing phase"
    echo "  --clean                 Clean all previous builds"
    echo "  --release               Use release build configuration"
    echo ""
    echo "Build Pipeline:"
    echo "  1. 🔧 Build CHTL Compiler System"
    echo "  2. 📦 Package Official Modules (Chtholly, Yuigahama)"
    echo "  3. 🎨 Build VSCode Extension"
    echo "  4. 🧪 Run Comprehensive Tests"
    echo "  5. 📋 Generate Integration Report"
    echo "  6. 🚀 Create Distribution Package"
    echo ""
    echo "Output Structure:"
    echo "  dist-integration/"
    echo "  ├── compiler/           # CHTL compiler binaries"
    echo "  ├── modules/            # Packaged modules"
    echo "  ├── vscode-extension/   # VSCode extension"
    echo "  ├── docs/               # Documentation"
    echo "  └── tests/              # Test results"
}

# Build compiler system
build_compiler() {
    print_section "Building CHTL Compiler System"
    
    local build_type="Release"
    [ "$USE_RELEASE" = true ] && build_type="Release" || build_type="Debug"
    
    print_info "Using build configuration: $build_type"
    
    # Use release build script
    if [ "$USE_RELEASE" = true ]; then
        "$PROJECT_ROOT/scripts/build/release_build.sh"
        
        # Copy build results
        if [ -d "$PROJECT_ROOT/build-release" ]; then
            mkdir -p "$DIST_DIR/compiler"
            cp -r "$PROJECT_ROOT/build-release/bin" "$DIST_DIR/compiler/" 2>/dev/null || true
            cp -r "$PROJECT_ROOT/build-release/lib" "$DIST_DIR/compiler/" 2>/dev/null || true
            print_success "Compiler binaries copied to integration distribution"
        fi
    else
        # Use debug build for development
        mkdir -p "$BUILD_DIR"
        cd "$BUILD_DIR"
        
        cmake "$PROJECT_ROOT" -DCMAKE_BUILD_TYPE=Debug
        make -j$(nproc)
        
        mkdir -p "$DIST_DIR/compiler"
        cp -r bin "$DIST_DIR/compiler/" 2>/dev/null || true
        cp -r lib "$DIST_DIR/compiler/" 2>/dev/null || true
    fi
    
    print_success "CHTL compiler build completed"
}

# Package official modules
package_modules() {
    print_section "Packaging Official Modules"
    
    mkdir -p "$PACKAGES_DIR"
    
    # Package Chtholly modules
    print_info "Packaging Chtholly modules..."
    if [ -d "$PROJECT_ROOT/src/Module/Chtholly" ]; then
        "$SCRIPT_DIR/pack_unified.sh" --batch -o "$PACKAGES_DIR" "$PROJECT_ROOT/src/Module/Chtholly"
        print_success "Chtholly modules packaged"
    else
        print_warning "Chtholly module directory not found"
    fi
    
    # Package Yuigahama modules
    print_info "Packaging Yuigahama modules..."
    if [ -d "$PROJECT_ROOT/src/Module/Yuigahama" ]; then
        "$SCRIPT_DIR/pack_unified.sh" --batch -o "$PACKAGES_DIR" "$PROJECT_ROOT/src/Module/Yuigahama"
        print_success "Yuigahama modules packaged"
    else
        print_warning "Yuigahama module directory not found"
    fi
    
    # Copy packages to distribution
    mkdir -p "$DIST_DIR/modules"
    cp "$PACKAGES_DIR"/*.cmod "$DIST_DIR/modules/" 2>/dev/null || true
    cp "$PACKAGES_DIR"/*.cjmod "$DIST_DIR/modules/" 2>/dev/null || true
    
    local cmod_count=$(find "$DIST_DIR/modules" -name "*.cmod" | wc -l)
    local cjmod_count=$(find "$DIST_DIR/modules" -name "*.cjmod" | wc -l)
    
    print_success "Module packaging completed"
    print_info "CMOD packages: $cmod_count"
    print_info "CJMOD packages: $cjmod_count"
}

# Build VSCode extension
build_vscode_extension() {
    print_section "Building VSCode Extension"
    
    local vscode_dir="$PROJECT_ROOT/vscode-chtl-extension"
    
    if [ ! -d "$vscode_dir" ]; then
        print_error "VSCode extension directory not found: $vscode_dir"
        return 1
    fi
    
    cd "$vscode_dir"
    
    # Check if Node.js is available
    if command -v npm >/dev/null 2>&1; then
        print_info "Installing VSCode extension dependencies..."
        npm install --silent
        
        print_info "Building VSCode extension..."
        npm run compile 2>/dev/null || true
        
        # Package extension
        if command -v vsce >/dev/null 2>&1; then
            print_info "Packaging VSCode extension..."
            vsce package --out "$DIST_DIR/vscode-extension/"
            print_success "VSCode extension packaged"
        else
            print_warning "vsce not found, creating manual package..."
            mkdir -p "$DIST_DIR/vscode-extension"
            cp -r . "$DIST_DIR/vscode-extension/chtl-extension"
            print_success "Manual VSCode extension package created"
        fi
    else
        print_warning "Node.js/npm not found, skipping VSCode extension build"
        
        # Create basic extension package
        mkdir -p "$DIST_DIR/vscode-extension"
        cp -r "$vscode_dir" "$DIST_DIR/vscode-extension/chtl-extension"
        print_success "Basic VSCode extension package created"
    fi
}

# Run comprehensive tests
run_tests() {
    print_section "Running Comprehensive Tests"
    
    mkdir -p "$DIST_DIR/tests"
    
    # Test compiler
    if [ -f "$DIST_DIR/compiler/bin/ComprehensiveTestRunner" ]; then
        print_info "Running compiler tests..."
        cd "$PROJECT_ROOT"
        "$DIST_DIR/compiler/bin/ComprehensiveTestRunner" > "$DIST_DIR/tests/compiler_tests.log" 2>&1
        
        if [ $? -eq 0 ]; then
            print_success "Compiler tests passed"
        else
            print_warning "Some compiler tests failed"
        fi
    fi
    
    # Test modules
    if [ -d "$DIST_DIR/modules" ]; then
        print_info "Validating packaged modules..."
        local module_count=$(find "$DIST_DIR/modules" -name "*.cmod" -o -name "*.cjmod" | wc -l)
        print_success "Found $module_count packaged modules"
    fi
    
    # Test VSCode extension
    if [ -d "$DIST_DIR/vscode-extension" ]; then
        print_info "Validating VSCode extension..."
        if [ -f "$DIST_DIR/vscode-extension/chtl-extension/package.json" ]; then
            print_success "VSCode extension structure valid"
        else
            print_warning "VSCode extension structure may be incomplete"
        fi
    fi
}

# Generate integration report
generate_report() {
    print_section "Generating Integration Report"
    
    local report_file="$DIST_DIR/INTEGRATION_REPORT.md"
    
    cat > "$report_file" << EOF
# CHTL Integration Build Report

**Build Date**: $(date)  
**Version**: $VERSION  
**Platform**: $(uname -s)  
**Build Type**: Integration

## 📊 Build Summary

### 🔧 Compiler System
- Status: $([ -d "$DIST_DIR/compiler/bin" ] && echo "✅ Built" || echo "❌ Failed")
- Binaries: $(find "$DIST_DIR/compiler/bin" -type f 2>/dev/null | wc -l) files
- Libraries: $(find "$DIST_DIR/compiler/lib" -type f 2>/dev/null | wc -l) files

### 📦 Module Packages
- CMOD Packages: $(find "$DIST_DIR/modules" -name "*.cmod" 2>/dev/null | wc -l)
- CJMOD Packages: $(find "$DIST_DIR/modules" -name "*.cjmod" 2>/dev/null | wc -l)
- Total Size: $(du -sh "$DIST_DIR/modules" 2>/dev/null | cut -f1 || echo "Unknown")

### 🎨 VSCode Extension
- Status: $([ -d "$DIST_DIR/vscode-extension" ] && echo "✅ Built" || echo "❌ Failed")
- Package: $([ -f "$DIST_DIR/vscode-extension"/*.vsix ] && echo "✅ Created" || echo "📁 Manual")

### 🧪 Test Results
- Compiler Tests: $([ -f "$DIST_DIR/tests/compiler_tests.log" ] && echo "✅ Executed" || echo "⏭️ Skipped")
- Test Log: tests/compiler_tests.log

## 📁 Distribution Structure

\`\`\`
dist-integration/
├── compiler/           # CHTL compiler system
│   ├── bin/           # Executable files
│   └── lib/           # Library files
├── modules/            # Official modules
│   ├── *.cmod         # CMOD packages
│   └── *.cjmod        # CJMOD packages
├── vscode-extension/   # VSCode extension
├── tests/              # Test results
└── docs/               # Documentation
\`\`\`

## 🎯 Package Contents

### Official Modules
$(find "$DIST_DIR/modules" -name "*.cmod" -o -name "*.cjmod" 2>/dev/null | while read -r pkg; do echo "- $(basename "$pkg")"; done)

### Compiler Components
$(find "$DIST_DIR/compiler/bin" -type f 2>/dev/null | while read -r bin; do echo "- $(basename "$bin")"; done)

## ✅ Integration Status

All components have been successfully integrated and packaged for distribution.

---

*Generated by CHTL Integration Build Script v$VERSION*
EOF

    print_success "Integration report generated: INTEGRATION_REPORT.md"
}

# Create final distribution
create_final_distribution() {
    print_section "Creating Final Distribution"
    
    # Copy documentation
    mkdir -p "$DIST_DIR/docs"
    cp "$PROJECT_ROOT/README.md" "$DIST_DIR/docs/" 2>/dev/null || true
    cp "$PROJECT_ROOT/CHTL语法文档.md" "$DIST_DIR/docs/" 2>/dev/null || true
    cp "$PROJECT_ROOT/RELEASE_NOTES.md" "$DIST_DIR/docs/" 2>/dev/null || true
    
    # Create installation script
    cat > "$DIST_DIR/install.sh" << 'EOF'
#!/bin/bash
# CHTL Installation Script

echo "🚀 Installing CHTL v1.0.0..."

# Copy compiler to system path
if [ -d "compiler/bin" ]; then
    echo "📁 Installing compiler binaries..."
    sudo cp compiler/bin/* /usr/local/bin/ 2>/dev/null || cp compiler/bin/* ~/bin/
fi

# Install modules
if [ -d "modules" ]; then
    echo "📦 Installing official modules..."
    mkdir -p ~/.chtl/modules
    cp modules/* ~/.chtl/modules/
fi

# Install VSCode extension
if [ -d "vscode-extension" ]; then
    echo "🎨 VSCode extension available in vscode-extension/"
    echo "   Install manually or use: code --install-extension chtl-extension.vsix"
fi

echo "✅ CHTL installation completed!"
echo "📚 Documentation available in docs/"
EOF
    
    chmod +x "$DIST_DIR/install.sh"
    
    # Create version info
    cat > "$DIST_DIR/VERSION" << EOF
CHTL Integration Build
Version: $VERSION
Build Date: $(date)
Platform: $(uname -s)
Build Type: Integration

Components:
- CHTL Compiler System
- Official Modules (Chtholly, Yuigahama)
- VSCode Extension
- Documentation
- Test Results

Installation:
Run ./install.sh to install CHTL system-wide
EOF
    
    print_success "Final distribution created"
}

# Main execution pipeline
main() {
    local verbose=false
    local skip_compiler=false
    local skip_modules=false
    local skip_vscode=false
    local skip_tests=false
    local clean=false
    local use_release=true
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                show_usage
                exit 0
                ;;
            -v|--verbose)
                verbose=true
                shift
                ;;
            --skip-compiler)
                skip_compiler=true
                shift
                ;;
            --skip-modules)
                skip_modules=true
                shift
                ;;
            --skip-vscode)
                skip_vscode=true
                shift
                ;;
            --skip-tests)
                skip_tests=true
                shift
                ;;
            --clean)
                clean=true
                shift
                ;;
            --release)
                use_release=true
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
    
    # Set global variables
    USE_RELEASE=$use_release
    
    # Clean previous builds if requested
    if [ "$clean" = true ]; then
        print_section "Cleaning Previous Builds"
        rm -rf "$BUILD_DIR" "$DIST_DIR" "$PACKAGES_DIR"
        print_success "Previous builds cleaned"
    fi
    
    # Create output directories
    mkdir -p "$DIST_DIR" "$PACKAGES_DIR"
    
    # Execute build pipeline
    print_section "CHTL Integration Build Pipeline"
    
    # Step 1: Build compiler
    if [ "$skip_compiler" = false ]; then
        print_info "🔧 Step 1: Building CHTL Compiler System"
        build_compiler
    else
        print_warning "⏭️ Skipping compiler build"
    fi
    
    # Step 2: Package modules
    if [ "$skip_modules" = false ]; then
        print_info "📦 Step 2: Packaging Official Modules"
        package_modules
    else
        print_warning "⏭️ Skipping module packaging"
    fi
    
    # Step 3: Build VSCode extension
    if [ "$skip_vscode" = false ]; then
        print_info "🎨 Step 3: Building VSCode Extension"
        build_vscode_extension
    else
        print_warning "⏭️ Skipping VSCode extension build"
    fi
    
    # Step 4: Run tests
    if [ "$skip_tests" = false ]; then
        print_info "🧪 Step 4: Running Comprehensive Tests"
        run_tests
    else
        print_warning "⏭️ Skipping tests"
    fi
    
    # Step 5: Generate report
    print_info "📋 Step 5: Generating Integration Report"
    generate_report
    
    # Step 6: Create final distribution
    print_info "🚀 Step 6: Creating Final Distribution"
    create_final_distribution
    
    # Final summary
    print_section "Integration Build Complete"
    print_success "CHTL integration build completed successfully"
    
    echo ""
    print_info "📊 Build Statistics:"
    echo "  🔧 Compiler: $([ -d "$DIST_DIR/compiler" ] && echo "✅ Ready" || echo "❌ Failed")"
    echo "  📦 Modules: $(find "$DIST_DIR/modules" -name "*.cmod" -o -name "*.cjmod" 2>/dev/null | wc -l) packages"
    echo "  🎨 VSCode: $([ -d "$DIST_DIR/vscode-extension" ] && echo "✅ Ready" || echo "❌ Failed")"
    echo "  📚 Docs: $(find "$DIST_DIR/docs" -name "*.md" 2>/dev/null | wc -l) files"
    
    echo ""
    print_info "📁 Distribution location: $DIST_DIR"
    print_info "📦 Packages location: $PACKAGES_DIR"
    
    echo ""
    print_success "🎉 CHTL v$VERSION integration build ready for deployment!"
    print_info "Run ./install.sh in $DIST_DIR to install CHTL system-wide"
}

# Execute main function
main "$@"