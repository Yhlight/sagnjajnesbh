#include "chtl_js/helpers/CHTLJSStateContextHelper.h"
#include "utils/Logger.h"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <unordered_set>

namespace chtl {
namespace chtljs {
namespace helpers {

using namespace ast;
using namespace compiler;

// CHTLJSStateContextHelper 实现

CHTLJSStateContextHelper::CHTLJSStateContextHelper(
    CHTLJSStateMachine& stateMachine,
    CHTLJSContext& context)
    : m_StateMachine(stateMachine)
    , m_Context(context)
    , m_DebugMode(false) {
    InitializeDefaultRules();
}

CHTLJSStateContextHelper::~CHTLJSStateContextHelper() = default;

void CHTLJSStateContextHelper::InitializeDefaultRules() {
    // 增强选择器转换规则
    RegisterTransitionRule({
        CHTLJSStateType::SCRIPT,
        CHTLJSStateType::ENHANCED_SELECTOR,
        [](const CHTLJSNodeStateInfo& info) {
            return info.nodeType == CHTLJSNodeType::ENHANCED_SELECTOR;
        },
        "脚本到增强选择器"
    });
    
    // 虚对象声明转换规则
    RegisterTransitionRule({
        CHTLJSStateType::SCRIPT,
        CHTLJSStateType::VIR_DECLARATION,
        [](const CHTLJSNodeStateInfo& info) {
            return info.nodeType == CHTLJSNodeType::VIR_DECLARATION;
        },
        "脚本到虚对象声明"
    });
    
    // CHTL JS函数转换规则
    RegisterTransitionRule({
        CHTLJSStateType::SCRIPT,
        CHTLJSStateType::CHTLJS_FUNCTION,
        [](const CHTLJSNodeStateInfo& info) {
            return info.nodeType == CHTLJSNodeType::LISTEN_CALL ||
                   info.nodeType == CHTLJSNodeType::DELEGATE_CALL ||
                   info.nodeType == CHTLJSNodeType::ANIMATE_CALL ||
                   info.nodeType == CHTLJSNodeType::I_NEVER_AWAY_CALL ||
                   info.nodeType == CHTLJSNodeType::PRINT_MY_LOVE_CALL;
        },
        "脚本到CHTL JS函数"
    });
    
    // JavaScript片段转换规则
    RegisterTransitionRule({
        CHTLJSStateType::SCRIPT,
        CHTLJSStateType::JS_FRAGMENT,
        [](const CHTLJSNodeStateInfo& info) {
            return info.nodeType == CHTLJSNodeType::JS_CODE_FRAGMENT;
        },
        "脚本到JS片段"
    });
}

std::unique_ptr<CHTLJSStateContextHelper::CHTLJSNodeStateGuard>
CHTLJSStateContextHelper::CreateNodeGuard(ast::CHTLJSASTNode* node,
                                         CHTLJSStateType state) {
    return std::make_unique<CHTLJSNodeStateGuard>(*this, node, state);
}

bool CHTLJSStateContextHelper::CheckCHTLJSSyntaxConstraints(const ast::CHTLJSASTNode* node) {
    if (!node) return false;
    
    auto it = m_NodeStates.find(node);
    if (it == m_NodeStates.end()) return true;
    
    const CHTLJSNodeStateInfo& info = it->second;
    
    // 根据节点类型检查约束
    switch (node->GetType()) {
        case CHTLJSNodeType::ENHANCED_SELECTOR:
            return CheckEnhancedSelectorConstraints(node, info);
            
        case CHTLJSNodeType::VIR_DECLARATION:
        case CHTLJSNodeType::VIR_ACCESS:
            return CheckVirConstraints(node, info);
            
        case CHTLJSNodeType::LISTEN_CALL:
        case CHTLJSNodeType::DELEGATE_CALL:
        case CHTLJSNodeType::ANIMATE_CALL:
        case CHTLJSNodeType::I_NEVER_AWAY_CALL:
        case CHTLJSNodeType::PRINT_MY_LOVE_CALL:
            return CheckCHTLJSFunctionConstraints(node, info);
            
        default:
            return true;
    }
}

CHTLJSNodeStateInfo CHTLJSStateContextHelper::GetNodeConstraints(
    CHTLJSNodeType nodeType,
    CHTLJSStateType state) {
    
    CHTLJSNodeStateInfo constraints;
    constraints.nodeType = nodeType;
    constraints.stateType = state;
    
    // CHTL JS在局部脚本中的约束
    // 根据CHTL语法文档，CHTL JS总是在局部脚本块中
    constraints.isInLocalScript = true;
    constraints.allowsEnhancedSelector = true;
    constraints.allowsArrowOperator = true;
    constraints.allowsVirDeclaration = true;
    constraints.allowsCHTLJSFunctions = true;
    
    return constraints;
}

bool CHTLJSStateContextHelper::CanTransition(CHTLJSStateType from,
                                            CHTLJSStateType to,
                                            const CHTLJSNodeStateInfo& nodeInfo) {
    // 检查注册的转换规则
    for (const auto& rule : m_TransitionRules) {
        if (rule.fromState == from && rule.toState == to) {
            if (rule.condition(nodeInfo)) {
                return true;
            }
        }
    }
    
    // 检查状态机的基本转换规则
    return m_StateMachine.CanTransitionTo(to);
}

void CHTLJSStateContextHelper::RegisterTransitionRule(const CHTLJSStateTransitionRule& rule) {
    m_TransitionRules.push_back(rule);
}

CHTLJSStateContextHelper::CHTLJSContextConstraints
CHTLJSStateContextHelper::GetCurrentConstraints() const {
    CHTLJSContextConstraints constraints;
    
    auto currentState = m_StateMachine.GetCurrentState();
    
    // CHTL JS总是在局部脚本环境中
    constraints.allowsEnhancedSelector = true;
    constraints.allowsArrowOperator = true;
    constraints.allowsVirDeclaration = true;
    constraints.allowsCHTLJSFunctions = true;
    constraints.allowsJavaScriptCode = true;
    
    // 根据当前状态设置允许的节点类型
    switch (currentState) {
        case CHTLJSStateType::SCRIPT:
            constraints.allowedNodeTypes = {
                CHTLJSNodeType::ENHANCED_SELECTOR,
                CHTLJSNodeType::ARROW_OPERATOR,
                CHTLJSNodeType::VIR_DECLARATION,
                CHTLJSNodeType::VIR_ACCESS,
                CHTLJSNodeType::LISTEN_CALL,
                CHTLJSNodeType::DELEGATE_CALL,
                CHTLJSNodeType::ANIMATE_CALL,
                CHTLJSNodeType::I_NEVER_AWAY_CALL,
                CHTLJSNodeType::PRINT_MY_LOVE_CALL,
                CHTLJSNodeType::JS_CODE_FRAGMENT,
                CHTLJSNodeType::IDENTIFIER,
                CHTLJSNodeType::STRING_LITERAL,
                CHTLJSNodeType::NUMBER_LITERAL,
                CHTLJSNodeType::BOOLEAN_LITERAL,
                CHTLJSNodeType::OBJECT_LITERAL,
                CHTLJSNodeType::ARRAY_LITERAL
            };
            break;
            
        case CHTLJSStateType::ENHANCED_SELECTOR:
            constraints.allowedNodeTypes = {
                CHTLJSNodeType::ARROW_OPERATOR,
                CHTLJSNodeType::DOT_OPERATOR
            };
            break;
            
        case CHTLJSStateType::VIR_DECLARATION:
            constraints.allowedNodeTypes = {
                CHTLJSNodeType::LISTEN_CALL,
                CHTLJSNodeType::I_NEVER_AWAY_CALL
            };
            break;
            
        default:
            // 默认允许所有CHTL JS节点类型
            break;
    }
    
    return constraints;
}

void CHTLJSStateContextHelper::RegisterVirObject(const std::string& name,
                                                const std::vector<std::string>& functionKeys) {
    m_VirObjects[name] = functionKeys;
    m_Context.AddVirtualObject(name);
}

bool CHTLJSStateContextHelper::IsVirObject(const std::string& name) const {
    return m_VirObjects.find(name) != m_VirObjects.end();
}

std::vector<std::string> CHTLJSStateContextHelper::GetVirFunctionKeys(
    const std::string& name) const {
    auto it = m_VirObjects.find(name);
    if (it != m_VirObjects.end()) {
        return it->second;
    }
    return {};
}

void CHTLJSStateContextHelper::RegisterSelector(const std::string& selector) {
    m_UsedSelectors.push_back(selector);
    m_Context.AddEnhancedSelector(selector);
}

std::vector<std::string> CHTLJSStateContextHelper::GetUsedSelectors() const {
    return m_UsedSelectors;
}

void CHTLJSStateContextHelper::CollectError(const std::string& error,
                                           const CHTLJSNodeStateInfo& nodeInfo) {
    std::stringstream ss;
    ss << "[CHTL JS " << nodeInfo.filename << ":" << nodeInfo.startLine 
       << ":" << nodeInfo.startColumn << "] " << error;
    m_Errors.push_back(ss.str());
    
    if (m_DebugMode) {
        utils::Logger::GetInstance().Error(ss.str());
    }
}

void CHTLJSStateContextHelper::DumpNodeState(const ast::CHTLJSASTNode* node) const {
    if (!node) return;
    
    auto it = m_NodeStates.find(node);
    if (it == m_NodeStates.end()) {
        utils::Logger::GetInstance().Debug("CHTL JS节点状态未找到");
        return;
    }
    
    const CHTLJSNodeStateInfo& info = it->second;
    std::stringstream ss;
    ss << "CHTL JS节点状态:\n"
       << "  类型: " << static_cast<int>(info.nodeType) << "\n"
       << "  状态类型: " << static_cast<int>(info.stateType) << "\n"
       << "  标识符: " << info.identifier << "\n"
       << "  位置: " << info.filename << ":" << info.startLine << ":" << info.startColumn << "\n"
       << "  约束:\n"
       << "    局部脚本中: " << info.isInLocalScript << "\n"
       << "    允许增强选择器: " << info.allowsEnhancedSelector << "\n"
       << "    允许箭头操作符: " << info.allowsArrowOperator << "\n"
       << "    允许虚对象声明: " << info.allowsVirDeclaration << "\n"
       << "    允许CHTL JS函数: " << info.allowsCHTLJSFunctions << "\n"
       << "  验证状态: " << (info.isValidated ? "已验证" : "未验证") << "\n"
       << "  错误: " << (info.hasErrors ? "有" : "无") << "\n";
    
    utils::Logger::GetInstance().Debug(ss.str());
}

void CHTLJSStateContextHelper::DumpStateHistory() const {
    std::stringstream ss;
    ss << "CHTL JS状态历史 (共" << m_StateHistory.size() << "条):\n";
    
    for (const auto& entry : m_StateHistory) {
        ss << "  [" << entry.timestamp << "] "
           << "状态=" << static_cast<int>(entry.state) << " "
           << "上下文=" << entry.context << " "
           << "节点=" << entry.nodeIdentifier << "\n";
    }
    
    utils::Logger::GetInstance().Debug(ss.str());
}

bool CHTLJSStateContextHelper::CheckEnhancedSelectorConstraints(
    const ast::CHTLJSASTNode* node,
    const CHTLJSNodeStateInfo& info) {
    // 增强选择器总是允许的
    return info.allowsEnhancedSelector;
}

bool CHTLJSStateContextHelper::CheckVirConstraints(
    const ast::CHTLJSASTNode* node,
    const CHTLJSNodeStateInfo& info) {
    // 虚对象在CHTL JS中总是允许的
    return info.allowsVirDeclaration;
}

bool CHTLJSStateContextHelper::CheckCHTLJSFunctionConstraints(
    const ast::CHTLJSASTNode* node,
    const CHTLJSNodeStateInfo& info) {
    // CHTL JS函数在局部脚本中总是允许的
    return info.allowsCHTLJSFunctions && info.isInLocalScript;
}

void CHTLJSStateContextHelper::RecordStateChange(const StateHistoryEntry& entry) {
    if (m_DebugMode) {
        m_StateHistory.push_back(entry);
        
        // 限制历史记录大小
        if (m_StateHistory.size() > 1000) {
            m_StateHistory.erase(m_StateHistory.begin());
        }
    }
}

std::string CHTLJSStateContextHelper::GetTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// CHTLJSNodeStateGuard 实现

CHTLJSStateContextHelper::CHTLJSNodeStateGuard::CHTLJSNodeStateGuard(
    CHTLJSStateContextHelper& helper,
    ast::CHTLJSASTNode* node,
    CHTLJSStateType state)
    : m_Helper(helper)
    , m_Node(node)
    , m_Active(true) {
    
    // 初始化节点状态
    m_NodeState.nodeType = node ? node->GetType() : CHTLJSNodeType::UNKNOWN;
    m_NodeState.stateType = state;
    
    // 获取位置信息
    if (node) {
        m_NodeState.startLine = node->GetStartLine();
        m_NodeState.startColumn = node->GetStartColumn();
        m_NodeState.endLine = node->GetEndLine();
        m_NodeState.endColumn = node->GetEndColumn();
    }
    
    // 获取约束信息
    auto constraints = m_Helper.GetNodeConstraints(m_NodeState.nodeType, state);
    m_NodeState.isInLocalScript = constraints.isInLocalScript;
    m_NodeState.allowsEnhancedSelector = constraints.allowsEnhancedSelector;
    m_NodeState.allowsArrowOperator = constraints.allowsArrowOperator;
    m_NodeState.allowsVirDeclaration = constraints.allowsVirDeclaration;
    m_NodeState.allowsCHTLJSFunctions = constraints.allowsCHTLJSFunctions;
    
    // 创建状态守卫
    m_StateGuard = std::make_unique<CHTLJSStateGuard>(
        m_Helper.m_StateMachine, state, m_NodeState.identifier);
    
    // 记录节点状态
    if (node) {
        m_Helper.m_NodeStates[node] = m_NodeState;
    }
    
    // 记录状态变化
    StateHistoryEntry entry;
    entry.timestamp = m_Helper.GetTimestamp();
    entry.state = state;
    entry.context = "进入";
    entry.nodeIdentifier = m_NodeState.identifier;
    m_Helper.RecordStateChange(entry);
}

CHTLJSStateContextHelper::CHTLJSNodeStateGuard::~CHTLJSNodeStateGuard() {
    if (m_Active && m_Node) {
        // 记录状态变化
        StateHistoryEntry entry;
        entry.timestamp = m_Helper.GetTimestamp();
        entry.state = m_NodeState.stateType;
        entry.context = "退出";
        entry.nodeIdentifier = m_NodeState.identifier;
        m_Helper.RecordStateChange(entry);
        
        // 自动验证（如果还未验证）
        if (!m_NodeState.isValidated) {
            Validate();
        }
    }
}

CHTLJSStateContextHelper::CHTLJSNodeStateGuard::CHTLJSNodeStateGuard(
    CHTLJSNodeStateGuard&& other) noexcept
    : m_Helper(other.m_Helper)
    , m_Node(other.m_Node)
    , m_NodeState(std::move(other.m_NodeState))
    , m_Active(other.m_Active)
    , m_StateGuard(std::move(other.m_StateGuard)) {
    other.m_Active = false;
}

void CHTLJSStateContextHelper::CHTLJSNodeStateGuard::UpdateNodeInfo(
    const std::function<void(CHTLJSNodeStateInfo&)>& updater) {
    updater(m_NodeState);
    
    // 更新映射中的状态
    if (m_Node) {
        m_Helper.m_NodeStates[m_Node] = m_NodeState;
    }
}

bool CHTLJSStateContextHelper::CHTLJSNodeStateGuard::Validate() {
    if (!m_Node) return false;
    
    m_NodeState.isValidated = true;
    
    // 执行约束检查
    bool valid = m_Helper.CheckCHTLJSSyntaxConstraints(m_Node);
    
    if (!valid) {
        m_NodeState.hasErrors = true;
    }
    
    // 更新映射
    m_Helper.m_NodeStates[m_Node] = m_NodeState;
    
    return valid;
}

void CHTLJSStateContextHelper::CHTLJSNodeStateGuard::MarkError(const std::string& error) {
    m_NodeState.hasErrors = true;
    m_NodeState.errors.push_back(error);
    
    m_Helper.CollectError(error, m_NodeState);
    
    // 更新映射
    if (m_Node) {
        m_Helper.m_NodeStates[m_Node] = m_NodeState;
    }
}

// CHTLJSParserStateHelper 实现

CHTLJSParserStateHelper::CHTLJSParserStateHelper(CHTLJSStateContextHelper& helper)
    : m_Helper(helper) {}

void CHTLJSParserStateHelper::EnterEnhancedSelector() {
    m_Helper.m_StateMachine.PushState(CHTLJSStateType::ENHANCED_SELECTOR);
}

void CHTLJSParserStateHelper::ExitEnhancedSelector() {
    m_Helper.m_StateMachine.PopState();
}

void CHTLJSParserStateHelper::EnterVirDeclaration(const std::string& virName) {
    m_Helper.m_StateMachine.PushState(CHTLJSStateType::VIR_DECLARATION, virName);
}

void CHTLJSParserStateHelper::ExitVirDeclaration() {
    m_Helper.m_StateMachine.PopState();
}

void CHTLJSParserStateHelper::EnterCHTLJSFunction(const std::string& functionName) {
    m_Helper.m_StateMachine.PushState(CHTLJSStateType::CHTLJS_FUNCTION, functionName);
}

void CHTLJSParserStateHelper::ExitCHTLJSFunction() {
    m_Helper.m_StateMachine.PopState();
}

void CHTLJSParserStateHelper::EnterJSFragment() {
    m_Helper.m_StateMachine.PushState(CHTLJSStateType::JS_FRAGMENT);
}

void CHTLJSParserStateHelper::ExitJSFragment() {
    m_Helper.m_StateMachine.PopState();
}

// CHTLJSGeneratorStateHelper 实现

CHTLJSGeneratorStateHelper::CHTLJSGeneratorStateHelper(CHTLJSStateContextHelper& helper)
    : m_Helper(helper) {}

std::unique_ptr<CHTLJSGeneratorStateHelper::GenerationGuard>
CHTLJSGeneratorStateHelper::BeginGeneration(ast::CHTLJSASTNode* node,
                                           const std::string& outputType) {
    return std::make_unique<GenerationGuard>(*this, node, outputType);
}

void CHTLJSGeneratorStateHelper::RegisterRuntimeFunction(const std::string& name) {
    m_RuntimeFunctions.insert(name);
}

bool CHTLJSGeneratorStateHelper::IsRuntimeFunctionRegistered(const std::string& name) const {
    return m_RuntimeFunctions.find(name) != m_RuntimeFunctions.end();
}

std::vector<std::string> CHTLJSGeneratorStateHelper::GetRuntimeFunctions() const {
    return std::vector<std::string>(m_RuntimeFunctions.begin(), m_RuntimeFunctions.end());
}

// GenerationGuard 实现

CHTLJSGeneratorStateHelper::GenerationGuard::GenerationGuard(
    CHTLJSGeneratorStateHelper& helper,
    ast::CHTLJSASTNode* node,
    const std::string& outputType)
    : m_Helper(helper)
    , m_Node(node)
    , m_OutputType(outputType)
    , m_Generated(false) {}

CHTLJSGeneratorStateHelper::GenerationGuard::~GenerationGuard() = default;

bool CHTLJSGeneratorStateHelper::GenerationGuard::ShouldGenerate() const {
    if (!m_Node) return false;
    
    // 检查节点是否应该生成
    auto it = m_Helper.m_Helper.m_NodeStates.find(m_Node);
    if (it != m_Helper.m_Helper.m_NodeStates.end()) {
        return !it->second.hasErrors;
    }
    
    return true;
}

void CHTLJSGeneratorStateHelper::GenerationGuard::MarkGenerated() {
    m_Generated = true;
}

std::string CHTLJSGeneratorStateHelper::GenerationGuard::GenerateVirFunctionName(
    const std::string& virName,
    const std::string& functionKey) {
    return "__chtljs_vir_" + virName + "_" + functionKey;
}

} // namespace helpers
} // namespace chtljs
} // namespace chtl