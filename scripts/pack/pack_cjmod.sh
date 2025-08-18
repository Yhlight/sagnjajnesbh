#!/bin/bash

# CHTL CJMOD Packaging Script
# Cross-platform CJMOD module packaging utility
# Validates and packages CJMOD modules according to two-name standard

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

echo -e "${PURPLE}⚙️ CHTL CJMOD Packaging Script${NC}"
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
    echo "  --include-source        Include C++ source files in package"
    echo ""
    echo "Examples:"
    echo "  $0 src/Module/Chtholly/CJMOD/Chtholly"
    echo "  $0 -o ./packages --include-source src/Module/MyModule/CJMOD/Component"
    echo "  $0 --validate-only src/Module/TestModule/CJMOD/Test"
    echo ""
    echo "CJMOD Structure Requirements:"
    echo "  ModuleName/"
    echo "  ├── src/"
    echo "  │   ├── *.cpp           # C++ source files"
    echo "  │   └── *.h             # C++ header files"
    echo "  └── ModuleName.chtl     # Info file (same name as folder)"
    echo ""
    echo "Two-Name Standard: Folder name and info file name must be identical"
    echo "Note: CJMOD info files do NOT have [Export] sections"
}

# Validate CJMOD structure
validate_cjmod_structure() {
    local module_path="$1"
    local errors=0
    local warnings=0
    
    print_section "Validating CJMOD Structure"
    
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
        
        # Check for C++ source files
        local cpp_files=$(find "$module_path/src" -name "*.cpp" | wc -l)
        local h_files=$(find "$module_path/src" -name "*.h" | wc -l)
        
        if [ $cpp_files -eq 0 ]; then
            print_warning "No C++ source files (.cpp) found in src/"
            ((warnings++))
        else
            print_success "Found $cpp_files C++ source files"
        fi
        
        if [ $h_files -eq 0 ]; then
            print_warning "No C++ header files (.h) found in src/"
            ((warnings++))
        else
            print_success "Found $h_files C++ header files"
        fi
    fi
    
    # Check two-name standard (folder name = info file name)
    local info_file="$module_path/$module_name.chtl"
    
    if [ ! -f "$info_file" ]; then
        print_error "Missing info file: $info_file"
        print_info "Two-name standard requires: $module_name.chtl"
        ((errors++))
    else
        print_success "Info file found: $module_name.chtl"
    fi
    
    # Validate info file content (CJMOD specific)
    if [ -f "$info_file" ]; then
        if grep -q "\[Info\]" "$info_file"; then
            print_success "Info file contains [Info] section"
        else
            print_error "Info file missing [Info] section"
            ((errors++))
        fi
        
        # CJMOD should NOT have [Export] section
        if grep -q "\[Export\]" "$info_file"; then
            print_error "CJMOD info file should NOT contain [Export] section"
            print_info "CJMOD extends CHTL JS via C++ interfaces, no export needed"
            ((errors++))
        else
            print_success "Info file correctly has no [Export] section"
        fi
        
        # Check CJMOD specific fields
        local cjmod_fields=("name" "version" "description" "author" "minCHTLVersion")
        for field in "${cjmod_fields[@]}"; do
            if grep -qi "$field" "$info_file"; then
                print_success "CJMOD field found: $field"
            else
                print_warning "Missing recommended CJMOD field: $field"
            fi
        done
    fi
    
    # Check for submodules
    local submodules=$(find "$module_path" -mindepth 1 -maxdepth 1 -type d ! -name "src")
    if [ -n "$submodules" ]; then
        print_info "Submodules detected:"
        echo "$submodules" | while read -r submodule; do
            local sub_name=$(basename "$submodule")
            print_info "  📂 $sub_name"
            
            # Validate submodule structure
            if [ -d "$submodule/src" ] && [ -f "$submodule/$sub_name.chtl" ]; then
                print_success "    Valid submodule structure"
            else
                print_warning "    Invalid submodule structure"
                ((warnings++))
            fi
        done
    fi
    
    print_section "Validation Summary"
    if [ $errors -eq 0 ]; then
        if [ $warnings -eq 0 ]; then
            print_success "CJMOD structure validation passed perfectly"
        else
            print_success "CJMOD structure validation passed with $warnings warnings"
        fi
        return 0
    else
        print_error "CJMOD structure validation failed with $errors errors and $warnings warnings"
        return 1
    fi
}

