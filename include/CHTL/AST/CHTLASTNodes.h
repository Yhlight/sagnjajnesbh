#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include "CHTL/Core/CHTLToken.h"

namespace CHTL {
namespace AST {

// 前向声明
class ASTNode;
class ElementNode;
class TextNode;
class AttributeNode;
class StyleBlockNode;
class ScriptBlockNode;
class TemplateNode;
class CustomNode;
class OriginNode;
class ImportNode;
class NamespaceNode;
class ConfigurationNode;

using ASTNodePtr = std::shared_ptr<ASTNode>;
using ASTNodeList = std::vector<ASTNodePtr>;

/**
 * @brief AST节点类型枚举
 */
enum class NodeType {
    // 基础节点
    ROOT,                   // 根节点
    ELEMENT,                // 元素节点
    TEXT,                   // 文本节点
    ATTRIBUTE,              // 属性节点
    COMMENT,                // 注释节点
    
    // 样式和脚本
    STYLE_BLOCK,            // 样式块节点
    SCRIPT_BLOCK,           // 脚本块节点
    CSS_SELECTOR,           // CSS选择器节点
    CSS_PROPERTY,           // CSS属性节点
    
    // 模板系统
    TEMPLATE_STYLE,         // 样式组模板节点
    TEMPLATE_ELEMENT,       // 元素模板节点
    TEMPLATE_VAR,           // 变量组模板节点
    
    // 自定义系统
    CUSTOM_STYLE,           // 自定义样式组节点
    CUSTOM_ELEMENT,         // 自定义元素节点
    CUSTOM_VAR,             // 自定义变量组节点
    
    // 原始嵌入
    ORIGIN_HTML,            // 原始HTML嵌入节点
    ORIGIN_STYLE,           // 原始样式嵌入节点
    ORIGIN_JAVASCRIPT,      // 原始JavaScript嵌入节点
    ORIGIN_CUSTOM,          // 自定义类型原始嵌入节点
    
    // 导入和命名空间
    IMPORT,                 // 导入节点
    NAMESPACE,              // 命名空间节点
    CONFIGURATION,          // 配置节点
    
    // 特殊操作
    INHERITANCE,            // 继承节点
    DELETION,               // 删除节点
    INSERTION,              // 插入节点
    INDEX_ACCESS,           // 索引访问节点
    CONSTRAINT,             // 约束节点
    
    // 引用节点
    TEMPLATE_REFERENCE,     // 模板引用节点
    CUSTOM_REFERENCE,       // 自定义引用节点
    
    // 特例化
    SPECIALIZATION,         // 特例化节点
    PROPERTY_DELETION,      // 属性删除节点
    INHERITANCE_DELETION,   // 继承删除节点
    
    // 字面量
    STRING_LITERAL,         // 字符串字面量节点
    UNQUOTED_LITERAL,       // 无修饰字面量节点
    NUMBER_LITERAL,         // 数字字面量节点
    
    // 其他
    UNKNOWN                 // 未知节点类型
};

/**
 * @brief AST节点基类
 */
class ASTNode {
public:
    /**
     * @brief 构造函数
     * @param type 节点类型
     * @param token 关联的Token
     */
    ASTNode(NodeType type, const Core::CHTLToken& token = Core::CHTLToken());
    
    /**
     * @brief 虚析构函数
     */
    virtual ~ASTNode() = default;
    
    /**
     * @brief 获取节点类型
     */
    NodeType GetType() const { return type_; }
    
    /**
     * @brief 获取关联的Token
     */
    const Core::CHTLToken& GetToken() const { return token_; }
    
    /**
     * @brief 设置关联的Token
     */
    void SetToken(const Core::CHTLToken& token) { token_ = token; }
    
    /**
     * @brief 获取父节点
     */
    ASTNode* GetParent() const { return parent_; }
    
    /**
     * @brief 设置父节点
     */
    void SetParent(ASTNode* parent) { parent_ = parent; }
    
    /**
     * @brief 添加子节点
     */
    void AddChild(ASTNodePtr child);
    
    /**
     * @brief 获取所有子节点
     */
    const ASTNodeList& GetChildren() const { return children_; }
    
    /**
     * @brief 获取子节点数量
     */
    size_t GetChildCount() const { return children_.size(); }
    
    /**
     * @brief 获取指定索引的子节点
     */
    ASTNodePtr GetChild(size_t index) const;
    
