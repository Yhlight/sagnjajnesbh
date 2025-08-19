#include "FileUtils.h"
#include <filesystem>
#include <sstream>
#include <iostream>

namespace chtl {
namespace utils {

bool FileUtils::readFile(const std::string& file_path, std::string& content) {
    try {
        std::ifstream file(file_path, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        std::ostringstream stream;
        stream << file.rdbuf();
        content = stream.str();
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

std::vector<std::string> FileUtils::readLines(const std::string& file_path) {
    std::vector<std::string> lines;
    
    try {
        std::ifstream file(file_path);
        if (!file.is_open()) {
            return lines;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            lines.push_back(line);
        }
    } catch (const std::exception&) {
        // 返回空vector
    }
    
    return lines;
}

bool FileUtils::writeFile(const std::string& file_path, const std::string& content) {
    try {
        std::ofstream file(file_path, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        file << content;
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool FileUtils::writeLines(const std::string& file_path, const std::vector<std::string>& lines) {
    try {
        std::ofstream file(file_path);
        if (!file.is_open()) {
            return false;
        }
        
        for (const auto& line : lines) {
            file << line << '\n';
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool FileUtils::fileExists(const std::string& file_path) {
    return std::filesystem::exists(file_path);
}

bool FileUtils::isDirectory(const std::string& path) {
    return std::filesystem::is_directory(path);
}

bool FileUtils::isFile(const std::string& path) {
    return std::filesystem::is_regular_file(path);
}

std::string FileUtils::getFileName(const std::string& file_path) {
    std::filesystem::path path(file_path);
    return path.filename().string();
}

std::string FileUtils::getFileExtension(const std::string& file_path) {
    std::filesystem::path path(file_path);
    return path.extension().string();
}

std::string FileUtils::getDirectory(const std::string& file_path) {
    std::filesystem::path path(file_path);
    return path.parent_path().string();
}

std::string FileUtils::joinPath(const std::string& dir, const std::string& file) {
    std::filesystem::path path = std::filesystem::path(dir) / file;
    return path.string();
}

bool FileUtils::createDirectory(const std::string& dir_path) {
    try {
        return std::filesystem::create_directories(dir_path);
    } catch (const std::exception&) {
        return false;
    }
}

bool FileUtils::removeFile(const std::string& file_path) {
    try {
        return std::filesystem::remove(file_path);
    } catch (const std::exception&) {
        return false;
    }
}

std::vector<std::string> FileUtils::listFiles(const std::string& dir_path) {
    std::vector<std::string> files;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(dir_path)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path().string());
            }
        }
    } catch (const std::exception&) {
        // 返回空vector
    }
    
    return files;
}

std::vector<std::string> FileUtils::listDirectories(const std::string& dir_path) {
    std::vector<std::string> directories;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(dir_path)) {
            if (entry.is_directory()) {
                directories.push_back(entry.path().string());
            }
        }
    } catch (const std::exception&) {
        // 返回空vector
    }
    
    return directories;
}

size_t FileUtils::getFileSize(const std::string& file_path) {
    try {
        return std::filesystem::file_size(file_path);
    } catch (const std::exception&) {
        return 0;
    }
}

std::string FileUtils::createTempFile(const std::string& prefix) {
    try {
        std::filesystem::path temp_dir = std::filesystem::temp_directory_path();
        std::string temp_file = prefix + std::to_string(std::time(nullptr));
        return (temp_dir / temp_file).string();
    } catch (const std::exception&) {
        return "";
    }
}

void FileUtils::cleanupTempFile(const std::string& file_path) {
    try {
        std::filesystem::remove(file_path);
    } catch (const std::exception&) {
        // 忽略清理错误
    }
}

} // namespace utils
} // namespace chtl