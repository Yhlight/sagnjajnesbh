#include "chtl/constraint/ConstraintSystem.h"
#include "chtl/ast/ASTNode.h"
#include "chtl/ast/StyleBlockNode.h"
#include "chtl/ast/ScriptBlockNode.h"
#include "chtl/ast/TemplateNode.h"
#include "chtl/ast/CustomNode.h"
#include "chtl/ast/OriginNode.h"
#include "chtl/ast/SpecializationNodes.h"
#include "chtl/ast/CommentNode.h"
#include "utils/Logger.h"
#include <sstream>

namespace chtl {
namespace constraint {

// ConstraintRule 实现

ConstraintRule::ConstraintRule(ContextType context)
    : m_Context(context)
    , m_UseWhitelist(true) {}  // 默认使用白名单模式

ConstraintRule::~ConstraintRule() = default;

void ConstraintRule::AddAllowedElement(SyntaxElement element) {
    m_AllowedElements.insert(element);
    m_UseWhitelist = true;
}

void ConstraintRule::AddForbiddenElement(SyntaxElement element) {
    m_ForbiddenElements.insert(element);
    m_UseWhitelist = false;
}

bool ConstraintRule::IsAllowed(SyntaxElement element) const {
    // 特殊元素总是允许的（注释和原始嵌入）
    if (ConstraintUtils::IsUniversallyAllowed(element)) {
        return true;
    }
    
    if (m_UseWhitelist) {
        // 白名单模式：只有在允许列表中的元素才能使用
        return m_AllowedElements.find(element) != m_AllowedElements.end();
    } else {
        // 黑名单模式：不在禁止列表中的元素都可以使用
        return m_ForbiddenElements.find(element) == m_ForbiddenElements.end();
    }
}

std::vector<SyntaxElement> ConstraintRule::GetAllowedElements() const {
    std::vector<SyntaxElement> result;
    for (const auto& elem : m_AllowedElements) {
        result.push_back(elem);
    }
    return result;
}

std::vector<SyntaxElement> ConstraintRule::GetForbiddenElements() const {
    std::vector<SyntaxElement> result;
    for (const auto& elem : m_ForbiddenElements) {
        result.push_back(elem);
    }
    return result;
}

// ConstraintValidator 实现

ConstraintValidator::ConstraintValidator()
    : m_StrictMode(false) {}

ConstraintValidator::~ConstraintValidator() = default;

bool ConstraintValidator::Validate(ast::ASTNode* node, ContextType context) {
    if (!node) return true;
    
    // 获取节点对应的语法元素
    SyntaxElement element = GetSyntaxElement(node);
    
    // 获取当前上下文的约束规则
    ConstraintManager manager;
    manager.InitializeDefaultRules();
    const ConstraintRule* rule = manager.GetRule(context);
    
    if (!rule) {
        // 没有特定规则，默认允许
        return true;
    }
    
    // 检查是否允许
    if (!rule->IsAllowed(element)) {
        std::stringstream ss;
        ss << "语法元素 " << ConstraintUtils::ElementToString(element)
           << " 不允许在 " << ConstraintUtils::ContextToString(context)
           << " 中使用";
        ReportError(ss.str(), node);
        return false;
    }
    
    // 递归验证子节点
    for (const auto& child : node->GetChildren()) {
        // 根据节点类型确定子节点的上下文
        ContextType childContext = context;
        
        if (dynamic_cast<ast::StyleBlockNode*>(node)) {
            // 进入样式块
            if (context == ContextType::TOP_LEVEL) {
                childContext = ContextType::GLOBAL_STYLE;
            } else {
                childContext = ContextType::LOCAL_STYLE;
            }
        } else if (dynamic_cast<ast::ScriptBlockNode*>(node)) {
            // 进入script块
            if (context == ContextType::ELEMENT_BODY) {
                childContext = ContextType::LOCAL_SCRIPT;
            } else {
                childContext = ContextType::GLOBAL_SCRIPT;
            }
        }
        
        if (!Validate(child.get(), childContext)) {
            return false;
        }
    }
    
    return true;
}

bool ConstraintValidator::ValidateTree(ast::ASTNode* root) {
    m_Errors.clear();
    return Validate(root, ContextType::TOP_LEVEL);
}

bool ConstraintValidator::ValidateStyleBlock(ast::StyleBlockNode* node, ContextType context) {
    // 特殊处理样式块
    return Validate(node, context);
}

bool ConstraintValidator::ValidateScriptBlock(ast::ScriptBlockNode* node, ContextType context) {
    // 特殊处理script块
    return Validate(node, context);
}

SyntaxElement ConstraintValidator::GetSyntaxElement(ast::ASTNode* node) const {
    if (!node) return SyntaxElement::CHTL_SYNTAX;
    
    // 根据节点类型判断语法元素
    switch (node->GetType()) {
        // 注释总是允许的
        case ast::ASTNodeType::COMMENT:
            return SyntaxElement::GENERATOR_COMMENT;
            
        // 原始嵌入总是允许的
        case ast::ASTNodeType::ORIGIN:
            return SyntaxElement::ORIGIN_EMBED;
            
        // 模板相关
        case ast::ASTNodeType::TEMPLATE_STYLE:
            return SyntaxElement::TEMPLATE_STYLE;
        case ast::ASTNodeType::TEMPLATE_ELEMENT:
            return SyntaxElement::CHTL_SYNTAX;  // 元素模板在样式块中不适用
        case ast::ASTNodeType::TEMPLATE_VAR:
            return SyntaxElement::TEMPLATE_VAR;
            
        // 自定义相关
        case ast::ASTNodeType::CUSTOM_STYLE:
            return SyntaxElement::CUSTOM_STYLE;
        case ast::ASTNodeType::CUSTOM_ELEMENT:
            return SyntaxElement::CHTL_SYNTAX;  // 自定义元素在样式块中不适用
        case ast::ASTNodeType::CUSTOM_VAR:
            return SyntaxElement::CUSTOM_VAR;
            
        // 特例化操作
        case ast::ASTNodeType::DELETE:
            // 需要进一步判断是删除属性还是删除继承
            if (node->GetValue().find("inherit") != std::string::npos) {
                return SyntaxElement::DELETE_INHERITANCE;
            } else {
                return SyntaxElement::DELETE_PROPERTY;
            }
            
        case ast::ASTNodeType::INHERIT:
            return SyntaxElement::STYLE_INHERITANCE;
            
        // 变量引用
        case ast::ASTNodeType::VAR_REFERENCE:
            // 需要判断是模板变量还是自定义变量
            return SyntaxElement::TEMPLATE_VAR;  // 简化处理
            
        // 命名空间相关
        case ast::ASTNodeType::NAMESPACE:
            return SyntaxElement::NAMESPACE_FROM;
            
        // HTML元素
        case ast::ASTNodeType::ELEMENT:
            return SyntaxElement::HTML_ELEMENT;
            
        // 约束
        case ast::ASTNodeType::EXCEPT:
            return SyntaxElement::EXCEPT_CONSTRAINT;
            
        default:
            return SyntaxElement::CHTL_SYNTAX;
    }
}

void ConstraintValidator::ReportError(const std::string& message, ast::ASTNode* node) {
    std::stringstream ss;
    ss << "[约束错误";
    
    if (node) {
        ss << " " << node->GetSourceLocation() << ":" 
           << node->GetLine() << ":" << node->GetColumn();
    }
    
    ss << "] " << message;
    
    std::string error = ss.str();
    m_Errors.push_back(error);
    
    if (m_StrictMode) {
        utils::Logger::GetInstance().Error(error);
    } else {
        utils::Logger::GetInstance().Warning(error);
    }
}

// ConstraintManager 实现

ConstraintManager::ConstraintManager() {}

ConstraintManager::~ConstraintManager() = default;

void ConstraintManager::InitializeDefaultRules() {
    InitializeGlobalStyleRules();
    InitializeLocalStyleRules();
    InitializeGlobalScriptRules();
    InitializeLocalScriptRules();
}

void ConstraintManager::InitializeGlobalStyleRules() {
    // 根据目标规划.ini第137行
    // 全局样式块允许的元素
    auto rule = std::make_unique<ConstraintRule>(ContextType::GLOBAL_STYLE);
    
    rule->AddAllowedElement(SyntaxElement::TEMPLATE_VAR);
    rule->AddAllowedElement(SyntaxElement::CUSTOM_VAR);
    rule->AddAllowedElement(SyntaxElement::VAR_SPECIALIZATION);
    rule->AddAllowedElement(SyntaxElement::TEMPLATE_STYLE);
    rule->AddAllowedElement(SyntaxElement::CUSTOM_STYLE);
    rule->AddAllowedElement(SyntaxElement::VALUELESS_STYLE);
    rule->AddAllowedElement(SyntaxElement::STYLE_SPECIALIZATION);
    rule->AddAllowedElement(SyntaxElement::DELETE_PROPERTY);
    rule->AddAllowedElement(SyntaxElement::DELETE_INHERITANCE);
    rule->AddAllowedElement(SyntaxElement::STYLE_INHERITANCE);
    rule->AddAllowedElement(SyntaxElement::GENERATOR_COMMENT);
    rule->AddAllowedElement(SyntaxElement::FULL_QUALIFIED_NAME);
    rule->AddAllowedElement(SyntaxElement::ORIGIN_EMBED);
    rule->AddAllowedElement(SyntaxElement::NAMESPACE_FROM);
    
    m_Rules[ContextType::GLOBAL_STYLE] = std::move(rule);
}

void ConstraintManager::InitializeLocalStyleRules() {
    // 根据目标规划.ini第141行
    // 局部样式块允许的元素（与全局样式块相同）
    auto rule = std::make_unique<ConstraintRule>(ContextType::LOCAL_STYLE);
    
    rule->AddAllowedElement(SyntaxElement::TEMPLATE_VAR);
    rule->AddAllowedElement(SyntaxElement::CUSTOM_VAR);
    rule->AddAllowedElement(SyntaxElement::VAR_SPECIALIZATION);
    rule->AddAllowedElement(SyntaxElement::TEMPLATE_STYLE);
    rule->AddAllowedElement(SyntaxElement::CUSTOM_STYLE);
    rule->AddAllowedElement(SyntaxElement::VALUELESS_STYLE);
    rule->AddAllowedElement(SyntaxElement::STYLE_SPECIALIZATION);
    rule->AddAllowedElement(SyntaxElement::DELETE_PROPERTY);
    rule->AddAllowedElement(SyntaxElement::DELETE_INHERITANCE);
    rule->AddAllowedElement(SyntaxElement::STYLE_INHERITANCE);
    rule->AddAllowedElement(SyntaxElement::GENERATOR_COMMENT);
    rule->AddAllowedElement(SyntaxElement::FULL_QUALIFIED_NAME);
    rule->AddAllowedElement(SyntaxElement::ORIGIN_EMBED);
    rule->AddAllowedElement(SyntaxElement::NAMESPACE_FROM);
    
    m_Rules[ContextType::LOCAL_STYLE] = std::move(rule);
}

void ConstraintManager::InitializeGlobalScriptRules() {
    // 根据目标规划.ini第139行
    // 除了局部script外，其他script禁止使用任何CHTL语法
    // 只允许注释和原始嵌入
    auto rule = std::make_unique<ConstraintRule>(ContextType::GLOBAL_SCRIPT);
    
    rule->AddAllowedElement(SyntaxElement::GENERATOR_COMMENT);
    rule->AddAllowedElement(SyntaxElement::ORIGIN_EMBED);
    
    m_Rules[ContextType::GLOBAL_SCRIPT] = std::move(rule);
}

void ConstraintManager::InitializeLocalScriptRules() {
    // 根据目标规划.ini第143行
    // 局部script允许的元素
    auto rule = std::make_unique<ConstraintRule>(ContextType::LOCAL_SCRIPT);
    
    rule->AddAllowedElement(SyntaxElement::TEMPLATE_VAR);
    rule->AddAllowedElement(SyntaxElement::CUSTOM_VAR);
    rule->AddAllowedElement(SyntaxElement::VAR_SPECIALIZATION);
    rule->AddAllowedElement(SyntaxElement::NAMESPACE_FROM);
    rule->AddAllowedElement(SyntaxElement::GENERATOR_COMMENT);
    rule->AddAllowedElement(SyntaxElement::ORIGIN_EMBED);
    
    m_Rules[ContextType::LOCAL_SCRIPT] = std::move(rule);
}

ConstraintRule* ConstraintManager::GetRule(ContextType context) {
    auto it = m_Rules.find(context);
    if (it != m_Rules.end()) {
        return it->second.get();
    }
    return nullptr;
}

const ConstraintRule* ConstraintManager::GetRule(ContextType context) const {
    auto it = m_Rules.find(context);
    if (it != m_Rules.end()) {
        return it->second.get();
    }
    return nullptr;
}

void ConstraintManager::AddRule(std::unique_ptr<ConstraintRule> rule) {
    if (rule) {
        m_Rules[rule->GetContext()] = std::move(rule);
    }
}

std::unique_ptr<ConstraintValidator> ConstraintManager::CreateValidator() const {
    return std::make_unique<ConstraintValidator>();
}

std::vector<const ConstraintRule*> ConstraintManager::GetAllRules() const {
    std::vector<const ConstraintRule*> result;
    for (const auto& pair : m_Rules) {
        result.push_back(pair.second.get());
    }
    return result;
}

// ConstraintContextGuard 实现

ConstraintContextGuard::ConstraintContextGuard(ConstraintValidator& validator,
                                               ContextType newContext)
    : m_Validator(validator)
    , m_PreviousContext(ContextType::TOP_LEVEL)  // 需要保存之前的上下文
    , m_CurrentContext(newContext) {
    // 设置新上下文
}

ConstraintContextGuard::~ConstraintContextGuard() {
    // 恢复之前的上下文
}

// ConstraintViolationException 实现

ConstraintViolationException::ConstraintViolationException(const std::string& message,
                                                         SyntaxElement element,
                                                         ContextType context)
    : m_Message(message)
    , m_Element(element)
    , m_Context(context) {}

// ConstraintUtils 实现

std::string ConstraintUtils::ElementToString(SyntaxElement element) {
    switch (element) {
        case SyntaxElement::TEMPLATE_VAR:
            return "模板变量";
        case SyntaxElement::CUSTOM_VAR:
            return "自定义变量";
        case SyntaxElement::VAR_SPECIALIZATION:
            return "变量特例化";
        case SyntaxElement::TEMPLATE_STYLE:
            return "模板样式组";
        case SyntaxElement::CUSTOM_STYLE:
            return "自定义样式组";
        case SyntaxElement::VALUELESS_STYLE:
            return "无值样式组";
        case SyntaxElement::STYLE_SPECIALIZATION:
            return "样式组特例化";
        case SyntaxElement::DELETE_PROPERTY:
            return "delete属性";
        case SyntaxElement::DELETE_INHERITANCE:
            return "delete继承";
        case SyntaxElement::STYLE_INHERITANCE:
            return "样式组继承";
        case SyntaxElement::GENERATOR_COMMENT:
            return "生成器注释";
        case SyntaxElement::ORIGIN_EMBED:
            return "原始嵌入";
        case SyntaxElement::NAMESPACE_FROM:
            return "命名空间from";
        case SyntaxElement::FULL_QUALIFIED_NAME:
            return "全缀名";
        case SyntaxElement::HTML_ELEMENT:
            return "HTML元素";
        case SyntaxElement::CHTL_SYNTAX:
            return "CHTL语法";
        case SyntaxElement::EXCEPT_CONSTRAINT:
            return "except约束";
        default:
            return "未知元素";
    }
}

std::string ConstraintUtils::ContextToString(ContextType context) {
    switch (context) {
        case ContextType::GLOBAL_STYLE:
            return "全局样式块";
        case ContextType::LOCAL_STYLE:
            return "局部样式块";
        case ContextType::GLOBAL_SCRIPT:
            return "全局script块";
        case ContextType::LOCAL_SCRIPT:
            return "局部script块";
        case ContextType::ELEMENT_BODY:
            return "元素内容";
        case ContextType::NAMESPACE:
            return "命名空间";
        case ContextType::TOP_LEVEL:
            return "顶层";
        default:
            return "未知上下文";
    }
}

bool ConstraintUtils::IsUniversallyAllowed(SyntaxElement element) {
    // 注释和原始嵌入在任何地方都允许
    return element == SyntaxElement::GENERATOR_COMMENT ||
           element == SyntaxElement::ORIGIN_EMBED;
}

std::string ConstraintUtils::GetContextDescription(ContextType context) {
    switch (context) {
        case ContextType::GLOBAL_STYLE:
            return "全局样式块：只能使用样式相关的CHTL语法，不能使用HTML元素";
        case ContextType::LOCAL_STYLE:
            return "局部样式块：可以使用样式相关的CHTL语法，支持内联样式和选择器";
        case ContextType::GLOBAL_SCRIPT:
            return "全局script块：只能使用--注释和原始嵌入，不能使用其他CHTL语法";
        case ContextType::LOCAL_SCRIPT:
            return "局部script块：可以使用变量相关的CHTL语法，支持{{}}等特殊语法";
        case ContextType::ELEMENT_BODY:
            return "元素内容：可以使用大部分CHTL语法";
        case ContextType::NAMESPACE:
            return "命名空间：可以定义和组织CHTL元素";
        case ContextType::TOP_LEVEL:
            return "顶层：可以使用所有CHTL语法";
        default:
            return "未知上下文";
    }
}

} // namespace constraint
} // namespace chtl