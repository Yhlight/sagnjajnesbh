#include "Utils/ErrorHandler.h"
#include <algorithm>

namespace CHTL {
namespace Utils {

ErrorHandler& ErrorHandler::GetInstance() {
    static ErrorHandler instance;
    return instance;
}

void ErrorHandler::LogInfo(const std::string& message, 
                          const std::string& fileName, int line, int column,
                          const std::string& context) {
    ErrorInfo error(ErrorLevel::Info, message, fileName, line, column, context);
    LogMessage(error);
}

void ErrorHandler::LogWarning(const std::string& message, 
                             const std::string& fileName, int line, int column,
                             const std::string& context) {
    ErrorInfo error(ErrorLevel::Warning, message, fileName, line, column, context);
    LogMessage(error);
}

void ErrorHandler::LogError(const std::string& message, 
                           const std::string& fileName, int line, int column,
                           const std::string& context) {
    ErrorInfo error(ErrorLevel::Error, message, fileName, line, column, context);
    LogMessage(error);
}

void ErrorHandler::LogFatal(const std::string& message, 
                           const std::string& fileName, int line, int column,
                           const std::string& context) {
    ErrorInfo error(ErrorLevel::Fatal, message, fileName, line, column, context);
    LogMessage(error);
}

size_t ErrorHandler::GetErrorCount() const {
    return std::count_if(errors_.begin(), errors_.end(),
                        [](const ErrorInfo& error) {
                            return error.level == ErrorLevel::Error || 
                                   error.level == ErrorLevel::Fatal;
                        });
}

size_t ErrorHandler::GetWarningCount() const {
    return std::count_if(errors_.begin(), errors_.end(),
                        [](const ErrorInfo& error) {
                            return error.level == ErrorLevel::Warning;
                        });
}

bool ErrorHandler::HasErrors() const {
    return GetErrorCount() > 0;
}

bool ErrorHandler::HasWarnings() const {
    return GetWarningCount() > 0;
}

void ErrorHandler::Clear() {
    errors_.clear();
}

void ErrorHandler::PrintErrors() const {
    for (const auto& error : errors_) {
        std::string output = GetLevelString(error.level) + ": " + error.message;
        
        if (!error.fileName.empty()) {
            output += " (文件: " + error.fileName;
            if (error.line > 0) {
                output += ", 行: " + std::to_string(error.line);
            }
            if (error.column > 0) {
                output += ", 列: " + std::to_string(error.column);
            }
            output += ")";
        }
        
        if (verbose_ && !error.context.empty()) {
            output += "\n  上下文: " + error.context;
        }
        
        if (error.level == ErrorLevel::Error || error.level == ErrorLevel::Fatal) {
            std::cerr << output << std::endl;
        } else {
            std::cout << output << std::endl;
        }
    }
}

void ErrorHandler::LogMessage(const ErrorInfo& error) {
    if (errors_.size() >= maxErrors_) {
        return;
    }
    
    errors_.push_back(error);
    
    if (verbose_) {
        std::string output = GetLevelString(error.level) + ": " + error.message;
        
        if (!error.fileName.empty()) {
            output += " (文件: " + error.fileName;
            if (error.line > 0) {
                output += ", 行: " + std::to_string(error.line);
            }
            if (error.column > 0) {
                output += ", 列: " + std::to_string(error.column);
            }
            output += ")";
        }
        
        if (!error.context.empty()) {
            output += "\n  上下文: " + error.context;
        }
        
        if (error.level == ErrorLevel::Error || error.level == ErrorLevel::Fatal) {
            std::cerr << output << std::endl;
        } else {
            std::cout << output << std::endl;
        }
    }
}

std::string ErrorHandler::GetLevelString(ErrorLevel level) const {
    switch (level) {
        case ErrorLevel::Info:    return "信息";
        case ErrorLevel::Warning: return "警告";
        case ErrorLevel::Error:   return "错误";
        case ErrorLevel::Fatal:   return "致命错误";
        default:                  return "未知";
    }
}

} // namespace Utils
} // namespace CHTL