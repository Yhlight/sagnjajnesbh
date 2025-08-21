#include "chtl/helpers/CHTLStateContextHelper.h"
#include "utils/Logger.h"
#include <chrono>
#include <iomanip>
#include <sstream>

namespace chtl {
namespace compiler {
namespace helpers {

using namespace ast;

// CHTLStateContextHelper 实现

CHTLStateContextHelper::CHTLStateContextHelper(CHTLStateMachine& stateMachine, 
                                               CHTLContext& context)
    : m_StateMachine(stateMachine)
    , m_Context(context)
    , m_DebugMode(false) {
    InitializeDefaultRules();
}

CHTLStateContextHelper::~CHTLStateContextHelper() = default;

void CHTLStateContextHelper::InitializeDefaultRules() {
    // 元素相关转换规则
    RegisterTransitionRule({
        CHTLCompilerState::TOP_LEVEL,
        CHTLCompilerState::IN_ELEMENT,
        [](const ASTNodeStateInfo& info) { 
            return info.nodeType == ASTNodeType::ELEMENT; 
        },
        "顶层到元素"
    });
    
    RegisterTransitionRule({
        CHTLCompilerState::IN_ELEMENT,
        CHTLCompilerState::IN_ATTRIBUTE,
        [](const ASTNodeStateInfo& info) { 
            return info.nodeType == ASTNodeType::ATTRIBUTE; 
        },
        "元素到属性"
    });
    
    // 样式块转换规则
    RegisterTransitionRule({
        CHTLCompilerState::IN_ELEMENT_BODY,
        CHTLCompilerState::IN_LOCAL_STYLE,
        [](const ASTNodeStateInfo& info) { 
            return info.nodeType == ASTNodeType::STYLE_BLOCK; 
        },
        "元素体到局部样式"
    });
    
    // 脚本块转换规则
    RegisterTransitionRule({
        CHTLCompilerState::IN_ELEMENT_BODY,
        CHTLCompilerState::IN_LOCAL_SCRIPT,
        [](const ASTNodeStateInfo& info) { 
            return info.nodeType == ASTNodeType::SCRIPT_BLOCK; 
        },
        "元素体到局部脚本"
    });
    
    // 模板转换规则
    RegisterTransitionRule({
        CHTLCompilerState::TOP_LEVEL,
        CHTLCompilerState::IN_TEMPLATE_DECLARATION,
        [](const ASTNodeStateInfo& info) { 
            return info.nodeType == ASTNodeType::TEMPLATE_STYLE ||
                   info.nodeType == ASTNodeType::TEMPLATE_ELEMENT ||
                   info.nodeType == ASTNodeType::TEMPLATE_VAR; 
        },
        "顶层到模板声明"
    });
}

std::unique_ptr<CHTLStateContextHelper::ASTNodeStateGuard> 
CHTLStateContextHelper::CreateNodeGuard(ast::ASTNode* node,
                                       CHTLCompilerState state,
                                       CHTLScopeType scope) {
    return std::make_unique<ASTNodeStateGuard>(*this, node, state, scope);
}

bool CHTLStateContextHelper::CheckSyntaxConstraints(const ast::ASTNode* node) {
    if (!node) return false;
    
    auto it = m_NodeStates.find(node);
    if (it == m_NodeStates.end()) return true;
    
    const ASTNodeStateInfo& info = it->second;
    
    // 根据节点类型和状态检查约束
    switch (info.compilerState) {
        case CHTLCompilerState::IN_LOCAL_STYLE:
            return CheckStyleConstraints(node, info);
        case CHTLCompilerState::IN_LOCAL_SCRIPT:
            return CheckScriptConstraints(node, info);
        case CHTLCompilerState::IN_TEMPLATE_DECLARATION:
            return CheckTemplateConstraints(node, info);
        default:
            return CheckElementConstraints(node, info);
    }
}

ASTNodeStateInfo CHTLStateContextHelper::GetNodeConstraints(
    ast::ASTNodeType nodeType,
    CHTLCompilerState state,
    CHTLScopeType scope) {
    
    ASTNodeStateInfo constraints;
    constraints.nodeType = nodeType;
    constraints.compilerState = state;
    constraints.scopeType = scope;
    
    // 根据目标规划文档设置约束
    // 全局样式块约束（第137行）
    if (state == CHTLCompilerState::IN_LOCAL_STYLE || 
        state == CHTLCompilerState::IN_GLOBAL_STYLE) {
        constraints.allowsCHTLSyntax = true;
        constraints.allowsVariables = true;      // 模板变量、自定义变量
        constraints.allowsTemplates = true;      // 模板样式组、自定义样式组
        constraints.allowsCustomElements = false;
        constraints.allowsOriginEmbedding = true; // 原始嵌入任何地方都可以
    }
    // 局部脚本约束（第143行）
    else if (state == CHTLCompilerState::IN_LOCAL_SCRIPT) {
        constraints.allowsCHTLSyntax = true;
        constraints.allowsVariables = true;      // 模板变量、自定义变量组
        constraints.allowsTemplates = false;
        constraints.allowsCustomElements = false;
        constraints.allowsOriginEmbedding = true; // 原始嵌入和--注释
    }
    // 全局脚本约束（第139行）
    else if (state == CHTLCompilerState::IN_GLOBAL_SCRIPT) {
        constraints.allowsCHTLSyntax = false;    // 禁止CHTL语法
        constraints.allowsVariables = false;     // 禁止模板变量等
        constraints.allowsTemplates = false;
        constraints.allowsCustomElements = false;
        constraints.allowsOriginEmbedding = true; // 只允许--注释和原始嵌入
    }
    
    return constraints;
}

bool CHTLStateContextHelper::CanTransition(CHTLCompilerState from,
                                          CHTLCompilerState to,
                                          const ASTNodeStateInfo& nodeInfo) {
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

void CHTLStateContextHelper::RegisterTransitionRule(const StateTransitionRule& rule) {
    m_TransitionRules.push_back(rule);
}

CHTLStateContextHelper::ContextConstraints 
CHTLStateContextHelper::GetCurrentConstraints() const {
    ContextConstraints constraints;
    
    auto currentState = m_StateMachine.GetCurrentState();
    auto currentScope = m_Context.GetCurrentScope();
    
    // 根据当前状态设置约束
    switch (currentState) {
        case CHTLCompilerState::IN_LOCAL_STYLE:
        case CHTLCompilerState::IN_GLOBAL_STYLE:
            constraints.allowsCHTLSyntax = true;
            constraints.allowsVariables = true;
            constraints.allowsTemplates = true;
            constraints.allowsCustomElements = false;
            constraints.allowsOriginEmbedding = true;
            constraints.allowsComments = true;
            constraints.allowedNodeTypes = {
                ASTNodeType::STYLE_RULE,
                ASTNodeType::VAR_REFERENCE,
                ASTNodeType::TEMPLATE_STYLE,
                ASTNodeType::CUSTOM_STYLE,
                ASTNodeType::ORIGIN,
                ASTNodeType::COMMENT
            };
            break;
            
        case CHTLCompilerState::IN_LOCAL_SCRIPT:
            constraints.allowsCHTLSyntax = true;
            constraints.allowsVariables = true;
            constraints.allowsTemplates = false;
            constraints.allowsCustomElements = false;
            constraints.allowsOriginEmbedding = true;
            constraints.allowsComments = true;
            constraints.allowedNodeTypes = {
                ASTNodeType::SCRIPT_BLOCK,
                ASTNodeType::VAR_REFERENCE,
                ASTNodeType::ORIGIN,
                ASTNodeType::COMMENT
            };
            break;
            
        case CHTLCompilerState::IN_GLOBAL_SCRIPT:
            constraints.allowsCHTLSyntax = false;
            constraints.allowsVariables = false;
            constraints.allowsTemplates = false;
            constraints.allowsCustomElements = false;
            constraints.allowsOriginEmbedding = true;
            constraints.allowsComments = true;
            constraints.allowedNodeTypes = {
                ASTNodeType::SCRIPT_BLOCK,
                ASTNodeType::ORIGIN,
                ASTNodeType::COMMENT
            };
            break;
            
        default:
            // 默认允许所有
            constraints.allowsCHTLSyntax = true;
            constraints.allowsVariables = true;
            constraints.allowsTemplates = true;
            constraints.allowsCustomElements = true;
            constraints.allowsOriginEmbedding = true;
            constraints.allowsComments = true;
            break;
    }
    
    return constraints;
}

void CHTLStateContextHelper::CollectError(const std::string& error,
                                         const ASTNodeStateInfo& nodeInfo) {
    std::stringstream ss;
    ss << "[" << nodeInfo.filename << ":" << nodeInfo.startLine 
       << ":" << nodeInfo.startColumn << "] " << error;
    m_Errors.push_back(ss.str());
    
    if (m_DebugMode) {
        utils::Logger::GetInstance().Error(ss.str());
    }
}

void CHTLStateContextHelper::DumpNodeState(const ast::ASTNode* node) const {
    if (!node) return;
    
    auto it = m_NodeStates.find(node);
    if (it == m_NodeStates.end()) {
        utils::Logger::GetInstance().Debug("节点状态未找到");
        return;
    }
    
    const ASTNodeStateInfo& info = it->second;
    std::stringstream ss;
    ss << "节点状态:\n"
       << "  类型: " << static_cast<int>(info.nodeType) << "\n"
       << "  编译状态: " << static_cast<int>(info.compilerState) << "\n"
       << "  作用域: " << static_cast<int>(info.scopeType) << "\n"
       << "  标识符: " << info.identifier << "\n"
       << "  位置: " << info.filename << ":" << info.startLine << ":" << info.startColumn << "\n"
       << "  约束:\n"
       << "    允许CHTL语法: " << info.allowsCHTLSyntax << "\n"
       << "    允许变量: " << info.allowsVariables << "\n"
       << "    允许模板: " << info.allowsTemplates << "\n"
       << "    允许自定义元素: " << info.allowsCustomElements << "\n"
       << "    允许原始嵌入: " << info.allowsOriginEmbedding << "\n"
       << "  验证状态: " << (info.isValidated ? "已验证" : "未验证") << "\n"
       << "  错误: " << (info.hasErrors ? "有" : "无") << "\n";
    
    utils::Logger::GetInstance().Debug(ss.str());
}

void CHTLStateContextHelper::DumpStateHistory() const {
    std::stringstream ss;
    ss << "状态历史 (共" << m_StateHistory.size() << "条):\n";
    
    for (const auto& entry : m_StateHistory) {
        ss << "  [" << entry.timestamp << "] "
           << "状态=" << static_cast<int>(entry.state) << " "
           << "作用域=" << static_cast<int>(entry.scope) << " "
           << "上下文=" << entry.context << " "
           << "节点=" << entry.nodeIdentifier << "\n";
    }
    
    utils::Logger::GetInstance().Debug(ss.str());
}

bool CHTLStateContextHelper::CheckElementConstraints(const ast::ASTNode* node,
                                                    const ASTNodeStateInfo& info) {
    // 元素内基本都允许CHTL语法
    return true;
}

bool CHTLStateContextHelper::CheckStyleConstraints(const ast::ASTNode* node,
                                                  const ASTNodeStateInfo& info) {
    // 检查样式块内的约束
    if (!info.allowsCHTLSyntax && node->GetType() != ASTNodeType::ORIGIN &&
        node->GetType() != ASTNodeType::COMMENT) {
        const_cast<ASTNodeStateInfo&>(info).hasErrors = true;
        const_cast<ASTNodeStateInfo&>(info).errors.push_back(
            "样式块内不允许此CHTL语法");
        return false;
    }
    return true;
}

bool CHTLStateContextHelper::CheckScriptConstraints(const ast::ASTNode* node,
                                                   const ASTNodeStateInfo& info) {
    // 检查脚本块内的约束
    bool isGlobalScript = info.compilerState == CHTLCompilerState::IN_GLOBAL_SCRIPT;
    
    if (isGlobalScript && info.allowsCHTLSyntax == false) {
        // 全局脚本只允许--注释和原始嵌入
        if (node->GetType() != ASTNodeType::ORIGIN &&
            node->GetType() != ASTNodeType::COMMENT) {
            const_cast<ASTNodeStateInfo&>(info).hasErrors = true;
            const_cast<ASTNodeStateInfo&>(info).errors.push_back(
                "全局脚本块内禁止使用CHTL语法");
            return false;
        }
    }
    return true;
}

bool CHTLStateContextHelper::CheckTemplateConstraints(const ast::ASTNode* node,
                                                     const ASTNodeStateInfo& info) {
    // 模板内的约束检查
    return true;
}

void CHTLStateContextHelper::RecordStateChange(const StateHistoryEntry& entry) {
    if (m_DebugMode) {
        m_StateHistory.push_back(entry);
        
        // 限制历史记录大小
        if (m_StateHistory.size() > 1000) {
            m_StateHistory.erase(m_StateHistory.begin());
        }
    }
}

std::string CHTLStateContextHelper::GetTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// ASTNodeStateGuard 实现

CHTLStateContextHelper::ASTNodeStateGuard::ASTNodeStateGuard(
    CHTLStateContextHelper& helper,
    ast::ASTNode* node,
    CHTLCompilerState state,
    CHTLScopeType scope)
    : m_Helper(helper)
    , m_Node(node)
    , m_Active(true) {
    
    // 初始化节点状态
    m_NodeState.nodeType = node ? node->GetType() : ASTNodeType::UNKNOWN;
    m_NodeState.compilerState = state;
    m_NodeState.scopeType = scope;
    
    // 获取位置信息
    if (node) {
        m_NodeState.startLine = node->GetStartLine();
        m_NodeState.startColumn = node->GetStartColumn();
        m_NodeState.endLine = node->GetEndLine();
        m_NodeState.endColumn = node->GetEndColumn();
    }
    
    // 获取约束信息
    auto constraints = m_Helper.GetNodeConstraints(
        m_NodeState.nodeType, state, scope);
    m_NodeState.allowsCHTLSyntax = constraints.allowsCHTLSyntax;
    m_NodeState.allowsVariables = constraints.allowsVariables;
    m_NodeState.allowsTemplates = constraints.allowsTemplates;
    m_NodeState.allowsCustomElements = constraints.allowsCustomElements;
    m_NodeState.allowsOriginEmbedding = constraints.allowsOriginEmbedding;
    
    // 创建状态和作用域守卫
    m_StateGuard = std::make_unique<CHTLStateGuard>(
        m_Helper.m_StateMachine, state, m_NodeState.identifier);
    m_ScopeGuard = std::make_unique<CHTLScopeGuard>(
        m_Helper.m_Context, scope, m_NodeState.identifier);
    
    // 记录节点状态
    if (node) {
        m_Helper.m_NodeStates[node] = m_NodeState;
    }
    
    // 记录状态变化
    StateHistoryEntry entry;
    entry.timestamp = m_Helper.GetTimestamp();
    entry.state = state;
    entry.scope = scope;
    entry.context = "进入";
    entry.nodeIdentifier = m_NodeState.identifier;
    m_Helper.RecordStateChange(entry);
}

CHTLStateContextHelper::ASTNodeStateGuard::~ASTNodeStateGuard() {
    if (m_Active && m_Node) {
        // 记录状态变化
        StateHistoryEntry entry;
        entry.timestamp = m_Helper.GetTimestamp();
        entry.state = m_NodeState.compilerState;
        entry.scope = m_NodeState.scopeType;
        entry.context = "退出";
        entry.nodeIdentifier = m_NodeState.identifier;
        m_Helper.RecordStateChange(entry);
        
        // 自动验证（如果还未验证）
        if (!m_NodeState.isValidated) {
            Validate();
        }
    }
}

CHTLStateContextHelper::ASTNodeStateGuard::ASTNodeStateGuard(
    ASTNodeStateGuard&& other) noexcept
    : m_Helper(other.m_Helper)
    , m_Node(other.m_Node)
    , m_NodeState(std::move(other.m_NodeState))
    , m_Active(other.m_Active)
    , m_StateGuard(std::move(other.m_StateGuard))
    , m_ScopeGuard(std::move(other.m_ScopeGuard)) {
    other.m_Active = false;
}

void CHTLStateContextHelper::ASTNodeStateGuard::UpdateNodeInfo(
    const std::function<void(ASTNodeStateInfo&)>& updater) {
    updater(m_NodeState);
    
    // 更新映射中的状态
    if (m_Node) {
        m_Helper.m_NodeStates[m_Node] = m_NodeState;
    }
}

bool CHTLStateContextHelper::ASTNodeStateGuard::Validate() {
    if (!m_Node) return false;
    
    m_NodeState.isValidated = true;
    
    // 执行约束检查
    bool valid = m_Helper.CheckSyntaxConstraints(m_Node);
    
    if (!valid) {
        m_NodeState.hasErrors = true;
    }
    
    // 更新映射
    m_Helper.m_NodeStates[m_Node] = m_NodeState;
    
    return valid;
}

void CHTLStateContextHelper::ASTNodeStateGuard::MarkError(const std::string& error) {
    m_NodeState.hasErrors = true;
    m_NodeState.errors.push_back(error);
    
    m_Helper.CollectError(error, m_NodeState);
    
    // 更新映射
    if (m_Node) {
        m_Helper.m_NodeStates[m_Node] = m_NodeState;
    }
}

// ParserStateHelper 实现

ParserStateHelper::ParserStateHelper(CHTLStateContextHelper& helper)
    : m_Helper(helper) {}

std::unique_ptr<ParserStateHelper::ElementParseGuard>
ParserStateHelper::ParseElement(const std::string& elementName) {
    return std::make_unique<ElementParseGuard>(*this, elementName);
}

void ParserStateHelper::EnterTemplate(const std::string& templateType) {
    CHTLCompilerState state;
    if (templateType == "Style") {
        state = CHTLCompilerState::IN_TEMPLATE_STYLE;
    } else if (templateType == "Element") {
        state = CHTLCompilerState::IN_TEMPLATE_ELEMENT;
    } else if (templateType == "Var") {
        state = CHTLCompilerState::IN_TEMPLATE_VAR;
    } else {
        state = CHTLCompilerState::IN_TEMPLATE_DECLARATION;
    }
    
    m_Helper.m_StateMachine.PushState(state, templateType);
    m_Helper.m_Context.PushScope(CHTLScopeType::TEMPLATE, templateType);
}

void ParserStateHelper::ExitTemplate() {
    m_Helper.m_StateMachine.PopState();
    m_Helper.m_Context.PopScope();
}

void ParserStateHelper::EnterStyleBlock(bool isLocal) {
    auto state = isLocal ? CHTLCompilerState::IN_LOCAL_STYLE 
                        : CHTLCompilerState::IN_GLOBAL_STYLE;
    m_Helper.m_StateMachine.PushState(state);
    m_Helper.m_Context.EnterStyleBlock(isLocal);
}

void ParserStateHelper::ExitStyleBlock() {
    m_Helper.m_StateMachine.PopState();
    m_Helper.m_Context.ExitStyleBlock();
}

void ParserStateHelper::EnterScriptBlock(bool isLocal) {
    auto state = isLocal ? CHTLCompilerState::IN_LOCAL_SCRIPT 
                        : CHTLCompilerState::IN_GLOBAL_SCRIPT;
    m_Helper.m_StateMachine.PushState(state);
    m_Helper.m_Context.EnterScriptBlock(isLocal);
}

void ParserStateHelper::ExitScriptBlock() {
    m_Helper.m_StateMachine.PopState();
    m_Helper.m_Context.ExitScriptBlock();
}

// ElementParseGuard 实现

ParserStateHelper::ElementParseGuard::ElementParseGuard(
    ParserStateHelper& helper,
    const std::string& elementName)
    : m_Helper(helper)
    , m_ElementName(elementName)
    , m_Complete(false) {
    
    m_Helper.m_Helper.m_StateMachine.PushState(
        CHTLCompilerState::IN_ELEMENT, elementName);
    m_Helper.m_Helper.m_Context.SetCurrentElement(elementName);
}

ParserStateHelper::ElementParseGuard::~ElementParseGuard() {
    if (!m_Complete) {
        m_Helper.m_Helper.m_StateMachine.PopState();
    }
}

void ParserStateHelper::ElementParseGuard::EnterAttribute(const std::string& attrName) {
    m_Helper.m_Helper.m_StateMachine.PushState(
        CHTLCompilerState::IN_ATTRIBUTE, attrName);
}

void ParserStateHelper::ElementParseGuard::ExitAttribute() {
    m_Helper.m_Helper.m_StateMachine.PopState();
}

void ParserStateHelper::ElementParseGuard::EnterBody() {
    m_Helper.m_Helper.m_StateMachine.PushState(
        CHTLCompilerState::IN_ELEMENT_BODY);
}

void ParserStateHelper::ElementParseGuard::MarkComplete() {
    m_Complete = true;
    // 弹出所有相关状态
    auto currentState = m_Helper.m_Helper.m_StateMachine.GetCurrentState();
    if (currentState == CHTLCompilerState::IN_ELEMENT_BODY) {
        m_Helper.m_Helper.m_StateMachine.PopState(); // 退出body
    }
    if (m_Helper.m_Helper.m_StateMachine.GetCurrentState() == 
        CHTLCompilerState::IN_ELEMENT) {
        m_Helper.m_Helper.m_StateMachine.PopState(); // 退出element
    }
}

// GeneratorStateHelper 实现

GeneratorStateHelper::GeneratorStateHelper(CHTLStateContextHelper& helper)
    : m_Helper(helper) {
    m_OutputTypeStack.push_back("html"); // 默认输出类型
}

std::unique_ptr<GeneratorStateHelper::GenerationGuard>
GeneratorStateHelper::BeginGeneration(ast::ASTNode* node,
                                     const std::string& outputType) {
    return std::make_unique<GenerationGuard>(*this, node, outputType);
}

void GeneratorStateHelper::PushOutputType(const std::string& type) {
    m_OutputTypeStack.push_back(type);
}

void GeneratorStateHelper::PopOutputType() {
    if (m_OutputTypeStack.size() > 1) {
        m_OutputTypeStack.pop_back();
    }
}

std::string GeneratorStateHelper::GetCurrentOutputType() const {
    return m_OutputTypeStack.empty() ? "html" : m_OutputTypeStack.back();
}

// GenerationGuard 实现

GeneratorStateHelper::GenerationGuard::GenerationGuard(
    GeneratorStateHelper& helper,
    ast::ASTNode* node,
    const std::string& outputType)
    : m_Helper(helper)
    , m_Node(node)
    , m_OutputType(outputType)
    , m_Generated(false) {
    
    m_Helper.PushOutputType(outputType);
}

GeneratorStateHelper::GenerationGuard::~GenerationGuard() {
    m_Helper.PopOutputType();
}

bool GeneratorStateHelper::GenerationGuard::ShouldGenerate() const {
    if (!m_Node) return false;
    
    // 检查节点是否应该生成
    auto it = m_Helper.m_Helper.m_NodeStates.find(m_Node);
    if (it != m_Helper.m_Helper.m_NodeStates.end()) {
        return !it->second.hasErrors;
    }
    
    return true;
}

void GeneratorStateHelper::GenerationGuard::MarkGenerated() {
    m_Generated = true;
}

} // namespace helpers
} // namespace compiler
} // namespace chtl