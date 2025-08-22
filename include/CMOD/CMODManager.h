#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace CHTL {
namespace CMOD {

/**
 * @brief 模块信息结构
 */
struct ModuleInfo {
    std::string name;           // 模块名称
    std::string version;        // 版本号
    std::string description;    // 描述
    std::string author;         // 作者
    std::string license;        // 许可证
    std::string dependencies;   // 依赖
    std::string category;       // 分类
    std::string minCHTLVersion; // 最小CHTL版本
    std::string maxCHTLVersion; // 最大CHTL版本
    
    ModuleInfo() = default;
};

/**
 * @brief 模块导出信息
 */
struct ModuleExports {
    std::vector<std::string> customStyles;      // 自定义样式
    std::vector<std::string> customElements;    // 自定义元素
    std::vector<std::string> customVars;        // 自定义变量
    std::vector<std::string> templateStyles;    // 模板样式
    std::vector<std::string> templateElements;  // 模板元素
    std::vector<std::string> templateVars;      // 模板变量
    std::vector<std::string> originHtml;        // 原始HTML
    std::vector<std::string> originStyle;       // 原始样式
    std::vector<std::string> originJavascript;  // 原始JavaScript
    std::vector<std::string> configurations;    // 配置
    
    ModuleExports() = default;
};

/**
 * @brief CMOD模块结构
 */
struct CMODModule {
    ModuleInfo info;                                    // 模块信息
    ModuleExports exports;                              // 导出信息
    std::string mainFilePath;                           // 主文件路径
    std::vector<std::string> sourceFiles;               // 源文件列表
    std::unordered_map<std::string, CMODModule> subModules; // 子模块
    
    CMODModule() = default;
};

/**
 * @brief CMOD管理器类
 * 
 * 负责CMOD模块的加载、解析、管理和导入
 */
class CMODManager {
public:
    /**
     * @brief 构造函数
     */
    CMODManager();
    
    /**
     * @brief 析构函数
     */
    ~CMODManager() = default;
    
    /**
     * @brief 加载CMOD模块
     * @param modulePath 模块路径
     * @return 是否成功加载
     */
    bool LoadModule(const std::string& modulePath);
    
    /**
     * @brief 解析CMOD文件夹结构
     * @param folderPath 文件夹路径
     * @return 解析的模块
     */
    std::unique_ptr<CMODModule> ParseModuleFolder(const std::string& folderPath);
    
    /**
     * @brief 解析模块信息文件
     * @param infoFilePath info文件路径
     * @return 模块信息
     */
    ModuleInfo ParseModuleInfo(const std::string& infoFilePath);
    
    /**
     * @brief 解析模块导出文件
     * @param infoFilePath info文件路径
     * @return 导出信息
     */
    ModuleExports ParseModuleExports(const std::string& infoFilePath);
    
    /**
     * @brief 查找模块
     * @param moduleName 模块名称
     * @return 模块指针，如果不存在返回nullptr
     */
    const CMODModule* FindModule(const std::string& moduleName) const;
    
    /**
     * @brief 查找子模块
     * @param moduleName 模块名称（支持点分割：Module.SubModule）
     * @return 子模块指针，如果不存在返回nullptr
     */
    const CMODModule* FindSubModule(const std::string& moduleName) const;
    
    /**
     * @brief 导入模块
     * @param moduleName 模块名称
     * @param alias 别名（可选）
     * @return 是否成功导入
     */
    bool ImportModule(const std::string& moduleName, const std::string& alias = "");
    
    /**
     * @brief 获取模块的源文件内容
     * @param moduleName 模块名称
     * @return 源文件内容列表
     */
    std::vector<std::string> GetModuleSourceContent(const std::string& moduleName) const;
    
    /**
     * @brief 验证模块依赖
     * @param module 模块
     * @return 是否满足依赖要求
     */
    bool ValidateModuleDependencies(const CMODModule& module) const;
    
    /**
     * @brief 获取所有已加载的模块
     * @return 模块名称列表
     */
    std::vector<std::string> GetLoadedModules() const;
    
    /**
     * @brief 清除所有模块
     */
    void ClearModules();
    
    /**
     * @brief 设置模块搜索路径
     * @param paths 搜索路径列表
     */
    void SetModuleSearchPaths(const std::vector<std::string>& paths);
    
    /**
     * @brief 添加模块搜索路径
     * @param path 搜索路径
     */
    void AddModuleSearchPath(const std::string& path);
    
    /**
     * @brief 自动生成模块导出信息
     * @param modulePath 模块路径
     * @return 自动生成的导出信息
     */
    ModuleExports GenerateAutoExports(const std::string& modulePath);
    
    /**
     * @brief 更新模块的导出信息文件
     * @param modulePath 模块路径
     * @param exports 导出信息
     * @return 是否成功更新
     */
    bool UpdateModuleExports(const std::string& modulePath, const ModuleExports& exports);
    
    /**
     * @brief 扫描源文件获取符号
     * @param sourceContent 源文件内容
     * @return 扫描到的导出信息
     */
    ModuleExports ScanSourceForSymbols(const std::string& sourceContent);

private:
    /**
     * @brief 解析Info标记块
     * @param content 文件内容
     * @return 模块信息
     */
    ModuleInfo ParseInfoBlock(const std::string& content);
    
    /**
     * @brief 解析Export标记块
     * @param content 文件内容
     * @return 导出信息
     */
    ModuleExports ParseExportBlock(const std::string& content);
    
    /**
     * @brief 检查模块版本兼容性
     * @param moduleVersion 模块版本
     * @param minVersion 最小版本
     * @param maxVersion 最大版本
     * @return 是否兼容
     */
    bool CheckVersionCompatibility(const std::string& moduleVersion,
                                  const std::string& minVersion,
                                  const std::string& maxVersion) const;
    
    /**
     * @brief 递归解析子模块
     * @param parentPath 父模块路径
     * @param parentModule 父模块
     */
    void ParseSubModules(const std::string& parentPath, CMODModule& parentModule);

private:
    std::unordered_map<std::string, std::unique_ptr<CMODModule>> modules_;  // 已加载的模块
    std::vector<std::string> searchPaths_;                                  // 模块搜索路径
    std::unordered_map<std::string, std::string> aliases_;                  // 模块别名映射
};

} // namespace CMOD
} // namespace CHTL