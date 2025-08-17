#pragma once

#include "CHTLCommon.h"
#include "CHTLTokens.h"
#include "StateMachine.h"
#include "ContextManager.h"
#include <queue>

namespace chtl {

// 词法分析器状态
enum class LexerState {
    NORMAL,                 // 正常状态
    IN_STRING,              // 字符串内
    IN_COMMENT,             // 注释内
    IN_IDENTIFIER,          // 标识符内
    IN_NUMBER,              // 数字内
    IN_TEMPLATE_DECL,       // 模板声明内
    IN_CUSTOM_DECL,         // 自定义声明内
    IN_IMPORT_DECL,         // 导入声明内
    IN_NAMESPACE_DECL,      // 命名空间声明内
    IN_CONFIGURATION_DECL,  // 配置声明内
    IN_CHTL_JS              // CHTL JS语法内
};

/**
 * CHTL词法分析器
 * 将CHTL源代码转换为Token流，严格按照CHTL语法文档实现
 */
class CHTLLexer {
public:
    CHTLLexer(const String& sourceCode, chtl::StateMachine& stateMachine, chtl::ContextManager& contextManager);
    ~CHTLLexer() = default;
    
    // 词法分析
    std::vector<Token> tokenize();
    Token nextToken();
    bool hasMoreTokens() const;
    
    // 位置信息
    size_t getCurrentPosition() const { return currentPos_; }
    size_t getCurrentLine() const { return currentLine_; }
    size_t getCurrentColumn() const { return currentColumn_; }
    
    // 错误处理
    const std::vector<chtl::CompilerError>& getErrors() const { return errors_; }
    bool hasErrors() const { return !errors_.empty(); }
    
    // 预览功能
    Token peekToken(size_t offset = 0);
    std::vector<Token> peekTokens(size_t count);
    
    // 状态管理
    void pushState(LexerState state);
    void popState();
    LexerState getCurrentState() const;
    
    // 配置
    void setSkipWhitespace(bool skip) { skipWhitespace_ = skip; }
    void setSkipComments(bool skip) { skipComments_ = skip; }
    void setGeneratePositionInfo(bool generate) { generatePositionInfo_ = generate; }
    
    // 统计信息
    struct Statistics {
        size_t tokensGenerated = 0;
        size_t identifiersFound = 0;
        size_t keywordsFound = 0;
        size_t literalsFound = 0;
        size_t commentsSkipped = 0;
        size_t errorsEncountered = 0;
    };
    
    Statistics getStatistics() const { return statistics_; }
    void resetStatistics() { statistics_ = Statistics{}; }
    
private:
    String sourceCode_;
    size_t currentPos_;
    size_t currentLine_;
    size_t currentColumn_;
    std::stack<LexerState> stateStack_;
    std::queue<Token> tokenBuffer_;
    std::vector<chtl::CompilerError> errors_;
    Statistics statistics_;
    
    // 配置选项
    bool skipWhitespace_;
    bool skipComments_;
    bool generatePositionInfo_;
    
    // 依赖组件
    chtl::StateMachine& stateMachine_;
    chtl::ContextManager& contextManager_;
    chtl::KeywordMap& keywordMap_;
    
    // 字符操作
    char currentChar() const;
    char peekChar(size_t offset = 1) const;
    char nextChar();
    void skipWhitespaceChars();
    bool isAtEnd() const;
    
    // Token生成
    Token makeToken(TokenType type, const String& value = "", size_t length = 0);
    Token makeErrorToken(const String& message);
    
    // 字面量解析
    Token scanString(char quote);
    Token scanUnquotedLiteral();
    Token scanNumber();
    Token scanIdentifier();
    
    // 注释处理
    Token scanSingleLineComment();
    Token scanMultiLineComment();
    Token scanGeneratorComment();
    
    // 特殊语法处理
    Token scanTemplateDeclaration();
    Token scanCustomDeclaration();
    Token scanOriginDeclaration();
    Token scanImportDeclaration();
    Token scanNamespaceDeclaration();
    Token scanConfigurationDeclaration();
    
    // CHTL JS语法处理
    Token scanCHTLJSSelector();
    Token scanArrowOperator();
    Token scanCHTLJSKeyword();
    
    // 符号处理
    Token scanOperator();
    Token scanPunctuation();
    
    // 关键字识别
    TokenType identifyKeyword(const String& word);
    bool isKeywordInContext(const String& word);
    
    // 上下文感知
    bool shouldScanAsUnquotedLiteral() const;
    bool shouldScanAsCHTLJS() const;
    bool isInStyleContext() const;
    bool isInScriptContext() const;
    
    // 验证
    bool validateToken(const Token& token);
    bool validateStringLiteral(const String& value);
    bool validateIdentifier(const String& value);
    bool validateNumber(const String& value);
    
    // 错误处理
    void reportError(const String& message);
    void reportWarning(const String& message);
    void recoverFromError();
    
    // 位置管理
    void updatePosition(char ch);
    void markPosition();
    void resetPosition();
    
    // 辅助方法
    bool isAlpha(char ch) const;
    bool isDigit(char ch) const;
    bool isAlphaNumeric(char ch) const;
    bool isWhitespace(char ch) const;
    bool isNewline(char ch) const;
    bool isValidIdentifierStart(char ch) const;
    bool isValidIdentifierChar(char ch) const;
    bool isValidNumberChar(char ch) const;
    
    // CE对等式支持 (: 等价于 =)
    TokenType getCEEquivalentToken(TokenType type) const;
    bool isCEContext() const;
    
    // 特殊字符序列检测
    bool matchSequence(const String& sequence) const;
    bool matchKeywordBoundary(const String& keyword) const;
    
    // 缓冲区管理
    void bufferToken(const Token& token);
    Token getBufferedToken();
    bool hasBufferedTokens() const;
    void clearBuffer();
    
    // 调试
    void dumpCurrentState() const;
    String getCurrentContext() const;
};

} // namespace chtl