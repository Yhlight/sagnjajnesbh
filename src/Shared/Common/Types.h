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

// 编译结果
struct CompilationResult {
    bool success;
    std::string output;
    StringList errors;
    StringList warnings;
    
    CompilationResult() : success(false) {}
};

} // namespace chtl