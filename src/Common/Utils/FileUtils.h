#pragma once
#include <string>
#include <vector>
#include <fstream>

namespace chtl {
namespace utils {

/**
 * 文件操作工具类
 */
class FileUtils {
public:
    // 文件读取
    static bool readFile(const std::string& file_path, std::string& content);
    static std::vector<std::string> readLines(const std::string& file_path);
    
    // 文件写入
    static bool writeFile(const std::string& file_path, const std::string& content);
    static bool writeLines(const std::string& file_path, const std::vector<std::string>& lines);
    
    // 文件检查
    static bool fileExists(const std::string& file_path);
    static bool isDirectory(const std::string& path);
    static bool isFile(const std::string& path);
    
    // 路径操作
    static std::string getFileName(const std::string& file_path);
    static std::string getFileExtension(const std::string& file_path);
    static std::string getDirectory(const std::string& file_path);
    static std::string joinPath(const std::string& dir, const std::string& file);
    
    // 目录操作
    static bool createDirectory(const std::string& dir_path);
    static bool removeFile(const std::string& file_path);
    static std::vector<std::string> listFiles(const std::string& dir_path);
    static std::vector<std::string> listDirectories(const std::string& dir_path);
    
    // 文件大小
    static size_t getFileSize(const std::string& file_path);
    
    // 临时文件
    static std::string createTempFile(const std::string& prefix = "chtl_temp_");
    static void cleanupTempFile(const std::string& file_path);
};

} // namespace utils
} // namespace chtl