#include "CHTLJS/Core/CHTLJSStateContext.h"
#include "Utils/ErrorHandler.h"
#include <algorithm>
#include <sstream>
#include <regex>

namespace CHTL {
namespace CHTLJS {
namespace Core {

// CHTLJSStateContext实现
CHTLJSStateContext::CHTLJSStateContext(const std::string& sourceFile, const std::vector<CHTLJSToken>& tokens)
    : sourceFile_(sourceFile), tokens_(tokens), currentPosition_(0) {
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "CHTL JS状态上下文初始化完成，源文件: " + sourceFile_ + 
        "，Token数量: " + std::to_string(tokens_.size())
    );
}

const CHTLJSToken& CHTLJSStateContext::GetCurrentToken() const {
    if (IsAtEnd()) {
        static CHTLJSToken eofToken(TokenType::END_OF_FILE, "", TokenPosition{0, 0});
        return eofToken;
    }
    return tokens_[currentPosition_];
}

const CHTLJSToken* CHTLJSStateContext::LookAhead(int offset) const {
    size_t targetPos = currentPosition_ + offset;
    if (targetPos >= tokens_.size()) {
        return nullptr;
    }
    return &tokens_[targetPos];
}

const CHTLJSToken* CHTLJSStateContext::LookBehind(int offset) const {
    if (currentPosition_ < static_cast<size_t>(offset)) {
        return nullptr;
    }
    size_t targetPos = currentPosition_ - offset;
    return &tokens_[targetPos];
}

std::vector<CHTLJSToken> CHTLJSStateContext::GetSurroundingContext(int radius) const {
    std::vector<CHTLJSToken> surrounding;
    
    // 向后看
    for (int i = radius; i > 0; --i) {
        const CHTLJSToken* token = LookBehind(i);
        if (token) {
            surrounding.push_back(*token);
        }
    }
    
    // 当前Token
    if (!IsAtEnd()) {
        surrounding.push_back(GetCurrentToken());
    }
    
    // 向前看
    for (int i = 1; i <= radius; ++i) {
        const CHTLJSToken* token = LookAhead(i);
        if (token) {
            surrounding.push_back(*token);
        }
    }
    
    return surrounding;
}

std::string CHTLJSStateContext::InferFromContext(CHTLJSContextInferenceType inferenceType, 
                                                 CHTLJSContextLookDirection direction, 
                                                 int depth) const {
    switch (inferenceType) {
        case CHTLJSContextInferenceType::SELECTOR_TYPE_INFERENCE:
            return InferSelectorType(direction, depth);
        case CHTLJSContextInferenceType::EVENT_TYPE_INFERENCE:
            return InferEventType(direction, depth);
        case CHTLJSContextInferenceType::ANIMATION_PROPERTY_INFERENCE:
            return InferAnimationProperty(direction, depth);
        case CHTLJSContextInferenceType::VIRTUAL_OBJECT_INFERENCE:
            return InferVirtualObject(direction, depth);
        case CHTLJSContextInferenceType::SYNTAX_COMPLETION:
            return InferCHTLJSSyntaxCompletion(direction, depth);
        default:
            return "";
    }
}

bool CHTLJSStateContext::CheckCHTLJSSyntaxCompleteness(const std::string& syntaxPattern) const {
    // 检查CHTL JS语法模式是否完整
    std::vector<CHTLJSToken> context = GetSurroundingContext(5);
    std::string contextStr;
    
    for (const auto& token : context) {
        contextStr += token.GetValue() + " ";
    }
    
    try {
        std::regex pattern(syntaxPattern);
        return std::regex_search(contextStr, pattern);
    } catch (const std::regex_error&) {
        return false;
    }
}

CHTLJSCompileState CHTLJSStateContext::DisambiguateCHTLJSState(const std::vector<CHTLJSCompileState>& possibleStates) const {
    return DisambiguateByContext(possibleStates);
}

void CHTLJSStateContext::Advance() {
    if (!IsAtEnd()) {
        currentPosition_++;
    }
}

void CHTLJSStateContext::SetPosition(size_t position) {
    currentPosition_ = std::min(position, tokens_.size());
}

bool CHTLJSStateContext::IsAtEnd() const {
    return currentPosition_ >= tokens_.size();
}

// CHTL JS推断辅助方法实现
std::string CHTLJSStateContext::InferSelectorType(CHTLJSContextLookDirection direction, int depth) const {
    // 推断增强选择器类型 - 语法文档第1130-1158行
    std::vector<CHTLJSToken> contextTokens = GetSurroundingContext(depth);
    
    for (const auto& token : contextTokens) {
        if (token.GetType() == TokenType::SELECTOR_START) {  // {{
            // 查找选择器内容
            const CHTLJSToken* nextToken = LookAhead(1);
            if (nextToken && nextToken->GetType() == TokenType::SELECTOR_CONTENT) {
                std::string content = nextToken->GetValue();
                
                // 根据语法文档第1147-1158行推断选择器类型
                if (!content.empty() && content[0] == '.') {
                    return "class_selector";  // {{.box}}
                } else if (!content.empty() && content[0] == '#') {
                    return "id_selector";     // {{#box}}
                } else if (content.find("[") != std::string::npos) {
                    return "indexed_selector"; // {{button[0]}}
                } else if (content.find(" ") != std::string::npos) {
                    return "complex_selector"; // {{.box button}}
                } else {
                    return "tag_selector";     // {{button}}
                }
            }
        }
    }
    
    return "";
}

std::string CHTLJSStateContext::InferEventType(CHTLJSContextLookDirection direction, int depth) const {
    // 推断事件类型 - 语法文档第1184-1228行
    std::vector<CHTLJSToken> contextTokens = GetSurroundingContext(depth);
    
    for (const auto& token : contextTokens) {
        if (token.GetType() == TokenType::LISTEN) {
            return "listen_event";  // listen({...})
        } else if (token.GetType() == TokenType::DELEGATE) {
            return "delegate_event"; // delegate({...})
        } else if (token.GetValue() == "click" || token.GetValue() == "mouseenter" ||
                   token.GetValue() == "mouseleave" || token.GetValue() == "mousemove") {
            return "mouse_event";
        } else if (token.GetValue() == "keydown" || token.GetValue() == "keyup") {
            return "keyboard_event";
        }
    }
    
    return "";
}

std::string CHTLJSStateContext::InferAnimationProperty(CHTLJSContextLookDirection direction, int depth) const {
    // 推断动画属性 - 语法文档第1233-1271行
    std::vector<CHTLJSToken> contextTokens = GetSurroundingContext(depth);
    
    for (const auto& token : contextTokens) {
        if (token.GetType() == TokenType::ANIMATE) {
            return "animate_block";
        } else if (token.GetValue() == "duration" || token.GetValue() == "easing" ||
                   token.GetValue() == "begin" || token.GetValue() == "end" ||
                   token.GetValue() == "when" || token.GetValue() == "loop" ||
                   token.GetValue() == "direction" || token.GetValue() == "delay" ||
                   token.GetValue() == "callback") {
            return "animation_property";
        } else if (token.GetType() == TokenType::AT) {
            const CHTLJSToken* nextToken = LookAhead(1);
            if (nextToken && nextToken->GetValue() == "0.4") {
                return "keyframe_time"; // at: 0.4
            }
        }
    }
    
    return "";
}

std::string CHTLJSStateContext::InferVirtualObject(CHTLJSContextLookDirection direction, int depth) const {
    // 推断虚对象 - 语法文档第1274-1311行
    std::vector<CHTLJSToken> contextTokens = GetSurroundingContext(depth);
    
    for (const auto& token : contextTokens) {
        if (token.GetType() == TokenType::VIR) {
            return "virtual_object_declaration"; // vir
        } else if (token.GetType() == TokenType::ARROW) {
            // 可能是虚对象调用 Test->click()
            const CHTLJSToken* prevToken = LookBehind(1);
            if (prevToken && prevToken->GetType() == TokenType::IDENTIFIER) {
                return "virtual_object_call";
            }
        }
    }
    
    return "";
}

std::string CHTLJSStateContext::InferCHTLJSSyntaxCompletion(CHTLJSContextLookDirection direction, int depth) const {
    // 推断CHTL JS语法补全
    std::vector<CHTLJSToken> contextTokens;
    
    if (direction == CHTLJSContextLookDirection::FORWARD) {
        for (int i = 1; i <= depth; ++i) {
            const CHTLJSToken* token = LookAhead(i);
            if (token) contextTokens.push_back(*token);
        }
    } else if (direction == CHTLJSContextLookDirection::BACKWARD) {
        for (int i = 1; i <= depth; ++i) {
            const CHTLJSToken* token = LookBehind(i);
            if (token) contextTokens.push_back(*token);
        }
    }
    
    // 基于语法文档进行CHTL JS语法补全
    for (const auto& token : contextTokens) {
        if (token.GetType() == TokenType::SELECTOR_START) {
            return "{{selector}}";  // 增强选择器
        } else if (token.GetType() == TokenType::VIR) {
            return "vir object = ";  // 虚对象
        } else if (token.GetType() == TokenType::LISTEN) {
            return "listen({...})";  // 监听器
        } else if (token.GetType() == TokenType::DELEGATE) {
            return "delegate({...})"; // 事件委托
        } else if (token.GetType() == TokenType::ANIMATE) {
            return "animate({...})";  // 动画
        } else if (token.GetType() == TokenType::ARROW) {
            return "->method()";      // 箭头操作符
        }
    }
    
    return "";
}

CHTLJSCompileState CHTLJSStateContext::DisambiguateByContext(const std::vector<CHTLJSCompileState>& states) const {
    if (states.empty()) {
        return CHTLJSCompileState::ERROR_STATE;
    }
    
    if (states.size() == 1) {
        return states[0];
    }
    
    // 基于CHTL JS上下文消除歧义
    std::vector<CHTLJSToken> context = GetSurroundingContext(3);
    
    for (const auto& token : context) {
        if (token.GetType() == TokenType::SELECTOR_START) {
            // 在增强选择器上下文中
            for (auto state : states) {
                if (state == CHTLJSCompileState::PARSING_ENHANCED_SELECTOR ||
                    state == CHTLJSCompileState::PARSING_SELECTOR_CONTENT) {
                    return state;
                }
            }
        } else if (token.GetType() == TokenType::VIR) {
            // 在虚对象上下文中
            for (auto state : states) {
                if (state == CHTLJSCompileState::PARSING_VIRTUAL_OBJECT ||
                    state == CHTLJSCompileState::PARSING_VIRTUAL_ASSIGNMENT) {
                    return state;
                }
            }
        } else if (token.GetType() == TokenType::LISTEN) {
            // 在监听器上下文中
            for (auto state : states) {
                if (state == CHTLJSCompileState::PARSING_LISTEN_BLOCK ||
                    state == CHTLJSCompileState::PARSING_EVENT_HANDLER) {
                    return state;
                }
            }
        } else if (token.GetType() == TokenType::ANIMATE) {
            // 在动画上下文中
            for (auto state : states) {
                if (state == CHTLJSCompileState::PARSING_ANIMATE_BLOCK ||
                    state == CHTLJSCompileState::PARSING_ANIMATION_CONFIG) {
                    return state;
                }
            }
        }
    }
    
    // 默认返回第一个状态
    return states[0];
}

// CHTLJSStateMachine实现
CHTLJSStateMachine::CHTLJSStateMachine(std::shared_ptr<CHTLJSStateContext> context)
    : context_(context), currentState_(CHTLJSCompileState::INITIAL) {
    
    InitializeCHTLJSTransitionRules();
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "CHTL JS RAII状态机初始化完成，源文件: " + context_->GetSourceFile()
    );
}

CHTLJSStateMachine::~CHTLJSStateMachine() {
    // RAII自动清理
    if (currentState_ != CHTLJSCompileState::INITIAL && 
        currentState_ != CHTLJSCompileState::COMPLETED_STATE &&
        currentState_ != CHTLJSCompileState::ERROR_STATE) {
        
        Utils::ErrorHandler::GetInstance().LogWarning(
            "CHTL JS状态机析构时状态未正确结束: " + GetStateName(currentState_)
        );
    }
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "CHTL JS RAII状态机清理完成，处理了 " + std::to_string(stateHistory_.size()) + " 个状态转换"
    );
}

