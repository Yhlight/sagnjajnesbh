#!/bin/bash

# CHTL ANTLR运行时 - 全平台构建脚本
# 在支持的平台上构建ANTLR运行时库

set -e

# 脚本目录
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
RUNTIME_DIR="$(dirname "$SCRIPT_DIR")"
PROJECT_ROOT="$(dirname "$RUNTIME_DIR")"

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

print_info() { echo -e "${BLUE}[INFO]${NC} $1"; }
print_success() { echo -e "${GREEN}[SUCCESS]${NC} $1"; }
print_warning() { echo -e "${YELLOW}[WARNING]${NC} $1"; }
print_error() { echo -e "${RED}[ERROR]${NC} $1"; }

# 显示帮助信息
show_help() {
    cat << EOF
CHTL ANTLR运行时全平台构建脚本

用法: $0 [选项]

选项:
    -h, --help          显示此帮助信息
    -p, --platforms     指定要构建的平台 (逗号分隔)
    -t, --type          构建类型 [Release|Debug|RelWithDebInfo] (默认: Release)
    -j, --jobs          并行作业数 (默认: 自动检测)
    -c, --clean         清理之前的构建
    -v, --verbose       详细输出
    --static-only       仅构建静态库
    --shared-only       仅构建动态库
    --no-test           跳过测试
    --docker            使用Docker构建

支持的平台:
    linux-x64           Linux x86_64
    linux-arm64         Linux ARM64
    windows-x64         Windows x86_64
    windows-x86         Windows x86
    macos-x64           macOS Intel
    macos-arm64         macOS Apple Silicon

示例:
    $0                                  # 构建当前平台
    $0 -p linux-x64,linux-arm64        # 构建指定平台
    $0 -t Debug -j 4                   # Debug构建，4个并行作业
    $0 --clean --static-only            # 清理并仅构建静态库

EOF
}

# 默认参数
BUILD_TYPE="Release"
PARALLEL_JOBS=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
CLEAN_BUILD=false
VERBOSE=false
STATIC_ONLY=false
SHARED_ONLY=false
NO_TEST=false
USE_DOCKER=false
PLATFORMS=""

# 解析命令行参数
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_help
            exit 0
            ;;
        -p|--platforms)
            PLATFORMS="$2"
            shift 2
            ;;
        -t|--type)
            BUILD_TYPE="$2"
            shift 2
            ;;
        -j|--jobs)
            PARALLEL_JOBS="$2"
            shift 2
            ;;
        -c|--clean)
            CLEAN_BUILD=true
            shift
            ;;
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        --static-only)
            STATIC_ONLY=true
            shift
            ;;
        --shared-only)
            SHARED_ONLY=true
            shift
            ;;
        --no-test)
            NO_TEST=true
            shift
            ;;
        --docker)
            USE_DOCKER=true
            shift
            ;;
        *)
            print_error "未知选项: $1"
            show_help
            exit 1
            ;;
    esac
done

# 检测当前平台
detect_current_platform() {
    local platform_info
    if ! platform_info=$("$RUNTIME_DIR/tools/detect-platform.sh" 2>/dev/null); then
        print_error "无法检测当前平台"
        exit 1
    fi
    
    local platform_id=$(echo "$platform_info" | grep "PLATFORM_ID=" | cut -d= -f2)
    echo "$platform_id"
}

# 验证平台是否支持
validate_platform() {
    local platform=$1
    local supported_platforms=(
        "linux-x64"
        "linux-arm64"
        "windows-x64"
        "windows-x86"
        "macos-x64"
        "macos-arm64"
    )
    
    for supported in "${supported_platforms[@]}"; do
        if [[ "$platform" == "$supported" ]]; then
            return 0
        fi
    done
    return 1
}

# 构建单个平台
build_platform() {
    local platform=$1
    local build_dir="$RUNTIME_DIR/build/temp/$platform"
    local install_dir="$RUNTIME_DIR/prebuilt/$platform"
    
    print_info "开始构建平台: $platform"
    
    # 创建构建目录
    if [[ "$CLEAN_BUILD" == "true" ]] && [[ -d "$build_dir" ]]; then
        print_info "清理构建目录: $build_dir"
        rm -rf "$build_dir"
    fi
    
    mkdir -p "$build_dir"
    mkdir -p "$install_dir"
    
    # 根据平台选择构建脚本
    case $platform in
        linux-*)
            build_linux "$platform" "$build_dir" "$install_dir"
            ;;
        windows-*)
            build_windows "$platform" "$build_dir" "$install_dir"
            ;;
        macos-*)
            build_macos "$platform" "$build_dir" "$install_dir"
            ;;
        *)
            print_error "不支持的平台: $platform"
            return 1
            ;;
    esac
    
    print_success "平台 $platform 构建完成"
}

# Linux构建
build_linux() {
    local platform=$1
    local build_dir=$2
    local install_dir=$3
    
    print_info "配置Linux构建环境..."
    
    cd "$build_dir"
    
    # 设置架构特定的编译器标志
    local cmake_args=()
    if [[ "$platform" == "linux-arm64" ]]; then
        cmake_args+=(-DCMAKE_SYSTEM_PROCESSOR=aarch64)
        if command -v aarch64-linux-gnu-gcc >/dev/null 2>&1; then
            cmake_args+=(-DCMAKE_C_COMPILER=aarch64-linux-gnu-gcc)
            cmake_args+=(-DCMAKE_CXX_COMPILER=aarch64-linux-gnu-g++)
        fi
    fi
    
    # 配置CMake
    cmake \
        -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
        -DCMAKE_INSTALL_PREFIX="$install_dir" \
        -DCMAKE_CXX_STANDARD=17 \
        -DBUILD_SHARED_LIBS=$([ "$STATIC_ONLY" == "true" ] && echo "OFF" || echo "ON") \
        -DBUILD_TESTING=$([ "$NO_TEST" == "true" ] && echo "OFF" || echo "ON") \
        -DANTLR4_INSTALL=ON \
        "${cmake_args[@]}" \
        "$RUNTIME_DIR/src/antlr4-runtime"
    
    # 构建
    make -j"$PARALLEL_JOBS"
    
    # 安装
    make install
    
    # 运行测试
    if [[ "$NO_TEST" != "true" ]]; then
        print_info "运行测试..."
        ctest --output-on-failure
    fi
}

