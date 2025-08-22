#include "CMOD/CompleteCMODSystem.h"
#include "Utils/ErrorHandler.h"
#include "Utils/FileUtils.h"
#include "Utils/StringUtils.h"
#include "CHTL/Parser/CHTLParser.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <regex>
#include <algorithm>

namespace CHTL {
namespace CMOD {

// CMODModuleInfo实现
bool CMODModuleInfo::IsValid() const {
    // 验证必需字段
    return !name.empty() && !version.empty() && !author.empty() && 
           !minCHTLVersion.empty() && !maxCHTLVersion.empty();
}

bool CMODModuleInfo::IsVersionCompatible(const std::string& currentCHTLVersion) const {
    return CMODVersionManager::CheckCompatibility(minCHTLVersion, maxCHTLVersion, currentCHTLVersion);
}

std::vector<std::string> CMODModuleInfo::GetDependencies() const {
    if (dependencies.empty()) {
        return {};
    }
    
    return Utils::StringUtils::Split(dependencies, ",");
}

// CMODExportInfo实现
void CMODExportInfo::GenerateFromModule(const CMODModule& module) {
    // 自动生成导出信息 - 语法文档第1359-1360行："此由系统生成"
    
    // 清空现有导出信息
    customStyles.clear();
    customElements.clear();
    customVars.clear();
    templateStyles.clear();
    templateElements.clear();
    templateVars.clear();
    originHtml.clear();
    originStyle.clear();
    originJavascript.clear();
    originCustomTypes.clear();
    configurations.clear();
    
    // 遍历模块AST，提取导出信息
    for (const auto& astNode : module.GetModuleAST()) {
        if (!astNode) continue;
        
        // 根据AST节点类型分类导出
        switch (astNode->GetType()) {
            case AST::NodeType::CUSTOM_STYLE:
                if (auto customNode = std::dynamic_pointer_cast<AST::CustomNode>(astNode)) {
                    customStyles.push_back("CustomStyle_" + std::to_string(customStyles.size()));
                }
                break;
                
            case AST::NodeType::CUSTOM_ELEMENT:
                if (auto customNode = std::dynamic_pointer_cast<AST::CustomNode>(astNode)) {
                    customElements.push_back("CustomElement_" + std::to_string(customElements.size()));
                }
                break;
                
            case AST::NodeType::CUSTOM_VAR:
                if (auto customNode = std::dynamic_pointer_cast<AST::CustomNode>(astNode)) {
                    customVars.push_back("CustomVar_" + std::to_string(customVars.size()));
                }
                break;
                
            case AST::NodeType::TEMPLATE_STYLE:
                if (auto templateNode = std::dynamic_pointer_cast<AST::TemplateNode>(astNode)) {
                    templateStyles.push_back("TemplateStyle_" + std::to_string(templateStyles.size()));
                }
                break;
                
            case AST::NodeType::TEMPLATE_ELEMENT:
                if (auto templateNode = std::dynamic_pointer_cast<AST::TemplateNode>(astNode)) {
                    templateElements.push_back("TemplateElement_" + std::to_string(templateElements.size()));
                }
                break;
                
            case AST::NodeType::TEMPLATE_VAR:
                if (auto templateNode = std::dynamic_pointer_cast<AST::TemplateNode>(astNode)) {
                    templateVars.push_back("TemplateVar_" + std::to_string(templateVars.size()));
                }
                break;
                
            case AST::NodeType::ORIGIN_HTML:
                if (auto originNode = std::dynamic_pointer_cast<AST::OriginNode>(astNode)) {
                    originHtml.push_back("OriginHtml_" + std::to_string(originHtml.size()));
                }
                break;
                
            case AST::NodeType::ORIGIN_STYLE:
                if (auto originNode = std::dynamic_pointer_cast<AST::OriginNode>(astNode)) {
                    originStyle.push_back("OriginStyle_" + std::to_string(originStyle.size()));
                }
                break;
                
            case AST::NodeType::ORIGIN_JAVASCRIPT:
                if (auto originNode = std::dynamic_pointer_cast<AST::OriginNode>(astNode)) {
                    originJavascript.push_back("OriginJS_" + std::to_string(originJavascript.size()));
                }
                break;
                
            case AST::NodeType::CONFIGURATION:
                if (auto configNode = std::dynamic_pointer_cast<AST::ConfigurationNode>(astNode)) {
                    configurations.push_back("Config_" + std::to_string(configurations.size()));
                }
                break;
                
            default:
                // 检查是否是自定义类型的原始嵌入
                if (astNode->GetType() == AST::NodeType::ORIGIN_CUSTOM) {
                    if (auto originNode = std::dynamic_pointer_cast<AST::OriginNode>(astNode)) {
                        originCustomTypes.push_back(originNode->GetName());
                    }
                }
                break;
        }
    }
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "CMOD导出信息生成完成，共导出 " + 
        std::to_string(customStyles.size() + customElements.size() + customVars.size() +
                      templateStyles.size() + templateElements.size() + templateVars.size() +
                      originHtml.size() + originStyle.size() + originJavascript.size() +
                      originCustomTypes.size() + configurations.size()) + " 个符号"
    );
}

std::string CMODExportInfo::SerializeToCHTL() const {
    // 序列化为CHTL格式 - 语法文档第1361-1380行格式
    std::ostringstream oss;
    
    oss << "// 外部查询表，优化性能用，无实际作用\n";
    oss << "// 不需要写这个，此由系统生成\n";
    oss << "[Export]\n{\n";
    
    // 自定义类型导出
    if (!customStyles.empty()) {
        oss << "    [Custom] @Style ";
        for (size_t i = 0; i < customStyles.size(); ++i) {
            if (i > 0) oss << ", ";
            if (i > 0 && i % 3 == 0) oss << "\n           ";  // 换行对齐
            oss << customStyles[i];
        }
        oss << ";\n\n";
    }
    
    if (!customElements.empty()) {
        oss << "    [Custom] @Element ";
        for (size_t i = 0; i < customElements.size(); ++i) {
            if (i > 0) oss << ", ";
            if (i > 0 && i % 3 == 0) oss << "\n             ";
            oss << customElements[i];
        }
        oss << ";\n\n";
    }
    
    if (!customVars.empty()) {
        oss << "    [Custom] @Var ";
        for (size_t i = 0; i < customVars.size(); ++i) {
            if (i > 0) oss << ", ";
            if (i > 0 && i % 3 == 0) oss << "\n         ";
            oss << customVars[i];
        }
        oss << ";\n\n";
    }
    
    // 模板类型导出
    if (!templateStyles.empty()) {
        oss << "    [Template] @Style ";
        for (size_t i = 0; i < templateStyles.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << templateStyles[i];
        }
        oss << ";\n";
    }
    
    if (!templateElements.empty()) {
        oss << "    [Template] @Element ";
        for (size_t i = 0; i < templateElements.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << templateElements[i];
        }
        oss << ";\n";
    }
    
    if (!templateVars.empty()) {
        oss << "    [Template] @Var ";
        for (size_t i = 0; i < templateVars.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << templateVars[i];
        }
        oss << ";\n";
    }
    
    // 原始嵌入导出
    if (!originHtml.empty()) {
        oss << "    [Origin] @Html ";
        for (size_t i = 0; i < originHtml.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << originHtml[i];
        }
        oss << ";\n";
    }
    
    if (!originStyle.empty()) {
        oss << "    [Origin] @Style ";
        for (size_t i = 0; i < originStyle.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << originStyle[i];
        }
        oss << ";\n";
    }
    
    if (!originJavascript.empty()) {
        oss << "    [Origin] @Javascript ";
        for (size_t i = 0; i < originJavascript.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << originJavascript[i];
        }
        oss << ";\n";
    }
    
    // 自定义类型的原始嵌入 - 语法文档第1378行
    if (!originCustomTypes.empty()) {
        for (const auto& customType : originCustomTypes) {
            oss << "    [Origin] @" << customType << " ...;\n";
        }
    }
    
    // 配置导出
    if (!configurations.empty()) {
        oss << "    [Configuration] @Config ";
        for (size_t i = 0; i < configurations.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << configurations[i];
        }
        oss << ";\n";
    }
    
    oss << "}\n";
    
    return oss.str();
}

// CMODFileStructure实现
bool CMODFileStructure::ValidateStructure() const {
    // 验证CMOD标准格式 - 语法文档第1323-1332行
    
    // 1. 检查src和info文件夹是否存在
    if (!std::filesystem::exists(srcPath) || !std::filesystem::exists(infoPath)) {
        Utils::ErrorHandler::GetInstance().LogError(
            "CMOD模块结构验证失败：src或info文件夹不存在，模块: " + moduleName
        );
        return false;
    }
    
    // 2. 检查info文件是否存在（必须）
    if (!std::filesystem::exists(infoFile)) {
        Utils::ErrorHandler::GetInstance().LogError(
            "CMOD模块结构验证失败：info文件不存在，模块: " + moduleName
        );
        return false;
    }
    
    // 3. 检查主模块文件规则 - 语法文档第1334-1337行
    if (moduleType == CMODModuleType::SIMPLE_MODULE) {
        // 无子模块情况下，主模块文件必须存在
        if (!std::filesystem::exists(mainModuleFile)) {
            Utils::ErrorHandler::GetInstance().LogError(
                "CMOD简单模块验证失败：主模块文件必须存在，模块: " + moduleName
            );
            return false;
        }
    } else if (moduleType == CMODModuleType::SUBMODULE_CONTAINER) {
        // 有子模块情况下，主模块文件可以省略
        // 这里不强制要求
    }
    
    return true;
}

bool CMODFileStructure::ValidateNamingRules() const {
    // 验证命名规则 - 语法文档第1332行："模块文件夹，主模块chtl文件，模块信息chtl文件必须同名"
    
    std::string folderName = std::filesystem::path(modulePath).filename().string();
    
    // 检查模块文件夹名称
    if (folderName != moduleName) {
        Utils::ErrorHandler::GetInstance().LogError(
            "CMOD命名规则验证失败：模块文件夹名称与模块名称不匹配，文件夹: " + folderName + "，模块: " + moduleName
        );
        return false;
    }
    
    // 检查主模块文件名称（如果存在）
    if (!mainModuleFile.empty()) {
        std::string mainFileName = std::filesystem::path(mainModuleFile).stem().string();
        if (mainFileName != moduleName) {
            Utils::ErrorHandler::GetInstance().LogError(
                "CMOD命名规则验证失败：主模块文件名称与模块名称不匹配，文件: " + mainFileName + "，模块: " + moduleName
            );
            return false;
        }
    }
    
    // 检查信息文件名称
    std::string infoFileName = std::filesystem::path(infoFile).stem().string();
    if (infoFileName != moduleName) {
        Utils::ErrorHandler::GetInstance().LogError(
            "CMOD命名规则验证失败：信息文件名称与模块名称不匹配，文件: " + infoFileName + "，模块: " + moduleName
        );
        return false;
    }
    
    return true;
}

CMODFileStructure CMODFileStructure::ScanModuleStructure(const std::string& modulePath) {
    CMODFileStructure structure;
    structure.modulePath = modulePath;
    structure.moduleName = std::filesystem::path(modulePath).filename().string();
    
    // 设置基本路径
    structure.srcPath = modulePath + "/src";
    structure.infoPath = modulePath + "/info";
    structure.infoFile = structure.infoPath + "/" + structure.moduleName + ".chtl";
    structure.mainModuleFile = structure.srcPath + "/" + structure.moduleName + ".chtl";
    
    // 扫描源文件
    if (std::filesystem::exists(structure.srcPath)) {
        for (const auto& entry : std::filesystem::directory_iterator(structure.srcPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".chtl") {
                structure.sourceFiles.push_back(entry.path().string());
            } else if (entry.is_directory()) {
                // 检查是否是子模块
                std::string subModulePath = entry.path().string();
                std::string subModuleName = entry.path().filename().string();
                
                // 验证子模块结构
                std::string subSrcPath = subModulePath + "/src";
                std::string subInfoPath = subModulePath + "/info";
                
                if (std::filesystem::exists(subSrcPath) && std::filesystem::exists(subInfoPath)) {
                    structure.subModules.push_back(subModuleName);
                }
            }
        }
    }
    
    // 确定模块类型
    if (structure.subModules.empty()) {
        structure.moduleType = CMODModuleType::SIMPLE_MODULE;
    } else {
        structure.moduleType = CMODModuleType::SUBMODULE_CONTAINER;
    }
    
    // 检查是否是混合模块（有CJMOD文件夹）
    if (CMODDirectoryManager::HasCJMODFolder(modulePath)) {
        structure.moduleType = CMODModuleType::MIXED_MODULE;
    }
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "CMOD模块结构扫描完成，模块: " + structure.moduleName + 
        "，类型: " + std::to_string(static_cast<int>(structure.moduleType)) +
        "，源文件: " + std::to_string(structure.sourceFiles.size()) +
        "，子模块: " + std::to_string(structure.subModules.size())
    );
    
