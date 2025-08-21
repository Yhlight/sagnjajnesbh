#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace chtl {
namespace ast {
namespace v3 {

// 前向声明
class ASTVisitor;

// 基础AST节点
class ASTNode {
public:
    virtual ~ASTNode() = default;
    
    virtual void Accept(ASTVisitor* visitor) = 0;
    
    void AddChild(std::shared_ptr<ASTNode> child) {
        m_Children.push_back(child);
    }
    
    const std::vector<std::shared_ptr<ASTNode>>& GetChildren() const {
        return m_Children;
    }
    
    void SetLocation(int line, int column) {
        m_Line = line;
        m_Column = column;
    }
    
    int GetLine() const { return m_Line; }
    int GetColumn() const { return m_Column; }

protected:
    std::vector<std::shared_ptr<ASTNode>> m_Children;
    int m_Line = 0;
    int m_Column = 0;
};

// 文档节点
class DocumentNode : public ASTNode {
public:
    void Accept(ASTVisitor* visitor) override;
};

// 元素节点
class ElementNode : public ASTNode {
public:
    explicit ElementNode(const std::string& tagName) : m_TagName(tagName) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    const std::string& GetTagName() const { return m_TagName; }
    
    void AddAttribute(const std::string& name, const std::string& value) {
        m_Attributes[name] = value;
    }
    
    const std::unordered_map<std::string, std::string>& GetAttributes() const {
        return m_Attributes;
    }
    
    bool HasAttribute(const std::string& name) const {
        return m_Attributes.find(name) != m_Attributes.end();
    }
    
    // 索引访问支持
    void SetIndex(int index) { m_Index = index; }
    bool HasIndex() const { return m_Index >= 0; }
    int GetIndex() const { return m_Index; }

private:
    std::string m_TagName;
    std::unordered_map<std::string, std::string> m_Attributes;
    int m_Index = -1;
};

// 文本节点
class TextNode : public ASTNode {
public:
    explicit TextNode(const std::string& content) : m_Content(content) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    const std::string& GetContent() const { return m_Content; }

private:
    std::string m_Content;
};

// 样式节点
class StyleNode : public ASTNode {
public:
    enum StyleType {
        GLOBAL,
        LOCAL,
        INLINE
    };
    
    explicit StyleNode(StyleType type) : m_Type(type) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    StyleType GetType() const { return m_Type; }
    
    void AddRule(const std::string& selector, const std::string& properties) {
        m_Rules.push_back({selector, properties});
    }
    
    void AddInlineProperty(const std::string& property, const std::string& value) {
        m_InlineProperties[property] = value;
    }
    
    // 无值属性支持
    void AddNoValueProperty(const std::string& property) {
        m_NoValueProperties.push_back(property);
    }
    
    const std::vector<std::pair<std::string, std::string>>& GetRules() const {
        return m_Rules;
    }
    
    const std::unordered_map<std::string, std::string>& GetInlineProperties() const {
        return m_InlineProperties;
    }
    
    const std::vector<std::string>& GetNoValueProperties() const {
        return m_NoValueProperties;
    }
    
    void SetAutoClassName(const std::string& className) { m_AutoClassName = className; }
    void SetAutoId(const std::string& id) { m_AutoId = id; }
    
    const std::string& GetAutoClassName() const { return m_AutoClassName; }
    const std::string& GetAutoId() const { return m_AutoId; }

private:
    StyleType m_Type;
    std::vector<std::pair<std::string, std::string>> m_Rules;
    std::unordered_map<std::string, std::string> m_InlineProperties;
    std::vector<std::string> m_NoValueProperties;
    std::string m_AutoClassName;
    std::string m_AutoId;
};

// 脚本节点
class ScriptNode : public ASTNode {
public:
    enum ScriptType {
        GLOBAL,
        LOCAL
    };
    
    explicit ScriptNode(ScriptType type) : m_Type(type) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    ScriptType GetType() const { return m_Type; }
    
    void SetContent(const std::string& content) { m_Content = content; }
    const std::string& GetContent() const { return m_Content; }
    
    void SetHasCHTLJS(bool has) { m_HasCHTLJS = has; }
    bool HasCHTLJS() const { return m_HasCHTLJS; }

private:
    ScriptType m_Type;
    std::string m_Content;
    bool m_HasCHTLJS = false;
};

// 注释节点
class CommentNode : public ASTNode {
public:
    enum CommentType {
        SINGLE_LINE,    // //
        MULTI_LINE,     // /* */
        HTML_COMMENT    // --
    };
    
