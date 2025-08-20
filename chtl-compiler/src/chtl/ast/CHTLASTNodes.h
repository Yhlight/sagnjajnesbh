#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace chtl {
namespace ast {

// 前向声明
class ASTVisitor;

/**
 * AST 节点基类
 */
class ASTNode {
public:
    virtual ~ASTNode() = default;
    
    // 访问者模式
    virtual void Accept(ASTVisitor* visitor) = 0;
    
    // 子节点管理
    void AddChild(std::shared_ptr<ASTNode> child) {
        m_Children.push_back(child);
    }
    
    const std::vector<std::shared_ptr<ASTNode>>& GetChildren() const {
        return m_Children;
    }
    
    // 位置信息
    void SetLocation(size_t line, size_t column) {
        m_Line = line;
        m_Column = column;
    }
    
    size_t GetLine() const { return m_Line; }
    size_t GetColumn() const { return m_Column; }
    
protected:
    std::vector<std::shared_ptr<ASTNode>> m_Children;
    size_t m_Line = 0;
    size_t m_Column = 0;
};

/**
 * 文档根节点
 */
class DocumentNode : public ASTNode {
public:
    void Accept(ASTVisitor* visitor) override;
};

/**
 * 注释节点
 */
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

/**
 * 文本节点
 */
class TextNode : public ASTNode {
public:
    explicit TextNode(const std::string& content) : m_Content(content) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    const std::string& GetContent() const { return m_Content; }
    void SetContent(const std::string& content) { m_Content = content; }
    
private:
    std::string m_Content;
};

/**
 * 元素节点
 */
class ElementNode : public ASTNode {
public:
    explicit ElementNode(const std::string& tagName) : m_TagName(tagName) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    const std::string& GetTagName() const { return m_TagName; }
    
    // 属性管理
    void AddAttribute(const std::string& name, const std::string& value) {
        m_Attributes[name] = value;
    }
    
    bool HasAttribute(const std::string& name) const {
        return m_Attributes.find(name) != m_Attributes.end();
    }
    
    const std::string& GetAttribute(const std::string& name) const {
        static const std::string empty;
        auto it = m_Attributes.find(name);
        return it != m_Attributes.end() ? it->second : empty;
    }
    
    const std::unordered_map<std::string, std::string>& GetAttributes() const {
        return m_Attributes;
    }
    
private:
    std::string m_TagName;
    std::unordered_map<std::string, std::string> m_Attributes;
};

/**
 * 样式块节点
 */
class StyleNode : public ASTNode {
public:
    enum StyleType {
        GLOBAL,     // 全局样式块
        LOCAL,      // 局部样式块
        INLINE      // 内联样式
    };
    
    explicit StyleNode(StyleType type = GLOBAL) : m_Type(type) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    StyleType GetType() const { return m_Type; }
    
    // 添加样式规则
    void AddRule(const std::string& selector, const std::string& properties) {
        m_Rules.push_back({selector, properties});
    }
    
    // 添加内联样式属性
    void AddInlineProperty(const std::string& property, const std::string& value) {
        m_InlineProperties[property] = value;
    }
    
    const std::vector<std::pair<std::string, std::string>>& GetRules() const {
        return m_Rules;
    }
    
    const std::unordered_map<std::string, std::string>& GetInlineProperties() const {
        return m_InlineProperties;
    }
    
    // 自动生成的类名/ID
    void SetAutoClassName(const std::string& className) { m_AutoClassName = className; }
    void SetAutoId(const std::string& id) { m_AutoId = id; }
    
    const std::string& GetAutoClassName() const { return m_AutoClassName; }
    const std::string& GetAutoId() const { return m_AutoId; }
    
private:
    StyleType m_Type;
    std::vector<std::pair<std::string, std::string>> m_Rules;
    std::unordered_map<std::string, std::string> m_InlineProperties;
    std::string m_AutoClassName;
    std::string m_AutoId;
};

/**
 * 脚本块节点
 */
class ScriptNode : public ASTNode {
public:
    enum ScriptType {
        GLOBAL,     // 全局脚本块
        LOCAL       // 局部脚本块
    };
    
    explicit ScriptNode(ScriptType type = GLOBAL) : m_Type(type) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    ScriptType GetType() const { return m_Type; }
    
    void SetContent(const std::string& content) { m_Content = content; }
    const std::string& GetContent() const { return m_Content; }
    
private:
    ScriptType m_Type;
    std::string m_Content;
};

/**
 * 模板节点
 */
class TemplateNode : public ASTNode {
public:
    enum TemplateType {
        STYLE,      // @Style
        ELEMENT,    // @Element
        VAR         // @Var
    };
    
    TemplateNode(TemplateType type, const std::string& name)
        : m_Type(type), m_Name(name) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    TemplateType GetType() const { return m_Type; }
    const std::string& GetName() const { return m_Name; }
    
    // 参数管理
    void AddParameter(const std::string& param) {
        m_Parameters.push_back(param);
    }
    
    const std::vector<std::string>& GetParameters() const {
        return m_Parameters;
    }
    
private:
    TemplateType m_Type;
    std::string m_Name;
    std::vector<std::string> m_Parameters;
};

/**
 * 自定义元素节点
 */
class CustomNode : public ASTNode {
public:
    enum CustomType {
        STYLE,      // @Style
        ELEMENT,    // @Element
        VAR         // @Var
    };
    
