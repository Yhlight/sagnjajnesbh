#include "../include/SimpleZip.h"
#include <filesystem>
#include <iostream>
#include <algorithm>

namespace fs = std::filesystem;

namespace chtl {

// CRC32表（简化版本）
static uint32_t crc32_table[256];
static bool crc32_table_initialized = false;

static void init_crc32_table() {
    if (crc32_table_initialized) return;
    
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t crc = i;
        for (int j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xEDB88320;
            } else {
                crc >>= 1;
            }
        }
        crc32_table[i] = crc;
    }
    crc32_table_initialized = true;
}

uint32_t SimpleZip::calculateCRC32(const std::vector<uint8_t>& data) {
    init_crc32_table();
    
    uint32_t crc = 0xFFFFFFFF;
    for (uint8_t byte : data) {
        crc = crc32_table[(crc ^ byte) & 0xFF] ^ (crc >> 8);
    }
    return crc ^ 0xFFFFFFFF;
}

std::vector<uint8_t> SimpleZip::compress(const std::vector<uint8_t>& data, uint16_t& method) {
    // 简单实现：只使用存储模式（无压缩）
    method = 0; // 存储模式
    return data;
}

std::vector<uint8_t> SimpleZip::decompress(const std::vector<uint8_t>& data, uint16_t method) {
    if (method == 0) {
        // 存储模式，无需解压
        return data;
    }
    // 其他压缩方法暂不支持
    return data;
}

ZipEntry SimpleZip::createEntryFromFile(const String& filePath, const String& entryName) {
    ZipEntry entry;
    
    try {
        // 确定条目名称
        if (entryName.empty()) {
            entry.filename = fs::path(filePath).filename().string();
        } else {
            entry.filename = entryName;
        }
        
        // 读取文件内容
        std::ifstream file(filePath, std::ios::binary);
        if (!file.is_open()) {
            return entry; // 返回空条目
        }
        
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        
        entry.data.resize(fileSize);
        file.read(reinterpret_cast<char*>(entry.data.data()), fileSize);
        file.close();
        
        // 计算CRC32和大小
        entry.crc32 = calculateCRC32(entry.data);
        entry.uncompressedSize = static_cast<uint32_t>(entry.data.size());
        
        // 压缩数据
        entry.data = compress(entry.data, entry.compressionMethod);
        entry.compressedSize = static_cast<uint32_t>(entry.data.size());
        
    } catch (const std::exception& e) {
        std::cerr << "创建ZIP条目失败: " << e.what() << std::endl;
    }
    
    return entry;
}

