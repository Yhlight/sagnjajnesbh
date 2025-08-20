#pragma once

#include <memory>

namespace chtl {
namespace simple {

// 前向声明
struct ASTNode;
class DocumentNode;
class ElementNode;
class TextNode;
class StyleNode;
class ScriptNode;
class TemplateNode;
class CustomNode;
class AttributeNode;

/**
 * AST 访问者接口
 * 使用双重分派模式实现类型安全的 AST 遍历
 */
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    
    // 访问方法
    virtual void VisitDocument(DocumentNode* node) = 0;
    virtual void VisitElement(ElementNode* node) = 0;
    virtual void VisitText(TextNode* node) = 0;
    virtual void VisitStyle(StyleNode* node) = 0;
    virtual void VisitScript(ScriptNode* node) = 0;
    virtual void VisitTemplate(TemplateNode* node) = 0;
    virtual void VisitCustom(CustomNode* node) = 0;
    virtual void VisitAttribute(AttributeNode* node) = 0;
};

/**
 * 默认访问者实现
 * 提供默认的递归遍历行为
 */
class DefaultASTVisitor : public ASTVisitor {
public:
    void VisitDocument(DocumentNode* node) override;
    void VisitElement(ElementNode* node) override;
    void VisitText(TextNode* node) override;
    void VisitStyle(StyleNode* node) override;
    void VisitScript(ScriptNode* node) override;
    void VisitTemplate(TemplateNode* node) override;
    void VisitCustom(CustomNode* node) override;
    void VisitAttribute(AttributeNode* node) override;
    
protected:
    // 辅助方法：访问所有子节点
    void VisitChildren(ASTNode* node);
};

} // namespace simple
} // namespace chtl