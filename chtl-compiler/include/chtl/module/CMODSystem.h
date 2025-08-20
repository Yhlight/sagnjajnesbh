#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <filesystem>

namespace chtl {
namespace module {

/**
 * CMOD模块信息
 * 对应info文件夹中的[Info]块
 */
struct CMODInfo {
    std::string name;                // 模块名称
    std::string version;             // 版本号
    std::string description;         // 描述
    std::string author;              // 作者
    std::string license;             // 许可证
    std::string dependencies;        // 依赖
    std::string category;            // 分类
    std::string minCHTLVersion;      // 最小CHTL版本
    std::string maxCHTLVersion;      // 最大CHTL版本
    
    // 从CHTL文件解析
    bool ParseFromFile(const std::string& filePath);
    
    // 序列化为CHTL格式
    std::string Serialize() const;
};

/**
 * CMOD导出信息
 * 对应info文件夹中的[Export]块
 */
struct CMODExport {
    std::vector<std::string> customStyles;
    std::vector<std::string> customElements;
    std::vector<std::string> customVars;
    std::vector<std::string> templateStyles;
    std::vector<std::string> templateElements;
    std::vector<std::string> templateVars;
    std::vector<std::string> originHtml;
    std::vector<std::string> originStyle;
    std::vector<std::string> originJavascript;
    std::vector<std::string> configurations;
    
    // 自定义类型的原始嵌入
    std::unordered_map<std::string, std::vector<std::string>> customOrigins;
    
    // 解析和序列化
    bool ParseFromFile(const std::string& filePath);
    std::string Serialize() const;
};

/**
 * CMOD模块
 * 表示一个完整的CMOD模块
 */
class CMODModule {
public:
    CMODModule(const std::string& name);
    ~CMODModule();
    
    /**
     * 加载模块
     * @param modulePath 模块根目录路径
     */
    bool Load(const std::filesystem::path& modulePath);
    
    /**
     * 保存模块
     * @param outputPath 输出目录路径
     */
    bool Save(const std::filesystem::path& outputPath) const;
    
    /**
     * 获取模块信息
     */
    const CMODInfo& GetInfo() const { return m_Info; }
    CMODInfo& GetInfo() { return m_Info; }
    
    /**
     * 获取导出信息
     */
    const CMODExport& GetExport() const { return m_Export; }
    CMODExport& GetExport() { return m_Export; }
    
    /**
     * 获取模块名称
     */
    const std::string& GetName() const { return m_Name; }
    
    /**
     * 获取源文件列表
     */
    std::vector<std::string> GetSourceFiles() const;
    
    /**
     * 添加源文件
     */
    void AddSourceFile(const std::string& relativePath, const std::string& content);
    
    /**
     * 获取源文件内容
     */
    std::string GetSourceFileContent(const std::string& relativePath) const;
    
    /**
     * 子模块管理
     */
    void AddSubModule(std::unique_ptr<CMODModule> subModule);
    std::vector<CMODModule*> GetSubModules() const;
    CMODModule* GetSubModule(const std::string& name) const;
    
    /**
     * 验证模块结构
     */
    bool ValidateStructure() const;
    
    /**
     * 获取验证错误
     */
    const std::vector<std::string>& GetValidationErrors() const { return m_ValidationErrors; }
    
private:
    std::string m_Name;
    CMODInfo m_Info;
    CMODExport m_Export;
    
    // 源文件映射（相对路径 -> 内容）
    std::unordered_map<std::string, std::string> m_SourceFiles;
    
    // 子模块
    std::vector<std::unique_ptr<CMODModule>> m_SubModules;
    
    // 验证错误
    mutable std::vector<std::string> m_ValidationErrors;
    
    // 辅助方法
    bool LoadDirectory(const std::filesystem::path& dirPath);
    bool SaveDirectory(const std::filesystem::path& dirPath) const;
    bool LoadInfoFile(const std::filesystem::path& infoPath);
    bool SaveInfoFile(const std::filesystem::path& infoPath) const;
};

/**
 * CMOD打包器
 * 负责将CMOD模块打包成.cmod文件
 */
class CMODPacker {
public:
    CMODPacker();
    ~CMODPacker();
    
