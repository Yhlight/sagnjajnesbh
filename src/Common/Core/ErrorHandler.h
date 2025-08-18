#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace chtl {
namespace shared {
namespace core {

/**
 * 错误级别枚举
 */
enum class ErrorLevel {
    INFO = 0,
    WARNING = 1,
    ERROR = 2,
    CRITICAL = 3
};

/**
 * 错误类型枚举
 */
enum class ErrorType {
    SYNTAX_ERROR = 1,
    SEMANTIC_ERROR = 2,
    IMPORT_ERROR = 3,
    NAMESPACE_ERROR = 4,
    MODULE_ERROR = 5,
    RUNTIME_ERROR = 6,
    VALIDATION_ERROR = 7
};

/**
 * 错误信息结构
 */
struct ErrorInfo {
    ErrorLevel level;
    ErrorType type;
    std::string code;           // 错误代码 (如 CHTL001)
    std::string message;        // 错误消息
    std::string details;        // 详细描述
    std::string file;           // 文件路径
    int line;                   // 行号
    int column;                 // 列号
    std::string context;        // 上下文代码
    std::vector<std::string> suggestions; // 修复建议
    
    ErrorInfo() : level(ErrorLevel::ERROR), type(ErrorType::SYNTAX_ERROR), line(0), column(0) {}
    
    ErrorInfo(ErrorLevel lvl, ErrorType tp, const std::string& cd, const std::string& msg)
        : level(lvl), type(tp), code(cd), message(msg), line(0), column(0) {}
};

/**
 * 错误处理器接口
 */
class IErrorHandler {
public:
    virtual ~IErrorHandler() = default;
    virtual void handleError(const ErrorInfo& error) = 0;
    virtual void handleWarning(const ErrorInfo& warning) = 0;
    virtual void handleInfo(const ErrorInfo& info) = 0;
};

/**
 * CHTL统一错误处理系统
 */
class ErrorHandler {
public:
    static ErrorHandler& getInstance();
    
    // === 错误报告 ===
    void reportError(const ErrorInfo& error);
    void reportError(ErrorType type, const std::string& code, const std::string& message, 
                    const std::string& file = "", int line = 0, int column = 0);
    void reportWarning(const std::string& message, const std::string& context = "");
    void reportInfo(const std::string& message);
    
    // === 错误管理 ===
    std::vector<ErrorInfo> getErrors() const;
    std::vector<ErrorInfo> getWarnings() const;
    std::vector<ErrorInfo> getErrorsByType(ErrorType type) const;
    int getErrorCount() const;
    int getWarningCount() const;
    bool hasErrors() const;
    bool hasCriticalErrors() const;
    void clearErrors();
    
    // === 错误处理器管理 ===
    void addHandler(std::shared_ptr<IErrorHandler> handler);
    void removeHandler(std::shared_ptr<IErrorHandler> handler);
    void setDefaultHandler(std::shared_ptr<IErrorHandler> handler);
    
    // === 错误恢复 ===
    void enableRecovery(bool enable);
    void setRecoveryStrategy(ErrorType type, std::function<bool(const ErrorInfo&)> strategy);
    bool attemptRecovery(const ErrorInfo& error);
    
    // === 报告生成 ===
    std::string generateErrorReport() const;
    std::string generateSummaryReport() const;
    std::string generateDetailedReport() const;
    bool exportReport(const std::string& filename, const std::string& format = "text") const;
    
    // === 配置 ===
    void setVerbose(bool verbose);
    void setMaxErrors(int maxErrors);
    void setSuppressWarnings(bool suppress);
    void setErrorLevel(ErrorLevel minLevel);

private:
    std::vector<ErrorInfo> errors_;
    std::vector<ErrorInfo> warnings_;
    std::vector<ErrorInfo> infos_;
    std::vector<std::shared_ptr<IErrorHandler>> handlers_;
    std::shared_ptr<IErrorHandler> defaultHandler_;
    std::map<ErrorType, std::function<bool(const ErrorInfo&)>> recoveryStrategies_;
    