    /**
     * @brief 移除子节点
     */
    bool RemoveChild(ASTNodePtr child);
    
    /**
     * @brief 移除指定索引的子节点
     */
    bool RemoveChild(size_t index);
    
    /**
     * @brief 查找指定类型的子节点
     */
    std::vector<ASTNodePtr> FindChildrenByType(NodeType type) const;
    
    /**
     * @brief 查找第一个指定类型的子节点
     */
    ASTNodePtr FindFirstChildByType(NodeType type) const;
    
    /**
     * @brief 接受访问者模式
     */
    virtual void Accept(class ASTVisitor& visitor) = 0;
    
    /**
     * @brief 克隆节点
     */
    virtual ASTNodePtr Clone() const = 0;
    
    /**
     * @brief 转换为字符串表示
     */
    virtual std::string ToString() const;
    
    /**
     * @brief 获取节点的源代码位置
     */
    std::string GetSourceLocation() const;
    
    /**
     * @brief 获取节点类型名称
     */
    static std::string GetNodeTypeName(NodeType type);

protected:
    NodeType type_;                 // 节点类型
    Core::CHTLToken token_;         // 关联的Token
    ASTNode* parent_;              // 父节点
    ASTNodeList children_;         // 子节点列表
};

/**
 * @brief 根节点类
 */
class RootNode : public ASTNode {
public:
    RootNode();
    void Accept(class ASTVisitor& visitor) override;
    ASTNodePtr Clone() const override;
    std::string ToString() const override;
    
    /**
     * @brief 获取文件名
     */
    const std::string& GetFileName() const { return fileName_; }
    
    /**
     * @brief 设置文件名
     */
    void SetFileName(const std::string& fileName) { fileName_ = fileName; }

private:
    std::string fileName_;
};

/**
 * @brief 元素节点类
 */
class ElementNode : public ASTNode {
public:
    ElementNode(const std::string& tagName, const Core::CHTLToken& token = Core::CHTLToken());
    void Accept(class ASTVisitor& visitor) override;
    ASTNodePtr Clone() const override;
    std::string ToString() const override;
    
    /**
     * @brief 获取标签名
     */
    const std::string& GetTagName() const { return tagName_; }
    
    /**
     * @brief 设置标签名
     */
    void SetTagName(const std::string& tagName) { tagName_ = tagName; }
    
    /**
     * @brief 添加属性
     */
    void AddAttribute(const std::string& name, const std::string& value);
    
    /**
     * @brief 获取属性值
     */
    std::string GetAttribute(const std::string& name) const;
    
    /**
     * @brief 检查是否有指定属性
     */
    bool HasAttribute(const std::string& name) const;
    
    /**
     * @brief 获取所有属性
     */
    const std::unordered_map<std::string, std::string>& GetAttributes() const { return attributes_; }
    
    /**
     * @brief 设置ID
     */
    void SetId(const std::string& id) { attributes_["id"] = id; }
    
    /**
     * @brief 添加CSS类
     */
    void AddClass(const std::string& className);
    
    /**
     * @brief 检查是否为自闭合标签
     */
    bool IsSelfClosing() const;

private:
    std::string tagName_;
    std::unordered_map<std::string, std::string> attributes_;
};

/**
 * @brief 文本节点类
 */
class TextNode : public ASTNode {
public:
    TextNode(const std::string& content, const Core::CHTLToken& token = Core::CHTLToken());
    void Accept(class ASTVisitor& visitor) override;
    ASTNodePtr Clone() const override;
    std::string ToString() const override;
    
    /**
     * @brief 获取文本内容
     */
    const std::string& GetContent() const { return content_; }
    
    /**
     * @brief 设置文本内容
     */
    void SetContent(const std::string& content) { content_ = content; }
    
    /**
     * @brief 检查是否为字面量
     */
    bool IsLiteral() const { return isLiteral_; }
    
    /**
     * @brief 设置是否为字面量
     */
    void SetIsLiteral(bool isLiteral) { isLiteral_ = isLiteral; }

private:
    std::string content_;
    bool isLiteral_;
};

/**
 * @brief 属性节点类
 */
class AttributeNode : public ASTNode {
public:
    AttributeNode(const std::string& name, const std::string& value, 
                  const Core::CHTLToken& token = Core::CHTLToken());
    void Accept(class ASTVisitor& visitor) override;
    ASTNodePtr Clone() const override;
    std::string ToString() const override;
    
