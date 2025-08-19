#pragma once
#include "../Selector/EnhancedSelectorProcessor.h"
#include "../Event/ListenSystemProcessor.h"
#include "../Event/DelegateSystemProcessor.h"
#include "../Animation/AnimateSystemProcessor.h"
#include "../Virtual/VirtualObjectProcessor.h"
#include "../Operator/ArrowOperatorProcessor.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace chtl {
namespace chtljs {

/**
 * 局部Script块处理器
 * 严格按照CHTL语法文档实现局部script块的全局合并和优先级处理
 */
class LocalScriptProcessor {
public:
    struct ScriptBlock {
        std::string elementContext;    // 所属元素上下文
        std::string content;           // script内容
        int priority = 0;              // 优先级
        bool isProcessed = false;      // 是否已处理
        std::string processedContent;  // 处理后的内容
    };
    
    struct GlobalScriptRegistry {
        std::vector<ScriptBlock> scriptBlocks;           // 所有局部script块
        std::unordered_map<std::string, int> priorityMap; // 元素类型优先级映射
        std::string combinedScript;                      // 合并后的全局script
    };
    
    LocalScriptProcessor();
    ~LocalScriptProcessor();
    
    // === 按语法文档：局部script处理 ===
    
    /**
     * 处理局部script块
     * 按语法文档：局部script会被添加到一个不会全局污染，具有高优先级的全局script块之中
     */
    std::string processLocalScript(const std::string& scriptContent, const std::string& elementContext);
    
    /**
     * 合并所有局部script到全局script
     * 按语法文档：具有高优先级的全局script块
     */
    std::string generateGlobalScript();
    
    /**
     * 处理script内容中的CHTL JS语法
     * 转换增强选择器、箭头操作符、listen等
     */
    std::string processCHTLJSSyntax(const std::string& scriptContent);
    
    // === 优先级管理 ===
    
    /**
     * 计算script块的优先级
     * 按语法文档：确定局部script的优先级
     */
    int calculateScriptPriority(const std::string& elementContext);
    
    /**
     * 按优先级排序script块
     */
    void sortScriptBlocksByPriority();
    
    /**
     * 设置元素类型的优先级
     */
    void setElementPriority(const std::string& elementType, int priority);
    
    // === CHTL JS语法处理 ===
    
    /**
     * 处理增强选择器
     * {{box}}, {{.box}}, {{#box}} 等
     */
    std::string processEnhancedSelectors(const std::string& content);
    
    /**
     * 处理箭头操作符
     * -> 转换为 .
     */
    std::string processArrowOperators(const std::string& content);
    
    /**
     * 处理listen语句
     * {{box}}->listen({ ... })
     */
    std::string processListenStatements(const std::string& content);
    
    /**
     * 处理delegate语句
     * {{parent}}->delegate({ ... })
     */
    std::string processDelegateStatements(const std::string& content);
    
    /**
     * 处理animate语句
     * animate({ ... })
     */
    std::string processAnimateStatements(const std::string& content);
    
    /**
     * 处理vir虚对象定义
     * vir test = listen({ ... });
     */
    std::string processVirtualObjectDefinitions(const std::string& content);
    
    /**
     * 处理vir虚对象调用
     * test->click();
     */
    std::string processVirtualObjectCalls(const std::string& content);
    
    // === 全局污染防护 ===
    
    /**
     * 包装script内容防止全局污染
     * 按语法文档：不会全局污染的script块
     */
    std::string wrapScriptForIsolation(const std::string& content, const std::string& context);
    
    /**
     * 生成命名空间保护
     */
    std::string generateNamespaceProtection(const std::string& content);
    
    /**
     * 处理变量作用域
     */
    std::string processVariableScope(const std::string& content);
    
    // === 辅助方法 ===
    
    /**
     * 检测CHTL JS语法
     * 识别内容中是否包含CHTL JS特殊语法
     */
    bool containsCHTLJSSyntax(const std::string& content);
    
    /**
     * 提取所有增强选择器
     * 找到所有 {{...}} 格式的选择器
     */
    std::vector<std::string> extractAllSelectors(const std::string& content);
    
    /**
     * 提取所有方法调用
     * 找到所有 target->method() 格式的调用
     */
    std::vector<std::string> extractAllMethodCalls(const std::string& content);
    
    /**
     * 生成唯一的script块ID
     */
    std::string generateScriptBlockId(const std::string& elementContext);
    
    /**
     * 清理和格式化script内容
     */
    std::string cleanAndFormatScript(const std::string& content);
    
    // === 错误处理 ===
    void addError(const std::string& message);
    void clearErrors();
    bool hasErrors() const;
    std::vector<std::string> getErrors() const;

private:
    // CHTL JS处理器
    std::shared_ptr<EnhancedSelectorProcessor> selectorProcessor_;
    std::shared_ptr<ListenSystemProcessor> listenProcessor_;
    std::shared_ptr<DelegateSystemProcessor> delegateProcessor_;
    std::shared_ptr<AnimateSystemProcessor> animateProcessor_;
    std::shared_ptr<VirtualObjectProcessor> virtualProcessor_;
    std::shared_ptr<ArrowOperatorProcessor> arrowProcessor_;
    
    // 全局script注册表
    GlobalScriptRegistry globalRegistry_;
    
    std::vector<std::string> errors_;
    
    // 默认优先级配置
    void initializeDefaultPriorities();
};

} // namespace chtljs
} // namespace chtl