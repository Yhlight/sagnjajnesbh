#!/bin/bash

# CJMOD Packaging Script
# Cross-platform shell script for packaging CHTL CJMOD modules

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
    echo "  -o, --output DIR     Output directory for .cjmod packages (default: ./packages)"
    echo "  -c, --compile        Compile C++ sources before packaging"
    echo "  -v, --verbose        Enable verbose output"
    echo "  -h, --help          Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0 src/Module/CJMOD/Chtholly"
    echo "  $0 -o /tmp/packages --compile src/Module/CJMOD/MyModule"
    echo "  $0 --verbose modules/MyCustomCJModule"
}

# Parse command line arguments
OUTPUT_DIR="./packages"
COMPILE=false
VERBOSE=false
MODULE_DIR=""

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

print_status "Starting CJMOD Packaging"
print_status "Project root: $PROJECT_ROOT"
print_status "Module directory: $MODULE_DIR"
print_status "Output directory: $OUTPUT_DIR"
print_status "Compile sources: $COMPILE"

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

# Validate CJMOD structure
print_status "Validating CJMOD structure..."

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
    print_error "CJMOD structure should be:"
    print_error "  $MODULE_NAME/"
    print_error "  ├── src/"
    print_error "  │   ├── *.cpp (C++ source files)"
    print_error "  │   ├── *.h (C++ header files)"
    print_error "  │   └── CMakeLists.txt (optional)"
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

# Check for C++ source files
CPP_FILES=($(find "$MODULE_DIR/src" -name "*.cpp" -type f 2>/dev/null || true))
H_FILES=($(find "$MODULE_DIR/src" -name "*.h" -o -name "*.hpp" -type f 2>/dev/null || true))

