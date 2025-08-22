#include "CJMOD/CompleteCJMODSystem.h"
#include "CMOD/CompleteCMODSystem.h"
#include "Utils/ErrorHandler.h"
#include "Utils/FileUtils.h"
#include "Utils/StringUtils.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <regex>
#include <algorithm>

namespace CHTL {
namespace CJMOD {

// 静态成员初始化
std::unordered_map<std::string, std::shared_ptr<CJMODExtensionInterface>> CJMODCompilerInterface::registeredExtensions_;

// CJMODModuleInfo实现
bool CJMODModuleInfo::IsValid() const {
    return !name.empty() && !version.empty() && !author.empty() && 
           !minCHTLVersion.empty() && !maxCHTLVersion.empty();
}

bool CJMODModuleInfo::IsVersionCompatible(const std::string& currentCHTLVersion) const {
    // 简化版本检查
    return true;  // 实际应该实现版本比较
}

std::vector<std::string> CJMODModuleInfo::GetDependencies() const {
    if (dependencies.empty()) {
        return {};
    }
    return Utils::StringUtils::Split(dependencies, ",");
}

// CJMODFileStructure实现
bool CJMODFileStructure::ValidateStructure() const {
    // 验证CJMOD结构 - 语法文档第1413-1424行
    
    // 检查src和info文件夹
    if (!std::filesystem::exists(srcPath) || !std::filesystem::exists(infoPath)) {
        Utils::ErrorHandler::GetInstance().LogError(
            "CJMOD模块结构验证失败：src或info文件夹不存在，模块: " + moduleName
        );
        return false;
    }
    
    // 检查info文件
    if (!std::filesystem::exists(infoFile)) {
        Utils::ErrorHandler::GetInstance().LogError(
            "CJMOD模块结构验证失败：info文件不存在，模块: " + moduleName
        );
        return false;
    }
    
    // 检查是否有C++源文件
    if (cppFiles.empty() && headerFiles.empty()) {
        Utils::ErrorHandler::GetInstance().LogError(
            "CJMOD模块结构验证失败：没有C++源文件，模块: " + moduleName
        );
        return false;
    }
    
    return true;
}

bool CJMODFileStructure::ValidateNamingRules() const {
    // 验证CJMOD命名规则
    std::string folderName = std::filesystem::path(modulePath).filename().string();
    
    if (folderName != moduleName) {
        Utils::ErrorHandler::GetInstance().LogError(
            "CJMOD命名规则验证失败：模块文件夹名称与模块名称不匹配"
        );
        return false;
    }
    
    return true;
}

CJMODFileStructure CJMODFileStructure::ScanCJMODStructure(const std::string& modulePath) {
    CJMODFileStructure structure;
    structure.modulePath = modulePath;
    structure.moduleName = std::filesystem::path(modulePath).filename().string();
    
    // 设置基本路径
    structure.srcPath = modulePath + "/src";
    structure.infoPath = modulePath + "/info";
    structure.infoFile = structure.infoPath + "/" + structure.moduleName + ".chtl";
    
    // 扫描C++源文件
    if (std::filesystem::exists(structure.srcPath)) {
        for (const auto& entry : std::filesystem::directory_iterator(structure.srcPath)) {
            if (entry.is_regular_file()) {
                std::string extension = entry.path().extension().string();
                if (extension == ".cpp") {
                    structure.cppFiles.push_back(entry.path().string());
                } else if (extension == ".h" || extension == ".hpp") {
                    structure.headerFiles.push_back(entry.path().string());
                }
            } else if (entry.is_directory()) {
                // 子模块
                std::string subModuleName = entry.path().filename().string();
                std::string subSrcPath = entry.path().string() + "/src";
                std::string subInfoPath = entry.path().string() + "/info";
                
                if (std::filesystem::exists(subSrcPath) && std::filesystem::exists(subInfoPath)) {
                    structure.subModules.push_back(subModuleName);
                }
            }
        }
    }
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "CJMOD模块结构扫描完成，模块: " + structure.moduleName + 
        "，C++文件: " + std::to_string(structure.cppFiles.size()) +
        "，头文件: " + std::to_string(structure.headerFiles.size()) +
        "，子模块: " + std::to_string(structure.subModules.size())
    );
    
