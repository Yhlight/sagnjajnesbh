#include "CHTL/Core/CHTLStateContext.h"
#include "Utils/ErrorHandler.h"
#include <algorithm>
#include <sstream>
#include <regex>

namespace CHTL {
namespace Core {

// CHTLStateContext实现
CHTLStateContext::CHTLStateContext(const std::string& sourceFile, const std::vector<CHTLToken>& tokens)
    : sourceFile_(sourceFile), tokens_(tokens), currentPosition_(0) {
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "CHTL状态上下文初始化完成，源文件: " + sourceFile_ + 
        "，Token数量: " + std::to_string(tokens_.size())
    );
}

const CHTLToken& CHTLStateContext::GetCurrentToken() const {
    if (IsAtEnd()) {
        static CHTLToken eofToken(TokenType::END_OF_FILE, "", 0, 0);
        return eofToken;
    }
    return tokens_[currentPosition_];
}

const CHTLToken* CHTLStateContext::LookAhead(int offset) const {
    size_t targetPos = currentPosition_ + offset;
    if (targetPos >= tokens_.size()) {
        return nullptr;
    }
    return &tokens_[targetPos];
}

const CHTLToken* CHTLStateContext::LookBehind(int offset) const {
    if (currentPosition_ < static_cast<size_t>(offset)) {
        return nullptr;
    }
    size_t targetPos = currentPosition_ - offset;
    return &tokens_[targetPos];
}

std::vector<CHTLToken> CHTLStateContext::GetSurroundingContext(int radius) const {
    std::vector<CHTLToken> surrounding;
    
    // 向后看
    for (int i = radius; i > 0; --i) {
        const CHTLToken* token = LookBehind(i);
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
        const CHTLToken* token = LookAhead(i);
        if (token) {
            surrounding.push_back(*token);
        }
    }
    
    return surrounding;
}

std::string CHTLStateContext::InferFromContext(ContextInferenceType inferenceType, 
                                              ContextLookDirection direction, 
                                              int depth) const {
    switch (inferenceType) {
        case ContextInferenceType::SYNTAX_COMPLETION:
            return InferSyntaxCompletion(direction, depth);
        case ContextInferenceType::SCOPE_RESOLUTION:
            return InferScopeResolution(direction, depth);
        case ContextInferenceType::TYPE_INFERENCE:
            return InferTypeFromContext(direction, depth);
        case ContextInferenceType::NAMESPACE_RESOLUTION:
            return InferNamespaceFromContext(direction, depth);
        default:
            return "";
    }
}

bool CHTLStateContext::CheckSyntaxCompleteness(const std::string& syntaxPattern) const {
    // 检查语法模式是否在当前上下文中完整
    // 例如：检查 [Template] @Style 模式是否完整
    
    std::vector<CHTLToken> context = GetSurroundingContext(5);
    std::string contextStr;
    
    for (const auto& token : context) {
        contextStr += token.GetValue() + " ";
    }
    
    // 使用正则表达式检查语法模式
    try {
        std::regex pattern(syntaxPattern);
        return std::regex_search(contextStr, pattern);
    } catch (const std::regex_error&) {
        return false;
    }
}

CHTLCompileState CHTLStateContext::DisambiguateState(const std::vector<CHTLCompileState>& possibleStates) const {
    return DisambiguateByContext(possibleStates);
}

void CHTLStateContext::Advance() {
    if (!IsAtEnd()) {
        currentPosition_++;
    }
}

void CHTLStateContext::SetPosition(size_t position) {
    currentPosition_ = std::min(position, tokens_.size());
}

bool CHTLStateContext::IsAtEnd() const {
    return currentPosition_ >= tokens_.size();
}

// 推断辅助方法实现
std::string CHTLStateContext::InferSyntaxCompletion(ContextLookDirection direction, int depth) const {
    std::vector<CHTLToken> contextTokens;
    
    if (direction == ContextLookDirection::FORWARD) {
        for (int i = 1; i <= depth; ++i) {
            const CHTLToken* token = LookAhead(i);
            if (token) contextTokens.push_back(*token);
        }
    } else if (direction == ContextLookDirection::BACKWARD) {
        for (int i = 1; i <= depth; ++i) {
            const CHTLToken* token = LookBehind(i);
            if (token) contextTokens.push_back(*token);
        }
    }
    
    // 基于语法文档进行语法补全推断
    std::string result;
    for (const auto& token : contextTokens) {
        if (token.GetType() == TokenType::LEFT_BRACKET) {
            // 可能是 [Template]、[Custom]、[Origin]、[Configuration]、[Namespace]、[Import]
            const CHTLToken* nextToken = (direction == ContextLookDirection::FORWARD) ? 
                                        LookAhead(2) : LookBehind(2);
            if (nextToken && nextToken->GetType() == TokenType::IDENTIFIER) {
                std::string identifier = nextToken->GetValue();
                if (identifier == "Template" || identifier == "Custom" || 
                    identifier == "Origin" || identifier == "Configuration" ||
                    identifier == "Namespace" || identifier == "Import") {
                    result = "[" + identifier + "]";
                    break;
                }
            }
        } else if (token.GetType() == TokenType::AT_STYLE || 
                   token.GetType() == TokenType::AT_ELEMENT ||
                   token.GetType() == TokenType::AT_VAR) {
            // 可能是模板或自定义类型
            result = token.GetValue();
            break;
        }
    }
    
    return result;
}

std::string CHTLStateContext::InferScopeResolution(ContextLookDirection direction, int depth) const {
    // 基于语法文档的作用域规则进行推断
    std::vector<CHTLToken> contextTokens = GetSurroundingContext(depth);
    
    for (const auto& token : contextTokens) {
        if (token.GetType() == TokenType::LEFT_BRACKET) {
            // 检查是否在特定作用域内
            const CHTLToken* nextToken = LookAhead(1);
            if (nextToken && nextToken->GetValue() == "Namespace") {
                return "namespace_scope";
            } else if (nextToken && nextToken->GetValue() == "Template") {
                return "template_scope";
            } else if (nextToken && nextToken->GetValue() == "Custom") {
                return "custom_scope";
            }
        }
    }
    
    return "global_scope";
}

std::string CHTLStateContext::InferTypeFromContext(ContextLookDirection direction, int depth) const {
    // 基于语法文档的类型系统进行推断
    const CHTLToken* prevToken = LookBehind(1);
    const CHTLToken* nextToken = LookAhead(1);
    
    if (prevToken && prevToken->GetType() == TokenType::AT) {
        // @Style、@Element、@Var等类型
        const CHTLToken& current = GetCurrentToken();
        if (current.GetType() == TokenType::IDENTIFIER) {
            std::string identifier = current.GetValue();
            if (identifier == "Style" || identifier == "Element" || identifier == "Var" ||
                identifier == "Html" || identifier == "JavaScript" || identifier == "Config") {
                return "@" + identifier;
            }
        }
    }
    
    return "";
}

std::string CHTLStateContext::InferNamespaceFromContext(ContextLookDirection direction, int depth) const {
    // 推断命名空间信息 - 语法文档第956-1061行
    std::vector<CHTLToken> contextTokens = GetSurroundingContext(depth);
    
    for (size_t i = 0; i < contextTokens.size() - 1; ++i) {
        if (contextTokens[i].GetType() == TokenType::LEFT_BRACKET &&
            contextTokens[i + 1].GetValue() == "Namespace") {
            // 找到命名空间声明
            if (i + 2 < contextTokens.size()) {
                return contextTokens[i + 2].GetValue();
            }
        }
    }
    
    // 如果没有明确的命名空间，使用文件名作为默认命名空间
    // 语法文档第962行："如果文件没有命名空间，则默认使用文件名称来作为命名空间"
    size_t lastSlash = sourceFile_.find_last_of('/');
    size_t lastDot = sourceFile_.find_last_of('.');
    if (lastSlash != std::string::npos && lastDot != std::string::npos && lastDot > lastSlash) {
        return sourceFile_.substr(lastSlash + 1, lastDot - lastSlash - 1);
    }
    
    return "";
}

CHTLCompileState CHTLStateContext::DisambiguateByContext(const std::vector<CHTLCompileState>& states) const {
    if (states.empty()) {
        return CHTLCompileState::ERROR_STATE;
    }
    
    if (states.size() == 1) {
        return states[0];
    }
    
    // 基于上下文消除歧义
    std::vector<CHTLToken> context = GetSurroundingContext(3);
    
    // 检查是否在特定的语法结构中
    for (const auto& token : context) {
        if (token.GetType() == TokenType::LEFT_BRACKET) {
            const CHTLToken* nextToken = LookAhead(1);
            if (nextToken) {
                std::string identifier = nextToken->GetValue();
                
                // 根据语法结构确定状态
                if (identifier == "Template") {
                    for (auto state : states) {
                        if (state == CHTLCompileState::PARSING_TEMPLATE_STYLE ||
                            state == CHTLCompileState::PARSING_TEMPLATE_ELEMENT ||
                            state == CHTLCompileState::PARSING_TEMPLATE_VAR) {
                            return state;
                        }
                    }
                } else if (identifier == "Custom") {
                    for (auto state : states) {
                        if (state == CHTLCompileState::PARSING_CUSTOM_STYLE ||
                            state == CHTLCompileState::PARSING_CUSTOM_ELEMENT ||
                            state == CHTLCompileState::PARSING_CUSTOM_VAR) {
                            return state;
                        }
                    }
                } else if (identifier == "Origin") {
                    for (auto state : states) {
                        if (state == CHTLCompileState::PARSING_ORIGIN_HTML ||
                            state == CHTLCompileState::PARSING_ORIGIN_STYLE ||
                            state == CHTLCompileState::PARSING_ORIGIN_JAVASCRIPT) {
                            return state;
                        }
                    }
                }
            }
        }
    }
    
    // 默认返回第一个状态
    return states[0];
}

// CHTLStateMachine实现
CHTLStateMachine::CHTLStateMachine(std::shared_ptr<CHTLStateContext> context)
    : context_(context), currentState_(CHTLCompileState::INITIAL) {
    
    InitializeTransitionRules();
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "CHTL RAII状态机初始化完成，源文件: " + context_->GetSourceFile()
    );
}