    /**
     * 打包模块
     * @param module 要打包的模块
     * @param outputFile 输出的.cmod文件路径
     * @param compress 是否压缩
     */
    bool Pack(const CMODModule& module, const std::string& outputFile, bool compress = true);
    
    /**
     * 打包目录
     * @param moduleDir 模块目录
     * @param outputFile 输出的.cmod文件路径
     * @param compress 是否压缩
     */
    bool PackDirectory(const std::filesystem::path& moduleDir, 
                      const std::string& outputFile, 
                      bool compress = true);
    
    /**
     * 获取错误信息
     */
    const std::vector<std::string>& GetErrors() const { return m_Errors; }
    
private:
    std::vector<std::string> m_Errors;
    
    // 简单的归档格式（如果不使用外部库）
    struct ArchiveHeader {
        char magic[8];      // "CHTLCMOD"
        uint32_t version;   // 版本号
        uint32_t fileCount; // 文件数量
        uint32_t flags;     // 标志（是否压缩等）
    };
    
    struct FileEntry {
        uint32_t nameLength;    // 文件名长度
        uint32_t dataLength;    // 数据长度
        uint32_t originalSize;  // 原始大小（压缩前）
        uint32_t checksum;      // 校验和
    };
    
    bool WriteArchive(const CMODModule& module, std::ofstream& output, bool compress);
    std::vector<uint8_t> CompressData(const std::vector<uint8_t>& data);
    uint32_t CalculateChecksum(const std::vector<uint8_t>& data);
};

/**
 * CMOD解包器
 * 负责从.cmod文件解包出模块
 */
class CMODUnpacker {
public:
    CMODUnpacker();
    ~CMODUnpacker();
    
    /**
     * 解包文件
     * @param cmodFile .cmod文件路径
     * @param outputDir 输出目录
     */
    bool Unpack(const std::string& cmodFile, const std::filesystem::path& outputDir);
    
    /**
     * 解包到模块对象
     * @param cmodFile .cmod文件路径
     * @return 解包后的模块（失败返回nullptr）
     */
    std::unique_ptr<CMODModule> UnpackToModule(const std::string& cmodFile);
    
    /**
     * 获取错误信息
     */
    const std::vector<std::string>& GetErrors() const { return m_Errors; }
    
private:
    std::vector<std::string> m_Errors;
    
    bool ReadArchive(std::ifstream& input, CMODModule& module);
    std::vector<uint8_t> DecompressData(const std::vector<uint8_t>& data, size_t originalSize);
    bool VerifyChecksum(const std::vector<uint8_t>& data, uint32_t expectedChecksum);
};

/**
 * CMOD管理器
 * 管理所有已加载的CMOD模块
 */
class CMODManager {
public:
    CMODManager();
    ~CMODManager();
    
    /**
     * 设置模块搜索路径
     */
    void AddSearchPath(const std::filesystem::path& path);
    void ClearSearchPaths();
    
    /**
     * 加载模块
     * @param moduleName 模块名称（可以是路径）
     * @return 是否加载成功
     */
    bool LoadModule(const std::string& moduleName);
    
    /**
     * 获取已加载的模块
     */
    CMODModule* GetModule(const std::string& name) const;
    
    /**
     * 获取所有已加载的模块
     */
    std::vector<CMODModule*> GetAllModules() const;
    
    /**
     * 查找模块文件
     * @param moduleName 模块名称
     * @return 模块文件路径（.cmod或目录）
     */
    std::filesystem::path FindModule(const std::string& moduleName) const;
    
    /**
     * 卸载模块
     */
    void UnloadModule(const std::string& name);
    
    /**
     * 清空所有模块
     */
    void Clear();
    
    /**
     * 获取错误信息
     */
    const std::vector<std::string>& GetErrors() const { return m_Errors; }
    void ClearErrors() { m_Errors.clear(); }
    
private:
    // 搜索路径
    std::vector<std::filesystem::path> m_SearchPaths;
    
    // 已加载的模块
    std::unordered_map<std::string, std::unique_ptr<CMODModule>> m_Modules;
    
    // 错误信息
    std::vector<std::string> m_Errors;
    
    // 辅助方法
    bool LoadFromDirectory(const std::filesystem::path& dirPath);
    bool LoadFromCMODFile(const std::filesystem::path& cmodFile);
};

} // namespace module
} // namespace chtl