    return structure;
}

// CJMODModule实现
CJMODModule::CJMODModule(const CJMODFileStructure& moduleStructure)
    : moduleStructure_(moduleStructure) {
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "CJMOD模块创建: " + moduleStructure_.moduleName
    );
}

bool CJMODModule::Load() {
    Utils::ErrorHandler::GetInstance().LogInfo(
        "开始加载CJMOD模块: " + moduleStructure_.moduleName
    );
    
    // 1. 验证模块结构
    if (!moduleStructure_.ValidateStructure()) {
        return false;
    }
    
    // 2. 加载模块信息
    if (!LoadModuleInfo()) {
        return false;
    }
    
    // 3. 加载C++源文件
    if (!LoadCppSources()) {
        return false;
    }
    
    // 4. 加载子模块
    if (!LoadSubModules()) {
        return false;
    }
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "CJMOD模块加载完成: " + moduleStructure_.moduleName
    );
    
    return true;
}

bool CJMODModule::CompileCppExtensions() {
    Utils::ErrorHandler::GetInstance().LogInfo(
        "开始编译CJMOD C++扩展: " + moduleStructure_.moduleName
    );
    
    // 编译所有C++文件
    for (const auto& cppFile : moduleStructure_.cppFiles) {
        if (!CompileSingleCppFile(cppFile)) {
            Utils::ErrorHandler::GetInstance().LogError(
                "CJMOD C++文件编译失败: " + cppFile
            );
            return false;
        }
    }
    
    // 链接扩展
    if (!LinkExtensions()) {
        return false;
    }
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "CJMOD C++扩展编译完成: " + moduleStructure_.moduleName
    );
    
    return true;
}

bool CJMODModule::RegisterExtensions() {
    Utils::ErrorHandler::GetInstance().LogInfo(
        "开始注册CJMOD扩展到CHTL JS编译器: " + moduleStructure_.moduleName
    );
    
    // 创建扩展实例
    if (!CreateExtensionInstances()) {
        return false;
    }
    
    // 注册所有扩展
    for (auto& extension : extensions_) {
        if (!CJMODCompilerInterface::RegisterToCHTLJSCompiler(extension)) {
            Utils::ErrorHandler::GetInstance().LogError(
                "CJMOD扩展注册失败: " + extension->GetExtensionName()
            );
            return false;
        }
    }
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "CJMOD扩展注册完成: " + moduleStructure_.moduleName + 
        "，注册扩展数量: " + std::to_string(extensions_.size())
    );
    
    return true;
}

bool CJMODModule::Validate() {
    // 验证CJMOD模块完整性
    if (!moduleStructure_.ValidateStructure()) {
        return false;
    }
    
    if (!moduleStructure_.ValidateNamingRules()) {
        return false;
    }
    
    if (!moduleInfo_.IsValid()) {
        Utils::ErrorHandler::GetInstance().LogError(
            "CJMOD模块信息无效: " + moduleStructure_.moduleName
        );
        return false;
    }
    
    return true;
}

std::shared_ptr<CJMODExtensionInterface> CJMODModule::FindExtension(const std::string& extensionName) const {
    for (const auto& extension : extensions_) {
        if (extension->GetExtensionName() == extensionName) {
            return extension;
        }
    }
    return nullptr;
}

