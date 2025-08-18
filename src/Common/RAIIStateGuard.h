#pragma once
#include "../CHTLCompiler/State/CHTLState.h"
#include <memory>
#include <functional>
#include <vector>

namespace chtl {
namespace raii {

/**
 * RAII状态守卫
 * 基于RAII自动化管理模式的状态机守卫
 * 用于标记AST节点状态，为解析器和生成器提供精确状态支持
 */
class RAIIStateGuard {
public:
    /**
     * 构造函数 - 进入新状态
     */
    RAIIStateGuard(StateManager& stateManager, CompilerState newState, SyntaxContext context, 
                   const std::string& description = "");
    
    /**
     * 析构函数 - 自动退出状态
     */
    ~RAIIStateGuard();
    
    // 禁止拷贝和移动，确保RAII语义
    RAIIStateGuard(const RAIIStateGuard&) = delete;
    RAIIStateGuard& operator=(const RAIIStateGuard&) = delete;
    RAIIStateGuard(RAIIStateGuard&&) = delete;
    RAIIStateGuard& operator=(RAIIStateGuard&&) = delete;
    
    // === AST节点状态管理 ===
    
    /**
     * 标记AST节点状态
     */
    void markNodeState(void* nodePtr, const std::string& state);
    
    /**
     * 获取AST节点状态
     */
    std::string getNodeState(void* nodePtr) const;
    
    /**
     * 设置节点数据
     */
    void setNodeData(void* nodePtr, const std::string& key, const std::string& value);
    
    /**
     * 获取节点数据
     */
    std::string getNodeData(void* nodePtr, const std::string& key) const;
    
    // === 解析器支持 ===
    
    /**
     * 为解析器提供精确状态支持
     */
    bool isValidParseState() const;
    
    /**
     * 获取解析器状态信息
     */
    std::string getParserStateInfo() const;
    
    /**
     * 检查解析器上下文
     */
    bool isInParserContext(SyntaxContext context) const;
    
    // === 生成器支持 ===
    
    /**
     * 为生成器提供精确状态支持
     */
    bool isValidGenerateState() const;
    
    /**
     * 获取生成器状态信息
     */
    std::string getGeneratorStateInfo() const;
    
    /**
     * 检查生成器上下文
     */
    bool isInGeneratorContext(SyntaxContext context) const;
    
    // === 状态监控和回调 ===
    
    /**
     * 注册状态进入回调
     */
    void onStateEnter(std::function<void(CompilerState, SyntaxContext)> callback);
    
    /**
     * 注册状态退出回调
     */
    void onStateExit(std::function<void(CompilerState, SyntaxContext)> callback);
    
    /**
     * 获取当前状态信息
     */
    CompilerState getCurrentState() const;
    SyntaxContext getCurrentContext() const;
    std::string getDescription() const;
    
    // === 错误处理 ===
    
    /**
     * 设置错误状态
     */
    void setError(const std::string& errorMessage);
    
    /**
     * 检查是否有错误
     */
    bool hasError() const;
    
    /**
     * 获取错误信息
     */
    std::string getErrorMessage() const;

private:
    StateManager& stateManager_;
    CompilerState previousState_;
    SyntaxContext previousContext_;
    CompilerState currentState_;
    SyntaxContext currentContext_;
    std::string description_;
    
    // AST节点状态映射
    std::unordered_map<void*, std::string> nodeStates_;
    std::unordered_map<void*, std::unordered_map<std::string, std::string>> nodeData_;
    
    // 回调函数
    std::vector<std::function<void(CompilerState, SyntaxContext)>> enterCallbacks_;
    std::vector<std::function<void(CompilerState, SyntaxContext)>> exitCallbacks_;
    
    // 错误处理
    bool hasError_ = false;
    std::string errorMessage_;
    
    // 私有方法
    void triggerEnterCallbacks();
    void triggerExitCallbacks();
};

/**
 * 上下文管理协助器
 * 与扫描器协同工作，为解析器和生成器提供精确支持
 */
class ContextHelper {
public:
    ContextHelper(StateManager& stateManager);
    ~ContextHelper();
    
    // === 与扫描器协同工作 ===
    
    /**
     * 根据扫描器结果更新上下文
     */
    void updateFromScannerResult(const scanner::CHTLUnifiedScanner::ScanResult& scanResult);
    
    /**
     * 为扫描器提供上下文信息
     */
    SyntaxContext getScannerContext(scanner::CHTLUnifiedScanner::CodeContext scannerContext) const;
    
    /**
     * 验证扫描器上下文与编译器状态的一致性
     */
    bool validateScannerContextConsistency(scanner::CHTLUnifiedScanner::CodeContext scannerContext) const;
    
    // === 语法约束支持 ===
    
    /**
     * 检查当前上下文允许的语法元素
     */
    bool isAllowedInCurrentContext(const std::string& syntaxElement) const;
    
    /**
     * 获取全局样式块允许的语法元素
     */
    std::vector<std::string> getGlobalStyleAllowedSyntax() const;
    
    /**
     * 获取局部样式块允许的语法元素
     */
    std::vector<std::string> getLocalStyleAllowedSyntax() const;
    
    /**
     * 获取局部script允许的语法元素
     */
    std::vector<std::string> getLocalScriptAllowedSyntax() const;
    
    /**
     * 检查非局部script的语法约束
     */
    bool isValidInNonLocalScript(const std::string& syntaxElement) const;
    
    // === 自动添加规则支持 ===
    
    /**
     * 检查是否需要自动添加class或id
     */
    bool needsAutoAddition(const std::string& elementCode) const;
    
    /**
     * 应用"缺什么，自动添加什么"规则
     */
    std::string applyAutoAdditionRules(const std::string& elementCode) const;

private:
    StateManager& stateManager_;
    
    // 语法约束规则
    std::vector<std::string> globalStyleAllowed_;
    std::vector<std::string> localStyleAllowed_;
    std::vector<std::string> localScriptAllowed_;
    std::vector<std::string> nonLocalScriptExceptions_;
    
    void initializeSyntaxConstraints();
};

} // namespace raii
} // namespace chtl