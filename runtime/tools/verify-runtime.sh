#!/bin/bash

# CHTL ANTLR运行时验证脚本
# 验证已构建的ANTLR运行时是否正常工作

set -e

# 脚本目录
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
CHTL ANTLR运行时验证脚本

用法: $0 [选项] [平台]

选项:
    -h, --help          显示此帮助信息
    -v, --verbose       详细输出
    -q, --quick         快速验证（跳过性能测试）
    --build-test        构建并运行测试程序
    --no-cleanup        不清理测试文件
    --all-platforms     验证所有可用平台

平台:
    如果不指定平台，将自动检测当前平台
    可用平台: linux-x64, linux-arm64, windows-x64, windows-x86, macos-x64, macos-arm64

示例:
    $0                          # 验证当前平台
    $0 linux-x64                # 验证指定平台
    $0 --all-platforms          # 验证所有平台
    $0 --build-test --verbose   # 构建测试并详细输出

EOF
}

# 默认参数
VERBOSE=false
QUICK=false
BUILD_TEST=false
NO_CLEANUP=false
ALL_PLATFORMS=false
TARGET_PLATFORM=""

# 解析命令行参数
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_help
            exit 0
            ;;
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        -q|--quick)
            QUICK=true
            shift
            ;;
        --build-test)
            BUILD_TEST=true
            shift
            ;;
        --no-cleanup)
            NO_CLEANUP=true
            shift
            ;;
        --all-platforms)
            ALL_PLATFORMS=true
            shift
            ;;
        -*)
            print_error "未知选项: $1"
            show_help
            exit 1
            ;;
        *)
            if [[ -z "$TARGET_PLATFORM" ]]; then
                TARGET_PLATFORM="$1"
            else
                print_error "只能指定一个平台"
                exit 1
            fi
            shift
            ;;
    esac
done

# 检测当前平台
detect_current_platform() {
    if command -v "$RUNTIME_DIR/tools/detect-platform.sh" >/dev/null 2>&1; then
        local platform_info
        platform_info=$("$RUNTIME_DIR/tools/detect-platform.sh" 2>/dev/null)
        echo "$platform_info" | grep "PLATFORM_ID=" | cut -d= -f2
    else
        print_error "无法检测当前平台"
        exit 1
    fi
}

# 验证单个平台
verify_platform() {
    local platform=$1
    local prebuilt_dir="$RUNTIME_DIR/prebuilt/$platform"
    
    print_info "验证平台: $platform"
    
    # 检查预构建目录
    if [[ ! -d "$prebuilt_dir" ]]; then
        print_error "预构建目录不存在: $prebuilt_dir"
        return 1
    fi
    
    local errors=0
    
    # 检查目录结构
    print_info "检查目录结构..."
    
    local required_dirs=(
        "$prebuilt_dir/include"
        "$prebuilt_dir/lib"
    )
    
    for dir in "${required_dirs[@]}"; do
        if [[ ! -d "$dir" ]]; then
            print_error "必需目录不存在: $dir"
            ((errors++))
        else
            if [[ "$VERBOSE" == "true" ]]; then
                print_success "找到目录: $dir"
            fi
        fi
    done
    
    # 检查头文件
    print_info "检查头文件..."
    
    local antlr_include_dir="$prebuilt_dir/include/antlr4-runtime"
    if [[ ! -d "$antlr_include_dir" ]]; then
        print_error "ANTLR头文件目录不存在: $antlr_include_dir"
        ((errors++))
    else
        local required_headers=(
            "antlr4-runtime.h"
            "ANTLRInputStream.h"
            "CommonTokenStream.h"
            "Lexer.h"
            "Parser.h"
        )
        
        for header in "${required_headers[@]}"; do
            if [[ ! -f "$antlr_include_dir/$header" ]]; then
                print_error "头文件不存在: $header"
                ((errors++))
            else
                if [[ "$VERBOSE" == "true" ]]; then
                    print_success "找到头文件: $header"
                fi
            fi
        done
    fi
    
    # 检查库文件
    print_info "检查库文件..."
    
    local lib_dir="$prebuilt_dir/lib"
    local found_static=false
    local found_shared=false
    
    # 检查静态库
    for static_lib in "$lib_dir"/libantlr4-runtime.a "$lib_dir"/antlr4-runtime.lib; do
        if [[ -f "$static_lib" ]]; then
            found_static=true
            print_success "找到静态库: $(basename "$static_lib")"
            
            # 验证库文件
            if command -v file >/dev/null 2>&1; then
                if [[ "$VERBOSE" == "true" ]]; then
                    print_info "库文件信息:"
                    file "$static_lib"
                fi
            fi
            break
        fi
    done
    
    # 检查动态库
    for shared_lib in "$lib_dir"/libantlr4-runtime.so* "$lib_dir"/antlr4-runtime.dll "$lib_dir"/libantlr4-runtime.dylib; do
        if [[ -f "$shared_lib" ]]; then
            found_shared=true
            print_success "找到动态库: $(basename "$shared_lib")"
            
            # 验证库文件
            if command -v file >/dev/null 2>&1; then
                if [[ "$VERBOSE" == "true" ]]; then
                    print_info "库文件信息:"
                    file "$shared_lib"
                fi
            fi
            break
        fi
    done
    
    if [[ "$found_static" == "false" && "$found_shared" == "false" ]]; then
        print_error "未找到任何ANTLR库文件"
        ((errors++))
    fi
    
    # 检查平台信息文件
    print_info "检查平台信息..."
    
    local info_file="$prebuilt_dir/platform-info.txt"
    if [[ -f "$info_file" ]]; then
        print_success "找到平台信息文件"
        if [[ "$VERBOSE" == "true" ]]; then
            echo "平台信息内容:"
            cat "$info_file"
            echo
        fi
    else
        print_warning "平台信息文件不存在: $info_file"
    fi
    
    # 构建测试程序
    if [[ "$BUILD_TEST" == "true" ]]; then
        if ! build_and_test "$platform" "$prebuilt_dir"; then
            ((errors++))
        fi
    fi
    
    # 总结
    if [[ $errors -eq 0 ]]; then
        print_success "平台 $platform 验证通过"
        return 0
    else
        print_error "平台 $platform 验证失败，发现 $errors 个错误"
        return 1
    fi
}