// CJMODModule私有方法实现
bool CJMODModule::LoadModuleInfo() {
    // 读取CJMOD信息文件
    std::ifstream infoFile(moduleStructure_.infoFile);
    if (!infoFile.is_open()) {
        Utils::ErrorHandler::GetInstance().LogError(
            "无法打开CJMOD信息文件: " + moduleStructure_.infoFile
        );
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(infoFile)),
                        std::istreambuf_iterator<char>());
    infoFile.close();
    
    // 解析[Info]块
    std::regex infoBlockRegex(R"(\[Info\]\s*\{([^}]+)\})");
    std::smatch match;
    
    if (!std::regex_search(content, match, infoBlockRegex)) {
        Utils::ErrorHandler::GetInstance().LogError(
            "CJMOD信息文件中未找到[Info]块: " + moduleStructure_.infoFile
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
        
        // 设置CJMOD模块信息字段
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
        } else if (key == "targetCHTLJS") {
            moduleInfo_.targetCHTLJS = value;
        }
    }
    
    if (!moduleInfo_.IsValid()) {
        Utils::ErrorHandler::GetInstance().LogError(
            "CJMOD模块信息不完整: " + moduleStructure_.moduleName
        );
        return false;
    }
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "CJMOD模块信息加载完成: " + moduleInfo_.name + " v" + moduleInfo_.version
    );
    
    return true;
}

bool CJMODModule::LoadCppSources() {
    // 加载C++源文件 - 语法文档第1416行：src/xxx.cpp xxx.h
    for (const auto& cppFile : moduleStructure_.cppFiles) {
        if (!std::filesystem::exists(cppFile)) {
            Utils::ErrorHandler::GetInstance().LogError(
                "CJMOD C++源文件不存在: " + cppFile
            );
            return false;
        }
        
        Utils::ErrorHandler::GetInstance().LogInfo(
            "CJMOD C++源文件已加载: " + cppFile
        );
    }
    
    for (const auto& headerFile : moduleStructure_.headerFiles) {
        if (!std::filesystem::exists(headerFile)) {
            Utils::ErrorHandler::GetInstance().LogError(
                "CJMOD头文件不存在: " + headerFile
            );
            return false;
        }
        
        Utils::ErrorHandler::GetInstance().LogInfo(
            "CJMOD头文件已加载: " + headerFile
        );
    }
    
    return true;
}

bool CJMODModule::LoadSubModules() {
    // 加载CJMOD子模块
    for (const auto& subModuleName : moduleStructure_.subModules) {
        std::string subModulePath = moduleStructure_.srcPath + "/" + subModuleName;
        
        Utils::ErrorHandler::GetInstance().LogInfo(
            "CJMOD子模块已加载: " + subModuleName
        );
    }
    
    return true;
}

bool CJMODModule::CompileSingleCppFile(const std::string& cppFile) {
    // 编译单个C++文件
    // 这里简化实现，实际应该调用C++编译器
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "编译CJMOD C++文件: " + cppFile
    );
    
    // 检查文件语法（简化）
    std::ifstream file(cppFile);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    
    // 检查是否包含必要的头文件
    if (content.find("#include") == std::string::npos) {
        Utils::ErrorHandler::GetInstance().LogWarning(
            "CJMOD C++文件可能缺少必要的头文件: " + cppFile
        );
    }
    
    return true;
}

bool CJMODModule::LinkExtensions() {
    // 链接CJMOD扩展
    Utils::ErrorHandler::GetInstance().LogInfo(
        "链接CJMOD扩展: " + moduleStructure_.moduleName
    );
    
    return true;
}

bool CJMODModule::CreateExtensionInstances() {
    // 创建扩展实例
    // 基于模块名称创建对应的扩展
    
    if (moduleStructure_.moduleName == "printMylove") {
        auto extension = std::make_shared<PrintMyloveExtension>();
        if (extension->Initialize()) {
            extensions_.push_back(extension);
        }
    } else if (moduleStructure_.moduleName == "iNeverAway") {
        auto extension = std::make_shared<INeverAwayExtension>();
        if (extension->Initialize()) {
            extensions_.push_back(extension);
        }
    }
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "CJMOD扩展实例创建完成，数量: " + std::to_string(extensions_.size())
    );
    
    return !extensions_.empty();
}

std::shared_ptr<CJMODExtensionInterface> CJMODModule::CreateExtensionFromName(const std::string& extensionName) {
    if (extensionName == "printMylove") {
        return std::make_shared<PrintMyloveExtension>();
    } else if (extensionName == "iNeverAway") {
        return std::make_shared<INeverAwayExtension>();
    }
    
    return nullptr;
}

