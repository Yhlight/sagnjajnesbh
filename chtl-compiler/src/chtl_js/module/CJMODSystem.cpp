#include "chtl_js/module/CJMODSystem.h"
#include "utils/Logger.h"
#include "utils/FileUtils.h"
#include "utils/StringUtils.h"
#include <fstream>
#include <sstream>
#include <regex>
#include <dlfcn.h> // Linux动态库加载
#ifdef _WIN32
#include <windows.h>
#endif

namespace chtl {
namespace chtljs {
namespace module {

namespace fs = std::filesystem;

// CJMODInfo 实现

bool CJMODInfo::ParseFromFile(const std::string& filePath) {
    std::string content;
    if (!utils::FileUtils::ReadFile(filePath, content)) {
        return false;
    }
    
    // 查找[Info]块
    std::regex infoRegex(R"(\[Info\]\s*\{([^}]*)\})", std::regex::multiline);
    std::smatch match;
    
    if (!std::regex_search(content, match, infoRegex)) {
        utils::Logger::GetInstance().Error("找不到[Info]块: " + filePath);
        return false;
    }
    
    std::string infoContent = match[1].str();
    
    // 解析各个字段
    auto parseField = [&infoContent](const std::string& fieldName) -> std::string {
        std::regex fieldRegex(fieldName + R"(\s*=\s*"([^"]*)")");
        std::smatch fieldMatch;
        if (std::regex_search(infoContent, fieldMatch, fieldRegex)) {
            return fieldMatch[1].str();
        }
        return "";
    };
    
    name = parseField("name");
    version = parseField("version");
    description = parseField("description");
    author = parseField("author");
    license = parseField("license");
    minCHTLVersion = parseField("minCHTLVersion");
    maxCHTLVersion = parseField("maxCHTLVersion");
    
    // C++特定字段
    entryPoint = parseField("entryPoint");
    headerFiles = parseField("headerFiles");
    linkLibraries = parseField("linkLibraries");
    compilerFlags = parseField("compilerFlags");
    
    // 如果没有指定入口点，使用默认值
    if (entryPoint.empty()) {
        entryPoint = "CreateCJMODExtension";
    }
    
    return !name.empty();
}

std::string CJMODInfo::Serialize() const {
    std::stringstream ss;
    ss << "[Info]\n{\n";
    ss << "    name = \"" << name << "\";\n";
    ss << "    version = \"" << version << "\";\n";
    ss << "    description = \"" << description << "\";\n";
    ss << "    author = \"" << author << "\";\n";
    ss << "    license = \"" << license << "\";\n";
    ss << "    minCHTLVersion = \"" << minCHTLVersion << "\";\n";
    ss << "    maxCHTLVersion = \"" << maxCHTLVersion << "\";\n";
    ss << "    entryPoint = \"" << entryPoint << "\";\n";
    ss << "    headerFiles = \"" << headerFiles << "\";\n";
    ss << "    linkLibraries = \"" << linkLibraries << "\";\n";
    ss << "    compilerFlags = \"" << compilerFlags << "\";\n";
    ss << "}\n";
    return ss.str();
}

// CJMODModule 实现

CJMODModule::CJMODModule(const std::string& name)
    : m_Name(name) {
    m_Info.name = name;
}

CJMODModule::~CJMODModule() {
    // 卸载动态库
    if (m_LibraryHandle) {
#ifdef _WIN32
        FreeLibrary((HMODULE)m_LibraryHandle);
#else
        dlclose(m_LibraryHandle);
#endif
    }
}

bool CJMODModule::Load(const fs::path& modulePath) {
    if (!fs::exists(modulePath)) {
        m_ValidationErrors.push_back("模块路径不存在: " + modulePath.string());
        return false;
    }
    
    // 检查CJMOD标准结构
    fs::path srcPath = modulePath / "src";
    fs::path infoPath = modulePath / "info";
    
    if (!fs::exists(srcPath) || !fs::is_directory(srcPath)) {
        m_ValidationErrors.push_back("缺少src目录");
        return false;
    }
    
    if (!fs::exists(infoPath) || !fs::is_directory(infoPath)) {
        m_ValidationErrors.push_back("缺少info目录");
        return false;
    }
    
    // 加载info文件
    fs::path infoFile = infoPath / (m_Name + ".chtl");
    if (!fs::exists(infoFile)) {
        m_ValidationErrors.push_back("找不到info文件: " + infoFile.string());
        return false;
    }
    
    if (!m_Info.ParseFromFile(infoFile.string())) {
        m_ValidationErrors.push_back("解析info文件失败");
        return false;
    }
    
    // 递归加载目录
    return LoadDirectory(modulePath);
}

bool CJMODModule::Save(const fs::path& outputPath) const {
    if (!ValidateStructure()) {
        return false;
    }
    
    // 创建目录结构
    fs::create_directories(outputPath / "src");
    fs::create_directories(outputPath / "info");
    
    // 保存info文件
    fs::path infoFile = outputPath / "info" / (m_Name + ".chtl");
    if (!utils::FileUtils::WriteFile(infoFile.string(), m_Info.Serialize())) {
        return false;
    }
    
    // 递归保存目录
    return SaveDirectory(outputPath);
}

std::vector<std::string> CJMODModule::GetSourceFiles() const {
    std::vector<std::string> files;
    for (const auto& pair : m_SourceFiles) {
        files.push_back(pair.first);
    }
    return files;
}

void CJMODModule::AddSourceFile(const std::string& relativePath, const std::string& content) {
    m_SourceFiles[relativePath] = content;
}

std::string CJMODModule::GetSourceFileContent(const std::string& relativePath) const {
    auto it = m_SourceFiles.find(relativePath);
    if (it != m_SourceFiles.end()) {
        return it->second;
    }
    return "";
}

bool CJMODModule::Compile(const fs::path& outputPath) const {
    if (!ValidateStructure()) {
        return false;
    }
    
    // 创建构建目录
    fs::path buildPath = outputPath / "build";
    fs::create_directories(buildPath);
    
    // 复制源文件
    for (const auto& pair : m_SourceFiles) {
        fs::path srcFile = buildPath / pair.first;
        fs::create_directories(srcFile.parent_path());
        
        if (!utils::FileUtils::WriteFile(srcFile.string(), pair.second)) {
            return false;
        }
    }
    
    // 生成Makefile
    std::string makefile = GenerateMakefile();
    if (!utils::FileUtils::WriteFile((buildPath / "Makefile").string(), makefile)) {
        return false;
    }
    
    // 编译
    std::string compileCmd = "cd " + buildPath.string() + " && make";
    int result = std::system(compileCmd.c_str());
    
    if (result != 0) {
        utils::Logger::GetInstance().Error("编译失败: " + m_Name);
        return false;
    }
    
    // 复制生成的库文件
    fs::path libFile = buildPath / ("lib" + m_Name + ".so");
    if (fs::exists(libFile)) {
        fs::copy(libFile, outputPath / libFile.filename());
        return true;
    }
    
    return false;
}

std::unique_ptr<ICJMODExtension> CJMODModule::LoadExtension(const std::string& libraryPath) {
    // 卸载之前的库
    if (m_LibraryHandle) {
#ifdef _WIN32
        FreeLibrary((HMODULE)m_LibraryHandle);
#else
        dlclose(m_LibraryHandle);
#endif
        m_LibraryHandle = nullptr;
    }
    
    // 加载动态库
#ifdef _WIN32
    m_LibraryHandle = LoadLibrary(libraryPath.c_str());
#else
    m_LibraryHandle = dlopen(libraryPath.c_str(), RTLD_LAZY);
#endif
    
    if (!m_LibraryHandle) {
#ifdef _WIN32
        utils::Logger::GetInstance().Error("无法加载库: " + libraryPath);
#else
        utils::Logger::GetInstance().Error("无法加载库: " + std::string(dlerror()));
#endif
        return nullptr;
    }
    
    // 获取工厂函数
    CJMODExtensionFactory factory = nullptr;
    
#ifdef _WIN32
    factory = (CJMODExtensionFactory)GetProcAddress((HMODULE)m_LibraryHandle, 
                                                   m_Info.entryPoint.c_str());
#else
    factory = (CJMODExtensionFactory)dlsym(m_LibraryHandle, m_Info.entryPoint.c_str());
#endif
    
    if (!factory) {
        utils::Logger::GetInstance().Error("找不到入口函数: " + m_Info.entryPoint);
        return nullptr;
    }
    
    // 创建扩展实例
    ICJMODExtension* extension = factory();
    if (!extension) {
        utils::Logger::GetInstance().Error("创建扩展失败");
        return nullptr;
    }
    
    // 初始化扩展
    if (!extension->Initialize()) {
        utils::Logger::GetInstance().Error("初始化扩展失败");
        delete extension;
        return nullptr;
    }
    
    return std::unique_ptr<ICJMODExtension>(extension);
}

bool CJMODModule::ValidateStructure() const {
    m_ValidationErrors.clear();
    
    // 验证基本信息
    if (m_Info.name.empty()) {
        m_ValidationErrors.push_back("模块名称不能为空");
    }
    
    if (m_Info.version.empty()) {
        m_ValidationErrors.push_back("模块版本不能为空");
    }
    
    // 验证至少有一个源文件
    bool hasCppFile = false;
    for (const auto& pair : m_SourceFiles) {
        if (pair.first.ends_with(".cpp") || pair.first.ends_with(".cxx")) {
            hasCppFile = true;
            break;
        }
    }
    
    if (!hasCppFile) {
        m_ValidationErrors.push_back("缺少C++源文件");
    }
    
    // 递归验证子模块
    for (const auto& subModule : m_SubModules) {
        if (!subModule->ValidateStructure()) {
            for (const auto& error : subModule->GetValidationErrors()) {
                m_ValidationErrors.push_back(subModule->m_Name + ": " + error);
            }
        }
    }
    
    return m_ValidationErrors.empty();
}

bool CJMODModule::LoadDirectory(const fs::path& dirPath) {
    // 加载src目录中的文件
    fs::path srcPath = dirPath / "src";
    
    for (const auto& entry : fs::recursive_directory_iterator(srcPath)) {
        if (entry.is_regular_file()) {
            // 计算相对路径
            fs::path relativePath = fs::relative(entry.path(), dirPath);
            
            // 读取文件内容
            std::string content;
            if (utils::FileUtils::ReadFile(entry.path().string(), content)) {
                m_SourceFiles[relativePath.string()] = content;
            }
        } else if (entry.is_directory() && entry.path().parent_path() == srcPath) {
            // 检查是否是子模块
            std::string dirName = entry.path().filename().string();
            fs::path subModuleSrc = entry.path() / "src";
            fs::path subModuleInfo = entry.path() / "info";
            
            if (fs::exists(subModuleSrc) && fs::exists(subModuleInfo)) {
                // 这是一个子模块
                auto subModule = std::make_unique<CJMODModule>(dirName);
                if (subModule->Load(entry.path())) {
                    m_SubModules.push_back(std::move(subModule));
                }
            }
        }
    }
    
    return true;
}

bool CJMODModule::SaveDirectory(const fs::path& dirPath) const {
    // 保存所有源文件
    for (const auto& pair : m_SourceFiles) {
        fs::path filePath = dirPath / pair.first;
        fs::create_directories(filePath.parent_path());
        
        if (!utils::FileUtils::WriteFile(filePath.string(), pair.second)) {
            return false;
        }
    }
    
    // 递归保存子模块
    for (const auto& subModule : m_SubModules) {
        fs::path subModulePath = dirPath / "src" / subModule->m_Name;
        if (!subModule->Save(subModulePath)) {
            return false;
        }
    }
    
    return true;
}

std::string CJMODModule::GenerateMakefile() const {
    std::stringstream makefile;
    
    makefile << "# CJMOD Makefile for " << m_Name << "\n";
    makefile << "CXX = g++\n";
    makefile << "CXXFLAGS = -std=c++17 -fPIC -shared " << m_Info.compilerFlags << "\n";
    makefile << "TARGET = lib" << m_Name << ".so\n";
    makefile << "SOURCES = ";
    
    // 列出所有C++源文件
    for (const auto& pair : m_SourceFiles) {
        if (pair.first.ends_with(".cpp") || pair.first.ends_with(".cxx")) {
            makefile << pair.first << " ";
        }
    }
    makefile << "\n";
    
    makefile << "OBJECTS = $(SOURCES:.cpp=.o)\n";
    makefile << "LIBS = " << m_Info.linkLibraries << "\n\n";
    
    makefile << "all: $(TARGET)\n\n";
    makefile << "$(TARGET): $(OBJECTS)\n";
    makefile << "\t$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)\n\n";
    makefile << "%.o: %.cpp\n";
    makefile << "\t$(CXX) $(CXXFLAGS) -c $< -o $@\n\n";
    makefile << "clean:\n";
    makefile << "\trm -f $(OBJECTS) $(TARGET)\n";
    
    return makefile.str();
}

// CJMODManager 实现

CJMODManager::CJMODManager() {
    // 添加默认搜索路径
    AddSearchPath("./module");
    AddSearchPath("./");
}

CJMODManager::~CJMODManager() {
    Clear();
}

void CJMODManager::AddSearchPath(const fs::path& path) {
    if (fs::exists(path) && fs::is_directory(path)) {
        m_SearchPaths.push_back(path);
    }
}

void CJMODManager::ClearSearchPaths() {
    m_SearchPaths.clear();
}

bool CJMODManager::LoadModule(const std::string& moduleName) {
    // 如果已经加载，直接返回
    if (m_Extensions.find(moduleName) != m_Extensions.end()) {
        return true;
    }
    
    // 查找模块
    fs::path modulePath = FindModule(moduleName);
    if (modulePath.empty()) {
        // 尝试查找已编译的库
        for (const auto& searchPath : m_SearchPaths) {
            fs::path libPath = searchPath / ("lib" + moduleName + ".so");
            if (fs::exists(libPath)) {
                return LoadCompiledExtension(libPath.string(), moduleName);
            }
            
#ifdef _WIN32
            libPath = searchPath / (moduleName + ".dll");
            if (fs::exists(libPath)) {
                return LoadCompiledExtension(libPath.string(), moduleName);
            }
#endif
        }
        
        m_Errors.push_back("找不到模块: " + moduleName);
        return false;
    }
    
    // 从目录加载
    return LoadFromDirectory(modulePath);
}

ICJMODExtension* CJMODManager::GetExtension(const std::string& name) const {
    auto it = m_Extensions.find(name);
    if (it != m_Extensions.end()) {
        return it->second.get();
    }
    return nullptr;
}

std::vector<ICJMODExtension*> CJMODManager::GetAllExtensions() const {
    std::vector<ICJMODExtension*> result;
    for (const auto& pair : m_Extensions) {
        result.push_back(pair.second.get());
    }
    return result;
}

ICJMODExtension* CJMODManager::FindExtensionByKeyword(const std::string& keyword) const {
    auto it = m_KeywordMap.find(keyword);
    if (it != m_KeywordMap.end()) {
        return it->second;
    }
    return nullptr;
}

void CJMODManager::UnloadModule(const std::string& name) {
    // 先从关键字映射中移除
    auto extIt = m_Extensions.find(name);
    if (extIt != m_Extensions.end()) {
        ICJMODExtension* ext = extIt->second.get();
        if (ext) {
            for (const auto& keyword : ext->GetKeywords()) {
                m_KeywordMap.erase(keyword);
            }
        }
    }
    
    // 移除扩展和模块
    m_Extensions.erase(name);
    m_Modules.erase(name);
}

void CJMODManager::Clear() {
    m_KeywordMap.clear();
    m_Extensions.clear();
    m_Modules.clear();
}

fs::path CJMODManager::FindModule(const std::string& moduleName) const {
    for (const auto& searchPath : m_SearchPaths) {
        // 查找目录
        fs::path dirPath = searchPath / moduleName;
        if (fs::exists(dirPath) && fs::is_directory(dirPath)) {
            // 检查是否是有效的CJMOD目录
            if (fs::exists(dirPath / "src") && fs::exists(dirPath / "info")) {
                return dirPath;
            }
        }
        
        // 查找.cjmod文件
        fs::path cjmodPath = searchPath / (moduleName + ".cjmod");
        if (fs::exists(cjmodPath) && fs::is_regular_file(cjmodPath)) {
            return cjmodPath;
        }
    }
    
    return fs::path();
}

bool CJMODManager::LoadFromDirectory(const fs::path& dirPath) {
    std::string moduleName = dirPath.filename().string();
    
    auto module = std::make_unique<CJMODModule>(moduleName);
    if (!module->Load(dirPath)) {
        m_Errors.insert(m_Errors.end(), 
                       module->GetValidationErrors().begin(),
                       module->GetValidationErrors().end());
        return false;
    }
    
    // 编译模块
    fs::path buildPath = dirPath / "build";
    if (!module->Compile(buildPath)) {
        m_Errors.push_back("编译模块失败: " + moduleName);
        return false;
    }
    
    // 加载扩展
    fs::path libPath = buildPath / ("lib" + moduleName + ".so");
    auto extension = module->LoadExtension(libPath.string());
    if (!extension) {
        m_Errors.push_back("加载扩展失败: " + moduleName);
        return false;
    }
    
    // 注册关键字
    for (const auto& keyword : extension->GetKeywords()) {
        m_KeywordMap[keyword] = extension.get();
    }
    
    m_Modules[moduleName] = std::move(module);
    m_Extensions[moduleName] = std::move(extension);
    
    utils::Logger::GetInstance().Info("加载CJMOD模块: " + moduleName);
    return true;
}

bool CJMODManager::LoadCompiledExtension(const std::string& libraryPath, 
                                        const std::string& moduleName) {
    // 创建一个空模块
    auto module = std::make_unique<CJMODModule>(moduleName);
    
    // 加载扩展
    auto extension = module->LoadExtension(libraryPath);
    if (!extension) {
        m_Errors.push_back("加载编译好的扩展失败: " + libraryPath);
        return false;
    }
    
    // 注册关键字
    for (const auto& keyword : extension->GetKeywords()) {
        m_KeywordMap[keyword] = extension.get();
    }
    
    m_Modules[moduleName] = std::move(module);
    m_Extensions[moduleName] = std::move(extension);
    
    utils::Logger::GetInstance().Info("加载CJMOD库: " + moduleName);
    return true;
}

// CJMODExtensionBase 实现

CJMODExtensionBase::CJMODExtensionBase(const std::string& name,
                                     const std::string& version,
                                     const std::string& description)
    : m_Name(name)
    , m_Version(version)
    , m_Description(description) {}

std::vector<std::string> CJMODExtensionBase::ParseArguments(const std::string& code,
                                                           const std::string& separator) {
    std::vector<std::string> args;
    
    std::string current;
    int parenDepth = 0;
    int braceDepth = 0;
    bool inString = false;
    char stringChar = '\0';
    
    for (size_t i = 0; i < code.length(); ++i) {
        char c = code[i];
        
        // 处理字符串
        if (!inString && (c == '"' || c == '\'')) {
            inString = true;
            stringChar = c;
        } else if (inString && c == stringChar && (i == 0 || code[i-1] != '\\')) {
            inString = false;
        }
        
        // 处理括号深度
        if (!inString) {
            if (c == '(') parenDepth++;
            else if (c == ')') parenDepth--;
            else if (c == '{') braceDepth++;
            else if (c == '}') braceDepth--;
            
            // 在顶层遇到分隔符
            if (parenDepth == 0 && braceDepth == 0 && 
                code.substr(i, separator.length()) == separator) {
                args.push_back(utils::StringUtils::Trim(current));
                current.clear();
                i += separator.length() - 1;
                continue;
            }
        }
        
        current += c;
    }
    
    if (!current.empty()) {
        args.push_back(utils::StringUtils::Trim(current));
    }
    
    return args;
}

std::string CJMODExtensionBase::EscapeJavaScript(const std::string& text) {
    std::string escaped;
    for (char c : text) {
        switch (c) {
            case '"': escaped += "\\\""; break;
            case '\'': escaped += "\\'"; break;
            case '\\': escaped += "\\\\"; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            default: escaped += c; break;
        }
    }
    return escaped;
}

} // namespace module
} // namespace chtljs
} // namespace chtl