bool CHTLJSStateMachine::TransitionTo(CHTLJSCompileState newState, const std::string& reason) {
    if (!IsValidTransition(currentState_, newState)) {
        Utils::ErrorHandler::GetInstance().LogError(
            "无效的CHTL JS状态转换: " + GetStateName(currentState_) + " -> " + GetStateName(newState) +
            (reason.empty() ? "" : " (原因: " + reason + ")")
        );
        return false;
    }
    
    // 记录状态历史
    stateHistory_.push_back(currentState_);
    
    CHTLJSCompileState previousState = currentState_;
    currentState_ = newState;
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "CHTL JS状态转换: " + GetStateName(previousState) + " -> " + GetStateName(newState) +
        (reason.empty() ? "" : " (原因: " + reason + ")")
    );
    
    return true;
}

bool CHTLJSStateMachine::SmartTransition(const CHTLJSToken& triggerToken) {
    // 基于CHTL JS Context推断进行智能状态转换
    
    std::vector<CHTLJSStateTransitionRule> applicableRules = FindApplicableCHTLJSRules(triggerToken);
    
    if (applicableRules.empty()) {
        Utils::ErrorHandler::GetInstance().LogWarning(
            "没有找到适用的CHTL JS状态转换规则，Token: " + triggerToken.GetValue()
        );
        return false;
    }
    
    CHTLJSStateTransitionRule selectedRule;
    if (applicableRules.size() > 1) {
        selectedRule = SelectBestCHTLJSRule(applicableRules);
    } else {
        selectedRule = applicableRules[0];
    }
    
    return TransitionTo(selectedRule.toState, selectedRule.condition.description);
}

