#include "CJmodManager.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <set>

namespace chtl {

// 静态常量定义
const std::string CJmodManager::CJMOD_EXTENSION = ".cjmod";
const std::string CJmodManager::CPP_EXTENSION = ".cpp";
const std::string CJmodManager::HEADER_EXTENSION = ".h";
const std::string CJmodManager::CHTL_EXTENSION = ".chtl";
const std::string CJmodManager::SRC_DIR = "src";
const std::string CJmodManager::INFO_DIR = "info";
const std::string CJmodManager::INFO_SECTION = "[Info]";
const std::string CJmodManager::OFFICIAL_PREFIX = "chtl::";

CJmodManager::CJmodManager() {
    // 添加默认搜索路径
    search_paths_ = CJmodUtils::getCJmodStandardSearchPaths();
}

CJmodManager::~CJmodManager() = default;

bool CJmodManager::packCJmod(const std::string& source_directory, const std::string& output_cjmod_path) {
    // 分析CJmod结构
    CJmodStructure structure;
    if (!analyzeCJmodStructure(source_directory, structure)) {
        return false;
    }
    
    // 验证结构
    if (!validateCJmodFileStructure(source_directory, structure)) {
        return false;
    }
    
    return packCJmodFromStructure(structure, source_directory, output_cjmod_path);
}

bool CJmodManager::packCJmodFromStructure(const CJmodStructure& structure, const std::string& base_path, 
                                         const std::string& output_cjmod_path) {
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
        
        // 保存CJmod文件
        if (!zip.saveToFile(output_cjmod_path)) {
            setError("保存CJmod文件失败: " + zip.getLastError());
            return false;
        }
        
        std::cout << "成功打包CJmod: " << output_cjmod_path << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        setError("打包CJmod时发生错误: " + std::string(e.what()));
        return false;
    }
}

bool CJmodManager::unpackCJmod(const std::string& cjmod_path, const std::string& output_directory) {
    SimpleZip zip;
    
    if (!zip.loadFromFile(cjmod_path)) {
        setError("无法加载CJmod文件: " + zip.getLastError());
        return false;
    }
    
    if (!zip.extractAll(output_directory)) {
        setError("解包CJmod文件失败: " + zip.getLastError());
        return false;
    }
    
    std::cout << "成功解包CJmod到: " << output_directory << std::endl;
    return true;
}

bool CJmodManager::validateCJmodStructure(const std::string& directory_path) {
    CJmodStructure structure;
    if (!analyzeCJmodStructure(directory_path, structure)) {
        return false;
    }
    
    return validateCJmodFileStructure(directory_path, structure);
}

bool CJmodManager::validateCJmodFile(const std::string& cjmod_path) {
    SimpleZip zip;
    
    if (!zip.loadFromFile(cjmod_path)) {
        setError("无法加载CJmod文件: " + zip.getLastError());
        return false;
    }
    
    auto files = zip.getFileList();
    
    // 检查必要的目录结构
    bool has_src = false, has_info = false;
    bool has_cpp = false;
    std::string main_module_name;
    
    for (const std::string& file : files) {
        if (file.size() > 4 && file.substr(0, 4) == "src/") {
            has_src = true;
            
            // 检查C++文件
            if (isCppFile(file)) {
                has_cpp = true;
            }
            
            // 检查头文件
            if (isHeaderFile(file)) {
                // 头文件存在但不强制要求
            }
            
            // 提取主模块名
            if (main_module_name.empty()) {
                std::string filename = getFileName(file);
                if (isCppFile(filename)) {
                    main_module_name = filename.substr(0, filename.length() - CPP_EXTENSION.length());
                }
            }
        }
        
        if (file.substr(0, 5) == "info/") {
            has_info = true;
        }
    }
    
    if (!has_src) {
        setError("CJmod文件缺少src目录");
        return false;
    }
    
    if (!has_info) {
        setError("CJmod文件缺少info目录");
        return false;
    }
    
    if (!has_cpp) {
        setError("CJmod文件缺少C++源文件");
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
                setError("缺少CJmod模块信息文件");
                return false;
            }
        }
    }
    
    return true;
}

