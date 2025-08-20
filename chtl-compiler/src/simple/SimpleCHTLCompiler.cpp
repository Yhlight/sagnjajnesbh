#include "SimpleCHTLCompiler.h"
#include <cctype>
#include <algorithm>
#include <iostream>

namespace chtl {
namespace simple {

SimpleCHTLCompiler::SimpleCHTLCompiler() 
    : m_Current(0), m_Line(1), m_Column(1), m_TokenIndex(0), m_DebugMode(false) {
}

std::string SimpleCHTLCompiler::Compile(const std::string& chtlCode) {
    m_Errors.clear();
    m_Input = chtlCode;
    m_Current = 0;
    m_Line = 1;
    m_Column = 1;
    
    // 1. 词法分析
    auto tokens = Tokenize(chtlCode);
    if (HasErrors()) {
        return "";
    }
    
    // 2. 语法分析
    auto ast = Parse(tokens);
    if (HasErrors()) {
        return "";
    }
    
    // 3. 代码生成
    return Generate(ast);
}

// ===== 词法分析 =====

std::vector<SimpleCHTLCompiler::Token> SimpleCHTLCompiler::Tokenize(const std::string& input) {
    std::vector<Token> tokens;
    m_Input = input;
    m_Current = 0;
    m_Line = 1;
    m_Column = 1;
    
    while (m_Current < m_Input.length()) {
        SkipWhitespace();
        
        if (m_Current >= m_Input.length()) {
            break;
        }
        
        Token token = NextToken();
        if (token.type != TokenType::COMMENT && token.type != TokenType::ERROR) {
            tokens.push_back(token);
        }
    }
    
    tokens.push_back(Token(TokenType::EOF_TOKEN, "", m_Line, m_Column));
    return tokens;
}

SimpleCHTLCompiler::Token SimpleCHTLCompiler::NextToken() {
    size_t startLine = m_Line;
    size_t startColumn = m_Column;
    
    char c = m_Input[m_Current];
    
    // 单字符标记
    switch (c) {
        case '{':
            m_Current++;
            m_Column++;
            return Token(TokenType::LBRACE, "{", startLine, startColumn);
        case '}':
            m_Current++;
            m_Column++;
            return Token(TokenType::RBRACE, "}", startLine, startColumn);
        case ':':
            m_Current++;
            m_Column++;
            return Token(TokenType::COLON, ":", startLine, startColumn);
        case ';':
            m_Current++;
            m_Column++;
            return Token(TokenType::SEMICOLON, ";", startLine, startColumn);
        case '"':
            return Token(TokenType::STRING, ReadString(), startLine, startColumn);
    }
    
    // 注释
    if (c == '/' && m_Current + 1 < m_Input.length()) {
        if (m_Input[m_Current + 1] == '/') {
            SkipComment();
            return Token(TokenType::COMMENT, "", startLine, startColumn);
        } else if (m_Input[m_Current + 1] == '*') {
            SkipComment();
            return Token(TokenType::COMMENT, "", startLine, startColumn);
        }
    }
    
    // 标识符
    if (std::isalpha(c) || c == '_') {
        return Token(TokenType::IDENTIFIER, ReadIdentifier(), startLine, startColumn);
    }
    
    // 数字
    if (std::isdigit(c)) {
        return Token(TokenType::NUMBER, ReadNumber(), startLine, startColumn);
    }
    
    // 对于text块中的内容，将所有其他字符视为有效
    // 这些字符将被ParseText特殊处理
    std::string specialChar(1, c);
    m_Current++;
    m_Column++;
    return Token(TokenType::IDENTIFIER, specialChar, startLine, startColumn);
}

void SimpleCHTLCompiler::SkipWhitespace() {
    while (m_Current < m_Input.length() && std::isspace(m_Input[m_Current])) {
        if (m_Input[m_Current] == '\n') {
            m_Line++;
            m_Column = 1;
        } else {
            m_Column++;
        }
        m_Current++;
    }
}

void SimpleCHTLCompiler::SkipComment() {
    if (m_Current + 1 >= m_Input.length()) return;
    
    if (m_Input[m_Current] == '/' && m_Input[m_Current + 1] == '/') {
        // 单行注释
        m_Current += 2;
        m_Column += 2;
        while (m_Current < m_Input.length() && m_Input[m_Current] != '\n') {
            m_Current++;
            m_Column++;
        }
    } else if (m_Input[m_Current] == '/' && m_Input[m_Current + 1] == '*') {
        // 多行注释
        m_Current += 2;
        m_Column += 2;
        while (m_Current + 1 < m_Input.length()) {
            if (m_Input[m_Current] == '*' && m_Input[m_Current + 1] == '/') {
                m_Current += 2;
                m_Column += 2;
                break;
            }
            if (m_Input[m_Current] == '\n') {
                m_Line++;
                m_Column = 1;
            } else {
                m_Column++;
            }
            m_Current++;
        }
    }
}

std::string SimpleCHTLCompiler::ReadString() {
    std::string result;
    m_Current++; // 跳过开始的引号
    m_Column++;
    
    while (m_Current < m_Input.length() && m_Input[m_Current] != '"') {
        if (m_Input[m_Current] == '\\' && m_Current + 1 < m_Input.length()) {
            // 处理转义字符
            m_Current++;
            m_Column++;
            switch (m_Input[m_Current]) {
                case 'n': result += '\n'; break;
                case 't': result += '\t'; break;
                case 'r': result += '\r'; break;
                case '"': result += '"'; break;
                case '\\': result += '\\'; break;
                default: result += m_Input[m_Current]; break;
            }
        } else {
            result += m_Input[m_Current];
        }
        if (m_Input[m_Current] == '\n') {
            m_Line++;
            m_Column = 1;
        } else {
            m_Column++;
        }
        m_Current++;
    }
    
    if (m_Current < m_Input.length()) {
        m_Current++; // 跳过结束的引号
        m_Column++;
    } else {
        ReportError("Unterminated string");
    }
    
    return result;
}

std::string SimpleCHTLCompiler::ReadIdentifier() {
    std::string result;
    while (m_Current < m_Input.length() && 
           (std::isalnum(m_Input[m_Current]) || m_Input[m_Current] == '_' || m_Input[m_Current] == '-')) {
        result += m_Input[m_Current];
        m_Current++;
        m_Column++;
    }
    return result;
}

std::string SimpleCHTLCompiler::ReadNumber() {
    std::string result;
    while (m_Current < m_Input.length() && 
           (std::isdigit(m_Input[m_Current]) || m_Input[m_Current] == '.')) {
        result += m_Input[m_Current];
        m_Current++;
        m_Column++;
    }
    return result;
}

// ===== 语法分析 =====

std::shared_ptr<SimpleCHTLCompiler::ASTNode> SimpleCHTLCompiler::Parse(const std::vector<Token>& tokens) {
    m_Tokens = tokens;
    m_TokenIndex = 0;
    
    auto root = std::make_shared<ASTNode>(ASTNode::DOCUMENT);
    
    while (!IsAtEnd()) {
        auto node = ParseContent();
        if (node) {
            root->children.push_back(node);
        }
    }
    
    return root;
}

std::shared_ptr<SimpleCHTLCompiler::ASTNode> SimpleCHTLCompiler::ParseContent() {
    if (Check(TokenType::IDENTIFIER)) {
        Token identifier = Peek();
        
        // 检查是否是特殊块
        if (identifier.value == "text") {
            return ParseText();
        } else if (identifier.value == "style") {
            return ParseStyle();
        } else if (identifier.value == "script") {
            return ParseScript();
        } else {
            // 否则是元素
            return ParseElement();
        }
    }
    
    // 跳过不认识的 token
    if (!IsAtEnd()) {
        Advance();
    }
    return nullptr;
}

std::shared_ptr<SimpleCHTLCompiler::ASTNode> SimpleCHTLCompiler::ParseElement() {
    Token name = Advance(); // 获取元素名
    
    auto node = std::make_shared<ASTNode>(ASTNode::ELEMENT);
    node->value = name.value;
    
    if (!Match(TokenType::LBRACE)) {
        ReportError("Expected '{' after element name");
        return nullptr;
    }
    
    // 解析内容
    while (!Check(TokenType::RBRACE) && !IsAtEnd()) {
        // 检查是否是属性
        if (Check(TokenType::IDENTIFIER)) {
            Token next = Peek();
            if (m_TokenIndex + 1 < m_Tokens.size() && 
                m_Tokens[m_TokenIndex + 1].type == TokenType::COLON) {
                // 这是一个属性
                auto attr = ParseAttribute();
                if (!attr.first.empty()) {
                    node->attributes.push_back(attr);
                }
            } else {
                // 这是子元素
                auto child = ParseContent();
                if (child) {
                    node->children.push_back(child);
                }
            }
        } else {
            auto child = ParseContent();
            if (child) {
                node->children.push_back(child);
            }
        }
    }
    
    if (!Match(TokenType::RBRACE)) {
        ReportError("Expected '}' after element content");
        return nullptr;
    }
    
    return node;
}

std::shared_ptr<SimpleCHTLCompiler::ASTNode> SimpleCHTLCompiler::ParseText() {
    Advance(); // 跳过 'text'
    
    if (!Match(TokenType::LBRACE)) {
        ReportError("Expected '{' after 'text'");
        return nullptr;
    }
    
    auto node = std::make_shared<ASTNode>(ASTNode::TEXT);
    
    // 收集文本内容，直到遇到匹配的 }
    std::string content;
    int braceCount = 1;
    
    // 保存当前位置，用于后续跳过
    size_t skipCount = 0;
    
    while (!IsAtEnd() && braceCount > 0) {
        Token token = Peek();
        
        if (token.type == TokenType::LBRACE) {
            braceCount++;
            content += "{";
            Advance();
            skipCount++;
        } else if (token.type == TokenType::RBRACE) {
            braceCount--;
            if (braceCount > 0) {
                content += "}";
                Advance();
                skipCount++;
            } else {
                // 找到匹配的 }，不添加到内容中
                Advance();
            }
        } else if (token.type == TokenType::ERROR) {
            // 错误的字符也作为文本的一部分
            content += token.value;
            Advance();
            skipCount++;
        } else {
            content += token.value;
            if (token.type == TokenType::IDENTIFIER || 
                token.type == TokenType::STRING || 
                token.type == TokenType::NUMBER) {
                content += " ";
            }
            Advance();
            skipCount++;
        }
    }
    
    // 去除首尾空白
    content.erase(0, content.find_first_not_of(" \t\n\r"));
    content.erase(content.find_last_not_of(" \t\n\r") + 1);
    
    node->value = content;
    
    return node;
}

std::shared_ptr<SimpleCHTLCompiler::ASTNode> SimpleCHTLCompiler::ParseStyle() {
    Advance(); // 跳过 'style'
    
    if (!Match(TokenType::LBRACE)) {
        ReportError("Expected '{' after 'style'");
        return nullptr;
    }
    
    auto node = std::make_shared<ASTNode>(ASTNode::STYLE);
    
    // 收集CSS内容，直到遇到匹配的 }
    std::string content;
    int braceCount = 1;
    
    while (!IsAtEnd() && braceCount > 0) {
        Token token = Peek();
        
        if (token.type == TokenType::LBRACE) {
            braceCount++;
            content += "{";
            Advance();
        } else if (token.type == TokenType::RBRACE) {
            braceCount--;
            if (braceCount > 0) {
                content += "}";
                Advance();
            } else {
                // 找到匹配的 }，不添加到内容中
                Advance();
            }
        } else {
            content += token.value;
            if (token.type == TokenType::IDENTIFIER || 
                token.type == TokenType::STRING || 
                token.type == TokenType::NUMBER) {
                content += " ";
            }
            Advance();
        }
    }
    
    // 去除首尾空白
    content.erase(0, content.find_first_not_of(" \t\n\r"));
    content.erase(content.find_last_not_of(" \t\n\r") + 1);
    
    node->value = content;
    return node;
}

std::shared_ptr<SimpleCHTLCompiler::ASTNode> SimpleCHTLCompiler::ParseScript() {
    Advance(); // 跳过 'script'
    
    if (!Match(TokenType::LBRACE)) {
        ReportError("Expected '{' after 'script'");
        return nullptr;
    }
    
    auto node = std::make_shared<ASTNode>(ASTNode::SCRIPT);
    
    // 收集JavaScript内容，直到遇到匹配的 }
    std::string content;
    int braceCount = 1;
    
    while (!IsAtEnd() && braceCount > 0) {
        Token token = Peek();
        
        if (token.type == TokenType::LBRACE) {
            braceCount++;
            content += "{";
            Advance();
        } else if (token.type == TokenType::RBRACE) {
            braceCount--;
            if (braceCount > 0) {
                content += "}";
                Advance();
            } else {
                // 找到匹配的 }，不添加到内容中
                Advance();
            }
        } else {
            content += token.value;
            if (token.type == TokenType::IDENTIFIER || 
                token.type == TokenType::STRING || 
                token.type == TokenType::NUMBER) {
                content += " ";
            }
            Advance();
        }
    }
    
    // 去除首尾空白
    content.erase(0, content.find_first_not_of(" \t\n\r"));
    content.erase(content.find_last_not_of(" \t\n\r") + 1);
    
    node->value = content;
    return node;
}

std::pair<std::string, std::string> SimpleCHTLCompiler::ParseAttribute() {
    Token name = Advance(); // 属性名
    
    if (!Match(TokenType::COLON)) {
        ReportError("Expected ':' after attribute name");
        return {"", ""};
    }
    
    std::string value;
    
    if (Match(TokenType::STRING)) {
        value = Previous().value;
    } else if (Match(TokenType::NUMBER)) {
        value = Previous().value;
    } else if (Match(TokenType::IDENTIFIER)) {
        value = Previous().value;
    } else {
        ReportError("Expected attribute value");
        return {"", ""};
    }
    
    Match(TokenType::SEMICOLON); // 分号是可选的
    
    return {name.value, value};
}

// ===== 代码生成 =====

std::string SimpleCHTLCompiler::Generate(const std::shared_ptr<ASTNode>& ast) {
    if (!ast) return "";
    
    std::stringstream output;
    std::stringstream styles;
    std::stringstream scripts;
    
    // 第一遍：收集所有style和script块，生成元素
    for (const auto& child : ast->children) {
        if (child->type == ASTNode::ELEMENT) {
            output << GenerateElement(child, 0);
        } else if (child->type == ASTNode::TEXT) {
            output << GenerateText(child);
        } else if (child->type == ASTNode::STYLE) {
            styles << child->value << "\n";
        } else if (child->type == ASTNode::SCRIPT) {
            scripts << child->value << "\n";
        }
    }
    
    // 生成完整的HTML文档
    std::stringstream html;
    html << "<!DOCTYPE html>\n";
    html << "<html>\n";
    html << "<head>\n";
    html << "  <meta charset=\"UTF-8\">\n";
    
    // 添加样式
    if (styles.str().length() > 0) {
        html << "  <style>\n";
        html << "    " << styles.str();
        html << "  </style>\n";
    }
    
    html << "</head>\n";
    html << "<body>\n";
    
    // 添加主体内容
    html << output.str();
    
    // 添加脚本
    if (scripts.str().length() > 0) {
        html << "  <script>\n";
        html << "    " << scripts.str();
        html << "  </script>\n";
    }
    
    html << "</body>\n";
    html << "</html>\n";
    
    return html.str();
}

std::string SimpleCHTLCompiler::GenerateElement(const std::shared_ptr<ASTNode>& node, int indent) {
    std::stringstream output;
    
    // 开始标签
    output << Indent(indent) << "<" << node->value;
    
    // 属性
    for (const auto& attr : node->attributes) {
        output << " " << attr.first << "=\"" << attr.second << "\"";
    }
    
    // 自闭合标签
    std::vector<std::string> selfClosing = {"img", "br", "hr", "input", "meta", "link"};
    if (std::find(selfClosing.begin(), selfClosing.end(), node->value) != selfClosing.end() &&
        node->children.empty()) {
        output << " />\n";
        return output.str();
    }
    
    output << ">";
    
    // 子元素
    bool hasElementChildren = false;
    for (const auto& child : node->children) {
        if (child->type == ASTNode::ELEMENT) {
            hasElementChildren = true;
            break;
        }
    }
    
    if (hasElementChildren) {
        output << "\n";
        for (const auto& child : node->children) {
            if (child->type == ASTNode::ELEMENT) {
                output << GenerateElement(child, indent + 1);
            } else if (child->type == ASTNode::TEXT) {
                output << Indent(indent + 1) << GenerateText(child) << "\n";
            }
        }
        output << Indent(indent);
    } else {
        // 纯文本内容
        for (const auto& child : node->children) {
            if (child->type == ASTNode::TEXT) {
                output << GenerateText(child);
            }
        }
    }
    
    // 结束标签
    output << "</" << node->value << ">\n";
    
    return output.str();
}

std::string SimpleCHTLCompiler::GenerateText(const std::shared_ptr<ASTNode>& node) {
    return node->value;
}

std::string SimpleCHTLCompiler::Indent(int level) {
    return std::string(level * 2, ' ');
}

// ===== 辅助方法 =====

void SimpleCHTLCompiler::ReportError(const std::string& message) {
    std::stringstream error;
    error << "Error at line " << m_Line << ", column " << m_Column << ": " << message;
    
    // 添加源代码片段以提供更好的上下文
    if (m_Current < m_Input.length()) {
        // 找到当前行的开始和结束
        size_t lineStart = m_Current;
        while (lineStart > 0 && m_Input[lineStart - 1] != '\n') {
            lineStart--;
        }
        
        size_t lineEnd = m_Current;
        while (lineEnd < m_Input.length() && m_Input[lineEnd] != '\n') {
            lineEnd++;
        }
        
        std::string line = m_Input.substr(lineStart, lineEnd - lineStart);
        error << "\n  " << line;
        error << "\n  " << std::string(m_Column - 1, ' ') << "^";
    }
    
    m_Errors.push_back(error.str());
    
    if (m_DebugMode) {
        std::cerr << error.str() << std::endl;
    }
}

bool SimpleCHTLCompiler::Match(TokenType type) {
    if (Check(type)) {
        Advance();
        return true;
    }
    return false;
}

bool SimpleCHTLCompiler::Check(TokenType type) {
    if (IsAtEnd()) return false;
    return Peek().type == type;
}

SimpleCHTLCompiler::Token SimpleCHTLCompiler::Advance() {
    if (!IsAtEnd()) m_TokenIndex++;
    return Previous();
}

SimpleCHTLCompiler::Token SimpleCHTLCompiler::Peek() {
    return m_Tokens[m_TokenIndex];
}

SimpleCHTLCompiler::Token SimpleCHTLCompiler::Previous() {
    return m_Tokens[m_TokenIndex - 1];
}

bool SimpleCHTLCompiler::IsAtEnd() {
    return m_TokenIndex >= m_Tokens.size() || Peek().type == TokenType::EOF_TOKEN;
}

} // namespace simple
} // namespace chtl