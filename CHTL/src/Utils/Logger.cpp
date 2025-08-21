#include "Utils/Logger.h"
#include <sstream>
#include <filesystem>

namespace Utils {

Logger::~Logger() {
    if (logFile_.is_open()) {
        logFile_.close();
    }
}

void Logger::SetLogFile(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (logFile_.is_open()) {
        logFile_.close();
    }
    logFile_.open(filename, std::ios::app);
}

void Logger::Debug(const std::string& message) {
    if (debugMode_) {
        Log(LogLevel::Debug, message);
    }
}

void Logger::Info(const std::string& message) {
    Log(LogLevel::Info, message);
}

void Logger::Warning(const std::string& message) {
    Log(LogLevel::Warning, message);
}

void Logger::Error(const std::string& message) {
    Log(LogLevel::Error, message);
}

void Logger::Fatal(const std::string& message) {
    Log(LogLevel::Fatal, message);
}

void Logger::Debug(const std::string& message, const std::string& file, int line) {
    if (debugMode_) {
        Log(LogLevel::Debug, message, file, line);
    }
}

void Logger::Info(const std::string& message, const std::string& file, int line) {
    Log(LogLevel::Info, message, file, line);
}

void Logger::Warning(const std::string& message, const std::string& file, int line) {
    Log(LogLevel::Warning, message, file, line);
}

void Logger::Error(const std::string& message, const std::string& file, int line) {
    Log(LogLevel::Error, message, file, line);
}

void Logger::Fatal(const std::string& message, const std::string& file, int line) {
    Log(LogLevel::Fatal, message, file, line);
    std::exit(1);
}

void Logger::Log(LogLevel level, const std::string& message, 
                 const std::string& file, int line) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::string timestamp = GetTimestamp();
    std::string levelStr = GetLevelString(level);
    std::string colorCode = GetColorCode(level);
    
    std::stringstream ss;
    ss << "[" << timestamp << "] ";
    
    if (!file.empty() && line >= 0) {
        std::filesystem::path filePath(file);
        ss << "[" << filePath.filename().string() << ":" << line << "] ";
    }
    
    ss << "[" << levelStr << "] " << message;
    
    // 输出到控制台（带颜色）
    std::cout << colorCode << ss.str() << "\033[0m" << std::endl;
    
    // 输出到文件（不带颜色）
    if (logFile_.is_open()) {
        logFile_ << ss.str() << std::endl;
        logFile_.flush();
    }
}

std::string Logger::GetTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string Logger::GetLevelString(LogLevel level) {
    switch (level) {
        case LogLevel::Debug:   return "调试";
        case LogLevel::Info:    return "信息";
        case LogLevel::Warning: return "警告";
        case LogLevel::Error:   return "错误";
        case LogLevel::Fatal:   return "致命";
        default:                return "未知";
    }
}

std::string Logger::GetColorCode(LogLevel level) {
    switch (level) {
        case LogLevel::Debug:   return "\033[90m";    // 灰色
        case LogLevel::Info:    return "\033[32m";    // 绿色
        case LogLevel::Warning: return "\033[33m";    // 黄色
        case LogLevel::Error:   return "\033[31m";    // 红色
        case LogLevel::Fatal:   return "\033[35m";    // 紫色
        default:                return "\033[0m";     // 默认
    }
}

} // namespace Utils