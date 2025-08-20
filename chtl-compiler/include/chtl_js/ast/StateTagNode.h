#pragma once

#include "chtl_js/ast/CHTLJSASTNode.h"

namespace chtl {
namespace chtljs {
namespace ast {

/**
 * 状态标签节点
 * 表示 <State> 语法，用于iNeverAway中的函数重载
 */
class StateTagNode : public CHTLJSASTNode {
public:
    StateTagNode(const std::string& state)
        : CHTLJSASTNode(CHTLJSNodeType::STATE_TAG, state) {}
    
    void Accept(CHTLJSVisitor* visitor) override;
    
    // 状态名称
    const std::string& GetStateName() const { return m_Value; }
    
    // 是否用于函数声明
    void SetIsForDeclaration(bool forDecl) { m_IsForDeclaration = forDecl; }
    bool IsForDeclaration() const { return m_IsForDeclaration; }
    
    // 是否用于函数调用
    void SetIsForCall(bool forCall) { m_IsForCall = forCall; }
    bool IsForCall() const { return m_IsForCall; }
    
private:
    bool m_IsForDeclaration = false;
    bool m_IsForCall = false;
};

/**
 * 带状态的函数节点
 * 表示 functionName<State> 语法
 */
class FunctionWithStateNode : public CHTLJSASTNode {
public:
    FunctionWithStateNode(const std::string& funcName)
        : CHTLJSASTNode(CHTLJSNodeType::FUNCTION_WITH_STATE, funcName) {}
    
    void Accept(CHTLJSVisitor* visitor) override;
    
    // 函数名
    const std::string& GetFunctionName() const { return m_Value; }
    
    // 状态标签
    void SetStateTag(std::shared_ptr<StateTagNode> state) {
        m_StateTag = state;
        if (state) {
            AddChild(state);
        }
    }
    std::shared_ptr<StateTagNode> GetStateTag() const { return m_StateTag; }
    
    // 函数体（用于声明）
    void SetFunctionBody(std::shared_ptr<CHTLJSASTNode> body) {
        m_FunctionBody = body;
        if (body) {
            AddChild(body);
        }
    }
    std::shared_ptr<CHTLJSASTNode> GetFunctionBody() const { return m_FunctionBody; }
    
    // 参数列表（用于声明）
    void SetParameters(std::shared_ptr<CHTLJSASTNode> params) {
        m_Parameters = params;
        if (params) {
            AddChild(params);
        }
    }
    std::shared_ptr<CHTLJSASTNode> GetParameters() const { return m_Parameters; }
    
    // 是否是声明还是调用
    void SetIsDeclaration(bool isDecl) { m_IsDeclaration = isDecl; }
    bool IsDeclaration() const { return m_IsDeclaration; }
    
    // 调用参数（用于调用）
    void SetArguments(std::shared_ptr<CHTLJSASTNode> args) {
        m_Arguments = args;
        if (args) {
            AddChild(args);
        }
    }
    std::shared_ptr<CHTLJSASTNode> GetArguments() const { return m_Arguments; }
    
private:
    std::shared_ptr<StateTagNode> m_StateTag;
    std::shared_ptr<CHTLJSASTNode> m_FunctionBody;
    std::shared_ptr<CHTLJSASTNode> m_Parameters;
    std::shared_ptr<CHTLJSASTNode> m_Arguments;
    bool m_IsDeclaration = false;
};

} // namespace ast
} // namespace chtljs
} // namespace chtl