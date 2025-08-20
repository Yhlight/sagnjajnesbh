#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace chtl {

// 前向声明
namespace ast {
    class NamespaceNode;
    class ASTNode;
}

namespace ns {

/**
 * 命名空间符号类型
 */
enum class SymbolType {
    TEMPLATE_ELEMENT,   // 模板元素
    TEMPLATE_STYLE,     // 模板样式组
    TEMPLATE_VAR,       // 模板变量组
    CUSTOM_ELEMENT,     // 自定义元素
    CUSTOM_STYLE,       // 自定义样式组
    CUSTOM_VAR,         // 自定义变量组
    ORIGIN_HTML,        // 原始HTML嵌入
    ORIGIN_STYLE,       // 原始Style嵌入
    ORIGIN_JAVASCRIPT,  // 原始JavaScript嵌入
    NAMESPACE          // 嵌套命名空间
};

/**
 * 命名空间符号信息
 */
struct SymbolInfo {
    std::string name;           // 符号名称
    SymbolType type;           // 符号类型
    std::string fullName;      // 完全限定名（包含命名空间）
    std::shared_ptr<ast::ASTNode> node;  // 关联的AST节点
    
    // 位置信息
    std::string sourceFile;
    size_t line;
    size_t column;
    
    // 额外信息
    bool isExported = true;    // 是否可被外部访问
    std::string description;   // 描述信息
};

/**
 * 命名空间定义
 */
class Namespace {
public:
    Namespace(const std::string& name, Namespace* parent = nullptr);
    ~Namespace();
    
    /**
     * 获取命名空间名称
     */
    const std::string& GetName() const { return m_Name; }
    
    /**
     * 获取完全限定名
     */
    std::string GetFullName() const;
    
    /**
     * 获取父命名空间
     */
    Namespace* GetParent() const { return m_Parent; }
    
    /**
     * 添加符号
     * @return 是否添加成功（false表示有冲突）
     */
    bool AddSymbol(const SymbolInfo& symbol);
    
    /**
     * 查找符号
     * @param name 符号名称
     * @param type 符号类型（可选，不指定则查找所有类型）
     */
    SymbolInfo* FindSymbol(const std::string& name, 
                          SymbolType type = SymbolType::NAMESPACE);
    const SymbolInfo* FindSymbol(const std::string& name,
                                SymbolType type = SymbolType::NAMESPACE) const;
    
    /**
     * 获取所有符号
     */
    std::vector<SymbolInfo> GetAllSymbols() const;
    
    /**
     * 获取指定类型的符号
     */
    std::vector<SymbolInfo> GetSymbolsByType(SymbolType type) const;
    
    /**
     * 创建或获取子命名空间
     */
    Namespace* CreateOrGetChildNamespace(const std::string& name);
    
    /**
     * 获取子命名空间
     */
    Namespace* GetChildNamespace(const std::string& name);
    const Namespace* GetChildNamespace(const std::string& name) const;
    
    /**
     * 获取所有子命名空间
     */
    std::vector<Namespace*> GetChildNamespaces() const;
    
    /**
     * 合并另一个命名空间
     * @param other 要合并的命名空间
     * @param conflicts 冲突的符号列表（输出）
     * @return 是否合并成功
     */
    bool Merge(const Namespace& other, std::vector<SymbolInfo>& conflicts);
    
private:
    std::string m_Name;
    Namespace* m_Parent;
    
    // 符号表（按名称索引）
    std::unordered_map<std::string, SymbolInfo> m_Symbols;
    
    // 子命名空间
    std::unordered_map<std::string, std::unique_ptr<Namespace>> m_Children;
    
    // 辅助方法
    bool HasConflict(const SymbolInfo& symbol) const;
};

/**
 * 命名空间管理器
 * 负责管理所有命名空间和符号解析
 */
class NamespaceManager {
public:
    NamespaceManager();
    ~NamespaceManager();
    
    /**
     * 获取全局命名空间
     */
    Namespace* GetGlobalNamespace() { return m_GlobalNamespace.get(); }
    const Namespace* GetGlobalNamespace() const { return m_GlobalNamespace.get(); }
    
