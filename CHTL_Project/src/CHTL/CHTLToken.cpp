#include "CHTL/CHTLToken.hpp"
#include <unordered_map>

namespace CHTL {

// Token方法实现
bool CHTLToken::IsKeyword() const {
    return type >= CHTLTokenType::TEXT && type <= CHTLTokenType::CONFIG_TYPE;
}

bool CHTLToken::IsOperator() const {
    return type == CHTLTokenType::COLON || 
           type == CHTLTokenType::EQUALS || 
           type == CHTLTokenType::AT_SYMBOL ||
           type == CHTLTokenType::AMPERSAND;
}

bool CHTLToken::IsLiteral() const {
    return type == CHTLTokenType::STRING_LITERAL || 
           type == CHTLTokenType::UNQUOTED_LITERAL ||
           type == CHTLTokenType::NUMBER;
}

bool CHTLToken::IsComment() const {
    return type == CHTLTokenType::LINE_COMMENT || 
           type == CHTLTokenType::BLOCK_COMMENT ||
           type == CHTLTokenType::GENERATOR_COMMENT;
}

String CHTLToken::ToString() const {
    return "[" + TokenTypeToString(type) + "] '" + value + "' @" + 
           std::to_string(location.line) + ":" + std::to_string(location.column);
}

// Token类型到字符串转换
String TokenTypeToString(CHTLTokenType type) {
    static const std::unordered_map<CHTLTokenType, String> tokenNames = {
        // 基础Token
        {CHTLTokenType::IDENTIFIER, "IDENTIFIER"},
        {CHTLTokenType::STRING_LITERAL, "STRING_LITERAL"},
        {CHTLTokenType::UNQUOTED_LITERAL, "UNQUOTED_LITERAL"},
        {CHTLTokenType::NUMBER, "NUMBER"},
        
        // 分隔符
        {CHTLTokenType::LEFT_BRACE, "LEFT_BRACE"},
        {CHTLTokenType::RIGHT_BRACE, "RIGHT_BRACE"},
        {CHTLTokenType::LEFT_BRACKET, "LEFT_BRACKET"},
        {CHTLTokenType::RIGHT_BRACKET, "RIGHT_BRACKET"},
        {CHTLTokenType::LEFT_PAREN, "LEFT_PAREN"},
        {CHTLTokenType::RIGHT_PAREN, "RIGHT_PAREN"},
        {CHTLTokenType::SEMICOLON, "SEMICOLON"},
        {CHTLTokenType::COMMA, "COMMA"},
        {CHTLTokenType::DOT, "DOT"},
        
        // 运算符
        {CHTLTokenType::COLON, "COLON"},
        {CHTLTokenType::EQUALS, "EQUALS"},
        {CHTLTokenType::AT_SYMBOL, "AT_SYMBOL"},
        {CHTLTokenType::AMPERSAND, "AMPERSAND"},
        
        // 关键字
        {CHTLTokenType::TEXT, "TEXT"},
        {CHTLTokenType::STYLE, "STYLE"},
        {CHTLTokenType::SCRIPT, "SCRIPT"},
        {CHTLTokenType::INHERIT, "INHERIT"},
        {CHTLTokenType::DELETE, "DELETE"},
        {CHTLTokenType::INSERT, "INSERT"},
        {CHTLTokenType::AFTER, "AFTER"},
        {CHTLTokenType::BEFORE, "BEFORE"},
        {CHTLTokenType::REPLACE, "REPLACE"},
        {CHTLTokenType::AT_TOP, "AT_TOP"},
        {CHTLTokenType::AT_BOTTOM, "AT_BOTTOM"},
        {CHTLTokenType::FROM, "FROM"},
        {CHTLTokenType::AS, "AS"},
        {CHTLTokenType::EXCEPT, "EXCEPT"},
        
        // 块类型关键字
        {CHTLTokenType::TEMPLATE, "TEMPLATE"},
        {CHTLTokenType::CUSTOM, "CUSTOM"},
        {CHTLTokenType::ORIGIN, "ORIGIN"},
        {CHTLTokenType::IMPORT, "IMPORT"},
        {CHTLTokenType::CONFIGURATION, "CONFIGURATION"},
        {CHTLTokenType::NAMESPACE, "NAMESPACE"},
        {CHTLTokenType::INFO, "INFO"},
        {CHTLTokenType::EXPORT, "EXPORT"},
        
        // 类型标识符
        {CHTLTokenType::STYLE_TYPE, "STYLE_TYPE"},
        {CHTLTokenType::ELEMENT_TYPE, "ELEMENT_TYPE"},
        {CHTLTokenType::VAR_TYPE, "VAR_TYPE"},
        {CHTLTokenType::HTML_TYPE, "HTML_TYPE"},
        {CHTLTokenType::JAVASCRIPT_TYPE, "JAVASCRIPT_TYPE"},
        {CHTLTokenType::CHTL_TYPE, "CHTL_TYPE"},
        {CHTLTokenType::CJMOD_TYPE, "CJMOD_TYPE"},
        {CHTLTokenType::CONFIG_TYPE, "CONFIG_TYPE"},
        
        // 注释
        {CHTLTokenType::LINE_COMMENT, "LINE_COMMENT"},
        {CHTLTokenType::BLOCK_COMMENT, "BLOCK_COMMENT"},
        {CHTLTokenType::GENERATOR_COMMENT, "GENERATOR_COMMENT"},
        
        // HTML元素
        {CHTLTokenType::HTML_ELEMENT, "HTML_ELEMENT"},
        
        // 特殊Token
        {CHTLTokenType::NEWLINE, "NEWLINE"},
        {CHTLTokenType::WHITESPACE, "WHITESPACE"},
        {CHTLTokenType::EOF_TOKEN, "EOF_TOKEN"},
        {CHTLTokenType::ERROR_TOKEN, "ERROR_TOKEN"},
        
        // CHTL JS相关Token（暂时保留，但不使用）
        {CHTLTokenType::CHTLJS_SELECTOR, "CHTLJS_SELECTOR"},
        {CHTLTokenType::CHTLJS_ARROW, "CHTLJS_ARROW"},
        {CHTLTokenType::CHTLJS_LISTEN, "CHTLJS_LISTEN"},
        {CHTLTokenType::CHTLJS_DELEGATE, "CHTLJS_DELEGATE"},
        {CHTLTokenType::CHTLJS_ANIMATE, "CHTLJS_ANIMATE"},
        {CHTLTokenType::CHTLJS_VIR, "CHTLJS_VIR"},
        
        {CHTLTokenType::UNKNOWN, "UNKNOWN"}
    };
    
    auto it = tokenNames.find(type);
    return (it != tokenNames.end()) ? it->second : "UNKNOWN";
}

// 关键字映射表实现
CHTLKeywordMap::CHTLKeywordMap() {
    // 基础关键字
    keywordMap_["text"] = CHTLTokenType::TEXT;
    keywordMap_["style"] = CHTLTokenType::STYLE;
    keywordMap_["script"] = CHTLTokenType::SCRIPT;
    keywordMap_["inherit"] = CHTLTokenType::INHERIT;
    keywordMap_["delete"] = CHTLTokenType::DELETE;
    keywordMap_["insert"] = CHTLTokenType::INSERT;
    keywordMap_["after"] = CHTLTokenType::AFTER;
    keywordMap_["before"] = CHTLTokenType::BEFORE;
    keywordMap_["replace"] = CHTLTokenType::REPLACE;
    keywordMap_["from"] = CHTLTokenType::FROM;
    keywordMap_["as"] = CHTLTokenType::AS;
    keywordMap_["except"] = CHTLTokenType::EXCEPT;
    
    // 复合关键字
    keywordMap_["at top"] = CHTLTokenType::AT_TOP;
    keywordMap_["at bottom"] = CHTLTokenType::AT_BOTTOM;
    
    // 块类型关键字
    keywordMap_["[Template]"] = CHTLTokenType::TEMPLATE;
    keywordMap_["[Custom]"] = CHTLTokenType::CUSTOM;
    keywordMap_["[Origin]"] = CHTLTokenType::ORIGIN;
    keywordMap_["[Import]"] = CHTLTokenType::IMPORT;
    keywordMap_["[Configuration]"] = CHTLTokenType::CONFIGURATION;
    keywordMap_["[Namespace]"] = CHTLTokenType::NAMESPACE;
    keywordMap_["[Info]"] = CHTLTokenType::INFO;
    keywordMap_["[Export]"] = CHTLTokenType::EXPORT;
    
    // 类型标识符
    keywordMap_["@Style"] = CHTLTokenType::STYLE_TYPE;
    keywordMap_["@Element"] = CHTLTokenType::ELEMENT_TYPE;
    keywordMap_["@Var"] = CHTLTokenType::VAR_TYPE;
    keywordMap_["@Html"] = CHTLTokenType::HTML_TYPE;
    keywordMap_["@JavaScript"] = CHTLTokenType::JAVASCRIPT_TYPE;
    keywordMap_["@Chtl"] = CHTLTokenType::CHTL_TYPE;
    keywordMap_["@CJmod"] = CHTLTokenType::CJMOD_TYPE;
    keywordMap_["@Config"] = CHTLTokenType::CONFIG_TYPE;
}

CHTLTokenType CHTLKeywordMap::GetKeywordType(const String& keyword) const {
    auto it = keywordMap_.find(keyword);
    return (it != keywordMap_.end()) ? it->second : CHTLTokenType::IDENTIFIER;
}

bool CHTLKeywordMap::IsKeyword(const String& word) const {
    return keywordMap_.find(word) != keywordMap_.end();
}

void CHTLKeywordMap::AddCustomKeyword(const String& keyword, CHTLTokenType type) {
    keywordMap_[keyword] = type;
}

} // namespace CHTL