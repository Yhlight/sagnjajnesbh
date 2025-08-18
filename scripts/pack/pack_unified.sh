#!/bin/bash

# CHTL Unified Packaging Script
# Cross-platform unified packaging utility for both CMOD and CJMOD modules
# Automatically detects module type and applies appropriate packaging

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

echo -e "${PURPLE}📦 CHTL Unified Packaging Script${NC}"
echo -e "${BLUE}Project Root: $PROJECT_ROOT${NC}"
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
    echo "Usage: $0 [OPTIONS] <module_path_or_directory>"
    echo ""
    echo "Options:"
    echo "  -h, --help              Show this help message"
    echo "  -o, --output DIR        Output directory (default: ./dist)"
    echo "  -v, --verbose           Verbose output"
    echo "  -c, --compress LEVEL    Compression level 1-9 (default: 6)"
    echo "  --validate-only         Only validate, don't package"
    echo "  --force                 Force packaging even with warnings"
    echo "  --include-source        Include C++ source files in CJMOD packages"
    echo "  --batch                 Process all modules in directory"
    echo "  --type TYPE             Force module type (cmod|cjmod)"
    echo ""
    echo "Examples:"
    echo "  $0 src/Module/Chtholly/CMOD/Accordion"
    echo "  $0 src/Module/Chtholly/CJMOD/Chtholly"
    echo "  $0 --batch src/Module/Chtholly"
    echo "  $0 --batch --output ./packages src/Module"
    echo ""
    echo "Module Type Detection:"
    echo "  🎨 CMOD: Has info/ directory with ModuleName.chtl"
    echo "  ⚙️ CJMOD: Has ModuleName.chtl in root and src/ with C++ files"
    echo "  🔄 Hybrid: Contains both CMOD/ and CJMOD/ subdirectories"
    echo ""
    echo "Supported Structures:"
    echo "  ModuleName/                    # CMOD"
    echo "  ├── src/ModuleName.chtl"
    echo "  └── info/ModuleName.chtl"
    echo ""
    echo "  ModuleName/                    # CJMOD"
    echo "  ├── src/*.cpp, *.h"
    echo "  └── ModuleName.chtl"
    echo ""
    echo "  ModuleName/                    # Hybrid"
    echo "  ├── CMOD/Component/"
    echo "  └── CJMOD/Component/"
}

# Detect module type
detect_module_type() {
    local module_path="$1"
    
    # Check for hybrid structure (CMOD + CJMOD)
    if [ -d "$module_path/CMOD" ] && [ -d "$module_path/CJMOD" ]; then
        echo "hybrid"
        return 0
    fi
    
    # Check for CMOD structure (info/ directory)
    if [ -d "$module_path/info" ]; then
        echo "cmod"
        return 0
    fi
    
    # Check for CJMOD structure (root info file + src/)
    local module_name=$(basename "$module_path")
    if [ -f "$module_path/$module_name.chtl" ] && [ -d "$module_path/src" ]; then
        echo "cjmod"
        return 0
    fi
    
    echo "unknown"
    return 1
}

