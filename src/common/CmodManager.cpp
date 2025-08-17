#include "CmodManager.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <regex>
#include <algorithm>
#include <set>

namespace chtl {

// 静态常量定义
const std::string CmodManager::CMOD_EXTENSION = ".cmod";
const std::string CmodManager::CHTL_EXTENSION = ".chtl";
const std::string CmodManager::SRC_DIR = "src";
const std::string CmodManager::INFO_DIR = "info";
const std::string CmodManager::INFO_SECTION = "[Info]";
const std::string CmodManager::EXPORT_SECTION = "[Export]";

CmodManager::CmodManager() {
    // 添加默认搜索路径
    search_paths_ = CmodUtils::getStandardSearchPaths();
}

CmodManager::~CmodManager() = default;

bool CmodManager::packCmod(const std::string& source_directory, const std::string& output_cmod_path) {
    // 分析Cmod结构
    CmodStructure structure;
    if (!analyzeCmodStructure(source_directory, structure)) {
        return false;
    }
    
    // 验证结构
    if (!validateFileStructure(source_directory, structure)) {
        return false;
    }
    
    return packCmodFromStructure(structure, source_directory, output_cmod_path);
}

bool CmodManager::packCmodFromStructure(const CmodStructure& structure, const std::string& base_path, 
                                       const std::string& output_cmod_path) {
    SimpleZip zip;
    
    try {
        // 添加src目录下的所有文件
        std::string src_path = joinPath(base_path, structure.src_dir);
        if (directoryExists(src_path)) {
            if (!zip.addDirectory("src", src_path)) {
                setError("添加src目录失败: " + zip.getLastError());
                return false;
            }
        }
        
        // 添加info目录下的所有文件
        std::string info_path = joinPath(base_path, structure.info_dir);
        if (directoryExists(info_path)) {
            if (!zip.addDirectory("info", info_path)) {
                setError("添加info目录失败: " + zip.getLastError());
                return false;
            }
        }
        
        // 保存ZIP文件
        if (!zip.saveToFile(output_cmod_path)) {
            setError("保存Cmod文件失败: " + zip.getLastError());
            return false;
        }
        
        std::cout << "成功打包Cmod: " << output_cmod_path << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        setError("打包Cmod时发生错误: " + std::string(e.what()));
        return false;
    }
}

bool CmodManager::unpackCmod(const std::string& cmod_path, const std::string& output_directory) {
    SimpleZip zip;
    
    if (!zip.loadFromFile(cmod_path)) {
        setError("无法加载Cmod文件: " + zip.getLastError());
        return false;
    }
    
    if (!zip.extractAll(output_directory)) {
        setError("解包Cmod文件失败: " + zip.getLastError());
        return false;
    }
    
    std::cout << "成功解包Cmod到: " << output_directory << std::endl;
    return true;
}

bool CmodManager::validateCmodStructure(const std::string& directory_path) {
    CmodStructure structure;
    if (!analyzeCmodStructure(directory_path, structure)) {
        return false;
    }
    
    return validateFileStructure(directory_path, structure);
}

bool CmodManager::validateCmodFile(const std::string& cmod_path) {
    SimpleZip zip;
    
    if (!zip.loadFromFile(cmod_path)) {
        setError("无法加载Cmod文件: " + zip.getLastError());
        return false;
    }
    
    auto files = zip.getFileList();
    
    // 检查必要的目录结构
    bool has_src = false, has_info = false;
    std::string main_module_name;
    
    for (const std::string& file : files) {
        if (file.substr(0, 4) == "src/") {
            has_src = true;
            if (file.size() >= CHTL_EXTENSION.size() && 
                file.substr(file.size() - CHTL_EXTENSION.size()) == CHTL_EXTENSION) {
                std::string filename = getFileName(file);
                if (main_module_name.empty()) {
                    main_module_name = filename.substr(0, filename.length() - CHTL_EXTENSION.length());
                }
            }
        }
        if (file.substr(0, 5) == "info/") {
            has_info = true;
        }
    }
    
    if (!has_src) {
        setError("Cmod文件缺少src目录");
        return false;
    }
    
    if (!has_info) {
        setError("Cmod文件缺少info目录");
        return false;
    }
    
    // 检查主模块信息文件
    if (!main_module_name.empty()) {
        std::string info_file = "info/" + main_module_name + CHTL_EXTENSION;
        if (!zip.hasFile(info_file)) {
            // 检查是否有任何info文件
            bool found_any_info = false;
            for (const std::string& file : files) {
                if (file.substr(0, 5) == "info/" && 
                    file.size() >= CHTL_EXTENSION.size() && 
                    file.substr(file.size() - CHTL_EXTENSION.size()) == CHTL_EXTENSION) {
                    found_any_info = true;
                    break;
                }
            }
            if (!found_any_info) {
                setError("缺少模块信息文件");
                return false;
            }
        }
    }
    
    return true;
}

bool CmodManager::parseCmodInfo(const std::string& info_file_content, CmodInfo& info) {
    std::istringstream stream(info_file_content);
    std::string line;
    bool in_info_section = false;
    bool in_export_section = false;
    
    while (std::getline(stream, line)) {
        // 去除前后空白
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        // 跳过空行和注释
        if (line.empty() || (line.size() >= 2 && line.substr(0, 2) == "//")) {
            continue;
        }
        
        // 检查节标记
        if (line == INFO_SECTION) {
            in_info_section = true;
            in_export_section = false;
            continue;
        }
        
        if (line == EXPORT_SECTION) {
            in_info_section = false;
            in_export_section = true;
            continue;
        }
        
        // 检查节结束
        if (line == "}") {
            in_info_section = false;
            in_export_section = false;
            continue;
        }
        
        // 解析信息节
        if (in_info_section && line != "{") {
            size_t eq_pos = line.find('=');
            if (eq_pos != std::string::npos) {
                std::string key = line.substr(0, eq_pos);
                std::string value = line.substr(eq_pos + 1);
                
                // 去除空白和引号
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
                
                // 去除分号
                if (!value.empty() && value.back() == ';') {
                    value.pop_back();
                    // 再次去除空白
                    value.erase(value.find_last_not_of(" \t") + 1);
                }
                
                // 去除引号
                if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                    value = value.substr(1, value.length() - 2);
                }
                
                // 赋值到结构体
                if (key == "name") info.name = value;
                else if (key == "version") info.version = value;
                else if (key == "description") info.description = value;
                else if (key == "author") info.author = value;
                else if (key == "license") info.license = value;
                else if (key == "dependencies") info.dependencies = value;
                else if (key == "category") info.category = value;
                else if (key == "minCHTLVersion") info.min_chtl_version = value;
                else if (key == "maxCHTLVersion") info.max_chtl_version = value;
            }
        }
        
        // 解析导出节
        if (in_export_section && line != "{") {
            if (line.find("@Style") != std::string::npos) {
                auto styles = parseExportList(line, "@Style");
                info.exported_styles.insert(info.exported_styles.end(), styles.begin(), styles.end());
            }
            else if (line.find("@Element") != std::string::npos) {
                auto elements = parseExportList(line, "@Element");
                info.exported_elements.insert(info.exported_elements.end(), elements.begin(), elements.end());
            }
            else if (line.find("@Var") != std::string::npos) {
                auto vars = parseExportList(line, "@Var");
                info.exported_vars.insert(info.exported_vars.end(), vars.begin(), vars.end());
            }
        }
    }
    
