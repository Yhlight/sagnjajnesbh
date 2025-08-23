#include "CHTL/Constraints/SyntaxBoundaryValidator.h"

namespace CHTL {
namespace Constraints {

SyntaxBoundaryValidator::SyntaxBoundaryValidator() {
    InitializeContextPermissions();
}

void SyntaxBoundaryValidator::InitializeContextPermissions() {
    // 任意地方都可以写的语法（生成器注释和原始嵌入）
    alwaysAllowed_ = {
        AST::NodeType::GENERATOR_COMMENT,
        AST::NodeType::ORIGIN_HTML,
        AST::NodeType::ORIGIN_STYLE,
        AST::NodeType::ORIGIN_JAVASCRIPT
    };
    
    // 全局样式块允许的语法 - 目标规划.ini第137行
    globalStyleAllowed_ = {
        AST::NodeType::TEMPLATE_VAR,        // 模板变量的使用
        AST::NodeType::CUSTOM_VAR,          // 自定义变量的使用
        AST::NodeType::CUSTOM_VAR,          // 自定义变量的特例化（通过CUSTOM_VAR节点处理）
        AST::NodeType::TEMPLATE_STYLE,      // 模板样式组
        AST::NodeType::CUSTOM_STYLE,        // 自定义样式组
        AST::NodeType::CUSTOM_STYLE,        // 无值样式组（通过CUSTOM_STYLE节点处理）
        AST::NodeType::CUSTOM_STYLE,        // 自定义样式组的特例化（通过CUSTOM_STYLE节点处理）
        AST::NodeType::DELETE,              // delete属性
        AST::NodeType::INHERITANCE,         // delete继承，继承(样式组之间的继承)
        AST::NodeType::NAMESPACE_ACCESS,    // 从命名空间中拿到某一个模板变量，自定义变量，模板样式组，自定义样式组，无值样式组
        AST::NodeType::CSS_PROPERTY,       // CSS属性
        AST::NodeType::CSS_SELECTOR        // CSS选择器
    };
    
    // 局部样式块允许的语法 - 目标规划.ini第141行
    localStyleAllowed_ = globalStyleAllowed_; // 与全局样式块相同的语法
    localStyleAllowed_.insert({
        AST::NodeType::REFERENCE            // & 引用选择器（style中使用&）
    });
    
    // 局部脚本允许的语法 - 目标规划.ini第143行
    localScriptAllowed_ = {
        AST::NodeType::TEMPLATE_VAR,        // 模板变量
        AST::NodeType::CUSTOM_VAR,          // 自定义变量组
        AST::NodeType::CUSTOM_VAR,          // 变量组特例化
        AST::NodeType::NAMESPACE_ACCESS,    // 命名空间from
        AST::NodeType::CHTL_JS,             // CHTL JS语法（{{&}}等特供语法）
        AST::NodeType::JAVASCRIPT,          // JavaScript代码
        AST::NodeType::REFERENCE            // {{&}} 引用选择器（script中使用{{&}}）
    };
    
    // 全局脚本（除局部script外的其他script）允许的语法 - 目标规划.ini第139行
    globalScriptAllowed_ = {
        AST::NodeType::JAVASCRIPT           // 只允许纯JavaScript，禁止使用任何CHTL语法
    };
}

bool SyntaxBoundaryValidator::ValidateNodeInContext(std::shared_ptr<AST::ASTNode> node, 
                                                   SyntaxBoundaryContext context) {
    if (!node) {
        return false;
    }
    
    // 检查是否为任意地方都允许的语法
    if (IsGeneratorComment(node) || IsOriginEmbedding(node)) {
        return true;
    }
    
    // 检查CHTL JS特供语法（在局部script中允许）
    if (context == SyntaxBoundaryContext::LOCAL_SCRIPT && IsCHTLJSNativeSyntax(node)) {
        return true;
    }
    
    AST::NodeType nodeType = node->GetType();
    
    // 根据上下文验证语法
    bool isAllowed = false;
    
    switch (context) {
        case SyntaxBoundaryContext::GLOBAL_STYLE:
            isAllowed = globalStyleAllowed_.find(nodeType) != globalStyleAllowed_.end();
            break;
            
        case SyntaxBoundaryContext::LOCAL_STYLE:
            isAllowed = localStyleAllowed_.find(nodeType) != localStyleAllowed_.end();
            break;
            
        case SyntaxBoundaryContext::LOCAL_SCRIPT:
            isAllowed = localScriptAllowed_.find(nodeType) != localScriptAllowed_.end();
            break;
            
        case SyntaxBoundaryContext::GLOBAL_SCRIPT:
            isAllowed = globalScriptAllowed_.find(nodeType) != globalScriptAllowed_.end();
            break;
            
        default:
            // 其他上下文暂时允许所有语法
            isAllowed = true;
            break;
    }
    
    if (!isAllowed) {
        BoundaryViolation violation;
        violation.type = BoundaryViolationType::FORBIDDEN_SYNTAX;
        violation.context = context;
        violation.nodeName = GetNodeTypeName(nodeType);
        violation.description = "语法 '" + violation.nodeName + "' 在当前上下文中不被允许";
        
        RecordViolation(violation);
    }
    
    return isAllowed;
}

bool SyntaxBoundaryValidator::IsNodeTypeAllowed(AST::NodeType nodeType, SyntaxBoundaryContext context) {
    // 检查任意地方都允许的类型
    if (alwaysAllowed_.find(nodeType) != alwaysAllowed_.end()) {
        return true;
    }
    
    switch (context) {
        case SyntaxBoundaryContext::GLOBAL_STYLE:
            return globalStyleAllowed_.find(nodeType) != globalStyleAllowed_.end();
            
        case SyntaxBoundaryContext::LOCAL_STYLE:
            return localStyleAllowed_.find(nodeType) != localStyleAllowed_.end();
            
        case SyntaxBoundaryContext::LOCAL_SCRIPT:
            return localScriptAllowed_.find(nodeType) != localScriptAllowed_.end();
            
        case SyntaxBoundaryContext::GLOBAL_SCRIPT:
            return globalScriptAllowed_.find(nodeType) != globalScriptAllowed_.end();
            
        default:
            return true;
    }
}

const std::unordered_set<AST::NodeType>& SyntaxBoundaryValidator::GetAllowedNodeTypes(SyntaxBoundaryContext context) {
    switch (context) {
        case SyntaxBoundaryContext::GLOBAL_STYLE:
            return globalStyleAllowed_;
        case SyntaxBoundaryContext::LOCAL_STYLE:
            return localStyleAllowed_;
        case SyntaxBoundaryContext::LOCAL_SCRIPT:
            return localScriptAllowed_;
        case SyntaxBoundaryContext::GLOBAL_SCRIPT:
            return globalScriptAllowed_;
        default:
            return alwaysAllowed_;
    }
}

void SyntaxBoundaryValidator::RecordViolation(const BoundaryViolation& violation) {
    violations_.push_back(violation);
}

bool SyntaxBoundaryValidator::IsGeneratorComment(std::shared_ptr<AST::ASTNode> node) {
    if (node->GetType() == AST::NodeType::COMMENT) {
        auto commentNode = std::dynamic_pointer_cast<AST::CommentNode>(node);
        return commentNode && commentNode->GetCommentType() == AST::CommentNode::CommentType::GENERATOR;
    }
    return false;
}

bool SyntaxBoundaryValidator::IsOriginEmbedding(std::shared_ptr<AST::ASTNode> node) {
    AST::NodeType type = node->GetType();
    return type == AST::NodeType::ORIGIN_HTML ||
           type == AST::NodeType::ORIGIN_STYLE ||
           type == AST::NodeType::ORIGIN_JAVASCRIPT;
}

bool SyntaxBoundaryValidator::IsCHTLJSNativeSyntax(std::shared_ptr<AST::ASTNode> node) {
    AST::NodeType type = node->GetType();
    
    // CHTL JS特供语法：{{&}}、{{.box}}、{{#box}}等
    if (type == AST::NodeType::CHTL_JS || type == AST::NodeType::REFERENCE) {
        // 检查是否为引用选择器 {{&}}
        if (type == AST::NodeType::REFERENCE) {
            auto refNode = std::dynamic_pointer_cast<AST::ReferenceNode>(node);
            return refNode && refNode->GetReferenceType() == AST::ReferenceNode::ReferenceType::SCRIPT_REFERENCE;
        }
        return true;
    }
    
    return false;
}

std::string SyntaxBoundaryValidator::GetNodeTypeName(AST::NodeType nodeType) {
    switch (nodeType) {
        case AST::NodeType::TEMPLATE_VAR: return "@Var模板";
        case AST::NodeType::CUSTOM_VAR: return "@Var自定义";
        case AST::NodeType::TEMPLATE_STYLE: return "@Style模板";
        case AST::NodeType::CUSTOM_STYLE: return "@Style自定义";
        case AST::NodeType::TEMPLATE_ELEMENT: return "@Element模板";
        case AST::NodeType::CUSTOM_ELEMENT: return "@Element自定义";
        case AST::NodeType::DELETE: return "delete";
        case AST::NodeType::INHERITANCE: return "inherit";
        case AST::NodeType::NAMESPACE_ACCESS: return "命名空间访问";
        case AST::NodeType::REFERENCE: return "引用选择器";
        case AST::NodeType::CHTL_JS: return "CHTL JS";
        case AST::NodeType::JAVASCRIPT: return "JavaScript";
        case AST::NodeType::CSS_PROPERTY: return "CSS属性";
        case AST::NodeType::CSS_SELECTOR: return "CSS选择器";
        case AST::NodeType::COMMENT: return "注释";
        case AST::NodeType::ORIGIN_HTML: return "原始HTML嵌入";
        case AST::NodeType::ORIGIN_STYLE: return "原始样式嵌入";
        case AST::NodeType::ORIGIN_JAVASCRIPT: return "原始JavaScript嵌入";
        default: return "未知节点类型";
    }
}

} // namespace Constraints
} // namespace CHTL