#pragma once

#include "ASTNode.h"
#include <string>
#include <memory>
#include <sstream>

namespace chtl {
namespace simple {

/**
 * 代码生成器
 * 将 AST 转换为 HTML
 */
class Generator {
public:
    Generator();
    
    /**
     * 生成 HTML 代码
     */
    std::string Generate(std::shared_ptr<ASTNode> ast);
    
    /**
     * 配置选项
     */
    void SetPrettyPrint(bool pretty) { m_PrettyPrint = pretty; }
    void SetIndentSize(int size) { m_IndentSize = size; }
    void SetGenerateFullDocument(bool full) { m_GenerateFullDocument = full; }
    
private:
    // 生成方法
    void GenerateNode(std::shared_ptr<ASTNode> node, std::stringstream& output, int indent);
    void GenerateElement(std::shared_ptr<ASTNode> node, std::stringstream& output, int indent);
    void GenerateText(std::shared_ptr<ASTNode> node, std::stringstream& output);
    void GenerateStyle(std::shared_ptr<ASTNode> node, std::stringstream& output);
    void GenerateScript(std::shared_ptr<ASTNode> node, std::stringstream& output);
    
    // 辅助方法
    std::string Indent(int level);
    bool IsInlineElement(const std::string& tag);
    bool IsSelfClosingTag(const std::string& tag);
    
    // 成员变量
    bool m_PrettyPrint;
    int m_IndentSize;
    bool m_GenerateFullDocument;
    
    // 收集的样式和脚本
    std::stringstream m_Styles;
    std::stringstream m_Scripts;
};

} // namespace simple
} // namespace chtl