#!/bin/bash

# CHTL ANTLR运行时 - Linux构建脚本

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
RUNTIME_DIR="$(dirname "$SCRIPT_DIR")"

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
CHTL ANTLR运行时 Linux构建脚本

用法: $0 [选项]

选项:
    -h, --help          显示此帮助信息
    -a, --arch          目标架构 [x64|arm64] (默认: 自动检测)
    -t, --type          构建类型 [Release|Debug|RelWithDebInfo] (默认: Release)
    -j, --jobs          并行作业数 (默认: 自动检测)
    -c, --clean         清理之前的构建
    -v, --verbose       详细输出
    --static-only       仅构建静态库
    --shared-only       仅构建动态库
    --no-test           跳过测试
    --cross-compile     交叉编译 (需要指定--arch)
    --install-deps      安装构建依赖

示例:
    $0                          # 构建当前架构
    $0 -a arm64 --cross-compile # 交叉编译ARM64版本
    $0 -t Debug -j 8            # Debug构建，8个并行作业
    $0 --install-deps           # 安装依赖并构建

EOF
}

# 默认参数
ARCH=""
BUILD_TYPE="Release"
PARALLEL_JOBS=$(nproc)
CLEAN_BUILD=false
VERBOSE=false
STATIC_ONLY=false
SHARED_ONLY=false
NO_TEST=false
CROSS_COMPILE=false
INSTALL_DEPS=false

# 解析命令行参数
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_help
            exit 0
            ;;
        -a|--arch)
            ARCH="$2"
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
        --cross-compile)
            CROSS_COMPILE=true
            shift
            ;;
        --install-deps)
            INSTALL_DEPS=true
            shift
            ;;
        *)
            print_error "未知选项: $1"
            show_help
            exit 1
            ;;
    esac
done

# 检测架构
if [[ -z "$ARCH" ]]; then
    case $(uname -m) in
        x86_64|amd64)
            ARCH="x64"
            ;;
        aarch64|arm64)
            ARCH="arm64"
            ;;
        *)
            print_error "不支持的架构: $(uname -m)"
            exit 1
            ;;
    esac
fi

# 验证架构
if [[ "$ARCH" != "x64" && "$ARCH" != "arm64" ]]; then
    print_error "不支持的架构: $ARCH"
    exit 1
fi

PLATFORM_ID="linux-$ARCH"
BUILD_DIR="$RUNTIME_DIR/build/temp/$PLATFORM_ID"
INSTALL_DIR="$RUNTIME_DIR/prebuilt/$PLATFORM_ID"

print_info "Linux ANTLR运行时构建"
print_info "平台: $PLATFORM_ID"
print_info "构建类型: $BUILD_TYPE"
print_info "并行作业: $PARALLEL_JOBS"

# 安装依赖
install_dependencies() {
    print_info "安装构建依赖..."
    
    # 检测包管理器
    if command -v apt-get >/dev/null 2>&1; then
        # Ubuntu/Debian
        sudo apt-get update
        sudo apt-get install -y \
            build-essential \
            cmake \
            git \
            uuid-dev \
            pkg-config \
            libtool \
            autoconf \
            automake
        
        # 交叉编译工具链
        if [[ "$CROSS_COMPILE" == "true" && "$ARCH" == "arm64" ]]; then
            sudo apt-get install -y gcc-aarch64-linux-gnu g++-aarch64-linux-gnu
        fi
        
    elif command -v yum >/dev/null 2>&1; then
        # CentOS/RHEL
        sudo yum groupinstall -y "Development Tools"
        sudo yum install -y \
            cmake3 \
            git \
            libuuid-devel \
            pkgconfig \
            libtool \
            autoconf \
            automake
        
        # 创建cmake符号链接
        if ! command -v cmake >/dev/null 2>&1 && command -v cmake3 >/dev/null 2>&1; then
            sudo ln -sf /usr/bin/cmake3 /usr/bin/cmake
        fi
        
    elif command -v pacman >/dev/null 2>&1; then
        # Arch Linux
        sudo pacman -S --needed \
            base-devel \
            cmake \
            git \
            util-linux \
            pkgconfig \
            libtool \
            autoconf \
            automake
    else
        print_warning "未识别的包管理器，请手动安装依赖"
    fi
}

# 设置交叉编译环境
setup_cross_compile() {
    if [[ "$CROSS_COMPILE" != "true" ]]; then
        return
    fi
    
    print_info "设置交叉编译环境..."
    
    case "$ARCH" in
        arm64)
            if ! command -v aarch64-linux-gnu-gcc >/dev/null 2>&1; then
                print_error "ARM64交叉编译工具链未安装"
                print_info "请运行: sudo apt-get install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu"
                exit 1
            fi
            
            export CC=aarch64-linux-gnu-gcc
            export CXX=aarch64-linux-gnu-g++
            export AR=aarch64-linux-gnu-ar
            export STRIP=aarch64-linux-gnu-strip
            export CMAKE_SYSTEM_PROCESSOR=aarch64
            ;;
        *)
            print_error "不支持的交叉编译架构: $ARCH"
            exit 1
            ;;
    esac
}

