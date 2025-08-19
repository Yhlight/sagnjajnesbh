#pragma once
#include "../Selector/EnhancedSelectorProcessor.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace chtl {
namespace chtljs {

/**
 * Listen系统处理器
 * 严格按照CHTL语法文档实现 listen 增强监听器功能
 */
class ListenSystemProcessor {
public:
    struct EventBinding {
        std::string eventType;      // click, mouseenter, mousemove等
        std::string handlerCode;    // 事件处理函数代码
        bool isInlineFunction;      // 是否为内联函数
        std::string functionName;   // 外部函数名（如果有）
    };
    
    struct ListenStatement {
        std::string targetSelector;           // 目标选择器 {{box}}
        std::vector<EventBinding> events;     // 事件绑定列表
        std::string generatedCode;            // 生成的JavaScript代码
    };
    
    ListenSystemProcessor();
    ~ListenSystemProcessor();
    
    // === 按语法文档：listen功能处理 ===
    
    /**
     * 处理listen语句
     * 按语法文档：{{box}}->listen({ click: () => {}, mouseenter: func })
     */
    std::string processListenStatement(const std::string& listenCode);
    
    /**
     * 解析listen语句结构
     * 提取目标选择器和事件绑定
     */
    ListenStatement parseListenStatement(const std::string& listenCode);
    
    /**
     * 生成事件绑定的JavaScript代码
     * 按语法文档：转换为标准的addEventListener调用
     */
    std::string generateEventBindingCode(const ListenStatement& statement);
    
    // === 事件类型处理 ===
    
    /**
     * 解析事件绑定对象
     * 按语法文档：{ click: () => {}, mouseenter: func, mousemove: function() {} }
     */
    std::vector<EventBinding> parseEventBindings(const std::string& eventObject);
    
    /**
     * 处理内联函数
     * 按语法文档：() => {} 或 function() {}
     */
    std::string processInlineFunction(const std::string& functionCode);
    
    /**
     * 处理外部函数引用
     * 按语法文档：mouseEnterEvent // 已经存在的函数
     */
    std::string processExternalFunction(const std::string& functionName);
    
    /**
     * 验证事件类型
     * 按语法文档：支持所有标准DOM事件
     */
    bool isValidEventType(const std::string& eventType);
    
    // === JavaScript代码生成 ===
    
    /**
     * 生成单个事件绑定代码
     */
    std::string generateSingleEventBinding(const std::string& targetJs, const EventBinding& binding);
    
    /**
     * 生成多事件绑定代码
     */
    std::string generateMultiEventBinding(const std::string& targetJs, const std::vector<EventBinding>& bindings);
    
    /**
     * 包装为安全的DOM操作
     * 确保元素存在时才绑定事件
     */
    std::string wrapWithSafetyCheck(const std::string& targetJs, const std::string& bindingCode);
    
    // === 辅助方法 ===
    
    /**
     * 提取选择器部分
     * 从 {{box}}->listen(...) 中提取 {{box}}
     */
    std::string extractSelector(const std::string& listenCode);
    
    /**
     * 提取事件对象部分
     * 从 listen({ ... }) 中提取 { ... }
     */
    std::string extractEventObject(const std::string& listenCode);
    
    /**
     * 解析事件对象中的键值对
     */
    std::vector<std::pair<std::string, std::string>> parseEventKeyValuePairs(const std::string& eventObject);
    
    /**
     * 清理和格式化代码
     */
    std::string cleanAndFormatCode(const std::string& code);
    
    // === 错误处理 ===
    void addError(const std::string& message);
    void clearErrors();
    bool hasErrors() const;
    std::vector<std::string> getErrors() const;

private:
    std::shared_ptr<EnhancedSelectorProcessor> selectorProcessor_;
    std::vector<std::string> errors_;
    
    // 支持的DOM事件类型（按语法文档）
    std::vector<std::string> supportedEvents_;
    
    void initializeSupportedEvents();
    
    // 缺少的方法声明
    std::vector<std::string> splitEventEntries(const std::string& content);
};

} // namespace chtljs
} // namespace chtl