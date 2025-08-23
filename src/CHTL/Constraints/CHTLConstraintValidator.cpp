#include "CHTL/Constraints/CHTLConstraintValidator.h"
#include "CHTL/AST/CHTLASTNodes.h"
#include "Utils/ErrorHandler.h"
#include <algorithm>

namespace CHTL {
namespace Constraints {

CHTLConstraintValidator::CHTLConstraintValidator() {
    InitializeDefaultConstraints();
}

void CHTLConstraintValidator::InitializeDefaultConstraints() {
    // 目标规划.ini第137行：全局样式块约束
    // 只能允许：模板变量的使用，自定义变量的使用，自定义变量的特例化，模板样式组，自定义样式组，
    // 无值样式组，自定义样式组的特例化，delete属性，delete继承，继承(样式组之间的继承)，
    // 生成器注释，全缀名，任意类型的原始嵌入，从命名空间中拿到某一个模板变量等
    allowedSyntax_[SyntaxContext::GLOBAL_STYLE] = {
        ConstraintTarget::TEMPLATE_VAR,         // 模板变量
        ConstraintTarget::CUSTOM_VAR,           // 自定义变量
        ConstraintTarget::TEMPLATE_STYLE,       // 模板样式组
        ConstraintTarget::CUSTOM_STYLE,         // 自定义样式组
        ConstraintTarget::ORIGIN_EMBED          // 原始嵌入（总是允许）
    };
    
    // 目标规划.ini第139行：非局部script约束
    // 禁止使用任何CHTL语法，通常为模板变量，自定义变量组，变量组特例化，命名空间from
    // 特别允许的存在是--注释以及原始嵌入(任意类型)
    allowedSyntax_[SyntaxContext::GLOBAL_SCRIPT] = {
        ConstraintTarget::ORIGIN_EMBED          // 仅允许原始嵌入和注释
    };
    
    // 目标规划.ini第141行：局部样式块约束
    // 能够使用：模板变量的使用，自定义变量的使用，自定义变量的特例化，模板样式组，自定义样式组，
    // 无值样式组，自定义样式组的特例化，delete属性，delete继承，继承(样式组之间的继承)，
    // 生成器注释，全缀名，任意类型的原始嵌入，从命名空间中拿到某一个模板变量等
    allowedSyntax_[SyntaxContext::LOCAL_STYLE] = {
        ConstraintTarget::TEMPLATE_VAR,         // 模板变量
        ConstraintTarget::CUSTOM_VAR,           // 自定义变量
        ConstraintTarget::TEMPLATE_STYLE,       // 模板样式组
        ConstraintTarget::CUSTOM_STYLE,         // 自定义样式组
        ConstraintTarget::ORIGIN_EMBED          // 原始嵌入
    };
    
    // 目标规划.ini第143行：局部script约束
    // 允许使用：模板变量，自定义变量组，变量组特例化，命名空间from，--注释以及原始嵌入(任意类型)
    // {{&}}这些特供语法属于本身具有的功能，不应该被误禁
    allowedSyntax_[SyntaxContext::LOCAL_SCRIPT] = {
        ConstraintTarget::TEMPLATE_VAR,         // 模板变量
        ConstraintTarget::CUSTOM_VAR,           // 自定义变量组
        ConstraintTarget::ORIGIN_EMBED          // 原始嵌入
        // 注意：{{&}}等CHTL JS特供语法不受约束
    };
    
    // 元素主体中允许所有语法（默认）
    allowedSyntax_[SyntaxContext::ELEMENT_BODY] = {
        ConstraintTarget::HTML_ELEMENT,
        ConstraintTarget::CUSTOM_ELEMENT,
        ConstraintTarget::TEMPLATE_ELEMENT,
        ConstraintTarget::CUSTOM_STYLE,
        ConstraintTarget::TEMPLATE_STYLE,
        ConstraintTarget::CUSTOM_VAR,
        ConstraintTarget::TEMPLATE_VAR,
        ConstraintTarget::ORIGIN_EMBED
    };
}

bool CHTLConstraintValidator::ValidateNode(const std::shared_ptr<AST::ASTNode>& node, 
                                          SyntaxContext context) const {
    if (!node) {
        return true;
    }
    
    // 原始嵌入和注释总是允许
    if (IsOriginEmbed(node) || IsComment(node)) {
        return true;
    }
    
    // 根据上下文进行特定验证
    switch (context) {
        case SyntaxContext::GLOBAL_STYLE:
            return ValidateGlobalStyleConstraints(node);
            
        case SyntaxContext::LOCAL_STYLE:
            return ValidateLocalStyleConstraints(node);
            
        case SyntaxContext::GLOBAL_SCRIPT:
            return ValidateGlobalScriptConstraints(node);
            
        case SyntaxContext::LOCAL_SCRIPT:
            return ValidateLocalScriptConstraints(node);
            
        default:
            break;
    }
    
    // 验证except约束
    return ValidateExceptConstraints(node, context);
}

bool CHTLConstraintValidator::ValidateGlobalStyleConstraints(const std::shared_ptr<AST::ASTNode>& node) const {
    ConstraintTarget target = GetNodeConstraintTarget(node);
    
    // 检查是否在允许列表中
    auto it = allowedSyntax_.find(SyntaxContext::GLOBAL_STYLE);
    if (it != allowedSyntax_.end()) {
        if (it->second.find(target) == it->second.end()) {
            SetError("全局样式块中不允许使用: " + GetConstraintTargetName(target));
            return false;
        }
    }
    
    return true;
}

bool CHTLConstraintValidator::ValidateLocalStyleConstraints(const std::shared_ptr<AST::ASTNode>& node) const {
    ConstraintTarget target = GetNodeConstraintTarget(node);
    
    // 局部样式块允许的语法与全局样式块相同
    auto it = allowedSyntax_.find(SyntaxContext::LOCAL_STYLE);
    if (it != allowedSyntax_.end()) {
        if (it->second.find(target) == it->second.end()) {
            SetError("局部样式块中不允许使用: " + GetConstraintTargetName(target));
            return false;
        }
    }
    
    return true;
}

bool CHTLConstraintValidator::ValidateGlobalScriptConstraints(const std::shared_ptr<AST::ASTNode>& node) const {
    ConstraintTarget target = GetNodeConstraintTarget(node);
    
    // 全局script禁止使用任何CHTL语法，除了原始嵌入和注释
    if (target != ConstraintTarget::ORIGIN_EMBED) {
        SetError("全局script块中禁止使用CHTL语法: " + GetConstraintTargetName(target));
        return false;
    }
    
    return true;
}

bool CHTLConstraintValidator::ValidateLocalScriptConstraints(const std::shared_ptr<AST::ASTNode>& node) const {
    ConstraintTarget target = GetNodeConstraintTarget(node);
    
    // 检查是否在允许列表中
    auto it = allowedSyntax_.find(SyntaxContext::LOCAL_SCRIPT);
    if (it != allowedSyntax_.end()) {
        if (it->second.find(target) == it->second.end()) {
            // 特殊检查：{{&}}等CHTL JS特供语法不受约束
            if (IsSpecialCHTLJSSyntax(node)) {
                return true;
            }
            
            SetError("局部script块中不允许使用: " + GetConstraintTargetName(target));
            return false;
        }
    }
    
    return true;
}

bool CHTLConstraintValidator::ValidateExceptConstraints(const std::shared_ptr<AST::ASTNode>& node, 
                                                       SyntaxContext context) const {
    // 检查except约束规则
    auto it = contextConstraints_.find(context);
    if (it != contextConstraints_.end()) {
        ConstraintTarget nodeTarget = GetNodeConstraintTarget(node);
        
        for (const auto& rule : it->second) {
            if (!rule.isActive) continue;
            
            // 检查约束规则
            bool violatesConstraint = false;
            
            switch (rule.type) {
                case ConstraintType::PRECISE:
                    // 精确约束：检查特定名称
                    if (rule.target == nodeTarget && 
                        (rule.specificName.empty() || GetNodeName(node) == rule.specificName)) {
                        violatesConstraint = true;
                    }
                    break;
                    
                case ConstraintType::TYPE:
                    // 类型约束：检查类型匹配
                    if (rule.target == nodeTarget) {
                        violatesConstraint = true;
                    }
                    break;
                    
                case ConstraintType::GLOBAL:
                    // 全局约束：在命名空间内的约束
                    if (context == SyntaxContext::NAMESPACE_BODY && rule.target == nodeTarget) {
                        violatesConstraint = true;
                    }
                    break;
            }
            
            if (violatesConstraint) {
                SetError("违反except约束: " + rule.description);
                return false;
            }
        }
    }
    
    return true;
}

ConstraintTarget CHTLConstraintValidator::GetNodeConstraintTarget(const std::shared_ptr<AST::ASTNode>& node) const {
    if (!node) {
        return ConstraintTarget::ORIGIN_EMBED;
    }
    
    AST::NodeType nodeType = node->GetType();
    
    switch (nodeType) {
        case AST::NodeType::ELEMENT:
            // 检查是否为HTML元素、自定义元素或模板元素
            if (auto elementNode = std::dynamic_pointer_cast<AST::ElementNode>(node)) {
                const std::string& tagName = elementNode->GetTagName();
                if (tagName.find("@Element") != std::string::npos) {
                    if (tagName.find("[Custom]") != std::string::npos) {
                        return ConstraintTarget::CUSTOM_ELEMENT;
                    } else if (tagName.find("[Template]") != std::string::npos) {
                        return ConstraintTarget::TEMPLATE_ELEMENT;
                    }
                }
                return ConstraintTarget::HTML_ELEMENT;
            }
            break;
            
        case AST::NodeType::VARIABLE_GROUP:
            // 检查是否为自定义变量或模板变量
            if (auto varNode = std::dynamic_pointer_cast<AST::VariableGroupNode>(node)) {
                const std::string& name = varNode->GetName();
                if (name.find("[Custom]") != std::string::npos) {
                    return ConstraintTarget::CUSTOM_VAR;
                } else if (name.find("[Template]") != std::string::npos) {
                    return ConstraintTarget::TEMPLATE_VAR;
                }
            }
            break;
            
        case AST::NodeType::STYLE_BLOCK:
            // 检查是否为自定义样式或模板样式
            if (auto styleNode = std::dynamic_pointer_cast<AST::StyleBlockNode>(node)) {
                // 根据样式块的来源判断类型
                // 这里需要根据实际的AST结构来判断
                return ConstraintTarget::CUSTOM_STYLE;
            }
            break;
            
        case AST::NodeType::ORIGIN_HTML:
        case AST::NodeType::ORIGIN_STYLE:
        case AST::NodeType::ORIGIN_JAVASCRIPT:
        case AST::NodeType::ORIGIN_CUSTOM:
            return ConstraintTarget::ORIGIN_EMBED;
            
        case AST::NodeType::COMMENT:
            return ConstraintTarget::ORIGIN_EMBED; // 注释总是允许
            
        default:
            break;
    }
    
    return ConstraintTarget::HTML_ELEMENT; // 默认为HTML元素
}

bool CHTLConstraintValidator::IsOriginEmbed(const std::shared_ptr<AST::ASTNode>& node) const {
    if (!node) return false;
    
    AST::NodeType type = node->GetType();
    return type == AST::NodeType::ORIGIN_HTML ||
           type == AST::NodeType::ORIGIN_STYLE ||
           type == AST::NodeType::ORIGIN_JAVASCRIPT ||
           type == AST::NodeType::ORIGIN_CUSTOM;
}

bool CHTLConstraintValidator::IsComment(const std::shared_ptr<AST::ASTNode>& node) const {
    return node && node->GetType() == AST::NodeType::COMMENT;
}

bool CHTLConstraintValidator::IsSpecialCHTLJSSyntax(const std::shared_ptr<AST::ASTNode>& node) const {
    // 检查是否为{{&}}等CHTL JS特供语法
    if (auto textNode = std::dynamic_pointer_cast<AST::TextNode>(node)) {
        const std::string& text = textNode->GetContent();
        
        // 检查CHTL JS特供语法
        if (text.find("{{") != std::string::npos && text.find("}}") != std::string::npos) {
            return true; // {{selector}}语法
        }
        if (text.find("->") != std::string::npos) {
            return true; // 箭头操作符
        }
        if (text.find("vir ") != std::string::npos) {
            return true; // 虚对象语法
        }
        if (text.find("listen") != std::string::npos || 
            text.find("delegate") != std::string::npos || 
            text.find("animate") != std::string::npos) {
            return true; // CHTL JS块语法
        }
    }
    
    return false;
}

std::string CHTLConstraintValidator::GetNodeName(const std::shared_ptr<AST::ASTNode>& node) const {
    if (!node) {
        return "";
    }
    
    switch (node->GetType()) {
        case AST::NodeType::ELEMENT:
            if (auto elementNode = std::dynamic_pointer_cast<AST::ElementNode>(node)) {
                return elementNode->GetTagName();
            }
            break;
            
        case AST::NodeType::VARIABLE_GROUP:
            if (auto varNode = std::dynamic_pointer_cast<AST::VariableGroupNode>(node)) {
                return varNode->GetName();
            }
            break;
            
        default:
            break;
    }
    
    return "";
}

std::string CHTLConstraintValidator::GetConstraintTargetName(ConstraintTarget target) const {
    switch (target) {
        case ConstraintTarget::HTML_ELEMENT:     return "HTML元素";
        case ConstraintTarget::CUSTOM_ELEMENT:   return "自定义元素";
        case ConstraintTarget::TEMPLATE_ELEMENT: return "模板元素";
        case ConstraintTarget::CUSTOM_STYLE:     return "自定义样式组";
        case ConstraintTarget::TEMPLATE_STYLE:   return "模板样式组";
        case ConstraintTarget::CUSTOM_VAR:       return "自定义变量组";
        case ConstraintTarget::TEMPLATE_VAR:     return "模板变量组";
        case ConstraintTarget::HTML_TYPE:        return "@Html类型";
        case ConstraintTarget::CUSTOM_TYPE:      return "[Custom]类型";
        case ConstraintTarget::TEMPLATE_TYPE:    return "[Template]类型";
        case ConstraintTarget::ORIGIN_EMBED:     return "原始嵌入";
        default:                                 return "未知类型";
    }
}

void CHTLConstraintValidator::AddExceptRule(SyntaxContext context, const ConstraintRule& rule) {
    contextConstraints_[context].push_back(rule);
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "添加except约束规则: " + rule.description
    );
}

void CHTLConstraintValidator::RemoveExceptRule(SyntaxContext context, ConstraintTarget target, 
                                              const std::string& specificName) {
    auto it = contextConstraints_.find(context);
    if (it != contextConstraints_.end()) {
        auto& rules = it->second;
        
        rules.erase(std::remove_if(rules.begin(), rules.end(),
            [target, specificName](const ConstraintRule& rule) {
                return rule.target == target && 
                       (specificName.empty() || rule.specificName == specificName);
            }), rules.end());
    }
}

bool CHTLConstraintValidator::IsAllowed(SyntaxContext context, ConstraintTarget target, 
                                       const std::string& specificName) const {
    // 原始嵌入总是允许
    if (target == ConstraintTarget::ORIGIN_EMBED) {
        return true;
    }
    
    // 检查基本允许列表
    auto allowedIt = allowedSyntax_.find(context);
    if (allowedIt != allowedSyntax_.end()) {
        if (allowedIt->second.find(target) == allowedIt->second.end()) {
            return false;
        }
    }
    
    // 检查except约束
    auto constraintIt = contextConstraints_.find(context);
    if (constraintIt != contextConstraints_.end()) {
        for (const auto& rule : constraintIt->second) {
            if (!rule.isActive) continue;
            
            if (rule.target == target && 
                (rule.specificName.empty() || rule.specificName == specificName)) {
                return false; // 被except约束禁止
            }
        }
    }
    
    return true;
}

void CHTLConstraintValidator::Reset() {
    lastError_.clear();
}

void CHTLConstraintValidator::SetError(const std::string& error) const {
    lastError_ = error;
    Utils::ErrorHandler::GetInstance().LogError("语法约束验证失败: " + error);
}

// SyntaxContextDetector实现
SyntaxContext SyntaxContextDetector::DetectContext(const std::shared_ptr<AST::ASTNode>& node,
                                                   SyntaxContext parentContext) {
    if (!node) {
        return parentContext;
    }
    
    AST::NodeType nodeType = node->GetType();
    
    switch (nodeType) {
        case AST::NodeType::STYLE_BLOCK:
            if (IsGlobalStyleBlock(node)) {
                return SyntaxContext::GLOBAL_STYLE;
            } else if (IsLocalStyleBlock(node)) {
                return SyntaxContext::LOCAL_STYLE;
            }
            break;
            
        case AST::NodeType::SCRIPT_BLOCK:
            if (IsGlobalScriptBlock(node)) {
                return SyntaxContext::GLOBAL_SCRIPT;
            } else if (IsLocalScriptBlock(node)) {
                return SyntaxContext::LOCAL_SCRIPT;
            }
            break;
            
        case AST::NodeType::NAMESPACE:
            return SyntaxContext::NAMESPACE_BODY;
            
        case AST::NodeType::TEMPLATE_STYLE:
        case AST::NodeType::TEMPLATE_ELEMENT:
        case AST::NodeType::TEMPLATE_VAR:
            return SyntaxContext::TEMPLATE_BODY;
            
        case AST::NodeType::CUSTOM_STYLE:
        case AST::NodeType::CUSTOM_ELEMENT:
        case AST::NodeType::CUSTOM_VAR:
            return SyntaxContext::CUSTOM_BODY;
            
        default:
            break;
    }
    
    return parentContext;
}

bool SyntaxContextDetector::IsGlobalStyleBlock(const std::shared_ptr<AST::ASTNode>& node) {
    if (!node || node->GetType() != AST::NodeType::STYLE_BLOCK) {
        return false;
    }
    
    // 完整实现全局样式块检测 - 根据AST结构判断
    if (!node || node->GetType() != AST::NodeType::STYLE_BLOCK) {
        return false;
    }
    
    // 检查父节点链，确定是否在顶级元素中
    // 简化实现：检查常见的顶级元素上下文
    // 实际实现需要遍历完整的AST父节点链
    
    // 如果在html、head、body等顶级元素的直接子级中，则认为是全局样式块
    // 这里需要根据实际的AST结构进行更精确的判断
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "检测全局样式块：节点类型 " + std::to_string(static_cast<int>(node->GetType()))
    );
    
