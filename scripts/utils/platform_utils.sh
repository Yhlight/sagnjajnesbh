#!/bin/bash

# CHTL Platform Utilities
# Cross-platform utility functions for build scripts
# Provides platform detection and compatibility functions

# Platform detection
detect_platform() {
    case "$(uname -s)" in
        Linux*)     echo "Linux";;
        Darwin*)    echo "macOS";;
        CYGWIN*)    echo "Windows";;
        MINGW*)     echo "Windows";;
        MSYS*)      echo "Windows";;
        *)          echo "Unknown";;
    esac
}

# Get number of CPU cores
get_cpu_cores() {
    case "$(detect_platform)" in
        "Linux")
            nproc 2>/dev/null || echo "4"
            ;;
        "macOS")
            sysctl -n hw.ncpu 2>/dev/null || echo "4"
            ;;
        "Windows")
            echo "${NUMBER_OF_PROCESSORS:-4}"
            ;;
        *)
            echo "4"
            ;;
    esac
}

# Check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Get file size in bytes
get_file_size() {
    local file="$1"
    
    if [ -f "$file" ]; then
        case "$(detect_platform)" in
            "Linux")
                stat -c%s "$file" 2>/dev/null || echo "0"
                ;;
            "macOS")
                stat -f%z "$file" 2>/dev/null || echo "0"
                ;;
            *)
                wc -c < "$file" 2>/dev/null || echo "0"
                ;;
        esac
    else
        echo "0"
    fi
}

# Create cross-platform timestamp
get_timestamp() {
    date +"%Y%m%d_%H%M%S"
}

# Normalize path separators
normalize_path() {
    local path="$1"
    echo "$path" | sed 's|\\|/|g'
}

# Check prerequisites for CHTL build
check_build_prerequisites() {
    local missing=0
    
    echo "🔍 Checking build prerequisites..."
    
    # Check CMake
    if command_exists cmake; then
        local cmake_version=$(cmake --version | head -n1 | grep -o '[0-9]\+\.[0-9]\+\.[0-9]\+')
        echo "  ✅ CMake: $cmake_version"
    else
        echo "  ❌ CMake not found"
        ((missing++))
    fi
    
    # Check C++ compiler
    if command_exists g++; then
        local gcc_version=$(g++ --version | head -n1 | grep -o '[0-9]\+\.[0-9]\+\.[0-9]\+' | head -1)
        echo "  ✅ GCC: $gcc_version"
    elif command_exists clang++; then
        local clang_version=$(clang++ --version | head -n1 | grep -o '[0-9]\+\.[0-9]\+\.[0-9]\+' | head -1)
        echo "  ✅ Clang: $clang_version"
    else
        echo "  ❌ C++ compiler not found"
        ((missing++))
    fi
    
    # Check Make
    if command_exists make; then
        echo "  ✅ Make: available"
    else
        echo "  ❌ Make not found"
        ((missing++))
    fi
    
    # Check ZIP
    if command_exists zip; then
        echo "  ✅ ZIP: available"
    else
        echo "  ⚠️ ZIP not found (required for module packaging)"
    fi
    
    # Check Git
    if command_exists git; then
        local git_version=$(git --version | grep -o '[0-9]\+\.[0-9]\+\.[0-9]\+')
        echo "  ✅ Git: $git_version"
    else
        echo "  ⚠️ Git not found"
    fi
    
    return $missing
}

# Install missing prerequisites (Linux/macOS)
install_prerequisites() {
    local platform=$(detect_platform)
    
    echo "🔧 Installing missing prerequisites for $platform..."
    
    case "$platform" in
        "Linux")
            if command_exists apt-get; then
                sudo apt-get update
                sudo apt-get install -y cmake build-essential zip git
            elif command_exists yum; then
                sudo yum install -y cmake gcc-c++ zip git
            elif command_exists pacman; then
                sudo pacman -S cmake gcc zip git
            else
                echo "  ⚠️ Unknown package manager, please install manually"
            fi
            ;;
        "macOS")
            if command_exists brew; then
                brew install cmake zip git
                # Xcode command line tools for compiler
                xcode-select --install 2>/dev/null || true
            else
                echo "  ⚠️ Homebrew not found, please install manually"
            fi
            ;;
        "Windows")
            echo "  ℹ️ For Windows, please install:"
            echo "    - Visual Studio or MinGW"
            echo "    - CMake"
            echo "    - Git"
            echo "    - 7-Zip"
            ;;
        *)
            echo "  ⚠️ Unknown platform, please install manually"
            ;;
    esac
}

# Create cross-platform executable script
create_executable() {
    local script_path="$1"
    
    chmod +x "$script_path"
    
    # Create .bat wrapper for Windows compatibility
    local bat_path="${script_path%.sh}.bat"
    if [ ! -f "$bat_path" ]; then
        cat > "$bat_path" << 'EOF'
@echo off
bash "%~dp0%~n0.sh" %*
EOF
    fi
}

# Validate script environment
validate_environment() {
    local errors=0
    
    echo "🌍 Validating script environment..."
    
    # Check if running in correct directory
    if [ ! -f "CMakeLists.txt" ]; then
        echo "  ❌ Not running from CHTL project root"
        ((errors++))
    else
        echo "  ✅ Running from project root"
    fi
    
    # Check essential directories
    local required_dirs=("src" "scripts")
    for dir in "${required_dirs[@]}"; do
        if [ -d "$dir" ]; then
            echo "  ✅ Directory exists: $dir"
        else
            echo "  ❌ Missing directory: $dir"
            ((errors++))
        fi
    done
    
    return $errors
}

# Export functions for use in other scripts
export -f detect_platform
export -f get_cpu_cores
export -f command_exists
export -f get_file_size
export -f get_timestamp
export -f normalize_path
export -f check_build_prerequisites
export -f install_prerequisites
export -f create_executable
export -f validate_environment