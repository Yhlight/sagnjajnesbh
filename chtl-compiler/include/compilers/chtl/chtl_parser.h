#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include "scanner/token.h"
#include "compilers/chtl/chtl_ast.h"

namespace chtl {

/**
 * CHTL解析器
 * 将CHTL代码解析成AST
 */
class CHTLParser {
public:
    CHTLParser();
    ~CHTLParser();
    
    /**
     * 解析CHTL代码
     * @param source CHTL源代码
     * @return AST根节点
     */
    std::shared_ptr<CHTLASTNode> parse(const std::string& source);
    
    /**
     * 获取解析错误
     */
    const std::vector<std::string>& getErrors() const { return errors_; }
    
    /**
     * 重置解析器状态
     */
    void reset();
    
private:
    // Token列表和当前位置
    std::vector<Token> tokens_;
    size_t current_;
    
    // 错误列表
    std::vector<std::string> errors_;
    
    // 词法分析
    void tokenize(const std::string& source);
    
    // 解析方法
    std::shared_ptr<CHTLASTNode> parseDocument();
    std::shared_ptr<CHTLASTNode> parseElement();
    std::shared_ptr<CHTLASTNode> parseText();
    std::shared_ptr<CHTLASTNode> parseStyleBlock();
    std::shared_ptr<CHTLASTNode> parseScriptBlock();
    std::shared_ptr<CHTLASTNode> parseTemplate();
    std::shared_ptr<CHTLASTNode> parseCustom();
    std::shared_ptr<CHTLASTNode> parseImport();
    std::shared_ptr<CHTLASTNode> parseNamespace();
    std::shared_ptr<CHTLASTNode> parseConfiguration();
    std::shared_ptr<CHTLASTNode> parseOrigin();
    
    // 解析属性
    void parseAttributes(ElementNode* element);
    
    // 辅助方法
    Token& peek(size_t offset = 0);
    Token& previous();
    Token advance();
    bool check(TokenType type);
    bool match(const std::vector<TokenType>& types);
    bool match(TokenType type);
    void consume(TokenType type, const std::string& message);
    bool isAtEnd();
    
    // 错误处理
    void error(const Token& token, const std::string& message);
    void synchronize();
};

} // namespace chtl

#endif // CHTL_PARSER_H