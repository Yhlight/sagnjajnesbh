#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <cstdint>

namespace chtl {

// ZIP文件格式的基本结构
struct ZipFileEntry {
    std::string filename;
    std::vector<uint8_t> data;
    uint32_t crc32;
    uint32_t compressed_size;
    uint32_t uncompressed_size;
    uint16_t compression_method;
    uint32_t last_modified_time;
    uint32_t last_modified_date;
};

// 简单的ZIP文件处理类
class SimpleZip {
public:
    SimpleZip();
    ~SimpleZip();
    
    // 添加文件到ZIP
    bool addFile(const std::string& filename, const std::vector<uint8_t>& data);
    bool addFile(const std::string& filename, const std::string& content);
    bool addFileFromPath(const std::string& zip_path, const std::string& file_path);
    
    // 添加目录（递归）
    bool addDirectory(const std::string& zip_dir_path, const std::string& local_dir_path);
    
    // 保存ZIP文件
    bool saveToFile(const std::string& zip_file_path);
    
    // 从ZIP文件加载
    bool loadFromFile(const std::string& zip_file_path);
    
    // 提取文件
    bool extractFile(const std::string& filename, std::vector<uint8_t>& data);
    bool extractFile(const std::string& filename, std::string& content);
    bool extractToPath(const std::string& filename, const std::string& output_path);
    
    // 提取所有文件到目录
    bool extractAll(const std::string& output_directory);
    
    // 获取文件列表
    std::vector<std::string> getFileList() const;
    
    // 检查文件是否存在
    bool hasFile(const std::string& filename) const;
    
    // 获取文件信息
    bool getFileInfo(const std::string& filename, ZipFileEntry& info) const;
    
    // 清空ZIP内容
    void clear();
    
    // 错误信息
    std::string getLastError() const { return last_error_; }

private:
    std::unordered_map<std::string, std::unique_ptr<ZipFileEntry>> entries_;
    std::string last_error_;
    
    // 内部辅助方法
    uint32_t calculateCRC32(const std::vector<uint8_t>& data);
    std::vector<uint8_t> compress(const std::vector<uint8_t>& data);
    std::vector<uint8_t> decompress(const std::vector<uint8_t>& compressed_data, uint32_t uncompressed_size);
    
    // ZIP格式相关
    void writeLocalFileHeader(std::ofstream& file, const ZipFileEntry& entry);
    void writeCentralDirectoryHeader(std::ofstream& file, const ZipFileEntry& entry, uint32_t local_header_offset);
    void writeEndOfCentralDirectory(std::ofstream& file, uint32_t central_dir_size, uint32_t central_dir_offset, uint16_t entry_count);
    
    bool readLocalFileHeader(std::ifstream& file, ZipFileEntry& entry);
    bool readCentralDirectoryHeader(std::ifstream& file, ZipFileEntry& entry, uint32_t& local_header_offset);
    bool readEndOfCentralDirectory(std::ifstream& file, uint32_t& central_dir_offset, uint16_t& entry_count);
    
    // 时间转换
    uint16_t getCurrentDate();
    uint16_t getCurrentTime();
    
    // 路径处理
    std::string normalizePath(const std::string& path) const;
    bool createDirectories(const std::string& path);
    
    // 文件系统操作
    bool readFileToVector(const std::string& file_path, std::vector<uint8_t>& data);
    bool writeVectorToFile(const std::string& file_path, const std::vector<uint8_t>& data);
    
    // 错误处理
    void setError(const std::string& error);
};

// ZIP文件格式常量
namespace ZipConstants {
    const uint32_t LOCAL_FILE_HEADER_SIGNATURE = 0x04034b50;
    const uint32_t CENTRAL_DIRECTORY_SIGNATURE = 0x02014b50;
    const uint32_t END_OF_CENTRAL_DIR_SIGNATURE = 0x06054b50;
    
    const uint16_t VERSION_NEEDED = 20;
    const uint16_t VERSION_MADE_BY = 20;
    const uint16_t COMPRESSION_STORED = 0;
    const uint16_t COMPRESSION_DEFLATED = 8;
}

} // namespace chtl