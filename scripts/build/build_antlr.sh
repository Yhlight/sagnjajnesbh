#!/bin/bash

# ANTLR4 Build Script for CHTL Compiler
# Downloads, builds, and integrates ANTLR4 C++ runtime

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

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

print_status "Starting ANTLR4 Build and Integration"
print_status "Project root: $PROJECT_ROOT"

cd "$PROJECT_ROOT"

# Configuration
ANTLR_VERSION="4.13.1"
ANTLR_URL="https://github.com/antlr/antlr4/archive/${ANTLR_VERSION}.tar.gz"
ANTLR_JAR_URL="https://www.antlr.org/download/antlr-${ANTLR_VERSION}-complete.jar"
EXTERNAL_DIR="$PROJECT_ROOT/external"
ANTLR_DIR="$EXTERNAL_DIR/antlr4"
ANTLR_BUILD_DIR="$ANTLR_DIR/build"
ANTLR_INSTALL_DIR="$EXTERNAL_DIR/antlr4-install"

# Check for required tools
print_status "Checking build dependencies..."

if ! command -v cmake &> /dev/null; then
    print_error "CMake is not installed or not in PATH"
    exit 1
fi

if ! command -v make &> /dev/null && ! command -v ninja &> /dev/null; then
    print_error "Neither Make nor Ninja build system found"
    exit 1
fi

if ! command -v wget &> /dev/null && ! command -v curl &> /dev/null; then
    print_error "Neither wget nor curl found for downloading"
    exit 1
fi

# Check for Java (required for ANTLR JAR)
if ! command -v java &> /dev/null; then
    print_warning "Java not found - ANTLR JAR will not be available for grammar generation"
    JAVA_AVAILABLE=false
else
    JAVA_VERSION=$(java -version 2>&1 | head -n1 | cut -d'"' -f2)
    print_status "Java version: $JAVA_VERSION"
    JAVA_AVAILABLE=true
fi

print_success "Build dependencies verified"

# Detect build system
BUILD_SYSTEM="make"
if command -v ninja &> /dev/null; then
    BUILD_SYSTEM="ninja"
    CMAKE_GENERATOR="-G Ninja"
else
    CMAKE_GENERATOR=""
fi

print_status "Using build system: $BUILD_SYSTEM"

# Create external directory
print_status "Creating external directory: $EXTERNAL_DIR"
mkdir -p "$EXTERNAL_DIR"
cd "$EXTERNAL_DIR"

# Download ANTLR4 source if not exists
if [ ! -d "$ANTLR_DIR" ]; then
    print_status "Downloading ANTLR4 v${ANTLR_VERSION}..."
    
    if command -v wget &> /dev/null; then
        wget -O "antlr4-${ANTLR_VERSION}.tar.gz" "$ANTLR_URL"
    else
        curl -L -o "antlr4-${ANTLR_VERSION}.tar.gz" "$ANTLR_URL"
    fi
    
    if [ $? -ne 0 ]; then
        print_error "Failed to download ANTLR4 source"
        exit 1
    fi
    
    print_status "Extracting ANTLR4 source..."
    tar -xzf "antlr4-${ANTLR_VERSION}.tar.gz"
    mv "antlr4-${ANTLR_VERSION}" antlr4
    rm "antlr4-${ANTLR_VERSION}.tar.gz"
    
    print_success "ANTLR4 source downloaded and extracted"
else
    print_status "ANTLR4 source already exists, skipping download"
fi

# Download ANTLR4 JAR if Java is available
if [ "$JAVA_AVAILABLE" = true ]; then
    ANTLR_JAR="$EXTERNAL_DIR/antlr-${ANTLR_VERSION}-complete.jar"
    if [ ! -f "$ANTLR_JAR" ]; then
        print_status "Downloading ANTLR4 JAR..."
        
        if command -v wget &> /dev/null; then
            wget -O "$ANTLR_JAR" "$ANTLR_JAR_URL"
        else
            curl -L -o "$ANTLR_JAR" "$ANTLR_JAR_URL"
        fi
        
        if [ $? -eq 0 ]; then
            print_success "ANTLR4 JAR downloaded"
        else
            print_warning "Failed to download ANTLR4 JAR"
        fi
    else
        print_status "ANTLR4 JAR already exists"
    fi
fi

# Build ANTLR4 C++ runtime
print_status "Building ANTLR4 C++ runtime..."

# Clean previous build
if [ -d "$ANTLR_BUILD_DIR" ]; then
    print_status "Cleaning previous ANTLR4 build..."
    rm -rf "$ANTLR_BUILD_DIR"
fi

if [ -d "$ANTLR_INSTALL_DIR" ]; then
    print_status "Cleaning previous ANTLR4 installation..."
    rm -rf "$ANTLR_INSTALL_DIR"
fi

mkdir -p "$ANTLR_BUILD_DIR"
mkdir -p "$ANTLR_INSTALL_DIR"

