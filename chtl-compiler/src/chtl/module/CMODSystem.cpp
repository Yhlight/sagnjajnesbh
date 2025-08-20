#include "chtl/module/CMODSystem.h"
#include "utils/Logger.h"
#include "utils/StringUtils.h"
#include "utils/FileUtils.h"
#include <fstream>
#include <sstream>
#include <regex>
#include <algorithm>
#include <cstring>

namespace chtl {
namespace module {

namespace fs = std::filesystem;

// CMODInfo 实现

bool CMODInfo::ParseFromFile(const std::string& filePath) {
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
    dependencies = parseField("dependencies");
    category = parseField("category");
    minCHTLVersion = parseField("minCHTLVersion");
    maxCHTLVersion = parseField("maxCHTLVersion");
    
    return !name.empty();
}

std::string CMODInfo::Serialize() const {
    std::stringstream ss;
    ss << "[Info]\n{\n";
    ss << "    name = \"" << name << "\";\n";
    ss << "    version = \"" << version << "\";\n";
    ss << "    description = \"" << description << "\";\n";
    ss << "    author = \"" << author << "\";\n";
    ss << "    license = \"" << license << "\";\n";
    ss << "    dependencies = \"" << dependencies << "\";\n";
    ss << "    category = \"" << category << "\";\n";
    ss << "    minCHTLVersion = \"" << minCHTLVersion << "\";\n";
    ss << "    maxCHTLVersion = \"" << maxCHTLVersion << "\";\n";
    ss << "}\n";
    return ss.str();
}

// CMODExport 实现

bool CMODExport::ParseFromFile(const std::string& filePath) {
    std::string content;
    if (!utils::FileUtils::ReadFile(filePath, content)) {
        return false;
    }
    
    // 查找[Export]块
    std::regex exportRegex(R"(\[Export\]\s*\{([^}]*)\})", std::regex::multiline);
    std::smatch match;
    
    if (!std::regex_search(content, match, exportRegex)) {
        // [Export]块是可选的
        return true;
    }
    
    std::string exportContent = match[1].str();
    
    // 解析各种导出项
    auto parseExportList = [&exportContent](const std::string& pattern) -> std::vector<std::string> {
        std::vector<std::string> result;
        std::regex listRegex(pattern + R"(\s+([^;]+);)");
        std::smatch listMatch;
        
        if (std::regex_search(exportContent, listMatch, listRegex)) {
            std::string items = listMatch[1].str();
            // 分割逗号分隔的项
            std::regex itemRegex(R"(\s*(\w+)\s*(?:,|$))");
            auto itemsBegin = std::sregex_iterator(items.begin(), items.end(), itemRegex);
            auto itemsEnd = std::sregex_iterator();
            
            for (auto it = itemsBegin; it != itemsEnd; ++it) {
                result.push_back((*it)[1].str());
            }
        }
        
        return result;
    };
    
    customStyles = parseExportList(R"(\[Custom\]\s*@Style)");
    customElements = parseExportList(R"(\[Custom\]\s*@Element)");
    customVars = parseExportList(R"(\[Custom\]\s*@Var)");
    templateStyles = parseExportList(R"(\[Template\]\s*@Style)");
    templateElements = parseExportList(R"(\[Template\]\s*@Element)");
    templateVars = parseExportList(R"(\[Template\]\s*@Var)");
    originHtml = parseExportList(R"(\[Origin\]\s*@Html)");
    originStyle = parseExportList(R"(\[Origin\]\s*@Style)");
    originJavascript = parseExportList(R"(\[Origin\]\s*@Javascript)");
    configurations = parseExportList(R"(\[Configuration\]\s*@Config)");
    
    // TODO: 解析自定义类型的原始嵌入
    
    return true;
}

std::string CMODExport::Serialize() const {
    std::stringstream ss;
    ss << "\n[Export]\n{\n";
    
    auto writeList = [&ss](const std::string& prefix, const std::vector<std::string>& items) {
        if (!items.empty()) {
            ss << "    " << prefix << " ";
            for (size_t i = 0; i < items.size(); ++i) {
                ss << items[i];
                if (i < items.size() - 1) ss << ", ";
            }
            ss << ";\n";
        }
    };
    
    writeList("[Custom] @Style", customStyles);
    writeList("[Custom] @Element", customElements);
    writeList("[Custom] @Var", customVars);
    writeList("[Template] @Style", templateStyles);
    writeList("[Template] @Element", templateElements);
    writeList("[Template] @Var", templateVars);
    writeList("[Origin] @Html", originHtml);
    writeList("[Origin] @Style", originStyle);
    writeList("[Origin] @Javascript", originJavascript);
    writeList("[Configuration] @Config", configurations);
    
    ss << "}\n";
    return ss.str();
}

// CMODModule 实现

CMODModule::CMODModule(const std::string& name)
    : m_Name(name) {
    m_Info.name = name;
}

CMODModule::~CMODModule() = default;

bool CMODModule::Load(const fs::path& modulePath) {
    if (!fs::exists(modulePath)) {
        m_ValidationErrors.push_back("模块路径不存在: " + modulePath.string());
        return false;
    }
    
    // 检查是否符合CMOD标准结构
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
    if (!LoadInfoFile(infoFile)) {
        return false;
    }
    
    // 递归加载目录
    return LoadDirectory(modulePath);
}

bool CMODModule::Save(const fs::path& outputPath) const {
    if (!ValidateStructure()) {
        return false;
    }
    
    // 创建目录结构
    fs::create_directories(outputPath / "src");
    fs::create_directories(outputPath / "info");
    
    // 保存info文件
    fs::path infoFile = outputPath / "info" / (m_Name + ".chtl");
    if (!SaveInfoFile(infoFile)) {
        return false;
    }
    
    // 递归保存目录
    return SaveDirectory(outputPath);
}

std::vector<std::string> CMODModule::GetSourceFiles() const {
    std::vector<std::string> files;
    for (const auto& pair : m_SourceFiles) {
        files.push_back(pair.first);
    }
    return files;
}

void CMODModule::AddSourceFile(const std::string& relativePath, const std::string& content) {
    m_SourceFiles[relativePath] = content;
}

std::string CMODModule::GetSourceFileContent(const std::string& relativePath) const {
    auto it = m_SourceFiles.find(relativePath);
    if (it != m_SourceFiles.end()) {
        return it->second;
    }
    return "";
}

void CMODModule::AddSubModule(std::unique_ptr<CMODModule> subModule) {
    if (subModule) {
        m_SubModules.push_back(std::move(subModule));
    }
}

std::vector<CMODModule*> CMODModule::GetSubModules() const {
    std::vector<CMODModule*> result;
    for (const auto& subModule : m_SubModules) {
        result.push_back(subModule.get());
    }
    return result;
}

CMODModule* CMODModule::GetSubModule(const std::string& name) const {
    for (const auto& subModule : m_SubModules) {
        if (subModule->GetName() == name) {
            return subModule.get();
        }
    }
    return nullptr;
}

bool CMODModule::ValidateStructure() const {
    m_ValidationErrors.clear();
    
    // 验证基本信息
    if (m_Info.name.empty()) {
        m_ValidationErrors.push_back("模块名称不能为空");
    }
    
    if (m_Info.version.empty()) {
        m_ValidationErrors.push_back("模块版本不能为空");
    }
    
    // 验证文件结构
    bool hasMainModule = false;
    for (const auto& pair : m_SourceFiles) {
        if (pair.first == "src/" + m_Name + ".chtl") {
            hasMainModule = true;
            break;
        }
    }
    
    // 如果没有子模块，必须有主模块文件
    if (m_SubModules.empty() && !hasMainModule) {
        m_ValidationErrors.push_back("缺少主模块文件: src/" + m_Name + ".chtl");
    }
    
    // 递归验证子模块
    for (const auto& subModule : m_SubModules) {
        if (!subModule->ValidateStructure()) {
            for (const auto& error : subModule->GetValidationErrors()) {
                m_ValidationErrors.push_back(subModule->GetName() + ": " + error);
            }
        }
    }
    
    return m_ValidationErrors.empty();
}

bool CMODModule::LoadDirectory(const fs::path& dirPath) {
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
                auto subModule = std::make_unique<CMODModule>(dirName);
                if (subModule->Load(entry.path())) {
                    AddSubModule(std::move(subModule));
                }
            }
        }
    }
    
    return true;
}

