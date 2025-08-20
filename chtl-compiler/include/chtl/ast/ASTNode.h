#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace chtl {
namespace ast {

// AST节点类型
enum class ASTNodeType {
    // 文档根节点
    DOCUMENT,
    
    // 注释节点
    COMMENT_SINGLE,      // // 注释
    COMMENT_MULTI,       // /* */ 注释
    COMMENT_GENERATOR,   // -- 生成器注释
    
    // 文本节点
    TEXT_NODE,           // text { }
    
    // 元素节点
    ELEMENT,             // HTML元素
    
    // 属性节点
    ATTRIBUTE,           // 属性名: 值;
    
    // 样式节点
    STYLE_BLOCK,         // style { }
    STYLE_RULE,          // 样式规则
    STYLE_PROPERTY,      // 样式属性
    STYLE_SELECTOR,      // 选择器(.class, #id, &等)
    
    // 脚本节点
    SCRIPT_BLOCK,        // script { }
    
    // 模板节点
    TEMPLATE_STYLE,      // [Template] @Style
    TEMPLATE_ELEMENT,    // [Template] @Element
    TEMPLATE_VAR,        // [Template] @Var
    
    // 自定义节点
    CUSTOM_STYLE,        // [Custom] @Style
    CUSTOM_ELEMENT,      // [Custom] @Element
    CUSTOM_VAR,          // [Custom] @Var
    
    // 继承和特化
    INHERIT,             // inherit语句
    DELETE,              // delete语句
    INSERT,              // insert语句
    
    // 原始嵌入
    ORIGIN_BLOCK,        // [Origin] @Type
    
    // 配置
    CONFIGURATION,       // [Configuration]
    CONFIG_PROPERTY,     // 配置属性
    CONFIG_NAME_BLOCK,   // [Name]块
    CONFIG_ORIGIN_TYPE,  // [OriginType]块
    
    // 导入
    IMPORT,              // [Import]
    
    // 命名空间
    NAMESPACE,           // [Namespace]
    
    // 约束
    EXCEPT,              // except语句
    
    // 表达式和字面量
    STRING_LITERAL,      // 字符串字面量
    UNQUOTED_LITERAL,    // 无引号字面量
    NUMBER_LITERAL,      // 数字字面量
    IDENTIFIER,          // 标识符
    
    // 特殊节点
    STYLE_REFERENCE,     // @Style引用
    ELEMENT_REFERENCE,   // @Element引用
    VAR_REFERENCE,       // 变量组引用
    VAR_CALL,            // 变量组调用 ThemeColor(tableColor)
    INDEX_ACCESS,        // 索引访问 div[1]
    FROM_CLAUSE,         // from子句
    AS_CLAUSE,           // as子句
    
    // 错误节点
    ERROR
};

// 前向声明
class ASTVisitor;

/**
 * AST节点基类
 * 所有CHTL AST节点都继承自此类
 */
class ASTNode {
public:
    ASTNode(ASTNodeType type, const std::string& value = "")
        : m_Type(type), m_Value(value), m_Line(0), m_Column(0) {}
        
    virtual ~ASTNode() = default;
    
    // 获取节点类型
    ASTNodeType GetType() const { return m_Type; }
    
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
    void AddChild(std::shared_ptr<ASTNode> child);
    const std::vector<std::shared_ptr<ASTNode>>& GetChildren() const { return m_Children; }
    std::shared_ptr<ASTNode> GetChild(size_t index) const;
    size_t GetChildCount() const { return m_Children.size(); }
    
    // 属性管理（用于存储额外信息）
    void SetAttribute(const std::string& key, const std::string& value);
    std::string GetAttribute(const std::string& key) const;
    bool HasAttribute(const std::string& key) const;
    const std::unordered_map<std::string, std::string>& GetAttributes() const { return m_Attributes; }
    
    // 访问者模式
    virtual void Accept(ASTVisitor* visitor) = 0;
    
    // 获取节点类型的字符串表示
    static std::string TypeToString(ASTNodeType type);
    std::string GetTypeString() const { return TypeToString(m_Type); }
    
    // 调试输出
    virtual std::string ToString() const;
    virtual void Print(int indent = 0) const;
    
protected:
    ASTNodeType m_Type;
    std::string m_Value;
    size_t m_Line;
    size_t m_Column;
    std::vector<std::shared_ptr<ASTNode>> m_Children;
    std::unordered_map<std::string, std::string> m_Attributes;
};

/**
 * AST访问者接口
 * 用于遍历和处理AST
 */
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    
    // 访问方法（每个具体节点类型都需要一个）
    virtual void VisitDocument(class DocumentNode* node) = 0;
    virtual void VisitComment(class CommentNode* node) = 0;
    virtual void VisitText(class TextNode* node) = 0;
    virtual void VisitElement(class ElementNode* node) = 0;
    virtual void VisitAttribute(class AttributeNode* node) = 0;
    virtual void VisitStyleBlock(class StyleBlockNode* node) = 0;
    virtual void VisitScriptBlock(class ScriptBlockNode* node) = 0;
    virtual void VisitTemplate(class TemplateNode* node) = 0;
    virtual void VisitCustom(class CustomNode* node) = 0;
    virtual void VisitImport(class ImportNode* node) = 0;
    virtual void VisitNamespace(class NamespaceNode* node) = 0;
    virtual void VisitConfiguration(class ConfigurationNode* node) = 0;
    virtual void VisitOrigin(class OriginNode* node) = 0;
    virtual void VisitExcept(class ExceptNode* node) = 0;
    virtual void VisitLiteral(class LiteralNode* node) = 0;
    virtual void VisitIdentifier(class IdentifierNode* node) = 0;
    virtual void VisitReference(class ReferenceNode* node) = 0;
};

} // namespace ast
} // namespace chtl