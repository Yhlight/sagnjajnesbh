#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

namespace CHTLJS {
namespace Events {

/**
 * @brief JS事件类型枚举
 */
enum class JSEventType {
    // 鼠标事件
    CLICK,              // click
    DBLCLICK,           // dblclick
    MOUSEDOWN,          // mousedown
    MOUSEUP,            // mouseup
    MOUSEOVER,          // mouseover
    MOUSEOUT,           // mouseout
    MOUSEMOVE,          // mousemove
    MOUSEENTER,         // mouseenter
    MOUSELEAVE,         // mouseleave
    CONTEXTMENU,        // contextmenu
    WHEEL,              // wheel
    
    // 键盘事件
    KEYDOWN,            // keydown
    KEYUP,              // keyup
    KEYPRESS,           // keypress
    
    // 表单事件
    SUBMIT,             // submit
    RESET,              // reset
    FOCUS,              // focus
    BLUR,               // blur
    CHANGE,             // change
    INPUT,              // input
    SELECT,             // select
    
    // 窗口事件
    LOAD,               // load
    UNLOAD,             // unload
    BEFOREUNLOAD,       // beforeunload
    RESIZE,             // resize
    SCROLL,             // scroll
    
    // 触摸事件
    TOUCHSTART,         // touchstart
    TOUCHEND,           // touchend
    TOUCHMOVE,          // touchmove
    TOUCHCANCEL,        // touchcancel
    
    // 拖拽事件
    DRAG,               // drag
    DRAGSTART,          // dragstart
    DRAGEND,            // dragend
    DRAGOVER,           // dragover
    DRAGENTER,          // dragenter
    DRAGLEAVE,          // dragleave
    DROP,               // drop
    
    // 媒体事件
    PLAY,               // play
    PAUSE,              // pause
    ENDED,              // ended
    TIMEUPDATE,         // timeupdate
    VOLUMECHANGE,       // volumechange
    LOADSTART,          // loadstart
    LOADEDDATA,         // loadeddata
    LOADEDMETADATA,     // loadedmetadata
    CANPLAY,            // canplay
    CANPLAYTHROUGH,     // canplaythrough
    
    // 网络事件
    ONLINE,             // online
    OFFLINE,            // offline
    
    // 其他事件
    ERROR,              // error
    ABORT,              // abort
    TOGGLE,             // toggle
    SHOW,               // show
    HIDE,               // hide
    OPEN,               // open
    CLOSE,              // close
    
    // 自定义事件
    CUSTOM              // 自定义事件
};

/**
 * @brief JS事件管理器
 */
class JSEventManager {
public:
    /**
     * @brief 构造函数
     */
    JSEventManager();
    
    /**
     * @brief 检查是否为有效的JS事件
     * @param eventName 事件名称
     * @return 是否为有效事件
     */
    bool IsValidJSEvent(const std::string& eventName) const;
    
    /**
     * @brief 获取事件类型
     * @param eventName 事件名称
     * @return 事件类型
     */
    JSEventType GetEventType(const std::string& eventName) const;
    
    /**
     * @brief 获取所有支持的事件名称
     * @return 事件名称列表
     */
    std::vector<std::string> GetAllEventNames() const;
    
    /**
     * @brief 获取事件分类
     * @param eventType 事件类型
     * @return 事件分类名称
     */
    std::string GetEventCategory(JSEventType eventType) const;
    
    /**
     * @brief 添加自定义事件
     * @param eventName 自定义事件名称
     */
    void AddCustomEvent(const std::string& eventName);
    
    /**
     * @brief 检查是否为自定义事件
     * @param eventName 事件名称
     * @return 是否为自定义事件
     */
    bool IsCustomEvent(const std::string& eventName) const;

private:
    /**
     * @brief 初始化事件映射
     */
    void InitializeEventMap();

private:
    std::unordered_map<std::string, JSEventType> eventMap_;     // 事件名称映射
    std::unordered_set<std::string> customEvents_;             // 自定义事件集合
    bool eventMapInitialized_;                                 // 是否已初始化
};

/**
 * @brief 键值对灵活性管理器
 */
class KeyValueFlexibilityManager {
public:
    /**
     * @brief 构造函数
     */
    KeyValueFlexibilityManager();
    
    /**
     * @brief 解析键值对参数（支持无序和可选）
     * @param keyValueString 键值对字符串
     * @param requiredKeys 必需的键列表
     * @param optionalKeys 可选的键列表
     * @return 解析后的键值对映射
     */
    std::unordered_map<std::string, std::string> ParseKeyValuePairs(
        const std::string& keyValueString,
        const std::vector<std::string>& requiredKeys = {},
        const std::vector<std::string>& optionalKeys = {}
    );
    
    /**
     * @brief 验证键值对完整性
     * @param keyValueMap 键值对映射
     * @param requiredKeys 必需的键列表
     * @return 是否完整
     */
    bool ValidateKeyValuePairs(
        const std::unordered_map<std::string, std::string>& keyValueMap,
        const std::vector<std::string>& requiredKeys
    ) const;
    
    /**
     * @brief 获取键值对的默认值
     * @param key 键名
     * @return 默认值
     */
    std::string GetDefaultValue(const std::string& key) const;
    
    /**
     * @brief 设置键值对的默认值
     * @param key 键名
     * @param defaultValue 默认值
     */
    void SetDefaultValue(const std::string& key, const std::string& defaultValue);

private:
    std::unordered_map<std::string, std::string> defaultValues_;    // 默认值映射
};

/**
 * @brief 无修饰字面量管理器
 */
class UndecoratedLiteralManager {
public:
    /**
     * @brief 构造函数
     */
    UndecoratedLiteralManager();
    
    /**
     * @brief 检查是否为无修饰字面量
     * @param literal 字面量字符串
     * @return 是否为无修饰字面量
     */
    bool IsUndecoratedLiteral(const std::string& literal) const;
    
    /**
     * @brief 解析无修饰字面量
     * @param literal 字面量字符串
     * @return 解析后的值
     */
    std::string ParseUndecoratedLiteral(const std::string& literal) const;
    
    /**
     * @brief 获取字面量类型
     * @param literal 字面量字符串
     * @return 类型名称
     */
    std::string GetLiteralType(const std::string& literal) const;

private:
    /**
     * @brief 检查是否为数字字面量
     * @param literal 字面量字符串
     * @return 是否为数字
     */
    bool IsNumberLiteral(const std::string& literal) const;
    
    /**
     * @brief 检查是否为布尔字面量
     * @param literal 字面量字符串
     * @return 是否为布尔值
     */
    bool IsBooleanLiteral(const std::string& literal) const;
    
    /**
     * @brief 检查是否为标识符
     * @param literal 字面量字符串
     * @return 是否为标识符
     */
    bool IsIdentifier(const std::string& literal) const;
};

} // namespace Events
} // namespace CHTLJS