#!/bin/bash

# ANTLR 4 Source Download Script
# Downloads ANTLR 4 source code and JAR tool for cross-platform builds
# Supports multiple ANTLR versions and mirrors

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
SOURCE_DIR="$ANTLR_ROOT/source"
ANTLR_VERSION="4.13.1"
ANTLR_BASE_URL="https://github.com/antlr/antlr4/archive/refs/tags"
ANTLR_JAR_URL="https://www.antlr.org/download"

echo -e "${PURPLE}📥 ANTLR 4 Source Download Script${NC}"
echo -e "${BLUE}ANTLR Root: $ANTLR_ROOT${NC}"
echo -e "${BLUE}Source Directory: $SOURCE_DIR${NC}"
echo -e "${BLUE}Version: $ANTLR_VERSION${NC}"
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
    echo "  -v, --version VERSION   ANTLR version to download (default: $ANTLR_VERSION)"
    echo "  --source-only           Download only source code (no JAR)"
    echo "  --jar-only              Download only JAR tool (no source)"
    echo "  --force                 Force re-download even if exists"
    echo "  --mirror URL            Use alternative download mirror"
    echo ""
    echo "Examples:"
    echo "  $0                      # Download ANTLR $ANTLR_VERSION"
    echo "  $0 -v 4.12.0            # Download specific version"
    echo "  $0 --source-only        # Source code only"
    echo "  $0 --jar-only           # JAR tool only"
}

# Check prerequisites
check_prerequisites() {
    print_section "Prerequisites Check"
    
    # Check download tools
    if command -v wget >/dev/null 2>&1; then
        DOWNLOAD_TOOL="wget"
        print_success "Download tool: wget"
    elif command -v curl >/dev/null 2>&1; then
        DOWNLOAD_TOOL="curl"
        print_success "Download tool: curl"
    else
        print_error "No download tool found (wget or curl required)"
        exit 1
    fi
    
    # Check extraction tools
    if command -v unzip >/dev/null 2>&1; then
        print_success "Extraction tool: unzip"
    else
        print_error "unzip not found (required for archive extraction)"
        exit 1
    fi
    
    # Check Git (optional)
    if command -v git >/dev/null 2>&1; then
        print_success "Git: available (for alternative download method)"
    else
        print_warning "Git not found (using archive download)"
    fi
}

# Download file with progress
download_file() {
    local url="$1"
    local output="$2"
    local description="$3"
    
    print_info "Downloading $description..."
    print_info "URL: $url"
    print_info "Output: $output"
    
    case "$DOWNLOAD_TOOL" in
        "wget")
            wget --progress=bar:force -O "$output" "$url"
            ;;
        "curl")
            curl -L --progress-bar -o "$output" "$url"
            ;;
    esac
    
    if [ $? -eq 0 ]; then
        local file_size=$(stat -f%z "$output" 2>/dev/null || stat -c%s "$output" 2>/dev/null || echo "unknown")
        print_success "$description downloaded ($file_size bytes)"
        return 0
    else
        print_error "Failed to download $description"
        return 1
    fi
}

# Download ANTLR source code
download_antlr_source() {
    print_section "Downloading ANTLR Source Code"
    
    local archive_name="$ANTLR_VERSION.zip"
    local archive_path="$SOURCE_DIR/$archive_name"
    local extract_dir="$SOURCE_DIR/antlr4-$ANTLR_VERSION"
    
    mkdir -p "$SOURCE_DIR"
    
    # Check if already exists
    if [ -d "$extract_dir" ] && [ "$FORCE_DOWNLOAD" != true ]; then
        print_success "ANTLR source already exists: $extract_dir"
        return 0
    fi
    
    # Download source archive
    local source_url="$ANTLR_BASE_URL/$ANTLR_VERSION.zip"
    
    if download_file "$source_url" "$archive_path" "ANTLR $ANTLR_VERSION source"; then
        print_info "Extracting source archive..."
        cd "$SOURCE_DIR"
        unzip -q "$archive_path"
        
        if [ -d "antlr4-$ANTLR_VERSION" ]; then
            print_success "ANTLR source extracted successfully"
            
            # Create symlink for easier access
            ln -sf "antlr4-$ANTLR_VERSION" current
            print_success "Created symlink: source/current -> antlr4-$ANTLR_VERSION"
            
            # Cleanup archive
            rm -f "$archive_path"
            
            return 0
        else
            print_error "Failed to extract ANTLR source"
            return 1
        fi
    else
        return 1
    fi
}

