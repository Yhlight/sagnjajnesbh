#!/bin/bash

# CHTL ANTLR运行时 - macOS构建脚本

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
CHTL ANTLR运行时 macOS构建脚本

用法: $0 [选项]

选项:
    -h, --help          显示此帮助信息
    -a, --arch          目标架构 [x64|arm64|universal] (默认: 自动检测)
    -t, --type          构建类型 [Release|Debug|RelWithDebInfo] (默认: Release)
    -j, --jobs          并行作业数 (默认: 自动检测)
    -c, --clean         清理之前的构建
    -v, --verbose       详细输出
    --static-only       仅构建静态库
    --shared-only       仅构建动态库
    --no-test           跳过测试
    --min-version       最低macOS版本 (默认: 自动检测)
    --install-deps      安装构建依赖 (使用Homebrew)
    --universal         构建Universal Binary (x64+arm64)

示例:
    $0                          # 构建当前架构
    $0 -a arm64                 # 构建Apple Silicon版本
    $0 --universal              # 构建Universal Binary
    $0 -t Debug -j 8            # Debug构建，8个并行作业
    $0 --install-deps           # 安装依赖并构建

EOF
}

# 默认参数
ARCH=""
BUILD_TYPE="Release"
PARALLEL_JOBS=$(sysctl -n hw.ncpu)
CLEAN_BUILD=false
VERBOSE=false
STATIC_ONLY=false
SHARED_ONLY=false
NO_TEST=false
MIN_MACOS_VERSION=""
INSTALL_DEPS=false
UNIVERSAL_BINARY=false

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
        --min-version)
            MIN_MACOS_VERSION="$2"
            shift 2
            ;;
        --install-deps)
            INSTALL_DEPS=true
            shift
            ;;
        --universal)
            UNIVERSAL_BINARY=true
            ARCH="universal"
            shift
            ;;
        *)
            print_error "未知选项: $1"
            show_help
            exit 1
            ;;
    esac
done

# 检测当前系统
detect_system() {
    # 检查是否在macOS上运行
    if [[ "$(uname -s)" != "Darwin" ]]; then
        print_error "此脚本只能在macOS上运行"
        exit 1
    fi
    
    # 检测架构
    if [[ -z "$ARCH" ]]; then
        case $(uname -m) in
            x86_64)
                ARCH="x64"
                ;;
            arm64)
                ARCH="arm64"
                ;;
            *)
                print_error "不支持的架构: $(uname -m)"
                exit 1
                ;;
        esac
    fi
    
    # 验证架构
    if [[ "$ARCH" != "x64" && "$ARCH" != "arm64" && "$ARCH" != "universal" ]]; then
        print_error "不支持的架构: $ARCH"
        exit 1
    fi
    
    # 检测macOS版本
    local macos_version=$(sw_vers -productVersion)
    local major_version=$(echo "$macos_version" | cut -d. -f1)
    local minor_version=$(echo "$macos_version" | cut -d. -f2)
    
    print_info "macOS版本: $macos_version"
    
    # 设置默认最低版本
    if [[ -z "$MIN_MACOS_VERSION" ]]; then
        if [[ "$ARCH" == "arm64" ]] || [[ "$UNIVERSAL_BINARY" == "true" ]]; then
            MIN_MACOS_VERSION="11.0"  # Apple Silicon需要macOS 11+
        else
            MIN_MACOS_VERSION="10.15"  # Intel Mac可以支持更老版本
        fi
    fi
    
    print_info "目标最低macOS版本: $MIN_MACOS_VERSION"
}

# 设置平台标识符
if [[ "$UNIVERSAL_BINARY" == "true" ]]; then
    PLATFORM_ID="macos-universal"
else
    PLATFORM_ID="macos-$ARCH"
fi

BUILD_DIR="$RUNTIME_DIR/build/temp/$PLATFORM_ID"
INSTALL_DIR="$RUNTIME_DIR/prebuilt/$PLATFORM_ID"