std::string CHTLJSStateMachine::GetStateName(CHTLJSCompileState state) const {
    switch (state) {
        case CHTLJSCompileState::INITIAL: return "INITIAL";
        case CHTLJSCompileState::PARSING_ENHANCED_SELECTOR: return "PARSING_ENHANCED_SELECTOR";
        case CHTLJSCompileState::PARSING_SELECTOR_CONTENT: return "PARSING_SELECTOR_CONTENT";
        case CHTLJSCompileState::PARSING_VIRTUAL_OBJECT: return "PARSING_VIRTUAL_OBJECT";
        case CHTLJSCompileState::PARSING_LISTEN_BLOCK: return "PARSING_LISTEN_BLOCK";
        case CHTLJSCompileState::PARSING_DELEGATE_BLOCK: return "PARSING_DELEGATE_BLOCK";
        case CHTLJSCompileState::PARSING_ANIMATE_BLOCK: return "PARSING_ANIMATE_BLOCK";
        case CHTLJSCompileState::PARSING_ARROW_FUNCTION: return "PARSING_ARROW_FUNCTION";
        case CHTLJSCompileState::PARSING_ARROW_OPERATION: return "PARSING_ARROW_OPERATION";
        case CHTLJSCompileState::PARSING_EVENT_HANDLER: return "PARSING_EVENT_HANDLER";
        case CHTLJSCompileState::PARSING_EVENT_MAPPING: return "PARSING_EVENT_MAPPING";
        case CHTLJSCompileState::PARSING_EVENT_DELEGATION: return "PARSING_EVENT_DELEGATION";
        case CHTLJSCompileState::PARSING_ANIMATION_CONFIG: return "PARSING_ANIMATION_CONFIG";
        case CHTLJSCompileState::PARSING_ANIMATION_KEYFRAME: return "PARSING_ANIMATION_KEYFRAME";
        case CHTLJSCompileState::PARSING_ANIMATION_CSS: return "PARSING_ANIMATION_CSS";
        case CHTLJSCompileState::PARSING_VIRTUAL_ASSIGNMENT: return "PARSING_VIRTUAL_ASSIGNMENT";
        case CHTLJSCompileState::PARSING_VIRTUAL_CALL: return "PARSING_VIRTUAL_CALL";
        case CHTLJSCompileState::ERROR_STATE: return "ERROR_STATE";
        case CHTLJSCompileState::COMPLETED_STATE: return "COMPLETED_STATE";
        default: return "UNKNOWN_STATE";
    }
}

