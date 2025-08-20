#ifndef CHTL_COMMON_STATE_H
#define CHTL_COMMON_STATE_H

#include <string>
#include <memory>
#include <stack>
#include <functional>
#include <unordered_map>

namespace chtl {

// 前向声明
class StateContext;

// 状态类型
enum class StateType {
    // 基础状态
    INITIAL,
    GLOBAL,
    
    // CHTL状态
    CHTL_ELEMENT,           // 元素内部
    CHTL_ATTRIBUTE,         // 属性定义
    CHTL_TEXT,              // text块
    CHTL_STYLE,             // 局部style块
    CHTL_SCRIPT,            // 局部script块
    CHTL_COMMENT,           // 注释
    
    // 模板和自定义状态
    TEMPLATE_DEFINITION,    // 模板定义
    CUSTOM_DEFINITION,      // 自定义定义
    ORIGIN_BLOCK,           // 原始嵌入块
    
    // 配置和导入状态
    CONFIGURATION_BLOCK,    // 配置块
    IMPORT_STATEMENT,       // 导入语句
    NAMESPACE_BLOCK,        // 命名空间块
    
    // CSS相关状态
    CSS_RULE,               // CSS规则
    CSS_SELECTOR,           // CSS选择器
    CSS_DECLARATION,        // CSS声明
    CSS_VALUE,              // CSS值
    
    // JavaScript相关状态
    JS_CODE,                // JavaScript代码
    JS_STRING,              // JavaScript字符串
    JS_COMMENT,             // JavaScript注释
    
    // CHTL JS特殊状态
    CHTLJS_SELECTOR,        // {{选择器}}
    CHTLJS_VIR,             // vir对象
    CHTLJS_FUNCTION_CALL,   // CHTL JS函数调用
    
    // 字符串状态
    STRING_SINGLE,          // 单引号字符串
    STRING_DOUBLE,          // 双引号字符串
    STRING_UNQUOTED,        // 无引号字符串
    
    // 错误状态
    ERROR
};

// 状态转换条件
using StateCondition = std::function<bool(const StateContext&)>;

// 状态转换动作
using StateAction = std::function<void(StateContext&)>;

// 状态转换
struct StateTransition {
    StateType fromState;
    StateType toState;
    StateCondition condition;
    StateAction enterAction;
    StateAction exitAction;
    
    StateTransition(StateType from, StateType to,
                   StateCondition cond = nullptr,
                   StateAction enter = nullptr,
                   StateAction exit = nullptr)
        : fromState(from), toState(to),
          condition(cond), enterAction(enter), exitAction(exit) {}
};

// 状态上下文
class StateContext {
private:
    std::stack<StateType> stateStack_;
    std::unordered_map<std::string, std::string> attributes_;
    std::string currentToken_;
    size_t position_ = 0;
    
public:
    StateContext() {
        stateStack_.push(StateType::INITIAL);
    }
    
    // 状态管理
    StateType getCurrentState() const {
        return stateStack_.empty() ? StateType::INITIAL : stateStack_.top();
    }
    
    void pushState(StateType state) {
        stateStack_.push(state);
    }
    
    void popState() {
        if (!stateStack_.empty()) {
            stateStack_.pop();
        }
    }
    
    void replaceState(StateType state) {
        if (!stateStack_.empty()) {
            stateStack_.pop();
        }
        stateStack_.push(state);
    }
    
    // 属性管理
    void setAttribute(const std::string& key, const std::string& value) {
        attributes_[key] = value;
    }
    
    std::string getAttribute(const std::string& key) const {
        auto it = attributes_.find(key);
        return it != attributes_.end() ? it->second : "";
    }
    
    void clearAttributes() {
        attributes_.clear();
    }
    
    // Token管理
    void setCurrentToken(const std::string& token) {
        currentToken_ = token;
    }
    
    const std::string& getCurrentToken() const {
        return currentToken_;
    }
    
