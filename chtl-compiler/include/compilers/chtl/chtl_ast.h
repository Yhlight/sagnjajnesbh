#ifndef CHTL_AST_H
#define CHTL_AST_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace chtl {

/**
 * AST节点类型
 */
enum class ASTNodeType {
    DOCUMENT,
    ELEMENT,
    TEXT,
    ATTRIBUTE,
    STYLE_BLOCK,
    SCRIPT_BLOCK,
    TEMPLATE_DEFINITION,
    CUSTOM_DEFINITION,
    IMPORT_STATEMENT,
    NAMESPACE_DECLARATION,
    CONFIGURATION_BLOCK,
    ORIGIN_BLOCK,
    VARIABLE_GROUP,
    STYLE_GROUP,
    ELEMENT_GROUP
};

/**
 * AST节点基类
 */
class CHTLASTNode {
public:
    CHTLASTNode(ASTNodeType type) : type_(type) {}
    virtual ~CHTLASTNode() = default;
    
    ASTNodeType getType() const { return type_; }
    
    // 添加子节点
    void addChild(std::shared_ptr<CHTLASTNode> child) {
        children_.push_back(child);
    }
    
    // 获取子节点
    const std::vector<std::shared_ptr<CHTLASTNode>>& getChildren() const {
        return children_;
    }
    
    // 访问者模式接口
    virtual void accept(class CHTLASTVisitor* visitor) = 0;
    
protected:
    ASTNodeType type_;
    std::vector<std::shared_ptr<CHTLASTNode>> children_;
};

/**
 * 文档节点
 */
class DocumentNode : public CHTLASTNode {
public:
    DocumentNode() : CHTLASTNode(ASTNodeType::DOCUMENT) {}
    void accept(CHTLASTVisitor* visitor) override;
};

/**
 * 元素节点
 */
class ElementNode : public CHTLASTNode {
public:
    ElementNode(const std::string& tagName) 
        : CHTLASTNode(ASTNodeType::ELEMENT), tagName_(tagName) {}
    
    const std::string& getTagName() const { return tagName_; }
    
    void addAttribute(const std::string& name, const std::string& value) {
        attributes_[name] = value;
    }
    
    const std::unordered_map<std::string, std::string>& getAttributes() const {
        return attributes_;
    }
    
    void accept(CHTLASTVisitor* visitor) override;
    
private:
    std::string tagName_;
    std::unordered_map<std::string, std::string> attributes_;
};

/**
 * 文本节点
 */
class TextNode : public CHTLASTNode {
public:
    TextNode(const std::string& content) 
        : CHTLASTNode(ASTNodeType::TEXT), content_(content) {}
    
    const std::string& getContent() const { return content_; }
    void accept(CHTLASTVisitor* visitor) override;
    
private:
    std::string content_;
};

/**
 * 样式块节点
 */
class StyleBlockNode : public CHTLASTNode {
public:
    StyleBlockNode(const std::string& content, bool isLocal = true) 
        : CHTLASTNode(ASTNodeType::STYLE_BLOCK), content_(content), isLocal_(isLocal) {}
    
    const std::string& getContent() const { return content_; }
    bool isLocal() const { return isLocal_; }
    void accept(CHTLASTVisitor* visitor) override;
    
private:
    std::string content_;
    bool isLocal_;
};

/**
 * 模板定义节点
 */
class TemplateDefinitionNode : public CHTLASTNode {
public:
    enum class TemplateType { STYLE, ELEMENT, VAR };
    
    TemplateDefinitionNode(const std::string& name, TemplateType templateType)
        : CHTLASTNode(ASTNodeType::TEMPLATE_DEFINITION), 
          name_(name), templateType_(templateType) {}
    
    const std::string& getName() const { return name_; }
    TemplateType getTemplateType() const { return templateType_; }
    void accept(CHTLASTVisitor* visitor) override;
    
private:
    std::string name_;
    TemplateType templateType_;
};

/**
 * AST访问者接口
 */
class CHTLASTVisitor {
public:
    virtual ~CHTLASTVisitor() = default;
    
    virtual void visitDocument(DocumentNode* node) = 0;
    virtual void visitElement(ElementNode* node) = 0;
    virtual void visitText(TextNode* node) = 0;
    virtual void visitStyleBlock(StyleBlockNode* node) = 0;
    virtual void visitTemplateDefinition(TemplateDefinitionNode* node) = 0;
};

} // namespace chtl

#endif // CHTL_AST_H