cd "$ANTLR_BUILD_DIR"

# Configure ANTLR4 C++ runtime
print_status "Configuring ANTLR4 C++ runtime..."

cmake $CMAKE_GENERATOR \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="$ANTLR_INSTALL_DIR" \
    -DANTLR_BUILD_CPP_TESTS=OFF \
    -DANTLR_BUILD_SHARED=ON \
    -DANTLR_BUILD_STATIC=ON \
    "$ANTLR_DIR/runtime/Cpp"

if [ $? -ne 0 ]; then
    print_error "ANTLR4 CMake configuration failed"
    exit 1
fi

print_success "ANTLR4 CMake configuration completed"

# Build ANTLR4 C++ runtime
print_status "Building ANTLR4 C++ runtime..."

if [ "$BUILD_SYSTEM" = "ninja" ]; then
    ninja -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
else
    make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
fi

if [ $? -ne 0 ]; then
    print_error "ANTLR4 build failed"
    exit 1
fi

print_success "ANTLR4 C++ runtime build completed"

# Install ANTLR4 C++ runtime
print_status "Installing ANTLR4 C++ runtime..."

if [ "$BUILD_SYSTEM" = "ninja" ]; then
    ninja install
else
    make install
fi

if [ $? -ne 0 ]; then
    print_error "ANTLR4 installation failed"
    exit 1
fi

print_success "ANTLR4 C++ runtime installed to: $ANTLR_INSTALL_DIR"

# Create ANTLR4 configuration for CHTL project
print_status "Creating ANTLR4 configuration for CHTL project..."

ANTLR_CONFIG_FILE="$PROJECT_ROOT/cmake/FindANTLR4.cmake"
mkdir -p "$(dirname "$ANTLR_CONFIG_FILE")"

cat > "$ANTLR_CONFIG_FILE" << 'EOF'
# FindANTLR4.cmake - Find ANTLR4 C++ runtime
# This module defines:
#  ANTLR4_FOUND - True if ANTLR4 is found
#  ANTLR4_INCLUDE_DIRS - Include directories for ANTLR4
#  ANTLR4_LIBRARIES - Libraries to link against
#  ANTLR4_JAR - Path to ANTLR4 JAR file (if available)

set(ANTLR4_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../external/antlr4-install")

# Find include directory
find_path(ANTLR4_INCLUDE_DIR
    NAMES antlr4-runtime.h
    PATHS ${ANTLR4_ROOT_DIR}/include
    PATH_SUFFIXES antlr4-runtime
    NO_DEFAULT_PATH
)

# Find library
find_library(ANTLR4_LIBRARY
    NAMES antlr4-runtime antlr4-runtime-static
    PATHS ${ANTLR4_ROOT_DIR}/lib ${ANTLR4_ROOT_DIR}/lib64
    NO_DEFAULT_PATH
)

# Find JAR file
find_file(ANTLR4_JAR
    NAMES antlr-*-complete.jar
    PATHS ${CMAKE_CURRENT_LIST_DIR}/../external
    NO_DEFAULT_PATH
)

# Handle standard arguments
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ANTLR4
    REQUIRED_VARS ANTLR4_LIBRARY ANTLR4_INCLUDE_DIR
    VERSION_VAR ANTLR4_VERSION
)

if(ANTLR4_FOUND)
    set(ANTLR4_INCLUDE_DIRS ${ANTLR4_INCLUDE_DIR})
    set(ANTLR4_LIBRARIES ${ANTLR4_LIBRARY})
    
    # Create imported target
    if(NOT TARGET antlr4-runtime)
        add_library(antlr4-runtime UNKNOWN IMPORTED)
        set_target_properties(antlr4-runtime PROPERTIES
            IMPORTED_LOCATION ${ANTLR4_LIBRARY}
            INTERFACE_INCLUDE_DIRECTORIES ${ANTLR4_INCLUDE_DIR}
        )
    endif()
endif()

mark_as_advanced(ANTLR4_INCLUDE_DIR ANTLR4_LIBRARY ANTLR4_JAR)
EOF

print_success "Created ANTLR4 CMake configuration"

# Create grammar directory and example grammars
GRAMMAR_DIR="$PROJECT_ROOT/grammars"
mkdir -p "$GRAMMAR_DIR"

# Create example CSS grammar
cat > "$GRAMMAR_DIR/CSS.g4" << 'EOF'
grammar CSS;

// CSS Grammar for CHTL Compiler
stylesheet
    : (charset | imports | rule)*
    ;

charset
    : CHARSET_SYM STRING SEMICOLON
    ;

imports
    : IMPORT_SYM (STRING | URI) mediaList? SEMICOLON
    ;

rule
    : selector (COMMA selector)* LBRACE declaration* RBRACE
    ;

selector
    : simpleSelector (combinator simpleSelector)*
    ;