bool CJmodManager::parseCJmodInfo(const std::string& info_file_content, CJmodInfo& info) {
    std::istringstream stream(info_file_content);
    std::string line;
    bool in_info_section = false;
    
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
            continue;
        }
        
        // 检查节结束
        if (line == "}") {
            in_info_section = false;
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
                else if (key == "apiVersion") info.api_version = value;
                else if (key == "entryPoint") info.entry_point = value;
            }
        }
    }
    
    // 验证必要字段
    if (info.name.empty()) {
        setError("CJmod信息缺少name字段");
        return false;
    }
    
    if (info.version.empty()) {
        setError("CJmod信息缺少version字段");
        return false;
    }
    
    return true;
}

bool CJmodManager::extractCJmodInfo(const std::string& cjmod_path, CJmodInfo& info) {
    SimpleZip zip;
    
    if (!zip.loadFromFile(cjmod_path)) {
        setError("无法加载CJmod文件: " + zip.getLastError());
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
        setError("未找到CJmod模块信息文件");
        return false;
    }
    
    std::string content;
    if (!zip.extractFile(info_file, content)) {
        setError("无法提取CJmod模块信息文件: " + zip.getLastError());
        return false;
    }
    
    return parseCJmodInfo(content, info);
}

bool CJmodManager::analyzeCJmodStructure(const std::string& directory_path, CJmodStructure& structure) {
    if (!directoryExists(directory_path)) {
        setError("目录不存在: " + directory_path);
        return false;
    }
    
    // 获取目录名作为主模块名
    structure.main_module_name = getFileName(directory_path);
    structure.src_dir = SRC_DIR;
    structure.info_dir = INFO_DIR;
    structure.has_main_cpp = false;
    structure.has_main_header = false;
    
    // 检查src目录
    std::string src_path = joinPath(directory_path, SRC_DIR);
    if (directoryExists(src_path)) {
        auto src_files = listDirectory(src_path);
        for (const std::string& file : src_files) {
            std::string full_path = joinPath(src_path, file);
            
            if (std::filesystem::is_regular_file(full_path)) {
                if (isCppFile(file)) {
                    structure.cpp_files.push_back(file);
                    
                    // 检查是否是主模块C++文件
                    std::string base_name = file.substr(0, file.length() - CPP_EXTENSION.length());
                    if (base_name == structure.main_module_name) {
                        structure.has_main_cpp = true;
                    }
                }
                else if (isHeaderFile(file)) {
                    structure.header_files.push_back(file);
                    
                    // 检查是否是主模块头文件
                    std::string base_name = file.substr(0, file.length() - HEADER_EXTENSION.length());
                    if (base_name == structure.main_module_name) {
                        structure.has_main_header = true;
                    }
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

std::vector<std::string> CJmodManager::getCJmodSubmodules(const std::string& cjmod_path) {
    SimpleZip zip;
    std::vector<std::string> submodules;
    
    if (!zip.loadFromFile(cjmod_path)) {
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

bool CJmodManager::hasCJmodSubmodule(const std::string& cjmod_path, const std::string& submodule_name) {
    auto submodules = getCJmodSubmodules(cjmod_path);
    return std::find(submodules.begin(), submodules.end(), submodule_name) != submodules.end();
}

std::vector<std::string> CJmodManager::listCJmodFiles(const std::string& cjmod_path) {
    SimpleZip zip;
    
    if (!zip.loadFromFile(cjmod_path)) {
        return {};
    }
    
    return zip.getFileList();
}

bool CJmodManager::extractCJmodFile(const std::string& cjmod_path, const std::string& file_path, std::string& content) {
    SimpleZip zip;
    
    if (!zip.loadFromFile(cjmod_path)) {
        setError("无法加载CJmod文件: " + zip.getLastError());
        return false;
    }
    
    if (!zip.extractFile(file_path, content)) {
        setError("无法提取文件: " + zip.getLastError());
        return false;
    }
    
    return true;
}

bool CJmodManager::extractCJmodBinary(const std::string& cjmod_path, const std::string& file_path, std::vector<uint8_t>& data) {
    SimpleZip zip;
    
    if (!zip.loadFromFile(cjmod_path)) {
        setError("无法加载CJmod文件: " + zip.getLastError());
        return false;
    }
    
    if (!zip.extractFile(file_path, data)) {
        setError("无法提取二进制文件: " + zip.getLastError());
        return false;
    }
    
    return true;
}

std::string CJmodManager::resolveCJmodPath(const std::string& module_name, const std::vector<std::string>& search_paths) {
    // 处理官方模块前缀
    std::string actual_module_name = module_name;
    bool is_official = isOfficialModule(module_name);
    
    if (is_official) {
        actual_module_name = stripOfficialPrefix(module_name);
        // 优先在官方路径搜索
        std::string official_path = resolveOfficialCJmodPath(actual_module_name);
        if (!official_path.empty()) {
            return official_path;
        }
    }
    
    for (const std::string& search_path : search_paths) {
        // 尝试.cjmod文件
        std::string cjmod_path = joinPath(search_path, actual_module_name + CJMOD_EXTENSION);
        if (fileExists(cjmod_path)) {
            return cjmod_path;
        }
        
        // 尝试目录
        std::string dir_path = joinPath(search_path, actual_module_name);
        if (directoryExists(dir_path)) {
            return dir_path;
        }
    }
    
    return "";
}

std::string CJmodManager::resolveOfficialCJmodPath(const std::string& module_name) {
    std::string official_path = CJmodUtils::getOfficialCJmodPath();
    
    // 尝试.cjmod文件
    std::string cjmod_path = joinPath(official_path, module_name + CJMOD_EXTENSION);
    if (fileExists(cjmod_path)) {
        return cjmod_path;
    }
    
    // 尝试目录
    std::string dir_path = joinPath(official_path, module_name);
    if (directoryExists(dir_path)) {
        return dir_path;
    }
    
    return "";
}

std::string CJmodManager::resolveCJmodSubmodulePath(const std::string& main_module, const std::string& submodule) {
    auto [module_name, sub_name] = CJmodUtils::parseCJmodModuleName(main_module + "." + submodule);
    return resolveCJmodPath(module_name, search_paths_);
}

std::vector<CJmodInterface> CJmodManager::analyzeCppInterfaces(const std::string& cpp_content) {
    std::vector<CJmodInterface> interfaces;
    
    // 简化实现：查找特定的函数签名模式
    // 这里可以根据CHTL JS编译器的具体接口规范来实现
    std::istringstream stream(cpp_content);
    std::string line;
    
    while (std::getline(stream, line)) {
        // 查找导出函数标记（例如：CHTL_JS_EXPORT）
        if (line.find("CHTL_JS_EXPORT") != std::string::npos) {
            CJmodInterface interface;
            // 这里需要根据具体的接口规范来解析
            // 简化实现
            interface.function_name = "exported_function";
            interface.return_type = "void";
            interfaces.push_back(interface);
        }
    }
    
    return interfaces;
}

std::vector<std::string> CJmodManager::extractIncludeHeaders(const std::string& cpp_content) {
    return CJmodUtils::extractIncludeDirectives(cpp_content);
}

std::vector<std::string> CJmodManager::extractExportedFunctions(const std::string& cpp_content) {
    return CJmodUtils::extractFunctionDeclarations(cpp_content);
}

std::vector<std::string> CJmodManager::parseCJmodDependencies(const std::string& dependencies_str) {
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

bool CJmodManager::checkCJmodDependencies(const CJmodInfo& info, const std::vector<std::string>& available_modules) {
    auto dependencies = parseCJmodDependencies(info.dependencies);
    
    for (const std::string& dependency : dependencies) {
        if (std::find(available_modules.begin(), available_modules.end(), dependency) == available_modules.end()) {
            setError("缺少CJmod依赖模块: " + dependency);
            return false;
        }
    }
    
    return true;
}

bool CJmodManager::isCJmodVersionCompatible(const std::string& required_version, const std::string& available_version) {
    return 1 >= 0;
}

bool CJmodManager::isAPIVersionCompatible(const std::string& cjmod_api_version, const std::string& compiler_api_version) {
    // API版本兼容性检查
    return 1 >= 0;
}

bool CJmodManager::generateCJmodWrapper(const CJmodInfo& info, const std::string& output_path) {
    std::string wrapper_code = CJmodUtils::generateCJmodWrapperCode(info);
    
    std::ofstream file(output_path);
    if (!file.is_open()) {
        setError("无法创建CJmod包装文件: " + output_path);
        return false;
    }
    
    file << wrapper_code;
    file.close();
    
    return true;
}

bool CJmodManager::compileCJmodSources(const std::string& cjmod_path, const std::string& output_path) {
    // 这里需要调用系统的C++编译器来编译CJmod源码
    // 简化实现：返回成功
    std::cout << "编译CJmod源码: " << cjmod_path << " -> " << output_path << std::endl;
    return true;
}

void CJmodManager::addCJmodSearchPath(const std::string& path) {
    if (std::find(search_paths_.begin(), search_paths_.end(), path) == search_paths_.end()) {
        search_paths_.push_back(path);
    }
}

void CJmodManager::removeCJmodSearchPath(const std::string& path) {
    search_paths_.erase(
        std::remove(search_paths_.begin(), search_paths_.end(), path),
        search_paths_.end()
    );
}

bool CJmodManager::isOfficialModule(const std::string& module_name) {
    return module_name.size() > OFFICIAL_PREFIX.size() && 
           module_name.substr(0, OFFICIAL_PREFIX.size()) == OFFICIAL_PREFIX;
}

std::string CJmodManager::stripOfficialPrefix(const std::string& module_name) {
    if (isOfficialModule(module_name)) {
        return module_name.substr(OFFICIAL_PREFIX.size());
    }
    return module_name;
}

std::string CJmodManager::addOfficialPrefix(const std::string& module_name) {
    if (!isOfficialModule(module_name)) {
        return OFFICIAL_PREFIX + module_name;
    }
    return module_name;
}

void CJmodManager::clearCJmodCache() {
    info_cache_.clear();
}

bool CJmodManager::isCJmodCached(const std::string& cjmod_path) {
    return info_cache_.find(cjmod_path) != info_cache_.end();
}

// 私有方法实现
bool CJmodManager::validateCJmodName(const std::string& name) {
    if (name.empty()) return false;
    
    // 检查字符是否合法
    for (char c : name) {
        if (!std::isalnum(c) && c != '_' && c != '-') {
            return false;
        }
    }
    
    return true;
}

bool CJmodManager::validateCJmodVersion(const std::string& version) {
    // 简单的版本号验证 (x.y.z格式)
    return !version.empty();
}

bool CJmodManager::validateCJmodFileStructure(const std::string& base_path, const CJmodStructure& structure) {
    // 验证主模块名
    if (!validateCJmodName(structure.main_module_name)) {
        setError("无效的CJmod模块名: " + structure.main_module_name);
        return false;
    }
    
    // 验证src目录
    std::string src_path = joinPath(base_path, structure.src_dir);
    if (!directoryExists(src_path)) {
        setError("缺少src目录");
        return false;
    }
    
    // 验证至少有一个C++文件
    if (structure.cpp_files.empty()) {
        setError("CJmod必须包含至少一个C++源文件");
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
    
    CJmodInfo info;
    if (!parseCJmodInfo(content, info)) {
        return false;
    }
    
    return true;
}

// 文件系统操作实现
bool CJmodManager::directoryExists(const std::string& path) {
    return std::filesystem::exists(path) && std::filesystem::is_directory(path);
}

bool CJmodManager::fileExists(const std::string& path) {
    return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
}

std::vector<std::string> CJmodManager::listDirectory(const std::string& path) {
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

bool CJmodManager::readFileContent(const std::string& file_path, std::string& content) {
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
std::string CJmodManager::joinPath(const std::string& base, const std::string& relative) {
    std::filesystem::path result = std::filesystem::path(base) / relative;
    return result.string();
}

std::string CJmodManager::getFileName(const std::string& path) {
    return std::filesystem::path(path).filename().string();
}

std::string CJmodManager::getFileExtension(const std::string& path) {
    return std::filesystem::path(path).extension().string();
}

std::string CJmodManager::getDirectoryName(const std::string& path) {
    return std::filesystem::path(path).parent_path().string();
}

// C++文件类型检查
bool CJmodManager::isCppFile(const std::string& filename) {
    return filename.size() >= CPP_EXTENSION.size() && 
           filename.substr(filename.size() - CPP_EXTENSION.size()) == CPP_EXTENSION;
}

bool CJmodManager::isHeaderFile(const std::string& filename) {
    return filename.size() >= HEADER_EXTENSION.size() && 
           filename.substr(filename.size() - HEADER_EXTENSION.size()) == HEADER_EXTENSION;
}

// 解析辅助实现
std::string CJmodManager::extractCJmodInfoValue(const std::string& content, const std::string& key) {
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

void CJmodManager::setError(const std::string& error) {
    last_error_ = error;
    std::cerr << "CJmodManager错误: " << error << std::endl;
}

// CJmodUtils命名空间实现
namespace CJmodUtils {

std::vector<std::string> getCJmodStandardSearchPaths() {
    std::vector<std::string> paths;
    
    // 添加官方CJmod模块路径
    paths.push_back(getOfficialCJmodPath());
    
    // 添加本地CJmod模块路径
    paths.push_back(getLocalCJmodPath());
    
    // 添加当前目录
    paths.push_back(".");
    
    return paths;
}

std::string getOfficialCJmodPath() {
    // 官方CJmod模块路径通常在编译器安装目录下的cjmodules文件夹
    return "cjmodules";
}

std::string getLocalCJmodPath(const std::string& base_path) {
    return base_path + "/cjmodules";
}

std::pair<std::string, std::string> parseCJmodModuleName(const std::string& full_name) {
    // 处理chtl::前缀
    std::string processed_name = full_name;
    if (processed_name.size() > 6 && processed_name.substr(0, 6) == "chtl::") {
        processed_name = processed_name.substr(6);
    }
    
    size_t dot_pos = processed_name.find('.');
    if (dot_pos == std::string::npos) {
        return {full_name, ""};
    }
    
    return {processed_name.substr(0, dot_pos), processed_name.substr(dot_pos + 1)};
}

bool isOfficialModuleName(const std::string& module_name) {
    return module_name.size() > 6 && module_name.substr(0, 6) == "chtl::";
}

std::vector<std::string> extractFunctionDeclarations(const std::string& cpp_content) {
    std::vector<std::string> functions;
    
    std::istringstream stream(cpp_content);
    std::string line;
    
    while (std::getline(stream, line)) {
        // 简化实现：查找函数声明模式
        if (line.find("extern \"C\"") != std::string::npos || 
            line.find("CHTL_JS_EXPORT") != std::string::npos) {
            functions.push_back(line);
        }
    }
    
    return functions;
}

std::vector<std::string> extractIncludeDirectives(const std::string& cpp_content) {
    std::vector<std::string> includes;
    
    std::istringstream stream(cpp_content);
    std::string line;
    
    while (std::getline(stream, line)) {
        // 去除前后空白
        line.erase(0, line.find_first_not_of(" \t"));
        
        if (line.size() > 8 && line.substr(0, 8) == "#include") {
            includes.push_back(line);
        }
    }
    
    return includes;
}

std::string extractNamespace(const std::string& cpp_content) {
    std::istringstream stream(cpp_content);
    std::string line;
    
    while (std::getline(stream, line)) {
        if (line.find("namespace") != std::string::npos) {
            // 简化实现：提取命名空间名
            size_t ns_pos = line.find("namespace");
            size_t name_start = ns_pos + 9; // "namespace"的长度
            size_t name_end = line.find('{', name_start);
            if (name_end == std::string::npos) {
                name_end = line.find(' ', name_start);
            }
            
            if (name_end != std::string::npos) {
                std::string ns_name = line.substr(name_start, name_end - name_start);
                // 去除空白
                ns_name.erase(0, ns_name.find_first_not_of(" \t"));
                ns_name.erase(ns_name.find_last_not_of(" \t") + 1);
                return ns_name;
            }
        }
    }
    
    return "";
}

std::string generateCJmodWrapperCode(const CJmodInfo& info) {
    std::ostringstream oss;
    
    oss << "// CJmod包装代码 - 自动生成\n";
    oss << "#include <iostream>\n";
    oss << "#include <string>\n";
    oss << "#include <vector>\n\n";
    
    oss << "// CJmod信息\n";
    oss << "extern \"C\" {\n";
    oss << "    const char* getCJmodName() { return \"" << info.name << "\"; }\n";
    oss << "    const char* getCJmodVersion() { return \"" << info.version << "\"; }\n";
    oss << "    const char* getCJmodAPIVersion() { return \"" << info.api_version << "\"; }\n";
    
    if (!info.entry_point.empty()) {
        oss << "    // 入口点\n";
        oss << "    extern void " << info.entry_point << "();\n";
        oss << "    void initCJmod() { " << info.entry_point << "(); }\n";
    }
    
    oss << "}\n";
    
    return oss.str();
}

std::vector<std::string> generateCompilerFlags(const CJmodInfo& info) {
    std::vector<std::string> flags;
    
    flags.push_back("-std=c++17");
    flags.push_back("-fPIC");
    flags.push_back("-shared");
    
    // 添加CHTL特定的定义
    flags.push_back("-DCHTL_CJMOD_BUILD");
    flags.push_back("-DCHTL_API_VERSION=\"" + info.api_version + "\"");
    
    return flags;
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

} // namespace CJmodUtils

} // namespace chtl