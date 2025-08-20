#include "../../include/common/Error.h"
#include <iostream>
#include <sstream>
#include <iomanip>

namespace chtl {

// 错误级别到字符串的转换
static const char* errorLevelToString(ErrorLevel level) {
    switch (level) {
        case ErrorLevel::INFO:    return "信息";
        case ErrorLevel::WARNING: return "警告";
        case ErrorLevel::ERROR:   return "错误";
        case ErrorLevel::FATAL:   return "致命错误";
        default:                  return "未知";
    }
}

// 错误类型到字符串的转换
static const char* errorTypeToString(ErrorType type) {
    switch (type) {
        case ErrorType::NONE:             return "";
        case ErrorType::SYNTAX_ERROR:     return "语法错误";
        case ErrorType::LEXICAL_ERROR:    return "词法错误";
        case ErrorType::SEMANTIC_ERROR:   return "语义错误";
        case ErrorType::TYPE_ERROR:       return "类型错误";
        case ErrorType::IMPORT_ERROR:     return "导入错误";
        case ErrorType::NAMESPACE_ERROR:  return "命名空间错误";
        case ErrorType::CONSTRAINT_ERROR: return "约束错误";
        case ErrorType::INTERNAL_ERROR:   return "内部错误";
        default:                          return "未知错误";
    }
}

// Diagnostic实现
std::string Diagnostic::format() const {
    std::stringstream ss;
    
    // 错误级别
    ss << "[" << errorLevelToString(level) << "]";
    
    // 错误类型
    if (type != ErrorType::NONE) {
        ss << " " << errorTypeToString(type);
    }
    
    // 位置信息
    if (location.isValid()) {
        ss << " " << location.toString();
    }
    
    ss << ": " << message;
    
    // 提示信息
    if (!hint.empty()) {
        ss << "\n提示: " << hint;
    }
    
    return ss.str();
}

// DiagnosticCollector实现
void DiagnosticCollector::add(const Diagnostic& diag) {
    diagnostics_.push_back(diag);
    
    if (diag.isError()) {
        errorCount_++;
    } else if (diag.level == ErrorLevel::WARNING) {
        warningCount_++;
    }
}

void DiagnosticCollector::info(const std::string& msg, const SourceLocation& loc) {
    add(Diagnostic(ErrorLevel::INFO, ErrorType::NONE, msg, loc));
}

void DiagnosticCollector::warning(const std::string& msg, const SourceLocation& loc) {
    add(Diagnostic(ErrorLevel::WARNING, ErrorType::NONE, msg, loc));
}

void DiagnosticCollector::error(const std::string& msg, const SourceLocation& loc) {
    add(Diagnostic(ErrorLevel::ERROR, ErrorType::NONE, msg, loc));
}

void DiagnosticCollector::fatal(const std::string& msg, const SourceLocation& loc) {
    add(Diagnostic(ErrorLevel::FATAL, ErrorType::NONE, msg, loc));
}

void DiagnosticCollector::syntaxError(const std::string& msg, const SourceLocation& loc) {
    add(Diagnostic(ErrorLevel::ERROR, ErrorType::SYNTAX_ERROR, msg, loc));
}

void DiagnosticCollector::lexicalError(const std::string& msg, const SourceLocation& loc) {
    add(Diagnostic(ErrorLevel::ERROR, ErrorType::LEXICAL_ERROR, msg, loc));
}

void DiagnosticCollector::semanticError(const std::string& msg, const SourceLocation& loc) {
    add(Diagnostic(ErrorLevel::ERROR, ErrorType::SEMANTIC_ERROR, msg, loc));
}

void DiagnosticCollector::importError(const std::string& msg, const SourceLocation& loc) {
    add(Diagnostic(ErrorLevel::ERROR, ErrorType::IMPORT_ERROR, msg, loc));
}

void DiagnosticCollector::clear() {
    diagnostics_.clear();
    errorCount_ = 0;
    warningCount_ = 0;
}

void DiagnosticCollector::print(std::ostream& out) const {
    for (const auto& diag : diagnostics_) {
        out << diag.format() << std::endl;
    }
    
    // 汇总信息
    if (errorCount_ > 0 || warningCount_ > 0) {
        out << "\n";
        if (errorCount_ > 0) {
            out << "共 " << errorCount_ << " 个错误";
        }
        if (warningCount_ > 0) {
            if (errorCount_ > 0) out << ", ";
            out << warningCount_ << " 个警告";
        }
        out << std::endl;
    }
}

} // namespace chtl