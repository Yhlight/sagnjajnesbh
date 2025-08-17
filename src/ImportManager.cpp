#include "ImportManager.h"
#include "CHTLParser.h"
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <regex>

namespace fs = std::filesystem;

namespace chtl {

// ==================== CircularDependencyDetector ====================

bool CircularDependencyDetector::hasCircularDependency(const String& currentFile, const String& targetFile) {
    std::unordered_set<String> visited;
    std::unordered_set<String> recursionStack;
    return dfsCheck(currentFile, targetFile, visited, recursionStack);
}

void CircularDependencyDetector::addDependency(const String& from, const String& to) {
    dependencies_[from].insert(to);
}

void CircularDependencyDetector::removeDependency(const String& from, const String& to) {
    auto it = dependencies_.find(from);
    if (it != dependencies_.end()) {
        it->second.erase(to);
        if (it->second.empty()) {
            dependencies_.erase(it);
        }
    }
}

std::vector<String> CircularDependencyDetector::getDependencyChain(const String& file) {
    std::vector<String> chain;
    std::unordered_set<String> visited;
    
    std::function<void(const String&)> dfs = [&](const String& current) {
        if (visited.find(current) != visited.end()) return;
        visited.insert(current);
        chain.push_back(current);
        
        auto it = dependencies_.find(current);
        if (it != dependencies_.end()) {
            for (const auto& dep : it->second) {
                dfs(dep);
            }
        }
    };
    
    dfs(file);
    return chain;
}

void CircularDependencyDetector::clear() {
    dependencies_.clear();
}

bool CircularDependencyDetector::dfsCheck(const String& current, const String& target, 
                                        std::unordered_set<String>& visited, 
                                        std::unordered_set<String>& recursionStack) {
    if (recursionStack.find(current) != recursionStack.end()) {
        return current == target;
    }
    
    if (visited.find(current) != visited.end()) {
        return false;
    }
    
    visited.insert(current);
    recursionStack.insert(current);
    
    auto it = dependencies_.find(current);
    if (it != dependencies_.end()) {
        for (const auto& dep : it->second) {
            if (dfsCheck(dep, target, visited, recursionStack)) {
                return true;
            }
        }
    }
    
    recursionStack.erase(current);
    return false;
}

// ==================== DuplicateImportManager ====================

bool DuplicateImportManager::isAlreadyImported(const String& normalizedPath, ImportNode::ImportType type) {
    String key = normalizedPath + "|" + std::to_string(static_cast<int>(type));
    return importCache_.find(key) != importCache_.end();
}

void DuplicateImportManager::markAsImported(const String& normalizedPath, ImportNode::ImportType type, std::shared_ptr<CHTLASTNode> ast) {
    String key = normalizedPath + "|" + std::to_string(static_cast<int>(type));
    importCache_[key] = {type, ast, normalizedPath};
}

std::shared_ptr<CHTLASTNode> DuplicateImportManager::getCachedImport(const String& normalizedPath, ImportNode::ImportType type) {
    String key = normalizedPath + "|" + std::to_string(static_cast<int>(type));
    auto it = importCache_.find(key);
    return (it != importCache_.end()) ? it->second.ast : nullptr;
}

void DuplicateImportManager::clearCache() {
    importCache_.clear();
}

size_t DuplicateImportManager::getCacheSize() const {
    return importCache_.size();
}

// ==================== PathResolver ====================

PathResolver::PathResolver(const String& currentDirectory, const String& officialModulePath)
    : currentDirectory_(currentDirectory), officialModulePath_(officialModulePath) {
    currentModuleDirectory_ = joinPath(currentDirectory_, "module");
}

PathNormalizationResult PathResolver::normalizePath(const String& path) {
    PathNormalizationResult result;
    
    try {
        fs::path fsPath(path);
        
        // 处理相对路径和绝对路径
        if (fsPath.is_relative()) {
            fsPath = fs::path(currentDirectory_) / fsPath;
        }
        
        // 规范化路径（解析 . 和 .. ）
        fs::path canonical = fs::weakly_canonical(fsPath);
        result.normalizedPath = canonical.string();
        result.canonicalForm = canonical.string();
        
        // 替换路径分隔符为统一格式
        std::replace(result.normalizedPath.begin(), result.normalizedPath.end(), '\\', '/');
        std::replace(result.canonicalForm.begin(), result.canonicalForm.end(), '\\', '/');
        
        result.isEquivalent = true;
    } catch (const std::exception& e) {
        result.normalizedPath = path;
        result.canonicalForm = path;
        result.isEquivalent = false;
    }
    
    return result;
}

bool PathResolver::arePathsEquivalent(const String& path1, const String& path2) {
    auto norm1 = normalizePath(path1);
    auto norm2 = normalizePath(path2);
    return norm1.canonicalForm == norm2.canonicalForm;
}

ImportPathType PathResolver::detectPathType(const String& path) {
    // 检查通配符模式
    if (path.find(".*") != String::npos || path.find("/*") != String::npos) {
        if (path.find(".*.cmod") != String::npos || path.find("/*.cmod") != String::npos) {
            return ImportPathType::WILDCARD_CMOD;
        } else if (path.find(".*.chtl") != String::npos || path.find("/*.chtl") != String::npos) {
            return ImportPathType::WILDCARD_CHTL;
        } else {
            return ImportPathType::WILDCARD_ALL;
        }
    }
    
    // 检查是否包含路径分隔符
    if (path.find('/') != String::npos || path.find('\\') != String::npos) {
        // 检查是否以文件名结尾
        fs::path fsPath(path);
        if (fsPath.has_filename() && !fsPath.filename().empty()) {
            String filename = fsPath.filename().string();
            if (filename.find('.') != String::npos) {
                return ImportPathType::FULL_FILE_PATH;
            } else {
                return ImportPathType::FULL_FILE_PATH; // 无扩展名的完整路径
            }
        } else {
            return ImportPathType::DIRECTORY_PATH;
        }
    }
    
    // 检查是否有文件扩展名
    if (path.find('.') != String::npos) {
        return ImportPathType::FILENAME_WITH_EXT;
    }
    
    return ImportPathType::FILENAME_ONLY;
}

std::vector<String> PathResolver::searchPaths(const String& path, ImportNode::ImportType importType) {
    ImportPathType pathType = detectPathType(path);
    
    switch (importType) {
        case ImportNode::ImportType::HTML:
        case ImportNode::ImportType::STYLE:
        case ImportNode::ImportType::JAVASCRIPT:
            return resolveHtmlStyleJsPath(path, importType);
            
        case ImportNode::ImportType::CHTL:
        case ImportNode::ImportType::CUSTOM_STYLE:
        case ImportNode::ImportType::CUSTOM_ELEMENT:
        case ImportNode::ImportType::CUSTOM_VAR:
        case ImportNode::ImportType::TEMPLATE_STYLE:
        case ImportNode::ImportType::TEMPLATE_ELEMENT:
        case ImportNode::ImportType::TEMPLATE_VAR:
            return resolveChtlPath(path);
            
        case ImportNode::ImportType::CJMOD:
            return resolveCJmodPath(path);
            
        default:
            return {};
    }
}

std::vector<String> PathResolver::resolveHtmlStyleJsPath(const String& path, ImportNode::ImportType type) {
    std::vector<String> results;
    ImportPathType pathType = detectPathType(path);
    
    // 确定文件扩展名
    std::vector<String> extensions;
    switch (type) {
        case ImportNode::ImportType::HTML:
            extensions = {".html", ".htm"};
            break;
        case ImportNode::ImportType::STYLE:
            extensions = {".css"};
            break;
        case ImportNode::ImportType::JAVASCRIPT:
            extensions = {".js", ".mjs"};
            break;
        default:
            return results;
    }
    
    switch (pathType) {
        case ImportPathType::FILENAME_ONLY: {
            // 在当前目录搜索
            auto found = searchInDirectory(currentDirectory_, path, extensions);
            results.insert(results.end(), found.begin(), found.end());
            break;
        }
        case ImportPathType::FILENAME_WITH_EXT: {
            // 直接在当前目录搜索该文件
            String fullPath = joinPath(currentDirectory_, path);
            if (fileExists(fullPath)) {
                results.push_back(fullPath);
            }
            break;
        }
        case ImportPathType::FULL_FILE_PATH: {
            // 直接使用该路径
            if (fileExists(path)) {
                results.push_back(path);
            }
            break;
        }
        case ImportPathType::DIRECTORY_PATH: {
            // 报错：不允许指向文件夹
            break;
        }
        default:
            break;
    }
    
    return results;
}

std::vector<String> PathResolver::resolveChtlPath(const String& path) {
    std::vector<String> results;
    ImportPathType pathType = detectPathType(path);
    
    // 检查是否是官方模块
    if (isOfficialModulePath(path)) {
        String resolvedPath = resolveOfficialModulePath(path);
        if (!resolvedPath.empty()) {
            results.push_back(resolvedPath);
        }
        return results;
    }
    
    std::vector<String> extensions = {".cmod", ".chtl"};
    
    switch (pathType) {
        case ImportPathType::FILENAME_ONLY: {
            // 1. 官方模块目录
            if (!officialModulePath_.empty()) {
                auto found = searchInDirectory(officialModulePath_, path, extensions);
                results.insert(results.end(), found.begin(), found.end());
            }
            
            // 2. 当前目录的module文件夹
            auto found = searchInDirectory(currentModuleDirectory_, path, extensions);
            results.insert(results.end(), found.begin(), found.end());
            
            // 3. 当前目录
            found = searchInDirectory(currentDirectory_, path, extensions);
            results.insert(results.end(), found.begin(), found.end());
            break;
        }
        case ImportPathType::FILENAME_WITH_EXT: {
            // 1. 官方模块目录
            if (!officialModulePath_.empty()) {
                String fullPath = joinPath(officialModulePath_, path);
                if (fileExists(fullPath)) {
                    results.push_back(fullPath);
                }
            }
            
            // 2. 当前目录的module文件夹
            String fullPath = joinPath(currentModuleDirectory_, path);
            if (fileExists(fullPath)) {
                results.push_back(fullPath);
            }
            
            // 3. 当前目录
            fullPath = joinPath(currentDirectory_, path);
            if (fileExists(fullPath)) {
                results.push_back(fullPath);
            }
            break;
        }
        case ImportPathType::FULL_FILE_PATH: {
            if (fileExists(path)) {
                results.push_back(path);
            }
            break;
        }
        case ImportPathType::WILDCARD_ALL:
        case ImportPathType::WILDCARD_CMOD:
        case ImportPathType::WILDCARD_CHTL: {
            return resolveWildcardPath(path, ImportNode::ImportType::CHTL);
        }
        default:
            break;
    }
    
    return results;
}

std::vector<String> PathResolver::resolveCJmodPath(const String& path) {
    std::vector<String> results;
    ImportPathType pathType = detectPathType(path);
    
    std::vector<String> extensions = {".cjmod"};
    
    switch (pathType) {
        case ImportPathType::FILENAME_ONLY: {
            // 1. 官方模块目录
            if (!officialModulePath_.empty()) {
                auto found = searchInDirectory(officialModulePath_, path, extensions);
                results.insert(results.end(), found.begin(), found.end());
            }
            
            // 2. 当前目录的module文件夹
            auto found = searchInDirectory(currentModuleDirectory_, path, extensions);
            results.insert(results.end(), found.begin(), found.end());
            
            // 3. 当前目录
            found = searchInDirectory(currentDirectory_, path, extensions);
            results.insert(results.end(), found.begin(), found.end());
            break;
        }
        case ImportPathType::FILENAME_WITH_EXT: {
            // 1. 官方模块目录
            if (!officialModulePath_.empty()) {
                String fullPath = joinPath(officialModulePath_, path);
                if (fileExists(fullPath)) {
                    results.push_back(fullPath);
                }
            }
            
            // 2. 当前目录的module文件夹
            String fullPath = joinPath(currentModuleDirectory_, path);
            if (fileExists(fullPath)) {
                results.push_back(fullPath);
            }
            
            // 3. 当前目录
            fullPath = joinPath(currentDirectory_, path);
            if (fileExists(fullPath)) {
                results.push_back(fullPath);
            }
            break;
        }
        case ImportPathType::FULL_FILE_PATH: {
            if (fileExists(path)) {
                results.push_back(path);
            }
            break;
        }
        default:
            break;
    }
    
    return results;
}

std::vector<String> PathResolver::resolveWildcardPath(const String& path, ImportNode::ImportType type) {
    std::vector<String> results;
    
    // 解析通配符路径
    String directory;
    String pattern;
    
    size_t wildcardPos = path.find(".*");
    if (wildcardPos == String::npos) {
        wildcardPos = path.find("/*");
    }
    
    if (wildcardPos != String::npos) {
        directory = path.substr(0, wildcardPos);
        pattern = path.substr(wildcardPos);
    }
    
    // 如果没有指定目录，使用当前目录
    if (directory.empty()) {
        directory = currentDirectory_;
    }
    
    // 确定搜索的文件扩展名
    std::vector<String> extensions;
    if (pattern.find(".cmod") != String::npos) {
        extensions = {".cmod"};
    } else if (pattern.find(".chtl") != String::npos) {
        extensions = {".chtl"};
    } else {
        // 根据导入类型确定扩展名
        switch (type) {
            case ImportNode::ImportType::CHTL:
                extensions = {".cmod", ".chtl"};
                break;
            case ImportNode::ImportType::CJMOD:
                extensions = {".cjmod"};
                break;
            default:
                extensions = {".cmod", ".chtl"};
                break;
        }
    }
    
    // 搜索匹配的文件
    try {
        if (fs::exists(directory) && fs::is_directory(directory)) {
            for (const auto& entry : fs::directory_iterator(directory)) {
                if (entry.is_regular_file()) {
                    String filename = entry.path().filename().string();
                    String ext = entry.path().extension().string();
                    
                    if (std::find(extensions.begin(), extensions.end(), ext) != extensions.end()) {
                        results.push_back(entry.path().string());
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        // 目录访问失败，忽略
    }
    
    return results;
}

bool PathResolver::isOfficialModulePath(const String& path) {
    return path.find("chtl::") == 0;
}

String PathResolver::resolveOfficialModulePath(const String& path) {
    if (!isOfficialModulePath(path)) {
        return "";
    }
    
    // 移除 "chtl::" 前缀
    String modulePath = path.substr(6); // "chtl::".length() == 6
    
    if (!officialModulePath_.empty()) {
        String fullPath = joinPath(officialModulePath_, modulePath);
        
        // 尝试不同的扩展名
        std::vector<String> extensions = {".cmod", ".chtl"};
        for (const auto& ext : extensions) {
            String pathWithExt = fullPath + ext;
            if (fileExists(pathWithExt)) {
                return pathWithExt;
            }
        }
        
        // 如果已经有扩展名
        if (fileExists(fullPath)) {
            return fullPath;
        }
    }
    
    return "";
}

// 辅助方法实现
std::vector<String> PathResolver::searchInDirectory(const String& directory, const String& filename, const std::vector<String>& extensions) {
    std::vector<String> results;
    
    try {
        if (!fs::exists(directory) || !fs::is_directory(directory)) {
            return results;
        }
        
        for (const auto& ext : extensions) {
            String fullPath = joinPath(directory, filename + ext);
            if (fileExists(fullPath)) {
                results.push_back(fullPath);
            }
        }
        
        // 如果filename已经包含扩展名，也尝试直接搜索
        if (filename.find('.') != String::npos) {
            String fullPath = joinPath(directory, filename);
            if (fileExists(fullPath)) {
                results.push_back(fullPath);
            }
        }
    } catch (const std::exception& e) {
        // 目录访问失败，忽略
    }
    
    return results;
}

std::vector<String> PathResolver::searchInDirectoryWithWildcard(const String& directory, const String& pattern) {
    std::vector<String> results;
    
    try {
        if (!fs::exists(directory) || !fs::is_directory(directory)) {
            return results;
        }
        
        std::regex regexPattern(pattern);
        
        for (const auto& entry : fs::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                String filename = entry.path().filename().string();
                if (std::regex_match(filename, regexPattern)) {
                    results.push_back(entry.path().string());
                }
            }
        }
    } catch (const std::exception& e) {
        // 目录访问失败或正则表达式错误，忽略
    }
    
    return results;
}

bool PathResolver::fileExists(const String& path) {
    try {
        return fs::exists(path) && fs::is_regular_file(path);
    } catch (const std::exception& e) {
        return false;
    }
}

bool PathResolver::isDirectory(const String& path) {
    try {
        return fs::exists(path) && fs::is_directory(path);
    } catch (const std::exception& e) {
        return false;
    }
}

String PathResolver::getFileExtension(const String& path) {
    fs::path fsPath(path);
    return fsPath.extension().string();
}

String PathResolver::getFileNameWithoutExtension(const String& path) {
    fs::path fsPath(path);
    return fsPath.stem().string();
}

String PathResolver::joinPath(const String& directory, const String& filename) {
    fs::path dirPath(directory);
    fs::path filePath(filename);
    fs::path result = dirPath / filePath;
    return result.string();
}

// ==================== ImportManager ====================

ImportManager::ImportManager(const String& currentDirectory, const String& officialModulePath)
    : pathResolver_(std::make_unique<PathResolver>(currentDirectory, officialModulePath)),
      circularDetector_(std::make_unique<CircularDependencyDetector>()),
      duplicateManager_(std::make_unique<DuplicateImportManager>()) {
}

ImportResult ImportManager::processImport(std::shared_ptr<ImportNode> importNode, const String& currentFile) {
    ImportResult result;
    
    if (!importNode) {
        result.error = "Invalid import node";
        return result;
    }
    
    const String& path = importNode->getPath();
    const String& alias = importNode->getAlias();
    ImportNode::ImportType importType = importNode->getImportType();
    
    // 验证导入路径
    if (!validateImportPath(path, importType)) {
        result.error = "Invalid import path: " + path;
        return result;
    }
    
    // 处理不同类型的导入
    switch (importType) {
        case ImportNode::ImportType::HTML:
            return processHtmlImport(path, alias, currentFile);
        case ImportNode::ImportType::STYLE:
            return processStyleImport(path, alias, currentFile);
        case ImportNode::ImportType::JAVASCRIPT:
            return processJavaScriptImport(path, alias, currentFile);
        case ImportNode::ImportType::CHTL:
        case ImportNode::ImportType::CUSTOM_STYLE:
        case ImportNode::ImportType::CUSTOM_ELEMENT:
        case ImportNode::ImportType::CUSTOM_VAR:
        case ImportNode::ImportType::TEMPLATE_STYLE:
        case ImportNode::ImportType::TEMPLATE_ELEMENT:
        case ImportNode::ImportType::TEMPLATE_VAR:
            return processChtlImport(path, currentFile);
        case ImportNode::ImportType::CJMOD:
            return processCJmodImport(path, currentFile);
        default:
            result.error = "Unsupported import type";
            return result;
    }
}

ImportResult ImportManager::processHtmlImport(const String& path, const String& alias, const String& currentFile) {
    ImportResult result;
    
    // 检查是否需要as语法
    if (alias.empty()) {
        // 如果没有as语法，直接跳过
        addWarning("HTML import without 'as' syntax skipped: " + path);
        result.success = true;
        return result;
    }
    
    // 解析路径
    auto resolvedPaths = pathResolver_->searchPaths(path, ImportNode::ImportType::HTML);
    
    if (resolvedPaths.empty()) {
        result.error = "HTML file not found: " + path;
        return result;
    }
    
    String resolvedPath = resolvedPaths[0]; // 取第一个匹配的文件
    
    // 规范化路径
    auto normResult = pathResolver_->normalizePath(resolvedPath);
    String normalizedPath = normResult.normalizedPath;
    
    // 检查循环依赖
    if (checkCircularDependency(currentFile, normalizedPath)) {
        result.error = "Circular dependency detected: " + currentFile + " -> " + normalizedPath;
        return result;
    }
    
    // 检查重复导入
    if (duplicateManager_->isAlreadyImported(normalizedPath, ImportNode::ImportType::HTML)) {
        result.ast = duplicateManager_->getCachedImport(normalizedPath, ImportNode::ImportType::HTML);
        result.success = true;
        result.resolvedPath = normalizedPath;
        addWarning("Using cached HTML import: " + path);
        return result;
    }
    
    // 加载文件内容
    result.ast = loadFileContent(resolvedPath, ImportNode::ImportType::HTML);
    
    if (result.ast) {
        // 创建带名原始嵌入节点
        auto originNode = createNamedOriginNode(ImportNode::ImportType::HTML, "", alias);
        result.ast = originNode;
        
        // 添加依赖关系
        addDependency(currentFile, normalizedPath);
        
        // 缓存导入结果
        duplicateManager_->markAsImported(normalizedPath, ImportNode::ImportType::HTML, result.ast);
        
        result.success = true;
        result.resolvedPath = normalizedPath;
    } else {
        result.error = "Failed to load HTML file: " + resolvedPath;
    }
    
    return result;
}

ImportResult ImportManager::processStyleImport(const String& path, const String& alias, const String& currentFile) {
    ImportResult result;
    
    // 检查是否需要as语法
    if (alias.empty()) {
        // 如果没有as语法，直接跳过
        addWarning("Style import without 'as' syntax skipped: " + path);
        result.success = true;
        return result;
    }
    
    // 解析路径
    auto resolvedPaths = pathResolver_->searchPaths(path, ImportNode::ImportType::STYLE);
    
    if (resolvedPaths.empty()) {
        result.error = "Style file not found: " + path;
        return result;
    }
    
    String resolvedPath = resolvedPaths[0];
    
    // 规范化路径
    auto normResult = pathResolver_->normalizePath(resolvedPath);
    String normalizedPath = normResult.normalizedPath;
    
    // 检查循环依赖
    if (checkCircularDependency(currentFile, normalizedPath)) {
        result.error = "Circular dependency detected: " + currentFile + " -> " + normalizedPath;
        return result;
    }
    
    // 检查重复导入
    if (duplicateManager_->isAlreadyImported(normalizedPath, ImportNode::ImportType::STYLE)) {
        result.ast = duplicateManager_->getCachedImport(normalizedPath, ImportNode::ImportType::STYLE);
        result.success = true;
        result.resolvedPath = normalizedPath;
        addWarning("Using cached Style import: " + path);
        return result;
    }
    
    // 加载文件内容
    result.ast = loadFileContent(resolvedPath, ImportNode::ImportType::STYLE);
    
    if (result.ast) {
        // 创建带名原始嵌入节点
        auto originNode = createNamedOriginNode(ImportNode::ImportType::STYLE, "", alias);
        result.ast = originNode;
        
        // 添加依赖关系
        addDependency(currentFile, normalizedPath);
        
        // 缓存导入结果
        duplicateManager_->markAsImported(normalizedPath, ImportNode::ImportType::STYLE, result.ast);
        
        result.success = true;
        result.resolvedPath = normalizedPath;
    } else {
        result.error = "Failed to load Style file: " + resolvedPath;
    }
    
    return result;
}

ImportResult ImportManager::processJavaScriptImport(const String& path, const String& alias, const String& currentFile) {
    ImportResult result;
    
    // 检查是否需要as语法
    if (alias.empty()) {
        // 如果没有as语法，直接跳过
        addWarning("JavaScript import without 'as' syntax skipped: " + path);
        result.success = true;
        return result;
    }
    
    // 解析路径
    auto resolvedPaths = pathResolver_->searchPaths(path, ImportNode::ImportType::JAVASCRIPT);
    
    if (resolvedPaths.empty()) {
        result.error = "JavaScript file not found: " + path;
        return result;
    }
    
    String resolvedPath = resolvedPaths[0];
    
    // 规范化路径
    auto normResult = pathResolver_->normalizePath(resolvedPath);
    String normalizedPath = normResult.normalizedPath;
    
    // 检查循环依赖
    if (checkCircularDependency(currentFile, normalizedPath)) {
        result.error = "Circular dependency detected: " + currentFile + " -> " + normalizedPath;
        return result;
    }
    
    // 检查重复导入
    if (duplicateManager_->isAlreadyImported(normalizedPath, ImportNode::ImportType::JAVASCRIPT)) {
        result.ast = duplicateManager_->getCachedImport(normalizedPath, ImportNode::ImportType::JAVASCRIPT);
        result.success = true;
        result.resolvedPath = normalizedPath;
        addWarning("Using cached JavaScript import: " + path);
        return result;
    }
    
    // 加载文件内容
    result.ast = loadFileContent(resolvedPath, ImportNode::ImportType::JAVASCRIPT);
    
    if (result.ast) {
        // 创建带名原始嵌入节点
        auto originNode = createNamedOriginNode(ImportNode::ImportType::JAVASCRIPT, "", alias);
        result.ast = originNode;
        
        // 添加依赖关系
        addDependency(currentFile, normalizedPath);
        
        // 缓存导入结果
        duplicateManager_->markAsImported(normalizedPath, ImportNode::ImportType::JAVASCRIPT, result.ast);
        
        result.success = true;
        result.resolvedPath = normalizedPath;
    } else {
        result.error = "Failed to load JavaScript file: " + resolvedPath;
    }
    
    return result;
}

ImportResult ImportManager::processChtlImport(const String& path, const String& currentFile) {
    ImportResult result;
    
    // 检查是否是通配符导入
    ImportPathType pathType = pathResolver_->detectPathType(path);
    if (pathType == ImportPathType::WILDCARD_ALL || 
        pathType == ImportPathType::WILDCARD_CMOD || 
        pathType == ImportPathType::WILDCARD_CHTL) {
        return processWildcardImport(path, ImportNode::ImportType::CHTL, currentFile);
    }
    
    // 解析路径
    auto resolvedPaths = pathResolver_->searchPaths(path, ImportNode::ImportType::CHTL);
    
    if (resolvedPaths.empty()) {
        result.error = "CHTL file not found: " + path;
        return result;
    }
    
    String resolvedPath = resolvedPaths[0]; // cmod优先
    
    // 规范化路径
    auto normResult = pathResolver_->normalizePath(resolvedPath);
    String normalizedPath = normResult.normalizedPath;
    
    // 检查循环依赖
    if (checkCircularDependency(currentFile, normalizedPath)) {
        result.error = "Circular dependency detected: " + currentFile + " -> " + normalizedPath;
        return result;
    }
    
    // 检查重复导入
    if (duplicateManager_->isAlreadyImported(normalizedPath, ImportNode::ImportType::CHTL)) {
        result.ast = duplicateManager_->getCachedImport(normalizedPath, ImportNode::ImportType::CHTL);
        result.success = true;
        result.resolvedPath = normalizedPath;
        addWarning("Using cached CHTL import: " + path);
        return result;
    }
    
    // 加载文件内容
    result.ast = loadFileContent(resolvedPath, ImportNode::ImportType::CHTL);
    
    if (result.ast) {
        // 添加依赖关系
        addDependency(currentFile, normalizedPath);
        
        // 缓存导入结果
        duplicateManager_->markAsImported(normalizedPath, ImportNode::ImportType::CHTL, result.ast);
        
        result.success = true;
        result.resolvedPath = normalizedPath;
    } else {
        result.error = "Failed to load CHTL file: " + resolvedPath;
    }
    
    return result;
}

ImportResult ImportManager::processCJmodImport(const String& path, const String& currentFile) {
    ImportResult result;
    
    // 解析路径
    auto resolvedPaths = pathResolver_->searchPaths(path, ImportNode::ImportType::CJMOD);
    
    if (resolvedPaths.empty()) {
        result.error = "CJmod file not found: " + path;
        return result;
    }
    
    String resolvedPath = resolvedPaths[0];
    
    // 规范化路径
    auto normResult = pathResolver_->normalizePath(resolvedPath);
    String normalizedPath = normResult.normalizedPath;
    
    // 检查循环依赖
    if (checkCircularDependency(currentFile, normalizedPath)) {
        result.error = "Circular dependency detected: " + currentFile + " -> " + normalizedPath;
        return result;
    }
    
    // 检查重复导入
    if (duplicateManager_->isAlreadyImported(normalizedPath, ImportNode::ImportType::CJMOD)) {
        result.ast = duplicateManager_->getCachedImport(normalizedPath, ImportNode::ImportType::CJMOD);
        result.success = true;
        result.resolvedPath = normalizedPath;
        addWarning("Using cached CJmod import: " + path);
        return result;
    }
    
    // 加载文件内容
    result.ast = loadFileContent(resolvedPath, ImportNode::ImportType::CJMOD);
    
    if (result.ast) {
        // 添加依赖关系
        addDependency(currentFile, normalizedPath);
        
        // 缓存导入结果
        duplicateManager_->markAsImported(normalizedPath, ImportNode::ImportType::CJMOD, result.ast);
        
        result.success = true;
        result.resolvedPath = normalizedPath;
    } else {
        result.error = "Failed to load CJmod file: " + resolvedPath;
    }
    
    return result;
}

ImportResult ImportManager::processWildcardImport(const String& path, ImportNode::ImportType type, const String& currentFile) {
    ImportResult result;
    
    // 解析通配符路径
    auto resolvedPaths = pathResolver_->resolveWildcardPath(path, type);
    
    if (resolvedPaths.empty()) {
        result.error = "No files found matching wildcard pattern: " + path;
        return result;
    }
    
    result.resolvedPaths = resolvedPaths;
    
    // 处理每个匹配的文件
    for (const auto& resolvedPath : resolvedPaths) {
        // 规范化路径
        auto normResult = pathResolver_->normalizePath(resolvedPath);
        String normalizedPath = normResult.normalizedPath;
        
        // 检查循环依赖
        if (checkCircularDependency(currentFile, normalizedPath)) {
            addWarning("Circular dependency detected, skipping: " + normalizedPath);
            continue;
        }
        
        // 检查重复导入
        if (duplicateManager_->isAlreadyImported(normalizedPath, type)) {
            auto cachedAst = duplicateManager_->getCachedImport(normalizedPath, type);
            if (cachedAst) {
                result.astNodes.push_back(cachedAst);
            }
            continue;
        }
        
        // 加载文件内容
        auto ast = loadFileContent(resolvedPath, type);
        
        if (ast) {
            result.astNodes.push_back(ast);
            
            // 添加依赖关系
            addDependency(currentFile, normalizedPath);
            
            // 缓存导入结果
            duplicateManager_->markAsImported(normalizedPath, type, ast);
        } else {
            addWarning("Failed to load file: " + resolvedPath);
        }
    }
    
    result.success = !result.astNodes.empty();
    
    if (!result.success) {
        result.error = "Failed to load any files from wildcard import: " + path;
    }
    
    return result;
}

std::shared_ptr<OriginNode> ImportManager::createNamedOriginNode(ImportNode::ImportType type, const String& content, const String& name) {
    OriginNode::OriginType originType;
    
    switch (type) {
        case ImportNode::ImportType::HTML:
            originType = OriginNode::OriginType::HTML;
            break;
        case ImportNode::ImportType::STYLE:
            originType = OriginNode::OriginType::STYLE;
            break;
        case ImportNode::ImportType::JAVASCRIPT:
            originType = OriginNode::OriginType::JAVASCRIPT;
            break;
        default:
            originType = OriginNode::OriginType::CUSTOM;
            break;
    }
    
    auto originNode = std::make_shared<OriginNode>(originType, content);
    originNode->setName(name);
    
    return originNode;
}

// 依赖管理方法
void ImportManager::addDependency(const String& from, const String& to) {
    circularDetector_->addDependency(from, to);
}

bool ImportManager::checkCircularDependency(const String& from, const String& to) {
    return circularDetector_->hasCircularDependency(from, to);
}

std::vector<String> ImportManager::getDependencyChain(const String& file) {
    return circularDetector_->getDependencyChain(file);
}

// 缓存管理方法
void ImportManager::clearImportCache() {
    duplicateManager_->clearCache();
}

size_t ImportManager::getImportCacheSize() const {
    return duplicateManager_->getCacheSize();
}

// 配置方法
void ImportManager::setOfficialModulePath(const String& path) {
    pathResolver_ = std::make_unique<PathResolver>(pathResolver_->currentDirectory_, path);
}

void ImportManager::setCurrentDirectory(const String& directory) {
    String officialPath = pathResolver_->officialModulePath_;
    pathResolver_ = std::make_unique<PathResolver>(directory, officialPath);
}

// 辅助方法
std::shared_ptr<CHTLASTNode> ImportManager::loadFileContent(const String& filePath, ImportNode::ImportType type) {
    try {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            return nullptr;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        
        // 根据文件类型处理内容
        String extension = pathResolver_->getFileExtension(filePath);
        
        if (extension == ".cmod" || extension == ".cjmod") {
            // TODO: 处理模块文件，这里暂时返回原始内容节点
            return std::make_shared<OriginNode>(OriginNode::OriginType::CUSTOM, content);
        } else if (extension == ".chtl") {
            // 解析CHTL文件
            CHTLParser parser;
            auto parseResult = parser.parse(content, filePath);
            return parseResult.ast;
        } else {
            // 其他文件类型作为原始内容处理
            OriginNode::OriginType originType = OriginNode::OriginType::CUSTOM;
            
            switch (type) {
                case ImportNode::ImportType::HTML:
                    originType = OriginNode::OriginType::HTML;
                    break;
                case ImportNode::ImportType::STYLE:
                    originType = OriginNode::OriginType::STYLE;
                    break;
                case ImportNode::ImportType::JAVASCRIPT:
                    originType = OriginNode::OriginType::JAVASCRIPT;
                    break;
                default:
                    originType = OriginNode::OriginType::CUSTOM;
                    break;
            }
            
            return std::make_shared<OriginNode>(originType, content);
        }
    } catch (const std::exception& e) {
        addError("Failed to load file: " + filePath + " - " + e.what());
        return nullptr;
    }
}

bool ImportManager::validateImportPath(const String& path, ImportNode::ImportType type) {
    if (path.empty()) {
        return false;
    }
    
    ImportPathType pathType = pathResolver_->detectPathType(path);
    
    // 检查是否指向目录（除了通配符情况）
    if (pathType == ImportPathType::DIRECTORY_PATH) {
        addError("Import path cannot point to directory: " + path);
        return false;
    }
    
    return true;
}

void ImportManager::addError(const String& error) {
    errors_.push_back(error);
}

void ImportManager::addWarning(const String& warning) {
    warnings_.push_back(warning);
}

} // namespace chtl