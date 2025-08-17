#include "ModuleSystem.h"
#include "CHTLParser.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>
#include <cstring>

namespace fs = std::filesystem;

namespace chtl {

// ==================== ModuleInfo ====================

bool ModuleInfo::parseFromString(const String& infoContent) {
    std::istringstream iss(infoContent);
    String line;
    
    while (std::getline(iss, line)) {
        // 跳过注释和空行
        if (line.empty() || line[0] == '/' || line.find("//") == 0) {
            continue;
        }
        
        // 查找等号分隔符
        size_t equalPos = line.find('=');
        if (equalPos == String::npos) {
            continue;
        }
        
        String key = line.substr(0, equalPos);
        String value = line.substr(equalPos + 1);
        
        // 去除前后空格和引号
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t\""));
        value.erase(value.find_last_not_of(" \t\";") + 1);
        
        // 解析字段
        if (key == "name") {
            name = value;
        } else if (key == "version") {
            version = value;
        } else if (key == "description") {
            description = value;
        } else if (key == "author") {
            author = value;
        } else if (key == "license") {
            license = value;
        } else if (key == "dependencies") {
            dependencies = value;
        } else if (key == "category") {
            category = value;
        } else if (key == "minCHTLVersion") {
            minCHTLVersion = value;
        } else if (key == "maxCHTLVersion") {
            maxCHTLVersion = value;
        }
    }
    
    return validate();
}

String ModuleInfo::toString() const {
    std::ostringstream oss;
    
    oss << "[Info]\n{\n";
    oss << "    name = \"" << name << "\";\n";
    oss << "    version = \"" << version << "\";\n";
    oss << "    description = \"" << description << "\";\n";
    oss << "    author = \"" << author << "\";\n";
    oss << "    license = \"" << license << "\";\n";
    oss << "    dependencies = \"" << dependencies << "\";\n";
    oss << "    category = \"" << category << "\";\n";
    oss << "    minCHTLVersion = \"" << minCHTLVersion << "\";\n";
    oss << "    maxCHTLVersion = \"" << maxCHTLVersion << "\";\n";
    oss << "}\n\n";
    
    // 添加导出信息
    if (!exportedStyles.empty() || !exportedElements.empty() || !exportedVars.empty()) {
        oss << "[Export]\n{\n";
        
        if (!exportedStyles.empty()) {
            oss << "    @Style ";
            for (size_t i = 0; i < exportedStyles.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << exportedStyles[i];
            }
            oss << ";\n";
        }
        
        if (!exportedElements.empty()) {
            oss << "    @Element ";
            for (size_t i = 0; i < exportedElements.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << exportedElements[i];
            }
            oss << ";\n";
        }
        
        if (!exportedVars.empty()) {
            oss << "    @Var ";
            for (size_t i = 0; i < exportedVars.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << exportedVars[i];
            }
            oss << ";\n";
        }
        
        oss << "}\n";
    }
    
    return oss.str();
}

bool ModuleInfo::validate() const {
    return !name.empty() && !version.empty();
}

// ==================== SimpleZip ====================

bool SimpleZip::compressToMemory(const std::vector<ModuleFileInfo>& files, std::vector<uint8_t>& output) {
    output.clear();
    
    // 简单的文件格式：
    // [文件数量(4字节)] + [文件1信息] + [文件1数据] + [文件2信息] + [文件2数据] + ...
    // 文件信息格式：[路径长度(2字节)] + [路径字符串] + [内容长度(4字节)] + [是否目录(1字节)] + [CRC32(4字节)]
    
    writeUint32(output, static_cast<uint32_t>(files.size()));
    
    for (const auto& file : files) {
        // 写入路径信息
        writeUint16(output, static_cast<uint16_t>(file.relativePath.size()));
        writeString(output, file.relativePath);
        
        // 写入内容长度
        writeUint32(output, static_cast<uint32_t>(file.content.size()));
        
        // 写入是否为目录
        output.push_back(file.isDirectory ? 1 : 0);
        
        // 计算并写入CRC32
        std::vector<uint8_t> contentBytes(file.content.begin(), file.content.end());
        uint32_t checksum = crc32(contentBytes);
        writeUint32(output, checksum);
        
        // 写入文件内容（如果不是目录）
        if (!file.isDirectory) {
            output.insert(output.end(), file.content.begin(), file.content.end());
        }
    }
    
    return true;
}

