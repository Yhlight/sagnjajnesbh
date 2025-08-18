#include "SyntaxConstrainer.h"
#include <iostream>
#include <sstream>

namespace chtl {

SyntaxConstrainer::SyntaxConstrainer() : strict_mode_(true) {
    initializeConstraintRules();
}

SyntaxConstrainer::~SyntaxConstrainer() = default;

void SyntaxConstrainer::initializeConstraintRules() {
    setupGlobalStyleConstraints();
    setupLocalStyleConstraints();
    setupGlobalScriptConstraints();
    setupLocalScriptConstraints();
}

void SyntaxConstrainer::setupGlobalStyleConstraints() {
    // 全局样式块允许的语法元素
    std::vector<SyntaxElement> allowed_elements = {
        // 变量相关
        SyntaxElement::TEMPLATE_VAR,
        SyntaxElement::CUSTOM_VAR,
        SyntaxElement::VAR_SPECIALIZATION,
        
        // 样式组相关
        SyntaxElement::TEMPLATE_STYLE,
        SyntaxElement::CUSTOM_STYLE,
        SyntaxElement::VALUELESS_STYLE,
        SyntaxElement::STYLE_SPECIALIZATION,
        
        // 操作相关
        SyntaxElement::DELETE_PROPERTY,
        SyntaxElement::DELETE_INHERITANCE,
        SyntaxElement::STYLE_INHERITANCE,
        
        // 访问相关
        SyntaxElement::NAMESPACE_FROM,
        SyntaxElement::QUALIFIED_NAME,
        
        // 特殊存在（可在任意位置使用）
        SyntaxElement::GENERATOR_COMMENT,
        SyntaxElement::ORIGIN_EMBED
    };
    
    // 设置允许的元素
    for (SyntaxElement element : allowed_elements) {
        constraint_rules_[ContextType::GLOBAL_STYLE][element] = true;
    }
    
    // 明确禁止的元素
    std::vector<SyntaxElement> forbidden_elements = {
        // CHTL JS特有语法
        SyntaxElement::CHTL_JS_SELECTOR,
        SyntaxElement::CHTL_JS_ARROW,
        SyntaxElement::CHTL_JS_LISTEN,
        SyntaxElement::CHTL_JS_DELEGATE,
        SyntaxElement::CHTL_JS_ANIMATE,
        SyntaxElement::CHTL_JS_VIR,
        
        // 元素相关（在样式块中不允许）
        SyntaxElement::TEMPLATE_ELEMENT,
        SyntaxElement::CUSTOM_ELEMENT,
        SyntaxElement::ELEMENT_SPECIALIZATION,
        SyntaxElement::ELEMENT_INSERT,
        SyntaxElement::ELEMENT_DELETE,
        
        // 纯JavaScript代码
        SyntaxElement::JAVASCRIPT_CODE
    };
    
    for (SyntaxElement element : forbidden_elements) {
        constraint_rules_[ContextType::GLOBAL_STYLE][element] = false;
    }
}

void SyntaxConstrainer::setupLocalStyleConstraints() {
    // 局部样式块允许的语法元素（与全局样式块相同）
    std::vector<SyntaxElement> allowed_elements = {
        // 变量相关
        SyntaxElement::TEMPLATE_VAR,
        SyntaxElement::CUSTOM_VAR,
        SyntaxElement::VAR_SPECIALIZATION,
        
        // 样式组相关
        SyntaxElement::TEMPLATE_STYLE,
        SyntaxElement::CUSTOM_STYLE,
        SyntaxElement::VALUELESS_STYLE,
        SyntaxElement::STYLE_SPECIALIZATION,
        
        // 操作相关
        SyntaxElement::DELETE_PROPERTY,
        SyntaxElement::DELETE_INHERITANCE,
        SyntaxElement::STYLE_INHERITANCE,
        
        // 访问相关
        SyntaxElement::NAMESPACE_FROM,
        SyntaxElement::QUALIFIED_NAME,
        
        // 特殊存在
        SyntaxElement::GENERATOR_COMMENT,
        SyntaxElement::ORIGIN_EMBED,
        
        // CSS相关
        SyntaxElement::CSS_PROPERTY
    };
    
    for (SyntaxElement element : allowed_elements) {
        constraint_rules_[ContextType::LOCAL_STYLE][element] = true;
    }
    
    // 禁止CHTL JS语法和元素操作
    std::vector<SyntaxElement> forbidden_elements = {
        SyntaxElement::CHTL_JS_SELECTOR,
        SyntaxElement::CHTL_JS_ARROW,
        SyntaxElement::CHTL_JS_LISTEN,
        SyntaxElement::CHTL_JS_DELEGATE,
        SyntaxElement::CHTL_JS_ANIMATE,
        SyntaxElement::CHTL_JS_VIR,
        SyntaxElement::ELEMENT_INSERT,
        SyntaxElement::ELEMENT_DELETE,
        SyntaxElement::JAVASCRIPT_CODE
    };
    
    for (SyntaxElement element : forbidden_elements) {
        constraint_rules_[ContextType::LOCAL_STYLE][element] = false;
    }
}

void SyntaxConstrainer::setupGlobalScriptConstraints() {
    // 全局script禁止使用任何CHTL语法，除了注释和原始嵌入
    std::vector<SyntaxElement> allowed_elements = {
        // 特殊存在（可在任意位置使用）
        SyntaxElement::GENERATOR_COMMENT,
        SyntaxElement::ORIGIN_EMBED,
        
        // 纯JavaScript代码
        SyntaxElement::JAVASCRIPT_CODE
    };
    
    for (SyntaxElement element : allowed_elements) {
        constraint_rules_[ContextType::GLOBAL_SCRIPT][element] = true;
    }
    
    // 禁止所有CHTL语法
    std::vector<SyntaxElement> forbidden_elements = {
        // 变量相关
        SyntaxElement::TEMPLATE_VAR,
        SyntaxElement::CUSTOM_VAR,
        SyntaxElement::VAR_SPECIALIZATION,
        
        // 样式组相关
        SyntaxElement::TEMPLATE_STYLE,
        SyntaxElement::CUSTOM_STYLE,
        SyntaxElement::VALUELESS_STYLE,
        SyntaxElement::STYLE_SPECIALIZATION,
        
        // 命名空间访问
        SyntaxElement::NAMESPACE_FROM,
        SyntaxElement::QUALIFIED_NAME,
        
        // CHTL JS语法（除了{{&}}等特供语法）
        SyntaxElement::CHTL_JS_LISTEN,
        SyntaxElement::CHTL_JS_DELEGATE,
        SyntaxElement::CHTL_JS_ANIMATE,
        SyntaxElement::CHTL_JS_VIR
    };
    
    for (SyntaxElement element : forbidden_elements) {
        constraint_rules_[ContextType::GLOBAL_SCRIPT][element] = false;
    }
    
    // 注意：{{&}}等特供语法属于CHTL本身功能，应该允许
    constraint_rules_[ContextType::GLOBAL_SCRIPT][SyntaxElement::CHTL_JS_SELECTOR] = true;
    constraint_rules_[ContextType::GLOBAL_SCRIPT][SyntaxElement::CHTL_JS_ARROW] = true;
}

void SyntaxConstrainer::setupLocalScriptConstraints() {
    // 局部script允许的CHTL语法元素
    std::vector<SyntaxElement> allowed_elements = {
        // 变量相关
        SyntaxElement::TEMPLATE_VAR,
        SyntaxElement::CUSTOM_VAR,
        SyntaxElement::VAR_SPECIALIZATION,
        
        // 命名空间访问
        SyntaxElement::NAMESPACE_FROM,
        
        // 特殊存在
        SyntaxElement::GENERATOR_COMMENT,
        SyntaxElement::ORIGIN_EMBED,
        
        // CHTL JS特供语法（{{&}}等属于CHTL本身功能）
        SyntaxElement::CHTL_JS_SELECTOR,
        SyntaxElement::CHTL_JS_ARROW,
        SyntaxElement::CHTL_JS_LISTEN,
        SyntaxElement::CHTL_JS_DELEGATE,
        SyntaxElement::CHTL_JS_ANIMATE,
        SyntaxElement::CHTL_JS_VIR,
        
        // JavaScript代码
        SyntaxElement::JAVASCRIPT_CODE
    };
    
    for (SyntaxElement element : allowed_elements) {
        constraint_rules_[ContextType::LOCAL_SCRIPT][element] = true;
    }
    
    // 禁止样式相关语法
    std::vector<SyntaxElement> forbidden_elements = {
        SyntaxElement::TEMPLATE_STYLE,
        SyntaxElement::CUSTOM_STYLE,
        SyntaxElement::VALUELESS_STYLE,
        SyntaxElement::STYLE_SPECIALIZATION,
        SyntaxElement::STYLE_INHERITANCE,
        SyntaxElement::DELETE_PROPERTY,
        SyntaxElement::DELETE_INHERITANCE,
        SyntaxElement::CSS_PROPERTY,
        
        // 元素相关
        SyntaxElement::TEMPLATE_ELEMENT,
        SyntaxElement::CUSTOM_ELEMENT,
        SyntaxElement::ELEMENT_SPECIALIZATION,
        SyntaxElement::ELEMENT_INSERT,
        SyntaxElement::ELEMENT_DELETE
    };
    
    for (SyntaxElement element : forbidden_elements) {
        constraint_rules_[ContextType::LOCAL_SCRIPT][element] = false;
    }
}

bool SyntaxConstrainer::isElementAllowed(ContextType context, SyntaxElement element) {
    // 检查特殊元素（注释和原始嵌入可在任意位置使用）
    if (isUniversalElement(element)) {
        return true;
    }
    
    auto context_it = constraint_rules_.find(context);
    if (context_it == constraint_rules_.end()) {
        return false; // 未知上下文，默认禁止
    }
    
    auto element_it = context_it->second.find(element);
    if (element_it == context_it->second.end()) {
        return false; // 未定义规则，默认禁止
    }
    
    return element_it->second;
}

std::vector<ConstraintViolation> SyntaxConstrainer::validateContext(ContextType context, ast::ASTNode* node) {
    std::vector<ConstraintViolation> violations;
    
    if (!node) return violations;
    
    // 识别当前节点的语法元素
    SyntaxElement element = identifySyntaxElement(node);
    
    // 检查是否允许
    if (!isElementAllowed(context, element)) {
        ConstraintViolation violation;
        violation.context = context;
        violation.forbidden_element = element;
        violation.element_name = node->toString();
        violation.line = node->position.line;
        violation.column = node->position.column;
        violation.message = "在" + getContextTypeName(context) + "中不允许使用" + getSyntaxElementName(element);
        violation.suggestion = generateSuggestion(violation);
        
        violations.push_back(violation);
    }
    
    // 递归检查子节点
    for (size_t i = 0; i < node->getChildCount(); ++i) {
        auto child_violations = validateContext(context, node->getChild(i));
        violations.insert(violations.end(), child_violations.begin(), child_violations.end());
    }
    
    return violations;
}

std::vector<ConstraintViolation> SyntaxConstrainer::validateGlobalStyle(ast::ASTNode* style_block) {
    return validateContext(ContextType::GLOBAL_STYLE, style_block);
}

std::vector<ConstraintViolation> SyntaxConstrainer::validateLocalStyle(ast::ASTNode* style_block) {
    return validateContext(ContextType::LOCAL_STYLE, style_block);
}

std::vector<ConstraintViolation> SyntaxConstrainer::validateGlobalScript(ast::ASTNode* script_block) {
    return validateContext(ContextType::GLOBAL_SCRIPT, script_block);
}

std::vector<ConstraintViolation> SyntaxConstrainer::validateLocalScript(ast::ASTNode* script_block) {
    return validateContext(ContextType::LOCAL_SCRIPT, script_block);
}

std::vector<ConstraintViolation> SyntaxConstrainer::validateAST(ast::ASTNode* root) {
    return validateNode(root, ContextType::ELEMENT_CONTENT);
}

std::vector<ConstraintViolation> SyntaxConstrainer::validateNode(ast::ASTNode* node, ContextType parent_context) {
    std::vector<ConstraintViolation> violations;
    
    if (!node) return violations;
    
    // 推断当前节点的上下文类型
    ContextType current_context = inferContextType(node, parent_context);
    
    // 验证当前上下文
    auto context_violations = validateContext(current_context, node);
    violations.insert(violations.end(), context_violations.begin(), context_violations.end());
    
    // 递归验证子节点
    for (size_t i = 0; i < node->getChildCount(); ++i) {
        auto child_violations = validateNode(node->getChild(i), current_context);
        violations.insert(violations.end(), child_violations.begin(), child_violations.end());
    }
    
    return violations;
}

bool SyntaxConstrainer::isUniversalElement(SyntaxElement element) {
    // 注释和原始嵌入可在任意位置使用
    return element == SyntaxElement::GENERATOR_COMMENT || 
           element == SyntaxElement::ORIGIN_EMBED;
}

bool SyntaxConstrainer::isCHTLJSSpecificElement(SyntaxElement element) {
    return element == SyntaxElement::CHTL_JS_SELECTOR ||
           element == SyntaxElement::CHTL_JS_ARROW ||
           element == SyntaxElement::CHTL_JS_LISTEN ||
           element == SyntaxElement::CHTL_JS_DELEGATE ||
           element == SyntaxElement::CHTL_JS_ANIMATE ||
           element == SyntaxElement::CHTL_JS_VIR;
}

bool SyntaxConstrainer::isCHTLSpecificElement(SyntaxElement element) {
    return element == SyntaxElement::TEMPLATE_VAR ||
           element == SyntaxElement::CUSTOM_VAR ||
           element == SyntaxElement::VAR_SPECIALIZATION ||
           element == SyntaxElement::TEMPLATE_STYLE ||
           element == SyntaxElement::CUSTOM_STYLE ||
           element == SyntaxElement::VALUELESS_STYLE ||
           element == SyntaxElement::STYLE_SPECIALIZATION ||
           element == SyntaxElement::TEMPLATE_ELEMENT ||
           element == SyntaxElement::CUSTOM_ELEMENT ||
           element == SyntaxElement::ELEMENT_SPECIALIZATION ||
           element == SyntaxElement::DELETE_PROPERTY ||
           element == SyntaxElement::DELETE_INHERITANCE ||
           element == SyntaxElement::STYLE_INHERITANCE ||
           element == SyntaxElement::NAMESPACE_FROM ||
           element == SyntaxElement::QUALIFIED_NAME;
}

ContextType SyntaxConstrainer::inferContextType(ast::ASTNode* node, ContextType parent_context) {
    if (!node) return parent_context;
    
    // 根据节点类型推断上下文
    std::string nodeType = node->getNodeType();
    
    if (nodeType == "StyleBlock") {
        // 判断是全局还是局部样式块
        return (parent_context == ContextType::ELEMENT_CONTENT) ? 
               ContextType::LOCAL_STYLE : ContextType::GLOBAL_STYLE;
    }
    else if (nodeType == "ScriptBlock") {
        // 判断是全局还是局部script
        return (parent_context == ContextType::ELEMENT_CONTENT) ? 
               ContextType::LOCAL_SCRIPT : ContextType::GLOBAL_SCRIPT;
    }
    else if (nodeType == "Template") {
        return ContextType::TEMPLATE_DEF;
    }
    else if (nodeType == "Custom") {
        return ContextType::CUSTOM_DEF;
    }
    else if (nodeType == "Namespace") {
        return ContextType::NAMESPACE_DEF;
    }
    else if (nodeType == "Import") {
        return ContextType::IMPORT_STMT;
    }
    else if (nodeType == "Origin") {
        return ContextType::ORIGIN_BLOCK;
    }
    else if (nodeType == "Configuration") {
        return ContextType::CONFIGURATION;
    }
    else if (nodeType == "Element") {
        return ContextType::ELEMENT_CONTENT;
    }
    else {
        return parent_context;
    }
}

SyntaxElement SyntaxConstrainer::identifySyntaxElement(ast::ASTNode* node) {
    if (!node) return SyntaxElement::JAVASCRIPT_CODE;
    
    // 根据AST节点类型映射到语法元素
    std::string nodeType = node->getNodeType();
    
    if (nodeType == "Template") {
        return SyntaxElement::TEMPLATE_VAR;
    }
    else if (nodeType == "Custom") {
        return SyntaxElement::CUSTOM_VAR;
    }
    else if (nodeType == "Element") {
        return SyntaxElement::TEMPLATE_ELEMENT;
    }
    else if (nodeType == "StyleBlock") {
        return SyntaxElement::TEMPLATE_STYLE;
    }
    else if (nodeType == "ScriptBlock") {
        return SyntaxElement::CHTL_JS_SELECTOR;
    }
    else if (nodeType == "Comment") {
        return SyntaxElement::GENERATOR_COMMENT;
    }
    else if (nodeType == "Import") {
        return SyntaxElement::ORIGIN_EMBED;
    }
    else {
        return SyntaxElement::JAVASCRIPT_CODE;
    }
}

void SyntaxConstrainer::reportViolation(const ConstraintViolation& violation) {
    violations_.push_back(violation);
    
    if (strict_mode_) {
        std::cerr << "语法约束违规 [" << violation.line << ":" << violation.column << "] "
                  << violation.message << std::endl;
        if (!violation.suggestion.empty()) {
            std::cerr << "建议: " << violation.suggestion << std::endl;
        }
    }
}

std::string SyntaxConstrainer::generateSuggestion(const ConstraintViolation& violation) {
    std::ostringstream suggestion;
    
    switch (violation.context) {
        case ContextType::GLOBAL_STYLE:
            if (isCHTLJSSpecificElement(violation.forbidden_element)) {
                suggestion << "CHTL JS语法不能在全局样式块中使用，请移至局部script块";
            } else {
                suggestion << "该语法元素不允许在全局样式块中使用";
            }
            break;
            
        case ContextType::GLOBAL_SCRIPT:
            if (isCHTLSpecificElement(violation.forbidden_element)) {
                suggestion << "CHTL语法不能在全局script中使用，请移至局部script块或使用原始嵌入";
            }
            break;
            
        case ContextType::LOCAL_STYLE:
            if (isCHTLJSSpecificElement(violation.forbidden_element)) {
                suggestion << "CHTL JS语法不能在样式块中使用，请移至script块";
            }
            break;
            
        case ContextType::LOCAL_SCRIPT:
            if (violation.forbidden_element == SyntaxElement::TEMPLATE_STYLE ||
                violation.forbidden_element == SyntaxElement::CUSTOM_STYLE) {
                suggestion << "样式组语法不能在script块中使用，请移至style块";
            }
            break;
            
        default:
            suggestion << "请检查语法使用的上下文是否正确";
            break;
    }
    
    return suggestion.str();
}

std::string SyntaxConstrainer::getSyntaxElementName(SyntaxElement element) const {
    switch (element) {
        case SyntaxElement::TEMPLATE_VAR: return "模板变量";
        case SyntaxElement::CUSTOM_VAR: return "自定义变量";
        case SyntaxElement::VAR_SPECIALIZATION: return "变量特例化";
        case SyntaxElement::TEMPLATE_STYLE: return "模板样式组";
        case SyntaxElement::CUSTOM_STYLE: return "自定义样式组";
        case SyntaxElement::VALUELESS_STYLE: return "无值样式组";
        case SyntaxElement::STYLE_SPECIALIZATION: return "样式组特例化";
        case SyntaxElement::TEMPLATE_ELEMENT: return "模板元素";
        case SyntaxElement::CUSTOM_ELEMENT: return "自定义元素";
        case SyntaxElement::DELETE_PROPERTY: return "delete属性";
        case SyntaxElement::DELETE_INHERITANCE: return "delete继承";
        case SyntaxElement::STYLE_INHERITANCE: return "样式继承";
        case SyntaxElement::NAMESPACE_FROM: return "命名空间from";
        case SyntaxElement::QUALIFIED_NAME: return "全缀名";
        case SyntaxElement::CHTL_JS_SELECTOR: return "CHTL JS选择器";
        case SyntaxElement::CHTL_JS_LISTEN: return "listen函数";
        case SyntaxElement::CHTL_JS_DELEGATE: return "delegate函数";
        case SyntaxElement::CHTL_JS_ANIMATE: return "animate函数";
        case SyntaxElement::CHTL_JS_VIR: return "vir虚对象";
        case SyntaxElement::GENERATOR_COMMENT: return "生成器注释";
        case SyntaxElement::ORIGIN_EMBED: return "原始嵌入";
        default: return "未知语法元素";
    }
}

std::string SyntaxConstrainer::getContextTypeName(ContextType context) const {
    switch (context) {
        case ContextType::GLOBAL_STYLE: return "全局样式块";
        case ContextType::LOCAL_STYLE: return "局部样式块";
        case ContextType::GLOBAL_SCRIPT: return "全局script";
        case ContextType::LOCAL_SCRIPT: return "局部script";
        case ContextType::ELEMENT_CONTENT: return "元素内容";
        case ContextType::TEMPLATE_DEF: return "模板定义";
        case ContextType::CUSTOM_DEF: return "自定义定义";
        case ContextType::NAMESPACE_DEF: return "命名空间定义";
        default: return "未知上下文";
    }
}

std::vector<ConstraintViolation> SyntaxConstrainer::getViolations() const {
    return violations_;
}

void SyntaxConstrainer::clearViolations() {
    violations_.clear();
}

void SyntaxConstrainer::addConstraintRule(ContextType context, SyntaxElement element, bool allowed) {
    constraint_rules_[context][element] = allowed;
}

void SyntaxConstrainer::removeConstraintRule(ContextType context, SyntaxElement element) {
    auto context_it = constraint_rules_.find(context);
    if (context_it != constraint_rules_.end()) {
        context_it->second.erase(element);
    }
}

bool SyntaxConstrainer::hasConstraintRule(ContextType context, SyntaxElement element) {
    auto context_it = constraint_rules_.find(context);
    if (context_it == constraint_rules_.end()) {
        return false;
    }
    
    return context_it->second.find(element) != context_it->second.end();
}

void SyntaxConstrainer::setStrictMode(bool strict) {
    strict_mode_ = strict;
}

bool SyntaxConstrainer::isStrictMode() const {
    return strict_mode_;
}

void SyntaxConstrainer::printConstraintRules() const {
    std::cout << "=== 语法约束规则 ===" << std::endl;
    
    for (const auto& [context, rules] : constraint_rules_) {
        std::cout << getContextTypeName(context) << ":" << std::endl;
        
        std::cout << "  允许的语法:" << std::endl;
        for (const auto& [element, allowed] : rules) {
            if (allowed) {
                std::cout << "    ✅ " << getSyntaxElementName(element) << std::endl;
            }
        }
        
        std::cout << "  禁止的语法:" << std::endl;
        for (const auto& [element, allowed] : rules) {
            if (!allowed) {
                std::cout << "    ❌ " << getSyntaxElementName(element) << std::endl;
            }
        }
        std::cout << std::endl;
    }
}

std::string SyntaxConstrainer::getConstraintStatistics() const {
    std::ostringstream stats;
    
    stats << "约束规则统计:" << std::endl;
    stats << "上下文类型数: " << constraint_rules_.size() << std::endl;
    stats << "违规记录数: " << violations_.size() << std::endl;
    stats << "严格模式: " << (strict_mode_ ? "开启" : "关闭") << std::endl;
    
    return stats.str();
}

// ConstrainerUtils命名空间实现
namespace ConstrainerUtils {

std::string contextTypeToString(ContextType type) {
    switch (type) {
        case ContextType::GLOBAL_STYLE: return "全局样式块";
        case ContextType::LOCAL_STYLE: return "局部样式块";
        case ContextType::GLOBAL_SCRIPT: return "全局script";
        case ContextType::LOCAL_SCRIPT: return "局部script";
        case ContextType::ELEMENT_CONTENT: return "元素内容";
        case ContextType::TEMPLATE_DEF: return "模板定义";
        case ContextType::CUSTOM_DEF: return "自定义定义";
        case ContextType::NAMESPACE_DEF: return "命名空间定义";
        case ContextType::IMPORT_STMT: return "导入语句";
        case ContextType::ORIGIN_BLOCK: return "原始嵌入块";
        case ContextType::CONFIGURATION: return "配置块";
        default: return "未知上下文";
    }
}

std::string syntaxElementToString(SyntaxElement element) {
    switch (element) {
        case SyntaxElement::TEMPLATE_VAR: return "模板变量";
        case SyntaxElement::CUSTOM_VAR: return "自定义变量";
        case SyntaxElement::VAR_SPECIALIZATION: return "变量特例化";
        case SyntaxElement::TEMPLATE_STYLE: return "模板样式组";
        case SyntaxElement::CUSTOM_STYLE: return "自定义样式组";
        case SyntaxElement::VALUELESS_STYLE: return "无值样式组";
        case SyntaxElement::CHTL_JS_LISTEN: return "listen函数";
        case SyntaxElement::CHTL_JS_DELEGATE: return "delegate函数";
        case SyntaxElement::CHTL_JS_VIR: return "vir虚对象";
        default: return "其他语法元素";
    }
}

bool isVariableElement(SyntaxElement element) {
    return element == SyntaxElement::TEMPLATE_VAR ||
           element == SyntaxElement::CUSTOM_VAR ||
           element == SyntaxElement::VAR_SPECIALIZATION;
}

bool isStyleElement(SyntaxElement element) {
    return element == SyntaxElement::TEMPLATE_STYLE ||
           element == SyntaxElement::CUSTOM_STYLE ||
           element == SyntaxElement::VALUELESS_STYLE ||
           element == SyntaxElement::STYLE_SPECIALIZATION ||
           element == SyntaxElement::CSS_PROPERTY;
}

std::vector<SyntaxElement> getAllowedElementsInContext(ContextType context) {
    std::vector<SyntaxElement> allowed;
    
    switch (context) {
        case ContextType::GLOBAL_STYLE:
        case ContextType::LOCAL_STYLE:
            allowed = {
                SyntaxElement::TEMPLATE_VAR, SyntaxElement::CUSTOM_VAR, SyntaxElement::VAR_SPECIALIZATION,
                SyntaxElement::TEMPLATE_STYLE, SyntaxElement::CUSTOM_STYLE, SyntaxElement::VALUELESS_STYLE,
                SyntaxElement::STYLE_SPECIALIZATION, SyntaxElement::DELETE_PROPERTY, SyntaxElement::DELETE_INHERITANCE,
                SyntaxElement::STYLE_INHERITANCE, SyntaxElement::NAMESPACE_FROM, SyntaxElement::QUALIFIED_NAME,
                SyntaxElement::GENERATOR_COMMENT, SyntaxElement::ORIGIN_EMBED
            };
            break;
            
        case ContextType::LOCAL_SCRIPT:
            allowed = {
                SyntaxElement::TEMPLATE_VAR, SyntaxElement::CUSTOM_VAR, SyntaxElement::VAR_SPECIALIZATION,
                SyntaxElement::NAMESPACE_FROM, SyntaxElement::GENERATOR_COMMENT, SyntaxElement::ORIGIN_EMBED,
                SyntaxElement::CHTL_JS_SELECTOR, SyntaxElement::CHTL_JS_ARROW, SyntaxElement::CHTL_JS_LISTEN,
                SyntaxElement::CHTL_JS_DELEGATE, SyntaxElement::CHTL_JS_ANIMATE, SyntaxElement::CHTL_JS_VIR,
                SyntaxElement::JAVASCRIPT_CODE
            };
            break;
            
        case ContextType::GLOBAL_SCRIPT:
            allowed = {
                SyntaxElement::GENERATOR_COMMENT, SyntaxElement::ORIGIN_EMBED,
                SyntaxElement::CHTL_JS_SELECTOR, SyntaxElement::CHTL_JS_ARROW, // {{&}}等特供语法
                SyntaxElement::JAVASCRIPT_CODE
            };
            break;
            
        default:
            break;
    }
    
    return allowed;
}

} // namespace ConstrainerUtils

} // namespace chtl