bool CMODModule::SaveDirectory(const fs::path& dirPath) const {
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
        fs::path subModulePath = dirPath / "src" / subModule->GetName();
        if (!subModule->Save(subModulePath)) {
            return false;
        }
    }
    
    return true;
}

bool CMODModule::LoadInfoFile(const fs::path& infoPath) {
    if (!fs::exists(infoPath)) {
        m_ValidationErrors.push_back("找不到info文件: " + infoPath.string());
        return false;
    }
    
    // 解析[Info]块
    if (!m_Info.ParseFromFile(infoPath.string())) {
        m_ValidationErrors.push_back("解析[Info]块失败");
        return false;
    }
    
    // 解析[Export]块
    m_Export.ParseFromFile(infoPath.string());
    
    return true;
}

bool CMODModule::SaveInfoFile(const fs::path& infoPath) const {
    std::string content;
    content += m_Info.Serialize();
    content += m_Export.Serialize();
    
    return utils::FileUtils::WriteFile(infoPath.string(), content);
}

// CMODPacker 实现

CMODPacker::CMODPacker() = default;
CMODPacker::~CMODPacker() = default;

bool CMODPacker::Pack(const CMODModule& module, const std::string& outputFile, bool compress) {
    if (!module.ValidateStructure()) {
        m_Errors = module.GetValidationErrors();
        return false;
    }
    
    std::ofstream output(outputFile, std::ios::binary);
    if (!output.is_open()) {
        m_Errors.push_back("无法创建输出文件: " + outputFile);
        return false;
    }
    
    return WriteArchive(module, output, compress);
}