    /**
     * @brief 获取属性名
     */
    const std::string& GetName() const { return name_; }
    
    /**
     * @brief 设置属性名
     */
    void SetName(const std::string& name) { name_ = name; }
    
    /**
     * @brief 获取属性值
     */
    const std::string& GetValue() const { return value_; }
    
    /**
     * @brief 设置属性值
     */
    void SetValue(const std::string& value) { value_ = value; }
    
    /**
     * @brief 检查是否使用CE对等式（= 而不是 :）
     */
    bool UsesCEEquality() const { return usesCEEquality_; }
    
    /**
     * @brief 设置是否使用CE对等式
     */
    void SetUsesCEEquality(bool uses) { usesCEEquality_ = uses; }

private:
    std::string name_;
    std::string value_;
    bool usesCEEquality_;
};

/**
 * @brief 样式块节点类
 */
class StyleBlockNode : public ASTNode {
public:
    StyleBlockNode(const Core::CHTLToken& token = Core::CHTLToken());
    void Accept(class ASTVisitor& visitor) override;
    ASTNodePtr Clone() const override;
    std::string ToString() const override;
    
    /**
     * @brief 检查是否为局部样式块
     */
    bool IsLocalStyleBlock() const { return isLocalStyleBlock_; }
    
    /**
     * @brief 设置是否为局部样式块
     */
    void SetIsLocalStyleBlock(bool isLocal) { isLocalStyleBlock_ = isLocal; }
    
    /**
     * @brief 添加CSS选择器节点
     */
    void AddSelector(ASTNodePtr selector);
    
    /**
     * @brief 添加CSS属性节点
     */
    void AddProperty(ASTNodePtr property);
    
    /**
     * @brief 添加模板引用
     */
    void AddTemplateReference(ASTNodePtr templateRef);

private:
    bool isLocalStyleBlock_;
};

/**
 * @brief 脚本块节点类
 */
class ScriptBlockNode : public ASTNode {
public:
    ScriptBlockNode(const Core::CHTLToken& token = Core::CHTLToken());
    void Accept(class ASTVisitor& visitor) override;
    ASTNodePtr Clone() const override;
    std::string ToString() const override;
    
    /**
     * @brief 检查是否为局部脚本块
     */
    bool IsLocalScriptBlock() const { return isLocalScriptBlock_; }
    
    /**
     * @brief 设置是否为局部脚本块
     */
    void SetIsLocalScriptBlock(bool isLocal) { isLocalScriptBlock_ = isLocal; }
    
    /**
     * @brief 获取脚本内容
     */
    const std::string& GetScriptContent() const { return scriptContent_; }
    
    /**
     * @brief 设置脚本内容
     */
    void SetScriptContent(const std::string& content) { scriptContent_ = content; }

private:
    bool isLocalScriptBlock_;
    std::string scriptContent_;
};

/**
 * @brief CSS选择器节点类
 */
class CSSSelectorNode : public ASTNode {
public:
    enum class SelectorType {
        CLASS,              // .className
        ID,                 // #idName
        PSEUDO_CLASS,       // :hover
        PSEUDO_ELEMENT,     // ::before
        AMPERSAND,          // &
        ELEMENT             // elementName
    };
    
    CSSSelectorNode(SelectorType selectorType, const std::string& selector, 
                   const Core::CHTLToken& token = Core::CHTLToken());
    void Accept(class ASTVisitor& visitor) override;
    ASTNodePtr Clone() const override;
    std::string ToString() const override;
    
    /**
     * @brief 获取选择器类型
     */
    SelectorType GetSelectorType() const { return selectorType_; }
    
    /**
     * @brief 获取选择器名称
     */
    const std::string& GetSelector() const { return selector_; }
    
    /**
     * @brief 检查是否会自动添加到全局样式块
     */
    bool IsAddedToGlobalStyle() const { return addToGlobalStyle_; }
    
    /**
     * @brief 设置是否添加到全局样式块
     */
    void SetAddToGlobalStyle(bool add) { addToGlobalStyle_ = add; }

private:
    SelectorType selectorType_;
    std::string selector_;
    bool addToGlobalStyle_;
};

/**
 * @brief CSS属性节点类
 */
class CSSPropertyNode : public ASTNode {
public:
    CSSPropertyNode(const std::string& property, const std::string& value,
                   const Core::CHTLToken& token = Core::CHTLToken());
    void Accept(class ASTVisitor& visitor) override;
    ASTNodePtr Clone() const override;
    std::string ToString() const override;
    