# Package single module
package_single_module() {
    local module_path="$1"
    local output_dir="$2"
    local options="$3"
    
    local module_name=$(basename "$module_path")
    local module_type=$(detect_module_type "$module_path")
    
    print_section "Processing Module: $module_name"
    print_info "Detected type: $module_type"
    
    case "$module_type" in
        "cmod")
            print_info "Packaging as CMOD module..."
            # Filter options for CMOD (remove CJMOD-specific options)
            local cmod_options=$(echo "$options" | sed 's/--include-source//g')
            "$SCRIPT_DIR/pack_cmod.sh" $cmod_options -o "$output_dir" "$module_path"
            ;;
        "cjmod")
            print_info "Packaging as CJMOD module..."
            "$SCRIPT_DIR/pack_cjmod.sh" $options -o "$output_dir" "$module_path"
            ;;
        "hybrid")
            print_info "Processing hybrid module (CMOD + CJMOD)..."
            
            # Package CMOD components
            for cmod_component in "$module_path/CMOD"/*; do
                if [ -d "$cmod_component" ]; then
                    print_info "Packaging CMOD component: $(basename "$cmod_component")"
                    local cmod_options=$(echo "$options" | sed 's/--include-source//g')
                    "$SCRIPT_DIR/pack_cmod.sh" $cmod_options -o "$output_dir" "$cmod_component"
                fi
            done
            
            # Package CJMOD components
            for cjmod_component in "$module_path/CJMOD"/*; do
                if [ -d "$cjmod_component" ]; then
                    print_info "Packaging CJMOD component: $(basename "$cjmod_component")"
                    "$SCRIPT_DIR/pack_cjmod.sh" $options -o "$output_dir" "$cjmod_component"
                fi
            done
            ;;
        "unknown")
            print_error "Unknown module type for: $module_path"
            print_info "Please check module structure and naming conventions"
            return 1
            ;;
    esac
}

# Batch processing
process_batch() {
    local base_dir="$1"
    local output_dir="$2"
    local options="$3"
    
    print_section "Batch Processing Modules"
    print_info "Scanning directory: $base_dir"
    
    local processed=0
    local failed=0
    
    # Find all potential module directories
    find "$base_dir" -type d -name "CMOD" -o -name "CJMOD" | while read -r module_type_dir; do
        local parent_dir=$(dirname "$module_type_dir")
        local type_name=$(basename "$module_type_dir")
        
        print_info "Found $type_name directory in: $parent_dir"
        
        # Process each component in CMOD/CJMOD directory
        for component in "$module_type_dir"/*; do
            if [ -d "$component" ]; then
                local component_name=$(basename "$component")
                
                # Skip info directories (these are not modules themselves)
                if [ "$component_name" = "info" ] || [ "$component_name" = "src" ]; then
                    continue
                fi
                
                print_info "Processing component: $component_name"
                
                if package_single_module "$component" "$output_dir" "$options"; then
                    ((processed++))
                    print_success "Successfully packaged: $component_name"
                else
                    ((failed++))
                    print_error "Failed to package: $component_name"
                fi
            fi
        done
    done
    
    # Also check for direct modules (not in CMOD/CJMOD subdirectories)
    for potential_module in "$base_dir"/*; do
        if [ -d "$potential_module" ]; then
            local module_name=$(basename "$potential_module")
            
            # Skip CMOD/CJMOD directories (already processed above)
            if [ "$module_name" = "CMOD" ] || [ "$module_name" = "CJMOD" ]; then
                continue
            fi
            
            # Check if it's a valid module
            local module_type=$(detect_module_type "$potential_module")
            if [ "$module_type" != "unknown" ]; then
                print_info "Processing direct module: $module_name"
                
                if package_single_module "$potential_module" "$output_dir" "$options"; then
                    ((processed++))
                    print_success "Successfully packaged: $module_name"
                else
                    ((failed++))
                    print_error "Failed to package: $module_name"
                fi
            fi
        fi
    done
    
    print_section "Batch Processing Summary"
    print_info "Processed modules: $processed"
    if [ $failed -gt 0 ]; then
        print_warning "Failed modules: $failed"
    fi
}

# Main function
main() {
    local module_path=""
    local output_dir="./dist"
    local verbose=false
    local compress_level=6
    local validate_only=false
    local force=false
    local include_source=true
    local batch=false
    local force_type=""
    
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
            -v|--verbose)
                verbose=true
                shift
                ;;
            -c|--compress)
                compress_level="$2"
                shift 2
                ;;
            --validate-only)
                validate_only=true
                shift
                ;;
            --force)
                force=true
                shift
                ;;
            --include-source)
                include_source=true
                shift
                ;;
            --batch)
                batch=true
                shift
                ;;
            --type)
                force_type="$2"
                shift 2
                ;;
            -*)
                print_error "Unknown option: $1"
                show_usage
                exit 1
                ;;
            *)
                module_path="$1"
                shift
                ;;
        esac
    done
    
    # Check if module path is provided
    if [ -z "$module_path" ]; then
        print_error "Module path or directory is required"
        show_usage
        exit 1
    fi
    
    # Convert to absolute path
    if [[ "$module_path" != /* ]]; then
        module_path="$PWD/$module_path"
    fi
    
    # Prepare options string
    local options=""
    [ "$verbose" = true ] && options="$options -v"
    [ "$validate_only" = true ] && options="$options --validate-only"
    [ "$force" = true ] && options="$options --force"
    [ "$include_source" = true ] && options="$options --include-source"
    options="$options -c $compress_level"
    
    # Execute based on mode
    if [ "$batch" = true ]; then
        process_batch "$module_path" "$output_dir" "$options"
    else
        package_single_module "$module_path" "$output_dir" "$options"
    fi
    
    print_section "Unified Packaging Complete"
    print_success "All packaging operations completed"
    print_info "Output directory: $output_dir"
    
    # List created packages
    if [ -d "$output_dir" ]; then
        local cmod_count=$(find "$output_dir" -name "*.cmod" | wc -l)
        local cjmod_count=$(find "$output_dir" -name "*.cjmod" | wc -l)
        
        print_info "Created packages:"
        echo "  📦 CMOD packages: $cmod_count"
        echo "  ⚙️ CJMOD packages: $cjmod_count"
        echo "  📋 Total packages: $((cmod_count + cjmod_count))"
    fi
    
    echo ""
    print_success "🎉 Unified packaging completed!"
}

# Execute main function
main "$@"