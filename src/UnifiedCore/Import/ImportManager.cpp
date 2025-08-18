#include "ImportManager.h"
#include "../ast/CHTLNodes.h"
#include "../ast/SpecializedNodes.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

namespace chtl {

// ImportManager 实现
ImportManager::ImportManager(CompilerContext& context) : context_(context) {
    // 初始化搜索配置
    search_config_.current_dir = getCurrentDirectory();
    search_config_.current_module_dir = getCurrentModuleDirectory();
    search_config_.official_module_dir = getOfficialModuleDirectory();
    
    // 初始化Cmod管理器
    cmod_manager_ = std::make_unique<CmodManager>();
    
    // 初始化CJmod管理器
    cjmod_manager_ = std::make_unique<CJmodManager>();
}

void ImportManager::setSearchConfig(const ImportSearchConfig& config) {
    search_config_ = config;
}

ImportSearchConfig ImportManager::getSearchConfig() const {
    return search_config_;
}

ImportInfo ImportManager::parseImportStatement(const std::string& import_type, 
                                              const std::string& path, 
                                              const std::string& alias) {
    ImportType type = stringToImportType(import_type);
    ImportInfo info(type, path, alias);
    info.path_type = analyzePathType(path);
    
    return info;
}

std::vector<ImportInfo> ImportManager::processHtmlImport(const std::string& path, const std::string& alias) {
    std::vector<ImportInfo> results;
    
    // 根据规则：无as语法时直接跳过
    if (alias.empty()) {
        reportImportWarning("HTML import without 'as' syntax is skipped", ImportInfo(ImportType::HTML, path));
        return results;
    }
    
    PathType path_type = analyzePathType(path);
    
    // 路径为文件夹或不包含具体文件信息时，触发报错
    if (path_type == PathType::DIRECTORY_PATH) {
        reportImportError("HTML import path cannot be a directory: " + path, ImportInfo(ImportType::HTML, path, alias));
        return results;
    }
    
    std::vector<std::string> resolved_paths;
    
    if (path_type == PathType::NAME_ONLY) {
        // 文件名（不带后缀）：在编译文件所在目录按类型搜索相关文件
        resolved_paths = searchResourceFiles(path, ".html");
    } else if (path_type == PathType::NAME_WITH_EXTENSION) {
        // 具体文件名（带后缀）：在编译文件所在目录直接搜索该文件
        std::string full_path = joinPath(getCurrentDirectory(), path);
        if (fileExists(full_path)) {
            resolved_paths.push_back(full_path);
        }
    } else if (path_type == PathType::SPECIFIC_PATH) {
        // 具体路径：直接查找
        if (fileExists(path)) {
            resolved_paths.push_back(getCanonicalPath(path));
        }
    }
    
    if (resolved_paths.empty()) {
        reportImportError("HTML file not found: " + path, ImportInfo(ImportType::HTML, path, alias));
        return results;
    }
    
    for (const std::string& resolved_path : resolved_paths) {
        ImportInfo info(ImportType::HTML, path, alias);
        info.resolved_path = resolved_path;
        info.path_type = path_type;
        
        // 检查重复导入
        if (!isDuplicateImport(info)) {
            results.push_back(info);
            markAsImported(info);
        }
    }
    
    return results;
}

std::vector<ImportInfo> ImportManager::processStyleImport(const std::string& path, const std::string& alias) {
    std::vector<ImportInfo> results;
    
    // 根据规则：无as语法时直接跳过
    if (alias.empty()) {
        reportImportWarning("Style import without 'as' syntax is skipped", ImportInfo(ImportType::STYLE, path));
        return results;
    }
    
    PathType path_type = analyzePathType(path);
    
    // 路径为文件夹或不包含具体文件信息时，触发报错
    if (path_type == PathType::DIRECTORY_PATH) {
        reportImportError("Style import path cannot be a directory: " + path, ImportInfo(ImportType::STYLE, path, alias));
        return results;
    }
    
    std::vector<std::string> resolved_paths;
    
    if (path_type == PathType::NAME_ONLY) {
        resolved_paths = searchResourceFiles(path, ".css");
    } else if (path_type == PathType::NAME_WITH_EXTENSION) {
        std::string full_path = joinPath(getCurrentDirectory(), path);
        if (fileExists(full_path)) {
            resolved_paths.push_back(full_path);
        }
    } else if (path_type == PathType::SPECIFIC_PATH) {
        if (fileExists(path)) {
            resolved_paths.push_back(getCanonicalPath(path));
        }
    }
    
    if (resolved_paths.empty()) {
        reportImportError("Style file not found: " + path, ImportInfo(ImportType::STYLE, path, alias));
        return results;
    }
    
    for (const std::string& resolved_path : resolved_paths) {
        ImportInfo info(ImportType::STYLE, path, alias);
        info.resolved_path = resolved_path;
        info.path_type = path_type;
        
        if (!isDuplicateImport(info)) {
            results.push_back(info);
            markAsImported(info);
        }
    }
    
    return results;
}

std::vector<ImportInfo> ImportManager::processJavaScriptImport(const std::string& path, const std::string& alias) {
    std::vector<ImportInfo> results;
    
    // 根据规则：无as语法时直接跳过
    if (alias.empty()) {
        reportImportWarning("JavaScript import without 'as' syntax is skipped", ImportInfo(ImportType::JAVASCRIPT, path));
        return results;
    }
    
    PathType path_type = analyzePathType(path);
    
    // 路径为文件夹或不包含具体文件信息时，触发报错
    if (path_type == PathType::DIRECTORY_PATH) {
        reportImportError("JavaScript import path cannot be a directory: " + path, ImportInfo(ImportType::JAVASCRIPT, path, alias));
        return results;
    }
    
    std::vector<std::string> resolved_paths;
    
    if (path_type == PathType::NAME_ONLY) {
        resolved_paths = searchResourceFiles(path, ".js");
    } else if (path_type == PathType::NAME_WITH_EXTENSION) {
        std::string full_path = joinPath(getCurrentDirectory(), path);
        if (fileExists(full_path)) {
            resolved_paths.push_back(full_path);
        }
    } else if (path_type == PathType::SPECIFIC_PATH) {
        if (fileExists(path)) {
            resolved_paths.push_back(getCanonicalPath(path));
        }
    }
    
    if (resolved_paths.empty()) {
        reportImportError("JavaScript file not found: " + path, ImportInfo(ImportType::JAVASCRIPT, path, alias));
        return results;
    }
    
    for (const std::string& resolved_path : resolved_paths) {
        ImportInfo info(ImportType::JAVASCRIPT, path, alias);
        info.resolved_path = resolved_path;
        info.path_type = path_type;
        
        if (!isDuplicateImport(info)) {
            results.push_back(info);
            markAsImported(info);
        }
    }
    
    return results;
}

std::vector<ImportInfo> ImportManager::processChtlImport(const std::string& path, const std::string& alias) {
    std::vector<ImportInfo> results;
    
    PathType path_type = analyzePathType(path);
    std::vector<std::string> resolved_paths;
    
    // 处理通配符路径
    if (isWildcardPath(path)) {
        resolved_paths = resolveWildcardPath(path, ImportType::CHTL);
    } else {
        // 转换点号路径为斜杠路径
        std::string converted_path = convertDotSlashPath(path);
        
        if (path_type == PathType::NAME_ONLY) {
            // 名称（不带后缀）：按搜索顺序查找
            std::vector<std::string> extensions = {".cmod", ".chtl"};
            resolved_paths = searchModuleFiles(converted_path, extensions);
        
        // 如果找到的是.cmod文件，进行特殊处理
        for (auto it = resolved_paths.begin(); it != resolved_paths.end(); ++it) {
            if (it->ends_with(".cmod")) {
                // 处理Cmod导入
                auto cmod_imports = processCmodImport(converted_path, alias);
                results.insert(results.end(), cmod_imports.begin(), cmod_imports.end());
                resolved_paths.erase(it);
                break;
            }
        }
        } else if (path_type == PathType::NAME_WITH_EXTENSION) {
            // 具体名称（带后缀）：按搜索顺序查找指定文件
            resolved_paths = searchModuleFiles(converted_path, {getFileExtension(converted_path)});
        } else if (path_type == PathType::SPECIFIC_PATH) {
            // 具体路径（含文件信息）：直接按路径查找
            if (fileExists(converted_path)) {
                resolved_paths.push_back(getCanonicalPath(converted_path));
            } else {
                reportImportError("CHTL file not found: " + converted_path, ImportInfo(ImportType::CHTL, path, alias));
                return results;
            }
        } else if (path_type == PathType::DIRECTORY_PATH) {
            // 具体路径（不含文件信息）：触发报错
            reportImportError("CHTL import path must contain file information: " + path, ImportInfo(ImportType::CHTL, path, alias));
            return results;
        }
    }
    
    if (resolved_paths.empty()) {
        reportImportError("CHTL file not found: " + path, ImportInfo(ImportType::CHTL, path, alias));
        return results;
    }
    
    for (const std::string& resolved_path : resolved_paths) {
        ImportInfo info(ImportType::CHTL, path, alias);
        info.resolved_path = resolved_path;
        info.path_type = path_type;
        info.resolved_files = resolved_paths;
        
        // 检查循环依赖
        std::string current_file = context_.getCurrentFile();
        if (detectCircularDependency(current_file, resolved_path)) {
            reportImportError("Circular dependency detected: " + current_file + " -> " + resolved_path, info);
            continue;
        }
        
        if (!isDuplicateImport(info)) {
            results.push_back(info);
            markAsImported(info);
            addDependency(current_file, resolved_path);
        }
    }
    
    return results;
}

std::vector<ImportInfo> ImportManager::processCJmodImport(const std::string& path, const std::string& alias) {
    std::vector<ImportInfo> results;
    
    // 解析模块名，检查是否包含子模块
    auto [main_module, submodule] = CJmodUtils::parseCJmodModuleName(path);
    
    if (!submodule.empty()) {
        // 处理子模块导入
        return processCJmodSubmoduleImport(main_module, submodule, alias);
    }
    
    // 使用CJmodManager解析CJmod路径
    std::string cjmod_path = cjmod_manager_->resolveCJmodPath(path, cjmod_manager_->getCJmodSearchPaths());
    
    if (cjmod_path.empty()) {
        reportImportError("CJmod not found: " + path, ImportInfo(ImportType::CJMOD, path, alias));
        return results;
    }
    
    // 验证CJmod文件
    if (cjmod_path.size() >= 7 && cjmod_path.substr(cjmod_path.size() - 7) == ".cjmod") {
        if (!cjmod_manager_->validateCJmodFile(cjmod_path)) {
            reportImportError("Invalid CJmod file: " + cjmod_manager_->getLastError(), 
                            ImportInfo(ImportType::CJMOD, path, alias));
            return results;
        }
        
        // 提取CJmod信息
        CJmodInfo info;
        if (!cjmod_manager_->extractCJmodInfo(cjmod_path, info)) {
            reportImportError("Cannot extract CJmod info: " + cjmod_manager_->getLastError(),
                            ImportInfo(ImportType::CJMOD, path, alias));
            return results;
        }
        
        // 创建CJmod导入信息
        ImportInfo import_info(ImportType::CJMOD, path, alias);
        import_info.resolved_path = cjmod_path;
        import_info.path_type = PathType::SPECIFIC_PATH;
        results.push_back(import_info);
    } else {
        // 处理目录形式的CJmod
        ImportInfo info(ImportType::CJMOD, path, alias);
        info.resolved_path = cjmod_path;
        info.path_type = PathType::DIRECTORY_PATH;
        results.push_back(info);
    }
    
    return results;
}

PathType ImportManager::analyzePathType(const std::string& path) const {
    if (isWildcardPath(path)) {
        return PathType::WILDCARD_PATH;
    }
    
    if (path.find('/') != std::string::npos || path.find('\\') != std::string::npos) {
        // 包含路径分隔符
        if (getFileExtension(path).empty()) {
            return PathType::DIRECTORY_PATH;
        } else {
            return PathType::SPECIFIC_PATH;
        }
    } else {
        // 不包含路径分隔符
        if (getFileExtension(path).empty()) {
            return PathType::NAME_ONLY;
        } else {
            return PathType::NAME_WITH_EXTENSION;
        }
    }
}

std::vector<std::string> ImportManager::resolveWildcardPath(const std::string& path, ImportType type) const {
    std::vector<std::string> results;
    
    // 解析通配符路径
    size_t wildcard_pos = path.find('*');
    if (wildcard_pos == std::string::npos) {
        return results;
    }
    
    std::string base_path = path.substr(0, wildcard_pos);
    std::string pattern = path.substr(wildcard_pos);
    
    // 转换点号路径为斜杠路径
    base_path = convertDotSlashPath(base_path);
    
    // 确定搜索目录
    std::vector<std::string> search_dirs;
    if (isAbsolutePath(base_path)) {
        search_dirs.push_back(base_path);
    } else {
        // 按搜索顺序添加目录
        search_dirs.push_back(joinPath(search_config_.official_module_dir, base_path));
        search_dirs.push_back(joinPath(search_config_.current_module_dir, base_path));
        search_dirs.push_back(joinPath(search_config_.current_dir, base_path));
    }
    
    // 确定搜索扩展名
    std::vector<std::string> extensions = getExtensionsForType(type);
    
    // 在每个目录中搜索匹配的文件
    for (const std::string& dir : search_dirs) {
        if (isDirectory(dir)) {
            auto matched_files = expandWildcard(dir, pattern);
            for (const std::string& file : matched_files) {
                std::string ext = getFileExtension(file);
                if (std::find(extensions.begin(), extensions.end(), ext) != extensions.end()) {
                    results.push_back(file);
                }
            }
        }
    }
    
    return results;
}

bool ImportManager::isWildcardPath(const std::string& path) const {
    return path.find('*') != std::string::npos;
}

std::string ImportManager::convertDotSlashPath(const std::string& path) const {
    std::string result = path;
    std::replace(result.begin(), result.end(), '.', '/');
    return result;
}

bool ImportManager::detectCircularDependency(const std::string& current_file, const std::string& target_file) {
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> recursion_stack;
    std::vector<std::string> path;
    
    return dfsDetectCycle(current_file, target_file, visited, recursion_stack, path);
}

bool ImportManager::isDuplicateImport(const ImportInfo& import_info) const {
    std::string canonical_path = getCanonicalPath(import_info.resolved_path);
    
    auto it = import_cache_.find(canonical_path);
    if (it != import_cache_.end()) {
        for (const ImportInfo& existing : it->second) {
            if (isSameImport(existing, import_info)) {
                return true;
            }
        }
    }
    
    return false;
}

void ImportManager::markAsImported(const ImportInfo& import_info) {
    std::string canonical_path = getCanonicalPath(import_info.resolved_path);
    import_cache_[canonical_path].push_back(import_info);
    imported_files_.insert(canonical_path);
}

std::string ImportManager::normalizeImportPath(const std::string& path) const {
    return getCanonicalPath(convertDotSlashPath(path));
}

std::string ImportManager::getCanonicalPath(const std::string& path) const {
    try {
        return fs::canonical(fs::path(path)).string();
    } catch (const fs::filesystem_error&) {
        return fs::absolute(fs::path(path)).string();
    }
}

bool ImportManager::isSameImport(const ImportInfo& a, const ImportInfo& b) const {
    return a.type == b.type && 
           getCanonicalPath(a.resolved_path) == getCanonicalPath(b.resolved_path) &&
           a.alias_name == b.alias_name;
}

void ImportManager::addDependency(const std::string& from_file, const std::string& to_file) {
    std::string canonical_from = getCanonicalPath(from_file);
    std::string canonical_to = getCanonicalPath(to_file);
    
    auto& node = dependency_graph_[canonical_from];
    node.file_path = canonical_from;
    node.dependencies.insert(canonical_to);
}

std::vector<std::string> ImportManager::searchResourceFiles(const std::string& name, const std::string& extension) const {
    std::vector<std::string> results;
    std::string current_dir = getCurrentDirectory();
    
    // 搜索当前目录
    std::string full_path = joinPath(current_dir, name + extension);
    if (fileExists(full_path)) {
        results.push_back(getCanonicalPath(full_path));
    }
    
    return results;
}

std::vector<std::string> ImportManager::searchModuleFiles(const std::string& name, const std::vector<std::string>& extensions) const {
    std::vector<std::string> results;
    
    // 搜索顺序：官方模块目录 → 当前目录module文件夹 → 当前目录
    std::vector<std::string> search_dirs = {
        search_config_.official_module_dir,
        search_config_.current_module_dir,
        search_config_.current_dir
    };
    
    for (const std::string& dir : search_dirs) {
        for (const std::string& ext : extensions) {
            std::string full_path = joinPath(dir, name + ext);
            if (fileExists(full_path)) {
                results.push_back(getCanonicalPath(full_path));
                if (ext == ".cmod") {
                    // 优先匹配cmod文件，找到后直接返回
                    return results;
                }
            }
        }
        
        // 如果在当前搜索目录找到了文件，不继续搜索下一个目录
        if (!results.empty()) {
            break;
        }
    }
    
    return results;
}

std::vector<std::string> ImportManager::expandWildcard(const std::string& base_path, const std::string& pattern) const {
    std::vector<std::string> results;
    
    try {
        if (!fs::exists(base_path) || !fs::is_directory(base_path)) {
            return results;
        }
        
        for (const auto& entry : fs::directory_iterator(base_path)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (matchesPattern(filename, pattern)) {
                    results.push_back(entry.path().string());
                }
            }
        }
    } catch (const fs::filesystem_error&) {
        // 忽略文件系统错误
    }
    
