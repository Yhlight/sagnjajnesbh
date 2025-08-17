#pragma once

#include "CHTLCommon.h"
#include "CHTLASTNodes.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>

namespace chtl {

// 命名空间项目类型
enum class NamespaceItemType {
    TEMPLATE_STYLE,
    TEMPLATE_ELEMENT,
    TEMPLATE_VAR,
    CUSTOM_STYLE,
    CUSTOM_ELEMENT,
    CUSTOM_VAR,
    ORIGIN_NODE,
    NESTED_NAMESPACE
};

// 命名空间项目
struct NamespaceManagerItem {
    String name;
    NamespaceItemType type;
    std::shared_ptr<CHTLASTNode> node;
    String sourceFile;  // 来源文件，用于冲突检测
    size_t line = 0;
    size_t column = 0;
    
    NamespaceManagerItem(const String& n, NamespaceItemType t, std::shared_ptr<CHTLASTNode> node_ptr, 
                  const String& file = "", size_t l = 0, size_t c = 0)
        : name(n), type(t), node(node_ptr), sourceFile(file), line(l), column(c) {}
};

// 命名空间冲突信息
struct NamespaceConflict {
    String namespaceName;
    String itemName;
    NamespaceItemType itemType;
    std::vector<NamespaceManagerItem> conflictingItems;
    
    NamespaceConflict(const String& ns, const String& item, NamespaceItemType type)
        : namespaceName(ns), itemName(item), itemType(type) {}
};

// 命名空间解析结果
struct NamespaceResolutionResult {
    bool success = false;
    std::shared_ptr<CHTLASTNode> resolvedNode = nullptr;
    String fullyQualifiedName;
    String error;
    std::vector<String> warnings;
};

// 单个命名空间
class Namespace {
public:
    Namespace(const String& name, const String& sourceFile = "");
    
    // 基本信息
    const String& getName() const { return name_; }
    const String& getSourceFile() const { return sourceFile_; }
    void setSourceFile(const String& file) { sourceFile_ = file; }
    
    // 项目管理
    bool addItem(const NamespaceManagerItem& item);
    bool hasItem(const String& name, NamespaceItemType type) const;
    std::shared_ptr<CHTLASTNode> getItem(const String& name, NamespaceItemType type) const;
    std::vector<NamespaceManagerItem> getItemsByName(const String& name) const;
    std::vector<NamespaceManagerItem> getAllItems() const;
    
    // 嵌套命名空间管理
    bool addNestedNamespace(std::shared_ptr<Namespace> nestedNs);
    std::shared_ptr<Namespace> getNestedNamespace(const String& name) const;
    std::vector<std::shared_ptr<Namespace>> getNestedNamespaces() const;
    
    // 合并操作
    bool mergeWith(const Namespace& other, std::vector<NamespaceConflict>& conflicts);
    
    // 查找操作
    NamespaceResolutionResult resolve(const String& path) const;
    NamespaceResolutionResult resolveRecursive(const String& path) const;
    
    // 冲突检测
    std::vector<NamespaceConflict> detectConflicts() const;
    bool hasConflicts() const;
    
    // 统计信息
    size_t getItemCount() const;
    size_t getNestedNamespaceCount() const;
    
private:
    String name_;
    String sourceFile_;
    std::unordered_map<String, std::vector<NamespaceManagerItem>> items_; // name -> items (支持同名不同类型)
    std::unordered_map<String, std::shared_ptr<Namespace>> nestedNamespaces_;
    
    // 辅助方法
    String getItemTypeString(NamespaceItemType type) const;
    bool areItemsCompatible(const NamespaceManagerItem& item1, const NamespaceManagerItem& item2) const;
};

// 命名空间管理器
class NamespaceManager {
public:
    NamespaceManager();
    ~NamespaceManager() = default;
    
    // 命名空间管理
    bool registerNamespace(std::shared_ptr<Namespace> ns);
    std::shared_ptr<Namespace> getNamespace(const String& name) const;
    std::vector<std::shared_ptr<Namespace>> getAllNamespaces() const;
    bool hasNamespace(const String& name) const;
    
    // 同名命名空间合并
    bool mergeNamespaces(const String& name);
    std::vector<NamespaceConflict> getMergeConflicts(const String& name) const;
    
    // 全局冲突检测
    std::vector<NamespaceConflict> detectAllConflicts() const;
    bool hasGlobalConflicts() const;
    
    // 解析操作
    NamespaceResolutionResult resolve(const String& fullyQualifiedName) const;
    NamespaceResolutionResult resolveWithFallback(const String& name, const String& currentNamespace = "") const;
    
    // 构建命名空间从AST
    bool buildFromAST(std::shared_ptr<CHTLASTNode> ast, const String& sourceFile = "");
    
    // 导入处理
    bool processImportedNamespace(std::shared_ptr<Namespace> importedNs, const String& alias = "");
    
    // 查询功能
    std::vector<String> getAvailableItems(const String& namespaceName) const;
    std::vector<String> getAvailableNamespaces() const;
    
    // 验证功能
    bool validateNamespaceName(const String& name) const;
    bool validateItemName(const String& name, const String& namespaceName = "") const;
    
    // 清理功能
    void clearNamespace(const String& name);
    void clearAll();
    
    // 错误处理
    std::vector<String> getErrors() const { return errors_; }
    std::vector<String> getWarnings() const { return warnings_; }
    void clearErrors() { errors_.clear(); warnings_.clear(); }

private:
    std::unordered_map<String, std::vector<std::shared_ptr<Namespace>>> namespaces_; // 支持同名命名空间
    std::vector<String> errors_;
    std::vector<String> warnings_;
    
    // 辅助方法
    void addError(const String& error);
    void addWarning(const String& warning);
    std::vector<String> parseQualifiedName(const String& qualifiedName) const;
    String joinNamespacePath(const std::vector<String>& parts) const;
    NamespaceItemType getItemTypeFromNode(std::shared_ptr<CHTLASTNode> node) const;
    void collectNamespaceItems(std::shared_ptr<CHTLASTNode> node, std::shared_ptr<Namespace> ns, const String& sourceFile);
    bool isValidNamespaceIdentifier(const String& identifier) const;
};

// 命名空间访问器 - 用于简化命名空间访问
class NamespaceAccessor {
public:
    NamespaceAccessor(NamespaceManager& manager, const String& currentNamespace = "");
    
    // 设置当前命名空间上下文
    void setCurrentNamespace(const String& namespaceName);
    const String& getCurrentNamespace() const { return currentNamespace_; }
    
    // 解析项目
    NamespaceResolutionResult resolve(const String& name) const;
    
    // 检查项目是否存在
    bool exists(const String& name) const;
    
    // 获取项目
    std::shared_ptr<CHTLASTNode> get(const String& name) const;
    
    // 列出可用项目
    std::vector<String> listAvailableItems() const;
    std::vector<String> listAvailableNamespaces() const;

private:
    NamespaceManager& manager_;
    String currentNamespace_;
};

} // namespace chtl