#pragma once
#include "Token.h"
#include "Context.h"
#include <string>
#include <vector>
#include <memory>
#include <istream>

namespace chtl {

// 词法分析器
class Lexer {
public:
    explicit Lexer(CompilerContext& context);
    ~Lexer() = default;
    
    // 设置输入源
    void setInput(const std::string& input);
    void setInput(std::istream& input_stream);
    void setInputFile(const std::string& file_path);
    
    // 获取下一个Token
    Token nextToken();
    
    // 预览下一个Token（不移动位置）
    Token peekToken();
    
    // 跳过空白字符和注释
    void skipWhitespace();
    void skipComment();
    
    // 位置管理
    TokenPosition getCurrentPosition() const;
    void setPosition(size_t line, size_t column, size_t offset = 0);
    
    // 错误处理
    bool hasError() const;
    std::string getLastError() const;
    void clearError();
    
    // 状态查询
    bool isAtEnd() const;
    size_t getCurrentOffset() const;
    size_t getInputLength() const;
    
    // 调试和诊断
    std::string getCurrentLine() const;
    std::string getContextAroundPosition(size_t radius = 20) const;
    
    // 重置
    void reset();
    
private:
    CompilerContext& context_;      // 编译上下文
    std::string input_;             // 输入字符串
    size_t current_pos_;            // 当前位置
    size_t line_number_;            // 当前行号
    size_t column_number_;          // 当前列号
    std::string last_error_;        // 最后的错误信息
    
    // 内部辅助方法
    char currentChar() const;
    char peekChar(size_t offset = 1) const;
    void advance();
    void advanceLine();
    
    // Token识别方法
    Token readIdentifier();
    Token readNumber();
    Token readStringLiteral(char quote_char);
    Token readUnquotedLiteral();
    Token readOperator();
    Token readComment();
    
    // 字符分类方法
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isWhitespace(char c) const;
    bool isNewline(char c) const;
    
    // 特殊语法处理
    Token handleDoubleSymbol();      // 处理 {{ }} -> 等双字符符号
    Token handleAtSymbol();          // 处理 @ 开头的类型标识符
    Token handleBracketKeyword();    // 处理 [Template] [Custom] 等关键字
    
    // 错误报告
    void reportError(const std::string& message);
    
    // CHTL特殊语法识别
    bool isCHTLJSStart() const;      // 检查是否是CHTL JS语法开始
    bool isCHTLKeywordStart() const; // 检查是否是CHTL关键字开始
    
    // 上下文相关的词法分析
    Token nextTokenInContext();
    Token nextTokenInStyleBlock();
    Token nextTokenInScriptBlock();
    Token nextTokenInTemplateBlock();
    Token nextTokenInCustomBlock();
};

// 词法分析器工厂
class LexerFactory {
public:
    static std::unique_ptr<Lexer> createLexer(CompilerContext& context);
    static std::unique_ptr<Lexer> createLexerFromFile(CompilerContext& context, 
                                                      const std::string& file_path);
    static std::unique_ptr<Lexer> createLexerFromString(CompilerContext& context, 
                                                        const std::string& input);
};

} // namespace chtl