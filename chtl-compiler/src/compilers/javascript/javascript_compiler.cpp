#include "compilers/javascript/javascript_compiler.h"
#include <sstream>
#include <regex>

#ifdef USE_ANTLR
#include "antlr4-runtime.h"
// ANTLR生成的JavaScript解析器头文件（需要ANTLR语法文件）
// #include "JavaScriptLexer.h"
// #include "JavaScriptParser.h"
#endif

namespace chtl {

// PIMPL实现类
class JavaScriptCompiler::Impl {
public:
    Impl() {}
    
    std::string compile(const std::string& source) {
#ifdef USE_ANTLR
        // 使用ANTLR解析JavaScript
        // antlr4::ANTLRInputStream input(source);
        // JavaScriptLexer lexer(&input);
        // antlr4::CommonTokenStream tokens(&lexer);
        // JavaScriptParser parser(&tokens);
        // ... 解析和处理
        
        // 暂时返回原始JavaScript
        return source;
#else
        // 使用备用实现
        return fallbackCompile(source);
#endif
    }
    
    std::string fallbackCompile(const std::string& source) {
        // 基本的JavaScript处理
        std::stringstream result;
        
        // 添加严格模式
        if (source.find("'use strict'") == std::string::npos && 
            source.find("\"use strict\"") == std::string::npos) {
            result << "'use strict';\n\n";
        }
        
        // 基本的语法检查
        int braceCount = 0;
        int parenCount = 0;
        int bracketCount = 0;
        bool inString = false;
        bool inRegex = false;
        char stringChar = 0;
        
        for (size_t i = 0; i < source.length(); ++i) {
            char ch = source[i];
            char prevCh = (i > 0) ? source[i-1] : 0;
            
            // 处理字符串
            if (!inString && !inRegex && (ch == '"' || ch == '\'' || ch == '`')) {
                inString = true;
                stringChar = ch;
            } else if (inString && ch == stringChar && prevCh != '\\') {
                inString = false;
            }
            
            // 处理正则表达式
            if (!inString && !inRegex && ch == '/' && 
                (i + 1 < source.length() && source[i+1] != '/' && source[i+1] != '*')) {
                // 简单的正则检测
                inRegex = true;
            } else if (inRegex && ch == '/' && prevCh != '\\') {
                inRegex = false;
            }
            
            // 计数括号
            if (!inString && !inRegex) {
                switch (ch) {
                    case '{': braceCount++; break;
                    case '}': braceCount--; break;
                    case '(': parenCount++; break;
                    case ')': parenCount--; break;
                    case '[': bracketCount++; break;
                    case ']': bracketCount--; break;
                }
            }
        }
        
        // 检查括号匹配
        if (braceCount != 0) {
            result << "/* JavaScript Syntax Error: Unmatched braces {} */\n";
        }
        if (parenCount != 0) {
            result << "/* JavaScript Syntax Error: Unmatched parentheses () */\n";
        }
        if (bracketCount != 0) {
            result << "/* JavaScript Syntax Error: Unmatched brackets [] */\n";
        }
        
        result << source;
        
        // 包装在IIFE中以避免全局污染
        std::string output = result.str();
        if (output.find("function") != std::string::npos || 
            output.find("var ") != std::string::npos ||
            output.find("let ") != std::string::npos ||
            output.find("const ") != std::string::npos) {
            
            return "(function() {\n" + output + "\n})();";
        }
        
        return output;
    }
};

JavaScriptCompiler::JavaScriptCompiler() : pImpl_(std::make_unique<Impl>()) {
}

JavaScriptCompiler::~JavaScriptCompiler() = default;

std::string JavaScriptCompiler::compile(const std::string& source) {
    errors_.clear();
    warnings_.clear();
    
    try {
#ifdef USE_ANTLR
        return pImpl_->compile(source);
#else
        return fallbackCompile(source);
#endif
    } catch (const std::exception& e) {
        errors_.push_back("JavaScript Compilation error: " + std::string(e.what()));
        return "";
    }
}

void JavaScriptCompiler::reset() {
    errors_.clear();
    warnings_.clear();
}

void JavaScriptCompiler::setOption(const std::string& key, const std::string& value) {
    // 存储选项供后续使用
}

std::string JavaScriptCompiler::fallbackCompile(const std::string& source) {
    // 基本的JavaScript验证和处理
    std::stringstream output;
    
    // 检查是否包含ES6+特性并添加提示
    std::regex es6Features(R"(\b(class|extends|arrow|=>|let|const|async|await|import|export)\b)");
    if (std::regex_search(source, es6Features)) {
        warnings_.push_back("ES6+ features detected. Consider transpiling for older browsers.");
    }
    
    // 检查常见的错误模式
    std::regex undefinedVar(R"(\b(undefined\s*=|null\s*=))");
    if (std::regex_search(source, undefinedVar)) {
        errors_.push_back("Attempting to assign to undefined or null");
    }
    
    // 添加错误处理包装
    output << "try {\n";
    output << pImpl_->fallbackCompile(source);
    output << "\n} catch (e) {\n";
    output << "  console.error('[CHTL JavaScript Runtime Error]', e);\n";
    output << "}\n";
    
    return output.str();
}

} // namespace chtl