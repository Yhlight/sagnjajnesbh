#include "../src/chtl/parser/CHTLParserV2.h"
#include "../src/chtl/ast/CHTLASTNodes.h"
#include <iostream>
#include <string>

using namespace chtl;
using namespace chtl::ast;

// 简单的AST打印访问者
class ASTPrinter : public ASTVisitor {
public:
    ASTPrinter() : m_Indent(0) {}
    
    void VisitDocument(DocumentNode* node) override {
        PrintIndent();
        std::cout << "Document\n";
        m_Indent++;
        for (const auto& child : node->GetChildren()) {
            child->Accept(this);
        }
        m_Indent--;
    }
    
    void VisitElement(ElementNode* node) override {
        PrintIndent();
        std::cout << "Element: " << node->GetTagName();
        
        // 打印属性
        auto& attrs = node->GetAttributes();
        if (!attrs.empty()) {
            std::cout << " [";
            bool first = true;
            for (const auto& attr : attrs) {
                if (!first) std::cout << ", ";
                std::cout << attr.first << "=\"" << attr.second << "\"";
                first = false;
            }
            std::cout << "]";
        }
        std::cout << "\n";
        
        m_Indent++;
        for (const auto& child : node->GetChildren()) {
            child->Accept(this);
        }
        m_Indent--;
    }
    
    void VisitText(TextNode* node) override {
        PrintIndent();
        std::cout << "Text: \"" << node->GetContent() << "\"\n";
    }
    
    void VisitStyle(StyleNode* node) override {
        PrintIndent();
        std::cout << "Style (" << (node->GetType() == StyleNode::LOCAL ? "local" : "global") << ")\n";
        
        m_Indent++;
        
        // 打印内联样式
        auto& inline_props = node->GetInlineProperties();
        if (!inline_props.empty()) {
            PrintIndent();
            std::cout << "Inline properties:\n";
            m_Indent++;
            for (const auto& prop : inline_props) {
                PrintIndent();
                std::cout << prop.first << ": " << prop.second << "\n";
            }
            m_Indent--;
        }
        
        // 打印CSS规则
        auto& rules = node->GetRules();
        if (!rules.empty()) {
            PrintIndent();
            std::cout << "CSS rules:\n";
            m_Indent++;
            for (const auto& rule : rules) {
                PrintIndent();
                std::cout << rule.first << " { " << rule.second << " }\n";
            }
            m_Indent--;
        }
        
        m_Indent--;
    }
    
    void VisitScript(ScriptNode* node) override {
        PrintIndent();
        std::cout << "Script (" << (node->GetType() == ScriptNode::LOCAL ? "local" : "global") << ")\n";
        m_Indent++;
        PrintIndent();
        std::cout << "Content: " << node->GetContent() << "\n";
        m_Indent--;
    }
    
    void VisitComment(CommentNode* node) override {
        PrintIndent();
        std::cout << "Comment (";
        switch (node->GetType()) {
            case CommentNode::SINGLE_LINE: std::cout << "//"; break;
            case CommentNode::MULTI_LINE: std::cout << "/* */"; break;
            case CommentNode::HTML_COMMENT: std::cout << "--"; break;
        }
        std::cout << "): \"" << node->GetContent() << "\"\n";
    }
    
    // 其他访问方法的简单实现
    void VisitTemplate(TemplateNode* node) override {
        PrintIndent();
        std::cout << "Template: " << node->GetName() << "\n";
    }
    
    void VisitCustom(CustomNode* node) override {}
    void VisitVar(VarNode* node) override {}
    void VisitInherit(InheritNode* node) override {}
    void VisitDelete(DeleteNode* node) override {}
    void VisitInsert(InsertNode* node) override {}
    void VisitImport(ImportNode* node) override {}
    void VisitNamespace(NamespaceNode* node) override {}
    void VisitConfiguration(ConfigurationNode* node) override {}
    void VisitOrigin(OriginNode* node) override {}
    void VisitExcept(ExceptNode* node) override {}
    
private:
    void PrintIndent() {
        for (int i = 0; i < m_Indent; i++) {
            std::cout << "  ";
        }
    }
    
    int m_Indent;
};

void TestBasicCHTL() {
    std::cout << "=== Test Basic CHTL ===\n";
    
    std::string code = R"(
// CHTL 注释
div {
    id: main;
    class: container;
    
    h1 {
        text { Hello CHTL! }
    }
    
    p {
        text { This is a paragraph with CHTL syntax. }
    }
}
)";
    
    CHTLParserV2 parser;
    auto ast = parser.Parse(code);
    
    if (parser.HasErrors()) {
        std::cout << "Parse errors:\n";
        for (const auto& error : parser.GetErrors()) {
            std::cout << "  " << error << "\n";
        }
    } else {
        ASTPrinter printer;
        ast->Accept(&printer);
    }
    
    std::cout << "\n";
}

void TestLocalStyle() {
    std::cout << "=== Test Local Style ===\n";
    
    std::string code = R"(
div {
    style {
        width: 100px;
        height: 200px;
        
        .box {
            background: red;
        }
        
        &:hover {
            background: blue;
        }
    }
    
    text { Styled div }
}
)";
    
    CHTLParserV2 parser;
    auto ast = parser.Parse(code);
    
    if (parser.HasErrors()) {
        std::cout << "Parse errors:\n";
        for (const auto& error : parser.GetErrors()) {
            std::cout << "  " << error << "\n";
        }
    } else {
        ASTPrinter printer;
        ast->Accept(&printer);
    }
    
    std::cout << "\n";
}

void TestCEEquivalence() {
    std::cout << "=== Test CE Equivalence ===\n";
    
    std::string code = R"(
div {
    // 使用冒号
    id: test1;
    
    // 使用等号（CE对等式）
    class = container;
    
    style {
        // 样式中也支持等号
        width = 100px;
        height: 200px;
    }
}
)";
    
    CHTLParserV2 parser;
    auto ast = parser.Parse(code);
    
    if (parser.HasErrors()) {
        std::cout << "Parse errors:\n";
        for (const auto& error : parser.GetErrors()) {
            std::cout << "  " << error << "\n";
        }
    } else {
        ASTPrinter printer;
        ast->Accept(&printer);
    }
    
    std::cout << "\n";
}

void TestUnquotedLiterals() {
    std::cout << "=== Test Unquoted Literals ===\n";
    
    std::string code = R"(
div {
    // 无引号字面量
    id: main-container;
    class: box_style;
    
    text {
        这是一段无引号的文本
    }
    
    style {
        color: red;
        font-size: 16px;
    }
}
)";
    
    CHTLParserV2 parser;
    auto ast = parser.Parse(code);
    
    if (parser.HasErrors()) {
        std::cout << "Parse errors:\n";
        for (const auto& error : parser.GetErrors()) {
            std::cout << "  " << error << "\n";
        }
    } else {
        ASTPrinter printer;
        ast->Accept(&printer);
    }
    
    std::cout << "\n";
}

int main() {
    std::cout << "CHTL V2 Parser Test\n";
    std::cout << "===================\n\n";
    
    TestBasicCHTL();
    TestLocalStyle();
    TestCEEquivalence();
    TestUnquotedLiterals();
    
    return 0;
}