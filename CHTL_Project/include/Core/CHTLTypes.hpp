#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <variant>
#include <optional>

namespace CHTL {

// 基础类型定义
using String = std::string;
using StringView = std::string_view;
using StringList = std::vector<String>;
using StringSet = std::unordered_set<String>;
using StringMap = std::unordered_map<String, String>;

// 智能指针类型
template<typename T>
using UniquePtr = std::unique_ptr<T>;

template<typename T>
using SharedPtr = std::shared_ptr<T>;

template<typename T>
using WeakPtr = std::weak_ptr<T>;

// 可选类型
template<typename T>
using Optional = std::optional<T>;

// 变体类型
template<typename... Types>
using Variant = std::variant<Types...>;

// 编译器状态枚举
enum class CompilerState {
    Idle,
    Scanning,
    Parsing,
    Generating,
    Error,
    Complete
};

// 代码片段类型
enum class FragmentType {
    CHTL,
    CHTLJS,
    CSS,
    JavaScript,
    Unknown
};

// 错误级别
enum class ErrorLevel {
    Info,
    Warning,
    Error,
    Fatal
};

// 编译器错误信息
struct CompilerError {
    ErrorLevel level;
    String message;
    String filename;
    size_t line;
    size_t column;
    
    CompilerError(ErrorLevel lvl, const String& msg, const String& file = "", 
                  size_t ln = 0, size_t col = 0)
        : level(lvl), message(msg), filename(file), line(ln), column(col) {}
};

// 代码位置信息
struct SourceLocation {
    String filename;
    size_t line;
    size_t column;
    size_t offset;
    
    SourceLocation(const String& file = "", size_t ln = 0, size_t col = 0, size_t off = 0)
        : filename(file), line(ln), column(col), offset(off) {}
};

// 代码片段
struct CodeFragment {
    FragmentType type;
    String content;
    SourceLocation location;
    
    CodeFragment(FragmentType t, const String& c, const SourceLocation& loc = {})
        : type(t), content(c), location(loc) {}
};

// 编译结果
struct CompilationResult {
    bool success;
    String output;
    std::vector<CompilerError> errors;
    
    CompilationResult() : success(false) {}
};

} // namespace CHTL