#include "CHTLJSCompiler.h"
#include <iostream>
#include <regex>
#include <algorithm>

namespace chtl {
namespace chtljs {

CHTLJSCompiler::CHTLJSCompiler() {
    initializeProcessors();
}

CHTLJSCompiler::~CHTLJSCompiler() = default;

// === 按语法文档：主编译接口 ===

CHTLJSCompiler::CompilationResult CHTLJSCompiler::compile(const std::string& chtljsCode) {
    // 按语法文档：将CHTL JS语法转换为标准JavaScript代码
    
    CompilationResult result;
    clearErrors();
    clearWarnings();
    
    if (chtljsCode.empty()) {
        addError("空的CHTL JS代码");
        result.success = false;
        result.errors = getErrors();
        return result;
    }
    
    try {
        // 1. 预处理代码
        std::string preprocessed = preprocessCode(chtljsCode);
        
        // 2. 按语法文档顺序编译各种语法
        std::string compiled = preprocessed;
        
        // 2.1 编译虚对象定义（必须最先）
        compiled = compileVirtualObjects(compiled);
        
        // 2.2 编译增强选择器
        compiled = compileEnhancedSelectors(compiled);
        
        // 2.3 编译箭头操作符
        compiled = compileArrowOperators(compiled);
        
        // 2.4 编译listen语句
        compiled = compileListenStatements(compiled);
        
        // 2.5 编译delegate语句
        compiled = compileDelegateStatements(compiled);
        
        // 2.6 编译animate语句
        compiled = compileAnimateStatements(compiled);
        
        // 3. 后处理代码
        compiled = postprocessCode(compiled);
        
        // 4. 验证编译结果
        bool isValid = validateCompiledCode(compiled);
        
        // 5. 生成结果
        result.compiledCode = compiled;
        result.success = isValid && !hasErrors();
        result.errors = getErrors();
        result.warnings = getWarnings();
        
        // 6. 更新统计信息
        updateCompilationStatistics(result, chtljsCode);
        
        // 7. 缓存结果
        lastResult_ = result;
        
    } catch (const std::exception& e) {
        addError("编译异常: " + std::string(e.what()));
        result.success = false;
        result.errors = getErrors();
    }
    
    return result;
}

std::string CHTLJSCompiler::compileLocalScript(const std::string& scriptContent, const std::string& elementContext) {
    // 按语法文档：处理元素内部的script{}块
    return scriptProcessor_->processLocalScript(scriptContent, elementContext);
}

std::string CHTLJSCompiler::generateGlobalScript() {
    // 按语法文档：合并所有局部script为高优先级全局script
    
    std::string globalScript = scriptProcessor_->generateGlobalScript();
    
    // 添加全局委托注册表
    std::string delegateRegistry = delegateProcessor_->generateGlobalDelegateRegistry();
    
    // 添加虚对象全局函数
    std::string virtualFunctions = virtualProcessor_->generateAllGlobalFunctions();
    
    // 合并所有全局代码
    std::string combined = "// 按CHTL语法文档：完整的CHTL JS全局代码\n\n";
    
    if (!virtualFunctions.empty()) {
        combined += "// === 虚对象全局函数 ===\n";
        combined += virtualFunctions + "\n";
    }
    
    if (!delegateRegistry.empty()) {
        combined += "// === 事件委托全局注册表 ===\n";
        combined += delegateRegistry + "\n";
    }
    
    if (!globalScript.empty()) {
        combined += "// === 局部script全局合并 ===\n";
        combined += globalScript + "\n";
    }
    
    return combined;
}

// === 按语法文档：各种CHTL JS语法编译 ===

std::string CHTLJSCompiler::compileEnhancedSelectors(const std::string& code) {
    // 按语法文档：{{CSS选择器}} → DOM对象
    return scriptProcessor_->processEnhancedSelectors(code);
}

std::string CHTLJSCompiler::compileArrowOperators(const std::string& code) {
    // 按语法文档：-> 与 . 完全等价
    return scriptProcessor_->processArrowOperators(code);
}

std::string CHTLJSCompiler::compileListenStatements(const std::string& code) {
    // 按语法文档：{{box}}->listen({ click: () => {} })
    return scriptProcessor_->processListenStatements(code);
}

std::string CHTLJSCompiler::compileDelegateStatements(const std::string& code) {
    // 按语法文档：{{parent}}->delegate({ target: {{child}}, click: func })
    return scriptProcessor_->processDelegateStatements(code);
}

std::string CHTLJSCompiler::compileAnimateStatements(const std::string& code) {
    // 按语法文档：animate({ target: {{selector}}, duration: 100 })
    return scriptProcessor_->processAnimateStatements(code);
}

std::string CHTLJSCompiler::compileVirtualObjects(const std::string& code) {
    // 按语法文档：vir test = listen({ click: () => {} }); test->click();
    std::string processed = scriptProcessor_->processVirtualObjectDefinitions(code);
    processed = scriptProcessor_->processVirtualObjectCalls(processed);
    return processed;
}

// === 编译流程控制 ===

std::string CHTLJSCompiler::preprocessCode(const std::string& code) {
    std::string preprocessed = code;
    
    // 移除CHTL注释，保留JavaScript注释
    std::regex chtlCommentRegex(R"(--[^\n]*\n)");
    preprocessed = std::regex_replace(preprocessed, chtlCommentRegex, "\n");
    
    // 标准化换行符
    std::regex windowsNewline(R"(\r\n)");
    preprocessed = std::regex_replace(preprocessed, windowsNewline, "\n");
    
    return preprocessed;
}

std::string CHTLJSCompiler::postprocessCode(const std::string& code) {
    std::string postprocessed = code;
    
    // 根据优化级别进行后处理
    if (optimizationLevel_ >= 1) {
        // 移除多余的空行
        std::regex multipleNewlines(R"(\n\s*\n\s*\n)");
        postprocessed = std::regex_replace(postprocessed, multipleNewlines, "\n\n");
    }
    
    if (optimizationLevel_ >= 2) {
        // 优化生成的代码结构
        // 简化实现：这里可以添加更多优化
    }
    
    if (optimizationLevel_ >= 3) {
        // 高级优化：内联小函数、消除死代码等
        // 简化实现：这里可以添加更多高级优化
    }
    
    return postprocessed;
}

bool CHTLJSCompiler::validateCompiledCode(const std::string& code) {
    // 基本的JavaScript语法验证
    
    // 检查括号匹配
    int braceCount = 0;
    int parenCount = 0;
    int bracketCount = 0;
    
    for (char c : code) {
        switch (c) {
            case '{': braceCount++; break;
            case '}': braceCount--; break;
            case '(': parenCount++; break;
            case ')': parenCount--; break;
            case '[': bracketCount++; break;
            case ']': bracketCount--; break;
        }
    }
    
    if (braceCount != 0) {
        addError("JavaScript代码大括号不匹配");
        return false;
    }
    
    if (parenCount != 0) {
        addError("JavaScript代码圆括号不匹配");
        return false;
    }
    
    if (bracketCount != 0) {
        addError("JavaScript代码方括号不匹配");
        return false;
    }
    
    return true;
}

// === 配置和选项 ===

void CHTLJSCompiler::setDebugMode(bool enabled) {
    debugMode_ = enabled;
    
    // 传递给所有处理器
    if (selectorProcessor_) selectorProcessor_->clearErrors();
    if (listenProcessor_) listenProcessor_->clearErrors();
    if (delegateProcessor_) delegateProcessor_->clearErrors();
    if (virtualProcessor_) virtualProcessor_->clearErrors();
    if (arrowProcessor_) arrowProcessor_->clearErrors();
    if (scriptProcessor_) scriptProcessor_->clearErrors();
}

void CHTLJSCompiler::setOptimizationLevel(int level) {
    optimizationLevel_ = std::max(0, std::min(3, level));
}

void CHTLJSCompiler::setTargetECMAVersion(const std::string& version) {
    targetECMAVersion_ = version;
}

CHTLJSCompiler::CompilationResult CHTLJSCompiler::getLastCompilationResult() const {
    return lastResult_;
}

void CHTLJSCompiler::reset() {
    clearErrors();
    clearWarnings();
    lastResult_ = CompilationResult{};
}

// === 内部方法 ===

void CHTLJSCompiler::initializeProcessors() {
    selectorProcessor_ = std::make_shared<EnhancedSelectorProcessor>();
    listenProcessor_ = std::make_shared<ListenSystemProcessor>();
    delegateProcessor_ = std::make_shared<DelegateSystemProcessor>();
    animateProcessor_ = std::make_shared<AnimateSystemProcessor>();
    virtualProcessor_ = std::make_shared<VirtualObjectProcessor>();
    arrowProcessor_ = std::make_shared<ArrowOperatorProcessor>();
    scriptProcessor_ = std::make_shared<LocalScriptProcessor>();
}

void CHTLJSCompiler::updateCompilationStatistics(CompilationResult& result, const std::string& code) {
    // 统计各种语法的使用情况
    
    // 统计选择器
    std::regex selectorRegex(R"(\{\{[^}]+\}\})");
    result.selectorsProcessed = std::distance(
        std::sregex_iterator(code.begin(), code.end(), selectorRegex),
        std::sregex_iterator()
    );
    
    // 统计listen语句
    std::regex listenRegex(R"(->listen\s*\()");
    result.listenStatementsProcessed = std::distance(
        std::sregex_iterator(code.begin(), code.end(), listenRegex),
        std::sregex_iterator()
    );
    
    // 统计delegate语句
    std::regex delegateRegex(R"(->delegate\s*\()");
    result.delegateStatementsProcessed = std::distance(
        std::sregex_iterator(code.begin(), code.end(), delegateRegex),
        std::sregex_iterator()
    );
    
    // 统计animate语句
    std::regex animateRegex(R"(\banimate\s*\()");
    result.animateStatementsProcessed = std::distance(
        std::sregex_iterator(code.begin(), code.end(), animateRegex),
        std::sregex_iterator()
    );
    
    // 统计虚对象
    std::regex virRegex(R"(\bvir\s+\w+\s*=)");
    result.virtualObjectsProcessed = std::distance(
        std::sregex_iterator(code.begin(), code.end(), virRegex),
        std::sregex_iterator()
    );
}

// === 错误和警告处理 ===

void CHTLJSCompiler::addError(const std::string& message) {
    errors_.push_back("CHTLJSCompiler: " + message);
    if (debugMode_) {
        std::cerr << "CHTL JS编译错误: " << message << std::endl;
    }
}

void CHTLJSCompiler::addWarning(const std::string& message) {
    warnings_.push_back("CHTLJSCompiler: " + message);
    if (debugMode_) {
        std::cout << "CHTL JS编译警告: " << message << std::endl;
    }
}

void CHTLJSCompiler::clearErrors() {
    errors_.clear();
}

void CHTLJSCompiler::clearWarnings() {
    warnings_.clear();
}

bool CHTLJSCompiler::hasErrors() const {
    return !errors_.empty();
}

bool CHTLJSCompiler::hasWarnings() const {
    return !warnings_.empty();
}

std::vector<std::string> CHTLJSCompiler::getErrors() const {
    return errors_;
}

std::vector<std::string> CHTLJSCompiler::getWarnings() const {
    return warnings_;
}

} // namespace chtljs
} // namespace chtl