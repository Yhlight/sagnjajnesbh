# Prebuilt ANTLR Libraries for CHTL

This directory contains precompiled static libraries for different platforms.

## Directory Structure

```
prebuilt/
├── windows/
│   ├── x64/
│   │   ├── Release/
│   │   │   ├── chtl_css.lib
│   │   │   ├── chtl_javascript.lib
│   │   │   ├── antlr4-runtime.lib
│   │   │   └── chtl_antlr_all.lib
│   │   └── Debug/
│   │       ├── chtl_css_d.lib
│   │       ├── chtl_javascript_d.lib
│   │       ├── antlr4-runtime_d.lib
│   │       └── chtl_antlr_all_d.lib
│   └── x86/
│       └── (32-bit versions if needed)
├── linux/
│   └── x64/
│       ├── libchtl_css.a
│       ├── libchtl_javascript.a
│       ├── libantlr4-runtime.a
│       └── libchtl_antlr_all.a
└── macos/
    └── universal/
        └── (Universal binaries)
```

## Windows Libraries

### Compiler Information
- **Compiler**: MSVC 2022 (v143)
- **Runtime**: Multi-threaded DLL (/MD for Release, /MDd for Debug)
- **C++ Standard**: C++17
- **Architecture**: x64

### Library Descriptions

- **chtl_css.lib**: CSS3 parser and compiler
- **chtl_javascript.lib**: JavaScript parser and compiler
- **antlr4-runtime.lib**: ANTLR4 C++ runtime
- **chtl_antlr_all.lib**: Combined library containing all above

### Usage

#### Release Build
```cpp
#pragma comment(lib, "chtl_antlr_all.lib")
// Or link individual libraries:
// #pragma comment(lib, "chtl_css.lib")
// #pragma comment(lib, "chtl_javascript.lib")
// #pragma comment(lib, "antlr4-runtime.lib")
```

#### Debug Build
```cpp
#ifdef _DEBUG
    #pragma comment(lib, "chtl_antlr_all_d.lib")
#else
    #pragma comment(lib, "chtl_antlr_all.lib")
#endif
```

### Visual Studio Configuration

1. Add to Include Directories:
   ```
   $(SolutionDir)src
   $(SolutionDir)third_party\antlr4-runtime\runtime\src
   ```

2. Add to Library Directories:
   ```
   $(SolutionDir)prebuilt\windows\x64\$(Configuration)
   ```

## Version Information

- **ANTLR Version**: 4.13.1
- **Build Date**: 2024-01-XX
- **ABI Compatibility**: MSVC 2019+ (v142+)

## Notes

- Debug libraries have '_d' suffix
- All libraries are built with `/EHsc` exception handling
- Static linking with ANTLR runtime to avoid DLL dependencies
- Thread-safe compilation