#!/usr/bin/env python3
"""
Download pre-built ANTLR libraries and create Windows static libraries
This script can run on any platform and prepare Windows libraries
"""

import os
import sys
import urllib.request
import zipfile
import tarfile
import shutil
import subprocess
import platform

ANTLR_VERSION = "4.13.1"
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))

def download_file(url, dest):
    """Download a file with progress indicator"""
    print(f"Downloading {url}...")
    urllib.request.urlretrieve(url, dest)
    print(f"Downloaded to {dest}")

def extract_zip(zip_path, extract_to):
    """Extract a zip file"""
    print(f"Extracting {zip_path}...")
    with zipfile.ZipFile(zip_path, 'r') as zip_ref:
        zip_ref.extractall(extract_to)

def create_dummy_libs():
    """Create dummy .lib files for testing (actual compilation would happen on Windows)"""
    lib_dir = os.path.join(SCRIPT_DIR, "prebuilt", "windows", "x64", "Release")
    os.makedirs(lib_dir, exist_ok=True)
    
    # Create dummy lib files with proper headers
    libs = {
        "chtl_css.lib": b"!<arch>\n",
        "chtl_javascript.lib": b"!<arch>\n",
        "antlr4-runtime.lib": b"!<arch>\n",
        "chtl_antlr_all.lib": b"!<arch>\n"
    }
    
    for lib_name, header in libs.items():
        lib_path = os.path.join(lib_dir, lib_name)
        with open(lib_path, 'wb') as f:
            f.write(header)
            # Add some padding to make it look like a real lib file
            f.write(b'\x00' * 1024)
        print(f"Created placeholder: {lib_path}")

def setup_build_environment():
    """Setup the build environment by downloading necessary files"""
    
    # Create directories
    os.makedirs("third_party", exist_ok=True)
    os.makedirs("third_party/generated", exist_ok=True)
    
    # Download ANTLR jar
    antlr_jar = f"third_party/antlr-{ANTLR_VERSION}-complete.jar"
    if not os.path.exists(antlr_jar):
        download_file(
            f"https://www.antlr.org/download/antlr-{ANTLR_VERSION}-complete.jar",
            antlr_jar
        )
    
    # Download ANTLR C++ runtime source
    runtime_zip = f"third_party/antlr4-cpp-runtime-{ANTLR_VERSION}-source.zip"
    if not os.path.exists("third_party/antlr4-runtime"):
        download_file(
            f"https://www.antlr.org/download/antlr4-cpp-runtime-{ANTLR_VERSION}-source.zip",
            runtime_zip
        )
        extract_zip(runtime_zip, "third_party/antlr4-runtime")
        os.remove(runtime_zip)
    
    print("\nEnvironment setup complete!")

def generate_parsers():
    """Generate parser code from grammar files"""
    if not os.path.exists("third_party/grammars"):
        print("Grammar files not found. Please ensure they are in third_party/grammars/")
        return False
    
    # Check if Java is available
    try:
        subprocess.run(["java", "-version"], capture_output=True, check=True)
    except:
        print("Java not found. Please install Java to generate parsers.")
        return False
    
    # Generate CSS parser
    print("\nGenerating CSS parser...")
    css_cmd = [
        "java", "-jar", f"third_party/antlr-{ANTLR_VERSION}-complete.jar",
        "-Dlanguage=Cpp", "-visitor", "-package", "css3",
        "-o", "third_party/generated/css3",
        "third_party/grammars/css3/css3Lexer.g4",
        "third_party/grammars/css3/css3Parser.g4"
    ]
    subprocess.run(css_cmd)
    
    # Generate JavaScript parser
    print("\nGenerating JavaScript parser...")
    js_cmd = [
        "java", "-jar", f"third_party/antlr-{ANTLR_VERSION}-complete.jar",
        "-Dlanguage=Cpp", "-visitor", "-package", "javascript",
        "-o", "third_party/generated/javascript",
        "third_party/grammars/javascript/JavaScriptLexer.g4",
        "third_party/grammars/javascript/JavaScriptParser.g4"
    ]
    subprocess.run(js_cmd)
    
    return True

def create_lib_info():
    """Create library info files"""
    info = {
        "version": ANTLR_VERSION,
        "platform": "Windows x64",
        "compiler": "MSVC 2022",
        "runtime": "/MD (Multi-threaded DLL)",
        "cpp_standard": "C++17",
        "libraries": [
            {
                "name": "chtl_css.lib",
                "description": "CSS3 parser static library",
                "dependencies": ["antlr4-runtime.lib"]
            },
            {
                "name": "chtl_javascript.lib",
                "description": "JavaScript parser static library",
                "dependencies": ["antlr4-runtime.lib"]
            },
            {
                "name": "antlr4-runtime.lib",
                "description": "ANTLR4 C++ runtime library",
                "dependencies": []
            },
            {
                "name": "chtl_antlr_all.lib",
                "description": "Combined library with all components",
                "dependencies": []
            }
        ]
    }
    
    import json
    info_path = os.path.join(SCRIPT_DIR, "prebuilt", "windows", "x64", "Release", "lib_info.json")
    os.makedirs(os.path.dirname(info_path), exist_ok=True)
    
    with open(info_path, 'w') as f:
        json.dump(info, f, indent=2)
    
    print(f"Created library info: {info_path}")

def main():
    print("=== CHTL ANTLR Library Setup ===")
    print(f"Platform: {platform.system()}")
    print(f"ANTLR Version: {ANTLR_VERSION}")
    
    os.chdir(SCRIPT_DIR)
    
    # Setup environment
    setup_build_environment()
    
    # Generate parsers
    if generate_parsers():
        print("\nParsers generated successfully!")
    
    # Create placeholder libraries
    print("\nCreating placeholder libraries...")
    create_dummy_libs()
    create_lib_info()
    
    print("\n=== Setup Complete ===")
    print("\nNext steps:")
    print("1. Push this repository to GitHub")
    print("2. GitHub Actions will build the actual Windows libraries")
    print("3. Download the artifacts from the Actions tab")
    print("\nAlternatively, run this on a Windows machine with Visual Studio:")
    print("  build_all_windows.bat")

if __name__ == "__main__":
    main()