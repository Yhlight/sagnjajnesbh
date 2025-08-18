#include "ErrorHandler.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <chrono>

namespace chtl {
namespace shared {
namespace core {

// === ErrorHandler 实现 ===

ErrorHandler& ErrorHandler::getInstance() {
    static ErrorHandler instance;
    return instance;
}

ErrorHandler::ErrorHandler() 
    : verbose_(false), maxErrors_(100), suppressWarnings_(false), 
      enableRecovery_(true), minLevel_(ErrorLevel::INFO) {
    // 设置默认控制台处理器
    defaultHandler_ = std::make_shared<ConsoleErrorHandler>();
    handlers_.push_back(defaultHandler_);
}

void ErrorHandler::reportError(const ErrorInfo& error) {
    if (error.level < minLevel_) {
        return;
    }
    
    if (error.level == ErrorLevel::WARNING && suppressWarnings_) {
        return;
    }
    
    // 检查错误数量限制
    if (error.level >= ErrorLevel::ERROR && errors_.size() >= static_cast<size_t>(maxErrors_)) {
        return;
    }
    
    // 存储错误
    switch (error.level) {
        case ErrorLevel::ERROR:
        case ErrorLevel::CRITICAL:
            errors_.push_back(error);
            break;
        case ErrorLevel::WARNING:
            warnings_.push_back(error);
            break;
        case ErrorLevel::INFO:
            infos_.push_back(error);
            break;
    }
    
    // 通知处理器
    notifyHandlers(error);
    
    // 尝试错误恢复
    if (enableRecovery_ && error.level >= ErrorLevel::ERROR) {
        attemptRecovery(error);
    }
}

void ErrorHandler::reportError(ErrorType type, const std::string& code, const std::string& message, 
                              const std::string& file, int line, int column) {
    ErrorInfo error(ErrorLevel::ERROR, type, code, message);
    error.file = file;
    error.line = line;
    error.column = column;
    reportError(error);
}

void ErrorHandler::reportWarning(const std::string& message, const std::string& context) {
    ErrorInfo warning(ErrorLevel::WARNING, ErrorType::VALIDATION_ERROR, "CHTL_WARN", message);
    warning.context = context;
    reportError(warning);
}

void ErrorHandler::reportInfo(const std::string& message) {
    ErrorInfo info(ErrorLevel::INFO, ErrorType::VALIDATION_ERROR, "CHTL_INFO", message);
    reportError(info);
}

std::vector<ErrorInfo> ErrorHandler::getErrors() const {
    return errors_;
}

std::vector<ErrorInfo> ErrorHandler::getWarnings() const {
    return warnings_;
}

std::vector<ErrorInfo> ErrorHandler::getErrorsByType(ErrorType type) const {
    std::vector<ErrorInfo> result;
    
    for (const auto& error : errors_) {
        if (error.type == type) {
            result.push_back(error);
        }
    }
    
    return result;
}

int ErrorHandler::getErrorCount() const {
    return static_cast<int>(errors_.size());
}

int ErrorHandler::getWarningCount() const {
    return static_cast<int>(warnings_.size());
}

bool ErrorHandler::hasErrors() const {
    return !errors_.empty();
}

bool ErrorHandler::hasCriticalErrors() const {
    return std::any_of(errors_.begin(), errors_.end(),
        [](const ErrorInfo& error) { return error.level == ErrorLevel::CRITICAL; });
}

void ErrorHandler::clearErrors() {
    errors_.clear();
    warnings_.clear();
    infos_.clear();
}

void ErrorHandler::addHandler(std::shared_ptr<IErrorHandler> handler) {
    handlers_.push_back(handler);
}

void ErrorHandler::removeHandler(std::shared_ptr<IErrorHandler> handler) {
    handlers_.erase(std::remove(handlers_.begin(), handlers_.end(), handler), handlers_.end());
}

void ErrorHandler::setDefaultHandler(std::shared_ptr<IErrorHandler> handler) {
    defaultHandler_ = handler;
}

void ErrorHandler::enableRecovery(bool enable) {
    enableRecovery_ = enable;
}

void ErrorHandler::setRecoveryStrategy(ErrorType type, std::function<bool(const ErrorInfo&)> strategy) {
    recoveryStrategies_[type] = strategy;
}

bool ErrorHandler::attemptRecovery(const ErrorInfo& error) {
    auto it = recoveryStrategies_.find(error.type);
    if (it != recoveryStrategies_.end()) {
        return it->second(error);
    }
    return false;
}

std::string ErrorHandler::generateErrorReport() const {
    return generateDetailedReport();
}

std::string ErrorHandler::generateSummaryReport() const {
    std::ostringstream oss;
    
    oss << "=== CHTL错误处理报告 ===\n";
    oss << "错误数: " << errors_.size() << "\n";
    oss << "警告数: " << warnings_.size() << "\n";
    oss << "信息数: " << infos_.size() << "\n";
    
    if (hasErrors()) {
        oss << "\n错误类型统计:\n";
        std::map<ErrorType, int> typeCount;
        for (const auto& error : errors_) {
            typeCount[error.type]++;
        }
        
        for (const auto& [type, count] : typeCount) {
            oss << "  " << getErrorTypeString(type) << ": " << count << "\n";
        }
    }
    
    return oss.str();
}

std::string ErrorHandler::generateDetailedReport() const {
    std::ostringstream oss;
    
    oss << "=== CHTL详细错误报告 ===\n";
    oss << "生成时间: " << std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count() << "\n\n";
    
    // 错误详情
    if (!errors_.empty()) {
        oss << "🔴 错误 (" << errors_.size() << "):\n";
        for (const auto& error : errors_) {
            oss << formatError(error) << "\n";
        }
        oss << "\n";
    }
    
    // 警告详情
    if (!warnings_.empty() && !suppressWarnings_) {
        oss << "🟡 警告 (" << warnings_.size() << "):\n";
        for (const auto& warning : warnings_) {
            oss << formatError(warning) << "\n";
        }
        oss << "\n";
    }
    
    // 信息详情
    if (!infos_.empty() && verbose_) {
        oss << "🔵 信息 (" << infos_.size() << "):\n";
        for (const auto& info : infos_) {
            oss << formatError(info) << "\n";
        }
    }
    
    return oss.str();
}

bool ErrorHandler::exportReport(const std::string& filename, const std::string& format) const {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        if (format == "json") {
            // JSON格式报告
            file << "{\n";
            file << "  \"timestamp\": " << std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()).count() << ",\n";
            file << "  \"summary\": {\n";
            file << "    \"errorCount\": " << errors_.size() << ",\n";
            file << "    \"warningCount\": " << warnings_.size() << ",\n";
            file << "    \"infoCount\": " << infos_.size() << "\n";
            file << "  },\n";
            file << "  \"errors\": [\n";
            
            for (size_t i = 0; i < errors_.size(); ++i) {
                const auto& error = errors_[i];
                file << "    {\n";
                file << "      \"code\": \"" << error.code << "\",\n";
                file << "      \"type\": \"" << getErrorTypeString(error.type) << "\",\n";
                file << "      \"level\": \"" << getErrorLevelString(error.level) << "\",\n";
                file << "      \"message\": \"" << error.message << "\",\n";
                file << "      \"file\": \"" << error.file << "\",\n";
                file << "      \"line\": " << error.line << ",\n";
                file << "      \"column\": " << error.column << "\n";
                file << "    }";
                if (i < errors_.size() - 1) file << ",";
                file << "\n";
            }
            
            file << "  ]\n";
            file << "}\n";
        } else {
            // 文本格式报告
            file << generateDetailedReport();
        }
        
