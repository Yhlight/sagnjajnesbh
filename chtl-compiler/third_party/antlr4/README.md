# ANTLR4 Runtime for CHTL Compiler

This directory contains the ANTLR4 runtime library and tools needed for the CSS and JavaScript parsers.

## Structure

- `bin/` - ANTLR4 tool JAR and scripts
- `include/` - C++ runtime headers
- `lib/` - Compiled runtime libraries
- `runtime/` - Runtime source code

## Version

ANTLR 4.13.1

## Building

Run the build script:
```bash
../scripts/build_antlr4.sh
```

## Usage

1. Generate parsers from grammar:
```bash
./bin/antlr4 -Dlanguage=Cpp -o output_dir grammar.g4
```

2. Link with runtime:
```cmake
target_link_libraries(your_target antlr4-runtime)
```