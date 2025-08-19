// ========================================
// 统一错误处理接口 - 完全兼容所有管理器
// 解决新旧接口不兼容问题
// ========================================

#pragma once
#include "ErrorHandler.h"
#include <memory>
#include <string>
#include <vector>

namespace chtl {
namespace common {

// 错误级别和类型别名 - 兼容管理器使用
using ErrorLevel = chtl::shared::core::ErrorLevel;
using ErrorType = chtl::shared::core::ErrorType;
using ErrorInfo = chtl::shared::core::ErrorInfo;

/**
 * 统一错误处理器 - 完全兼容所有管理器的接口需求
 * 支持所有可能的调用方式和参数组合
 */
class UnifiedErrorHandler {
public:
    static std::shared_ptr<UnifiedErrorHandler> create() {
        return std::make_shared<UnifiedErrorHandler>();
    }
    
    // ========================================
    // 兼容ImportManager的调用方式
    // ========================================
    
    /**
     * ImportManager使用的6参数接口 (ErrorLevel, ErrorType, message, context, line, column)
     */
    void reportError(ErrorLevel level, ErrorType type, const std::string& message, 
                    const std::string& context, int line, int column) {
        auto& handler = chtl::shared::core::ErrorHandler::getInstance();
        
        std::string code = errorTypeToCode(type);
        std::string fullMessage = message;
        if (!context.empty()) {
            fullMessage += " (" + context + ")";
        }
        
        if (level == ErrorLevel::ERROR || level == ErrorLevel::CRITICAL) {
            handler.reportError(type, code, fullMessage, "", line, column);
        } else if (level == ErrorLevel::WARNING) {
            handler.reportWarning(fullMessage, code);
        } else {
            handler.reportInfo(fullMessage);
        }
    }
    
    // ========================================
    // 兼容NamespaceManager的调用方式
    // ========================================
    
    /**
     * NamespaceManager使用的4参数接口 (ErrorLevel, ErrorType, message, context)
     */
    void reportError(ErrorLevel level, ErrorType type, const std::string& message, const std::string& context = "") {
        auto& handler = chtl::shared::core::ErrorHandler::getInstance();
        
        std::string code = errorTypeToCode(type);
        std::string fullMessage = message;
        if (!context.empty()) {
            fullMessage += " (" + context + ")";
        }
        
        if (level == ErrorLevel::ERROR || level == ErrorLevel::CRITICAL) {
            handler.reportError(type, code, fullMessage);
        } else if (level == ErrorLevel::WARNING) {
            handler.reportWarning(fullMessage, code);
        } else {
            handler.reportInfo(fullMessage);
        }
    }
    
    // ========================================
    // 标准错误处理接口
    // ========================================
    
    /**
     * 标准错误报告接口 (ErrorType, code, message)
     */
    void reportError(ErrorType type, const std::string& code, const std::string& message, 
                    const std::string& file = "", int line = 0, int column = 0) {
        auto& handler = chtl::shared::core::ErrorHandler::getInstance();
        handler.reportError(type, code, message, file, line, column);
    }
    
    /**
     * 简单错误报告
     */
    void reportError(const std::string& message, const std::string& context = "") {
        reportError(ErrorLevel::ERROR, ErrorType::SYNTAX_ERROR, message, context);
    }
    
    /**
     * 警告报告
     */
    void reportWarning(const std::string& message, const std::string& context = "") {
        reportError(ErrorLevel::WARNING, ErrorType::SYNTAX_ERROR, message, context);
    }
    
    /**
     * 信息报告
     */
    void reportInfo(const std::string& message) {
        auto& handler = chtl::shared::core::ErrorHandler::getInstance();
        handler.reportInfo(message);
    }
    
    // ========================================
    // 查询接口
    // ========================================
    
    /**
     * 获取错误信息
     */
    std::vector<chtl::shared::core::ErrorInfo> getErrors() const {
        auto& handler = chtl::shared::core::ErrorHandler::getInstance();
        return handler.getErrors();
    }
    
    std::vector<std::string> getWarnings() const {
        auto& handler = chtl::shared::core::ErrorHandler::getInstance();
        auto warnings = handler.getWarnings();
        std::vector<std::string> result;
        for (const auto& warning : warnings) {
            result.push_back(warning.message);  // 提取ErrorInfo的message字段
        }
        return result;
    }
    
    bool hasErrors() const {
        auto& handler = chtl::shared::core::ErrorHandler::getInstance();
        return handler.hasErrors();
    }
    
    void clearErrors() {
        auto& handler = chtl::shared::core::ErrorHandler::getInstance();
        handler.clearErrors();
    }

private:
    std::string errorTypeToCode(ErrorType type) const {
        switch (type) {
            case ErrorType::SYNTAX_ERROR: return "SYNTAX";
            case ErrorType::SEMANTIC_ERROR: return "SEMANTIC";
            case ErrorType::IMPORT_ERROR: return "IMPORT";
            case ErrorType::NAMESPACE_ERROR: return "NAMESPACE";
            case ErrorType::MODULE_ERROR: return "MODULE";
            case ErrorType::RUNTIME_ERROR: return "RUNTIME";
            case ErrorType::VALIDATION_ERROR: return "VALIDATION";
            default: return "UNKNOWN";
        }
    }
};

// 为了兼容，创建ErrorHandler别名
using ErrorHandler = UnifiedErrorHandler;

} // namespace common
} // namespace chtl