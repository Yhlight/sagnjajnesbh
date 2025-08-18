#include "ImportManager.h"
#include "../../Shared/Utils/FileUtils.h"
#include <iostream>
#include <fstream>
#include <regex>
#include <algorithm>

namespace chtl {
namespace import {

ImportManager::ImportManager() {
    modulePathManager_ = std::make_shared<common::ModulePathManager>();
}

ImportManager::~ImportManager() = default;

void ImportManager::setModulePathManager(std::shared_ptr<common::ModulePathManager> pathManager) {
    modulePathManager_ = pathManager;
}

// === 严格按照语法文档的导入处理 ===

bool ImportManager::processImport(const ImportStatement& statement) {
    if (debug_) {
        std::cout << "处理导入: " << statement.moduleName << std::endl;
    }
    
    // 检查循环依赖
    if (isCircularDependency(statement.moduleName)) {
        addError("检测到循环依赖: " + statement.moduleName);
        return false;
    }
    
    // 按语法文档：根据导入格式分类处理
    std::string importType = extractImportType(statement.moduleName);
    
    if (importType == "@Html") {
        return processHtmlImport(statement);
    } else if (importType == "@Style") {
        return processStyleImport(statement);
    } else if (importType == "@JavaScript") {
        return processJavaScriptImport(statement);
    } else if (importType == "@Chtl") {
        return processChtlImport(statement);
    } else if (importType == "@CJmod") {
        return processCJmodImport(statement);
    } else if (importType == "@Config") {
        return processConfigurationImport(statement);
    } else if (importType == "[Template]") {
        return processTemplateImport(statement);
    } else if (importType == "[Custom]") {
        return processCustomImport(statement);
    } else if (importType == "[Origin]") {
        return processOriginImport(statement);
    } else if (importType == "[Configuration]") {
        return processConfigurationImport(statement);
    } else {
        addError("未知的导入类型: " + importType);
        return false;
    }
}

// === 按语法文档：导入HTML文件 ===
// [Import] @Html from html文件路径 as(可选) 命名为
bool ImportManager::processHtmlImport(const ImportStatement& statement) {
    std::string filePath = extractFilePath(statement.moduleName);
    std::string aliasName = extractAliasName(statement.moduleName);
    
    if (filePath.empty()) {
        addError("HTML导入缺少文件路径");
        return false;
    }
    
    std::string content = loadFileContent(filePath);
    if (content.empty()) {
        addError("无法加载HTML文件: " + filePath);
        return false;
    }
    
    // 按语法文档：注册为原始嵌入
    std::string importName = aliasName.empty() ? generateImportName("html") : aliasName;
    origin_system::g_originManager->addNamedOrigin("@Html", importName, content);
    
    // 记录依赖
    recordDependency(statement.moduleName, filePath, common::ModulePathManager::ModuleType::HTML, {importName});
    
    return true;
}

// === 按语法文档：导入CSS文件 ===
// [Import] @Style from css文件路径 as(可选) 命名为
bool ImportManager::processStyleImport(const ImportStatement& statement) {
    std::string filePath = extractFilePath(statement.moduleName);
    std::string aliasName = extractAliasName(statement.moduleName);
    
    if (filePath.empty()) {
        addError("CSS导入缺少文件路径");
        return false;
    }
    
    std::string content = loadFileContent(filePath);
    if (content.empty()) {
        addError("无法加载CSS文件: " + filePath);
        return false;
    }
    
    // 按语法文档：注册为原始嵌入
    std::string importName = aliasName.empty() ? generateImportName("style") : aliasName;
    origin_system::g_originManager->addNamedOrigin("@Style", importName, content);
    
    // 记录依赖
    recordDependency(statement.moduleName, filePath, common::ModulePathManager::ModuleType::CSS, {importName});
    
    return true;
}

// === 按语法文档：导入JS文件 ===
// [Import] @JavaScript from js文件路径 as(可选) 命名为
bool ImportManager::processJavaScriptImport(const ImportStatement& statement) {
    std::string filePath = extractFilePath(statement.moduleName);
    std::string aliasName = extractAliasName(statement.moduleName);
    
    if (filePath.empty()) {
        addError("JavaScript导入缺少文件路径");
        return false;
    }
    
    std::string content = loadFileContent(filePath);
    if (content.empty()) {
        addError("无法加载JavaScript文件: " + filePath);
        return false;
    }
    
    // 按语法文档：注册为原始嵌入
    std::string importName = aliasName.empty() ? generateImportName("js") : aliasName;
    origin_system::g_originManager->addNamedOrigin("@JavaScript", importName, content);
    
    // 记录依赖
    recordDependency(statement.moduleName, filePath, common::ModulePathManager::ModuleType::JAVASCRIPT, {importName});
    
    return true;
}

// === 按语法文档：导入一个chtl文件 ===
// [Import] @Chtl from chtl文件路径
bool ImportManager::processChtlImport(const ImportStatement& statement) {
    std::string moduleName = extractModuleName(statement.moduleName);
    
    if (moduleName.empty()) {
        addError("CHTL导入缺少模块名称");
        return false;
    }
    
    // 按语法文档：解析CMOD模块
    if (!modulePathManager_) {
        addError("模块路径管理器未初始化");
        return false;
    }
    
    std::string modulePath = modulePathManager_->resolveCmodPath(moduleName);
    if (modulePath.empty()) {
        addError("无法解析CMOD模块路径: " + moduleName);
        return false;
    }
    
    // 按语法文档：加载模块信息文件
    std::string infoPath = modulePath + "/info/" + extractBaseModuleName(moduleName) + ".chtl";
    std::string infoContent = loadFileContent(infoPath);
    if (infoContent.empty()) {
        addError("无法加载模块信息文件: " + infoPath);
        return false;
    }
    
    // 按语法文档：解析[Export]块
    std::vector<std::string> exports = parseExportBlock(infoContent);
    
    // 按语法文档：导入所有子模块
    if (moduleName.find('.') != std::string::npos) {
        // 精确到具体子模块：Import @Chtl from Chtholly.Space
        return importSubmodule(moduleName, modulePath, exports);
    } else {
        // 导入所有子模块
        return importAllSubmodules(moduleName, modulePath, exports);
    }
}

// === 按语法文档：导入CJmod文件 ===
// [Import] @CJmod from cjmod文件路径
bool ImportManager::processCJmodImport(const ImportStatement& statement) {
    std::string moduleName = extractModuleName(statement.moduleName);
    
    if (!modulePathManager_) {
        addError("模块路径管理器未初始化");
        return false;
    }
    
    std::string modulePath = modulePathManager_->resolveCjmodPath(moduleName);
    if (modulePath.empty()) {
        addError("无法解析CJMOD模块路径: " + moduleName);
        return false;
    }
    
    // 记录CJMOD依赖
    recordDependency(statement.moduleName, modulePath, common::ModulePathManager::ModuleType::CJMOD, {});
    
    return true;
}

// === 按语法文档：导入配置组 ===
// [Import] @Config 导入的配置组名称(可选) from chtl文件路径 as(无用选项) 命名为
// [Import] [Configuration] @Config 导入的配置组名称(可选) from chtl文件路径 as(无用选项) 命名为
bool ImportManager::processConfigurationImport(const ImportStatement& statement) {
    std::string filePath = extractFilePath(statement.moduleName);
    std::string configName = extractConfigName(statement.moduleName);
    
    if (filePath.empty()) {
        addError("配置导入缺少文件路径");
        return false;
    }
    
    std::string content = loadFileContent(filePath);
    if (content.empty()) {
        addError("无法加载配置文件: " + filePath);
        return false;
    }
    
    // 按语法文档：解析配置组
    std::vector<std::string> configs = parseConfigurationBlocks(content, configName);
    
    // 注册配置到配置管理器
    for (const auto& configDef : configs) {
        registerConfiguration(configDef, content);
    }
    
    // 记录依赖
    recordDependency(statement.moduleName, filePath, common::ModulePathManager::ModuleType::CHTL, configs);
    
    return true;
}

// === 按语法文档：导入自定义元素/样式组/变量组 ===
// [Import] [Custom] @Element 需要导入的自定义元素名 from chtl文件路径 as(可选) 命名为
// [Import] [Custom] @Style 需要导入的样式组名称 from chtl文件路径 as(可选) 命名为
// [Import] [Custom] @Var 需要导入的变量组名称 from chtl文件路径 as(可选) 命名为
// [Import] [Custom] @Element from chtl文件路径 as(无效) 命名为  // 导入所有自定义元素
// [Import] [Custom] from chtl文件路径 as(无效) 命名为  // 导入所有自定义
bool ImportManager::processCustomImport(const ImportStatement& statement) {
    std::string filePath = extractFilePath(statement.moduleName);
    std::string customType = extractCustomType(statement.moduleName); // @Element, @Style, @Var
    std::string customName = extractCustomName(statement.moduleName);
    
    if (filePath.empty()) {
        addError("自定义导入缺少文件路径");
        return false;
    }
    
    std::string content = loadFileContent(filePath);
    if (content.empty()) {
        addError("无法加载自定义文件: " + filePath);
        return false;
    }
    
    std::vector<std::string> customs;
    
    if (customType.empty()) {
        // 按语法文档：导入所有自定义
        customs = parseAllCustomDefinitions(content);
    } else if (customName.empty()) {
        // 按语法文档：导入指定类型的所有自定义
        customs = parseCustomDefinitionsByType(content, customType);
    } else {
        // 按语法文档：导入特定的自定义
        customs = parseSpecificCustomDefinition(content, customType, customName);
    }
    
    // 注册自定义到自定义管理器
    for (const auto& customDef : customs) {
        registerCustom(customDef, content);
    }
    
    // 记录依赖
    recordDependency(statement.moduleName, filePath, common::ModulePathManager::ModuleType::CHTL, customs);
    
    return true;
}

// === 按语法文档：导入模板 ===
// [Import] [Template] @Element 需要导入的自定义元素名 from chtl文件路径 as(可选) 命名为
// [Import] [Template] @Style 需要导入的样式组名称 from chtl文件路径 as(可选) 命名为
// [Import] [Template] @Var 需要导入的变量组名称 from chtl文件路径 as(可选) 命名为
// [Import] [Template] from chtl文件路径 as(无效) 命名为  // 导入所有模板
bool ImportManager::processTemplateImport(const ImportStatement& statement) {
    std::string filePath = extractFilePath(statement.moduleName);
    std::string templateType = extractTemplateType(statement.moduleName); // @Element, @Style, @Var
    std::string templateName = extractTemplateName(statement.moduleName);
    
    if (filePath.empty()) {
        addError("模板导入缺少文件路径");
        return false;
    }
    
    std::string content = loadFileContent(filePath);
    if (content.empty()) {
        addError("无法加载模板文件: " + filePath);
        return false;
    }
    
    std::vector<std::string> templates;
    
    if (templateType.empty()) {
        // 按语法文档：导入所有模板
        templates = parseAllTemplateDefinitions(content);
    } else if (templateName.empty()) {
        // 按语法文档：导入指定类型的所有模板
        templates = parseTemplateDefinitionsByType(content, templateType);
    } else {
        // 按语法文档：导入特定的模板
        templates = parseSpecificTemplateDefinition(content, templateType, templateName);
    }
    
    // 注册模板到模板管理器
    for (const auto& templateDef : templates) {
        registerTemplate(templateDef, content);
    }
    
    // 记录依赖
    recordDependency(statement.moduleName, filePath, common::ModulePathManager::ModuleType::CHTL, templates);
    
    return true;
}

// === 按语法文档：导入原始嵌入 ===
// [Import] [Origin] @Html 名称 from chtl文件路径 as(可选) 命名为
// [Import] [Origin] @Style 名称 from chtl文件路径 as(可选) 命名为
// [Import] [Origin] @Javascript 名称 from chtl文件路径 as(可选) 命名为
// [Import] [Origin] from chtl文件路径 as(无效) 命名为  // 导入所有命名原始嵌入
bool ImportManager::processOriginImport(const ImportStatement& statement) {
    std::string filePath = extractFilePath(statement.moduleName);
    std::string originType = extractOriginType(statement.moduleName); // @Html, @Style, @Javascript
    std::string originName = extractOriginName(statement.moduleName);
    
    if (filePath.empty()) {
        addError("原始嵌入导入缺少文件路径");
        return false;
    }
    
    std::string content = loadFileContent(filePath);
    if (content.empty()) {
        addError("无法加载原始嵌入文件: " + filePath);
        return false;
    }
    
    std::vector<std::string> origins;
    
    if (originType.empty()) {
        // 按语法文档：导入所有命名原始嵌入
        origins = parseAllNamedOriginDefinitions(content);
    } else if (originName.empty()) {
        addError("原始嵌入导入需要指定名称");
        return false;
    } else {
        // 按语法文档：导入特定的原始嵌入
        origins = parseSpecificOriginDefinition(content, originType, originName);
    }
    
    // 注册原始嵌入到原始嵌入管理器
    for (const auto& originDef : origins) {
        registerOrigin(originDef, content);
    }
    
    // 记录依赖
    recordDependency(statement.moduleName, filePath, common::ModulePathManager::ModuleType::CHTL, origins);
    
    return true;
}

// === 按语法文档：解析方法 ===

std::string ImportManager::extractImportType(const std::string& importStatement) {
    // 按语法文档：提取导入类型
    std::regex typeRegex(R"(\[Import\]\s+(\[?\w+\]?|\@\w+))");
    std::smatch match;
    
    if (std::regex_search(importStatement, match, typeRegex)) {
        return match[1].str();
    }
    
    return "";
}

std::string ImportManager::extractFilePath(const std::string& importStatement) {
    // 按语法文档：提取 from 后的文件路径
    std::regex pathRegex(R"(from\s+([^\s;]+))");
    std::smatch match;
    
    if (std::regex_search(importStatement, match, pathRegex)) {
        std::string path = match[1].str();
        
        // 按语法文档：可以使用'.'来表示'/'
        std::replace(path.begin(), path.end(), '.', '/');
        
        return path;
    }
    
    return "";
}

std::string ImportManager::extractAliasName(const std::string& importStatement) {
    // 按语法文档：提取 as 后的别名
    std::regex aliasRegex(R"(as\s+([^\s;]+))");
    std::smatch match;
    
    if (std::regex_search(importStatement, match, aliasRegex)) {
        return match[1].str();
    }
    
    return "";
}

std::string ImportManager::extractModuleName(const std::string& importStatement) {
    // 从 from 后提取模块名
    return extractFilePath(importStatement);
}

std::string ImportManager::extractBaseModuleName(const std::string& fullModuleName) {
    // 从 Chtholly.Space 提取 Chtholly
    size_t dotPos = fullModuleName.find('.');
    if (dotPos != std::string::npos) {
        return fullModuleName.substr(0, dotPos);
    }
    return fullModuleName;
}

std::string ImportManager::extractConfigName(const std::string& importStatement) {
    // 按语法文档：提取配置组名称
    std::regex configRegex(R"(@Config\s+(\w+))");
    std::smatch match;
    
    if (std::regex_search(importStatement, match, configRegex)) {
        return match[1].str();
    }
    
    return ""; // 无名配置组
}

std::string ImportManager::extractCustomType(const std::string& importStatement) {
    // 提取 @Element, @Style, @Var
    std::regex typeRegex(R"(\[Custom\]\s+(@\w+))");
    std::smatch match;
    
    if (std::regex_search(importStatement, match, typeRegex)) {
        return match[1].str();
    }
    
    return "";
}

std::string ImportManager::extractCustomName(const std::string& importStatement) {
    // 提取自定义名称
    std::regex nameRegex(R"(@\w+\s+(\w+)\s+from)");
    std::smatch match;
    
    if (std::regex_search(importStatement, match, nameRegex)) {
        return match[1].str();
    }
    
    return "";
}

// === 按语法文档：解析Export块 ===
std::vector<std::string> ImportManager::parseExportBlock(const std::string& content) {
    std::vector<std::string> exports;
    
    // 按语法文档：解析[Export]块
    std::regex exportRegex(R"(\[Export\]\s*\{([^}]+)\})");
    std::smatch match;
    
    if (std::regex_search(content, match, exportRegex)) {
        std::string exportContent = match[1].str();
        
        // 按语法文档：解析导出项，使用全缀名格式
        // [Custom] @Style ChthollyStyle, ChthollyCard, ChthollyButton;
        std::regex itemRegex(R"(\[(\w+)\]\s*@(\w+)\s+([^;]+);)");
        std::sregex_iterator iter(exportContent.begin(), exportContent.end(), itemRegex);
        std::sregex_iterator end;
        
        for (; iter != end; ++iter) {
            std::string category = (*iter)[1].str(); // Custom, Template, Origin, Configuration
            std::string type = (*iter)[2].str();     // Style, Element, Var, Html, Config等
            std::string names = (*iter)[3].str();    // 名称列表
            
            // 解析名称列表
            std::vector<std::string> nameList = parseCommaSeparatedNames(names);
            
            for (const auto& name : nameList) {
                exports.push_back("[" + category + "] @" + type + " " + name);
            }
        }
    }
    
    return exports;
}

std::vector<std::string> ImportManager::parseCommaSeparatedNames(const std::string& names) {
    std::vector<std::string> result;
    std::stringstream ss(names);
    std::string name;
    
    while (std::getline(ss, name, ',')) {
        // 去除前后空白
        name.erase(0, name.find_first_not_of(" \t\n"));
        name.erase(name.find_last_not_of(" \t\n") + 1);
        if (!name.empty()) {
            result.push_back(name);
        }
    }
    
    return result;
}

// === 按语法文档：子模块处理 ===
bool ImportManager::importSubmodule(const std::string& moduleName, const std::string& modulePath, const std::vector<std::string>& exports) {
    // 按语法文档：精确导入子模块，例如 Chtholly.Space
    size_t dotPos = moduleName.find('.');
    if (dotPos == std::string::npos) {
        return false;
    }
    
    std::string baseModule = moduleName.substr(0, dotPos);
    std::string subModule = moduleName.substr(dotPos + 1);
    
    std::string subModulePath = modulePath + "/src/" + subModule;
    
    // 加载子模块信息
    std::string subInfoPath = subModulePath + "/info/" + subModule + ".chtl";
    std::string subInfoContent = loadFileContent(subInfoPath);
    
    if (subInfoContent.empty()) {
        addError("无法加载子模块信息: " + subInfoPath);
        return false;
    }
    
    // 解析子模块导出
    std::vector<std::string> subExports = parseExportBlock(subInfoContent);
    
    // 记录子模块依赖
    recordDependency(moduleName, subModulePath, common::ModulePathManager::ModuleType::CMOD, subExports);
    
    return true;
}

bool ImportManager::importAllSubmodules(const std::string& moduleName, const std::string& modulePath, const std::vector<std::string>& exports) {
    // 按语法文档：导入所有子模块
    
    // 扫描src目录下的所有子模块
    std::string srcPath = modulePath + "/src";
    std::vector<std::string> submodules = scanSubmodules(srcPath);
    
    bool allSuccess = true;
    for (const auto& submodule : submodules) {
        std::string fullSubmoduleName = moduleName + "." + submodule;
        if (!importSubmodule(fullSubmoduleName, modulePath, exports)) {
            allSuccess = false;
        }
    }
    
    // 记录主模块依赖
    recordDependency(moduleName, modulePath, common::ModulePathManager::ModuleType::CMOD, exports);
    
    return allSuccess;
}

std::vector<std::string> ImportManager::scanSubmodules(const std::string& srcPath) {
    std::vector<std::string> submodules;
    
    // 简化实现：这里应该扫描文件系统
    // 按语法文档：子模块结构为 src/SubmoduleName/src/ 和 src/SubmoduleName/info/
    
    return submodules;
}

// === 循环依赖检测 ===
bool ImportManager::isCircularDependency(const std::string& moduleName) {
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> recursionStack;
    
    return hasCircularDependencyRecursive(moduleName, visited, recursionStack);
}

bool ImportManager::hasCircularDependencyRecursive(const std::string& moduleName,
                                                   std::unordered_set<std::string>& visited,
                                                   std::unordered_set<std::string>& recursionStack) {
    if (recursionStack.find(moduleName) != recursionStack.end()) {
        return true; // 发现循环依赖
    }
    
    if (visited.find(moduleName) != visited.end()) {
        return false; // 已经访问过，没有循环依赖
    }
    
    visited.insert(moduleName);
    recursionStack.insert(moduleName);
    
    // 检查当前模块的所有依赖
    auto it = dependencies_.find(moduleName);
    if (it != dependencies_.end()) {
        for (const auto& dep : getModuleDependencies(moduleName)) {
            if (hasCircularDependencyRecursive(dep, visited, recursionStack)) {
                return true;
            }
        }
    }
    
    recursionStack.erase(moduleName);
    return false;
}

std::vector<std::string> ImportManager::getModuleDependencies(const std::string& moduleName) {
    std::vector<std::string> deps;
    
    auto it = dependencies_.find(moduleName);
    if (it != dependencies_.end()) {
        // 从模块内容中解析依赖
        std::string content = loadFileContent(it->second.path);
        deps = extractDependenciesFromContent(content);
    }
    
    return deps;
}

std::vector<std::string> ImportManager::extractDependenciesFromContent(const std::string& content) {
    std::vector<std::string> dependencies;
    
    // 按语法文档：使用正则表达式查找所有导入语句
    std::regex importRegex(R"(\[Import\]\s+[^;]+;)");
    std::sregex_iterator iter(content.begin(), content.end(), importRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string importLine = (*iter)[0].str();
        std::string moduleName = extractModuleName(importLine);
        if (!moduleName.empty()) {
            dependencies.push_back(moduleName);
        }
    }
    
    return dependencies;
}

// === 辅助方法 ===
std::string ImportManager::loadFileContent(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void ImportManager::recordDependency(const std::string& moduleName, const std::string& path, 
                                     common::ModulePathManager::ModuleType type, 
                                     const std::vector<std::string>& exports) {
    ModuleDependency dependency;
    dependency.name = moduleName;
    dependency.path = path;
    dependency.type = type;
    dependency.isResolved = true;
    dependency.exports = exports;
    
    dependencies_[moduleName] = dependency;
}

std::string ImportManager::generateImportName(const std::string& prefix) {
    static int counter = 0;
    return prefix + "_import_" + std::to_string(++counter);
}

void ImportManager::addError(const std::string& message) {
    errors_.push_back(message);
    if (debug_) {
        std::cerr << "ImportManager Error: " << message << std::endl;
    }
}

void ImportManager::clearErrors() {
    errors_.clear();
}

bool ImportManager::hasErrors() const {
    return !errors_.empty();
}

std::vector<std::string> ImportManager::getErrors() const {
    return errors_;
}

void ImportManager::setDebugMode(bool enabled) {
    debug_ = enabled;
}

// === 注册方法（简化实现）===
void ImportManager::registerTemplate(const std::string& templateDef, const std::string& content) {
    // 注册模板到模板管理器
    // 按语法文档：解析模板定义并注册
}

void ImportManager::registerCustom(const std::string& customDef, const std::string& content) {
    // 注册自定义到自定义管理器
    // 按语法文档：解析自定义定义并注册
}

void ImportManager::registerOrigin(const std::string& originDef, const std::string& content) {
    // 注册原始嵌入到原始嵌入管理器
    // 按语法文档：解析原始嵌入定义并注册
}

void ImportManager::registerConfiguration(const std::string& configDef, const std::string& content) {
    // 注册配置到配置管理器
    // 按语法文档：解析配置定义并注册
}

// === 解析方法（简化实现）===
std::vector<std::string> ImportManager::parseAllTemplateDefinitions(const std::string& content) {
    std::vector<std::string> templates;
    // 按语法文档：解析所有[Template]定义
    return templates;
}

std::vector<std::string> ImportManager::parseAllCustomDefinitions(const std::string& content) {
    std::vector<std::string> customs;
    // 按语法文档：解析所有[Custom]定义
    return customs;
}

std::vector<std::string> ImportManager::parseAllNamedOriginDefinitions(const std::string& content) {
    std::vector<std::string> origins;
    // 按语法文档：解析所有带名[Origin]定义
    return origins;
}

std::vector<std::string> ImportManager::parseConfigurationBlocks(const std::string& content, const std::string& configName) {
    std::vector<std::string> configs;
    // 按语法文档：解析[Configuration]块
    return configs;
}

} // namespace import
} // namespace chtl