        file.close();
        return true;
        
    } catch (...) {
        return false;
    }
}

void ErrorHandler::setVerbose(bool verbose) {
    verbose_ = verbose;
}

void ErrorHandler::setMaxErrors(int maxErrors) {
    maxErrors_ = maxErrors;
}

void ErrorHandler::setSuppressWarnings(bool suppress) {
    suppressWarnings_ = suppress;
}

void ErrorHandler::setErrorLevel(ErrorLevel minLevel) {
    minLevel_ = minLevel;
}

void ErrorHandler::notifyHandlers(const ErrorInfo& error) {
    for (auto& handler : handlers_) {
        if (handler) {
            switch (error.level) {
                case ErrorLevel::ERROR:
                case ErrorLevel::CRITICAL:
                    handler->handleError(error);
                    break;
                case ErrorLevel::WARNING:
                    handler->handleWarning(error);
                    break;
                case ErrorLevel::INFO:
                    handler->handleInfo(error);
                    break;
            }
        }
    }
}

std::string ErrorHandler::formatError(const ErrorInfo& error) const {
    std::ostringstream oss;
    
    oss << "[" << error.code << "] " << getErrorLevelString(error.level) 
        << " - " << getErrorTypeString(error.type) << ": " << error.message;
    
    if (!error.file.empty()) {
        oss << " (在 " << error.file;
        if (error.line > 0) {
            oss << ":" << error.line;
            if (error.column > 0) {
                oss << ":" << error.column;
            }
        }
        oss << ")";
    }
    
    if (!error.context.empty()) {
        oss << "\n    上下文: " << error.context;
    }
    
    if (!error.suggestions.empty()) {
        oss << "\n    建议: ";
        for (size_t i = 0; i < error.suggestions.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << error.suggestions[i];
        }
    }
    
    return oss.str();
}

std::string ErrorHandler::getErrorTypeString(ErrorType type) const {
    switch (type) {
        case ErrorType::SYNTAX_ERROR: return "语法错误";
        case ErrorType::SEMANTIC_ERROR: return "语义错误";
        case ErrorType::IMPORT_ERROR: return "导入错误";
        case ErrorType::NAMESPACE_ERROR: return "命名空间错误";
        case ErrorType::MODULE_ERROR: return "模块错误";
        case ErrorType::RUNTIME_ERROR: return "运行时错误";
        case ErrorType::VALIDATION_ERROR: return "验证错误";
        default: return "未知错误";
    }
}

std::string ErrorHandler::getErrorLevelString(ErrorLevel level) const {
    switch (level) {
        case ErrorLevel::INFO: return "信息";
        case ErrorLevel::WARNING: return "警告";
        case ErrorLevel::ERROR: return "错误";
        case ErrorLevel::CRITICAL: return "严重错误";
        default: return "未知级别";
    }
}

// === ConsoleErrorHandler 实现 ===

ConsoleErrorHandler::ConsoleErrorHandler(bool useColors) : useColors_(useColors) {}

void ConsoleErrorHandler::handleError(const ErrorInfo& error) {
    std::string levelColor = (error.level == ErrorLevel::CRITICAL) ? "91" : "31"; // 红色
    std::string prefix = (error.level == ErrorLevel::CRITICAL) ? "💥" : "❌";
    
    if (useColors_) {
        printColored(prefix + " " + error.message, levelColor);
    } else {
        std::cerr << prefix << " " << error.message << std::endl;
    }
    
    if (!error.file.empty() && error.line > 0) {
        std::cerr << "    位置: " << error.file << ":" << error.line;
        if (error.column > 0) {
            std::cerr << ":" << error.column;
        }
        std::cerr << std::endl;
    }
}

void ConsoleErrorHandler::handleWarning(const ErrorInfo& warning) {
    std::string prefix = "⚠️";
    
    if (useColors_) {
        printColored(prefix + " " + warning.message, "33"); // 黄色
    } else {
        std::cout << prefix << " " << warning.message << std::endl;
    }
}

void ConsoleErrorHandler::handleInfo(const ErrorInfo& info) {
    std::string prefix = "ℹ️";
    
    if (useColors_) {
        printColored(prefix + " " + info.message, "36"); // 青色
    } else {
        std::cout << prefix << " " << info.message << std::endl;
    }
}

void ConsoleErrorHandler::printColored(const std::string& text, const std::string& color) const {
    std::cout << "\033[" << color << "m" << text << "\033[0m" << std::endl;
}

// === FileErrorHandler 实现 ===

FileErrorHandler::FileErrorHandler(const std::string& filename) : filename_(filename), logFile_(nullptr) {
    logFile_ = new std::ofstream(filename, std::ios::app);
}

FileErrorHandler::~FileErrorHandler() {
    close();
}

void FileErrorHandler::handleError(const ErrorInfo& error) {
    writeToFile("[ERROR] " + error.message);
}

void FileErrorHandler::handleWarning(const ErrorInfo& warning) {
    writeToFile("[WARNING] " + warning.message);
}

void FileErrorHandler::handleInfo(const ErrorInfo& info) {
    writeToFile("[INFO] " + info.message);
}

void FileErrorHandler::flush() {
    if (logFile_) {
        logFile_->flush();
    }
}

void FileErrorHandler::close() {
    if (logFile_) {
        logFile_->close();
        delete logFile_;
        logFile_ = nullptr;
    }
}

void FileErrorHandler::writeToFile(const std::string& message) {
    if (logFile_ && logFile_->is_open()) {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        
        *logFile_ << "[" << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S") << "] " 
                  << message << std::endl;
    }
}

} // namespace core
} // namespace shared
} // namespace chtl