void CHTLJSStateMachine::InitializeCHTLJSTransitionRules() {
    transitionRules_.clear();
    
    InitializeEnhancedSelectorTransitions();
    InitializeVirtualObjectTransitions();
    InitializeListenBlockTransitions();
    InitializeDelegateBlockTransitions();
    InitializeAnimateBlockTransitions();
    InitializeArrowFunctionTransitions();
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "CHTL JS状态转换规则初始化完成，共 " + std::to_string(transitionRules_.size()) + " 条规则"
    );
}

void CHTLJSStateMachine::InitializeEnhancedSelectorTransitions() {
    // 增强选择器转换 - 语法文档第1130-1158行
    
    transitionRules_.push_back({
        CHTLJSCompileState::INITIAL,
        CHTLJSCompileState::PARSING_ENHANCED_SELECTOR,
        {TokenType::SELECTOR_START, nullptr, "增强选择器开始", false, false},
        1
    });
    
    transitionRules_.push_back({
        CHTLJSCompileState::PARSING_ENHANCED_SELECTOR,
        CHTLJSCompileState::PARSING_SELECTOR_CONTENT,
        {TokenType::SELECTOR_CONTENT, nullptr, "选择器内容", false, false},
        1
    });
    
    transitionRules_.push_back({
        CHTLJSCompileState::PARSING_SELECTOR_CONTENT,
        CHTLJSCompileState::INITIAL,
        {TokenType::SELECTOR_END, nullptr, "增强选择器结束", false, false},
        1
    });
}

