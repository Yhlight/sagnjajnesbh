#pragma once

#include "CHTLCommon.h"
#include <vector>
#include <fstream>
#include <memory>

namespace chtl {

// ZIP文件条目
struct ZipEntry {
    String filename;
    std::vector<uint8_t> data;
    uint32_t crc32;
    uint32_t compressedSize;
    uint32_t uncompressedSize;
    uint16_t compressionMethod;
    
    ZipEntry(const String& name = "") 
        : filename(name), crc32(0), compressedSize(0), uncompressedSize(0), compressionMethod(0) {}
};

// 简单ZIP操作结果
struct ZipResult {
    bool success = false;
    String error;
    std::vector<ZipEntry> entries;
};

// 简单ZIP库实现
class SimpleZip {
public:
    SimpleZip() = default;
    ~SimpleZip() = default;
    
    // 创建ZIP文件
    static ZipResult createZip(const String& zipPath, const std::vector<ZipEntry>& entries);
    
    // 解压ZIP文件
    static ZipResult extractZip(const String& zipPath);
    
    // 添加文件到ZIP条目
    static ZipEntry createEntryFromFile(const String& filePath, const String& entryName = "");
    
    // 添加目录到ZIP条目（递归）
    static std::vector<ZipEntry> createEntriesFromDirectory(const String& dirPath, const String& baseDir = "");
    
    // 将ZIP条目写入文件系统
    static bool writeEntryToFile(const ZipEntry& entry, const String& outputPath);
    
    // CRC32计算
    static uint32_t calculateCRC32(const std::vector<uint8_t>& data);
    
    // 简单压缩（存储模式，无压缩）
    static std::vector<uint8_t> compress(const std::vector<uint8_t>& data, uint16_t& method);
    
    // 简单解压
    static std::vector<uint8_t> decompress(const std::vector<uint8_t>& data, uint16_t method);

private:
    // ZIP文件格式常量
    static const uint32_t LOCAL_FILE_HEADER_SIGNATURE = 0x04034b50;
    static const uint32_t CENTRAL_DIR_HEADER_SIGNATURE = 0x02014b50;
    static const uint32_t END_OF_CENTRAL_DIR_SIGNATURE = 0x06054b50;
    
    // 写入ZIP文件头
    static void writeLocalFileHeader(std::ofstream& file, const ZipEntry& entry);
    static void writeCentralDirHeader(std::ofstream& file, const ZipEntry& entry, uint32_t localHeaderOffset);
    static void writeEndOfCentralDir(std::ofstream& file, uint16_t entryCount, uint32_t centralDirSize, uint32_t centralDirOffset);
    
    // 读取ZIP文件头
    static bool readLocalFileHeader(std::ifstream& file, ZipEntry& entry);
    static bool readCentralDirHeader(std::ifstream& file, ZipEntry& entry, uint32_t& localHeaderOffset);
    static bool findEndOfCentralDir(std::ifstream& file, uint16_t& entryCount, uint32_t& centralDirOffset);
    
    // 工具方法
    static void writeUint16(std::ofstream& file, uint16_t value);
    static void writeUint32(std::ofstream& file, uint32_t value);
    static uint16_t readUint16(std::ifstream& file);
    static uint32_t readUint32(std::ifstream& file);
};

// CMOD打包器
class CmodPacker {
public:
    // 打包CMOD文件夹
    static bool packCmod(const String& sourceDir, const String& outputPath);
    
    // 解包CMOD文件
    static bool unpackCmod(const String& cmodPath, const String& outputDir);
    
    // 验证CMOD文件结构
    static bool validateCmodStructure(const String& sourceDir);
    
private:
    static bool validateModuleInfo(const String& infoPath);
    static bool validateSourceStructure(const String& srcPath);
};

// CJMOD打包器
class CJmodPacker {
public:
    // 打包CJMOD文件夹
    static bool packCJmod(const String& sourceDir, const String& outputPath);
    
    // 解包CJMOD文件
    static bool unpackCJmod(const String& cjmodPath, const String& outputDir);
    
    // 验证CJMOD文件结构
    static bool validateCJmodStructure(const String& sourceDir);
    
private:
    static bool validateCJmodInfo(const String& infoPath);
    static bool validateCJmodSource(const String& srcPath);
};

} // namespace chtl