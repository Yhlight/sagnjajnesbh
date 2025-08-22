#include "JavaScript/JavaScriptCompiler.h"
#include "Utils/ErrorHandler.h"
#include "Utils/StringUtils.h"

namespace CHTL {
namespace JavaScript {

JavaScriptCompiler::JavaScriptCompiler() {
    InitializeParser();
}

JavaScriptCompiler::~JavaScriptCompiler() {
    Cleanup();
}

void JavaScriptCompiler::InitializeParser() {
    // 初始化ANTLR4 JavaScript解析器
    // 基础实现：暂时不集成复杂的ANTLR4
    errors_.clear();
}

std::string JavaScriptCompiler::Compile(const std::string& jsCode, const std::string& fileName) {
    // 编译JavaScript代码
    errors_.clear();
    
    if (jsCode.empty()) {
        return "";
    }
    
    try {
        // 基础JavaScript处理：移除注释，格式化
        std::string processedJS = Utils::StringUtils::Trim(jsCode);
        
        // 基础语法验证
        if (!Validate(processedJS)) {
            Utils::ErrorHandler::GetInstance().LogError(
                "JavaScript语法验证失败: " + fileName
            );
            return "";
        }
        
        // 返回处理后的JavaScript（基础实现）
        return processedJS;
        
    } catch (const std::exception& e) {
        errors_.push_back("JavaScript编译异常: " + std::string(e.what()));
        Utils::ErrorHandler::GetInstance().LogError(
            "JavaScript编译异常: " + fileName + " - " + e.what()
        );
        return "";
    }
}

bool JavaScriptCompiler::Validate(const std::string& jsCode) {
    // 基础JavaScript语法验证
    if (jsCode.empty()) {
        return true;
    }
    
    // 检查基础的JavaScript语法结构
    int braceCount = 0;
    int parenCount = 0;
    int bracketCount = 0;
    bool inString = false;
    char stringChar = '\0';
    
    for (size_t i = 0; i < jsCode.length(); ++i) {
        char c = jsCode[i];
        
        if (inString) {
            if (c == stringChar && (i == 0 || jsCode[i-1] != '\\')) {
                inString = false;
            }
        } else {
            if (c == '"' || c == '\'' || c == '`') {
                inString = true;
                stringChar = c;
            } else if (c == '{') {
                braceCount++;
            } else if (c == '}') {
                braceCount--;
                if (braceCount < 0) {
                    errors_.push_back("JavaScript语法错误：多余的右花括号");
                    return false;
                }
            } else if (c == '(') {
                parenCount++;
            } else if (c == ')') {
                parenCount--;
                if (parenCount < 0) {
                    errors_.push_back("JavaScript语法错误：多余的右括号");
                    return false;
                }
            } else if (c == '[') {
                bracketCount++;
            } else if (c == ']') {
                bracketCount--;
                if (bracketCount < 0) {
                    errors_.push_back("JavaScript语法错误：多余的右方括号");
                    return false;
                }
            }
        }
    }
    
    if (braceCount != 0) {
        errors_.push_back("JavaScript语法错误：花括号不匹配");
        return false;
    }
    
    if (parenCount != 0) {
        errors_.push_back("JavaScript语法错误：圆括号不匹配");
        return false;
    }
    
    if (bracketCount != 0) {
        errors_.push_back("JavaScript语法错误：方括号不匹配");
        return false;
    }
    
    return true;
}

void JavaScriptCompiler::Cleanup() {
    // 清理资源
    errors_.clear();
}

} // namespace JavaScript
} // namespace CHTL
