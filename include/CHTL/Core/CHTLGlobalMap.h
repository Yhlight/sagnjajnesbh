#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <unordered_set>
#include "CHTL/Core/CHTLToken.h"

namespace CHTL {
namespace Core {

/**
 * @brief CHTL符号类型枚举
 */
enum class SymbolType {
    TEMPLATE_STYLE,         // 模板样式组
    TEMPLATE_ELEMENT,       // 模板元素
    TEMPLATE_VAR,           // 模板变量组
    CUSTOM_STYLE,           // 自定义样式组
    CUSTOM_ELEMENT,         // 自定义元素
    CUSTOM_VAR,             // 自定义变量组
    ORIGIN_HTML,            // 原始HTML嵌入
    ORIGIN_STYLE,           // 原始样式嵌入
    ORIGIN_JAVASCRIPT,      // 原始JavaScript嵌入
    ORIGIN_CUSTOM,          // 自定义类型原始嵌入
    NAMESPACE,              // 命名空间
    CONFIGURATION,          // 配置
    IMPORT,                 // 导入
    HTML_ELEMENT,           // HTML元素
    VARIABLE,               // 变量
    FUNCTION,               // 函数
    UNKNOWN                 // 未知类型
};

/**
 * @brief CHTL符号信息结构体
 */
struct SymbolInfo {
    SymbolType type;                    // 符号类型
    std::string name;                   // 符号名称
    std::string fullName;               // 全限定名称
    std::string namespaceName;          // 命名空间名称
    std::string fileName;               // 定义文件名
    int line;                           // 定义行号
    int column;                         // 定义列号
    std::vector<std::string> content;   // 符号内容
    std::unordered_map<std::string, std::string> properties;  // 属性映射
    std::vector<std::string> inherits;  // 继承关系
    bool isExported;                    // 是否导出
    
    SymbolInfo(SymbolType t = SymbolType::UNKNOWN, const std::string& n = "")
        : type(t), name(n), line(1), column(1), isExported(false) {}
};

/**
 * @brief 命名空间信息结构体
 */
struct NamespaceInfo {
    std::string name;                   // 命名空间名称
    std::string parentNamespace;        // 父命名空间
    std::vector<std::string> children;  // 子命名空间
    std::unordered_set<std::string> symbols;  // 包含的符号
    std::string fileName;               // 定义文件名
    int line;                           // 定义行号
    
    NamespaceInfo(const std::string& n = "") : name(n), line(1) {}
};

/**
 * @brief 配置信息结构体
 */
struct ConfigurationInfo {
    std::string name;                   // 配置名称
    std::unordered_map<std::string, std::string> settings;  // 配置设置
    std::unordered_map<std::string, std::vector<std::string>> groups;  // 配置组
    std::string fileName;               // 定义文件名
    int line;                           // 定义行号
    
    ConfigurationInfo(const std::string& n = "") : name(n), line(1) {}
};

/**
 * @brief CHTL全局映射表类
 * 
 * 管理CHTL编译过程中的所有全局符号，包括模板、自定义、命名空间、配置等
 */
class CHTLGlobalMap {
public:
    /**
     * @brief 构造函数
     */
    CHTLGlobalMap();
    
    /**
     * @brief 析构函数
     */
    ~CHTLGlobalMap() = default;
    
    /**
     * @brief 添加符号
     * @param symbol 符号信息
     * @return 添加是否成功
     */
    bool AddSymbol(const SymbolInfo& symbol);
    
    /**
     * @brief 查找符号
     * @param name 符号名称
     * @param namespaceName 命名空间名称（可选）
     * @return 符号信息指针，未找到返回nullptr
     */
    const SymbolInfo* FindSymbol(const std::string& name, 
                                 const std::string& namespaceName = "") const;
    
    /**
     * @brief 查找指定类型的符号
     * @param name 符号名称
     * @param type 符号类型
     * @param namespaceName 命名空间名称（可选）
     * @return 符号信息指针，未找到返回nullptr
     */
    const SymbolInfo* FindSymbolByType(const std::string& name, 
                                       SymbolType type,
                                       const std::string& namespaceName = "") const;
    
    /**
     * @brief 获取所有符号
     * @return 符号映射表的常量引用
     */
    const std::unordered_map<std::string, SymbolInfo>& GetAllSymbols() const {
        return symbols_;
    }
    
    /**
     * @brief 添加命名空间
     * @param namespaceInfo 命名空间信息
     * @return 添加是否成功
     */
    bool AddNamespace(const NamespaceInfo& namespaceInfo);
    