    /**
     * @brief 获取属性名
     */
    const std::string& GetProperty() const { return property_; }
    
    /**
     * @brief 获取属性值
     */
    const std::string& GetValue() const { return value_; }
    
    /**
     * @brief 设置属性值
     */
    void SetValue(const std::string& value) { value_ = value; }
    
    /**
     * @brief 检查是否使用CE对等式
     */
    bool UsesCEEquality() const { return usesCEEquality_; }
    
    /**
     * @brief 设置是否使用CE对等式
     */
    void SetUsesCEEquality(bool uses) { usesCEEquality_ = uses; }

private:
    std::string property_;
    std::string value_;
    bool usesCEEquality_;
};

/**
 * @brief 模板节点基类
 */
class TemplateNode : public ASTNode {
public:
    enum class TemplateType {
        STYLE,              // @Style模板
        ELEMENT,            // @Element模板
        VAR                 // @Var模板
    };
    
    TemplateNode(TemplateType templateType, const std::string& name,
                const Core::CHTLToken& token = Core::CHTLToken());
    void Accept(class ASTVisitor& visitor) override;
    ASTNodePtr Clone() const override;
    std::string ToString() const override;
    
    /**
     * @brief 获取模板类型
     */
    TemplateType GetTemplateType() const { return templateType_; }
    
    /**
     * @brief 获取模板名称
     */
    const std::string& GetName() const { return name_; }
    
    /**
     * @brief 设置模板名称
     */
    void SetName(const std::string& name) { name_ = name; }
    
    /**
     * @brief 添加继承
     */
    void AddInheritance(ASTNodePtr inheritance);
    
    /**
     * @brief 获取继承列表
     */
    const ASTNodeList& GetInheritances() const { return inheritances_; }
    
    /**
     * @brief 添加变量（用于VAR类型模板）
     */
    void AddVariable(const std::string& name, const std::string& value);
    
    /**
     * @brief 获取变量值
     */
    std::string GetVariable(const std::string& name) const;
    
    /**
     * @brief 获取所有变量
     */
    const std::unordered_map<std::string, std::string>& GetVariables() const { return variables_; }

protected:
    TemplateType templateType_;
    std::string name_;
    ASTNodeList inheritances_;
    std::unordered_map<std::string, std::string> variables_;
};

/**
 * @brief 自定义节点基类
 */
class CustomNode : public ASTNode {
public:
    enum class CustomType {
        STYLE,              // @Style自定义
        ELEMENT,            // @Element自定义
        VAR                 // @Var自定义
    };
    
    CustomNode(CustomType customType, const std::string& name,
              const Core::CHTLToken& token = Core::CHTLToken());
    void Accept(class ASTVisitor& visitor) override;
    ASTNodePtr Clone() const override;
    std::string ToString() const override;
    
    /**
     * @brief 获取自定义类型
     */
    CustomType GetCustomType() const { return customType_; }
    
    /**
     * @brief 获取自定义名称
     */
    const std::string& GetName() const { return name_; }
    
    /**
     * @brief 设置自定义名称
     */
    void SetName(const std::string& name) { name_ = name; }
    
    /**
     * @brief 添加继承
     */
    void AddInheritance(ASTNodePtr inheritance);
    
    /**
     * @brief 添加特例化
     */
    void AddSpecialization(ASTNodePtr specialization);
    
    /**
     * @brief 获取继承列表
     */
    const ASTNodeList& GetInheritances() const { return inheritances_; }
    
    /**
     * @brief 获取特例化列表
     */
    const ASTNodeList& GetSpecializations() const { return specializations_; }
    
    /**
     * @brief 添加变量（用于VAR类型自定义）
     */
    void AddVariable(const std::string& name, const std::string& value);
    
    /**
     * @brief 获取变量值
     */
    std::string GetVariable(const std::string& name) const;
    
    /**
     * @brief 获取所有变量
     */
    const std::unordered_map<std::string, std::string>& GetVariables() const { return variables_; }

protected:
    CustomType customType_;
    std::string name_;
    ASTNodeList inheritances_;
    ASTNodeList specializations_;
    std::unordered_map<std::string, std::string> variables_;
};

/**
 * @brief 原始嵌入节点类
 */
class OriginNode : public ASTNode {
public:
    enum class OriginType {
        HTML,               // @Html
        STYLE,              // @Style
        JAVASCRIPT,         // @JavaScript
        CUSTOM              // 自定义类型（如@Vue）
    };
    