CHTLStateMachine::~CHTLStateMachine() {
    // RAII自动清理
    if (currentState_ != CHTLCompileState::INITIAL && 
        currentState_ != CHTLCompileState::COMPLETED_STATE &&
        currentState_ != CHTLCompileState::ERROR_STATE) {
        
        Utils::ErrorHandler::GetInstance().LogWarning(
            "状态机析构时状态未正确结束: " + GetStateName(currentState_)
        );
    }
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "CHTL RAII状态机清理完成，处理了 " + std::to_string(stateHistory_.size()) + " 个状态转换"
    );
}

bool CHTLStateMachine::TransitionTo(CHTLCompileState newState, const std::string& reason) {
    if (!IsValidTransition(currentState_, newState)) {
        Utils::ErrorHandler::GetInstance().LogError(
            "无效的状态转换: " + GetStateName(currentState_) + " -> " + GetStateName(newState) +
            (reason.empty() ? "" : " (原因: " + reason + ")")
        );
        return false;
    }
    
    // 记录状态历史
    stateHistory_.push_back(currentState_);
    
    CHTLCompileState previousState = currentState_;
    currentState_ = newState;
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "CHTL状态转换: " + GetStateName(previousState) + " -> " + GetStateName(newState) +
        (reason.empty() ? "" : " (原因: " + reason + ")")
    );
    
    return true;
}

