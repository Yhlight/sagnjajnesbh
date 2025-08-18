#pragma once
#include "../State/CHTLGlobalMap.h"
#include "../../Common/Core/ErrorHandler.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace chtl {
namespace ns {

/**
 * 命名空间节点类型
 */
enum class NamespaceNodeType {
    ROOT,           // 根命名空间
    NAMED,          // 命名命名空间
    FILE_DEFAULT,   // 文件默认命名空间（使用文件名）
    NESTED          // 嵌套命名空间
};

/**
 * 符号类型
 */
enum class SymbolType {
    CUSTOM_ELEMENT,     // [Custom] @Element
    CUSTOM_STYLE,       // [Custom] @Style
    CUSTOM_VAR,         // [Custom] @Var
    TEMPLATE_ELEMENT,   // [Template] @Element
    TEMPLATE_STYLE,     // [Template] @Style
    TEMPLATE_VAR,       // [Template] @Var
    ORIGIN_HTML,        // [Origin] @Html
    ORIGIN_STYLE,       // [Origin] @Style
    ORIGIN_JAVASCRIPT,  // [Origin] @Javascript
    CONFIGURATION       // [Configuration] @Config
};

/**
 * 命名空间符号信息
 */
struct NamespaceSymbol {
    std::string name;
    SymbolType type;
    std::string sourceFile;
    int lineNumber = 0;
    int columnNumber = 0;
    std::string definition;  // 符号的完整定义
    std::unordered_map<std::string, std::string> attributes;
    
    NamespaceSymbol() = default;
    NamespaceSymbol(const std::string& n, SymbolType t, const std::string& file)
        : name(n), type(t), sourceFile(file) {}
};

/**
 * 命名空间冲突信息
 */
struct NamespaceConflict {
    std::string namespaceName;
    std::string symbolName;
    SymbolType conflictType;
    std::vector<NamespaceSymbol> conflictingSymbols;
    std::string conflictDescription;
    
    enum class ConflictSeverity {
        WARNING,    // 警告级别（可以自动解决）
        ERROR,      // 错误级别（需要手动解决）
        CRITICAL    // 严重级别（阻止编译）
    } severity = ConflictSeverity::ERROR;
};

/**
 * 命名空间节点
 */
class NamespaceNode {
public:
    NamespaceNode(const std::string& name, NamespaceNodeType type, NamespaceNode* parent = nullptr);
    ~NamespaceNode() = default;
    
    // 基本信息
    const std::string& getName() const { return name_; }
    NamespaceNodeType getType() const { return type_; }
    NamespaceNode* getParent() const { return parent_; }
    const std::vector<std::unique_ptr<NamespaceNode>>& getChildren() const { return children_; }
    
    // 全限定名称
    std::string getFullQualifiedName() const;
    
    // 子命名空间管理
    NamespaceNode* addChild(const std::string& childName, NamespaceNodeType childType);
    NamespaceNode* findChild(const std::string& childName) const;
    NamespaceNode* findDescendant(const std::string& path) const;
    
    // 符号管理
    bool addSymbol(const NamespaceSymbol& symbol);
    bool hasSymbol(const std::string& symbolName, SymbolType type) const;
    NamespaceSymbol* findSymbol(const std::string& symbolName, SymbolType type);
    const NamespaceSymbol* findSymbol(const std::string& symbolName, SymbolType type) const;
    std::vector<NamespaceSymbol*> findSymbolsByName(const std::string& symbolName);
    std::vector<NamespaceSymbol*> findSymbolsByType(SymbolType type);
    const std::unordered_map<std::string, std::vector<NamespaceSymbol>>& getAllSymbols() const { return symbols_; }
    
    // 约束管理
    void addConstraint(const std::string& constraint);
    bool isConstraintViolated(const std::string& symbolName, SymbolType type) const;
    const std::vector<std::string>& getConstraints() const { return constraints_; }
    
