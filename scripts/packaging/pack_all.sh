#!/bin/bash

# Unified Packaging Script
# Cross-platform shell script for packaging all CHTL modules and components

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
    echo "Options:"
    echo "  -o, --output DIR     Output directory for packages (default: ./packages)"
    echo "  -c, --compile        Compile C++ sources for CJMOD modules"
    echo "  -m, --modules DIR    Module source directory (default: src/Module)"
    echo "  -v, --verbose        Enable verbose output"
    echo "  --cmod-only         Package only CMOD modules"
    echo "  --cjmod-only        Package only CJMOD modules"
    echo "  -h, --help          Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0                              # Package all modules"
    echo "  $0 -o /tmp/packages --compile   # Package all with compilation"
    echo "  $0 --cmod-only -v               # Package only CMOD modules verbosely"
}

# Parse command line arguments
OUTPUT_DIR="./packages"
COMPILE=false
MODULE_SOURCE_DIR="src/Module"
VERBOSE=false
CMOD_ONLY=false
CJMOD_ONLY=false

while [[ $# -gt 0 ]]; do
    case $1 in
        -o|--output)
            OUTPUT_DIR="$2"
            shift 2
            ;;
        -c|--compile)
            COMPILE=true
            shift
            ;;
        -m|--modules)
            MODULE_SOURCE_DIR="$2"
            shift 2
            ;;
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        --cmod-only)
            CMOD_ONLY=true
            shift
            ;;
        --cjmod-only)
            CJMOD_ONLY=true
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

# Validate conflicting options
if [ "$CMOD_ONLY" = true ] && [ "$CJMOD_ONLY" = true ]; then
    print_error "Cannot specify both --cmod-only and --cjmod-only"
    exit 1
fi

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

print_status "Starting Unified Packaging Process"
print_status "Project root: $PROJECT_ROOT"
print_status "Module source directory: $MODULE_SOURCE_DIR"
print_status "Output directory: $OUTPUT_DIR"
print_status "Compile CJMOD: $COMPILE"
print_status "CMOD only: $CMOD_ONLY"
print_status "CJMOD only: $CJMOD_ONLY"