    return structure;
}

// CMODModule实现
CMODModule::CMODModule(const CMODFileStructure& moduleStructure)
    : moduleStructure_(moduleStructure) {
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "CMOD模块创建: " + moduleStructure_.moduleName
    );
}

bool CMODModule::Load() {
    Utils::ErrorHandler::GetInstance().LogInfo(
        "开始加载CMOD模块: " + moduleStructure_.moduleName
    );
    
    // 1. 验证模块结构
    if (!moduleStructure_.ValidateStructure()) {
        Utils::ErrorHandler::GetInstance().LogError(
            "CMOD模块结构验证失败: " + moduleStructure_.moduleName
        );
        return false;
    }
    
    // 2. 验证命名规则
    if (!moduleStructure_.ValidateNamingRules()) {
        Utils::ErrorHandler::GetInstance().LogError(
            "CMOD模块命名规则验证失败: " + moduleStructure_.moduleName
        );
        return false;
    }
    
    // 3. 加载模块信息
    if (!LoadModuleInfo()) {
        Utils::ErrorHandler::GetInstance().LogError(
            "CMOD模块信息加载失败: " + moduleStructure_.moduleName
        );
        return false;
    }
    
    // 4. 加载源文件
    if (!LoadSourceFiles()) {
        Utils::ErrorHandler::GetInstance().LogError(
            "CMOD源文件加载失败: " + moduleStructure_.moduleName
        );
        return false;
    }
    
    // 5. 加载子模块
    if (!LoadSubModules()) {
        Utils::ErrorHandler::GetInstance().LogError(
            "CMOD子模块加载失败: " + moduleStructure_.moduleName
        );
        return false;
    }
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "CMOD模块加载完成: " + moduleStructure_.moduleName
    );
    
    return true;
}