bool CMODPacker::PackDirectory(const fs::path& moduleDir, 
                              const std::string& outputFile, 
                              bool compress) {
    // 从目录名获取模块名
    std::string moduleName = moduleDir.filename().string();
    
    CMODModule module(moduleName);
    if (!module.Load(moduleDir)) {
        m_Errors = module.GetValidationErrors();
        return false;
    }
    
    return Pack(module, outputFile, compress);
}

bool CMODPacker::WriteArchive(const CMODModule& module, std::ofstream& output, bool compress) {
    // 写入文件头
    ArchiveHeader header;
    std::memcpy(header.magic, "CHTLCMOD", 8);
    header.version = 1;
    header.fileCount = 0;  // 稍后更新
    header.flags = compress ? 1 : 0;
    
    output.write(reinterpret_cast<const char*>(&header), sizeof(header));
    
    // 收集所有文件
    std::vector<std::pair<std::string, std::string>> files;
    
    // info文件
    std::string infoContent = module.GetInfo().Serialize() + module.GetExport().Serialize();
    files.push_back({"info/" + module.GetName() + ".chtl", infoContent});
    
    // 源文件
    for (const auto& srcFile : module.GetSourceFiles()) {
        files.push_back({srcFile, module.GetSourceFileContent(srcFile)});
    }
    
    // 写入文件数量
    header.fileCount = static_cast<uint32_t>(files.size());
    output.seekp(0);
    output.write(reinterpret_cast<const char*>(&header), sizeof(header));
    output.seekp(0, std::ios::end);
    
    // 写入每个文件
    for (const auto& file : files) {
        FileEntry entry;
        entry.nameLength = static_cast<uint32_t>(file.first.length());
        
        std::vector<uint8_t> data(file.second.begin(), file.second.end());
        entry.originalSize = static_cast<uint32_t>(data.size());
        
        if (compress) {
            data = CompressData(data);
        }
        
        entry.dataLength = static_cast<uint32_t>(data.size());
        entry.checksum = CalculateChecksum(data);
        
        // 写入文件项
        output.write(reinterpret_cast<const char*>(&entry), sizeof(entry));
        output.write(file.first.c_str(), entry.nameLength);
        output.write(reinterpret_cast<const char*>(data.data()), entry.dataLength);
    }
    
    return true;
}

std::vector<uint8_t> CMODPacker::CompressData(const std::vector<uint8_t>& data) {
    // 简单的RLE压缩实现（实际项目中应使用zlib等）
    std::vector<uint8_t> compressed;
    
    for (size_t i = 0; i < data.size(); ) {
        uint8_t value = data[i];
        size_t count = 1;
        
        while (i + count < data.size() && data[i + count] == value && count < 255) {
            count++;
        }
        
        if (count > 3) {
            compressed.push_back(0xFF);  // 标记
            compressed.push_back(static_cast<uint8_t>(count));
            compressed.push_back(value);
            i += count;
        } else {
            compressed.push_back(value);
            i++;
        }
    }
    
    return compressed;
}