    return results;
}

bool ImportManager::matchesPattern(const std::string& filename, const std::string& pattern) const {
    if (pattern == "*") {
        return true;
    }
    
    if (pattern.find("*.") == 0) {
        std::string ext = pattern.substr(1); // 去掉 '*'
        return filename.length() >= ext.length() && 
               filename.substr(filename.length() - ext.length()) == ext;
    }
    
    // 简单的通配符匹配
    return filename == pattern;
}

bool ImportManager::dfsDetectCycle(const std::string& current, 
                                  const std::string& target,
                                  std::unordered_set<std::string>& visited,
                                  std::unordered_set<std::string>& recursion_stack,
                                  std::vector<std::string>& path) const {
    if (recursion_stack.count(current)) {
        return true; // 发现循环
    }
    
    if (visited.count(current)) {
        return false;
    }
    
    visited.insert(current);
    recursion_stack.insert(current);
    path.push_back(current);
    
    auto it = dependency_graph_.find(current);
    if (it != dependency_graph_.end()) {
        for (const std::string& dep : it->second.dependencies) {
            if (dep == target || dfsDetectCycle(dep, target, visited, recursion_stack, path)) {
                return true;
            }
        }
    }
    
    recursion_stack.erase(current);
    path.pop_back();
    return false;
}