    OriginNode(OriginType originType, const std::string& content,
              const std::string& name = "", const Core::CHTLToken& token = Core::CHTLToken());
    void Accept(class ASTVisitor& visitor) override;
    ASTNodePtr Clone() const override;
    std::string ToString() const override;
    
    /**
     * @brief 获取原始嵌入类型
     */
    OriginType GetOriginType() const { return originType_; }
    
    /**
     * @brief 获取内容
     */
    const std::string& GetContent() const { return content_; }
    
    /**
     * @brief 设置内容
     */
    void SetContent(const std::string& content) { content_ = content; }
    
    /**
     * @brief 获取名称（用于带名原始嵌入）
     */
    const std::string& GetName() const { return name_; }
    
    /**
     * @brief 设置名称
     */
    void SetName(const std::string& name) { name_ = name; }
    
    /**
     * @brief 检查是否为带名原始嵌入
     */
    bool IsNamed() const { return !name_.empty(); }
    
    /**
     * @brief 获取自定义类型名称（仅当originType_为CUSTOM时有效）
     */
    const std::string& GetCustomTypeName() const { return customTypeName_; }
    
    /**
     * @brief 设置自定义类型名称
     */
    void SetCustomTypeName(const std::string& typeName) { customTypeName_ = typeName; }

private:
    OriginType originType_;
    std::string content_;
    std::string name_;
    std::string customTypeName_;
};

/**
 * @brief 导入节点类
 */
class ImportNode : public ASTNode {
public:
    enum class ImportType {
        HTML,               // @Html (旧版本，保持兼容)
        STYLE,              // @Style (旧版本，保持兼容)
        JAVASCRIPT,         // @JavaScript (旧版本，保持兼容)
        CHTL,               // @Chtl
        CJMOD,              // @CJmod
        CONFIG,             // @Config
        
        // 媒体文件导入（需要as语法）
        MEDIA_HTML,         // @Html (新版本，需要as)
        MEDIA_STYLE,        // @Style (新版本，需要as)
        MEDIA_JAVASCRIPT,   // @JavaScript (新版本，需要as)
        
        // 原始嵌入导入
        ORIGIN_HTML,        // [Origin] @Html
        ORIGIN_STYLE,       // [Origin] @Style
        ORIGIN_JAVASCRIPT,  // [Origin] @JavaScript
        ORIGIN_VUE,         // [Origin] @Vue
        ORIGIN_REACT,       // [Origin] @React
        ORIGIN_ANGULAR,     // [Origin] @Angular
        ORIGIN_CUSTOM,      // [Origin] @CustomType
        
        // 模板导入（保持兼容性）
        TEMPLATE_STYLE,     // [Template] @Style
        TEMPLATE_ELEMENT,   // [Template] @Element
        TEMPLATE_VAR,       // [Template] @Var
        CUSTOM_STYLE,       // [Custom] @Style
        CUSTOM_ELEMENT,     // [Custom] @Element
        CUSTOM_VAR,         // [Custom] @Var
        
        // 批量导入
        ALL_TEMPLATES,      // [Template]
        ALL_CUSTOMS,        // [Custom]
        ALL_ORIGINS         // [Origin]
    };
    
    ImportNode(ImportType importType, const std::string& path,
              const std::string& name = "", const std::string& alias = "",
              const Core::CHTLToken& token = Core::CHTLToken());
    void Accept(class ASTVisitor& visitor) override;
    ASTNodePtr Clone() const override;
    std::string ToString() const override;
    
    /**
     * @brief 获取导入类型
     */
    ImportType GetImportType() const { return importType_; }
    
    /**
     * @brief 获取导入路径
     */
    const std::string& GetPath() const { return path_; }
    
    /**
     * @brief 获取导入名称
     */
    const std::string& GetName() const { return name_; }
    
    /**
     * @brief 获取别名
     */
    const std::string& GetAlias() const { return alias_; }
    
    /**
     * @brief 检查是否有别名
     */
    bool HasAlias() const { return !alias_.empty(); }
    
    /**
     * @brief 设置导入名称
     */
    void SetName(const std::string& name) { name_ = name; }
    
