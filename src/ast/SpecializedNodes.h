#pragma once
#include "ASTNode.h"

namespace chtl {
namespace ast {

// CSS属性节点
class CSSPropertyNode : public ASTNode {
public:
    std::string property_name;      // 属性名
    std::string property_value;     // 属性值
    bool is_important;              // 是否有!important
    
    CSSPropertyNode(const std::string& name, const std::string& value,
                   const TokenPosition& pos = TokenPosition())
        : ASTNode(NodeType::CSS_PROPERTY, pos), property_name(name), 
          property_value(value), is_important(false) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }
    
    std::unique_ptr<ASTNode> clone() const override {
        auto cloned = std::make_unique<CSSPropertyNode>(property_name, property_value, position);
        cloned->is_important = is_important;
        cloneBaseProperties(cloned.get());
        return std::move(cloned);
    }
    
    std::string toString() const override {
        return "CSSProperty(" + property_name + ": " + property_value + 
               (is_important ? " !important" : "") + ") " + ASTNode::toString();
    }
};

// CSS选择器节点
class CSSSelectorNode : public ASTNode {
public:
    std::string selector_text;      // 选择器文本
    bool auto_generated;            // 是否自动生成的类名/ID
    
    CSSSelectorNode(const std::string& selector, const TokenPosition& pos = TokenPosition())
        : ASTNode(NodeType::CSS_SELECTOR, pos), selector_text(selector), auto_generated(false) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }
    
    std::unique_ptr<ASTNode> clone() const override {
        auto cloned = std::make_unique<CSSSelectorNode>(selector_text, position);
        cloned->auto_generated = auto_generated;
        cloneBaseProperties(cloned.get());
        return std::move(cloned);
    }
    
    std::string toString() const override {
        return "CSSSelector(" + selector_text + 
               (auto_generated ? " [auto]" : "") + ") " + ASTNode::toString();
    }
};

// 样式继承节点
class StyleInheritanceNode : public ASTNode {
public:
    std::string inherited_style;    // 被继承的样式名
    bool is_explicit;               // 是否显式继承(inherit关键字)
    
    StyleInheritanceNode(const std::string& style_name, bool explicit_inherit = false,
                        const TokenPosition& pos = TokenPosition())
        : ASTNode(NodeType::STYLE_INHERITANCE, pos), inherited_style(style_name), 
          is_explicit(explicit_inherit) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }
    
    std::unique_ptr<ASTNode> clone() const override {
        auto cloned = std::make_unique<StyleInheritanceNode>(inherited_style, is_explicit, position);
        cloneBaseProperties(cloned.get());
        return std::move(cloned);
    }
    
    std::string toString() const override {
        return "StyleInheritance(" + std::string(is_explicit ? "inherit " : "") + 
               inherited_style + ") " + ASTNode::toString();
    }
};

// 样式删除节点
class StyleDeleteNode : public ASTNode {
public:
    std::vector<std::string> deleted_properties; // 删除的属性列表
    std::vector<std::string> deleted_inheritance; // 删除的继承列表
    
    StyleDeleteNode(const TokenPosition& pos = TokenPosition())
        : ASTNode(NodeType::STYLE_DELETE, pos) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }
    
    std::unique_ptr<ASTNode> clone() const override {
        auto cloned = std::make_unique<StyleDeleteNode>(position);
        cloned->deleted_properties = deleted_properties;
        cloned->deleted_inheritance = deleted_inheritance;
        cloneBaseProperties(cloned.get());
        return std::move(cloned);
    }
    
    void addDeletedProperty(const std::string& property) {
        deleted_properties.push_back(property);
    }
    
    void addDeletedInheritance(const std::string& inheritance) {
        deleted_inheritance.push_back(inheritance);
    }
    
    std::string toString() const override {
        std::string result = "StyleDelete(";
        if (!deleted_properties.empty()) {
            result += "props: ";
            for (size_t i = 0; i < deleted_properties.size(); ++i) {
                if (i > 0) result += ", ";
                result += deleted_properties[i];
            }
        }
        if (!deleted_inheritance.empty()) {
            if (!deleted_properties.empty()) result += "; ";
            result += "inherit: ";
            for (size_t i = 0; i < deleted_inheritance.size(); ++i) {
                if (i > 0) result += ", ";
                result += deleted_inheritance[i];
            }
        }
        result += ") " + ASTNode::toString();
        return result;
    }
};

// 变量引用节点
class VariableReferenceNode : public ASTNode {
public:
    std::string variable_group;     // 变量组名
    std::string variable_name;      // 变量名
    
