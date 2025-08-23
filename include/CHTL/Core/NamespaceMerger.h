#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include "CHTL/AST/CHTLASTNodes.h"

namespace CHTL {
namespace Core {

/**
 * @brief 命名空间冲突类型
 */
enum class NamespaceConflictType {
    NO_CONFLICT,                    // 无冲突
    SYMBOL_NAME_CONFLICT,           // 符号名称冲突
    TYPE_CONFLICT,                  // 类型冲突
    NESTED_NAMESPACE_CONFLICT,      // 嵌套命名空间冲突
    IMPORT_CONFLICT                 // 导入冲突
};

/**
 * @brief 命名空间冲突信息
 */
struct NamespaceConflict {
    NamespaceConflictType type;     // 冲突类型
    std::string conflictingSymbol;  // 冲突的符号名
    std::string sourceNamespace;    // 源命名空间
    std::string targetNamespace;    // 目标命名空间
    std::string description;        // 冲突描述
    
    NamespaceConflict() : type(NamespaceConflictType::NO_CONFLICT) {}
};

/**
 * @brief 命名空间符号信息
 */
struct NamespaceSymbol {
    std::string name;               // 符号名称
    std::string type;               // 符号类型 (@Element, @Style, @Var等)
    std::string source;             // 来源文件
    bool isImported;                // 是否为导入符号
    
    NamespaceSymbol() : isImported(false) {}
};

/**
 * @brief 命名空间合并结果
 */
struct NamespaceMergeResult {
    bool success;                           // 合并是否成功
    std::vector<NamespaceConflict> conflicts; // 冲突列表
    std::shared_ptr<AST::NamespaceNode> mergedNamespace; // 合并后的命名空间
    
    NamespaceMergeResult() : success(false) {}
};

/**
 * @brief 命名空间合并管理器
 * 严格按照目标规划.ini第107行实现同名命名空间合并，冲突检测策略
 */
class NamespaceMerger {
public:
    NamespaceMerger();
    
    /**
     * @brief 尝试合并两个同名命名空间
     * @param source 源命名空间
     * @param target 目标命名空间
     * @return 合并结果
     */
    NamespaceMergeResult MergeNamespaces(std::shared_ptr<AST::NamespaceNode> source,
                                        std::shared_ptr<AST::NamespaceNode> target);
    
    /**
     * @brief 检测命名空间冲突
     * @param source 源命名空间
     * @param target 目标命名空间
     * @return 冲突列表
     */
    std::vector<NamespaceConflict> DetectConflicts(std::shared_ptr<AST::NamespaceNode> source,
                                                   std::shared_ptr<AST::NamespaceNode> target);
    
    /**
     * @brief 注册命名空间
     * @param namespaceNode 命名空间节点
     * @param sourceFile 来源文件
     */
    void RegisterNamespace(std::shared_ptr<AST::NamespaceNode> namespaceNode, const std::string& sourceFile);
    
    /**
     * @brief 获取已注册的命名空间
     * @param namespaceName 命名空间名称
     * @return 命名空间节点列表
     */
    std::vector<std::shared_ptr<AST::NamespaceNode>> GetRegisteredNamespaces(const std::string& namespaceName);
    
    /**
     * @brief 清理所有注册的命名空间
     */
    void Clear();
    
    /**
     * @brief 设置冲突解决策略
     * @param allowMerge 是否允许自动合并
     * @param strictMode 是否启用严格模式
     */
    void SetConflictResolutionStrategy(bool allowMerge, bool strictMode);

private:
    // 已注册的命名空间映射 (命名空间名称 -> 节点列表)
    std::unordered_map<std::string, std::vector<std::shared_ptr<AST::NamespaceNode>>> registeredNamespaces_;
    
    // 命名空间符号表 (命名空间名称 -> 符号列表)
    std::unordered_map<std::string, std::vector<NamespaceSymbol>> namespaceSymbols_;
    
    // 冲突解决策略
    bool allowMerge_;               // 是否允许自动合并
    bool strictMode_;               // 是否启用严格模式
    
    /**
     * @brief 提取命名空间中的所有符号
     * @param namespaceNode 命名空间节点
     * @param sourceFile 来源文件
     * @return 符号列表
     */
    std::vector<NamespaceSymbol> ExtractSymbols(std::shared_ptr<AST::NamespaceNode> namespaceNode,
                                                const std::string& sourceFile);
    
    /**
     * @brief 从AST节点提取符号
     * @param node AST节点
     * @param sourceFile 来源文件
     * @param symbols 符号列表（输出参数）
     */
    void ExtractSymbolsFromNode(std::shared_ptr<AST::ASTNode> node,
                               const std::string& sourceFile,
                               std::vector<NamespaceSymbol>& symbols);
    
    /**
     * @brief 检测符号冲突
     * @param sourceSymbols 源符号列表
     * @param targetSymbols 目标符号列表
     * @param sourceNamespace 源命名空间名称
     * @param targetNamespace 目标命名空间名称
     * @return 冲突列表
     */
    std::vector<NamespaceConflict> DetectSymbolConflicts(const std::vector<NamespaceSymbol>& sourceSymbols,
                                                         const std::vector<NamespaceSymbol>& targetSymbols,
                                                         const std::string& sourceNamespace,
                                                         const std::string& targetNamespace);
    
    /**
     * @brief 检测嵌套命名空间冲突
     * @param source 源命名空间
     * @param target 目标命名空间
     * @return 冲突列表
     */
    std::vector<NamespaceConflict> DetectNestedConflicts(std::shared_ptr<AST::NamespaceNode> source,
                                                         std::shared_ptr<AST::NamespaceNode> target);
    
    /**
     * @brief 合并符号表
     * @param sourceSymbols 源符号列表
     * @param targetSymbols 目标符号列表
     * @param conflicts 冲突列表
     * @return 合并后的符号列表
     */
    std::vector<NamespaceSymbol> MergeSymbols(const std::vector<NamespaceSymbol>& sourceSymbols,
                                             const std::vector<NamespaceSymbol>& targetSymbols,
                                             const std::vector<NamespaceConflict>& conflicts);
    
    /**
     * @brief 检查符号兼容性
     * @param symbol1 符号1
     * @param symbol2 符号2
     * @return 是否兼容
     */
    bool AreSymbolsCompatible(const NamespaceSymbol& symbol1, const NamespaceSymbol& symbol2);
    
    /**
     * @brief 获取节点类型字符串
     * @param node AST节点
     * @return 类型字符串
     */
    std::string GetNodeTypeString(std::shared_ptr<AST::ASTNode> node);
    
    /**
     * @brief 生成合并后的命名空间节点
     * @param source 源命名空间
     * @param target 目标命名空间
     * @param mergedSymbols 合并后的符号列表
     * @return 合并后的命名空间节点
     */
    std::shared_ptr<AST::NamespaceNode> CreateMergedNamespace(std::shared_ptr<AST::NamespaceNode> source,
                                                              std::shared_ptr<AST::NamespaceNode> target,
                                                              const std::vector<NamespaceSymbol>& mergedSymbols);
    
    /**
     * @brief 获取节点名称
     * @param node AST节点
     * @return 节点名称
     */
    std::string GetNodeName(std::shared_ptr<AST::ASTNode> node);
};

} // namespace Core
} // namespace CHTL