# 准备ANTLR源码
prepare_source() {
    local src_dir="$RUNTIME_DIR/src/antlr4-runtime"
    
    if [[ ! -d "$src_dir" ]]; then
        print_info "下载ANTLR 4源码..."
        mkdir -p "$RUNTIME_DIR/src"
        cd "$RUNTIME_DIR/src"
        
        # 从GitHub下载ANTLR 4.13.2源码
        if ! git clone --depth 1 --branch 4.13.2 https://github.com/antlr/antlr4.git antlr4-full; then
            print_error "下载ANTLR源码失败"
            exit 1
        fi
        
        # 只需要C++运行时部分
        mv antlr4-full/runtime/Cpp antlr4-runtime
        rm -rf antlr4-full
        
        print_success "ANTLR源码准备完成"
    fi
}

# 应用Linux特定补丁
apply_patches() {
    local patches_dir="$RUNTIME_DIR/src/patches/linux"
    
    if [[ -d "$patches_dir" ]]; then
        print_info "应用Linux特定补丁..."
        
        for patch_file in "$patches_dir"/*.patch; do
            if [[ -f "$patch_file" ]]; then
                print_info "应用补丁: $(basename "$patch_file")"
                cd "$RUNTIME_DIR/src/antlr4-runtime"
                patch -p1 < "$patch_file"
            fi
        done
    fi
}

# 构建ANTLR运行时
build_runtime() {
    print_info "开始构建ANTLR运行时..."
    
    # 创建构建目录
    if [[ "$CLEAN_BUILD" == "true" ]] && [[ -d "$BUILD_DIR" ]]; then
        print_info "清理构建目录..."
        rm -rf "$BUILD_DIR"
    fi
    
    mkdir -p "$BUILD_DIR"
    mkdir -p "$INSTALL_DIR"
    
    cd "$BUILD_DIR"
    
    # 准备CMake参数
    local cmake_args=(
        -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
        -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR"
        -DCMAKE_CXX_STANDARD=17
        -DCMAKE_CXX_STANDARD_REQUIRED=ON
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON
        -DANTLR4_INSTALL=ON
        -DANTLR4_USE_THREAD_LOCAL_CACHE=ON
    )
    
    # 库类型设置
    if [[ "$STATIC_ONLY" == "true" ]]; then
        cmake_args+=(-DBUILD_SHARED_LIBS=OFF)
    elif [[ "$SHARED_ONLY" == "true" ]]; then
        cmake_args+=(-DBUILD_SHARED_LIBS=ON)
    else
        # 构建两种类型
        cmake_args+=(-DBUILD_SHARED_LIBS=ON)
    fi
    
    # 测试设置
    if [[ "$NO_TEST" == "true" ]]; then
        cmake_args+=(-DBUILD_TESTING=OFF)
    else
        cmake_args+=(-DBUILD_TESTING=ON)
    fi
    
    # 交叉编译设置
    if [[ "$CROSS_COMPILE" == "true" ]]; then
        case "$ARCH" in
            arm64)
                cmake_args+=(-DCMAKE_SYSTEM_NAME=Linux)
                cmake_args+=(-DCMAKE_SYSTEM_PROCESSOR=aarch64)
                cmake_args+=(-DCMAKE_C_COMPILER=aarch64-linux-gnu-gcc)
                cmake_args+=(-DCMAKE_CXX_COMPILER=aarch64-linux-gnu-g++)
                ;;
        esac
    fi
    
    # 详细输出
    if [[ "$VERBOSE" == "true" ]]; then
        cmake_args+=(-DCMAKE_VERBOSE_MAKEFILE=ON)
    fi
    
    print_info "配置CMake..."
    cmake "${cmake_args[@]}" "$RUNTIME_DIR/src/antlr4-runtime"
    
    print_info "开始编译 (使用 $PARALLEL_JOBS 个并行作业)..."
    make -j"$PARALLEL_JOBS"
    
    print_info "安装到: $INSTALL_DIR"
    make install
    
    # 构建静态库版本（如果需要）
    if [[ "$STATIC_ONLY" != "true" && "$SHARED_ONLY" != "true" ]]; then
        print_info "构建静态库版本..."
        
        local static_build_dir="$BUILD_DIR-static"
        mkdir -p "$static_build_dir"
        cd "$static_build_dir"
        
        cmake_args[-1]=-DBUILD_SHARED_LIBS=OFF  # 修改最后一个参数
        cmake "${cmake_args[@]}" "$RUNTIME_DIR/src/antlr4-runtime"
        make -j"$PARALLEL_JOBS"
        make install
    fi
}

# 运行测试
run_tests() {
    if [[ "$NO_TEST" == "true" ]]; then
        return
    fi
    
    print_info "运行测试..."
    cd "$BUILD_DIR"
    
    if ! ctest --output-on-failure -j"$PARALLEL_JOBS"; then
        print_warning "部分测试失败，但继续构建过程"
    else
        print_success "所有测试通过"
    fi
}

# 验证构建结果
verify_build() {
    print_info "验证构建结果..."
    
    local lib_dir="$INSTALL_DIR/lib"
    local include_dir="$INSTALL_DIR/include"
    
    # 检查头文件
    if [[ ! -d "$include_dir/antlr4-runtime" ]]; then
        print_error "头文件目录不存在: $include_dir/antlr4-runtime"
        return 1
    fi
    
    # 检查库文件
    local expected_libs=()
    if [[ "$SHARED_ONLY" != "true" ]]; then
        expected_libs+=(libantlr4-runtime.a)
    fi
    if [[ "$STATIC_ONLY" != "true" ]]; then
        expected_libs+=(libantlr4-runtime.so)
    fi
    
    for lib in "${expected_libs[@]}"; do
        if [[ ! -f "$lib_dir/$lib" ]]; then
            print_error "库文件不存在: $lib_dir/$lib"
            return 1
        fi
        
        print_success "找到库文件: $lib"
        
        # 显示库信息
        if command -v file >/dev/null 2>&1; then
            file "$lib_dir/$lib"
        fi
        
        if [[ "$lib" == *.so ]] && command -v ldd >/dev/null 2>&1; then
            echo "依赖关系:"
            ldd "$lib_dir/$lib" || true
        fi
    done
    
    print_success "构建验证通过"
}

# 创建平台信息文件
create_platform_info() {
    local info_file="$INSTALL_DIR/platform-info.txt"
    
    cat > "$info_file" << EOF
CHTL ANTLR Runtime Build Information
====================================

Platform: $PLATFORM_ID
Build Type: $BUILD_TYPE
Build Date: $(date -u +"%Y-%m-%d %H:%M:%S UTC")
Build Host: $(hostname)

System Information:
- OS: $(uname -s) $(uname -r)
- Architecture: $(uname -m)
- Compiler: $(gcc --version | head -n1 2>/dev/null || echo "Unknown")
- CMake: $(cmake --version | head -n1 2>/dev/null || echo "Unknown")

Build Configuration:
- Static Library: $([ "$SHARED_ONLY" != "true" ] && echo "Yes" || echo "No")
- Shared Library: $([ "$STATIC_ONLY" != "true" ] && echo "Yes" || echo "No")
- Cross Compile: $([ "$CROSS_COMPILE" == "true" ] && echo "Yes" || echo "No")
- Tests Built: $([ "$NO_TEST" != "true" ] && echo "Yes" || echo "No")
- Parallel Jobs: $PARALLEL_JOBS

ANTLR Version: 4.13.2
C++ Standard: 17
EOF

    print_info "平台信息已保存到: $info_file"
}

# 主函数
main() {
    print_info "开始Linux ANTLR运行时构建..."
    
    # 安装依赖
    if [[ "$INSTALL_DEPS" == "true" ]]; then
        install_dependencies
    fi
    
    # 检查必要工具
    if ! command -v cmake >/dev/null 2>&1; then
        print_error "CMake未安装，请运行 --install-deps 或手动安装"
        exit 1
    fi
    
    if ! command -v make >/dev/null 2>&1; then
        print_error "Make未安装，请运行 --install-deps 或手动安装"
        exit 1
    fi
    
    # 设置交叉编译
    setup_cross_compile
    
    # 准备源码
    prepare_source
    
    # 应用补丁
    apply_patches
    
    # 构建
    build_runtime
    
    # 运行测试
    run_tests
    
    # 验证构建
    verify_build
    
    # 创建平台信息
    create_platform_info
    
    print_success "Linux ANTLR运行时构建完成！"
    print_info "安装位置: $INSTALL_DIR"
    
    # 显示使用说明
    cat << EOF

使用说明:
1. 在CMakeLists.txt中添加:
   set(ANTLR4_ROOT $INSTALL_DIR)
   find_package(PkgConfig REQUIRED)
   pkg_check_modules(ANTLR4 REQUIRED antlr4-runtime)

2. 链接库文件:
   target_link_libraries(your_target \${ANTLR4_LIBRARIES})
   target_include_directories(your_target PRIVATE \${ANTLR4_INCLUDE_DIRS})

EOF
}

# 运行主函数
main "$@"