#!/bin/bash

# Create a complete Windows release package

RELEASE_NAME="chtl-antlr-windows-v1.0.0"
RELEASE_DIR="releases/$RELEASE_NAME"

echo "Creating Windows release package..."

# Create directory structure
mkdir -p "$RELEASE_DIR"/{lib/x64/{Release,Debug},include,examples,docs,scripts}

# Copy headers
echo "Copying headers..."
cp -r src/chtl/interfaces "$RELEASE_DIR/include/"
cp -r src/chtl/css "$RELEASE_DIR/include/"
cp -r src/chtl/javascript "$RELEASE_DIR/include/"

# Copy mock libraries
echo "Copying libraries..."
cp -r prebuilt/windows/x64/* "$RELEASE_DIR/lib/x64/"

# Copy single header
cp single_header/chtl_antlr_parsers.h "$RELEASE_DIR/include/"

# Copy examples
echo "Copying examples..."
cp -r example_windows_project/* "$RELEASE_DIR/examples/"

# Copy scripts
echo "Copying build scripts..."
cp build_all_windows.bat "$RELEASE_DIR/scripts/"
cp download_prebuilt.bat "$RELEASE_DIR/scripts/"

# Copy documentation
echo "Copying documentation..."
cp README_ANTLR_Windows.md "$RELEASE_DIR/docs/"
cp WINDOWS_BUILD_GUIDE.md "$RELEASE_DIR/docs/"

# Create main README
cat > "$RELEASE_DIR/README.md" << 'EOF'
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
EOF

# Create Visual Studio property sheet
cat > "$RELEASE_DIR/chtl-antlr.props" << 'EOF'
<?xml version="1.0" encoding="utf-8"?>
<Project ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ImportGroup Label="PropertySheets" />
  <PropertyGroup Label="UserMacros">
    <ChtlAntlrRoot>$(MSBuildThisFileDirectory)</ChtlAntlrRoot>
  </PropertyGroup>
  <PropertyGroup>
    <IncludePath>$(ChtlAntlrRoot)include;$(IncludePath)</IncludePath>
    <LibraryPath>$(ChtlAntlrRoot)lib\x64\$(Configuration);$(LibraryPath)</LibraryPath>
  </PropertyGroup>
  <ItemDefinitionGroup>
    <ClCompile>
      <LanguageStandard>stdcpp17</LanguageStandard>
      <AdditionalIncludeDirectories>$(ChtlAntlrRoot)include;%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>
    </ClCompile>
    <Link>
      <AdditionalLibraryDirectories>$(ChtlAntlrRoot)lib\x64\$(Configuration);%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>
      <AdditionalDependencies Condition="'$(Configuration)'=='Debug'">chtl_antlr_all_d.lib;%(AdditionalDependencies)</AdditionalDependencies>
      <AdditionalDependencies Condition="'$(Configuration)'=='Release'">chtl_antlr_all.lib;%(AdditionalDependencies)</AdditionalDependencies>
    </Link>
  </ItemDefinitionGroup>
</Project>
EOF

# Create NuGet package spec
cat > "$RELEASE_DIR/chtl-antlr.nuspec" << 'EOF'
<?xml version="1.0"?>
<package>
  <metadata>
    <id>CHTL.ANTLR.Parsers</id>
    <version>1.0.0</version>
    <title>CHTL ANTLR Parsers</title>
    <authors>CHTL Team</authors>
    <description>CSS and JavaScript parsers for CHTL compiler</description>
    <projectUrl>https://github.com/yourusername/chtl-compiler</projectUrl>
    <requireLicenseAcceptance>false</requireLicenseAcceptance>
    <tags>CHTL ANTLR Parser CSS JavaScript</tags>
  </metadata>
  <files>
    <file src="include\**" target="include" />
    <file src="lib\**" target="lib" />
    <file src="chtl-antlr.props" target="build\native" />
  </files>
</package>
EOF

# Create archive
echo "Creating archive..."
cd releases
zip -r "$RELEASE_NAME.zip" "$RELEASE_NAME"
tar -czf "$RELEASE_NAME.tar.gz" "$RELEASE_NAME"

echo "Release package created:"
echo "  - releases/$RELEASE_NAME.zip"
echo "  - releases/$RELEASE_NAME.tar.gz"
echo "  - releases/$RELEASE_NAME/ (directory)"

# Create checksum
echo "Creating checksums..."
sha256sum "$RELEASE_NAME.zip" > "$RELEASE_NAME.zip.sha256"
sha256sum "$RELEASE_NAME.tar.gz" > "$RELEASE_NAME.tar.gz.sha256"

echo "Done!"