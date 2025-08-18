#!/bin/bash

# CHTL ANTLR运行时打包脚本
# 将构建好的运行时打包成分发包

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
CHTL ANTLR运行时打包脚本

用法: $0 [选项]

选项:
    -h, --help          显示此帮助信息
    -p, --platforms     指定要打包的平台 (逗号分隔，默认: 所有可用平台)
    -o, --output        输出目录 (默认: ./packages)
    -f, --format        打包格式 [tar.gz|zip|all] (默认: tar.gz)
    -v, --version       版本号 (默认: 自动检测)
    --include-source    包含源代码
    --include-docs      包含文档
    --include-examples  包含示例代码
    --no-compress       不压缩，只创建目录
    -c, --clean         清理输出目录

支持的平台:
    linux-x64, linux-arm64, windows-x64, windows-x86, macos-x64, macos-arm64

打包格式:
    tar.gz              Linux/macOS标准格式
    zip                 Windows标准格式  
    all                 所有格式

示例:
    $0                                      # 打包所有平台
    $0 -p linux-x64,windows-x64             # 打包指定平台
    $0 -f zip -o ./dist                     # 使用ZIP格式输出到dist目录
    $0 --include-source --include-docs      # 包含源码和文档

EOF
}

# 默认参数
PLATFORMS=""
OUTPUT_DIR="./packages"
FORMAT="tar.gz"
VERSION=""
INCLUDE_SOURCE=false
INCLUDE_DOCS=false
INCLUDE_EXAMPLES=false
NO_COMPRESS=false
CLEAN_OUTPUT=false

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
        -o|--output)
            OUTPUT_DIR="$2"
            shift 2
            ;;
        -f|--format)
            FORMAT="$2"
            shift 2
            ;;
        -v|--version)
            VERSION="$2"
            shift 2
            ;;
        --include-source)
            INCLUDE_SOURCE=true
            shift
            ;;
        --include-docs)
            INCLUDE_DOCS=true
            shift
            ;;
        --include-examples)
            INCLUDE_EXAMPLES=true
            shift
            ;;
        --no-compress)
            NO_COMPRESS=true
            shift
            ;;
        -c|--clean)
            CLEAN_OUTPUT=true
            shift
            ;;
        *)
            print_error "未知选项: $1"
            show_help
            exit 1
            ;;
    esac
done

# 验证格式
if [[ "$FORMAT" != "tar.gz" && "$FORMAT" != "zip" && "$FORMAT" != "all" ]]; then
    print_error "不支持的格式: $FORMAT"
    exit 1
fi