std::vector<ZipEntry> SimpleZip::createEntriesFromDirectory(const String& dirPath, const String& baseDir) {
    std::vector<ZipEntry> entries;
    
    try {
        for (const auto& entry : fs::recursive_directory_iterator(dirPath)) {
            if (entry.is_regular_file()) {
                String filePath = entry.path().string();
                String relativePath = fs::relative(entry.path(), baseDir.empty() ? dirPath : baseDir).string();
                
                // 将路径分隔符统一为 '/'
                std::replace(relativePath.begin(), relativePath.end(), '\\', '/');
                
                auto zipEntry = createEntryFromFile(filePath, relativePath);
                if (!zipEntry.filename.empty() && !zipEntry.data.empty()) {
                    entries.push_back(zipEntry);
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "遍历目录失败: " << e.what() << std::endl;
    }
    
    return entries;
}

void SimpleZip::writeUint16(std::ofstream& file, uint16_t value) {
    file.write(reinterpret_cast<const char*>(&value), sizeof(value));
}

void SimpleZip::writeUint32(std::ofstream& file, uint32_t value) {
    file.write(reinterpret_cast<const char*>(&value), sizeof(value));
}

uint16_t SimpleZip::readUint16(std::ifstream& file) {
    uint16_t value;
    file.read(reinterpret_cast<char*>(&value), sizeof(value));
    return value;
}

uint32_t SimpleZip::readUint32(std::ifstream& file) {
    uint32_t value;
    file.read(reinterpret_cast<char*>(&value), sizeof(value));
    return value;
}

void SimpleZip::writeLocalFileHeader(std::ofstream& file, const ZipEntry& entry) {
    writeUint32(file, LOCAL_FILE_HEADER_SIGNATURE);
    writeUint16(file, 20); // 版本
    writeUint16(file, 0);  // 标志
    writeUint16(file, entry.compressionMethod);
    writeUint16(file, 0);  // 修改时间
    writeUint16(file, 0);  // 修改日期
    writeUint32(file, entry.crc32);
    writeUint32(file, entry.compressedSize);
    writeUint32(file, entry.uncompressedSize);
    writeUint16(file, static_cast<uint16_t>(entry.filename.length()));
    writeUint16(file, 0);  // 额外字段长度
    
    // 写入文件名
    file.write(entry.filename.c_str(), entry.filename.length());
}

ZipResult SimpleZip::createZip(const String& zipPath, const std::vector<ZipEntry>& entries) {
    ZipResult result;
    
    try {
        std::ofstream file(zipPath, std::ios::binary);
        if (!file.is_open()) {
            result.error = "无法创建ZIP文件: " + zipPath;
            return result;
        }
        
        std::vector<uint32_t> localHeaderOffsets;
        
        // 写入本地文件头和数据
        for (const auto& entry : entries) {
            localHeaderOffsets.push_back(static_cast<uint32_t>(file.tellp()));
            writeLocalFileHeader(file, entry);
            file.write(reinterpret_cast<const char*>(entry.data.data()), entry.data.size());
        }
        
        // 记录中央目录起始位置
        uint32_t centralDirOffset = static_cast<uint32_t>(file.tellp());
        
        // 写入中央目录头
        for (size_t i = 0; i < entries.size(); ++i) {
            writeCentralDirHeader(file, entries[i], localHeaderOffsets[i]);
        }
        
        // 计算中央目录大小
        uint32_t centralDirSize = static_cast<uint32_t>(file.tellp()) - centralDirOffset;
        
        // 写入中央目录结束记录
        writeEndOfCentralDir(file, static_cast<uint16_t>(entries.size()), centralDirSize, centralDirOffset);
        
        file.close();
        result.success = true;
        
    } catch (const std::exception& e) {
        result.error = "创建ZIP文件失败: " + String(e.what());
    }
    
    return result;
}

void SimpleZip::writeCentralDirHeader(std::ofstream& file, const ZipEntry& entry, uint32_t localHeaderOffset) {
    writeUint32(file, CENTRAL_DIR_HEADER_SIGNATURE);
    writeUint16(file, 20); // 创建版本
    writeUint16(file, 20); // 提取版本
    writeUint16(file, 0);  // 标志
    writeUint16(file, entry.compressionMethod);
    writeUint16(file, 0);  // 修改时间
    writeUint16(file, 0);  // 修改日期
    writeUint32(file, entry.crc32);
    writeUint32(file, entry.compressedSize);
    writeUint32(file, entry.uncompressedSize);
    writeUint16(file, static_cast<uint16_t>(entry.filename.length()));
    writeUint16(file, 0);  // 额外字段长度
    writeUint16(file, 0);  // 注释长度
    writeUint16(file, 0);  // 磁盘号
    writeUint16(file, 0);  // 内部属性
    writeUint32(file, 0);  // 外部属性
    writeUint32(file, localHeaderOffset);
    
    // 写入文件名
    file.write(entry.filename.c_str(), entry.filename.length());
}

void SimpleZip::writeEndOfCentralDir(std::ofstream& file, uint16_t entryCount, uint32_t centralDirSize, uint32_t centralDirOffset) {
    writeUint32(file, END_OF_CENTRAL_DIR_SIGNATURE);
    writeUint16(file, 0);  // 磁盘号
    writeUint16(file, 0);  // 中央目录磁盘号
    writeUint16(file, entryCount); // 本磁盘条目数
    writeUint16(file, entryCount); // 总条目数
    writeUint32(file, centralDirSize);
    writeUint32(file, centralDirOffset);
    writeUint16(file, 0);  // 注释长度
}

ZipResult SimpleZip::extractZip(const String& zipPath) {
    ZipResult result;
    
    try {
        std::ifstream file(zipPath, std::ios::binary);
        if (!file.is_open()) {
            result.error = "无法打开ZIP文件: " + zipPath;
            return result;
        }
        
        // 查找中央目录结束记录
        uint16_t entryCount;
        uint32_t centralDirOffset;
        if (!findEndOfCentralDir(file, entryCount, centralDirOffset)) {
            result.error = "无效的ZIP文件格式";
            return result;
        }
        
        // 读取中央目录
        file.seekg(centralDirOffset);
        for (uint16_t i = 0; i < entryCount; ++i) {
            ZipEntry entry;
            uint32_t localHeaderOffset;
            if (!readCentralDirHeader(file, entry, localHeaderOffset)) {
                result.error = "读取中央目录失败";
                return result;
            }
            
            // 读取文件数据
            std::streampos currentPos = file.tellg();
            file.seekg(localHeaderOffset);
            
            ZipEntry localEntry;
            if (readLocalFileHeader(file, localEntry)) {
                entry.data.resize(entry.compressedSize);
                file.read(reinterpret_cast<char*>(entry.data.data()), entry.compressedSize);
                
                // 解压数据
                entry.data = decompress(entry.data, entry.compressionMethod);
            }
            
            file.seekg(currentPos);
            result.entries.push_back(entry);
        }
        
        file.close();
        result.success = true;
        
    } catch (const std::exception& e) {
        result.error = "解压ZIP文件失败: " + String(e.what());
    }
    
    return result;
}

bool SimpleZip::readCentralDirHeader(std::ifstream& file, ZipEntry& entry, uint32_t& localHeaderOffset) {
    uint32_t signature = readUint32(file);
    if (signature != CENTRAL_DIR_HEADER_SIGNATURE) {
        return false;
    }
    
    readUint16(file); // 创建版本
    readUint16(file); // 提取版本
    readUint16(file); // 标志
    entry.compressionMethod = readUint16(file);
    readUint16(file); // 修改时间
    readUint16(file); // 修改日期
    entry.crc32 = readUint32(file);
    entry.compressedSize = readUint32(file);
    entry.uncompressedSize = readUint32(file);
    uint16_t filenameLength = readUint16(file);
    uint16_t extraFieldLength = readUint16(file);
    uint16_t commentLength = readUint16(file);
    readUint16(file); // 磁盘号
    readUint16(file); // 内部属性
    readUint32(file); // 外部属性
    localHeaderOffset = readUint32(file);
    
    // 读取文件名
    entry.filename.resize(filenameLength);
    file.read(&entry.filename[0], filenameLength);
    
    // 跳过额外字段和注释
    file.seekg(extraFieldLength + commentLength, std::ios::cur);
    
    return true;
}

bool SimpleZip::readLocalFileHeader(std::ifstream& file, ZipEntry& entry) {
    uint32_t signature = readUint32(file);
    if (signature != LOCAL_FILE_HEADER_SIGNATURE) {
        return false;
    }
    
    readUint16(file); // 版本
    readUint16(file); // 标志
    entry.compressionMethod = readUint16(file);
    readUint16(file); // 修改时间
    readUint16(file); // 修改日期
    entry.crc32 = readUint32(file);
    entry.compressedSize = readUint32(file);
    entry.uncompressedSize = readUint32(file);
    uint16_t filenameLength = readUint16(file);
    uint16_t extraFieldLength = readUint16(file);
    
    // 读取文件名
    entry.filename.resize(filenameLength);
    file.read(&entry.filename[0], filenameLength);
    
    // 跳过额外字段
    file.seekg(extraFieldLength, std::ios::cur);
    
    return true;
}

bool SimpleZip::findEndOfCentralDir(std::ifstream& file, uint16_t& entryCount, uint32_t& centralDirOffset) {
    // 从文件末尾开始搜索
    file.seekg(-22, std::ios::end); // 最小EOCD记录大小
    
    for (int i = 0; i < 65536; ++i) { // 最大注释长度
        uint32_t signature = readUint32(file);
        if (signature == END_OF_CENTRAL_DIR_SIGNATURE) {
            readUint16(file); // 磁盘号
            readUint16(file); // 中央目录磁盘号
            readUint16(file); // 本磁盘条目数
            entryCount = readUint16(file); // 总条目数
            readUint32(file); // 中央目录大小
            centralDirOffset = readUint32(file);
            return true;
        }
        
        // 向前移动一个字节继续搜索
        if (file.tellg() <= 4) break;
        file.seekg(-5, std::ios::cur);
    }
    
    return false;
}

bool SimpleZip::writeEntryToFile(const ZipEntry& entry, const String& outputPath) {
    try {
        // 确保输出目录存在
        fs::path filePath(outputPath);
        fs::create_directories(filePath.parent_path());
        
        std::ofstream file(outputPath, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        file.write(reinterpret_cast<const char*>(entry.data.data()), entry.data.size());
        file.close();
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "写入文件失败: " << e.what() << std::endl;
        return false;
    }
}

// ==================== CmodPacker ====================

bool CmodPacker::validateCmodStructure(const String& sourceDir) {
    try {
        fs::path dirPath(sourceDir);
        
        // 检查基本结构
        fs::path srcPath = dirPath / "src";
        fs::path infoPath = dirPath / "info";
        
        if (!fs::exists(srcPath) || !fs::is_directory(srcPath)) {
            std::cerr << "CMOD结构错误: 缺少src目录" << std::endl;
            return false;
        }
        
        if (!fs::exists(infoPath) || !fs::is_directory(infoPath)) {
            std::cerr << "CMOD结构错误: 缺少info目录" << std::endl;
            return false;
        }
        
        // 检查主模块文件
        String moduleName = dirPath.filename().string();
        fs::path mainModuleInfo = infoPath / (moduleName + ".chtl");
        
        if (!fs::exists(mainModuleInfo)) {
            std::cerr << "CMOD结构错误: 缺少主模块信息文件 " << mainModuleInfo << std::endl;
            return false;
        }
        
        // 验证模块信息文件
        if (!validateModuleInfo(mainModuleInfo.string())) {
            return false;
        }
        
        // 验证源码结构
        if (!validateSourceStructure(srcPath.string())) {
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "验证CMOD结构失败: " << e.what() << std::endl;
        return false;
    }
}

bool CmodPacker::validateModuleInfo(const String& infoPath) {
    try {
        std::ifstream file(infoPath);
        if (!file.is_open()) {
            std::cerr << "无法打开模块信息文件: " << infoPath << std::endl;
            return false;
        }
        
        String content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        
        // 简单验证：检查是否包含[Info]块
        if (content.find("[Info]") == String::npos) {
            std::cerr << "模块信息文件格式错误: 缺少[Info]块" << std::endl;
            return false;
        }
        
        // 检查必要字段
        std::vector<String> requiredFields = {"name", "version", "description", "author", "license"};
        for (const auto& field : requiredFields) {
            if (content.find(field + " =") == String::npos && content.find(field + "=") == String::npos) {
                std::cerr << "模块信息文件缺少必要字段: " << field << std::endl;
                return false;
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "验证模块信息失败: " << e.what() << std::endl;
        return false;
    }
}

bool CmodPacker::validateSourceStructure(const String& srcPath) {
    try {
        // 检查src目录中是否有有效的CHTL文件
        bool hasValidSource = false;
        
        for (const auto& entry : fs::recursive_directory_iterator(srcPath)) {
            if (entry.is_regular_file()) {
                String extension = entry.path().extension().string();
                if (extension == ".chtl") {
                    hasValidSource = true;
                    break;
                }
            }
        }
        
        if (!hasValidSource) {
            std::cerr << "CMOD源码目录中没有找到有效的CHTL文件" << std::endl;
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "验证源码结构失败: " << e.what() << std::endl;
        return false;
    }
}

bool CmodPacker::packCmod(const String& sourceDir, const String& outputPath) {
    try {
        // 验证CMOD结构
        if (!validateCmodStructure(sourceDir)) {
            return false;
        }
        
        // 创建ZIP条目
        auto entries = SimpleZip::createEntriesFromDirectory(sourceDir);
        if (entries.empty()) {
            std::cerr << "没有找到要打包的文件" << std::endl;
            return false;
        }
        
        // 创建CMOD文件
        auto result = SimpleZip::createZip(outputPath, entries);
        if (!result.success) {
            std::cerr << "创建CMOD文件失败: " << result.error << std::endl;
            return false;
        }
        
        std::cout << "成功创建CMOD文件: " << outputPath << std::endl;
        std::cout << "包含 " << entries.size() << " 个文件" << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "打包CMOD失败: " << e.what() << std::endl;
        return false;
    }
}

bool CmodPacker::unpackCmod(const String& cmodPath, const String& outputDir) {
    try {
        // 解压CMOD文件
        auto result = SimpleZip::extractZip(cmodPath);
        if (!result.success) {
            std::cerr << "解压CMOD文件失败: " << result.error << std::endl;
            return false;
        }
        
        // 写入文件
        fs::create_directories(outputDir);
        
        for (const auto& entry : result.entries) {
            String outputPath = outputDir + "/" + entry.filename;
            if (!SimpleZip::writeEntryToFile(entry, outputPath)) {
                std::cerr << "写入文件失败: " << outputPath << std::endl;
                return false;
            }
        }
        
        std::cout << "成功解压CMOD文件到: " << outputDir << std::endl;
        std::cout << "解压了 " << result.entries.size() << " 个文件" << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "解压CMOD失败: " << e.what() << std::endl;
        return false;
    }
}

// ==================== CJmodPacker ====================

bool CJmodPacker::validateCJmodStructure(const String& sourceDir) {
    try {
        fs::path dirPath(sourceDir);
        
        // 检查基本结构
        fs::path srcPath = dirPath / "src";
        fs::path infoPath = dirPath / "info";
        
        if (!fs::exists(srcPath) || !fs::is_directory(srcPath)) {
            std::cerr << "CJMOD结构错误: 缺少src目录" << std::endl;
            return false;
        }
        
        if (!fs::exists(infoPath) || !fs::is_directory(infoPath)) {
            std::cerr << "CJMOD结构错误: 缺少info目录" << std::endl;
            return false;
        }
        
        // 检查主模块文件
        String moduleName = dirPath.filename().string();
        fs::path mainModuleInfo = infoPath / (moduleName + ".chtl");
        
        if (!fs::exists(mainModuleInfo)) {
            std::cerr << "CJMOD结构错误: 缺少主模块信息文件 " << mainModuleInfo << std::endl;
            return false;
        }
        
        // 验证模块信息文件
        if (!validateCJmodInfo(mainModuleInfo.string())) {
            return false;
        }
        
        // 验证源码结构
        if (!validateCJmodSource(srcPath.string())) {
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "验证CJMOD结构失败: " << e.what() << std::endl;
        return false;
    }
}

bool CJmodPacker::validateCJmodInfo(const String& infoPath) {
    try {
        std::ifstream file(infoPath);
        if (!file.is_open()) {
            std::cerr << "无法打开CJMOD信息文件: " << infoPath << std::endl;
            return false;
        }
        
        String content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        
        // 简单验证：检查是否包含[Info]块
        if (content.find("[Info]") == String::npos) {
            std::cerr << "CJMOD信息文件格式错误: 缺少[Info]块" << std::endl;
            return false;
        }
        
        // 检查CJMOD特有字段
        std::vector<String> requiredFields = {"name", "version", "description", "author", "license"};
        for (const auto& field : requiredFields) {
            if (content.find(field + " =") == String::npos && content.find(field + "=") == String::npos) {
                std::cerr << "CJMOD信息文件缺少必要字段: " << field << std::endl;
                return false;
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "验证CJMOD信息失败: " << e.what() << std::endl;
        return false;
    }
}

bool CJmodPacker::validateCJmodSource(const String& srcPath) {
    try {
        // 检查src目录中是否有有效的C++文件
        bool hasValidSource = false;
        
        for (const auto& entry : fs::recursive_directory_iterator(srcPath)) {
            if (entry.is_regular_file()) {
                String extension = entry.path().extension().string();
                if (extension == ".cpp" || extension == ".h" || extension == ".hpp") {
                    hasValidSource = true;
                    break;
                }
            }
        }
        
        if (!hasValidSource) {
            std::cerr << "CJMOD源码目录中没有找到有效的C++文件" << std::endl;
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "验证CJMOD源码结构失败: " << e.what() << std::endl;
        return false;
    }
}

bool CJmodPacker::packCJmod(const String& sourceDir, const String& outputPath) {
    try {
        // 验证CJMOD结构
        if (!validateCJmodStructure(sourceDir)) {
            return false;
        }
        
        // 创建ZIP条目
        auto entries = SimpleZip::createEntriesFromDirectory(sourceDir);
        if (entries.empty()) {
            std::cerr << "没有找到要打包的文件" << std::endl;
            return false;
        }
        
        // 创建CJMOD文件
        auto result = SimpleZip::createZip(outputPath, entries);
        if (!result.success) {
            std::cerr << "创建CJMOD文件失败: " << result.error << std::endl;
            return false;
        }
        
        std::cout << "成功创建CJMOD文件: " << outputPath << std::endl;
        std::cout << "包含 " << entries.size() << " 个文件" << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "打包CJMOD失败: " << e.what() << std::endl;
        return false;
    }
}

bool CJmodPacker::unpackCJmod(const String& cjmodPath, const String& outputDir) {
    try {
        // 解压CJMOD文件
        auto result = SimpleZip::extractZip(cjmodPath);
        if (!result.success) {
            std::cerr << "解压CJMOD文件失败: " << result.error << std::endl;
            return false;
        }
        
        // 写入文件
        fs::create_directories(outputDir);
        
        for (const auto& entry : result.entries) {
            String outputPath = outputDir + "/" + entry.filename;
            if (!SimpleZip::writeEntryToFile(entry, outputPath)) {
                std::cerr << "写入文件失败: " << outputPath << std::endl;
                return false;
            }
        }
        
        std::cout << "成功解压CJMOD文件到: " << outputDir << std::endl;
        std::cout << "解压了 " << result.entries.size() << " 个文件" << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "解压CJMOD失败: " << e.what() << std::endl;
        return false;
    }
}

} // namespace chtl