    VariableReferenceNode(const std::string& group, const std::string& name,
                         const TokenPosition& pos = TokenPosition())
        : ASTNode(NodeType::VARIABLE_REFERENCE, pos), variable_group(group), variable_name(name) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }
    
    std::unique_ptr<ASTNode> clone() const override {
        auto cloned = std::make_unique<VariableReferenceNode>(variable_group, variable_name, position);
        cloneBaseProperties(cloned.get());
        return std::move(cloned);
    }
    
    std::string toString() const override {
        return "VariableReference(" + variable_group + "(" + variable_name + ")) " + ASTNode::toString();
    }
};

// 变量特例化节点
class VariableSpecializationNode : public ASTNode {
public:
    std::string variable_group;     // 变量组名
    std::unordered_map<std::string, std::string> specializations; // 特例化映射
    
    VariableSpecializationNode(const std::string& group, const TokenPosition& pos = TokenPosition())
        : ASTNode(NodeType::VARIABLE_SPECIALIZATION, pos), variable_group(group) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }
    
    std::unique_ptr<ASTNode> clone() const override {
        auto cloned = std::make_unique<VariableSpecializationNode>(variable_group, position);
        cloned->specializations = specializations;
        cloneBaseProperties(cloned.get());
        return std::move(cloned);
    }
    
    void addSpecialization(const std::string& var_name, const std::string& value) {
        specializations[var_name] = value;
    }
    
    std::string toString() const override {
        std::string result = "VariableSpecialization(" + variable_group + " {";
        bool first = true;
        for (const auto& spec : specializations) {
            if (!first) result += ", ";
            result += spec.first + "=" + spec.second;
            first = false;
        }
        result += "}) " + ASTNode::toString();
        return result;
    }
};

// 元素插入节点
class ElementInsertNode : public ASTNode {
public:
    enum InsertPosition {
        BEFORE,
        AFTER,
        REPLACE,
        AT_TOP,
        AT_BOTTOM
    };
    
    InsertPosition insert_position; // 插入位置
    std::string target_selector;    // 目标选择器
    
    ElementInsertNode(InsertPosition pos, const std::string& selector,
                     const TokenPosition& token_pos = TokenPosition())
        : ASTNode(NodeType::ELEMENT_INSERT, token_pos), insert_position(pos), target_selector(selector) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }
    
    std::unique_ptr<ASTNode> clone() const override {
        auto cloned = std::make_unique<ElementInsertNode>(insert_position, target_selector, position);
        cloneBaseProperties(cloned.get());
        return std::move(cloned);
    }
    
    std::string getPositionString() const {
        switch (insert_position) {
            case BEFORE: return "before";
            case AFTER: return "after";
            case REPLACE: return "replace";
            case AT_TOP: return "at top";
            case AT_BOTTOM: return "at bottom";
            default: return "unknown";
        }
    }
    
    std::string toString() const override {
        return "ElementInsert(" + getPositionString() + " " + target_selector + ") " + ASTNode::toString();
    }
};

// 元素删除节点
class ElementDeleteNode : public ASTNode {
public:
    std::string target_selector;    // 目标选择器
    
    ElementDeleteNode(const std::string& selector, const TokenPosition& pos = TokenPosition())
        : ASTNode(NodeType::ELEMENT_DELETE, pos), target_selector(selector) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }
    
    std::unique_ptr<ASTNode> clone() const override {
        auto cloned = std::make_unique<ElementDeleteNode>(target_selector, position);
        cloneBaseProperties(cloned.get());
        return std::move(cloned);
    }
    
    std::string toString() const override {
        return "ElementDelete(" + target_selector + ") " + ASTNode::toString();
    }
};

// 元素索引访问节点
class ElementIndexNode : public ASTNode {
public:
    std::string element_name;       // 元素名
    int index;                      // 索引
    
    ElementIndexNode(const std::string& name, int idx, const TokenPosition& pos = TokenPosition())
        : ASTNode(NodeType::ELEMENT_INDEX, pos), element_name(name), index(idx) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }
    
    std::unique_ptr<ASTNode> clone() const override {
        auto cloned = std::make_unique<ElementIndexNode>(element_name, index, position);
        cloneBaseProperties(cloned.get());
        return std::move(cloned);
    }
    
    std::string toString() const override {
        return "ElementIndex(" + element_name + "[" + std::to_string(index) + "]) " + ASTNode::toString();
    }
};

// CHTL JS Listen节点
class CHTLJSListenNode : public ASTNode {
public:
    std::string target_selector;    // 目标选择器
    std::unordered_map<std::string, std::string> event_handlers; // 事件处理器映射
    
