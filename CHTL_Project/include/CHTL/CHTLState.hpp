#pragma once

#include "../Core/CHTLTypes.hpp"
#include <stack>

namespace CHTL {

/**
 * @brief CHTL编译状态枚举
 */
enum class CHTLState {
    // 基础状态
    Initial,             // 初始状态
    Global,              // 全局状态
    
    // 元素状态
    InElement,           // 在元素内部
    InElementAttributes, // 在元素属性内部
    InTextNode,          // 在文本节点内部
    
    // 样式相关状态
    InLocalStyle,        // 在局部样式块内部
    InGlobalStyle,       // 在全局样式块内部
    InStyleRule,         // 在样式规则内部
    InStyleProperties,   // 在样式属性内部
    
    // 脚本相关状态
    InLocalScript,       // 在局部脚本块内部
    InGlobalScript,      // 在全局脚本块内部
    
    // 模板相关状态
    InTemplate,          // 在模板内部
    InTemplateStyle,     // 在模板样式组内部
    InTemplateElement,   // 在模板元素内部
    InTemplateVar,       // 在模板变量组内部
    
    // 自定义相关状态
    InCustom,            // 在自定义内部
    InCustomStyle,       // 在自定义样式组内部
    InCustomElement,     // 在自定义元素内部
    InCustomVar,         // 在自定义变量组内部
    InCustomSpecialization, // 在自定义特例化内部
    
    // 原始嵌入状态
    InOrigin,            // 在原始嵌入内部
    InOriginHtml,        // 在HTML原始嵌入内部
    InOriginStyle,       // 在样式原始嵌入内部
    InOriginScript,      // 在脚本原始嵌入内部
    
    // 导入相关状态
    InImport,            // 在导入语句内部
    
    // 配置相关状态
    InConfiguration,     // 在配置块内部
    InNameConfiguration, // 在名称配置内部
    InOriginTypeConfiguration, // 在原始类型配置内部
    
    // 命名空间相关状态
    InNamespace,         // 在命名空间内部
    InNestedNamespace,   // 在嵌套命名空间内部
    
    // 注释状态
    InLineComment,       // 在行注释内部
    InBlockComment,      // 在块注释内部
    InGeneratorComment,  // 在生成器注释内部
    
    // 字符串状态
    InDoubleQuotedString, // 在双引号字符串内部
    InSingleQuotedString, // 在单引号字符串内部
    InUnquotedLiteral,    // 在无修饰字面量内部
    
    // 错误状态
    Error,               // 错误状态
    
    // 结束状态
    End                  // 结束状态
};

/**
 * @brief CHTL状态上下文信息
 */
struct CHTLStateContext {
    CHTLState state;
    String contextName;        // 上下文名称（如元素名、模板名等）
    String contextType;        // 上下文类型（如@Style、@Element等）
    size_t nestingLevel;       // 嵌套层级
    SourceLocation location;   // 状态开始位置
    StringMap properties;      // 上下文属性
    
    CHTLStateContext(CHTLState s = CHTLState::Initial, 
                     const String& name = "",
                     const String& type = "",
                     size_t level = 0,
                     const SourceLocation& loc = {})
        : state(s), contextName(name), contextType(type), 
          nestingLevel(level), location(loc) {}
};

/**
 * @brief CHTL状态管理器
 * 
 * 使用RAII自动化管理模式的状态机
 * 提供状态转换、上下文管理等功能
 */
class CHTLStateManager {
public:
    CHTLStateManager();
    ~CHTLStateManager() = default;

    // 状态管理
    void PushState(CHTLState newState, const String& contextName = "", 
                   const String& contextType = "", const SourceLocation& location = {});
    bool PopState();
    CHTLState GetCurrentState() const;
    const CHTLStateContext& GetCurrentContext() const;
    
    // 状态查询
    bool IsInState(CHTLState state) const;
    bool IsInAnyState(const std::vector<CHTLState>& states) const;
    size_t GetStateDepth() const;
    size_t GetNestingLevel() const;
    
    // 上下文管理
    void SetContextProperty(const String& key, const String& value);
    Optional<String> GetContextProperty(const String& key) const;
    bool HasContextProperty(const String& key) const;
    
    // 状态历史
    std::vector<CHTLStateContext> GetStateHistory() const;
    CHTLStateContext GetParentContext(size_t levelsUp = 1) const;
    
    // 状态验证
    bool CanTransitionTo(CHTLState newState) const;
    bool IsValidStateTransition(CHTLState from, CHTLState to) const;
    
    // 实用方法
    String GetCurrentContextName() const;
    String GetCurrentContextType() const;
    bool IsInTemplate() const;
    bool IsInCustom() const;
    bool IsInStyle() const;
    bool IsInScript() const;
    bool IsInElement() const;
    bool IsInComment() const;
    bool IsInString() const;
    
    // 重置
    void Reset();
    void Clear();

    // RAII状态管理助手类
    class StateGuard {
    public:
        StateGuard(CHTLStateManager& manager, CHTLState state, 
                   const String& contextName = "", const String& contextType = "",
                   const SourceLocation& location = {});
        ~StateGuard();
        
        // 禁用拷贝
        StateGuard(const StateGuard&) = delete;
        StateGuard& operator=(const StateGuard&) = delete;
        
        // 支持移动
        StateGuard(StateGuard&& other) noexcept;
        StateGuard& operator=(StateGuard&& other) noexcept;
        
    private:
        CHTLStateManager* manager_;
        bool active_;
    };
    
    // 创建状态守护
    StateGuard CreateStateGuard(CHTLState state, const String& contextName = "",
                                const String& contextType = "", const SourceLocation& location = {});

private:
    std::stack<CHTLStateContext> stateStack_;
    
    // 状态转换表
    void InitializeStateTransitions();
    std::map<CHTLState, std::set<CHTLState>> validTransitions_;
};

/**
 * @brief 状态转换到字符串
 */
String StateToString(CHTLState state);

} // namespace CHTL