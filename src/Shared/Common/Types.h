#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace chtl {

// 前向声明
class ASTNode;
class Token;

// 基础类型别名
using StringList = std::vector<std::string>;
using StringMap = std::unordered_map<std::string, std::string>;
using ASTNodePtr = std::unique_ptr<ASTNode>;
using TokenPtr = std::unique_ptr<Token>;

// 位置信息
struct Position {
    size_t line;
    size_t column;
    size_t offset;
    
    Position(size_t l = 1, size_t c = 1, size_t o = 0) 
        : line(l), column(c), offset(o) {}
};

// 编译器类型
enum class CompilerType {
    CHTL,
    CHTL_JS,
    CSS,
    JAVASCRIPT
};

// 模块类型
enum class ModuleType {
    CMOD,
    CJMOD
};

// 编译阶段
enum class CompilationPhase {
    LEXICAL_ANALYSIS,
    SYNTAX_ANALYSIS,
    AST_GENERATION,
    CODE_GENERATION,
    OPTIMIZATION,
    COMPLETED
};

// 编译结果
struct CompilationResult {
    bool success;
    std::string output;
    StringList errors;
    StringList warnings;
    double compilation_time_ms;
    
    CompilationResult() : success(false), compilation_time_ms(0.0) {}
};

// 编译选项
struct CompilationOptions {
    bool enable_optimization;
    bool enable_debug_info;
    bool strict_mode;
    bool enable_warnings;
    std::string output_format;
    std::string target_directory;
    
    CompilationOptions() 
        : enable_optimization(false), enable_debug_info(false),
          strict_mode(true), enable_warnings(true),
          output_format("html") {}
};

} // namespace chtl