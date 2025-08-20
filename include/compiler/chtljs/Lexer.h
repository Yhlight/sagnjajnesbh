#ifndef CHTL_COMPILER_CHTLJS_LEXER_H
#define CHTL_COMPILER_CHTLJS_LEXER_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "../../common/Token.h"
#include "../../common/SourceLocation.h"
#include "../../common/Error.h"
#include "../../common/State.h"

namespace chtl {
namespace compiler {

// CHTL JS特有的Token类型
enum class CHTLJSTokenType {
    // 增强选择器
    SELECTOR_START,         // {{
    SELECTOR_END,           // }}
    SELECTOR_CLASS,         // .class
    SELECTOR_ID,            // #id
    SELECTOR_TAG,           // tag
    SELECTOR_INDEX,         // [0]
    
    // 虚对象
    VIR_KEYWORD,            // vir
    VIR_ARROW,              // ->
    
    // CHTL JS函数
    LISTEN_FUNCTION,        // listen
    DELEGATE_FUNCTION,      // delegate
    ANIMATE_FUNCTION,       // animate
    
    // 特殊语法
    TEMPLATE_VAR_CALL,      // 变量组调用 ThemeColor(tableColor)
    CONTEXT_REFERENCE,      // & 上下文引用
    
    // JavaScript基础Token
    JS_IDENTIFIER,
    JS_STRING,
    JS_NUMBER,
    JS_OPERATOR,
    JS_PUNCTUATION,
    JS_KEYWORD,
    JS_COMMENT
};

// CHTL JS词法分析器
class CHTLJSLexer {
private:
    // 输入管理
    std::string filename_;
    std::string content_;
    size_t position_ = 0;
    size_t line_ = 1;
    size_t column_ = 1;
    size_t tokenStart_ = 0;
    
    // 状态机
    std::unique_ptr<StateMachine> stateMachine_;
    
    // 诊断收集器
    DiagnosticCollector* diagnostics_;
    
    // Token缓冲
    std::vector<TokenPtr> tokenBuffer_;
    size_t bufferPosition_ = 0;
    
    // 当前是否在CHTL JS上下文中
    bool inCHTLJSContext_ = false;
    
    // CHTL JS关键字
    static const std::unordered_map<std::string, TokenType> chtljsKeywords_;
    
    // JavaScript关键字
    static const std::unordered_map<std::string, TokenType> jsKeywords_;
    
    // 辅助方法
    char peek(size_t offset = 0) const;
    char advance();
    void skipWhitespace();
    bool match(const std::string& expected);
    bool isAtEnd() const { return position_ >= content_.size(); }
    
    // 位置管理
    TokenLocation getCurrentLocation() const;
    void updatePosition(char ch);
    void markTokenStart() { tokenStart_ = position_; }
    
    // Token创建
    TokenPtr makeToken(TokenType type);
    TokenPtr makeToken(TokenType type, const std::string& value);
    
    // 扫描方法
    TokenPtr scanToken();
    TokenPtr scanSelector();           // {{选择器}}
    TokenPtr scanVirObject();          // vir对象定义
    TokenPtr scanArrowAccess();        // -> 访问
    TokenPtr scanTemplateVarCall();    // 模板变量调用
    TokenPtr scanJSIdentifier();       // JavaScript标识符
    TokenPtr scanJSString(char quote); // JavaScript字符串
    TokenPtr scanJSNumber();           // JavaScript数字
    TokenPtr scanJSOperator();         // JavaScript运算符
    TokenPtr scanJSComment();          // JavaScript注释
    
    // 选择器解析
    void parseSelectorContent(const std::string& selector,
                             std::vector<TokenPtr>& tokens);
    bool isValidSelector(const std::string& selector) const;
    
    // 验证方法
    bool isCHTLJSFunction(const std::string& name) const;
    bool isVirObjectName(const std::string& name) const;
    
    // 上下文管理
    void enterCHTLJSContext() { inCHTLJSContext_ = true; }
    void exitCHTLJSContext() { inCHTLJSContext_ = false; }
    bool isInCHTLJSContext() const { return inCHTLJSContext_; }
    
    // 错误处理
    void reportError(const std::string& message);
    void reportWarning(const std::string& message);
    
public:
    CHTLJSLexer(const std::string& filename, const std::string& content,
                DiagnosticCollector* diag = nullptr);
    
    // 主接口
    std::vector<TokenPtr> tokenize();
    TokenPtr nextToken();
    
    // 预览
    TokenPtr peekToken(size_t offset = 0);
    
    // 状态查询
    bool hasMoreTokens() const;
    
    // 重置
    void reset();
    
    // 调试
    void dumpTokens(const std::vector<TokenPtr>& tokens) const;
};

// CHTL JS词法分析工具
namespace CHTLJSLexerUtils {
    // 判断是否是CHTL JS特殊语法的开始
    bool isCHTLJSSyntaxStart(const std::string& content, size_t pos);
    
    // 判断是否是有效的选择器
    bool isValidSelectorSyntax(const std::string& selector);
    
    // 提取选择器类型
    std::string extractSelectorType(const std::string& selector);
    
    // 判断是否是CHTL JS函数
    bool isCHTLJSBuiltinFunction(const std::string& name);
}

// 初始化关键字映射
const std::unordered_map<std::string, TokenType> CHTLJSLexer::chtljsKeywords_ = {
    {"vir", TokenType::KEYWORD_VIR},
    {"listen", TokenType::KEYWORD_LISTEN},
    {"delegate", TokenType::KEYWORD_DELEGATE},
    {"animate", TokenType::KEYWORD_ANIMATE}
};

const std::unordered_map<std::string, TokenType> CHTLJSLexer::jsKeywords_ = {
    // JavaScript关键字（部分）
    {"var", TokenType::IDENTIFIER},
    {"let", TokenType::IDENTIFIER},
    {"const", TokenType::IDENTIFIER},
    {"function", TokenType::IDENTIFIER},
    {"if", TokenType::IDENTIFIER},
    {"else", TokenType::IDENTIFIER},
    {"for", TokenType::IDENTIFIER},
    {"while", TokenType::IDENTIFIER},
    {"return", TokenType::IDENTIFIER},
    {"break", TokenType::IDENTIFIER},
    {"continue", TokenType::IDENTIFIER},
    {"switch", TokenType::IDENTIFIER},
    {"case", TokenType::IDENTIFIER},
    {"default", TokenType::IDENTIFIER},
    {"try", TokenType::IDENTIFIER},
    {"catch", TokenType::IDENTIFIER},
    {"finally", TokenType::IDENTIFIER},
    {"throw", TokenType::IDENTIFIER},
    {"new", TokenType::IDENTIFIER},
    {"this", TokenType::IDENTIFIER},
    {"class", TokenType::IDENTIFIER},
    {"extends", TokenType::IDENTIFIER},
    {"import", TokenType::IDENTIFIER},
    {"export", TokenType::IDENTIFIER},
    {"async", TokenType::IDENTIFIER},
    {"await", TokenType::IDENTIFIER}
};

} // namespace compiler
} // namespace chtl

#endif // CHTL_COMPILER_CHTLJS_LEXER_H