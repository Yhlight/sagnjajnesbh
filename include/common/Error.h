#ifndef CHTL_COMMON_ERROR_H
#define CHTL_COMMON_ERROR_H

#include <string>
#include <vector>
#include <memory>
#include <exception>
#include "SourceLocation.h"

namespace chtl {

// 错误级别
enum class ErrorLevel {
    INFO,
    WARNING,
    ERROR,
    FATAL
};

// 错误类型
enum class ErrorType {
    NONE,
    SYNTAX_ERROR,
    LEXICAL_ERROR,
    SEMANTIC_ERROR,
    TYPE_ERROR,
    IMPORT_ERROR,
    NAMESPACE_ERROR,
    CONSTRAINT_ERROR,
    INTERNAL_ERROR
};

// 诊断信息
class Diagnostic {
public:
    ErrorLevel level;
    ErrorType type;
    std::string message;
    SourceLocation location;
    std::string hint;  // 提示信息
    
    Diagnostic(ErrorLevel lvl, ErrorType t, const std::string& msg,
               const SourceLocation& loc = SourceLocation())
        : level(lvl), type(t), message(msg), location(loc) {}
    
    // 设置提示信息
    Diagnostic& withHint(const std::string& h) {
        hint = h;
        return *this;
    }
    
    // 格式化输出
    std::string format() const;
    
    // 判断是否是错误
    bool isError() const {
        return level == ErrorLevel::ERROR || level == ErrorLevel::FATAL;
    }
};

// 诊断收集器
class DiagnosticCollector {
private:
    std::vector<Diagnostic> diagnostics_;
    size_t errorCount_ = 0;
    size_t warningCount_ = 0;
    
public:
    // 添加诊断信息
    void add(const Diagnostic& diag);
    
    // 快捷方法
    void info(const std::string& msg, const SourceLocation& loc = SourceLocation());
    void warning(const std::string& msg, const SourceLocation& loc = SourceLocation());
    void error(const std::string& msg, const SourceLocation& loc = SourceLocation());
    void fatal(const std::string& msg, const SourceLocation& loc = SourceLocation());
    
    // 语法错误
    void syntaxError(const std::string& msg, const SourceLocation& loc);
    
    // 词法错误
    void lexicalError(const std::string& msg, const SourceLocation& loc);
    
    // 语义错误
    void semanticError(const std::string& msg, const SourceLocation& loc);
    
    // 导入错误
    void importError(const std::string& msg, const SourceLocation& loc);
    
    // 获取诊断信息
    const std::vector<Diagnostic>& getDiagnostics() const { return diagnostics_; }
    
    // 获取错误数量
    size_t getErrorCount() const { return errorCount_; }
    size_t getWarningCount() const { return warningCount_; }
    
    // 是否有错误
    bool hasErrors() const { return errorCount_ > 0; }
    
    // 清空
    void clear();
    
    // 输出所有诊断信息
    void print(std::ostream& out) const;
};

// CHTL异常基类
class CHTLException : public std::exception {
protected:
    std::string message_;
    SourceLocation location_;
    
public:
    CHTLException(const std::string& msg, const SourceLocation& loc = SourceLocation())
        : message_(msg), location_(loc) {}
    
    const char* what() const noexcept override {
        return message_.c_str();
    }
    
    const SourceLocation& getLocation() const { return location_; }
};

// 语法异常
class SyntaxException : public CHTLException {
public:
    SyntaxException(const std::string& msg, const SourceLocation& loc)
        : CHTLException("Syntax Error: " + msg, loc) {}
};

// 词法异常
class LexicalException : public CHTLException {
public:
    LexicalException(const std::string& msg, const SourceLocation& loc)
        : CHTLException("Lexical Error: " + msg, loc) {}
};

// 内部错误异常
class InternalException : public CHTLException {
public:
    InternalException(const std::string& msg)
        : CHTLException("Internal Error: " + msg) {}
};

} // namespace chtl

#endif // CHTL_COMMON_ERROR_H