# Download ANTLR JAR tool
download_antlr_jar() {
    print_section "Downloading ANTLR JAR Tool"
    
    local jar_name="antlr-$ANTLR_VERSION-complete.jar"
    local jar_path="$ANTLR_ROOT/tools/$jar_name"
    
    mkdir -p "$ANTLR_ROOT/tools"
    
    # Check if already exists
    if [ -f "$jar_path" ] && [ "$FORCE_DOWNLOAD" != true ]; then
        print_success "ANTLR JAR already exists: $jar_path"
        return 0
    fi
    
    # Download JAR
    local jar_url="$ANTLR_JAR_URL/antlr-$ANTLR_VERSION-complete.jar"
    
    if download_file "$jar_url" "$jar_path" "ANTLR $ANTLR_VERSION JAR tool"; then
        # Create symlink for easier access
        cd "$ANTLR_ROOT/tools"
        ln -sf "$jar_name" antlr.jar
        print_success "Created symlink: tools/antlr.jar -> $jar_name"
        
        # Verify JAR
        if command -v java >/dev/null 2>&1; then
            java -jar "$jar_path" -version >/dev/null 2>&1
            if [ $? -eq 0 ]; then
                print_success "ANTLR JAR verified successfully"
            else
                print_warning "ANTLR JAR verification failed (Java may not be available)"
            fi
        else
            print_warning "Java not found, cannot verify ANTLR JAR"
        fi
        
        return 0
    else
        return 1
    fi
}

# Download grammar files
download_grammars() {
    print_section "Downloading Official Grammar Files"
    
    local grammars_dir="$ANTLR_ROOT/grammars"
    mkdir -p "$grammars_dir"
    
    # CSS3 Grammar
    print_info "Downloading CSS3 grammar..."
    local css_base="https://raw.githubusercontent.com/antlr/grammars-v4/master/css3"
    download_file "$css_base/css3.g4" "$grammars_dir/css3.g4" "CSS3 grammar"
    
    # JavaScript Grammar
    print_info "Downloading JavaScript grammar..."
    local js_base="https://raw.githubusercontent.com/antlr/grammars-v4/master/javascript/javascript"
    download_file "$js_base/JavaScriptLexer.g4" "$grammars_dir/JavaScriptLexer.g4" "JavaScript lexer grammar"
    download_file "$js_base/JavaScriptParser.g4" "$grammars_dir/JavaScriptParser.g4" "JavaScript parser grammar"
    
    # Additional support files
    if download_file "$js_base/JavaScriptLexerBase.java" "$grammars_dir/JavaScriptLexerBase.java" "JavaScript lexer base"; then
        print_info "Converting Java base to C++..."
        # Create C++ version of the base class
        cat > "$grammars_dir/JavaScriptLexerBase.h" << 'EOF'
#pragma once
#include "antlr4-runtime.h"

class JavaScriptLexerBase : public antlr4::Lexer {
public:
    JavaScriptLexerBase(antlr4::CharStream* input);
    
protected:
    bool IsRegexPossible();
    bool IsStrictMode();
};
EOF
    fi
    
    print_success "Official grammars downloaded"
}