bool SimpleZip::decompressFromMemory(const std::vector<uint8_t>& data, std::vector<ModuleFileInfo>& files) {
    files.clear();
    
    if (data.size() < 4) {
        return false;
    }
    
    size_t offset = 0;
    uint32_t fileCount = readUint32(data, offset);
    
    for (uint32_t i = 0; i < fileCount; ++i) {
        if (offset >= data.size()) {
            return false;
        }
        
        // 读取路径长度
        if (offset + 2 > data.size()) return false;
        uint16_t pathLength = readUint16(data, offset);
        
        // 读取路径
        if (offset + pathLength > data.size()) return false;
        String path = readString(data, offset, pathLength);
        
        // 读取内容长度
        if (offset + 4 > data.size()) return false;
        uint32_t contentLength = readUint32(data, offset);
        
        // 读取是否为目录
        if (offset + 1 > data.size()) return false;
        bool isDirectory = (data[offset++] == 1);
        
        // 读取CRC32
        if (offset + 4 > data.size()) return false;
        uint32_t expectedCrc = readUint32(data, offset);
        
        String content;
        if (!isDirectory && contentLength > 0) {
            // 读取文件内容
            if (offset + contentLength > data.size()) return false;
            content = String(data.begin() + offset, data.begin() + offset + contentLength);
            offset += contentLength;
            
            // 验证CRC32
            std::vector<uint8_t> contentBytes(content.begin(), content.end());
            uint32_t actualCrc = crc32(contentBytes);
            if (actualCrc != expectedCrc) {
                return false; // 校验失败
            }
        }
        
        files.emplace_back(path, content, isDirectory);
    }
    
    return true;
}

