#ifndef CHTL_JS_PARSER_H
#define CHTL_JS_PARSER_H

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include "compilers/chtl_js/chtl_js_ast.h"

namespace chtl {

/**
 * CHTL JS解析器
 */
class CHTLJSParser {
public:
    CHTLJSParser();
    ~CHTLJSParser();
    
    /**
     * 解析CHTL JS代码
     * @param source CHTL JS源代码
     * @return AST根节点
     */
    std::shared_ptr<CHTLJSASTNode> parse(const std::string& source);
    
    /**
     * 获取解析错误
     */
    const std::vector<std::string>& getErrors() const { return errors_; }
    
    /**
     * 重置解析器状态
     */
    void reset();
    
private:
    std::string source_;
    size_t current_;
    std::vector<std::string> errors_;
    
    // 解析方法
    std::shared_ptr<CHTLJSASTNode> parseStatement();
    std::shared_ptr<CHTLJSASTNode> parseEnhancedSelector();
    std::shared_ptr<CHTLJSASTNode> parseVirtualObject();
    std::shared_ptr<CHTLJSASTNode> parseArrowCall(const std::string& object);
    std::shared_ptr<CHTLJSASTNode> parseListen();
    std::shared_ptr<CHTLJSASTNode> parseDelegate();
    std::shared_ptr<CHTLJSASTNode> parseAnimate();
    
    // 辅助方法
    bool match(const std::string& str);
    bool matchRegex(const std::string& pattern);
    std::string extractUntil(const std::string& delimiter);
    std::string extractBetween(char open, char close);
    void skipWhitespace();
    bool isAtEnd() const;
    char peek(size_t offset = 0) const;
    char advance();
    
    // 错误处理
    void addError(const std::string& message);
};

} // namespace chtl

#endif // CHTL_JS_PARSER_H