#include "CSS/CSSCompiler.h"
#include "Utils/ErrorHandler.h"
#include "Utils/StringUtils.h"

namespace CHTL {
namespace CSS {

CSSCompiler::CSSCompiler() {
    InitializeParser();
}

CSSCompiler::~CSSCompiler() {
    Cleanup();
}

void CSSCompiler::InitializeParser() {
    // 初始化ANTLR4 CSS解析器
    // 基础实现：暂时不集成复杂的ANTLR4
    errors_.clear();
}

std::string CSSCompiler::Compile(const std::string& cssCode, const std::string& fileName) {
    // 编译CSS代码
    errors_.clear();
    
    if (cssCode.empty()) {
        return "";
    }
    
    try {
        // 基础CSS处理：移除注释，格式化
        std::string processedCSS = Utils::StringUtils::Trim(cssCode);
        
        // 基础语法验证
        if (!Validate(processedCSS)) {
            Utils::ErrorHandler::GetInstance().LogError(
                "CSS语法验证失败: " + fileName
            );
            return "";
        }
        
        // 返回处理后的CSS（基础实现）
        return processedCSS;
        
    } catch (const std::exception& e) {
        errors_.push_back("CSS编译异常: " + std::string(e.what()));
        Utils::ErrorHandler::GetInstance().LogError(
            "CSS编译异常: " + fileName + " - " + e.what()
        );
        return "";
    }
}

bool CSSCompiler::Validate(const std::string& cssCode) {
    // 基础CSS语法验证
    if (cssCode.empty()) {
        return true;
    }
    
    // 检查基础的CSS语法结构
    int braceCount = 0;
    bool inString = false;
    char stringChar = '\0';
    
    for (size_t i = 0; i < cssCode.length(); ++i) {
        char c = cssCode[i];
        
        if (inString) {
            if (c == stringChar && (i == 0 || cssCode[i-1] != '\\')) {
                inString = false;
            }
        } else {
            if (c == '"' || c == '\'') {
                inString = true;
                stringChar = c;
            } else if (c == '{') {
                braceCount++;
            } else if (c == '}') {
                braceCount--;
                if (braceCount < 0) {
                    errors_.push_back("CSS语法错误：多余的右花括号");
                    return false;
                }
            }
        }
    }
    
    if (braceCount != 0) {
        errors_.push_back("CSS语法错误：花括号不匹配");
        return false;
    }
    
    return true;
}

void CSSCompiler::Cleanup() {
    // 清理资源
    errors_.clear();
}

} // namespace CSS
} // namespace CHTL
