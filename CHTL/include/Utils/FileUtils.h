#ifndef CHTL_UTILS_FILEUTILS_H
#define CHTL_UTILS_FILEUTILS_H

#include <string>
#include <vector>
#include <filesystem>

namespace Utils {

class FileUtils {
public:
    // 读取文件内容（UTF-8编码）
    static std::string ReadFile(const std::string& filename);
    
    // 写入文件内容（UTF-8编码）
    static bool WriteFile(const std::string& filename, const std::string& content);
    
    // 获取文件扩展名
    static std::string GetFileExtension(const std::string& filename);
    
    // 获取文件所在目录
    static std::string GetDirectory(const std::string& filename);
    
    // 获取文件名（不含路径）
    static std::string GetFilename(const std::string& path);
    
    // 获取文件名（不含路径和扩展名）
    static std::string GetFilenameWithoutExtension(const std::string& path);
    
    // 路径拼接
    static std::string JoinPath(const std::string& path1, const std::string& path2);
    
    // 检查文件是否存在
    static bool FileExists(const std::string& filename);
    
    // 检查目录是否存在
    static bool DirectoryExists(const std::string& dirname);
    
    // 创建目录（包括父目录）
    static bool CreateDirectory(const std::string& dirname);
    
    // 获取目录下的所有文件
    static std::vector<std::string> GetFilesInDirectory(const std::string& dirname, 
                                                        const std::string& extension = "");
    
    // 规范化路径
    static std::string NormalizePath(const std::string& path);
    
    // 获取绝对路径
    static std::string GetAbsolutePath(const std::string& path);
    
    // 获取相对路径
    static std::string GetRelativePath(const std::string& path, const std::string& base);
    
    // 判断是否为绝对路径
    static bool IsAbsolutePath(const std::string& path);
    
    // 将路径中的'.'转换为'/'
    static std::string ConvertDotToSlash(const std::string& path);
};

} // namespace Utils

#endif // CHTL_UTILS_FILEUTILS_H