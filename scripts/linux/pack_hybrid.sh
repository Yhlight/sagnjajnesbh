#!/bin/bash
# CHTL Hybrid CMOD+CJMOD Module Packaging Script for Linux
# Packages hybrid modules with both CMOD and CJMOD components according to CHTL syntax documentation

if [ $# -eq 0 ]; then
    echo "Usage: $0 <hybrid_module_directory>"
    echo
    echo "Example: $0 Chtholly"
    echo
    echo "Hybrid Module Structure Requirements:"
    echo "  HybridModuleName/"
    echo "    CMOD/|Cmod/|cmod/"
    echo "      SubModule1/"
    echo "        src/ *.chtl files"
    echo "        info/ SubModule1.chtl"
    echo "      SubModule2/"
    echo "        src/ *.chtl files"
    echo "        info/ SubModule2.chtl"
    echo "    CJMOD/|CJmod/|cjmod/"
    echo "      SubModule1/"
    echo "        src/ *.cpp *.h files"
    echo "        info/ SubModule1.chtl"
    echo
    echo "Note: Hybrid modules are packaged as .cmod format but contain both CMOD and CJMOD components"
    echo
    exit 1
fi

HYBRID_DIR="$1"
HYBRID_NAME="$1"

echo "Packaging Hybrid CMOD+CJMOD module: $HYBRID_NAME"

# Check if hybrid directory exists
if [ ! -d "$HYBRID_DIR" ]; then
    echo "Error: Hybrid module directory '$HYBRID_DIR' not found"
    exit 1
fi

# Validate hybrid structure according to CHTL syntax documentation
echo "Validating Hybrid module structure..."

CMOD_DIR=""
CJMOD_DIR=""

# Find CMOD directory (case-insensitive)
if [ -d "$HYBRID_DIR/CMOD" ]; then
    CMOD_DIR="$HYBRID_DIR/CMOD"
elif [ -d "$HYBRID_DIR/Cmod" ]; then
    CMOD_DIR="$HYBRID_DIR/Cmod"
elif [ -d "$HYBRID_DIR/cmod" ]; then
    CMOD_DIR="$HYBRID_DIR/cmod"
fi

# Find CJMOD directory (case-insensitive)
if [ -d "$HYBRID_DIR/CJMOD" ]; then
    CJMOD_DIR="$HYBRID_DIR/CJMOD"
elif [ -d "$HYBRID_DIR/CJmod" ]; then
    CJMOD_DIR="$HYBRID_DIR/CJmod"
elif [ -d "$HYBRID_DIR/cjmod" ]; then
    CJMOD_DIR="$HYBRID_DIR/cjmod"
fi

if [ -z "$CMOD_DIR" ]; then
    echo "Error: Missing CMOD directory in hybrid module"
    echo "Required: $HYBRID_NAME/CMOD/ or $HYBRID_NAME/Cmod/ or $HYBRID_NAME/cmod/"
    exit 1
fi

if [ -z "$CJMOD_DIR" ]; then
    echo "Error: Missing CJMOD directory in hybrid module"
    echo "Required: $HYBRID_NAME/CJMOD/ or $HYBRID_NAME/CJmod/ or $HYBRID_NAME/cjmod/"
    exit 1
fi

echo "Found CMOD directory: $CMOD_DIR"
echo "Found CJMOD directory: $CJMOD_DIR"

# Validate CMOD submodules
echo "Validating CMOD submodules..."
CMOD_VALID=0
for submodule in "$CMOD_DIR"/*; do
    if [ -d "$submodule" ]; then
        submodule_name=$(basename "$submodule")
        echo "Checking CMOD submodule: $submodule_name"
        
        if [ ! -d "$submodule/src" ]; then
            echo "Error: Missing src directory in CMOD submodule $submodule_name"
            exit 1
        fi
        
        if [ ! -d "$submodule/info" ]; then
            echo "Error: Missing info directory in CMOD submodule $submodule_name"
            exit 1
        fi
        
        if [ ! -f "$submodule/info/$submodule_name.chtl" ]; then
            echo "Error: Missing info file: $submodule_name/info/$submodule_name.chtl"
            exit 1
        fi
        
        if ! ls "$submodule/src"/*.chtl 1> /dev/null 2>&1; then
            echo "Error: No CHTL files found in CMOD submodule $submodule_name"
            exit 1
        fi
        
        CMOD_VALID=1
        echo "CMOD submodule $submodule_name validated"
    fi
done

if [ $CMOD_VALID -eq 0 ]; then
    echo "Error: No valid CMOD submodules found"
    exit 1
fi

# Validate CJMOD submodules
echo "Validating CJMOD submodules..."
CJMOD_VALID=0
for submodule in "$CJMOD_DIR"/*; do
    if [ -d "$submodule" ]; then
        submodule_name=$(basename "$submodule")
        echo "Checking CJMOD submodule: $submodule_name"
        
        if [ ! -d "$submodule/src" ]; then
            echo "Error: Missing src directory in CJMOD submodule $submodule_name"
            exit 1
        fi
        
        if [ ! -d "$submodule/info" ]; then
            echo "Error: Missing info directory in CJMOD submodule $submodule_name"
            exit 1
        fi
        
        if [ ! -f "$submodule/info/$submodule_name.chtl" ]; then
            echo "Error: Missing info file: $submodule_name/info/$submodule_name.chtl"
            exit 1
        fi
        
        if ! ls "$submodule/src"/*.cpp "$submodule/src"/*.h 1> /dev/null 2>&1; then
            echo "Error: No C++ files found in CJMOD submodule $submodule_name"
            exit 1
        fi
        
        CJMOD_VALID=1
        echo "CJMOD submodule $submodule_name validated"
    fi
done

if [ $CJMOD_VALID -eq 0 ]; then
    echo "Error: No valid CJMOD submodules found"
    exit 1
fi

# Create packages directory
mkdir -p packages

# Create temporary directory for packaging
TEMP_DIR="temp_${HYBRID_NAME}_$$"
mkdir "$TEMP_DIR"

# Copy hybrid module structure
echo "Copying hybrid module files..."
cp -r "$HYBRID_DIR" "$TEMP_DIR/$HYBRID_NAME"

# Create .cmod package (hybrid modules use .cmod format)
echo "Creating Hybrid CMOD+CJMOD package..."
cd "$TEMP_DIR"
zip -r "../packages/${HYBRID_NAME}.cmod" "$HYBRID_NAME"
cd ..

# Clean up
rm -rf "$TEMP_DIR"

echo
echo "Hybrid CMOD+CJMOD package created successfully!"
echo "Package: packages/${HYBRID_NAME}.cmod"
echo
echo "Package contents validate according to CHTL syntax documentation:"
echo "- Contains both CMOD and CJMOD components in separate subdirectories"
echo "- CMOD: Provides reusable CHTL components"
echo "- CJMOD: Extends CHTL JS syntax through C++ implementation"
echo "- Imported separately: [Import] @Chtl for CMOD, [Import] @CJmod for CJMOD"
echo "- Not recommended to use @Chtl for both CMOD and CJMOD simultaneously"
echo