bool CHTLStateMachine::SmartTransition(const CHTLToken& triggerToken) {
    // 基于Context推断进行智能状态转换
    
    // 1. 查找适用的转换规则
    std::vector<StateTransitionRule> applicableRules = FindApplicableRules(triggerToken);
    
    if (applicableRules.empty()) {
        Utils::ErrorHandler::GetInstance().LogWarning(
            "没有找到适用的状态转换规则，Token: " + triggerToken.GetValue()
        );
        return false;
    }
    
    // 2. 如果有多个规则，使用Context进行消歧
    StateTransitionRule selectedRule;
    if (applicableRules.size() > 1) {
        selectedRule = SelectBestRule(applicableRules);
    } else {
        selectedRule = applicableRules[0];
    }
    
    // 3. 执行状态转换
    return TransitionTo(selectedRule.toState, selectedRule.condition.description);
}

bool CHTLStateMachine::RollbackToPreviousState() {
    if (stateHistory_.empty()) {
        Utils::ErrorHandler::GetInstance().LogWarning(
            "无法回滚状态，状态历史为空"
        );
        return false;
    }
    
    CHTLCompileState previousState = stateHistory_.back();
    stateHistory_.pop_back();
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "状态回滚: " + GetStateName(currentState_) + " -> " + GetStateName(previousState)
    );
    
    currentState_ = previousState;
    return true;
}

bool CHTLStateMachine::IsValidTransition(CHTLCompileState from, CHTLCompileState to) const {
    // 检查转换规则表
    for (const auto& rule : transitionRules_) {
        if (rule.fromState == from && rule.toState == to) {
            return CheckTransitionCondition(rule.condition);
        }
    }
    
    // 特殊情况：任何状态都可以转换到错误状态
    if (to == CHTLCompileState::ERROR_STATE) {
        return true;
    }
    
    return false;
}

std::vector<CHTLCompileState> CHTLStateMachine::PredictNextStates() const {
    std::vector<CHTLCompileState> nextStates;
    
    // 基于当前状态和Context预测可能的下一个状态
    for (const auto& rule : transitionRules_) {
        if (rule.fromState == currentState_) {
            // 检查条件是否可能满足
            if (CheckTransitionCondition(rule.condition)) {
                nextStates.push_back(rule.toState);
            }
        }
    }
    
    return nextStates;
}

