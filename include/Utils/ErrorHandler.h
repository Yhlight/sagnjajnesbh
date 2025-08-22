#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>

namespace CHTL {
namespace Utils {

/**
 * @brief 错误级别枚举
 */
enum class ErrorLevel {
    Info,       // 信息
    Warning,    // 警告
    Error,      // 错误
    Fatal       // 致命错误
};

/**
 * @brief 错误信息结构体
 */
struct ErrorInfo {
    ErrorLevel level;           // 错误级别
    std::string message;        // 错误消息
    std::string fileName;       // 文件名
    int line;                   // 行号
    int column;                 // 列号
    std::string context;        // 上下文信息
    
    ErrorInfo(ErrorLevel lvl, const std::string& msg, 
              const std::string& file = "", int ln = -1, int col = -1, 
              const std::string& ctx = "")
        : level(lvl), message(msg), fileName(file), line(ln), column(col), context(ctx) {}
};

/**
 * @brief 错误处理器类
 */
class ErrorHandler {
public:
    /**
     * @brief 获取单例实例
     * @return 错误处理器实例
     */
    static ErrorHandler& GetInstance();
    
    /**
     * @brief 记录信息
     * @param message 信息内容
     * @param fileName 文件名
     * @param line 行号
     * @param column 列号
     * @param context 上下文
     */
    void LogInfo(const std::string& message, 
                 const std::string& fileName = "", int line = -1, int column = -1,
                 const std::string& context = "");
    
    /**
     * @brief 记录警告
     * @param message 警告内容
     * @param fileName 文件名
     * @param line 行号
     * @param column 列号
     * @param context 上下文
     */
    void LogWarning(const std::string& message, 
                    const std::string& fileName = "", int line = -1, int column = -1,
                    const std::string& context = "");
    
    /**
     * @brief 记录错误
     * @param message 错误内容
     * @param fileName 文件名
     * @param line 行号
     * @param column 列号
     * @param context 上下文
     */
    void LogError(const std::string& message, 
                  const std::string& fileName = "", int line = -1, int column = -1,
                  const std::string& context = "");
    
    /**
     * @brief 记录致命错误
     * @param message 错误内容
     * @param fileName 文件名
     * @param line 行号
     * @param column 列号
     * @param context 上下文
     */
    void LogFatal(const std::string& message, 
                  const std::string& fileName = "", int line = -1, int column = -1,
                  const std::string& context = "");
    
    /**
     * @brief 获取所有错误信息
     * @return 错误信息列表
     */
    const std::vector<ErrorInfo>& GetErrors() const { return errors_; }
    
    /**
     * @brief 获取错误数量
     * @return 错误数量
     */
    size_t GetErrorCount() const;
    
    /**
     * @brief 获取警告数量
     * @return 警告数量
     */
    size_t GetWarningCount() const;
    
    /**
     * @brief 检查是否有错误
     * @return 是否有错误
     */
    bool HasErrors() const;
    
    /**
     * @brief 检查是否有警告
     * @return 是否有警告
     */
    bool HasWarnings() const;
    
    /**
     * @brief 清空所有错误信息
     */
    void Clear();
    
    /**
     * @brief 打印所有错误信息
     */
    void PrintErrors() const;
    
    /**
     * @brief 设置是否启用详细模式
     * @param verbose 是否启用详细模式
     */
    void SetVerbose(bool verbose) { verbose_ = verbose; }
    
    /**
     * @brief 设置最大错误数量
     * @param maxErrors 最大错误数量
     */
    void SetMaxErrors(size_t maxErrors) { maxErrors_ = maxErrors; }

private:
    ErrorHandler() : verbose_(false), maxErrors_(100) {}
    
    void LogMessage(const ErrorInfo& error);
    std::string GetLevelString(ErrorLevel level) const;
    
    std::vector<ErrorInfo> errors_;
    bool verbose_;
    size_t maxErrors_;
};

// 便捷宏定义
#define CHTL_LOG_INFO(msg) \
    CHTL::Utils::ErrorHandler::GetInstance().LogInfo(msg, __FILE__, __LINE__)

#define CHTL_LOG_WARNING(msg) \
    CHTL::Utils::ErrorHandler::GetInstance().LogWarning(msg, __FILE__, __LINE__)

#define CHTL_LOG_ERROR(msg) \
    CHTL::Utils::ErrorHandler::GetInstance().LogError(msg, __FILE__, __LINE__)

#define CHTL_LOG_FATAL(msg) \
    CHTL::Utils::ErrorHandler::GetInstance().LogFatal(msg, __FILE__, __LINE__)

} // namespace Utils
} // namespace CHTL