uint32_t CMODPacker::CalculateChecksum(const std::vector<uint8_t>& data) {
    // 简单的CRC32实现
    uint32_t crc = 0xFFFFFFFF;
    
    for (uint8_t byte : data) {
        crc ^= byte;
        for (int i = 0; i < 8; i++) {
            crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
        }
    }
    
    return ~crc;
}

// CMODUnpacker 实现

CMODUnpacker::CMODUnpacker() = default;
CMODUnpacker::~CMODUnpacker() = default;

bool CMODUnpacker::Unpack(const std::string& cmodFile, const fs::path& outputDir) {
    auto module = UnpackToModule(cmodFile);
    if (!module) {
        return false;
    }
    
    return module->Save(outputDir);
}

std::unique_ptr<CMODModule> CMODUnpacker::UnpackToModule(const std::string& cmodFile) {
    std::ifstream input(cmodFile, std::ios::binary);
    if (!input.is_open()) {
        m_Errors.push_back("无法打开文件: " + cmodFile);
        return nullptr;
    }
    
    // 从文件名获取模块名
    fs::path filePath(cmodFile);
    std::string moduleName = filePath.stem().string();
    
    auto module = std::make_unique<CMODModule>(moduleName);
    
    if (!ReadArchive(input, *module)) {
        return nullptr;
    }
    
    return module;
}

bool CMODUnpacker::ReadArchive(std::ifstream& input, CMODModule& module) {
    // 读取文件头
    ArchiveHeader header;
    input.read(reinterpret_cast<char*>(&header), sizeof(header));
    
    if (std::memcmp(header.magic, "CHTLCMOD", 8) != 0) {
        m_Errors.push_back("无效的CMOD文件格式");
        return false;
    }
    
    if (header.version != 1) {
        m_Errors.push_back("不支持的CMOD版本: " + std::to_string(header.version));
        return false;
    }
    
    bool compressed = (header.flags & 1) != 0;
    
    // 读取文件项
    for (uint32_t i = 0; i < header.fileCount; ++i) {
        FileEntry entry;
        input.read(reinterpret_cast<char*>(&entry), sizeof(entry));
        
        // 读取文件名
        std::string fileName(entry.nameLength, '\0');
        input.read(&fileName[0], entry.nameLength);
        
        // 读取数据
        std::vector<uint8_t> data(entry.dataLength);
        input.read(reinterpret_cast<char*>(data.data()), entry.dataLength);
        
        // 验证校验和
        if (!VerifyChecksum(data, entry.checksum)) {
            m_Errors.push_back("文件校验失败: " + fileName);
            return false;
        }
        
        // 解压数据
        if (compressed) {
            data = DecompressData(data, entry.originalSize);
        }
        
        // 转换为字符串
        std::string content(data.begin(), data.end());
        
        // 特殊处理info文件
        if (fileName.find("info/") == 0 && fileName.ends_with(".chtl")) {
            // 临时保存到文件以便解析
            std::string tempFile = "temp_info.chtl";
            utils::FileUtils::WriteFile(tempFile, content);
            module.GetInfo().ParseFromFile(tempFile);
            module.GetExport().ParseFromFile(tempFile);
            fs::remove(tempFile);
        } else {
            module.AddSourceFile(fileName, content);
        }
    }
    
    return true;
}

std::vector<uint8_t> CMODUnpacker::DecompressData(const std::vector<uint8_t>& data, size_t originalSize) {
    // 简单的RLE解压实现
    std::vector<uint8_t> decompressed;
    decompressed.reserve(originalSize);
    
    for (size_t i = 0; i < data.size(); ) {
        if (i + 2 < data.size() && data[i] == 0xFF) {
            uint8_t count = data[i + 1];
            uint8_t value = data[i + 2];
            for (uint8_t j = 0; j < count; ++j) {
                decompressed.push_back(value);
            }
            i += 3;
        } else {
            decompressed.push_back(data[i]);
            i++;
        }
    }
    
    return decompressed;
}

