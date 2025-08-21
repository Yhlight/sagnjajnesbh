#pragma once

#include "ASTVisitor.h"
#include <string>
#include <sstream>
#include <vector>

namespace chtl {
namespace simple {

/**
 * HTML 生成器
 * 使用访问者模式遍历 AST 并生成 HTML
 */
class HTMLGenerator : public DefaultASTVisitor {
public:
    HTMLGenerator();
    
    /**
     * 生成 HTML
     */
    std::string Generate(ASTNode* root);
    
    /**
     * 配置选项
     */
    void SetPrettyPrint(bool pretty) { m_PrettyPrint = pretty; }
    void SetIndentSize(int size) { m_IndentSize = size; }
    void SetGenerateFullDocument(bool full) { m_GenerateFullDocument = full; }
    
    // 访问者方法
    void VisitDocument(DocumentNode* node) override;
    void VisitElement(ElementNode* node) override;
    void VisitText(TextNode* node) override;
    void VisitStyle(StyleNode* node) override;
    void VisitScript(ScriptNode* node) override;
    void VisitTemplate(TemplateNode* node) override;
    void VisitCustom(CustomNode* node) override;
    
private:
    // 辅助方法
    void WriteIndent();
    void Write(const std::string& text);
    void WriteLine(const std::string& text = "");
    void IncreaseIndent() { m_CurrentIndent++; }
    void DecreaseIndent() { if (m_CurrentIndent > 0) m_CurrentIndent--; }
    bool IsInlineElement(const std::string& tag);
    bool IsSelfClosingTag(const std::string& tag);
    
    // 成员变量
    std::stringstream m_Output;
    std::stringstream m_Styles;
    std::stringstream m_Scripts;
    bool m_PrettyPrint;
    int m_IndentSize;
    int m_CurrentIndent;
    bool m_GenerateFullDocument;
};

} // namespace simple
} // namespace chtl