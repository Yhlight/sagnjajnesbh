#pragma once

#include "chtl_js/ast/CHTLJSASTNode.h"
#include <unordered_map>

namespace chtl {
namespace chtljs {
namespace ast {

/**
 * 对象字面量节点
 * 表示 { key: value, ... } 结构
 * 用于各种CHTL JS函数的配置对象
 */
class ObjectLiteralNode : public CHTLJSASTNode {
public:
    ObjectLiteralNode()
        : CHTLJSASTNode(CHTLJSNodeType::OBJECT_LITERAL) {}
    
    void Accept(CHTLJSVisitor* visitor) override;
    
    // 添加属性
    void AddProperty(std::shared_ptr<class PropertyNode> property);
    
    // 获取所有属性
    const std::vector<std::shared_ptr<class PropertyNode>>& GetProperties() const {
        return m_Properties;
    }
    
    // 通过键查找属性
    std::shared_ptr<class PropertyNode> GetProperty(const std::string& key) const;
    bool HasProperty(const std::string& key) const;
    
    // 特殊属性快速访问（用于特定的CHTL JS配置）
    
    // 对于animate配置
    bool HasDuration() const { return HasProperty("duration"); }
    bool HasEasing() const { return HasProperty("easing"); }
    bool HasBegin() const { return HasProperty("begin"); }
    bool HasEnd() const { return HasProperty("end"); }
    bool HasWhen() const { return HasProperty("when"); }
    
    // 对于delegate配置
    bool HasTarget() const { return HasProperty("target"); }
    
    // 对于printMylove配置
    bool HasSrc() const { return HasProperty("src"); }
    bool HasType() const { return HasProperty("type"); }
    
private:
    std::vector<std::shared_ptr<class PropertyNode>> m_Properties;
    std::unordered_map<std::string, std::shared_ptr<class PropertyNode>> m_PropertyMap;
};

/**
 * 属性节点
 * 表示对象中的 key: value 对
 */
class PropertyNode : public CHTLJSASTNode {
public:
    PropertyNode(const std::string& key)
        : CHTLJSASTNode(CHTLJSNodeType::PROPERTY, key) {}
    
    void Accept(CHTLJSVisitor* visitor) override;
    
    // 属性键
    const std::string& GetKey() const { return m_Value; }
    
    // 属性值
    void SetValue(std::shared_ptr<CHTLJSASTNode> value) {
        m_PropertyValue = value;
        if (value) {
            AddChild(value);
        }
    }
    std::shared_ptr<CHTLJSASTNode> GetValue() const { return m_PropertyValue; }
    
    // 是否是函数属性（值是函数）
    bool IsFunctionProperty() const;
    
    // 是否是对象属性（值是对象）
    bool IsObjectProperty() const;
    
    // 是否是数组属性（值是数组）
    bool IsArrayProperty() const;
    
    // 对于带状态标签的函数键（如 Void<A>）
    void SetStateTag(std::shared_ptr<class StateTagNode> state) {
        m_StateTag = state;
        if (state) {
            AddChild(state);
        }
    }
    std::shared_ptr<class StateTagNode> GetStateTag() const { return m_StateTag; }
    bool HasStateTag() const { return m_StateTag != nullptr; }
    
private:
    std::shared_ptr<CHTLJSASTNode> m_PropertyValue;
    std::shared_ptr<class StateTagNode> m_StateTag;
};

} // namespace ast
} // namespace chtljs
} // namespace chtl