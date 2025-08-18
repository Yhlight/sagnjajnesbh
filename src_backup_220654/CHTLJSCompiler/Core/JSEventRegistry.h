#pragma once
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

namespace chtl {
namespace chtljs {

/**
 * JavaScript事件注册表
 * 包含所有标准JS事件的完整列表
 */
class JSEventRegistry {
public:
    JSEventRegistry();
    ~JSEventRegistry() = default;
    
    // 事件查询
    bool isValidEvent(const std::string& event_name) const;
    std::vector<std::string> getAllEvents() const;
    std::vector<std::string> getEventsByCategory(const std::string& category) const;
    
    // 事件分类
    std::string getEventCategory(const std::string& event_name) const;
    std::vector<std::string> getAllCategories() const;
    
    // 事件信息
    std::string getEventDescription(const std::string& event_name) const;
    bool isDeprecatedEvent(const std::string& event_name) const;
    std::vector<std::string> getEventAliases(const std::string& event_name) const;
    
    // 静态访问
    static JSEventRegistry& getInstance();
    
private:
    std::unordered_set<std::string> all_events_;
    std::unordered_map<std::string, std::string> event_categories_;
    std::unordered_map<std::string, std::string> event_descriptions_;
    std::unordered_set<std::string> deprecated_events_;
    std::unordered_map<std::string, std::vector<std::string>> event_aliases_;
    
    // 初始化方法
    void initializeMouseEvents();
    void initializeKeyboardEvents();
    void initializeTouchEvents();
    void initializeFormEvents();
    void initializeWindowEvents();
    void initializeDocumentEvents();
    void initializeMediaEvents();
    void initializeDragEvents();
    void initializeClipboardEvents();
    void initializeAnimationEvents();
    void initializeTransitionEvents();
    void initializePointerEvents();
    void initializeCustomEvents();
    
    // 辅助方法
    void addEvent(const std::string& name, const std::string& category, const std::string& description);
    void addEventAlias(const std::string& event, const std::string& alias);
    void markDeprecated(const std::string& event);
};

/**
 * CHTL JS函数键值对管理器
 * 支持无序和可选键值对
 */
class CHTLJSKeyValueManager {
public:
    // 键值对信息
    struct KeyValueInfo {
        std::string key;
        std::string value;
        bool is_optional;
        bool is_function;
        std::string value_type; // "function", "string", "number", "boolean"
        
        KeyValueInfo(const std::string& k = "", const std::string& v = "", 
                    bool optional = true, bool func = false, const std::string& type = "function")
            : key(k), value(v), is_optional(optional), is_function(func), value_type(type) {}
    };
    
    CHTLJSKeyValueManager();
    ~CHTLJSKeyValueManager() = default;
    
    // 键值对解析
    std::vector<KeyValueInfo> parseKeyValuePairs(const std::string& input);
    bool validateKeyValuePair(const KeyValueInfo& kv, const std::string& function_name);
    
    // 无序支持
    std::vector<KeyValueInfo> reorderKeyValuePairs(const std::vector<KeyValueInfo>& pairs, 
                                                   const std::string& function_name);
    
    // 可选键值对支持
    std::vector<KeyValueInfo> filterOptionalKeys(const std::vector<KeyValueInfo>& pairs,
                                                 const std::string& function_name);
    bool isRequiredKey(const std::string& key, const std::string& function_name);
    
    // 函数特定的键定义
    void defineFunctionKeys(const std::string& function_name, const std::vector<std::string>& required_keys,
                           const std::vector<std::string>& optional_keys);
    std::vector<std::string> getRequiredKeys(const std::string& function_name) const;
    std::vector<std::string> getOptionalKeys(const std::string& function_name) const;
    std::vector<std::string> getAllKeys(const std::string& function_name) const;
    
    // 代码生成
    std::string generateKeyValueCode(const std::vector<KeyValueInfo>& pairs, const std::string& function_name);
    std::string generateEventListenerCode(const KeyValueInfo& kv);
    
private:
    std::unordered_map<std::string, std::vector<std::string>> function_required_keys_;
    std::unordered_map<std::string, std::vector<std::string>> function_optional_keys_;
    JSEventRegistry* event_registry_;
    
    // 解析辅助
    KeyValueInfo parseKeyValue(const std::string& kv_string);
    bool isValidKey(const std::string& key, const std::string& function_name);
    std::string inferValueType(const std::string& value);
    
    // 代码生成辅助
    std::string escapeJavaScript(const std::string& input);
    std::string formatFunctionValue(const std::string& value);
};

} // namespace chtljs
} // namespace chtl