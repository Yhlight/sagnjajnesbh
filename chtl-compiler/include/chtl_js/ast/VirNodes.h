#pragma once

#include "chtl_js/ast/CHTLJSASTNode.h"

namespace chtl {
namespace chtljs {
namespace ast {

/**
 * 虚对象声明节点
 * 表示 vir name = ...
 */
class VirDeclarationNode : public CHTLJSASTNode {
public:
    VirDeclarationNode(const std::string& name)
        : CHTLJSASTNode(CHTLJSNodeType::VIR_DECLARATION, name) {}
    
    void Accept(CHTLJSVisitor* visitor) override;
    
    // 虚对象名称
    const std::string& GetVirName() const { return m_Value; }
    
    // 初始化表达式（通常是CHTL JS函数调用）
    void SetInitializer(std::shared_ptr<CHTLJSASTNode> initializer) {
        m_Initializer = initializer;
        if (initializer) {
            AddChild(initializer);
        }
    }
    std::shared_ptr<CHTLJSASTNode> GetInitializer() const { return m_Initializer; }
    
    // 记录的函数键（用于后续的虚对象访问）
    void AddFunctionKey(const std::string& key) { m_FunctionKeys.push_back(key); }
    const std::vector<std::string>& GetFunctionKeys() const { return m_FunctionKeys; }
    
private:
    std::shared_ptr<CHTLJSASTNode> m_Initializer;
    std::vector<std::string> m_FunctionKeys;
};

/**
 * 虚对象访问节点
 * 表示 virObject->method() 或 virObject.method()
 */
class VirAccessNode : public CHTLJSASTNode {
public:
    VirAccessNode()
        : CHTLJSASTNode(CHTLJSNodeType::VIR_ACCESS) {}
    
    void Accept(CHTLJSVisitor* visitor) override;
    
    // 虚对象名称
    void SetVirObject(const std::string& name) { m_VirObject = name; }
    const std::string& GetVirObject() const { return m_VirObject; }
    
    // 访问的方法或属性
    void SetMember(const std::string& member) { m_Member = member; }
    const std::string& GetMember() const { return m_Member; }
    
    // 是否使用箭头操作符
    void SetUseArrow(bool useArrow) { m_UseArrow = useArrow; }
    bool UseArrow() const { return m_UseArrow; }
    
    // 是否是方法调用
    void SetIsMethodCall(bool isCall) { m_IsMethodCall = isCall; }
    bool IsMethodCall() const { return m_IsMethodCall; }
    
    // 方法参数（如果是方法调用）
    void SetArguments(std::shared_ptr<CHTLJSASTNode> args) {
        m_Arguments = args;
        if (args) {
            AddChild(args);
        }
    }
    std::shared_ptr<CHTLJSASTNode> GetArguments() const { return m_Arguments; }
    
private:
    std::string m_VirObject;
    std::string m_Member;
    bool m_UseArrow = true;
    bool m_IsMethodCall = false;
    std::shared_ptr<CHTLJSASTNode> m_Arguments;
};

} // namespace ast
} // namespace chtljs
} // namespace chtl