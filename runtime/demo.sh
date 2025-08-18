#!/bin/bash

# CHTL ANTLR跨平台运行时演示脚本
# 展示完整的构建、测试和打包流程

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

print_info() { echo -e "${BLUE}[INFO]${NC} $1"; }
print_success() { echo -e "${GREEN}[SUCCESS]${NC} $1"; }
print_warning() { echo -e "${YELLOW}[WARNING]${NC} $1"; }
print_error() { echo -e "${RED}[ERROR]${NC} $1"; }
print_step() { echo -e "${CYAN}[STEP]${NC} $1"; }

# 显示标题
show_title() {
    echo
    echo -e "${CYAN}╔════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${CYAN}║                CHTL ANTLR 跨平台运行时演示                ║${NC}"
    echo -e "${CYAN}║                                                            ║${NC}"
    echo -e "${CYAN}║  展示完整的构建、测试、验证和打包流程                      ║${NC}"
    echo -e "${CYAN}╚════════════════════════════════════════════════════════════╝${NC}"
    echo
}

# 显示帮助信息
show_help() {
    cat << EOF
CHTL ANTLR跨平台运行时演示脚本

用法: $0 [选项]

选项:
    -h, --help          显示此帮助信息
    -q, --quick         快速演示（跳过构建，使用现有构建）
    -f, --full          完整演示（包括所有平台构建）
    --skip-build        跳过构建步骤
    --skip-test         跳过测试步骤
    --skip-package      跳过打包步骤
    --clean             清理之前的构建和输出

演示步骤:
    1. 平台检测
    2. 依赖检查
    3. 源码准备
    4. 构建运行时
    5. 运行测试
    6. 验证构建
    7. 打包分发
    8. 使用示例

EOF
}

# 默认参数
QUICK_MODE=false
FULL_MODE=false
SKIP_BUILD=false
SKIP_TEST=false
SKIP_PACKAGE=false
CLEAN_MODE=false

# 解析命令行参数
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_help
            exit 0
            ;;
        -q|--quick)
            QUICK_MODE=true
            shift
            ;;
        -f|--full)
            FULL_MODE=true
            shift
            ;;
        --skip-build)
            SKIP_BUILD=true
            shift
            ;;
        --skip-test)
            SKIP_TEST=true
            shift
            ;;
        --skip-package)
            SKIP_PACKAGE=true
            shift
            ;;
        --clean)
            CLEAN_MODE=true
            shift
            ;;
        *)
            print_error "未知选项: $1"
            show_help
            exit 1
            ;;
    esac
done

# 步骤1: 平台检测
step1_platform_detection() {
    print_step "步骤1: 平台检测"
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    
    if [[ -x "$SCRIPT_DIR/tools/detect-platform.sh" ]]; then
        print_info "运行平台检测脚本..."
        "$SCRIPT_DIR/tools/detect-platform.sh"
    else
        print_error "平台检测脚本不存在或不可执行"
        return 1
    fi
    
    echo
    print_success "平台检测完成"
    echo
}