// 辅助方法实现
std::string ImportManager::getCurrentDirectory() const {
    return fs::current_path().string();
}

std::string ImportManager::getOfficialModuleDirectory() const {
    // 假设官方模块目录在可执行文件同级的module文件夹
    return joinPath(getCurrentDirectory(), "module");
}

std::string ImportManager::getCurrentModuleDirectory() const {
    return joinPath(getCurrentDirectory(), "module");
}

std::string ImportManager::joinPath(const std::string& base, const std::string& relative) const {
    fs::path result = fs::path(base) / fs::path(relative);
    return result.string();
}

std::string ImportManager::getFileExtension(const std::string& path) const {
    fs::path p(path);
    return p.extension().string();
}

std::string ImportManager::getBaseName(const std::string& path) const {
    fs::path p(path);
    return p.stem().string();
}

bool ImportManager::fileExists(const std::string& path) const {
    return fs::exists(path) && fs::is_regular_file(path);
}

bool ImportManager::isDirectory(const std::string& path) const {
    return fs::exists(path) && fs::is_directory(path);
}

ImportType ImportManager::stringToImportType(const std::string& type_str) const {
    if (type_str == "@Html") return ImportType::HTML;
    if (type_str == "@Style") return ImportType::STYLE;
    if (type_str == "@JavaScript") return ImportType::JAVASCRIPT;
    if (type_str == "@Chtl") return ImportType::CHTL;
    if (type_str == "@CJmod") return ImportType::CJMOD;
    return ImportType::CHTL; // 默认
}