void CHTLJSStateMachine::InitializeVirtualObjectTransitions() {
    // 虚对象转换 - 语法文档第1274-1311行
    
    transitionRules_.push_back({
        CHTLJSCompileState::INITIAL,
        CHTLJSCompileState::PARSING_VIRTUAL_OBJECT,
        {TokenType::VIR, nullptr, "虚对象声明", false, false},
        1
    });
    
    transitionRules_.push_back({
        CHTLJSCompileState::PARSING_VIRTUAL_OBJECT,
        CHTLJSCompileState::PARSING_VIRTUAL_ASSIGNMENT,
        {TokenType::EQUAL, nullptr, "虚对象赋值", false, false},
        1
    });
}

void CHTLJSStateMachine::InitializeListenBlockTransitions() {
    // 监听器转换 - 语法文档第1184-1213行
    
    transitionRules_.push_back({
        CHTLJSCompileState::INITIAL,
        CHTLJSCompileState::PARSING_LISTEN_BLOCK,
        {TokenType::LISTEN, nullptr, "监听器块", false, false},
        1
    });
    
    transitionRules_.push_back({
        CHTLJSCompileState::PARSING_LISTEN_BLOCK,
        CHTLJSCompileState::PARSING_EVENT_MAPPING,
        {TokenType::LEFT_BRACE, nullptr, "事件映射", false, false},
        1
    });
}

void CHTLJSStateMachine::InitializeDelegateBlockTransitions() {
    // 事件委托转换 - 语法文档第1215-1232行
    
    transitionRules_.push_back({
        CHTLJSCompileState::INITIAL,
        CHTLJSCompileState::PARSING_DELEGATE_BLOCK,
        {TokenType::DELEGATE, nullptr, "事件委托块", false, false},
        1
    });
    
    transitionRules_.push_back({
        CHTLJSCompileState::PARSING_DELEGATE_BLOCK,
        CHTLJSCompileState::PARSING_EVENT_DELEGATION,
        {TokenType::LEFT_BRACE, nullptr, "事件委托配置", false, false},
        1
    });
}