    CommentNode(CommentType type, const std::string& content)
        : m_Type(type), m_Content(content) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    CommentType GetType() const { return m_Type; }
    const std::string& GetContent() const { return m_Content; }

private:
    CommentType m_Type;
    std::string m_Content;
};

// 模板节点
class TemplateNode : public ASTNode {
public:
    enum TemplateType {
        STYLE,
        ELEMENT,
        VAR
    };
    
    TemplateNode(TemplateType type, const std::string& name)
        : m_Type(type), m_Name(name) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    TemplateType GetType() const { return m_Type; }
    const std::string& GetName() const { return m_Name; }
    
    void AddParameter(const std::string& param) {
        m_Parameters.push_back(param);
    }

    void AddParameter(const std::string& param, const std::string& defaultValue) {
        m_Parameters.push_back(param);
        m_ParameterDefaults[param] = defaultValue;
    }

    void AddStyleProperty(const std::string& key, const std::string& value) {
        m_StyleProperties[key] = value;
    }

    void SetVarContent(const std::string& content) {
        m_VarContent = content;
    }
    
    const std::vector<std::string>& GetParameters() const {
        return m_Parameters;
    }
    
    const std::unordered_map<std::string, std::string>& GetParameterDefaults() const {
        return m_ParameterDefaults;
    }
    
    const std::unordered_map<std::string, std::string>& GetStyleProperties() const {
        return m_StyleProperties;
    }
    
    const std::string& GetVarContent() const {
        return m_VarContent;
    }

private:
    TemplateType m_Type;
    std::string m_Name;
    std::vector<std::string> m_Parameters;
    std::unordered_map<std::string, std::string> m_ParameterDefaults;
    std::unordered_map<std::string, std::string> m_StyleProperties;
    std::string m_VarContent;
};

// 自定义节点
class CustomNode : public ASTNode {
public:
    enum CustomType {
        STYLE,
        ELEMENT,
        VAR
    };
    
    CustomNode(CustomType type, const std::string& name)
        : m_Type(type), m_Name(name) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    CustomType GetType() const { return m_Type; }
    const std::string& GetName() const { return m_Name; }
    
    // 特例化支持
    void SetSpecialization(bool spec) { m_IsSpecialization = spec; }
    bool IsSpecialization() const { return m_IsSpecialization; }
    
    // 无值样式组支持
    void SetHasNoValueProperties(bool has) { m_HasNoValueProperties = has; }
    bool HasNoValueProperties() const { return m_HasNoValueProperties; }

private:
    CustomType m_Type;
    std::string m_Name;
    bool m_IsSpecialization = false;
    bool m_HasNoValueProperties = false;
};

// 继承节点
class InheritNode : public ASTNode {
public:
    explicit InheritNode(const std::string& base, const std::string& qualifier = "")
        : m_Base(base), m_Qualifier(qualifier) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    const std::string& GetBase() const { return m_Base; }
    const std::string& GetQualifier() const { return m_Qualifier; }
    
    // 显性继承支持
    void SetExplicit(bool exp) { m_IsExplicit = exp; }
    bool IsExplicit() const { return m_IsExplicit; }

private:
    std::string m_Base;
    std::string m_Qualifier;
    bool m_IsExplicit = false;
};

// 删除节点
class DeleteNode : public ASTNode {
public:
    enum DeleteType {
        PROPERTY,       // delete property
        INHERITANCE,    // delete @Style Base
        ELEMENT,        // delete element
        PRECISE         // 精确删除
    };
    
    DeleteNode(DeleteType type, const std::string& target = "")
        : m_Type(type), m_Target(target) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    DeleteType GetType() const { return m_Type; }
    const std::string& GetTarget() const { return m_Target; }
    
    // 多目标删除
    void AddTarget(const std::string& target) {
        m_Targets.push_back(target);
    }
    
    const std::vector<std::string>& GetTargets() const {
        return m_Targets;
    }
    
    // 添加额外的删除目标
    void AddAdditionalTarget(const std::string& target) {
        m_AdditionalTargets.push_back(target);
    }
    
    const std::vector<std::string>& GetAdditionalTargets() const {
        return m_AdditionalTargets;
    }

private:
    DeleteType m_Type;
    std::string m_Target;
    std::vector<std::string> m_Targets;
    std::vector<std::string> m_AdditionalTargets;
};

// 插入节点
class InsertNode : public ASTNode {
public:
    enum Position {
        BEFORE,
        AFTER,
        REPLACE,
        AT_TOP,
        AT_BOTTOM,
        AT_INDEX
    };
    