# 构建并测试
build_and_test() {
    local platform=$1
    local prebuilt_dir=$2
    
    print_info "构建测试程序..."
    
    local test_dir="$RUNTIME_DIR/build/verify-$platform"
    mkdir -p "$test_dir"
    
    # 创建测试程序
    cat > "$test_dir/test_antlr.cpp" << 'EOF'
#include "antlr4-runtime.h"
#include <iostream>
#include <string>

using namespace antlr4;

// 简单的测试词法分析器
class TestLexer : public Lexer {
public:
    TestLexer(CharStream* input) : Lexer(input) {}
    
    std::string getGrammarFileName() const override {
        return "Test.g4";
    }
    
    const std::vector<std::string>& getRuleNames() const override {
        static std::vector<std::string> names = {"ID", "WS"};
        return names;
    }
    
    const std::vector<std::string>& getChannelNames() const override {
        static std::vector<std::string> names = {"DEFAULT_TOKEN_CHANNEL", "HIDDEN"};
        return names;
    }
    
    const std::vector<std::string>& getModeNames() const override {
        static std::vector<std::string> names = {"DEFAULT_MODE"};
        return names;
    }
    
    const std::vector<std::string>& getTokenNames() const override {
        static std::vector<std::string> names = {"<INVALID>", "ID", "WS"};
        return names;
    }
    
    antlr4::dfa::Vocabulary& getVocabulary() const override {
        static antlr4::dfa::VocabularyImpl vocab(getTokenNames(), {}, {});
        return vocab;
    }
    
    const std::string getSerializedATN() const override {
        return "";
    }
    
    const antlr4::atn::ATN& getATN() const override {
        static antlr4::atn::ATN atn;
        return atn;
    }
};

int main() {
    try {
        std::cout << "测试ANTLR运行时..." << std::endl;
        
        // 测试基本功能
        std::string input = "hello world";
        ANTLRInputStream stream(input);
        
        std::cout << "✅ ANTLRInputStream 创建成功" << std::endl;
        
        // 测试Token
        auto token = std::make_unique<CommonToken>(1, "test");
        if (token) {
            std::cout << "✅ CommonToken 创建成功" << std::endl;
        }
        
        // 测试TokenStream
        std::vector<std::unique_ptr<Token>> tokens;
        tokens.push_back(std::make_unique<CommonToken>(1, "hello"));
        tokens.push_back(std::make_unique<CommonToken>(1, "world"));
        
        std::cout << "✅ Token操作成功" << std::endl;
        
        std::cout << "🎉 ANTLR运行时测试通过！" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 测试失败: " << e.what() << std::endl;
        return 1;
    }
}
EOF
    
    # 创建CMakeLists.txt
    cat > "$test_dir/CMakeLists.txt" << EOF
cmake_minimum_required(VERSION 3.16)
project(antlr_runtime_test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置ANTLR运行时路径
set(ANTLR4_ROOT "$prebuilt_dir")

# 查找ANTLR运行时
list(APPEND CMAKE_MODULE_PATH "$RUNTIME_DIR/build/cmake")
find_package(ANTLRRuntime REQUIRED)

# 创建测试可执行文件
add_executable(test_antlr test_antlr.cpp)

# 链接ANTLR运行时
target_link_libraries(test_antlr antlr4-runtime::antlr4-runtime)

# 设置编译选项
if(MSVC)
    target_compile_options(test_antlr PRIVATE /wd4267 /wd4244)
else()
    target_compile_options(test_antlr PRIVATE -Wno-unused-parameter)
endif()
EOF
    
    cd "$test_dir"
    
    # 配置CMake
    if [[ "$VERBOSE" == "true" ]]; then
        print_info "配置CMake..."
        cmake -DCMAKE_BUILD_TYPE=Release .
    else
        cmake -DCMAKE_BUILD_TYPE=Release . >/dev/null 2>&1
    fi
    
    if [[ $? -ne 0 ]]; then
        print_error "CMake配置失败"
        return 1
    fi
    
    # 构建
    if [[ "$VERBOSE" == "true" ]]; then
        print_info "构建测试程序..."
        make
    else
        make >/dev/null 2>&1
    fi
    
    if [[ $? -ne 0 ]]; then
        print_error "测试程序构建失败"
        return 1
    fi
    
    print_success "测试程序构建成功"
    
    # 运行测试
    print_info "运行测试程序..."
    if ./test_antlr; then
        print_success "测试程序运行成功"
    else
        print_error "测试程序运行失败"
        return 1
    fi
    
    # 清理
    if [[ "$NO_CLEANUP" != "true" ]]; then
        cd "$RUNTIME_DIR"
        rm -rf "$test_dir"
    fi
    
    return 0
}

# 性能测试
performance_test() {
    local platform=$1
    local prebuilt_dir=$2
    
    if [[ "$QUICK" == "true" ]]; then
        return 0
    fi
    
    print_info "运行性能测试..."
    
    # TODO: 实现性能测试
    print_warning "性能测试尚未实现"
    
    return 0
}

# 主函数
main() {
    print_info "CHTL ANTLR运行时验证开始..."
    
    local platforms_to_verify=()
    
    if [[ "$ALL_PLATFORMS" == "true" ]]; then
        # 查找所有可用平台
        if [[ -d "$RUNTIME_DIR/prebuilt" ]]; then
            for platform_dir in "$RUNTIME_DIR/prebuilt"/*; do
                if [[ -d "$platform_dir" ]]; then
                    platforms_to_verify+=($(basename "$platform_dir"))
                fi
            done
        fi
        
        if [[ ${#platforms_to_verify[@]} -eq 0 ]]; then
            print_error "未找到任何预构建平台"
            exit 1
        fi
    elif [[ -n "$TARGET_PLATFORM" ]]; then
        platforms_to_verify=("$TARGET_PLATFORM")
    else
        # 检测当前平台
        local current_platform
        current_platform=$(detect_current_platform)
        platforms_to_verify=("$current_platform")
        print_info "自动检测到平台: $current_platform"
    fi
    
    print_info "将验证以下平台: ${platforms_to_verify[*]}"
    
    # 验证每个平台
    local success_count=0
    local total_count=${#platforms_to_verify[@]}
    
    for platform in "${platforms_to_verify[@]}"; do
        print_info "[$((success_count + 1))/$total_count] 验证平台: $platform"
        echo "----------------------------------------"
        
        if verify_platform "$platform"; then
            ((success_count++))
        fi
        
        echo
    done
    
    # 总结
    print_info "=== 验证总结 ==="
    print_info "成功验证: $success_count/$total_count 个平台"
    
    if [[ $success_count -eq $total_count ]]; then
        print_success "所有平台验证成功！"
        exit 0
    else
        print_error "部分平台验证失败"
        exit 1
    fi
}

# 运行主函数
main "$@"