# Convert to absolute paths
if [[ "$MODULE_SOURCE_DIR" != /* ]]; then
    MODULE_SOURCE_DIR="$PROJECT_ROOT/$MODULE_SOURCE_DIR"
fi

if [[ "$OUTPUT_DIR" != /* ]]; then
    OUTPUT_DIR="$PROJECT_ROOT/$OUTPUT_DIR"
fi

# Check if module source directory exists
if [ ! -d "$MODULE_SOURCE_DIR" ]; then
    print_error "Module source directory does not exist: $MODULE_SOURCE_DIR"
    exit 1
fi

# Create output directory
mkdir -p "$OUTPUT_DIR"

# Initialize counters
CMOD_COUNT=0
CJMOD_COUNT=0
CMOD_SUCCESS=0
CJMOD_SUCCESS=0
CMOD_FAILED=0
CJMOD_FAILED=0

# Find CMOD modules
CMOD_MODULES=()
if [ "$CJMOD_ONLY" != true ]; then
    print_status "Scanning for CMOD modules..."
    
    # Check for different case variants
    for variant in "CMOD" "Cmod" "cmod"; do
        if [ -d "$MODULE_SOURCE_DIR/$variant" ]; then
            while IFS= read -r -d '' module_dir; do
                if [ -d "$module_dir/src" ] && [ -d "$module_dir/info" ]; then
                    CMOD_MODULES+=("$module_dir")
                    ((CMOD_COUNT++))
                    if [ "$VERBOSE" = true ]; then
                        print_status "Found CMOD module: $(basename "$module_dir")"
                    fi
                fi
            done < <(find "$MODULE_SOURCE_DIR/$variant" -mindepth 1 -maxdepth 1 -type d -print0 2>/dev/null)
        fi
    done
fi

# Find CJMOD modules
CJMOD_MODULES=()
if [ "$CMOD_ONLY" != true ]; then
    print_status "Scanning for CJMOD modules..."
    
    # Check for different case variants
    for variant in "CJMOD" "CJmod" "cjmod"; do
        if [ -d "$MODULE_SOURCE_DIR/$variant" ]; then
            while IFS= read -r -d '' module_dir; do
                if [ -d "$module_dir/src" ] && [ -d "$module_dir/info" ]; then
                    # Check for C++ files
                    if find "$module_dir/src" -name "*.cpp" -type f | head -n1 | grep -q .; then
                        CJMOD_MODULES+=("$module_dir")
                        ((CJMOD_COUNT++))
                        if [ "$VERBOSE" = true ]; then
                            print_status "Found CJMOD module: $(basename "$module_dir")"
                        fi
                    fi
                fi
            done < <(find "$MODULE_SOURCE_DIR/$variant" -mindepth 1 -maxdepth 1 -type d -print0 2>/dev/null)
        fi
    done
fi

print_status "Found $CMOD_COUNT CMOD module(s) and $CJMOD_COUNT CJMOD module(s)"

if [ $CMOD_COUNT -eq 0 ] && [ $CJMOD_COUNT -eq 0 ]; then
    print_warning "No modules found to package"
    print_status "Expected structure:"
    print_status "  $MODULE_SOURCE_DIR/"
    print_status "  ├── CMOD/ (or Cmod/, cmod/)"
    print_status "  │   └── ModuleName/"
    print_status "  │       ├── src/"
    print_status "  │       └── info/"
    print_status "  └── CJMOD/ (or CJmod/, cjmod/)"
    print_status "      └── ModuleName/"
    print_status "          ├── src/ (with .cpp files)"
    print_status "          └── info/"
    exit 0
fi

# Package CMOD modules
if [ $CMOD_COUNT -gt 0 ]; then
    print_status "Packaging CMOD modules..."
    
    for module_dir in "${CMOD_MODULES[@]}"; do
        MODULE_NAME=$(basename "$module_dir")
        print_status "Packaging CMOD module: $MODULE_NAME"
        
        # Build command arguments
        PACK_ARGS=()
        PACK_ARGS+=("-o" "$OUTPUT_DIR")
        
        if [ "$VERBOSE" = true ]; then
            PACK_ARGS+=("--verbose")
        fi
        
        PACK_ARGS+=("$module_dir")
        
        # Run CMOD packaging script
        if "$SCRIPT_DIR/pack_cmod.sh" "${PACK_ARGS[@]}"; then
            ((CMOD_SUCCESS++))
            print_success "CMOD module packaged: $MODULE_NAME"
        else
            ((CMOD_FAILED++))
            print_error "Failed to package CMOD module: $MODULE_NAME"
        fi
    done
fi

# Package CJMOD modules
if [ $CJMOD_COUNT -gt 0 ]; then
    print_status "Packaging CJMOD modules..."
    
    for module_dir in "${CJMOD_MODULES[@]}"; do
        MODULE_NAME=$(basename "$module_dir")
        print_status "Packaging CJMOD module: $MODULE_NAME"
        
        # Build command arguments
        PACK_ARGS=()
        PACK_ARGS+=("-o" "$OUTPUT_DIR")
        
        if [ "$COMPILE" = true ]; then
            PACK_ARGS+=("--compile")
        fi
        
        if [ "$VERBOSE" = true ]; then
            PACK_ARGS+=("--verbose")
        fi
        
        PACK_ARGS+=("$module_dir")
        
        # Run CJMOD packaging script
        if "$SCRIPT_DIR/pack_cjmod.sh" "${PACK_ARGS[@]}"; then
            ((CJMOD_SUCCESS++))
            print_success "CJMOD module packaged: $MODULE_NAME"
        else
            ((CJMOD_FAILED++))
            print_error "Failed to package CJMOD module: $MODULE_NAME"
        fi
    done
fi

# Generate summary report
TOTAL_SUCCESS=$((CMOD_SUCCESS + CJMOD_SUCCESS))
TOTAL_FAILED=$((CMOD_FAILED + CJMOD_FAILED))
TOTAL_MODULES=$((CMOD_COUNT + CJMOD_COUNT))

print_status "Packaging Summary:"
print_status "=================="
print_status "Total modules found: $TOTAL_MODULES"
print_status "CMOD modules: $CMOD_COUNT (Success: $CMOD_SUCCESS, Failed: $CMOD_FAILED)"
print_status "CJMOD modules: $CJMOD_COUNT (Success: $CJMOD_SUCCESS, Failed: $CJMOD_FAILED)"
print_status "Overall success rate: $TOTAL_SUCCESS/$TOTAL_MODULES"

# Create package index
PACKAGE_INDEX="$OUTPUT_DIR/package-index.txt"
cat > "$PACKAGE_INDEX" << EOF
CHTL Module Package Index
=========================
Generated: $(date)

Summary:
  Total Packages: $TOTAL_SUCCESS
  CMOD Packages: $CMOD_SUCCESS
  CJMOD Packages: $CJMOD_SUCCESS
  Failed: $TOTAL_FAILED

Packages:
EOF

# List all package files
if [ -d "$OUTPUT_DIR" ]; then
    for package in "$OUTPUT_DIR"/*.cmod "$OUTPUT_DIR"/*.cjmod; do
        if [ -f "$package" ]; then
            PACKAGE_NAME=$(basename "$package")
            PACKAGE_SIZE=$(ls -lh "$package" | awk '{print $5}')
            echo "  - $PACKAGE_NAME ($PACKAGE_SIZE)" >> "$PACKAGE_INDEX"
        fi
    done
fi

print_success "Package index created: $PACKAGE_INDEX"

# Create installation script
INSTALL_SCRIPT="$OUTPUT_DIR/install-all-modules.sh"
cat > "$INSTALL_SCRIPT" << 'EOF'
#!/bin/bash
# CHTL Module Installation Script

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "Installing all CHTL modules..."

if ! command -v chtl_compiler &> /dev/null; then
    echo "ERROR: CHTL compiler not found in PATH"
    echo "Please ensure CHTL compiler is installed and available"
    exit 1
fi

INSTALLED=0
FAILED=0

for package in "$SCRIPT_DIR"/*.cmod "$SCRIPT_DIR"/*.cjmod; do
    if [ -f "$package" ]; then
        PACKAGE_NAME=$(basename "$package")
        echo "Installing: $PACKAGE_NAME"
        
        if [[ "$package" == *.cmod ]]; then
            if chtl_compiler --unpack-cmod "$package"; then
                ((INSTALLED++))
                echo "SUCCESS: $PACKAGE_NAME installed"
            else
                ((FAILED++))
                echo "ERROR: Failed to install $PACKAGE_NAME"
            fi
        elif [[ "$package" == *.cjmod ]]; then
            if chtl_compiler --unpack-cjmod "$package"; then
                ((INSTALLED++))
                echo "SUCCESS: $PACKAGE_NAME installed"
            else
                ((FAILED++))
                echo "ERROR: Failed to install $PACKAGE_NAME"
            fi
        fi
    fi
done

echo "Installation complete: $INSTALLED installed, $FAILED failed"
EOF

chmod +x "$INSTALL_SCRIPT"
print_success "Installation script created: $INSTALL_SCRIPT"

# Final status
if [ $TOTAL_FAILED -eq 0 ]; then
    print_success "All modules packaged successfully!"
    exit 0
else
    print_warning "$TOTAL_FAILED module(s) failed to package"
    exit 1
fi