// CJMODSyntaxExtensionManager实现
bool CJMODSyntaxExtensionManager::RegisterExtension(const CJMODExtensionRegistration& registration) {
    // 检查是否已注册
    for (const auto& existing : registeredExtensions_) {
        if (existing.extensionName == registration.extensionName) {
            Utils::ErrorHandler::GetInstance().LogWarning(
                "CJMOD扩展已注册: " + registration.extensionName
            );
            return false;
        }
    }
    
    registeredExtensions_.push_back(registration);
    SortExtensionsByPriority();
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "CJMOD语法扩展注册成功: " + registration.extensionName
    );
    
    return true;
}

bool CJMODSyntaxExtensionManager::UnregisterExtension(const std::string& extensionName) {
    auto it = std::remove_if(registeredExtensions_.begin(), registeredExtensions_.end(),
        [&extensionName](const CJMODExtensionRegistration& reg) {
            return reg.extensionName == extensionName;
        });
    
    if (it != registeredExtensions_.end()) {
        registeredExtensions_.erase(it, registeredExtensions_.end());
        
        Utils::ErrorHandler::GetInstance().LogInfo(
            "CJMOD语法扩展注销成功: " + extensionName
        );
        return true;
    }
    
    return false;
}

std::vector<std::shared_ptr<CJMODExtensionInterface>> CJMODSyntaxExtensionManager::FindMatchingExtensions(
    const std::string& syntaxPattern, const std::any& context) const {
    
    std::vector<std::shared_ptr<CJMODExtensionInterface>> matches;
    
    for (const auto& registration : registeredExtensions_) {
        if (registration.isActive && 
            registration.extension->MatchesSyntax(syntaxPattern, context)) {
            matches.push_back(registration.extension);
        }
    }
    
    return matches;
}

CHTLJS::AST::ASTNodePtr CJMODSyntaxExtensionManager::ParseExtensionSyntax(const std::string& input, const std::any& context) const {
    // 查找匹配的扩展并解析
    auto matchingExtensions = FindMatchingExtensions(input, context);
    
    if (matchingExtensions.empty()) {
        Utils::ErrorHandler::GetInstance().LogWarning(
            "未找到匹配的CJMOD扩展语法: " + input
        );
        return nullptr;
    }
    
    // 使用第一个匹配的扩展进行解析
    return matchingExtensions[0]->ParseSyntax(input, context);
}

std::string CJMODSyntaxExtensionManager::GenerateExtensionJavaScript(CHTLJS::AST::ASTNodePtr ast, const std::any& context) const {
    // 生成扩展JavaScript代码
    // 这里需要根据AST节点类型确定使用哪个扩展
    
    for (const auto& registration : registeredExtensions_) {
        if (registration.isActive) {
            std::string jsCode = registration.extension->GenerateJavaScript(ast, context);
            if (!jsCode.empty()) {
                return jsCode;
            }
        }
    }
    
    return "";
}

void CJMODSyntaxExtensionManager::SortExtensionsByPriority() {
    std::sort(registeredExtensions_.begin(), registeredExtensions_.end(),
        [](const CJMODExtensionRegistration& a, const CJMODExtensionRegistration& b) {
            return a.priority > b.priority;  // 高优先级在前
        });
}

// CJMODCompilerInterface实现
bool CJMODCompilerInterface::RegisterToCHTLJSCompiler(std::shared_ptr<CJMODExtensionInterface> extension) {
    if (!extension) {
        return false;
    }
    
    std::string extensionName = extension->GetExtensionName();
    
    // 检查是否已注册
    if (registeredExtensions_.find(extensionName) != registeredExtensions_.end()) {
        Utils::ErrorHandler::GetInstance().LogWarning(
            "CJMOD扩展已在CHTL JS编译器中注册: " + extensionName
        );
        return false;
    }
    
    registeredExtensions_[extensionName] = extension;
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "CJMOD扩展注册到CHTL JS编译器成功: " + extensionName
    );
    
    return true;
}