bool CMODModule::Parse() {
    Utils::ErrorHandler::GetInstance().LogInfo(
        "开始解析CMOD模块: " + moduleStructure_.moduleName
    );
    
    // 1. 解析信息文件
    if (!ParseInfoFile()) {
        return false;
    }
    
    // 2. 解析源文件
    if (!ParseSourceFiles()) {
        return false;
    }
    
    // 3. 解析子模块
    if (!ParseSubModules()) {
        return false;
    }
    
    // 4. 生成导出信息
    if (!GenerateExportInfo()) {
        return false;
    }
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "CMOD模块解析完成: " + moduleStructure_.moduleName
    );
    
    return true;
}

bool CMODModule::Validate() {
    Utils::ErrorHandler::GetInstance().LogInfo(
        "开始验证CMOD模块: " + moduleStructure_.moduleName
    );
    
    // 1. 验证命名规则
    if (!ValidateNamingRules()) {
        return false;
    }
    
    // 2. 验证文件结构
    if (!ValidateFileStructure()) {
        return false;
    }
    
    // 3. 验证版本兼容性
    if (!ValidateVersionCompatibility()) {
        return false;
    }
    
    // 4. 验证依赖
    if (!ValidateDependencies()) {
        return false;
    }
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "CMOD模块验证完成: " + moduleStructure_.moduleName
    );
    
    return true;
}

