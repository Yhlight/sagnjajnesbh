#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <filesystem>

namespace chtl {
namespace chtljs {
namespace module {

// 前向声明
namespace ast {
    class CHTLJSASTNode;
}

/**
 * CJMOD扩展接口
 * 开发者通过实现这个接口来扩展CHTL JS语法
 */
class ICJMODExtension {
public:
    virtual ~ICJMODExtension() = default;
    
    /**
     * 获取扩展名称
     */
    virtual std::string GetName() const = 0;
    
    /**
     * 获取扩展版本
     */
    virtual std::string GetVersion() const = 0;
    
    /**
     * 获取扩展描述
     */
    virtual std::string GetDescription() const = 0;
    
    /**
     * 获取支持的关键字列表
     */
    virtual std::vector<std::string> GetKeywords() const = 0;
    
    /**
     * 语法分析
     * @param code 要分析的代码
     * @param ignoreChars 要忽略的字符集
     * @return 分析后的语法结构
     */
    virtual std::vector<std::string> SyntaxAnalysis(const std::string& code, 
                                                   const std::string& ignoreChars = "") = 0;
    
    /**
     * 扫描关键字
     * @param scanner 扫描器接口
     * @param keyword 要扫描的关键字
     * @param callback 扫描到关键字时的回调
     */
    virtual void ScanKeyword(class IScannerInterface* scanner,
                           const std::string& keyword,
                           std::function<void()> callback) = 0;
    
    /**
     * 转换为JavaScript代码
     * @param node AST节点
     * @return 生成的JavaScript代码
     */
    virtual std::string GenerateCode(ast::CHTLJSASTNode* node) = 0;
    
    /**
     * 初始化扩展
     * 在加载扩展时调用
     */
    virtual bool Initialize() = 0;
    
    /**
     * 清理扩展
     * 在卸载扩展时调用
     */
    virtual void Cleanup() = 0;
};

/**
 * 扫描器接口
 * 提供给CJMOD扩展使用的扫描器功能
 */
class IScannerInterface {
public:
    virtual ~IScannerInterface() = default;
    
    /**
     * 查看相对位置的关键字
     * @param offset 相对当前位置的偏移
     * @return 关键字内容
     */
    virtual std::string PeekKeyword(int offset) = 0;
    
    /**
     * 改变扫描策略
     */
    enum class ScanPolicy {
        NORMAL,     // 常规
        COLLECT,    // 收集
        SKIP        // 跳过
    };
    
    /**
     * 开始策略改变
     * @param trigger 触发字符
     * @param policy 新策略
     */
    virtual void PolicyChangeBegin(const std::string& trigger, ScanPolicy policy) = 0;
    
    /**
     * 结束策略改变
     * @param trigger 触发字符
     * @param policy 恢复策略
     * @return 收集的内容（仅在COLLECT策略下有效）
     */
    virtual std::string PolicyChangeEnd(const std::string& trigger, ScanPolicy policy) = 0;
    
    /**
     * 获取当前位置信息
     */
    virtual size_t GetCurrentLine() const = 0;
    virtual size_t GetCurrentColumn() const = 0;
    virtual std::string GetCurrentFile() const = 0;
};

/**
 * CJMOD模块信息
 */
struct CJMODInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    std::string minCHTLVersion;
    std::string maxCHTLVersion;
    
    // C++特定信息
    std::string entryPoint;      // 入口函数名
    std::string headerFiles;      // 头文件列表
    std::string linkLibraries;    // 链接库列表
    std::string compilerFlags;    // 编译标志
    
    bool ParseFromFile(const std::string& filePath);
    std::string Serialize() const;
};

/**
 * CJMOD模块
 * 表示一个CHTL JS扩展模块
 */
class CJMODModule {
public:
    CJMODModule(const std::string& name);
    ~CJMODModule();
    
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
    const CJMODInfo& GetInfo() const { return m_Info; }
    CJMODInfo& GetInfo() { return m_Info; }
    
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
     * 编译模块
     * @param outputPath 输出路径
     * @return 是否编译成功
     */
    bool Compile(const std::filesystem::path& outputPath) const;
    
