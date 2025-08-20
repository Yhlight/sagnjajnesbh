#pragma once

#include "chtl_js/ast/CHTLJSASTNode.h"

namespace chtl {
namespace chtljs {
namespace ast {

/**
 * 函数声明节点
 * 表示 function(params) { body }
 */
class FunctionDeclarationNode : public CHTLJSASTNode {
public:
    FunctionDeclarationNode(const std::string& name = "")
        : CHTLJSASTNode(CHTLJSNodeType::FUNCTION_DECLARATION, name) {}
    
    void Accept(CHTLJSVisitor* visitor) override;
    
    // 函数名（可选，匿名函数为空）
    const std::string& GetFunctionName() const { return m_Value; }
    bool IsAnonymous() const { return m_Value.empty(); }
    
    // 参数列表
    void SetParameters(std::shared_ptr<class ParameterListNode> params) {
        m_Parameters = params;
        if (params) {
            AddChild(params);
        }
    }
    std::shared_ptr<class ParameterListNode> GetParameters() const { return m_Parameters; }
    
    // 函数体
    void SetBody(std::shared_ptr<class BlockStatementNode> body) {
        m_Body = body;
        if (body) {
            AddChild(body);
        }
    }
    std::shared_ptr<class BlockStatementNode> GetBody() const { return m_Body; }
    
private:
    std::shared_ptr<class ParameterListNode> m_Parameters;
    std::shared_ptr<class BlockStatementNode> m_Body;
};

/**
 * 箭头函数节点
 * 表示 (params) => { body } 或 param => expr
 */
class ArrowFunctionNode : public CHTLJSASTNode {
public:
    ArrowFunctionNode()
        : CHTLJSASTNode(CHTLJSNodeType::ARROW_FUNCTION) {}
    
    void Accept(CHTLJSVisitor* visitor) override;
    
    // 参数列表
    void SetParameters(std::shared_ptr<class ParameterListNode> params) {
        m_Parameters = params;
        if (params) {
            AddChild(params);
        }
    }
    std::shared_ptr<class ParameterListNode> GetParameters() const { return m_Parameters; }
    
    // 函数体（可能是块语句或表达式）
    void SetBody(std::shared_ptr<CHTLJSASTNode> body) {
        m_Body = body;
        if (body) {
            AddChild(body);
        }
    }
    std::shared_ptr<CHTLJSASTNode> GetBody() const { return m_Body; }
    
    // 是否是表达式形式（没有花括号）
    bool IsExpression() const { return m_IsExpression; }
    void SetIsExpression(bool isExpr) { m_IsExpression = isExpr; }
    
private:
    std::shared_ptr<class ParameterListNode> m_Parameters;
    std::shared_ptr<CHTLJSASTNode> m_Body;
    bool m_IsExpression = false;
};

/**
 * 参数列表节点
 * 表示 (param1, param2, ...)
 */
class ParameterListNode : public CHTLJSASTNode {
public:
    ParameterListNode()
        : CHTLJSASTNode(CHTLJSNodeType::PARAMETER_LIST) {}
    
    void Accept(CHTLJSVisitor* visitor) override;
    
    // 添加参数
    void AddParameter(const std::string& param) {
        m_Parameters.push_back(param);
    }
    
    // 获取参数列表
    const std::vector<std::string>& GetParameters() const { return m_Parameters; }
    size_t GetParameterCount() const { return m_Parameters.size(); }
    
private:
    std::vector<std::string> m_Parameters;
};

/**
 * 块语句节点
 * 表示 { statements... }
 */
class BlockStatementNode : public CHTLJSASTNode {
public:
    BlockStatementNode()
        : CHTLJSASTNode(CHTLJSNodeType::BLOCK_STATEMENT) {}
    
    void Accept(CHTLJSVisitor* visitor) override;
    
    // 语句通过子节点管理
};

/**
 * 表达式语句节点
 * 表示单个表达式语句
 */
class ExpressionStatementNode : public CHTLJSASTNode {
public:
    ExpressionStatementNode()
        : CHTLJSASTNode(CHTLJSNodeType::EXPRESSION_STATEMENT) {}
    
    void Accept(CHTLJSVisitor* visitor) override;
    
    // 表达式
    void SetExpression(std::shared_ptr<CHTLJSASTNode> expr) {
        m_Expression = expr;
        if (expr) {
            AddChild(expr);
        }
    }
    std::shared_ptr<CHTLJSASTNode> GetExpression() const { return m_Expression; }
    
private:
    std::shared_ptr<CHTLJSASTNode> m_Expression;
};

} // namespace ast
} // namespace chtljs
} // namespace chtl