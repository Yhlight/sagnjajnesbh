#pragma once
#include "../common/Token.h"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

namespace chtl {
namespace ast {

// 前向声明
class ASTVisitor;

// AST节点类型枚举
enum class NodeType {
    // 基础节点
    PROGRAM,                    // 程序根节点
    ELEMENT,                    // HTML元素节点
    TEXT,                       // 文本节点
    ATTRIBUTE,                  // 属性节点
    
    // CHTL特殊节点
    STYLE_BLOCK,               // 样式块
    SCRIPT_BLOCK,              // 脚本块
    TEMPLATE_DEFINITION,       // 模板定义
    CUSTOM_DEFINITION,         // 自定义定义
    ORIGIN_BLOCK,              // 原始嵌入块
    IMPORT_STATEMENT,          // 导入语句
    NAMESPACE_DEFINITION,      // 命名空间定义
    CONFIGURATION_BLOCK,       // 配置块
    
    // 模板相关
    TEMPLATE_STYLE,            // 模板样式组
    TEMPLATE_ELEMENT,          // 模板元素
    TEMPLATE_VAR,              // 模板变量组
    
    // 自定义相关
    CUSTOM_STYLE,              // 自定义样式组
    CUSTOM_ELEMENT,            // 自定义元素
    CUSTOM_VAR,                // 自定义变量组
    
    // 样式相关
    CSS_PROPERTY,              // CSS属性
    CSS_SELECTOR,              // CSS选择器
    STYLE_INHERITANCE,         // 样式继承
    STYLE_DELETE,              // 样式删除
    
    // 变量相关
    VARIABLE_REFERENCE,        // 变量引用
    VARIABLE_SPECIALIZATION,   // 变量特例化
    
    // 元素操作
    ELEMENT_INSERT,            // 元素插入
    ELEMENT_DELETE,            // 元素删除
    ELEMENT_INDEX,             // 元素索引访问
    
    // CHTL JS相关
    CHTL_JS_SELECTOR,          // CHTL JS选择器 {{...}}
    CHTL_JS_LISTEN,            // listen函数
    CHTL_JS_DELEGATE,          // delegate函数
    CHTL_JS_ANIMATE,           // animate函数
    CHTL_JS_VIR,               // vir虚对象
    
    // 注释
    SINGLE_COMMENT,            // 单行注释
    MULTI_COMMENT,             // 多行注释
    GENERATOR_COMMENT,         // 生成器注释
    
    // 字面量
    STRING_LITERAL,            // 字符串字面量
    NUMBER_LITERAL,            // 数字字面量
    UNQUOTED_LITERAL,          // 无引号字面量
    IDENTIFIER,                // 标识符
    
    // 约束
    EXCEPT_CONSTRAINT,         // except约束
    
    // 未知节点
    UNKNOWN
};

// AST节点基类
class ASTNode {
public:
    NodeType type;                      // 节点类型
    TokenPosition position;             // 源码位置
    std::vector<std::unique_ptr<ASTNode>> children; // 子节点
    std::unordered_map<std::string, std::string> attributes; // 节点属性
    
    ASTNode(NodeType node_type, const TokenPosition& pos = TokenPosition())
        : type(node_type), position(pos) {}
    
    virtual ~ASTNode() = default;
    
    // 添加子节点
    void addChild(std::unique_ptr<ASTNode> child);
    
    // 获取子节点
    ASTNode* getChild(size_t index) const;
    size_t getChildCount() const;
    
    // 属性管理
    void setAttribute(const std::string& key, const std::string& value);
    std::string getAttribute(const std::string& key) const;
    bool hasAttribute(const std::string& key) const;
    
    // 访问者模式支持
    virtual void accept(ASTVisitor& visitor) = 0;
    
    // 调试和诊断
    virtual std::string toString() const;
    virtual std::string getNodeTypeName() const;
    
    // 查找子节点
    std::vector<ASTNode*> findChildrenByType(NodeType target_type) const;
    ASTNode* findFirstChildByType(NodeType target_type) const;
    
    // 克隆节点
    virtual std::unique_ptr<ASTNode> clone() const = 0;
    
protected:
    // 子类实现的克隆帮助方法
    void cloneBaseProperties(ASTNode* target) const;
};

// 访问者接口
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    
    // 访问方法（纯虚函数，由子类实现）
    virtual void visitProgram(class ProgramNode& node) = 0;
    virtual void visitElement(class ElementNode& node) = 0;
    virtual void visitText(class TextNode& node) = 0;
    virtual void visitAttribute(class AttributeNode& node) = 0;
    virtual void visitStyleBlock(class StyleBlockNode& node) = 0;
    virtual void visitScriptBlock(class ScriptBlockNode& node) = 0;
    virtual void visitTemplateDefinition(class TemplateDefinitionNode& node) = 0;
    virtual void visitCustomDefinition(class CustomDefinitionNode& node) = 0;
    virtual void visitOriginBlock(class OriginBlockNode& node) = 0;
    virtual void visitImportStatement(class ImportStatementNode& node) = 0;
    virtual void visitNamespaceDefinition(class NamespaceDefinitionNode& node) = 0;
    virtual void visitCHTLJSSelector(class CHTLJSSelectorNode& node) = 0;
    virtual void visitComment(class CommentNode& node) = 0;
    virtual void visitLiteral(class LiteralNode& node) = 0;
    virtual void visitIdentifier(class IdentifierNode& node) = 0;
    
    // 默认访问方法
    virtual void visit(ASTNode& node);
};

// 工具函数
std::string nodeTypeToString(NodeType type);
bool isTemplateNode(NodeType type);
bool isCustomNode(NodeType type);
bool isCHTLJSNode(NodeType type);
bool isStyleNode(NodeType type);

} // namespace ast
} // namespace chtl