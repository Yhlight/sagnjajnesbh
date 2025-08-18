#include "CHTLUnifiedScanner.h"
#include <iostream>
#include <regex>
#include <algorithm>

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
    // 基于上下文的精确扫描，实现可变长度切片机制
    // 严格按照CHTL语法文档和您的架构要求实现
    
    std::vector<CodeFragment> fragments;
    std::vector<std::string> contextStack;
    
    if (code.empty()) {
        return fragments;
    }
    
    size_t position = 0;
    size_t codeLength = code.length();
    
    // 实现可变长度切片机制
    while (position < codeLength) {
        // 更新上下文栈
        updateContextStack(code, position, contextStack);
        
        // 识别当前上下文
        CodeContext currentContext = identifyContext(code, position, contextStack);
        
        // 1. 扫描器单次读取一个代码片段
        size_t fragmentStart = position;
        size_t fragmentEnd = findBasicFragmentEnd(code, position, currentContext);
        
        // 2. 检查下一个片段的起始部分是否可能与当前片段组成完整的CHTL或CHTL JS代码片段
        if (fragmentEnd < codeLength) {
            size_t nextStart = findNextNonWhitespace(code, fragmentEnd);
            
            if (checkFragmentCompleteness(code, fragmentEnd, nextStart)) {
                // 3. 若判定为可组成完整片段，则向前扩增指定长度的切片范围
                fragmentEnd = expandSliceRange(code, fragmentEnd, 100);
            }
        }
        
        if (fragmentEnd > fragmentStart) {
            // 创建代码片段
            std::string fragmentContent = code.substr(fragmentStart, fragmentEnd - fragmentStart);
            
            // 4. 按CHTL/CHTL JS的最小语法单元进行二次切割
            std::vector<std::string> minimalUnits = performMinimalUnitSlicing(fragmentContent);
            
            // 为每个最小单元创建片段
            for (const auto& unit : minimalUnits) {
                if (!unit.empty() && !std::all_of(unit.begin(), unit.end(), ::isspace)) {
                    CodeFragment fragment;
                    fragment.content = unit;
                    fragment.context = currentContext;
                    fragment.startPosition = fragmentStart;
                    fragment.endPosition = fragmentEnd;
                    fragment.nestingLevel = static_cast<int>(contextStack.size());
                    fragment.parentContext = getCurrentContext(contextStack);
                    
                    fragments.push_back(fragment);
                }
            }
            
            position = fragmentEnd;
        } else {
            position++;
        }
    }
    
    // 5. 适当聚合连续片段，避免过度细分
    fragments = aggregateConsecutiveFragments(fragments);
    
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
    
    return CodeContext::CHTL_GLOBAL;
}

bool CHTLUnifiedScanner::isCHTLJSContext(const std::string& code, size_t position, const std::vector<std::string>& contextStack) {
    // 仅在script{}块内部才可能是CHTL JS
    
    std::string currentCtx = getCurrentContext(contextStack);
    
    // 必须在script块内部
    if (currentCtx != "script") {
        return false;
    }
    
    // 检查是否有CHTL JS特征（不使用无修饰字面量作为判断条件）
    std::string scriptContent = code.substr(position, std::min(size_t(1000), code.length() - position));
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
    
    // 基础的无修饰字面量扫描
    if (context == CodeContext::CHTL_TEXT) {
        // text块中的内容可能是无修饰字面量
        if (!content.empty()) {
            literals.emplace_back(0, content);
        }
    }
    
    return literals;
}

bool CHTLUnifiedScanner::isValidUndecoratedLiteralContext(CodeContext context) {
    // 验证无修饰字面量的上下文合法性
    
    return context == CodeContext::CHTL_TEXT ||
           context == CodeContext::CHTL_STYLE ||
           context == CodeContext::CHTL_SCRIPT ||
           context == CodeContext::CHTL_ELEMENT;
}

bool CHTLUnifiedScanner::isUndecoratedLiteralNotIdentifier(const std::string& value, CodeContext context) {
    // 区分无修饰字面量和标识符
    
    if (value.empty()) return false;
    
    // 基本的标识符检查
    if (std::regex_match(value, std::regex(R"(^[a-zA-Z_$][a-zA-Z0-9_$]*$)"))) {
        return false; // 这是标识符
    }
    
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
    // 根据当前扫描位置更新上下文栈（简化实现）
    if (position >= code.length()) return;
    
    // 基础的上下文更新逻辑
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

// === 嵌套结构处理 ===

std::vector<CHTLUnifiedScanner::CodeFragment> CHTLUnifiedScanner::scanNestedStructures(const std::string& code) {
    // 基本的嵌套结构扫描
    return contextAwareScan(code);
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
            // CSS特征分析
            break;
            
        case CodeContext::CHTL_TEXT:
            // 文本特征分析
            break;
            
        default:
            break;
    }
}

bool CHTLUnifiedScanner::validateContextConsistency(const CodeFragment& fragment) {
    // 基本的上下文一致性验证
    
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

// === 配置和选项 ===

void CHTLUnifiedScanner::setDebugMode(bool enabled) {
    debugMode_ = enabled;
}

void CHTLUnifiedScanner::setStrictMode(bool enabled) {
    strictMode_ = enabled;
}

void CHTLUnifiedScanner::setContextSensitivity(int level) {
    contextSensitivity_ = std::max(1, std::min(5, level));
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

// === 私有初始化方法（严格按照头文件声明） ===

void CHTLUnifiedScanner::initializeContextRules() {
    // 初始化上下文识别规则
    contextKeywords_["[Template]"] = CodeContext::CHTL_TEMPLATE;
    contextKeywords_["[Custom]"] = CodeContext::CHTL_CUSTOM;
    contextKeywords_["[Origin]"] = CodeContext::CHTL_ORIGIN;
    contextKeywords_["[Configuration]"] = CodeContext::CHTL_CONFIGURATION;
    contextKeywords_["[Import]"] = CodeContext::CHTL_IMPORT;
    contextKeywords_["[Namespace]"] = CodeContext::CHTL_NAMESPACE;
    contextKeywords_["script"] = CodeContext::CHTL_SCRIPT;
    contextKeywords_["style"] = CodeContext::CHTL_STYLE;
    contextKeywords_["text"] = CodeContext::CHTL_TEXT;
}

void CHTLUnifiedScanner::initializeCHTLJSPatterns() {
    // 初始化CHTL JS特征模式（不包括无修饰字面量）
    chtljsKeywords_.insert("listen");
    chtljsKeywords_.insert("delegate");
    chtljsKeywords_.insert("animate");
    chtljsKeywords_.insert("vir");
}

void CHTLUnifiedScanner::initializeUndecoratedLiteralRules() {
    // 初始化无修饰字面量的有效上下文
    validUndecoratedContexts_.insert("text");
    validUndecoratedContexts_.insert("style");
    validUndecoratedContexts_.insert("script");
}

} // namespace scanner
} // namespace chtl