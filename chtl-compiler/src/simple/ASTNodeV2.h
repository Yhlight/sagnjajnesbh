#pragma once

#include "ASTVisitor.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace chtl {
namespace simple {

/**
 * 增强的 AST 节点基类
 * 支持访问者模式
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
 * 文档节点
 */
class DocumentNode : public ASTNode {
public:
    void Accept(ASTVisitor* visitor) override {
        visitor->VisitDocument(this);
    }
};

/**
 * 元素节点
 */
class ElementNode : public ASTNode {
public:
    explicit ElementNode(const std::string& tag) : m_TagName(tag) {}
    
    void Accept(ASTVisitor* visitor) override {
        visitor->VisitElement(this);
    }
    
    const std::string& GetTagName() const { return m_TagName; }
    
    void AddAttribute(const std::string& name, const std::string& value) {
        m_Attributes[name] = value;
    }
    
    const std::unordered_map<std::string, std::string>& GetAttributes() const {
        return m_Attributes;
    }
    
private:
    std::string m_TagName;
    std::unordered_map<std::string, std::string> m_Attributes;
};

/**
 * 文本节点
 */
class TextNode : public ASTNode {
public:
    explicit TextNode(const std::string& text) : m_Text(text) {}
    
    void Accept(ASTVisitor* visitor) override {
        visitor->VisitText(this);
    }
    
    const std::string& GetText() const { return m_Text; }
    
private:
    std::string m_Text;
};

/**
 * 样式节点
 */
class StyleNode : public ASTNode {
public:
    explicit StyleNode(const std::string& css) : m_CSS(css) {}
    
    void Accept(ASTVisitor* visitor) override {
        visitor->VisitStyle(this);
    }
    
    const std::string& GetCSS() const { return m_CSS; }
    
private:
    std::string m_CSS;
};

/**
 * 脚本节点
 */
class ScriptNode : public ASTNode {
public:
    explicit ScriptNode(const std::string& js) : m_JavaScript(js) {}
    
    void Accept(ASTVisitor* visitor) override {
        visitor->VisitScript(this);
    }
    
    const std::string& GetJavaScript() const { return m_JavaScript; }
    
private:
    std::string m_JavaScript;
};

/**
 * 模板节点
 */
class TemplateNode : public ASTNode {
public:
    enum TemplateType {
        STYLE_TEMPLATE,
        ELEMENT_TEMPLATE,
        VAR_TEMPLATE
    };
    
    TemplateNode(TemplateType type, const std::string& name) 
        : m_Type(type), m_Name(name) {}
    
    void Accept(ASTVisitor* visitor) override {
        visitor->VisitTemplate(this);
    }
    
    TemplateType GetType() const { return m_Type; }
    const std::string& GetName() const { return m_Name; }
    
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
    explicit CustomNode(const std::string& name) : m_Name(name) {}
    
    void Accept(ASTVisitor* visitor) override {
        visitor->VisitCustom(this);
    }
    
    const std::string& GetName() const { return m_Name; }
    
    void AddProperty(const std::string& name, const std::string& value) {
        m_Properties[name] = value;
    }
    
    const std::unordered_map<std::string, std::string>& GetProperties() const {
        return m_Properties;
    }
    
private:
    std::string m_Name;
    std::unordered_map<std::string, std::string> m_Properties;
};

/**
 * 属性节点
 */
class AttributeNode : public ASTNode {
public:
    AttributeNode(const std::string& name, const std::string& value)
        : m_Name(name), m_Value(value) {}
    
    void Accept(ASTVisitor* visitor) override {
        visitor->VisitAttribute(this);
    }
    
    const std::string& GetName() const { return m_Name; }
    const std::string& GetValue() const { return m_Value; }
    
private:
    std::string m_Name;
    std::string m_Value;
};

} // namespace simple
} // namespace chtl