std::vector<std::string> ImportManager::getExtensionsForType(ImportType type) const {
    switch (type) {
        case ImportType::HTML:
            return {".html", ".htm"};
        case ImportType::STYLE:
            return {".css"};
        case ImportType::JAVASCRIPT:
            return {".js"};
        case ImportType::CHTL:
            return {".cmod", ".chtl"};
        case ImportType::CJMOD:
            return {".cjmod"};
        default:
            return {".chtl"};
    }
}

void ImportManager::reportImportError(const std::string& message, const ImportInfo& import_info) {
    std::string full_message = "Import Error: " + message + " (type: " + 
                              importTypeToString(import_info.type) + ", path: " + import_info.original_path + ")";
    import_errors_.push_back(full_message);
    context_.reportError(full_message);
}

void ImportManager::reportImportWarning(const std::string& message, const ImportInfo& import_info) {
    std::string full_message = "Import Warning: " + message + " (type: " + 
                              importTypeToString(import_info.type) + ", path: " + import_info.original_path + ")";
    import_warnings_.push_back(full_message);
    context_.reportWarning(full_message);
}

std::string ImportManager::importTypeToString(ImportType type) const {
    switch (type) {
        case ImportType::HTML: return "@Html";
        case ImportType::STYLE: return "@Style";
        case ImportType::JAVASCRIPT: return "@JavaScript";
        case ImportType::CHTL: return "@Chtl";
        case ImportType::CJMOD: return "@CJmod";
        case ImportType::TEMPLATE_STYLE: return "[Template] @Style";
        case ImportType::TEMPLATE_ELEMENT: return "[Template] @Element";
        case ImportType::TEMPLATE_VAR: return "[Template] @Var";
        case ImportType::CUSTOM_STYLE: return "[Custom] @Style";
        case ImportType::CUSTOM_ELEMENT: return "[Custom] @Element";
        case ImportType::CUSTOM_VAR: return "[Custom] @Var";
        default: return "Unknown";
    }
}

