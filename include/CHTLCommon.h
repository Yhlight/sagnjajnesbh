#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <optional>

namespace CHTL {

// 前向声明
class CHTLASTNode;
class CHTLJSASTNode;

// 基础类型定义
using String = std::string;
using StringView = std::string_view;
using StringList = std::vector<String>;
using StringMap = std::unordered_map<String, String>;
using StringSet = std::unordered_set<String>;

// 代码片段类型
enum class FragmentType {
    CHTL,           // CHTL代码片段
    CHTL_JS,        // CHTL JS代码片段  
    CSS,            // CSS代码片段
    JAVASCRIPT,     // JavaScript代码片段
    HTML,           // HTML代码片段
    UNKNOWN         // 未知类型
};

// Token类型
enum class TokenType {
    // 基础Token
    IDENTIFIER,     // 标识符
    STRING,         // 字符串字面量
    NUMBER,         // 数字字面量
    COMMENT,        // 注释
    
    // 符号
    LEFT_BRACE,     // {
    RIGHT_BRACE,    // }
    LEFT_PAREN,     // (
    RIGHT_PAREN,    // )
    LEFT_BRACKET,   // [
    RIGHT_BRACKET,  // ]
    SEMICOLON,      // ;
    COLON,          // :
    EQUALS,         // =
    COMMA,          // ,
    DOT,            // .
    ARROW,          // ->
    AMPERSAND,      // &
    
    // CHTL关键字
    TEXT,           // text
    STYLE,          // style
    SCRIPT,         // script
    
    // 模板相关
    TEMPLATE,       // [Template]
    CUSTOM,         // [Custom]
    ORIGIN,         // [Origin]
    IMPORT,         // [Import]
    NAMESPACE,      // [Namespace]
    CONFIGURATION,  // [Configuration]
    
    // 类型标识
    AT_STYLE,       // @Style
    AT_ELEMENT,     // @Element
    AT_VAR,         // @Var
    AT_HTML,        // @Html
    AT_JAVASCRIPT,  // @JavaScript
    AT_CHTL,        // @Chtl
    AT_CJMOD,       // @CJmod
    
    // 关键字
    INHERIT,        // inherit
    DELETE,         // delete
    INSERT,         // insert
    AFTER,          // after
    BEFORE,         // before
    REPLACE,        // replace
    AT_TOP,         // at top
    AT_BOTTOM,      // at bottom
    FROM,           // from
    AS,             // as
    EXCEPT,         // except
    
    // CHTL JS特殊符号
    DOUBLE_LEFT_BRACE,  // {{
    DOUBLE_RIGHT_BRACE, // }}
    VIR,            // vir
    LISTEN,         // listen
    DELEGATE,       // delegate
    ANIMATE,        // animate
    
    // 结束符
    END_OF_FILE,    // EOF
    NEWLINE,        // 换行
    WHITESPACE,     // 空白字符
    
    UNKNOWN         // 未知Token
};

// AST节点类型
enum class ASTNodeType {
    // 基础节点
    ROOT,
    ELEMENT,
    TEXT_NODE,
    ATTRIBUTE,
    
    // 样式相关
    STYLE_BLOCK,
    CSS_RULE,
    CSS_PROPERTY,
    
    // 脚本相关
    SCRIPT_BLOCK,
    JS_EXPRESSION,
    JS_STATEMENT,
    
    // 模板相关
    TEMPLATE_STYLE,
    TEMPLATE_ELEMENT,
    TEMPLATE_VAR,
    CUSTOM_STYLE,
    CUSTOM_ELEMENT,
    CUSTOM_VAR,
    
    // 导入相关
    IMPORT_STATEMENT,
    NAMESPACE_DECLARATION,
    
    // CHTL JS相关
    ENHANCED_SELECTOR,
    EVENT_LISTENER,
    ANIMATION,
    VIRTUAL_OBJECT,
    
    UNKNOWN_NODE
};

// Token结构
struct Token {
    TokenType type;
    String value;
    size_t line;
    size_t column;
    size_t position;
    
    Token(TokenType t = TokenType::UNKNOWN, const String& v = "", 
          size_t l = 0, size_t c = 0, size_t p = 0)
        : type(t), value(v), line(l), column(c), position(p) {}
};

// 代码片段结构
struct CodeFragment {
    FragmentType type;
    String content;
    size_t startLine;
    size_t startColumn;
    size_t endLine;
    size_t endColumn;
    
    CodeFragment(FragmentType t = FragmentType::UNKNOWN, const String& c = "",
                size_t sl = 0, size_t sc = 0, size_t el = 0, size_t ec = 0)
        : type(t), content(c), startLine(sl), startColumn(sc), endLine(el), endColumn(ec) {}
};

// 编译选项
struct CompilerOptions {
    bool debugMode = false;
    bool disableNameGroup = true;
    bool disableCustomOriginType = false;
    int indexInitialCount = 0;
    int optionCount = 3;
    String outputPath = "output.html";
    StringList modulePaths;
};

// 错误信息
struct CompilerError {
    String message;
    String filename;
    size_t line;
    size_t column;
    
    CompilerError(const String& msg, const String& file = "", size_t l = 0, size_t c = 0)
        : message(msg), filename(file), line(l), column(c) {}
};

// 编译结果
struct CompilerResult {
    bool success = false;
    String output;
    std::vector<CompilerError> errors;
    std::vector<String> warnings;
};

} // namespace CHTL