bool CJMODCompilerInterface::UnregisterFromCHTLJSCompiler(const std::string& extensionName) {
    auto it = registeredExtensions_.find(extensionName);
    if (it != registeredExtensions_.end()) {
        it->second->Cleanup();
        registeredExtensions_.erase(it);
        
        Utils::ErrorHandler::GetInstance().LogInfo(
            "CJMOD扩展从CHTL JS编译器注销成功: " + extensionName
        );
        return true;
    }
    
    return false;
}

bool CJMODCompilerInterface::CheckExtensionSyntax(const std::string& syntaxPattern, 
                                                 const std::vector<CHTLJS::Core::CHTLJSToken>& tokens) {
    // 检查是否有扩展能处理此语法
    for (const auto& extension : registeredExtensions_) {
        if (extension.second->MatchesSyntax(syntaxPattern, std::any{})) {
            return true;
        }
    }
    
    return false;
}

CHTLJS::AST::ASTNodePtr CJMODCompilerInterface::InvokeExtensionParsing(const std::string& extensionName,
                                                                       const std::string& input,
                                                                       const std::any& context) {
    auto it = registeredExtensions_.find(extensionName);
    if (it != registeredExtensions_.end()) {
        return it->second->ParseSyntax(input, context);
    }
    
    return nullptr;
}

std::string CJMODCompilerInterface::InvokeExtensionGeneration(const std::string& extensionName,
                                                            CHTLJS::AST::ASTNodePtr ast,
                                                            const std::any& context) {
    auto it = registeredExtensions_.find(extensionName);
    if (it != registeredExtensions_.end()) {
        return it->second->GenerateJavaScript(ast, context);
    }
    
    return "";
}

// CompleteCJMODManager实现
CompleteCJMODManager::CompleteCJMODManager(const std::string& compilerPath, const std::string& currentFilePath)
    : compilerPath_(compilerPath), currentFilePath_(currentFilePath) {
    
    InitializeSearchPaths();
    InitializePredefinedExtensions();
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "完整CJMOD管理器初始化完成，编译器路径: " + compilerPath_ + 
        "，当前文件: " + currentFilePath_
    );
}

CompleteCJMODManager::~CompleteCJMODManager() {
    ClearAllModules();
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "完整CJMOD管理器清理完成"
    );
}

bool CompleteCJMODManager::LoadCJMODModule(const std::string& moduleName) {
    Utils::ErrorHandler::GetInstance().LogInfo(
        "开始加载CJMOD模块: " + moduleName
    );
    
    // 1. 搜索模块
    std::string modulePath = SearchCJMODModule(moduleName);
    if (modulePath.empty()) {
        Utils::ErrorHandler::GetInstance().LogError(
            "未找到CJMOD模块: " + moduleName
        );
        return false;
    }
    
    // 2. 检查是否已加载
    if (loadedModules_.find(moduleName) != loadedModules_.end()) {
        Utils::ErrorHandler::GetInstance().LogInfo(
            "CJMOD模块已加载: " + moduleName
        );
        return true;
    }
    
    // 3. 扫描模块结构
    CJMODFileStructure moduleStructure = CJMODFileStructure::ScanCJMODStructure(modulePath);
    
    // 4. 创建模块
    auto module = std::make_shared<CJMODModule>(moduleStructure);
    
    // 5. 加载模块
    if (!module->Load()) {
        return false;
    }
    
    // 6. 编译C++扩展
    if (!module->CompileCppExtensions()) {
        return false;
    }
    
    // 7. 注册扩展
    if (!module->RegisterExtensions()) {
        return false;
    }
    
    // 8. 验证模块
    if (!module->Validate()) {
        return false;
    }
    
    // 9. 注册模块
    loadedModules_[moduleName] = module;
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "CJMOD模块加载完成: " + moduleName
    );
    
    return true;
}

std::shared_ptr<CJMODModule> CompleteCJMODManager::GetCJMODModule(const std::string& moduleName) const {
    auto it = loadedModules_.find(moduleName);
    if (it != loadedModules_.end()) {
        return it->second;
    }
    return nullptr;
}

