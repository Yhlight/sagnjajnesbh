#!/bin/bash

# CMOD Packaging Script
# Cross-platform shell script for packaging CHTL CMOD modules

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
    echo "Usage: $0 [OPTIONS] <module_directory>"
    echo ""
    echo "Options:"
    echo "  -o, --output DIR     Output directory for .cmod packages (default: ./packages)"
    echo "  -v, --verbose        Enable verbose output"
    echo "  -h, --help          Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0 src/Module/CMOD/Chtholly"
    echo "  $0 -o /tmp/packages src/Module/CMOD/MyModule"
    echo "  $0 --verbose modules/MyCustomModule"
}

# Parse command line arguments
OUTPUT_DIR="./packages"
VERBOSE=false
MODULE_DIR=""

while [[ $# -gt 0 ]]; do
    case $1 in
        -o|--output)
            OUTPUT_DIR="$2"
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
            if [ -z "$MODULE_DIR" ]; then
                MODULE_DIR="$1"
            else
                print_error "Multiple module directories specified"
                show_usage
                exit 1
            fi
            shift
            ;;
    esac
done

# Validate arguments
if [ -z "$MODULE_DIR" ]; then
    print_error "Module directory not specified"
    show_usage
    exit 1
fi

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

print_status "Starting CMOD Packaging"
print_status "Project root: $PROJECT_ROOT"
print_status "Module directory: $MODULE_DIR"
print_status "Output directory: $OUTPUT_DIR"

# Convert to absolute paths
if [[ "$MODULE_DIR" != /* ]]; then
    MODULE_DIR="$PROJECT_ROOT/$MODULE_DIR"
fi

if [[ "$OUTPUT_DIR" != /* ]]; then
    OUTPUT_DIR="$PROJECT_ROOT/$OUTPUT_DIR"
fi

# Validate module directory
if [ ! -d "$MODULE_DIR" ]; then
    print_error "Module directory does not exist: $MODULE_DIR"
    exit 1
fi

# Get module name from directory
MODULE_NAME=$(basename "$MODULE_DIR")
print_status "Module name: $MODULE_NAME"

# Validate CMOD structure
print_status "Validating CMOD structure..."

# Check for required directories and files
REQUIRED_DIRS=("src" "info")
MISSING_DIRS=()

for dir in "${REQUIRED_DIRS[@]}"; do
    if [ ! -d "$MODULE_DIR/$dir" ]; then
        MISSING_DIRS+=("$dir")
    fi
done

if [ ${#MISSING_DIRS[@]} -gt 0 ]; then
    print_error "Missing required directories: ${MISSING_DIRS[*]}"
    print_error "CMOD structure should be:"
    print_error "  $MODULE_NAME/"
    print_error "  ├── src/"
    print_error "  │   ├── $MODULE_NAME.chtl (optional if submodules exist)"
    print_error "  │   └── [submodules...]"
    print_error "  └── info/"
    print_error "      └── $MODULE_NAME.chtl"
    exit 1
fi

# Check for info file (must match module name)
INFO_FILE="$MODULE_DIR/info/$MODULE_NAME.chtl"
if [ ! -f "$INFO_FILE" ]; then
    print_error "Info file not found: $INFO_FILE"
    print_error "The info file must have the same name as the module directory"
    exit 1
fi

# Check for main module file or submodules
MAIN_FILE="$MODULE_DIR/src/$MODULE_NAME.chtl"
SUBMODULES=($(find "$MODULE_DIR/src" -mindepth 1 -maxdepth 1 -type d 2>/dev/null || true))

if [ ! -f "$MAIN_FILE" ] && [ ${#SUBMODULES[@]} -eq 0 ]; then
    print_error "Neither main module file nor submodules found"
    print_error "Expected: $MAIN_FILE or submodule directories in $MODULE_DIR/src/"
    exit 1
fi

if [ -f "$MAIN_FILE" ]; then
    print_success "Found main module file: $MAIN_FILE"
fi

if [ ${#SUBMODULES[@]} -gt 0 ]; then
    print_success "Found ${#SUBMODULES[@]} submodule(s):"
    for submodule in "${SUBMODULES[@]}"; do
        print_status "  - $(basename "$submodule")"
    done
fi

# Validate info file format
print_status "Validating info file format..."

if ! grep -q "^\[Info\]" "$INFO_FILE"; then
    print_error "Info file missing [Info] section: $INFO_FILE"
    exit 1
fi

# Extract module info
MODULE_INFO_NAME=$(grep -A 10 "^\[Info\]" "$INFO_FILE" | grep "name\s*=" | head -n1 | sed 's/.*name\s*=\s*["\x27]\?\([^"\x27;]*\)["\x27]\?\s*;.*/\1/' | tr -d ' ')
MODULE_VERSION=$(grep -A 10 "^\[Info\]" "$INFO_FILE" | grep "version\s*=" | head -n1 | sed 's/.*version\s*=\s*["\x27]\?\([^"\x27;]*\)["\x27]\?\s*;.*/\1/' | tr -d ' ')

if [ -z "$MODULE_INFO_NAME" ]; then
    print_warning "Module name not found in info file, using directory name"
    MODULE_INFO_NAME="$MODULE_NAME"
fi

if [ -z "$MODULE_VERSION" ]; then
    print_warning "Module version not found in info file, using default"
    MODULE_VERSION="1.0.0"
fi

print_status "Module info name: $MODULE_INFO_NAME"
print_status "Module version: $MODULE_VERSION"

print_success "CMOD structure validation passed"

# Create output directory
mkdir -p "$OUTPUT_DIR"

# Create temporary packaging directory
TEMP_DIR=$(mktemp -d)
PACKAGE_DIR="$TEMP_DIR/$MODULE_NAME"

if [ "$VERBOSE" = true ]; then
    print_status "Using temporary directory: $TEMP_DIR"
fi

# Copy module structure to temporary directory
print_status "Preparing module for packaging..."

mkdir -p "$PACKAGE_DIR"
cp -r "$MODULE_DIR"/* "$PACKAGE_DIR/"

if [ "$VERBOSE" = true ]; then
    print_status "Module structure copied to: $PACKAGE_DIR"
    find "$PACKAGE_DIR" -type f | head -20
fi

# Generate or update [Export] section in info file
print_status "Generating export information..."

INFO_FILE_TEMP="$PACKAGE_DIR/info/$MODULE_NAME.chtl"
EXPORT_CONTENT=""

# Scan for exportable items in main file
if [ -f "$PACKAGE_DIR/src/$MODULE_NAME.chtl" ]; then
    if [ "$VERBOSE" = true ]; then
        print_status "Scanning main module file for exports..."
    fi
    
    # Extract [Template] exports
    TEMPLATES=$(grep -n "^\[Template\]\s*@\w\+" "$PACKAGE_DIR/src/$MODULE_NAME.chtl" | sed 's/.*@\(\w\+\).*/[Template] @\1/' || true)
    if [ ! -z "$TEMPLATES" ]; then
        EXPORT_CONTENT="$EXPORT_CONTENT$TEMPLATES"$'\n'
    fi
    
    # Extract [Custom] exports
    CUSTOMS=$(grep -n "^\[Custom\]\s*@\w\+" "$PACKAGE_DIR/src/$MODULE_NAME.chtl" | sed 's/.*@\(\w\+\).*/[Custom] @\1/' || true)
    if [ ! -z "$CUSTOMS" ]; then
        EXPORT_CONTENT="$EXPORT_CONTENT$CUSTOMS"$'\n'
    fi
fi

# Scan submodules for exports
for submodule in "${SUBMODULES[@]}"; do
    SUBMODULE_NAME=$(basename "$submodule")
    SUBMODULE_FILES=$(find "$PACKAGE_DIR/src/$SUBMODULE_NAME" -name "*.chtl" -type f 2>/dev/null || true)
    
    if [ "$VERBOSE" = true ]; then
        print_status "Scanning submodule: $SUBMODULE_NAME"
    fi
    
    for file in $SUBMODULE_FILES; do
        # Extract exports with submodule prefix
        TEMPLATES=$(grep -n "^\[Template\]\s*@\w\+" "$file" | sed "s/.*@\(\w\+\).*/[Template] @$SUBMODULE_NAME.\1/" || true)
        if [ ! -z "$TEMPLATES" ]; then
            EXPORT_CONTENT="$EXPORT_CONTENT$TEMPLATES"$'\n'
        fi
        
        CUSTOMS=$(grep -n "^\[Custom\]\s*@\w\+" "$file" | sed "s/.*@\(\w\+\).*/[Custom] @$SUBMODULE_NAME.\1/" || true)
        if [ ! -z "$CUSTOMS" ]; then
            EXPORT_CONTENT="$EXPORT_CONTENT$CUSTOMS"$'\n'
        fi
    done
done

# Update info file with export section
if [ ! -z "$EXPORT_CONTENT" ]; then
    # Remove existing [Export] section if present
    sed -i '/^\[Export\]/,/^$/d' "$INFO_FILE_TEMP" 2>/dev/null || true
    
    # Add new [Export] section
    echo "" >> "$INFO_FILE_TEMP"
    echo "[Export]" >> "$INFO_FILE_TEMP"
    echo "{" >> "$INFO_FILE_TEMP"
    echo "$EXPORT_CONTENT" | sed 's/^/    /' | sed 's/$/;/' >> "$INFO_FILE_TEMP"
    echo "}" >> "$INFO_FILE_TEMP"
    
    print_success "Generated export section with $(echo "$EXPORT_CONTENT" | wc -l) items"
else
    print_warning "No exportable items found"
fi

# Create package archive
PACKAGE_NAME="${MODULE_NAME}-${MODULE_VERSION}.cmod"
PACKAGE_PATH="$OUTPUT_DIR/$PACKAGE_NAME"

print_status "Creating CMOD package: $PACKAGE_NAME"

cd "$TEMP_DIR"

# Use available compression tool
if command -v tar &> /dev/null; then
    tar -czf "$PACKAGE_PATH" "$MODULE_NAME"
    COMPRESSION_METHOD="tar+gzip"
elif command -v zip &> /dev/null; then
    zip -r "$PACKAGE_PATH" "$MODULE_NAME" >/dev/null
    COMPRESSION_METHOD="zip"
else
    print_error "No suitable compression tool found (tar or zip required)"
    rm -rf "$TEMP_DIR"
    exit 1
fi

# Cleanup temporary directory
rm -rf "$TEMP_DIR"

# Verify package was created
if [ -f "$PACKAGE_PATH" ]; then
    PACKAGE_SIZE=$(ls -lh "$PACKAGE_PATH" | awk '{print $5}')
    print_success "CMOD package created successfully"
    print_status "Package: $PACKAGE_PATH"
    print_status "Size: $PACKAGE_SIZE"
    print_status "Compression: $COMPRESSION_METHOD"
    
    # Create package info file
    INFO_FILE="$OUTPUT_DIR/$MODULE_NAME-info.txt"
    cat > "$INFO_FILE" << EOF
CMOD Package Information
========================

Module Name: $MODULE_INFO_NAME
Directory Name: $MODULE_NAME
Version: $MODULE_VERSION
Package File: $PACKAGE_NAME
Size: $PACKAGE_SIZE
Compression: $COMPRESSION_METHOD
Created: $(date)

Structure Validation: PASSED
Export Generation: $([ ! -z "$EXPORT_CONTENT" ] && echo "SUCCESS" || echo "NO EXPORTS")

Installation:
  1. Copy $PACKAGE_NAME to your CHTL module directory
  2. Use CHTL compiler to unpack: chtl --unpack-cmod $PACKAGE_NAME
  3. Import in your CHTL files: [Import] @$MODULE_INFO_NAME;
EOF
    
    print_success "Package info saved: $INFO_FILE"
    
    # Create verification script
    VERIFY_SCRIPT="$OUTPUT_DIR/verify-$MODULE_NAME.sh"
    cat > "$VERIFY_SCRIPT" << EOF
#!/bin/bash
# CMOD Package Verification Script

PACKAGE="$PACKAGE_PATH"
TEMP_DIR=\$(mktemp -d)

echo "Verifying CMOD package: \$PACKAGE"

if [ ! -f "\$PACKAGE" ]; then
    echo "ERROR: Package file not found"
    exit 1
fi

cd "\$TEMP_DIR"

if [[ "\$PACKAGE" == *.tar.gz ]] || [[ "\$PACKAGE" == *.cmod ]]; then
    tar -tzf "\$PACKAGE" | head -20
else
    unzip -l "\$PACKAGE" | head -20
fi

echo "Package contents verified"
rm -rf "\$TEMP_DIR"
EOF
    
    chmod +x "$VERIFY_SCRIPT"
    print_success "Verification script created: $VERIFY_SCRIPT"
    
else
    print_error "Failed to create package"
    exit 1
fi

print_success "CMOD packaging completed successfully"

exit 0