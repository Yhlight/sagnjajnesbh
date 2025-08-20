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
    static Logger& GetInstance() {
        static Logger instance;
        return instance;
    }

    void SetLevel(LogLevel level) { 
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_CurrentLevel = level; 
    }
    
    void SetOutputFile(const std::string& filename) {
        std::lock_guard<std::mutex> lock(m_Mutex);
        if (m_FileStream.is_open()) {
            m_FileStream.close();
        }
        m_FileStream.open(filename, std::ios::app);
    }

    void Error(const std::string& message, const std::string& file = "", int line = 0) {
        Log(LogLevel::ERROR, message, file, line);
    }

    void Warning(const std::string& message, const std::string& file = "", int line = 0) {
        Log(LogLevel::WARNING, message, file, line);
    }

    void Info(const std::string& message) {
        Log(LogLevel::INFO, message);
    }

    void Debug(const std::string& message) {
        Log(LogLevel::DEBUG, message);
    }

    void Verbose(const std::string& message) {
        Log(LogLevel::VERBOSE, message);
    }

private:
    Logger() : m_CurrentLevel(LogLevel::INFO) {}
    ~Logger() {
        if (m_FileStream.is_open()) {
            m_FileStream.close();
        }
    }

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void Log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0) {
        if (level > m_CurrentLevel) return;

        std::lock_guard<std::mutex> lock(m_Mutex);
        
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        
        std::string levelStr = GetLevelString(level);
        std::string color = GetColorCode(level);
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
        if (m_FileStream.is_open()) {
            m_FileStream << logLine << std::endl;
            m_FileStream.flush();
        }
    }

    std::string GetLevelString(LogLevel level) {
        switch (level) {
            case LogLevel::ERROR: return "错误";
            case LogLevel::WARNING: return "警告";
            case LogLevel::INFO: return "信息";
            case LogLevel::DEBUG: return "调试";
            case LogLevel::VERBOSE: return "详细";
            default: return "未知";
        }
    }

    std::string GetColorCode(LogLevel level) {
        switch (level) {
            case LogLevel::ERROR: return "\033[31m";    // 红色
            case LogLevel::WARNING: return "\033[33m";  // 黄色
            case LogLevel::INFO: return "\033[32m";     // 绿色
            case LogLevel::DEBUG: return "\033[36m";    // 青色
            case LogLevel::VERBOSE: return "\033[37m";  // 白色
            default: return "\033[0m";
        }
    }

    LogLevel m_CurrentLevel;
    std::ofstream m_FileStream;
    std::mutex m_Mutex;
};

} // namespace utils
} // namespace chtl