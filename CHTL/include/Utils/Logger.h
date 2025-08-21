#ifndef CHTL_UTILS_LOGGER_H
#define CHTL_UTILS_LOGGER_H

#include <string>
#include <iostream>
#include <fstream>
#include <mutex>
#include <chrono>
#include <iomanip>

namespace Utils {

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error,
    Fatal
};

class Logger {
public:
    static Logger& GetInstance() {
        static Logger instance;
        return instance;
    }
    
    void SetDebugMode(bool enable) { debugMode_ = enable; }
    void SetLogFile(const std::string& filename);
    
    void Debug(const std::string& message);
    void Info(const std::string& message);
    void Warning(const std::string& message);
    void Error(const std::string& message);
    void Fatal(const std::string& message);
    
    // 带位置信息的日志
    void Debug(const std::string& message, const std::string& file, int line);
    void Info(const std::string& message, const std::string& file, int line);
    void Warning(const std::string& message, const std::string& file, int line);
    void Error(const std::string& message, const std::string& file, int line);
    void Fatal(const std::string& message, const std::string& file, int line);
    
private:
    Logger() = default;
    ~Logger();
    
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    void Log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = -1);
    
    std::string GetTimestamp();
    std::string GetLevelString(LogLevel level);
    std::string GetColorCode(LogLevel level);
    
    bool debugMode_ = false;
    std::ofstream logFile_;
    std::mutex mutex_;
};

// 宏定义，方便记录位置信息
#define LOG_DEBUG(msg) Utils::Logger::GetInstance().Debug(msg, __FILE__, __LINE__)
#define LOG_INFO(msg) Utils::Logger::GetInstance().Info(msg, __FILE__, __LINE__)
#define LOG_WARNING(msg) Utils::Logger::GetInstance().Warning(msg, __FILE__, __LINE__)
#define LOG_ERROR(msg) Utils::Logger::GetInstance().Error(msg, __FILE__, __LINE__)
#define LOG_FATAL(msg) Utils::Logger::GetInstance().Fatal(msg, __FILE__, __LINE__)

} // namespace Utils

#endif // CHTL_UTILS_LOGGER_H