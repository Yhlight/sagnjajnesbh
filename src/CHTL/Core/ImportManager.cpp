#include "CHTL/Core/ImportManager.h"
#include "Utils/StringUtils.h"
#include "Utils/ErrorHandler.h"
#include <filesystem>
#include <algorithm>
#include <regex>

namespace CHTL {
namespace Core {

ImportManager::ImportManager() {
    // 设置默认的官方模块目录
    officialModuleDir_ = "./module";
}

void ImportManager::SetCurrentDirectory(const std::string& currentDir) {
    currentDir_ = NormalizePath(currentDir);
}

void ImportManager::SetOfficialModuleDirectory(const std::string& moduleDir) {
    officialModuleDir_ = NormalizePath(moduleDir);
}

ImportSearchResult ImportManager::ResolveImportPath(const std::string& importPath, AST::ImportNode::ImportType importType) {
    ImportSearchResult result;
    
    // 解析路径信息
    ImportPathInfo pathInfo = ParsePathInfo(importPath);
    
    // 根据导入类型选择搜索策略
    switch (importType) {
        // 旧版本媒体文件导入（保持兼容）
        case AST::ImportNode::ImportType::HTML:
        case AST::ImportNode::ImportType::STYLE:
        case AST::ImportNode::ImportType::JAVASCRIPT:
        // 新版本媒体文件导入（需要as语法）
        case AST::ImportNode::ImportType::MEDIA_HTML:
        case AST::ImportNode::ImportType::MEDIA_STYLE:
        case AST::ImportNode::ImportType::MEDIA_JAVASCRIPT:
            result = SearchMediaFiles(pathInfo, importType);
            break;
            
        case AST::ImportNode::ImportType::CHTL:
        case AST::ImportNode::ImportType::TEMPLATE_ELEMENT:
        case AST::ImportNode::ImportType::TEMPLATE_STYLE:
        case AST::ImportNode::ImportType::TEMPLATE_VAR:
        case AST::ImportNode::ImportType::CUSTOM_ELEMENT:
        case AST::ImportNode::ImportType::CUSTOM_STYLE:
        case AST::ImportNode::ImportType::CUSTOM_VAR:
        case AST::ImportNode::ImportType::CONFIG:
            result = SearchCHTLFiles(pathInfo);
            break;
            
        case AST::ImportNode::ImportType::CJMOD:
            result = SearchCJMODFiles(pathInfo);
            break;
            
        // 原始嵌入导入（包括自定义类型）
        case AST::ImportNode::ImportType::ORIGIN_HTML:
        case AST::ImportNode::ImportType::ORIGIN_STYLE:
        case AST::ImportNode::ImportType::ORIGIN_JAVASCRIPT:
        case AST::ImportNode::ImportType::ORIGIN_CUSTOM:
            result = SearchOriginFiles(pathInfo, importType);
            break;
            
        default:
            result.success = false;
            result.errorMessage = "不支持的导入类型";
            break;
    }
    
    return result;
}

bool ImportManager::CheckCircularDependency(const std::string& fromFile, const std::string& toFile) {
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> path;
    
    return DFSCheckCircular(NormalizePath(toFile), NormalizePath(fromFile), visited, path);
}

bool ImportManager::CheckDuplicateImport(const std::string& fromFile, const std::string& importPath, AST::ImportNode::ImportType importType) {
    std::string normalizedFrom = NormalizePath(fromFile);
    std::string key = importPath + "|" + std::to_string(static_cast<int>(importType));
    
    auto it = importHistory_.find(normalizedFrom);
    if (it != importHistory_.end()) {
        return it->second.find(key) != it->second.end();
    }
    
    return false;
}

void ImportManager::RegisterImport(const std::string& fromFile, const std::string& toFile, AST::ImportNode::ImportType importType) {
    std::string normalizedFrom = NormalizePath(fromFile);
    std::string normalizedTo = NormalizePath(toFile);
    
    // 注册依赖关系
    importGraph_[normalizedFrom].insert(normalizedTo);
    
    // 注册导入历史
    std::string key = toFile + "|" + std::to_string(static_cast<int>(importType));
    importHistory_[normalizedFrom].insert(key);
}

void ImportManager::Clear() {
    importGraph_.clear();
    importHistory_.clear();
}

ImportPathInfo ImportManager::ParsePathInfo(const std::string& path) {
    ImportPathInfo info;
    info.originalPath = path;
    
    // 替换'.'为'/'  - 目标规划.ini要求
    std::string normalizedPath = path;
    std::replace(normalizedPath.begin(), normalizedPath.end(), '.', '/');
    
    // 检查通配符
    if (normalizedPath.find("*") != std::string::npos || 
        normalizedPath.find("*.cmod") != std::string::npos ||
        normalizedPath.find("*.chtl") != std::string::npos) {
        info.isWildcard = true;
    }
    
    // 提取文件名和扩展名
    size_t lastSlash = normalizedPath.find_last_of('/');
    if (lastSlash != std::string::npos) {
        info.fileName = normalizedPath.substr(lastSlash + 1);
        info.resolvedPath = normalizedPath.substr(0, lastSlash);
    } else {
        info.fileName = normalizedPath;
        info.resolvedPath = "";
    }
    
    // 获取扩展名
    info.extension = GetFileExtension(info.fileName);
    
    return info;
}

ImportSearchResult ImportManager::SearchMediaFiles(const ImportPathInfo& pathInfo, AST::ImportNode::ImportType importType) {
    ImportSearchResult result;
    
    // 确定文件扩展名
    std::string expectedExt;
    switch (importType) {
        case AST::ImportNode::ImportType::HTML:
            expectedExt = ".html";
            break;
        case AST::ImportNode::ImportType::STYLE:
            expectedExt = ".css";
            break;
        case AST::ImportNode::ImportType::JAVASCRIPT:
            expectedExt = ".js";
            break;
        default:
            result.errorMessage = "无效的媒体文件类型";
            return result;
    }
    
    // 目标规划.ini第70-76行：媒体文件搜索策略
    std::vector<std::string> searchPaths;
    
    if (pathInfo.extension.empty()) {
        // 没有扩展名，在当前目录搜索
        std::string searchPattern = pathInfo.fileName + expectedExt;
        auto files = SearchFilesInDirectory(currentDir_, searchPattern);
        result.foundPaths.insert(result.foundPaths.end(), files.begin(), files.end());
    } else if (pathInfo.extension == expectedExt) {
        // 有具体扩展名，直接搜索
        std::string fullPath = currentDir_ + "/" + pathInfo.fileName;
        if (FileExists(fullPath)) {
            result.foundPaths.push_back(fullPath);
        }
    } else if (!pathInfo.resolvedPath.empty()) {
        // 具体路径
        std::string fullPath = pathInfo.resolvedPath + "/" + pathInfo.fileName;
        if (FileExists(fullPath)) {
            result.foundPaths.push_back(fullPath);
        } else {
            result.errorMessage = "文件不存在: " + fullPath;
            return result;
        }
    }
    
    result.success = !result.foundPaths.empty();
    if (!result.success && result.errorMessage.empty()) {
        result.errorMessage = "未找到匹配的" + expectedExt + "文件";
    }
    
    return result;
}

ImportSearchResult ImportManager::SearchCHTLFiles(const ImportPathInfo& pathInfo) {
    ImportSearchResult result;
    
    // 目标规划.ini第77-83行：CHTL文件搜索策略
    std::vector<std::string> searchDirs = {
        officialModuleDir_,                    // 官方模块目录
        currentDir_ + "/module",               // 当前目录的module文件夹
        currentDir_                            // 当前目录
    };
    
    if (pathInfo.isWildcard) {
        // 通配符搜索
        for (const auto& dir : searchDirs) {
            if (IsDirectory(dir)) {
                if (pathInfo.fileName == "*") {
                    // 搜索所有cmod和chtl文件
                    auto cmodFiles = SearchFilesInDirectory(dir, "*.cmod");
                    auto chtlFiles = SearchFilesInDirectory(dir, "*.chtl");
                    result.foundPaths.insert(result.foundPaths.end(), cmodFiles.begin(), cmodFiles.end());
                    result.foundPaths.insert(result.foundPaths.end(), chtlFiles.begin(), chtlFiles.end());
                } else if (pathInfo.fileName == "*.cmod") {
                    auto files = SearchFilesInDirectory(dir, "*.cmod");
                    result.foundPaths.insert(result.foundPaths.end(), files.begin(), files.end());
                } else if (pathInfo.fileName == "*.chtl") {
                    auto files = SearchFilesInDirectory(dir, "*.chtl");
                    result.foundPaths.insert(result.foundPaths.end(), files.begin(), files.end());
                }
            }
        }
    } else if (pathInfo.extension.empty()) {
        // 没有扩展名，搜索cmod和chtl文件，cmod优先
        for (const auto& dir : searchDirs) {
            std::string cmodPath = dir + "/" + pathInfo.fileName + ".cmod";
            std::string chtlPath = dir + "/" + pathInfo.fileName + ".chtl";
            
            if (FileExists(cmodPath)) {
                result.foundPaths.push_back(cmodPath);
                break; // cmod优先，找到就停止
            } else if (FileExists(chtlPath)) {
                result.foundPaths.push_back(chtlPath);
                break;
            }
        }
    } else {
        // 有具体扩展名
        for (const auto& dir : searchDirs) {
            std::string fullPath = dir + "/" + pathInfo.fileName;
            if (FileExists(fullPath)) {
                result.foundPaths.push_back(fullPath);
                break;
            }
        }
    }
    
    // 如果还没找到且有具体路径，直接搜索
    if (result.foundPaths.empty() && !pathInfo.resolvedPath.empty()) {
        std::string fullPath = pathInfo.resolvedPath + "/" + pathInfo.fileName;
        if (FileExists(fullPath)) {
            result.foundPaths.push_back(fullPath);
        } else {
            result.errorMessage = "文件不存在: " + fullPath;
            return result;
        }
    }
    
    result.success = !result.foundPaths.empty();
    if (!result.success && result.errorMessage.empty()) {
        result.errorMessage = "未找到匹配的CHTL文件";
    }
    
    return result;
}

ImportSearchResult ImportManager::SearchCJMODFiles(const ImportPathInfo& pathInfo) {
    ImportSearchResult result;
    
    // 目标规划.ini第84-89行：CJMOD文件搜索策略
    std::vector<std::string> searchDirs = {
        officialModuleDir_,                    // 官方模块目录
        currentDir_ + "/module",               // 当前目录的module文件夹
        currentDir_                            // 当前目录
    };
    
    if (pathInfo.extension.empty()) {
        // 没有扩展名，搜索cjmod文件
        for (const auto& dir : searchDirs) {
            std::string cjmodPath = dir + "/" + pathInfo.fileName + ".cjmod";
            if (FileExists(cjmodPath)) {
                result.foundPaths.push_back(cjmodPath);
                break;
            }
        }
    } else {
        // 有具体扩展名
        for (const auto& dir : searchDirs) {
            std::string fullPath = dir + "/" + pathInfo.fileName;
            if (FileExists(fullPath)) {
                result.foundPaths.push_back(fullPath);
                break;
            }
        }
    }
    
    // 如果还没找到且有具体路径，直接搜索
    if (result.foundPaths.empty() && !pathInfo.resolvedPath.empty()) {
        std::string fullPath = pathInfo.resolvedPath + "/" + pathInfo.fileName;
        if (FileExists(fullPath)) {
            result.foundPaths.push_back(fullPath);
        } else {
            result.errorMessage = "文件不存在: " + fullPath;
            return result;
        }
    }
    
    result.success = !result.foundPaths.empty();
    if (!result.success && result.errorMessage.empty()) {
        result.errorMessage = "未找到匹配的CJMOD文件";
    }
    
    return result;
}

ImportSearchResult ImportManager::SearchOriginFiles(const ImportPathInfo& pathInfo, AST::ImportNode::ImportType importType) {
    ImportSearchResult result;
    
    // 原始嵌入文件直接从指定路径导入，不进行复杂的搜索策略
    // 它们主要用于嵌入特定文件类型的内容
    
    std::string targetPath = pathInfo.fullPath;
    
    // 检查文件是否存在
    if (FileExists(targetPath)) {
        result.success = true;
        result.resolvedPath = NormalizePath(targetPath);
        result.isWildcard = false;
        result.foundFiles.push_back(result.resolvedPath);
        
        Utils::ErrorHandler::GetInstance().LogInfo(
            "找到原始嵌入文件: " + result.resolvedPath
        );
    } else {
        result.success = false;
        result.errorMessage = "原始嵌入文件不存在: " + targetPath;
    }
    
    return result;
}

std::vector<std::string> ImportManager::SearchFilesInDirectory(const std::string& directory, const std::string& pattern) {
    std::vector<std::string> foundFiles;
    
    try {
        if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) {
            return foundFiles;
        }
        
        // 将通配符模式转换为正则表达式
        std::string regexPattern = pattern;
        std::replace(regexPattern.begin(), regexPattern.end(), '*', '.');
        regexPattern = ".*" + regexPattern + ".*";
        
        std::regex fileRegex(regexPattern);
        
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string fileName = entry.path().filename().string();
                if (std::regex_match(fileName, fileRegex)) {
                    foundFiles.push_back(entry.path().string());
                }
            }
        }
    } catch (const std::exception& e) {
        Utils::ErrorHandler::GetInstance().LogError(
            "搜索文件时出错: " + std::string(e.what())
        );
    }
    
    return foundFiles;
}

