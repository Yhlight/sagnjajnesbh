#include "SimpleZip.h"
#include <iostream>
#include <filesystem>
#include <chrono>
#include <algorithm>
#include <cstring>

namespace chtl {

// CRC32表
static const uint32_t crc32_table[256] = {
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
    0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
    0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
    0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
    0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
    0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
    0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
    0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
    0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
    0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
    0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
    0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
    0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
    0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
    0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
    0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
    0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
    0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
    0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
    0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
    0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
    0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
    0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
    0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
    0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
    0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
    0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
    0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
    0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
    0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
    0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
    0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
    0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
    0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
    0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
    0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
    0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
    0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
    0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
    0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
    0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
    0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
    0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

SimpleZip::SimpleZip() = default;

SimpleZip::~SimpleZip() = default;

bool SimpleZip::addFile(const std::string& filename, const std::vector<uint8_t>& data) {
    auto entry = std::make_unique<ZipFileEntry>();
    entry->filename = normalizePath(filename);
    entry->data = data;
    entry->uncompressed_size = static_cast<uint32_t>(data.size());
    entry->crc32 = calculateCRC32(data);
    
    // 简单实现：不压缩，直接存储
    entry->compression_method = ZipConstants::COMPRESSION_STORED;
    entry->compressed_size = entry->uncompressed_size;
    
    entry->last_modified_date = getCurrentDate();
    entry->last_modified_time = getCurrentTime();
    
    entries_[entry->filename] = std::move(entry);
    return true;
}

bool SimpleZip::addFile(const std::string& filename, const std::string& content) {
    std::vector<uint8_t> data(content.begin(), content.end());
    return addFile(filename, data);
}

bool SimpleZip::addFileFromPath(const std::string& zip_path, const std::string& file_path) {
    std::vector<uint8_t> data;
    if (!readFileToVector(file_path, data)) {
        setError("无法读取文件: " + file_path);
        return false;
    }
    return addFile(zip_path, data);
}

bool SimpleZip::addDirectory(const std::string& zip_dir_path, const std::string& local_dir_path) {
    try {
        std::filesystem::path base_path(local_dir_path);
        if (!std::filesystem::exists(base_path) || !std::filesystem::is_directory(base_path)) {
            setError("目录不存在: " + local_dir_path);
            return false;
        }
        
        for (const auto& entry : std::filesystem::recursive_directory_iterator(base_path)) {
            if (entry.is_regular_file()) {
                std::filesystem::path relative_path = std::filesystem::relative(entry.path(), base_path);
                std::string zip_file_path = zip_dir_path + "/" + relative_path.string();
                
                if (!addFileFromPath(zip_file_path, entry.path().string())) {
                    return false;
                }
            }
        }
        return true;
    } catch (const std::exception& e) {
        setError("添加目录时发生错误: " + std::string(e.what()));
        return false;
    }
}

bool SimpleZip::saveToFile(const std::string& zip_file_path) {
    std::ofstream file(zip_file_path, std::ios::binary);
    if (!file.is_open()) {
        setError("无法创建ZIP文件: " + zip_file_path);
        return false;
    }
    
    std::vector<uint32_t> local_header_offsets;
    
    // 写入本地文件头和数据
    for (const auto& [filename, entry] : entries_) {
        local_header_offsets.push_back(static_cast<uint32_t>(file.tellp()));
        writeLocalFileHeader(file, *entry);
        file.write(reinterpret_cast<const char*>(entry->data.data()), entry->data.size());
    }
    
    // 写入中央目录
    uint32_t central_dir_offset = static_cast<uint32_t>(file.tellp());
    size_t i = 0;
    for (const auto& [filename, entry] : entries_) {
        writeCentralDirectoryHeader(file, *entry, local_header_offsets[i++]);
    }
    
    uint32_t central_dir_size = static_cast<uint32_t>(file.tellp()) - central_dir_offset;
    
    // 写入中央目录结束记录
    writeEndOfCentralDirectory(file, central_dir_size, central_dir_offset, 
                              static_cast<uint16_t>(entries_.size()));
    
    file.close();
    return true;
}

bool SimpleZip::loadFromFile(const std::string& zip_file_path) {
    std::ifstream file(zip_file_path, std::ios::binary);
    if (!file.is_open()) {
        setError("无法打开ZIP文件: " + zip_file_path);
        return false;
    }
    
    clear();
    
    // 读取中央目录结束记录
    uint32_t central_dir_offset;
    uint16_t entry_count;
    if (!readEndOfCentralDirectory(file, central_dir_offset, entry_count)) {
        return false;
    }
    
    // 读取中央目录
    file.seekg(central_dir_offset);
    for (uint16_t i = 0; i < entry_count; ++i) {
        auto entry = std::make_unique<ZipFileEntry>();
        uint32_t local_header_offset;
        
        if (!readCentralDirectoryHeader(file, *entry, local_header_offset)) {
            return false;
        }
        
        // 读取文件数据
        std::streampos current_pos = file.tellg();
        file.seekg(local_header_offset);
        
        ZipFileEntry temp_entry;
        if (!readLocalFileHeader(file, temp_entry)) {
            return false;
        }
        
        entry->data.resize(entry->compressed_size);
        file.read(reinterpret_cast<char*>(entry->data.data()), entry->compressed_size);
        
        entries_[entry->filename] = std::move(entry);
        file.seekg(current_pos);
    }
    
    file.close();
    return true;
}

bool SimpleZip::extractFile(const std::string& filename, std::vector<uint8_t>& data) {
    auto it = entries_.find(normalizePath(filename));
    if (it == entries_.end()) {
        setError("文件不存在: " + filename);
        return false;
    }
    
    data = it->second->data;
    return true;
}

bool SimpleZip::extractFile(const std::string& filename, std::string& content) {
    std::vector<uint8_t> data;
    if (!extractFile(filename, data)) {
        return false;
    }
    
    content = std::string(data.begin(), data.end());
    return true;
}

bool SimpleZip::extractToPath(const std::string& filename, const std::string& output_path) {
    std::vector<uint8_t> data;
    if (!extractFile(filename, data)) {
        return false;
    }
    
    // 创建目录
    std::filesystem::path file_path(output_path);
    std::filesystem::path dir_path = file_path.parent_path();
    if (!dir_path.empty() && !createDirectories(dir_path.string())) {
        return false;
    }
    
    return writeVectorToFile(output_path, data);
}

bool SimpleZip::extractAll(const std::string& output_directory) {
    if (!createDirectories(output_directory)) {
        return false;
    }
    
    for (const auto& [filename, entry] : entries_) {
        std::string output_path = output_directory + "/" + filename;
        if (!extractToPath(filename, output_path)) {
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> SimpleZip::getFileList() const {
    std::vector<std::string> files;
    files.reserve(entries_.size());
    
    for (const auto& [filename, entry] : entries_) {
        files.push_back(filename);
    }
    
    return files;
}

bool SimpleZip::hasFile(const std::string& filename) const {
    return entries_.find(normalizePath(filename)) != entries_.end();
}

bool SimpleZip::getFileInfo(const std::string& filename, ZipFileEntry& info) const {
    auto it = entries_.find(normalizePath(filename));
    if (it == entries_.end()) {
        return false;
    }
    
    info = *it->second;
    return true;
}

void SimpleZip::clear() {
    entries_.clear();
    last_error_.clear();
}

uint32_t SimpleZip::calculateCRC32(const std::vector<uint8_t>& data) {
    uint32_t crc = 0xFFFFFFFF;
    for (uint8_t byte : data) {
        crc = crc32_table[(crc ^ byte) & 0xFF] ^ (crc >> 8);
    }
    return crc ^ 0xFFFFFFFF;
}

std::vector<uint8_t> SimpleZip::compress(const std::vector<uint8_t>& data) {
    // 简单实现：不压缩，直接返回原数据
    return data;
}

std::vector<uint8_t> SimpleZip::decompress(const std::vector<uint8_t>& compressed_data, uint32_t /*uncompressed_size*/) {
    // 简单实现：假设数据未压缩
    return compressed_data;
}

void SimpleZip::writeLocalFileHeader(std::ofstream& file, const ZipFileEntry& entry) {
    // 本地文件头签名
    uint32_t signature = ZipConstants::LOCAL_FILE_HEADER_SIGNATURE;
    file.write(reinterpret_cast<const char*>(&signature), 4);
    
    // 解压缩所需版本
    uint16_t version = ZipConstants::VERSION_NEEDED;
    file.write(reinterpret_cast<const char*>(&version), 2);
    
    // 通用位标记
    uint16_t flags = 0;
    file.write(reinterpret_cast<const char*>(&flags), 2);
    
    // 压缩方法
    file.write(reinterpret_cast<const char*>(&entry.compression_method), 2);
    
    // 最后修改时间和日期
    file.write(reinterpret_cast<const char*>(&entry.last_modified_time), 2);
    file.write(reinterpret_cast<const char*>(&entry.last_modified_date), 2);
    
    // CRC-32
    file.write(reinterpret_cast<const char*>(&entry.crc32), 4);
    
    // 压缩后大小
    file.write(reinterpret_cast<const char*>(&entry.compressed_size), 4);
    
    // 未压缩大小
    file.write(reinterpret_cast<const char*>(&entry.uncompressed_size), 4);
    
    // 文件名长度
    uint16_t filename_length = static_cast<uint16_t>(entry.filename.length());
    file.write(reinterpret_cast<const char*>(&filename_length), 2);
    
    // 扩展字段长度
    uint16_t extra_length = 0;
    file.write(reinterpret_cast<const char*>(&extra_length), 2);
    
    // 文件名
    file.write(entry.filename.c_str(), filename_length);
}

void SimpleZip::writeCentralDirectoryHeader(std::ofstream& file, const ZipFileEntry& entry, uint32_t local_header_offset) {
    // 中央目录文件头签名
    uint32_t signature = ZipConstants::CENTRAL_DIRECTORY_SIGNATURE;
    file.write(reinterpret_cast<const char*>(&signature), 4);
    
    // 压缩使用的版本
    uint16_t version_made_by = ZipConstants::VERSION_MADE_BY;
    file.write(reinterpret_cast<const char*>(&version_made_by), 2);
    
    // 解压缩所需版本
    uint16_t version_needed = ZipConstants::VERSION_NEEDED;
    file.write(reinterpret_cast<const char*>(&version_needed), 2);
    
    // 通用位标记
    uint16_t flags = 0;
    file.write(reinterpret_cast<const char*>(&flags), 2);
    
    // 压缩方法
    file.write(reinterpret_cast<const char*>(&entry.compression_method), 2);
    
    // 最后修改时间和日期
    file.write(reinterpret_cast<const char*>(&entry.last_modified_time), 2);
    file.write(reinterpret_cast<const char*>(&entry.last_modified_date), 2);
    
    // CRC-32
    file.write(reinterpret_cast<const char*>(&entry.crc32), 4);
    
    // 压缩后大小
    file.write(reinterpret_cast<const char*>(&entry.compressed_size), 4);
    
    // 未压缩大小
    file.write(reinterpret_cast<const char*>(&entry.uncompressed_size), 4);
    
    // 文件名长度
    uint16_t filename_length = static_cast<uint16_t>(entry.filename.length());
    file.write(reinterpret_cast<const char*>(&filename_length), 2);
    
    // 扩展字段长度
    uint16_t extra_length = 0;
    file.write(reinterpret_cast<const char*>(&extra_length), 2);
    
    // 文件注释长度
    uint16_t comment_length = 0;
    file.write(reinterpret_cast<const char*>(&comment_length), 2);
    
    // 文件开始的磁盘编号
    uint16_t disk_number = 0;
    file.write(reinterpret_cast<const char*>(&disk_number), 2);
    
    // 内部文件属性
    uint16_t internal_attributes = 0;
    file.write(reinterpret_cast<const char*>(&internal_attributes), 2);
    
    // 外部文件属性
    uint32_t external_attributes = 0;
    file.write(reinterpret_cast<const char*>(&external_attributes), 4);
    
    // 本地文件头偏移量
    file.write(reinterpret_cast<const char*>(&local_header_offset), 4);
    
    // 文件名
    file.write(entry.filename.c_str(), filename_length);
}

void SimpleZip::writeEndOfCentralDirectory(std::ofstream& file, uint32_t central_dir_size, 
                                          uint32_t central_dir_offset, uint16_t entry_count) {
    // 中央目录结束记录签名
    uint32_t signature = ZipConstants::END_OF_CENTRAL_DIR_SIGNATURE;
    file.write(reinterpret_cast<const char*>(&signature), 4);
    
    // 当前磁盘编号
    uint16_t disk_number = 0;
    file.write(reinterpret_cast<const char*>(&disk_number), 2);
    
    // 中央目录开始磁盘编号
    uint16_t central_dir_disk = 0;
    file.write(reinterpret_cast<const char*>(&central_dir_disk), 2);
    
    // 当前磁盘上的中央目录记录数
    file.write(reinterpret_cast<const char*>(&entry_count), 2);
    
    // 中央目录记录总数
    file.write(reinterpret_cast<const char*>(&entry_count), 2);
    
    // 中央目录大小
    file.write(reinterpret_cast<const char*>(&central_dir_size), 4);
    
    // 中央目录偏移量
    file.write(reinterpret_cast<const char*>(&central_dir_offset), 4);
    
    // ZIP文件注释长度
    uint16_t comment_length = 0;
    file.write(reinterpret_cast<const char*>(&comment_length), 2);
}

bool SimpleZip::readLocalFileHeader(std::ifstream& file, ZipFileEntry& entry) {
    uint32_t signature;
    file.read(reinterpret_cast<char*>(&signature), 4);
    if (signature != ZipConstants::LOCAL_FILE_HEADER_SIGNATURE) {
        setError("无效的本地文件头签名");
        return false;
    }
    
    uint16_t version, flags;
    file.read(reinterpret_cast<char*>(&version), 2);
    file.read(reinterpret_cast<char*>(&flags), 2);
    file.read(reinterpret_cast<char*>(&entry.compression_method), 2);
    file.read(reinterpret_cast<char*>(&entry.last_modified_time), 2);
    file.read(reinterpret_cast<char*>(&entry.last_modified_date), 2);
    file.read(reinterpret_cast<char*>(&entry.crc32), 4);
    file.read(reinterpret_cast<char*>(&entry.compressed_size), 4);
    file.read(reinterpret_cast<char*>(&entry.uncompressed_size), 4);
    
    uint16_t filename_length, extra_length;
    file.read(reinterpret_cast<char*>(&filename_length), 2);
    file.read(reinterpret_cast<char*>(&extra_length), 2);
    
    // 读取文件名
    std::vector<char> filename_buffer(filename_length);
    file.read(filename_buffer.data(), filename_length);
    entry.filename = std::string(filename_buffer.begin(), filename_buffer.end());
    
    // 跳过扩展字段
    file.seekg(extra_length, std::ios::cur);
    
    return true;
}

bool SimpleZip::readCentralDirectoryHeader(std::ifstream& file, ZipFileEntry& entry, uint32_t& local_header_offset) {
    uint32_t signature;
    file.read(reinterpret_cast<char*>(&signature), 4);
    if (signature != ZipConstants::CENTRAL_DIRECTORY_SIGNATURE) {
        setError("无效的中央目录头签名");
        return false;
    }
    
    uint16_t version_made_by, version_needed, flags;
    file.read(reinterpret_cast<char*>(&version_made_by), 2);
    file.read(reinterpret_cast<char*>(&version_needed), 2);
    file.read(reinterpret_cast<char*>(&flags), 2);
    file.read(reinterpret_cast<char*>(&entry.compression_method), 2);
    file.read(reinterpret_cast<char*>(&entry.last_modified_time), 2);
    file.read(reinterpret_cast<char*>(&entry.last_modified_date), 2);
    file.read(reinterpret_cast<char*>(&entry.crc32), 4);
    file.read(reinterpret_cast<char*>(&entry.compressed_size), 4);
    file.read(reinterpret_cast<char*>(&entry.uncompressed_size), 4);
    
    uint16_t filename_length, extra_length, comment_length;
    file.read(reinterpret_cast<char*>(&filename_length), 2);
    file.read(reinterpret_cast<char*>(&extra_length), 2);
    file.read(reinterpret_cast<char*>(&comment_length), 2);
    
    uint16_t disk_number, internal_attributes;
    uint32_t external_attributes;
    file.read(reinterpret_cast<char*>(&disk_number), 2);
    file.read(reinterpret_cast<char*>(&internal_attributes), 2);
    file.read(reinterpret_cast<char*>(&external_attributes), 4);
    file.read(reinterpret_cast<char*>(&local_header_offset), 4);
    
    // 读取文件名
    std::vector<char> filename_buffer(filename_length);
    file.read(filename_buffer.data(), filename_length);
    entry.filename = std::string(filename_buffer.begin(), filename_buffer.end());
    
    // 跳过扩展字段和注释
    file.seekg(extra_length + comment_length, std::ios::cur);
    
    return true;
}

bool SimpleZip::readEndOfCentralDirectory(std::ifstream& file, uint32_t& central_dir_offset, uint16_t& entry_count) {
    // 从文件末尾开始查找中央目录结束记录
    file.seekg(0, std::ios::end);
    std::streamsize file_size = file.tellg();
    
    // 最小的中央目录结束记录大小是22字节
    const int min_eocd_size = 22;
    const int max_comment_size = 65535;
    
    int search_start = std::max(0, static_cast<int>(file_size) - min_eocd_size - max_comment_size);
    file.seekg(search_start);
    
    std::vector<char> buffer(static_cast<size_t>(file_size - search_start));
    file.read(buffer.data(), buffer.size());
    
    // 查找签名
    for (int i = static_cast<int>(buffer.size()) - min_eocd_size; i >= 0; --i) {
        uint32_t signature = *reinterpret_cast<const uint32_t*>(&buffer[i]);
        if (signature == ZipConstants::END_OF_CENTRAL_DIR_SIGNATURE) {
            // 找到了，解析记录
            file.seekg(search_start + i);
            
            uint32_t sig;
            uint16_t disk_number, central_dir_disk, entries_on_disk;
            uint32_t central_dir_size;
            uint16_t comment_length;
            
            file.read(reinterpret_cast<char*>(&sig), 4);
            file.read(reinterpret_cast<char*>(&disk_number), 2);
            file.read(reinterpret_cast<char*>(&central_dir_disk), 2);
            file.read(reinterpret_cast<char*>(&entries_on_disk), 2);
            file.read(reinterpret_cast<char*>(&entry_count), 2);
            file.read(reinterpret_cast<char*>(&central_dir_size), 4);
            file.read(reinterpret_cast<char*>(&central_dir_offset), 4);
            file.read(reinterpret_cast<char*>(&comment_length), 2);
            
            return true;
        }
    }
    
    setError("未找到中央目录结束记录");
    return false;
}

uint16_t SimpleZip::getCurrentDate() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::localtime(&time_t);
    
    uint16_t year = static_cast<uint16_t>(tm.tm_year + 1900 - 1980);
    uint16_t month = static_cast<uint16_t>(tm.tm_mon + 1);
    uint16_t day = static_cast<uint16_t>(tm.tm_mday);
    
    return (year << 9) | (month << 5) | day;
}

uint16_t SimpleZip::getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::localtime(&time_t);
    
    uint16_t hour = static_cast<uint16_t>(tm.tm_hour);
    uint16_t minute = static_cast<uint16_t>(tm.tm_min);
    uint16_t second = static_cast<uint16_t>(tm.tm_sec / 2);
    
    return (hour << 11) | (minute << 5) | second;
}

std::string SimpleZip::normalizePath(const std::string& path) const {
    std::string normalized = path;
    std::replace(normalized.begin(), normalized.end(), '\\', '/');
    return normalized;
}

bool SimpleZip::createDirectories(const std::string& path) {
    try {
        std::filesystem::create_directories(path);
        return true;
    } catch (const std::exception& e) {
        setError("创建目录失败: " + std::string(e.what()));
        return false;
    }
}

bool SimpleZip::readFileToVector(const std::string& file_path, std::vector<uint8_t>& data) {
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        setError("无法打开文件: " + file_path);
        return false;
    }
    
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    data.resize(size);
    file.read(reinterpret_cast<char*>(data.data()), size);
    
    return true;
}

bool SimpleZip::writeVectorToFile(const std::string& file_path, const std::vector<uint8_t>& data) {
    std::ofstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        setError("无法创建文件: " + file_path);
        return false;
    }
    
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    return true;
}

void SimpleZip::setError(const std::string& error) {
    last_error_ = error;
    std::cerr << "SimpleZip错误: " << error << std::endl;
}

} // namespace chtl