void CHTLJSStateMachine::InitializeAnimateBlockTransitions() {
    // 动画转换 - 语法文档第1233-1271行
    
    transitionRules_.push_back({
        CHTLJSCompileState::INITIAL,
        CHTLJSCompileState::PARSING_ANIMATE_BLOCK,
        {TokenType::ANIMATE, nullptr, "动画块", false, false},
        1
    });
    
    transitionRules_.push_back({
        CHTLJSCompileState::PARSING_ANIMATE_BLOCK,
        CHTLJSCompileState::PARSING_ANIMATION_CONFIG,
        {TokenType::LEFT_BRACE, nullptr, "动画配置", false, false},
        1
    });
}

void CHTLJSStateMachine::InitializeArrowFunctionTransitions() {
    // 箭头函数转换 - 语法文档第1202行支持
    
    transitionRules_.push_back({
        CHTLJSCompileState::PARSING_EVENT_MAPPING,
        CHTLJSCompileState::PARSING_ARROW_FUNCTION,
        {TokenType::ARROW, nullptr, "箭头函数", false, false},
        1
    });
}

bool CHTLJSStateMachine::IsValidTransition(CHTLJSCompileState from, CHTLJSCompileState to) const {
    // 检查CHTL JS转换规则表
    for (const auto& rule : transitionRules_) {
        if (rule.fromState == from && rule.toState == to) {
            return CheckCHTLJSTransitionCondition(rule.condition);
        }
    }
    
    // 特殊情况：任何状态都可以转换到错误状态
    if (to == CHTLJSCompileState::ERROR_STATE) {
        return true;
    }
    
    return false;
}

bool CHTLJSStateMachine::CheckCHTLJSTransitionCondition(const CHTLJSStateTransitionCondition& condition) const {
    // 检查CHTL JS Token类型
    if (context_->GetCurrentToken().GetType() != condition.triggerToken) {
        return false;
    }
    
    // 检查CHTL JS上下文条件
    if (condition.contextCheck && !condition.contextCheck(*context_)) {
        return false;
    }
    
    return true;
}

std::vector<CHTLJSStateTransitionRule> CHTLJSStateMachine::FindApplicableCHTLJSRules(const CHTLJSToken& triggerToken) const {
    std::vector<CHTLJSStateTransitionRule> applicable;
    
    for (const auto& rule : transitionRules_) {
        if (rule.fromState == currentState_ && 
            rule.condition.triggerToken == triggerToken.GetType()) {
            
            if (CheckCHTLJSTransitionCondition(rule.condition)) {
                applicable.push_back(rule);
            }
        }
    }
    
    return applicable;
}

CHTLJSStateTransitionRule CHTLJSStateMachine::SelectBestCHTLJSRule(const std::vector<CHTLJSStateTransitionRule>& rules) const {
    // 按优先级选择最佳CHTL JS规则
    auto bestRule = std::max_element(rules.begin(), rules.end(),
        [](const CHTLJSStateTransitionRule& a, const CHTLJSStateTransitionRule& b) {
            return a.priority < b.priority;
        });
    
    return *bestRule;
}

// CHTLJSStateGuard实现（RAII守卫）
CHTLJSStateGuard::CHTLJSStateGuard(CHTLJSStateMachine& stateMachine, CHTLJSCompileState targetState, const std::string& reason)
    : stateMachine_(stateMachine), previousState_(stateMachine.GetCurrentState()), 
      enteredState_(targetState), isCommitted_(false) {
    
    isValid_ = stateMachine_.TransitionTo(targetState, reason);
    
    if (!isValid_) {
        Utils::ErrorHandler::GetInstance().LogError(
            "CHTL JS RAII状态守卫创建失败，无法转换到状态: " + stateMachine_.GetStateName(targetState)
        );
    }
}

CHTLJSStateGuard::~CHTLJSStateGuard() {
    // RAII自动状态管理
    if (isValid_ && !isCommitted_) {
        // 自动回滚到之前的状态
        stateMachine_.TransitionTo(previousState_, "CHTL JS RAII自动回滚");
        
        Utils::ErrorHandler::GetInstance().LogInfo(
            "CHTL JS RAII状态守卫自动回滚: " + stateMachine_.GetStateName(enteredState_) + 
            " -> " + stateMachine_.GetStateName(previousState_)
        );
    }
}

