#include "Utils/FileUtils.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <locale>
#include <codecvt>

namespace CHTL {
namespace Utils {

std::string FileUtils::ReadFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string FileUtils::ReadFileUTF8(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        return "";
    }
    
    // 确保以UTF-8方式读取
    file.imbue(std::locale(""));
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // 检查并处理BOM
    if (content.length() >= 3 && 
        (unsigned char)content[0] == 0xEF && 
        (unsigned char)content[1] == 0xBB && 
        (unsigned char)content[2] == 0xBF) {
        content = content.substr(3);  // 移除BOM
    }
    
    return content;
}

bool FileUtils::WriteFile(const std::string& filePath, const std::string& content) {
    try {
        // 确保目录存在
        auto parentPath = std::filesystem::path(filePath).parent_path();
        if (!parentPath.empty()) {
            std::filesystem::create_directories(parentPath);
        }
        
        std::ofstream file(filePath, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        file << content;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "写入文件失败: " << filePath << " - " << e.what() << std::endl;
        return false;
    }
}

bool FileUtils::FileExists(const std::string& filePath) {
    return std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath);
}

std::string FileUtils::GetFileExtension(const std::string& filePath) {
    std::filesystem::path path(filePath);
    return path.extension().string();
}

std::string FileUtils::GetFileName(const std::string& filePath) {
    std::filesystem::path path(filePath);
    return path.stem().string();
}

std::string FileUtils::GetFileDirectory(const std::string& filePath) {
    std::filesystem::path path(filePath);
    return path.parent_path().string();
}

bool FileUtils::CreateDirectory(const std::string& dirPath) {
    try {
        return std::filesystem::create_directories(dirPath);
    } catch (const std::exception& e) {
        std::cerr << "创建目录失败: " << dirPath << " - " << e.what() << std::endl;
        return false;
    }
}

std::vector<std::string> FileUtils::ListFiles(const std::string& dirPath, bool recursive) {
    std::vector<std::string> files;
    
    try {
        if (recursive) {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(dirPath)) {
                if (entry.is_regular_file()) {
                    files.push_back(entry.path().string());
                }
            }
        } else {
            for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
                if (entry.is_regular_file()) {
                    files.push_back(entry.path().string());
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "列出文件失败: " << dirPath << " - " << e.what() << std::endl;
    }
    
    return files;
}

std::vector<std::string> FileUtils::FindFilesByExtension(
    const std::string& dirPath, 
    const std::string& extension, 
    bool recursive) {
    
    std::vector<std::string> files;
    std::string targetExt = extension.front() == '.' ? extension : "." + extension;
    
    try {
        if (recursive) {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(dirPath)) {
                if (entry.is_regular_file() && entry.path().extension() == targetExt) {
                    files.push_back(entry.path().string());
                }
            }
        } else {
            for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
                if (entry.is_regular_file() && entry.path().extension() == targetExt) {
                    files.push_back(entry.path().string());
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "搜索文件失败: " << dirPath << " - " << e.what() << std::endl;
    }
    
    return files;
}

std::string FileUtils::NormalizePath(const std::string& path) {
    try {
        return std::filesystem::canonical(path).string();
    } catch (const std::exception&) {
        // 如果路径不存在，使用lexically_normal
        return std::filesystem::path(path).lexically_normal().string();
    }
}

std::string FileUtils::JoinPath(const std::string& basePath, const std::string& relativePath) {
    std::filesystem::path base(basePath);
    std::filesystem::path relative(relativePath);
    return (base / relative).string();
}

} // namespace Utils
} // namespace CHTL