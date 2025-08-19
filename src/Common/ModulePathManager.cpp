#include "ModulePathManager.h"
#include <algorithm>
#include <iostream>
#include <cctype>

namespace chtl {
namespace common {

ModulePathManager::ModulePathManager() {
    // 默认添加一些标准搜索路径
    addSearchPath("src/Module", "user");
    addSearchPath("modules", "system");
}

void ModulePathManager::addSearchPath(const std::string& path, const std::string& category) {
    searchPaths_.push_back(path);
    pathCategories_[path] = category;
}

void ModulePathManager::scanModules() {
    modules_.clear();
    
    for (const auto& searchPath : searchPaths_) {
        if (std::filesystem::exists(searchPath)) {
            const std::string& category = pathCategories_[searchPath];
            scanPath(searchPath, category);
        }
    }
    
    std::cout << "模块扫描完成，发现 " << modules_.size() << " 个模块" << std::endl;
}

ModulePathManager::ModuleLocation* ModulePathManager::findModule(const std::string& name, ModuleType type) {
    const std::string normalizedName = normalizeModuleName(name);
    
    for (auto& module : modules_) {
        if (normalizeModuleName(module.name) == normalizedName) {
            if (type == ModuleType::UNKNOWN || module.type == type) {
                return &module;
            }
        }
    }
    
    return nullptr;
}

std::vector<ModulePathManager::ModuleLocation> ModulePathManager::getModulesByType(ModuleType type) const {
    std::vector<ModuleLocation> result;
    
    for (const auto& module : modules_) {
        if (module.type == type) {
            result.push_back(module);
        }
    }
    
    return result;
}

std::vector<ModulePathManager::ModuleLocation> ModulePathManager::getModulesByCategory(const std::string& category) const {
    std::vector<ModuleLocation> result;
    
    for (const auto& module : modules_) {
        if (module.category == category) {
            result.push_back(module);
        }
    }
    
    return result;
}

std::string ModulePathManager::getModuleFullPath(const ModuleLocation& location) const {
    return std::filesystem::absolute(location.path).string();
}

bool ModulePathManager::moduleExists(const std::string& name, ModuleType type) const {
    return const_cast<ModulePathManager*>(this)->findModule(name, type) != nullptr;
}

std::vector<std::string> ModulePathManager::getFolderVariants(ModuleType type) {
    std::string baseType = typeToString(type);  // CMOD, CJMOD
    
    // 生成所有三种格式
    std::string lowercase = baseType;
    std::transform(lowercase.begin(), lowercase.end(), lowercase.begin(), ::tolower);  // cmod, cjmod
    
    std::string titlecase = lowercase;
    if (!titlecase.empty()) {
        titlecase[0] = std::toupper(titlecase[0]);  // Cmod, Cjmod
    }
    
    std::string uppercase = baseType;  // CMOD, CJMOD (保持原样)
    
    return {
        lowercase,   // cmod, cjmod
        titlecase,   // Cmod, Cjmod
        uppercase    // CMOD, CJMOD
    };
}

ModulePathManager::ModuleType ModulePathManager::getTypeFromExtension(const std::string& extension) {
    std::string ext = extension;
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".cmod") return ModuleType::CMOD;
    if (ext == ".cjmod") return ModuleType::CJMOD;
    