# 安装依赖
install_dependencies() {
    print_info "安装构建依赖..."
    
    # 检查Homebrew
    if ! command -v brew >/dev/null 2>&1; then
        print_info "安装Homebrew..."
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
        
        # 添加Homebrew到PATH
        if [[ -f "/opt/homebrew/bin/brew" ]]; then
            eval "$(/opt/homebrew/bin/brew shellenv)"
        elif [[ -f "/usr/local/bin/brew" ]]; then
            eval "$(/usr/local/bin/brew shellenv)"
        fi
    fi
    
    # 安装必要工具
    local packages=(
        cmake
        git
        pkg-config
        libtool
        autoconf
        automake
    )
    
    for package in "${packages[@]}"; do
        if ! brew list "$package" >/dev/null 2>&1; then
            print_info "安装 $package..."
            brew install "$package"
        else
            print_success "$package 已安装"
        fi
    done
    
    # 检查Xcode Command Line Tools
    if ! xcode-select -p >/dev/null 2>&1; then
        print_info "安装Xcode Command Line Tools..."
        xcode-select --install
        
        print_warning "请完成Xcode Command Line Tools安装后重新运行此脚本"
        exit 1
    fi
    
    print_success "依赖安装完成"
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

# 应用macOS特定补丁
apply_patches() {
    local patches_dir="$RUNTIME_DIR/src/patches/macos"
    
    if [[ -d "$patches_dir" ]]; then
        print_info "应用macOS特定补丁..."
        
        for patch_file in "$patches_dir"/*.patch; do
            if [[ -f "$patch_file" ]]; then
                print_info "应用补丁: $(basename "$patch_file")"
                cd "$RUNTIME_DIR/src/antlr4-runtime"
                patch -p1 < "$patch_file"
            fi
        done
    fi
}

# 构建单架构版本
build_single_arch() {
    local target_arch=$1
    local arch_build_dir="$BUILD_DIR-$target_arch"
    local arch_install_dir="$INSTALL_DIR-$target_arch"
    
    print_info "构建 $target_arch 架构版本..."
    
    # 创建构建目录
    mkdir -p "$arch_build_dir"
    mkdir -p "$arch_install_dir"
    
    cd "$arch_build_dir"
    
    # 准备CMake参数
    local cmake_args=(
        -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
        -DCMAKE_INSTALL_PREFIX="$arch_install_dir"
        -DCMAKE_CXX_STANDARD=17
        -DCMAKE_CXX_STANDARD_REQUIRED=ON
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON
        -DCMAKE_OSX_DEPLOYMENT_TARGET="$MIN_MACOS_VERSION"
        -DANTLR4_INSTALL=ON
        -DANTLR4_USE_THREAD_LOCAL_CACHE=ON
    )
    
    # 架构特定设置
    if [[ "$target_arch" == "arm64" ]]; then
        cmake_args+=(-DCMAKE_OSX_ARCHITECTURES=arm64)
        # Apple Silicon特定优化
        cmake_args+=(-DCMAKE_CXX_FLAGS="-mcpu=apple-a14")
    else
        cmake_args+=(-DCMAKE_OSX_ARCHITECTURES=x86_64)
        # Intel特定优化
        cmake_args+=(-DCMAKE_CXX_FLAGS="-march=nehalem -mtune=intel")
    fi
    
    # 库类型设置
    if [[ "$STATIC_ONLY" == "true" ]]; then
        cmake_args+=(-DBUILD_SHARED_LIBS=OFF)
    elif [[ "$SHARED_ONLY" == "true" ]]; then
        cmake_args+=(-DBUILD_SHARED_LIBS=ON)
    else
        cmake_args+=(-DBUILD_SHARED_LIBS=ON)
    fi
    
    # 测试设置
    if [[ "$NO_TEST" == "true" ]]; then
        cmake_args+=(-DBUILD_TESTING=OFF)
    else
        cmake_args+=(-DBUILD_TESTING=ON)
    fi
    
    # 详细输出
    if [[ "$VERBOSE" == "true" ]]; then
        cmake_args+=(-DCMAKE_VERBOSE_MAKEFILE=ON)
    fi
    
    # 配置CMake
    cmake "${cmake_args[@]}" "$RUNTIME_DIR/src/antlr4-runtime"
    
    # 构建
    make -j"$PARALLEL_JOBS"
    
    # 安装
    make install
    
    # 运行测试（仅在原生架构上）
    if [[ "$NO_TEST" != "true" && "$target_arch" == "$(uname -m | sed 's/x86_64/x64/')" ]]; then
        print_info "运行 $target_arch 测试..."
        if ! ctest --output-on-failure -j"$PARALLEL_JOBS"; then
            print_warning "$target_arch 部分测试失败，但继续构建过程"
        else
            print_success "$target_arch 所有测试通过"
        fi
    fi
    
    # 构建静态库版本（如果需要）
    if [[ "$STATIC_ONLY" != "true" && "$SHARED_ONLY" != "true" ]]; then
        print_info "构建 $target_arch 静态库版本..."
        
        local static_build_dir="$arch_build_dir-static"
        mkdir -p "$static_build_dir"
        cd "$static_build_dir"
        
        cmake_args[-1]=-DBUILD_SHARED_LIBS=OFF  # 修改库类型参数
        cmake "${cmake_args[@]}" "$RUNTIME_DIR/src/antlr4-runtime"
        make -j"$PARALLEL_JOBS"
        make install
    fi
    
    print_success "$target_arch 架构构建完成"
}

# 创建Universal Binary
create_universal_binary() {
    print_info "创建Universal Binary..."
    
    local x64_install="$INSTALL_DIR-x64"
    local arm64_install="$INSTALL_DIR-arm64"
    
    # 检查两个架构的构建是否存在
    if [[ ! -d "$x64_install" || ! -d "$arm64_install" ]]; then
        print_error "需要先构建x64和arm64版本"
        return 1
    fi
    
    # 创建Universal目录结构
    mkdir -p "$INSTALL_DIR"
    
    # 复制头文件（两个架构相同）
    cp -r "$x64_install/include" "$INSTALL_DIR/"
    
    # 复制pkg-config文件
    if [[ -d "$x64_install/lib/pkgconfig" ]]; then
        mkdir -p "$INSTALL_DIR/lib/pkgconfig"
        cp -r "$x64_install/lib/pkgconfig/"* "$INSTALL_DIR/lib/pkgconfig/"
    fi
    
    # 复制cmake文件
    if [[ -d "$x64_install/lib/cmake" ]]; then
        mkdir -p "$INSTALL_DIR/lib/cmake"
        cp -r "$x64_install/lib/cmake/"* "$INSTALL_DIR/lib/cmake/"
    fi
    
    # 合并库文件
    mkdir -p "$INSTALL_DIR/lib"
    
    # 处理静态库
    for x64_lib in "$x64_install/lib"/*.a; do
        if [[ -f "$x64_lib" ]]; then
            local lib_name=$(basename "$x64_lib")
            local arm64_lib="$arm64_install/lib/$lib_name"
            local universal_lib="$INSTALL_DIR/lib/$lib_name"
            
            if [[ -f "$arm64_lib" ]]; then
                print_info "合并静态库: $lib_name"
                lipo -create "$x64_lib" "$arm64_lib" -output "$universal_lib"
            else
                print_warning "ARM64版本不存在: $lib_name"
                cp "$x64_lib" "$universal_lib"
            fi
        fi
    done
    
    # 处理动态库
    for x64_lib in "$x64_install/lib"/*.dylib; do
        if [[ -f "$x64_lib" ]]; then
            local lib_name=$(basename "$x64_lib")
            local arm64_lib="$arm64_install/lib/$lib_name"
            local universal_lib="$INSTALL_DIR/lib/$lib_name"
            
            if [[ -f "$arm64_lib" ]]; then
                print_info "合并动态库: $lib_name"
                lipo -create "$x64_lib" "$arm64_lib" -output "$universal_lib"
                
                # 保持符号链接
                if [[ -L "$x64_lib" ]]; then
                    local link_target=$(readlink "$x64_lib")
                    ln -sf "$link_target" "$universal_lib"
                fi
            else
                print_warning "ARM64版本不存在: $lib_name"
                cp "$x64_lib" "$universal_lib"
            fi
        fi
    done
    
    print_success "Universal Binary创建完成"
}

# 构建ANTLR运行时
build_runtime() {
    print_info "开始构建ANTLR运行时..."
    
    # 清理构建目录
    if [[ "$CLEAN_BUILD" == "true" ]] && [[ -d "$BUILD_DIR" ]]; then
        print_info "清理构建目录..."
        rm -rf "$BUILD_DIR"*
    fi
    
    if [[ "$UNIVERSAL_BINARY" == "true" ]]; then
        # 构建Universal Binary
        build_single_arch "x64"
        build_single_arch "arm64"
        create_universal_binary
    else
        # 构建单架构版本
        build_single_arch "$ARCH"
        
        # 移动到最终位置
        local arch_install_dir="$INSTALL_DIR-$ARCH"
        if [[ -d "$arch_install_dir" ]]; then
            mv "$arch_install_dir" "$INSTALL_DIR"
        fi
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
        expected_libs+=(libantlr4-runtime.dylib)
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
        
        # 显示架构信息
        if command -v lipo >/dev/null 2>&1; then
            echo "架构信息:"
            lipo -info "$lib_dir/$lib"
        fi
        
        if [[ "$lib" == *.dylib ]] && command -v otool >/dev/null 2>&1; then
            echo "依赖关系:"
            otool -L "$lib_dir/$lib" || true
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
- OS: $(sw_vers -productName) $(sw_vers -productVersion)
- Architecture: $(uname -m)
- Min macOS Version: $MIN_MACOS_VERSION
- Xcode: $(xcodebuild -version 2>/dev/null | head -n1 || echo "Not available")
- Compiler: $(clang --version | head -n1 2>/dev/null || echo "Unknown")
- CMake: $(cmake --version | head -n1 2>/dev/null || echo "Unknown")

Build Configuration:
- Static Library: $([ "$SHARED_ONLY" != "true" ] && echo "Yes" || echo "No")
- Shared Library: $([ "$STATIC_ONLY" != "true" ] && echo "Yes" || echo "No")
- Universal Binary: $([ "$UNIVERSAL_BINARY" == "true" ] && echo "Yes" || echo "No")
- Tests Built: $([ "$NO_TEST" != "true" ] && echo "Yes" || echo "No")
- Parallel Jobs: $PARALLEL_JOBS

ANTLR Version: 4.13.2
C++ Standard: 17
EOF

    print_info "平台信息已保存到: $info_file"
}

# 主函数
main() {
    print_info "开始macOS ANTLR运行时构建..."
    
    # 检测系统
    detect_system
    
    print_info "macOS ANTLR运行时构建"
    print_info "平台: $PLATFORM_ID"
    print_info "构建类型: $BUILD_TYPE"
    print_info "并行作业: $PARALLEL_JOBS"
    
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
        print_error "Make未安装，请安装Xcode Command Line Tools"
        exit 1
    fi
    
    if ! xcode-select -p >/dev/null 2>&1; then
        print_error "Xcode Command Line Tools未安装"
        print_info "请运行: xcode-select --install"
        exit 1
    fi
    
    # 准备源码
    prepare_source
    
    # 应用补丁
    apply_patches
    
    # 构建
    build_runtime
    
    # 验证构建
    verify_build
    
    # 创建平台信息
    create_platform_info
    
    print_success "macOS ANTLR运行时构建完成！"
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