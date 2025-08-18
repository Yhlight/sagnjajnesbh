# ANTLR 4 Cross-Platform Integration Guide

Complete guide for integrating ANTLR 4 with CHTL across multiple platforms.

## 📋 Overview

CHTL uses ANTLR 4 for parsing CSS and JavaScript code within CHTL templates. This integration provides:

- **Native CSS parsing** using official CSS3 grammar
- **Native JavaScript parsing** using official JavaScript grammar  
- **Cross-platform support** for Linux, macOS, and Windows
- **Precompiled packages** for easy distribution
- **Automatic platform detection** in build scripts

## 🏗️ Architecture

### Directory Structure
```
external/antlr4-cross-platform/
├── source/                     # ANTLR source code
│   ├── antlr4-4.13.1/         # Downloaded source
│   └── current -> antlr4-4.13.1/  # Symlink to current version
├── precompiled/               # Platform-specific builds
│   ├── linux-x64/
│   │   ├── include/           # Headers
│   │   └── lib/               # Libraries
│   ├── macos-x64/
│   └── windows-x64/
├── tools/                     # ANTLR JAR tools
│   ├── antlr-4.13.1-complete.jar
│   └── antlr.jar -> antlr-4.13.1-complete.jar
├── grammars/                  # Official grammar files
│   ├── css3.g4
│   ├── JavaScriptLexer.g4
│   └── JavaScriptParser.g4
├── scripts/                   # Build and packaging scripts
└── packages/                  # Distribution packages
```

### Integration Points
1. **CSS Compiler** - Uses ANTLR-generated CSS3 parser
2. **JavaScript Compiler** - Uses ANTLR-generated JavaScript parser
3. **CHTL Parser** - Integrates with ANTLR for embedded languages
4. **Build System** - CMake automatically finds and links ANTLR

## 🚀 Quick Start

### 1. Download ANTLR
```bash
cd external/antlr4-cross-platform
./scripts/download_antlr.sh
```

### 2. Build ANTLR Runtime
```bash
# For current platform
./scripts/build_antlr.sh

# For all platforms (on respective systems)
./scripts/build_antlr.sh --platform all
```

### 3. Create Precompiled Packages
```bash
./scripts/package_antlr.sh --include-tools
```

### 4. Build CHTL with ANTLR
```bash
cd ../../scripts/build
./build_with_antlr.sh --auto-build --test
```

## 🔧 Build Scripts

### Download Script
**File**: `scripts/download_antlr.sh`  
**Purpose**: Download ANTLR source code and JAR tools

**Features**:
- Multiple version support
- Official grammar files download
- Mirror support for reliability
- Automatic symlink creation

**Usage**:
```bash
# Download latest version
./scripts/download_antlr.sh

# Download specific version
./scripts/download_antlr.sh -v 4.12.0

# Source code only
./scripts/download_antlr.sh --source-only

# JAR tools only
./scripts/download_antlr.sh --jar-only
```

### Build Script
**File**: `scripts/build_antlr.sh`  
**Purpose**: Cross-platform ANTLR C++ runtime compilation

**Features**:
- Multi-platform support
- Debug/Release builds
- Static/Shared library options
- Automatic prerequisite checking

**Usage**:
```bash
# Build for current platform
./scripts/build_antlr.sh

# Build for specific platform
./scripts/build_antlr.sh --platform linux

# Debug build
./scripts/build_antlr.sh --debug

# Clean build
./scripts/build_antlr.sh --clean
```

### Package Script
**File**: `scripts/package_antlr.sh`  
**Purpose**: Create distributable precompiled packages

**Features**:
- Platform-specific packages
- Unified cross-platform package
- CMake integration files
- Checksum generation

**Usage**:
```bash
# Create runtime packages
./scripts/package_antlr.sh

# Include source code
./scripts/package_antlr.sh --include-source

# Include JAR tools
./scripts/package_antlr.sh --include-tools
```

## 🌍 Platform Support

### Linux (x86_64)
- **Compiler**: GCC 7+ or Clang 10+
- **Build Tool**: Make
- **Package**: `antlr4-4.13.1-linux-x64.tar.gz`
- **Libraries**: `libantlr4-runtime.a` (static), `libantlr4-runtime.so` (shared)

### macOS (x86_64)
- **Compiler**: Clang (Xcode Command Line Tools)
- **Build Tool**: Make
- **Package**: `antlr4-4.13.1-macos-x64.tar.gz`
- **Libraries**: `libantlr4-runtime.a` (static), `libantlr4-runtime.dylib` (shared)

### Windows (x86_64)
- **Compiler**: Visual Studio 2019+ or MinGW
- **Build Tool**: MSBuild or Make
- **Package**: `antlr4-4.13.1-windows-x64.tar.gz`
- **Libraries**: `antlr4-runtime.lib` (static), `antlr4-runtime.dll` (shared)

## 🔗 CMake Integration

