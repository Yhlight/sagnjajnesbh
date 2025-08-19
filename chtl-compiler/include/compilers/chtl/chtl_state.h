#ifndef CHTL_STATE_H
#define CHTL_STATE_H

#include <string>
#include <stack>
#include <vector>
#include <memory>

namespace chtl {

/**
 * CHTL编译器状态枚举
 */
enum class CHTLCompilerState {
    // 基础状态
    INITIAL,              // 初始状态
    IN_ELEMENT,           // 在元素内部
    IN_TEXT_BLOCK,        // 在text块内部
    IN_STYLE_BLOCK,       // 在style块内部
    IN_SCRIPT_BLOCK,      // 在script块内部
    
    // 模板和自定义状态
    IN_TEMPLATE,          // 在[Template]定义中
    IN_CUSTOM,            // 在[Custom]定义中
    IN_ORIGIN,            // 在[Origin]块中
    IN_IMPORT,            // 在[Import]语句中
    IN_NAMESPACE,         // 在[Namespace]定义中
    IN_CONFIGURATION,     // 在[Configuration]块中
    
    // 属性状态
    IN_ATTRIBUTE,         // 在属性定义中
    IN_ATTRIBUTE_VALUE,   // 在属性值中
    
    // 样式相关状态
    IN_CSS_RULE,          // 在CSS规则中
    IN_CSS_SELECTOR,      // 在CSS选择器中
    IN_CSS_DECLARATION,   // 在CSS声明中
    
    // 字符串状态
    IN_STRING,            // 在字符串中
    IN_COMMENT,           // 在注释中
    
    // 错误状态
    ERROR                 // 错误状态
};

/**
 * 状态上下文信息
 */
struct StateContext {
    CHTLCompilerState state;
    size_t enterLine;      // 进入状态的行号
    size_t enterColumn;    // 进入状态的列号
    std::string info;      // 额外信息（如元素名、模板名等）
    
    StateContext(CHTLCompilerState s, size_t line, size_t col, const std::string& info = "")
        : state(s), enterLine(line), enterColumn(col), info(info) {}
};

/**
 * CHTL编译器状态管理器
 */
class CHTLStateManager {
public:
    CHTLStateManager();
    ~CHTLStateManager();
    
    /**
     * 进入新状态
     */
    void enterState(CHTLCompilerState state, size_t line, size_t column, 
                   const std::string& info = "");
    
    /**
     * 退出当前状态
     */
    void exitState();
    
    /**
     * 获取当前状态
     */
    CHTLCompilerState getCurrentState() const;
    
    /**
     * 获取当前状态上下文
     */
    const StateContext* getCurrentContext() const;
    
    /**
     * 检查是否在特定状态
     */
    bool isInState(CHTLCompilerState state) const;
    
    /**
     * 检查是否在任一状态中
     */
    bool isInAnyState(const std::vector<CHTLCompilerState>& states) const;
    
    /**
     * 获取状态栈深度
     */
    size_t getStateDepth() const { return stateStack_.size(); }
    
    /**
     * 检查状态转换是否合法
     */
    bool canTransitionTo(CHTLCompilerState newState) const;
    
    /**
     * 重置状态管理器
     */
    void reset();
    
    /**
     * 获取状态历史（用于调试）
     */
    std::vector<StateContext> getStateHistory() const { return stateHistory_; }
    
    /**
     * 获取当前元素深度（用于判断局部样式）
     */
    int getElementDepth() const;
    
    /**
     * 检查是否在元素内部
     */
    bool isInsideElement() const;
    
private:
    std::stack<StateContext> stateStack_;
    std::vector<StateContext> stateHistory_;  // 状态历史记录
    
    // 验证状态转换规则
    bool validateTransition(CHTLCompilerState from, CHTLCompilerState to) const;
};

/**
 * 获取状态的字符串表示
 */
std::string stateToString(CHTLCompilerState state);

} // namespace chtl

#endif // CHTL_STATE_H