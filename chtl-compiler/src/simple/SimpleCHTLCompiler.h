#pragma once

#include <string>
#include <vector>
#include <memory>
#include <regex>
#include <sstream>
#include <stack>

namespace chtl {
namespace simple {

/**
 * 简化版 CHTL 编译器
 * 目标：快速实现基础功能，让它能工作
 * 
 * 支持的语法：
 *   基本元素: div { ... }
 *   文本内容: text { Hello World }
 *   属性: class: "container"; id: "main";
 *   注释: 单行注释(//) 和 多行注释
 *   嵌套结构
 */
class SimpleCHTLCompiler {
public:
    SimpleCHTLCompiler();
    
    /**
     * 编译 CHTL 代码为 HTML
     */
    std::string Compile(const std::string& chtlCode);
    
    /**
     * 获取错误信息
     */
    const std::vector<std::string>& GetErrors() const { return m_Errors; }
    bool HasErrors() const { return !m_Errors.empty(); }
    
private:
    // 简单的 Token 类型
    enum class TokenType {
        // 标识符和字面值
        IDENTIFIER,     // div, text, class
        STRING,         // "content"
        NUMBER,         // 123
        
        // 标点符号
        LBRACE,        // {
        RBRACE,        // }
        COLON,         // :
        SEMICOLON,     // ;
        
        // 注释
        COMMENT,
        
        // 特殊
        EOF_TOKEN,
        ERROR
    };
    
    struct Token {
        TokenType type;
        std::string value;
        size_t line;
        size_t column;
        
        Token(TokenType t = TokenType::EOF_TOKEN, const std::string& v = "", 
              size_t l = 0, size_t c = 0)
            : type(t), value(v), line(l), column(c) {}
    };
    
    // 简单的 AST 节点
    struct ASTNode {
        enum Type {
            ELEMENT,
            TEXT,
            ATTRIBUTE,
            DOCUMENT
        };
        
        Type type;
        std::string value;
        std::vector<std::shared_ptr<ASTNode>> children;
        std::vector<std::pair<std::string, std::string>> attributes;
        
        ASTNode(Type t = DOCUMENT) : type(t) {}
    };
    
    // 词法分析
    std::vector<Token> Tokenize(const std::string& input);
    Token NextToken();
    void SkipWhitespace();
    void SkipComment();
    std::string ReadString();
    std::string ReadIdentifier();
    std::string ReadNumber();
    
    // 语法分析
    std::shared_ptr<ASTNode> Parse(const std::vector<Token>& tokens);
    std::shared_ptr<ASTNode> ParseElement();
    std::shared_ptr<ASTNode> ParseText();
    std::shared_ptr<ASTNode> ParseContent();
    std::pair<std::string, std::string> ParseAttribute();
    
    // 代码生成
    std::string Generate(const std::shared_ptr<ASTNode>& ast);
    std::string GenerateElement(const std::shared_ptr<ASTNode>& node, int indent = 0);
    std::string GenerateText(const std::shared_ptr<ASTNode>& node);
    std::string Indent(int level);
    
    // 辅助方法
    void ReportError(const std::string& message);
    bool Match(TokenType type);
    bool Check(TokenType type);
    Token Advance();
    Token Peek();
    Token Previous();
    bool IsAtEnd();
    
    // 成员变量
    std::vector<std::string> m_Errors;
    std::string m_Input;
    size_t m_Current;
    size_t m_Line;
    size_t m_Column;
    
    // Parser state
    std::vector<Token> m_Tokens;
    size_t m_TokenIndex;
};

} // namespace simple
} // namespace chtl