    // 位置管理
    void setPosition(size_t pos) { position_ = pos; }
    size_t getPosition() const { return position_; }
    
    // 状态检查
    bool isInState(StateType state) const {
        return getCurrentState() == state;
    }
    
    bool hasState(StateType state) const {
        std::stack<StateType> temp = stateStack_;
        while (!temp.empty()) {
            if (temp.top() == state) {
                return true;
            }
            temp.pop();
        }
        return false;
    }
    
    // 获取状态深度
    size_t getStateDepth() const {
        return stateStack_.size();
    }
};

// 状态机
class StateMachine {
private:
    std::vector<StateTransition> transitions_;
    std::unique_ptr<StateContext> context_;
    std::unordered_map<StateType, std::string> stateNames_;
    
    // 初始化状态名称
    void initializeStateNames();
    
public:
    StateMachine();
    
    // 添加转换
    void addTransition(const StateTransition& transition);
    
    // 添加简单转换
    void addTransition(StateType from, StateType to,
                      StateCondition condition = nullptr);
    
    // 执行转换
    bool transition(StateType targetState);
    
    // 自动转换（根据条件）
    bool autoTransition();
    
    // 获取上下文
    StateContext& getContext() { return *context_; }
    const StateContext& getContext() const { return *context_; }
    
    // 获取当前状态
    StateType getCurrentState() const {
        return context_->getCurrentState();
    }
    
    // 获取状态名称
    std::string getStateName(StateType state) const;
    
    // 重置状态机
    void reset();
    
    // 验证状态转换是否有效
    bool canTransition(StateType from, StateType to) const;
};

// RAII状态保护器
class StateGuard {
private:
    StateContext& context_;
    StateType savedState_;
    bool shouldRestore_;
    
public:
    StateGuard(StateContext& ctx, StateType newState, bool pushState = true)
        : context_(ctx), savedState_(ctx.getCurrentState()), shouldRestore_(true) {
        if (pushState) {
            context_.pushState(newState);
        } else {
            context_.replaceState(newState);
        }
    }
    
    ~StateGuard() {
        if (shouldRestore_) {
            context_.popState();
        }
    }
    
    // 禁用恢复
    void disableRestore() { shouldRestore_ = false; }
    
    // 禁止拷贝
    StateGuard(const StateGuard&) = delete;
    StateGuard& operator=(const StateGuard&) = delete;
};

// 状态助手函数
inline bool isInCHTLContext(const StateContext& ctx) {
    StateType state = ctx.getCurrentState();
    return state == StateType::CHTL_ELEMENT ||
           state == StateType::CHTL_ATTRIBUTE ||
           state == StateType::CHTL_TEXT ||
           state == StateType::CHTL_STYLE ||
           state == StateType::CHTL_SCRIPT;
}

inline bool isInCSSContext(const StateContext& ctx) {
    StateType state = ctx.getCurrentState();
    return state == StateType::CSS_RULE ||
           state == StateType::CSS_SELECTOR ||
           state == StateType::CSS_DECLARATION ||
           state == StateType::CSS_VALUE ||
           state == StateType::CHTL_STYLE;
}

inline bool isInJSContext(const StateContext& ctx) {
    StateType state = ctx.getCurrentState();
    return state == StateType::JS_CODE ||
           state == StateType::JS_STRING ||
           state == StateType::JS_COMMENT ||
           state == StateType::CHTL_SCRIPT ||
           state == StateType::CHTLJS_SELECTOR ||
           state == StateType::CHTLJS_VIR ||
           state == StateType::CHTLJS_FUNCTION_CALL;
}

inline bool isInStringContext(const StateContext& ctx) {
    StateType state = ctx.getCurrentState();
    return state == StateType::STRING_SINGLE ||
           state == StateType::STRING_DOUBLE ||
           state == StateType::STRING_UNQUOTED;
}

} // namespace chtl

#endif // CHTL_COMMON_STATE_H