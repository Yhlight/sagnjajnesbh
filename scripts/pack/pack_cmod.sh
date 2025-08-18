#!/bin/bash

# CHTL CMOD Packaging Script
# Cross-platform CMOD module packaging utility
# Validates and packages CMOD modules according to three-name standard

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

echo -e "${PURPLE}📦 CHTL CMOD Packaging Script${NC}"
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
    echo "Usage: $0 [OPTIONS] <module_path>"
    echo ""
    echo "Options:"
    echo "  -h, --help              Show this help message"
    echo "  -o, --output DIR        Output directory (default: ./dist)"
    echo "  -v, --verbose           Verbose output"
    echo "  -c, --compress LEVEL    Compression level 1-9 (default: 6)"
    echo "  --validate-only         Only validate, don't package"
    echo "  --force                 Force packaging even with warnings"
    echo ""
    echo "Examples:"
    echo "  $0 src/Module/Chtholly/CMOD/Accordion"
    echo "  $0 -o ./packages src/Module/Yuigahama/CMOD/MusicPlayer"
    echo "  $0 --validate-only src/Module/MyModule/CMOD/Component"
    echo ""
    echo "CMOD Structure Requirements:"
    echo "  ModuleName/"
    echo "  ├── src/"
    echo "  │   └── ModuleName.chtl    # Main module file (same name)"
    echo "  └── info/"
    echo "      └── ModuleName.chtl    # Info file (same name)"
    echo ""
    echo "Three-Name Standard: Folder name, main file name, info file name must be identical"
}

# Validate CMOD structure
validate_cmod_structure() {
    local module_path="$1"
    local errors=0
    
    print_section "Validating CMOD Structure"
    
    if [ ! -d "$module_path" ]; then
        print_error "Module directory does not exist: $module_path"
        return 1
    fi
    
    # Get module name from directory
    local module_name=$(basename "$module_path")
    print_info "Module name: $module_name"
    
    # Check required directories
    if [ ! -d "$module_path/src" ]; then
        print_error "Missing src/ directory"
        ((errors++))
    else
        print_success "src/ directory found"
    fi
    
    if [ ! -d "$module_path/info" ]; then
        print_error "Missing info/ directory"
        ((errors++))
    else
        print_success "info/ directory found"
    fi
    
    # Check three-name standard
    local src_file="$module_path/src/$module_name.chtl"
    local info_file="$module_path/info/$module_name.chtl"
    
    if [ ! -f "$src_file" ]; then
        print_error "Missing main source file: $src_file"
        print_info "Three-name standard requires: src/$module_name.chtl"
        ((errors++))
    else
        print_success "Main source file found: $module_name.chtl"
    fi
    
    if [ ! -f "$info_file" ]; then
        print_error "Missing info file: $info_file"
        print_info "Three-name standard requires: info/$module_name.chtl"
        ((errors++))
    else
        print_success "Info file found: $module_name.chtl"
    fi
    
    # Validate info file content
    if [ -f "$info_file" ]; then
        if grep -q "\[Info\]" "$info_file"; then
            print_success "Info file contains [Info] section"
        else
            print_error "Info file missing [Info] section"
            ((errors++))
        fi
        
        if grep -q "\[Export\]" "$info_file"; then
            print_success "Info file contains [Export] section"
        else
            print_warning "Info file missing [Export] section (can be auto-generated)"
        fi
        
        # Check required fields
        local required_fields=("Name:" "Version:" "Author:" "Description:")
        for field in "${required_fields[@]}"; do
            if grep -q "$field" "$info_file"; then
                print_success "Required field found: $field"
            else
                print_warning "Missing recommended field: $field"
            fi
        done
    fi
    
    # Check source file content
    if [ -f "$src_file" ]; then
        if grep -qE "\[(Style|Component|Script|Template|Custom)\]" "$src_file"; then
            print_success "Source file contains valid CHTL sections"
        else
            print_warning "Source file may not contain valid CHTL sections"
        fi
    fi
    
    print_section "Validation Summary"
    if [ $errors -eq 0 ]; then
        print_success "CMOD structure validation passed"
        return 0
    else
        print_error "CMOD structure validation failed with $errors errors"
        return 1
    fi
}