    // 验证必要字段
    if (info.name.empty()) {
        setError("Cmod信息缺少name字段");
        return false;
    }
    
    if (info.version.empty()) {
        setError("Cmod信息缺少version字段");
        return false;
    }
    
    return true;
}

bool CmodManager::extractCmodInfo(const std::string& cmod_path, CmodInfo& info) {
    SimpleZip zip;
    
    if (!zip.loadFromFile(cmod_path)) {
        setError("无法加载Cmod文件: " + zip.getLastError());
        return false;
    }
    
    // 查找主模块信息文件
    auto files = zip.getFileList();
    std::string info_file;
    
    for (const std::string& file : files) {
        if (file.substr(0, 5) == "info/" && 
            file.size() >= CHTL_EXTENSION.size() && 
            file.substr(file.size() - CHTL_EXTENSION.size()) == CHTL_EXTENSION) {
            info_file = file;
            break;
        }
    }
    
    if (info_file.empty()) {
        setError("未找到模块信息文件");
        return false;
    }
    
    std::string content;
    if (!zip.extractFile(info_file, content)) {
        setError("无法提取模块信息文件: " + zip.getLastError());
        return false;
    }
    
    return parseCmodInfo(content, info);
}

bool CmodManager::analyzeCmodStructure(const std::string& directory_path, CmodStructure& structure) {
    if (!directoryExists(directory_path)) {
        setError("目录不存在: " + directory_path);
        return false;
    }
    
    // 获取目录名作为主模块名
    structure.main_module_name = getFileName(directory_path);
    structure.src_dir = SRC_DIR;
    structure.info_dir = INFO_DIR;
    structure.has_main_file = false;
    
    // 检查src目录
    std::string src_path = joinPath(directory_path, SRC_DIR);
    if (directoryExists(src_path)) {
        auto src_files = listDirectory(src_path);
        for (const std::string& file : src_files) {
            std::string full_path = joinPath(src_path, file);
            if (std::filesystem::is_regular_file(full_path) && 
                file.size() >= CHTL_EXTENSION.size() && 
                file.substr(file.size() - CHTL_EXTENSION.size()) == CHTL_EXTENSION) {
                structure.source_files.push_back(file);
                
                // 检查是否是主模块文件
                std::string base_name = file.substr(0, file.length() - CHTL_EXTENSION.length());
                if (base_name == structure.main_module_name) {
                    structure.has_main_file = true;
                }
            }
            else if (std::filesystem::is_directory(full_path)) {
                structure.submodules.push_back(file);
            }
        }
    }
    
    // 检查info目录
    std::string info_path = joinPath(directory_path, INFO_DIR);
    if (!directoryExists(info_path)) {
        setError("缺少info目录");
        return false;
    }
    
    // 检查主模块信息文件
    std::string main_info_file = joinPath(info_path, structure.main_module_name + CHTL_EXTENSION);
    if (!fileExists(main_info_file)) {
        setError("缺少主模块信息文件: " + main_info_file);
        return false;
    }
    
    return true;
}

