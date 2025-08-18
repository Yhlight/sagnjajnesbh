#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

namespace chtl {
namespace state {

/**
 * 编译器状态枚举
 * 严格按照CHTL语法文档定义的状态
 */
enum class CompilerState {
    INITIAL,                // 初始状态
    PARSING_ELEMENT,        // 解析元素
    PARSING_ATTRIBUTE,      // 解析属性
    PARSING_TEXT,           // 解析文本块
    PARSING_STYLE_LOCAL,    // 解析局部样式块
    PARSING_STYLE_GLOBAL,   // 解析全局样式块
    PARSING_SCRIPT_LOCAL,   // 解析局部脚本块
    PARSING_SCRIPT_GLOBAL,  // 解析全局脚本块
    PARSING_TEMPLATE,       // 解析模板定义
    PARSING_CUSTOM,         // 解析自定义定义
    PARSING_ORIGIN,         // 解析原始嵌入
    PARSING_CONFIGURATION,  // 解析配置块
    PARSING_IMPORT,         // 解析导入语句
    PARSING_NAMESPACE,      // 解析命名空间
    ERROR_STATE,            // 错误状态
    COMPLETED               // 完成状态
};

/**
 * AST节点状态
 * 用于标记AST节点的处理状态
 */
enum class NodeState {
    CREATED,               // 已创建
    PARSING,               // 解析中
    PARSED,                // 解析完成
    VALIDATING,            // 验证中
    VALIDATED,             // 验证完成
    GENERATING,            // 生成中
    GENERATED,             // 生成完成
    ERROR                  // 错误状态
};

/**
 * 上下文信息
 * 记录当前解析上下文的详细信息
 */
struct ContextInfo {
    CompilerState state;                    // 当前状态
    std::string contextName;                // 上下文名称
    size_t startPosition;                   // 开始位置
    size_t currentPosition;                 // 当前位置
    int nestingLevel;                       // 嵌套层级
    std::string parentContext;              // 父上下文
    std::unordered_map<std::string, std::string> contextData; // 上下文数据
};

/**
 * RAII状态管理器
 * 基于RAII自动化管理模式的状态机
 * 为解析器和生成器提供精确状态支持
 */
class RAIIStateManager {
public:
    /**
     * RAII状态守卫
     * 自动管理状态的生命周期
     */
    class StateGuard {
    public:
        StateGuard(RAIIStateManager& manager, CompilerState newState, const std::string& contextName);
        ~StateGuard();
        
        // 禁止拷贝和移动
        StateGuard(const StateGuard&) = delete;
        StateGuard& operator=(const StateGuard&) = delete;
        StateGuard(StateGuard&&) = delete;
        StateGuard& operator=(StateGuard&&) = delete;
        
        // 状态操作
        void updatePosition(size_t position);
        void setContextData(const std::string& key, const std::string& value);
        std::string getContextData(const std::string& key) const;
        
        // 获取上下文信息
        const ContextInfo& getContext() const;
        
    private:
        RAIIStateManager& manager_;
        CompilerState previousState_;
        std::string contextName_;
        size_t guardId_;
    };
    
    RAIIStateManager();
    ~RAIIStateManager();
    
    // === 状态管理 ===
    
    /**
     * 获取当前状态
     */
    CompilerState getCurrentState() const;
    
    /**
     * 获取当前上下文
     */
    const ContextInfo& getCurrentContext() const;
    
    /**
     * 获取上下文栈
     */
    const std::vector<ContextInfo>& getContextStack() const;
    
    /**
     * 检查是否在特定状态
     */
    bool isInState(CompilerState state) const;
    
    /**
     * 检查状态转换是否合法
     */
    bool isValidStateTransition(CompilerState from, CompilerState to) const;
    
    // === AST节点状态管理 ===
    
    /**
     * 设置AST节点状态
     */
    void setNodeState(void* nodePtr, NodeState state);
    
    /**
     * 获取AST节点状态
     */
    NodeState getNodeState(void* nodePtr) const;
    
    /**
     * 检查AST节点是否在特定状态
     */
    bool isNodeInState(void* nodePtr, NodeState state) const;
    
