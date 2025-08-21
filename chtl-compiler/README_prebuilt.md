# CHTL Prebuilt Libraries

This repository includes prebuilt static libraries for the ANTLR-based CSS and JavaScript parsers.

## Quick Start

### Using Prebuilt Libraries (Recommended)

For Windows users who want to use the libraries without building from source:

```batch
# Download prebuilt libraries
download_prebuilt.bat

# Build example project
cd example_windows_project
build.bat
```

### Library Files

The prebuilt libraries are located in `prebuilt/windows/x64/`:

- **Release Mode**:
  - `chtl_css.lib` - CSS parser
  - `chtl_javascript.lib` - JavaScript parser
  - `antlr4-runtime.lib` - ANTLR runtime
  - `chtl_antlr_all.lib` - All-in-one library

- **Debug Mode**:
  - Same libraries with `_d` suffix

## Integration Guide

### Method 1: Direct Linking

```cpp
#pragma comment(lib, "chtl_antlr_all.lib")
```

### Method 2: CMake

```cmake
# Find libraries
find_library(CHTL_ANTLR_LIB 
    NAMES chtl_antlr_all
    PATHS ${CMAKE_CURRENT_SOURCE_DIR}/prebuilt/windows/x64/Release
)

target_link_libraries(your_app ${CHTL_ANTLR_LIB})
```

### Method 3: Visual Studio Project

1. Right-click project → Properties
2. Configuration Properties → VC++ Directories
3. Add to Include Directories:
   - `$(ProjectDir)..\src`
   - `$(ProjectDir)..\third_party\antlr4-runtime\runtime\src`
4. Add to Library Directories:
   - `$(ProjectDir)..\prebuilt\windows\x64\$(Configuration)`
5. Linker → Input → Additional Dependencies:
   - Add `chtl_antlr_all.lib`

## Building from Source

If you need to build the libraries yourself:

```batch
# Complete build
build_all_windows.bat

# Or step by step:
build_antlr_runtime_windows.bat
build_antlr_libs.bat
```

## Requirements

- **Runtime**: Visual C++ Redistributable 2019 or later
- **For building**: Visual Studio 2019/2022, CMake 3.14+, Java 8+

## Binary Compatibility

The prebuilt libraries are compiled with:
- MSVC v143 toolset (VS 2022)
- `/MD` (Multi-threaded DLL) runtime
- C++17 standard
- x64 architecture

They are compatible with:
- Visual Studio 2019 and later
- Windows 10/11 x64
- Any project using `/MD` or `/MDd` runtime

## File Sizes

Approximate sizes:
- Release: ~10MB total
- Debug: ~40MB total (includes debug symbols)

## Git LFS

Due to the size of binary files, this repository uses Git LFS for the prebuilt libraries. 
Make sure you have Git LFS installed:

```batch
git lfs install
git lfs pull
```

## License

The ANTLR runtime is distributed under the BSD license.
CHTL components are under the project license.