#pragma once
#include <memory>
#include <vector>
#include <string>

namespace CHTL {
namespace JSCompiler {

// CHTL JS AST节点类型
enum class ASTNodeType {
    // 根节点
    ROOT,
    
    // 表达式节点
    SELECTOR_EXPRESSION,      // {{选择器}}
    VIR_DECLARATION,         // vir对象声明
    ARROW_EXPRESSION,        // ->操作符表达式
    FUNCTION_CALL,           // 函数调用
    LISTEN_EXPRESSION,       // listen表达式
    DELEGATE_EXPRESSION,     // delegate表达式
    ANIMATE_EXPRESSION,      // animate表达式
    INEVERAWAY_EXPRESSION,   // iNeverAway表达式
    
    // 字面量节点
    IDENTIFIER,
    STRING_LITERAL,
    NUMBER_LITERAL,
    OBJECT_LITERAL,          // JS对象字面量
    ARRAY_LITERAL,           // JS数组字面量
    
    // 特殊节点
    JS_CODE_BLOCK,           // 普通JS代码块
    PROPERTY,                // 属性（键值对）
    PARAMETER                // 参数
};

// CHTL JS AST节点基类
class ASTNode {
public:
    ASTNodeType type;
    std::string value;
    size_t line;
    size_t column;
    std::vector<std::shared_ptr<ASTNode>> children;
    
    // 额外属性
    std::unordered_map<std::string, std::string> attributes;
    
    ASTNode(ASTNodeType t, const std::string& v = "", size_t l = 0, size_t c = 0)
        : type(t), value(v), line(l), column(c) {}
    
    virtual ~ASTNode() = default;
    
    // 添加子节点
    void AddChild(std::shared_ptr<ASTNode> child) {
        children.push_back(child);
    }
    
    // 设置属性
    void SetAttribute(const std::string& key, const std::string& value) {
        attributes[key] = value;
    }
    
    // 获取属性
    std::string GetAttribute(const std::string& key) const {
        auto it = attributes.find(key);
        return it != attributes.end() ? it->second : "";
    }
    
    // 获取节点类型名称
    virtual std::string GetTypeName() const {
        switch (type) {
            case ASTNodeType::ROOT: return "ROOT";
            case ASTNodeType::SELECTOR_EXPRESSION: return "SELECTOR_EXPRESSION";
            case ASTNodeType::VIR_DECLARATION: return "VIR_DECLARATION";
            case ASTNodeType::ARROW_EXPRESSION: return "ARROW_EXPRESSION";
            case ASTNodeType::FUNCTION_CALL: return "FUNCTION_CALL";
            case ASTNodeType::LISTEN_EXPRESSION: return "LISTEN_EXPRESSION";
            case ASTNodeType::DELEGATE_EXPRESSION: return "DELEGATE_EXPRESSION";
            case ASTNodeType::ANIMATE_EXPRESSION: return "ANIMATE_EXPRESSION";
            case ASTNodeType::INEVERAWAY_EXPRESSION: return "INEVERAWAY_EXPRESSION";
            case ASTNodeType::IDENTIFIER: return "IDENTIFIER";
            case ASTNodeType::STRING_LITERAL: return "STRING_LITERAL";
            case ASTNodeType::NUMBER_LITERAL: return "NUMBER_LITERAL";
            case ASTNodeType::OBJECT_LITERAL: return "OBJECT_LITERAL";
            case ASTNodeType::ARRAY_LITERAL: return "ARRAY_LITERAL";
            case ASTNodeType::JS_CODE_BLOCK: return "JS_CODE_BLOCK";
            case ASTNodeType::PROPERTY: return "PROPERTY";
            case ASTNodeType::PARAMETER: return "PARAMETER";
            default: return "UNKNOWN";
        }
    }
};

} // namespace JSCompiler
} // namespace CHTL