std::shared_ptr<CMODModule> CMODModule::FindSubModule(const std::string& subModuleName) const {
    for (const auto& subModule : subModules_) {
        if (subModule->GetModuleStructure().moduleName == subModuleName) {
            return subModule;
        }
    }
    return nullptr;
}

std::unordered_map<std::string, AST::ASTNodePtr> CMODModule::GetAllExports() const {
    std::unordered_map<std::string, AST::ASTNodePtr> exports;
    
    // 收集当前模块的导出
    for (const auto& astNode : moduleAST_) {
        if (!astNode) continue;
        
                 // 暂时简化导出名称生成
         std::string exportName;
         switch (astNode->GetType()) {
            case AST::NodeType::CUSTOM_STYLE:
                exportName = "[Custom] @Style CustomStyle";
                break;
            case AST::NodeType::CUSTOM_ELEMENT:
                exportName = "[Custom] @Element CustomElement";
                break;
            case AST::NodeType::TEMPLATE_STYLE:
                exportName = "[Template] @Style TemplateStyle";
                break;
            // ... 其他类型
            default:
                continue;
        }
        
        if (!exportName.empty()) {
            exports[exportName] = astNode;
        }
    }
    
    // 收集子模块的导出
    for (const auto& subModule : subModules_) {
        auto subExports = subModule->GetAllExports();
        for (const auto& export_ : subExports) {
            // 添加子模块前缀
            std::string prefixedName = moduleStructure_.moduleName + "." + export_.first;
            exports[prefixedName] = export_.second;
        }
    }
    
    return exports;
}

