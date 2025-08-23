#!/bin/bash
# CHTL Unified Module Packaging Script for macOS
# Automatically detects and packages CMOD, CJMOD, or Hybrid modules according to CHTL syntax documentation

if [ $# -eq 0 ]; then
    echo "Usage: $0 <module_directory>"
    echo
    echo "This script automatically detects module type and packages accordingly:"
    echo "  - CMOD: Contains src/*.chtl files only"
    echo "  - CJMOD: Contains src/*.cpp and *.h files only"
    echo "  - Hybrid CMOD+CJMOD: Contains both CMOD and CJMOD subdirectories"
    echo
    echo "Examples:"
    echo "  $0 MyModule         (auto-detect type)"
    echo "  $0 Chtholly         (hybrid module)"
    echo
    echo "Module Structure Support:"
    echo
    echo "Regular CMOD:"
    echo "  ModuleName/"
    echo "    src/ *.chtl files"
    echo "    info/ ModuleName.chtl"
    echo
    echo "Regular CJMOD:"
    echo "  ModuleName/"
    echo "    src/ *.cpp *.h files"  
    echo "    info/ ModuleName.chtl"
    echo
    echo "Hybrid CMOD+CJMOD (Mixed):"
    echo "  ModuleName/"
    echo "    CMOD/|Cmod/|cmod/"
    echo "      SubModule/"
    echo "        src/ *.chtl files"
    echo "        info/ SubModule.chtl"
    echo "    CJMOD/|CJmod/|cjmod/"
    echo "      SubModule/"
    echo "        src/ *.cpp *.h files"
    echo "        info/ SubModule.chtl"
    echo
    exit 1
fi

MODULE_DIR="$1"
MODULE_NAME="$1"

echo "CHTL Unified Module Packaging Tool"
echo "=================================="
echo "Analyzing module: $MODULE_NAME"
echo

# Check if module directory exists
if [ ! -d "$MODULE_DIR" ]; then
    echo "Error: Module directory '$MODULE_DIR' not found"
    exit 1
fi

# Detect module type according to CHTL syntax documentation
echo "Detecting module structure..."

MODULE_TYPE="UNKNOWN"
HAS_CMOD_SUBDIR=0
HAS_CJMOD_SUBDIR=0
HAS_DIRECT_SRC=0
HAS_CHTL_FILES=0
HAS_CPP_FILES=0

# Check for hybrid structure (CMOD + CJMOD subdirectories)
if [ -d "$MODULE_DIR/CMOD" ] || [ -d "$MODULE_DIR/Cmod" ] || [ -d "$MODULE_DIR/cmod" ]; then
    HAS_CMOD_SUBDIR=1
fi

if [ -d "$MODULE_DIR/CJMOD" ] || [ -d "$MODULE_DIR/CJmod" ] || [ -d "$MODULE_DIR/cjmod" ]; then
    HAS_CJMOD_SUBDIR=1
fi

# Check for direct src directory
if [ -d "$MODULE_DIR/src" ]; then
    HAS_DIRECT_SRC=1
    
    # Check content types
    if ls "$MODULE_DIR/src"/*.chtl 1> /dev/null 2>&1; then
        HAS_CHTL_FILES=1
    fi
    
    if ls "$MODULE_DIR/src"/*.cpp "$MODULE_DIR/src"/*.h 1> /dev/null 2>&1; then
        HAS_CPP_FILES=1
    fi
fi

# Determine module type based on structure analysis
if [ $HAS_CMOD_SUBDIR -eq 1 ]; then
    if [ $HAS_CJMOD_SUBDIR -eq 1 ]; then
        MODULE_TYPE="HYBRID"
        echo "> Detected: Hybrid CMOD+CJMOD module structure"
    else
        MODULE_TYPE="CMOD"
        echo "> Detected: CMOD-only module with subdirectory structure"
    fi
elif [ $HAS_CJMOD_SUBDIR -eq 1 ]; then
    MODULE_TYPE="CJMOD"  
    echo "> Detected: CJMOD-only module with subdirectory structure"
elif [ $HAS_DIRECT_SRC -eq 1 ]; then
    if [ $HAS_CHTL_FILES -eq 1 ]; then
        if [ $HAS_CPP_FILES -eq 1 ]; then
            echo "Warning: Direct src contains both CHTL and C++ files"
            echo "This is not recommended. Consider using hybrid structure."
            MODULE_TYPE="CMOD"
            echo "> Treating as: CMOD module (CHTL files take priority)"
        else
            MODULE_TYPE="CMOD"
            echo "> Detected: Standard CMOD module"
        fi
    elif [ $HAS_CPP_FILES -eq 1 ]; then
        MODULE_TYPE="CJMOD"
        echo "> Detected: Standard CJMOD module"
    else
        echo "Error: No valid content found in src directory"
        exit 1
    fi
else
    echo "Error: No valid module structure detected"
    echo "Please ensure your module follows CHTL syntax documentation structure"
    exit 1
fi

echo
echo "Module Type: $MODULE_TYPE"
echo

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Package according to detected type
if [ "$MODULE_TYPE" = "HYBRID" ]; then
    echo "Packaging as Hybrid CMOD+CJMOD module..."
    echo "Note: Hybrid modules are packaged as .cmod format but contain both types"
    bash "$SCRIPT_DIR/pack_hybrid.sh" "$MODULE_DIR"
elif [ "$MODULE_TYPE" = "CMOD" ]; then
    echo "Packaging as CMOD module..."
    bash "$SCRIPT_DIR/pack_cmod.sh" "$MODULE_DIR"
elif [ "$MODULE_TYPE" = "CJMOD" ]; then
    echo "Packaging as CJMOD module..."
    bash "$SCRIPT_DIR/pack_cjmod.sh" "$MODULE_DIR"
else
    echo "Error: Unable to determine packaging method"
    exit 1
fi

echo
echo "Unified packaging completed!"
echo "Module $MODULE_NAME has been packaged as $MODULE_TYPE type."
echo