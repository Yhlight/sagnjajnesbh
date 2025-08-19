#pragma once
#include <string>
#include <vector>
#include <regex>
#include <map>

namespace chtl {
namespace test {
namespace utils {

/**
 * CHTL语法验证器
 * 根据CHTL语法文档实现精确的语法验证
 */
class CHTLSyntaxValidator {
public:
    struct ValidationResult {
        bool isValid;
        std::string errorMessage;
        std::vector<std::string> warnings;
    };
    
    // === 基础语法验证 ===
    static ValidationResult validateCHTLCode(const std::string& code);
    static ValidationResult validateCHTLJSCode(const std::string& code);
    static ValidationResult validateImportStatement(const std::string& import);
    
    // === 语法元素验证 ===
    static bool hasValidComments(const std::string& code);
    static bool hasValidTextNodes(const std::string& code);
    static bool hasValidLiterals(const std::string& code);
    static bool hasValidCEEquality(const std::string& code);
    static bool hasValidElementNodes(const std::string& code);
    
    // === CHTL JS语法验证 ===
    static bool hasValidSelectors(const std::string& code);
    static bool hasValidArrowOperators(const std::string& code);
    static bool hasValidFunctions(const std::string& code);
    static bool hasValidVirtualObjects(const std::string& code);
    
    // === 导入系统验证 ===
    static bool isValidHtmlImport(const std::string& import);
    static bool isValidStyleImport(const std::string& import);
    static bool isValidJavaScriptImport(const std::string& import);
    static bool isValidChtlImport(const std::string& import);
    static bool isValidCJmodImport(const std::string& import);
    static bool isValidWildcardImport(const std::string& import);
    
    // === 命名空间验证 ===
    static bool hasValidNamespaces(const std::string& code);
    static bool hasValidNestedNamespaces(const std::string& code);
    
    // === 模块系统验证 ===
    static bool isValidCMODStructure(const std::string& path);
    static bool isValidCJMODStructure(const std::string& path);
    
    // === 错误检测 ===
    static std::vector<std::string> detectSyntaxErrors(const std::string& code);
    static std::vector<std::string> detectImportErrors(const std::string& code);
    static std::vector<std::string> detectNamespaceErrors(const std::string& code);
    
private:
    // 正则表达式模式
    static const std::regex COMMENT_PATTERN;
    static const std::regex TEXT_NODE_PATTERN;
    static const std::regex ELEMENT_PATTERN;
    static const std::regex CHTLJS_SELECTOR_PATTERN;
    static const std::regex ARROW_OPERATOR_PATTERN;
    static const std::regex IMPORT_PATTERN;
    static const std::regex NAMESPACE_PATTERN;
    static const std::regex VIR_OBJECT_PATTERN;
    
    // 辅助方法
    static bool matchesPattern(const std::string& text, const std::regex& pattern);
    static std::vector<std::string> extractMatches(const std::string& text, const std::regex& pattern);
    static bool isValidIdentifier(const std::string& identifier);
    static bool isValidPath(const std::string& path);
};

} // namespace utils
} // namespace test
} // namespace chtl