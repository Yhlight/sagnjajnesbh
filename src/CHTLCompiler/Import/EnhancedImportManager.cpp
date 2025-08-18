#include "EnhancedImportManager.h"
#include <iostream>
#include <filesystem>
#include <regex>
#include <algorithm>

namespace chtl {
namespace import {

EnhancedImportManager::EnhancedImportManager() = default;
EnhancedImportManager::~EnhancedImportManager() = default;

// === 设置和初始化 ===

void EnhancedImportManager::setSearchPaths(const SearchPath& paths) {
    searchPaths_ = paths;
}

void EnhancedImportManager::setCurrentCompilingFile(const std::string& filePath) {
    currentCompilingFile_ = filePath;
    searchPaths_.compilingFileDir = getDirectoryPath(filePath);
}

// === 导入处理 ===

bool EnhancedImportManager::parseImportStatement(const std::string& statement, ImportRule& rule) {
    // 解析导入语句，严格按照CHTL语法文档
    
    clearErrors();
    
    // 基本的导入语句解析
    std::regex importRegex(R"(\[Import\]\s*(@\w+)\s+from\s+([^\s]+)(?:\s+as\s+([^\s]+))?)");
    std::smatch match;
    
    if (!std::regex_search(statement, match, importRegex)) {
        addError("无效的导入语句格式: " + statement);
        return false;
    }
    
    std::string typeStr = match[1].str();
    rule.path = match[2].str();
    rule.alias = match.size() > 3 ? match[3].str() : "";
    rule.hasAs = !rule.alias.empty();
    
    // 确定导入类型
    if (typeStr == "@Html") {
        rule.type = ImportType::HTML;
    } else if (typeStr == "@Style") {
        rule.type = ImportType::STYLE;
    } else if (typeStr == "@JavaScript") {
        rule.type = ImportType::JAVASCRIPT;
    } else if (typeStr == "@Chtl") {
        rule.type = ImportType::CHTL;
    } else if (typeStr == "@CJmod") {
        rule.type = ImportType::CJMOD;
    } else {
        addError("不支持的导入类型: " + typeStr);
        return false;
    }
    
    // 检查通配符
    if (rule.path.find('*') != std::string::npos || rule.path.find('?') != std::string::npos) {
        rule.isWildcard = true;
        rule.wildcardPattern = rule.path;
    }
    
    // 检查子模块
    if (rule.path.find('.') != std::string::npos && rule.type == ImportType::CHTL) {
        rule.isSubmodule = true;
        rule.submodulePath = rule.path;
    }
    
    return true;
}

bool EnhancedImportManager::processResourceImport(const ImportRule& rule) {
    // 处理@Html、@Style、@JavaScript导入
    // 严格按照您的要求
    
    if (rule.type != ImportType::HTML && rule.type != ImportType::STYLE && rule.type != ImportType::JAVASCRIPT) {
        return false;
    }
    
    // 无as语法时直接跳过
    if (!rule.hasAs) {
        if (stateManager_) {
            // 使用RAII状态管理
            auto guard = std::make_unique<state::RAIIStateManager::StateGuard>(
                *stateManager_, state::CompilerState::PARSING_IMPORT, "resource_import_skip");
            guard->setContextData("skip_reason", "no_as_syntax");
        }
        return true; // 直接跳过，不是错误
    }
    
    // 有as语法时，创建对应类型的带名原始嵌入节点
    std::string pathType, fileName, extension;
    if (!analyzePath(rule.path, pathType, fileName, extension)) {
        return false;
    }
    
    // 路径处理规则
    std::string resolvedPath;
    
    if (pathType == "filename_only") {
        // 文件名（不带后缀）：在编译文件所在目录（非递归）按类型搜索相关文件
        resolvedPath = searchFile(fileName, getExpectedExtension(rule.type), rule.type);
    } else if (pathType == "specific_filename") {
        // 具体文件名（带后缀）：在编译文件所在目录（非递归）直接搜索该文件
        resolvedPath = searchPaths_.compilingFileDir + "/" + rule.path;
        if (!validateFilePath(resolvedPath)) {
            addError("文件未找到: " + resolvedPath);
            return false;
        }
    } else if (pathType == "folder_or_no_file") {
        // 路径为文件夹或不包含具体文件信息时，触发报错
        addError("@" + getImportTypeString(rule.type) + "导入不支持文件夹路径: " + rule.path);
        return false;
    }
    
    // 创建带名原始嵌入节点（这里记录，实际创建由AST处理）
    if (stateManager_) {
        auto guard = std::make_unique<state::RAIIStateManager::StateGuard>(
            *stateManager_, state::CompilerState::PARSING_ORIGIN, "named_origin_embed");
        guard->setContextData("origin_type", getImportTypeString(rule.type));
        guard->setContextData("alias", rule.alias);
        guard->setContextData("resolved_path", resolvedPath);
    }
    
    return true;
}

bool EnhancedImportManager::processChtlImport(const ImportRule& rule) {
    // 处理@Chtl导入
    // 严格按照您的要求
    
    if (rule.type != ImportType::CHTL) {
        return false;
    }
    
    // 检查是否为官方模块
    if (isOfficialModuleReference(rule.path)) {
        return processOfficialModuleImport(rule);
    }
    
    // 检查是否为通配符导入
    if (rule.isWildcard) {
        auto resolvedFiles = resolveWildcardImport(rule);
        for (const auto& file : resolvedFiles) {
            addToImportedModules(file);
        }
        return !resolvedFiles.empty();
    }
    
    // 检查是否为子模块导入
    if (rule.isSubmodule) {
        auto resolvedSubmodules = resolveSubmoduleImport(rule);
        for (const auto& submodule : resolvedSubmodules) {
            addToImportedModules(submodule);
        }
        return !resolvedSubmodules.empty();
    }
    
    std::string pathType, fileName, extension;
    if (!analyzePath(rule.path, pathType, fileName, extension)) {
        return false;
    }
    
    std::string resolvedPath;
    
    if (pathType == "filename_only") {
        // 名称（不带后缀）：按优先级搜索，优先匹配cmod文件
        resolvedPath = searchFile(fileName, ".cmod", rule.type);
        if (resolvedPath.empty()) {
            resolvedPath = searchFile(fileName, ".chtl", rule.type);
        }
    } else if (pathType == "specific_filename") {
        // 具体名称（带后缀）：按优先级顺序搜索指定文件
        resolvedPath = searchFile(fileName, extension, rule.type);
    } else if (pathType == "specific_path_with_file") {
        // 具体路径（含文件信息）：直接按路径查找
        if (!validateFilePath(rule.path)) {
            addError("@Chtl导入文件未找到: " + rule.path);
            return false;
        }
        resolvedPath = rule.path;
    } else if (pathType == "specific_path_without_file") {
        // 具体路径（不含文件信息）：触发报错
        addError("@Chtl导入不支持无文件信息的路径: " + rule.path);
        return false;
    }
    
    if (resolvedPath.empty()) {
        addError("@Chtl导入无法解析路径: " + rule.path);
        return false;
    }
    
    // 检测循环依赖
    if (detectCircularDependency(fileName)) {
        addError("检测到循环依赖: " + fileName);
        return false;
    }
    
    // 检测重复导入
    if (detectDuplicateImport(rule)) {
        addError("检测到重复导入: " + rule.path);
        return false;
    }
    
    addToImportedModules(fileName);
    return true;
}

bool EnhancedImportManager::processCJmodImport(const ImportRule& rule) {
    // 处理@CJmod导入
    // CJmod与Cmod采用相同的路径搜索策略，但仅匹配cjmod文件
    
    if (rule.type != ImportType::CJMOD) {
        return false;
    }
    
    std::string pathType, fileName, extension;
    if (!analyzePath(rule.path, pathType, fileName, extension)) {
        return false;
    }
    
    std::string resolvedPath;
    
    if (pathType == "filename_only") {
        // 名称（不带后缀）：仅匹配cjmod文件
        resolvedPath = searchFile(fileName, ".cjmod", rule.type);
    } else if (pathType == "specific_filename") {
        // 具体名称（带后缀）：按优先级顺序搜索指定文件
        if (extension != ".cjmod") {
            addError("@CJmod导入只支持.cjmod文件: " + rule.path);
            return false;
        }
        resolvedPath = searchFile(fileName, extension, rule.type);
    } else if (pathType == "specific_path_with_file") {
        // 具体路径（含文件信息）：直接按路径查找
        if (!validateFilePath(rule.path)) {
            addError("@CJmod导入文件未找到: " + rule.path);
            return false;
        }
        resolvedPath = rule.path;
    } else if (pathType == "specific_path_without_file") {
        // 具体路径（不含文件信息）：触发报错
        addError("@CJmod导入不支持无文件信息的路径: " + rule.path);
        return false;
    }
    
    if (resolvedPath.empty()) {
        addError("@CJmod导入无法解析路径: " + rule.path);
        return false;
    }
    
    // 检测循环依赖和重复导入
    if (detectCircularDependency(fileName)) {
        addError("检测到循环依赖: " + fileName);
        return false;
    }
    
    if (detectDuplicateImport(rule)) {
        addError("检测到重复导入: " + rule.path);
        return false;
    }
    
    addToImportedModules(fileName);
    return true;
}

// === 通配符支持 ===

std::vector<std::string> EnhancedImportManager::resolveWildcardImport(const ImportRule& rule) {
    // 处理通配符导入
    // [Import] @Chtl from 具体路径.* 等价于 [Import] @Chtl from 具体路径/*
    
    std::vector<std::string> resolvedFiles;
    
    std::string pattern = rule.wildcardPattern;
    
    // 转换.*为/*格式
    if (pattern.find(".*") != std::string::npos) {
        std::regex dotStarRegex(R"(\.(\*(?:\.[\w]+)?))");
        pattern = std::regex_replace(pattern, dotStarRegex, "/$1");
    }
    
    // 解析目录和文件模式
    size_t lastSlash = pattern.find_last_of('/');
    if (lastSlash != std::string::npos) {
        std::string directory = pattern.substr(0, lastSlash);
        std::string filePattern = pattern.substr(lastSlash + 1);
        
        resolvedFiles = expandWildcard(filePattern, directory);
    }
    
    return resolvedFiles;
}

std::vector<std::string> EnhancedImportManager::resolveSubmoduleImport(const ImportRule& rule) {
    // 处理子模块导入
    // 支持使用'/'替代'.'作为路径分隔符
    
    std::vector<std::string> resolvedSubmodules;
    
    std::string submodulePath = rule.submodulePath;
    
    // 转换'.'为'/'
    std::replace(submodulePath.begin(), submodulePath.end(), '.', '/');
    
    // 解析主模块和子模块
    size_t firstSlash = submodulePath.find('/');
    if (firstSlash != std::string::npos) {
        std::string mainModule = submodulePath.substr(0, firstSlash);
        std::string subModule = submodulePath.substr(firstSlash + 1);
        
        if (subModule == "*") {
            // 导入所有子模块
            std::string mainModulePath = searchFile(mainModule, ".cmod", ImportType::CHTL);
            if (!mainModulePath.empty()) {
                // 搜索主模块的所有子模块
                resolvedSubmodules = searchInDirectory(mainModulePath + "/src", "*");
            }
        } else {
            // 导入指定子模块
            std::string submodulePath = searchFile(mainModule + "/" + subModule, ".chtl", ImportType::CHTL);
            if (!submodulePath.empty()) {
                resolvedSubmodules.push_back(submodulePath);
            }
        }
    }
    
    return resolvedSubmodules;
}

// === 路径处理 ===

bool EnhancedImportManager::analyzePath(const std::string& path, std::string& pathType, std::string& fileName, std::string& extension) {
    // 解析路径类型，严格按照您的要求
    
    if (path.empty()) {
        addError("路径不能为空");
        return false;
    }
    
    // 检查是否包含路径分隔符
    bool hasPathSeparator = path.find('/') != std::string::npos || path.find('\\') != std::string::npos;
    
    // 检查是否有文件扩展名
    size_t lastDot = path.find_last_of('.');
    bool hasExtension = lastDot != std::string::npos && lastDot > path.find_last_of('/');
    
    if (!hasPathSeparator && !hasExtension) {
        // 文件名（不带后缀）
        pathType = "filename_only";
        fileName = path;
        extension = "";
    } else if (!hasPathSeparator && hasExtension) {
        // 具体文件名（带后缀）
        pathType = "specific_filename";
        fileName = path.substr(0, lastDot);
        extension = path.substr(lastDot);
    } else if (hasPathSeparator && hasExtension) {
        // 具体路径（含文件信息）
        pathType = "specific_path_with_file";
        size_t lastSlash = path.find_last_of('/');
        fileName = path.substr(lastSlash + 1, lastDot - lastSlash - 1);
        extension = path.substr(lastDot);
    } else {
        // 具体路径（不含文件信息）
        pathType = "specific_path_without_file";
        fileName = "";
        extension = "";
    }
    
    return true;
}

std::string EnhancedImportManager::searchFile(const std::string& fileName, const std::string& extension, ImportType type) {
    // 按照您要求的优先级顺序搜索文件
    
    std::string fullFileName = fileName + extension;
    std::vector<std::string> searchDirs;
    
    if (type == ImportType::HTML || type == ImportType::STYLE || type == ImportType::JAVASCRIPT) {
        // 资源文件只在编译文件所在目录搜索
        searchDirs.push_back(searchPaths_.compilingFileDir);
    } else {
        // CHTL和CJmod按优先级搜索
        searchDirs.push_back(searchPaths_.officialModuleDir);
        searchDirs.push_back(searchPaths_.currentModuleDir);
        searchDirs.push_back(searchPaths_.currentDir);
    }
    
    for (const auto& dir : searchDirs) {
        if (dir.empty()) continue;
        
        std::string fullPath = dir + "/" + fullFileName;
        if (validateFilePath(fullPath)) {
            return fullPath;
        }
    }
    
    return ""; // 未找到
}

bool EnhancedImportManager::validateFilePath(const std::string& path) {
    return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
}

// === 循环依赖和重复导入处理 ===

bool EnhancedImportManager::detectCircularDependency(const std::string& moduleName) {
    // 检测循环依赖
    
    return std::find(dependencyChain_.begin(), dependencyChain_.end(), moduleName) != dependencyChain_.end();
}

bool EnhancedImportManager::detectDuplicateImport(const ImportRule& rule) {
    // 检测重复导入，修正同一路径多表达方式问题
    
    std::string normalizedPath = normalizePath(rule.path);
    
    if (normalizedPaths_.find(normalizedPath) != normalizedPaths_.end()) {
        return true; // 重复导入
    }
    
    normalizedPaths_.insert(normalizedPath);
    return false;
}

std::string EnhancedImportManager::normalizePath(const std::string& path) {
    // 规范化路径，修正同一路径多表达方式问题
    
    try {
        std::filesystem::path p(path);
        return p.lexically_normal().string();
    } catch (...) {
        return path; // 如果规范化失败，返回原路径
    }
}

// === 官方模块支持 ===

bool EnhancedImportManager::processOfficialModuleImport(const ImportRule& rule) {
    // 处理官方模块前缀"chtl::"
    
    std::string officialModuleName = parseOfficialModuleName(rule.path);
    if (officialModuleName.empty()) {
        addError("无效的官方模块引用: " + rule.path);
        return false;
    }
    
    // 在官方模块目录中搜索
    std::string resolvedPath = searchPaths_.officialModuleDir + "/" + officialModuleName;
    
    // 优先搜索.cmod文件
    if (validateFilePath(resolvedPath + ".cmod")) {
        resolvedPath += ".cmod";
    } else if (validateFilePath(resolvedPath + ".chtl")) {
        resolvedPath += ".chtl";
    } else {
        addError("官方模块未找到: " + officialModuleName);
        return false;
    }
    
    addToImportedModules(officialModuleName);
    return true;
}

bool EnhancedImportManager::isOfficialModuleReference(const std::string& moduleName) {
    return moduleName.find("chtl::") == 0;
}

std::string EnhancedImportManager::parseOfficialModuleName(const std::string& reference) {
    if (reference.find("chtl::") == 0) {
        return reference.substr(6); // 移除"chtl::"前缀
    }
    return "";
}

// === 错误处理 ===

bool EnhancedImportManager::hasErrors() const {
    return !errors_.empty();
}

std::vector<std::string> EnhancedImportManager::getErrors() const {
    return errors_;
}

void EnhancedImportManager::clearErrors() {
    errors_.clear();
}

void EnhancedImportManager::addError(const std::string& message) {
    errors_.push_back("EnhancedImportManager: " + message);
}

// === 状态管理集成 ===

void EnhancedImportManager::setStateManager(std::shared_ptr<state::RAIIStateManager> stateManager) {
    stateManager_ = stateManager;
}

std::unordered_map<std::string, int> EnhancedImportManager::getImportStatistics() const {
    std::unordered_map<std::string, int> stats;
    
    for (const auto& rule : importRules_) {
        std::string typeStr = getImportTypeString(rule.type);
        stats[typeStr]++;
    }
    
    return stats;
}

// === 私有方法 ===

std::string EnhancedImportManager::getImportTypeString(ImportType type) const {
    switch (type) {
        case ImportType::HTML: return "Html";
        case ImportType::STYLE: return "Style";
        case ImportType::JAVASCRIPT: return "JavaScript";
        case ImportType::CHTL: return "Chtl";
        case ImportType::CJMOD: return "CJmod";
        default: return "Unknown";
    }
}

std::string EnhancedImportManager::getExpectedExtension(ImportType type) const {
    switch (type) {
        case ImportType::HTML: return ".html";
        case ImportType::STYLE: return ".css";
        case ImportType::JAVASCRIPT: return ".js";
        case ImportType::CHTL: return ".chtl";
        case ImportType::CJMOD: return ".cjmod";
        default: return "";
    }
}

bool EnhancedImportManager::isFileNameOnly(const std::string& path) {
    return path.find('/') == std::string::npos && path.find('.') == std::string::npos;
}

bool EnhancedImportManager::isSpecificFileName(const std::string& path) {
    return path.find('/') == std::string::npos && path.find('.') != std::string::npos;
}

bool EnhancedImportManager::isSpecificPathWithFile(const std::string& path) {
    return path.find('/') != std::string::npos && path.find('.') != std::string::npos;
}

bool EnhancedImportManager::isSpecificPathWithoutFile(const std::string& path) {
    return path.find('/') != std::string::npos && path.find('.') == std::string::npos;
}

std::string EnhancedImportManager::getFileExtension(const std::string& fileName) {
    size_t lastDot = fileName.find_last_of('.');
    return lastDot != std::string::npos ? fileName.substr(lastDot) : "";
}

std::string EnhancedImportManager::getFileNameWithoutExtension(const std::string& fileName) {
    size_t lastDot = fileName.find_last_of('.');
    return lastDot != std::string::npos ? fileName.substr(0, lastDot) : fileName;
}

std::string EnhancedImportManager::getDirectoryPath(const std::string& filePath) {
    size_t lastSlash = filePath.find_last_of('/');
    return lastSlash != std::string::npos ? filePath.substr(0, lastSlash) : ".";
}

std::vector<std::string> EnhancedImportManager::searchInDirectory(const std::string& directory, const std::string& pattern) {
    std::vector<std::string> results;
    
    try {
        if (std::filesystem::exists(directory) && std::filesystem::is_directory(directory)) {
            for (const auto& entry : std::filesystem::directory_iterator(directory)) {
                if (entry.is_regular_file()) {
                    std::string fileName = entry.path().filename().string();
                    if (pattern == "*" || fileName.find(pattern) != std::string::npos) {
                        results.push_back(entry.path().string());
                    }
                }
            }
        }
    } catch (...) {
        // 忽略文件系统错误
    }
    
    return results;
}

std::vector<std::string> EnhancedImportManager::expandWildcard(const std::string& pattern, const std::string& directory) {
    // 展开通配符模式
    
    std::vector<std::string> results;
    
    if (pattern == "*") {
        // 所有.cmod和.chtl文件
        auto cmodFiles = searchInDirectory(directory, ".cmod");
        auto chtlFiles = searchInDirectory(directory, ".chtl");
        results.insert(results.end(), cmodFiles.begin(), cmodFiles.end());
        results.insert(results.end(), chtlFiles.begin(), chtlFiles.end());
    } else if (pattern == "*.cmod") {
        // 所有.cmod文件
        results = searchInDirectory(directory, ".cmod");
    } else if (pattern == "*.chtl") {
        // 所有.chtl文件
        results = searchInDirectory(directory, ".chtl");
    }
    
    return results;
}

void EnhancedImportManager::addToImportedModules(const std::string& moduleName) {
    importedModules_.insert(moduleName);
    addToDependencyChain(moduleName);
}

void EnhancedImportManager::addToDependencyChain(const std::string& moduleName) {
    dependencyChain_.push_back(moduleName);
}

void EnhancedImportManager::removeFromDependencyChain(const std::string& moduleName) {
    auto it = std::find(dependencyChain_.begin(), dependencyChain_.end(), moduleName);
    if (it != dependencyChain_.end()) {
        dependencyChain_.erase(it);
    }
}

} // namespace import
} // namespace chtl