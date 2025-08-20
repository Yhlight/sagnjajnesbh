#include "chtl_js/ast/CHTLJSDocumentNode.h"
#include "chtl_js/ast/ScriptBlockNode.h"
#include "chtl_js/ast/EnhancedSelectorNode.h"
#include "chtl_js/ast/OperatorNodes.h"
#include "chtl_js/ast/SpecialFunctionNodes.h"
#include "chtl_js/ast/VirNodes.h"
#include "chtl_js/ast/StateTagNode.h"
#include "chtl_js/ast/ObjectLiteralNode.h"
#include "chtl_js/ast/LiteralNodes.h"
#include "chtl_js/ast/FunctionNodes.h"
#include "chtl_js/ast/JSCodeFragmentNode.h"

namespace chtl {
namespace chtljs {
namespace ast {

// CHTLJSDocumentNode
void CHTLJSDocumentNode::Accept(CHTLJSVisitor* visitor) {
    visitor->VisitDocument(this);
}

// ScriptBlockNode
void ScriptBlockNode::Accept(CHTLJSVisitor* visitor) {
    visitor->VisitScriptBlock(this);
}

// EnhancedSelectorNode
void EnhancedSelectorNode::Accept(CHTLJSVisitor* visitor) {
    visitor->VisitEnhancedSelector(this);
}

void EnhancedSelectorNode::ParseSelectorType() {
    if (m_Value.empty()) {
        m_SelectorType = AUTO;
        return;
    }
    
    // 检查是否有索引
    size_t bracketPos = m_Value.find('[');
    if (bracketPos != std::string::npos) {
        m_SelectorType = INDEXED;
        // 提取索引
        size_t endBracket = m_Value.find(']', bracketPos);
        if (endBracket != std::string::npos) {
            std::string indexStr = m_Value.substr(bracketPos + 1, endBracket - bracketPos - 1);
            try {
                m_Index = std::stoi(indexStr);
            } catch (...) {
                m_Index = -1;
            }
        }
        return;
    }
    
    // 检查是否是复合选择器（包含空格）
    if (m_Value.find(' ') != std::string::npos) {
        m_SelectorType = DESCENDANT;
        m_IsCompound = true;
        // TODO: 解析各个部分
        return;
    }
    
    // 简单选择器
    if (m_Value[0] == '.') {
        m_SelectorType = CLASS;
    } else if (m_Value[0] == '#') {
        m_SelectorType = ID;
    } else if (std::isalpha(m_Value[0])) {
        m_SelectorType = TAG;
    } else {
        m_SelectorType = AUTO;
    }
}

// SelectorAccessNode
void SelectorAccessNode::Accept(CHTLJSVisitor* visitor) {
    visitor->VisitEnhancedSelector(this);
}

// ArrowOperatorNode
void ArrowOperatorNode::Accept(CHTLJSVisitor* visitor) {
    visitor->VisitArrowOperator(this);
}

// DotOperatorNode
void DotOperatorNode::Accept(CHTLJSVisitor* visitor) {
    visitor->VisitArrowOperator(this); // 使用相同的访问方法
}

// ListenCallNode
void ListenCallNode::Accept(CHTLJSVisitor* visitor) {
    visitor->VisitListenCall(this);
}

// DelegateCallNode
void DelegateCallNode::Accept(CHTLJSVisitor* visitor) {
    visitor->VisitDelegateCall(this);
}

// AnimateCallNode
void AnimateCallNode::Accept(CHTLJSVisitor* visitor) {
    visitor->VisitAnimateCall(this);
}

// INeverAwayCallNode
void INeverAwayCallNode::Accept(CHTLJSVisitor* visitor) {
    visitor->VisitINeverAwayCall(this);
}

// PrintMyLoveCallNode
void PrintMyLoveCallNode::Accept(CHTLJSVisitor* visitor) {
    visitor->VisitPrintMyLoveCall(this);
}

// VirDeclarationNode
void VirDeclarationNode::Accept(CHTLJSVisitor* visitor) {
    visitor->VisitVirDeclaration(this);
}

// VirAccessNode
void VirAccessNode::Accept(CHTLJSVisitor* visitor) {
    visitor->VisitVirAccess(this);
}

// StateTagNode
void StateTagNode::Accept(CHTLJSVisitor* visitor) {
    visitor->VisitStateTag(this);
}

// FunctionWithStateNode
void FunctionWithStateNode::Accept(CHTLJSVisitor* visitor) {
    // 通过父节点访问
}

// ObjectLiteralNode
void ObjectLiteralNode::Accept(CHTLJSVisitor* visitor) {
    visitor->VisitObjectLiteral(this);
}

void ObjectLiteralNode::AddProperty(std::shared_ptr<PropertyNode> property) {
    if (property) {
        m_Properties.push_back(property);
        m_PropertyMap[property->GetKey()] = property;
        AddChild(property);
    }
}

std::shared_ptr<PropertyNode> ObjectLiteralNode::GetProperty(const std::string& key) const {
    auto it = m_PropertyMap.find(key);
    if (it != m_PropertyMap.end()) {
        return it->second;
    }
    return nullptr;
}

bool ObjectLiteralNode::HasProperty(const std::string& key) const {
    return m_PropertyMap.find(key) != m_PropertyMap.end();
}

// PropertyNode
void PropertyNode::Accept(CHTLJSVisitor* visitor) {
    visitor->VisitProperty(this);
}

bool PropertyNode::IsFunctionProperty() const {
    if (!m_PropertyValue) return false;
    auto type = m_PropertyValue->GetType();
    return type == CHTLJSNodeType::FUNCTION_DECLARATION || 
           type == CHTLJSNodeType::ARROW_FUNCTION;
}

bool PropertyNode::IsObjectProperty() const {
    if (!m_PropertyValue) return false;
    return m_PropertyValue->GetType() == CHTLJSNodeType::OBJECT_LITERAL;
}

bool PropertyNode::IsArrayProperty() const {
    if (!m_PropertyValue) return false;
    return m_PropertyValue->GetType() == CHTLJSNodeType::ARRAY_LITERAL;
}

// LiteralNode
void LiteralNode::Accept(CHTLJSVisitor* visitor) {
    visitor->VisitLiteral(this);
}

// ArrayLiteralNode
void ArrayLiteralNode::Accept(CHTLJSVisitor* visitor) {
    visitor->VisitLiteral(this);
}

// FunctionDeclarationNode
void FunctionDeclarationNode::Accept(CHTLJSVisitor* visitor) {
    // 通过父节点访问
}

// ArrowFunctionNode
void ArrowFunctionNode::Accept(CHTLJSVisitor* visitor) {
    // 通过父节点访问
}

// ParameterListNode
void ParameterListNode::Accept(CHTLJSVisitor* visitor) {
    // 通过父节点访问
}

// BlockStatementNode
void BlockStatementNode::Accept(CHTLJSVisitor* visitor) {
    // 通过父节点访问
}

// ExpressionStatementNode
void ExpressionStatementNode::Accept(CHTLJSVisitor* visitor) {
    // 通过父节点访问
}

// JSCodeFragmentNode
void JSCodeFragmentNode::Accept(CHTLJSVisitor* visitor) {
    visitor->VisitJSCodeFragment(this);
}

} // namespace ast
} // namespace chtljs
} // namespace chtl