std::string CHTLStateMachine::GetStateName(CHTLCompileState state) const {
    switch (state) {
        case CHTLCompileState::INITIAL: return "INITIAL";
        case CHTLCompileState::PARSING_HTML_ELEMENT: return "PARSING_HTML_ELEMENT";
        case CHTLCompileState::PARSING_TEXT_NODE: return "PARSING_TEXT_NODE";
        case CHTLCompileState::PARSING_ATTRIBUTES: return "PARSING_ATTRIBUTES";
        case CHTLCompileState::PARSING_LOCAL_STYLE: return "PARSING_LOCAL_STYLE";
        case CHTLCompileState::PARSING_INLINE_STYLE: return "PARSING_INLINE_STYLE";
        case CHTLCompileState::PARSING_AUTO_CLASS_ID: return "PARSING_AUTO_CLASS_ID";
        case CHTLCompileState::PARSING_REFERENCE_SELECTOR: return "PARSING_REFERENCE_SELECTOR";
        case CHTLCompileState::PARSING_TEMPLATE_STYLE: return "PARSING_TEMPLATE_STYLE";
        case CHTLCompileState::PARSING_TEMPLATE_ELEMENT: return "PARSING_TEMPLATE_ELEMENT";
        case CHTLCompileState::PARSING_TEMPLATE_VAR: return "PARSING_TEMPLATE_VAR";
        case CHTLCompileState::PARSING_TEMPLATE_INHERITANCE: return "PARSING_TEMPLATE_INHERITANCE";
        case CHTLCompileState::PARSING_CUSTOM_STYLE: return "PARSING_CUSTOM_STYLE";
        case CHTLCompileState::PARSING_CUSTOM_ELEMENT: return "PARSING_CUSTOM_ELEMENT";
        case CHTLCompileState::PARSING_CUSTOM_VAR: return "PARSING_CUSTOM_VAR";
        case CHTLCompileState::PARSING_CUSTOM_SPECIALIZATION: return "PARSING_CUSTOM_SPECIALIZATION";
        case CHTLCompileState::PARSING_CUSTOM_DELETION: return "PARSING_CUSTOM_DELETION";
        case CHTLCompileState::PARSING_CUSTOM_INSERTION: return "PARSING_CUSTOM_INSERTION";
        case CHTLCompileState::PARSING_CUSTOM_INDEX_ACCESS: return "PARSING_CUSTOM_INDEX_ACCESS";
        case CHTLCompileState::PARSING_ORIGIN_HTML: return "PARSING_ORIGIN_HTML";
        case CHTLCompileState::PARSING_ORIGIN_STYLE: return "PARSING_ORIGIN_STYLE";
        case CHTLCompileState::PARSING_ORIGIN_JAVASCRIPT: return "PARSING_ORIGIN_JAVASCRIPT";
        case CHTLCompileState::PARSING_ORIGIN_NAMED: return "PARSING_ORIGIN_NAMED";
        case CHTLCompileState::PARSING_ORIGIN_CUSTOM_TYPE: return "PARSING_ORIGIN_CUSTOM_TYPE";
        case CHTLCompileState::PARSING_CONFIGURATION: return "PARSING_CONFIGURATION";
        case CHTLCompileState::PARSING_CONFIGURATION_NAMED: return "PARSING_CONFIGURATION_NAMED";
        case CHTLCompileState::PARSING_NAME_CONFIG: return "PARSING_NAME_CONFIG";
        case CHTLCompileState::PARSING_ORIGIN_TYPE_CONFIG: return "PARSING_ORIGIN_TYPE_CONFIG";
        case CHTLCompileState::PARSING_IMPORT_HTML: return "PARSING_IMPORT_HTML";
        case CHTLCompileState::PARSING_IMPORT_STYLE: return "PARSING_IMPORT_STYLE";
        case CHTLCompileState::PARSING_IMPORT_JAVASCRIPT: return "PARSING_IMPORT_JAVASCRIPT";
        case CHTLCompileState::PARSING_IMPORT_CHTL: return "PARSING_IMPORT_CHTL";
        case CHTLCompileState::PARSING_IMPORT_CJMOD: return "PARSING_IMPORT_CJMOD";
        case CHTLCompileState::PARSING_IMPORT_CONFIG: return "PARSING_IMPORT_CONFIG";
        case CHTLCompileState::PARSING_IMPORT_TEMPLATE: return "PARSING_IMPORT_TEMPLATE";
        case CHTLCompileState::PARSING_IMPORT_CUSTOM: return "PARSING_IMPORT_CUSTOM";
        case CHTLCompileState::PARSING_IMPORT_ORIGIN: return "PARSING_IMPORT_ORIGIN";
        case CHTLCompileState::PARSING_IMPORT_BATCH: return "PARSING_IMPORT_BATCH";
        case CHTLCompileState::PARSING_NAMESPACE: return "PARSING_NAMESPACE";
        case CHTLCompileState::PARSING_NAMESPACE_NESTED: return "PARSING_NAMESPACE_NESTED";
        case CHTLCompileState::PARSING_NAMESPACE_USAGE: return "PARSING_NAMESPACE_USAGE";
        case CHTLCompileState::PARSING_CONSTRAINT_PRECISE: return "PARSING_CONSTRAINT_PRECISE";
        case CHTLCompileState::PARSING_CONSTRAINT_TYPE: return "PARSING_CONSTRAINT_TYPE";
        case CHTLCompileState::PARSING_CONSTRAINT_GLOBAL: return "PARSING_CONSTRAINT_GLOBAL";
        case CHTLCompileState::PARSING_LOCAL_SCRIPT: return "PARSING_LOCAL_SCRIPT";
        case CHTLCompileState::ERROR_STATE: return "ERROR_STATE";
        case CHTLCompileState::COMPLETED_STATE: return "COMPLETED_STATE";
        default: return "UNKNOWN_STATE";
    }
}

void CHTLStateMachine::Reset() {
    currentState_ = CHTLCompileState::INITIAL;
    stateHistory_.clear();
    while (!stateStack_.empty()) {
        stateStack_.pop();
    }
    
    Utils::ErrorHandler::GetInstance().LogInfo("CHTL状态机已重置");
}

std::string CHTLStateMachine::GetStatusReport() const {
    std::ostringstream report;
    report << "CHTL RAII状态机状态报告:\n";
    report << "  当前状态: " << GetStateName(currentState_) << "\n";
    report << "  状态历史长度: " << stateHistory_.size() << "\n";
    report << "  状态栈深度: " << stateStack_.size() << "\n";
    report << "  转换规则数量: " << transitionRules_.size() << "\n";
    
    if (!stateHistory_.empty()) {
        report << "  最近状态转换: ";
        for (size_t i = std::max(0, static_cast<int>(stateHistory_.size()) - 3); 
             i < stateHistory_.size(); ++i) {
            if (i > 0) report << " -> ";
            report << GetStateName(stateHistory_[i]);
        }
        report << " -> " << GetStateName(currentState_) << "\n";
    }
    
    return report.str();
}

// 初始化转换规则（基于语法文档的完整语法结构）
void CHTLStateMachine::InitializeTransitionRules() {
    transitionRules_.clear();
    
    InitializeBasicTransitions();
    InitializeTemplateTransitions();
    InitializeCustomTransitions();
    InitializeOriginTransitions();
    InitializeConfigTransitions();
    InitializeImportTransitions();
    InitializeNamespaceTransitions();
    InitializeConstraintTransitions();
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "CHTL状态转换规则初始化完成，共 " + std::to_string(transitionRules_.size()) + " 条规则"
    );
}

