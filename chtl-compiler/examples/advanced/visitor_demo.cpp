#include "../../src/simple/ASTNodeV2.h"
#include "../../src/simple/HTMLGenerator.h"
#include <iostream>
#include <algorithm>
#include <cctype>

using namespace chtl::simple;

/**
 * 统计访问器
 * 展示如何使用访问者模式分析 AST
 */
class StatisticsVisitor : public DefaultASTVisitor {
public:
    void VisitElement(ElementNode* node) override {
        m_ElementCount++;
        m_TagCounts[node->GetTagName()]++;
        DefaultASTVisitor::VisitElement(node);
    }
    
    void VisitText(TextNode* node) override {
        m_TextCount++;
        m_TotalTextLength += node->GetText().length();
    }
    
    void PrintStatistics() {
        std::cout << "=== AST Statistics ===\n";
        std::cout << "Total elements: " << m_ElementCount << "\n";
        std::cout << "Total text nodes: " << m_TextCount << "\n";
        std::cout << "Total text length: " << m_TotalTextLength << "\n";
        std::cout << "\nTag frequency:\n";
        for (const auto& pair : m_TagCounts) {
            std::cout << "  " << pair.first << ": " << pair.second << "\n";
        }
    }
    
private:
    int m_ElementCount = 0;
    int m_TextCount = 0;
    size_t m_TotalTextLength = 0;
    std::unordered_map<std::string, int> m_TagCounts;
};

/**
 * 转换访问器
 * 展示如何使用访问者模式转换 AST
 */
class TransformVisitor : public DefaultASTVisitor {
public:
    void VisitElement(ElementNode* node) override {
        // 将所有 b 标签转换为 strong
        if (node->GetTagName() == "b") {
            // 在实际实现中，这里应该修改节点
            std::cout << "Transform: <b> -> <strong>\n";
        }
        DefaultASTVisitor::VisitElement(node);
    }
    
    void VisitText(TextNode* node) override {
        // 将文本转换为大写
        std::string upper = node->GetText();
        std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
        std::cout << "Transform text: \"" << node->GetText() 
                  << "\" -> \"" << upper << "\"\n";
    }
};

int main() {
    // 创建示例 AST
    auto doc = std::make_shared<DocumentNode>();
    
    auto div = std::make_shared<ElementNode>("div");
    div->AddAttribute("class", "container");
    
    auto h1 = std::make_shared<ElementNode>("h1");
    h1->AddChild(std::make_shared<TextNode>("Hello World"));
    
    auto p = std::make_shared<ElementNode>("p");
    p->AddChild(std::make_shared<TextNode>("This is a "));
    
    auto b = std::make_shared<ElementNode>("b");
    b->AddChild(std::make_shared<TextNode>("bold"));
    p->AddChild(b);
    
    p->AddChild(std::make_shared<TextNode>(" text."));
    
    div->AddChild(h1);
    div->AddChild(p);
    doc->AddChild(div);
    
    // 使用统计访问器
    std::cout << "=== Statistics Visitor Demo ===\n";
    StatisticsVisitor stats;
    doc->Accept(&stats);
    stats.PrintStatistics();
    
    std::cout << "\n=== Transform Visitor Demo ===\n";
    TransformVisitor transform;
    doc->Accept(&transform);
    
    std::cout << "\n=== HTML Generation Demo ===\n";
    HTMLGenerator generator;
    generator.SetGenerateFullDocument(false);
    generator.SetPrettyPrint(true);
    
    std::string html = generator.Generate(doc.get());
    std::cout << "Generated HTML:\n" << html << std::endl;
    
    return 0;
}