# 检测版本
detect_version() {
    if [[ -n "$VERSION" ]]; then
        echo "$VERSION"
        return
    fi
    
    # 从platform-info.txt检测版本
    for platform_dir in "$RUNTIME_DIR/prebuilt"/*; do
        if [[ -d "$platform_dir" && -f "$platform_dir/platform-info.txt" ]]; then
            local version=$(grep "ANTLR Version:" "$platform_dir/platform-info.txt" | cut -d: -f2 | xargs)
            if [[ -n "$version" ]]; then
                echo "$version"
                return
            fi
        fi
    done
    
    # 默认版本
    echo "4.13.2"
}

# 获取可用平台
get_available_platforms() {
    local platforms=()
    
    if [[ -d "$RUNTIME_DIR/prebuilt" ]]; then
        for platform_dir in "$RUNTIME_DIR/prebuilt"/*; do
            if [[ -d "$platform_dir" ]]; then
                platforms+=($(basename "$platform_dir"))
            fi
        done
    fi
    
    echo "${platforms[@]}"
}

# 创建包目录结构
create_package_structure() {
    local platform=$1
    local package_dir=$2
    local prebuilt_dir="$RUNTIME_DIR/prebuilt/$platform"
    
    print_info "创建包目录结构: $package_dir"
    
    # 创建基础目录结构
    mkdir -p "$package_dir"/{bin,lib,include,share,cmake,docs,examples}
    
    # 复制库文件
    if [[ -d "$prebuilt_dir/lib" ]]; then
        cp -r "$prebuilt_dir/lib/"* "$package_dir/lib/"
    fi
    
    # 复制头文件
    if [[ -d "$prebuilt_dir/include" ]]; then
        cp -r "$prebuilt_dir/include/"* "$package_dir/include/"
    fi
    
    # 复制CMake文件
    if [[ -d "$RUNTIME_DIR/build/cmake" ]]; then
        cp "$RUNTIME_DIR/build/cmake/"*.cmake "$package_dir/cmake/"
    fi
    
    # 复制平台信息
    if [[ -f "$prebuilt_dir/platform-info.txt" ]]; then
        cp "$prebuilt_dir/platform-info.txt" "$package_dir/"
    fi
    
    # 创建README
    create_package_readme "$platform" "$package_dir"
    
    # 创建安装脚本
    create_install_script "$platform" "$package_dir"
    
    # 包含源代码
    if [[ "$INCLUDE_SOURCE" == "true" && -d "$RUNTIME_DIR/src" ]]; then
        print_info "包含源代码..."
        cp -r "$RUNTIME_DIR/src" "$package_dir/"
    fi
    
    # 包含文档
    if [[ "$INCLUDE_DOCS" == "true" ]]; then
        print_info "包含文档..."
        cp "$RUNTIME_DIR/README.md" "$package_dir/docs/" 2>/dev/null || true
        
        # 复制其他文档
        for doc_file in "$RUNTIME_DIR"/*.md; do
            if [[ -f "$doc_file" ]]; then
                cp "$doc_file" "$package_dir/docs/"
            fi
        done
    fi
    
    # 包含示例
    if [[ "$INCLUDE_EXAMPLES" == "true" && -d "$RUNTIME_DIR/test" ]]; then
        print_info "包含示例代码..."
        cp -r "$RUNTIME_DIR/test/"* "$package_dir/examples/"
    fi
}

# 创建包README
create_package_readme() {
    local platform=$1
    local package_dir=$2
    local readme_file="$package_dir/README.md"
    
    cat > "$readme_file" << EOF
# CHTL ANTLR Runtime - $platform

这是CHTL项目的ANTLR 4运行时预构建包，适用于 $platform 平台。

## 版本信息

- ANTLR版本: $DETECTED_VERSION
- 平台: $platform
- 打包日期: $(date -u +"%Y-%m-%d %H:%M:%S UTC")

## 目录结构

- \`lib/\` - 库文件（静态库和动态库）
- \`include/\` - 头文件
- \`cmake/\` - CMake配置文件
- \`docs/\` - 文档
- \`examples/\` - 示例代码

## 安装

### 自动安装
运行安装脚本：
\`\`\`bash
./install.sh
\`\`\`

### 手动安装
1. 将 \`lib/\` 目录中的文件复制到系统库目录
2. 将 \`include/\` 目录中的文件复制到系统头文件目录
3. 将 \`cmake/\` 目录中的文件复制到CMake模块目录

## CMake集成

在你的CMakeLists.txt中：

\`\`\`cmake
# 设置ANTLR运行时路径
set(ANTLR4_ROOT /path/to/antlr/runtime)

# 查找ANTLR运行时
find_package(ANTLRRuntime REQUIRED)

# 链接到你的目标
target_link_libraries(your_target antlr4-runtime::antlr4-runtime)
\`\`\`

## 许可证

本运行时遵循ANTLR 4的BSD许可证。

## 支持

如有问题，请访问: https://github.com/your-org/chtl-compiler

EOF
}

# 创建安装脚本
create_install_script() {
    local platform=$1
    local package_dir=$2
    
    # Linux/macOS安装脚本
    if [[ "$platform" =~ ^(linux|macos) ]]; then
        local install_script="$package_dir/install.sh"
        
        cat > "$install_script" << 'EOF'
#!/bin/bash

# CHTL ANTLR Runtime 安装脚本

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m'

print_info() { echo -e "${BLUE}[INFO]${NC} $1"; }
print_success() { echo -e "${GREEN}[SUCCESS]${NC} $1"; }
print_error() { echo -e "${RED}[ERROR]${NC} $1"; }

# 检测系统
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    SYSTEM="linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    SYSTEM="macos"
else
    print_error "不支持的系统: $OSTYPE"
    exit 1
fi

# 默认安装路径
if [[ "$SYSTEM" == "linux" ]]; then
    DEFAULT_PREFIX="/usr/local"
else
    DEFAULT_PREFIX="/usr/local"
fi

PREFIX="${1:-$DEFAULT_PREFIX}"

print_info "安装CHTL ANTLR Runtime到: $PREFIX"

# 检查权限
if [[ ! -w "$PREFIX" ]]; then
    print_error "没有写入权限: $PREFIX"
    print_info "请使用sudo运行或指定其他路径"
    exit 1
fi

# 创建目录
mkdir -p "$PREFIX"/{lib,include,share/cmake}

# 安装库文件
if [[ -d "lib" ]]; then
    print_info "安装库文件..."
    cp -r lib/* "$PREFIX/lib/"
fi

# 安装头文件
if [[ -d "include" ]]; then
    print_info "安装头文件..."
    cp -r include/* "$PREFIX/include/"
fi

# 安装CMake文件
if [[ -d "cmake" ]]; then
    print_info "安装CMake文件..."
    cp cmake/* "$PREFIX/share/cmake/"
fi

print_success "安装完成！"
print_info "使用方法："
print_info "  在CMakeLists.txt中设置: set(ANTLR4_ROOT $PREFIX)"
print_info "  然后: find_package(ANTLRRuntime REQUIRED)"

EOF
        
        chmod +x "$install_script"
        
    # Windows安装脚本
    elif [[ "$platform" =~ ^windows ]]; then
        local install_script="$package_dir/install.bat"
        
        cat > "$install_script" << 'EOF'
@echo off
setlocal enabledelayedexpansion

REM CHTL ANTLR Runtime 安装脚本

set "PREFIX=%1"
if "%PREFIX%"=="" set "PREFIX=C:\Program Files\CHTL\ANTLR"

echo 安装CHTL ANTLR Runtime到: %PREFIX%

REM 创建目录
if not exist "%PREFIX%" mkdir "%PREFIX%"
if not exist "%PREFIX%\lib" mkdir "%PREFIX%\lib"
if not exist "%PREFIX%\include" mkdir "%PREFIX%\include"
if not exist "%PREFIX%\cmake" mkdir "%PREFIX%\cmake"

REM 安装文件
if exist "lib" xcopy /E /I /Y lib "%PREFIX%\lib"
if exist "include" xcopy /E /I /Y include "%PREFIX%\include"
if exist "cmake" xcopy /E /I /Y cmake "%PREFIX%\cmake"

echo 安装完成！
echo 使用方法:
echo   在CMakeLists.txt中设置: set(ANTLR4_ROOT "%PREFIX%")
echo   然后: find_package(ANTLRRuntime REQUIRED)

pause
EOF
    fi
}

# 压缩包
create_archive() {
    local platform=$1
    local package_dir=$2
    local output_dir=$3
    local format=$4
    
    local package_name="chtl-antlr-runtime-${DETECTED_VERSION}-${platform}"
    
    cd "$(dirname "$package_dir")"
    
    case "$format" in
        tar.gz)
            local archive_file="$output_dir/${package_name}.tar.gz"
            print_info "创建tar.gz包: $archive_file"
            tar -czf "$archive_file" "$(basename "$package_dir")"
            ;;
        zip)
            local archive_file="$output_dir/${package_name}.zip"
            print_info "创建ZIP包: $archive_file"
            zip -r "$archive_file" "$(basename "$package_dir")" >/dev/null
            ;;
    esac
    
    if [[ -f "$archive_file" ]]; then
        print_success "创建包: $archive_file"
        
        # 显示包信息
        if command -v ls >/dev/null 2>&1; then
            local size=$(ls -lh "$archive_file" | awk '{print $5}')
            print_info "包大小: $size"
        fi
    fi
}

# 打包单个平台
package_platform() {
    local platform=$1
    local temp_dir="$OUTPUT_DIR/temp"
    local package_dir="$temp_dir/chtl-antlr-runtime-${DETECTED_VERSION}-${platform}"
    
    print_info "打包平台: $platform"
    
    # 检查预构建目录
    local prebuilt_dir="$RUNTIME_DIR/prebuilt/$platform"
    if [[ ! -d "$prebuilt_dir" ]]; then
        print_error "预构建目录不存在: $prebuilt_dir"
        return 1
    fi
    
    # 创建临时目录
    mkdir -p "$temp_dir"
    
    # 创建包结构
    create_package_structure "$platform" "$package_dir"
    
    if [[ "$NO_COMPRESS" == "true" ]]; then
        # 不压缩，直接移动到输出目录
        local final_dir="$OUTPUT_DIR/chtl-antlr-runtime-${DETECTED_VERSION}-${platform}"
        mv "$package_dir" "$final_dir"
        print_success "创建目录: $final_dir"
    else
        # 创建压缩包
        if [[ "$FORMAT" == "all" ]]; then
            create_archive "$platform" "$package_dir" "$OUTPUT_DIR" "tar.gz"
            create_archive "$platform" "$package_dir" "$OUTPUT_DIR" "zip"
        else
            create_archive "$platform" "$package_dir" "$OUTPUT_DIR" "$FORMAT"
        fi
    fi
    
    # 清理临时目录
    rm -rf "$temp_dir"
}

# 创建源码包
create_source_package() {
    if [[ "$INCLUDE_SOURCE" != "true" ]]; then
        return
    fi
    
    print_info "创建源码包..."
    
    local temp_dir="$OUTPUT_DIR/temp"
    local source_dir="$temp_dir/chtl-antlr-runtime-${DETECTED_VERSION}-source"
    
    mkdir -p "$source_dir"
    
    # 复制源码
    cp -r "$RUNTIME_DIR"/* "$source_dir/"
    
    # 清理不需要的文件
    rm -rf "$source_dir/prebuilt"
    rm -rf "$source_dir/build/temp"
    rm -rf "$source_dir/packages"
    
    # 创建压缩包
    if [[ "$FORMAT" == "all" ]]; then
        create_archive "source" "$source_dir" "$OUTPUT_DIR" "tar.gz"
        create_archive "source" "$source_dir" "$OUTPUT_DIR" "zip"
    else
        create_archive "source" "$source_dir" "$OUTPUT_DIR" "$FORMAT"
    fi
    
    # 清理临时目录
    rm -rf "$temp_dir"
}

# 主函数
main() {
    print_info "CHTL ANTLR运行时打包开始..."
    
    # 检测版本
    DETECTED_VERSION=$(detect_version)
    print_info "检测到版本: $DETECTED_VERSION"
    
    # 清理输出目录
    if [[ "$CLEAN_OUTPUT" == "true" && -d "$OUTPUT_DIR" ]]; then
        print_info "清理输出目录: $OUTPUT_DIR"
        rm -rf "$OUTPUT_DIR"
    fi
    
    # 创建输出目录
    mkdir -p "$OUTPUT_DIR"
    
    # 确定要打包的平台
    local platforms_to_package=()
    
    if [[ -n "$PLATFORMS" ]]; then
        IFS=',' read -ra platforms_to_package <<< "$PLATFORMS"
    else
        # 获取所有可用平台
        local available_platforms
        read -ra available_platforms <<< "$(get_available_platforms)"
        platforms_to_package=("${available_platforms[@]}")
        
        if [[ ${#platforms_to_package[@]} -eq 0 ]]; then
            print_error "未找到任何可用平台"
            print_info "请先构建ANTLR运行时"
            exit 1
        fi
    fi
    
    print_info "将打包以下平台: ${platforms_to_package[*]}"
    print_info "输出目录: $OUTPUT_DIR"
    print_info "打包格式: $FORMAT"
    
    # 打包每个平台
    local success_count=0
    local total_count=${#platforms_to_package[@]}
    
    for platform in "${platforms_to_package[@]}"; do
        print_info "[$((success_count + 1))/$total_count] 打包平台: $platform"
        
        if package_platform "$platform"; then
            ((success_count++))
        else
            print_error "平台 $platform 打包失败"
        fi
    done
    
    # 创建源码包
    create_source_package
    
    # 总结
    echo
    print_info "=== 打包总结 ==="
    print_success "成功打包: $success_count/$total_count 个平台"
    
    if [[ $success_count -eq $total_count ]]; then
        print_success "所有平台打包成功！"
        
        # 显示输出文件
        print_info "输出文件:"
        if command -v ls >/dev/null 2>&1; then
            ls -lh "$OUTPUT_DIR"/*.tar.gz "$OUTPUT_DIR"/*.zip 2>/dev/null | while read -r line; do
                echo "  $line"
            done
        fi
        
        exit 0
    else
        print_error "部分平台打包失败"
        exit 1
    fi
}

# 运行主函数
main "$@"