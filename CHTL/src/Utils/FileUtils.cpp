#include "Utils/FileUtils.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <codecvt>
#include <locale>

namespace Utils {

namespace fs = std::filesystem;

std::string FileUtils::ReadFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("无法打开文件: " + filename);
    }
    
    // 读取文件内容
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    
    // 检查BOM并移除
    if (content.size() >= 3 && 
        static_cast<unsigned char>(content[0]) == 0xEF &&
        static_cast<unsigned char>(content[1]) == 0xBB &&
        static_cast<unsigned char>(content[2]) == 0xBF) {
        content = content.substr(3);
    }
    
    return content;
}

bool FileUtils::WriteFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    // 写入UTF-8 BOM（可选）
    // file.write("\xEF\xBB\xBF", 3);
    
    file.write(content.c_str(), content.size());
    return file.good();
}

std::string FileUtils::GetFileExtension(const std::string& filename) {
    fs::path path(filename);
    return path.extension().string();
}

std::string FileUtils::GetDirectory(const std::string& filename) {
    fs::path path(filename);
    return path.parent_path().string();
}

std::string FileUtils::GetFilename(const std::string& path) {
    fs::path p(path);
    return p.filename().string();
}

std::string FileUtils::GetFilenameWithoutExtension(const std::string& path) {
    fs::path p(path);
    return p.stem().string();
}

std::string FileUtils::JoinPath(const std::string& path1, const std::string& path2) {
    fs::path p1(path1);
    fs::path p2(path2);
    return (p1 / p2).string();
}

bool FileUtils::FileExists(const std::string& filename) {
    return fs::exists(filename) && fs::is_regular_file(filename);
}

bool FileUtils::DirectoryExists(const std::string& dirname) {
    return fs::exists(dirname) && fs::is_directory(dirname);
}

bool FileUtils::CreateDirectory(const std::string& dirname) {
    try {
        return fs::create_directories(dirname);
    } catch (const std::exception&) {
        return false;
    }
}

std::vector<std::string> FileUtils::GetFilesInDirectory(const std::string& dirname, 
                                                        const std::string& extension) {
    std::vector<std::string> files;
    
    if (!DirectoryExists(dirname)) {
        return files;
    }
    
    for (const auto& entry : fs::directory_iterator(dirname)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().string();
            if (extension.empty() || GetFileExtension(filename) == extension) {
                files.push_back(filename);
            }
        }
    }
    
    return files;
}

std::string FileUtils::NormalizePath(const std::string& path) {
    fs::path p(path);
    return p.lexically_normal().string();
}

std::string FileUtils::GetAbsolutePath(const std::string& path) {
    fs::path p(path);
    return fs::absolute(p).string();
}

std::string FileUtils::GetRelativePath(const std::string& path, const std::string& base) {
    fs::path p(path);
    fs::path b(base);
    return fs::relative(p, b).string();
}

bool FileUtils::IsAbsolutePath(const std::string& path) {
    fs::path p(path);
    return p.is_absolute();
}

std::string FileUtils::ConvertDotToSlash(const std::string& path) {
    std::string result = path;
    std::replace(result.begin(), result.end(), '.', '/');
    return result;
}

} // namespace Utils