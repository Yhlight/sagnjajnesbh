# CHTL ANTLR Windows Libraries

Version: 1.0.0
Platform: Windows x64
Compiler: MSVC 2019+

## Quick Start

1. Copy the `lib` folder to your project
2. Copy the `include` folder to your project
3. In Visual Studio:
   - Add `include` to "Additional Include Directories"
   - Add `lib/x64/$(Configuration)` to "Additional Library Directories"
   - Add `chtl_antlr_all.lib` to "Additional Dependencies"

## Contents

- `lib/` - Prebuilt static libraries
- `include/` - Header files
- `examples/` - Example code
- `docs/` - Documentation
- `scripts/` - Build scripts

## Usage Example

```cpp
#include "chtl/css/CSSCompiler.h"
#include "chtl/javascript/JavaScriptCompiler.h"

int main() {
    chtl::css::CSSCompiler css;
    css.Compile("body { color: red; }");
    
    chtl::javascript::JavaScriptCompiler js;
    js.Compile("console.log('Hello');");
    
    return 0;
}
```

## Building from Source

If you need to rebuild the libraries:

1. Ensure you have Visual Studio 2019+ installed
2. Run `scripts/build_all_windows.bat`

Or use GitHub Actions for automated builds.

## License

See LICENSE file for details.