std::unique_ptr<ast::ASTNode> ImportManager::createOriginBlockNode(ImportType type, 
                                                                  const std::string& name,
                                                                  const std::string& content) const {
    // 根据类型创建相应的原始嵌入节点
    auto origin_node = std::make_unique<ast::OriginBlockNode>();
    
    switch (type) {
        case ImportType::HTML:
            origin_node->setAttribute("type", "@Html");
            break;
        case ImportType::STYLE:
            origin_node->setAttribute("type", "@Style");
            break;
        case ImportType::JAVASCRIPT:
            origin_node->setAttribute("type", "@JavaScript");
            break;
        default:
            origin_node->setAttribute("type", "@Html");
            break;
    }
    
    if (!name.empty()) {
        origin_node->setAttribute("name", name);
    }
    
    // 创建文本节点存储内容
    auto text_node = std::make_unique<ast::TextNode>(content);
    origin_node->addChild(std::move(text_node));
    
    return std::move(origin_node);
}

std::vector<std::string> ImportManager::getImportErrors() const {
    return import_errors_;
}

std::vector<std::string> ImportManager::getImportWarnings() const {
    return import_warnings_;
}

// ImportStatementParser 实现
ImportStatementParser::ParsedImport ImportStatementParser::parseImportStatement(const std::string& statement) {
    ParsedImport result;
    
    // 使用正则表达式解析导入语句
    std::regex import_regex = getImportRegex();
    std::smatch matches;
    
    if (std::regex_match(statement, matches, import_regex)) {
        // 解析修饰符 [Template] 或 [Custom]
        std::string modifiers = matches[1].str();
        auto [is_template, is_custom] = parseModifiers(modifiers);
        result.is_template = is_template;
        result.is_custom = is_custom;
        
        // 解析导入类型
        std::string type_part = matches[2].str();
        result.type = parseImportType(type_part);
        
        // 解析元素名称（对于模板/自定义导入）
        result.element_name = matches[3].str();
        
        // 解析路径
        result.path = matches[4].str();
        
        // 解析别名
        result.alias = matches[5].str();
    }
    
    return result;
}