void CHTLStateMachine::InitializeBasicTransitions() {
    // 基础语法转换 - 语法文档第1-156行
    
    // HTML元素解析转换
    transitionRules_.push_back({
        CHTLCompileState::INITIAL, 
        CHTLCompileState::PARSING_HTML_ELEMENT,
        {TokenType::IDENTIFIER, nullptr, "HTML元素开始", false, false},
        1
    });
    
    // 文本节点转换 - 语法文档第10-18行
    transitionRules_.push_back({
        CHTLCompileState::INITIAL,
        CHTLCompileState::PARSING_TEXT_NODE,
        {TokenType::TEXT, nullptr, "文本节点开始", false, false},
        1
    });
    
    // 属性解析转换 - 语法文档第70-84行
    transitionRules_.push_back({
        CHTLCompileState::PARSING_HTML_ELEMENT,
        CHTLCompileState::PARSING_ATTRIBUTES,
        {TokenType::IDENTIFIER, nullptr, "属性解析", false, false},
        1
    });
    
    // 局部样式块转换 - 语法文档第86-156行
    transitionRules_.push_back({
        CHTLCompileState::PARSING_HTML_ELEMENT,
        CHTLCompileState::PARSING_LOCAL_STYLE,
        {TokenType::STYLE, nullptr, "局部样式块", false, false},
        1
    });
    
    // 局部脚本块转换 - 语法文档第1104-1128行
    transitionRules_.push_back({
        CHTLCompileState::PARSING_HTML_ELEMENT,
        CHTLCompileState::PARSING_LOCAL_SCRIPT,
        {TokenType::SCRIPT, nullptr, "局部脚本块", false, false},
        1
    });
}

void CHTLStateMachine::InitializeTemplateTransitions() {
    // 模板系统转换 - 语法文档第158-287行
    
    // 样式组模板 - 语法文档第161-178行
    transitionRules_.push_back({
        CHTLCompileState::INITIAL,
        CHTLCompileState::PARSING_TEMPLATE_STYLE,
        {TokenType::LEFT_BRACKET, 
         [](const CHTLStateContext& ctx) {
             const CHTLToken* next1 = ctx.LookAhead(1);
             const CHTLToken* next2 = ctx.LookAhead(2);
             const CHTLToken* next3 = ctx.LookAhead(3);
             return next1 && next1->GetValue() == "Template" &&
                    next2 && next2->GetType() == TokenType::RIGHT_BRACKET &&
                    next3 && next3->GetType() == TokenType::AT_STYLE;
         }, 
         "模板样式组", true, false},
        2
    });
    
    // 元素模板 - 语法文档第180-219行
    transitionRules_.push_back({
        CHTLCompileState::INITIAL,
        CHTLCompileState::PARSING_TEMPLATE_ELEMENT,
        {TokenType::LEFT_BRACKET,
         [](const CHTLStateContext& ctx) {
             const CHTLToken* next1 = ctx.LookAhead(1);
             const CHTLToken* next2 = ctx.LookAhead(2);
             const CHTLToken* next3 = ctx.LookAhead(3);
             return next1 && next1->GetValue() == "Template" &&
                    next2 && next2->GetType() == TokenType::RIGHT_BRACKET &&
                    next3 && next3->GetType() == TokenType::AT_ELEMENT;
         },
         "模板元素", true, false},
        2
    });
    
    // 变量组模板 - 语法文档第221-252行
    transitionRules_.push_back({
        CHTLCompileState::INITIAL,
        CHTLCompileState::PARSING_TEMPLATE_VAR,
        {TokenType::LEFT_BRACKET,
         [](const CHTLStateContext& ctx) {
             const CHTLToken* next1 = ctx.LookAhead(1);
             const CHTLToken* next2 = ctx.LookAhead(2);
             const CHTLToken* next3 = ctx.LookAhead(3);
             return next1 && next1->GetValue() == "Template" &&
                    next2 && next2->GetType() == TokenType::RIGHT_BRACKET &&
                    next3 && next3->GetType() == TokenType::AT_VAR;
         },
         "模板变量组", true, false},
        2
    });
}

void CHTLStateMachine::InitializeCustomTransitions() {
    // 自定义系统转换 - 语法文档第288-627行
    
    // 自定义样式组 - 语法文档第294-385行
    transitionRules_.push_back({
        CHTLCompileState::INITIAL,
        CHTLCompileState::PARSING_CUSTOM_STYLE,
        {TokenType::LEFT_BRACKET,
         [](const CHTLStateContext& ctx) {
             const CHTLToken* next1 = ctx.LookAhead(1);
             const CHTLToken* next2 = ctx.LookAhead(2);
             const CHTLToken* next3 = ctx.LookAhead(3);
             return next1 && next1->GetValue() == "Custom" &&
                    next2 && next2->GetType() == TokenType::RIGHT_BRACKET &&
                    next3 && next3->GetType() == TokenType::AT_STYLE;
         },
         "自定义样式组", true, false},
        2
    });
    
    // 自定义元素 - 语法文档第387-583行
    transitionRules_.push_back({
        CHTLCompileState::INITIAL,
        CHTLCompileState::PARSING_CUSTOM_ELEMENT,
        {TokenType::LEFT_BRACKET,
         [](const CHTLStateContext& ctx) {
             const CHTLToken* next1 = ctx.LookAhead(1);
             const CHTLToken* next2 = ctx.LookAhead(2);
             const CHTLToken* next3 = ctx.LookAhead(3);
             return next1 && next1->GetValue() == "Custom" &&
                    next2 && next2->GetType() == TokenType::RIGHT_BRACKET &&
                    next3 && next3->GetType() == TokenType::AT_ELEMENT;
         },
         "自定义元素", true, false},
        2
    });
    
    // 自定义变量组 - 语法文档第585-601行
    transitionRules_.push_back({
        CHTLCompileState::INITIAL,
        CHTLCompileState::PARSING_CUSTOM_VAR,
        {TokenType::LEFT_BRACKET,
         [](const CHTLStateContext& ctx) {
             const CHTLToken* next1 = ctx.LookAhead(1);
             const CHTLToken* next2 = ctx.LookAhead(2);
             const CHTLToken* next3 = ctx.LookAhead(3);
             return next1 && next1->GetValue() == "Custom" &&
                    next2 && next2->GetType() == TokenType::RIGHT_BRACKET &&
                    next3 && next3->GetType() == TokenType::AT_VAR;
         },
         "自定义变量组", true, false},
        2
    });
}