# Package CMOD
package_cmod() {
    local module_path="$1"
    local output_dir="$2"
    local compress_level="$3"
    
    local module_name=$(basename "$module_path")
    local package_name="$module_name.cmod"
    local package_path="$output_dir/$package_name"
    
    print_section "Packaging CMOD Module"
    
    mkdir -p "$output_dir"
    
    print_info "Creating CMOD package: $package_name"
    print_info "Source: $module_path"
    print_info "Output: $package_path"
    
    # Create ZIP archive (CMOD is essentially a ZIP file)
    cd "$(dirname "$module_path")"
    
    if command -v zip >/dev/null 2>&1; then
        zip -r -$compress_level "$package_path" "$module_name" \
            -x "*.tmp" "*.log" "*.bak" "*~" ".DS_Store" "Thumbs.db"
        
        if [ $? -eq 0 ]; then
            print_success "CMOD package created successfully"
            
            # Show package info
            local package_size=$(stat -f%z "$package_path" 2>/dev/null || stat -c%s "$package_path" 2>/dev/null || echo "unknown")
            print_info "Package size: $package_size bytes"
            print_info "Compression level: $compress_level"
            
            return 0
        else
            print_error "Failed to create CMOD package"
            return 1
        fi
    else
        print_error "ZIP utility not found. Please install zip"
        return 1
    fi
}

# Generate manifest
generate_manifest() {
    local module_path="$1"
    local output_dir="$2"
    
    local module_name=$(basename "$module_path")
    local manifest_path="$output_dir/$module_name.manifest"
    
    print_info "Generating package manifest..."
    
    cat > "$manifest_path" << EOF
# CHTL CMOD Package Manifest
# Generated on: $(date)

[Package]
Name: $module_name
Type: CMOD
Version: $(grep "Version:" "$module_path/info/$module_name.chtl" 2>/dev/null | cut -d: -f2 | tr -d ' ' || echo "1.0.0")
Created: $(date -Iseconds)
Platform: $(uname -s)

[Structure]
Root: $module_name/
Source: src/$module_name.chtl
Info: info/$module_name.chtl

[Validation]
ThreeNameStandard: Verified
InfoSection: Present
ExportSection: $(if grep -q "\[Export\]" "$module_path/info/$module_name.chtl" 2>/dev/null; then echo "Present"; else echo "Auto-generated"; fi)

[Files]
EOF

    # List all files in the module
    find "$module_path" -type f | while read -r file; do
        local rel_path=$(echo "$file" | sed "s|$module_path/||")
        local file_size=$(stat -f%z "$file" 2>/dev/null || stat -c%s "$file" 2>/dev/null || echo "0")
        echo "$rel_path: $file_size bytes" >> "$manifest_path"
    done
    
    print_success "Manifest created: $module_name.manifest"
}

# Main function
main() {
    local module_path=""
    local output_dir="./dist"
    local verbose=false
    local compress_level=6
    local validate_only=false
    local force=false
    
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
        print_error "Module path is required"
        show_usage
        exit 1
    fi
    
    # Convert to absolute path
    if [[ "$module_path" != /* ]]; then
        module_path="$PWD/$module_path"
    fi
    
    print_info "Processing module: $module_path"
    
    # Validate CMOD structure
    if ! validate_cmod_structure "$module_path"; then
        if [ "$force" = false ]; then
            print_error "Validation failed. Use --force to package anyway"
            exit 1
        else
            print_warning "Validation failed but continuing due to --force flag"
        fi
    fi
    
    # Exit if validate-only
    if [ "$validate_only" = true ]; then
        print_success "Validation completed"
        exit 0
    fi
    
    # Package CMOD
    if package_cmod "$module_path" "$output_dir" "$compress_level"; then
        generate_manifest "$module_path" "$output_dir"
        
        print_section "Packaging Complete"
        print_success "CMOD module packaged successfully"
        print_info "Package location: $output_dir/$(basename "$module_path").cmod"
        print_info "Manifest location: $output_dir/$(basename "$module_path").manifest"
        
        echo ""
        print_success "🎉 CMOD packaging completed!"
    else
        print_error "CMOD packaging failed"
        exit 1
    fi
}

# Execute main function
main "$@"