    // 合并操作
    bool canMergeWith(const NamespaceNode& other) const;
    bool mergeWith(const NamespaceNode& other);
    
    // 冲突检测
    std::vector<NamespaceConflict> detectConflicts() const;

private:
    std::string name_;
    NamespaceNodeType type_;
    NamespaceNode* parent_;
    std::vector<std::unique_ptr<NamespaceNode>> children_;
    
    // 符号存储：符号名 -> 符号列表（支持同名不同类型）
    std::unordered_map<std::string, std::vector<NamespaceSymbol>> symbols_;
    
    // 约束列表
    std::vector<std::string> constraints_;
    
    // 私有辅助方法
    bool isValidConstraint(const std::string& constraint) const;
    bool checkConstraintConflict(const std::string& symbolName, SymbolType type, const std::string& constraint) const;
};

/**
 * 同名命名空间自动合并策略
 */
enum class MergeStrategy {
    AUTOMATIC,      // 自动合并（默认）
    MANUAL,         // 手动确认合并
    REJECT,         // 拒绝合并，报告冲突
    OVERWRITE       // 覆盖现有命名空间
};

/**
 * 冲突检测策略
 */
enum class ConflictDetectionStrategy {
    STRICT,         // 严格模式：任何冲突都报告错误
    LENIENT,        // 宽松模式：只报告严重冲突
    TYPE_AWARE,     // 类型感知：同名不同类型不算冲突
    SCOPE_AWARE     // 作用域感知：考虑作用域的冲突检测
};

/**
 * 增强的命名空间管理器
 * 实现同名命名空间自动合并和冲突检测策略
 */
class EnhancedNamespaceManager {
public:
    EnhancedNamespaceManager();
    ~EnhancedNamespaceManager();
    
    /**
     * 设置错误处理器
     */
    void setErrorHandler(std::shared_ptr<common::ErrorHandler> errorHandler);
    
    /**
     * 设置合并策略
     */
    void setMergeStrategy(MergeStrategy strategy);
    
    /**
     * 设置冲突检测策略
     */
    void setConflictDetectionStrategy(ConflictDetectionStrategy strategy);
    
    // === 命名空间管理 ===
    
    /**
     * 创建命名空间
     * [Namespace] space
     */
    NamespaceNode* createNamespace(const std::string& name, const std::string& sourceFile);
    
    /**
     * 创建嵌套命名空间
     * [Namespace] space { [Namespace] room }
     */
    NamespaceNode* createNestedNamespace(const std::string& parentPath, const std::string& childName, 
                                        const std::string& sourceFile);
    
    /**
     * 创建文件默认命名空间
     * 如果文件没有命名空间，则默认使用文件名称来作为命名空间
     */
    NamespaceNode* createFileDefaultNamespace(const std::string& fileName, const std::string& sourceFile);
    
    /**
     * 查找命名空间
     */
    NamespaceNode* findNamespace(const std::string& path);
    const NamespaceNode* findNamespace(const std::string& path) const;
    
    /**
     * 获取根命名空间
     */
    NamespaceNode* getRootNamespace() { return rootNamespace_.get(); }
    const NamespaceNode* getRootNamespace() const { return rootNamespace_.get(); }
    
    // === 同名命名空间自动合并 ===
    
    /**
     * 检查是否存在同名命名空间
     */
    bool hasSameNameNamespace(const std::string& name) const;
    
    /**
     * 获取同名命名空间列表
     */
    std::vector<NamespaceNode*> getSameNameNamespaces(const std::string& name);
    
    /**
     * 自动合并同名命名空间
     */
    bool autoMergeSameNameNamespaces(const std::string& name);
    
    /**
     * 手动合并命名空间
     */
    bool mergeNamespaces(const std::string& targetPath, const std::string& sourcePath);
    
    /**
     * 检查命名空间是否可以合并
     */
    bool canMergeNamespaces(const std::string& targetPath, const std::string& sourcePath) const;
    
    // === 冲突检测策略 ===
    
