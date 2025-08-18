#pragma once
#include "../../CHTLCompiler/AST/CHTLNodes.h"
#include "Context.h"
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <memory>

namespace chtl {

// 上下文类型枚举
enum class ContextType {
    GLOBAL_STYLE,      // 全局样式块
    LOCAL_STYLE,       // 局部样式块
    GLOBAL_SCRIPT,     // 全局script
    LOCAL_SCRIPT,      // 局部script
    ELEMENT_CONTENT,   // 元素内容
    TEMPLATE_DEF,      // 模板定义
    CUSTOM_DEF,        // 自定义定义
    NAMESPACE_DEF,     // 命名空间定义
    IMPORT_STMT,       // 导入语句
    ORIGIN_BLOCK,      // 原始嵌入块
    CONFIGURATION      // 配置块
};

// 语法元素类型
enum class SyntaxElement {
    // 变量相关
    TEMPLATE_VAR,              // 模板变量
    CUSTOM_VAR,                // 自定义变量
    VAR_SPECIALIZATION,        // 变量特例化
    
    // 样式组相关
    TEMPLATE_STYLE,            // 模板样式组
    CUSTOM_STYLE,              // 自定义样式组
    VALUELESS_STYLE,           // 无值样式组
    STYLE_SPECIALIZATION,      // 样式组特例化
    
    // 元素相关
    TEMPLATE_ELEMENT,          // 模板元素
    CUSTOM_ELEMENT,            // 自定义元素
    ELEMENT_SPECIALIZATION,    // 元素特例化
    
    // 操作相关
    DELETE_PROPERTY,           // delete属性
    DELETE_INHERITANCE,        // delete继承
    STYLE_INHERITANCE,         // 样式组继承
    ELEMENT_INSERT,            // 元素插入
    ELEMENT_DELETE,            // 元素删除
    
    // 访问相关
    NAMESPACE_FROM,            // 命名空间from语法
    QUALIFIED_NAME,            // 全缀名
    
    // 注释和嵌入（特殊存在）
    GENERATOR_COMMENT,         // 生成器注释
    ORIGIN_EMBED,              // 原始嵌入（任意类型）
    
    // CHTL JS特有语法
    CHTL_JS_SELECTOR,          // {{...}}选择器
    CHTL_JS_ARROW,             // ->操作符
    CHTL_JS_LISTEN,            // listen函数
    CHTL_JS_DELEGATE,          // delegate函数
    CHTL_JS_ANIMATE,           // animate函数
    CHTL_JS_VIR,               // vir虚对象
    
    // 其他
    CSS_PROPERTY,              // CSS属性
    HTML_ELEMENT,              // HTML元素
    JAVASCRIPT_CODE            // 纯JavaScript代码
};

// 约束违规信息
struct ConstraintViolation {
    ContextType context;
    SyntaxElement forbidden_element;
    std::string element_name;
    std::string message;
    size_t line;
    size_t column;
    std::string suggestion;
};

// 语法约束器
class SyntaxConstrainer {
public:
    SyntaxConstrainer();
    ~SyntaxConstrainer();
    
    // 上下文约束检查
    bool isElementAllowed(ContextType context, SyntaxElement element);
    std::vector<ConstraintViolation> validateContext(ContextType context, ast::ASTNode* node);
    
    // 特定上下文的约束检查
    std::vector<ConstraintViolation> validateGlobalStyle(ast::ASTNode* style_block);
    std::vector<ConstraintViolation> validateLocalStyle(ast::ASTNode* style_block);
    std::vector<ConstraintViolation> validateGlobalScript(ast::ASTNode* script_block);
    std::vector<ConstraintViolation> validateLocalScript(ast::ASTNode* script_block);
    
    // 递归约束检查
    std::vector<ConstraintViolation> validateAST(ast::ASTNode* root);
    std::vector<ConstraintViolation> validateNode(ast::ASTNode* node, ContextType parent_context);
    
    // 约束规则管理
    void addConstraintRule(ContextType context, SyntaxElement element, bool allowed);
    void removeConstraintRule(ContextType context, SyntaxElement element);
    bool hasConstraintRule(ContextType context, SyntaxElement element);
    
    // 特殊语法检查（注释和原始嵌入可在任意位置使用）
    bool isUniversalElement(SyntaxElement element);
    bool isCHTLJSSpecificElement(SyntaxElement element);
    bool isCHTLSpecificElement(SyntaxElement element);
    
    // 上下文推断
    ContextType inferContextType(ast::ASTNode* node, ContextType parent_context);
    bool isGlobalContext(ContextType context);
    bool isLocalContext(ContextType context);
    bool isStyleContext(ContextType context);
    bool isScriptContext(ContextType context);
    
    // 错误报告
    void reportViolation(const ConstraintViolation& violation);
    std::vector<ConstraintViolation> getViolations() const;
    void clearViolations();
    
    // 建议生成
    std::string generateSuggestion(const ConstraintViolation& violation);
    std::vector<std::string> getAlternativeSyntax(ContextType context, SyntaxElement forbidden_element);
    
    // 配置和调试
    void setStrictMode(bool strict);
    bool isStrictMode() const;
    void printConstraintRules() const;
    std::string getConstraintStatistics() const;
    
private:
    // 约束规则表：context -> element -> allowed
    std::unordered_map<ContextType, std::unordered_map<SyntaxElement, bool>> constraint_rules_;
    std::vector<ConstraintViolation> violations_;
    bool strict_mode_;
    
    // 初始化约束规则
    void initializeConstraintRules();
    void setupGlobalStyleConstraints();
    void setupLocalStyleConstraints();
    void setupGlobalScriptConstraints();
    void setupLocalScriptConstraints();
    
    // 语法元素识别
    SyntaxElement identifySyntaxElement(ast::ASTNode* node);
    std::string getSyntaxElementName(SyntaxElement element) const;
    std::string getContextTypeName(ContextType context) const;
    
    // 节点分析
    bool containsCHTLSyntax(ast::ASTNode* node);
    bool containsCHTLJSSyntax(ast::ASTNode* node);
    std::vector<SyntaxElement> extractSyntaxElements(ast::ASTNode* node);
    
    // 错误处理
    void addViolation(ContextType context, SyntaxElement element, const std::string& element_name,
                     const std::string& message, size_t line, size_t column);
};

// 约束器工具函数
namespace ConstrainerUtils {
    // 上下文类型转换
    std::string contextTypeToString(ContextType type);
    std::string syntaxElementToString(SyntaxElement element);
    
    // 语法分类
    bool isVariableElement(SyntaxElement element);
    bool isStyleElement(SyntaxElement element);
    bool isElementElement(SyntaxElement element);
    bool isOperationElement(SyntaxElement element);
    
    // 约束规则辅助
    std::vector<SyntaxElement> getAllowedElementsInContext(ContextType context);
    std::vector<SyntaxElement> getForbiddenElementsInContext(ContextType context);
    
    // 建议生成
    std::vector<std::string> generateConstraintSuggestions(ContextType context, SyntaxElement element);
}

} // namespace chtl