    InsertNode(Position pos, const std::string& target = "")
        : m_Position(pos), m_Target(target) {}
    
    InsertNode(int index) : m_Position(AT_INDEX), m_Index(index) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    Position GetPosition() const { return m_Position; }
    const std::string& GetTarget() const { return m_Target; }
    int GetIndex() const { return m_Index; }

private:
    Position m_Position;
    std::string m_Target;
    int m_Index = -1;
};

// 变量节点
class VarNode : public ASTNode {
public:
    explicit VarNode(const std::string& name) : m_Name(name) {}
    
    VarNode(const std::string& name, const std::string& value) : m_Name(name), m_Value(value) {}
    void Accept(ASTVisitor* visitor) override;
    
    const std::string& GetName() const { return m_Name; }
    
    void SetValue(const std::string& value) { m_Value = value; }
    const std::string& GetValue() const { return m_Value; }
    
    void SetExpression(std::shared_ptr<ASTNode> expr) { m_Expression = expr; }
    std::shared_ptr<ASTNode> GetExpression() const { return m_Expression; }

private:
    std::string m_Name;
    std::string m_Value;
    std::shared_ptr<ASTNode> m_Expression;
};

// 变量调用节点
class VarCallNode : public ASTNode {
public:
    // 双参数构造函数：用于变量组.变量名的情况
    VarCallNode(const std::string& varGroup, const std::string& varName)
        : m_VarGroup(varGroup), m_VarName(varName) {}
    
    // 单参数构造函数：用于单独变量名的情况
    explicit VarCallNode(const std::string& varName)
        : m_VarGroup(""), m_VarName(varName) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    const std::string& GetVarGroup() const { return m_VarGroup; }
    const std::string& GetVarName() const { return m_VarName; }
    bool HasVarGroup() const { return !m_VarGroup.empty(); }
    
    // 特例化支持
    void SetOverrideValue(const std::string& value) { m_OverrideValue = value; }
    bool HasOverride() const { return !m_OverrideValue.empty(); }
    const std::string& GetOverrideValue() const { return m_OverrideValue; }

private:
    std::string m_VarGroup;
    std::string m_VarName;
    std::string m_OverrideValue;
};

// 导入节点
class ImportNode : public ASTNode {
public:
    enum ImportType {
        HTML,
        STYLE,
        JAVASCRIPT,
        CHTL,
        CJMOD,
        CONFIG,
        AUTO,
        NORMAL,    // 用于普通导入
        ELEMENT,   // 用于导入元素
        VAR        // 用于导入变量
    };
    
    ImportNode(const std::string& path, ImportType type = AUTO)
        : m_Path(path), m_Type(type) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    const std::string& GetPath() const { return m_Path; }
    ImportType GetType() const { return m_Type; }
    
    void SetAlias(const std::string& alias) { m_Alias = alias; }
    const std::string& GetAlias() const { return m_Alias; }
    
    void SetFromClause(const std::string& from) { m_FromClause = from; }
    const std::string& GetFromClause() const { return m_FromClause; }
    
    void AddImportItem(const std::string& item) {
        m_ImportItems.push_back(item);
    }
    
    const std::vector<std::string>& GetImportItems() const {
        return m_ImportItems;
    }
    
    // 添加导入目标（用于选择性导入）
    void AddTarget(const std::string& target) {
        m_Targets.push_back(target);
    }
    
    const std::vector<std::string>& GetTargets() const {
        return m_Targets;
    }

private:
    std::string m_Path;
    ImportType m_Type;
    std::string m_Alias;
    std::string m_FromClause;
    std::vector<std::string> m_ImportItems;
    std::vector<std::string> m_Targets;
};

// 命名空间节点
class NamespaceNode : public ASTNode {
public:
    explicit NamespaceNode(const std::string& name) : m_Name(name) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    const std::string& GetName() const { return m_Name; }
    
    void SetFromClause(const std::string& from) { m_FromClause = from; }
    const std::string& GetFromClause() const { return m_FromClause; }

private:
    std::string m_Name;
    std::string m_FromClause;
};

// 配置节点
class ConfigurationNode : public ASTNode {
public:
    ConfigurationNode() = default;
    explicit ConfigurationNode(const std::string& name) : m_Name(name) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    const std::string& GetName() const { return m_Name; }
    
