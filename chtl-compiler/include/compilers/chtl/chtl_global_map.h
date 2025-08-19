#ifndef CHTL_GLOBAL_MAP_H
#define CHTL_GLOBAL_MAP_H

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <optional>
#include "compilers/chtl/chtl_ast.h"

namespace chtl {

/**
 * 符号类型
 */
enum class SymbolType {
    TEMPLATE_STYLE,     // @Style模板
    TEMPLATE_ELEMENT,   // @Element模板
    TEMPLATE_VAR,       // @Var模板
    CUSTOM_STYLE,       // 自定义样式组
    CUSTOM_ELEMENT,     // 自定义元素
    CUSTOM_VAR,         // 自定义变量组
    NAMESPACE,          // 命名空间
    ORIGIN_BLOCK,       // 原始嵌入块
    CONFIGURATION,      // 配置
    IMPORT              // 导入
};

/**
 * 符号信息
 */
struct Symbol {
    std::string name;
    SymbolType type;
    std::shared_ptr<CHTLASTNode> node;  // 关联的AST节点
    std::string namespace_;              // 所属命名空间
    std::string sourceFile;              // 来源文件
    size_t defineLine;                   // 定义行号
    size_t defineColumn;                 // 定义列号
    
    // 额外信息
    std::unordered_map<std::string, std::string> metadata;
    
    Symbol(const std::string& name, SymbolType type, 
           std::shared_ptr<CHTLASTNode> node = nullptr)
        : name(name), type(type), node(node), defineLine(0), defineColumn(0) {}
};

/**
 * 命名空间信息
 */
struct NamespaceInfo {
    std::string name;
    std::string parentNamespace;  // 父命名空间（支持嵌套）
    std::vector<std::string> childNamespaces;
    std::unordered_map<std::string, std::shared_ptr<Symbol>> symbols;
    
    NamespaceInfo(const std::string& name, const std::string& parent = "")
        : name(name), parentNamespace(parent) {}
};

/**
 * CHTL全局符号表
 */
class CHTLGlobalMap {
public:
    CHTLGlobalMap();
    ~CHTLGlobalMap();
    
    /**
     * 注册符号
     */
    bool registerSymbol(const std::string& name, SymbolType type,
                       std::shared_ptr<CHTLASTNode> node = nullptr,
                       const std::string& namespace_ = "");
    
    /**
     * 查找符号
     * @param name 符号名
     * @param namespace_ 命名空间（可选）
     * @param searchParent 是否搜索父命名空间
     */
    std::shared_ptr<Symbol> lookupSymbol(const std::string& name,
                                        const std::string& namespace_ = "",
                                        bool searchParent = true) const;
    
    /**
     * 查找特定类型的符号
     */
    std::vector<std::shared_ptr<Symbol>> lookupSymbolsByType(SymbolType type,
                                                            const std::string& namespace_ = "") const;
    
    /**
     * 创建命名空间
     */
    bool createNamespace(const std::string& name, const std::string& parent = "");
    
    /**
     * 获取命名空间信息
     */
    std::shared_ptr<NamespaceInfo> getNamespace(const std::string& name) const;
    
    /**
     * 进入命名空间
     */
    void enterNamespace(const std::string& name);
    
    /**
     * 退出当前命名空间
     */
    void exitNamespace();
    
    /**
     * 获取当前命名空间
     */
    std::string getCurrentNamespace() const { return currentNamespace_; }
    
    /**
     * 检查符号是否存在
     */
    bool symbolExists(const std::string& name, const std::string& namespace_ = "") const;
    
    /**
     * 获取完整的符号名（包含命名空间）
     */
    std::string getFullyQualifiedName(const std::string& name, 
                                     const std::string& namespace_) const;
    
    /**
     * 解析符号引用（处理from语法）
     * 如：@Element Box from space.room2
     */
    std::shared_ptr<Symbol> resolveSymbolReference(const std::string& name,
                                                   const std::string& fromNamespace) const;
    
    /**
     * 导入符号
     */
    bool importSymbol(const std::string& name, const std::string& fromNamespace,
                     const std::string& asName = "", const std::string& toNamespace = "");
    
    /**
     * 清空符号表
     */
    void clear();
    
    /**
     * 获取所有符号（用于调试）
     */
    std::vector<std::shared_ptr<Symbol>> getAllSymbols() const;
    
    /**
     * 获取配置信息
     */
    std::unordered_map<std::string, std::string> getConfiguration() const;
    
    /**
     * 设置配置项
     */
    void setConfiguration(const std::string& key, const std::string& value);
    
private:
    // 全局符号表（平坦结构，使用完全限定名）
    std::unordered_map<std::string, std::shared_ptr<Symbol>> globalSymbols_;
    
    // 命名空间表
    std::unordered_map<std::string, std::shared_ptr<NamespaceInfo>> namespaces_;
    
    // 当前命名空间栈
    std::vector<std::string> namespaceStack_;
    std::string currentNamespace_;
    
    // 配置信息
    std::unordered_map<std::string, std::string> configuration_;
    
    // 辅助方法
    std::string makeQualifiedName(const std::string& name, const std::string& namespace_) const;
    std::vector<std::string> parseNamespacePath(const std::string& path) const;
};

} // namespace chtl

#endif // CHTL_GLOBAL_MAP_H