    CHTLJSListenNode(const std::string& selector, const TokenPosition& pos = TokenPosition())
        : ASTNode(NodeType::CHTL_JS_LISTEN, pos), target_selector(selector) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }
    
    std::unique_ptr<ASTNode> clone() const override {
        auto cloned = std::make_unique<CHTLJSListenNode>(target_selector, position);
        cloned->event_handlers = event_handlers;
        cloneBaseProperties(cloned.get());
        return std::move(cloned);
    }
    
    void addEventHandler(const std::string& event, const std::string& handler) {
        event_handlers[event] = handler;
    }
    
    std::string toString() const override {
        std::string result = "CHTLJSListen(" + target_selector + " {";
        bool first = true;
        for (const auto& handler : event_handlers) {
            if (!first) result += ", ";
            result += handler.first + ": " + handler.second;
            first = false;
        }
        result += "}) " + ASTNode::toString();
        return result;
    }
};

// CHTL JS Delegate节点
class CHTLJSDelegateNode : public ASTNode {
public:
    std::string parent_selector;    // 父元素选择器
    std::vector<std::string> target_selectors; // 目标选择器列表
    std::unordered_map<std::string, std::string> event_handlers; // 事件处理器映射
    
    CHTLJSDelegateNode(const std::string& parent, const TokenPosition& pos = TokenPosition())
        : ASTNode(NodeType::CHTL_JS_DELEGATE, pos), parent_selector(parent) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }
    
    std::unique_ptr<ASTNode> clone() const override {
        auto cloned = std::make_unique<CHTLJSDelegateNode>(parent_selector, position);
        cloned->target_selectors = target_selectors;
        cloned->event_handlers = event_handlers;
        cloneBaseProperties(cloned.get());
        return std::move(cloned);
    }
    
    void addTargetSelector(const std::string& selector) {
        target_selectors.push_back(selector);
    }
    
    void addEventHandler(const std::string& event, const std::string& handler) {
        event_handlers[event] = handler;
    }
    
    std::string toString() const override {
        return "CHTLJSDelegate(" + parent_selector + " -> targets) " + ASTNode::toString();
    }
};

// CHTL JS Vir节点
class CHTLJSVirNode : public ASTNode {
public:
    std::string vir_name;           // 虚对象名称
    std::string function_name;      // 函数名称
    std::unordered_map<std::string, std::string> function_map; // 函数映射
    
    CHTLJSVirNode(const std::string& name, const std::string& func_name,
                 const TokenPosition& pos = TokenPosition())
        : ASTNode(NodeType::CHTL_JS_VIR, pos), vir_name(name), function_name(func_name) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }
    
    std::unique_ptr<ASTNode> clone() const override {
        auto cloned = std::make_unique<CHTLJSVirNode>(vir_name, function_name, position);
        cloned->function_map = function_map;
        cloneBaseProperties(cloned.get());
        return std::move(cloned);
    }
    
    void addFunction(const std::string& key, const std::string& func) {
        function_map[key] = func;
    }
    
    std::string toString() const override {
        return "CHTLJSVir(" + vir_name + " = " + function_name + ") " + ASTNode::toString();
    }
};

// 约束节点
class ExceptConstraintNode : public ASTNode {
public:
    std::vector<std::string> except_elements;   // 被约束的元素
    std::vector<std::string> except_types;      // 被约束的类型
    
    ExceptConstraintNode(const TokenPosition& pos = TokenPosition())
        : ASTNode(NodeType::EXCEPT_CONSTRAINT, pos) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }
    
    std::unique_ptr<ASTNode> clone() const override {
        auto cloned = std::make_unique<ExceptConstraintNode>(position);
        cloned->except_elements = except_elements;
        cloned->except_types = except_types;
        cloneBaseProperties(cloned.get());
        return std::move(cloned);
    }
    
    void addExceptElement(const std::string& element) {
        except_elements.push_back(element);
    }
    
    void addExceptType(const std::string& type) {
        except_types.push_back(type);
    }
    
    std::string toString() const override {
        std::string result = "ExceptConstraint(";
        if (!except_elements.empty()) {
            result += "elements: ";
            for (size_t i = 0; i < except_elements.size(); ++i) {
                if (i > 0) result += ", ";
                result += except_elements[i];
            }
        }
        if (!except_types.empty()) {
            if (!except_elements.empty()) result += "; ";
            result += "types: ";
            for (size_t i = 0; i < except_types.size(); ++i) {
                if (i > 0) result += ", ";
                result += except_types[i];
            }
        }
        result += ") " + ASTNode::toString();
        return result;
    }
};

} // namespace ast
} // namespace chtl