if [ ${#CPP_FILES[@]} -eq 0 ]; then
    print_error "No C++ source files found in $MODULE_DIR/src/"
    print_error "CJMOD modules require C++ implementation files"
    exit 1
fi

print_success "Found ${#CPP_FILES[@]} C++ source file(s):"
for cpp_file in "${CPP_FILES[@]}"; do
    print_status "  - $(basename "$cpp_file")"
done

if [ ${#H_FILES[@]} -gt 0 ]; then
    print_success "Found ${#H_FILES[@]} header file(s):"
    for h_file in "${H_FILES[@]}"; do
        print_status "  - $(basename "$h_file")"
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
MODULE_DESCRIPTION=$(grep -A 10 "^\[Info\]" "$INFO_FILE" | grep "description\s*=" | head -n1 | sed 's/.*description\s*=\s*["\x27]\?\([^"\x27;]*\)["\x27]\?\s*;.*/\1/')

if [ -z "$MODULE_INFO_NAME" ]; then
    print_warning "Module name not found in info file, using directory name"
    MODULE_INFO_NAME="$MODULE_NAME"
fi

if [ -z "$MODULE_VERSION" ]; then
    print_warning "Module version not found in info file, using default"
    MODULE_VERSION="1.0.0"
fi

if [ -z "$MODULE_DESCRIPTION" ]; then
    MODULE_DESCRIPTION="CJMOD extension module"
fi

print_status "Module info name: $MODULE_INFO_NAME"
print_status "Module version: $MODULE_VERSION"
print_status "Module description: $MODULE_DESCRIPTION"

print_success "CJMOD structure validation passed"

# Check for compilation requirements if compile flag is set
if [ "$COMPILE" = true ]; then
    print_status "Checking compilation requirements..."
    
    if ! command -v cmake &> /dev/null; then
        print_error "CMake is required for compilation but not found"
        exit 1
    fi
    
    if ! command -v make &> /dev/null && ! command -v ninja &> /dev/null; then
        print_error "Build system (make or ninja) is required for compilation but not found"
        exit 1
    fi
    
    # Check for C++ compiler
    if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
        print_error "C++ compiler (g++ or clang++) is required for compilation but not found"
        exit 1
    fi
    
    print_success "Compilation requirements verified"
fi

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
fi

# Compile C++ sources if requested
if [ "$COMPILE" = true ]; then
    print_status "Compiling C++ sources..."
    
    BUILD_DIR="$PACKAGE_DIR/build"
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    # Create CMakeLists.txt if it doesn't exist
    if [ ! -f "$PACKAGE_DIR/src/CMakeLists.txt" ]; then
        print_status "Creating CMakeLists.txt for compilation..."
        
        cat > "$PACKAGE_DIR/src/CMakeLists.txt" << EOF
cmake_minimum_required(VERSION 3.10)
project($MODULE_INFO_NAME)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find all source files
file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h" "*.hpp")

# Create shared library
add_library(\${PROJECT_NAME} SHARED \${SOURCES})

# Set output directory
set_target_properties(\${PROJECT_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "\${CMAKE_CURRENT_BINARY_DIR}/../lib"
    RUNTIME_OUTPUT_DIRECTORY "\${CMAKE_CURRENT_BINARY_DIR}/../lib"
)

# Include directories
target_include_directories(\${PROJECT_NAME} PRIVATE .)

# Install targets
install(TARGETS \${PROJECT_NAME}
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION lib
)
EOF
    fi
    
    # Configure and build
    cmake -DCMAKE_BUILD_TYPE=Release "$PACKAGE_DIR/src"
    
    if [ $? -ne 0 ]; then
        print_error "CMake configuration failed"
        rm -rf "$TEMP_DIR"
        exit 1
    fi
    
    make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
    
    if [ $? -ne 0 ]; then
        print_error "Compilation failed"
        rm -rf "$TEMP_DIR"
        exit 1
    fi
    
    print_success "Compilation completed successfully"
    
    # Clean up build artifacts but keep libraries
    cd "$PACKAGE_DIR"
    if [ -d "lib" ]; then
        print_status "Compiled libraries available in lib/ directory"
        ls -la lib/
    fi
    
    # Remove build directory to save space
    rm -rf "$BUILD_DIR"
fi

# Analyze C++ sources for function exports
print_status "Analyzing C++ sources for exports..."

EXPORT_FUNCTIONS=()

for cpp_file in "${CPP_FILES[@]}"; do
    RELATIVE_PATH=$(echo "$cpp_file" | sed "s|$MODULE_DIR/||")
    TEMP_CPP_FILE="$PACKAGE_DIR/$RELATIVE_PATH"
    
    if [ "$VERBOSE" = true ]; then
        print_status "Analyzing: $RELATIVE_PATH"
    fi
    
    # Look for CJMOD function definitions (simplified pattern matching)
    # This would need more sophisticated parsing in a real implementation
    FUNCTIONS=$(grep -n "void\s\+\w\+\s*(" "$TEMP_CPP_FILE" | sed 's/.*void\s\+\(\w\+\)\s*(.*/\1/' || true)
    
    for func in $FUNCTIONS; do
        if [[ "$func" != "main" ]] && [[ "$func" != "" ]]; then
            EXPORT_FUNCTIONS+=("$func")
        fi
    done
done

# Generate or update [Export] section in info file
INFO_FILE_TEMP="$PACKAGE_DIR/info/$MODULE_NAME.chtl"
EXPORT_CONTENT=""

if [ ${#EXPORT_FUNCTIONS[@]} -gt 0 ]; then
    for func in "${EXPORT_FUNCTIONS[@]}"; do
        EXPORT_CONTENT="$EXPORT_CONTENT    @Function $func;"$'\n'
    done
    
    # Remove existing [Export] section if present
    sed -i '/^\[Export\]/,/^$/d' "$INFO_FILE_TEMP" 2>/dev/null || true
    
    # Add new [Export] section
    echo "" >> "$INFO_FILE_TEMP"
    echo "[Export]" >> "$INFO_FILE_TEMP"
    echo "{" >> "$INFO_FILE_TEMP"
    echo "$EXPORT_CONTENT" >> "$INFO_FILE_TEMP"
    echo "}" >> "$INFO_FILE_TEMP"
    
    print_success "Generated export section with ${#EXPORT_FUNCTIONS[@]} function(s)"
else
    print_warning "No exportable functions found"
fi

# Create package archive
PACKAGE_NAME="${MODULE_NAME}-${MODULE_VERSION}.cjmod"
PACKAGE_PATH="$OUTPUT_DIR/$PACKAGE_NAME"

print_status "Creating CJMOD package: $PACKAGE_NAME"

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
    print_success "CJMOD package created successfully"
    print_status "Package: $PACKAGE_PATH"
    print_status "Size: $PACKAGE_SIZE"
    print_status "Compression: $COMPRESSION_METHOD"
    
    # Create package info file
    INFO_FILE="$OUTPUT_DIR/$MODULE_NAME-info.txt"
    cat > "$INFO_FILE" << EOF
CJMOD Package Information
=========================

Module Name: $MODULE_INFO_NAME
Directory Name: $MODULE_NAME
Version: $MODULE_VERSION
Description: $MODULE_DESCRIPTION
Package File: $PACKAGE_NAME
Size: $PACKAGE_SIZE
Compression: $COMPRESSION_METHOD
Compiled: $([ "$COMPILE" = true ] && echo "YES" || echo "NO")
Created: $(date)

C++ Sources: ${#CPP_FILES[@]}
Header Files: ${#H_FILES[@]}
Exported Functions: ${#EXPORT_FUNCTIONS[@]}

Structure Validation: PASSED
Export Generation: $([ ${#EXPORT_FUNCTIONS[@]} -gt 0 ] && echo "SUCCESS" || echo "NO EXPORTS")

Installation:
  1. Copy $PACKAGE_NAME to your CHTL module directory
  2. Use CHTL compiler to unpack: chtl --unpack-cjmod $PACKAGE_NAME
  3. The module will be compiled and linked automatically
  4. Import functions in your CHTL JS code: ${EXPORT_FUNCTIONS[*]}

Requirements:
  - C++ compiler (g++ or clang++)
  - CMake (for building)
  - CHTL JS runtime with CJMOD support
EOF
    
    print_success "Package info saved: $INFO_FILE"
    
    # Create verification script
    VERIFY_SCRIPT="$OUTPUT_DIR/verify-$MODULE_NAME.sh"
    cat > "$VERIFY_SCRIPT" << EOF
#!/bin/bash
# CJMOD Package Verification Script

PACKAGE="$PACKAGE_PATH"
TEMP_DIR=\$(mktemp -d)

echo "Verifying CJMOD package: \$PACKAGE"

if [ ! -f "\$PACKAGE" ]; then
    echo "ERROR: Package file not found"
    exit 1
fi

cd "\$TEMP_DIR"

if [[ "\$PACKAGE" == *.tar.gz ]] || [[ "\$PACKAGE" == *.cjmod ]]; then
    echo "Package contents:"
    tar -tzf "\$PACKAGE" | head -30
    echo ""
    echo "C++ source files:"
    tar -tzf "\$PACKAGE" | grep "\.cpp\$"
    echo ""
    echo "Header files:"
    tar -tzf "\$PACKAGE" | grep "\.\(h\|hpp\)\$"
else
    echo "Package contents:"
    unzip -l "\$PACKAGE" | head -30
fi

echo "Package verification completed"
rm -rf "\$TEMP_DIR"
EOF
    
    chmod +x "$VERIFY_SCRIPT"
    print_success "Verification script created: $VERIFY_SCRIPT"
    
else
    print_error "Failed to create package"
    exit 1
fi

print_success "CJMOD packaging completed successfully"

if [ ${#EXPORT_FUNCTIONS[@]} -gt 0 ]; then
    print_status "Exported functions:"
    for func in "${EXPORT_FUNCTIONS[@]}"; do
        print_status "  - $func"
    done
fi

exit 0