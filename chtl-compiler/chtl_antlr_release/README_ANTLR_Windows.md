# ANTLR Integration for CHTL Compiler - Windows Guide

## Prerequisites

### Required Software
1. **Visual Studio 2019/2022** with C++ development workload
2. **CMake** (3.14 or higher) - [Download](https://cmake.org/download/)
3. **Java Runtime** (JRE 8 or higher) - [Download](https://www.java.com/)
4. **Git** (optional, for cloning grammars)

### Environment Setup
Ensure the following are in your PATH:
- Visual Studio compiler (`cl.exe`)
- CMake (`cmake.exe`)
- Java (`java.exe`)

## Quick Start

### Option 1: Automated Build (Recommended)

Run the complete build script from a **Visual Studio Developer Command Prompt**:

```batch
build_all_windows.bat
```

This script will:
1. Download ANTLR tools and runtime
2. Build ANTLR C++ runtime
3. Generate CSS and JavaScript parsers
4. Build static libraries
5. Create example executable

### Option 2: Step-by-Step Build

#### Step 1: Build ANTLR Runtime
```batch
build_antlr_runtime_windows.bat
```

#### Step 2: Generate Parsers
```batch
cd third_party
java -jar antlr-4.13.1-complete.jar -Dlanguage=Cpp -visitor -o generated\css3 grammars\css3\*.g4
java -jar antlr-4.13.1-complete.jar -Dlanguage=Cpp -visitor -o generated\javascript grammars\javascript\*.g4
cd ..
```

#### Step 3: Build Libraries
```batch
build_antlr_libs.bat
```

### Option 3: CMake Build

```batch
mkdir build_cmake
cd build_cmake
cmake -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release ../CMakeLists_antlr.txt
cmake --build . --config Release
```

## Output Files

After successful build, you'll find:

```
build\antlr\
├── chtl_css.lib           # CSS parser library
├── chtl_javascript.lib    # JavaScript parser library
├── antlr4-runtime.lib     # ANTLR runtime library
├── chtl_antlr_all.lib     # Combined library
└── antlr_example.exe      # Example application
```

## Usage in Your Project

### Visual Studio Project Settings

1. **Include Directories**:
   - `$(ProjectDir)third_party\antlr4-runtime\runtime\src`
   - `$(ProjectDir)src`

2. **Library Directories**:
   - `$(ProjectDir)build\antlr`

3. **Additional Dependencies**:
   - `chtl_antlr_all.lib` (or individual libraries)

### Code Example

```cpp
#include "chtl/css/CSSCompiler.h"
#include "chtl/javascript/JavaScriptCompiler.h"

// CSS Compilation
chtl::css::CSSCompiler cssCompiler;
if (cssCompiler.Compile(cssSource)) {
    std::string output = cssCompiler.GetOutput();
}

// JavaScript Compilation  
chtl::javascript::JavaScriptCompiler jsCompiler;
if (jsCompiler.Compile(jsSource)) {
    std::string output = jsCompiler.GetOutput();
}
```

## Troubleshooting

### "Visual Studio compiler not found"
- Run the script from **Visual Studio Developer Command Prompt**
- Or manually set up environment: `"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"`

### "CMake not found"
- Install CMake from https://cmake.org/download/
- Add CMake to PATH during installation

### "Java not found"
- Install Java from https://www.java.com/
- Add Java to PATH: typically `C:\Program Files\Java\jre1.8.0_xxx\bin`

### Build Errors
- Ensure all paths in scripts match your directory structure
- Check that ANTLR grammar files are downloaded correctly
- Verify Visual Studio version in CMake generator (`-G` option)

## Integration with CHTL V3

To integrate with the main CHTL compiler:

1. Link against `chtl_antlr_all.lib`
2. Use `CSSCompiler` for `style` blocks
3. Use `JavaScriptCompiler` for `script` blocks
4. Handle compilation errors appropriately

## Advanced Configuration

### Debug Build
Modify scripts to use `/MDd` instead of `/MD` and link debug libraries.

### Custom Visual Studio Version
Edit `build_antlr_runtime_windows.bat` to use your VS version:
- VS 2019: `-G "Visual Studio 16 2019"`
- VS 2022: `-G "Visual Studio 17 2022"`

### Static vs Dynamic Runtime
- `/MT` or `/MTd`: Static runtime
- `/MD` or `/MDd`: Dynamic runtime (default)

## Performance Notes

- Static libraries are optimized for size and speed
- ANTLR parsers use efficient prediction algorithms
- Consider enabling link-time optimization (`/LTCG`) for release builds