void CHTLStateMachine::InitializeOriginTransitions() {
    // 原始嵌入转换 - 语法文档第629-697行
    
    // HTML原始嵌入 - 语法文档第634-648行
    transitionRules_.push_back({
        CHTLCompileState::INITIAL,
        CHTLCompileState::PARSING_ORIGIN_HTML,
        {TokenType::LEFT_BRACKET,
         [](const CHTLStateContext& ctx) {
             const CHTLToken* next1 = ctx.LookAhead(1);
             const CHTLToken* next2 = ctx.LookAhead(2);
             const CHTLToken* next3 = ctx.LookAhead(3);
             return next1 && next1->GetValue() == "Origin" &&
                    next2 && next2->GetType() == TokenType::RIGHT_BRACKET &&
                    next3 && next3->GetType() == TokenType::AT_HTML;
         },
         "HTML原始嵌入", true, false},
        2
    });
    
    // CSS原始嵌入 - 语法文档第650-656行
    transitionRules_.push_back({
        CHTLCompileState::INITIAL,
        CHTLCompileState::PARSING_ORIGIN_STYLE,
        {TokenType::LEFT_BRACKET,
         [](const CHTLStateContext& ctx) {
             const CHTLToken* next1 = ctx.LookAhead(1);
             const CHTLToken* next2 = ctx.LookAhead(2);
             const CHTLToken* next3 = ctx.LookAhead(3);
             return next1 && next1->GetValue() == "Origin" &&
                    next2 && next2->GetType() == TokenType::RIGHT_BRACKET &&
                    next3 && next3->GetType() == TokenType::AT_STYLE;
         },
         "CSS原始嵌入", true, false},
        2
    });
    
    // JS原始嵌入 - 语法文档第658-664行
    transitionRules_.push_back({
        CHTLCompileState::INITIAL,
        CHTLCompileState::PARSING_ORIGIN_JAVASCRIPT,
        {TokenType::LEFT_BRACKET,
         [](const CHTLStateContext& ctx) {
             const CHTLToken* next1 = ctx.LookAhead(1);
             const CHTLToken* next2 = ctx.LookAhead(2);
             const CHTLToken* next3 = ctx.LookAhead(3);
             return next1 && next1->GetValue() == "Origin" &&
                    next2 && next2->GetType() == TokenType::RIGHT_BRACKET &&
                    next3 && next3->GetType() == TokenType::AT_JAVASCRIPT;
         },
         "JS原始嵌入", true, false},
        2
    });
}

void CHTLStateMachine::InitializeConfigTransitions() {
    // 配置组转换 - 语法文档第699-879行
    
    // 普通配置组 - 语法文档第703-767行
    transitionRules_.push_back({
        CHTLCompileState::INITIAL,
        CHTLCompileState::PARSING_CONFIGURATION,
        {TokenType::LEFT_BRACKET,
         [](const CHTLStateContext& ctx) {
             const CHTLToken* next1 = ctx.LookAhead(1);
             const CHTLToken* next2 = ctx.LookAhead(2);
             return next1 && next1->GetValue() == "Configuration" &&
                    next2 && next2->GetType() == TokenType::RIGHT_BRACKET;
         },
         "配置组", true, false},
        2
    });
    
    // 命名配置组 - 语法文档第770-879行
    transitionRules_.push_back({
        CHTLCompileState::INITIAL,
        CHTLCompileState::PARSING_CONFIGURATION_NAMED,
        {TokenType::LEFT_BRACKET,
         [](const CHTLStateContext& ctx) {
             const CHTLToken* next1 = ctx.LookAhead(1);
             const CHTLToken* next2 = ctx.LookAhead(2);
             const CHTLToken* next3 = ctx.LookAhead(3);
             return next1 && next1->GetValue() == "Configuration" &&
                    next2 && next2->GetType() == TokenType::RIGHT_BRACKET &&
                    next3 && next3->GetType() == TokenType::AT_CONFIG;
         },
         "命名配置组", true, false},
        3
    });
}

void CHTLStateMachine::InitializeImportTransitions() {
    // 导入系统转换 - 语法文档第881-955行
    
    // Import转换的通用模式
    transitionRules_.push_back({
        CHTLCompileState::INITIAL,
        CHTLCompileState::PARSING_IMPORT_HTML,
        {TokenType::LEFT_BRACKET,
         [](const CHTLStateContext& ctx) {
             const CHTLToken* next1 = ctx.LookAhead(1);
             const CHTLToken* next2 = ctx.LookAhead(2);
             const CHTLToken* next3 = ctx.LookAhead(3);
             return next1 && next1->GetValue() == "Import" &&
                    next2 && next2->GetType() == TokenType::RIGHT_BRACKET &&
                    next3 && next3->GetType() == TokenType::AT_HTML;
         },
         "HTML导入", true, false},
        2
    });
}

void CHTLStateMachine::InitializeNamespaceTransitions() {
    // 命名空间转换 - 语法文档第956-1061行
    
    transitionRules_.push_back({
        CHTLCompileState::INITIAL,
        CHTLCompileState::PARSING_NAMESPACE,
        {TokenType::LEFT_BRACKET,
         [](const CHTLStateContext& ctx) {
             const CHTLToken* next1 = ctx.LookAhead(1);
             const CHTLToken* next2 = ctx.LookAhead(2);
             return next1 && next1->GetValue() == "Namespace" &&
                    next2 && next2->GetType() == TokenType::RIGHT_BRACKET;
         },
         "命名空间", true, false},
        2
    });
}