    /**
     * @brief 查找命名空间
     * @param name 命名空间名称
     * @return 命名空间信息指针，未找到返回nullptr
     */
    const NamespaceInfo* FindNamespace(const std::string& name) const;
    
    /**
     * @brief 获取当前命名空间
     * @return 当前命名空间名称
     */
    const std::string& GetCurrentNamespace() const { return currentNamespace_; }
    
    /**
     * @brief 设置当前命名空间
     * @param namespaceName 命名空间名称
     */
    void SetCurrentNamespace(const std::string& namespaceName);
    
    /**
     * @brief 进入命名空间
     * @param namespaceName 命名空间名称
     */
    void EnterNamespace(const std::string& namespaceName);
    
    /**
     * @brief 退出当前命名空间
     */
    void ExitNamespace();
    
    /**
     * @brief 添加配置
     * @param config 配置信息
     * @return 添加是否成功
     */
    bool AddConfiguration(const ConfigurationInfo& config);
    
    /**
     * @brief 查找配置
     * @param name 配置名称
     * @return 配置信息指针，未找到返回nullptr
     */
    const ConfigurationInfo* FindConfiguration(const std::string& name) const;
    
    /**
     * @brief 获取当前配置
     * @return 当前配置信息指针，未设置返回nullptr
     */
    const ConfigurationInfo* GetCurrentConfiguration() const;
    
    /**
     * @brief 设置当前配置
     * @param configName 配置名称
     */
    void SetCurrentConfiguration(const std::string& configName);
    
    /**
     * @brief 检查符号是否存在
     * @param name 符号名称
     * @param namespaceName 命名空间名称（可选）
     * @return 符号是否存在
     */
    bool HasSymbol(const std::string& name, const std::string& namespaceName = "") const;
    
    /**
     * @brief 检查命名冲突
     * @param name 符号名称
     * @param type 符号类型
     * @param namespaceName 命名空间名称（可选）
     * @return 是否存在冲突
     */
    bool HasConflict(const std::string& name, SymbolType type, 
                     const std::string& namespaceName = "") const;
    
    /**
     * @brief 解析全限定名称
     * @param fullName 全限定名称
     * @param outNamespace 输出命名空间名称
     * @param outSymbolName 输出符号名称
     * @return 解析是否成功
     */
    static bool ParseFullName(const std::string& fullName, 
                             std::string& outNamespace, 
                             std::string& outSymbolName);
    
    /**
     * @brief 构建全限定名称
     * @param namespaceName 命名空间名称
     * @param symbolName 符号名称
     * @return 全限定名称
     */
    static std::string BuildFullName(const std::string& namespaceName, 
                                    const std::string& symbolName);
    
    /**
     * @brief 获取符号类型名称
     * @param type 符号类型
     * @return 类型名称
     */
    static std::string GetSymbolTypeName(SymbolType type);
    
    /**
     * @brief 检查符号类型是否兼容
     * @param type1 类型1
     * @param type2 类型2
     * @return 是否兼容
     */
    static bool IsCompatibleType(SymbolType type1, SymbolType type2);
    
    /**
     * @brief 清空所有符号
     */
    void Clear();
    
    /**
     * @brief 导出符号到文件
     * @param fileName 文件名
     * @return 导出是否成功
     */
    bool ExportToFile(const std::string& fileName) const;
    
    /**
     * @brief 从文件导入符号
     * @param fileName 文件名
     * @return 导入是否成功
     */
    bool ImportFromFile(const std::string& fileName);
    
    /**
     * @brief 获取统计信息
     * @return 统计信息字符串
     */
    std::string GetStatistics() const;

private:
    std::unordered_map<std::string, SymbolInfo> symbols_;           // 符号映射表
    std::unordered_map<std::string, NamespaceInfo> namespaces_;     // 命名空间映射表
    std::unordered_map<std::string, ConfigurationInfo> configurations_;  // 配置映射表
    std::vector<std::string> namespaceStack_;                      // 命名空间栈
    std::string currentNamespace_;                                  // 当前命名空间
    std::string currentConfiguration_;                              // 当前配置
    
    /**
     * @brief 生成符号的完整键名
     * @param name 符号名称
     * @param namespaceName 命名空间名称
     * @return 完整键名
     */
    std::string GenerateKey(const std::string& name, const std::string& namespaceName) const;
    
    /**
     * @brief 检查符号名称是否有效
     * @param name 符号名称
     * @return 名称是否有效
     */
    bool IsValidSymbolName(const std::string& name) const;
    
    /**
     * @brief 检查命名空间名称是否有效
     * @param name 命名空间名称
     * @return 名称是否有效
     */
    bool IsValidNamespaceName(const std::string& name) const;
};

} // namespace Core
} // namespace CHTL