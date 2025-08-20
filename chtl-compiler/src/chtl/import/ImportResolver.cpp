#include "chtl/import/ImportResolver.h"
#include "utils/Logger.h"
#include "utils/StringUtils.h"
#include <algorithm>
#include <regex>

namespace chtl {
namespace import {

namespace fs = std::filesystem;

// ImportResolver 实现

ImportResolver::ImportResolver() {}

ImportResolver::~ImportResolver() = default;

void ImportResolver::SetCompilerModulePath(const std::string& path) {
    m_CompilerModulePath = path;
    utils::Logger::GetInstance().Info("设置编译器模块路径: " + path);
}

void ImportResolver::SetCurrentFile(const std::string& filePath) {
    m_CurrentFilePath = fs::path(filePath);
    m_CurrentDirectory = m_CurrentFilePath.parent_path();
    
    if (m_CurrentDirectory.empty()) {
        m_CurrentDirectory = fs::current_path();
    }
    
    utils::Logger::GetInstance().Debug("当前文件: " + filePath);
    utils::Logger::GetInstance().Debug("当前目录: " + m_CurrentDirectory.string());
}

bool ImportResolver::ResolveImport(ImportInfo& import) {
    switch (import.type) {
        case ImportType::HTML:
        case ImportType::STYLE:
        case ImportType::JAVASCRIPT:
            return ResolveHtmlStyleJS(import);
            
        case ImportType::CHTL:
            return ResolveChtl(import);
            
        case ImportType::CJMOD:
            return ResolveCJmod(import);
            
        case ImportType::CUSTOM_ELEMENT:
        case ImportType::CUSTOM_STYLE:
        case ImportType::CUSTOM_VAR:
        case ImportType::TEMPLATE_ELEMENT:
        case ImportType::TEMPLATE_STYLE:
        case ImportType::TEMPLATE_VAR:
            return ResolveCustomOrTemplate(import);
            
        case ImportType::CONFIG:
            return ResolveConfig(import);
            
        case ImportType::ORIGIN_HTML:
        case ImportType::ORIGIN_STYLE:
        case ImportType::ORIGIN_JAVASCRIPT:
            return ResolveOrigin(import);
            
        default:
            ReportError(import, "未知的导入类型");
            return false;
    }
}

bool ImportResolver::ResolveImports(std::vector<ImportInfo>& imports) {
    bool allSuccess = true;
    
    for (auto& import : imports) {
        if (!ResolveImport(import)) {
            allSuccess = false;
        }
    }
    
    return allSuccess;
}

bool ImportResolver::ResolveHtmlStyleJS(ImportInfo& import) {
    // 根据目标规划文档第70-75行
    // 如果没有as语法，则直接跳过
    if (!import.hasAs) {
        utils::Logger::GetInstance().Debug("跳过没有as语法的导入: " + import.path);
        import.isResolved = true;
        return true;
    }
    
    fs::path importPath(import.path);
    std::string resolvedPath;
    
    // 如果是相对路径
    if (importPath.is_relative()) {
        // 检查是否是文件夹
        fs::path fullPath = m_CurrentDirectory / importPath;
        if (fs::is_directory(fullPath)) {
            ReportError(import, "不能导入文件夹: " + fullPath.string());
            return false;
        }
        
        // 如果没有后缀名，根据类型查找文件
        if (!importPath.has_extension()) {
            std::vector<std::string> extensions;
            switch (import.type) {
                case ImportType::HTML:
                    extensions = {".html", ".htm"};
                    break;
                case ImportType::STYLE:
                    extensions = {".css"};
                    break;
                case ImportType::JAVASCRIPT:
                    extensions = {".js"};
                    break;
                default:
                    break;
            }
            
            resolvedPath = FindFileWithExtensions(m_CurrentDirectory, 
                                                 importPath.string(), 
                                                 extensions);
        } else {
            // 有后缀名，直接查找
            resolvedPath = FindFile(m_CurrentDirectory, importPath.string());
        }
    } else {
        // 绝对路径
        if (fs::exists(importPath) && fs::is_regular_file(importPath)) {
            resolvedPath = importPath.string();
        }
    }
    
    if (resolvedPath.empty()) {
        ReportError(import, "找不到文件: " + import.path);
        return false;
    }
    
    import.resolvedPath = resolvedPath;
    import.isResolved = true;
    return true;
}

bool ImportResolver::ResolveChtl(ImportInfo& import) {
    // 根据目标规划文档第77-82行和第91-97行
    fs::path importPath(import.path);
    std::string resolvedPath;
    
    // 处理通配符导入
    if (import.path.find('*') != std::string::npos || 
        import.path.find("/*") != std::string::npos) {
        // 批量导入处理
        import.importAll = true;
        
        // 提取基础路径
        std::string basePath = import.path;
        std::string pattern;
        
        if (basePath.find("/*") != std::string::npos) {
            basePath = basePath.substr(0, basePath.find("/*"));
            pattern = basePath.substr(basePath.find("/*") + 2);
        } else if (basePath.find(".*") != std::string::npos) {
            basePath = basePath.substr(0, basePath.find(".*"));
            pattern = basePath.substr(basePath.find(".*") + 2);
        }
        
        fs::path baseDir = fs::path(basePath);
        if (baseDir.is_relative()) {
            baseDir = m_CurrentDirectory / baseDir;
        }
        
        if (!fs::exists(baseDir) || !fs::is_directory(baseDir)) {
            ReportError(import, "找不到目录: " + baseDir.string());
            return false;
        }
        
        import.resolvedPath = baseDir.string();
        import.isResolved = true;
        return true;
    }
    
    // 处理子模块导入 (Chtholly.Space 或 Chtholly/Space)
    std::string moduleName = import.path;
    std::replace(moduleName.begin(), moduleName.end(), '.', '/');
    
    // 搜索顺序
    std::vector<fs::path> searchPaths;
    
    // 1. 官方模块路径
    if (!m_CompilerModulePath.empty()) {
        searchPaths.push_back(fs::path(m_CompilerModulePath));
    }
    
    // 2. 当前目录的module文件夹
    searchPaths.push_back(m_CurrentDirectory / "module");
    
    // 3. 当前目录
    searchPaths.push_back(m_CurrentDirectory);
    
    // 根据是否有后缀名决定搜索策略
    if (!importPath.has_extension()) {
        // 没有后缀名，搜索.cmod和.chtl，cmod优先
        for (const auto& searchPath : searchPaths) {
            // 先找.cmod
            resolvedPath = FindFile(searchPath, moduleName + ".cmod");
            if (!resolvedPath.empty()) break;
            
            // 再找.chtl
            resolvedPath = FindFile(searchPath, moduleName + ".chtl");
            if (!resolvedPath.empty()) break;
        }
    } else {
        // 有后缀名，直接搜索
        for (const auto& searchPath : searchPaths) {
            resolvedPath = FindFile(searchPath, import.path);
            if (!resolvedPath.empty()) break;
        }
    }
    
    // 如果是具体路径
    if (resolvedPath.empty() && importPath.is_absolute()) {
        if (fs::exists(importPath) && fs::is_regular_file(importPath)) {
            resolvedPath = importPath.string();
        } else if (fs::is_directory(importPath)) {
            ReportError(import, "不能导入目录，需要指定具体文件: " + import.path);
            return false;
        }
    }
    
    if (resolvedPath.empty()) {
        ReportError(import, "找不到CHTL模块: " + import.path);
        return false;
    }
    
    import.resolvedPath = resolvedPath;
    import.isResolved = true;
    return true;
}

bool ImportResolver::ResolveCJmod(ImportInfo& import) {
    // 根据目标规划文档第84-89行，与Cmod使用同种路径搜索策略
    fs::path importPath(import.path);
    std::string resolvedPath;
    
    // 搜索顺序
    std::vector<fs::path> searchPaths;
    
    // 1. 官方模块路径
    if (!m_CompilerModulePath.empty()) {
        searchPaths.push_back(fs::path(m_CompilerModulePath));
    }
    
    // 2. 当前目录的module文件夹
    searchPaths.push_back(m_CurrentDirectory / "module");
    
    // 3. 当前目录
    searchPaths.push_back(m_CurrentDirectory);
    
    // 根据是否有后缀名决定搜索策略
    if (!importPath.has_extension()) {
        // 没有后缀名，搜索.cjmod
        for (const auto& searchPath : searchPaths) {
            resolvedPath = FindFile(searchPath, import.path + ".cjmod");
            if (!resolvedPath.empty()) break;
        }
    } else {
        // 有后缀名，直接搜索
        for (const auto& searchPath : searchPaths) {
            resolvedPath = FindFile(searchPath, import.path);
            if (!resolvedPath.empty()) break;
        }
    }
    
    // 如果是具体路径
    if (resolvedPath.empty() && importPath.is_absolute()) {
        if (fs::exists(importPath) && fs::is_regular_file(importPath)) {
            resolvedPath = importPath.string();
        } else if (fs::is_directory(importPath)) {
            ReportError(import, "不能导入目录，需要指定具体文件: " + import.path);
            return false;
        }
    }
    
    if (resolvedPath.empty()) {
        ReportError(import, "找不到CJmod模块: " + import.path);
        return false;
    }
    
    import.resolvedPath = resolvedPath;
    import.isResolved = true;
    return true;
}

bool ImportResolver::ResolveCustomOrTemplate(ImportInfo& import) {
    // 自定义元素、样式、变量和模板都从CHTL文件导入
    // 使用与CHTL相同的路径解析策略
    ImportInfo chtlImport = import;
    chtlImport.type = ImportType::CHTL;
    
    if (!ResolveChtl(chtlImport)) {
        return false;
    }
    
    import.resolvedPath = chtlImport.resolvedPath;
    import.isResolved = true;
    return true;
}

bool ImportResolver::ResolveOrigin(ImportInfo& import) {
    // 原始嵌入从CHTL文件导入
    return ResolveCustomOrTemplate(import);
}

bool ImportResolver::ResolveConfig(ImportInfo& import) {
    // 配置从CHTL文件导入
    return ResolveCustomOrTemplate(import);
}

std::string ImportResolver::FindFileWithExtensions(const fs::path& dir,
                                                  const std::string& basename,
                                                  const std::vector<std::string>& extensions) {
    for (const auto& ext : extensions) {
        std::string filename = basename + ext;
        std::string result = FindFile(dir, filename);
        if (!result.empty()) {
            return result;
        }
    }
    return "";
}

std::string ImportResolver::FindFile(const fs::path& dir,
                                    const std::string& filename) {
    fs::path fullPath = dir / filename;
    
    if (fs::exists(fullPath) && fs::is_regular_file(fullPath)) {
        return fullPath.string();
    }
    
    return "";
}

std::vector<std::string> ImportResolver::FindAllFiles(const fs::path& dir,
                                                     const std::string& pattern) {
    std::vector<std::string> results;
    
    if (!fs::exists(dir) || !fs::is_directory(dir)) {
        return results;
    }
    
    // 将通配符模式转换为正则表达式
    std::string regexPattern = pattern;
    regexPattern = std::regex_replace(regexPattern, std::regex("\\."), "\\.");
    regexPattern = std::regex_replace(regexPattern, std::regex("\\*"), ".*");
    std::regex re(regexPattern);
    
    for (const auto& entry : fs::directory_iterator(dir)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            if (std::regex_match(filename, re)) {
                results.push_back(entry.path().string());
            }
        }
    }
    