    /**
     * @brief 检查是否为批量导入
     */
    bool IsBatchImport() const;

private:
    ImportType importType_;
    std::string path_;
    std::string name_;
    std::string alias_;
};

/**
 * @brief 命名空间节点类
 */
class NamespaceNode : public ASTNode {
public:
    NamespaceNode(const std::string& name, const Core::CHTLToken& token = Core::CHTLToken());
    void Accept(class ASTVisitor& visitor) override;
    ASTNodePtr Clone() const override;
    std::string ToString() const override;
    
    /**
     * @brief 获取命名空间名称
     */
    const std::string& GetName() const { return name_; }
    
    /**
     * @brief 设置命名空间名称
     */
    void SetName(const std::string& name) { name_ = name; }
    
    /**
     * @brief 检查是否为嵌套命名空间
     */
    bool IsNested() const;
    
    /**
     * @brief 添加约束
     */
    void AddConstraint(ASTNodePtr constraint);
    
    /**
     * @brief 获取约束列表
     */
    const ASTNodeList& GetConstraints() const { return constraints_; }

private:
    std::string name_;
    ASTNodeList constraints_;
};

/**
 * @brief 配置节点类
 */
class ConfigurationNode : public ASTNode {
public:
    ConfigurationNode(const std::string& name = "", const Core::CHTLToken& token = Core::CHTLToken());
    void Accept(class ASTVisitor& visitor) override;
    ASTNodePtr Clone() const override;
    std::string ToString() const override;
    
    /**
     * @brief 获取配置名称
     */
    const std::string& GetName() const { return name_; }
    
    /**
     * @brief 设置配置名称
     */
    void SetName(const std::string& name) { name_ = name; }
    
    /**
     * @brief 添加配置设置
     */
    void AddSetting(const std::string& key, const std::string& value);
    
    /**
     * @brief 获取配置设置
     */
    std::string GetSetting(const std::string& key) const;
    
    /**
     * @brief 获取所有设置
     */
    const std::unordered_map<std::string, std::string>& GetSettings() const { return settings_; }
    
    /**
     * @brief 添加配置组
     */
    void AddGroup(const std::string& groupName, const std::vector<std::string>& items);
    
    /**
     * @brief 获取配置组
     */
    std::vector<std::string> GetGroup(const std::string& groupName) const;
    
    /**
     * @brief 获取所有配置组
     */
    const std::unordered_map<std::string, std::vector<std::string>>& GetGroups() const { return groups_; }
    
    /**
     * @brief 检查是否为命名配置
     */
    bool IsNamed() const { return !name_.empty(); }

private:
    std::string name_;
    std::unordered_map<std::string, std::string> settings_;
    std::unordered_map<std::string, std::vector<std::string>> groups_;
};

/**
 * @brief 继承节点类
 */
class InheritanceNode : public ASTNode {
public:
    InheritanceNode(const std::string& targetType, const std::string& targetName,
                   bool isExplicit = false, const Core::CHTLToken& token = Core::CHTLToken());
    void Accept(class ASTVisitor& visitor) override;
    ASTNodePtr Clone() const override;
    std::string ToString() const override;
    
    /**
     * @brief 获取继承目标类型
     */
    const std::string& GetTargetType() const { return targetType_; }
    
    /**
     * @brief 获取继承目标名称
     */
    const std::string& GetTargetName() const { return targetName_; }
    
    /**
     * @brief 检查是否为显式继承
     */
    bool IsExplicit() const { return isExplicit_; }
    
    /**
     * @brief 设置继承目标的命名空间
     */
    void SetNamespace(const std::string& namespaceName) { namespaceName_ = namespaceName; }
    
    /**
     * @brief 获取继承目标的命名空间
     */
    const std::string& GetNamespace() const { return namespaceName_; }

private:
    std::string targetType_;
    std::string targetName_;
    std::string namespaceName_;
    bool isExplicit_;
};

/**
 * @brief 删除节点类
 */
class DeletionNode : public ASTNode {
public:
    enum class DeletionType {
        PROPERTY,           // 删除属性
        INHERITANCE,        // 删除继承
        ELEMENT,            // 删除元素
        ELEMENT_INHERITANCE // 删除元素继承
    };
    
    DeletionNode(DeletionType deletionType, const std::vector<std::string>& targets,
                const Core::CHTLToken& token = Core::CHTLToken());
    void Accept(class ASTVisitor& visitor) override;
    ASTNodePtr Clone() const override;
    std::string ToString() const override;
    