    /**
     * 设置当前命名空间
     */
    void SetCurrentNamespace(Namespace* ns);
    
    /**
     * 获取当前命名空间
     */
    Namespace* GetCurrentNamespace() { return m_CurrentNamespace; }
    const Namespace* GetCurrentNamespace() const { return m_CurrentNamespace; }
    
    /**
     * 进入命名空间
     */
    void EnterNamespace(const std::string& name);
    
    /**
     * 退出当前命名空间
     */
    void ExitNamespace();
    
    /**
     * 解析命名空间路径（支持嵌套）
     * @param path 命名空间路径，如 "space::room" 或 "space.room"
     */
    Namespace* ResolveNamespacePath(const std::string& path);
    
    /**
     * 解析符号
     * @param name 符号名称（可能包含命名空间前缀）
     * @param type 符号类型
     * @param searchParent 是否搜索父命名空间
     */
    SymbolInfo* ResolveSymbol(const std::string& name,
                             SymbolType type = SymbolType::NAMESPACE,
                             bool searchParent = true);
    
    /**
     * 从特定命名空间解析符号
     * 支持 from 语法
     */
    SymbolInfo* ResolveSymbolFrom(const std::string& name,
                                 const std::string& fromNamespace,
                                 SymbolType type = SymbolType::NAMESPACE);
    
    /**
     * 注册符号到当前命名空间
     */
    bool RegisterSymbol(const SymbolInfo& symbol);
    
    /**
     * 合并命名空间（同名合并）
     * @param ns 要合并的命名空间节点
     * @return 是否合并成功
     */
    bool MergeNamespace(ast::NamespaceNode* ns);
    
    /**
     * 获取冲突信息
     */
    const std::vector<std::string>& GetConflicts() const { return m_Conflicts; }
    void ClearConflicts() { m_Conflicts.clear(); }
    
    /**
     * 导出符号表（用于调试）
     */
    std::string ExportSymbolTable() const;
    
    /**
     * 重置管理器
     */
    void Reset();
    
private:
    // 全局命名空间（根）
    std::unique_ptr<Namespace> m_GlobalNamespace;
    
    // 当前命名空间
    Namespace* m_CurrentNamespace;
    
    // 命名空间栈（用于进入/退出）
    std::vector<Namespace*> m_NamespaceStack;
    
    // 冲突记录
    std::vector<std::string> m_Conflicts;
    
    // 辅助方法
    std::vector<std::string> SplitNamespacePath(const std::string& path) const;
    void ReportConflict(const SymbolInfo& existing, const SymbolInfo& newSymbol);
};

/**
 * 命名空间解析器
 * 处理from语法和符号查找
 */
class NamespaceResolver {
public:
    NamespaceResolver(NamespaceManager& manager);
    
    /**
     * 解析from语法
     * 如：@Element Box from space
     */
    SymbolInfo* ResolveFromClause(const std::string& symbolName,
                                 const std::string& fromNamespace,
                                 SymbolType expectedType);
    
    /**
     * 解析完全限定名
     * 如：space::room::Box
     */
    SymbolInfo* ResolveQualifiedName(const std::string& qualifiedName,
                                    SymbolType expectedType = SymbolType::NAMESPACE);
    
    /**
     * 批量解析符号
     * 用于导入所有符号的情况
     */
    std::vector<SymbolInfo> ResolveAllSymbols(const std::string& fromNamespace,
                                             SymbolType type);
    
private:
    NamespaceManager& m_Manager;
};

/**
 * RAII命名空间守卫
 * 自动管理命名空间的进入和退出
 */
class NamespaceGuard {
public:
    NamespaceGuard(NamespaceManager& manager, const std::string& namespaceName)
        : m_Manager(manager) {
        m_Manager.EnterNamespace(namespaceName);
    }
    
    ~NamespaceGuard() {
        m_Manager.ExitNamespace();
    }
    
    // 禁止拷贝
    NamespaceGuard(const NamespaceGuard&) = delete;
    NamespaceGuard& operator=(const NamespaceGuard&) = delete;
    
    // 允许移动
    NamespaceGuard(NamespaceGuard&&) = default;
    
private:
    NamespaceManager& m_Manager;
};

} // namespace ns
} // namespace chtl