std::vector<std::string> CmodManager::getSubmodules(const std::string& cmod_path) {
    SimpleZip zip;
    std::vector<std::string> submodules;
    
    if (!zip.loadFromFile(cmod_path)) {
        return submodules;
    }
    
    auto files = zip.getFileList();
    std::set<std::string> submodule_set;
    
    for (const std::string& file : files) {
        if (file.size() > 4 && file.substr(0, 4) == "src/" && file.find('/', 4) != std::string::npos) {
            // 提取子模块名
            size_t start = 4; // "src/"的长度
            size_t end = file.find('/', start);
            if (end != std::string::npos) {
                std::string submodule = file.substr(start, end - start);
                submodule_set.insert(submodule);
            }
        }
    }
    
    submodules.assign(submodule_set.begin(), submodule_set.end());
    return submodules;
}

bool CmodManager::hasSubmodule(const std::string& cmod_path, const std::string& submodule_name) {
    auto submodules = getSubmodules(cmod_path);
    return std::find(submodules.begin(), submodules.end(), submodule_name) != submodules.end();
}

std::vector<std::string> CmodManager::listCmodFiles(const std::string& cmod_path) {
    SimpleZip zip;
    
    if (!zip.loadFromFile(cmod_path)) {
        return {};
    }
    
    return zip.getFileList();
}

bool CmodManager::extractCmodFile(const std::string& cmod_path, const std::string& file_path, std::string& content) {
    SimpleZip zip;
    
    if (!zip.loadFromFile(cmod_path)) {
        setError("无法加载Cmod文件: " + zip.getLastError());
        return false;
    }
    
    if (!zip.extractFile(file_path, content)) {
        setError("无法提取文件: " + zip.getLastError());
        return false;
    }
    
    return true;
}

std::string CmodManager::resolveCmodPath(const std::string& module_name, const std::vector<std::string>& search_paths) {
    for (const std::string& search_path : search_paths) {
        // 尝试.cmod文件
        std::string cmod_path = joinPath(search_path, module_name + CMOD_EXTENSION);
        if (fileExists(cmod_path)) {
            return cmod_path;
        }
        
        // 尝试.chtl文件
        std::string chtl_path = joinPath(search_path, module_name + CHTL_EXTENSION);
        if (fileExists(chtl_path)) {
            return chtl_path;
        }
        
        // 尝试目录
        std::string dir_path = joinPath(search_path, module_name);
        if (directoryExists(dir_path)) {
            return dir_path;
        }
    }
    
    return "";
}

std::string CmodManager::resolveSubmodulePath(const std::string& main_module, const std::string& submodule) {
    auto [module_name, sub_name] = CmodUtils::parseModuleName(main_module + "." + submodule);
    return resolveCmodPath(module_name, search_paths_);
}

std::vector<std::string> CmodManager::parseDependencies(const std::string& dependencies_str) {
    std::vector<std::string> dependencies;
    
    if (dependencies_str.empty()) {
        return dependencies;
    }
    
    std::istringstream stream(dependencies_str);
    std::string dependency;
    
    while (std::getline(stream, dependency, ',')) {
        // 去除前后空白
        dependency.erase(0, dependency.find_first_not_of(" \t"));
        dependency.erase(dependency.find_last_not_of(" \t") + 1);
        
        if (!dependency.empty()) {
            dependencies.push_back(dependency);
        }
    }
    
    return dependencies;
}