std::regex ImportStatementParser::getImportRegex() {
    // 匹配导入语句的正则表达式
    return std::regex(R"(\[Import\]\s*(\[(?:Template|Custom)\])?\s*(@\w+)\s*(\w+)?\s*from\s+([^\s]+)(?:\s+as\s+(\w+))?)");
}

ImportType ImportStatementParser::parseImportType(const std::string& type_part) {
    if (type_part == "@Html") return ImportType::HTML;
    if (type_part == "@Style") return ImportType::STYLE;
    if (type_part == "@JavaScript") return ImportType::JAVASCRIPT;
    if (type_part == "@Chtl") return ImportType::CHTL;
    if (type_part == "@CJmod") return ImportType::CJMOD;
    if (type_part == "@Element") return ImportType::TEMPLATE_ELEMENT; // 默认为模板，后续根据修饰符调整
    if (type_part == "@Var") return ImportType::TEMPLATE_VAR;
    return ImportType::CHTL;
}

std::pair<bool, bool> ImportStatementParser::parseModifiers(const std::string& modifiers) {
    bool is_template = modifiers.find("[Template]") != std::string::npos;
    bool is_custom = modifiers.find("[Custom]") != std::string::npos;
    return {is_template, is_custom};
}

// Cmod相关方法实现
std::vector<ImportInfo> ImportManager::processCmodImport(const std::string& module_name, const std::string& alias) {
    std::vector<ImportInfo> results;
    
    // 解析模块名，检查是否包含子模块
    auto [main_module, submodule] = CmodUtils::parseModuleName(module_name);
    
    if (!submodule.empty()) {
        // 处理子模块导入
        return processSubmoduleImport(main_module, submodule, alias);
    }
    
    // 使用CmodManager解析Cmod路径
    std::string cmod_path = cmod_manager_->resolveCmodPath(main_module, cmod_manager_->getSearchPaths());
    
    if (cmod_path.empty()) {
        reportImportError("Cmod not found: " + main_module, ImportInfo(ImportType::CHTL, module_name, alias));
        return results;
    }
    
    // 验证Cmod文件
    if (cmod_path.ends_with(".cmod")) {
        if (!cmod_manager_->validateCmodFile(cmod_path)) {
            reportImportError("Invalid Cmod file: " + cmod_manager_->getLastError(), 
                            ImportInfo(ImportType::CHTL, module_name, alias));
            return results;
        }
        
        // 提取Cmod信息
        CmodInfo info;
        if (!cmod_manager_->extractCmodInfo(cmod_path, info)) {
            reportImportError("Cannot extract Cmod info: " + cmod_manager_->getLastError(),
                            ImportInfo(ImportType::CHTL, module_name, alias));
            return results;
        }
        
        // 获取Cmod中的所有CHTL文件
        auto files = cmod_manager_->listCmodFiles(cmod_path);
        for (const std::string& file : files) {
            if (file.starts_with("src/") && file.ends_with(".chtl")) {
                ImportInfo import_info(ImportType::CHTL, module_name, alias);
                import_info.resolved_path = cmod_path + ":" + file; // 使用特殊格式标识Cmod内文件
                import_info.path_type = PathType::SPECIFIC_PATH;
                results.push_back(import_info);
            }
        }
    } else {
        // 处理目录形式的Cmod
        ImportInfo info(ImportType::CHTL, module_name, alias);
        info.resolved_path = cmod_path;
        info.path_type = PathType::DIRECTORY_PATH;
        results.push_back(info);
    }
    
    return results;
}

