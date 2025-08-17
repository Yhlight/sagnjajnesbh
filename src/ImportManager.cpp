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
    if (officialModulePath_.empty()) {
        officialModulePath_ = "./modules"; // 默认官方模块路径
    }
}

PathNormalizationResult PathResolver::normalizePath(const String& path) {
    PathNormalizationResult result;
    
    try {
        fs::path fsPath(path);
        
        // 处理相对路径
        if (fsPath.is_relative()) {
            fsPath = fs::path(currentDirectory_) / fsPath;
        }
        
        // 规范化路径
        result.normalizedPath = fs::canonical(fsPath).string();
        result.canonicalForm = result.normalizedPath;
        result.isEquivalent = true;
        
        // 将路径分隔符统一为 '/'
        std::replace(result.normalizedPath.begin(), result.normalizedPath.end(), '\\', '/');
        std::replace(result.canonicalForm.begin(), result.canonicalForm.end(), '\\', '/');
        
    } catch (const fs::filesystem_error&) {
        // 如果路径不存在，仍然进行基本规范化
        result.normalizedPath = path;
        result.canonicalForm = path;
        result.isEquivalent = false;
        
        // 替换 '.' 为 '/'
        std::replace(result.normalizedPath.begin(), result.normalizedPath.end(), '.', '/');
        std::replace(result.canonicalForm.begin(), result.canonicalForm.end(), '.', '/');
    }
    
    return result;
}

bool PathResolver::arePathsEquivalent(const String& path1, const String& path2) {
    auto norm1 = normalizePath(path1);
    auto norm2 = normalizePath(path2);
    return norm1.canonicalForm == norm2.canonicalForm;
}