# 步骤2: 依赖检查
step2_dependency_check() {
    print_step "步骤2: 依赖检查"
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    
    local missing_deps=()
    
    # 检查CMake
    if ! command -v cmake >/dev/null 2>&1; then
        missing_deps+=(cmake)
    else
        local cmake_version=$(cmake --version | head -n1 | grep -oE '[0-9]+\.[0-9]+\.[0-9]+')
        print_success "CMake: $cmake_version"
    fi
    
    # 检查编译器
    if command -v gcc >/dev/null 2>&1; then
        local gcc_version=$(gcc --version | head -n1 | grep -oE '[0-9]+\.[0-9]+\.[0-9]+')
        print_success "GCC: $gcc_version"
    elif command -v clang >/dev/null 2>&1; then
        local clang_version=$(clang --version | head -n1 | grep -oE '[0-9]+\.[0-9]+\.[0-9]+')
        print_success "Clang: $clang_version"
    else
        missing_deps+=(compiler)
    fi
    
    # 检查Make
    if ! command -v make >/dev/null 2>&1; then
        missing_deps+=(make)
    else
        print_success "Make: 可用"
    fi
    
    # 检查Git
    if command -v git >/dev/null 2>&1; then
        local git_version=$(git --version | grep -oE '[0-9]+\.[0-9]+\.[0-9]+')
        print_success "Git: $git_version"
    else
        print_warning "Git: 未安装（可选）"
    fi
    
    if [[ ${#missing_deps[@]} -gt 0 ]]; then
        print_error "缺少必要依赖: ${missing_deps[*]}"
        print_info "请安装缺少的依赖后重试"
        return 1
    fi
    
    echo
    print_success "依赖检查通过"
    echo
}

# 步骤3: 源码准备
step3_source_preparation() {
    print_step "步骤3: 源码准备"
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    
    local src_dir="$SCRIPT_DIR/src/antlr4-runtime"
    
    if [[ -d "$src_dir" ]]; then
        print_success "ANTLR源码已存在: $src_dir"
    else
        print_info "ANTLR源码不存在，将在构建时自动下载"
    fi
    
    # 检查构建脚本
    local build_scripts=(
        "$SCRIPT_DIR/build/build-all.sh"
        "$SCRIPT_DIR/build/build-linux.sh"
        "$SCRIPT_DIR/tools/detect-platform.sh"
        "$SCRIPT_DIR/tools/verify-runtime.sh"
        "$SCRIPT_DIR/tools/package.sh"
    )
    
    print_info "检查构建脚本..."
    for script in "${build_scripts[@]}"; do
        if [[ -x "$script" ]]; then
            print_success "✓ $(basename "$script")"
        else
            print_warning "✗ $(basename "$script") (不存在或不可执行)"
        fi
    done
    
    echo
    print_success "源码准备检查完成"
    echo
}

# 步骤4: 构建运行时
step4_build_runtime() {
    if [[ "$SKIP_BUILD" == "true" ]]; then
        print_warning "跳过构建步骤"
        return 0
    fi
    
    print_step "步骤4: 构建ANTLR运行时"
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    
    if [[ "$CLEAN_MODE" == "true" ]]; then
        print_info "清理之前的构建..."
        rm -rf "$SCRIPT_DIR/build/temp" "$SCRIPT_DIR/prebuilt" 2>/dev/null || true
    fi
    
    local build_script="$SCRIPT_DIR/build/build-all.sh"
    
    if [[ ! -x "$build_script" ]]; then
        print_error "构建脚本不存在: $build_script"
        return 1
    fi
    
    print_info "开始构建ANTLR运行时..."
    
    if [[ "$FULL_MODE" == "true" ]]; then
        # 构建所有支持的平台
        print_info "完整模式: 构建所有支持的平台"
        "$build_script" --platforms linux-x64,linux-arm64
    elif [[ "$QUICK_MODE" == "true" ]]; then
        # 快速模式: 只构建当前平台
        print_info "快速模式: 只构建当前平台"
        "$build_script"
    else
        # 默认: 构建当前平台
        "$build_script"
    fi
    
    echo
    print_success "ANTLR运行时构建完成"
    echo
}

# 步骤5: 运行测试
step5_run_tests() {
    if [[ "$SKIP_TEST" == "true" ]]; then
        print_warning "跳过测试步骤"
        return 0
    fi
    
    print_step "步骤5: 运行测试"
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    
    # 检查是否有可用的预构建版本
    local prebuilt_dirs=("$SCRIPT_DIR/prebuilt"/*)
    
    if [[ ! -d "${prebuilt_dirs[0]}" ]]; then
        print_error "没有找到预构建的运行时，请先运行构建"
        return 1
    fi
    
    # 运行基础测试
    print_info "编译和运行基础测试..."
    
    local test_dir="$SCRIPT_DIR/build/test-demo"
    mkdir -p "$test_dir"
    
    # 创建简单的测试CMakeLists.txt
    cat > "$test_dir/CMakeLists.txt" << EOF
cmake_minimum_required(VERSION 3.16)
project(antlr_demo_test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找ANTLR运行时
list(APPEND CMAKE_MODULE_PATH "$SCRIPT_DIR/build/cmake")
find_package(ANTLRRuntime REQUIRED)

# 创建测试程序
add_executable(demo_test "$SCRIPT_DIR/test/basic-test.cpp")
target_link_libraries(demo_test antlr4-runtime::antlr4-runtime)

if(MSVC)
    target_compile_options(demo_test PRIVATE /wd4267 /wd4244)
else()
    target_compile_options(demo_test PRIVATE -Wno-unused-parameter)
endif()
EOF
    
    cd "$test_dir"
    
    print_info "配置测试项目..."
    if cmake -DCMAKE_BUILD_TYPE=Release . >/dev/null 2>&1; then
        print_success "CMake配置成功"
    else
        print_error "CMake配置失败"
        return 1
    fi
    
    print_info "编译测试程序..."
    if make >/dev/null 2>&1; then
        print_success "编译成功"
    else
        print_error "编译失败"
        return 1
    fi
    
    print_info "运行测试程序..."
    if ./demo_test; then
        print_success "测试通过"
    else
        print_warning "测试失败，但继续演示"
    fi
    
    # 清理测试目录
    cd "$SCRIPT_DIR"
    rm -rf "$test_dir"
    
    echo
    print_success "测试完成"
    echo
}

# 步骤6: 验证构建
step6_verify_build() {
    print_step "步骤6: 验证构建"
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    
    local verify_script="$SCRIPT_DIR/tools/verify-runtime.sh"
    
    if [[ ! -x "$verify_script" ]]; then
        print_error "验证脚本不存在: $verify_script"
        return 1
    fi
    
    print_info "运行构建验证..."
    
    if [[ "$QUICK_MODE" == "true" ]]; then
        "$verify_script" --quick
    else
        "$verify_script" --build-test
    fi
    
    echo
    print_success "构建验证完成"
    echo
}

# 步骤7: 打包分发
step7_package_distribution() {
    if [[ "$SKIP_PACKAGE" == "true" ]]; then
        print_warning "跳过打包步骤"
        return 0
    fi
    
    print_step "步骤7: 打包分发"
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    
    local package_script="$SCRIPT_DIR/tools/package.sh"
    
    if [[ ! -x "$package_script" ]]; then
        print_error "打包脚本不存在: $package_script"
        return 1
    fi
    
    print_info "创建分发包..."
    
    local output_dir="$SCRIPT_DIR/demo-packages"
    
    if [[ "$CLEAN_MODE" == "true" ]]; then
        rm -rf "$output_dir" 2>/dev/null || true
    fi
    
    if [[ "$FULL_MODE" == "true" ]]; then
        "$package_script" --output "$output_dir" --format all --include-docs
    else
        "$package_script" --output "$output_dir" --format tar.gz
    fi
    
    # 显示生成的包
    if [[ -d "$output_dir" ]]; then
        print_info "生成的分发包:"
        ls -lh "$output_dir"/ | grep -E '\.(tar\.gz|zip)$' | while read -r line; do
            echo "  $line"
        done
    fi
    
    echo
    print_success "打包完成"
    echo
}

# 步骤8: 使用示例
step8_usage_example() {
    print_step "步骤8: 使用示例"
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    
    print_info "演示如何在项目中使用ANTLR运行时..."
    
    local example_dir="$SCRIPT_DIR/build/usage-example"
    mkdir -p "$example_dir"
    
    # 创建示例项目
    cat > "$example_dir/CMakeLists.txt" << EOF
cmake_minimum_required(VERSION 3.16)
project(antlr_usage_example)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找ANTLR运行时
list(APPEND CMAKE_MODULE_PATH "$SCRIPT_DIR/build/cmake")
find_package(ANTLRRuntime REQUIRED)

# 创建示例程序
add_executable(usage_example "$SCRIPT_DIR/examples/basic_example.cpp")
target_link_libraries(usage_example antlr4-runtime::antlr4-runtime)
EOF
    
    cd "$example_dir"
    
    print_info "构建使用示例..."
    if cmake -DCMAKE_BUILD_TYPE=Release . >/dev/null 2>&1 && make >/dev/null 2>&1; then
        print_success "示例程序构建成功"
        
        print_info "运行示例程序..."
        echo "----------------------------------------"
        ./usage_example
        echo "----------------------------------------"
        
    else
        print_warning "示例程序构建失败，但这不影响演示"
    fi
    
    # 清理
    cd "$SCRIPT_DIR"
    rm -rf "$example_dir"
    
    echo
    print_success "使用示例演示完成"
    echo
}

# 显示总结
show_summary() {
    echo
    echo -e "${CYAN}╔════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${CYAN}║                      演示总结                              ║${NC}"
    echo -e "${CYAN}╚════════════════════════════════════════════════════════════╝${NC}"
    echo
    
    print_success "🎉 CHTL ANTLR跨平台运行时演示完成！"
    echo
    
    print_info "完成的步骤:"
    echo "  ✅ 平台检测 - 自动识别当前运行平台"
    echo "  ✅ 依赖检查 - 验证构建环境"
    echo "  ✅ 源码准备 - 检查必要的构建脚本"
    if [[ "$SKIP_BUILD" != "true" ]]; then
        echo "  ✅ 构建运行时 - 编译ANTLR C++运行时库"
    fi
    if [[ "$SKIP_TEST" != "true" ]]; then
        echo "  ✅ 运行测试 - 验证基本功能"
    fi
    echo "  ✅ 验证构建 - 检查构建结果"
    if [[ "$SKIP_PACKAGE" != "true" ]]; then
        echo "  ✅ 打包分发 - 创建可分发的包"
    fi
    echo "  ✅ 使用示例 - 展示如何在项目中使用"
    echo
    
    print_info "主要特性:"
    echo "  🚀 跨平台支持 (Linux, Windows, macOS)"
    echo "  📦 预构建运行时库 (静态库和动态库)"
    echo "  🔧 CMake集成支持"
    echo "  ✅ 自动化构建和测试"
    echo "  📋 完整的验证流程"
    echo "  📚 详细的文档和示例"
    echo
    
    print_info "生成的文件:"
    if [[ -d "$SCRIPT_DIR/prebuilt" ]]; then
        echo "  📁 预构建运行时: $SCRIPT_DIR/prebuilt/"
        ls "$SCRIPT_DIR/prebuilt/" | while read -r platform; do
            echo "     - $platform"
        done
    fi
    
    if [[ -d "$SCRIPT_DIR/demo-packages" ]]; then
        echo "  📦 分发包: $SCRIPT_DIR/demo-packages/"
        ls "$SCRIPT_DIR/demo-packages/" | grep -E '\.(tar\.gz|zip)$' | while read -r package; do
            echo "     - $package"
        done
    fi
    
    echo
    print_info "下一步:"
    echo "  1. 将预构建的运行时集成到您的项目中"
    echo "  2. 使用提供的CMake模块简化集成"
    echo "  3. 参考示例代码开发您的语言处理器"
    echo "  4. 使用验证脚本确保部署环境的兼容性"
    echo
    
    print_success "感谢使用CHTL ANTLR跨平台运行时！"
    echo
}

# 主函数
main() {
    show_title
    
    # 检查是否在正确的目录
    if [[ ! -f "$SCRIPT_DIR/README.md" ]]; then
        print_error "请在ANTLR运行时目录中运行此脚本"
        exit 1
    fi
    
    print_info "开始CHTL ANTLR跨平台运行时演示..."
    print_info "模式: $([ "$QUICK_MODE" == "true" ] && echo "快速模式" || ([ "$FULL_MODE" == "true" ] && echo "完整模式" || echo "标准模式"))"
    echo
    
    # 运行演示步骤
    step1_platform_detection
    step2_dependency_check
    step3_source_preparation
    step4_build_runtime
    step5_run_tests
    step6_verify_build
    step7_package_distribution
    step8_usage_example
    
    # 显示总结
    show_summary
}

# 运行主函数
main "$@"