# Create build info
create_build_info() {
    print_section "Creating Build Information"
    
    cat > "$ANTLR_ROOT/BUILD_INFO.md" << EOF
# ANTLR 4 Cross-Platform Build Information

## Version Information
- **ANTLR Version**: $ANTLR_VERSION
- **Download Date**: $(date)
- **Platform**: $(uname -s)
- **Architecture**: $(uname -m)

## Directory Structure
\`\`\`
external/antlr4-cross-platform/
├── source/                 # ANTLR source code
│   ├── antlr4-$ANTLR_VERSION/
│   └── current -> antlr4-$ANTLR_VERSION/
├── precompiled/           # Precompiled binaries
│   ├── linux-x64/
│   ├── macos-x64/
│   └── windows-x64/
├── tools/                 # ANTLR JAR tool
│   ├── antlr-$ANTLR_VERSION-complete.jar
│   └── antlr.jar -> antlr-$ANTLR_VERSION-complete.jar
├── grammars/              # Official grammar files
│   ├── css3.g4
│   ├── JavaScriptLexer.g4
│   └── JavaScriptParser.g4
├── scripts/               # Build scripts
└── docs/                  # Documentation
\`\`\`

## Build Scripts
- \`scripts/download_antlr.sh\` - Download ANTLR source and tools
- \`scripts/build_antlr.sh\` - Cross-platform ANTLR build
- \`scripts/package_antlr.sh\` - Create precompiled packages

## Usage
1. Download ANTLR: \`./scripts/download_antlr.sh\`
2. Build ANTLR: \`./scripts/build_antlr.sh --platform all\`
3. Package: \`./scripts/package_antlr.sh\`

## Integration
ANTLR is integrated into CHTL build system via CMake:
- Headers: \`external/antlr4-cross-platform/precompiled/{platform}/include\`
- Libraries: \`external/antlr4-cross-platform/precompiled/{platform}/lib\`

---

*Generated by ANTLR download script on $(date)*
EOF

    print_success "Build information created"
}

# Main function
main() {
    local version="$ANTLR_VERSION"
    local source_only=false
    local jar_only=false
    local force=false
    local mirror=""
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                show_usage
                exit 0
                ;;
            -v|--version)
                version="$2"
                ANTLR_VERSION="$version"
                shift 2
                ;;
            --source-only)
                source_only=true
                shift
                ;;
            --jar-only)
                jar_only=true
                shift
                ;;
            --force)
                force=true
                FORCE_DOWNLOAD=true
                shift
                ;;
            --mirror)
                mirror="$2"
                ANTLR_BASE_URL="$mirror"
                shift 2
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
    
    print_info "ANTLR version: $ANTLR_VERSION"
    
    check_prerequisites
    
    # Download based on options
    if [ "$jar_only" = false ]; then
        download_antlr_source
        download_grammars
    fi
    
    if [ "$source_only" = false ]; then
        download_antlr_jar
    fi
    
    create_build_info
    
    print_section "Download Summary"
    print_success "ANTLR $ANTLR_VERSION download completed"
    
    echo ""
    print_info "📊 Downloaded Components:"
    [ -d "$SOURCE_DIR/current" ] && echo "  📁 Source Code: ✅ Available"
    [ -f "$ANTLR_ROOT/tools/antlr.jar" ] && echo "  ☕ JAR Tool: ✅ Available"
    [ -f "$ANTLR_ROOT/grammars/css3.g4" ] && echo "  📝 CSS3 Grammar: ✅ Available"
    [ -f "$ANTLR_ROOT/grammars/JavaScriptLexer.g4" ] && echo "  📝 JavaScript Grammar: ✅ Available"
    
    echo ""
    print_info "Next steps:"
    echo "  1. Build ANTLR runtime: ./scripts/build_antlr.sh"
    echo "  2. Generate parsers: ./scripts/generate_parsers.sh"
    echo "  3. Package binaries: ./scripts/package_antlr.sh"
    
    print_success "🎉 ANTLR download ready for cross-platform builds!"
}

# Execute main function
main "$@"