    /**
     * @brief 获取删除类型
     */
    DeletionType GetDeletionType() const { return deletionType_; }
    
    /**
     * @brief 获取删除目标列表
     */
    const std::vector<std::string>& GetTargets() const { return targets_; }
    
    /**
     * @brief 添加删除目标
     */
    void AddTarget(const std::string& target);

private:
    DeletionType deletionType_;
    std::vector<std::string> targets_;
};

/**
 * @brief 插入节点类
 */
class InsertionNode : public ASTNode {
public:
    enum class InsertionPosition {
        AFTER,              // after
        BEFORE,             // before
        REPLACE,            // replace
        AT_TOP,             // at top
        AT_BOTTOM           // at bottom
    };
    
    InsertionNode(InsertionPosition position, const std::string& target,
                 const Core::CHTLToken& token = Core::CHTLToken());
    void Accept(class ASTVisitor& visitor) override;
    ASTNodePtr Clone() const override;
    std::string ToString() const override;
    
    /**
     * @brief 获取插入位置
     */
    InsertionPosition GetPosition() const { return position_; }
    
    /**
     * @brief 获取插入目标
     */
    const std::string& GetTarget() const { return target_; }
    
    /**
     * @brief 设置插入目标
     */
    void SetTarget(const std::string& target) { target_ = target; }
    
    /**
     * @brief 添加要插入的内容
     */
    void AddInsertContent(ASTNodePtr content);
    
    /**
     * @brief 获取插入内容列表
     */
    const ASTNodeList& GetInsertContents() const { return insertContents_; }

private:
    InsertionPosition position_;
    std::string target_;
    ASTNodeList insertContents_;
};

/**
 * @brief 索引访问节点类
 */
class IndexAccessNode : public ASTNode {
public:
    IndexAccessNode(const std::string& elementName, int index,
                   const Core::CHTLToken& token = Core::CHTLToken());
    void Accept(class ASTVisitor& visitor) override;
    ASTNodePtr Clone() const override;
    std::string ToString() const override;
    
    /**
     * @brief 获取元素名称
     */
    const std::string& GetElementName() const { return elementName_; }
    
    /**
     * @brief 获取索引
     */
    int GetIndex() const { return index_; }
    
    /**
     * @brief 设置索引
     */
    void SetIndex(int index) { index_ = index; }

private:
    std::string elementName_;
    int index_;
};

/**
 * @brief 约束节点类
 */
class ConstraintNode : public ASTNode {
public:
    enum class ConstraintType {
        PRECISE,            // 精确约束
        TYPE,               // 类型约束
        GLOBAL              // 全局约束
    };
    
    ConstraintNode(ConstraintType constraintType, const std::vector<std::string>& targets,
                  const Core::CHTLToken& token = Core::CHTLToken());
    void Accept(class ASTVisitor& visitor) override;
    ASTNodePtr Clone() const override;
    std::string ToString() const override;
    
    /**
     * @brief 获取约束类型
     */
    ConstraintType GetConstraintType() const { return constraintType_; }
    
    /**
     * @brief 获取约束目标列表
     */
    const std::vector<std::string>& GetTargets() const { return targets_; }
    
    /**
     * @brief 添加约束目标
     */
    void AddTarget(const std::string& target);

private:
    ConstraintType constraintType_;
    std::vector<std::string> targets_;
};



/**
 * @brief 模板引用节点类
 */
class TemplateReferenceNode : public ASTNode {
public:
    TemplateReferenceNode(const std::string& templateType, const std::string& templateName,
                         const std::string& namespaceName = "",
                         const Core::CHTLToken& token = Core::CHTLToken());
    void Accept(class ASTVisitor& visitor) override;
    ASTNodePtr Clone() const override;
    std::string ToString() const override;
    
    /**
     * @brief 获取模板类型
     */
    const std::string& GetTemplateType() const { return templateType_; }
    
    /**
     * @brief 获取模板名称
     */
    const std::string& GetTemplateName() const { return templateName_; }
    
    /**
     * @brief 获取命名空间名称
     */
    const std::string& GetNamespace() const { return namespaceName_; }
    
    /**
     * @brief 检查是否使用全缀名
     */
    bool IsFullyQualified() const { return isFullyQualified_; }
    
