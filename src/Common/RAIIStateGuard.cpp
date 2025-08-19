#include "RAIIStateGuard.h"
#include <iostream>
#include <algorithm>

namespace chtl {
namespace raii {

// === RAIIStateGuard实现 ===

RAIIStateGuard::RAIIStateGuard(StateManager& stateManager, CompilerState newState, SyntaxContext context, 
                               const std::string& description)
    : stateManager_(stateManager), description_(description) {
    
    // 保存之前的状态
    previousState_ = stateManager_.getCurrentState();
    previousContext_ = stateManager_.getCurrentContext();
    
    // 设置新状态
    currentState_ = newState;
    currentContext_ = context;
    
    // 尝试转换状态
    if (stateManager_.transition(newState, context, description)) {
        triggerEnterCallbacks();
        
        if (stateManager_.isDebugMode()) {
            std::cout << "[RAII Guard] 进入状态: " << static_cast<int>(newState) 
                      << " 上下文: " << static_cast<int>(context)
                      << " (" << description << ")" << std::endl;
        }
    } else {
        setError("状态转换失败: " + description);
    }
}

RAIIStateGuard::~RAIIStateGuard() {
    try {
        triggerExitCallbacks();
        
        if (stateManager_.isDebugMode()) {
            std::cout << "[RAII Guard] 退出状态: " << static_cast<int>(currentState_) 
                      << " 恢复到: " << static_cast<int>(previousState_)
                      << " (" << description_ << ")" << std::endl;
        }
        
        // 自动恢复到之前的状态
        stateManager_.popState();
        
    } catch (const std::exception& e) {
        std::cerr << "[RAII Guard] 析构异常: " << e.what() << std::endl;
    }
}

// === AST节点状态管理 ===

void RAIIStateGuard::markNodeState(void* nodePtr, const std::string& state) {
    if (nodePtr) {
        nodeStates_[nodePtr] = state;
        
        if (stateManager_.isDebugMode()) {
            std::cout << "[AST Node] 节点 " << nodePtr << " 状态: " << state << std::endl;
        }
    }
}

std::string RAIIStateGuard::getNodeState(void* nodePtr) const {
    auto it = nodeStates_.find(nodePtr);
    return it != nodeStates_.end() ? it->second : "unknown";
}

void RAIIStateGuard::setNodeData(void* nodePtr, const std::string& key, const std::string& value) {
    if (nodePtr) {
        nodeData_[nodePtr][key] = value;
    }
}

std::string RAIIStateGuard::getNodeData(void* nodePtr, const std::string& key) const {
    auto nodeIt = nodeData_.find(nodePtr);
    if (nodeIt != nodeData_.end()) {
        auto dataIt = nodeIt->second.find(key);
        if (dataIt != nodeIt->second.end()) {
            return dataIt->second;
        }
    }
    return "";
}

// === 解析器支持 ===

bool RAIIStateGuard::isValidParseState() const {
    // 检查当前状态是否适合解析
    return currentState_ != CompilerState::ERROR_STATE &&
           currentState_ != CompilerState::IDLE;
}

std::string RAIIStateGuard::getParserStateInfo() const {
    return "解析器状态: " + std::to_string(static_cast<int>(currentState_)) + 
           " 上下文: " + std::to_string(static_cast<int>(currentContext_));
}

bool RAIIStateGuard::isInParserContext(SyntaxContext context) const {
    return currentContext_ == context;
}

// === 生成器支持 ===

bool RAIIStateGuard::isValidGenerateState() const {
    // 检查当前状态是否适合生成
    std::vector<CompilerState> validGenerateStates = {
        CompilerState::PARSING_CHTL,
        CompilerState::PARSING_TEMPLATE,
        CompilerState::PARSING_CUSTOM,
        CompilerState::PARSING_ORIGIN
    };
    
    return std::find(validGenerateStates.begin(), validGenerateStates.end(), currentState_) 
           != validGenerateStates.end();
}

std::string RAIIStateGuard::getGeneratorStateInfo() const {
    return "生成器状态: " + std::to_string(static_cast<int>(currentState_)) + 
           " 上下文: " + std::to_string(static_cast<int>(currentContext_));
}

bool RAIIStateGuard::isInGeneratorContext(SyntaxContext context) const {
    return currentContext_ == context;
}

// === 状态监控和回调 ===

void RAIIStateGuard::onStateEnter(std::function<void(CompilerState, SyntaxContext)> callback) {
    enterCallbacks_.push_back(callback);
}

void RAIIStateGuard::onStateExit(std::function<void(CompilerState, SyntaxContext)> callback) {
    exitCallbacks_.push_back(callback);
}

CompilerState RAIIStateGuard::getCurrentState() const {
    return currentState_;
}

SyntaxContext RAIIStateGuard::getCurrentContext() const {
    return currentContext_;
}

std::string RAIIStateGuard::getDescription() const {
    return description_;
}

// === 错误处理 ===

void RAIIStateGuard::setError(const std::string& errorMessage) {
    hasError_ = true;
    errorMessage_ = errorMessage;
    
    if (stateManager_.isDebugMode()) {
        std::cout << "[RAII Guard Error] " << errorMessage << std::endl;
    }
}

bool RAIIStateGuard::hasError() const {
    return hasError_;
}

std::string RAIIStateGuard::getErrorMessage() const {
    return errorMessage_;
}

// === 私有方法 ===

void RAIIStateGuard::triggerEnterCallbacks() {
    for (const auto& callback : enterCallbacks_) {
        try {
            callback(currentState_, currentContext_);
        } catch (const std::exception& e) {
            setError("进入回调异常: " + std::string(e.what()));
        }
    }
}

void RAIIStateGuard::triggerExitCallbacks() {
    for (const auto& callback : exitCallbacks_) {
        try {
            callback(currentState_, currentContext_);
        } catch (const std::exception& e) {
            std::cerr << "[RAII Guard] 退出回调异常: " << e.what() << std::endl;
        }
    }
}

// === ContextHelper实现 ===

ContextHelper::ContextHelper(StateManager& stateManager) : stateManager_(stateManager) {
    initializeSyntaxConstraints();
}

ContextHelper::~ContextHelper() = default;

// === 与扫描器协同工作 ===

void ContextHelper::updateFromScannerResult(const scanner::CHTLUnifiedScanner::ScanResult& scanResult) {
    // 根据扫描器结果更新上下文
    
    for (const auto& fragment : scanResult.fragments) {
        SyntaxContext syntaxContext = getScannerContext(fragment.context);
        
        // 验证一致性
        if (!validateScannerContextConsistency(fragment.context)) {
            std::cerr << "[ContextHelper] 扫描器上下文不一致" << std::endl;
        }
    }
}

SyntaxContext ContextHelper::getScannerContext(scanner::CHTLUnifiedScanner::CodeContext scannerContext) const {
    // 将扫描器上下文映射到语法上下文
    
    switch (scannerContext) {
        case scanner::CHTLUnifiedScanner::CodeContext::CHTL_GLOBAL:
            return SyntaxContext::GLOBAL;
        case scanner::CHTLUnifiedScanner::CodeContext::CHTL_STYLE:
            return SyntaxContext::LOCAL_STYLE;
        case scanner::CHTLUnifiedScanner::CodeContext::CHTL_SCRIPT:
            return SyntaxContext::LOCAL_SCRIPT;
        case scanner::CHTLUnifiedScanner::CodeContext::CHTL_TEMPLATE:
            return SyntaxContext::TEMPLATE_DEFINITION;
        case scanner::CHTLUnifiedScanner::CodeContext::CHTL_CUSTOM:
            return SyntaxContext::CUSTOM_DEFINITION;
        case scanner::CHTLUnifiedScanner::CodeContext::CHTL_NAMESPACE:
            return SyntaxContext::NAMESPACE_DEFINITION;
        case scanner::CHTLUnifiedScanner::CodeContext::CHTL_IMPORT:
            return SyntaxContext::IMPORT_STATEMENT;
        case scanner::CHTLUnifiedScanner::CodeContext::CHTL_TEXT:
            return SyntaxContext::TEXT_CONTENT;
        case scanner::CHTLUnifiedScanner::CodeContext::CHTL_ORIGIN:
            return SyntaxContext::ORIGIN_BLOCK;
        default:
            return SyntaxContext::GLOBAL;
    }
}

bool ContextHelper::validateScannerContextConsistency(scanner::CHTLUnifiedScanner::CodeContext scannerContext) const {
    // 验证扫描器上下文与编译器状态的一致性
    
    SyntaxContext expectedSyntaxContext = getScannerContext(scannerContext);
    SyntaxContext currentSyntaxContext = stateManager_.getCurrentContext();
    
    return expectedSyntaxContext == currentSyntaxContext || currentSyntaxContext == SyntaxContext::GLOBAL;
}

// === 语法约束支持 ===

bool ContextHelper::isAllowedInCurrentContext(const std::string& syntaxElement) const {
    SyntaxContext currentContext = stateManager_.getCurrentContext();
    
    switch (currentContext) {
        case SyntaxContext::GLOBAL:
            return true; // 全局上下文允许所有语法
            
        case SyntaxContext::LOCAL_STYLE:
            return std::find(localStyleAllowed_.begin(), localStyleAllowed_.end(), syntaxElement) 
                   != localStyleAllowed_.end();
            
        case SyntaxContext::LOCAL_SCRIPT:
            return std::find(localScriptAllowed_.begin(), localScriptAllowed_.end(), syntaxElement) 
                   != localScriptAllowed_.end();
            
        default:
            return true;
    }
}

std::vector<std::string> ContextHelper::getGlobalStyleAllowedSyntax() const {
    // 全局样式块允许的语法元素（严格按照您的要求）
    return {
        "模板变量", "自定义变量", "自定义变量特例化",
        "模板样式组", "自定义样式组", "无值样式组", "自定义样式组特例化",
        "delete属性", "delete继承", "样式组间继承",
        "生成器注释", "全缀名", "任意类型原始嵌入",
        "命名空间from语法"
    };
}

std::vector<std::string> ContextHelper::getLocalStyleAllowedSyntax() const {
    // 局部样式块允许的语法元素（与全局样式块相同）
    return getGlobalStyleAllowedSyntax();
}

std::vector<std::string> ContextHelper::getLocalScriptAllowedSyntax() const {
    // 局部script允许的CHTL语法元素（严格按照您的要求）
    return {
        "模板变量", "自定义变量组", "变量组特例化", "命名空间from",
        "注释", "任意类型原始嵌入", "{{&}}等特供语法"
    };
}

bool ContextHelper::isValidInNonLocalScript(const std::string& syntaxElement) const {
    // 除局部script外，其他script禁止使用任何CHTL语法
    // 允许例外：注释及任意类型原始嵌入
    return std::find(nonLocalScriptExceptions_.begin(), nonLocalScriptExceptions_.end(), syntaxElement) 
           != nonLocalScriptExceptions_.end();
}

// === 自动添加规则支持 ===

bool ContextHelper::needsAutoAddition(const std::string& elementCode) const {
    // 检查是否需要自动添加class或id
    
    // 检查元素是否已有class或id
    bool hasClass = elementCode.find("class:") != std::string::npos;
    bool hasId = elementCode.find("id:") != std::string::npos;
    
    if (hasClass && hasId) {
        return false; // 都有，不需要自动添加
    }
    
    // 检查style块中是否有选择器
    bool hasStyleSelectors = elementCode.find(".") != std::string::npos || 
                             elementCode.find("#") != std::string::npos;
    
    // 检查script块中是否有特定选择器
    bool hasScriptSelectors = elementCode.find("{{.") != std::string::npos || 
                             elementCode.find("{{#") != std::string::npos;
    
    return hasStyleSelectors || hasScriptSelectors;
}

std::string ContextHelper::applyAutoAdditionRules(const std::string& elementCode) const {
    // 应用"缺什么，自动添加什么"规则
    // 严格按照您的要求实现
    
    std::string result = elementCode;
    
    // 检查元素当前状态
    bool hasClass = elementCode.find("class:") != std::string::npos;
    bool hasId = elementCode.find("id:") != std::string::npos;
    
    if (!hasClass && !hasId) {
        // 都没有，根据style和script的使用情况自动添加
        
        // 检查style中的第一个类选择器和ID选择器
        std::regex styleClassRegex(R"(style\s*\{[^}]*\.(\w+))");
        std::regex styleIdRegex(R"(style\s*\{[^}]*#(\w+))");
        
        std::smatch classMatch, idMatch;
        
        bool styleHasClass = std::regex_search(elementCode, classMatch, styleClassRegex);
        bool styleHasId = std::regex_search(elementCode, idMatch, styleIdRegex);
        
        if (styleHasClass) {
            // 添加第一个类选择器作为class属性
            std::string className = classMatch[1].str();
            result = addClassAttribute(result, className);
            hasClass = true;
        }
        
        if (styleHasId) {
            // 添加第一个ID选择器作为id属性
            std::string idName = idMatch[1].str();
            result = addIdAttribute(result, idName);
            hasId = true;
        }
        
        // 如果style没有自动添加，检查script的{{.xxx}}和{{#xxx}}
        if (!hasClass || !hasId) {
            std::regex scriptClassRegex(R"(\{\{\.(\w+)\}\})");
            std::regex scriptIdRegex(R"(\{\{#(\w+)\}\})");
            
            std::smatch scriptClassMatch, scriptIdMatch;
            
            if (!hasClass && std::regex_search(elementCode, scriptClassMatch, scriptClassRegex)) {
                std::string className = scriptClassMatch[1].str();
                result = addClassAttribute(result, className);
                hasClass = true;
            }
            
            if (!hasId && std::regex_search(elementCode, scriptIdMatch, scriptIdRegex)) {
                std::string idName = scriptIdMatch[1].str();
                result = addIdAttribute(result, idName);
                hasId = true;
            }
        }
    }
    
    return result;
}

// === 私有方法 ===

void ContextHelper::initializeSyntaxConstraints() {
    // 初始化语法约束规则
    
    // 全局样式块允许的语法元素
    globalStyleAllowed_ = {
        "模板变量", "自定义变量", "自定义变量特例化",
        "模板样式组", "自定义样式组", "无值样式组", "自定义样式组特例化",
        "delete属性", "delete继承", "样式组间继承",
        "生成器注释", "全缀名", "任意类型原始嵌入", "命名空间from语法"
    };
    
    // 局部样式块允许的语法元素（与全局相同）
    localStyleAllowed_ = globalStyleAllowed_;
    
    // 局部script允许的CHTL语法元素
    localScriptAllowed_ = {
        "模板变量", "自定义变量组", "变量组特例化", "命名空间from",
        "注释", "任意类型原始嵌入", "{{&}}等特供语法"
    };
    
    // 非局部script的例外元素
    nonLocalScriptExceptions_ = {
        "注释", "任意类型原始嵌入"
    };
}

std::string ContextHelper::addClassAttribute(const std::string& elementCode, const std::string& className) const {
    // 在元素中添加class属性
    // 简化实现，实际应该更复杂的解析和插入
    
    size_t bracePos = elementCode.find('{');
    if (bracePos != std::string::npos) {
        std::string before = elementCode.substr(0, bracePos + 1);
        std::string after = elementCode.substr(bracePos + 1);
        return before + "\n    class: " + className + ";" + after;
    }
    
    return elementCode;
}

std::string ContextHelper::addIdAttribute(const std::string& elementCode, const std::string& idName) const {
    // 在元素中添加id属性
    // 简化实现，实际应该更复杂的解析和插入
    
    size_t bracePos = elementCode.find('{');
    if (bracePos != std::string::npos) {
        std::string before = elementCode.substr(0, bracePos + 1);
        std::string after = elementCode.substr(bracePos + 1);
        return before + "\n    id: " + idName + ";" + after;
    }
    
    return elementCode;
}

void RAIIStateGuard::triggerEnterCallbacks() {
    for (const auto& callback : enterCallbacks_) {
        try {
            callback(currentState_, currentContext_);
        } catch (const std::exception& e) {
            setError("进入回调异常: " + std::string(e.what()));
        }
    }
}

void RAIIStateGuard::triggerExitCallbacks() {
    for (const auto& callback : exitCallbacks_) {
        try {
            callback(currentState_, currentContext_);
        } catch (const std::exception& e) {
            std::cerr << "[RAII Guard] 退出回调异常: " << e.what() << std::endl;
        }
    }
}

} // namespace raii
} // namespace chtl