bool CmodManager::checkDependencies(const CmodInfo& info, const std::vector<std::string>& available_modules) {
    auto dependencies = parseDependencies(info.dependencies);
    
    for (const std::string& dependency : dependencies) {
        if (std::find(available_modules.begin(), available_modules.end(), dependency) == available_modules.end()) {
            setError("缺少依赖模块: " + dependency);
            return false;
        }
    }
    
    return true;
}

bool CmodManager::isVersionCompatible(const std::string& required_version, const std::string& available_version) {
    return CmodUtils::compareVersions(available_version, required_version) >= 0;
}

bool CmodManager::isCHTLVersionCompatible(const std::string& min_version, const std::string& max_version, 
                                         const std::string& current_version) {
    if (!min_version.empty()) {
        if (CmodUtils::compareVersions(current_version, min_version) < 0) {
            return false;
        }
    }
    
    if (!max_version.empty()) {
        if (CmodUtils::compareVersions(current_version, max_version) > 0) {
            return false;
        }
    }
    
    return true;
}

void CmodManager::addSearchPath(const std::string& path) {
    if (std::find(search_paths_.begin(), search_paths_.end(), path) == search_paths_.end()) {
        search_paths_.push_back(path);
    }
}

void CmodManager::removeSearchPath(const std::string& path) {
    search_paths_.erase(
        std::remove(search_paths_.begin(), search_paths_.end(), path),
        search_paths_.end()
    );
}

void CmodManager::clearCache() {
    info_cache_.clear();
}

bool CmodManager::isCached(const std::string& cmod_path) {
    return info_cache_.find(cmod_path) != info_cache_.end();
}

// 私有方法实现
bool CmodManager::validateCmodName(const std::string& name) {
    if (name.empty()) return false;
    
    // 检查字符是否合法
    for (char c : name) {
        if (!std::isalnum(c) && c != '_' && c != '-') {
            return false;
        }
    }
    
    return true;
}

bool CmodManager::validateCmodVersion(const std::string& version) {
    // 简单的版本号验证 (x.y.z格式)
    std::regex version_regex(R"(\d+\.\d+\.\d+)");
    return std::regex_match(version, version_regex);
}

bool CmodManager::validateFileStructure(const std::string& base_path, const CmodStructure& structure) {
    // 验证主模块名
    if (!validateCmodName(structure.main_module_name)) {
        setError("无效的模块名: " + structure.main_module_name);
        return false;
    }
    
    // 验证src目录
    std::string src_path = joinPath(base_path, structure.src_dir);
    if (!directoryExists(src_path)) {
        setError("缺少src目录");
        return false;
    }
    
    // 验证info目录
    std::string info_path = joinPath(base_path, structure.info_dir);
    if (!directoryExists(info_path)) {
        setError("缺少info目录");
        return false;
    }
    
    // 验证主模块信息文件
    std::string main_info_file = joinPath(info_path, structure.main_module_name + CHTL_EXTENSION);
    if (!fileExists(main_info_file)) {
        setError("缺少主模块信息文件");
        return false;
    }
    
    // 验证信息文件内容
    std::string content;
    if (!readFileContent(main_info_file, content)) {
        setError("无法读取主模块信息文件");
        return false;
    }
    
    CmodInfo info;
    if (!parseCmodInfo(content, info)) {
        return false;
    }
    
    return true;
}

// 文件系统操作实现
bool CmodManager::directoryExists(const std::string& path) {
    return std::filesystem::exists(path) && std::filesystem::is_directory(path);
}

bool CmodManager::fileExists(const std::string& path) {
    return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
}

std::vector<std::string> CmodManager::listDirectory(const std::string& path) {
    std::vector<std::string> files;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            files.push_back(entry.path().filename().string());
        }
    } catch (const std::exception&) {
        // 忽略错误
    }
    
    return files;
}

bool CmodManager::readFileContent(const std::string& file_path, std::string& content) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        return false;
    }
    
    std::ostringstream stream;
    stream << file.rdbuf();
    content = stream.str();
    
    return true;
}

// 路径处理实现
std::string CmodManager::joinPath(const std::string& base, const std::string& relative) {
    std::filesystem::path result = std::filesystem::path(base) / relative;
    return result.string();
}