void CHTLStateMachine::InitializeConstraintTransitions() {
    // 约束系统转换 - 语法文档第1062-1097行
    
    transitionRules_.push_back({
        CHTLCompileState::PARSING_HTML_ELEMENT,
        CHTLCompileState::PARSING_CONSTRAINT_PRECISE,
        {TokenType::EXCEPT, nullptr, "精确约束", false, false},
        1
    });
}

// 状态转换辅助方法实现
bool CHTLStateMachine::CheckTransitionCondition(const StateTransitionCondition& condition) const {
    // 检查Token类型
    if (context_->GetCurrentToken().GetType() != condition.triggerToken) {
        return false;
    }
    
    // 检查上下文条件
    if (condition.contextCheck && !condition.contextCheck(*context_)) {
        return false;
    }
    
    return true;
}

std::vector<StateTransitionRule> CHTLStateMachine::FindApplicableRules(const CHTLToken& triggerToken) const {
    std::vector<StateTransitionRule> applicable;
    
    for (const auto& rule : transitionRules_) {
        if (rule.fromState == currentState_ && 
            rule.condition.triggerToken == triggerToken.GetType()) {
            
            if (CheckTransitionCondition(rule.condition)) {
                applicable.push_back(rule);
            }
        }
    }
    
    return applicable;
}

StateTransitionRule CHTLStateMachine::SelectBestRule(const std::vector<StateTransitionRule>& rules) const {
    // 按优先级排序，选择最高优先级的规则
    auto bestRule = std::max_element(rules.begin(), rules.end(),
        [](const StateTransitionRule& a, const StateTransitionRule& b) {
            return a.priority < b.priority;
        });
    
    return *bestRule;
}

bool CHTLStateMachine::RequiresContextInference(CHTLCompileState state) const {
    // 某些状态需要Context推断来消歧
    switch (state) {
        case CHTLCompileState::PARSING_TEMPLATE_STYLE:
        case CHTLCompileState::PARSING_TEMPLATE_ELEMENT:
        case CHTLCompileState::PARSING_TEMPLATE_VAR:
        case CHTLCompileState::PARSING_CUSTOM_STYLE:
        case CHTLCompileState::PARSING_CUSTOM_ELEMENT:
        case CHTLCompileState::PARSING_CUSTOM_VAR:
        case CHTLCompileState::PARSING_ORIGIN_HTML:
        case CHTLCompileState::PARSING_ORIGIN_STYLE:
        case CHTLCompileState::PARSING_ORIGIN_JAVASCRIPT:
            return true;
        default:
            return false;
    }
}

CHTLCompileState CHTLStateMachine::InferStateFromContext(const std::vector<CHTLCompileState>& candidates) const {
    return context_->DisambiguateState(candidates);
}

// CHTLStateGuard实现（RAII守卫）
CHTLStateGuard::CHTLStateGuard(CHTLStateMachine& stateMachine, CHTLCompileState targetState, const std::string& reason)
    : stateMachine_(stateMachine), previousState_(stateMachine.GetCurrentState()), 
      enteredState_(targetState), isCommitted_(false) {
    
    isValid_ = stateMachine_.TransitionTo(targetState, reason);
    
    if (!isValid_) {
        Utils::ErrorHandler::GetInstance().LogError(
            "RAII状态守卫创建失败，无法转换到状态: " + stateMachine_.GetStateName(targetState)
        );
    }
}

CHTLStateGuard::~CHTLStateGuard() {
    // RAII自动状态管理
    if (isValid_ && !isCommitted_) {
        // 自动回滚到之前的状态
        stateMachine_.TransitionTo(previousState_, "RAII自动回滚");
        
        Utils::ErrorHandler::GetInstance().LogInfo(
            "RAII状态守卫自动回滚: " + stateMachine_.GetStateName(enteredState_) + 
            " -> " + stateMachine_.GetStateName(previousState_)
        );
    }
}

void CHTLStateGuard::Commit() {
    isCommitted_ = true;
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "RAII状态守卫已提交，状态: " + stateMachine_.GetStateName(enteredState_)
    );
}

// CHTLStateInferenceEngine实现
CHTLStateInferenceEngine::CHTLStateInferenceEngine(std::shared_ptr<CHTLStateContext> context)
    : context_(context) {
    InitializeInferenceRules();
}

CHTLCompileState CHTLStateInferenceEngine::InferObjectState(const std::vector<CHTLToken>& objectTokens) const {
    // 推断物体的不同状态
    // 这是您提到的"一个物体不同的状态的分辨问题"的核心解决方案
    
    if (objectTokens.empty()) {
        return CHTLCompileState::ERROR_STATE;
    }
    
    // 分析Token序列的模式
    std::string pattern;
    for (const auto& token : objectTokens) {
        pattern += token.GetValue() + " ";
    }
    
    // 基于语法文档进行状态推断
    for (const auto& rule : inferenceRules_) {
        if (pattern.find(rule.pattern) != std::string::npos) {
            if (!rule.validator || rule.validator(*context_)) {
                return rule.resultState;
            }
        }
    }
    
    return CHTLCompileState::INITIAL;
}

