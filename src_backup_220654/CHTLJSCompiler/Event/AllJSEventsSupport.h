#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace chtl {
namespace chtljs {

/**
 * 完整的JavaScript事件支持
 * listen和delegate应该支持所有JS事件
 */
class AllJSEventsSupport {
public:
    struct EventCategory {
        std::string name;
        std::vector<std::string> events;
        std::string description;
    };
    
    struct EventInfo {
        std::string name;           // 事件名称
        std::string category;       // 事件分类
        std::string description;    // 事件描述
        bool isBubbling = true;     // 是否支持冒泡
        bool isCancelable = true;   // 是否可取消
        std::vector<std::string> supportedElements; // 支持的元素类型
    };
    
    AllJSEventsSupport();
    ~AllJSEventsSupport();
    
    // === 完整事件支持 ===
    
    /**
     * 获取所有支持的JavaScript事件
     * 包括DOM事件、自定义事件、现代Web API事件
     */
    std::vector<std::string> getAllSupportedEvents();
    
    /**
     * 按分类获取事件
     */
    std::vector<std::string> getEventsByCategory(const std::string& category);
    
    /**
     * 获取所有事件分类
     */
    std::vector<EventCategory> getAllEventCategories();
    
    /**
     * 验证事件名称
     */
    bool isValidEventName(const std::string& eventName);
    
    /**
     * 获取事件信息
     */
    EventInfo getEventInfo(const std::string& eventName);
    
    // === 事件兼容性检查 ===
    
    /**
     * 检查事件与元素的兼容性
     * 某些事件只在特定元素上有效
     */
    bool isEventCompatibleWithElement(const std::string& eventName, const std::string& elementType);
    
    /**
     * 获取元素支持的事件列表
     */
    std::vector<std::string> getSupportedEventsForElement(const std::string& elementType);
    
    /**
     * 检查事件是否支持冒泡
     * 对delegate事件委托很重要
     */
    bool isEventBubbling(const std::string& eventName);
    
    // === 事件代码生成优化 ===
    
    /**
     * 生成优化的事件绑定代码
     * 根据事件特性生成最优的JavaScript代码
     */
    std::string generateOptimizedEventBinding(const std::string& eventName, const std::string& handler, const std::string& target);
    
    /**
     * 生成事件委托代码
     * 考虑事件冒泡特性
     */
    std::string generateEventDelegateCode(const std::string& eventName, const std::string& parentSelector, 
                                          const std::string& childSelector, const std::string& handler);
    
    /**
     * 生成事件兼容性检查代码
     */
    std::string generateCompatibilityCheck(const std::string& eventName, const std::string& elementType);

private:
    // 完整的JavaScript事件映射
    std::unordered_map<std::string, EventInfo> eventInfoMap_;
    std::unordered_map<std::string, EventCategory> eventCategories_;
    
    // 元素与事件的兼容性映射
    std::unordered_map<std::string, std::unordered_set<std::string>> elementEventCompatibility_;
    
    void initializeAllJSEvents();
    void initializeEventCategories();
    void initializeElementCompatibility();
};

} // namespace chtljs
} // namespace chtl