    return results;
}

void ImportResolver::ReportError(const ImportInfo& import, const std::string& message) {
    std::string error = "[Import错误 " + import.sourceFile + ":" +
                       std::to_string(import.line) + ":" +
                       std::to_string(import.column) + "] " + message;
    m_Errors.push_back(error);
    utils::Logger::GetInstance().Error(error);
}

// ImportDependencyGraph 实现

ImportDependencyGraph::ImportDependencyGraph() {}

ImportDependencyGraph::~ImportDependencyGraph() = default;

bool ImportDependencyGraph::AddDependency(const std::string& from, const std::string& to) {
    // 规范化路径
    std::string normalizedFrom = ImportManager::NormalizePath(from);
    std::string normalizedTo = ImportManager::NormalizePath(to);
    
    // 自引用检查
    if (normalizedFrom == normalizedTo) {
        return false;
    }
    
    // 添加边
    m_Graph[normalizedFrom].insert(normalizedTo);
    
    // 确保目标节点存在
    if (m_Graph.find(normalizedTo) == m_Graph.end()) {
        m_Graph[normalizedTo] = {};
    }
    
    // 检查是否产生循环
    if (HasCycle()) {
        // 回滚
        m_Graph[normalizedFrom].erase(normalizedTo);
        return false;
    }
    
    return true;
}

bool ImportDependencyGraph::HasCycle() const {
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> recStack;
    m_CyclePath.clear();
    
    for (const auto& pair : m_Graph) {
        if (visited.find(pair.first) == visited.end()) {
            std::vector<std::string> path;
            if (DFSCycleDetection(pair.first, visited, recStack, path)) {
                m_CyclePath = path;
                return true;
            }
        }
    }
    
    return false;
}

bool ImportDependencyGraph::DFSCycleDetection(const std::string& node,
                                            std::unordered_set<std::string>& visited,
                                            std::unordered_set<std::string>& recStack,
                                            std::vector<std::string>& path) const {
    visited.insert(node);
    recStack.insert(node);
    path.push_back(node);
    
    auto it = m_Graph.find(node);
    if (it != m_Graph.end()) {
        for (const auto& neighbor : it->second) {
            if (recStack.find(neighbor) != recStack.end()) {
                // 找到循环，记录路径
                path.push_back(neighbor);
                return true;
            }
            
            if (visited.find(neighbor) == visited.end()) {
                if (DFSCycleDetection(neighbor, visited, recStack, path)) {
                    return true;
                }
            }
        }
    }
    
    recStack.erase(node);
    path.pop_back();
    return false;
}

std::vector<std::string> ImportDependencyGraph::GetTopologicalOrder() const {
    if (HasCycle()) {
        return {};
    }
    
    // 计算入度
    std::unordered_map<std::string, int> inDegree;
    for (const auto& pair : m_Graph) {
        if (inDegree.find(pair.first) == inDegree.end()) {
            inDegree[pair.first] = 0;
        }
        for (const auto& neighbor : pair.second) {
            inDegree[neighbor]++;
        }
    }
    
    // 拓扑排序
    std::vector<std::string> result;
    std::vector<std::string> queue;
    
    // 找到所有入度为0的节点
    for (const auto& pair : inDegree) {
        if (pair.second == 0) {
            queue.push_back(pair.first);
        }
    }
    
    while (!queue.empty()) {
        std::string node = queue.back();
        queue.pop_back();
        result.push_back(node);
        
        auto it = m_Graph.find(node);
        if (it != m_Graph.end()) {
            for (const auto& neighbor : it->second) {
                inDegree[neighbor]--;
                if (inDegree[neighbor] == 0) {
                    queue.push_back(neighbor);
                }
            }
        }
    }
    
    return result;
}

bool ImportDependencyGraph::IsImported(const std::string& file) const {
    std::string normalized = ImportManager::NormalizePath(file);
    return m_ImportedFiles.find(normalized) != m_ImportedFiles.end();
}

void ImportDependencyGraph::MarkAsImported(const std::string& file) {
    std::string normalized = ImportManager::NormalizePath(file);
    m_ImportedFiles.insert(normalized);
}

std::vector<std::string> ImportDependencyGraph::GetCyclePath() const {
    return m_CyclePath;
}

// ImportManager 实现

ImportManager::ImportManager() {
    m_Resolver = std::make_unique<ImportResolver>();
    m_DependencyGraph = std::make_unique<ImportDependencyGraph>();
}

ImportManager::~ImportManager() = default;

void ImportManager::SetCompilerModulePath(const std::string& path) {
    m_Resolver->SetCompilerModulePath(path);
}

bool ImportManager::ProcessImports(std::vector<ImportInfo>& imports,
                                  const std::string& currentFile) {
    // 设置当前文件
    m_Resolver->SetCurrentFile(currentFile);
    
    // 规范化当前文件路径
    std::string normalizedCurrent = NormalizePath(currentFile);
    
    // 解析所有导入路径
    if (!m_Resolver->ResolveImports(imports)) {
        for (const auto& error : m_Resolver->GetErrors()) {
            m_Errors.push_back(error);
        }
        return false;
    }
    
    // 处理每个导入
    for (const auto& import : imports) {
        if (!import.isResolved) continue;
        
        std::string normalizedImport = NormalizePath(import.resolvedPath);
        
        // 检查重复导入
        if (m_ProcessedImports.find(normalizedImport) != m_ProcessedImports.end()) {
            utils::Logger::GetInstance().Debug("跳过重复导入: " + normalizedImport);
            continue;
        }
        
        // 添加依赖关系
        if (!m_DependencyGraph->AddDependency(normalizedCurrent, normalizedImport)) {
            ReportError("循环依赖: " + normalizedCurrent + " -> " + normalizedImport);
            return false;
        }
        
        // 标记为已导入
        m_DependencyGraph->MarkAsImported(normalizedImport);
        m_ProcessedImports[normalizedImport].push_back(import);
    }
    
    return true;
}

bool ImportManager::HasCircularDependency() const {
    return m_DependencyGraph->HasCycle();
}

std::string ImportManager::GetCircularDependencyInfo() const {
    if (!HasCircularDependency()) {
        return "";
    }
    
    auto cyclePath = m_DependencyGraph->GetCyclePath();
    std::string info = "循环依赖路径: ";
    
    for (size_t i = 0; i < cyclePath.size(); ++i) {
        info += cyclePath[i];
        if (i < cyclePath.size() - 1) {
            info += " -> ";
        }
    }
    
    return info;
}

std::vector<std::string> ImportManager::GetImportedFiles() const {
    return m_DependencyGraph->GetTopologicalOrder();
}

std::string ImportManager::NormalizePath(const std::string& path) {
    try {
        fs::path p(path);
        
        // 转换为绝对路径
        if (p.is_relative()) {
            p = fs::absolute(p);
        }
        
        // 规范化路径（解析.和..）
        p = p.lexically_normal();
        
        return p.string();
    } catch (const std::exception& e) {
        utils::Logger::GetInstance().Warning("路径规范化失败: " + path + " - " + e.what());
        return path;
    }
}

void ImportManager::ReportError(const std::string& message) {
    m_Errors.push_back(message);
    utils::Logger::GetInstance().Error(message);
}

} // namespace import
} // namespace chtl