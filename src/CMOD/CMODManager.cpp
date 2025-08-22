#include "CMOD/CMODManager.h"
#include "Utils/FileUtils.h"
#include "Utils/StringUtils.h"
#include "Utils/ErrorHandler.h"
#include "CHTL/Lexer/CHTLLexer.h"
#include "CHTL/Parser/CHTLParser.h"
#include <filesystem>
#include <algorithm>
#include <regex>

namespace CHTL {
namespace CMOD {

// C++17兼容的字符串辅助函数
namespace {
    bool starts_with(const std::string& str, const std::string& prefix) {
        return str.length() >= prefix.length() && 
               str.substr(0, prefix.length()) == prefix;
    }
    
    bool ends_with(const std::string& str, const std::string& suffix) {
        return str.length() >= suffix.length() && 
               str.substr(str.length() - suffix.length()) == suffix;
    }
}

CMODManager::CMODManager() {
    // 按照目标规划.ini第78-89行的要求：
    // 对于@Chtl导入语法的路径搜索：
    // 1. 官方模块目录（源码编译后生成的module文件夹，里面存放着编译src/Module好的cmod和cjmod文件）
    // 2. 当前目录的module文件夹
    // 3. 当前目录
    // 
    // 对于@CJmod导入语法的路径搜索：
    // 1. 官方模块目录（源码编译后生成的module文件夹，里面存放着编译src/Module好的cjmod文件）
    // 2. 当前目录的module文件夹
    // 3. 当前目录
    
    // 搜索路径在运行时根据当前编译文件动态确定，不使用固定路径
}

bool CMODManager::LoadModule(const std::string& modulePath) {
    try {
        // 检查路径是否存在
        if (!std::filesystem::exists(modulePath)) {
            Utils::ErrorHandler::GetInstance().LogError(
                "模块路径不存在: " + modulePath
            );
            return false;
        }
        
        // 解析模块文件夹
        auto module = ParseModuleFolder(modulePath);
        if (!module) {
            Utils::ErrorHandler::GetInstance().LogError(
                "解析模块失败: " + modulePath
            );
            return false;
        }
        
        // 验证模块依赖
        if (!ValidateModuleDependencies(*module)) {
            Utils::ErrorHandler::GetInstance().LogError(
                "模块依赖验证失败: " + module->info.name
            );
            return false;
        }
        
        // 存储模块
        std::string moduleName = module->info.name;
        modules_[moduleName] = std::move(module);
        
        Utils::ErrorHandler::GetInstance().LogInfo(
            "成功加载模块: " + moduleName
        );
        
        return true;
        
    } catch (const std::exception& e) {
        Utils::ErrorHandler::GetInstance().LogError(
            "加载模块异常: " + modulePath + " - " + e.what()
        );
        return false;
    }
}

std::unique_ptr<CMODModule> CMODManager::ParseModuleFolder(const std::string& folderPath) {
    auto module = std::make_unique<CMODModule>();
    
    // 获取模块名称（文件夹名称）
    std::string moduleName = std::filesystem::path(folderPath).filename().string();
    
    // 构建路径
    std::string srcPath = folderPath + "/src";
    std::string infoPath = folderPath + "/info";
    std::string mainInfoFile = infoPath + "/" + moduleName + ".chtl";
    
    // 检查必需的文件夹
    if (!std::filesystem::exists(srcPath)) {
        Utils::ErrorHandler::GetInstance().LogError(
            "模块缺少src文件夹: " + folderPath
        );
        return nullptr;
    }
    
    if (!std::filesystem::exists(infoPath)) {
        Utils::ErrorHandler::GetInstance().LogError(
            "模块缺少info文件夹: " + folderPath
        );
        return nullptr;
    }
    
    // 解析模块信息
    if (std::filesystem::exists(mainInfoFile)) {
        module->info = ParseModuleInfo(mainInfoFile);
        module->exports = ParseModuleExports(mainInfoFile);
        
        // 如果Export块为空或不存在，自动生成
        if (module->exports.customStyles.empty() && 
            module->exports.customElements.empty() && 
            module->exports.customVars.empty() && 
            module->exports.templateStyles.empty() && 
            module->exports.templateElements.empty() && 
            module->exports.templateVars.empty()) {
            
            Utils::ErrorHandler::GetInstance().LogInfo(
                "Export块为空，自动生成导出信息: " + moduleName
            );
            
            // 自动生成导出信息
            ModuleExports autoExports = GenerateAutoExports(folderPath);
            module->exports = autoExports;
            
            // 更新信息文件
            UpdateModuleExports(folderPath, autoExports);
        }
    } else {
        Utils::ErrorHandler::GetInstance().LogError(
            "模块缺少主信息文件: " + mainInfoFile
        );
        return nullptr;
    }
    
    // 如果info中没有名称，使用文件夹名称
    if (module->info.name.empty()) {
        module->info.name = moduleName;
    }
    
    // 收集源文件
    std::string mainSourceFile = srcPath + "/" + moduleName + ".chtl";
    if (std::filesystem::exists(mainSourceFile)) {
        module->mainFilePath = mainSourceFile;
        module->sourceFiles.push_back(mainSourceFile);
    }
    
    // 收集其他源文件
    for (const auto& entry : std::filesystem::directory_iterator(srcPath)) {
        if (entry.is_regular_file()) {
            std::string filePath = entry.path().string();
            if (ends_with(filePath, ".chtl") && filePath != mainSourceFile) {
                module->sourceFiles.push_back(filePath);
            }
        }
    }
    
    // 解析子模块
    ParseSubModules(srcPath, *module);
    
    return module;
}

ModuleInfo CMODManager::ParseModuleInfo(const std::string& infoFilePath) {
    ModuleInfo info;
    
    try {
        std::string content = Utils::FileUtils::ReadFile(infoFilePath);
        info = ParseInfoBlock(content);
        
    } catch (const std::exception& e) {
        Utils::ErrorHandler::GetInstance().LogError(
            "解析模块信息失败: " + infoFilePath + " - " + e.what()
        );
    }
    
    return info;
}

ModuleExports CMODManager::ParseModuleExports(const std::string& infoFilePath) {
    ModuleExports exports;
    
    try {
        std::string content = Utils::FileUtils::ReadFile(infoFilePath);
        exports = ParseExportBlock(content);
        
    } catch (const std::exception& e) {
        Utils::ErrorHandler::GetInstance().LogError(
            "解析模块导出失败: " + infoFilePath + " - " + e.what()
        );
    }
    
    return exports;
}

ModuleInfo CMODManager::ParseInfoBlock(const std::string& content) {
    ModuleInfo info;
    
    // 查找[Info]块
    std::regex infoBlockRegex(R"(\[Info\]\s*\{([^}]*)\})");
    std::smatch match;
    
    if (std::regex_search(content, match, infoBlockRegex)) {
        std::string infoContent = match[1].str();
        
        // 解析键值对
        std::istringstream iss(infoContent);
        std::string line;
        
        while (std::getline(iss, line)) {
            line = Utils::StringUtils::Trim(line);
            
            // 跳过注释和空行
            if (line.empty() || (line.length() >= 2 && line.substr(0, 2) == "//")) {
                continue;
            }
            
            // 解析键值对：key = "value";
            size_t equalPos = line.find('=');
            if (equalPos != std::string::npos) {
                std::string key = Utils::StringUtils::Trim(line.substr(0, equalPos));
                std::string value = Utils::StringUtils::Trim(line.substr(equalPos + 1));
                
                // 移除分号
                if (!value.empty() && value.back() == ';') {
                    value = value.substr(0, value.length() - 1);
                    value = Utils::StringUtils::Trim(value);
                }
                
                // 移除引号
                if (value.length() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.length() - 2);
                }
                
                // 设置对应的字段
                if (key == "name") {
                    info.name = value;
                } else if (key == "version") {
                    info.version = value;
                } else if (key == "description") {
                    info.description = value;
                } else if (key == "author") {
                    info.author = value;
                } else if (key == "license") {
                    info.license = value;
                } else if (key == "dependencies") {
                    info.dependencies = value;
                } else if (key == "category") {
                    info.category = value;
                } else if (key == "minCHTLVersion") {
                    info.minCHTLVersion = value;
                } else if (key == "maxCHTLVersion") {
                    info.maxCHTLVersion = value;
                }
            }
        }
    }
    