void CHTLJSStateGuard::Commit() {
    isCommitted_ = true;
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "CHTL JS RAII状态守卫已提交，状态: " + stateMachine_.GetStateName(enteredState_)
    );
}

// CHTLJSStateInferenceEngine实现
CHTLJSStateInferenceEngine::CHTLJSStateInferenceEngine(std::shared_ptr<CHTLJSStateContext> context)
    : context_(context) {
    InitializeCHTLJSInferenceRules();
}

CHTLJSCompileState CHTLJSStateInferenceEngine::InferCHTLJSObjectState(const std::vector<CHTLJSToken>& objectTokens) const {
    // 推断CHTL JS对象的不同状态
    if (objectTokens.empty()) {
        return CHTLJSCompileState::ERROR_STATE;
    }
    
    // 分析CHTL JS Token序列的模式
    std::string pattern;
    for (const auto& token : objectTokens) {
        pattern += token.GetValue() + " ";
    }
    
    // 基于语法文档进行CHTL JS状态推断
    for (const auto& rule : inferenceRules_) {
        if (pattern.find(rule.pattern) != std::string::npos) {
            if (!rule.validator || rule.validator(*context_)) {
                return rule.resultState;
            }
        }
    }
    
    return CHTLJSCompileState::INITIAL;
}

void CHTLJSStateMachine::Reset() {
    currentState_ = CHTLJSCompileState::INITIAL;
    stateHistory_.clear();
    while (!stateStack_.empty()) {
        stateStack_.pop();
    }
    
    Utils::ErrorHandler::GetInstance().LogInfo("CHTL JS状态机已重置");
}

std::string CHTLJSStateMachine::GetStatusReport() const {
    std::ostringstream report;
    report << "CHTL JS RAII状态机状态报告:\n";
    report << "  当前状态: " << GetStateName(currentState_) << "\n";
    report << "  状态历史长度: " << stateHistory_.size() << "\n";
    report << "  状态栈深度: " << stateStack_.size() << "\n";
    report << "  转换规则数量: " << transitionRules_.size() << "\n";
    
    return report.str();
}

void CHTLJSStateInferenceEngine::InitializeCHTLJSInferenceRules() {
    // 初始化CHTL JS推断规则（严格按照语法文档第1099-1531行）
    
    inferenceRules_ = {
        // 增强选择器推断规则 - 语法文档第1130-1158行
        {"{{", CHTLJSCompileState::PARSING_ENHANCED_SELECTOR, 95, nullptr},
        {"{{.box}}", CHTLJSCompileState::PARSING_ENHANCED_SELECTOR, 95, nullptr},
        {"{{#box}}", CHTLJSCompileState::PARSING_ENHANCED_SELECTOR, 95, nullptr},
        {"{{button}}", CHTLJSCompileState::PARSING_ENHANCED_SELECTOR, 95, nullptr},
        {"{{button[0]}}", CHTLJSCompileState::PARSING_ENHANCED_SELECTOR, 95, nullptr},
        
        // 虚对象推断规则 - 语法文档第1274-1311行
        {"vir", CHTLJSCompileState::PARSING_VIRTUAL_OBJECT, 95, nullptr},
        {"vir Test =", CHTLJSCompileState::PARSING_VIRTUAL_ASSIGNMENT, 95, nullptr},
        {"Test->", CHTLJSCompileState::PARSING_VIRTUAL_CALL, 90, nullptr},
        
        // 监听器推断规则 - 语法文档第1184-1213行
        {"listen", CHTLJSCompileState::PARSING_LISTEN_BLOCK, 95, nullptr},
        {"listen({", CHTLJSCompileState::PARSING_EVENT_MAPPING, 95, nullptr},
        
        // 事件委托推断规则 - 语法文档第1215-1232行
        {"delegate", CHTLJSCompileState::PARSING_DELEGATE_BLOCK, 95, nullptr},
        {"delegate({", CHTLJSCompileState::PARSING_EVENT_DELEGATION, 95, nullptr},
        
        // 动画推断规则 - 语法文档第1233-1271行
        {"animate", CHTLJSCompileState::PARSING_ANIMATE_BLOCK, 95, nullptr},
        {"animate({", CHTLJSCompileState::PARSING_ANIMATION_CONFIG, 95, nullptr},
        
        // 箭头函数推断规则 - 语法文档第1202行支持
        {"() =>", CHTLJSCompileState::PARSING_ARROW_FUNCTION, 95, nullptr},
        {"->", CHTLJSCompileState::PARSING_ARROW_OPERATION, 90, nullptr}
    };
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "CHTL JS状态推断规则初始化完成，共 " + std::to_string(inferenceRules_.size()) + " 条规则"
    );
}