bool CMODUnpacker::VerifyChecksum(const std::vector<uint8_t>& data, uint32_t expectedChecksum) {
    uint32_t actualChecksum = 0xFFFFFFFF;
    
    for (uint8_t byte : data) {
        actualChecksum ^= byte;
        for (int i = 0; i < 8; i++) {
            actualChecksum = (actualChecksum >> 1) ^ (0xEDB88320 & -(actualChecksum & 1));
        }
    }
    
    return ~actualChecksum == expectedChecksum;
}

// CMODManager 实现

CMODManager::CMODManager() {
    // 添加默认搜索路径
    AddSearchPath("./module");
    AddSearchPath("./");
}

CMODManager::~CMODManager() = default;

void CMODManager::AddSearchPath(const fs::path& path) {
    if (fs::exists(path) && fs::is_directory(path)) {
        m_SearchPaths.push_back(path);
    }
}

void CMODManager::ClearSearchPaths() {
    m_SearchPaths.clear();
}

bool CMODManager::LoadModule(const std::string& moduleName) {
    // 如果已经加载，直接返回
    if (m_Modules.find(moduleName) != m_Modules.end()) {
        return true;
    }
    
    // 查找模块
    fs::path modulePath = FindModule(moduleName);
    if (modulePath.empty()) {
        m_Errors.push_back("找不到模块: " + moduleName);
        return false;
    }
    
    // 根据类型加载
    if (fs::is_directory(modulePath)) {
        return LoadFromDirectory(modulePath);
    } else if (modulePath.extension() == ".cmod") {
        return LoadFromCMODFile(modulePath);
    }
    
    m_Errors.push_back("未知的模块类型: " + modulePath.string());
    return false;
}

CMODModule* CMODManager::GetModule(const std::string& name) const {
    auto it = m_Modules.find(name);
    if (it != m_Modules.end()) {
        return it->second.get();
    }
    return nullptr;
}

std::vector<CMODModule*> CMODManager::GetAllModules() const {
    std::vector<CMODModule*> result;
    for (const auto& pair : m_Modules) {
        result.push_back(pair.second.get());
    }
    return result;
}

fs::path CMODManager::FindModule(const std::string& moduleName) const {
    for (const auto& searchPath : m_SearchPaths) {
        // 查找目录
        fs::path dirPath = searchPath / moduleName;
        if (fs::exists(dirPath) && fs::is_directory(dirPath)) {
            // 检查是否是有效的CMOD目录
            if (fs::exists(dirPath / "src") && fs::exists(dirPath / "info")) {
                return dirPath;
            }
        }
        
        // 查找.cmod文件
        fs::path cmodPath = searchPath / (moduleName + ".cmod");
        if (fs::exists(cmodPath) && fs::is_regular_file(cmodPath)) {
            return cmodPath;
        }
    }
    
    return fs::path();
}

void CMODManager::UnloadModule(const std::string& name) {
    m_Modules.erase(name);
}

void CMODManager::Clear() {
    m_Modules.clear();
}

bool CMODManager::LoadFromDirectory(const fs::path& dirPath) {
    std::string moduleName = dirPath.filename().string();
    
    auto module = std::make_unique<CMODModule>(moduleName);
    if (!module->Load(dirPath)) {
        m_Errors.insert(m_Errors.end(), 
                       module->GetValidationErrors().begin(),
                       module->GetValidationErrors().end());
        return false;
    }
    
    m_Modules[moduleName] = std::move(module);
    utils::Logger::GetInstance().Info("加载CMOD模块: " + moduleName);
    return true;
}

bool CMODManager::LoadFromCMODFile(const fs::path& cmodFile) {
    CMODUnpacker unpacker;
    auto module = unpacker.UnpackToModule(cmodFile.string());
    
    if (!module) {
        m_Errors.insert(m_Errors.end(),
                       unpacker.GetErrors().begin(),
                       unpacker.GetErrors().end());
        return false;
    }
    
    std::string moduleName = module->GetName();
    m_Modules[moduleName] = std::move(module);
    utils::Logger::GetInstance().Info("加载CMOD包: " + moduleName);
    return true;
}

} // namespace module
} // namespace chtl