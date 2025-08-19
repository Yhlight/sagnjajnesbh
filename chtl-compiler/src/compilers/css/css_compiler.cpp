#include "compilers/css/css_compiler.h"
#include <sstream>

#ifdef USE_ANTLR
#include "antlr4-runtime.h"
// ANTLR生成的CSS解析器头文件（需要ANTLR语法文件）
// #include "CSSLexer.h"
// #include "CSSParser.h"
#endif

namespace chtl {

// PIMPL实现类
class CSSCompiler::Impl {
public:
    Impl() {}
    
    std::string compile(const std::string& source) {
#ifdef USE_ANTLR
        // 使用ANTLR解析CSS
        // antlr4::ANTLRInputStream input(source);
        // CSSLexer lexer(&input);
        // antlr4::CommonTokenStream tokens(&lexer);
        // CSSParser parser(&tokens);
        // ... 解析和处理
        
        // 暂时返回原始CSS
        return source;
#else
        // 使用备用实现
        return fallbackCompile(source);
#endif
    }
    
    std::string fallbackCompile(const std::string& source) {
        // 简单的CSS处理
        // 这里只做最基本的验证和格式化
        std::stringstream result;
        std::string processedSource = source;
        
        // 移除多余的空白
        bool inString = false;
        char stringChar = 0;
        std::string cleaned;
        
        for (size_t i = 0; i < processedSource.length(); ++i) {
            char ch = processedSource[i];
            
            if (!inString && (ch == '"' || ch == '\'')) {
                inString = true;
                stringChar = ch;
            } else if (inString && ch == stringChar && 
                      (i == 0 || processedSource[i-1] != '\\')) {
                inString = false;
            }
            
            if (!inString) {
                // 压缩多个空白为一个
                if (std::isspace(ch)) {
                    if (cleaned.empty() || !std::isspace(cleaned.back())) {
                        cleaned += ' ';
                    }
                } else {
                    cleaned += ch;
                }
            } else {
                cleaned += ch;
            }
        }
        
        // 简单的语法检查
        int braceCount = 0;
        for (char ch : cleaned) {
            if (ch == '{') braceCount++;
            else if (ch == '}') braceCount--;
        }
        
        if (braceCount != 0) {
            return "/* CSS Syntax Error: Unmatched braces */\n" + source;
        }
        
        return cleaned;
    }
};

CSSCompiler::CSSCompiler() : pImpl_(std::make_unique<Impl>()) {
}

CSSCompiler::~CSSCompiler() = default;

std::string CSSCompiler::compile(const std::string& source) {
    errors_.clear();
    warnings_.clear();
    
    try {
#ifdef USE_ANTLR
        return pImpl_->compile(source);
#else
        return fallbackCompile(source);
#endif
    } catch (const std::exception& e) {
        errors_.push_back("CSS Compilation error: " + std::string(e.what()));
        return "";
    }
}

void CSSCompiler::reset() {
    errors_.clear();
    warnings_.clear();
}

void CSSCompiler::setOption(const std::string& key, const std::string& value) {
    // 存储选项供后续使用
}

std::string CSSCompiler::fallbackCompile(const std::string& source) {
    // 基本的CSS验证和处理
    std::stringstream output;
    
    // 添加一些基本的CSS重置
    if (source.find("*") == std::string::npos && 
        source.find("body") == std::string::npos) {
        output << "/* CHTL CSS Compiler - Basic Reset */\n";
        output << "* { margin: 0; padding: 0; box-sizing: border-box; }\n";
        output << "body { font-family: sans-serif; line-height: 1.6; }\n\n";
    }
    
    // 处理CSS变量
    std::string processed = source;
    
    // 简单的语法高亮注释
    size_t pos = 0;
    while ((pos = processed.find("/*", pos)) != std::string::npos) {
        size_t endPos = processed.find("*/", pos + 2);
        if (endPos != std::string::npos) {
            std::string comment = processed.substr(pos, endPos - pos + 2);
            if (comment.find("CHTL") != std::string::npos) {
                // 保留CHTL相关注释
                pos = endPos + 2;
            } else {
                // 普通注释
                pos = endPos + 2;
            }
        } else {
            errors_.push_back("Unclosed comment in CSS");
            break;
        }
    }
    
    output << processed;
    
    return output.str();
}

} // namespace chtl