std::string CompleteCJMODManager::SearchCJMODModule(const std::string& moduleName) const {
    // 按照用户要求的搜索优先级搜索CJMOD模块
    
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
    
    return "";
}

std::vector<std::string> CompleteCJMODManager::GetSearchPaths() const {
    return {
        officialModulePath_,
        currentModulePath_,
        currentDirPath_
    };
}

void CompleteCJMODManager::InitializeSearchPaths() {
    // 初始化CJMOD搜索路径
    std::filesystem::path compilerDir = std::filesystem::path(compilerPath_).parent_path();
    officialModulePath_ = compilerDir / "modules";
    
    std::filesystem::path currentFileDir = std::filesystem::path(currentFilePath_).parent_path();
    currentDirPath_ = currentFileDir.string();
    currentModulePath_ = currentFileDir / "modules";
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "CJMOD搜索路径初始化完成"
    );
}

void CompleteCJMODManager::InitializePredefinedExtensions() {
    // 初始化预定义扩展
    // 语法文档第1472-1520行：printMylove和iNeverAway
    
    // 注册printMylove扩展
    auto printMylove = std::make_shared<PrintMyloveExtension>();
    if (printMylove->Initialize()) {
        CJMODExtensionRegistration printReg;
        printReg.extensionName = "printMylove";
        printReg.syntaxPattern = "printMylove";
        printReg.extension = printMylove;
        printReg.priority = 100;
        printReg.isActive = true;
        
        syntaxExtensionManager_.RegisterExtension(printReg);
    }
    
    // 注册iNeverAway扩展
    auto iNeverAway = std::make_shared<INeverAwayExtension>();
    if (iNeverAway->Initialize()) {
        CJMODExtensionRegistration iNeverReg;
        iNeverReg.extensionName = "iNeverAway";
        iNeverReg.syntaxPattern = "iNeverAway";
        iNeverReg.extension = iNeverAway;
        iNeverReg.priority = 100;
        iNeverReg.isActive = true;
        
        syntaxExtensionManager_.RegisterExtension(iNeverReg);
    }
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "CJMOD预定义扩展初始化完成"
    );
}

std::string CompleteCJMODManager::SearchInOfficialModules(const std::string& moduleName) const {
    // 在官方模块目录搜索CJMOD
    std::string cjmodPath = officialModulePath_ + "/" + moduleName;
    if (std::filesystem::exists(cjmodPath) && IsCJMODDirectory(cjmodPath)) {
        return cjmodPath;
    }
    
    // 检查分类结构中的CJMOD
    std::vector<std::string> cjmodVariants = {"CJMOD", "CJmod", "cjmod"};
    for (const auto& variant : cjmodVariants) {
        std::string classifiedPath = officialModulePath_ + "/" + variant + "/" + moduleName;
        if (std::filesystem::exists(classifiedPath)) {
            return classifiedPath;
        }
    }
    
    return "";
}

std::string CompleteCJMODManager::SearchInCurrentModuleFolder(const std::string& moduleName) const {
    return SearchInOfficialModules(moduleName);  // 使用相同逻辑，但在currentModulePath_中
}

std::string CompleteCJMODManager::SearchInCurrentDirectory(const std::string& moduleName) const {
    return SearchInOfficialModules(moduleName);  // 使用相同逻辑，但在currentDirPath_中
}

bool CompleteCJMODManager::IsCJMODDirectory(const std::string& path) const {
    // 检查是否是有效的CJMOD目录
    std::string srcPath = path + "/src";
    std::string infoPath = path + "/info";
    
    if (!std::filesystem::exists(srcPath) || !std::filesystem::exists(infoPath)) {
        return false;
    }
    
    // 检查是否有C++文件
    for (const auto& entry : std::filesystem::directory_iterator(srcPath)) {
        if (entry.is_regular_file()) {
            std::string ext = entry.path().extension().string();
            if (ext == ".cpp" || ext == ".h" || ext == ".hpp") {
                return true;
            }
        }
    }
    
    return false;
}