    // === 状态转换回调 ===
    
    /**
     * 注册状态转换回调
     */
    void registerStateTransitionCallback(CompilerState from, CompilerState to, 
                                        std::function<void(const ContextInfo&)> callback);
    
    /**
     * 注册状态进入回调
     */
    void registerStateEnterCallback(CompilerState state, 
                                   std::function<void(const ContextInfo&)> callback);
    
    /**
     * 注册状态退出回调
     */
    void registerStateExitCallback(CompilerState state, 
                                  std::function<void(const ContextInfo&)> callback);
    
    // === 错误处理 ===
    
    /**
     * 进入错误状态
     */
    void enterErrorState(const std::string& errorMessage);
    
    /**
     * 检查是否在错误状态
     */
    bool isInErrorState() const;
    
    /**
     * 获取错误信息
     */
    std::vector<std::string> getErrors() const;
    
    /**
     * 清除错误
     */
    void clearErrors();
    
    // === 调试和监控 ===
    
    void setDebugMode(bool enabled);
    void dumpState() const;
    void dumpContextStack() const;

private:
    CompilerState currentState_;
    std::vector<ContextInfo> contextStack_;
    std::unordered_map<void*, NodeState> nodeStates_;
    std::vector<std::string> errors_;
    
    // 状态转换回调
    std::unordered_map<std::string, std::function<void(const ContextInfo&)>> stateTransitionCallbacks_;
    std::unordered_map<CompilerState, std::function<void(const ContextInfo&)>> stateEnterCallbacks_;
    std::unordered_map<CompilerState, std::function<void(const ContextInfo&)>> stateExitCallbacks_;
    
    // 配置
    bool debugMode_ = false;
    size_t nextGuardId_ = 1;
    
    // 私有方法
    void enterState(CompilerState newState, const std::string& contextName);
    void exitState();
    void triggerCallbacks(CompilerState from, CompilerState to, const ContextInfo& context);
    std::string stateToString(CompilerState state) const;
    std::string nodeStateToString(NodeState state) const;
};

/**
 * 上下文管理协助器
 * 与状态机协同工作，为解析器和生成器提供精确支持
 */
class ContextHelper {
public:
    ContextHelper(RAIIStateManager& stateManager);
    ~ContextHelper();
    
    // === 解析器支持 ===
    
    /**
     * 检查当前上下文是否允许特定语法
     */
    bool isAllowedInCurrentContext(const std::string& syntaxElement) const;
    
    /**
     * 获取当前上下文允许的语法元素
     */
    std::vector<std::string> getAllowedSyntaxInCurrentContext() const;
    
    /**
     * 验证语法元素在当前上下文的合法性
     */
    bool validateSyntaxInContext(const std::string& syntaxElement, CompilerState context) const;
    
    // === 生成器支持 ===
    
    /**
     * 获取当前上下文的生成策略
     */
    std::string getGenerationStrategy() const;
    
    /**
     * 检查是否需要特殊处理
     */
    bool needsSpecialHandling() const;
    
    /**
     * 获取上下文特定的生成选项
     */
    std::unordered_map<std::string, std::string> getContextGenerationOptions() const;
    
    // === 语法约束支持 ===
    
    /**
     * 检查全局样式块语法约束
     */
    bool isValidInGlobalStyle(const std::string& syntaxElement) const;
    
    /**
     * 检查局部样式块语法约束
     */
    bool isValidInLocalStyle(const std::string& syntaxElement) const;
    
    /**
     * 检查局部script语法约束
     */
    bool isValidInLocalScript(const std::string& syntaxElement) const;
    
    /**
     * 检查非局部script语法约束
     */
    bool isValidInNonLocalScript(const std::string& syntaxElement) const;

private:
    RAIIStateManager& stateManager_;
    
    // 语法约束规则
    std::unordered_map<CompilerState, std::vector<std::string>> allowedSyntax_;
    std::unordered_map<CompilerState, std::vector<std::string>> forbiddenSyntax_;
    
    void initializeSyntaxConstraints();
};

} // namespace state
} // namespace chtl