bool ImportManager::FileExists(const std::string& filePath) {
    try {
        return std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath);
    } catch (...) {
        return false;
    }
}

bool ImportManager::IsDirectory(const std::string& path) {
    try {
        return std::filesystem::exists(path) && std::filesystem::is_directory(path);
    } catch (...) {
        return false;
    }
}

std::string ImportManager::GetFileExtension(const std::string& fileName) {
    size_t dotPos = fileName.find_last_of('.');
    if (dotPos != std::string::npos && dotPos != fileName.length() - 1) {
        return fileName.substr(dotPos);
    }
    return "";
}

std::string ImportManager::NormalizePath(const std::string& path) {
    try {
        return std::filesystem::canonical(std::filesystem::path(path)).string();
    } catch (...) {
        // 如果canonical失败，进行基本的标准化
        std::string normalized = path;
        std::replace(normalized.begin(), normalized.end(), '\\', '/');
        return normalized;
    }
}

bool ImportManager::DFSCheckCircular(const std::string& current, const std::string& target, 
                                     std::unordered_set<std::string>& visited, 
                                     std::unordered_set<std::string>& path) {
    if (path.find(current) != path.end()) {
        // 发现循环
        return true;
    }
    
    if (visited.find(current) != visited.end()) {
        // 已访问过且无循环
        return false;
    }
    
    if (current == target) {
        // 找到目标，存在循环依赖
        return true;
    }
    
    visited.insert(current);
    path.insert(current);
    
    auto it = importGraph_.find(current);
    if (it != importGraph_.end()) {
        for (const auto& dependency : it->second) {
            if (DFSCheckCircular(dependency, target, visited, path)) {
                return true;
            }
        }
    }
    
    path.erase(current);
    return false;
}

} // namespace Core
} // namespace CHTL