#include "CmodManager.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <regex>
#include <sstream>

namespace chtl {
namespace module {

// === Version实现 ===

CmodManager::Version::Version(const std::string& versionStr) {
    std::regex versionRegex(R"((\d+)\.(\d+)\.(\d+))");
    std::smatch matches;
    
    if (std::regex_match(versionStr, matches, versionRegex)) {
        major = std::stoi(matches[1].str());
        minor = std::stoi(matches[2].str());
        patch = std::stoi(matches[3].str());
    }
}

bool CmodManager::Version::operator<(const Version& other) const {
    if (major != other.major) return major < other.major;
    if (minor != other.minor) return minor < other.minor;
    return patch < other.patch;
}

bool CmodManager::Version::operator<=(const Version& other) const {
    return *this < other || *this == other;
}

bool CmodManager::Version::operator>(const Version& other) const {
    return !(*this <= other);
}

bool CmodManager::Version::operator>=(const Version& other) const {
    return !(*this < other);
}

bool CmodManager::Version::operator==(const Version& other) const {
    return major == other.major && minor == other.minor && patch == other.patch;
}

// === CmodModule实现 ===

bool CmodModule::isValidNaming() const {
    // 严格按照CHTL语法文档：三同名规则
    // 模块文件夹，主模块chtl文件，模块信息chtl文件必须同名
    
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

// === CmodManager实现 ===

CmodManager::CmodManager() = default;

CmodManager::~CmodManager() = default;

void CmodManager::setErrorHandler(std::shared_ptr<common::ErrorHandler> errorHandler) {
    errorHandler_ = errorHandler;
}

// === CMOD验证 ===

bool CmodManager::validateCmodStructure(const std::string& modulePath) {
    if (!directoryExists(modulePath)) {
        reportError("模块目录不存在: " + modulePath);
        return false;
    }
    
    std::string moduleName = getFileBaseName(modulePath);
    
    // 检查必需的目录结构
    std::string srcPath = joinPath(modulePath, "src");
    std::string infoPath = joinPath(modulePath, "info");
    
    if (!directoryExists(srcPath)) {
        reportError("缺少src目录: " + srcPath);
        return false;
    }
    
    if (!directoryExists(infoPath)) {
        reportError("缺少info目录: " + infoPath);
        return false;
    }
    
    // 检查info文件（必须存在且同名）
    std::string infoFilePath = joinPath(infoPath, moduleName + ".chtl");
    if (!fileExists(infoFilePath)) {
        reportError("缺少模块信息文件: " + infoFilePath);
        return false;
    }
    
    // 验证info文件格式
    if (!validateInfoFile(infoFilePath)) {
        return false;
    }
    
    // 检查主模块文件（如果没有子模块则必须存在）
    std::string mainChtlPath = joinPath(srcPath, moduleName + ".chtl");
    bool hasMainChtl = fileExists(mainChtlPath);
    
    // 检查是否有子模块
    bool hasSubmodules = false;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(srcPath)) {
            if (entry.is_directory()) {
                hasSubmodules = true;
                if (!validateSubmoduleStructure(entry.path().string())) {
                    return false;
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        reportError("无法遍历src目录: " + std::string(e.what()));
        return false;
    }
    
    // 严格按照CHTL语法文档：在没有子模块的情况下，主模块chtl文件必须存在
    if (!hasSubmodules && !hasMainChtl) {
        reportError("没有子模块时必须存在主模块文件: " + mainChtlPath);
        return false;
    }
    
    return true;
}

bool CmodManager::validateSubmoduleStructure(const std::string& submodulePath) {
    std::string submoduleName = getFileBaseName(submodulePath);
    
    // 子模块必须有src和info目录
    std::string subSrcPath = joinPath(submodulePath, "src");
    std::string subInfoPath = joinPath(submodulePath, "info");
    
    if (!directoryExists(subSrcPath)) {
        reportError("子模块缺少src目录: " + subSrcPath);
        return false;
    }
    
    if (!directoryExists(subInfoPath)) {
        reportError("子模块缺少info目录: " + subInfoPath);
        return false;
    }
    
    // 子模块必须有同名的chtl文件
    std::string subChtlPath = joinPath(subSrcPath, submoduleName + ".chtl");
    std::string subInfoFilePath = joinPath(subInfoPath, submoduleName + ".chtl");
    
    if (!fileExists(subChtlPath)) {
        reportError("子模块缺少主文件: " + subChtlPath);
        return false;
    }
    
    if (!fileExists(subInfoFilePath)) {
        reportError("子模块缺少信息文件: " + subInfoFilePath);
        return false;
    }
    
    // 验证子模块info文件
    if (!validateInfoFile(subInfoFilePath)) {
        return false;
    }
    
    return true;
}

bool CmodManager::validateInfoFile(const std::string& infoFilePath) {
    std::string content;
    if (!readFileContent(infoFilePath, content)) {
        reportError("无法读取info文件: " + infoFilePath);
        return false;
    }
    
    // 严格按照CHTL语法文档：必须包含[Info]部分
    if (content.find("[Info]") == std::string::npos) {
        reportError("info文件缺少[Info]部分: " + infoFilePath);
        return false;
    }
    
    // 解析[Info]部分验证必需字段
    CmodInfo info;
    if (!parseInfoSection(content, info)) {
        reportError("无法解析[Info]部分: " + infoFilePath);
        return false;
    }
    
    // 验证必需字段
    if (!hasRequiredInfoFields(info)) {
        reportError("info文件缺少必需字段: " + infoFilePath);
        return false;
    }
    
    return true;
}

// === CMOD解析 ===

bool CmodManager::parseCmodModule(const std::string& modulePath, CmodModule& module) {
    if (!validateCmodStructure(modulePath)) {
        return false;
    }
    
    module.rootPath = modulePath;
    std::string moduleName = getFileBaseName(modulePath);
    
    // 解析模块信息
    std::string infoFilePath = joinPath(joinPath(modulePath, "info"), moduleName + ".chtl");
    if (!parseInfoFile(infoFilePath, module.info)) {
        return false;
    }
    
    module.info.modulePath = modulePath;
    module.info.infoPath = infoFilePath;
    module.info.srcPath = joinPath(modulePath, "src");
    
    // 检查主模块文件
    std::string mainChtlPath = joinPath(module.info.srcPath, moduleName + ".chtl");
    if (fileExists(mainChtlPath)) {
        module.mainChtlPath = mainChtlPath;
    }
    
    // 解析导出信息
    if (!parseExportFile(infoFilePath, module.exports)) {
        // [Export]部分可能不存在，尝试自动生成
        if (!generateExports(module.info.srcPath, module.exports)) {
            reportWarning("无法生成导出信息: " + modulePath);
        }
    }
    
    // 解析子模块
    if (!parseSubmodules(module.info.srcPath, module.submodules)) {
        return false;
    }
    
    // 验证三同名规则
    if (!module.isValidNaming()) {
        reportError("模块不符合三同名规则: " + modulePath);
        return false;
    }
    
    return true;
}

bool CmodManager::parseInfoFile(const std::string& infoFilePath, CmodInfo& info) {
    std::string content;
    if (!readFileContent(infoFilePath, content)) {
        return false;
    }
    
    return parseInfoSection(content, info);
}

bool CmodManager::parseExportFile(const std::string& infoFilePath, std::vector<CmodExport>& exports) {
    std::string content;
    if (!readFileContent(infoFilePath, content)) {
        return false;
    }
    
    return parseExportSection(content, exports);
}

bool CmodManager::parseSubmodules(const std::string& srcPath, std::vector<SubmoduleInfo>& submodules) {
    try {
        for (const auto& entry : std::filesystem::directory_iterator(srcPath)) {
            if (entry.is_directory()) {
                SubmoduleInfo submodule(entry.path().filename().string());
                submodule.srcPath = joinPath(entry.path().string(), "src");
                submodule.infoPath = joinPath(entry.path().string(), "info");
                
                // 解析子模块信息
                std::string subInfoFile = joinPath(submodule.infoPath, submodule.name + ".chtl");
                if (parseInfoFile(subInfoFile, submodule.info)) {
                    parseExportFile(subInfoFile, submodule.exports);
                    submodules.push_back(submodule);
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        reportError("解析子模块失败: " + std::string(e.what()));
        return false;
    }
    
    return true;
}

bool CmodManager::generateExports(const std::string& srcPath, std::vector<CmodExport>& exports) {
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(srcPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".chtl") {
                if (!scanChtlFile(entry.path().string(), exports)) {
                    reportWarning("无法扫描CHTL文件: " + entry.path().string());
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        reportError("生成导出失败: " + std::string(e.what()));
        return false;
    }
    
    return true;
}

// === CMOD打包 ===

bool CmodManager::packCmod(const std::string& modulePath, const std::string& outputPath) {
    if (!validateCmodStructure(modulePath)) {
        return false;
    }
    
    SimpleZip zip;
    
    // 添加整个模块目录到ZIP
    if (!addDirectoryToZip(zip, modulePath)) {
        reportError("无法添加模块目录到ZIP: " + modulePath);
        return false;
    }
    
    // 保存ZIP文件
    if (!zip.saveToFile(outputPath)) {
        reportError("无法保存CMOD文件: " + outputPath);
        return false;
    }
    
    return true;
}

bool CmodManager::packCmodWithSubmodules(const std::string& modulePath, const std::string& outputPath) {
    // 与普通打包相同，因为addDirectoryToZip会递归处理子目录
    return packCmod(modulePath, outputPath);
}

// === CMOD解包 ===

bool CmodManager::unpackCmod(const std::string& cmodFilePath, const std::string& outputDir) {
    if (!fileExists(cmodFilePath)) {
        reportError("CMOD文件不存在: " + cmodFilePath);
        return false;
    }
    
    if (!validateCmodFile(cmodFilePath)) {
        return false;
    }
    
    return extractZipToDirectory(cmodFilePath, outputDir);
}

bool CmodManager::validateCmodFile(const std::string& cmodFilePath) {
    SimpleZip zip;
    
    if (!zip.loadFromFile(cmodFilePath)) {
        reportError("无法加载CMOD文件: " + cmodFilePath);
        return false;
    }
    
    auto fileList = zip.getFileList();
    
    // 检查必需的文件结构
    bool hasSrc = false;
    bool hasInfo = false;
    
    for (const auto& file : fileList) {
        if (file.find("/src/") != std::string::npos) {
            hasSrc = true;
        }
        if (file.find("/info/") != std::string::npos && file.find(".chtl") != std::string::npos) {
            hasInfo = true;
        }
    }
    
    if (!hasSrc || !hasInfo) {
        reportError("CMOD文件结构不完整: " + cmodFilePath);
        return false;
    }
    
    return true;
}

// === CMOD安装和管理 ===

bool CmodManager::installCmod(const std::string& cmodFilePath, const std::string& targetDir) {
    if (!createDirectory(targetDir)) {
        reportError("无法创建目标目录: " + targetDir);
        return false;
    }
    
    return unpackCmod(cmodFilePath, targetDir);
}

bool CmodManager::uninstallCmod(const std::string& moduleName, const std::string& moduleDir) {
    std::string modulePath = getModuleFullPath(moduleDir, moduleName);
    
    if (!directoryExists(modulePath)) {
        reportError("模块不存在: " + modulePath);
        return false;
    }
    
    try {
        std::filesystem::remove_all(modulePath);
        return true;
    } catch (const std::filesystem::filesystem_error& e) {
        reportError("无法删除模块: " + std::string(e.what()));
        return false;
    }
}

std::vector<CmodInfo> CmodManager::listInstalledCmods(const std::string& moduleDir) {
    std::vector<CmodInfo> cmods;
    
    if (!directoryExists(moduleDir)) {
        return cmods;
    }
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(moduleDir)) {
            if (entry.is_directory()) {
                CmodModule module;
                if (parseCmodModule(entry.path().string(), module)) {
                    cmods.push_back(module.info);
                }
            }
        }
    } catch (const std::filesystem::filesystem_error&) {
        // 忽略错误，返回已找到的模块
    }
    
    return cmods;
}

std::string CmodManager::findCmodModule(const std::string& moduleName) {
    // 严格按照CHTL语法文档的Import规则搜索顺序
    std::vector<std::string> searchPaths = {
        "module",                    // 官方模块目录
        "./module",                  // 当前目录module文件夹
        "."                          // 当前目录
    };
    
    for (const auto& searchPath : searchPaths) {
        std::string modulePath = getModuleFullPath(searchPath, moduleName);
        
        // 优先匹配.cmod文件
        std::string cmodPath = modulePath + ".cmod";
        if (fileExists(cmodPath)) {
            return cmodPath;
        }
        
        // 然后匹配目录
        if (directoryExists(modulePath) && validateCmodStructure(modulePath)) {
            return modulePath;
        }
    }
    
    return "";
}

// === 版本兼容性 ===

bool CmodManager::checkVersionCompatibility(const CmodInfo& info, const std::string& currentVersion) {
    if (info.minCHTLVersion.empty() && info.maxCHTLVersion.empty()) {
        return true; // 没有版本限制
    }
    
    Version current(currentVersion);
    
    if (!info.minCHTLVersion.empty()) {
        Version minVersion(info.minCHTLVersion);
        if (current < minVersion) {
            return false;
        }
    }
    
    if (!info.maxCHTLVersion.empty()) {
        Version maxVersion(info.maxCHTLVersion);
        if (current > maxVersion) {
            return false;
        }
    }
    
    return true;
}

CmodManager::Version CmodManager::parseVersion(const std::string& versionStr) {
    return Version(versionStr);
}

// === 依赖管理 ===

std::vector<std::string> CmodManager::parseDependencies(const std::string& dependencies) {
    std::vector<std::string> deps;
    
    if (dependencies.empty()) {
        return deps;
    }
    
    std::stringstream ss(dependencies);
    std::string dep;
    
    while (std::getline(ss, dep, ',')) {
        // 去除前后空白
        dep.erase(0, dep.find_first_not_of(" \t"));
        dep.erase(dep.find_last_not_of(" \t") + 1);
        
        if (!dep.empty()) {
            deps.push_back(dep);
        }
    }
    
    return deps;
}

bool CmodManager::checkDependencies(const CmodInfo& info, const std::string& moduleDir) {
    auto deps = parseDependencies(info.dependencies);
    
    for (const auto& dep : deps) {
        std::string depPath = findCmodModule(dep);
        if (depPath.empty()) {
            reportError("缺少依赖模块: " + dep);
            return false;
        }
    }
    
    return true;
}

bool CmodManager::installDependencies(const CmodInfo& info, const std::string& moduleDir) {
    auto deps = parseDependencies(info.dependencies);
    
    for (const auto& dep : deps) {
        std::string depPath = findCmodModule(dep);
        if (depPath.empty()) {
            reportError("无法找到依赖模块进行安装: " + dep);
            return false;
        }
        
        // 如果依赖是.cmod文件，需要安装
        if (depPath.find(".cmod") != std::string::npos) {
            if (!installCmod(depPath, moduleDir)) {
                reportError("无法安装依赖模块: " + dep);
                return false;
            }
        }
    }
    
    return true;
}

// === 实用工具 ===

std::string CmodManager::getModuleFullPath(const std::string& moduleDir, const std::string& moduleName) {
    return joinPath(moduleDir, moduleName);
}

bool CmodManager::fileExists(const std::string& filePath) {
    return std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath);
}

bool CmodManager::directoryExists(const std::string& dirPath) {
    return std::filesystem::exists(dirPath) && std::filesystem::is_directory(dirPath);
}

bool CmodManager::createDirectory(const std::string& dirPath) {
    try {
        return std::filesystem::create_directories(dirPath);
    } catch (const std::filesystem::filesystem_error&) {
        return false;
    }
}

std::string CmodManager::getCurrentCHTLVersion() {
    return "1.0.0"; // 硬编码版本，实际应该从编译器获取
}

// === 私有辅助方法 ===

void CmodManager::reportError(const std::string& message, const std::string& context) {
    std::string fullMessage = message;
    if (!context.empty()) {
        fullMessage += " (" + context + ")";
    }
    
    errors_.push_back(fullMessage);
    
    if (errorHandler_) {
        errorHandler_->reportError(common::ErrorLevel::ERROR, common::ErrorType::MODULE, 
                                 fullMessage, context);
    }
}

void CmodManager::reportWarning(const std::string& message, const std::string& context) {
    if (errorHandler_) {
        errorHandler_->reportError(common::ErrorLevel::WARNING, common::ErrorType::MODULE, 
                                 message, context);
    }
}

bool CmodManager::readFileContent(const std::string& filePath, std::string& content) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    content = buffer.str();
    
    return true;
}

bool CmodManager::writeFileContent(const std::string& filePath, const std::string& content) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return true;
}

std::string CmodManager::normalizePath(const std::string& path) {
    std::string normalized = path;
    std::replace(normalized.begin(), normalized.end(), '\\', '/');
    return normalized;
}

std::string CmodManager::joinPath(const std::string& base, const std::string& relative) {
    std::string result = normalizePath(base);
    if (!result.empty() && result.back() != '/') {
        result += '/';
    }
    result += normalizePath(relative);
    return result;
}

std::string CmodManager::getFileName(const std::string& filePath) {
    std::filesystem::path path(filePath);
    return path.filename().string();
}

std::string CmodManager::getFileBaseName(const std::string& filePath) {
    std::filesystem::path path(filePath);
    return path.stem().string();
}

std::string CmodManager::getDirectoryName(const std::string& filePath) {
    std::filesystem::path path(filePath);
    return path.parent_path().string();
}

bool CmodManager::parseInfoSection(const std::string& content, CmodInfo& info) {
    // 严格按照CHTL语法文档解析[Info]部分
    std::regex infoRegex(R"(\[Info\]\s*\{([^}]*)\})");
    std::smatch matches;
    
    if (!std::regex_search(content, matches, infoRegex)) {
        return false;
    }
    
    std::string infoContent = matches[1].str();
    
    // 解析各个字段
    std::regex fieldRegex(R"((\w+)\s*=\s*"([^"]*)";?)");
    std::sregex_iterator iter(infoContent.begin(), infoContent.end(), fieldRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string key = (*iter)[1].str();
        std::string value = (*iter)[2].str();
        
        if (key == "name") info.name = value;
        else if (key == "version") info.version = value;
        else if (key == "description") info.description = value;
        else if (key == "author") info.author = value;
        else if (key == "license") info.license = value;
        else if (key == "dependencies") info.dependencies = value;
        else if (key == "category") info.category = value;
        else if (key == "minCHTLVersion") info.minCHTLVersion = value;
        else if (key == "maxCHTLVersion") info.maxCHTLVersion = value;
    }
    
    return true;
}

bool CmodManager::parseExportSection(const std::string& content, std::vector<CmodExport>& exports) {
    // 严格按照CHTL语法文档解析[Export]部分
    std::regex exportRegex(R"(\[Export\]\s*\{([^}]*)\})");
    std::smatch matches;
    
    if (!std::regex_search(content, matches, exportRegex)) {
        return false; // [Export]部分可能不存在
    }
    
    std::string exportContent = matches[1].str();
    
    // 解析导出行：[Custom] @Style ChthollyStyle, ChthollyCard, ChthollyButton;
    std::regex lineRegex(R"(\[(\w+)\]\s*@(\w+)\s+([^;]+);)");
    std::sregex_iterator iter(exportContent.begin(), exportContent.end(), lineRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string type = (*iter)[1].str();
        std::string subType = (*iter)[2].str();
        std::string symbolsStr = (*iter)[3].str();
        
        // 解析符号列表
        std::vector<std::string> symbols;
        std::stringstream ss(symbolsStr);
        std::string symbol;
        
        while (std::getline(ss, symbol, ',')) {
            // 去除前后空白
            symbol.erase(0, symbol.find_first_not_of(" \t\n"));
            symbol.erase(symbol.find_last_not_of(" \t\n") + 1);
            
            if (!symbol.empty()) {
                symbols.push_back(symbol);
            }
        }
        
        if (!symbols.empty()) {
            exports.emplace_back(type, subType, symbols);
        }
    }
    
    return true;
}

bool CmodManager::scanChtlFile(const std::string& filePath, std::vector<CmodExport>& exports) {
    std::string content;
    if (!readFileContent(filePath, content)) {
        return false;
    }
    
    return extractSymbolsFromChtl(content, exports);
}

bool CmodManager::extractSymbolsFromChtl(const std::string& content, std::vector<CmodExport>& exports) {
    // 简化的符号提取，实际应该使用完整的CHTL解析器
    
    // 提取[Custom] @Element
    std::regex customElementRegex(R"(\[Custom\]\s*@Element\s+(\w+))");
    std::sregex_iterator iter(content.begin(), content.end(), customElementRegex);
    std::sregex_iterator end;
    
    std::vector<std::string> customElements;
    for (; iter != end; ++iter) {
        customElements.push_back((*iter)[1].str());
    }
    
    if (!customElements.empty()) {
        exports.emplace_back("Custom", "Element", customElements);
    }
    
    // 类似地提取其他符号类型...
    // 这里简化处理，实际需要完整实现
    
    return true;
}

bool CmodManager::addDirectoryToZip(SimpleZip& zip, const std::string& dirPath, const std::string& zipPrefix) {
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(dirPath)) {
            if (entry.is_regular_file()) {
                std::string relativePath = std::filesystem::relative(entry.path(), dirPath).string();
                std::string zipPath = zipPrefix.empty() ? relativePath : zipPrefix + "/" + relativePath;
                
                if (!zip.addFileFromPath(zipPath, entry.path().string())) {
                    return false;
                }
            }
        }
    } catch (const std::filesystem::filesystem_error&) {
        return false;
    }
    
    return true;
}

bool CmodManager::extractZipToDirectory(const std::string& zipPath, const std::string& outputDir) {
    SimpleZip zip;
    
    if (!zip.loadFromFile(zipPath)) {
        return false;
    }
    
    return zip.extractAll(outputDir);
}

bool CmodManager::isValidModuleName(const std::string& name) {
    if (name.empty()) {
        return false;
    }
    
    // 模块名只能包含字母、数字、下划线
    std::regex nameRegex(R"(^[a-zA-Z_][a-zA-Z0-9_]*$)");
    return std::regex_match(name, nameRegex);
}

bool CmodManager::isValidVersion(const std::string& version) {
    std::regex versionRegex(R"(^\d+\.\d+\.\d+$)");
    return std::regex_match(version, versionRegex);
}

bool CmodManager::hasRequiredInfoFields(const CmodInfo& info) {
    // 严格按照CHTL语法文档检查必需字段
    return !info.name.empty() && 
           !info.version.empty() && 
           isValidModuleName(info.name) && 
           isValidVersion(info.version);
}

} // namespace module
} // namespace chtl