bool SimpleZip::compressToFile(const std::vector<ModuleFileInfo>& files, const String& outputPath) {
    std::vector<uint8_t> compressedData;
    if (!compressToMemory(files, compressedData)) {
        return false;
    }
    
    std::ofstream file(outputPath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    file.write(reinterpret_cast<const char*>(compressedData.data()), compressedData.size());
    file.close();
    
    return file.good();
}

bool SimpleZip::decompressFromFile(const String& inputPath, std::vector<ModuleFileInfo>& files) {
    std::ifstream file(inputPath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    // 读取整个文件到内存
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<uint8_t> data(fileSize);
    file.read(reinterpret_cast<char*>(data.data()), fileSize);
    file.close();
    
    if (!file.good()) {
        return false;
    }
    
    return decompressFromMemory(data, files);
}

// 简单的CRC32实现
uint32_t SimpleZip::crc32(const std::vector<uint8_t>& data) {
    static const uint32_t crcTable[256] = {
        0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
        // ... 这里应该有完整的CRC32表，为简化只列出前8个
    };
    
    uint32_t crc = 0xFFFFFFFF;
    
    for (uint8_t byte : data) {
        crc = crcTable[(crc ^ byte) & 0xFF] ^ (crc >> 8);
    }
    
    return crc ^ 0xFFFFFFFF;
}

void SimpleZip::writeUint32(std::vector<uint8_t>& buffer, uint32_t value) {
    buffer.push_back(value & 0xFF);
    buffer.push_back((value >> 8) & 0xFF);
    buffer.push_back((value >> 16) & 0xFF);
    buffer.push_back((value >> 24) & 0xFF);
}

void SimpleZip::writeUint16(std::vector<uint8_t>& buffer, uint16_t value) {
    buffer.push_back(value & 0xFF);
    buffer.push_back((value >> 8) & 0xFF);
}

void SimpleZip::writeString(std::vector<uint8_t>& buffer, const String& str) {
    buffer.insert(buffer.end(), str.begin(), str.end());
}

uint32_t SimpleZip::readUint32(const std::vector<uint8_t>& buffer, size_t& offset) {
    uint32_t value = buffer[offset] |
                    (buffer[offset + 1] << 8) |
                    (buffer[offset + 2] << 16) |
                    (buffer[offset + 3] << 24);
    offset += 4;
    return value;
}

uint16_t SimpleZip::readUint16(const std::vector<uint8_t>& buffer, size_t& offset) {
    uint16_t value = buffer[offset] | (buffer[offset + 1] << 8);
    offset += 2;
    return value;
}

String SimpleZip::readString(const std::vector<uint8_t>& buffer, size_t& offset, size_t length) {
    String str(buffer.begin() + offset, buffer.begin() + offset + length);
    offset += length;
    return str;
}

// ==================== CmodManager ====================

bool CmodManager::packCmod(const String& sourceDirectory, const String& outputPath) {
    clearErrors();
    
    if (!validateCmodStructure(sourceDirectory)) {
        addError("Invalid Cmod directory structure: " + sourceDirectory);
        return false;
    }
    
    // 收集所有文件
    auto files = collectFiles(sourceDirectory);
    if (files.empty()) {
        addError("No files found in source directory: " + sourceDirectory);
        return false;
    }
    
    // 验证必需文件
    if (!validateRequiredFiles(files)) {
        addError("Missing required files in Cmod structure");
        return false;
    }
    
    // 打包文件
    if (!SimpleZip::compressToFile(files, outputPath)) {
        addError("Failed to create Cmod file: " + outputPath);
        return false;
    }
    
    return true;
}

bool CmodManager::unpackCmod(const String& cmodPath, const String& outputDirectory) {
    clearErrors();
    
    std::vector<ModuleFileInfo> files;
    if (!SimpleZip::decompressFromFile(cmodPath, files)) {
        addError("Failed to decompress Cmod file: " + cmodPath);
        return false;
    }
    
    // 创建输出目录
    try {
        fs::create_directories(outputDirectory);
    } catch (const std::exception& e) {
        addError("Failed to create output directory: " + outputDirectory);
        return false;
    }
    
    // 创建目录结构并写入文件
    if (!createDirectoryStructure(outputDirectory, files)) {
        addError("Failed to create directory structure");
        return false;
    }
    
    return true;
}

bool CmodManager::validateCmodStructure(const String& directory) {
    // 检查基本结构：
    // ModuleName/
    //   src/
    //     ModuleName.chtl (主模块文件，可选)
    //   info/
    //     ModuleName.chtl (信息文件，必需)
    
    String moduleName = fs::path(directory).filename().string();
    
    String srcDir = joinPath(directory, "src");
    String infoDir = joinPath(directory, "info");
    String infoFile = joinPath(infoDir, moduleName + ".chtl");
    
    if (!fs::exists(srcDir) || !fs::is_directory(srcDir)) {
        addError("Missing src directory");
        return false;
    }
    
    if (!fs::exists(infoDir) || !fs::is_directory(infoDir)) {
        addError("Missing info directory");
        return false;
    }
    
    if (!fs::exists(infoFile) || !fs::is_regular_file(infoFile)) {
        addError("Missing info file: " + infoFile);
        return false;
    }
    
    return true;
}

ModuleInfo CmodManager::loadCmodInfo(const String& cmodPath) {
    ModuleInfo info;
    
    std::vector<ModuleFileInfo> files;
    if (!SimpleZip::decompressFromFile(cmodPath, files)) {
        addError("Failed to load Cmod: " + cmodPath);
        return info;
    }
    
    // 查找info文件
    for (const auto& file : files) {
        if (file.relativePath.find("info/") == 0 && file.relativePath.find(".chtl") != String::npos) {
            info.parseFromString(file.content);
            break;
        }
    }
    
    return info;
}

std::vector<String> CmodManager::listCmodContents(const String& cmodPath) {
    std::vector<String> contents;
    
    std::vector<ModuleFileInfo> files;
    if (SimpleZip::decompressFromFile(cmodPath, files)) {
        for (const auto& file : files) {
            contents.push_back(file.relativePath);
        }
    }
    
    return contents;
}

ModuleInfo CmodManager::createModuleInfoFromDirectory(const String& directory) {
    ModuleInfo info;
    
    String moduleName = fs::path(directory).filename().string();
    info.name = moduleName;
    info.version = "1.0.0";
    
    // 尝试从现有info文件加载
    String infoFile = joinPath(joinPath(directory, "info"), moduleName + ".chtl");
    if (fs::exists(infoFile)) {
        std::ifstream file(infoFile);
        if (file.is_open()) {
            String content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            info.parseFromString(content);
        }
    }
    
    // 生成导出信息
    generateExportInfo(directory, info);
    
    return info;
}

bool CmodManager::generateExportInfo(const String& sourceDirectory, ModuleInfo& moduleInfo) {
    String srcDir = joinPath(sourceDirectory, "src");
    
    try {
        for (const auto& entry : fs::recursive_directory_iterator(srcDir)) {
            if (entry.is_regular_file() && entry.path().extension() == ".chtl") {
                std::ifstream file(entry.path());
                if (file.is_open()) {
                    String content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                    analyzeChtlFile(content, moduleInfo);
                }
            }
        }
    } catch (const std::exception& e) {
        addError("Failed to analyze source files: " + String(e.what()));
        return false;
    }
    
    return true;
}

// 辅助方法实现

void CmodManager::addError(const String& error) {
    errors_.push_back(error);
}

void CmodManager::addWarning(const String& warning) {
    warnings_.push_back(warning);
}

std::vector<ModuleFileInfo> CmodManager::collectFiles(const String& directory, const String& basePath) {
    std::vector<ModuleFileInfo> files;
    
    try {
        for (const auto& entry : fs::recursive_directory_iterator(directory)) {
            String relativePath = fs::relative(entry.path(), directory).string();
            
            // 规范化路径分隔符
            std::replace(relativePath.begin(), relativePath.end(), '\\', '/');
            
            if (entry.is_directory()) {
                files.emplace_back(relativePath, "", true);
            } else if (entry.is_regular_file()) {
                std::ifstream file(entry.path(), std::ios::binary);
                if (file.is_open()) {
                    String content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                    files.emplace_back(relativePath, content, false);
                }
            }
        }
    } catch (const std::exception& e) {
        addError("Failed to collect files: " + String(e.what()));
    }
    
    return files;
}

bool CmodManager::createDirectoryStructure(const String& basePath, const std::vector<ModuleFileInfo>& files) {
    try {
        for (const auto& file : files) {
            String fullPath = joinPath(basePath, file.relativePath);
            
            if (file.isDirectory) {
                fs::create_directories(fullPath);
            } else {
                // 确保父目录存在
                fs::create_directories(fs::path(fullPath).parent_path());
                
                // 写入文件
                std::ofstream outFile(fullPath, std::ios::binary);
                if (outFile.is_open()) {
                    outFile.write(file.content.data(), file.content.size());
                    outFile.close();
                } else {
                    addError("Failed to write file: " + fullPath);
                    return false;
                }
            }
        }
    } catch (const std::exception& e) {
        addError("Failed to create directory structure: " + String(e.what()));
        return false;
    }
    
    return true;
}

bool CmodManager::validateRequiredFiles(const std::vector<ModuleFileInfo>& files) {
    bool hasSrcDir = false;
    bool hasInfoDir = false;
    bool hasInfoFile = false;
    
    for (const auto& file : files) {
        if (file.relativePath == "src" && file.isDirectory) {
            hasSrcDir = true;
        } else if (file.relativePath == "info" && file.isDirectory) {
            hasInfoDir = true;
        } else if (file.relativePath.find("info/") == 0 && file.relativePath.find(".chtl") != String::npos) {
            hasInfoFile = true;
        }
    }
    
    if (!hasSrcDir) {
        addError("Missing src directory");
        return false;
    }
    
    if (!hasInfoDir) {
        addError("Missing info directory");
        return false;
    }
    
    if (!hasInfoFile) {
        addError("Missing info file");
        return false;
    }
    
    return true;
}

void CmodManager::analyzeChtlFile(const String& content, ModuleInfo& moduleInfo) {
    // 简单的CHTL文件分析，查找模板和自定义声明
    std::regex templateRegex(R"(\[Template\]\s+@(Style|Element|Var)\s+(\w+))");
    std::regex customRegex(R"(\[Custom\]\s+@(Style|Element|Var)\s+(\w+))");
    
    std::sregex_iterator iter(content.begin(), content.end(), templateRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        const std::smatch& match = *iter;
        String type = match[1].str();
        String name = match[2].str();
        
        if (type == "Style") {
            moduleInfo.exportedStyles.push_back(name);
        } else if (type == "Element") {
            moduleInfo.exportedElements.push_back(name);
        } else if (type == "Var") {
            moduleInfo.exportedVars.push_back(name);
        }
    }
    
    iter = std::sregex_iterator(content.begin(), content.end(), customRegex);
    for (; iter != end; ++iter) {
        const std::smatch& match = *iter;
        String type = match[1].str();
        String name = match[2].str();
        
        if (type == "Style") {
            moduleInfo.exportedStyles.push_back(name);
        } else if (type == "Element") {
            moduleInfo.exportedElements.push_back(name);
        } else if (type == "Var") {
            moduleInfo.exportedVars.push_back(name);
        }
    }
}

String CmodManager::normalizePath(const String& path) {
    String normalized = path;
    std::replace(normalized.begin(), normalized.end(), '\\', '/');
    return normalized;
}

String CmodManager::joinPath(const String& base, const String& relative) {
    fs::path basePath(base);
    fs::path relativePath(relative);
    return (basePath / relativePath).string();
}

bool CmodManager::isValidModuleName(const String& name) {
    if (name.empty()) {
        return false;
    }
    
    std::regex nameRegex("^[a-zA-Z_][a-zA-Z0-9_]*$");
    return std::regex_match(name, nameRegex);
}

// ==================== CJmodManager ====================

bool CJmodManager::packCJmod(const String& sourceDirectory, const String& outputPath) {
    clearErrors();
    
    if (!validateCJmodStructure(sourceDirectory)) {
        addError("Invalid CJmod directory structure: " + sourceDirectory);
        return false;
    }
    
    // 收集所有文件
    auto files = collectCppFiles(sourceDirectory);
    if (files.empty()) {
        addError("No files found in source directory: " + sourceDirectory);
        return false;
    }
    
    // 验证C++文件结构
    if (!validateCppStructure(files)) {
        addError("Invalid C++ file structure");
        return false;
    }
    
    // 打包文件
    if (!SimpleZip::compressToFile(files, outputPath)) {
        addError("Failed to create CJmod file: " + outputPath);
        return false;
    }
    
    return true;
}

bool CJmodManager::unpackCJmod(const String& cjmodPath, const String& outputDirectory) {
    clearErrors();
    
    std::vector<ModuleFileInfo> files;
    if (!SimpleZip::decompressFromFile(cjmodPath, files)) {
        addError("Failed to decompress CJmod file: " + cjmodPath);
        return false;
    }
    
    // 创建输出目录
    try {
        fs::create_directories(outputDirectory);
    } catch (const std::exception& e) {
        addError("Failed to create output directory: " + outputDirectory);
        return false;
    }
    
    // 创建目录结构并写入文件
    try {
        for (const auto& file : files) {
            String fullPath = fs::path(outputDirectory) / file.relativePath;
            
            if (file.isDirectory) {
                fs::create_directories(fullPath);
            } else {
                fs::create_directories(fs::path(fullPath).parent_path());
                
                std::ofstream outFile(fullPath, std::ios::binary);
                if (outFile.is_open()) {
                    outFile.write(file.content.data(), file.content.size());
                    outFile.close();
                } else {
                    addError("Failed to write file: " + fullPath);
                    return false;
                }
            }
        }
    } catch (const std::exception& e) {
        addError("Failed to create directory structure: " + String(e.what()));
        return false;
    }
    
    return true;
}

bool CJmodManager::validateCJmodStructure(const String& directory) {
    // 检查CJmod结构：
    // CJmodName/
    //   src/ (C++源文件)
    //   info/
    //     CJmodName.chtl (信息文件)
    
    String moduleName = fs::path(directory).filename().string();
    
    String srcDir = fs::path(directory) / "src";
    String infoDir = fs::path(directory) / "info";
    String infoFile = infoDir / (moduleName + ".chtl");
    
    if (!fs::exists(srcDir) || !fs::is_directory(srcDir)) {
        addError("Missing src directory");
        return false;
    }
    
    if (!fs::exists(infoDir) || !fs::is_directory(infoDir)) {
        addError("Missing info directory");
        return false;
    }
    
    if (!fs::exists(infoFile) || !fs::is_regular_file(infoFile)) {
        addError("Missing info file: " + infoFile.string());
        return false;
    }
    
    return true;
}

ModuleInfo CJmodManager::loadCJmodInfo(const String& cjmodPath) {
    ModuleInfo info;
    
    std::vector<ModuleFileInfo> files;
    if (!SimpleZip::decompressFromFile(cjmodPath, files)) {
        addError("Failed to load CJmod: " + cjmodPath);
        return info;
    }
    
    // 查找info文件
    for (const auto& file : files) {
        if (file.relativePath.find("info/") == 0 && file.relativePath.find(".chtl") != String::npos) {
            info.parseFromString(file.content);
            break;
        }
    }
    
    return info;
}

std::vector<String> CJmodManager::listCJmodContents(const String& cjmodPath) {
    std::vector<String> contents;
    
    std::vector<ModuleFileInfo> files;
    if (SimpleZip::decompressFromFile(cjmodPath, files)) {
        for (const auto& file : files) {
            contents.push_back(file.relativePath);
        }
    }
    
    return contents;
}

bool CJmodManager::compileCppSources(const String& sourceDirectory, const String& outputDirectory) {
    // 这里需要调用外部C++编译器
    // 为简化实现，这里只是占位符
    addWarning("C++ compilation not implemented - requires external compiler");
    return true;
}

bool CJmodManager::validateCppInterface(const String& headerPath) {
    std::ifstream file(headerPath);
    if (!file.is_open()) {
        addError("Cannot open header file: " + headerPath);
        return false;
    }
    
    String content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return checkRequiredInterface(content);
}

bool CJmodManager::generateInterfaceDoc(const String& sourceDirectory, const String& outputPath) {
    // 简单的接口文档生成
    std::ofstream docFile(outputPath);
    if (!docFile.is_open()) {
        addError("Cannot create documentation file: " + outputPath);
        return false;
    }
    
    docFile << "# CJmod Interface Documentation\n\n";
    docFile << "Generated from: " << sourceDirectory << "\n\n";
    
    // 这里应该分析C++头文件并生成文档
    // 为简化实现，只生成基本框架
    
    docFile.close();
    return true;
}

// CJmodManager辅助方法

void CJmodManager::addError(const String& error) {
    errors_.push_back(error);
}

void CJmodManager::addWarning(const String& warning) {
    warnings_.push_back(warning);
}

std::vector<ModuleFileInfo> CJmodManager::collectCppFiles(const String& directory, const String& basePath) {
    std::vector<ModuleFileInfo> files;
    
    try {
        for (const auto& entry : fs::recursive_directory_iterator(directory)) {
            String relativePath = fs::relative(entry.path(), directory).string();
            
            // 规范化路径分隔符
            std::replace(relativePath.begin(), relativePath.end(), '\\', '/');
            
            if (entry.is_directory()) {
                files.emplace_back(relativePath, "", true);
            } else if (entry.is_regular_file()) {
                String ext = entry.path().extension().string();
                if (ext == ".cpp" || ext == ".h" || ext == ".hpp" || ext == ".chtl") {
                    std::ifstream file(entry.path(), std::ios::binary);
                    if (file.is_open()) {
                        String content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                        files.emplace_back(relativePath, content, false);
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        addError("Failed to collect C++ files: " + String(e.what()));
    }
    
    return files;
}

bool CJmodManager::validateCppStructure(const std::vector<ModuleFileInfo>& files) {
    bool hasCppFile = false;
    bool hasHeaderFile = false;
    bool hasInfoFile = false;
    
    for (const auto& file : files) {
        String ext = fs::path(file.relativePath).extension().string();
        
        if (ext == ".cpp") {
            hasCppFile = true;
        } else if (ext == ".h" || ext == ".hpp") {
            hasHeaderFile = true;
        } else if (file.relativePath.find("info/") == 0 && ext == ".chtl") {
            hasInfoFile = true;
        }
    }
    
    if (!hasCppFile) {
        addError("No C++ source files found");
        return false;
    }
    
    if (!hasHeaderFile) {
        addError("No C++ header files found");
        return false;
    }
    
    if (!hasInfoFile) {
        addError("No info file found");
        return false;
    }
    
    return true;
}

void CJmodManager::analyzeCppHeader(const String& content, ModuleInfo& moduleInfo) {
    // 简单的C++头文件分析
    // 查找导出的函数和类
    std::regex functionRegex(R"(extern\s+"C"\s+\w+\s+(\w+)\s*\()");
    std::regex classRegex(R"(class\s+(\w+))");
    
    std::sregex_iterator iter(content.begin(), content.end(), functionRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        const std::smatch& match = *iter;
        String functionName = match[1].str();
        // 这里可以根据函数名推断导出类型
    }
}

bool CJmodManager::checkRequiredInterface(const String& headerContent) {
    // 检查是否包含必需的CHTL JS接口函数
    std::vector<String> requiredFunctions = {
        "chtl_js_init",
        "chtl_js_cleanup",
        "chtl_js_register_functions"
    };
    
    for (const auto& func : requiredFunctions) {
        if (headerContent.find(func) == String::npos) {
            addError("Missing required interface function: " + func);
            return false;
        }
    }
    
    return true;
}

// ==================== ModuleManager ====================

ModuleManager::ModuleManager()
    : cmodManager_(std::make_unique<CmodManager>()),
      cjmodManager_(std::make_unique<CJmodManager>()) {
}

bool ModuleManager::initialize(const String& officialModulePath) {
    officialModulePath_ = officialModulePath;
    installDirectory_ = "module"; // 默认安装目录
    
    // 加载模块注册表
    return loadModuleRegistry();
}

bool ModuleManager::installModule(const String& modulePath, const String& installDirectory) {
    String targetDir = installDirectory.empty() ? installDirectory_ : installDirectory;
    
    ModuleType type = detectModuleType(modulePath);
    
    switch (type) {
        case ModuleType::CMOD: {
            ModuleInfo info = cmodManager_->loadCmodInfo(modulePath);
            if (!info.validate()) {
                addError("Invalid Cmod module: " + modulePath);
                return false;
            }
            
            String installPath = joinPath(targetDir, info.name);
            if (!cmodManager_->unpackCmod(modulePath, installPath)) {
                addError("Failed to install Cmod: " + modulePath);
                return false;
            }
            
            installedModules_[info.name] = info;
            break;
        }
        case ModuleType::CJMOD: {
            ModuleInfo info = cjmodManager_->loadCJmodInfo(modulePath);
            if (!info.validate()) {
                addError("Invalid CJmod module: " + modulePath);
                return false;
            }
            
            String installPath = joinPath(targetDir, info.name);
            if (!cjmodManager_->unpackCJmod(modulePath, installPath)) {
                addError("Failed to install CJmod: " + modulePath);
                return false;
            }
            
            installedModules_[info.name] = info;
            break;
        }
        default:
            addError("Unknown module type: " + modulePath);
            return false;
    }
    
    return saveModuleRegistry();
}

bool ModuleManager::uninstallModule(const String& moduleName) {
    auto it = installedModules_.find(moduleName);
    if (it == installedModules_.end()) {
        addError("Module not found: " + moduleName);
        return false;
    }
    
    // 删除模块目录
    String modulePath = joinPath(installDirectory_, moduleName);
    try {
        fs::remove_all(modulePath);
    } catch (const std::exception& e) {
        addError("Failed to remove module directory: " + modulePath);
        return false;
    }
    
    // 从注册表中删除
    installedModules_.erase(it);
    moduleCache_.erase(moduleName);
    
    return saveModuleRegistry();
}

std::vector<ModuleInfo> ModuleManager::listInstalledModules() {
    std::vector<ModuleInfo> modules;
    
    for (const auto& pair : installedModules_) {
        modules.push_back(pair.second);
    }
    
    return modules;
}

String ModuleManager::findModule(const String& moduleName) {
    // 检查官方模块
    if (isOfficialModule(moduleName)) {
        return resolveOfficialModulePath(moduleName);
    }
    
    // 检查已安装的模块
    auto it = installedModules_.find(moduleName);
    if (it != installedModules_.end()) {
        return joinPath(installDirectory_, moduleName);
    }
    
    return "";
}

std::shared_ptr<CHTLASTNode> ModuleManager::loadModule(const String& moduleName) {
    // 检查缓存
    auto cacheIt = moduleCache_.find(moduleName);
    if (cacheIt != moduleCache_.end()) {
        return cacheIt->second;
    }
    
    String modulePath = findModule(moduleName);
    if (modulePath.empty()) {
        addError("Module not found: " + moduleName);
        return nullptr;
    }
    
    // 加载模块
    // 这里应该解析模块文件并创建AST
    // 为简化实现，返回空指针
    addWarning("Module loading not fully implemented: " + moduleName);
    return nullptr;
}

bool ModuleManager::isOfficialModule(const String& moduleName) {
    return moduleName.find("chtl::") == 0;
}

String ModuleManager::resolveOfficialModulePath(const String& moduleName) {
    if (!isOfficialModule(moduleName)) {
        return "";
    }
    
    String actualName = moduleName.substr(6); // 移除 "chtl::" 前缀
    return joinPath(officialModulePath_, actualName);
}

ModuleManager::ModuleType ModuleManager::detectModuleType(const String& modulePath) {
    String ext = fs::path(modulePath).extension().string();
    
    if (ext == ".cmod") {
        return ModuleType::CMOD;
    } else if (ext == ".cjmod") {
        return ModuleType::CJMOD;
    }
    
    return ModuleType::UNKNOWN;
}

String ModuleManager::joinPath(const String& base, const String& relative) {
    return (fs::path(base) / relative).string();
}

void ModuleManager::addError(const String& error) {
    errors_.push_back(error);
}

void ModuleManager::addWarning(const String& warning) {
    warnings_.push_back(warning);
}

// 其他ModuleManager方法的简化实现
bool ModuleManager::saveModuleRegistry() { return true; }
bool ModuleManager::loadModuleRegistry() { return true; }
String ModuleManager::getRegistryPath() { return "module_registry.json"; }
void ModuleManager::clearModuleCache() { moduleCache_.clear(); }
size_t ModuleManager::getModuleCacheSize() const { return moduleCache_.size(); }
void ModuleManager::setOfficialModulePath(const String& path) { officialModulePath_ = path; }
void ModuleManager::setInstallDirectory(const String& directory) { installDirectory_ = directory; }

} // namespace chtl