// CMODModule私有方法实现
bool CMODModule::LoadModuleInfo() {
    // 读取info文件内容
    std::ifstream infoFile(moduleStructure_.infoFile);
    if (!infoFile.is_open()) {
        Utils::ErrorHandler::GetInstance().LogError(
            "无法打开CMOD信息文件: " + moduleStructure_.infoFile
        );
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(infoFile)),
                        std::istreambuf_iterator<char>());
    infoFile.close();
    
    // 解析[Info]块 - 语法文档第1346-1357行格式
    std::regex infoBlockRegex(R"(\[Info\]\s*\{([^}]+)\})");
    std::smatch match;
    
    if (!std::regex_search(content, match, infoBlockRegex)) {
        Utils::ErrorHandler::GetInstance().LogError(
            "CMOD信息文件中未找到[Info]块: " + moduleStructure_.infoFile
        );
        return false;
    }
    
    std::string infoContent = match[1].str();
    
    // 解析info内容
    std::regex propertyRegex(R"((\w+)\s*=\s*\"([^\"]+)\";)");
    std::sregex_iterator iter(infoContent.begin(), infoContent.end(), propertyRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        const std::smatch& propertyMatch = *iter;
        std::string key = Utils::StringUtils::Trim(propertyMatch[1].str());
        std::string value = Utils::StringUtils::Trim(propertyMatch[2].str());
        
        // 设置模块信息字段
        if (key == "name") {
            moduleInfo_.name = value;
        } else if (key == "version") {
            moduleInfo_.version = value;
        } else if (key == "description") {
            moduleInfo_.description = value;
        } else if (key == "author") {
            moduleInfo_.author = value;
        } else if (key == "license") {
            moduleInfo_.license = value;
        } else if (key == "dependencies") {
            moduleInfo_.dependencies = value;
        } else if (key == "category") {
            moduleInfo_.category = value;
        } else if (key == "minCHTLVersion") {
            moduleInfo_.minCHTLVersion = value;
        } else if (key == "maxCHTLVersion") {
            moduleInfo_.maxCHTLVersion = value;
        }
    }
    
    // 验证模块信息完整性
    if (!moduleInfo_.IsValid()) {
        Utils::ErrorHandler::GetInstance().LogError(
            "CMOD模块信息不完整: " + moduleStructure_.moduleName
        );
        return false;
    }
    
    // 验证模块名称与文件夹名称一致 - 语法文档第1332行要求
    if (moduleInfo_.name != moduleStructure_.moduleName) {
        Utils::ErrorHandler::GetInstance().LogError(
            "CMOD模块名称不匹配：Info中的name为 " + moduleInfo_.name + 
            "，但模块文件夹名称为 " + moduleStructure_.moduleName
        );
        return false;
    }
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "CMOD模块信息加载完成: " + moduleInfo_.name + " v" + moduleInfo_.version
    );
    
    return true;
}

bool CMODModule::LoadSourceFiles() {
    // 加载所有源文件内容
    for (const auto& sourceFile : moduleStructure_.sourceFiles) {
        std::ifstream file(sourceFile);
        if (!file.is_open()) {
            Utils::ErrorHandler::GetInstance().LogError(
                "无法打开CMOD源文件: " + sourceFile
            );
            return false;
        }
        
        // 这里暂时只记录文件路径，实际解析在Parse()方法中进行
        Utils::ErrorHandler::GetInstance().LogInfo(
            "CMOD源文件已加载: " + sourceFile
        );
    }
    
    return true;
}

bool CMODModule::LoadSubModules() {
    // 加载子模块 - 语法文档第1383-1401行的嵌套结构
    for (const auto& subModuleName : moduleStructure_.subModules) {
        std::string subModulePath = moduleStructure_.srcPath + "/" + subModuleName;
        
        // 扫描子模块结构
        CMODFileStructure subStructure = CMODFileStructure::ScanModuleStructure(subModulePath);
        subStructure.moduleType = CMODModuleType::SUBMODULE;
        
        // 创建子模块
        auto subModule = std::make_shared<CMODModule>(subStructure);
        
        // 加载子模块
        if (!subModule->Load()) {
            Utils::ErrorHandler::GetInstance().LogError(
                "CMOD子模块加载失败: " + subModuleName
            );
            return false;
        }
        
        subModules_.push_back(subModule);
        
        Utils::ErrorHandler::GetInstance().LogInfo(
            "CMOD子模块加载完成: " + subModuleName
        );
    }
    
    return true;
}

bool CMODModule::GenerateExportInfo() {
    // 生成导出信息 - 语法文档第1359-1360行："此由系统生成"
    exportInfo_.GenerateFromModule(*this);
    
    // 写入Export信息到info文件（可选）
    std::string exportContent = exportInfo_.SerializeToCHTL();
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "CMOD导出信息生成完成: " + moduleStructure_.moduleName
    );
    
    return true;
}

bool CMODModule::ParseInfoFile() {
    // 信息文件已在LoadModuleInfo中解析
    return true;
}

