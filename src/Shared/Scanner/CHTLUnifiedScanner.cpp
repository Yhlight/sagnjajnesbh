#include "CHTLUnifiedScanner.h"
#include <iostream>
#include <regex>
#include <algorithm>
#include <sstream>

namespace chtl {
namespace scanner {

CHTLUnifiedScanner::CHTLUnifiedScanner() {
    initializeContextRules();
    initializeCHTLJSPatterns();
    initializeUndecoratedLiteralRules();
}

CHTLUnifiedScanner::~CHTLUnifiedScanner() = default;

// === 主扫描接口 ===

CHTLUnifiedScanner::ScanResult CHTLUnifiedScanner::scanCode(const std::string& code) {
    ScanResult result;
    clearErrors();
    
    try {
        // 基于上下文的精确扫描
        result.fragments = contextAwareScan(code);
        
        // 分析每个片段的特征
        for (auto& fragment : result.fragments) {
            analyzeCodeFeatures(fragment);
            
            // 统计上下文
            result.contextCounts[fragment.context]++;
        }
        
        result.hasErrors = hasErrors();
        result.errors = getErrors();
        result.warnings = getWarnings();
        
    } catch (const std::exception& e) {
        addError("扫描异常: " + std::string(e.what()));
        result.hasErrors = true;
        result.errors = getErrors();
    }
    
    return result;
}

std::vector<CHTLUnifiedScanner::CodeFragment> CHTLUnifiedScanner::contextAwareScan(const std::string& code) {
    // 基于上下文的精确扫描，不将无修饰字面量作为CHTL JS判断条件
    
    std::vector<CodeFragment> fragments;
    std::vector<std::string> contextStack;
    
    size_t position = 0;
    size_t codeLength = code.length();
    
    while (position < codeLength) {
        // 更新上下文栈
        updateContextStack(code, position, contextStack);
        
        // 识别当前上下文
        CodeContext currentContext = identifyContext(code, position, contextStack);
        
        // 找到当前上下文的结束位置
        size_t contextStart = position;
        size_t contextEnd = findContextEnd(code, position, currentContext, contextStack);
        
        if (contextEnd > contextStart) {
            // 创建代码片段
            CodeFragment fragment;
            fragment.content = extractBlockContent(code, contextStart, contextEnd);
            fragment.context = currentContext;
            fragment.startPosition = contextStart;
            fragment.endPosition = contextEnd;
            fragment.nestingLevel = static_cast<int>(contextStack.size());
            fragment.parentContext = getCurrentContext(contextStack);
            
            // 分析片段特征
            analyzeCodeFeatures(fragment);
            
            fragments.push_back(fragment);
            position = contextEnd;
        } else {
            position++;
        }
    }
    
    return fragments;
}

// === 上下文识别 ===

CHTLUnifiedScanner::CodeContext CHTLUnifiedScanner::identifyContext(const std::string& code, size_t position, const std::vector<std::string>& contextStack) {
    // 基于当前位置和上下文栈确定上下文类型
    
    if (position >= code.length()) {
        return CodeContext::CHTL_GLOBAL;
    }
    
    // 检查当前上下文栈
    std::string currentCtx = getCurrentContext(contextStack);
    
    // 如果在script块内部，这是CHTL JS的潜在区域
    if (currentCtx == "script") {
        return CodeContext::CHTL_SCRIPT;
    }
    
    // 如果在style块内部
    if (currentCtx == "style") {
        return CodeContext::CHTL_STYLE;
    }
    
    // 如果在text块内部
    if (currentCtx == "text") {
        return CodeContext::CHTL_TEXT;
    }
    
    // 检查是否在特殊块内部
    if (currentCtx.find("[Template]") != std::string::npos) {
        return CodeContext::CHTL_TEMPLATE;
    }
    
    if (currentCtx.find("[Custom]") != std::string::npos) {
        return CodeContext::CHTL_CUSTOM;
    }
    
    if (currentCtx.find("[Origin]") != std::string::npos) {
        return CodeContext::CHTL_ORIGIN;
    }
    
    if (currentCtx.find("[Configuration]") != std::string::npos) {
        return CodeContext::CHTL_CONFIGURATION;
    }
    
    if (currentCtx.find("[Import]") != std::string::npos) {
        return CodeContext::CHTL_IMPORT;
    }
    
    if (currentCtx.find("[Namespace]") != std::string::npos) {
        return CodeContext::CHTL_NAMESPACE;
    }
    
    // 检查是否在HTML元素内部
    if (isInHTMLElement(code, position, contextStack)) {
        return CodeContext::CHTL_ELEMENT;
    }
    
    return CodeContext::CHTL_GLOBAL;
}

bool CHTLUnifiedScanner::isCHTLJSContext(const std::string& code, size_t position, const std::vector<std::string>& contextStack) {
    // 仅在script{}块内部才可能是CHTL JS
    
    std::string currentCtx = getCurrentContext(contextStack);
    
    // 必须在script块内部
    if (currentCtx != "script") {
        return false;
    }
    
    // 进一步检查是否有CHTL JS特征
    // 注意：不使用无修饰字面量作为判断条件
    std::string scriptContent = extractCurrentBlockContent(code, position, contextStack);
    
    return detectCHTLJSFeatures(scriptContent);
}

bool CHTLUnifiedScanner::detectCHTLJSFeatures(const std::string& scriptContent) {
    // 基于特定语法特征（而非无修饰字面量）判断CHTL JS
    
    if (scriptContent.empty()) {
        return false;
    }
    
    // 检测CHTL JS特有的语法特征
    bool hasFeatures = false;
    
    // 1. 增强选择器 {{}}
    if (hasEnhancedSelectors(scriptContent)) {
        hasFeatures = true;
    }
    
    // 2. 箭头操作符 -> (排除注释)
    if (hasArrowOperators(scriptContent)) {
        hasFeatures = true;
    }
    
    // 3. CHTL JS函数调用
    if (hasCHTLJSFunctionCalls(scriptContent)) {
        hasFeatures = true;
    }
    
    // 4. 虚对象语法
    if (hasVirtualObjectSyntax(scriptContent)) {
        hasFeatures = true;
    }
    
    return hasFeatures;
}

// === 无修饰字面量处理 ===

std::vector<std::pair<size_t, std::string>> CHTLUnifiedScanner::scanUndecoratedLiterals(const std::string& content, CodeContext context) {
    // 只在适当的上下文中识别无修饰字面量
    
    std::vector<std::pair<size_t, std::string>> literals;
    
    if (!isValidUndecoratedLiteralContext(context)) {
        return literals; // 在不支持的上下文中返回空
    }
    
    // 在支持的上下文中扫描无修饰字面量
    if (context == CodeContext::CHTL_TEXT) {
        // text块中的无修饰字面量
        literals = scanTextBlockLiterals(content);
    } else if (context == CodeContext::CHTL_STYLE) {
        // style块中的CSS属性值无修饰字面量
        literals = scanStyleBlockLiterals(content);
    } else if (context == CodeContext::CHTL_SCRIPT) {
        // script块中的CHTL JS无修饰字面量（仅在特定位置）
        literals = scanScriptBlockLiterals(content);
    }
    
    return literals;
}

bool CHTLUnifiedScanner::isValidUndecoratedLiteralContext(CodeContext context) {
    // 验证无修饰字面量的上下文合法性
    
    return context == CodeContext::CHTL_TEXT ||          // text块支持
           context == CodeContext::CHTL_STYLE ||         // style块支持（CSS属性值）
           context == CodeContext::CHTL_SCRIPT ||        // script块支持（CHTL JS特定位置）
           context == CodeContext::CHTL_ELEMENT;         // 元素属性值支持
}

bool CHTLUnifiedScanner::isUndecoratedLiteralNotIdentifier(const std::string& value, CodeContext context) {
    // 区分无修饰字面量和标识符
    
    if (value.empty()) return false;
    
    // 在script上下文中，需要更严格的判断
    if (context == CodeContext::CHTL_SCRIPT) {
        // 检查是否在键值对的值位置
        // 只有在 key: value 的value位置才可能是无修饰字面量
        // 而不是在表达式或函数调用中
        
        // JavaScript关键字不是无修饰字面量
        std::vector<std::string> jsKeywords = {
            "function", "var", "let", "const", "if", "else", "for", "while", 
            "return", "true", "false", "null", "undefined", "this", "new"
        };
        
        if (std::find(jsKeywords.begin(), jsKeywords.end(), value) != jsKeywords.end()) {
            return false;
        }
        
        // 变量名不是无修饰字面量
        if (std::regex_match(value, std::regex(R"(^[a-zA-Z_$][a-zA-Z0-9_$]*$)"))) {
            return false; // 这是标识符
        }
    }
    
    // 在其他上下文中，常见的CSS值和文本值可以是无修饰字面量
    return true;
}

// === CHTL JS特征检测 ===

bool CHTLUnifiedScanner::hasEnhancedSelectors(const std::string& content) {
    // 检测增强选择器 {{选择器}}
    std::regex selectorRegex(R"(\{\{[^}]+\}\})");
    return std::regex_search(content, selectorRegex);
}

bool CHTLUnifiedScanner::hasArrowOperators(const std::string& content) {
    // 检测箭头操作符 -> (排除注释中的箭头)
    
    std::regex arrowRegex(R"([^/\*-]+->[^/\*-])"); // 排除注释中的箭头
    return std::regex_search(content, arrowRegex);
}

bool CHTLUnifiedScanner::hasCHTLJSFunctionCalls(const std::string& content) {
    // 检测CHTL JS函数调用
    
    for (const auto& keyword : chtljsKeywords_) {
        if (content.find(keyword) != std::string::npos) {
            return true;
        }
    }
    
    return false;
}

bool CHTLUnifiedScanner::hasVirtualObjectSyntax(const std::string& content) {
    // 检测虚对象语法 vir
    std::regex virRegex(R"(\bvir\s+\w+\s*=)");
    return std::regex_search(content, virRegex);
}

// === 上下文栈管理 ===

void CHTLUnifiedScanner::updateContextStack(const std::string& code, size_t position, std::vector<std::string>& contextStack) {
    // 根据当前扫描位置更新上下文栈
    
    if (position >= code.length()) return;
    
    // 检查是否遇到新的上下文开始
    for (const auto& [keyword, context] : contextKeywords_) {
        if (position + keyword.length() <= code.length() &&
            code.substr(position, keyword.length()) == keyword) {
            
            // 检查是否是完整的关键字（不是其他词的一部分）
            bool isCompleteKeyword = true;
            
            if (position > 0 && std::isalnum(code[position - 1])) {
                isCompleteKeyword = false;
            }
            
            if (position + keyword.length() < code.length() && 
                std::isalnum(code[position + keyword.length()])) {
                isCompleteKeyword = false;
            }
            
            if (isCompleteKeyword) {
                enterContext(keyword, contextStack);
                break;
            }
        }
    }
    
    // 检查是否遇到上下文结束（大括号匹配）
    if (position < code.length() && code[position] == '}') {
        if (!contextStack.empty()) {
            exitContext(contextStack);
        }
    }
}

void CHTLUnifiedScanner::enterContext(const std::string& contextName, std::vector<std::string>& contextStack) {
    contextStack.push_back(contextName);
    
    if (debugMode_) {
        std::cout << "进入上下文: " << contextName << " (层级: " << contextStack.size() << ")" << std::endl;
    }
}

void CHTLUnifiedScanner::exitContext(std::vector<std::string>& contextStack) {
    if (!contextStack.empty()) {
        std::string exitedContext = contextStack.back();
        contextStack.pop_back();
        
        if (debugMode_) {
            std::cout << "退出上下文: " << exitedContext << " (层级: " << contextStack.size() << ")" << std::endl;
        }
    }
}

std::string CHTLUnifiedScanner::getCurrentContext(const std::vector<std::string>& contextStack) {
    return contextStack.empty() ? "global" : contextStack.back();
}

// === 智能分析 ===

void CHTLUnifiedScanner::analyzeCodeFeatures(CodeFragment& fragment) {
    // 为每个代码片段分析其特征
    
    // 扫描无修饰字面量（仅在适当上下文）
    fragment.undecoratedLiterals = scanUndecoratedLiterals(fragment.content, fragment.context);
    fragment.containsUndecoratedLiterals = !fragment.undecoratedLiterals.empty();
    
    // 仅在CHTL_SCRIPT上下文中分析CHTL JS特征
    if (fragment.context == CodeContext::CHTL_SCRIPT) {
        fragment.containsCHTLJSFeatures = detectCHTLJSFeatures(fragment.content);
        
        if (fragment.containsCHTLJSFeatures) {
            fragment.hasEnhancedSelectors = hasEnhancedSelectors(fragment.content);
            fragment.hasArrowOperators = hasArrowOperators(fragment.content);
            fragment.hasListenCalls = fragment.content.find("listen(") != std::string::npos;
            fragment.hasDelegateCalls = fragment.content.find("delegate(") != std::string::npos;
            fragment.hasAnimateCalls = fragment.content.find("animate(") != std::string::npos;
            fragment.hasVirtualObjects = hasVirtualObjectSyntax(fragment.content);
        }
    }
    
    // 验证上下文一致性
    if (!validateContextConsistency(fragment)) {
        addWarning("上下文一致性验证失败: " + fragment.parentContext);
    }
}

void CHTLUnifiedScanner::analyzeContextSpecificFeatures(CodeFragment& fragment) {
    // 根据不同上下文分析不同的特征
    
    switch (fragment.context) {
        case CodeContext::CHTL_SCRIPT:
            // 仅在script上下文中检测CHTL JS特征
            fragment.containsCHTLJSFeatures = detectCHTLJSFeatures(fragment.content);
            break;
            
        case CodeContext::CHTL_STYLE:
            // 在style上下文中检测CSS特征
            fragment.containsUndecoratedLiterals = !scanStyleBlockLiterals(fragment.content).empty();
            break;
            
        case CodeContext::CHTL_TEXT:
            // 在text上下文中检测文本特征
            fragment.containsUndecoratedLiterals = !scanTextBlockLiterals(fragment.content).empty();
            break;
            
        default:
            // 其他上下文的特征分析
            break;
    }
}

bool CHTLUnifiedScanner::validateContextConsistency(const CodeFragment& fragment) {
    // 验证上下文一致性
    
    // script块内部不应该有CHTL元素语法
    if (fragment.context == CodeContext::CHTL_SCRIPT) {
        if (fragment.content.find("[Template]") != std::string::npos ||
            fragment.content.find("[Custom]") != std::string::npos) {
            return false;
        }
    }
    
    // style块内部不应该有JavaScript语法
    if (fragment.context == CodeContext::CHTL_STYLE) {
        if (fragment.content.find("function") != std::string::npos ||
            fragment.content.find("console.log") != std::string::npos) {
            return false;
        }
    }
    
    return true;
}

// === 内部辅助方法 ===

size_t CHTLUnifiedScanner::findContextEnd(const std::string& code, size_t startPos, CodeContext context, const std::vector<std::string>& contextStack) {
    // 找到上下文的结束位置
    
    size_t bracePos = code.find('{', startPos);
    if (bracePos == std::string::npos) {
        return code.length();
    }
    
    return findMatchingBrace(code, bracePos);
}

size_t CHTLUnifiedScanner::findMatchingBrace(const std::string& code, size_t startPos) {
    // 找到匹配的大括号
    
    if (startPos >= code.length() || code[startPos] != '{') {
        return std::string::npos;
    }
    
    int braceCount = 1;
    size_t pos = startPos + 1;
    bool inString = false;
    char stringChar = '\0';
    
    while (pos < code.length() && braceCount > 0) {
        char c = code[pos];
        
        // 处理字符串状态
        if ((c == '"' || c == '\'') && (pos == 0 || code[pos-1] != '\\')) {
            if (!inString) {
                inString = true;
                stringChar = c;
            } else if (c == stringChar) {
                inString = false;
                stringChar = '\0';
            }
        }
        
        if (!inString) {
            if (c == '{') {
                braceCount++;
            } else if (c == '}') {
                braceCount--;
            }
        }
        
        pos++;
    }
    
    return braceCount == 0 ? pos : std::string::npos;
}

std::string CHTLUnifiedScanner::extractBlockContent(const std::string& code, size_t startPos, size_t endPos) {
    if (startPos >= endPos || endPos > code.length()) {
        return "";
    }
    
    return code.substr(startPos, endPos - startPos);
}

std::string CHTLUnifiedScanner::extractCurrentBlockContent(const std::string& code, size_t position, const std::vector<std::string>& contextStack) {
    // 提取当前块的内容
    
    // 向前找到块的开始
    size_t blockStart = position;
    while (blockStart > 0 && code[blockStart] != '{') {
        blockStart--;
    }
    
    if (blockStart < code.length() && code[blockStart] == '{') {
        blockStart++; // 跳过开始的大括号
    }
    
    // 向后找到块的结束
    size_t blockEnd = findMatchingBrace(code, blockStart - 1);
    if (blockEnd != std::string::npos) {
        blockEnd--; // 排除结束的大括号
    } else {
        blockEnd = code.length();
    }
    
    return extractBlockContent(code, blockStart, blockEnd);
}

bool CHTLUnifiedScanner::isInHTMLElement(const std::string& code, size_t position, const std::vector<std::string>& contextStack) {
    // 检查是否在HTML元素内部
    
    for (const auto& context : contextStack) {
        // 检查是否为HTML标签名
        if (isHTMLTag(context)) {
            return true;
        }
    }
    
    return false;
}

bool CHTLUnifiedScanner::isHTMLTag(const std::string& tagName) {
    // 检查是否为HTML标签
    std::vector<std::string> htmlTags = {
        "html", "head", "body", "div", "span", "p", "a", "img", "input", "button",
        "form", "table", "tr", "td", "ul", "ol", "li", "h1", "h2", "h3", "h4", "h5", "h6",
        "section", "article", "header", "footer", "nav", "main", "aside"
    };
    
    return std::find(htmlTags.begin(), htmlTags.end(), tagName) != htmlTags.end();
}

// === 特定上下文的字面量扫描 ===

std::vector<std::pair<size_t, std::string>> CHTLUnifiedScanner::scanTextBlockLiterals(const std::string& content) {
    // text块中的无修饰字面量扫描
    std::vector<std::pair<size_t, std::string>> literals;
    
    // text块中基本上所有内容都可以是无修饰字面量
    if (!content.empty() && content.find('"') == std::string::npos && content.find('\'') == std::string::npos) {
        literals.emplace_back(0, content);
    }
    
    return literals;
}

std::vector<std::pair<size_t, std::string>> CHTLUnifiedScanner::scanStyleBlockLiterals(const std::string& content) {
    // style块中的CSS属性值无修饰字面量
    std::vector<std::pair<size_t, std::string>> literals;
    
    // 查找CSS属性值
    std::regex cssPropertyRegex(R"((\w+[-\w]*)\s*:\s*([^;{}\n]+);)");
    std::sregex_iterator iter(content.begin(), content.end(), cssPropertyRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string property = (*iter)[1].str();
        std::string value = (*iter)[2].str();
        
        // 清理值
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        // 检查是否为无修饰字面量
        if (!value.empty() && value.front() != '"' && value.front() != '\'' && 
            value.find('(') == std::string::npos) { // 排除函数调用
            
            size_t pos = (*iter).position(2);
            literals.emplace_back(pos, value);
        }
    }
    
    return literals;
}

std::vector<std::pair<size_t, std::string>> CHTLUnifiedScanner::scanScriptBlockLiterals(const std::string& content) {
    // script块中的CHTL JS无修饰字面量（仅在键值对的值位置）
    std::vector<std::pair<size_t, std::string>> literals;
    
    // 只在对象字面量的值位置查找无修饰字面量
    std::regex keyValueRegex(R"((\w+)\s*:\s*([^,}\n;]+))");
    std::sregex_iterator iter(content.begin(), content.end(), keyValueRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string key = (*iter)[1].str();
        std::string value = (*iter)[2].str();
        
        // 清理值
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t,") + 1);
        
        // 检查是否为无修饰字面量且不是标识符
        if (isUndecoratedLiteralNotIdentifier(value, CodeContext::CHTL_SCRIPT)) {
            size_t pos = (*iter).position(2);
            literals.emplace_back(pos, value);
        }
    }
    
    return literals;
}

// === 初始化方法 ===

void CHTLUnifiedScanner::initializeContextRules() {
    // 初始化上下文识别规则
    contextKeywords_ = {
        {"[Template]", CodeContext::CHTL_TEMPLATE},
        {"[Custom]", CodeContext::CHTL_CUSTOM},
        {"[Origin]", CodeContext::CHTL_ORIGIN},
        {"[Configuration]", CodeContext::CHTL_CONFIGURATION},
        {"[Import]", CodeContext::CHTL_IMPORT},
        {"[Namespace]", CodeContext::CHTL_NAMESPACE},
        {"script", CodeContext::CHTL_SCRIPT},
        {"style", CodeContext::CHTL_STYLE},
        {"text", CodeContext::CHTL_TEXT}
    };
}

void CHTLUnifiedScanner::initializeCHTLJSPatterns() {
    // 初始化CHTL JS特征模式（不包括无修饰字面量）
    chtljsKeywords_ = {
        "listen", "delegate", "animate", "vir"
    };
    
    enhancedSelectorPatterns_ = {
        R"(\{\{[^}]+\}\})",           // 增强选择器
    };
    
    chtljsFunctionPatterns_ = {
        R"(->listen\s*\()",          // listen调用
        R"(->delegate\s*\()",        // delegate调用
        R"(\banimate\s*\()",         // animate调用
        R"(\bvir\s+\w+\s*=)"         // vir定义
    };
}

void CHTLUnifiedScanner::initializeUndecoratedLiteralRules() {
    // 初始化无修饰字面量的有效上下文
    validUndecoratedContexts_ = {
        "text", "style", "script"  // 只在这些上下文中支持无修饰字面量
    };
}

// === 错误处理 ===

void CHTLUnifiedScanner::addError(const std::string& message) {
    errors_.push_back("CHTLUnifiedScanner: " + message);
}

void CHTLUnifiedScanner::addWarning(const std::string& message) {
    warnings_.push_back("CHTLUnifiedScanner: " + message);
}

void CHTLUnifiedScanner::clearErrors() {
    errors_.clear();
    warnings_.clear();
}

bool CHTLUnifiedScanner::hasErrors() const {
    return !errors_.empty();
}

std::vector<std::string> CHTLUnifiedScanner::getErrors() const {
    return errors_;
}

std::vector<std::string> CHTLUnifiedScanner::getWarnings() const {
    return warnings_;
}

void CHTLUnifiedScanner::setDebugMode(bool enabled) {
    debugMode_ = enabled;
}

void CHTLUnifiedScanner::setStrictMode(bool enabled) {
    strictMode_ = enabled;
}

void CHTLUnifiedScanner::setContextSensitivity(int level) {
    contextSensitivity_ = std::max(1, std::min(5, level));
}

} // namespace scanner
} // namespace chtl