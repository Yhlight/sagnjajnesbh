#include "CHTL/Import/EnhancedImportSystem.h"
#include "CHTL/AST/CHTLASTNodes.h"
#include "Utils/FileUtils.h"
#include "Utils/StringUtils.h"
#include "Utils/ErrorHandler.h"
#include <filesystem>
#include <regex>
#include <algorithm>

namespace CHTL {
namespace Import {

// PathResolver实现
PathResolver::PathResolver(const std::string& currentFilePath, const std::string& officialModulePath)
    : currentFilePath_(currentFilePath), officialModulePath_(officialModulePath) {
    
    // 提取当前目录
    size_t lastSlash = currentFilePath_.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        currentDirectory_ = currentFilePath_.substr(0, lastSlash);
    } else {
        currentDirectory_ = ".";
    }
}

std::vector<std::string> PathResolver::ResolvePath(const ImportInfo& importInfo) {
    switch (importInfo.type) {
        case ImportType::HTML:
        case ImportType::STYLE:
        case ImportType::JAVASCRIPT:
            return ResolveWebAssetPath(importInfo);
            
        case ImportType::CHTL:
            return ResolveChtlPath(importInfo);
            
        case ImportType::CJMOD:
            return ResolveCJmodPath(importInfo);
            
        case ImportType::ORIGIN_CUSTOM:
            // [Origin] @CustomType的处理
            return {importInfo.path}; // 直接使用提供的路径
            
        default:
            Utils::ErrorHandler::GetInstance().LogError(
                "不支持的导入类型: " + std::to_string(static_cast<int>(importInfo.type))
            );
            return {};
    }
}

std::vector<std::string> PathResolver::ResolveWebAssetPath(const ImportInfo& importInfo) {
    std::vector<std::string> results;
    
    switch (importInfo.pathType) {
        case PathType::NAME_ONLY: {
            // 文件名（不带后缀）：在编译文件所在目录（非递归）按类型搜索相关文件
            std::string extension;
            switch (importInfo.type) {
                case ImportType::HTML:       extension = ".html"; break;
                case ImportType::STYLE:      extension = ".css"; break;
                case ImportType::JAVASCRIPT: extension = ".js"; break;
                default: break;
            }
            
            std::string filePath = currentDirectory_ + "/" + importInfo.path + extension;
            if (FileExists(filePath)) {
                results.push_back(filePath);
            }
            break;
        }
        
        case PathType::SPECIFIC_FILE: {
            // 具体文件名（带后缀）：在编译文件所在目录（非递归）直接搜索该文件
            std::string filePath = currentDirectory_ + "/" + importInfo.path;
            if (FileExists(filePath)) {
                results.push_back(filePath);
            }
            break;
        }
        
        case PathType::SPECIFIC_PATH: {
            // 具体路径（含文件信息）：直接按路径查找
            if (FileExists(importInfo.path)) {
                results.push_back(importInfo.path);
            } else {
                Utils::ErrorHandler::GetInstance().LogError(
                    "文件未找到: " + importInfo.path
                );
            }
            break;
        }
        
        case PathType::DIRECTORY_PATH: {
            // 目录路径（不含文件信息）：触发报错
            Utils::ErrorHandler::GetInstance().LogError(
                "Web资源导入不支持目录路径: " + importInfo.path
            );
            break;
        }
        
        default:
            Utils::ErrorHandler::GetInstance().LogError(
                "Web资源导入不支持的路径类型"
            );
            break;
    }
    
    return results;
}

std::vector<std::string> PathResolver::ResolveChtlPath(const ImportInfo& importInfo) {
    std::vector<std::string> results;
    
    // 检查是否使用官方模块前缀
    if (importInfo.useOfficialPrefix) {
        // 直接在官方模块目录中搜索
        std::string officialPath = officialModulePath_ + "/" + importInfo.path;
        if (FileExists(officialPath)) {
            results.push_back(officialPath);
        }
        return results;
    }
    
    switch (importInfo.pathType) {
        case PathType::NAME_ONLY: {
            // 名称（不带后缀）：官方模块目录→当前目录module文件夹→当前目录
            // 优先匹配cmod文件，其次chtl，不匹配cjmod文件
            std::vector<std::string> extensions = {".cmod", ".chtl"};
            
            // 1. 搜索官方模块目录
            std::string found = SearchOfficialModules(importInfo.path, extensions);
            if (!found.empty()) {
                results.push_back(found);
                break;
            }
            
            // 2. 搜索当前目录的module文件夹
            found = SearchCurrentModules(importInfo.path, extensions);
            if (!found.empty()) {
                results.push_back(found);
                break;
            }
            
            // 3. 搜索当前目录
            found = SearchCurrentDirectory(importInfo.path, extensions);
            if (!found.empty()) {
                results.push_back(found);
            }
            break;
        }
        
        case PathType::SPECIFIC_FILE: {
            // 具体名称（带后缀）：按官方模块目录→当前目录module文件夹→当前目录顺序搜索
            std::vector<std::string> searchPaths = {
                officialModulePath_ + "/" + importInfo.path,
                currentDirectory_ + "/module/" + importInfo.path,
                currentDirectory_ + "/" + importInfo.path
            };
            
            for (const auto& path : searchPaths) {
                if (FileExists(path)) {
                    results.push_back(path);
                    break;
                }
            }
            break;
        }
        
        case PathType::SPECIFIC_PATH: {
            // 具体路径（含文件信息）：直接按路径查找，未找到则报错
            if (FileExists(importInfo.path)) {
                results.push_back(importInfo.path);
            } else {
                Utils::ErrorHandler::GetInstance().LogError(
                    "CHTL文件未找到: " + importInfo.path
                );
            }
            break;
        }
        
        case PathType::DIRECTORY_PATH: {
            // 具体路径（不含文件信息）：触发报错
            Utils::ErrorHandler::GetInstance().LogError(
                "CHTL导入不支持目录路径: " + importInfo.path
            );
            break;
        }
        
        case PathType::WILDCARD_ALL:
        case PathType::WILDCARD_CMOD:
        case PathType::WILDCARD_CHTL:
            return ResolveWildcardPath(importInfo);
            
        case PathType::SUBMODULE_ALL:
        case PathType::SUBMODULE_SPECIFIC:
            return ResolveSubmodulePath(importInfo);
            
        default:
            break;
    }
    
    return results;
}

std::vector<std::string> PathResolver::ResolveCJmodPath(const ImportInfo& importInfo) {
    std::vector<std::string> results;
    
    // 检查是否使用官方模块前缀
    if (importInfo.useOfficialPrefix) {
        // 直接在官方模块目录中搜索
        std::string officialPath = officialModulePath_ + "/" + importInfo.path + ".cjmod";
        if (FileExists(officialPath)) {
            results.push_back(officialPath);
        }
        return results;
    }
    
    switch (importInfo.pathType) {
        case PathType::NAME_ONLY: {
            // 名称（不带后缀）：官方模块目录→当前目录module文件夹→当前目录，仅匹配cjmod文件
            std::vector<std::string> extensions = {".cjmod"};
            
            // 1. 搜索官方模块目录
            std::string found = SearchOfficialModules(importInfo.path, extensions);
            if (!found.empty()) {
                results.push_back(found);
                break;
            }
            
            // 2. 搜索当前目录的module文件夹
            found = SearchCurrentModules(importInfo.path, extensions);
            if (!found.empty()) {
                results.push_back(found);
                break;
            }
            
            // 3. 搜索当前目录
            found = SearchCurrentDirectory(importInfo.path, extensions);
            if (!found.empty()) {
                results.push_back(found);
            }
            break;
        }
        
        case PathType::SPECIFIC_FILE: {
            // 具体名称（带后缀）：按官方模块目录→当前目录module文件夹→当前目录顺序搜索
            std::vector<std::string> searchPaths = {
                officialModulePath_ + "/" + importInfo.path,
                currentDirectory_ + "/module/" + importInfo.path,
                currentDirectory_ + "/" + importInfo.path
            };
            
            for (const auto& path : searchPaths) {
                if (FileExists(path)) {
                    results.push_back(path);
                    break;
                }
            }
            break;
        }
        
        case PathType::SPECIFIC_PATH: {
            // 具体路径（含文件信息）：直接按路径查找，未找到则报错
            if (FileExists(importInfo.path)) {
                results.push_back(importInfo.path);
            } else {
                Utils::ErrorHandler::GetInstance().LogError(
                    "CJmod文件未找到: " + importInfo.path
                );
            }
            break;
        }
        
        case PathType::DIRECTORY_PATH: {
            // 具体路径（不含文件信息）：触发报错
            Utils::ErrorHandler::GetInstance().LogError(
                "CJmod导入不支持目录路径: " + importInfo.path
            );
            break;
        }
        
        default:
            break;
    }
    
    return results;
}

std::vector<std::string> PathResolver::ResolveWildcardPath(const ImportInfo& importInfo) {
    std::vector<std::string> results;
    
    // 解析通配符路径：具体路径.*、具体路径.*.cmod、具体路径.*.chtl
    // 等价于：具体路径/*、具体路径/*.cmod、具体路径/*.chtl
    
    std::string basePath;
    std::vector<std::string> extensions;
    
    // 提取基础路径
    size_t wildcardPos = importInfo.path.find(".*");
    if (wildcardPos == std::string::npos) {
        wildcardPos = importInfo.path.find("/*");
    }
    
    if (wildcardPos != std::string::npos) {
        basePath = importInfo.path.substr(0, wildcardPos);
    } else {
        basePath = importInfo.path;
    }
    
    // 确定扩展名
    switch (importInfo.pathType) {
        case PathType::WILDCARD_ALL:
            extensions = {".cmod", ".chtl"};
            break;
        case PathType::WILDCARD_CMOD:
            extensions = {".cmod"};
            break;
        case PathType::WILDCARD_CHTL:
            extensions = {".chtl"};
            break;
        default:
            break;
    }
    
    // 搜索文件
    for (const auto& ext : extensions) {
        auto files = GetDirectoryFiles(basePath, ext);
        results.insert(results.end(), files.begin(), files.end());
    }
    
    return results;
}

std::vector<std::string> PathResolver::ResolveSubmodulePath(const ImportInfo& importInfo) {
    std::vector<std::string> results;
    
    // 解析子模块路径：Module.*或Module.SubModule
    size_t dotPos = importInfo.path.find('.');
    if (dotPos == std::string::npos) {
        return results;
    }
    
    std::string moduleName = importInfo.path.substr(0, dotPos);
    std::string submodulePart = importInfo.path.substr(dotPos + 1);
    
    if (submodulePart == "*") {
        // 导入模块的所有子模块
        std::string moduleDir = currentDirectory_ + "/module/" + moduleName;
        auto files = GetDirectoryFiles(moduleDir, ".cmod");
        auto chtlFiles = GetDirectoryFiles(moduleDir, ".chtl");
        results.insert(results.end(), files.begin(), files.end());
        results.insert(results.end(), chtlFiles.begin(), chtlFiles.end());
    } else {
        // 导入特定子模块
        std::vector<std::string> extensions = {".cmod", ".chtl"};
        std::string submodulePath = currentDirectory_ + "/module/" + moduleName + "/" + submodulePart;
        
        for (const auto& ext : extensions) {
            std::string fullPath = submodulePath + ext;
            if (FileExists(fullPath)) {
                results.push_back(fullPath);
                break;
            }
        }
    }
    
    return results;
}

std::string PathResolver::SearchOfficialModules(const std::string& fileName, 
                                               const std::vector<std::string>& extensions) {
    ModuleDirectoryManager moduleManager(officialModulePath_);
    
    // 检查是否使用分类结构
    if (moduleManager.IsClassifiedStructure(officialModulePath_)) {
        return moduleManager.SearchInClassifiedStructure(officialModulePath_, fileName, extensions);
    } else {
        return moduleManager.SearchInMixedStructure(officialModulePath_, fileName, extensions);
    }
}

std::string PathResolver::SearchCurrentModules(const std::string& fileName, 
                                              const std::vector<std::string>& extensions) {
    std::string moduleDir = currentDirectory_ + "/module";
    ModuleDirectoryManager moduleManager(moduleDir);
    
    // 检查是否使用分类结构
    if (moduleManager.IsClassifiedStructure(moduleDir)) {
        return moduleManager.SearchInClassifiedStructure(moduleDir, fileName, extensions);
    } else {
        return moduleManager.SearchInMixedStructure(moduleDir, fileName, extensions);
    }
}

std::string PathResolver::SearchCurrentDirectory(const std::string& fileName, 
                                                const std::vector<std::string>& extensions) {
    for (const auto& ext : extensions) {
        std::string fullPath = currentDirectory_ + "/" + fileName + ext;
        if (FileExists(fullPath)) {
            return fullPath;
        }
    }
    
    return "";
}

bool PathResolver::FileExists(const std::string& filePath) const {
    return Utils::FileUtils::FileExists(filePath);
}

std::vector<std::string> PathResolver::GetDirectoryFiles(const std::string& directory, 
                                                        const std::string& extension) const {
    std::vector<std::string> files;
    
    try {
        if (std::filesystem::exists(directory) && std::filesystem::is_directory(directory)) {
            for (const auto& entry : std::filesystem::directory_iterator(directory)) {
                if (entry.is_regular_file()) {
                    std::string filePath = entry.path().string();
                    if (extension.empty() || 
                        (filePath.length() >= extension.length() && 
                         filePath.compare(filePath.length() - extension.length(), extension.length(), extension) == 0)) {
                        files.push_back(filePath);
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        Utils::ErrorHandler::GetInstance().LogError(
            "读取目录失败: " + directory + ", 错误: " + e.what()
        );
    }
    
    return files;
}

// ImportConflictDetector实现
ImportConflictDetector::ImportConflictDetector() {}

bool ImportConflictDetector::CheckCircularDependency(const std::string& currentFile, const std::string& importPath) {
    std::string normalizedCurrent = NormalizePath(currentFile);
    std::string normalizedImport = NormalizePath(importPath);
    
    // 检查是否会形成循环
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> recursionStack;
    
    if (DFSCheckCycle(normalizedImport, normalizedCurrent, visited, recursionStack)) {
        Utils::ErrorHandler::GetInstance().LogError(
            "检测到循环依赖: " + currentFile + " <-> " + importPath
        );
        return true;
    }
    
    return false;
}

bool ImportConflictDetector::CheckDuplicateImport(const std::string& importPath) {
    std::string normalizedPath = NormalizePath(importPath);
    
    if (importedPaths_.find(normalizedPath) != importedPaths_.end()) {
        Utils::ErrorHandler::GetInstance().LogWarning(
            "重复导入: " + importPath
        );
        return true;
    }
    
    return false;
}

std::string ImportConflictDetector::NormalizePath(const std::string& path) {
    // 检查缓存
    auto it = pathNormalizationCache_.find(path);
    if (it != pathNormalizationCache_.end()) {
        return it->second;
    }
    
    std::string normalized = path;
    
    // 规范化路径分隔符
    std::replace(normalized.begin(), normalized.end(), '\\', '/');
    
    // 移除重复的斜杠
    std::regex multiSlashRegex(R"(/+)");
    normalized = std::regex_replace(normalized, multiSlashRegex, "/");
    
    // 解析相对路径
    std::regex dotSlashRegex(R"(\./+)");
    normalized = std::regex_replace(normalized, dotSlashRegex, "");
    
    // 处理../路径
    std::vector<std::string> parts = Utils::StringUtils::Split(normalized, "/");
    std::vector<std::string> resolvedParts;
    
    for (const auto& part : parts) {
        if (part == "..") {
            if (!resolvedParts.empty()) {
                resolvedParts.pop_back();
            }
        } else if (!part.empty() && part != ".") {
            resolvedParts.push_back(part);
        }
    }
    
    normalized = Utils::StringUtils::Join(resolvedParts, "/");
    
    // 缓存结果
    pathNormalizationCache_[path] = normalized;
    
    return normalized;
}

void ImportConflictDetector::AddImportRecord(const std::string& currentFile, const std::string& importPath) {
    std::string normalizedCurrent = NormalizePath(currentFile);
    std::string normalizedImport = NormalizePath(importPath);
    
    dependencyGraph_[normalizedCurrent].insert(normalizedImport);
    importedPaths_.insert(normalizedImport);
}

void ImportConflictDetector::Reset() {
    dependencyGraph_.clear();
    importedPaths_.clear();
    pathNormalizationCache_.clear();
}

bool ImportConflictDetector::DFSCheckCycle(const std::string& current, const std::string& target,
                                          std::unordered_set<std::string>& visited,
                                          std::unordered_set<std::string>& recursionStack) {
    visited.insert(current);
    recursionStack.insert(current);
    
    auto it = dependencyGraph_.find(current);
    if (it != dependencyGraph_.end()) {
        for (const auto& neighbor : it->second) {
            if (neighbor == target) {
                return true; // 找到循环
            }
            
            if (recursionStack.find(neighbor) != recursionStack.end()) {
                return true; // 在递归栈中，存在循环
            }
            
            if (visited.find(neighbor) == visited.end()) {
                if (DFSCheckCycle(neighbor, target, visited, recursionStack)) {
                    return true;
                }
            }
        }
    }
    
    recursionStack.erase(current);
    return false;
}

// ImportParser实现
ImportParser::ImportParser() {}

ImportInfo ImportParser::ParseImportStatement(const std::string& importStatement) {
    ImportInfo info;
    
    // 解析导入语句：[Import] @Type from "path" as name
    std::regex importRegex(R"(\[Import\]\s*(@?\w+|\[Origin\]\s*@\w+)\s+from\s+["\']([^"\']+)["\'](?:\s+as\s+(\w+))?(?:\s*\{\s*import:\s*\[([^\]]+)\]\s*\})?)", std::regex::icase);
    std::smatch match;
    
    if (std::regex_search(importStatement, match, importRegex)) {
        // 解析导入类型
        std::string typeStr = match[1].str();
        info.type = GetImportType(typeStr);
        
        // 解析路径
        info.path = match[2].str();
        info.originalPath = info.path;
        info.pathType = GetPathType(info.path);
        
        // 解析as子句
        if (match[3].matched) {
            info.asName = match[3].str();
            info.hasAsClause = true;
        } else {
            info.hasAsClause = false;
        }
        
        // 解析import列表（对于@Chtl）
        if (match[4].matched) {
            std::string importListStr = match[4].str();
            info.importList = ParseImportList(importListStr);
        }
        
        // 检查是否使用官方模块前缀
        info.useOfficialPrefix = (info.path.find("chtl::") == 0);
        if (info.useOfficialPrefix) {
            info.path = info.path.substr(6); // 移除"chtl::"前缀
        }
    } else {
        info.type = ImportType::UNKNOWN;
        Utils::ErrorHandler::GetInstance().LogError(
            "无法解析导入语句: " + importStatement
        );
    }
    
    return info;
}

ImportType ImportParser::GetImportType(const std::string& typeStr) {
    if (typeStr == "@Html") {
        return ImportType::HTML;
    } else if (typeStr == "@Style") {
        return ImportType::STYLE;
    } else if (typeStr == "@JavaScript") {
        return ImportType::JAVASCRIPT;
    } else if (typeStr == "@Chtl") {
        return ImportType::CHTL;
    } else if (typeStr == "@CJmod") {
        return ImportType::CJMOD;
    } else if (typeStr.find("[Origin]") != std::string::npos) {
        return ImportType::ORIGIN_CUSTOM;
    } else {
        return ImportType::UNKNOWN;
    }
}

PathType ImportParser::GetPathType(const std::string& path) {
    // 检查通配符
    if (path.find(".*") != std::string::npos || path.find("/*") != std::string::npos) {
        if (path.find(".*.cmod") != std::string::npos || path.find("/*.cmod") != std::string::npos) {
            return PathType::WILDCARD_CMOD;
        } else if (path.find(".*.chtl") != std::string::npos || path.find("/*.chtl") != std::string::npos) {
            return PathType::WILDCARD_CHTL;
        } else {
            return PathType::WILDCARD_ALL;
        }
    }
    
    // 检查子模块
    size_t dotPos = path.find('.');
    if (dotPos != std::string::npos && dotPos > 0) {
        std::string afterDot = path.substr(dotPos + 1);
        if (afterDot == "*") {
            return PathType::SUBMODULE_ALL;
        } else if (!afterDot.empty() && afterDot.find('/') == std::string::npos) {
            return PathType::SUBMODULE_SPECIFIC;
        }
    }
    
    // 检查路径类型
    if (path.find('/') != std::string::npos || path.find('\\') != std::string::npos) {
        // 包含路径分隔符
        size_t lastSlash = std::max(path.find_last_of('/'), path.find_last_of('\\'));
        if (lastSlash != std::string::npos && lastSlash < path.length() - 1) {
            // 路径后面还有文件名
            return PathType::SPECIFIC_PATH;
        } else {
            // 路径后面没有文件名
            return PathType::DIRECTORY_PATH;
        }
    } else {
        // 不包含路径分隔符
        if (path.find('.') != std::string::npos && path.find('.') > 0) {
            // 包含扩展名
            return PathType::SPECIFIC_FILE;
        } else {
            // 仅文件名
            return PathType::NAME_ONLY;
        }
    }
}

std::vector<std::string> ImportParser::ParseImportList(const std::string& importListStr) {
    std::vector<std::string> importList;
    
    // 解析import列表：[item1, item2, item3]
    std::string cleanList = importListStr;
    cleanList = Utils::StringUtils::Trim(cleanList);
    
    // 移除方括号
    if (cleanList.front() == '[') cleanList = cleanList.substr(1);
    if (cleanList.back() == ']') cleanList = cleanList.substr(0, cleanList.length() - 1);
    
    // 按逗号分割
    auto items = Utils::StringUtils::Split(cleanList, ",");
    for (const auto& item : items) {
        std::string trimmedItem = Utils::StringUtils::Trim(item);
        if (!trimmedItem.empty()) {
            importList.push_back(trimmedItem);
        }
    }
    
    return importList;
}

// ModuleDirectoryManager实现
ModuleDirectoryManager::ModuleDirectoryManager(const std::string& officialModulePath)
    : officialModulePath_(officialModulePath) {}

bool ModuleDirectoryManager::IsClassifiedStructure(const std::string& directory) const {
    // 检查是否存在CMOD和CJMOD子目录
    std::string cmodDir = directory + "/CMOD";
    std::string cjmodDir = directory + "/CJMOD";
    
    return std::filesystem::exists(cmodDir) && std::filesystem::is_directory(cmodDir) &&
           std::filesystem::exists(cjmodDir) && std::filesystem::is_directory(cjmodDir);
}

std::string ModuleDirectoryManager::SearchInClassifiedStructure(const std::string& directory,
                                                               const std::string& fileName,
                                                               const std::vector<std::string>& extensions) {
    // 在分类结构中搜索：CMOD文件夹包含.chtl和.cmod文件，CJMOD文件夹包含.cjmod文件
    
    for (const auto& ext : extensions) {
        std::string filePath;
        
        if (ext == ".cmod" || ext == ".chtl") {
            // 在CMOD文件夹中搜索
            filePath = directory + "/CMOD/" + fileName + ext;
        } else if (ext == ".cjmod") {
            // 在CJMOD文件夹中搜索
            filePath = directory + "/CJMOD/" + fileName + ext;
        }
        
        if (!filePath.empty() && std::filesystem::exists(filePath)) {
            return filePath;
        }
    }
    
    return "";
}

std::string ModuleDirectoryManager::SearchInMixedStructure(const std::string& directory,
                                                          const std::string& fileName,
                                                          const std::vector<std::string>& extensions) {
    // 在混杂结构中搜索：所有文件都在同一个目录中
    
    for (const auto& ext : extensions) {
        std::string filePath = directory + "/" + fileName + ext;
        if (std::filesystem::exists(filePath)) {
            return filePath;
        }
    }
    
    return "";
}

// EnhancedImportSystem实现
EnhancedImportSystem::EnhancedImportSystem(const std::string& currentFilePath, 
                                         const std::string& officialModulePath)
    : pathResolver_(currentFilePath, officialModulePath)
    , currentFilePath_(currentFilePath)
    , disableDefaultNamespace_(false)
    , htmlImportCount_(0), styleImportCount_(0), jsImportCount_(0)
    , chtlImportCount_(0), cjmodImportCount_(0), originImportCount_(0)
    , duplicateImportCount_(0), circularDependencyCount_(0) {}

std::vector<std::shared_ptr<AST::ASTNode>> EnhancedImportSystem::ProcessImport(const std::string& importStatement) {
    std::vector<std::shared_ptr<AST::ASTNode>> results;
    
    // 解析导入语句
    ImportInfo importInfo = parser_.ParseImportStatement(importStatement);
    
    if (importInfo.type == ImportType::UNKNOWN) {
        return results;
    }
    
    // 解析路径
    auto resolvedPaths = pathResolver_.ResolvePath(importInfo);
    
    if (resolvedPaths.empty()) {
        Utils::ErrorHandler::GetInstance().LogError(
            "无法解析导入路径: " + importInfo.originalPath
        );
        return results;
    }
    
    // 检查冲突
    for (const auto& path : resolvedPaths) {
        if (conflictDetector_.CheckCircularDependency(currentFilePath_, path)) {
            circularDependencyCount_++;
            continue; // 跳过循环依赖的文件
        }
        
        if (conflictDetector_.CheckDuplicateImport(path)) {
            duplicateImportCount_++;
            continue; // 跳过重复导入的文件
        }
        
        conflictDetector_.AddImportRecord(currentFilePath_, path);
    }
    
    // 根据导入类型处理
    switch (importInfo.type) {
        case ImportType::HTML:
        case ImportType::STYLE:
        case ImportType::JAVASCRIPT:
            results = ProcessWebAssetImport(importInfo);
            break;
            
        case ImportType::CHTL:
            results = ProcessChtlImport(importInfo);
            break;
            
        case ImportType::CJMOD:
            results = ProcessCJmodImport(importInfo);
            break;
            
        case ImportType::ORIGIN_CUSTOM:
            results = ProcessOriginImport(importInfo);
            break;
            
        default:
            break;
    }
    
    return results;
}

std::vector<std::shared_ptr<AST::ASTNode>> EnhancedImportSystem::ProcessWebAssetImport(const ImportInfo& importInfo) {
    std::vector<std::shared_ptr<AST::ASTNode>> results;
    
    // 对于@Html，@Style，@JavaScript
    // 如果没有使用as语法，则直接跳过
    if (!importInfo.hasAsClause) {
        Utils::ErrorHandler::GetInstance().LogInfo(
            "Web资源导入没有as子句，跳过: " + importInfo.originalPath
        );
        return results;
    }
    
    // 如果使用as语法，则创建相应类型的带名原始嵌入节点
    auto resolvedPaths = pathResolver_.ResolvePath(importInfo);
    
    for (const auto& path : resolvedPaths) {
        std::string content = ReadFileContent(path);
        if (!content.empty()) {
            auto originNode = CreateNamedOriginNode(importInfo.type, content, importInfo.asName);
            if (originNode) {
                results.push_back(originNode);
                
                // 更新统计
                switch (importInfo.type) {
                    case ImportType::HTML:       htmlImportCount_++; break;
                    case ImportType::STYLE:      styleImportCount_++; break;
                    case ImportType::JAVASCRIPT: jsImportCount_++; break;
                    default: break;
                }
            }
        }
    }
    
    return results;
}

std::vector<std::shared_ptr<AST::ASTNode>> EnhancedImportSystem::ProcessChtlImport(const ImportInfo& importInfo) {
    std::vector<std::shared_ptr<AST::ASTNode>> results;
    
    // 处理@Chtl导入
    auto resolvedPaths = pathResolver_.ResolvePath(importInfo);
    
    for (const auto& path : resolvedPaths) {
        // 这里应该解析CHTL文件并创建相应的AST节点
        // 简化实现：创建导入节点
        auto importNode = std::make_shared<AST::ImportNode>(
            AST::ImportNode::ImportType::CHTL, 
            path, 
            "", 
            "", 
            Core::CHTLToken()
        );
        
        // 注意：ImportNode可能没有AddImportItem方法，这里只做日志记录
        Utils::ErrorHandler::GetInstance().LogInfo(
            "处理CHTL导入: " + path + "，导入项数量: " + std::to_string(importInfo.importList.size())
        );
        
        results.push_back(importNode);
        chtlImportCount_++;
    }
    
    return results;
}

std::vector<std::shared_ptr<AST::ASTNode>> EnhancedImportSystem::ProcessCJmodImport(const ImportInfo& importInfo) {
    std::vector<std::shared_ptr<AST::ASTNode>> results;
    
    // 处理@CJmod导入
    auto resolvedPaths = pathResolver_.ResolvePath(importInfo);
    
    for (const auto& path : resolvedPaths) {
        // 创建CJmod导入节点
        auto importNode = std::make_shared<AST::ImportNode>(
            AST::ImportNode::ImportType::CJMOD, 
            path, 
            "", 
            "", 
            Core::CHTLToken()
        );
        
        results.push_back(importNode);
        cjmodImportCount_++;
    }
    
    return results;
}

std::vector<std::shared_ptr<AST::ASTNode>> EnhancedImportSystem::ProcessOriginImport(const ImportInfo& importInfo) {
    std::vector<std::shared_ptr<AST::ASTNode>> results;
    
    // 处理[Origin] @CustomType导入
    auto resolvedPaths = pathResolver_.ResolvePath(importInfo);
    
    for (const auto& path : resolvedPaths) {
        std::string content = ReadFileContent(path);
        if (!content.empty()) {
            auto originNode = std::make_shared<AST::OriginNode>(
                AST::OriginNode::OriginType::CUSTOM, 
                content, 
                importInfo.asName.empty() ? "CustomOrigin" : importInfo.asName
            );
            
            results.push_back(originNode);
            originImportCount_++;
        }
    }
    
    return results;
}

std::shared_ptr<AST::OriginNode> EnhancedImportSystem::CreateNamedOriginNode(ImportType type,
                                                                            const std::string& content,
                                                                            const std::string& name) {
    AST::OriginNode::OriginType originType;
    
    switch (type) {
        case ImportType::HTML:
            originType = AST::OriginNode::OriginType::HTML;
            break;
        case ImportType::STYLE:
            originType = AST::OriginNode::OriginType::STYLE;
            break;
        case ImportType::JAVASCRIPT:
            originType = AST::OriginNode::OriginType::JAVASCRIPT;
            break;
        default:
            originType = AST::OriginNode::OriginType::CUSTOM;
            break;
    }
    
    return std::make_shared<AST::OriginNode>(originType, content, name);
}

std::string EnhancedImportSystem::ReadFileContent(const std::string& filePath) {
    try {
        return Utils::FileUtils::ReadFile(filePath);
    } catch (const std::exception& e) {
        Utils::ErrorHandler::GetInstance().LogError(
            "读取文件失败: " + filePath + ", 错误: " + e.what()
        );
        return "";
    }
}

std::string EnhancedImportSystem::CreateDefaultNamespace(const std::string& fileName) {
    if (disableDefaultNamespace_) {
        return "";
    }
    
    // 提取文件名（不含扩展名）作为默认命名空间
    size_t lastSlash = fileName.find_last_of("/\\");
    std::string baseName = (lastSlash != std::string::npos) ? fileName.substr(lastSlash + 1) : fileName;
    
    size_t lastDot = baseName.find_last_of('.');
    if (lastDot != std::string::npos) {
        baseName = baseName.substr(0, lastDot);
    }
    
    return baseName;
}

void EnhancedImportSystem::SetConfig(bool disableDefaultNamespace) {
    disableDefaultNamespace_ = disableDefaultNamespace;
}

std::string EnhancedImportSystem::GetStatistics() const {
    std::ostringstream stats;
    stats << "增强导入系统统计:\n";
    stats << "  HTML导入: " << htmlImportCount_ << "\n";
    stats << "  样式导入: " << styleImportCount_ << "\n";
    stats << "  JavaScript导入: " << jsImportCount_ << "\n";
    stats << "  CHTL导入: " << chtlImportCount_ << "\n";
    stats << "  CJmod导入: " << cjmodImportCount_ << "\n";
    stats << "  原始嵌入导入: " << originImportCount_ << "\n";
    stats << "  重复导入: " << duplicateImportCount_ << "\n";
    stats << "  循环依赖: " << circularDependencyCount_ << "\n";
    stats << "  配置状态:\n";
    stats << "    禁用默认命名空间: " << (disableDefaultNamespace_ ? "是" : "否") << "\n";
    
    return stats.str();
}

void EnhancedImportSystem::Reset() {
    conflictDetector_.Reset();
    htmlImportCount_ = 0;
    styleImportCount_ = 0;
    jsImportCount_ = 0;
    chtlImportCount_ = 0;
    cjmodImportCount_ = 0;
    originImportCount_ = 0;
    duplicateImportCount_ = 0;
    circularDependencyCount_ = 0;
}

} // namespace Import
} // namespace CHTL