### Automatic Integration
CHTL's CMake system automatically detects and integrates ANTLR:

```cmake
# FindANTLR4.cmake is included in packages
find_package(ANTLR4 REQUIRED)
target_link_libraries(your_target antlr4::runtime)
```

### Manual Integration
For custom setups:

```cmake
# Set ANTLR paths
set(ANTLR4_ROOT ${CMAKE_SOURCE_DIR}/external/antlr4-cross-platform/precompiled/linux-x64)
include_directories(${ANTLR4_ROOT}/include)
link_directories(${ANTLR4_ROOT}/lib)

# Link ANTLR runtime
target_link_libraries(your_target antlr4-runtime)
```

### Platform Detection
CMake automatically detects the platform:

```cmake
if(WIN32)
    set(ANTLR4_PLATFORM "windows-x64")
elseif(APPLE)
    set(ANTLR4_PLATFORM "macos-x64")
else()
    set(ANTLR4_PLATFORM "linux-x64")
endif()
```

## 📦 Precompiled Packages

### Package Contents
Each platform package contains:

```
antlr4-4.13.1-platform-x64/
├── include/                   # ANTLR C++ headers
│   ├── antlr4-runtime.h
│   ├── ANTLRInputStream.h
│   ├── CommonTokenStream.h
│   └── ... (50+ header files)
├── lib/                       # ANTLR libraries
│   ├── libantlr4-runtime.a    # Static library
│   └── libantlr4-runtime.so   # Shared library (Linux/macOS)
├── docs/                      # Documentation
└── PACKAGE_INFO.md            # Package information
```

### Package Sizes (Approximate)
- **Linux**: ~2.5 MB compressed, ~12 MB extracted
- **macOS**: ~2.8 MB compressed, ~13 MB extracted  
- **Windows**: ~3.2 MB compressed, ~15 MB extracted
- **Unified**: ~8.5 MB compressed, ~40 MB extracted

## 🧪 Testing Integration

### ANTLR-Specific Tests
```bash
# Basic ANTLR functionality
./bin/test_antlr_basic

# CSS/JavaScript parser integration
./bin/test_antlr_compilers

# Comprehensive CHTL tests
./bin/ComprehensiveTestRunner
```

### Expected Results
- **Basic ANTLR**: Parser initialization and basic functionality
- **Compiler Integration**: CSS and JavaScript parsing with ANTLR
- **Comprehensive**: 100% test pass rate with ANTLR integration

## 🔧 Troubleshooting

### Common Issues

#### "ANTLR not found" Error
**Cause**: ANTLR precompiled package not available for platform  
**Solution**: 
```bash
# Download and build ANTLR
./external/antlr4-cross-platform/scripts/download_antlr.sh
./external/antlr4-cross-platform/scripts/build_antlr.sh
```

#### "CMake configuration failed" Error
**Cause**: Missing CMake integration or wrong paths  
**Solution**:
```bash
# Use ANTLR integration build
./scripts/build/build_with_antlr.sh --auto-build
```

#### "Linker error" with ANTLR
**Cause**: Library path or name mismatch  
**Solution**: Check platform-specific library names and paths

### Platform-Specific Notes

#### Linux
- Requires `libstdc++` development packages
- May need `libuuid-dev` on some distributions

#### macOS
- Requires Xcode Command Line Tools
- May need to set `MACOSX_DEPLOYMENT_TARGET`

#### Windows
- Requires Visual Studio 2019+ or MinGW-w64
- May need Windows SDK

## 📊 Performance Impact

### Build Time
- **ANTLR Download**: ~30 seconds (one-time)
- **ANTLR Build**: ~2-5 minutes per platform
- **CHTL Build**: +10-20% with ANTLR integration

### Runtime Performance
- **CSS Parsing**: Native ANTLR performance
- **JavaScript Parsing**: Native ANTLR performance
- **Memory Usage**: +2-5 MB for ANTLR runtime
- **Startup Time**: +50-100ms for parser initialization

### Package Size Impact
- **Executable Size**: +1-2 MB with static linking
- **Distribution**: +2-8 MB depending on package type

## 🔄 Update Process

### Updating ANTLR Version
1. Update version in scripts: `ANTLR_VERSION="4.14.0"`
2. Download new version: `./scripts/download_antlr.sh -v 4.14.0`
3. Build for all platforms
4. Create new packages
5. Update CHTL CMake configuration

### Maintaining Compatibility
- Test with multiple ANTLR versions
- Maintain backward compatibility
- Document breaking changes
- Provide migration guides

## 🎯 Distribution Strategy

### For End Users
- Include precompiled packages in CHTL releases
- Provide platform-specific installers
- Support package managers (apt, brew, chocolatey)

### For Developers
- Include source code packages
- Provide build instructions
- Support custom ANTLR builds
- Document API changes

---

**ANTLR 4 Cross-Platform Integration - Ready for Production!** 🚀