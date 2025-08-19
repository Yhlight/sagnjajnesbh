#include "ImportManager.h"
#include <iostream>
#include <filesystem>
#include <algorithm>

namespace chtl {
namespace import {

// === CircularDependencyDetector实现 ===

bool CircularDependencyDetector::addDependency(const std::string& from, const std::string& to) {
    dependencies_[from].push_back(to);
    return !hasCircularDependency();
}

bool CircularDependencyDetector::hasCircularDependency() const {
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> recStack;
    std::vector<std::string> path;
    
    for (const auto& pair : dependencies_) {
        if (visited.find(pair.first) == visited.end()) {
            if (dfs(pair.first, visited, recStack, path)) {
                return true;
            }
        }
    }
    return false;
}

std::vector<std::string> CircularDependencyDetector::getCircularPath() const {
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> recStack;
    std::vector<std::string> path;
    
    for (const auto& pair : dependencies_) {
        if (visited.find(pair.first) == visited.end()) {
            if (dfs(pair.first, visited, recStack, path)) {
                return path;
            }
        }
    }
    return {};
}

void CircularDependencyDetector::clear() {
    dependencies_.clear();
}

bool CircularDependencyDetector::dfs(const std::string& node, std::unordered_set<std::string>& visited, 
                                    std::unordered_set<std::string>& recStack, std::vector<std::string>& path) const {
    visited.insert(node);
    recStack.insert(node);
    path.push_back(node);
    
    auto it = dependencies_.find(node);
    if (it != dependencies_.end()) {
        for (const std::string& neighbor : it->second) {
            if (recStack.find(neighbor) != recStack.end()) {
                path.push_back(neighbor);
                return true;
            }
            if (visited.find(neighbor) == visited.end()) {
                if (dfs(neighbor, visited, recStack, path)) {
                    return true;
                }
            }
        }
    }
    
    recStack.erase(node);
    path.pop_back();
    return false;
}

// === DuplicateImportDetector实现 ===

bool DuplicateImportDetector::addImport(const EnhancedImportStatement& import) {
    if (isDuplicate(import)) {
        return false;
    }
    imports_.push_back(import);
    return true;
}

bool DuplicateImportDetector::isDuplicate(const EnhancedImportStatement& import) const {
    std::string key = getImportKey(import);
    
    for (const auto& existing : imports_) {
        if (getImportKey(existing) == key) {
            return true;
        }
    }
    return false;
}

std::vector<EnhancedImportStatement> DuplicateImportDetector::getDuplicates() const {
    std::vector<EnhancedImportStatement> duplicates;
    std::unordered_set<std::string> seen;
    
    for (const auto& import : imports_) {
        std::string key = getImportKey(import);
        if (seen.find(key) != seen.end()) {
            duplicates.push_back(import);
        } else {
            seen.insert(key);
        }
    }
    
    return duplicates;
}

void DuplicateImportDetector::clear() {
    imports_.clear();
}

std::string DuplicateImportDetector::getImportKey(const EnhancedImportStatement& import) const {
    return std::to_string(static_cast<int>(import.type)) + ":" + 
           import.sourcePath + ":" + import.targetName + ":" + import.alias;
}

// === EnhancedImportManager实现 ===

ImportManager::ImportManager() = default;

ImportManager::~ImportManager() = default;

void ImportManager::setModulePathManager(std::shared_ptr<common::ModulePathManager> pathManager) {
    pathManager_ = pathManager;
}

void ImportManager::setErrorHandler(std::shared_ptr<common::ErrorHandler> errorHandler) {
    errorHandler_ = errorHandler;
}

// === 导入语句解析 - 严格按照CHTL语法文档 ===

bool ImportManager::parseHtmlImport(const std::string& statement, EnhancedImportStatement& result) {
    // [Import] @Html from html文件路径 as(可选) 命名为
    
    std::regex htmlImportRegex(R"(\[Import\]\s*@Html\s+from\s+([^\s]+)(?:\s+as\s+([^\s]+))?)");
    std::smatch matches;
    
    if (!std::regex_search(statement, matches, htmlImportRegex)) {
        reportError("HTML导入语句格式错误: " + statement);
        return false;
    }
    
    result.type = ImportType::HTML_FILE;
    result.sourcePath = matches[1].str();
    result.hasAlias = matches.size() > 2 && !matches[2].str().empty();
    if (result.hasAlias) {
        result.alias = matches[2].str();
    }
    
    // 严格按照您的要求：无as语法时直接跳过
    if (!result.hasAlias) {
        reportError("HTML文件导入必须提供as别名: " + statement);
        return false;
    }
    
    return true;
}

bool ImportManager::parseCssImport(const std::string& statement, EnhancedImportStatement& result) {
    // [Import] @Style from css文件路径 as(可选) 命名为
    
    std::regex cssImportRegex(R"(\[Import\]\s*@Style\s+from\s+([^\s]+)(?:\s+as\s+([^\s]+))?)");
    std::smatch matches;
    
    if (!std::regex_search(statement, matches, cssImportRegex)) {
        reportError("CSS导入语句格式错误: " + statement);
        return false;
    }
    
    result.type = ImportType::CSS_FILE;
    result.sourcePath = matches[1].str();
    result.hasAlias = matches.size() > 2 && !matches[2].str().empty();
    if (result.hasAlias) {
        result.alias = matches[2].str();
    }
    
    // 严格按照您的要求：无as语法时直接跳过
    if (!result.hasAlias) {
        reportError("CSS文件导入必须提供as别名: " + statement);
        return false;
    }
    
    return true;
}

bool ImportManager::parseJavaScriptImport(const std::string& statement, EnhancedImportStatement& result) {
    // [Import] @JavaScript from js文件路径 as(可选) 命名为
    
    std::regex jsImportRegex(R"(\[Import\]\s*@JavaScript\s+from\s+([^\s]+)(?:\s+as\s+([^\s]+))?)");
    std::smatch matches;
    
    if (!std::regex_search(statement, matches, jsImportRegex)) {
        reportError("JavaScript导入语句格式错误: " + statement);
        return false;
    }
    
    result.type = ImportType::JAVASCRIPT_FILE;
    result.sourcePath = matches[1].str();
    result.hasAlias = matches.size() > 2 && !matches[2].str().empty();
    if (result.hasAlias) {
        result.alias = matches[2].str();
    }
    
    // 严格按照您的要求：无as语法时直接跳过
    if (!result.hasAlias) {
        reportError("JavaScript文件导入必须提供as别名: " + statement);
        return false;
    }
    
    return true;
}

bool ImportManager::parseChtlImport(const std::string& statement, EnhancedImportStatement& result) {
    // [Import] @Chtl from chtl文件路径
    // 支持通配符：[Import] @Chtl from 具体路径.*
    
    std::regex chtlImportRegex(R"(\[Import\]\s*@Chtl\s+from\s+([^\s]+))");
    std::smatch matches;
    
    if (!std::regex_search(statement, matches, chtlImportRegex)) {
        reportError("CHTL导入语句格式错误: " + statement);
        return false;
    }
    
    result.type = ImportType::CHTL_FILE;
    result.sourcePath = matches[1].str();
    result.hasAlias = false; // CHTL导入不支持as语法
    
    // 检查通配符
    if (result.sourcePath.find(".*") != std::string::npos || 
        result.sourcePath.find("/*") != std::string::npos) {
        result.isWildcard = true;
    }
    
    return true;
}

bool ImportManager::parseCJmodImport(const std::string& statement, EnhancedImportStatement& result) {
    // [Import] @CJmod from cjmod文件路径
    
    std::regex cjmodImportRegex(R"(\[Import\]\s*@CJmod\s+from\s+([^\s]+))");
    std::smatch matches;
    
    if (!std::regex_search(statement, matches, cjmodImportRegex)) {
        reportError("CJmod导入语句格式错误: " + statement);
        return false;
    }
    
    result.type = ImportType::CJMOD_FILE;
    result.sourcePath = matches[1].str();
    result.hasAlias = false; // CJmod导入不支持as语法
    
    return true;
}

bool ImportManager::parseConfigImport(const std::string& statement, EnhancedImportStatement& result) {
    // [Import] @Config 导入的配置组名称(可选) from chtl文件路径 as 命名为
    // [Import] [Configuration] @Config 导入的配置组名称(可选) from chtl文件路径 as 命名为
    
    std::regex configImportRegex(R"(\[Import\]\s*(?:\[Configuration\]\s*)?@Config(?:\s+([^\s]+))?\s+from\s+([^\s]+)(?:\s+as\s+([^\s]+))?)");
    std::smatch matches;
    
    if (!std::regex_search(statement, matches, configImportRegex)) {
        reportError("配置导入语句格式错误: " + statement);
        return false;
    }
    
    result.type = ImportType::CONFIGURATION;
    result.configName = matches[1].str(); // 可选的配置组名称
    result.sourcePath = matches[2].str();
    result.hasAlias = matches.size() > 3 && !matches[3].str().empty();
    if (result.hasAlias) {
        result.alias = matches[3].str();
    }
    
    return true;
}

bool ImportManager::parseOriginImport(const std::string& statement, EnhancedImportStatement& result) {
    // [Import] [Origin] @Html 名称 from chtl文件路径 as(可选) 命名为
    // [Import] [Origin] @Style 名称 from chtl文件路径 as(可选) 命名为
    // [Import] [Origin] @Javascript 名称 from chtl文件路径 as(可选) 命名为
    
    std::regex originImportRegex(R"(\[Import\]\s*\[Origin\]\s*@(Html|Style|Javascript)\s+([^\s]+)\s+from\s+([^\s]+)(?:\s+as\s+([^\s]+))?)");
    std::smatch matches;
    
    if (!std::regex_search(statement, matches, originImportRegex)) {
        reportError("原始嵌入导入语句格式错误: " + statement);
        return false;
    }
    
    std::string originType = matches[1].str();
    if (originType == "Html") {
        result.type = ImportType::ORIGIN_HTML;
    } else if (originType == "Style") {
        result.type = ImportType::ORIGIN_STYLE;
    } else if (originType == "Javascript") {
        result.type = ImportType::ORIGIN_JAVASCRIPT;
    }
    
    result.targetName = matches[2].str();
    result.sourcePath = matches[3].str();
    result.hasAlias = matches.size() > 4 && !matches[4].str().empty();
    if (result.hasAlias) {
        result.alias = matches[4].str();
    }
    
    return true;
}

bool ImportManager::parseCustomImport(const std::string& statement, EnhancedImportStatement& result) {
    // [Import] [Custom] @Element 名称 from chtl文件路径 as(可选) 命名为
    // [Import] [Custom] @Style 名称 from chtl文件路径 as(可选) 命名为
    // [Import] [Custom] @Var 名称 from chtl文件路径 as(可选) 命名为
    
    std::regex customImportRegex(R"(\[Import\]\s*\[Custom\]\s*@(Element|Style|Var)\s+([^\s]+)\s+from\s+([^\s]+)(?:\s+as\s+([^\s]+))?)");
    std::smatch matches;
    
    if (!std::regex_search(statement, matches, customImportRegex)) {
        reportError("自定义导入语句格式错误: " + statement);
        return false;
    }
    
    std::string customType = matches[1].str();
    if (customType == "Element") {
        result.type = ImportType::CUSTOM_ELEMENT;
    } else if (customType == "Style") {
        result.type = ImportType::CUSTOM_STYLE;
    } else if (customType == "Var") {
        result.type = ImportType::CUSTOM_VAR;
    }
    
    result.targetName = matches[2].str();
    result.sourcePath = matches[3].str();
    result.hasAlias = matches.size() > 4 && !matches[4].str().empty();
    if (result.hasAlias) {
        result.alias = matches[4].str();
    }
    
    return true;
}

bool ImportManager::parseTemplateImport(const std::string& statement, EnhancedImportStatement& result) {
    // [Import] [Template] @Element 名称 from chtl文件路径 as(可选) 命名为
    // [Import] [Template] @Style 名称 from chtl文件路径 as(可选) 命名为
    // [Import] [Template] @Var 名称 from chtl文件路径 as(可选) 命名为
    
    std::regex templateImportRegex(R"(\[Import\]\s*\[Template\]\s*@(Element|Style|Var)\s+([^\s]+)\s+from\s+([^\s]+)(?:\s+as\s+([^\s]+))?)");
    std::smatch matches;
    
    if (!std::regex_search(statement, matches, templateImportRegex)) {
        reportError("模板导入语句格式错误: " + statement);
        return false;
    }
    
    std::string templateType = matches[1].str();
    if (templateType == "Element") {
        result.type = ImportType::TEMPLATE_ELEMENT;
    } else if (templateType == "Style") {
        result.type = ImportType::TEMPLATE_STYLE;
    } else if (templateType == "Var") {
        result.type = ImportType::TEMPLATE_VAR;
    }
    
    result.targetName = matches[2].str();
    result.sourcePath = matches[3].str();
    result.hasAlias = matches.size() > 4 && !matches[4].str().empty();
    if (result.hasAlias) {
        result.alias = matches[4].str();
    }
    
    return true;
}

bool ImportManager::parseBatchImport(const std::string& statement, EnhancedImportStatement& result) {
    // [Import] [Template] from chtl文件路径
    // [Import] [Custom] from chtl文件路径
    // [Import] [Origin] from chtl文件路径
    // [Import] [Custom] @Element from chtl文件路径
    // [Import] [Custom] @Style from chtl文件路径
    // [Import] [Custom] @Var from chtl文件路径
    // [Import] [Template] @Element from chtl文件路径
    // [Import] [Template] @Style from chtl文件路径
    // [Import] [Template] @Var from chtl文件路径
    
    std::regex batchImportRegex(R"(\[Import\]\s*\[(Template|Custom|Origin)\](?:\s*@(Element|Style|Var))?\s+from\s+([^\s]+))");
    std::smatch matches;
    
    if (!std::regex_search(statement, matches, batchImportRegex)) {
        reportError("批量导入语句格式错误: " + statement);
        return false;
    }
    
    std::string category = matches[1].str();
    std::string specificType = matches[2].str();
    result.sourcePath = matches[3].str();
    result.hasAlias = false; // 批量导入不支持as语法
    
    if (category == "Template") {
        if (specificType.empty()) {
            result.type = ImportType::ALL_TEMPLATES;
        } else if (specificType == "Element") {
            result.type = ImportType::ALL_TEMPLATE_ELEMENTS;
        } else if (specificType == "Style") {
            result.type = ImportType::ALL_TEMPLATE_STYLES;
        } else if (specificType == "Var") {
            result.type = ImportType::ALL_TEMPLATE_VARS;
        }
    } else if (category == "Custom") {
        if (specificType.empty()) {
            result.type = ImportType::ALL_CUSTOMS;
        } else if (specificType == "Element") {
            result.type = ImportType::ALL_CUSTOM_ELEMENTS;
        } else if (specificType == "Style") {
            result.type = ImportType::ALL_CUSTOM_STYLES;
        } else if (specificType == "Var") {
            result.type = ImportType::ALL_CUSTOM_VARS;
        }
    } else if (category == "Origin") {
        result.type = ImportType::ALL_ORIGINS;
    }
    
    return true;
}

bool ImportManager::parseImportStatement(const std::string& statement, EnhancedImportStatement& result) {
    // 主解析函数 - 自动识别导入类型
    
    // 清理语句
    std::string cleanStatement = statement;
    std::regex whitespaceRegex(R"(\s+)");
    cleanStatement = std::regex_replace(cleanStatement, whitespaceRegex, " ");
    
    // 尝试各种导入类型
    if (parseHtmlImport(cleanStatement, result)) return true;
    if (parseCssImport(cleanStatement, result)) return true;
    if (parseJavaScriptImport(cleanStatement, result)) return true;
    if (parseChtlImport(cleanStatement, result)) return true;
    if (parseCJmodImport(cleanStatement, result)) return true;
    if (parseConfigImport(cleanStatement, result)) return true;
    if (parseOriginImport(cleanStatement, result)) return true;
    if (parseCustomImport(cleanStatement, result)) return true;
    if (parseTemplateImport(cleanStatement, result)) return true;
    if (parseBatchImport(cleanStatement, result)) return true;
    
    reportError("无法识别的导入语句类型: " + statement);
    return false;
}

// === 路径处理规则 - 严格按照您的要求 ===

PathResolutionRule ImportManager::resolveStaticFileImportPath(const EnhancedImportStatement& import) {
    PathResolutionRule rule;
    rule.originalPath = import.sourcePath;
    
    // 检查是否有文件扩展名
    rule.hasFileExtension = import.sourcePath.find('.') != std::string::npos;
    
    // 检查是否是目录
    rule.isDirectory = import.sourcePath.back() == '/' || 
                      std::filesystem::is_directory(import.sourcePath);
    
    if (rule.isDirectory) {
        reportPathError(import.sourcePath, "静态文件导入不能指向目录");
        return rule;
    }
    
    std::string currentDir = getCurrentDirectory();
    
    if (rule.hasFileExtension) {
        // 具体文件名（带后缀）：直接搜索该文件
        rule.resolvedPath = currentDir + "/" + import.sourcePath;
    } else {
        // 文件名（不带后缀）：按类型搜索相关文件
        std::vector<std::string> extensions;
        
        switch (import.type) {
            case ImportType::HTML_FILE:
                extensions = {".html", ".htm"};
                break;
            case ImportType::CSS_FILE:
                extensions = {".css"};
                break;
            case ImportType::JAVASCRIPT_FILE:
                extensions = {".js", ".mjs"};
                break;
            default:
                break;
        }
        
        for (const std::string& ext : extensions) {
            std::string testPath = currentDir + "/" + import.sourcePath + ext;
            if (fileExists(testPath)) {
                rule.resolvedPath = testPath;
                break;
            }
        }
        
        if (rule.resolvedPath.empty()) {
            reportPathError(import.sourcePath, "找不到匹配的静态文件");
        }
    }
    
    return rule;
}

PathResolutionRule ImportManager::resolveChtlImportPath(const EnhancedImportStatement& import) {
    PathResolutionRule rule;
    rule.originalPath = import.sourcePath;
    
    // 检查是否有文件扩展名
    rule.hasFileExtension = import.sourcePath.find('.') != std::string::npos &&
                           import.sourcePath.find(".*") == std::string::npos;
    
    // 检查是否包含路径分隔符
    bool hasPathSeparator = import.sourcePath.find('/') != std::string::npos ||
                           import.sourcePath.find('\\') != std::string::npos;
    
    std::vector<std::string> searchPaths = {
        getOfficialModuleDirectory(),
        getCurrentModuleDirectory(),
        getCurrentDirectory()
    };
    
    if (hasPathSeparator && !import.isWildcard) {
        // 具体路径（含文件信息）：直接按路径查找
        if (fileExists(import.sourcePath)) {
            rule.resolvedPath = import.sourcePath;
        } else {
            reportPathError(import.sourcePath, "指定路径的CHTL文件不存在");
        }
    } else {
        // 名称搜索：按优先级搜索
        std::vector<std::string> extensions = rule.hasFileExtension ? 
            std::vector<std::string>{""} : 
            std::vector<std::string>{".cmod", ".chtl"};
        
        for (const std::string& searchPath : searchPaths) {
            for (const std::string& ext : extensions) {
                std::string testPath = searchPath + "/" + import.sourcePath + ext;
                if (fileExists(testPath)) {
                    rule.resolvedPath = testPath;
                    return rule;
                }
            }
        }
        
        if (rule.resolvedPath.empty()) {
            reportPathError(import.sourcePath, "在所有搜索路径中都找不到CHTL文件");
        }
    }
    
    return rule;
}

PathResolutionRule ImportManager::resolveCJmodImportPath(const EnhancedImportStatement& import) {
    PathResolutionRule rule;
    rule.originalPath = import.sourcePath;
    
    // 与CHTL类似，但仅匹配.cjmod文件
    rule.hasFileExtension = import.sourcePath.find('.') != std::string::npos;
    
    bool hasPathSeparator = import.sourcePath.find('/') != std::string::npos ||
                           import.sourcePath.find('\\') != std::string::npos;
    
    std::vector<std::string> searchPaths = {
        getOfficialModuleDirectory(),
        getCurrentModuleDirectory(),
        getCurrentDirectory()
    };
    
    if (hasPathSeparator) {
        // 具体路径：直接按路径查找
        if (fileExists(import.sourcePath)) {
            rule.resolvedPath = import.sourcePath;
        } else {
            reportPathError(import.sourcePath, "指定路径的CJmod文件不存在");
        }
    } else {
        // 名称搜索：仅匹配.cjmod文件
        std::string extension = rule.hasFileExtension ? "" : ".cjmod";
        
        for (const std::string& searchPath : searchPaths) {
            std::string testPath = searchPath + "/" + import.sourcePath + extension;
            if (fileExists(testPath)) {
                rule.resolvedPath = testPath;
                return rule;
            }
        }
        
        if (rule.resolvedPath.empty()) {
            reportPathError(import.sourcePath, "在所有搜索路径中都找不到CJmod文件");
        }
    }
    
    return rule;
}

std::vector<PathResolutionRule> ImportManager::resolveWildcardImport(const EnhancedImportStatement& import) {
    std::vector<PathResolutionRule> rules;
    
    std::vector<std::string> expandedPaths = expandWildcardPath(import.sourcePath);
    
    for (const std::string& path : expandedPaths) {
        PathResolutionRule rule;
        rule.originalPath = import.sourcePath;
        rule.resolvedPath = path;
        rule.isWildcard = true;
        rules.push_back(rule);
    }
    
    return rules;
}

PathResolutionRule ImportManager::resolveSubmoduleImport(const EnhancedImportStatement& import) {
    PathResolutionRule rule;
    rule.originalPath = import.sourcePath;
    rule.isSubmodule = true;
    
    // 支持使用'/'替代'.'作为路径分隔符
    std::string normalizedPath = normalizePathSeparators(import.sourcePath);
    
    // 解析子模块路径
    size_t dotPos = normalizedPath.find('.');
    if (dotPos != std::string::npos) {
        std::string moduleName = normalizedPath.substr(0, dotPos);
        std::string submoduleName = normalizedPath.substr(dotPos + 1);
        
        // 在模块的src目录中查找子模块
        std::vector<std::string> searchPaths = {
            getOfficialModuleDirectory() + "/" + moduleName + "/src/" + submoduleName,
            getCurrentModuleDirectory() + "/" + moduleName + "/src/" + submoduleName,
            getCurrentDirectory() + "/" + moduleName + "/src/" + submoduleName
        };
        
        for (const std::string& searchPath : searchPaths) {
            if (fileExists(searchPath + ".chtl") || fileExists(searchPath + ".cmod")) {
                rule.resolvedPath = searchPath;
                break;
            }
        }
        
        if (rule.resolvedPath.empty()) {
            reportPathError(import.sourcePath, "找不到指定的子模块");
        }
    }
    
    return rule;
}

// === 依赖管理 ===

bool ImportManager::addImport(const EnhancedImportStatement& import) {
    // 检查重复导入
    if (!duplicateDetector_.addImport(import)) {
        reportError("重复的导入语句: " + import.sourcePath);
        return false;
    }
    
    // 添加到循环依赖检测器
    std::string currentFile = getCurrentDirectory(); // 简化，实际应该是当前正在编译的文件
    if (!circularDetector_.addDependency(currentFile, import.sourcePath)) {
        reportError("检测到循环依赖: " + import.sourcePath);
        return false;
    }
    
    imports_.push_back(import);
    return true;
}

bool ImportManager::checkCircularDependencies() {
    if (circularDetector_.hasCircularDependency()) {
        auto circularPath = circularDetector_.getCircularPath();
        std::string pathStr;
        for (size_t i = 0; i < circularPath.size(); ++i) {
            pathStr += circularPath[i];
            if (i < circularPath.size() - 1) pathStr += " -> ";
        }
        reportError("检测到循环依赖: " + pathStr);
        return false;
    }
    return true;
}

bool ImportManager::checkDuplicateImports() {
    auto duplicates = duplicateDetector_.getDuplicates();
    if (!duplicates.empty()) {
        for (const auto& duplicate : duplicates) {
            reportError("重复导入: " + duplicate.sourcePath);
        }
        return false;
    }
    return true;
}

bool ImportManager::resolveAllDependencies() {
    resolvedImports_.clear();
    
    for (const auto& import : imports_) {
        EnhancedImportStatement resolved = import;
        
        // 解析路径
        if (import.isWildcard) {
            auto wildcardRules = resolveWildcardImport(import);
            for (const auto& rule : wildcardRules) {
                resolved.pathRule = rule;
                resolvedImports_.push_back(resolved);
            }
        } else {
            switch (import.type) {
                case ImportType::HTML_FILE:
                case ImportType::CSS_FILE:
                case ImportType::JAVASCRIPT_FILE:
                    resolved.pathRule = resolveStaticFileImportPath(import);
                    break;
                case ImportType::CHTL_FILE:
                    if (import.sourcePath.find('.') != std::string::npos && 
                        import.sourcePath.find('/') == std::string::npos) {
                        resolved.pathRule = resolveSubmoduleImport(import);
                    } else {
                        resolved.pathRule = resolveChtlImportPath(import);
                    }
                    break;
                case ImportType::CJMOD_FILE:
                    resolved.pathRule = resolveCJmodImportPath(import);
                    break;
                default:
                    resolved.pathRule = resolveChtlImportPath(import);
                    break;
            }
            
            if (!resolved.pathRule.resolvedPath.empty()) {
                resolvedImports_.push_back(resolved);
            }
        }
    }
    
    return errors_.empty();
}

const std::vector<EnhancedImportStatement>& ImportManager::getResolvedImports() const {
    return resolvedImports_;
}

std::vector<std::string> ImportManager::getErrors() const {
    return errors_;
}

void ImportManager::clear() {
    imports_.clear();
    resolvedImports_.clear();
    errors_.clear();
    circularDetector_.clear();
    duplicateDetector_.clear();
}

// === 私有辅助方法 ===

bool ImportManager::extractFromClause(const std::string& statement, std::string& path) {
    std::regex fromRegex(R"(from\s+([^\s]+))");
    std::smatch matches;
    
    if (std::regex_search(statement, matches, fromRegex)) {
        path = matches[1].str();
        return true;
    }
    return false;
}

bool ImportManager::extractAsClause(const std::string& statement, std::string& alias) {
    std::regex asRegex(R"(as\s+([^\s]+))");
    std::smatch matches;
    
    if (std::regex_search(statement, matches, asRegex)) {
        alias = matches[1].str();
        return true;
    }
    return false;
}

bool ImportManager::isValidPath(const std::string& path) {
    return !path.empty() && path.find("..") == std::string::npos;
}

std::string ImportManager::normalizePathSeparators(const std::string& path) {
    std::string result = path;
    std::replace(result.begin(), result.end(), '.', '/');
    return result;
}

std::vector<std::string> ImportManager::expandWildcardPath(const std::string& wildcardPath) {
    std::vector<std::string> result;
    
    // 简化实现 - 实际应该使用文件系统API进行通配符展开
    std::string basePath = wildcardPath;
    size_t wildcardPos = basePath.find(".*");
    if (wildcardPos == std::string::npos) {
        wildcardPos = basePath.find("/*");
    }
    
    if (wildcardPos != std::string::npos) {
        basePath = basePath.substr(0, wildcardPos);
        
        try {
            if (std::filesystem::exists(basePath)) {
                for (const auto& entry : std::filesystem::directory_iterator(basePath)) {
                    if (entry.is_regular_file()) {
                        result.push_back(entry.path().string());
                    }
                }
            }
        } catch (const std::filesystem::filesystem_error&) {
            // 处理文件系统错误
        }
    }
    
    return result;
}

bool ImportManager::fileExists(const std::string& path) {
    return std::filesystem::exists(path);
}

std::string ImportManager::getCurrentDirectory() {
    try {
        return std::filesystem::current_path().string();
    } catch (const std::filesystem::filesystem_error&) {
        return ".";
    }
}

std::string ImportManager::getOfficialModuleDirectory() {
    // 官方模块目录（源码编译后生成的module文件夹）
    return getCurrentDirectory() + "/module";
}

std::string ImportManager::getCurrentModuleDirectory() {
    // 当前目录的module文件夹
    return getCurrentDirectory() + "/module";
}

void ImportManager::reportError(const std::string& message, int line, int column) {
    std::string error = message;
    if (line > 0) {
        error += " (行: " + std::to_string(line);
        if (column > 0) {
            error += ", 列: " + std::to_string(column);
        }
        error += ")";
    }
    
    errors_.push_back(error);
    
    if (errorHandler_) {
        errorHandler_->reportError(common::ErrorLevel::ERROR, 
                                 common::ErrorType::IMPORT, 
                                 error, "", line, column);
    }
}

void ImportManager::reportPathError(const std::string& path, const std::string& context) {
    reportError("路径错误: " + path + " - " + context);
}

} // namespace import
} // namespace chtl