simpleSelector
    : (HASH | DOT IDENT | IDENT | STAR) (DOT IDENT | HASH | LBRACKET IDENT (EQUALS | CONTAINS | BEGINS | ENDS)? STRING? RBRACKET)*
    ;

combinator
    : GREATER | PLUS | TILDE | WS
    ;

declaration
    : property COLON value (IMPORTANT_SYM)? SEMICOLON?
    ;

property
    : IDENT
    ;

value
    : (IDENT | STRING | NUMBER | HASH | URI | FUNCTION)*
    ;

mediaList
    : IDENT (COMMA IDENT)*
    ;

// Lexer rules
CHARSET_SYM : '@charset' ;
IMPORT_SYM  : '@import' ;
IMPORTANT_SYM : '!important' ;

LBRACE      : '{' ;
RBRACE      : '}' ;
LBRACKET    : '[' ;
RBRACKET    : ']' ;
LPAREN      : '(' ;
RPAREN      : ')' ;
SEMICOLON   : ';' ;
COLON       : ':' ;
COMMA       : ',' ;
DOT         : '.' ;
HASH        : '#' [a-fA-F0-9]+ ;
STAR        : '*' ;
GREATER     : '>' ;
PLUS        : '+' ;
TILDE       : '~' ;
EQUALS      : '=' ;
CONTAINS    : '*=' ;
BEGINS      : '^=' ;
ENDS        : '$=' ;

IDENT       : [a-zA-Z_-] [a-zA-Z0-9_-]* ;
STRING      : '"' (~["\r\n])* '"' | '\'' (~['\r\n])* '\'' ;
NUMBER      : [0-9]+ ('.' [0-9]+)? [a-zA-Z%]* ;
URI         : 'url(' (~[)])* ')' ;
FUNCTION    : IDENT '(' ;

WS          : [ \t\r\n]+ -> skip ;
COMMENT     : '/*' .*? '*/' -> skip ;
EOF

print_success "Created example CSS grammar"

# Verify installation
print_status "Verifying ANTLR4 installation..."

if [ -f "$ANTLR_INSTALL_DIR/lib/libantlr4-runtime.so" ] || [ -f "$ANTLR_INSTALL_DIR/lib/libantlr4-runtime.a" ] || [ -f "$ANTLR_INSTALL_DIR/lib/libantlr4-runtime.dylib" ]; then
    print_success "ANTLR4 C++ runtime library found"
else
    print_warning "ANTLR4 C++ runtime library not found in expected location"
fi

if [ -f "$ANTLR_INSTALL_DIR/include/antlr4-runtime/antlr4-runtime.h" ]; then
    print_success "ANTLR4 C++ runtime headers found"
else
    print_warning "ANTLR4 C++ runtime headers not found in expected location"
fi

# Create helper script for grammar generation
if [ "$JAVA_AVAILABLE" = true ] && [ -f "$ANTLR_JAR" ]; then
    GRAMMAR_SCRIPT="$PROJECT_ROOT/scripts/build/generate_grammars.sh"
    cat > "$GRAMMAR_SCRIPT" << EOF
#!/bin/bash
# Grammar Generation Script for CHTL Compiler

set -e

SCRIPT_DIR="\$(cd "\$(dirname "\${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="\$(cd "\$SCRIPT_DIR/../.." && pwd)"
ANTLR_JAR="$ANTLR_JAR"
GRAMMAR_DIR="\$PROJECT_ROOT/grammars"
OUTPUT_DIR="\$PROJECT_ROOT/src/Generated"

echo "[INFO] Generating C++ code from ANTLR grammars..."

mkdir -p "\$OUTPUT_DIR"

for grammar in "\$GRAMMAR_DIR"/*.g4; do
    if [ -f "\$grammar" ]; then
        echo "[INFO] Processing: \$(basename "\$grammar")"
        java -jar "\$ANTLR_JAR" -Dlanguage=Cpp -o "\$OUTPUT_DIR" "\$grammar"
    fi
done

echo "[SUCCESS] Grammar generation completed"
EOF

    chmod +x "$GRAMMAR_SCRIPT"
    print_success "Created grammar generation script: $GRAMMAR_SCRIPT"
fi

print_success "ANTLR4 build and integration completed successfully"
print_status "ANTLR4 installation directory: $ANTLR_INSTALL_DIR"
print_status "ANTLR4 CMake configuration: $ANTLR_CONFIG_FILE"
print_status "Grammar directory: $GRAMMAR_DIR"

if [ "$JAVA_AVAILABLE" = true ]; then
    print_status "ANTLR4 JAR: $ANTLR_JAR"
    print_status "Grammar generation script: scripts/build/generate_grammars.sh"
fi

print_status "To use ANTLR4 in your CMakeLists.txt, add:"
echo "    find_package(ANTLR4 REQUIRED)"
echo "    target_link_libraries(your_target antlr4-runtime)"

exit 0