std::vector<ImportInfo> ImportManager::processSubmoduleImport(const std::string& main_module, 
                                                             const std::string& submodule, 
                                                             const std::string& alias) {
    std::vector<ImportInfo> results;
    
    // 解析主模块路径
    std::string main_cmod_path = cmod_manager_->resolveCmodPath(main_module, cmod_manager_->getSearchPaths());
    
    if (main_cmod_path.empty()) {
        reportImportError("Main module not found: " + main_module, 
                        ImportInfo(ImportType::CHTL, main_module + "." + submodule, alias));
        return results;
    }
    
    if (main_cmod_path.ends_with(".cmod")) {
        // 检查子模块是否存在
        if (!cmod_manager_->hasSubmodule(main_cmod_path, submodule)) {
            reportImportError("Submodule not found: " + submodule + " in " + main_module,
                            ImportInfo(ImportType::CHTL, main_module + "." + submodule, alias));
            return results;
        }
        
        // 获取子模块文件
        auto files = cmod_manager_->listCmodFiles(main_cmod_path);
        std::string submodule_prefix = "src/" + submodule + "/";
        
        for (const std::string& file : files) {
            if (file.starts_with(submodule_prefix) && file.ends_with(".chtl")) {
                ImportInfo import_info(ImportType::CHTL, main_module + "." + submodule, alias);
                import_info.resolved_path = main_cmod_path + ":" + file;
                import_info.path_type = PathType::SPECIFIC_PATH;
                results.push_back(import_info);
            }
        }
    }
    
    return results;
}