    CustomNode(CustomType type, const std::string& name)
        : m_Type(type), m_Name(name) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    CustomType GetType() const { return m_Type; }
    const std::string& GetName() const { return m_Name; }
    
private:
    CustomType m_Type;
    std::string m_Name;
};

/**
 * 变量节点
 */
class VarNode : public ASTNode {
public:
    explicit VarNode(const std::string& name) : m_Name(name) {}
    
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

/**
 * 特殊化节点 - 继承
 */
class InheritNode : public ASTNode {
public:
    InheritNode(const std::string& base, const std::string& qualifier = "")
        : m_Base(base), m_Qualifier(qualifier) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    const std::string& GetBase() const { return m_Base; }
    const std::string& GetQualifier() const { return m_Qualifier; }
    
private:
    std::string m_Base;
    std::string m_Qualifier;
};

/**
 * 特殊化节点 - 删除
 */
class DeleteNode : public ASTNode {
public:
    enum DeleteType {
        PROPERTY,       // 删除属性
        INHERITANCE,    // 删除继承
        ELEMENT        // 删除元素
    };
    
    DeleteNode(DeleteType type, const std::string& target)
        : m_Type(type), m_Target(target) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    DeleteType GetType() const { return m_Type; }
    const std::string& GetTarget() const { return m_Target; }
    
private:
    DeleteType m_Type;
    std::string m_Target;
};

/**
 * 特殊化节点 - 插入
 */
class InsertNode : public ASTNode {
public:
    enum InsertPosition {
        BEFORE,
        AFTER,
        AT_INDEX
    };
    
    InsertNode(InsertPosition position, const std::string& target = "")
        : m_Position(position), m_Target(target), m_Index(-1) {}
    
    InsertNode(int index) 
        : m_Position(AT_INDEX), m_Index(index) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    InsertPosition GetPosition() const { return m_Position; }
    const std::string& GetTarget() const { return m_Target; }
    int GetIndex() const { return m_Index; }
    
private:
    InsertPosition m_Position;
    std::string m_Target;
    int m_Index;
};

/**
 * Import 节点
 */
class ImportNode : public ASTNode {
public:
    enum ImportType {
        HTML,       // @Html
        STYLE,      // @Style
        JAVASCRIPT, // @JavaScript
        CHTL,       // @Chtl
        CJMOD,      // @CJmod
        AUTO        // 自动推断
    };
    
    ImportNode(const std::string& path, ImportType type = AUTO)
        : m_Path(path), m_Type(type) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    const std::string& GetPath() const { return m_Path; }
    ImportType GetType() const { return m_Type; }
    
    // from 子句
    void SetFromClause(const std::string& from) { m_FromClause = from; }
    const std::string& GetFromClause() const { return m_FromClause; }
    
    // 导入的具体项
    void AddImportItem(const std::string& item) {
        m_ImportItems.push_back(item);
    }
    
    const std::vector<std::string>& GetImportItems() const {
        return m_ImportItems;
    }
    
private:
    std::string m_Path;
    ImportType m_Type;
    std::string m_FromClause;
    std::vector<std::string> m_ImportItems;
};

/**
 * Namespace 节点
 */
class NamespaceNode : public ASTNode {
public:
    explicit NamespaceNode(const std::string& name) : m_Name(name) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    const std::string& GetName() const { return m_Name; }
    
    // from 子句
    void SetFromClause(const std::string& from) { m_FromClause = from; }
    const std::string& GetFromClause() const { return m_FromClause; }
    
private:
    std::string m_Name;
    std::string m_FromClause;
};

/**
 * Configuration 节点
 */
class ConfigurationNode : public ASTNode {
public:
    ConfigurationNode() = default;
    
    void Accept(ASTVisitor* visitor) override;
    
    // 添加配置项
    void AddConfig(const std::string& name, const std::string& value) {
        m_Configs[name] = value;
    }
    
    const std::unordered_map<std::string, std::string>& GetConfigs() const {
        return m_Configs;
    }
    
private:
    std::unordered_map<std::string, std::string> m_Configs;
};

/**
 * Origin 节点 - 原始代码嵌入
 */
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
    
    // 自定义类型名称
    void SetCustomTypeName(const std::string& name) { m_CustomTypeName = name; }
    const std::string& GetCustomTypeName() const { return m_CustomTypeName; }
    
private:
    OriginType m_Type;
    std::string m_Content;
    std::string m_CustomTypeName;
};

/**
 * Except 节点 - 约束例外
 */
class ExceptNode : public ASTNode {
public:
    explicit ExceptNode(const std::string& constraint) : m_Constraint(constraint) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    const std::string& GetConstraint() const { return m_Constraint; }
    
private:
    std::string m_Constraint;
};

/**
 * 访问者接口
 */
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    
    virtual void VisitDocument(DocumentNode* node) = 0;
    virtual void VisitComment(CommentNode* node) = 0;
    virtual void VisitText(TextNode* node) = 0;
    virtual void VisitElement(ElementNode* node) = 0;
    virtual void VisitStyle(StyleNode* node) = 0;
    virtual void VisitScript(ScriptNode* node) = 0;
    virtual void VisitTemplate(TemplateNode* node) = 0;
    virtual void VisitCustom(CustomNode* node) = 0;
    virtual void VisitVar(VarNode* node) = 0;
    virtual void VisitInherit(InheritNode* node) = 0;
    virtual void VisitDelete(DeleteNode* node) = 0;
    virtual void VisitInsert(InsertNode* node) = 0;
    virtual void VisitImport(ImportNode* node) = 0;
    virtual void VisitNamespace(NamespaceNode* node) = 0;
    virtual void VisitConfiguration(ConfigurationNode* node) = 0;
    virtual void VisitOrigin(OriginNode* node) = 0;
    virtual void VisitExcept(ExceptNode* node) = 0;
};

} // namespace ast
} // namespace chtl