std::string CmodManager::getFileName(const std::string& path) {
    return std::filesystem::path(path).filename().string();
}

std::string CmodManager::getFileExtension(const std::string& path) {
    return std::filesystem::path(path).extension().string();
}

std::string CmodManager::getDirectoryName(const std::string& path) {
    return std::filesystem::path(path).parent_path().string();
}

// 解析辅助实现
std::string CmodManager::extractInfoValue(const std::string& content, const std::string& key) {
    // 简化实现：查找key = "value"的模式
    size_t key_pos = content.find(key);
    if (key_pos == std::string::npos) return "";
    
    size_t eq_pos = content.find('=', key_pos);
    if (eq_pos == std::string::npos) return "";
    
    size_t quote1 = content.find('"', eq_pos);
    if (quote1 == std::string::npos) return "";
    
    size_t quote2 = content.find('"', quote1 + 1);
    if (quote2 == std::string::npos) return "";
    
    return content.substr(quote1 + 1, quote2 - quote1 - 1);
}

std::vector<std::string> CmodManager::parseExportList(const std::string& content, const std::string& type) {
    std::vector<std::string> result;
    
    // 查找类型标记后的符号列表
    size_t type_pos = content.find(type);
    if (type_pos == std::string::npos) {
        return result;
    }
    
    // 提取符号列表部分
    std::string symbols_part = content.substr(type_pos + type.length());
    
    // 去除分号
    size_t semicolon_pos = symbols_part.find(';');
    if (semicolon_pos != std::string::npos) {
        symbols_part = symbols_part.substr(0, semicolon_pos);
    }
    
    // 分割符号
    std::istringstream stream(symbols_part);
    std::string symbol;
    
    while (std::getline(stream, symbol, ',')) {
        // 去除前后空白
        symbol.erase(0, symbol.find_first_not_of(" \t"));
        symbol.erase(symbol.find_last_not_of(" \t") + 1);
        
        if (!symbol.empty()) {
            result.push_back(symbol);
        }
    }
    
    return result;
}

void CmodManager::setError(const std::string& error) {
    last_error_ = error;
    std::cerr << "CmodManager错误: " << error << std::endl;
}

// CmodUtils命名空间实现
namespace CmodUtils {

std::vector<std::string> getStandardSearchPaths() {
    std::vector<std::string> paths;
    
    // 添加官方模块路径
    paths.push_back(getOfficialModulePath());
    
    // 添加本地模块路径
    paths.push_back(getLocalModulePath());
    
    // 添加当前目录
    paths.push_back(".");
    
    return paths;
}

std::string getOfficialModulePath() {
    // 官方模块路径通常在编译器安装目录下的modules文件夹
    return "modules";
}

std::string getLocalModulePath(const std::string& base_path) {
    return base_path + "/modules";
}

std::pair<std::string, std::string> parseModuleName(const std::string& full_name) {
    size_t dot_pos = full_name.find('.');
    if (dot_pos == std::string::npos) {
        return {full_name, ""};
    }
    
    return {full_name.substr(0, dot_pos), full_name.substr(dot_pos + 1)};
}

int compareVersions(const std::string& version1, const std::string& version2) {
    std::vector<int> v1_parts, v2_parts;
    
    // 解析版本1
    std::istringstream stream1(version1);
    std::string part;
    while (std::getline(stream1, part, '.')) {
        v1_parts.push_back(std::stoi(part));
    }
    
    // 解析版本2
    std::istringstream stream2(version2);
    while (std::getline(stream2, part, '.')) {
        v2_parts.push_back(std::stoi(part));
    }
    
    // 比较版本号
    size_t max_parts = std::max(v1_parts.size(), v2_parts.size());
    
    for (size_t i = 0; i < max_parts; ++i) {
        int v1_part = (i < v1_parts.size()) ? v1_parts[i] : 0;
        int v2_part = (i < v2_parts.size()) ? v2_parts[i] : 0;
        
        if (v1_part < v2_part) return -1;
        if (v1_part > v2_part) return 1;
    }
    
    return 0;
}

bool matchesPattern(const std::string& path, const std::string& pattern) {
    // 简单的通配符匹配实现
    if (pattern == "*") return true;
    if (pattern.size() >= 2 && pattern.substr(pattern.size() - 2) == "/*") {
        std::string prefix = pattern.substr(0, pattern.length() - 2);
        return path.size() >= prefix.size() && path.substr(0, prefix.size()) == prefix;
    }
    
    return path == pattern;
}

} // namespace CmodUtils

} // namespace chtl