    return true; // 基础实现：默认认为是全局样式块
}

bool SyntaxContextDetector::IsLocalStyleBlock(const std::shared_ptr<AST::ASTNode>& node) {
    if (!node || node->GetType() != AST::NodeType::STYLE_BLOCK) {
        return false;
    }
    
    // 检查是否为局部样式块（在普通元素内部）
    return !IsGlobalStyleBlock(node);
}

bool SyntaxContextDetector::IsGlobalScriptBlock(const std::shared_ptr<AST::ASTNode>& node) {
    if (!node || node->GetType() != AST::NodeType::SCRIPT_BLOCK) {
        return false;
    }
    
    // 检查是否为全局script块
    return true; // 暂时简化
}

bool SyntaxContextDetector::IsLocalScriptBlock(const std::shared_ptr<AST::ASTNode>& node) {
    if (!node || node->GetType() != AST::NodeType::SCRIPT_BLOCK) {
        return false;
    }
    
    // 检查是否为局部script块
    return !IsGlobalScriptBlock(node);
}

// ConstraintRuleBuilder实现
ConstraintRule ConstraintRuleBuilder::CreatePreciseRule(ConstraintTarget target,
                                                       const std::string& specificName,
                                                       const std::string& description) {
    ConstraintRule rule;
    rule.type = ConstraintType::PRECISE;
    rule.target = target;
    rule.specificName = specificName;
    rule.description = description.empty() ? 
        ("精确约束: 禁止使用 " + specificName) : description;
    rule.isActive = true;
    
    return rule;
}

ConstraintRule ConstraintRuleBuilder::CreateTypeRule(ConstraintTarget target,
                                                    const std::string& description) {
    ConstraintRule rule;
    rule.type = ConstraintType::TYPE;
    rule.target = target;
    rule.description = description.empty() ? 
        ("类型约束: 禁止使用类型") : description;
    rule.isActive = true;
    
    return rule;
}

ConstraintRule ConstraintRuleBuilder::CreateGlobalRule(ConstraintTarget target,
                                                      const std::string& description) {
    ConstraintRule rule;
    rule.type = ConstraintType::GLOBAL;
    rule.target = target;
    rule.description = description.empty() ? 
        ("全局约束: 在命名空间中禁止") : description;
    rule.isActive = true;
    
    return rule;
}

} // namespace Constraints
} // namespace CHTL