    return ModuleType::UNKNOWN;
}

ModulePathManager::ModuleType ModulePathManager::getTypeFromFolderName(const std::string& folderName) {
    std::string name = folderName;
    std::transform(name.begin(), name.end(), name.begin(), ::tolower);
    
    if (name == "cmod") return ModuleType::CMOD;
    if (name == "cjmod") return ModuleType::CJMOD;
    
    return ModuleType::UNKNOWN;
}

std::string ModulePathManager::typeToString(ModuleType type) {
    switch (type) {
        case ModuleType::CMOD: return "CMOD";
        case ModuleType::CJMOD: return "CJMOD";
        default: return "UNKNOWN";
    }
}

void ModulePathManager::scanPath(const std::string& basePath, const std::string& category) {
    try {
        // 扫描CMOD模块
        scanModuleTypeFolder(basePath, ModuleType::CMOD, category);
        
        // 扫描CJMOD模块
        scanModuleTypeFolder(basePath, ModuleType::CJMOD, category);
        
        // 也扫描根目录中的直接模块文件
        for (const auto& entry : std::filesystem::directory_iterator(basePath)) {
            if (entry.is_regular_file()) {
                const std::string extension = entry.path().extension().string();
                ModuleType type = getTypeFromExtension(extension);
                
                if (type != ModuleType::UNKNOWN) {
                    ModuleLocation location = analyzeModuleItem(entry.path().string(), type, category);
                    modules_.push_back(location);
                }
            }
        }
        
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "扫描路径失败 " << basePath << ": " << e.what() << std::endl;
    }
}

void ModulePathManager::scanModuleTypeFolder(const std::string& basePath, ModuleType type, const std::string& category) {
    const auto variants = getFolderVariants(type);
    
    for (const std::string& variant : variants) {
        const std::string typeFolder = basePath + "/" + variant;
        
        if (!std::filesystem::exists(typeFolder)) {
            continue;
        }
        
        try {
            for (const auto& entry : std::filesystem::directory_iterator(typeFolder)) {
                ModuleLocation location = analyzeModuleItem(entry.path().string(), type, category);
                modules_.push_back(location);
            }
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "扫描类型文件夹失败 " << typeFolder << ": " << e.what() << std::endl;
        }
    }
}

ModulePathManager::ModuleLocation ModulePathManager::analyzeModuleItem(const std::string& itemPath, ModuleType type, const std::string& category) {
    ModuleLocation location;
    location.type = type;
    location.path = itemPath;
    location.category = category;
    
    if (std::filesystem::is_regular_file(itemPath)) {
        // 打包的模块文件
        location.isPacked = true;
        location.name = std::filesystem::path(itemPath).stem().string();
    } else if (std::filesystem::is_directory(itemPath)) {
        // 源码模块目录
        location.isPacked = false;
        location.name = std::filesystem::path(itemPath).filename().string();
        
        // 验证是否为有效的模块目录
        if (!isSourceModule(itemPath, type)) {
            std::cerr << "警告: 目录 " << itemPath << " 不是有效的" << typeToString(type) << "模块" << std::endl;
        }
    }
    
    return location;
}

bool ModulePathManager::isPackedModule(const std::string& path, ModuleType type) const {
    if (!std::filesystem::is_regular_file(path)) {
        return false;
    }
    
    const std::string extension = std::filesystem::path(path).extension().string();
    return getTypeFromExtension(extension) == type;
}

bool ModulePathManager::isSourceModule(const std::string& path, ModuleType type) const {
    if (!std::filesystem::is_directory(path)) {
        return false;
    }
    
    // 检查必需的目录结构
    const std::string infoDir = path + "/info";
    const std::string srcDir = path + "/src";
    
    if (!std::filesystem::exists(infoDir) || !std::filesystem::exists(srcDir)) {
        return false;
    }
    
    // 检查info文件
    const std::string moduleName = std::filesystem::path(path).filename().string();
    const std::string infoFile = infoDir + "/" + moduleName + ".chtl";
    
    if (!std::filesystem::exists(infoFile)) {
        return false;
    }
    
    // 检查源码文件类型
    bool hasValidSources = false;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(srcDir)) {
            if (entry.is_regular_file()) {
                const std::string extension = entry.path().extension().string();
                
                if (type == ModuleType::CMOD && extension == ".chtl") {
                    hasValidSources = true;
                    break;
                } else if (type == ModuleType::CJMOD && (extension == ".cpp" || extension == ".h")) {
                    hasValidSources = true;
                    break;
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "检查源码目录失败 " << srcDir << ": " << e.what() << std::endl;
        return false;
    }
    
    return hasValidSources;
}

std::string ModulePathManager::normalizeModuleName(const std::string& name) const {
    std::string normalized = name;
    std::transform(normalized.begin(), normalized.end(), normalized.begin(), ::tolower);
    return normalized;
}

} // namespace common
} // namespace chtl