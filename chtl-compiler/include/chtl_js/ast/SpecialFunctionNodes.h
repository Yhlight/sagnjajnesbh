#pragma once

#include "chtl_js/ast/CHTLJSASTNode.h"

namespace chtl {
namespace chtljs {
namespace ast {

/**
 * listen调用节点
 * 表示 listen({ }) 增强监听器
 */
class ListenCallNode : public CHTLJSASTNode {
public:
    ListenCallNode()
        : CHTLJSASTNode(CHTLJSNodeType::LISTEN_CALL, "listen") {}
    
    void Accept(CHTLJSVisitor* visitor) override;
    
    // 配置对象（包含事件处理器）
    void SetConfig(std::shared_ptr<class ObjectLiteralNode> config) {
        m_Config = config;
        if (config) {
            AddChild(config);
        }
    }
    std::shared_ptr<class ObjectLiteralNode> GetConfig() const { return m_Config; }
    
    // 目标元素（被监听的元素）
    void SetTarget(std::shared_ptr<CHTLJSASTNode> target) {
        m_Target = target;
        if (target) {
            AddChild(target);
        }
    }
    std::shared_ptr<CHTLJSASTNode> GetTarget() const { return m_Target; }
    
private:
    std::shared_ptr<class ObjectLiteralNode> m_Config;
    std::shared_ptr<CHTLJSASTNode> m_Target;
};

/**
 * delegate调用节点
 * 表示 delegate({ }) 事件委托
 */
class DelegateCallNode : public CHTLJSASTNode {
public:
    DelegateCallNode()
        : CHTLJSASTNode(CHTLJSNodeType::DELEGATE_CALL, "delegate") {}
    
    void Accept(CHTLJSVisitor* visitor) override;
    
    // 配置对象（包含target和事件处理器）
    void SetConfig(std::shared_ptr<class ObjectLiteralNode> config) {
        m_Config = config;
        if (config) {
            AddChild(config);
        }
    }
    std::shared_ptr<class ObjectLiteralNode> GetConfig() const { return m_Config; }
    
    // 父元素（事件委托的目标）
    void SetParent(std::shared_ptr<CHTLJSASTNode> parent) {
        m_Parent = parent;
        if (parent) {
            AddChild(parent);
        }
    }
    std::shared_ptr<CHTLJSASTNode> GetParent() const { return m_Parent; }
    
private:
    std::shared_ptr<class ObjectLiteralNode> m_Config;
    std::shared_ptr<CHTLJSASTNode> m_Parent;
};

/**
 * animate调用节点
 * 表示 animate({ }) 动画函数
 */
class AnimateCallNode : public CHTLJSASTNode {
public:
    AnimateCallNode()
        : CHTLJSASTNode(CHTLJSNodeType::ANIMATE_CALL, "animate") {}
    
    void Accept(CHTLJSVisitor* visitor) override;
    
    // 动画配置对象
    void SetConfig(std::shared_ptr<class ObjectLiteralNode> config) {
        m_Config = config;
        if (config) {
            AddChild(config);
        }
    }
    std::shared_ptr<class ObjectLiteralNode> GetConfig() const { return m_Config; }
    
private:
    std::shared_ptr<class ObjectLiteralNode> m_Config;
};

/**
 * iNeverAway调用节点
 * 表示 iNeverAway({ }) 特殊函数
 */
class INeverAwayCallNode : public CHTLJSASTNode {
public:
    INeverAwayCallNode()
        : CHTLJSASTNode(CHTLJSNodeType::I_NEVER_AWAY_CALL, "iNeverAway") {}
    
    void Accept(CHTLJSVisitor* visitor) override;
    
    // 函数定义对象（包含状态标签函数）
    void SetConfig(std::shared_ptr<class ObjectLiteralNode> config) {
        m_Config = config;
        if (config) {
            AddChild(config);
        }
    }
    std::shared_ptr<class ObjectLiteralNode> GetConfig() const { return m_Config; }
    
private:
    std::shared_ptr<class ObjectLiteralNode> m_Config;
};

/**
 * printMylove调用节点
 * 表示 printMylove({ }) 图片转字符功能
 */
class PrintMyLoveCallNode : public CHTLJSASTNode {
public:
    PrintMyLoveCallNode()
        : CHTLJSASTNode(CHTLJSNodeType::PRINT_MY_LOVE_CALL, "printMylove") {}
    
    void Accept(CHTLJSVisitor* visitor) override;
    
    // 配置对象
    void SetConfig(std::shared_ptr<class ObjectLiteralNode> config) {
        m_Config = config;
        if (config) {
            AddChild(config);
        }
    }
    std::shared_ptr<class ObjectLiteralNode> GetConfig() const { return m_Config; }
    
private:
    std::shared_ptr<class ObjectLiteralNode> m_Config;
};

} // namespace ast
} // namespace chtljs
} // namespace chtl