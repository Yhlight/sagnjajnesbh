#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <mutex>
#include <chrono>
#include <iomanip>

namespace chtl {
namespace utils {

enum class LogLevel {
    ERROR = 0,
    WARNING = 1,
    INFO = 2,
    DEBUG = 3,
    VERBOSE = 4
};

class Logger {
public:
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    void setLevel(LogLevel level) { 
        std::lock_guard<std::mutex> lock(mutex_);
        currentLevel_ = level; 
    }
    
    void setOutputFile(const std::string& filename) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (fileStream_.is_open()) {
            fileStream_.close();
        }
        fileStream_.open(filename, std::ios::app);
    }

    void error(const std::string& message, const std::string& file = "", int line = 0) {
        log(LogLevel::ERROR, message, file, line);
    }

    void warning(const std::string& message, const std::string& file = "", int line = 0) {
        log(LogLevel::WARNING, message, file, line);
    }

    void info(const std::string& message) {
        log(LogLevel::INFO, message);
    }

    void debug(const std::string& message) {
        log(LogLevel::DEBUG, message);
    }

    void verbose(const std::string& message) {
        log(LogLevel::VERBOSE, message);
    }

private:
    Logger() : currentLevel_(LogLevel::INFO) {}
    ~Logger() {
        if (fileStream_.is_open()) {
            fileStream_.close();
        }
    }

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0) {
        if (level > currentLevel_) return;

        std::lock_guard<std::mutex> lock(mutex_);
        
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        
        std::string levelStr = getLevelString(level);
        std::string color = getColorCode(level);
        std::string reset = "\033[0m";

        std::stringstream ss;
        ss << "[" << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S") << "] ";
        ss << color << "[" << levelStr << "]" << reset << " ";
        
        if (!file.empty() && line > 0) {
            ss << file << ":" << line << " - ";
        }
        
        ss << message;

        std::string logLine = ss.str();

        // 输出到控制台
        if (level <= LogLevel::ERROR) {
            std::cerr << logLine << std::endl;
        } else {
            std::cout << logLine << std::endl;
        }

        // 输出到文件
        if (fileStream_.is_open()) {
            fileStream_ << logLine << std::endl;
            fileStream_.flush();
        }
    }

    std::string getLevelString(LogLevel level) {
        switch (level) {
            case LogLevel::ERROR: return "错误";
            case LogLevel::WARNING: return "警告";
            case LogLevel::INFO: return "信息";
            case LogLevel::DEBUG: return "调试";
            case LogLevel::VERBOSE: return "详细";
            default: return "未知";
        }
    }

    std::string getColorCode(LogLevel level) {
        switch (level) {
            case LogLevel::ERROR: return "\033[31m";    // 红色
            case LogLevel::WARNING: return "\033[33m";  // 黄色
            case LogLevel::INFO: return "\033[32m";     // 绿色
            case LogLevel::DEBUG: return "\033[36m";    // 青色
            case LogLevel::VERBOSE: return "\033[37m";  // 白色
            default: return "\033[0m";
        }
    }

    LogLevel currentLevel_;
    std::ofstream fileStream_;
    std::mutex mutex_;
};

} // namespace utils
} // namespace chtl