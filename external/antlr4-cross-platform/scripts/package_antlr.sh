#!/bin/bash

# ANTLR 4 Precompiled Package Creation Script
# Creates distributable precompiled packages for all platforms
# Generates packages ready for CHTL integration

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
PRECOMPILED_DIR="$ANTLR_ROOT/precompiled"
PACKAGES_DIR="$ANTLR_ROOT/packages"
VERSION="4.13.1"

echo -e "${PURPLE}📦 ANTLR 4 Precompiled Package Creation Script${NC}"
echo -e "${BLUE}ANTLR Root: $ANTLR_ROOT${NC}"
echo -e "${BLUE}Precompiled Directory: $PRECOMPILED_DIR${NC}"
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
    echo "  -o, --output DIR        Output directory (default: ./packages)"
    echo "  -v, --version VERSION   ANTLR version (default: $VERSION)"
    echo "  --include-source        Include source code in packages"
    echo "  --include-tools         Include JAR tools in packages"
    echo "  --compress LEVEL        Compression level 1-9 (default: 6)"
    echo ""
    echo "Package Types:"
    echo "  📦 Runtime packages     - Headers and libraries only"
    echo "  🔧 Development packages - Runtime + source + tools"
    echo "  🚀 Minimal packages     - Libraries only"
    echo ""
    echo "Examples:"
    echo "  $0                      # Create runtime packages"
    echo "  $0 --include-source     # Create development packages"
    echo "  $0 --include-tools      # Include JAR tools"
}

# Validate precompiled builds
validate_builds() {
    print_section "Validating Precompiled Builds"
    
    local platforms=("linux-x64" "macos-x64" "windows-x64")
    local valid_platforms=0
    
    for platform in "${platforms[@]}"; do
        local platform_dir="$PRECOMPILED_DIR/$platform"
        
        if [ -d "$platform_dir" ]; then
            print_info "Checking $platform build..."
            
            # Check headers
            local header_count=$(find "$platform_dir/include" -name "*.h" 2>/dev/null | wc -l)
            if [ $header_count -gt 0 ]; then
                print_success "  Headers: $header_count files"
            else
                print_warning "  Headers: missing"
                continue
            fi
            
            # Check libraries
            local lib_count=$(find "$platform_dir/lib" -name "*.a" -o -name "*.so" -o -name "*.dylib" -o -name "*.lib" 2>/dev/null | wc -l)
            if [ $lib_count -gt 0 ]; then
                print_success "  Libraries: $lib_count files"
                ((valid_platforms++))
            else
                print_warning "  Libraries: missing"
            fi
        else
            print_warning "$platform build not found"
        fi
    done
    
    if [ $valid_platforms -eq 0 ]; then
        print_error "No valid precompiled builds found"
        print_info "Run build_antlr.sh first to build ANTLR runtime"
        return 1
    else
        print_success "Found $valid_platforms valid platform builds"
        return 0
    fi
}

