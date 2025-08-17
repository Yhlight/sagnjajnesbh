#include "../../include/CHTLParser.h"
#include "../../include/CHTLLexer.h"
#include "../../include/StateMachine.h"
#include "../../include/ContextManager.h"
#include <iostream>

namespace chtl {

/**
 * CHTL基础语法解析器
 * 专门处理基础语法：注释、文本节点、字面量、CE对等式、元素节点、属性
 */
class CHTLBasicParser {
public:
    explicit CHTLBasicParser(const CompilerOptions& options = CompilerOptions{})
        : options_(options), currentPos_(0) {
        
        stateMachine_ = std::make_unique<StateMachine>();
        contextManager_ = std::make_unique<ContextManager>();
        
        if (options_.debugMode) {
            debugLog("CHTL基础解析器初始化完成");
        }
    }

    // 解析CHTL源代码
    ParseResult parseBasicSyntax(const String& source, const String& filename = "") {
        currentFilename_ = filename;
        result_ = ParseResult{};
        
        if (options_.debugMode) {
            debugLog("开始解析基础语法: " + filename);
        }
        
        // 使用词法分析器生成Token
        CHTLLexer lexer(source, *stateMachine_, *contextManager_);
        tokens_ = lexer.tokenize();
        currentPos_ = 0;
        
        if (tokens_.empty()) {
            result_.addError(ParseErrorType::SYNTAX_ERROR, "空的Token流", 0, 0, filename);
            return result_;
        }
        
        try {
            // 创建根节点
            auto root = std::make_shared<RootNode>(0, 0);
            
            // 解析基础语法结构
            while (!isAtEnd()) {
                skipComments();
                
                if (isAtEnd()) break;
                
                auto node = parseBasicStatement();
                if (node) {
                    root->addChild(node);
                }
            }
            
            result_.ast = root;
            result_.success = true;
            
            if (options_.debugMode) {
                debugLog("基础语法解析完成，生成AST节点: " + 
                        std::to_string(root->getChildren().size()) + " 个顶层节点");
            }
            
        } catch (const std::exception& e) {
            result_.addError(ParseErrorType::SYNTAX_ERROR, 
                           "解析异常: " + String(e.what()), 
                           currentToken().line, currentToken().column, filename);
        }
        
        return result_;
    }

private:
    CompilerOptions options_;
    String currentFilename_;
    std::vector<Token> tokens_;
    size_t currentPos_;
    ParseResult result_;
    std::unique_ptr<StateMachine> stateMachine_;
    std::unique_ptr<ContextManager> contextManager_;

    // Token操作
    const Token& currentToken() const {
        if (currentPos_ >= tokens_.size()) {
            static Token eofToken(TokenType::END_OF_FILE, "", 0, 0);
            return eofToken;
        }
        return tokens_[currentPos_];
    }
    
    const Token& peekToken(size_t offset = 1) const {
        size_t pos = currentPos_ + offset;
        if (pos >= tokens_.size()) {
            static Token eofToken(TokenType::END_OF_FILE, "", 0, 0);
            return eofToken;
        }
        return tokens_[pos];
    }
    
    void skipToken() {
        if (currentPos_ < tokens_.size()) {
            currentPos_++;
        }
    }
    
    bool isAtEnd() const {
        return currentPos_ >= tokens_.size() || 
               currentToken().type == TokenType::END_OF_FILE;
    }
    
    bool matchToken(TokenType type) const {
        return currentToken().type == type;
    }
    
    Token consumeToken(TokenType expectedType = TokenType::UNKNOWN) {
        const Token& token = currentToken();
        if (expectedType != TokenType::UNKNOWN && token.type != expectedType) {
            reportError(ParseErrorType::UNEXPECTED_TOKEN, 
                       "期望 " + tokenTypeToString(expectedType) + 
                       "，得到 " + tokenTypeToString(token.type));
        }
        skipToken();
        return token;
    }

    // 跳过注释
    void skipComments() {
        while (!isAtEnd() && isCommentToken(currentToken().type)) {
            skipToken();
        }
    }
    
    bool isCommentToken(TokenType type) const {
        return type == TokenType::COMMENT_SINGLE || 
               type == TokenType::COMMENT_MULTI ||
               type == TokenType::COMMENT_GENERATOR;
    }

    // 解析基础语句
    std::shared_ptr<CHTLASTNode> parseBasicStatement() {
        const auto& token = currentToken();
        
        switch (token.type) {
            case TokenType::TEXT:
                return parseTextNode();
                
            case TokenType::IDENTIFIER:
            case TokenType::HTML_TAG:
                return parseElementNode();
                
            case TokenType::COMMENT_GENERATOR:
                return parseGeneratorComment();
                
            default:
                reportError(ParseErrorType::UNEXPECTED_TOKEN, 
                           "意外的Token: " + token.value);
                skipToken();
                return nullptr;
        }
    }

    // 解析文本节点
    std::shared_ptr<CHTLASTNode> parseTextNode() {
        const auto& textToken = consumeToken(TokenType::TEXT);
        
        if (!matchToken(TokenType::LEFT_BRACE)) {
            reportError(ParseErrorType::MISSING_TOKEN, "文本节点缺少左花括号 '{'");
            return nullptr;
        }
        
        consumeToken(TokenType::LEFT_BRACE);
        
        // 解析文本内容
        String textContent;
        if (!matchToken(TokenType::RIGHT_BRACE)) {
            textContent = parseLiteral();
        }
        
        if (!matchToken(TokenType::RIGHT_BRACE)) {
            reportError(ParseErrorType::MISSING_TOKEN, "文本节点缺少右花括号 '}'");
            return nullptr;
        }
        
        consumeToken(TokenType::RIGHT_BRACE);
        
        return std::make_shared<TextNode>(textContent, textToken.line, textToken.column);
    }