ImportPathType PathResolver::detectPathType(const String& path) {
    // 检查通配符
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
    bool hasPathSeparator = path.find('/') != String::npos || path.find('\\') != String::npos || path.find('.') != String::npos;
    
    if (hasPathSeparator) {
        // 检查是否有文件扩展名
        size_t lastDot = path.find_last_of('.');
        size_t lastSlash = path.find_last_of("/\\");
        
        if (lastDot != String::npos && (lastSlash == String::npos || lastDot > lastSlash)) {
            return ImportPathType::FULL_FILE_PATH;
        } else {
            return ImportPathType::DIRECTORY_PATH;
        }
    } else {
        // 纯文件名，检查是否有扩展名
        if (path.find('.') != String::npos) {
            return ImportPathType::FILENAME_WITH_EXT;
        } else {
            return ImportPathType::FILENAME_ONLY;
        }
    }
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
    String extension;
    switch (type) {
        case ImportNode::ImportType::HTML:
            extension = ".html";
            break;
        case ImportNode::ImportType::STYLE:
            extension = ".css";
            break;
        case ImportNode::ImportType::JAVASCRIPT:
            extension = ".js";
            break;
        default:
            return results;
    }
    
    switch (pathType) {
        case ImportPathType::FILENAME_ONLY: {
            // 在当前目录搜索对应类型的文件
            String targetFile = path + extension;
            fs::path fullPath = fs::path(currentDirectory_) / targetFile;
            if (fs::exists(fullPath)) {
                results.push_back(fullPath.string());
            }
            break;
        }
        
        case ImportPathType::FILENAME_WITH_EXT: {
            // 直接在当前目录搜索具体文件
            fs::path fullPath = fs::path(currentDirectory_) / path;
            if (fs::exists(fullPath)) {
                results.push_back(fullPath.string());
            }
            break;
        }
        
        case ImportPathType::DIRECTORY_PATH: {
            // 路径是文件夹，直接报错
            throw std::runtime_error("导入路径不能指向文件夹: " + path);
        }
        
        case ImportPathType::FULL_FILE_PATH: {
            // 具体路径，直接检查文件是否存在
            if (fs::exists(path)) {
                results.push_back(path);
            }
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
    
    switch (pathType) {
        case ImportPathType::FILENAME_ONLY: {
            // 搜索顺序：官方模块 -> 当前目录/module -> 当前目录
            // 优先级：cmod > chtl
            
            // 1. 官方模块搜索
            std::vector<String> searchPaths = {
                officialModulePath_ + "/" + path + ".cmod",
                officialModulePath_ + "/" + path + ".chtl"
            };
            
            // 2. 当前目录/module搜索
            searchPaths.push_back(currentDirectory_ + "/module/" + path + ".cmod");
            searchPaths.push_back(currentDirectory_ + "/module/" + path + ".chtl");
            
            // 3. 当前目录搜索
            searchPaths.push_back(currentDirectory_ + "/" + path + ".cmod");
            searchPaths.push_back(currentDirectory_ + "/" + path + ".chtl");
            
            for (const auto& searchPath : searchPaths) {
                if (fs::exists(searchPath)) {
                    results.push_back(searchPath);
                    break; // 找到第一个就停止
                }
            }
            break;
        }
        
        case ImportPathType::FILENAME_WITH_EXT: {
            // 具体文件名，按相同搜索顺序
            std::vector<String> searchPaths = {
                officialModulePath_ + "/" + path,
                currentDirectory_ + "/module/" + path,
                currentDirectory_ + "/" + path
            };
            
            for (const auto& searchPath : searchPaths) {
                if (fs::exists(searchPath)) {
                    results.push_back(searchPath);
                    break;
                }
            }
            break;
        }
        
        case ImportPathType::FULL_FILE_PATH: {
            // 具体路径，直接检查
            if (fs::exists(path)) {
                results.push_back(path);
            }
            break;
        }
        
        case ImportPathType::DIRECTORY_PATH: {
            // 路径不包含文件信息，报错
            throw std::runtime_error("CHTL导入路径必须包含文件信息: " + path);
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
    
    switch (pathType) {
        case ImportPathType::FILENAME_ONLY: {
            // 搜索顺序：官方模块 -> 当前目录/module -> 当前目录
            std::vector<String> searchPaths = {
                officialModulePath_ + "/" + path + ".cjmod",
                currentDirectory_ + "/module/" + path + ".cjmod",
                currentDirectory_ + "/" + path + ".cjmod"
            };
            
            for (const auto& searchPath : searchPaths) {
                if (fs::exists(searchPath)) {
                    results.push_back(searchPath);
                    break;
                }
            }
            break;
        }
        
        case ImportPathType::FILENAME_WITH_EXT: {
            std::vector<String> searchPaths = {
                officialModulePath_ + "/" + path,
                currentDirectory_ + "/module/" + path,
                currentDirectory_ + "/" + path
            };
            
            for (const auto& searchPath : searchPaths) {
                if (fs::exists(searchPath)) {
                    results.push_back(searchPath);
                    break;
                }
            }
            break;
        }
        
        case ImportPathType::FULL_FILE_PATH: {
            if (fs::exists(path)) {
                results.push_back(path);
            }
            break;
        }
        
        case ImportPathType::DIRECTORY_PATH: {
            throw std::runtime_error("CJmod导入路径必须包含文件信息: " + path);
        }
        
        default:
            break;
    }
    
    return results;
}

std::vector<String> PathResolver::resolveWildcardPath(const String& path, ImportNode::ImportType type) {
    std::vector<String> results;
    
    // 解析通配符路径
    String basePath = path;
    String pattern;
    
    if (path.find(".*") != String::npos) {
        size_t pos = path.find(".*");
        basePath = path.substr(0, pos);
        pattern = path.substr(pos + 1);
    } else if (path.find("/*") != String::npos) {
        size_t pos = path.find("/*");
        basePath = path.substr(0, pos);
        pattern = path.substr(pos + 1);
    }
    
    // 将 '.' 替换为 '/'
    std::replace(basePath.begin(), basePath.end(), '.', '/');
    
    try {
        if (fs::exists(basePath) && fs::is_directory(basePath)) {
            for (const auto& entry : fs::directory_iterator(basePath)) {
                if (entry.is_regular_file()) {
                    String filename = entry.path().filename().string();
                    String extension = entry.path().extension().string();
                    
                    bool shouldInclude = false;
                    
                    if (pattern == "*") {
                        // 所有cmod和chtl文件
                        shouldInclude = (extension == ".cmod" || extension == ".chtl");
                    } else if (pattern == "*.cmod") {
                        shouldInclude = (extension == ".cmod");
                    } else if (pattern == "*.chtl") {
                        shouldInclude = (extension == ".chtl");
                    }
                    
                    if (shouldInclude) {
                        results.push_back(entry.path().string());
                    }
                }
            }
        }
    } catch (const fs::filesystem_error&) {
        // 目录不存在或无法访问
    }
    
    return results;
}

bool PathResolver::isOfficialModulePath(const String& path) {
    return path.find("chtl::") == 0;
}

String PathResolver::resolveOfficialModulePath(const String& path) {
    if (isOfficialModulePath(path)) {
        return officialModulePath_ + "/" + path.substr(6); // 移除 "chtl::" 前缀
    }
    return path;
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
    : pathResolver_(currentDirectory, officialModulePath),
      currentDirectory_(currentDirectory) {
}

ImportResult ImportManager::importFile(const ImportNode& importNode) {
    ImportResult result;
    
    try {
        // 检查官方模块前缀
        String resolvedPath = importNode.getPath();
        if (pathResolver_.isOfficialModulePath(resolvedPath)) {
            resolvedPath = pathResolver_.resolveOfficialModulePath(resolvedPath);
        }
        
        // 检查循环依赖
        if (circularDetector_.hasCircularDependency(currentDirectory_, resolvedPath)) {
            result.error = "检测到循环依赖: " + resolvedPath;
            return result;
        }
        
        // 路径规范化
        auto normResult = pathResolver_.normalizePath(resolvedPath);
        String normalizedPath = normResult.normalizedPath;
        
        // 检查重复导入
        if (duplicateManager_.isAlreadyImported(normalizedPath, importNode.getImportType())) {
            result = ImportResult{};
            result.success = true;
            result.resolvedPath = normalizedPath;
            result.ast = duplicateManager_.getCachedImport(normalizedPath, importNode.getImportType());
            return result;
        }
        
        // 根据导入类型处理
        switch (importNode.getImportType()) {
            case ImportNode::ImportType::HTML:
            case ImportNode::ImportType::STYLE:
            case ImportNode::ImportType::JAVASCRIPT:
                return importHtmlStyleJs(importNode, resolvedPath);
                
            case ImportNode::ImportType::CHTL:
                return importChtl(importNode, resolvedPath);
                
            case ImportNode::ImportType::CJMOD:
                return importCJmod(importNode, resolvedPath);
                
            default:
                result.error = "不支持的导入类型";
                return result;
        }
        
    } catch (const std::exception& e) {
        result.error = "导入失败: " + String(e.what());
        return result;
    }
}

ImportResult ImportManager::importHtmlStyleJs(const ImportNode& importNode, const String& path) {
    ImportResult result;
    
    // 对于@Html，@Style，@JavaScript等导入语法，如果没有as语法，则直接跳过
    if (importNode.getAlias().empty()) {
        result.success = true; // 直接跳过，不报错
        return result;
    }
    
    // 有as语法，创建对应类型的带名原始嵌入节点
    try {
        auto resolvedPaths = pathResolver_.resolveHtmlStyleJsPath(path, importNode.getImportType());
        
        if (resolvedPaths.empty()) {
            result.error = "找不到文件: " + path;
            return result;
        }
        
        String targetPath = resolvedPaths[0];
        
        // 读取文件内容
        std::ifstream file(targetPath);
        if (!file.is_open()) {
            result.error = "无法打开文件: " + targetPath;
            return result;
        }
        
        std::ostringstream contentStream;
        contentStream << file.rdbuf();
        String content = contentStream.str();
        file.close();
        
        // 创建带名原始嵌入节点
        String originType;
        switch (importNode.getImportType()) {
            case ImportNode::ImportType::HTML:
                originType = "@Html";
                break;
            case ImportNode::ImportType::STYLE:
                originType = "@Style";
                break;
            case ImportNode::ImportType::JAVASCRIPT:
                originType = "@JavaScript";
                break;
            default:
                result.error = "无效的导入类型";
                return result;
        }
        
        auto originNode = std::make_shared<OriginNode>(originType, importNode.getAlias(), 0, 0);
        originNode->setContent(content);
        originNode->setSourcePath(targetPath);
        
        result.success = true;
        result.resolvedPath = targetPath;
        result.ast = originNode;
        
        // 添加到缓存
        duplicateManager_.markAsImported(targetPath, importNode.getImportType(), originNode);
        circularDetector_.addDependency(currentDirectory_, targetPath);
        
    } catch (const std::exception& e) {
        result.error = "导入HTML/CSS/JS文件失败: " + String(e.what());
    }
    
    return result;
}

ImportResult ImportManager::importChtl(const ImportNode& importNode, const String& path) {
    ImportResult result;
    
    try {
        auto resolvedPaths = pathResolver_.resolveChtlPath(path);
        
        if (resolvedPaths.empty()) {
            result.error = "找不到CHTL模块: " + path;
            return result;
        }
        
        // 处理通配符导入（多个文件）
        if (resolvedPaths.size() > 1) {
            result.resolvedPaths = resolvedPaths;
            
            for (const auto& filePath : resolvedPaths) {
                auto fileResult = importSingleChtlFile(filePath, importNode.getAlias());
                if (fileResult.success) {
                    result.astNodes.push_back(fileResult.ast);
                } else {
                    result.error += "导入 " + filePath + " 失败: " + fileResult.error + "; ";
                }
            }
            
            result.success = !result.astNodes.empty();
            return result;
        }
        
        // 单个文件导入
        String targetPath = resolvedPaths[0];
        return importSingleChtlFile(targetPath, importNode.getAlias());
        
    } catch (const std::exception& e) {
        result.error = "导入CHTL模块失败: " + String(e.what());
        return result;
    }
}

ImportResult ImportManager::importSingleChtlFile(const String& filePath, const String& alias) {
    ImportResult result;
    
    try {
        // 检查文件扩展名
        String extension = fs::path(filePath).extension().string();
        
        if (extension == ".cmod") {
            // 处理CMOD文件
            return importCmodFile(filePath, alias);
        } else if (extension == ".chtl") {
            // 处理CHTL文件
            return importChtlFile(filePath, alias);
        } else {
            result.error = "不支持的文件类型: " + extension;
            return result;
        }
        
    } catch (const std::exception& e) {
        result.error = "导入CHTL文件失败: " + String(e.what());
        return result;
    }
}

ImportResult ImportManager::importChtlFile(const String& filePath, const String& alias) {
    ImportResult result;
    
    try {
        // 读取CHTL文件
        std::ifstream file(filePath);
        if (!file.is_open()) {
            result.error = "无法打开CHTL文件: " + filePath;
            return result;
        }
        
        std::ostringstream contentStream;
        contentStream << file.rdbuf();
        String content = contentStream.str();
        file.close();
        
        // 解析CHTL内容
        // 这里需要使用CHTLParser来解析内容
        // 暂时创建一个简单的节点包装
        auto importedNode = std::make_shared<ImportedCHTLNode>(filePath, alias, 0, 0);
        importedNode->setContent(content);
        
        result.success = true;
        result.resolvedPath = filePath;
        result.ast = importedNode;
        
        // 添加到缓存和依赖跟踪
        duplicateManager_.markAsImported(filePath, ImportNode::ImportType::CHTL, importedNode);
        circularDetector_.addDependency(currentDirectory_, filePath);
        
    } catch (const std::exception& e) {
        result.error = "导入CHTL文件失败: " + String(e.what());
    }
    
    return result;
}

ImportResult ImportManager::importCJmod(const ImportNode& importNode, const String& path) {
    ImportResult result;
    
    try {
        auto resolvedPaths = pathResolver_.resolveCJmodPath(path);
        
        if (resolvedPaths.empty()) {
            result.error = "找不到CJmod模块: " + path;
            return result;
        }
        
        String targetPath = resolvedPaths[0];
        
        // 处理CJmod文件
        return importCJmodFile(targetPath, importNode.getAlias());
        
    } catch (const std::exception& e) {
        result.error = "导入CJmod模块失败: " + String(e.what());
        return result;
    }
}

ImportResult ImportManager::importCJmodFile(const String& filePath, const String& alias) {
    ImportResult result;
    
    try {
        // CJmod文件是二进制模块文件，需要特殊处理
        // 这里创建一个CJmod节点来表示导入的模块
        auto cjmodNode = std::make_shared<CJmodNode>(filePath, alias, 0, 0);
        cjmodNode->setModulePath(filePath);
        
        result.success = true;
        result.resolvedPath = filePath;
        result.ast = cjmodNode;
        
        // 添加到缓存和依赖跟踪
        duplicateManager_.markAsImported(filePath, ImportNode::ImportType::CJMOD, cjmodNode);
        circularDetector_.addDependency(currentDirectory_, filePath);
        
    } catch (const std::exception& e) {
        result.error = "导入CJmod文件失败: " + String(e.what());
    }
    
    return result;
}

std::vector<ImportResult> ImportManager::importMultipleFiles(const std::vector<ImportNode>& importNodes) {
    std::vector<ImportResult> results;
    
    for (const auto& importNode : importNodes) {
        auto result = importFile(importNode);
        results.push_back(result);
    }
    
    return results;
}

void ImportManager::clearCache() {
    duplicateManager_.clearCache();
    circularDetector_.clear();
}

bool ImportManager::hasCircularDependency(const String& fromFile, const String& toFile) {
    return circularDetector_.hasCircularDependency(fromFile, toFile);
}

std::vector<String> ImportManager::getDependencyChain(const String& file) {
    return circularDetector_.getDependencyChain(file);
}

} // namespace chtl