    return info;
}

ModuleExports CMODManager::ParseExportBlock(const std::string& content) {
    ModuleExports exports;
    
    try {
        // 查找[Export]块
        std::regex exportBlockRegex(R"(\[Export\]\s*\{([^}]*)\})");
        std::smatch match;
        
        if (std::regex_search(content, match, exportBlockRegex)) {
        std::string exportContent = match[1].str();
        
        // 解析导出项
        std::istringstream iss(exportContent);
        std::string line;
        
        while (std::getline(iss, line)) {
            line = Utils::StringUtils::Trim(line);
            
            // 跳过注释和空行
            if (line.empty() || (line.length() >= 2 && line.substr(0, 2) == "//")) {
                continue;
            }
            
            // 解析导出声明：[Custom] @Style Name1, Name2;
            if (line.length() >= 8 && line.substr(0, 8) == "[Custom]") {
                std::string remaining = line.substr(8); // 移除"[Custom]"
                remaining = Utils::StringUtils::Trim(remaining);
                
                if (starts_with(remaining, "@Style")) {
                    std::string names = remaining.substr(6); // 移除"@Style"
                                         if (!names.empty()) {
                         auto nameList = Utils::StringUtils::Split(names, ",");
                         for (const auto& name : nameList) {
                             std::string trimmedName = Utils::StringUtils::Trim(name);
                             if (!trimmedName.empty()) {
                                 // 移除末尾的分号
                                 if (trimmedName.back() == ';') {
                                     trimmedName = trimmedName.substr(0, trimmedName.length() - 1);
                                     trimmedName = Utils::StringUtils::Trim(trimmedName);
                                 }
                                 if (!trimmedName.empty()) {
                                     exports.customStyles.push_back(trimmedName);
                                 }
                             }
                         }
                     }
                } else if (starts_with(remaining, "@Element")) {
                    std::string names = remaining.substr(8); // 移除"@Element"
                    auto nameList = Utils::StringUtils::Split(names, ",");
                                         for (const auto& name : nameList) {
                         std::string trimmedName = Utils::StringUtils::Trim(name);
                         if (!trimmedName.empty()) {
                             // 移除末尾的分号
                             if (!trimmedName.empty() && trimmedName.back() == ';') {
                                 trimmedName = trimmedName.substr(0, trimmedName.length() - 1);
                                 trimmedName = Utils::StringUtils::Trim(trimmedName);
                             }
                             exports.customElements.push_back(trimmedName);
                         }
                     }
                } else if (starts_with(remaining, "@Var")) {
                    std::string names = remaining.substr(4); // 移除"@Var"
                    auto nameList = Utils::StringUtils::Split(names, ",");
                    for (const auto& name : nameList) {
                        std::string trimmedName = Utils::StringUtils::Trim(name);
                        if (!trimmedName.empty()) {
                            exports.customVars.push_back(trimmedName);
                        }
                    }
                }
            } else if (starts_with(line, "[Template]")) {
                // 类似地处理模板导出
                std::string remaining = line.substr(10); // 移除"[Template]"
                remaining = Utils::StringUtils::Trim(remaining);
                
                if (starts_with(remaining, "@Style")) {
                    std::string names = remaining.substr(6);
                    auto nameList = Utils::StringUtils::Split(names, ",");
                    for (const auto& name : nameList) {
                        std::string trimmedName = Utils::StringUtils::Trim(name);
                        if (!trimmedName.empty()) {
                            exports.templateStyles.push_back(trimmedName);
                        }
                    }
                } else if (starts_with(remaining, "@Element")) {
                    std::string names = remaining.substr(8);
                    auto nameList = Utils::StringUtils::Split(names, ",");
                    for (const auto& name : nameList) {
                        std::string trimmedName = Utils::StringUtils::Trim(name);
                        if (!trimmedName.empty()) {
                            exports.templateElements.push_back(trimmedName);
                        }
                    }
                } else if (starts_with(remaining, "@Var")) {
                    std::string names = remaining.substr(4);
                    auto nameList = Utils::StringUtils::Split(names, ",");
                    for (const auto& name : nameList) {
                        std::string trimmedName = Utils::StringUtils::Trim(name);
                        if (!trimmedName.empty()) {
                            exports.templateVars.push_back(trimmedName);
                        }
                    }
                }
            } else if (starts_with(line, "[Origin]")) {
                // 处理原始嵌入导出
                std::string remaining = line.substr(8); // 移除"[Origin]"
                remaining = Utils::StringUtils::Trim(remaining);
                
                if (starts_with(remaining, "@Html")) {
                    std::string names = remaining.substr(5);
                    auto nameList = Utils::StringUtils::Split(names, ",");
                    for (const auto& name : nameList) {
                        std::string trimmedName = Utils::StringUtils::Trim(name);
                        if (!trimmedName.empty()) {
                            exports.originHtml.push_back(trimmedName);
                        }
                    }
                } else if (starts_with(remaining, "@Style")) {
                    std::string names = remaining.substr(6);
                    auto nameList = Utils::StringUtils::Split(names, ",");
                    for (const auto& name : nameList) {
                        std::string trimmedName = Utils::StringUtils::Trim(name);
                        if (!trimmedName.empty()) {
                            exports.originStyle.push_back(trimmedName);
                        }
                    }
                } else if (starts_with(remaining, "@Javascript")) {
                    std::string names = remaining.substr(11);
                    auto nameList = Utils::StringUtils::Split(names, ",");
                    for (const auto& name : nameList) {
                        std::string trimmedName = Utils::StringUtils::Trim(name);
                        if (!trimmedName.empty()) {
                            exports.originJavascript.push_back(trimmedName);
                        }
                    }
                }
            } else if (starts_with(line, "[Configuration]")) {
                // 处理配置导出
                std::string remaining = line.substr(15); // 移除"[Configuration]"
                remaining = Utils::StringUtils::Trim(remaining);
                
                if (starts_with(remaining, "@Config")) {
                    std::string names = remaining.substr(7);
                    auto nameList = Utils::StringUtils::Split(names, ",");
                    for (const auto& name : nameList) {
                        std::string trimmedName = Utils::StringUtils::Trim(name);
                        if (!trimmedName.empty()) {
                            exports.configurations.push_back(trimmedName);
                        }
                    }
                }
            }
        }
    }
    
    } catch (const std::exception& e) {
        Utils::ErrorHandler::GetInstance().LogError(
            "解析Export块异常: " + std::string(e.what())
        );
    }
    
    return exports;
}

const CMODModule* CMODManager::FindModule(const std::string& moduleName) const {
    auto it = modules_.find(moduleName);
    return (it != modules_.end()) ? it->second.get() : nullptr;
}

const CMODModule* CMODManager::FindSubModule(const std::string& moduleName) const {
    // 支持点分割的模块名：Module.SubModule.SubSubModule
    auto parts = Utils::StringUtils::Split(moduleName, ".");
    
    if (parts.empty()) {
        return nullptr;
    }
    
    // 查找主模块
    const CMODModule* currentModule = FindModule(parts[0]);
    if (!currentModule) {
        return nullptr;
    }
    
    // 递归查找子模块
    for (size_t i = 1; i < parts.size(); ++i) {
        auto it = currentModule->subModules.find(parts[i]);
        if (it == currentModule->subModules.end()) {
            return nullptr;
        }
        currentModule = &it->second;
    }
    
    return currentModule;
}

bool CMODManager::ImportModule(const std::string& moduleName, const std::string& alias) {
    // 首先尝试从已加载的模块中查找
    const CMODModule* module = FindModule(moduleName);
    
    if (!module) {
        // 尝试从搜索路径中加载
        for (const auto& searchPath : searchPaths_) {
            std::string fullPath = searchPath + "/" + moduleName;
            if (std::filesystem::exists(fullPath)) {
                if (LoadModule(fullPath)) {
                    module = FindModule(moduleName);
                    break;
                }
            }
        }
    }
    
    if (!module) {
        Utils::ErrorHandler::GetInstance().LogError(
            "无法找到或加载模块: " + moduleName
        );
        return false;
    }
    
    // 设置别名
    if (!alias.empty()) {
        aliases_[alias] = moduleName;
    }
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "成功导入模块: " + moduleName + 
        (alias.empty() ? "" : " as " + alias)
    );
    