    bool verbose_;
    int maxErrors_;
    bool suppressWarnings_;
    bool enableRecovery_;
    ErrorLevel minLevel_;
    
    ErrorHandler();
    void notifyHandlers(const ErrorInfo& error);
    std::string formatError(const ErrorInfo& error) const;
    std::string getErrorTypeString(ErrorType type) const;
    std::string getErrorLevelString(ErrorLevel level) const;
};

/**
 * 控制台错误处理器
 */
class ConsoleErrorHandler : public IErrorHandler {
public:
    ConsoleErrorHandler(bool useColors = true);
    
    void handleError(const ErrorInfo& error) override;
    void handleWarning(const ErrorInfo& warning) override;
    void handleInfo(const ErrorInfo& info) override;

private:
    bool useColors_;
    void printColored(const std::string& text, const std::string& color) const;
};

/**
 * 文件错误处理器
 */
class FileErrorHandler : public IErrorHandler {
public:
    FileErrorHandler(const std::string& filename);
    ~FileErrorHandler();
    
    void handleError(const ErrorInfo& error) override;
    void handleWarning(const ErrorInfo& warning) override;
    void handleInfo(const ErrorInfo& info) override;
    
    void flush();
    void close();

private:
    std::string filename_;
    std::ofstream* logFile_;
    void writeToFile(const std::string& message);
};

/**
 * CHTL特定错误代码定义
 */
namespace ErrorCodes {
    // 语法错误 (CHTL001-099)
    constexpr const char* UNCLOSED_TAG = "CHTL001";
    constexpr const char* INVALID_SELECTOR = "CHTL002";
    constexpr const char* UNKNOWN_FUNCTION = "CHTL003";
    constexpr const char* MALFORMED_ARROW = "CHTL004";
    constexpr const char* INVALID_LITERAL = "CHTL005";
    
    // 导入错误 (CHTL100-199)
    constexpr const char* INVALID_IMPORT_TYPE = "CHTL100";
    constexpr const char* MODULE_NOT_FOUND = "CHTL101";
    constexpr const char* CIRCULAR_IMPORT = "CHTL102";
    constexpr const char* IMPORT_SYNTAX_ERROR = "CHTL103";
    
    // 命名空间错误 (CHTL200-299)
    constexpr const char* NAMESPACE_CONFLICT = "CHTL200";
    constexpr const char* INVALID_NAMESPACE_NAME = "CHTL201";
    constexpr const char* NAMESPACE_NOT_FOUND = "CHTL202";
    
    // 模块错误 (CHTL300-399)
    constexpr const char* INVALID_CMOD_STRUCTURE = "CHTL300";
    constexpr const char* INVALID_CJMOD_STRUCTURE = "CHTL301";
    constexpr const char* MODULE_VERSION_MISMATCH = "CHTL302";
    constexpr const char* MISSING_MODULE_INFO = "CHTL303";
    
    // 运行时错误 (CHTL400-499)
    constexpr const char* VIRTUAL_OBJECT_ERROR = "CHTL400";
    constexpr const char* FUNCTION_CALL_ERROR = "CHTL401";
    constexpr const char* ANIMATION_ERROR = "CHTL402";
}

/**
 * 错误处理宏
 */
#define CHTL_ERROR(type, code, message, ...) \
    chtl::shared::core::ErrorHandler::getInstance().reportError(type, code, message, __VA_ARGS__)

#define CHTL_WARNING(message, ...) \
    chtl::shared::core::ErrorHandler::getInstance().reportWarning(message, ##__VA_ARGS__)

#define CHTL_INFO(message) \
    chtl::shared::core::ErrorHandler::getInstance().reportInfo(message)

} // namespace core
} // namespace shared
} // namespace chtl