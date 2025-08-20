#ifndef CHTL_COMPILER_CHTL_LEXER_H
#define CHTL_COMPILER_CHTL_LEXER_H

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

// CHTL词法分析器
class CHTLLexer {
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
    
    // 关键字映射
    static const std::unordered_map<std::string, TokenType> keywords_;
    static const std::unordered_map<std::string, TokenType> typeIdentifiers_;
    
    // 辅助方法
    char peek(size_t offset = 0) const;
    char advance();
    void skipWhitespace();
    void skipToEndOfLine();
    bool match(const std::string& expected);
    bool isAtEnd() const { return position_ >= content_.size(); }
    
    // 位置管理
    TokenLocation getCurrentLocation() const;
    void updatePosition(char ch);
    void markTokenStart() { tokenStart_ = position_; }
    
    // Token创建
    TokenPtr makeToken(TokenType type);
    TokenPtr makeToken(TokenType type, const std::string& value);
    TokenPtr makeToken(TokenType type, const TokenValue& value);
    
    // 扫描方法
    TokenPtr scanToken();
    TokenPtr scanIdentifier();
    TokenPtr scanString(char quote);
    TokenPtr scanUnquotedLiteral();
    TokenPtr scanNumber();
    TokenPtr scanComment();
    TokenPtr scanGeneratorComment();
    TokenPtr scanBracketKeyword();  // [Template], [Custom]等
    TokenPtr scanTypeIdentifier();   // @Style, @Element等
    TokenPtr scanHTMLElement();
    TokenPtr scanCSSProperty();
    
    // 特殊处理
    TokenPtr handleDoubleLeftBrace();   // {{
    TokenPtr handleDoubleRightBrace();  // }}
    TokenPtr handleArrow();             // ->
    
    // 验证方法
    bool isValidIdentifier(const std::string& id) const;
    bool isHTMLElement(const std::string& name) const;
    bool isCSSProperty(const std::string& name) const;
    
    // 错误处理
    void reportError(const std::string& message);
    void reportWarning(const std::string& message);
    
public:
    CHTLLexer(const std::string& filename, const std::string& content,
              DiagnosticCollector* diag = nullptr);
    
    // 主接口
    std::vector<TokenPtr> tokenize();
    TokenPtr nextToken();
    
    // 预览
    TokenPtr peekToken(size_t offset = 0);
    
    // 状态查询
    bool hasMoreTokens() const;
    size_t getCurrentLine() const { return line_; }
    size_t getCurrentColumn() const { return column_; }
    
    // 重置
    void reset();
    
    // 调试
    void dumpTokens(const std::vector<TokenPtr>& tokens) const;
};

// 词法分析工具
namespace LexerUtils {
    // 字符判断
    bool isAlpha(char ch);
    bool isDigit(char ch);
    bool isAlphaNumeric(char ch);
    bool isIdentifierStart(char ch);
    bool isIdentifierPart(char ch);
    bool isWhitespace(char ch);
    bool isNewline(char ch);
    
    // 字符串处理
    std::string unescapeString(const std::string& str);
    char unescapeChar(char ch);
    
    // 关键字检查
    bool isKeyword(const std::string& str);
    bool isTypeIdentifier(const std::string& str);
    bool isBracketKeyword(const std::string& str);
}

// 初始化关键字映射
const std::unordered_map<std::string, TokenType> CHTLLexer::keywords_ = {
    {"text", TokenType::KEYWORD_TEXT},
    {"style", TokenType::KEYWORD_STYLE},
    {"script", TokenType::KEYWORD_SCRIPT},
    {"inherit", TokenType::KEYWORD_INHERIT},
    {"delete", TokenType::KEYWORD_DELETE},
    {"insert", TokenType::KEYWORD_INSERT},
    {"after", TokenType::KEYWORD_AFTER},
    {"before", TokenType::KEYWORD_BEFORE},
    {"replace", TokenType::KEYWORD_REPLACE},
    {"at", TokenType::KEYWORD_AT_TOP},  // 需要特殊处理 "at top" 和 "at bottom"
    {"from", TokenType::KEYWORD_FROM},
    {"as", TokenType::KEYWORD_AS},
    {"except", TokenType::KEYWORD_EXCEPT},
    {"vir", TokenType::KEYWORD_VIR},
    {"animate", TokenType::KEYWORD_ANIMATE},
    {"listen", TokenType::KEYWORD_LISTEN},
    {"delegate", TokenType::KEYWORD_DELEGATE}
};

const std::unordered_map<std::string, TokenType> CHTLLexer::typeIdentifiers_ = {
    {"@Style", TokenType::TYPE_STYLE},
    {"@style", TokenType::TYPE_STYLE},
    {"@CSS", TokenType::TYPE_STYLE},
    {"@Css", TokenType::TYPE_STYLE},
    {"@css", TokenType::TYPE_STYLE},
    {"@Element", TokenType::TYPE_ELEMENT},
    {"@Var", TokenType::TYPE_VAR},
    {"@Html", TokenType::TYPE_HTML},
    {"@JavaScript", TokenType::TYPE_JAVASCRIPT},
    {"@Chtl", TokenType::TYPE_CHTL},
    {"@CJmod", TokenType::TYPE_CJMOD},
    {"@Config", TokenType::TYPE_CONFIG}
};

} // namespace compiler
} // namespace chtl

#endif // CHTL_COMPILER_CHTL_LEXER_H