    /**
     * 检测所有命名空间冲突
     */
    std::vector<NamespaceConflict> detectAllConflicts();
    
    /**
     * 检测特定命名空间的冲突
     */
    std::vector<NamespaceConflict> detectNamespaceConflicts(const std::string& namespacePath);
    
    /**
     * 检测符号冲突
     */
    std::vector<NamespaceConflict> detectSymbolConflicts(const std::string& namespacePath, 
                                                        const std::string& symbolName);
    
    /**
     * 解决冲突
     */
    bool resolveConflict(const NamespaceConflict& conflict);
    
    /**
     * 自动解决所有可解决的冲突
     */
    int autoResolveConflicts();
    
    // === 符号管理 ===
    
    /**
     * 添加符号到命名空间
     */
    bool addSymbolToNamespace(const std::string& namespacePath, const NamespaceSymbol& symbol);
    
    /**
     * 查找符号
     */
    NamespaceSymbol* findSymbol(const std::string& namespacePath, const std::string& symbolName, 
                               SymbolType type);
    
    /**
     * 查找符号（支持嵌套查找）
     */
    NamespaceSymbol* findSymbolRecursive(const std::string& startNamespace, const std::string& symbolName, 
                                        SymbolType type);
    
    /**
     * 解析符号引用
     * 例如：@Element Box from space.room2
     */
    NamespaceSymbol* resolveSymbolReference(const std::string& symbolName, SymbolType type, 
                                           const std::string& fromNamespace);
    
    // === 约束管理 ===
    
    /**
     * 添加约束到命名空间
     */
    bool addConstraintToNamespace(const std::string& namespacePath, const std::string& constraint);
    
    /**
     * 检查约束违反
     */
    bool checkConstraintViolation(const std::string& namespacePath, const std::string& symbolName, 
                                 SymbolType type);
    
    /**
     * 验证全局约束
     * 在命名空间内使用全局约束，全局约束只支持前面列出来的类型
     */
    bool validateGlobalConstraints(const std::string& namespacePath);
    
    // === 实用工具 ===
    
    /**
     * 获取命名空间的完整路径
     */
    std::string getNamespaceFullPath(const NamespaceNode* node) const;
    
    /**
     * 解析命名空间路径
     */
    std::vector<std::string> parseNamespacePath(const std::string& path) const;
    
    /**
     * 格式化命名空间树
     */
    std::string formatNamespaceTree() const;
    
    /**
     * 获取统计信息
     */
    struct Statistics {
        int totalNamespaces = 0;
        int totalSymbols = 0;
        int totalConflicts = 0;
        int mergedNamespaces = 0;
        int resolvedConflicts = 0;
    };
    Statistics getStatistics() const;
    
    /**
     * 清理所有数据
     */
    void clear();

private:
    std::unique_ptr<NamespaceNode> rootNamespace_;
    std::shared_ptr<common::ErrorHandler> errorHandler_;
    
    MergeStrategy mergeStrategy_ = MergeStrategy::AUTOMATIC;
    ConflictDetectionStrategy conflictStrategy_ = ConflictDetectionStrategy::TYPE_AWARE;
    
    // 统计信息
    mutable Statistics statistics_;
    
    // 私有辅助方法
    NamespaceNode* createNamespaceInternal(const std::string& name, NamespaceNodeType type, 
                                          NamespaceNode* parent, const std::string& sourceFile);
    
    bool performMerge(NamespaceNode* target, NamespaceNode* source);
    
    std::vector<NamespaceConflict> detectConflictsInternal(NamespaceNode* node) const;
    
    bool isConflictResolvable(const NamespaceConflict& conflict) const;
    
    void reportError(const std::string& message, const std::string& context = "");
    void reportWarning(const std::string& message, const std::string& context = "");
    
    void updateStatistics() const;
    
    std::string symbolTypeToString(SymbolType type) const;
    SymbolType stringToSymbolType(const std::string& typeStr) const;
};

} // namespace ns
} // namespace chtl