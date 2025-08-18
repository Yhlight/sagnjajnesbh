#pragma once
#include "../Selector/EnhancedSelectorProcessor.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace chtl {
namespace chtljs {

/**
 * Delegate事件委托系统处理器
 * 严格按照CHTL语法文档实现事件委托功能
 */
class DelegateSystemProcessor {
public:
    struct DelegateTarget {
        std::string selector;           // 目标选择器
        std::vector<std::string> targetSelectors; // 多目标选择器数组
        bool isArray = false;          // 是否为数组格式
    };
    
    struct DelegateBinding {
        std::string eventType;         // 事件类型
        std::string handlerCode;       // 处理函数代码
        std::string functionName;      // 函数名（如果是外部函数）
    };
    
    struct DelegateStatement {
        std::string parentSelector;           // 父元素选择器
        DelegateTarget target;                // 委托目标
        std::vector<DelegateBinding> events;  // 事件绑定
        std::string delegateId;               // 委托ID（用于全局注册表）
    };
    
    /**
     * 全局委托注册表项
     * 按语法文档：管理所有事件委托的父元素，避免重复绑定
     */
    struct GlobalDelegateEntry {
        std::string parentSelector;                    // 父元素选择器
        std::unordered_map<std::string, std::vector<DelegateBinding>> eventMap; // 事件类型 -> 绑定列表
        std::unordered_set<std::string> childSelectors; // 子元素选择器集合
        bool isRegistered = false;                     // 是否已注册到DOM
    };
    
    DelegateSystemProcessor();
    ~DelegateSystemProcessor();
    
    // === 按语法文档：delegate功能处理 ===
    
    /**
     * 处理delegate语句
     * 按语法文档：{{父元素选择器}}->delegate({ target: {{选择器}}, click: 函数 })
     */
    std::string processDelegateStatement(const std::string& delegateCode);
    
    /**
     * 解析delegate语句结构
     * 提取父元素、目标元素和事件绑定
     */
    DelegateStatement parseDelegateStatement(const std::string& delegateCode);
    
    /**
     * 生成事件委托的JavaScript代码
     * 按语法文档：基于事件冒泡的委托实现
     */
    std::string generateDelegateCode(const DelegateStatement& statement);
    
    // === 全局注册表管理 ===
    
    /**
     * 添加到全局委托注册表
     * 按语法文档：重复绑定父元素的子元素会作为分支合并
     */
    void addToGlobalRegistry(const DelegateStatement& statement);
    
    /**
     * 合并相同父元素的委托
     * 按语法文档：避免创建多个相同的事件委托
     */
    void mergeDelegateBindings(const std::string& parentSelector, const DelegateStatement& statement);
    
    /**
     * 生成全局委托注册代码
     * 为所有注册的委托生成统一的JavaScript代码
     */
    std::string generateGlobalDelegateRegistry();
    
    /**
     * 检查是否已存在委托
     */
    bool hasDelegateForParent(const std::string& parentSelector, const std::string& eventType);
    
    // === 委托目标处理 ===
    
    /**
     * 解析委托目标
     * 按语法文档：target: {{选择器}} | [{{选择器1}}, {{选择器2}},...]
     */
    DelegateTarget parseDelegateTarget(const std::string& targetDefinition);
    
    /**
     * 处理单个目标
     * target: {{选择器}}
     */
    DelegateTarget parseSingleTarget(const std::string& targetSelector);
    
    /**
     * 处理多个目标
     * target: [{{选择器1}}, {{选择器2}}]
     */
    DelegateTarget parseArrayTarget(const std::string& targetArray);
    
    // === JavaScript代码生成 ===
    
    /**
     * 生成事件匹配逻辑
     * 检查事件目标是否匹配委托选择器
     */
    std::string generateEventMatchingLogic(const DelegateTarget& target);
    
    /**
     * 生成委托事件处理器
     */
    std::string generateDelegateEventHandler(const DelegateBinding& binding, const DelegateTarget& target);
    
    /**
     * 生成父元素绑定代码
     */
    std::string generateParentElementBinding(const std::string& parentSelector, const std::string& eventType, 
                                            const std::string& handlerCode);
    
    // === 辅助方法 ===
    
    /**
     * 提取父元素选择器
     * 从 {{父元素}}->delegate(...) 中提取 {{父元素}}
     */
    std::string extractParentSelector(const std::string& delegateCode);
    
    /**
     * 提取委托对象
     * 从 delegate({ ... }) 中提取 { ... }
     */
    std::string extractDelegateObject(const std::string& delegateCode);
    
    /**
     * 生成唯一的委托ID
     */
    std::string generateDelegateId(const std::string& parentSelector, const std::string& eventType);
    
    /**
     * 清理和验证选择器
     */
    std::string cleanAndValidateSelector(const std::string& selector);
    
    // === 错误处理 ===
    void addError(const std::string& message);
    void clearErrors();
    bool hasErrors() const;
    std::vector<std::string> getErrors() const;

private:
    std::shared_ptr<EnhancedSelectorProcessor> selectorProcessor_;
    std::vector<std::string> errors_;
    
    // 全局委托注册表
    std::unordered_map<std::string, GlobalDelegateEntry> globalDelegateRegistry_;
    
    // 支持的事件类型
    std::vector<std::string> supportedEvents_;
    
    void initializeSupportedEvents();
};

} // namespace chtljs
} // namespace chtl