#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace chtl {
namespace chtljs {
namespace ast {

// CHTL JS AST节点类型
enum class CHTLJSNodeType {
    // 文档根节点
    DOCUMENT,
    
    // Script块
    SCRIPT_BLOCK,           // script { } 块
    
    // 增强选择器
    ENHANCED_SELECTOR,      // {{selector}}
    SELECTOR_ACCESS,        // {{selector}}[index]
    
    // CHTL JS操作符
    ARROW_OPERATOR,         // -> 操作符
    DOT_OPERATOR,          // . 操作符（与->等价）
    
    // CHTL JS特殊函数
    LISTEN_CALL,           // listen({ })
    DELEGATE_CALL,         // delegate({ })
    ANIMATE_CALL,          // animate({ })
    I_NEVER_AWAY_CALL,     // iNeverAway({ })
    PRINT_MY_LOVE_CALL,    // printMylove({ })
    
    // 虚对象
    VIR_DECLARATION,       // vir name = ...
    VIR_ACCESS,            // virObject->method()
    
    // 状态标签
    STATE_TAG,             // <State>
    FUNCTION_WITH_STATE,   // functionName<State>
    
    // 对象字面量（用于配置）
    OBJECT_LITERAL,        // { key: value, ... }
    PROPERTY,              // key: value
    
    // 函数和参数
    FUNCTION_DECLARATION,  // function() { }
    ARROW_FUNCTION,        // () => { }
    PARAMETER_LIST,        // (param1, param2)
    
    // 表达式
    IDENTIFIER,            // 标识符
    STRING_LITERAL,        // 字符串
    NUMBER_LITERAL,        // 数字
    BOOLEAN_LITERAL,       // true/false
    ARRAY_LITERAL,         // [item1, item2]
    
    // 语句
    EXPRESSION_STATEMENT,  // 表达式语句
    BLOCK_STATEMENT,       // { }
    
    // JavaScript代码片段（原始JS）
    JS_CODE_FRAGMENT,      // 纯JavaScript代码
    
    // 错误节点
    ERROR
};

// 前向声明
class CHTLJSVisitor;

/**
 * CHTL JS AST节点基类
 * 所有CHTL JS AST节点都继承自此类
 * 注意：CHTL JS AST完全独立于CHTL AST
 */
class CHTLJSASTNode {
public:
    CHTLJSASTNode(CHTLJSNodeType type, const std::string& value = "")
        : m_Type(type), m_Value(value), m_Line(0), m_Column(0) {}
        
    virtual ~CHTLJSASTNode() = default;
    
    // 获取节点类型
    CHTLJSNodeType GetType() const { return m_Type; }
    
    // 获取节点值
    const std::string& GetValue() const { return m_Value; }
    void SetValue(const std::string& value) { m_Value = value; }
    
    // 位置信息
    void SetLocation(size_t line, size_t column) {
        m_Line = line;
        m_Column = column;
    }
    size_t GetLine() const { return m_Line; }
    size_t GetColumn() const { return m_Column; }
    
    // 子节点管理
    void AddChild(std::shared_ptr<CHTLJSASTNode> child);
    const std::vector<std::shared_ptr<CHTLJSASTNode>>& GetChildren() const { return m_Children; }
    std::shared_ptr<CHTLJSASTNode> GetChild(size_t index) const;
    size_t GetChildCount() const { return m_Children.size(); }
    
    // 属性管理
    void SetAttribute(const std::string& key, const std::string& value);
    std::string GetAttribute(const std::string& key) const;
    bool HasAttribute(const std::string& key) const;
    const std::unordered_map<std::string, std::string>& GetAttributes() const { return m_Attributes; }
    
    // 访问者模式
    virtual void Accept(CHTLJSVisitor* visitor) = 0;
    
    // 获取节点类型的字符串表示
    static std::string TypeToString(CHTLJSNodeType type);
    std::string GetTypeString() const { return TypeToString(m_Type); }
    
    // 调试输出
    virtual std::string ToString() const;
    virtual void Print(int indent = 0) const;
    
protected:
    CHTLJSNodeType m_Type;
    std::string m_Value;
    size_t m_Line;
    size_t m_Column;
    std::vector<std::shared_ptr<CHTLJSASTNode>> m_Children;
    std::unordered_map<std::string, std::string> m_Attributes;
};

/**
 * CHTL JS AST访问者接口
 * 用于遍历和处理CHTL JS AST
 */
class CHTLJSVisitor {
public:
    virtual ~CHTLJSVisitor() = default;
    
    // 访问方法
    virtual void VisitDocument(class CHTLJSDocumentNode* node) = 0;
    virtual void VisitScriptBlock(class ScriptBlockNode* node) = 0;
    virtual void VisitEnhancedSelector(class EnhancedSelectorNode* node) = 0;
    virtual void VisitArrowOperator(class ArrowOperatorNode* node) = 0;
    virtual void VisitListenCall(class ListenCallNode* node) = 0;
    virtual void VisitDelegateCall(class DelegateCallNode* node) = 0;
    virtual void VisitAnimateCall(class AnimateCallNode* node) = 0;
    virtual void VisitINeverAwayCall(class INeverAwayCallNode* node) = 0;
    virtual void VisitPrintMyLoveCall(class PrintMyLoveCallNode* node) = 0;
    virtual void VisitVirDeclaration(class VirDeclarationNode* node) = 0;
    virtual void VisitVirAccess(class VirAccessNode* node) = 0;
    virtual void VisitStateTag(class StateTagNode* node) = 0;
    virtual void VisitObjectLiteral(class ObjectLiteralNode* node) = 0;
    virtual void VisitProperty(class PropertyNode* node) = 0;
    virtual void VisitIdentifier(class IdentifierNode* node) = 0;
    virtual void VisitLiteral(class LiteralNode* node) = 0;
    virtual void VisitJSCodeFragment(class JSCodeFragmentNode* node) = 0;
};

} // namespace ast
} // namespace chtljs
} // namespace chtl