    /**
     * 加载已编译的扩展
     * @param libraryPath 动态库路径
     * @return 扩展接口（失败返回nullptr）
     */
    std::unique_ptr<ICJMODExtension> LoadExtension(const std::string& libraryPath);
    
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
    CJMODInfo m_Info;
    
    // 源文件映射
    std::unordered_map<std::string, std::string> m_SourceFiles;
    
    // 子模块
    std::vector<std::unique_ptr<CJMODModule>> m_SubModules;
    
    // 验证错误
    mutable std::vector<std::string> m_ValidationErrors;
    
    // 动态库句柄
    void* m_LibraryHandle = nullptr;
    
    // 辅助方法
    bool LoadDirectory(const std::filesystem::path& dirPath);
    bool SaveDirectory(const std::filesystem::path& dirPath) const;
    bool CompileSourceFiles(const std::filesystem::path& outputPath) const;
    std::string GenerateMakefile() const;
};

/**
 * CJMOD扩展管理器
 * 管理所有已加载的CJMOD扩展
 */
class CJMODManager {
public:
    CJMODManager();
    ~CJMODManager();
    
    /**
     * 设置模块搜索路径
     */
    void AddSearchPath(const std::filesystem::path& path);
    void ClearSearchPaths();
    
    /**
     * 加载模块
     * @param moduleName 模块名称
     * @return 是否加载成功
     */
    bool LoadModule(const std::string& moduleName);
    
    /**
     * 获取已加载的扩展
     */
    ICJMODExtension* GetExtension(const std::string& name) const;
    
    /**
     * 获取所有扩展
     */
    std::vector<ICJMODExtension*> GetAllExtensions() const;
    
    /**
     * 根据关键字查找扩展
     */
    ICJMODExtension* FindExtensionByKeyword(const std::string& keyword) const;
    
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
    std::unordered_map<std::string, std::unique_ptr<CJMODModule>> m_Modules;
    
    // 已加载的扩展
    std::unordered_map<std::string, std::unique_ptr<ICJMODExtension>> m_Extensions;
    
    // 关键字到扩展的映射
    std::unordered_map<std::string, ICJMODExtension*> m_KeywordMap;
    
    // 错误信息
    std::vector<std::string> m_Errors;
    
    // 辅助方法
    std::filesystem::path FindModule(const std::string& moduleName) const;
    bool LoadFromDirectory(const std::filesystem::path& dirPath);
    bool LoadCompiledExtension(const std::string& libraryPath, const std::string& moduleName);
};

/**
 * CJMOD扩展基类
 * 提供默认实现，简化扩展开发
 */
class CJMODExtensionBase : public ICJMODExtension {
public:
    CJMODExtensionBase(const std::string& name,
                      const std::string& version,
                      const std::string& description);
    
    std::string GetName() const override { return m_Name; }
    std::string GetVersion() const override { return m_Version; }
    std::string GetDescription() const override { return m_Description; }
    
    bool Initialize() override { return true; }
    void Cleanup() override {}
    
protected:
    std::string m_Name;
    std::string m_Version;
    std::string m_Description;
    std::vector<std::string> m_Keywords;
    
    // 辅助方法
    std::vector<std::string> ParseArguments(const std::string& code, 
                                          const std::string& separator = ",");
    std::string EscapeJavaScript(const std::string& text);
};

/**
 * CJMOD导出宏
 * 用于导出扩展
 */
#define CJMOD_EXPORT extern "C"

/**
 * CJMOD扩展工厂函数类型
 */
using CJMODExtensionFactory = ICJMODExtension* (*)();

/**
 * CJMOD标准导出函数
 */
CJMOD_EXPORT ICJMODExtension* CreateCJMODExtension();
CJMOD_EXPORT void DestroyCJMODExtension(ICJMODExtension* extension);
CJMOD_EXPORT const char* GetCJMODVersion();

} // namespace module
} // namespace chtljs
} // namespace chtl