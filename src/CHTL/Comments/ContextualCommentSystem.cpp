#include "CHTL/Comments/ContextualCommentSystem.h"
#include "Utils/StringUtils.h"
#include "Utils/ErrorHandler.h"
#include <regex>

namespace CHTL {
namespace Comments {

// ContextualCommentGenerator实现
ContextualCommentGenerator::ContextualCommentGenerator() {}

std::string ContextualCommentGenerator::GenerateComment(const std::string& comment, ProgrammingContext context) const {
    switch (context) {
        case ProgrammingContext::HTML:
            return GenerateHTMLComment(comment);
            
        case ProgrammingContext::CSS:
            return GenerateCSSComment(comment);
            
        case ProgrammingContext::JAVASCRIPT:
            return GenerateJavaScriptComment(comment);
            
        case ProgrammingContext::CHTL:
            return GenerateCHTLComment(comment);
            
        case ProgrammingContext::CHTL_JS:
            return GenerateCHTLJSComment(comment);
            
        default:
            return GenerateCHTLComment(comment); // 默认使用CHTL注释格式
    }
}

ProgrammingContext ContextualCommentGenerator::DetectContext(const std::string& surroundingCode) const {
    // 增强的上下文检测逻辑
    
    // 使用正则表达式进行更精确的检测
    
    // 检查是否在[Origin] @Style块中或CSS样式块中
    std::regex cssPatterns(R"((\[Origin\]\s*@Style|style\s*\{|\.[\w-]+\s*\{|#[\w-]+\s*\{|@media|@keyframes|background\s*:|color\s*:|font\s*:|margin\s*:|padding\s*:|display\s*:|position\s*:))");
    if (std::regex_search(surroundingCode, cssPatterns)) {
        return ProgrammingContext::CSS;
    }
    
    // 检查是否在[Origin] @JavaScript块中或CHTL JS脚本块中
    std::regex chtlJSPatterns(R"((\[Origin\]\s*@JavaScript|script\s*\{|\{\{[^}]*\}\}|vir\s+\w+|printMylove|iNeverAway|listen\s*\(|delegate\s*\(|->|\$\(\w+\)))");
    if (std::regex_search(surroundingCode, chtlJSPatterns)) {
        return ProgrammingContext::CHTL_JS;
    }
    
    // 检查是否在原始JavaScript代码中
    std::regex jsPatterns(R"((function\s+\w+|var\s+\w+|const\s+\w+|let\s+\w+|console\.(log|error|warn)|document\.|window\.|addEventListener|setTimeout|setInterval|new\s+\w+))");
    if (std::regex_search(surroundingCode, jsPatterns)) {
        return ProgrammingContext::JAVASCRIPT;
    }
    
    // 检查是否在[Origin] @Html块中或HTML元素中
    std::regex htmlPatterns(R"((\[Origin\]\s*@Html|<[\w\-]+[^>]*>|</[\w\-]+>|html\s*\{|body\s*\{|div\s*\{|span\s*\{|p\s*\{|a\s*\{|img\s*\{|input\s*\{))");
    if (std::regex_search(surroundingCode, htmlPatterns)) {
        return ProgrammingContext::HTML;
    }
    
    // 检查是否在CHTL模板或自定义定义中
    std::regex chtlPatterns(R"((\[Template\]|\[Custom\]|\[Import\]|\[Namespace\]|\[Configuration\]|@Style\s+\w+|@Element\s+\w+|@Var\s+\w+))");
    if (std::regex_search(surroundingCode, chtlPatterns)) {
        return ProgrammingContext::CHTL;
    }
    
    // 默认为CHTL上下文
    return ProgrammingContext::CHTL;
}

std::string ContextualCommentGenerator::ProcessDashComment(const std::string& comment, ProgrammingContext context) const {
    // "--"注释会根据上下文生成不同编程语言的注释
    return GenerateComment(comment, context);
}

std::string ContextualCommentGenerator::GenerateHTMLComment(const std::string& comment) const {
    return "<!-- " + comment + " -->";
}

std::string ContextualCommentGenerator::GenerateCSSComment(const std::string& comment) const {
    return "/* " + comment + " */";
}

std::string ContextualCommentGenerator::GenerateJavaScriptComment(const std::string& comment) const {
    // 检查是否为多行注释
    if (comment.find('\n') != std::string::npos) {
        return "/* " + comment + " */";
    } else {
        return "// " + comment;
    }
}

std::string ContextualCommentGenerator::GenerateCHTLComment(const std::string& comment) const {
    return "// " + comment;
}

std::string ContextualCommentGenerator::GenerateCHTLJSComment(const std::string& comment) const {
    // CHTL JS使用JavaScript风格的注释
    return GenerateJavaScriptComment(comment);
}

// OriginAnywhereManager实现
OriginAnywhereManager::OriginAnywhereManager() {}

bool OriginAnywhereManager::CanUseOriginHere(const std::string& context, const std::string& surroundingCode) const {
    // [Origin]原始嵌入节点任意地方都可以使用，不应该被束缚
    // 问题由开发者自己控制
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "检查[Origin]使用位置: " + context + "，周围代码长度: " + std::to_string(surroundingCode.length())
    );
    
    // 始终返回true，允许在任意位置使用
    return true;
}

std::pair<bool, std::string> OriginAnywhereManager::ValidateOriginUsage(const std::string& originType, 
                                                                       const std::string& context) const {
    // [Origin]原始嵌入节点任意地方都可以使用
    // 但可以提供一些建议
    
    bool isValid = true;
    std::string suggestion;
    
    // 检查上下文兼容性并提供建议
    if (IsContextCompatible(originType, context)) {
        suggestion = "建议：" + originType + "类型在" + context + "上下文中使用是合理的";
    } else {
        suggestion = "注意：" + originType + "类型在" + context + "上下文中使用可能不太常见，请确认是否符合预期";
    }
    
    return {isValid, suggestion};
}

std::string OriginAnywhereManager::GetOriginUsageSuggestion(const std::string& context) const {
    std::string suggestion = "[Origin]原始嵌入节点可以在任意地方使用，包括：\n";
    suggestion += "- HTML元素内部：嵌入HTML片段\n";
    suggestion += "- 样式块内部：嵌入CSS代码\n";
    suggestion += "- 脚本块内部：嵌入JavaScript代码\n";
    suggestion += "- CHTL结构内部：嵌入CHTL代码片段\n";
    suggestion += "- 自定义位置：嵌入自定义类型内容\n";
    suggestion += "\n当前上下文: " + context;
    suggestion += "\n注意：使用的合理性由开发者自己控制";
    
    return suggestion;
}

bool OriginAnywhereManager::IsContextCompatible(const std::string& originType, const std::string& context) const {
    // 提供官方支持的三种原始嵌入类型的兼容性建议
    std::unordered_map<std::string, std::vector<std::string>> compatibilityMap = {
        {"@Html", {"HTML", "CHTL", "element"}},
        {"@Style", {"CSS", "style", "CHTL"}},
        {"@JavaScript", {"JavaScript", "script", "CHTL_JS"}}
    };
    
    auto it = compatibilityMap.find(originType);
    if (it != compatibilityMap.end()) {
        for (const auto& compatibleContext : it->second) {
            if (context.find(compatibleContext) != std::string::npos) {
                return true;
            }
        }
    }
    
    // 默认认为兼容（因为[Origin]可以在任意地方使用）
    return true;
}

} // namespace Comments
} // namespace CHTL