    /**
     * @brief 设置是否使用全缀名
     */
    void SetIsFullyQualified(bool isFullyQualified) { isFullyQualified_ = isFullyQualified; }

private:
    std::string templateType_;
    std::string templateName_;
    std::string namespaceName_;
    bool isFullyQualified_;
};

/**
 * @brief 自定义引用节点类
 */
class CustomReferenceNode : public ASTNode {
public:
    CustomReferenceNode(const std::string& customType, const std::string& customName,
                       const std::string& namespaceName = "",
                       const Core::CHTLToken& token = Core::CHTLToken());
    void Accept(class ASTVisitor& visitor) override;
    ASTNodePtr Clone() const override;
    std::string ToString() const override;
    
    /**
     * @brief 获取自定义类型
     */
    const std::string& GetCustomType() const { return customType_; }
    
    /**
     * @brief 获取自定义名称
     */
    const std::string& GetCustomName() const { return customName_; }
    
    /**
     * @brief 获取命名空间名称
     */
    const std::string& GetNamespace() const { return namespaceName_; }
    
    /**
     * @brief 添加特例化内容
     */
    void AddSpecialization(ASTNodePtr specialization);
    
    /**
     * @brief 获取特例化内容列表
     */
    const ASTNodeList& GetSpecializations() const { return specializations_; }
    
    /**
     * @brief 检查是否有特例化
     */
    bool HasSpecialization() const { return !specializations_.empty(); }

private:
    std::string customType_;
    std::string customName_;
    std::string namespaceName_;
    ASTNodeList specializations_;
};

/**
 * @brief 特例化节点类
 */
class SpecializationNode : public ASTNode {
public:
    SpecializationNode(const Core::CHTLToken& token = Core::CHTLToken());
    void Accept(class ASTVisitor& visitor) override;
    ASTNodePtr Clone() const override;
    std::string ToString() const override;
    
    /**
     * @brief 添加删除操作
     */
    void AddDeletion(ASTNodePtr deletion);
    
    /**
     * @brief 添加属性覆盖
     */
    void AddPropertyOverride(const std::string& property, const std::string& value);
    
    /**
     * @brief 获取删除操作列表
     */
    const ASTNodeList& GetDeletions() const { return deletions_; }
    
    /**
     * @brief 获取属性覆盖
     */
    const std::unordered_map<std::string, std::string>& GetPropertyOverrides() const { 
        return propertyOverrides_; 
    }

private:
    ASTNodeList deletions_;
    std::unordered_map<std::string, std::string> propertyOverrides_;
};

/**
 * @brief 注释节点类
 */
class CommentNode : public ASTNode {
public:
    enum class CommentType {
        SINGLE_LINE,        // //
        MULTI_LINE,         // /* */
        GENERATOR           // --
    };
    
    CommentNode(CommentType commentType, const std::string& content,
               const Core::CHTLToken& token = Core::CHTLToken());
    void Accept(class ASTVisitor& visitor) override;
    ASTNodePtr Clone() const override;
    std::string ToString() const override;
    
    /**
     * @brief 获取注释类型
     */
    CommentType GetCommentType() const { return commentType_; }
    
    /**
     * @brief 获取注释内容
     */
    const std::string& GetContent() const { return content_; }
    
    /**
     * @brief 检查是否为生成器注释
     */
    bool IsGeneratorComment() const { return commentType_ == CommentType::GENERATOR; }

private:
    CommentType commentType_;
    std::string content_;
};

/**
 * @brief 字面量节点类
 */
class LiteralNode : public ASTNode {
public:
    enum class LiteralType {
        STRING,             // 字符串字面量
        UNQUOTED,           // 无修饰字面量
        NUMBER              // 数字字面量
    };
    
    LiteralNode(LiteralType literalType, const std::string& value,
               const Core::CHTLToken& token = Core::CHTLToken());
    void Accept(class ASTVisitor& visitor) override;
    ASTNodePtr Clone() const override;
    std::string ToString() const override;
    
    /**
     * @brief 获取字面量类型
     */
    LiteralType GetLiteralType() const { return literalType_; }
    
    /**
     * @brief 获取字面量值
     */
    const std::string& GetValue() const { return value_; }
    
    /**
     * @brief 设置字面量值
     */
    void SetValue(const std::string& value) { value_ = value; }

private:
    LiteralType literalType_;
    std::string value_;
};

} // namespace AST
} // namespace CHTL