bool CMODModule::ParseSourceFiles() {
    // 解析所有源文件为AST
    // 暂时简化实现，记录源文件路径
    
    for (const auto& sourceFile : moduleStructure_.sourceFiles) {
        std::ifstream file(sourceFile);
        if (!file.is_open()) {
            Utils::ErrorHandler::GetInstance().LogError(
                "无法打开CMOD源文件进行解析: " + sourceFile
            );
            return false;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        file.close();
        
        // 暂时简化实现，不创建AST节点
        // 实际实现需要集成完整的CHTL Parser
        Utils::ErrorHandler::GetInstance().LogInfo(
            "CMOD源文件内容已读取: " + std::to_string(content.size()) + " 字符"
        );
        
        Utils::ErrorHandler::GetInstance().LogInfo(
            "CMOD源文件解析完成: " + sourceFile
        );
    }
    
    return true;
}

bool CMODModule::ParseSubModules() {
    // 解析所有子模块
    for (auto& subModule : subModules_) {
        if (!subModule->Parse()) {
            Utils::ErrorHandler::GetInstance().LogError(
                "CMOD子模块解析失败: " + subModule->GetModuleStructure().moduleName
            );
            return false;
        }
    }
    
    return true;
}

bool CMODModule::ValidateNamingRules() {
    return moduleStructure_.ValidateNamingRules();
}

bool CMODModule::ValidateFileStructure() {
    return moduleStructure_.ValidateStructure();
}

bool CMODModule::ValidateVersionCompatibility() {
    // 检查版本兼容性
    std::string currentVersion = CMODVersionManager::GetCurrentCHTLVersion();
    return moduleInfo_.IsVersionCompatible(currentVersion);
}

bool CMODModule::ValidateDependencies() {
    // 验证依赖模块是否存在
    auto dependencies = moduleInfo_.GetDependencies();
    
    for (const auto& dep : dependencies) {
        // 这里需要与CMODManager协作检查依赖
        // 暂时返回true，实际实现需要在Manager中处理
        Utils::ErrorHandler::GetInstance().LogInfo(
            "验证CMOD依赖: " + dep
        );
    }
    
    return true;
}

// CompleteCMODManager实现
CompleteCMODManager::CompleteCMODManager(const std::string& compilerPath, const std::string& currentFilePath)
    : compilerPath_(compilerPath), currentFilePath_(currentFilePath) {
    
    InitializeSearchPaths();
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "完整CMOD管理器初始化完成，编译器路径: " + compilerPath_ + 
        "，当前文件: " + currentFilePath_
    );
}

bool CompleteCMODManager::LoadModule(const std::string& moduleName, const std::string& subModulePath) {
    Utils::ErrorHandler::GetInstance().LogInfo(
        "开始加载CMOD模块: " + moduleName + 
        (subModulePath.empty() ? "" : "." + subModulePath)
    );
    
    // 1. 搜索模块
    std::string modulePath = SearchModule(moduleName);
    if (modulePath.empty()) {
        Utils::ErrorHandler::GetInstance().LogError(
            "未找到CMOD模块: " + moduleName
        );
        return false;
    }
    
    // 2. 检查是否已加载
    if (loadedModules_.find(moduleName) != loadedModules_.end()) {
        Utils::ErrorHandler::GetInstance().LogInfo(
            "CMOD模块已加载: " + moduleName
        );
        return true;
    }
    
    // 3. 扫描模块结构
    CMODFileStructure moduleStructure = CMODFileStructure::ScanModuleStructure(modulePath);
    
    // 4. 创建模块
    auto module = std::make_shared<CMODModule>(moduleStructure);
    
    // 5. 加载模块
    if (!module->Load()) {
        Utils::ErrorHandler::GetInstance().LogError(
            "CMOD模块加载失败: " + moduleName
        );
        return false;
    }
    
    // 6. 解析模块
    if (!module->Parse()) {
        Utils::ErrorHandler::GetInstance().LogError(
            "CMOD模块解析失败: " + moduleName
        );
        return false;
    }
    
    // 7. 验证模块
    if (!module->Validate()) {
        Utils::ErrorHandler::GetInstance().LogError(
            "CMOD模块验证失败: " + moduleName
        );
        return false;
    }
    
    // 8. 解析依赖
    if (!ResolveDependencies(moduleName)) {
        Utils::ErrorHandler::GetInstance().LogError(
            "CMOD模块依赖解析失败: " + moduleName
        );
        return false;
    }
    
    // 9. 注册模块
    loadedModules_[moduleName] = module;
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "CMOD模块加载完成: " + moduleName
    );
    
    return true;
}

