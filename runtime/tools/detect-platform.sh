#!/bin/bash

# CHTL ANTLR运行时平台检测脚本
# 检测当前系统平台并返回对应的预构建目录名

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 打印带颜色的信息
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# 检测操作系统
detect_os() {
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        echo "linux"
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        echo "macos"
    elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]] || [[ "$OSTYPE" == "win32" ]]; then
        echo "windows"
    else
        echo "unknown"
    fi
}

# 检测CPU架构
detect_arch() {
    local arch=$(uname -m)
    case $arch in
        x86_64|amd64)
            echo "x64"
            ;;
        aarch64|arm64)
            echo "arm64"
            ;;
        i386|i686)
            echo "x86"
            ;;
        armv7l)
            echo "arm32"
            ;;
        *)
            echo "unknown"
            ;;
    esac
}

# 检测Linux发行版
detect_linux_distro() {
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        echo "$ID"
    elif [ -f /etc/redhat-release ]; then
        echo "rhel"
    elif [ -f /etc/debian_version ]; then
        echo "debian"
    else
        echo "unknown"
    fi
}

# 检测编译器
detect_compiler() {
    local os=$1
    
    case $os in
        linux)
            if command -v gcc >/dev/null 2>&1; then
                local gcc_version=$(gcc --version | head -n1 | grep -oE '[0-9]+\.[0-9]+\.[0-9]+')
                echo "gcc-$gcc_version"
            elif command -v clang >/dev/null 2>&1; then
                local clang_version=$(clang --version | head -n1 | grep -oE '[0-9]+\.[0-9]+\.[0-9]+')
                echo "clang-$clang_version"
            else
                echo "none"
            fi
            ;;
        macos)
            if command -v clang >/dev/null 2>&1; then
                local clang_version=$(clang --version | head -n1 | grep -oE '[0-9]+\.[0-9]+\.[0-9]+')
                echo "clang-$clang_version"
            else
                echo "none"
            fi
            ;;
        windows)
            if command -v cl >/dev/null 2>&1; then
                echo "msvc"
            elif command -v gcc >/dev/null 2>&1; then
                echo "mingw"
            else
                echo "none"
            fi
            ;;
        *)
            echo "unknown"
            ;;
    esac
}

# 检测CMake
detect_cmake() {
    if command -v cmake >/dev/null 2>&1; then
        local cmake_version=$(cmake --version | head -n1 | grep -oE '[0-9]+\.[0-9]+\.[0-9]+')
        echo "$cmake_version"
    else
        echo "none"
    fi
}

# 主函数
main() {
    print_info "检测平台信息..."
    
    local os=$(detect_os)
    local arch=$(detect_arch)
    local compiler=$(detect_compiler "$os")
    local cmake_version=$(detect_cmake)
    
    print_info "操作系统: $os"
    print_info "CPU架构: $arch"
    print_info "编译器: $compiler"
    print_info "CMake版本: $cmake_version"
    
    # 构建平台标识符
    local platform_id="${os}-${arch}"
    
    # 检查是否支持该平台
    local supported_platforms=(
        "linux-x64"
        "linux-arm64"
        "macos-x64"
        "macos-arm64"
        "windows-x64"
        "windows-x86"
    )
    
    local is_supported=false
    for supported in "${supported_platforms[@]}"; do
        if [[ "$platform_id" == "$supported" ]]; then
            is_supported=true
            break
        fi
    done
    
    if [[ "$is_supported" == "true" ]]; then
        print_success "平台 '$platform_id' 受支持"
        
        # 检查预构建是否存在
        local runtime_dir="$(dirname "$(dirname "$(readlink -f "$0")")")"
        local prebuilt_dir="$runtime_dir/prebuilt/$platform_id"
        
        if [ -d "$prebuilt_dir" ]; then
            print_success "找到预构建运行时: $prebuilt_dir"
        else
            print_warning "预构建运行时不存在: $prebuilt_dir"
            print_info "可以运行构建脚本来创建它"
        fi
        
        # 检查构建要求
        print_info "检查构建要求..."
        
        local requirements_met=true
        
        if [[ "$cmake_version" == "none" ]]; then
            print_error "CMake未安装"
            requirements_met=false
        else
            local cmake_major=$(echo "$cmake_version" | cut -d. -f1)
            local cmake_minor=$(echo "$cmake_version" | cut -d. -f2)
            if [[ $cmake_major -lt 3 ]] || [[ $cmake_major -eq 3 && $cmake_minor -lt 16 ]]; then
                print_error "CMake版本过低，需要3.16+，当前: $cmake_version"
                requirements_met=false
            else
                print_success "CMake版本满足要求: $cmake_version"
            fi
        fi
        
        if [[ "$compiler" == "none" ]]; then
            print_error "未找到合适的C++编译器"
            requirements_met=false
        else
            print_success "找到编译器: $compiler"
        fi
        
        # 输出结果
        echo
        print_info "=== 平台检测结果 ==="
        echo "PLATFORM_ID=$platform_id"
        echo "OS=$os"
        echo "ARCH=$arch"
        echo "COMPILER=$compiler"
        echo "CMAKE_VERSION=$cmake_version"
        echo "SUPPORTED=$is_supported"
        echo "REQUIREMENTS_MET=$requirements_met"
        
        if [[ "$requirements_met" == "true" ]]; then
            print_success "所有要求都满足，可以进行构建"
            exit 0
        else
            print_error "构建要求不满足"
            exit 1
        fi
        
    else
        print_error "平台 '$platform_id' 暂不支持"
        print_info "支持的平台: ${supported_platforms[*]}"
        exit 1
    fi
}

# 如果作为脚本直接运行
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    main "$@"
fi