std::vector<CHTLJSCompileState> CHTLJSStateMachine::PredictNextStates() const {
    std::vector<CHTLJSCompileState> nextStates;
    
    for (const auto& rule : transitionRules_) {
        if (rule.fromState == currentState_) {
            if (CheckCHTLJSTransitionCondition(rule.condition)) {
                nextStates.push_back(rule.toState);
            }
        }
    }
    
    return nextStates;
}

bool CHTLJSStateMachine::RollbackToPreviousState() {
    if (stateHistory_.empty()) {
        return false;
    }
    
    CHTLJSCompileState previousState = stateHistory_.back();
    stateHistory_.pop_back();
    currentState_ = previousState;
    
    return true;
}

CHTLJSCompileState CHTLJSStateInferenceEngine::ResolveCHTLJSStateAmbiguity(const std::vector<CHTLJSToken>& ambiguousTokens) const {
    // 解决CHTL JS状态分辨问题
    std::vector<CHTLJSCompileState> candidates;
    
    for (const auto& token : ambiguousTokens) {
        if (token.GetType() == TokenType::SELECTOR_START) {
            candidates.push_back(CHTLJSCompileState::PARSING_ENHANCED_SELECTOR);
        } else if (token.GetType() == TokenType::VIR) {
            candidates.push_back(CHTLJSCompileState::PARSING_VIRTUAL_OBJECT);
        } else if (token.GetType() == TokenType::LISTEN) {
            candidates.push_back(CHTLJSCompileState::PARSING_LISTEN_BLOCK);
        } else if (token.GetType() == TokenType::DELEGATE) {
            candidates.push_back(CHTLJSCompileState::PARSING_DELEGATE_BLOCK);
        } else if (token.GetType() == TokenType::ANIMATE) {
            candidates.push_back(CHTLJSCompileState::PARSING_ANIMATE_BLOCK);
        }
    }
    
    return context_->DisambiguateCHTLJSState(candidates);
}

CHTLJSCompileState CHTLJSStateInferenceEngine::InferFromCHTLJSSyntaxPattern(const std::string& pattern) const {
    for (const auto& rule : inferenceRules_) {
        if (rule.pattern == pattern) {
            return rule.resultState;
        }
    }
    
    return CHTLJSCompileState::INITIAL;
}

std::string CHTLJSStateInferenceEngine::InferEnhancedSelectorType(const std::string& selectorContent) const {
    // 推断增强选择器类型 - 语法文档第1147-1158行
    if (!selectorContent.empty() && selectorContent[0] == '.') {
        return "class";     // {{.box}}
    } else if (!selectorContent.empty() && selectorContent[0] == '#') {
        return "id";        // {{#box}}
    } else if (selectorContent.find("[") != std::string::npos) {
        return "indexed";   // {{button[0]}}
    } else if (selectorContent.find(" ") != std::string::npos) {
        return "complex";   // {{.box button}}
    } else {
        return "tag";       // {{button}}
    }
}

std::string CHTLJSStateInferenceEngine::InferEventType(const std::string& eventContext) const {
    // 推断事件类型
    if (eventContext.find("click") != std::string::npos) {
        return "click";
    } else if (eventContext.find("mouseenter") != std::string::npos) {
        return "mouseenter";
    } else if (eventContext.find("mouseleave") != std::string::npos) {
        return "mouseleave";
    } else if (eventContext.find("mousemove") != std::string::npos) {
        return "mousemove";
    }
    
    return "unknown";
}

} // namespace Core
} // namespace CHTLJS
} // namespace CHTL