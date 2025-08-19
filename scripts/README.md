# CHTL Build Scripts

This directory contains comprehensive cross-platform build scripts for the CHTL compiler project. All scripts are available in both shell (`.sh`) and batch (`.bat`) formats for maximum compatibility.

## Directory Structure

```
scripts/
├── build/                  # Core build scripts
│   ├── build_debug.sh/bat         # Debug build
│   ├── build_release.sh/bat       # Release build
│   ├── build_antlr.sh/bat         # ANTLR4 integration
│   └── build_vscode_plugin.sh/bat # VSCode extension
├── packaging/              # Module packaging scripts
│   ├── pack_cmod.sh/bat           # CMOD module packaging
│   ├── pack_cjmod.sh/bat          # CJMOD module packaging
│   └── pack_all.sh/bat            # Unified packaging
├── integration/            # Integrated build scripts
│   └── build_complete.sh/bat      # Complete build system
└── README.md              # This file
```

## Quick Start

### Complete Build (Recommended)
```bash
# Linux/macOS
./scripts/integration/build_complete.sh

# Windows
scripts\integration\build_complete.bat
```

This will build the compiler, VSCode plugin, ANTLR dependencies, and package all modules.

### Individual Components

#### Debug Build
```bash
# Linux/macOS
./scripts/build/build_debug.sh

# Windows
scripts\build\build_debug.bat
```

#### Release Build
```bash
# Linux/macOS
./scripts/build/build_release.sh

# Windows
scripts\build\build_release.bat
```

#### VSCode Plugin
```bash
# Linux/macOS
./scripts/build/build_vscode_plugin.sh production

# Windows
scripts\build\build_vscode_plugin.bat production
```

#### ANTLR4 Integration
```bash
# Linux/macOS
./scripts/build/build_antlr.sh

# Windows
scripts\build\build_antlr.bat
```

## Module Packaging

### Package All Modules
```bash
# Linux/macOS
./scripts/packaging/pack_all.sh

# Windows
scripts\packaging\pack_all.bat
```

### Package Individual Module Types
```bash
# CMOD modules only
./scripts/packaging/pack_all.sh --cmod-only

# CJMOD modules only (with compilation)
./scripts/packaging/pack_all.sh --cjmod-only --compile
```

### Package Specific Modules
```bash
# Package a specific CMOD module
./scripts/packaging/pack_cmod.sh src/Module/CMOD/Chtholly

# Package a specific CJMOD module with compilation
./scripts/packaging/pack_cjmod.sh --compile src/Module/CJMOD/MyExtension
```

## Advanced Usage

### Clean Build
```bash
./scripts/integration/build_complete.sh --clean --release
```

### Build Only Specific Components
```bash
# Build only compiler
./scripts/integration/build_complete.sh --compiler --skip-vscode --skip-modules

# Build only VSCode plugin
./scripts/integration/build_complete.sh --vscode --skip-compiler --skip-modules
```

### Verbose Output
```bash
./scripts/integration/build_complete.sh --verbose
```

### Parallel Jobs
```bash
./scripts/integration/build_complete.sh --jobs 8
```

## Script Features

### Cross-Platform Compatibility
- All scripts available in both shell (Linux/macOS) and batch (Windows) formats
- Automatic detection of available build tools
- Proper path handling for different operating systems

### Error Handling
- Comprehensive error checking and reporting
- Graceful failure handling with meaningful error messages
- Build verification and validation

### Build Optimization
- Parallel compilation support
- Automatic detection of optimal job count
- Clean build options
- Incremental build support

### Packaging Features
- Automatic module discovery
- Structure validation
- Export generation
- Compression with multiple formats
- Package verification scripts

### Integration Features
- Complete build orchestration
- Build reporting and timing
- Installation script generation
- Artifact management

## Output Directories

- `build-debug/` - Debug build artifacts
- `build-release/` - Release build artifacts
- `packages/` - Module packages and VSCode extension
- `dist/` - Distribution-ready files
- `external/` - External dependencies (ANTLR4)

## Requirements

### Common Requirements
- CMake 3.10 or higher
- C++ compiler (GCC, Clang, or MSVC)

### Platform-Specific Requirements

#### Linux/macOS
- Make or Ninja build system
- curl or wget (for downloads)
- tar (for packaging)

#### Windows
- Visual Studio Build Tools or MinGW
- PowerShell (for some operations)
- 7-Zip (optional, for better packaging)

### VSCode Plugin Requirements
- Node.js 16 or higher
- npm
- vsce (Visual Studio Code Extension Manager)

### ANTLR4 Requirements
- Java 8 or higher (for grammar generation)
- Internet connection (for initial download)

## Environment Variables

The scripts respect the following environment variables:

- `CMAKE_GENERATOR` - Override CMake generator
- `CMAKE_BUILD_TYPE` - Override build type
- `NUMBER_OF_PROCESSORS` (Windows) - Parallel job count
- `MAKEFLAGS` - Make flags for parallel builds

## Troubleshooting

### Common Issues

1. **Permission Denied (Linux/macOS)**
   ```bash
   chmod +x scripts/**/*.sh
   ```

2. **CMake Not Found**
   - Install CMake from https://cmake.org/
   - Ensure CMake is in your PATH

3. **Compiler Not Found**
   - Install build tools for your platform
   - Windows: Visual Studio Build Tools
   - Linux: `sudo apt install build-essential`
   - macOS: Xcode Command Line Tools

4. **Node.js Not Found (VSCode Plugin)**
   - Install Node.js from https://nodejs.org/
   - Ensure node and npm are in your PATH

5. **Java Not Found (ANTLR)**
   - Install Java 8 or higher
   - Set JAVA_HOME environment variable

### Debug Information

All scripts support verbose mode with `--verbose` or `-v` flag for detailed output.

Build reports are automatically generated in the project root as `build-report-*.txt`.

## Contributing

When adding new scripts:

1. Create both `.sh` and `.bat` versions
2. Follow the existing naming conventions
3. Include comprehensive error handling
4. Add usage information with `--help`
5. Update this README

## License

These scripts are part of the CHTL project and are subject to the same MIT license.