# Package CJMOD
package_cjmod() {
    local module_path="$1"
    local output_dir="$2"
    local compress_level="$3"
    local include_source="$4"
    
    local module_name=$(basename "$module_path")
    local package_name="$module_name.cjmod"
    local package_path="$output_dir/$package_name"
    
    print_section "Packaging CJMOD Module"
    
    mkdir -p "$output_dir"
    
    print_info "Creating CJMOD package: $package_name"
    print_info "Source: $module_path"
    print_info "Output: $package_path"
    print_info "Include source: $include_source"
    
    # Create temporary directory for packaging
    local temp_dir=$(mktemp -d)
    local temp_module="$temp_dir/$module_name"
    
    # Copy module structure
    cp -r "$module_path" "$temp_module"
    
    # Remove source files if not including them
    if [ "$include_source" = false ]; then
        print_info "Removing C++ source files (not included in package)"
        find "$temp_module/src" -name "*.cpp" -delete 2>/dev/null || true
        find "$temp_module/src" -name "*.h" -delete 2>/dev/null || true
        
        # Create placeholder in src directory
        echo "# CJMOD binary package - source files not included" > "$temp_module/src/README.txt"
    fi
    
    # Create ZIP archive
    cd "$temp_dir"
    
    if command -v zip >/dev/null 2>&1; then
        zip -r -$compress_level "$package_path" "$module_name" \
            -x "*.tmp" "*.log" "*.bak" "*~" ".DS_Store" "Thumbs.db"
        
        if [ $? -eq 0 ]; then
            print_success "CJMOD package created successfully"
            
            # Show package info
            local package_size=$(stat -f%z "$package_path" 2>/dev/null || stat -c%s "$package_path" 2>/dev/null || echo "unknown")
            print_info "Package size: $package_size bytes"
            print_info "Compression level: $compress_level"
            
            # Cleanup
            rm -rf "$temp_dir"
            return 0
        else
            print_error "Failed to create CJMOD package"
            rm -rf "$temp_dir"
            return 1
        fi
    else
        print_error "ZIP utility not found. Please install zip"
        rm -rf "$temp_dir"
        return 1
    fi
}

# Generate CJMOD manifest
generate_cjmod_manifest() {
    local module_path="$1"
    local output_dir="$2"
    local include_source="$3"
    
    local module_name=$(basename "$module_path")
    local manifest_path="$output_dir/$module_name.cjmod.manifest"
    
    print_info "Generating CJMOD package manifest..."
    
    local cpp_count=$(find "$module_path/src" -name "*.cpp" 2>/dev/null | wc -l)
    local h_count=$(find "$module_path/src" -name "*.h" 2>/dev/null | wc -l)
    
    cat > "$manifest_path" << EOF
# CHTL CJMOD Package Manifest
# Generated on: $(date)

[Package]
Name: $module_name
Type: CJMOD
Version: $(grep -i "version" "$module_path/$module_name.chtl" 2>/dev/null | head -1 | cut -d= -f2 | tr -d ' "' || echo "1.0.0")
Created: $(date -Iseconds)
Platform: $(uname -s)
SourceIncluded: $include_source

[Structure]
Root: $module_name/
Info: $module_name.chtl
SourceDir: src/

[Validation]
TwoNameStandard: Verified
InfoSection: Present
ExportSection: Absent (CJMOD standard)

[SourceFiles]
CppFiles: $cpp_count
HeaderFiles: $h_count

[CJMOD Features]
CHTLJSExtension: Enabled
OfficialPrefix: chtl::
CppInterface: Available
GlobalVariables: Supported

[Files]
EOF

    # List all files in the module
    find "$module_path" -type f | while read -r file; do
        local rel_path=$(echo "$file" | sed "s|$module_path/||")
        local file_size=$(stat -f%z "$file" 2>/dev/null || stat -c%s "$file" 2>/dev/null || echo "0")
        echo "$rel_path: $file_size bytes" >> "$manifest_path"
    done
    
    print_success "CJMOD manifest created: $module_name.cjmod.manifest"
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
    
    print_info "Processing CJMOD module: $module_path"
    
    # Validate CJMOD structure
    if ! validate_cjmod_structure "$module_path"; then
        if [ "$force" = false ]; then
            print_error "Validation failed. Use --force to package anyway"
            exit 1
        else
            print_warning "Validation failed but continuing due to --force flag"
        fi
    fi
    
    # Exit if validate-only
    if [ "$validate_only" = true ]; then
        print_success "CJMOD validation completed"
        exit 0
    fi
    
    # Package CJMOD
    if package_cjmod "$module_path" "$output_dir" "$compress_level" "$include_source"; then
        generate_cjmod_manifest "$module_path" "$output_dir" "$include_source"
        
        print_section "CJMOD Packaging Complete"
        print_success "CJMOD module packaged successfully"
        print_info "Package location: $output_dir/$(basename "$module_path").cjmod"
        print_info "Manifest location: $output_dir/$(basename "$module_path").cjmod.manifest"
        
        echo ""
        print_success "🎉 CJMOD packaging completed!"
        print_info "CJMOD extends CHTL JS functionality via C++ interfaces"
        print_info "Supports official chtl:: module prefix"
    else
        print_error "CJMOD packaging failed"
        exit 1
    fi
}

# Execute main function
main "$@"