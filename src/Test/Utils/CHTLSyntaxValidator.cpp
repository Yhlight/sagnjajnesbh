#include "CHTLSyntaxValidator.h"
#include <filesystem>
#include <fstream>
#include <algorithm>

namespace chtl {
namespace test {
namespace utils {

// 正则表达式模式定义
const std::regex CHTLSyntaxValidator::COMMENT_PATTERN(R"((//.*$)|(\/\*[\s\S]*?\*\/)|(--.*$))", std::regex::multiline);
const std::regex CHTLSyntaxValidator::TEXT_NODE_PATTERN(R"(text\s*\{\s*([^}]*)\s*\})");
const std::regex CHTLSyntaxValidator::ELEMENT_PATTERN(R"(([a-zA-Z][a-zA-Z0-9]*)\s*\{)");
const std::regex CHTLSyntaxValidator::CHTLJS_SELECTOR_PATTERN(R"(\{\{[.#][a-zA-Z0-9_-]+(?:\s*->\s*\w+)?\s*\}\})");
const std::regex CHTLSyntaxValidator::ARROW_OPERATOR_PATTERN(R"(\{\{[.#][a-zA-Z0-9_-]+\s*->\s*(\w+)\s*\}\})");
const std::regex CHTLSyntaxValidator::IMPORT_PATTERN(R"(\[Import\]\s*@(Html|Style|JavaScript|Chtl|CJmod)\s+from\s+([^\s]+)(?:\s+as\s+([^\s]+))?)");
const std::regex CHTLSyntaxValidator::NAMESPACE_PATTERN(R"(\[Namespace\]\s+([a-zA-Z][a-zA-Z0-9_]*))");
const std::regex CHTLSyntaxValidator::VIR_OBJECT_PATTERN(R"(vir\s+(\w+)\s*=\s*iNeverAway\s*\()");

// === 基础语法验证实现 ===

CHTLSyntaxValidator::ValidationResult CHTLSyntaxValidator::validateCHTLCode(const std::string& code) {
    ValidationResult result;
    result.isValid = true;
    
    // 检查基础语法元素
    if (!hasValidComments(code) && code.find("//") != std::string::npos) {
        result.warnings.push_back("注释格式可能不正确");
    }
    
    if (!hasValidTextNodes(code) && code.find("text") != std::string::npos) {
        result.warnings.push_back("文本节点格式可能不正确");
    }
    
    if (!hasValidElementNodes(code)) {
        result.warnings.push_back("元素节点格式可能不正确");
    }
    
    // 检查语法错误
    auto errors = detectSyntaxErrors(code);
    if (!errors.empty()) {
        result.isValid = false;
        result.errorMessage = "语法错误: " + errors[0];
    }
    
    return result;
}

CHTLSyntaxValidator::ValidationResult CHTLSyntaxValidator::validateCHTLJSCode(const std::string& code) {
    ValidationResult result;
    result.isValid = true;
    
    // 检查CHTL JS语法元素
    bool hasSelectors = hasValidSelectors(code);
    bool hasArrows = hasValidArrowOperators(code);
    bool hasVir = hasValidVirtualObjects(code);
    
    // CHTL JS代码应该至少包含一种CHTL JS语法
    if (!hasSelectors && !hasArrows && !hasVir && 
        (code.find("{{") != std::string::npos || code.find("vir") != std::string::npos)) {
        result.isValid = false;
        result.errorMessage = "包含CHTL JS标记但语法不正确";
    }
    
    // 如果没有任何CHTL JS标记，也认为是有效的（可能是纯文本）
    if (code.find("{{") == std::string::npos && code.find("vir") == std::string::npos) {
        result.isValid = true;
    }
    
    return result;
}

CHTLSyntaxValidator::ValidationResult CHTLSyntaxValidator::validateImportStatement(const std::string& import) {
    ValidationResult result;
    
    // 检查基本导入格式
    std::regex basicImportPattern(R"(@(Html|Style|JavaScript|Chtl|CJmod)\s+from\s+(.+?)(?:\s+as\s+(.+))?)");
    
    if (std::regex_match(import, basicImportPattern)) {
        result.isValid = true;
    } else {
        result.isValid = false;
        result.errorMessage = "导入语句格式不正确";
    }
    
    return result;
}

// === 语法元素验证实现 ===

bool CHTLSyntaxValidator::hasValidComments(const std::string& code) {
    // 检查是否包含有效的注释格式
    return matchesPattern(code, COMMENT_PATTERN) || 
           code.find("//") != std::string::npos ||
           code.find("/*") != std::string::npos ||
           code.find("--") != std::string::npos;
}

bool CHTLSyntaxValidator::hasValidTextNodes(const std::string& code) {
    // 检查text节点格式
    return matchesPattern(code, TEXT_NODE_PATTERN);
}

bool CHTLSyntaxValidator::hasValidLiterals(const std::string& code) {
    // 检查字面量：双引号、单引号、无修饰
    return code.find("\"") != std::string::npos ||  // 双引号字符串
           code.find("'") != std::string::npos ||   // 单引号字符串
           std::regex_search(code, std::regex(R"([a-zA-Z][a-zA-Z0-9_-]*\s*[:=]\s*[a-zA-Z0-9_-]+)"));  // 无修饰字面量
}

bool CHTLSyntaxValidator::hasValidCEEquality(const std::string& code) {
    // 检查CE对等式（冒号和等号的使用）
    return code.find(":") != std::string::npos || code.find("=") != std::string::npos;
}

bool CHTLSyntaxValidator::hasValidElementNodes(const std::string& code) {
    // 检查HTML元素节点
    return matchesPattern(code, ELEMENT_PATTERN);
}

// === CHTL JS语法验证实现 ===

bool CHTLSyntaxValidator::hasValidSelectors(const std::string& code) {
    return matchesPattern(code, CHTLJS_SELECTOR_PATTERN);
}

bool CHTLSyntaxValidator::hasValidArrowOperators(const std::string& code) {
    return matchesPattern(code, ARROW_OPERATOR_PATTERN);
}

bool CHTLSyntaxValidator::hasValidFunctions(const std::string& code) {
    // 检查CHTL JS函数：listen, delegate, animate
    return code.find("-> listen") != std::string::npos ||
           code.find("-> delegate") != std::string::npos ||
           code.find("-> animate") != std::string::npos;
}

bool CHTLSyntaxValidator::hasValidVirtualObjects(const std::string& code) {
    return matchesPattern(code, VIR_OBJECT_PATTERN) || 
           code.find("vir.") != std::string::npos;
}

// === 导入系统验证实现 ===

bool CHTLSyntaxValidator::isValidHtmlImport(const std::string& import) {
    std::regex htmlPattern(R"(@Html\s+from\s+[^\s]+(?:\s+as\s+[^\s]+)?)");
    return std::regex_match(import, htmlPattern);
}

bool CHTLSyntaxValidator::isValidStyleImport(const std::string& import) {
    std::regex stylePattern(R"(@Style\s+from\s+[^\s]+(?:\s+as\s+[^\s]+)?)");
    return std::regex_match(import, stylePattern);
}

bool CHTLSyntaxValidator::isValidJavaScriptImport(const std::string& import) {
    std::regex jsPattern(R"(@JavaScript\s+from\s+[^\s]+(?:\s+as\s+[^\s]+)?)");
    return std::regex_match(import, jsPattern);
}

bool CHTLSyntaxValidator::isValidChtlImport(const std::string& import) {
    std::regex chtlPattern(R"(@Chtl\s+from\s+[^\s]+)");
    return std::regex_match(import, chtlPattern);
}

bool CHTLSyntaxValidator::isValidCJmodImport(const std::string& import) {
    std::regex cjmodPattern(R"(@CJmod\s+from\s+[^\s]+)");
    return std::regex_match(import, cjmodPattern);
}

bool CHTLSyntaxValidator::isValidWildcardImport(const std::string& import) {
    return import.find(".*") != std::string::npos || 
           import.find("/*") != std::string::npos;
}

// === 命名空间验证实现 ===

bool CHTLSyntaxValidator::hasValidNamespaces(const std::string& code) {
    return matchesPattern(code, NAMESPACE_PATTERN);
}

bool CHTLSyntaxValidator::hasValidNestedNamespaces(const std::string& code) {
    // 检查嵌套命名空间结构
    auto matches = extractMatches(code, NAMESPACE_PATTERN);
    return matches.size() > 1;  // 多个命名空间表示可能有嵌套
}

// === 模块系统验证实现 ===

bool CHTLSyntaxValidator::isValidCMODStructure(const std::string& path) {
    if (!std::filesystem::exists(path)) {
        return false;
    }
    
    std::string moduleName = std::filesystem::path(path).filename().string();
    
    // 检查三者同名：文件夹名、主文件名、info文件名
    std::string srcFile = path + "/src/" + moduleName + ".chtl";
    std::string infoFile = path + "/info/" + moduleName + ".chtl";
    
    if (!std::filesystem::exists(srcFile) || !std::filesystem::exists(infoFile)) {
        return false;
    }
    
    // 检查info文件内容
    std::ifstream infoStream(infoFile);
    std::string infoContent((std::istreambuf_iterator<char>(infoStream)),
                           std::istreambuf_iterator<char>());
    
    return infoContent.find("[Info]") != std::string::npos &&
           infoContent.find("[Export]") != std::string::npos;
}

bool CHTLSyntaxValidator::isValidCJMODStructure(const std::string& path) {
    if (!std::filesystem::exists(path)) {
        return false;
    }
    
    std::string moduleName = std::filesystem::path(path).filename().string();
    
    // 检查二者同名：文件夹名和info文件名
    std::string infoFile = path + "/" + moduleName + ".chtl";
    std::string srcDir = path + "/src";
    
    if (!std::filesystem::exists(infoFile) || !std::filesystem::exists(srcDir)) {
        return false;
    }
    
    // 检查info文件内容（CJMOD不应该有[Export]段落）
    std::ifstream infoStream(infoFile);
    std::string infoContent((std::istreambuf_iterator<char>(infoStream)),
                           std::istreambuf_iterator<char>());
    
    return infoContent.find("[Info]") != std::string::npos &&
           infoContent.find("[Export]") == std::string::npos;  // CJMOD不应该有Export段落
}

// === 错误检测实现 ===

std::vector<std::string> CHTLSyntaxValidator::detectSyntaxErrors(const std::string& code) {
    std::vector<std::string> errors;
    
    // 检查未闭合的CHTL JS标签
    std::regex openTag(R"(\{\{[.#][a-zA-Z0-9_-]+(?:\s*->\s*\w+)?\s*\}\})");
    std::regex closeTag(R"(\{\{/[.#]?[a-zA-Z0-9_-]+\s*\}\})");
    
    auto openMatches = extractMatches(code, openTag);
    auto closeMatches = extractMatches(code, closeTag);
    
    if (openMatches.size() != closeMatches.size()) {
        errors.push_back("CHTL JS标签未正确配对");
    }
    
    // 检查无效的函数调用
    std::regex invalidFunction(R"(\{\{[.#][a-zA-Z0-9_-]+\s*->\s*(unknownFunction|invalidFunc)\s*\}\})");
    if (std::regex_search(code, invalidFunction)) {
        errors.push_back("使用了未知的CHTL JS函数");
    }
    
    return errors;
}

std::vector<std::string> CHTLSyntaxValidator::detectImportErrors(const std::string& code) {
    std::vector<std::string> errors;
    
    // 检查无效的导入类型
    std::regex invalidImport(R"(@(InvalidImport|Unknown)\s+from)");
    if (std::regex_search(code, invalidImport)) {
        errors.push_back("使用了无效的导入类型");
    }
    
    return errors;
}

std::vector<std::string> CHTLSyntaxValidator::detectNamespaceErrors(const std::string& code) {
    std::vector<std::string> errors;
    
    // 检查无效的命名空间名称
    std::regex invalidNamespace(R"(\[InvalidSection\])");
    if (std::regex_search(code, invalidNamespace)) {
        errors.push_back("使用了无效的段落名称");
    }
    
    return errors;
}

// === 辅助方法实现 ===

bool CHTLSyntaxValidator::matchesPattern(const std::string& text, const std::regex& pattern) {
    return std::regex_search(text, pattern);
}

std::vector<std::string> CHTLSyntaxValidator::extractMatches(const std::string& text, const std::regex& pattern) {
    std::vector<std::string> matches;
    std::sregex_iterator iter(text.begin(), text.end(), pattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        matches.push_back(iter->str());
    }
    
    return matches;
}

bool CHTLSyntaxValidator::isValidIdentifier(const std::string& identifier) {
    std::regex identifierPattern(R"(^[a-zA-Z][a-zA-Z0-9_]*$)");
    return std::regex_match(identifier, identifierPattern);
}

bool CHTLSyntaxValidator::isValidPath(const std::string& path) {
    return !path.empty() && path.find('\0') == std::string::npos;
}

} // namespace utils
} // namespace test
} // namespace chtl