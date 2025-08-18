#pragma once
#include <string>
#include <vector>
#include <unordered_map>

namespace chtl {
namespace chtljs {

/**
 * 箭头操作符处理器
 * 严格按照CHTL语法文档实现 -> 与 . 的完全等价性
 */
class ArrowOperatorProcessor {
public:
    struct MethodCall {
        std::string target;         // 调用目标
        std::string method;         // 方法名
        std::string parameters;     // 参数列表
        bool isChained = false;     // 是否为链式调用
    };
    
    ArrowOperatorProcessor();
    ~ArrowOperatorProcessor();
    
    // === 按语法文档：箭头操作符处理 ===
    
    /**
     * 处理箭头操作符表达式
     * 按语法文档：-> 与 . 是完全等价的，可以直接使用->进行链式访问
     */
    std::string processArrowExpression(const std::string& expression);
    
    /**
     * 转换箭头操作符为点操作符
     * 按语法文档：{{box}}->textContent() 等价于 {{box}}.textContent()
     */
    std::string convertArrowToDot(const std::string& expression);
    
    /**
     * 解析方法调用链
     * 支持链式调用：{{box}}->method1()->method2()
     */
    std::vector<MethodCall> parseMethodChain(const std::string& expression);
    
    /**
     * 生成等价的JavaScript代码
     * 确保箭头操作符和点操作符的完全等价性
     */
    std::string generateEquivalentCode(const std::string& expression);
    
    // === 链式调用处理 ===
    
    /**
     * 处理单个方法调用
     * target->method(params)
     */
    MethodCall parseMethodCall(const std::string& callExpression);
    
    /**
     * 处理链式调用
     * target->method1()->method2()->method3()
     */
    std::string processChainedCalls(const std::vector<MethodCall>& methodChain);
    
    /**
     * 验证方法调用的有效性
     */
    bool isValidMethodCall(const MethodCall& call);
    
    // === CHTL JS特殊方法处理 ===
    
    /**
     * 检查是否为CHTL JS特殊方法
     * listen, delegate, animate 等
     */
    bool isCHTLJSMethod(const std::string& methodName);
    
    /**
     * 处理CHTL JS特殊方法
     * 这些方法需要特殊的代码生成逻辑
     */
    std::string processCHTLJSMethod(const std::string& target, const std::string& method, const std::string& params);
    
    // === 辅助方法 ===
    
    /**
     * 提取目标对象
     * 从 {{box}}->method() 中提取 {{box}}
     */
    std::string extractTarget(const std::string& expression);
    
    /**
     * 提取方法名
     * 从 target->method() 中提取 method
     */
    std::string extractMethod(const std::string& expression);
    
    /**
     * 提取参数列表
     * 从 method(param1, param2) 中提取 param1, param2
     */
    std::string extractParameters(const std::string& methodCall);
    
    /**
     * 分割链式调用
     * target->method1()->method2() → [target->method1(), method2()]
     */
    std::vector<std::string> splitChainedCalls(const std::string& expression);
    
    /**
     * 验证表达式格式
     */
    bool isValidArrowExpression(const std::string& expression);
    
    /**
     * 清理和格式化表达式
     */
    std::string cleanExpression(const std::string& expression);
    
    // === 错误处理 ===
    void addError(const std::string& message);
    void clearErrors();
    bool hasErrors() const;
    std::vector<std::string> getErrors() const;

private:
    std::vector<std::string> errors_;
    
    // CHTL JS特殊方法列表
    std::vector<std::string> chtljsMethods_;
    
    // 标准DOM方法列表
    std::vector<std::string> domMethods_;
    
    void initializeCHTLJSMethods();
    void initializeDOMMethods();
};

} // namespace chtljs
} // namespace chtl