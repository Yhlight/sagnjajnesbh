#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <functional>

namespace chtl {

// 前向声明
namespace ast {
    class ASTNode;
    class StyleBlockNode;
    class ScriptBlockNode;
}

namespace constraint {

/**
 * 语法元素类型
 * 用于约束系统
 */
enum class SyntaxElement {
    // 变量相关
    TEMPLATE_VAR,           // 模板变量
    CUSTOM_VAR,             // 自定义变量
    VAR_SPECIALIZATION,     // 变量特例化
    
    // 样式组相关
    TEMPLATE_STYLE,         // 模板样式组
    CUSTOM_STYLE,           // 自定义样式组
    VALUELESS_STYLE,        // 无值样式组
    STYLE_SPECIALIZATION,   // 样式组特例化
    
    // 样式操作
    DELETE_PROPERTY,        // delete属性
    DELETE_INHERITANCE,     // delete继承
    STYLE_INHERITANCE,      // 样式组之间的继承
    
    // 注释和原始嵌入
    GENERATOR_COMMENT,      // 生成器注释 --
    ORIGIN_EMBED,          // 原始嵌入（任意类型）
    
    // 命名空间
    NAMESPACE_FROM,         // from语法
    FULL_QUALIFIED_NAME,    // 全缀名
    
    // HTML相关
    HTML_ELEMENT,          // HTML元素
    
    // CHTL特殊语法
    CHTL_SYNTAX,           // 其他CHTL语法
    
    // 约束相关
    EXCEPT_CONSTRAINT      // except约束
};

/**
 * 上下文类型
 * 定义不同的语法上下文
 */
enum class ContextType {
    GLOBAL_STYLE,      // 全局样式块
    LOCAL_STYLE,       // 局部样式块  
    GLOBAL_SCRIPT,     // 全局script块
    LOCAL_SCRIPT,      // 局部script块
    ELEMENT_BODY,      // 元素内容
    NAMESPACE,         // 命名空间
    TOP_LEVEL          // 顶层
};

/**
 * 约束规则
 * 定义某个上下文中允许或禁止的语法元素
 */
class ConstraintRule {
public:
    ConstraintRule(ContextType context);
    ~ConstraintRule();
    
    /**
     * 添加允许的语法元素
     */
    void AddAllowedElement(SyntaxElement element);
    
    /**
     * 添加禁止的语法元素
     */
    void AddForbiddenElement(SyntaxElement element);
    
    /**
     * 检查语法元素是否允许
     */
    bool IsAllowed(SyntaxElement element) const;
    
    /**
     * 获取上下文类型
     */
    ContextType GetContext() const { return m_Context; }
    
    /**
     * 获取允许的元素列表
     */
    std::vector<SyntaxElement> GetAllowedElements() const;
    
    /**
     * 获取禁止的元素列表
     */
    std::vector<SyntaxElement> GetForbiddenElements() const;
    
public:
    static SyntaxElement GetSyntaxElement(const ast::ASTNode* node);

private:
    ContextType m_Context;
    std::unordered_set<SyntaxElement> m_AllowedElements;
    std::unordered_set<SyntaxElement> m_ForbiddenElements;
    bool m_UseWhitelist;  // true使用白名单，false使用黑名单
};

/**
 * 约束验证器
 * 验证AST节点是否符合约束规则
 */
class ConstraintValidator {
public:
    ConstraintValidator();
    ~ConstraintValidator();
    
    /**
     * 验证AST节点
     * @param node AST节点
     * @param context 当前上下文
     * @return 是否通过验证
     */
    bool Validate(ast::ASTNode* node, ContextType context);
    
    /**
     * 验证整个AST树
     * @param root AST根节点
     * @return 是否通过验证
     */
    bool ValidateTree(ast::ASTNode* root);
    
    /**
     * 获取验证错误
     */
    const std::vector<std::string>& GetErrors() const { return m_Errors; }
    void ClearErrors() { m_Errors.clear(); }
    
    /**
     * 设置严格模式
     */
    void SetStrictMode(bool strict) { m_StrictMode = strict; }
    
private:
    std::vector<std::string> m_Errors;
    bool m_StrictMode;
    
    // 验证特定类型的节点
    bool ValidateStyleBlock(ast::StyleBlockNode* node, ContextType context);
    bool ValidateScriptBlock(ast::ScriptBlockNode* node, ContextType context);
    
    // 获取节点对应的语法元素类型
    SyntaxElement GetSyntaxElement(ast::ASTNode* node) const;
    
    // 报告错误
    void ReportError(const std::string& message, ast::ASTNode* node);
};

/**
 * 约束管理器
 * 管理所有约束规则
 */
class ConstraintManager {
public:
    ConstraintManager();
    ~ConstraintManager();
    
    /**
     * 初始化默认约束规则
     */
    void InitializeDefaultRules();
    
    /**
     * 获取约束规则
     */
    ConstraintRule* GetRule(ContextType context);
    const ConstraintRule* GetRule(ContextType context) const;
    
    /**
     * 添加自定义约束规则
     */
    void AddRule(std::unique_ptr<ConstraintRule> rule);
    
    /**
     * 创建验证器
     */
    std::unique_ptr<ConstraintValidator> CreateValidator() const;
    
    /**
     * 获取所有规则
     */
    std::vector<const ConstraintRule*> GetAllRules() const;
    
private:
    std::unordered_map<ContextType, std::unique_ptr<ConstraintRule>> m_Rules;
    
    // 初始化各个上下文的规则
    void InitializeGlobalStyleRules();
    void InitializeLocalStyleRules();
    void InitializeGlobalScriptRules();
    void InitializeLocalScriptRules();
};

/**
 * 约束上下文守卫
 * RAII模式管理约束上下文
 */
class ConstraintContextGuard {
public:
    ConstraintContextGuard(ConstraintValidator& validator, ContextType newContext);
    ~ConstraintContextGuard();
    
    // 禁止拷贝
    ConstraintContextGuard(const ConstraintContextGuard&) = delete;
    ConstraintContextGuard& operator=(const ConstraintContextGuard&) = delete;
    
    // 允许移动
    ConstraintContextGuard(ConstraintContextGuard&&) = default;
    
private:
    ConstraintValidator& m_Validator;
    ContextType m_PreviousContext;
    ContextType m_CurrentContext;
};

/**
 * 约束异常
 * 用于报告约束违反
 */
class ConstraintViolationException : public std::exception {
public:
    ConstraintViolationException(const std::string& message,
                               SyntaxElement element,
                               ContextType context);
    
    const char* what() const noexcept override { return m_Message.c_str(); }
    SyntaxElement GetElement() const { return m_Element; }
    ContextType GetContext() const { return m_Context; }
    
private:
    std::string m_Message;
    SyntaxElement m_Element;
    ContextType m_Context;
};

/**
 * 约束辅助工具
 */
class ConstraintUtils {
public:
    /**
     * 将语法元素转换为字符串
     */
    static std::string ElementToString(SyntaxElement element);
    
    /**
     * 将上下文类型转换为字符串
     */
    static std::string ContextToString(ContextType context);
    
    /**
     * 检查是否是特殊允许的元素（注释和原始嵌入）
     */
    static bool IsUniversallyAllowed(SyntaxElement element);
    
    /**
     * 获取上下文的描述信息
     */
    static std::string GetContextDescription(ContextType context);
};

} // namespace constraint
} // namespace chtl