    // 解析元素节点
    std::shared_ptr<CHTLASTNode> parseElementNode() {
        const auto& elementToken = consumeToken();
        String tagName = elementToken.value;
        
        auto element = std::make_shared<ElementNode>(tagName, 
                                                    elementToken.line, 
                                                    elementToken.column);
        
        if (!matchToken(TokenType::LEFT_BRACE)) {
            reportError(ParseErrorType::MISSING_TOKEN, 
                       "元素 '" + tagName + "' 缺少左花括号 '{'");
            return element;
        }
        
        consumeToken(TokenType::LEFT_BRACE);
        
        // 解析元素内容：属性和子元素
        while (!matchToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            skipComments();
            
            if (matchToken(TokenType::RIGHT_BRACE)) break;
            
            if (isAttributeStart()) {
                auto attribute = parseAttribute();
                if (attribute) {
                    element->addChild(attribute);
                }
            } else {
                auto child = parseBasicStatement();
                if (child) {
                    element->addChild(child);
                }
            }
        }
        
        if (!matchToken(TokenType::RIGHT_BRACE)) {
            reportError(ParseErrorType::MISSING_TOKEN, 
                       "元素 '" + tagName + "' 缺少右花括号 '}'");
        } else {
            consumeToken(TokenType::RIGHT_BRACE);
        }
        
        return element;
    }

    // 解析属性
    std::shared_ptr<AttributeNode> parseAttribute() {
        if (!matchToken(TokenType::IDENTIFIER)) {
            reportError(ParseErrorType::UNEXPECTED_TOKEN, "期望属性名");
            return nullptr;
        }
        
        const auto& nameToken = consumeToken(TokenType::IDENTIFIER);
        String attributeName = nameToken.value;
        
        // CE对等式：支持 ':' 或 '='
        if (!matchToken(TokenType::COLON) && !matchToken(TokenType::EQUALS)) {
            reportError(ParseErrorType::SYNTAX_ERROR, 
                       "属性定义缺少 ':' 或 '='");
            return nullptr;
        }
        
        consumeToken(); // 消费 : 或 =
        
        String attributeValue = parseLiteral();
        
        // 可选的分号
        if (matchToken(TokenType::SEMICOLON)) {
            consumeToken(TokenType::SEMICOLON);
        }
        
        return std::make_shared<AttributeNode>(attributeName, attributeValue, 
                                              nameToken.line, nameToken.column);
    }

    // 解析字面量（支持三种类型）
    String parseLiteral() {
        const auto& token = currentToken();
        
        switch (token.type) {
            case TokenType::STRING_LITERAL:
            case TokenType::SINGLE_STRING:
                skipToken();
                // 字符串字面量已经去除引号
                return token.value;
                
            case TokenType::UNQUOTED_LITERAL:
            case TokenType::IDENTIFIER:
            case TokenType::NUMBER:
                skipToken();
                return token.value;
                
            default:
                // 尝试解析无修饰字面量
                if (isValidUnquotedLiteralStart()) {
                    return parseUnquotedLiteral();
                }
                
                reportError(ParseErrorType::UNEXPECTED_TOKEN, 
                           "期望字面量，得到: " + token.value);
                return "";
        }
    }

    // 解析无修饰字面量
    String parseUnquotedLiteral() {
        String literal;
        
        while (!isAtEnd() && isValidUnquotedLiteralChar()) {
            literal += currentToken().value;
            skipToken();
        }
        
        return literal;
    }

    // 解析生成器注释
    std::shared_ptr<CHTLASTNode> parseGeneratorComment() {
        const auto& commentToken = consumeToken(TokenType::COMMENT_GENERATOR);
        
        // 生成器注释作为特殊节点保留
        auto commentNode = std::make_shared<TextNode>(commentToken.value, 
                                                     commentToken.line, 
                                                     commentToken.column);
        commentNode->setAttribute("type", "generator-comment");
        
        return commentNode;
    }

    // 辅助方法
    bool isAttributeStart() const {
        return matchToken(TokenType::IDENTIFIER) && 
               (peekToken().type == TokenType::COLON || 
                peekToken().type == TokenType::EQUALS);
    }
    
    bool isValidUnquotedLiteralStart() const {
        TokenType type = currentToken().type;
        return type == TokenType::IDENTIFIER || 
               type == TokenType::NUMBER ||
               (type != TokenType::LEFT_BRACE && 
                type != TokenType::RIGHT_BRACE &&
                type != TokenType::SEMICOLON &&
                type != TokenType::END_OF_FILE);
    }
    
    bool isValidUnquotedLiteralChar() const {
        TokenType type = currentToken().type;
        return type != TokenType::LEFT_BRACE && 
               type != TokenType::RIGHT_BRACE &&
               type != TokenType::SEMICOLON &&
               type != TokenType::END_OF_FILE;
    }

    // 错误处理
    void reportError(ParseErrorType type, const String& message) {
        result_.addError(type, message, 
                        currentToken().line, 
                        currentToken().column, 
                        currentFilename_);
    }
    
    void debugLog(const String& message) const {
        if (options_.debugMode) {
            std::cout << "[CHTLBasicParser] " << message << std::endl;
        }
    }
    
    String tokenTypeToString(TokenType type) const {
        // 简化实现
        switch (type) {
            case TokenType::TEXT: return "text";
            case TokenType::IDENTIFIER: return "identifier";
            case TokenType::LEFT_BRACE: return "{";
            case TokenType::RIGHT_BRACE: return "}";
            case TokenType::COLON: return ":";
            case TokenType::EQUALS: return "=";
            case TokenType::SEMICOLON: return ";";
            default: return "unknown";
        }
    }
};

} // namespace chtl