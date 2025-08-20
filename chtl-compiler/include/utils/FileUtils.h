#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

namespace chtl {
namespace utils {

class FileUtils {
public:
    static std::string ReadFile(const std::string& path) {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("无法打开文件: " + path);
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    static void WriteFile(const std::string& path, const std::string& content) {
        std::ofstream file(path, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("无法写入文件: " + path);
        }
        
        file << content;
        file.close();
    }

    static bool Exists(const std::string& path) {
        return fs::exists(path);
    }

    static bool IsDirectory(const std::string& path) {
        return fs::is_directory(path);
    }

    static bool IsFile(const std::string& path) {
        return fs::is_regular_file(path);
    }

    static std::string GetExtension(const std::string& path) {
        fs::path p(path);
        return p.extension().string();
    }

    static std::string GetFilename(const std::string& path) {
        fs::path p(path);
        return p.filename().string();
    }

    static std::string GetBasename(const std::string& path) {
        fs::path p(path);
        return p.stem().string();
    }

    static std::string GetDirectory(const std::string& path) {
        fs::path p(path);
        return p.parent_path().string();
    }

    static std::string GetAbsolutePath(const std::string& path) {
        return fs::absolute(path).string();
    }

    static std::string NormalizePath(const std::string& path) {
        return fs::path(path).lexically_normal().string();
    }

    static std::string JoinPath(const std::string& base, const std::string& relative) {
        return (fs::path(base) / relative).string();
    }

    static std::vector<std::string> ListFiles(const std::string& directory, 
                                            const std::string& extension = "") {
        std::vector<std::string> files;
        
        if (!IsDirectory(directory)) {
            return files;
        }
        
        for (const auto& entry : fs::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string path = entry.path().string();
                if (extension.empty() || GetExtension(path) == extension) {
                    files.push_back(path);
                }
            }
        }
        
        return files;
    }

    static void CreateDirectory(const std::string& path) {
        fs::create_directories(path);
    }

    static void CopyFile(const std::string& from, const std::string& to) {
        fs::copy_file(from, to, fs::copy_options::overwrite_existing);
    }

    static void RemoveFile(const std::string& path) {
        fs::remove(path);
    }

    static void RemoveDirectory(const std::string& path) {
        fs::remove_all(path);
    }
};

} // namespace utils
} // namespace chtl