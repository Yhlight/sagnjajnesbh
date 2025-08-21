# CHTL ANTLR Parsers Release v1.0.0

This package contains CSS and JavaScript parsers for the CHTL compiler.

## Contents

- `include/` - Header files for using the libraries
- `lib/` - Prebuilt static libraries for Windows
- `single_header/` - Single header version (stub implementation)
- `examples/` - Example code
- `build_scripts/` - Scripts to build from source

## Quick Start

### Option 1: Use Prebuilt Libraries (Windows)

```cpp
#include "chtl/css/CSSCompiler.h"
#include "chtl/javascript/JavaScriptCompiler.h"

// Link with lib/windows/x64/Release/chtl_antlr_all.lib
```

### Option 2: Single Header (Development)

```cpp
#include "single_header/chtl_antlr_parsers.h"
// No linking required, but limited functionality
```

## Requirements

- C++17 compiler
- Windows: Visual Studio 2019 or later
- Linux/macOS: Build from source required

## License

ANTLR runtime is under BSD license.
CHTL components are under the project license.