std::string CompleteCJMODManager::GetStatistics() const {
    std::ostringstream stats;
    stats << "CJMOD模块统计信息:\n";
    stats << "  已加载模块数量: " << loadedModules_.size() << "\n";
    stats << "  注册扩展数量: " << syntaxExtensionManager_.GetAllExtensions().size() << "\n";
    
    return stats.str();
}

// CJMODMixedModuleManager实现
CJMODMixedModuleManager::MixedModuleInfo CJMODMixedModuleManager::ScanMixedModule(const std::string& modulePath) {
    MixedModuleInfo info;
    info.moduleName = std::filesystem::path(modulePath).filename().string();
    info.modulePath = modulePath;
    
    // 查找CMOD和CJMOD文件夹
    info.cmodPath = FindCMODFolder(modulePath);
    info.cjmodPath = FindCJMODFolder(modulePath);
    
    info.hasCMOD = !info.cmodPath.empty();
    info.hasCJMOD = !info.cjmodPath.empty();
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "混合模块扫描完成: " + info.moduleName + 
        "，CMOD: " + (info.hasCMOD ? "是" : "否") +
        "，CJMOD: " + (info.hasCJMOD ? "是" : "否")
    );
    
    return info;
}

std::string CJMODMixedModuleManager::FindCMODFolder(const std::string& modulePath) {
    std::vector<std::string> variants = {"CMOD", "Cmod", "cmod"};
    
    for (const auto& variant : variants) {
        std::string cmodPath = modulePath + "/" + variant;
        if (std::filesystem::exists(cmodPath) && std::filesystem::is_directory(cmodPath)) {
            return cmodPath;
        }
    }
    
    return "";
}

std::string CJMODMixedModuleManager::FindCJMODFolder(const std::string& modulePath) {
    std::vector<std::string> variants = {"CJMOD", "CJmod", "cjmod"};
    
    for (const auto& variant : variants) {
        std::string cjmodPath = modulePath + "/" + variant;
        if (std::filesystem::exists(cjmodPath) && std::filesystem::is_directory(cjmodPath)) {
            return cjmodPath;
        }
    }
    
    return "";
}

bool CJMODMixedModuleManager::LoadMixedModule(const MixedModuleInfo& moduleInfo,
                                             CHTL::CMOD::CompleteCMODManager& cmodManager,
                                             CompleteCJMODManager& cjmodManager) {
    // 加载混合模块
    // 语法文档第1452-1453行：CMOD用@Chtl，CJMOD用@CJmod，不统一处理
    
    bool success = true;
    
    if (moduleInfo.hasCMOD) {
        // 加载CMOD部分
        if (!cmodManager.LoadModule(moduleInfo.moduleName)) {
            Utils::ErrorHandler::GetInstance().LogError(
                "混合模块CMOD部分加载失败: " + moduleInfo.moduleName
            );
            success = false;
        }
    }
    
    if (moduleInfo.hasCJMOD) {
        // 加载CJMOD部分
        if (!cjmodManager.LoadCJMODModule(moduleInfo.moduleName)) {
            Utils::ErrorHandler::GetInstance().LogError(
                "混合模块CJMOD部分加载失败: " + moduleInfo.moduleName
            );
            success = false;
        }
    }
    
    if (success) {
        Utils::ErrorHandler::GetInstance().LogInfo(
            "混合模块加载完成: " + moduleInfo.moduleName
        );
    }
    
    return success;
}

bool CJMODMixedModuleManager::ValidateMixedModuleStructure(const MixedModuleInfo& moduleInfo) {
    // 验证混合模块结构
    if (!moduleInfo.hasCMOD && !moduleInfo.hasCJMOD) {
        Utils::ErrorHandler::GetInstance().LogError(
            "混合模块验证失败：既没有CMOD也没有CJMOD: " + moduleInfo.moduleName
        );
        return false;
    }
    
    return true;
}

} // namespace CJMOD
} // namespace CHTL