# Windows构建
build_windows() {
    local platform=$1
    local build_dir=$2
    local install_dir=$3
    
    print_info "配置Windows构建环境..."
    
    cd "$build_dir"
    
    # 设置架构
    local arch="x64"
    if [[ "$platform" == "windows-x86" ]]; then
        arch="Win32"
    fi
    
    # 配置CMake（使用Visual Studio生成器）
    cmake \
        -G "Visual Studio 16 2019" \
        -A "$arch" \
        -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
        -DCMAKE_INSTALL_PREFIX="$install_dir" \
        -DCMAKE_CXX_STANDARD=17 \
        -DBUILD_SHARED_LIBS=$([ "$STATIC_ONLY" == "true" ] && echo "OFF" || echo "ON") \
        -DBUILD_TESTING=$([ "$NO_TEST" == "true" ] && echo "OFF" || echo "ON") \
        -DANTLR4_INSTALL=ON \
        "$RUNTIME_DIR/src/antlr4-runtime"
    
    # 构建
    cmake --build . --config "$BUILD_TYPE" --parallel "$PARALLEL_JOBS"
    
    # 安装
    cmake --build . --config "$BUILD_TYPE" --target install
    
    # 运行测试
    if [[ "$NO_TEST" != "true" ]]; then
        print_info "运行测试..."
        ctest -C "$BUILD_TYPE" --output-on-failure
    fi
}

# macOS构建
build_macos() {
    local platform=$1
    local build_dir=$2
    local install_dir=$3
    
    print_info "配置macOS构建环境..."
    
    cd "$build_dir"
    
    # 设置架构特定的编译器标志
    local cmake_args=()
    if [[ "$platform" == "macos-arm64" ]]; then
        cmake_args+=(-DCMAKE_OSX_ARCHITECTURES=arm64)
        cmake_args+=(-DCMAKE_OSX_DEPLOYMENT_TARGET=11.0)
    else
        cmake_args+=(-DCMAKE_OSX_ARCHITECTURES=x86_64)
        cmake_args+=(-DCMAKE_OSX_DEPLOYMENT_TARGET=10.15)
    fi
    
    # 配置CMake
    cmake \
        -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
        -DCMAKE_INSTALL_PREFIX="$install_dir" \
        -DCMAKE_CXX_STANDARD=17 \
        -DBUILD_SHARED_LIBS=$([ "$STATIC_ONLY" == "true" ] && echo "OFF" || echo "ON") \
        -DBUILD_TESTING=$([ "$NO_TEST" == "true" ] && echo "OFF" || echo "ON") \
        -DANTLR4_INSTALL=ON \
        "${cmake_args[@]}" \
        "$RUNTIME_DIR/src/antlr4-runtime"
    
    # 构建
    make -j"$PARALLEL_JOBS"
    
    # 安装
    make install
    
    # 运行测试
    if [[ "$NO_TEST" != "true" ]]; then
        print_info "运行测试..."
        ctest --output-on-failure
    fi
}

# 主函数
main() {
    print_info "CHTL ANTLR运行时全平台构建开始..."
    
    # 检查必要工具
    if ! command -v cmake >/dev/null 2>&1; then
        print_error "CMake未安装"
        exit 1
    fi
    
    # 确定要构建的平台
    local platforms_to_build=()
    
    if [[ -n "$PLATFORMS" ]]; then
        IFS=',' read -ra platforms_to_build <<< "$PLATFORMS"
    else
        # 检测当前平台
        local current_platform
        current_platform=$(detect_current_platform)
        platforms_to_build=("$current_platform")
        print_info "自动检测到平台: $current_platform"
    fi
    
    # 验证平台
    for platform in "${platforms_to_build[@]}"; do
        if ! validate_platform "$platform"; then
            print_error "不支持的平台: $platform"
            exit 1
        fi
    done
    
    print_info "将构建以下平台: ${platforms_to_build[*]}"
    print_info "构建类型: $BUILD_TYPE"
    print_info "并行作业数: $PARALLEL_JOBS"
    
    # 构建每个平台
    local success_count=0
    local total_count=${#platforms_to_build[@]}
    
    for platform in "${platforms_to_build[@]}"; do
        print_info "[$((success_count + 1))/$total_count] 构建平台: $platform"
        
        if build_platform "$platform"; then
            ((success_count++))
        else
            print_error "平台 $platform 构建失败"
        fi
    done
    
    # 总结
    echo
    print_info "=== 构建总结 ==="
    print_success "成功构建: $success_count/$total_count 个平台"
    
    if [[ $success_count -eq $total_count ]]; then
        print_success "所有平台构建成功！"
        
        # 显示构建结果
        print_info "构建结果位置:"
        for platform in "${platforms_to_build[@]}"; do
            local install_dir="$RUNTIME_DIR/prebuilt/$platform"
            if [[ -d "$install_dir" ]]; then
                echo "  $platform: $install_dir"
            fi
        done
        
        exit 0
    else
        print_error "部分平台构建失败"
        exit 1
    fi
}

# 运行主函数
main "$@"