#pragma once
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

namespace CHTL {
namespace Compiler {

// AST节点类型
enum class ASTNodeType {
    // 根节点
    ROOT,
    DOCUMENT,
    
    // 声明节点
    TEMPLATE_DECLARATION,     // [Template] 声明
    CUSTOM_DECLARATION,       // [Custom] 声明
    ORIGIN_DECLARATION,       // [Origin] 声明
    CONFIGURATION_DECLARATION,// [Configuration] 声明
    NAMESPACE_DECLARATION,    // [Namespace] 声明
    IMPORT_DECLARATION,       // [Import] 声明
    INFO_DECLARATION,         // [Info] 声明
    EXPORT_DECLARATION,       // [Export] 声明
    
    // 类型声明节点
    STYLE_DECLARATION,        // @Style 声明
    ELEMENT_DECLARATION,      // @Element 声明
    VAR_DECLARATION,          // @Var 声明
    
    // 元素节点
    HTML_ELEMENT,            // HTML元素
    TEXT_NODE,               // text { } 节点
    STYLE_BLOCK,             // style { } 局部样式块
    SCRIPT_BLOCK,            // script { } 局部脚本块
    
    // 样式相关节点
    STYLE_RULE,              // 样式规则
    STYLE_PROPERTY,          // 样式属性
    STYLE_SELECTOR,          // 样式选择器
    CLASS_SELECTOR,          // .class 选择器
    ID_SELECTOR,             // #id 选择器
    PSEUDO_SELECTOR,         // :hover, ::before 等
    AMPERSAND_SELECTOR,      // & 选择器
    
    // 使用节点
    STYLE_USE,               // @Style 使用
    ELEMENT_USE,             // @Element 使用
    VAR_USE,                 // 变量使用
    
    // 特例化节点
    SPECIALIZATION,          // 特例化块
    DELETE_STATEMENT,        // delete 语句
    INSERT_STATEMENT,        // insert 语句
    
    // 表达式节点
    IDENTIFIER,              // 标识符
    STRING_LITERAL,          // 字符串字面量
    NUMBER_LITERAL,          // 数字字面量
    UNQUOTED_LITERAL,        // 无引号字面量
    VAR_EXPRESSION,          // VarName(key) 或 VarName(key = value)
    FROM_EXPRESSION,         // from 表达式
    INDEX_EXPRESSION,        // [index] 表达式
    
    // 语句节点
    INHERIT_STATEMENT,       // inherit 语句
    EXCEPT_STATEMENT,        // except 语句
    ATTRIBUTE_STATEMENT,     // 属性语句
    
    // 配置相关节点
    CONFIG_PROPERTY,         // 配置属性
    CONFIG_NAME_BLOCK,       // [Name] 块
    CONFIG_ORIGINTYPE_BLOCK, // [OriginType] 块
    
    // 其他
    COMMENT,                 // 注释
    GENERATOR_COMMENT        // 生成器注释 --
};

// AST节点基类
class ASTNode {
public:
    ASTNodeType type;
    std::string value;
    size_t line;
    size_t column;
    size_t endLine;
    size_t endColumn;
    std::vector<std::shared_ptr<ASTNode>> children;
    std::unordered_map<std::string, std::string> attributes;
    
    ASTNode(ASTNodeType t, const std::string& v = "", size_t l = 0, size_t c = 0)
        : type(t), value(v), line(l), column(c), endLine(l), endColumn(c) {}
    
    virtual ~ASTNode() = default;
    
    // 添加子节点
    void AddChild(std::shared_ptr<ASTNode> child) {
        if (child) {
            children.push_back(child);
        }
    }
    
    // 设置属性
    void SetAttribute(const std::string& key, const std::string& val) {
        attributes[key] = val;
    }
    
    // 获取属性
    std::string GetAttribute(const std::string& key) const {
        auto it = attributes.find(key);
        return it != attributes.end() ? it->second : "";
    }
    
    // 查找特定类型的子节点
    std::vector<std::shared_ptr<ASTNode>> FindChildrenByType(ASTNodeType nodeType) const {
        std::vector<std::shared_ptr<ASTNode>> result;
        for (const auto& child : children) {
            if (child && child->type == nodeType) {
                result.push_back(child);
            }
        }
        return result;
    }
    
    // 查找第一个特定类型的子节点
    std::shared_ptr<ASTNode> FindFirstChildByType(ASTNodeType nodeType) const {
        for (const auto& child : children) {
            if (child && child->type == nodeType) {
                return child;
            }
        }
        return nullptr;
    }
    
    // 获取节点类型名称
    virtual std::string GetTypeName() const;
    
    // 调试输出
    void Dump(int indent = 0) const;
};

// 特定的AST节点类型

// HTML元素节点
class HTMLElementNode : public ASTNode {
public:
    std::string tagName;
    bool isSelfClosing;
    
    HTMLElementNode(const std::string& tag, bool selfClosing = false)
        : ASTNode(ASTNodeType::HTML_ELEMENT), tagName(tag), isSelfClosing(selfClosing) {
        value = tag;
    }
};

// 样式属性节点
class StylePropertyNode : public ASTNode {
public:
    std::string property;
    std::string propertyValue;
    
    StylePropertyNode(const std::string& prop, const std::string& val)
        : ASTNode(ASTNodeType::STYLE_PROPERTY), property(prop), propertyValue(val) {
        value = prop + ": " + val;
    }
};

// 变量表达式节点
class VarExpressionNode : public ASTNode {
public:
    std::string varGroupName;
    std::string varKey;
    std::string defaultValue;
    
    VarExpressionNode(const std::string& group, const std::string& key)
        : ASTNode(ASTNodeType::VAR_EXPRESSION), varGroupName(group), varKey(key) {
        value = group + "(" + key + ")";
    }
};

} // namespace Compiler
} // namespace CHTL