    void AddConfig(const std::string& key, const std::string& value) {
        m_Configs[key] = value;
    }
    
    const std::unordered_map<std::string, std::string>& GetConfigs() const {
        return m_Configs;
    }
    
    // Name配置块
    void AddNameConfig(const std::string& key, const std::vector<std::string>& values) {
        m_NameConfigs[key] = values;
    }
    
    const std::unordered_map<std::string, std::vector<std::string>>& GetNameConfigs() const {
        return m_NameConfigs;
    }
    
    // OriginType配置块
    void AddOriginType(const std::string& key, const std::string& value) {
        m_OriginTypes[key] = value;
    }
    
    const std::unordered_map<std::string, std::string>& GetOriginTypes() const {
        return m_OriginTypes;
    }

private:
    std::string m_Name;
    std::unordered_map<std::string, std::string> m_Configs;
    std::unordered_map<std::string, std::vector<std::string>> m_NameConfigs;
    std::unordered_map<std::string, std::string> m_OriginTypes;
};

// 原始嵌入节点
class OriginNode : public ASTNode {
public:
    enum OriginType {
        HTML,
        STYLE,
        JAVASCRIPT,
        CUSTOM
    };
    
    OriginNode(OriginType type, const std::string& content)
        : m_Type(type), m_Content(content) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    OriginType GetType() const { return m_Type; }
    const std::string& GetContent() const { return m_Content; }
    void SetContent(const std::string& content) { m_Content = content; }
    
    void SetName(const std::string& name) { m_Name = name; }
    const std::string& GetName() const { return m_Name; }
    
    void SetCustomTypeName(const std::string& typeName) { m_CustomTypeName = typeName; }
    const std::string& GetCustomTypeName() const { return m_CustomTypeName; }
    
    // 为了兼容性，添加SetCustomType别名
    void SetCustomType(const std::string& typeName) { SetCustomTypeName(typeName); }

private:
    OriginType m_Type;
    std::string m_Content;
    std::string m_Name;
    std::string m_CustomTypeName;
};

// 约束节点
class ExceptNode : public ASTNode {
public:
    enum ExceptType {
        PRECISE,    // except span, [Custom] @Element Box
        TYPE,       // except @Html, [Template]
        GLOBAL      // 在命名空间内的全局约束
    };
    
    explicit ExceptNode(ExceptType type = PRECISE) : m_Type(type) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    ExceptType GetType() const { return m_Type; }
    
    void AddTarget(const std::string& target) {
        m_Targets.push_back(target);
    }
    
    const std::vector<std::string>& GetTargets() const {
        return m_Targets;
    }

private:
    ExceptType m_Type;
    std::vector<std::string> m_Targets;
};

// 属性节点（用于更精确的属性处理）
class AttributeNode : public ASTNode {
public:
    AttributeNode(const std::string& name, const std::string& value)
        : m_Name(name), m_Value(value) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    const std::string& GetName() const { return m_Name; }
    const std::string& GetValue() const { return m_Value; }

private:
    std::string m_Name;
    std::string m_Value;
};

// 访问者接口
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    
    virtual void VisitDocument(DocumentNode* node) = 0;
    virtual void VisitElement(ElementNode* node) = 0;
    virtual void VisitText(TextNode* node) = 0;
    virtual void VisitStyle(StyleNode* node) = 0;
    virtual void VisitScript(ScriptNode* node) = 0;
    virtual void VisitComment(CommentNode* node) = 0;
    virtual void VisitTemplate(TemplateNode* node) = 0;
    virtual void VisitCustom(CustomNode* node) = 0;
    virtual void VisitAttribute(AttributeNode* node) = 0;
    virtual void VisitVar(VarNode* node) = 0;
    virtual void VisitVarCall(VarCallNode* node) = 0;
    virtual void VisitImport(ImportNode* node) = 0;
    virtual void VisitNamespace(NamespaceNode* node) = 0;
    virtual void VisitConfiguration(ConfigurationNode* node) = 0;
    virtual void VisitOrigin(OriginNode* node) = 0;
    virtual void VisitInherit(InheritNode* node) = 0;
    virtual void VisitDelete(DeleteNode* node) = 0;
    virtual void VisitInsert(InsertNode* node) = 0;
    virtual void VisitExcept(ExceptNode* node) = 0;
};

} // namespace v3
} // namespace ast
} // namespace chtl