bool ImportManager::loadCmodContent(const std::string& cmod_path, const std::string& file_path, std::string& content) {
    size_t colon_pos = cmod_path.find(':');
    if (colon_pos == std::string::npos) {
        return false;
    }
    
    std::string actual_cmod_path = cmod_path.substr(0, colon_pos);
    std::string internal_file_path = cmod_path.substr(colon_pos + 1);
    
    return cmod_manager_->extractCmodFile(actual_cmod_path, internal_file_path, content);
}

std::vector<std::string> ImportManager::extractCmodSymbols(const std::string& cmod_path, const std::string& symbol_type) {
    std::vector<std::string> symbols;
    
    CmodInfo info;
    if (!cmod_manager_->extractCmodInfo(cmod_path, info)) {
        return symbols;
    }
    
    if (symbol_type == "@Style") {
        return info.exported_styles;
    } else if (symbol_type == "@Element") {
        return info.exported_elements;
    } else if (symbol_type == "@Var") {
        return info.exported_vars;
    }
    
    return symbols;
}

// CJmod相关方法实现
std::vector<ImportInfo> ImportManager::processCJmodSubmoduleImport(const std::string& main_module, 
                                                                  const std::string& submodule, 
                                                                  const std::string& alias) {
    std::vector<ImportInfo> results;
    
    // 解析主模块路径
    std::string main_cjmod_path = cjmod_manager_->resolveCJmodPath(main_module, cjmod_manager_->getCJmodSearchPaths());
    
    if (main_cjmod_path.empty()) {
        reportImportError("Main CJmod module not found: " + main_module, 
                        ImportInfo(ImportType::CJMOD, main_module + "." + submodule, alias));
        return results;
    }
    
    if (main_cjmod_path.size() >= 7 && main_cjmod_path.substr(main_cjmod_path.size() - 7) == ".cjmod") {
        // 检查子模块是否存在
        if (!cjmod_manager_->hasCJmodSubmodule(main_cjmod_path, submodule)) {
            reportImportError("CJmod submodule not found: " + submodule + " in " + main_module,
                            ImportInfo(ImportType::CJMOD, main_module + "." + submodule, alias));
            return results;
        }
        
        // 创建子模块导入信息
        ImportInfo import_info(ImportType::CJMOD, main_module + "." + submodule, alias);
        import_info.resolved_path = main_cjmod_path + ":" + submodule;
        import_info.path_type = PathType::SPECIFIC_PATH;
        results.push_back(import_info);
    }
    
    return results;
}

bool ImportManager::loadCJmodContent(const std::string& cjmod_path, const std::string& file_path, std::string& content) {
    size_t colon_pos = cjmod_path.find(':');
    if (colon_pos == std::string::npos) {
        return cjmod_manager_->extractCJmodFile(cjmod_path, file_path, content);
    }
    
    std::string actual_cjmod_path = cjmod_path.substr(0, colon_pos);
    std::string submodule_name = cjmod_path.substr(colon_pos + 1);
    std::string internal_file_path = "src/" + submodule_name + "/" + file_path;
    
    return cjmod_manager_->extractCJmodFile(actual_cjmod_path, internal_file_path, content);
}

bool ImportManager::compileCJmodModule(const std::string& cjmod_path, const std::string& output_path) {
    return cjmod_manager_->compileCJmodSources(cjmod_path, output_path);
}

} // namespace chtl