CHTLCompileState CHTLStateInferenceEngine::ResolveStateAmbiguity(const std::vector<CHTLToken>& ambiguousTokens) const {
    // 解决状态分辨问题
    // 结合前后文推断，确定确切的状态
    
    std::vector<CHTLCompileState> candidates;
    
    // 基于Token序列推断可能的状态
    for (const auto& token : ambiguousTokens) {
        if (token.GetType() == TokenType::AT_STYLE) {
            candidates.push_back(CHTLCompileState::PARSING_TEMPLATE_STYLE);
            candidates.push_back(CHTLCompileState::PARSING_CUSTOM_STYLE);
            candidates.push_back(CHTLCompileState::PARSING_ORIGIN_STYLE);
        } else if (token.GetType() == TokenType::AT_ELEMENT) {
            candidates.push_back(CHTLCompileState::PARSING_TEMPLATE_ELEMENT);
            candidates.push_back(CHTLCompileState::PARSING_CUSTOM_ELEMENT);
        } else if (token.GetType() == TokenType::AT_VAR) {
            candidates.push_back(CHTLCompileState::PARSING_TEMPLATE_VAR);
            candidates.push_back(CHTLCompileState::PARSING_CUSTOM_VAR);
        }
    }
    
    // 使用Context消歧
    return context_->DisambiguateState(candidates);
}

CHTLCompileState CHTLStateInferenceEngine::InferFromSyntaxPattern(const std::string& pattern) const {
    // 基于语法模式推断状态
    for (const auto& rule : inferenceRules_) {
        if (rule.pattern == pattern) {
            return rule.resultState;
        }
    }
    
    return CHTLCompileState::INITIAL;
}

CHTLCompileState CHTLStateInferenceEngine::InferFromNesting(int nestingLevel, CHTLCompileState parentState) const {
    // 基于嵌套层次推断状态
    // 考虑语法文档中的嵌套规则
    
    if (nestingLevel == 0) {
        return CHTLCompileState::INITIAL;
    }
    
    // 根据父状态和嵌套层次推断子状态
    switch (parentState) {
        case CHTLCompileState::PARSING_TEMPLATE_ELEMENT:
            return CHTLCompileState::PARSING_HTML_ELEMENT;
        case CHTLCompileState::PARSING_CUSTOM_ELEMENT:
            return CHTLCompileState::PARSING_CUSTOM_SPECIALIZATION;
        case CHTLCompileState::PARSING_NAMESPACE:
            if (nestingLevel > 1) {
                return CHTLCompileState::PARSING_NAMESPACE_NESTED;
            }
            break;
        default:
            break;
    }
    
    return CHTLCompileState::INITIAL;
}

void CHTLStateInferenceEngine::InitializeInferenceRules() {
    // 初始化推断规则（基于语法文档的完整语法模式）
    
    inferenceRules_ = {
        // 模板系统推断规则
        {"[Template] @Style", CHTLCompileState::PARSING_TEMPLATE_STYLE, 95, nullptr},
        {"[Template] @Element", CHTLCompileState::PARSING_TEMPLATE_ELEMENT, 95, nullptr},
        {"[Template] @Var", CHTLCompileState::PARSING_TEMPLATE_VAR, 95, nullptr},
        
        // 自定义系统推断规则
        {"[Custom] @Style", CHTLCompileState::PARSING_CUSTOM_STYLE, 95, nullptr},
        {"[Custom] @Element", CHTLCompileState::PARSING_CUSTOM_ELEMENT, 95, nullptr},
        {"[Custom] @Var", CHTLCompileState::PARSING_CUSTOM_VAR, 95, nullptr},
        
        // 原始嵌入推断规则
        {"[Origin] @Html", CHTLCompileState::PARSING_ORIGIN_HTML, 95, nullptr},
        {"[Origin] @Style", CHTLCompileState::PARSING_ORIGIN_STYLE, 95, nullptr},
        {"[Origin] @JavaScript", CHTLCompileState::PARSING_ORIGIN_JAVASCRIPT, 95, nullptr},
        
        // 配置组推断规则
        {"[Configuration]", CHTLCompileState::PARSING_CONFIGURATION, 90, nullptr},
        {"[Configuration] @Config", CHTLCompileState::PARSING_CONFIGURATION_NAMED, 95, nullptr},
        
        // 导入系统推断规则
        {"[Import] @Html", CHTLCompileState::PARSING_IMPORT_HTML, 95, nullptr},
        {"[Import] @Style", CHTLCompileState::PARSING_IMPORT_STYLE, 95, nullptr},
        {"[Import] @JavaScript", CHTLCompileState::PARSING_IMPORT_JAVASCRIPT, 95, nullptr},
        {"[Import] @Chtl", CHTLCompileState::PARSING_IMPORT_CHTL, 95, nullptr},
        {"[Import] @CJmod", CHTLCompileState::PARSING_IMPORT_CJMOD, 95, nullptr},
        
        // 命名空间推断规则
        {"[Namespace]", CHTLCompileState::PARSING_NAMESPACE, 90, nullptr},
        
        // 约束系统推断规则
        {"except", CHTLCompileState::PARSING_CONSTRAINT_PRECISE, 85, nullptr},
        
        // 基础语法推断规则
        {"text", CHTLCompileState::PARSING_TEXT_NODE, 80, nullptr},
        {"style", CHTLCompileState::PARSING_LOCAL_STYLE, 80, nullptr},
        {"script", CHTLCompileState::PARSING_LOCAL_SCRIPT, 80, nullptr}
    };
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "状态推断规则初始化完成，共 " + std::to_string(inferenceRules_.size()) + " 条规则"
    );
}

} // namespace Core
} // namespace CHTL