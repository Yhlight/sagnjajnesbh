#include "CmodManager.h"
#include <filesystem>
#include <fstream>
#include <regex>

namespace {
    // 辅助函数：获取文件基本名（不含扩展名）
    std::string getFileBaseName(const std::string& path) {
        std::filesystem::path p(path);
        return p.stem().string();
    }
}

namespace chtl {
namespace module {

CmodManager::CmodManager() = default;
CmodManager::~CmodManager() = default;

void CmodManager::setErrorHandler(std::shared_ptr<chtl::common::ErrorHandler> errorHandler) {
    errorHandler_ = errorHandler;
}

bool CmodManager::validateThreeSameNames(const std::string& rootPath, const CmodInfo& info, const std::string& mainChtlPath) {
    // CMOD要求：模块文件夹，主模块chtl文件，模块信息chtl文件必须同名
    
    std::string folderName = getFileBaseName(rootPath);
    std::string infoFileName = getFileBaseName(info.infoPath);
    
    if (folderName != info.name || folderName != infoFileName) {
        return false;
    }
    
    // 如果有主模块文件，也必须同名
    if (!mainChtlPath.empty()) {
        std::string mainFileName = getFileBaseName(mainChtlPath);
        if (folderName != mainFileName) {
            return false;
        }
    }
    
    return true;
}

bool CmodManager::packModule(const std::string& modulePath, const std::string& outputPath) {
    try {
        if (!std::filesystem::exists(modulePath)) {
            reportError("Module path does not exist: " + modulePath);
            return false;
        }
        
        // 验证模块结构
        CmodInfo info;
        std::string mainChtlPath;
        if (!validateModuleStructure(modulePath, info, mainChtlPath)) {
            return false;
        }
        
        // 创建ZIP文件
        SimpleZip zip;
        if (!zip.create(outputPath)) {
            reportError("Failed to create output file: " + outputPath);
            return false;
        }
        
        // 添加文件到ZIP
        if (!addDirectoryToZip(zip, modulePath, "")) {
            reportError("Failed to add files to module package");
            return false;
        }
        
        zip.close();
        return true;
        
    } catch (const std::exception& e) {
        reportError("Exception during module packing: " + std::string(e.what()));
        return false;
    }
}

bool CmodManager::unpackModule(const std::string& packagePath, const std::string& outputDir) {
    try {
        if (!std::filesystem::exists(packagePath)) {
            reportError("Package file does not exist: " + packagePath);
            return false;
        }
        
        SimpleZip zip;
        if (!zip.open(packagePath)) {
            reportError("Failed to open package file: " + packagePath);
            return false;
        }
        
        if (!zip.extractAll(outputDir)) {
            reportError("Failed to extract package to: " + outputDir);
            return false;
        }
        
        zip.close();
        return true;
        
    } catch (const std::exception& e) {
        reportError("Exception during module unpacking: " + std::string(e.what()));
        return false;
    }
}

bool CmodManager::validateModuleStructure(const std::string& rootPath, CmodInfo& info, std::string& mainChtlPath) {
    namespace fs = std::filesystem;
    
    // 检查基本结构
    fs::path root(rootPath);
    fs::path srcPath = root / "src";
    fs::path infoPath = root / "info";
    
    if (!fs::exists(srcPath) || !fs::is_directory(srcPath)) {
        reportError("Missing src directory in module: " + rootPath);
        return false;
    }
    
    if (!fs::exists(infoPath) || !fs::is_directory(infoPath)) {
        reportError("Missing info directory in module: " + rootPath);
        return false;
    }
    
    // 查找info文件
    std::string moduleName = root.filename().string();
    fs::path expectedInfoFile = infoPath / (moduleName + ".chtl");
    
    if (!fs::exists(expectedInfoFile)) {
        reportError("Missing info file: " + expectedInfoFile.string());
        return false;
    }
    
    // 解析info文件
    if (!parseInfoFile(expectedInfoFile.string(), info)) {
        return false;
    }
    
    // 查找主模块文件
    fs::path expectedMainFile = srcPath / (moduleName + ".chtl");
    if (fs::exists(expectedMainFile)) {
        mainChtlPath = expectedMainFile.string();
    }
    
    // 验证三者同名规则
    if (!validateThreeSameNames(rootPath, info, mainChtlPath)) {
        reportError("Three same names rule violation for module: " + rootPath);
        return false;
    }
    
    return true;
}

bool CmodManager::parseInfoFile(const std::string& filePath, CmodInfo& info) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        reportError("Cannot open info file: " + filePath);
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    file.close();
    
    info.infoPath = filePath;
    
    // 解析[Info]段
    if (!parseInfoSection(content, info)) {
        return false;
    }
    
    // 解析[Export]段（如果存在）
    parseExportSection(content, info);
    
    return true;
}

bool CmodManager::parseInfoSection(const std::string& content, CmodInfo& info) {
    // 查找[Info]段
    std::regex infoSectionRegex(R"(\[Info\]\s*\{([^}]*)\})");
    std::smatch match;
    
    if (!std::regex_search(content, match, infoSectionRegex)) {
        reportError("Missing [Info] section in info file");
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
        reportError("Missing required field 'name' in [Info] section");
        return false;
    }
    
    if (info.version.empty()) {
        reportError("Missing required field 'version' in [Info] section");
        return false;
    }
    
    return true;
}

void CmodManager::parseExportSection(const std::string& content, CmodInfo& info) {
    // 查找[Export]段
    std::regex exportSectionRegex(R"(\[Export\]\s*\{([^}]*)\})");
    std::smatch match;
    
    if (!std::regex_search(content, match, exportSectionRegex)) {
        return; // [Export]段是可选的
    }
    
    std::string exportContent = match[1].str();
    
    // 解析导出项
    std::regex exportItemRegex(R"(\[(\w+)\]\s*@(\w+)\s+([^;]+);)");
    std::sregex_iterator iter(exportContent.begin(), exportContent.end(), exportItemRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        ExportItem item;
        item.category = (*iter)[1].str();
        item.type = (*iter)[2].str();
        item.names = (*iter)[3].str();
        
        info.exports.push_back(item);
    }
}

bool CmodManager::addDirectoryToZip(SimpleZip& zip, const std::string& dirPath, const std::string& zipPrefix) {
    namespace fs = std::filesystem;
    
    try {
        for (const auto& entry : fs::recursive_directory_iterator(dirPath)) {
            if (entry.is_regular_file()) {
                fs::path relativePath = fs::relative(entry.path(), dirPath);
                std::string zipPath = zipPrefix.empty() ? relativePath.string() : zipPrefix + "/" + relativePath.string();
                
                if (!zip.addFile(entry.path().string(), zipPath)) {
                    return false;
                }
            }
        }
        return true;
    } catch (const std::exception& e) {
        reportError("Error adding directory to zip: " + std::string(e.what()));
        return false;
    }
}

void CmodManager::reportError(const std::string& message) {
    if (errorHandler_) {
        errorHandler_->reportError(chtl::common::ErrorLevel::ERROR, 
                                 chtl::common::ErrorType::MODULE_ERROR, 
                                 message);
    }
}

void CmodManager::reportWarning(const std::string& message) {
    if (errorHandler_) {
        errorHandler_->reportError(chtl::common::ErrorLevel::WARNING, 
                                 chtl::common::ErrorType::MODULE_ERROR, 
                                 message);
    }
}

} // namespace module
} // namespace chtl