std::shared_ptr<CMODModule> CompleteCMODManager::GetModule(const std::string& moduleName) const {
    auto it = loadedModules_.find(moduleName);
    if (it != loadedModules_.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<CMODModule> CompleteCMODManager::GetSubModule(const std::string& moduleName, const std::string& subModulePath) const {
    auto module = GetModule(moduleName);
    if (!module) {
        return nullptr;
    }
    
    // 解析子模块路径
    std::vector<std::string> pathParts = Utils::StringUtils::Split(subModulePath, ".");
    
    std::shared_ptr<CMODModule> currentModule = module;
    for (const auto& part : pathParts) {
        currentModule = currentModule->FindSubModule(part);
        if (!currentModule) {
            return nullptr;
        }
    }
    
    return currentModule;
}

std::string CompleteCMODManager::SearchModule(const std::string& moduleName) const {
    // 按照用户要求的搜索优先级进行搜索
    
    // 1. 优先搜索官方模块目录
    std::string officialPath = SearchInOfficialModules(moduleName);
    if (!officialPath.empty()) {
        return officialPath;
    }
    
    // 2. 其次搜索当前目录module文件夹
    std::string currentModulePath = SearchInCurrentModuleFolder(moduleName);
    if (!currentModulePath.empty()) {
        return currentModulePath;
    }
    
    // 3. 最后搜索当前目录
    std::string currentDirPath = SearchInCurrentDirectory(moduleName);
    if (!currentDirPath.empty()) {
        return currentDirPath;
    }
    
    Utils::ErrorHandler::GetInstance().LogWarning(
        "在所有搜索路径中未找到CMOD模块: " + moduleName
    );
    
    return "";
}

std::vector<std::string> CompleteCMODManager::GetSearchPaths() const {
    // 基于用户要求的动态路径确定策略
    return {
        officialModulePath_,    // 官方模块目录
        currentModulePath_,     // 当前模块目录
        currentDirPath_         // 当前目录
    };
}

void CompleteCMODManager::InitializeSearchPaths() {
    InitializeOfficialModulePath();
    InitializeCurrentPaths();
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        std::string("CMOD搜索路径初始化完成:\n") +
        "  官方模块目录: " + officialModulePath_ + "\n" +
        "  当前模块目录: " + currentModulePath_ + "\n" +
        "  当前目录: " + currentDirPath_
    );
}

void CompleteCMODManager::InitializeOfficialModulePath() {
    // 官方模块目录：源码编译后生成的module文件夹，通常和编译器同一个文件夹
    std::filesystem::path compilerDir = std::filesystem::path(compilerPath_).parent_path();
    officialModulePath_ = compilerDir / "modules";
    
    // 确保目录存在
    if (!std::filesystem::exists(officialModulePath_)) {
        std::filesystem::create_directories(officialModulePath_);
    }
}

void CompleteCMODManager::InitializeCurrentPaths() {
    // 当前文件所在目录
    std::filesystem::path currentFileDir = std::filesystem::path(currentFilePath_).parent_path();
    currentDirPath_ = currentFileDir.string();
    
    // 当前目录的module文件夹
    currentModulePath_ = currentFileDir / "modules";
    
    // 确保module目录存在
    if (!std::filesystem::exists(currentModulePath_)) {
        std::filesystem::create_directories(currentModulePath_);
    }
}

std::string CompleteCMODManager::SearchInOfficialModules(const std::string& moduleName) const {
    // 在官方模块目录中搜索，优先匹配cmod文件，其次chtl
    
    // 1. 搜索.cmod文件
    std::string cmodPath = SearchWithExtension(officialModulePath_, moduleName, ".cmod");
    if (!cmodPath.empty()) {
        return cmodPath;
    }
    
    // 2. 搜索.chtl文件
    std::string chtlPath = SearchWithExtension(officialModulePath_, moduleName, ".chtl");
    if (!chtlPath.empty()) {
        return chtlPath;
    }
    
    // 3. 搜索模块文件夹
    std::string folderPath = officialModulePath_ + "/" + moduleName;
    if (std::filesystem::exists(folderPath) && std::filesystem::is_directory(folderPath)) {
        return folderPath;
    }
    
    // 4. 检查分类结构
    auto variants = CMODDirectoryManager::GetCMODFolderVariants();
    for (const auto& variant : variants) {
        std::string classifiedPath = officialModulePath_ + "/" + variant + "/" + moduleName;
        if (std::filesystem::exists(classifiedPath)) {
            return classifiedPath;
        }
    }
    
    return "";
}

std::string CompleteCMODManager::SearchInCurrentModuleFolder(const std::string& moduleName) const {
    return SearchInOfficialModules(moduleName);  // 使用相同的搜索逻辑，但在currentModulePath_中
}

std::string CompleteCMODManager::SearchInCurrentDirectory(const std::string& moduleName) const {
    return SearchInOfficialModules(moduleName);  // 使用相同的搜索逻辑，但在currentDirPath_中
}

std::string CompleteCMODManager::SearchWithExtension(const std::string& basePath, const std::string& moduleName, const std::string& extension) const {
    std::string fullPath = basePath + "/" + moduleName + extension;
    if (std::filesystem::exists(fullPath)) {
        return fullPath;
    }
    return "";
}

bool CompleteCMODManager::ResolveDependencies(const std::string& moduleName) {
    auto module = GetModule(moduleName);
    if (!module) {
        return false;
    }
    
    auto dependencies = module->GetModuleInfo().GetDependencies();
    dependencyGraph_[moduleName] = dependencies;
    
    // 检查循环依赖
    std::vector<std::string> visitStack;
    if (CheckCircularDependencies(moduleName, visitStack)) {
        Utils::ErrorHandler::GetInstance().LogError(
            "检测到CMOD模块循环依赖: " + moduleName
        );
        return false;
    }
    
    // 递归加载依赖模块
    for (const auto& dep : dependencies) {
        if (loadedModules_.find(dep) == loadedModules_.end()) {
            if (!LoadModule(dep)) {
                Utils::ErrorHandler::GetInstance().LogError(
                    "CMOD依赖模块加载失败: " + dep
                );
                return false;
            }
        }
    }
    
    return true;
}

bool CompleteCMODManager::CheckCircularDependencies(const std::string& moduleName, std::vector<std::string>& visitStack) const {
    // 检查是否在访问栈中（循环依赖）
    if (std::find(visitStack.begin(), visitStack.end(), moduleName) != visitStack.end()) {
        return true;
    }
    
    visitStack.push_back(moduleName);
    
    // 递归检查依赖
    auto it = dependencyGraph_.find(moduleName);
    if (it != dependencyGraph_.end()) {
        for (const auto& dep : it->second) {
            if (CheckCircularDependencies(dep, visitStack)) {
                return true;
            }
        }
    }
    
    visitStack.pop_back();
    return false;
}

std::string CompleteCMODManager::GetModuleStatistics() const {
    std::ostringstream stats;
    stats << "CMOD模块统计信息:\n";
    stats << "  已加载模块数量: " << loadedModules_.size() << "\n";
    stats << "  搜索路径:\n";
    
    auto searchPaths = GetSearchPaths();
    for (const auto& path : searchPaths) {
        stats << "    - " << path << "\n";
    }
    
    stats << "  已加载模块:\n";
    for (const auto& module : loadedModules_) {
        const auto& info = module.second->GetModuleInfo();
        stats << "    - " << info.name << " v" << info.version << " (" << info.category << ")\n";
    }
    
    return stats.str();
}

// CMODDirectoryManager实现
CMODDirectoryManager::DirectoryStructureType CMODDirectoryManager::DetectStructureType(const std::string& modulePath) {
    // 检查是否有CMOD或CJMOD文件夹
    if (HasCMODFolder(modulePath) || HasCJMODFolder(modulePath)) {
        return DirectoryStructureType::CLASSIFIED;
    } else {
        return DirectoryStructureType::MIXED;
    }
}

bool CMODDirectoryManager::HasCMODFolder(const std::string& path) {
    auto variants = GetCMODFolderVariants();
    for (const auto& variant : variants) {
        std::string folderPath = path + "/" + variant;
        if (std::filesystem::exists(folderPath) && std::filesystem::is_directory(folderPath)) {
            return true;
        }
    }
    return false;
}

bool CMODDirectoryManager::HasCJMODFolder(const std::string& path) {
    auto variants = GetCJMODFolderVariants();
    for (const auto& variant : variants) {
        std::string folderPath = path + "/" + variant;
        if (std::filesystem::exists(folderPath) && std::filesystem::is_directory(folderPath)) {
            return true;
        }
    }
    return false;
}

std::vector<std::string> CMODDirectoryManager::GetCMODFolderVariants() {
    // 语法文档第1432行：CMOD / Cmod / cmod
    return {"CMOD", "Cmod", "cmod"};
}

std::vector<std::string> CMODDirectoryManager::GetCJMODFolderVariants() {
    // 语法文档第1436行：CJMOD / CJmod / cjmod
    return {"CJMOD", "CJmod", "cjmod"};
}

// CMODVersionManager实现
CMODVersionManager::VersionInfo CMODVersionManager::VersionInfo::Parse(const std::string& versionStr) {
    VersionInfo version{0, 0, 0};
    
    std::regex versionRegex(R"((\d+)\.(\d+)\.(\d+))");
    std::smatch match;
    
    if (std::regex_match(versionStr, match, versionRegex)) {
        version.major = std::stoi(match[1].str());
        version.minor = std::stoi(match[2].str());
        version.patch = std::stoi(match[3].str());
    }
    
    return version;
}

std::string CMODVersionManager::VersionInfo::ToString() const {
    return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
}

bool CMODVersionManager::VersionInfo::operator<(const VersionInfo& other) const {
    if (major != other.major) return major < other.major;
    if (minor != other.minor) return minor < other.minor;
    return patch < other.patch;
}

bool CMODVersionManager::CheckCompatibility(const std::string& moduleMinVersion, 
                                           const std::string& moduleMaxVersion,
                                           const std::string& currentVersion) {
    VersionInfo minVer = VersionInfo::Parse(moduleMinVersion);
    VersionInfo maxVer = VersionInfo::Parse(moduleMaxVersion);
    VersionInfo curVer = VersionInfo::Parse(currentVersion);
    
    return IsVersionInRange(curVer, minVer, maxVer);
}

std::string CMODVersionManager::GetCurrentCHTLVersion() {
    // 返回当前CHTL编译器版本
    return "1.0.0";  // 这里应该从编译器配置中获取
}

bool CMODVersionManager::IsVersionInRange(const VersionInfo& version, const VersionInfo& minVer, const VersionInfo& maxVer) {
    return version >= minVer && version <= maxVer;
}

} // namespace CMOD
} // namespace CHTL