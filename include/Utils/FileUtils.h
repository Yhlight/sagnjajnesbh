#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace CHTL {
namespace Utils {

/**
 * @brief 文件操作工具类
 */
class FileUtils {
public:
    /**
     * @brief 读取文件内容
     * @param filePath 文件路径
     * @return 文件内容，读取失败返回空字符串
     */
    static std::string ReadFile(const std::string& filePath);
    
    /**
     * @brief 读取UTF-8编码的文件内容
     * @param filePath 文件路径
     * @return 文件内容，读取失败返回空字符串
     */
    static std::string ReadFileUTF8(const std::string& filePath);
    
    /**
     * @brief 写入文件内容
     * @param filePath 文件路径
     * @param content 要写入的内容
     * @return 写入是否成功
     */
    static bool WriteFile(const std::string& filePath, const std::string& content);
    
    /**
     * @brief 检查文件是否存在
     * @param filePath 文件路径
     * @return 文件是否存在
     */
    static bool FileExists(const std::string& filePath);
    
    /**
     * @brief 获取文件扩展名
     * @param filePath 文件路径
     * @return 文件扩展名（包含点号）
     */
    static std::string GetFileExtension(const std::string& filePath);
    
    /**
     * @brief 获取文件名（不包含路径和扩展名）
     * @param filePath 文件路径
     * @return 文件名
     */
    static std::string GetFileName(const std::string& filePath);
    
    /**
     * @brief 获取文件目录
     * @param filePath 文件路径
     * @return 文件所在目录
     */
    static std::string GetFileDirectory(const std::string& filePath);
    
    /**
     * @brief 创建目录
     * @param dirPath 目录路径
     * @return 创建是否成功
     */
    static bool CreateDirectory(const std::string& dirPath);
    
    /**
     * @brief 列出目录中的所有文件
     * @param dirPath 目录路径
     * @param recursive 是否递归搜索子目录
     * @return 文件路径列表
     */
    static std::vector<std::string> ListFiles(const std::string& dirPath, bool recursive = false);
    
    /**
     * @brief 搜索特定扩展名的文件
     * @param dirPath 搜索目录
     * @param extension 文件扩展名（不包含点号）
     * @param recursive 是否递归搜索
     * @return 匹配的文件路径列表
     */
    static std::vector<std::string> FindFilesByExtension(
        const std::string& dirPath, 
        const std::string& extension, 
        bool recursive = false
    );
    
    /**
     * @brief 规范化路径
     * @param path 原始路径
     * @return 规范化后的路径
     */
    static std::string NormalizePath(const std::string& path);
    
    /**
     * @brief 连接路径
     * @param basePath 基础路径
     * @param relativePath 相对路径
     * @return 连接后的路径
     */
    static std::string JoinPath(const std::string& basePath, const std::string& relativePath);
};

} // namespace Utils
} // namespace CHTL