# Create platform package
create_platform_package() {
    local platform="$1"
    local output_dir="$2"
    local include_source="$3"
    local include_tools="$4"
    local compress_level="$5"
    
    local platform_dir="$PRECOMPILED_DIR/$platform"
    local package_name="antlr4-$VERSION-$platform"
    local package_path="$output_dir/$package_name.tar.gz"
    
    if [ ! -d "$platform_dir" ]; then
        print_warning "Platform build not found: $platform"
        return 1
    fi
    
    print_info "Creating package for $platform..."
    
    # Create temporary directory for packaging
    local temp_dir=$(mktemp -d)
    local temp_package="$temp_dir/$package_name"
    
    mkdir -p "$temp_package"/{include,lib,docs}
    
    # Copy headers and libraries
    cp -r "$platform_dir/include"/* "$temp_package/include/" 2>/dev/null || true
    cp -r "$platform_dir/lib"/* "$temp_package/lib/" 2>/dev/null || true
    
    # Include source if requested
    if [ "$include_source" = true ] && [ -d "$ANTLR_ROOT/source/current" ]; then
        print_info "Including source code..."
        mkdir -p "$temp_package/source"
        cp -r "$ANTLR_ROOT/source/current/runtime/Cpp"/* "$temp_package/source/" 2>/dev/null || true
    fi
    
    # Include tools if requested
    if [ "$include_tools" = true ] && [ -f "$ANTLR_ROOT/tools/antlr.jar" ]; then
        print_info "Including JAR tools..."
        mkdir -p "$temp_package/tools"
        cp "$ANTLR_ROOT/tools"/* "$temp_package/tools/" 2>/dev/null || true
    fi
    
    # Create package info
    cat > "$temp_package/PACKAGE_INFO.md" << EOF
# ANTLR 4 Precompiled Package

**Version**: $VERSION  
**Platform**: $platform  
**Build Date**: $(date)  
**Build Type**: Release

## Contents
- 📁 include/ - ANTLR C++ runtime headers
- 📁 lib/ - ANTLR C++ runtime libraries
- 📁 docs/ - Documentation
$([ "$include_source" = true ] && echo "- 📁 source/ - C++ runtime source code")
$([ "$include_tools" = true ] && echo "- 📁 tools/ - ANTLR JAR tools")

## Integration
Add to CMakeLists.txt:
\`\`\`cmake
# ANTLR 4 Integration
include_directories(\${CMAKE_SOURCE_DIR}/external/antlr4-$platform/include)
link_directories(\${CMAKE_SOURCE_DIR}/external/antlr4-$platform/lib)
target_link_libraries(your_target antlr4-runtime)
\`\`\`

## Library Files
$(find "$temp_package/lib" -name "*.a" -o -name "*.so" -o -name "*.dylib" -o -name "*.lib" 2>/dev/null | while read -r lib; do echo "- $(basename "$lib")"; done)

---
*Generated by ANTLR packaging script*
EOF
    
    # Copy documentation
    if [ -f "$ANTLR_ROOT/BUILD_INFO.md" ]; then
        cp "$ANTLR_ROOT/BUILD_INFO.md" "$temp_package/docs/"
    fi
    
    # Create archive
    cd "$temp_dir"
    
    if command -v tar >/dev/null 2>&1; then
        tar -czf "$package_path" "$package_name"
        
        if [ $? -eq 0 ]; then
            local package_size=$(stat -f%z "$package_path" 2>/dev/null || stat -c%s "$package_path" 2>/dev/null || echo "unknown")
            print_success "Package created: $package_name.tar.gz ($package_size bytes)"
            
            # Create checksum
            if command -v sha256sum >/dev/null 2>&1; then
                cd "$output_dir"
                sha256sum "$(basename "$package_path")" > "$(basename "$package_path").sha256"
                print_success "Checksum created: $(basename "$package_path").sha256"
            fi
            
            # Cleanup
            rm -rf "$temp_dir"
            return 0
        else
            print_error "Failed to create package for $platform"
            rm -rf "$temp_dir"
            return 1
        fi
    else
        print_error "tar not found"
        rm -rf "$temp_dir"
        return 1
    fi
}

# Create CMake integration file
create_cmake_integration() {
    local output_dir="$1"
    
    print_section "Creating CMake Integration Files"
    
    cat > "$output_dir/FindANTLR4.cmake" << 'EOF'
# FindANTLR4.cmake - CMake module for finding ANTLR 4 C++ runtime
# Automatically detects platform and links appropriate precompiled package

# Platform detection
if(WIN32)
    set(ANTLR4_PLATFORM "windows-x64")
elseif(APPLE)
    set(ANTLR4_PLATFORM "macos-x64")
else()
    set(ANTLR4_PLATFORM "linux-x64")
endif()

# Find ANTLR 4 installation
find_path(ANTLR4_INCLUDE_DIR
    NAMES antlr4-runtime.h
    PATHS
        ${CMAKE_SOURCE_DIR}/external/antlr4-cross-platform/precompiled/${ANTLR4_PLATFORM}/include
        ${CMAKE_SOURCE_DIR}/external/antlr4-install/include/antlr4-runtime
        /usr/local/include/antlr4-runtime
        /opt/antlr4/include
    PATH_SUFFIXES antlr4-runtime
)

find_library(ANTLR4_LIBRARY
    NAMES antlr4-runtime libantlr4-runtime
    PATHS
        ${CMAKE_SOURCE_DIR}/external/antlr4-cross-platform/precompiled/${ANTLR4_PLATFORM}/lib
        ${CMAKE_SOURCE_DIR}/external/antlr4-install/lib
        /usr/local/lib
        /opt/antlr4/lib
)

# Set variables
if(ANTLR4_INCLUDE_DIR AND ANTLR4_LIBRARY)
    set(ANTLR4_FOUND TRUE)
    set(ANTLR4_INCLUDE_DIRS ${ANTLR4_INCLUDE_DIR})
    set(ANTLR4_LIBRARIES ${ANTLR4_LIBRARY})
    
    # Create imported target
    if(NOT TARGET antlr4::runtime)
        add_library(antlr4::runtime UNKNOWN IMPORTED)
        set_target_properties(antlr4::runtime PROPERTIES
            IMPORTED_LOCATION ${ANTLR4_LIBRARY}
            INTERFACE_INCLUDE_DIRECTORIES ${ANTLR4_INCLUDE_DIR}
        )
    endif()
    
    message(STATUS "Found ANTLR 4: ${ANTLR4_LIBRARY}")
    message(STATUS "ANTLR 4 platform: ${ANTLR4_PLATFORM}")
else()
    set(ANTLR4_FOUND FALSE)
    if(ANTLR4_FIND_REQUIRED)
        message(FATAL_ERROR "ANTLR 4 C++ runtime not found")
    endif()
endif()

# Usage example:
# find_package(ANTLR4 REQUIRED)
# target_link_libraries(your_target antlr4::runtime)
EOF
    
    print_success "CMake integration file created: FindANTLR4.cmake"
    
    # Create simple integration example
    cat > "$output_dir/INTEGRATION_EXAMPLE.cmake" << 'EOF'
# Example CMakeLists.txt integration for ANTLR 4

cmake_minimum_required(VERSION 3.16)
project(YourProject)

# Add ANTLR 4 CMake module path
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/external/antlr4-cross-platform/packages)

# Find ANTLR 4
find_package(ANTLR4 REQUIRED)

# Your executable
add_executable(your_app main.cpp)

# Link ANTLR 4 runtime
target_link_libraries(your_app antlr4::runtime)

# Alternative manual setup:
# include_directories(${CMAKE_SOURCE_DIR}/external/antlr4-cross-platform/precompiled/linux-x64/include)
# target_link_libraries(your_app ${CMAKE_SOURCE_DIR}/external/antlr4-cross-platform/precompiled/linux-x64/lib/libantlr4-runtime.a)
EOF
    
    print_success "Integration example created: INTEGRATION_EXAMPLE.cmake"
}

# Main function
main() {
    local output_dir="$PACKAGES_DIR"
    local version="$VERSION"
    local include_source=false
    local include_tools=false
    local compress_level=6
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                show_usage
                exit 0
                ;;
            -o|--output)
                output_dir="$2"
                shift 2
                ;;
            -v|--version)
                version="$2"
                VERSION="$version"
                shift 2
                ;;
            --include-source)
                include_source=true
                shift
                ;;
            --include-tools)
                include_tools=true
                shift
                ;;
            --compress)
                compress_level="$2"
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
    
    mkdir -p "$output_dir"
    
    # Validate builds
    if ! validate_builds; then
        exit 1
    fi
    
    print_section "Creating Precompiled Packages"
    
    local platforms=("linux-x64" "macos-x64" "windows-x64")
    local created=0
    local failed=0
    
    # Create packages for each platform
    for platform in "${platforms[@]}"; do
        if create_platform_package "$platform" "$output_dir" "$include_source" "$include_tools" "$compress_level"; then
            ((created++))
        else
            ((failed++))
        fi
    done
    
    # Create CMake integration files
    create_cmake_integration "$output_dir"
    
    # Create unified package with all platforms
    print_section "Creating Unified Package"
    
    local unified_name="antlr4-$VERSION-all-platforms"
    local unified_path="$output_dir/$unified_name.tar.gz"
    
    print_info "Creating unified cross-platform package..."
    
    local temp_dir=$(mktemp -d)
    local temp_unified="$temp_dir/$unified_name"
    
    mkdir -p "$temp_unified"
    
    # Copy all platform builds
    for platform in "${platforms[@]}"; do
        local platform_dir="$PRECOMPILED_DIR/$platform"
        if [ -d "$platform_dir" ]; then
            cp -r "$platform_dir" "$temp_unified/"
            print_info "  Added $platform build"
        fi
    done
    
    # Copy tools and integration files
    [ -d "$ANTLR_ROOT/tools" ] && cp -r "$ANTLR_ROOT/tools" "$temp_unified/"
    [ -f "$output_dir/FindANTLR4.cmake" ] && cp "$output_dir/FindANTLR4.cmake" "$temp_unified/"
    [ -f "$output_dir/INTEGRATION_EXAMPLE.cmake" ] && cp "$output_dir/INTEGRATION_EXAMPLE.cmake" "$temp_unified/"
    
    # Create unified package info
    cat > "$temp_unified/README.md" << EOF
# ANTLR 4 Cross-Platform Precompiled Package

**Version**: $VERSION  
**Created**: $(date)  
**Platforms**: All supported platforms

## Contents
- 📁 linux-x64/ - Linux x86_64 build
- 📁 macos-x64/ - macOS x86_64 build  
- 📁 windows-x64/ - Windows x86_64 build
- 📁 tools/ - ANTLR JAR tools
- 📄 FindANTLR4.cmake - CMake integration module
- 📄 INTEGRATION_EXAMPLE.cmake - Usage example

## Quick Integration
1. Extract to your project: \`external/antlr4/\`
2. Add to CMakeLists.txt: \`find_package(ANTLR4 REQUIRED)\`
3. Link to target: \`target_link_libraries(your_target antlr4::runtime)\`

## Platform Selection
CMake automatically selects the correct platform build:
- Linux → linux-x64/
- macOS → macos-x64/
- Windows → windows-x64/

---
*ANTLR 4 cross-platform package for CHTL integration*
EOF
    
    # Create unified archive
    cd "$temp_dir"
    tar -czf "$unified_path" "$unified_name"
    
    if [ $? -eq 0 ]; then
        local unified_size=$(stat -f%z "$unified_path" 2>/dev/null || stat -c%s "$unified_path" 2>/dev/null || echo "unknown")
        print_success "Unified package created: $unified_name.tar.gz ($unified_size bytes)"
        
        # Create checksum
        if command -v sha256sum >/dev/null 2>&1; then
            cd "$output_dir"
            sha256sum "$(basename "$unified_path")" > "$(basename "$unified_path").sha256"
        fi
    fi
    
    # Cleanup
    rm -rf "$temp_dir"
    
    print_section "Package Creation Summary"
    print_info "Individual packages: $created created, $failed failed"
    print_info "Unified package: $([ -f "$unified_path" ] && echo "✅ Created" || echo "❌ Failed")"
    
    if [ $created -gt 0 ] || [ -f "$unified_path" ]; then
        print_success "Package creation completed successfully"
        
        echo ""
        print_info "📦 Created packages:"
        find "$output_dir" -name "*.tar.gz" | while read -r package; do
            local size=$(stat -f%z "$package" 2>/dev/null || stat -c%s "$package" 2>/dev/null || echo "unknown")
            echo "  📦 $(basename "$package") ($size bytes)"
        done
        
        echo ""
        print_info "🔧 Integration files:"
        [ -f "$output_dir/FindANTLR4.cmake" ] && echo "  📄 FindANTLR4.cmake"
        [ -f "$output_dir/INTEGRATION_EXAMPLE.cmake" ] && echo "  📄 INTEGRATION_EXAMPLE.cmake"
        
        echo ""
        print_success "🎉 ANTLR precompiled packages ready for distribution!"
        print_info "Upload packages to repository or distribute separately"
        
        return 0
    else
        print_error "Package creation failed"
        return 1
    fi
}

# Execute main function
main "$@"