    return true;
}

std::vector<std::string> CMODManager::GetModuleSourceContent(const std::string& moduleName) const {
    std::vector<std::string> contents;
    
    const CMODModule* module = FindModule(moduleName);
    if (!module) {
        return contents;
    }
    
    // 读取所有源文件内容
    for (const auto& sourceFile : module->sourceFiles) {
        try {
            std::string content = Utils::FileUtils::ReadFile(sourceFile);
            contents.push_back(content);
        } catch (const std::exception& e) {
            Utils::ErrorHandler::GetInstance().LogError(
                "读取模块源文件失败: " + sourceFile + " - " + e.what()
            );
        }
    }
    
    return contents;
}

bool CMODManager::ValidateModuleDependencies(const CMODModule& module) const {
    // 检查CHTL版本兼容性
    if (!module.info.minCHTLVersion.empty() || !module.info.maxCHTLVersion.empty()) {
        std::string currentVersion = "1.0.0"; // 当前CHTL版本
        
        if (!CheckVersionCompatibility(currentVersion, 
                                     module.info.minCHTLVersion,
                                     module.info.maxCHTLVersion)) {
            return false;
        }
    }
    
    // 检查依赖模块
    if (!module.info.dependencies.empty()) {
        auto dependencies = Utils::StringUtils::Split(module.info.dependencies, ",");
        for (const auto& dep : dependencies) {
            std::string depName = Utils::StringUtils::Trim(dep);
            if (!depName.empty() && !FindModule(depName)) {
                Utils::ErrorHandler::GetInstance().LogError(
                    "缺少依赖模块: " + depName
                );
                return false;
            }
        }
    }
    
    return true;
}

