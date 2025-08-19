#include "CmodManager.h"
#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>

namespace chtl {
namespace module {

CmodManager::CmodManager() = default;
CmodManager::~CmodManager() = default;

void CmodManager::setErrorHandler(std::shared_ptr<chtl::common::ErrorHandler> errorHandler) {
    errorHandler_ = errorHandler;
}

bool CmodManager::validateCmodStructure(const std::string& modulePath) {
    namespace fs = std::filesystem;
    
    // 检查基本结构
    fs::path root(modulePath);
    fs::path srcPath = root / "src";
    fs::path infoPath = root / "info";
    
    if (!fs::exists(srcPath) || !fs::is_directory(srcPath)) {
        reportError("Missing src directory in module: " + modulePath);
        return false;
    }
    
    if (!fs::exists(infoPath) || !fs::is_directory(infoPath)) {
        reportError("Missing info directory in module: " + modulePath);
        return false;
    }
    
    // 验证三同名规则
    std::string moduleName = root.filename().string();
    fs::path expectedInfoFile = infoPath / (moduleName + ".chtl");
    
    if (!fs::exists(expectedInfoFile)) {
        reportError("Missing info file: " + expectedInfoFile.string());
        return false;
    }
    
    // 检查主模块文件（可选）
    fs::path expectedMainFile = srcPath / (moduleName + ".chtl");
    if (fs::exists(expectedMainFile)) {
        // 主模块文件存在，验证名称匹配
        std::string mainFileName = expectedMainFile.stem().string();
        if (mainFileName != moduleName) {
            reportError("Main module file name must match module name: " + modulePath);
            return false;
        }
    }
    
    return true;
}

bool CmodManager::validateSubmoduleStructure(const std::string& submodulePath) {
    namespace fs = std::filesystem;
    
    fs::path submodule(submodulePath);
    fs::path srcPath = submodule / "src";
    fs::path infoPath = submodule / "info";
    
    if (!fs::exists(srcPath) || !fs::is_directory(srcPath)) {
        reportError("Missing src directory in submodule: " + submodulePath);
        return false;
    }
    
    if (!fs::exists(infoPath) || !fs::is_directory(infoPath)) {
        reportError("Missing info directory in submodule: " + submodulePath);
        return false;
    }
    
    // 子模块也需要遵循同名规则
    std::string submoduleName = submodule.filename().string();
    fs::path expectedInfoFile = infoPath / (submoduleName + ".chtl");
    
    if (!fs::exists(expectedInfoFile)) {
        reportError("Missing info file in submodule: " + expectedInfoFile.string());
        return false;
    }
    
    return true;
}

bool CmodManager::validateInfoFile(const std::string& infoFilePath) {
    std::ifstream file(infoFilePath);
    if (!file.is_open()) {
        reportError("Cannot open info file: " + infoFilePath);
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    file.close();
    
    // 检查是否包含[Info]部分
    std::regex infoSectionRegex(R"(\[Info\]\s*\{[^}]*\})");
    if (!std::regex_search(content, infoSectionRegex)) {
        reportError("Missing [Info] section in info file: " + infoFilePath);
        return false;
    }
    
    return true;
}

bool CmodManager::parseCmodModule(const std::string& modulePath, CmodModule& module) {
    // 验证结构
    if (!validateCmodStructure(modulePath)) {
        return false;
    }
    
    // 解析模块信息
    namespace fs = std::filesystem;
    fs::path root(modulePath);
    std::string moduleName = root.filename().string();
    fs::path infoFile = root / "info" / (moduleName + ".chtl");
    
    if (!parseInfoFile(infoFile.string(), module.info)) {
        return false;
    }
    
    // 解析导出信息
    if (!parseExportFile(infoFile.string(), module.exports)) {
        return false;
    }
    
    // 解析子模块
    fs::path srcPath = root / "src";
    if (!parseSubmodules(srcPath.string(), module.submodules)) {
        return false;
    }
    
    module.rootPath = modulePath;
    return true;
}

bool CmodManager::parseInfoFile(const std::string& infoFilePath, CmodInfo& info) {
    std::ifstream file(infoFilePath);
    if (!file.is_open()) {
        reportError("Cannot open info file: " + infoFilePath);
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    file.close();
    
    // 解析[Info]段
    std::regex infoSectionRegex(R"(\[Info\]\s*\{([^}]*)\})");
    std::smatch match;
    
    if (!std::regex_search(content, match, infoSectionRegex)) {
        reportError("Missing [Info] section in info file: " + infoFilePath);
        return false;
    }
    
    std::string infoContent = match[1].str();
    
    // 解析字段
    std::regex fieldRegex(R"((\w+)\s*=\s*\"([^\"]*)\"\s*;?)");
    std::sregex_iterator iter(infoContent.begin(), infoContent.end(), fieldRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string key = (*iter)[1].str();
        std::string value = (*iter)[2].str();
        
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
    
    // 验证必需字段
    if (info.name.empty()) {
        reportError("Missing required field 'name' in [Info] section: " + infoFilePath);
        return false;
    }
    
    if (info.version.empty()) {
        reportError("Missing required field 'version' in [Info] section: " + infoFilePath);
        return false;
    }
    
    return true;
}

bool CmodManager::parseExportFile(const std::string& infoFilePath, std::vector<CmodExport>& exports) {
    std::ifstream file(infoFilePath);
    if (!file.is_open()) {
        return false; // Export部分是可选的
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    file.close();
    
    // 查找[Export]段
    std::regex exportSectionRegex(R"(\[Export\]\s*\{([^}]*)\})");
    std::smatch match;
    
    if (!std::regex_search(content, match, exportSectionRegex)) {
        return true; // [Export]段是可选的
    }
    
    std::string exportContent = match[1].str();
    
    // 解析导出项
    std::regex exportItemRegex(R"(\[(\w+)\]\s*@(\w+)\s+([^;]+);)");
    std::sregex_iterator iter(exportContent.begin(), exportContent.end(), exportItemRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        CmodExport exportItem;
        exportItem.type = (*iter)[1].str();
        exportItem.subType = (*iter)[2].str();
        // 将names字符串解析为symbols向量
        std::string namesStr = (*iter)[3].str();
        // 简单分割，按逗号分隔
        std::stringstream ss(namesStr);
        std::string symbol;
        while (std::getline(ss, symbol, ',')) {
            // 去除前后空格
            symbol.erase(0, symbol.find_first_not_of(" \t"));
            symbol.erase(symbol.find_last_not_of(" \t") + 1);
            if (!symbol.empty()) {
                exportItem.symbols.push_back(symbol);
            }
        }
        
        exports.push_back(exportItem);
    }
    
    return true;
}

bool CmodManager::parseSubmodules(const std::string& srcPath, std::vector<SubmoduleInfo>& submodules) {
    namespace fs = std::filesystem;
    
    try {
        for (const auto& entry : fs::directory_iterator(srcPath)) {
            if (entry.is_directory()) {
                std::string submodulePath = entry.path().string();
                
                // 检查是否是子模块（包含src和info目录）
                if (validateSubmoduleStructure(submodulePath)) {
                    SubmoduleInfo submodule;
                    submodule.srcPath = submodulePath;
                    submodule.name = entry.path().filename().string();
                    
                    // 解析子模块信息
                    fs::path infoFile = entry.path() / "info" / (submodule.name + ".chtl");
                    parseInfoFile(infoFile.string(), submodule.info);
                    
                    submodules.push_back(submodule);
                }
            }
        }
        return true;
    } catch (const std::exception& e) {
        reportError("Error parsing submodules: " + std::string(e.what()));
        return false;
    }
}

bool CmodManager::generateExports(const std::string& srcPath, std::vector<CmodExport>& exports) {
    // TODO: 实现自动导出生成
    // 这里应该扫描src目录中的.chtl文件，分析其中定义的模板、自定义等元素
    // 然后自动生成[Export]部分
    
    reportWarning("Auto-generation of exports not yet implemented");
    return true;
}

bool CmodManager::packCmod(const std::string& modulePath, const std::string& outputPath) {
    try {
        if (!std::filesystem::exists(modulePath)) {
            reportError("Module path does not exist: " + modulePath);
            return false;
        }
        
        // 验证模块结构
        if (!validateCmodStructure(modulePath)) {
            return false;
        }
        
        // 使用SimpleZip打包
        SimpleZip zip;
        
        // 添加目录到ZIP
        if (!zip.addDirectory("", modulePath)) {
            reportError("Failed to add files to module package");
            return false;
        }
        
        // 保存ZIP文件
        if (!zip.saveToFile(outputPath)) {
            reportError("Failed to create output file: " + outputPath);
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        reportError("Exception during module packing: " + std::string(e.what()));
        return false;
    }
}

bool CmodManager::unpackCmod(const std::string& packagePath, const std::string& outputDir) {
    try {
        if (!std::filesystem::exists(packagePath)) {
            reportError("Package file does not exist: " + packagePath);
            return false;
        }
        
        SimpleZip zip;
        if (!zip.loadFromFile(packagePath)) {
            reportError("Failed to open package file: " + packagePath);
            return false;
        }
        
        if (!zip.extractAll(outputDir)) {
            reportError("Failed to extract package to: " + outputDir);
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        reportError("Exception during module unpacking: " + std::string(e.what()));
        return false;
    }
}



void CmodManager::reportError(const std::string& message, const std::string& context) {
    if (errorHandler_) {
        errorHandler_->reportError(chtl::common::ErrorLevel::ERROR, 
                                 chtl::common::ErrorType::MODULE_ERROR, 
                                 message, context);
    }
}

void CmodManager::reportWarning(const std::string& message, const std::string& context) {
    if (errorHandler_) {
        errorHandler_->reportError(chtl::common::ErrorLevel::WARNING, 
                                 chtl::common::ErrorType::MODULE_ERROR, 
                                 message, context);
    }
}

} // namespace module
} // namespace chtl