std::vector<std::string> CMODManager::GetLoadedModules() const {
    std::vector<std::string> moduleNames;
    for (const auto& pair : modules_) {
        moduleNames.push_back(pair.first);
    }
    return moduleNames;
}

void CMODManager::ClearModules() {
    modules_.clear();
    aliases_.clear();
}

void CMODManager::SetModuleSearchPaths(const std::vector<std::string>& paths) {
    searchPaths_ = paths;
}

void CMODManager::AddModuleSearchPath(const std::string& path) {
    searchPaths_.push_back(path);
}

bool CMODManager::CheckVersionCompatibility(const std::string& moduleVersion,
                                          const std::string& minVersion,
                                          const std::string& maxVersion) const {
    // 简化的版本比较（实际实现需要更复杂的版本解析）
    if (!minVersion.empty() && moduleVersion < minVersion) {
        return false;
    }
    
    if (!maxVersion.empty() && moduleVersion > maxVersion) {
        return false;
    }
    
    return true;
}

void CMODManager::ParseSubModules(const std::string& parentPath, CMODModule& parentModule) {
    try {
        // 递归解析子模块
        for (const auto& entry : std::filesystem::directory_iterator(parentPath)) {
            if (entry.is_directory()) {
                std::string subModulePath = entry.path().string();
                std::string subModuleName = entry.path().filename().string();
                
                // 检查是否是有效的子模块（包含src和info文件夹）
                std::string subSrcPath = subModulePath + "/src";
                std::string subInfoPath = subModulePath + "/info";
                
                if (std::filesystem::exists(subSrcPath) && 
                    std::filesystem::exists(subInfoPath)) {
                    
                    auto subModule = ParseModuleFolder(subModulePath);
                    if (subModule) {
                        parentModule.subModules[subModuleName] = std::move(*subModule);
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        Utils::ErrorHandler::GetInstance().LogError(
            "解析子模块异常: " + parentPath + " - " + e.what()
        );
    }
}

ModuleExports CMODManager::GenerateAutoExports(const std::string& modulePath) {
    ModuleExports exports;
    
    try {
        std::string srcPath = modulePath + "/src";
        if (!std::filesystem::exists(srcPath)) {
            return exports;
        }
        
        // 扫描所有源文件
        for (const auto& entry : std::filesystem::directory_iterator(srcPath)) {
            if (entry.is_regular_file()) {
                std::string filePath = entry.path().string();
                if (ends_with(filePath, ".chtl")) {
                    std::string content = Utils::FileUtils::ReadFile(filePath);
                    
                    // 扫描文件内容获取符号
                    ModuleExports fileExports = ScanSourceForSymbols(content);
                    
                    // 合并导出信息
                    exports.customStyles.insert(exports.customStyles.end(), 
                                               fileExports.customStyles.begin(), 
                                               fileExports.customStyles.end());
                    exports.customElements.insert(exports.customElements.end(), 
                                                 fileExports.customElements.begin(), 
                                                 fileExports.customElements.end());
                    exports.customVars.insert(exports.customVars.end(), 
                                             fileExports.customVars.begin(), 
                                             fileExports.customVars.end());
                    exports.templateStyles.insert(exports.templateStyles.end(), 
                                                 fileExports.templateStyles.begin(), 
                                                 fileExports.templateStyles.end());
                    exports.templateElements.insert(exports.templateElements.end(), 
                                                   fileExports.templateElements.begin(), 
                                                   fileExports.templateElements.end());
                    exports.templateVars.insert(exports.templateVars.end(), 
                                               fileExports.templateVars.begin(), 
                                               fileExports.templateVars.end());
                    exports.originHtml.insert(exports.originHtml.end(), 
                                             fileExports.originHtml.begin(), 
                                             fileExports.originHtml.end());
                    exports.originStyle.insert(exports.originStyle.end(), 
                                              fileExports.originStyle.begin(), 
                                              fileExports.originStyle.end());
                    exports.originJavascript.insert(exports.originJavascript.end(), 
                                                   fileExports.originJavascript.begin(), 
                                                   fileExports.originJavascript.end());
                    exports.configurations.insert(exports.configurations.end(), 
                                                 fileExports.configurations.begin(), 
                                                 fileExports.configurations.end());
                }
            }
        }
        
        // 移除重复项
        std::sort(exports.customStyles.begin(), exports.customStyles.end());
        exports.customStyles.erase(std::unique(exports.customStyles.begin(), exports.customStyles.end()), 
                                  exports.customStyles.end());
        
        std::sort(exports.customElements.begin(), exports.customElements.end());
        exports.customElements.erase(std::unique(exports.customElements.begin(), exports.customElements.end()), 
                                    exports.customElements.end());
        
        std::sort(exports.customVars.begin(), exports.customVars.end());
        exports.customVars.erase(std::unique(exports.customVars.begin(), exports.customVars.end()), 
                                exports.customVars.end());
        
        std::sort(exports.templateStyles.begin(), exports.templateStyles.end());
        exports.templateStyles.erase(std::unique(exports.templateStyles.begin(), exports.templateStyles.end()), 
                                    exports.templateStyles.end());
        
        std::sort(exports.templateElements.begin(), exports.templateElements.end());
        exports.templateElements.erase(std::unique(exports.templateElements.begin(), exports.templateElements.end()), 
                                      exports.templateElements.end());
        
        std::sort(exports.templateVars.begin(), exports.templateVars.end());
        exports.templateVars.erase(std::unique(exports.templateVars.begin(), exports.templateVars.end()), 
                                  exports.templateVars.end());
        
    } catch (const std::exception& e) {
        Utils::ErrorHandler::GetInstance().LogError(
            "自动生成导出信息异常: " + modulePath + " - " + e.what()
        );
    }
    
    return exports;
}

ModuleExports CMODManager::ScanSourceForSymbols(const std::string& sourceContent) {
    ModuleExports exports;
    
    try {
        // 使用正则表达式扫描各种符号定义
        
        // 扫描[Custom] @Style
        std::regex customStyleRegex(R"(\[Custom\]\s*@Style\s+([a-zA-Z_][a-zA-Z0-9_]*))");
        std::sregex_iterator customStyleIter(sourceContent.begin(), sourceContent.end(), customStyleRegex);
        std::sregex_iterator end;
        
        for (std::sregex_iterator i = customStyleIter; i != end; ++i) {
            std::smatch match = *i;
            std::string styleName = match[1].str();
            exports.customStyles.push_back(styleName);
        }
        
        // 扫描[Custom] @Element
        std::regex customElementRegex(R"(\[Custom\]\s*@Element\s+([a-zA-Z_][a-zA-Z0-9_]*))");
        std::sregex_iterator customElementIter(sourceContent.begin(), sourceContent.end(), customElementRegex);
        
        for (std::sregex_iterator i = customElementIter; i != end; ++i) {
            std::smatch match = *i;
            std::string elementName = match[1].str();
            exports.customElements.push_back(elementName);
        }
        
        // 扫描[Custom] @Var
        std::regex customVarRegex(R"(\[Custom\]\s*@Var\s+([a-zA-Z_][a-zA-Z0-9_]*))");
        std::sregex_iterator customVarIter(sourceContent.begin(), sourceContent.end(), customVarRegex);
        
        for (std::sregex_iterator i = customVarIter; i != end; ++i) {
            std::smatch match = *i;
            std::string varName = match[1].str();
            exports.customVars.push_back(varName);
        }
        
        // 扫描[Template] @Style
        std::regex templateStyleRegex(R"(\[Template\]\s*@Style\s+([a-zA-Z_][a-zA-Z0-9_]*))");
        std::sregex_iterator templateStyleIter(sourceContent.begin(), sourceContent.end(), templateStyleRegex);
        
        for (std::sregex_iterator i = templateStyleIter; i != end; ++i) {
            std::smatch match = *i;
            std::string styleName = match[1].str();
            exports.templateStyles.push_back(styleName);
        }
        
        // 扫描[Template] @Element
        std::regex templateElementRegex(R"(\[Template\]\s*@Element\s+([a-zA-Z_][a-zA-Z0-9_]*))");
        std::sregex_iterator templateElementIter(sourceContent.begin(), sourceContent.end(), templateElementRegex);
        
        for (std::sregex_iterator i = templateElementIter; i != end; ++i) {
            std::smatch match = *i;
            std::string elementName = match[1].str();
            exports.templateElements.push_back(elementName);
        }
        
        // 扫描[Template] @Var
        std::regex templateVarRegex(R"(\[Template\]\s*@Var\s+([a-zA-Z_][a-zA-Z0-9_]*))");
        std::sregex_iterator templateVarIter(sourceContent.begin(), sourceContent.end(), templateVarRegex);
        
        for (std::sregex_iterator i = templateVarIter; i != end; ++i) {
            std::smatch match = *i;
            std::string varName = match[1].str();
            exports.templateVars.push_back(varName);
        }
        
        // 扫描[Origin] @Html
        std::regex originHtmlRegex(R"(\[Origin\]\s*@Html\s+([a-zA-Z_][a-zA-Z0-9_]*))");
        std::sregex_iterator originHtmlIter(sourceContent.begin(), sourceContent.end(), originHtmlRegex);
        
        for (std::sregex_iterator i = originHtmlIter; i != end; ++i) {
            std::smatch match = *i;
            std::string htmlName = match[1].str();
            exports.originHtml.push_back(htmlName);
        }
        
        // 扫描[Origin] @Style
        std::regex originStyleRegex(R"(\[Origin\]\s*@Style\s+([a-zA-Z_][a-zA-Z0-9_]*))");
        std::sregex_iterator originStyleIter(sourceContent.begin(), sourceContent.end(), originStyleRegex);
        
        for (std::sregex_iterator i = originStyleIter; i != end; ++i) {
            std::smatch match = *i;
            std::string styleName = match[1].str();
            exports.originStyle.push_back(styleName);
        }
        
        // 扫描[Origin] @Javascript
        std::regex originJsRegex(R"(\[Origin\]\s*@Javascript\s+([a-zA-Z_][a-zA-Z0-9_]*))");
        std::sregex_iterator originJsIter(sourceContent.begin(), sourceContent.end(), originJsRegex);
        
        for (std::sregex_iterator i = originJsIter; i != end; ++i) {
            std::smatch match = *i;
            std::string jsName = match[1].str();
            exports.originJavascript.push_back(jsName);
        }
        
        // 扫描[Configuration]
        std::regex configRegex(R"(\[Configuration\]\s+([a-zA-Z_][a-zA-Z0-9_]*))");
        std::sregex_iterator configIter(sourceContent.begin(), sourceContent.end(), configRegex);
        
        for (std::sregex_iterator i = configIter; i != end; ++i) {
            std::smatch match = *i;
            std::string configName = match[1].str();
            exports.configurations.push_back(configName);
        }
        
    } catch (const std::exception& e) {
        Utils::ErrorHandler::GetInstance().LogError(
            "扫描源文件符号异常: " + std::string(e.what())
        );
    }
    
    return exports;
}

bool CMODManager::UpdateModuleExports(const std::string& modulePath, const ModuleExports& exports) {
    try {
        std::string moduleName = std::filesystem::path(modulePath).filename().string();
        std::string infoFilePath = modulePath + "/info/" + moduleName + ".chtl";
        
        // 读取现有的信息文件
        std::string existingContent;
        if (std::filesystem::exists(infoFilePath)) {
            existingContent = Utils::FileUtils::ReadFile(infoFilePath);
        }
        
        // 生成新的Export块
        std::ostringstream exportBlock;
        exportBlock << "\n// 外部查询表，由系统自动生成\n";
        exportBlock << "[Export]\n{\n";
        
        // 添加自定义样式
        if (!exports.customStyles.empty()) {
            exportBlock << "    [Custom] @Style ";
            for (size_t i = 0; i < exports.customStyles.size(); ++i) {
                if (i > 0) exportBlock << ", ";
                exportBlock << exports.customStyles[i];
            }
            exportBlock << ";\n";
        }
        
        // 添加自定义元素
        if (!exports.customElements.empty()) {
            exportBlock << "    [Custom] @Element ";
            for (size_t i = 0; i < exports.customElements.size(); ++i) {
                if (i > 0) exportBlock << ", ";
                exportBlock << exports.customElements[i];
            }
            exportBlock << ";\n";
        }
        
        // 添加自定义变量
        if (!exports.customVars.empty()) {
            exportBlock << "    [Custom] @Var ";
            for (size_t i = 0; i < exports.customVars.size(); ++i) {
                if (i > 0) exportBlock << ", ";
                exportBlock << exports.customVars[i];
            }
            exportBlock << ";\n";
        }
        
        // 添加模板样式
        if (!exports.templateStyles.empty()) {
            exportBlock << "    [Template] @Style ";
            for (size_t i = 0; i < exports.templateStyles.size(); ++i) {
                if (i > 0) exportBlock << ", ";
                exportBlock << exports.templateStyles[i];
            }
            exportBlock << ";\n";
        }
        
        // 添加模板元素
        if (!exports.templateElements.empty()) {
            exportBlock << "    [Template] @Element ";
            for (size_t i = 0; i < exports.templateElements.size(); ++i) {
                if (i > 0) exportBlock << ", ";
                exportBlock << exports.templateElements[i];
            }
            exportBlock << ";\n";
        }
        
        // 添加模板变量
        if (!exports.templateVars.empty()) {
            exportBlock << "    [Template] @Var ";
            for (size_t i = 0; i < exports.templateVars.size(); ++i) {
                if (i > 0) exportBlock << ", ";
                exportBlock << exports.templateVars[i];
            }
            exportBlock << ";\n";
        }
        
        // 添加原始嵌入
        if (!exports.originHtml.empty()) {
            exportBlock << "    [Origin] @Html ";
            for (size_t i = 0; i < exports.originHtml.size(); ++i) {
                if (i > 0) exportBlock << ", ";
                exportBlock << exports.originHtml[i];
            }
            exportBlock << ";\n";
        }
        
        if (!exports.originStyle.empty()) {
            exportBlock << "    [Origin] @Style ";
            for (size_t i = 0; i < exports.originStyle.size(); ++i) {
                if (i > 0) exportBlock << ", ";
                exportBlock << exports.originStyle[i];
            }
            exportBlock << ";\n";
        }
        
        if (!exports.originJavascript.empty()) {
            exportBlock << "    [Origin] @Javascript ";
            for (size_t i = 0; i < exports.originJavascript.size(); ++i) {
                if (i > 0) exportBlock << ", ";
                exportBlock << exports.originJavascript[i];
            }
            exportBlock << ";\n";
        }
        
        // 添加配置
        if (!exports.configurations.empty()) {
            exportBlock << "    [Configuration] @Config ";
            for (size_t i = 0; i < exports.configurations.size(); ++i) {
                if (i > 0) exportBlock << ", ";
                exportBlock << exports.configurations[i];
            }
            exportBlock << ";\n";
        }
        
        exportBlock << "}\n";
        
        // 更新或添加Export块到信息文件
        std::string newContent = existingContent;
        
        // 检查是否已存在Export块
        std::regex existingExportRegex(R"(\n?// 外部查询表[^\n]*\n\[Export\]\s*\{[^}]*\}\n?)");
        if (std::regex_search(existingContent, existingExportRegex)) {
            // 替换现有的Export块
            newContent = std::regex_replace(existingContent, existingExportRegex, exportBlock.str());
        } else {
            // 添加新的Export块
            newContent += exportBlock.str();
        }
        
        // 写入文件
        Utils::FileUtils::WriteFile(infoFilePath, newContent);
        
        Utils::ErrorHandler::GetInstance().LogInfo(
            "成功更新模块导出信息: " + infoFilePath
        );
        
        return true;
        
    } catch (const std::exception& e) {
        Utils::ErrorHandler::GetInstance().LogError(
            "更新模块导出信息失败: " + modulePath + " - " + e.what()
        );
        return false;
    }
}

} // namespace CMOD
} // namespace CHTL