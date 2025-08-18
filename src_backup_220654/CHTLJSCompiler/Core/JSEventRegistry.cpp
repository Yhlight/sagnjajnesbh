#include <sstream>
#include "JSEventRegistry.h"
#include <algorithm>

namespace chtl {
namespace chtljs {

JSEventRegistry::JSEventRegistry() {
    // 初始化所有事件类型
    initializeMouseEvents();
    initializeKeyboardEvents();
    initializeTouchEvents();
    initializeFormEvents();
    initializeWindowEvents();
    initializeDocumentEvents();
    initializeMediaEvents();
    initializeDragEvents();
    initializeClipboardEvents();
    initializeAnimationEvents();
    initializeTransitionEvents();
    initializePointerEvents();
    initializeCustomEvents();
}

void JSEventRegistry::initializeMouseEvents() {
    // 鼠标事件
    addEvent("click", "mouse", "鼠标点击事件");
    addEvent("dblclick", "mouse", "鼠标双击事件");
    addEvent("mousedown", "mouse", "鼠标按下事件");
    addEvent("mouseup", "mouse", "鼠标释放事件");
    addEvent("mousemove", "mouse", "鼠标移动事件");
    addEvent("mouseover", "mouse", "鼠标悬停事件");
    addEvent("mouseout", "mouse", "鼠标离开事件");
    addEvent("mouseenter", "mouse", "鼠标进入事件");
    addEvent("mouseleave", "mouse", "鼠标离开事件");
    addEvent("contextmenu", "mouse", "右键菜单事件");
    addEvent("wheel", "mouse", "鼠标滚轮事件");
    
    // 添加别名
    addEventAlias("click", "onclick");
    addEventAlias("mousedown", "onmousedown");
    addEventAlias("mouseup", "onmouseup");
}

void JSEventRegistry::initializeKeyboardEvents() {
    // 键盘事件
    addEvent("keydown", "keyboard", "键盘按下事件");
    addEvent("keyup", "keyboard", "键盘释放事件");
    addEvent("keypress", "keyboard", "键盘按键事件");
    
    // 标记已废弃的事件
    markDeprecated("keypress");
    
    addEventAlias("keydown", "onkeydown");
    addEventAlias("keyup", "onkeyup");
}

void JSEventRegistry::initializeTouchEvents() {
    // 触摸事件
    addEvent("touchstart", "touch", "触摸开始事件");
    addEvent("touchend", "touch", "触摸结束事件");
    addEvent("touchmove", "touch", "触摸移动事件");
    addEvent("touchcancel", "touch", "触摸取消事件");
}

void JSEventRegistry::initializeFormEvents() {
    // 表单事件
    addEvent("submit", "form", "表单提交事件");
    addEvent("reset", "form", "表单重置事件");
    addEvent("change", "form", "值改变事件");
    addEvent("input", "form", "输入事件");
    addEvent("focus", "form", "获得焦点事件");
    addEvent("blur", "form", "失去焦点事件");
    addEvent("focusin", "form", "焦点进入事件");
    addEvent("focusout", "form", "焦点离开事件");
    addEvent("select", "form", "文本选择事件");
    addEvent("invalid", "form", "验证失败事件");
}

void JSEventRegistry::initializeWindowEvents() {
    // 窗口事件
    addEvent("load", "window", "页面加载完成事件");
    addEvent("unload", "window", "页面卸载事件");
    addEvent("beforeunload", "window", "页面卸载前事件");
    addEvent("resize", "window", "窗口大小改变事件");
    addEvent("scroll", "window", "滚动事件");
    addEvent("hashchange", "window", "哈希改变事件");
    addEvent("popstate", "window", "历史状态改变事件");
    addEvent("storage", "window", "存储改变事件");
    addEvent("online", "window", "网络连接事件");
    addEvent("offline", "window", "网络断开事件");
}

void JSEventRegistry::initializeDocumentEvents() {
    // 文档事件
    addEvent("DOMContentLoaded", "document", "DOM内容加载完成事件");
    addEvent("readystatechange", "document", "文档状态改变事件");
    addEvent("visibilitychange", "document", "页面可见性改变事件");
}

void JSEventRegistry::initializeMediaEvents() {
    // 媒体事件
    addEvent("play", "media", "媒体播放事件");
    addEvent("pause", "media", "媒体暂停事件");
    addEvent("ended", "media", "媒体播放结束事件");
    addEvent("volumechange", "media", "音量改变事件");
    addEvent("timeupdate", "media", "播放时间更新事件");
    addEvent("loadstart", "media", "开始加载媒体事件");
    addEvent("loadeddata", "media", "媒体数据加载完成事件");
    addEvent("loadedmetadata", "media", "媒体元数据加载完成事件");
    addEvent("canplay", "media", "可以播放事件");
    addEvent("canplaythrough", "media", "可以连续播放事件");
}

void JSEventRegistry::initializeDragEvents() {
    // 拖拽事件
    addEvent("drag", "drag", "拖拽事件");
    addEvent("dragstart", "drag", "开始拖拽事件");
    addEvent("dragend", "drag", "结束拖拽事件");
    addEvent("dragenter", "drag", "拖拽进入事件");
    addEvent("dragover", "drag", "拖拽悬停事件");
    addEvent("dragleave", "drag", "拖拽离开事件");
    addEvent("drop", "drag", "放置事件");
}

void JSEventRegistry::initializeClipboardEvents() {
    // 剪贴板事件
    addEvent("copy", "clipboard", "复制事件");
    addEvent("cut", "clipboard", "剪切事件");
    addEvent("paste", "clipboard", "粘贴事件");
}

void JSEventRegistry::initializeAnimationEvents() {
    // 动画事件
    addEvent("animationstart", "animation", "动画开始事件");
    addEvent("animationend", "animation", "动画结束事件");
    addEvent("animationiteration", "animation", "动画迭代事件");
}

void JSEventRegistry::initializeTransitionEvents() {
    // 过渡事件
    addEvent("transitionstart", "transition", "过渡开始事件");
    addEvent("transitionend", "transition", "过渡结束事件");
    addEvent("transitionrun", "transition", "过渡运行事件");
    addEvent("transitioncancel", "transition", "过渡取消事件");
}

void JSEventRegistry::initializePointerEvents() {
    // 指针事件
    addEvent("pointerdown", "pointer", "指针按下事件");
    addEvent("pointerup", "pointer", "指针释放事件");
    addEvent("pointermove", "pointer", "指针移动事件");
    addEvent("pointerenter", "pointer", "指针进入事件");
    addEvent("pointerleave", "pointer", "指针离开事件");
    addEvent("pointerover", "pointer", "指针悬停事件");
    addEvent("pointerout", "pointer", "指针离开事件");
    addEvent("pointercancel", "pointer", "指针取消事件");
}

void JSEventRegistry::initializeCustomEvents() {
    // 自定义事件和其他事件
    addEvent("error", "error", "错误事件");
    addEvent("abort", "error", "中止事件");
    addEvent("beforeprint", "print", "打印前事件");
    addEvent("afterprint", "print", "打印后事件");
    addEvent("message", "communication", "消息事件");
    addEvent("messageerror", "communication", "消息错误事件");
}

bool JSEventRegistry::isValidEvent(const std::string& event_name) const {
    return all_events_.find(event_name) != all_events_.end();
}

std::vector<std::string> JSEventRegistry::getAllEvents() const {
    return std::vector<std::string>(all_events_.begin(), all_events_.end());
}

std::vector<std::string> JSEventRegistry::getEventsByCategory(const std::string& category) const {
    std::vector<std::string> events;
    
    for (const auto& [event, cat] : event_categories_) {
        if (cat == category) {
            events.push_back(event);
        }
    }
    
    return events;
}

std::string JSEventRegistry::getEventCategory(const std::string& event_name) const {
    auto it = event_categories_.find(event_name);
    return it != event_categories_.end() ? it->second : "unknown";
}

std::vector<std::string> JSEventRegistry::getAllCategories() const {
    std::unordered_set<std::string> categories;
    
    for (const auto& [event, category] : event_categories_) {
        categories.insert(category);
    }
    
    return std::vector<std::string>(categories.begin(), categories.end());
}

std::string JSEventRegistry::getEventDescription(const std::string& event_name) const {
    auto it = event_descriptions_.find(event_name);
    return it != event_descriptions_.end() ? it->second : "";
}

bool JSEventRegistry::isDeprecatedEvent(const std::string& event_name) const {
    return deprecated_events_.find(event_name) != deprecated_events_.end();
}

std::vector<std::string> JSEventRegistry::getEventAliases(const std::string& event_name) const {
    auto it = event_aliases_.find(event_name);
    return it != event_aliases_.end() ? it->second : std::vector<std::string>();
}

JSEventRegistry& JSEventRegistry::getInstance() {
    static JSEventRegistry instance;
    return instance;
}

void JSEventRegistry::addEvent(const std::string& name, const std::string& category, const std::string& description) {
    all_events_.insert(name);
    event_categories_[name] = category;
    event_descriptions_[name] = description;
}

void JSEventRegistry::addEventAlias(const std::string& event, const std::string& alias) {
    event_aliases_[event].push_back(alias);
}

void JSEventRegistry::markDeprecated(const std::string& event) {
    deprecated_events_.insert(event);
}

// CHTLJSKeyValueManager实现

CHTLJSKeyValueManager::CHTLJSKeyValueManager() {
    event_registry_ = &JSEventRegistry::getInstance();
    
    // 定义listen函数的键
    std::vector<std::string> listen_optional_keys = event_registry_->getAllEvents();
    defineFunctionKeys("listen", {}, listen_optional_keys); // listen所有键都是可选的
    
    // 定义delegate函数的键
    std::vector<std::string> delegate_required_keys = {"selector"}; // delegate需要selector
    std::vector<std::string> delegate_optional_keys = event_registry_->getAllEvents();
    defineFunctionKeys("delegate", delegate_required_keys, delegate_optional_keys);
    
    // 定义animate函数的键
    std::vector<std::string> animate_required_keys = {"duration"};
    std::vector<std::string> animate_optional_keys = {
        "delay", "easing", "fill", "direction", "iterations", "playbackRate",
        "from", "to", "keyframes", "composite", "iterationComposite"
    };
    defineFunctionKeys("animate", animate_required_keys, animate_optional_keys);
}

std::vector<CHTLJSKeyValueManager::KeyValueInfo> CHTLJSKeyValueManager::parseKeyValuePairs(const std::string& input) {
    std::vector<KeyValueInfo> pairs;
    
    // 简化的键值对解析（需要更复杂的解析器）
    size_t pos = 0;
    while (pos < input.length()) {
        // 跳过空白和逗号
        while (pos < input.length() && (std::isspace(input[pos]) || input[pos] == ',')) {
            pos++;
        }
        
        if (pos >= input.length()) break;
        
        // 查找键值对
        size_t colon_pos = input.find(':', pos);
        if (colon_pos == std::string::npos) break;
        
        // 提取键
        std::string key = input.substr(pos, colon_pos - pos);
        // 去除空白
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        
        // 查找值的结束位置
        pos = colon_pos + 1;
        while (pos < input.length() && std::isspace(input[pos])) pos++;
        
        size_t value_start = pos;
        int brace_count = 0;
        bool in_string = false;
        char string_char = '\0';
        
        while (pos < input.length()) {
            char c = input[pos];
            
            if (!in_string && (c == '"' || c == '\'')) {
                in_string = true;
                string_char = c;
            } else if (in_string && c == string_char) {
                in_string = false;
            } else if (!in_string) {
                if (c == '{') brace_count++;
                else if (c == '}') brace_count--;
                else if (c == ',' && brace_count == 0) break;
            }
            pos++;
        }
        
        std::string value = input.substr(value_start, pos - value_start);
        // 去除空白
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        // 创建键值对信息
        KeyValueInfo kv;
        kv.key = key;
        kv.value = value;
        kv.is_optional = true; // 默认所有键都是可选的
        kv.is_function = (value.find("=>") != std::string::npos || 
                         value.find("function") != std::string::npos ||
                         value.find("()") != std::string::npos);
        kv.value_type = inferValueType(value);
        
        pairs.push_back(kv);
    }
    
    return pairs;
}

bool CHTLJSKeyValueManager::validateKeyValuePair(const KeyValueInfo& kv, const std::string& function_name) {
    // 检查键是否有效
    if (!isValidKey(kv.key, function_name)) {
        return false;
    }
    
    // 对于delegate函数，selector键是特殊的，不是事件
    if (function_name == "delegate" && kv.key == "selector") {
        return true; // selector键总是有效的
    }
    
    // 对于事件键，检查是否为有效事件
    if (function_name == "listen" || function_name == "delegate") {
        return event_registry_->isValidEvent(kv.key);
    }
    
    return true;
}

void CHTLJSKeyValueManager::defineFunctionKeys(const std::string& function_name, 
                                              const std::vector<std::string>& required_keys,
                                              const std::vector<std::string>& optional_keys) {
    function_required_keys_[function_name] = required_keys;
    function_optional_keys_[function_name] = optional_keys;
}

std::vector<std::string> CHTLJSKeyValueManager::getRequiredKeys(const std::string& function_name) const {
    auto it = function_required_keys_.find(function_name);
    return it != function_required_keys_.end() ? it->second : std::vector<std::string>();
}

std::vector<std::string> CHTLJSKeyValueManager::getOptionalKeys(const std::string& function_name) const {
    auto it = function_optional_keys_.find(function_name);
    return it != function_optional_keys_.end() ? it->second : std::vector<std::string>();
}

std::vector<std::string> CHTLJSKeyValueManager::getAllKeys(const std::string& function_name) const {
    auto required = getRequiredKeys(function_name);
    auto optional = getOptionalKeys(function_name);
    
    required.insert(required.end(), optional.begin(), optional.end());
    return required;
}

std::string CHTLJSKeyValueManager::generateKeyValueCode(const std::vector<KeyValueInfo>& pairs, const std::string& function_name) {
    std::ostringstream code;
    
    code << "{\n";
    
    for (size_t i = 0; i < pairs.size(); ++i) {
        const auto& kv = pairs[i];
        
        code << "    " << kv.key << ": ";
        
        if (kv.is_function) {
            code << formatFunctionValue(kv.value);
        } else {
            code << kv.value;
        }
        
        if (i < pairs.size() - 1) {
            code << ",";
        }
        code << "\n";
    }
    
    code << "}";
    
    return code.str();
}

std::string CHTLJSKeyValueManager::generateEventListenerCode(const KeyValueInfo& kv) {
    std::ostringstream code;
    
    code << "addEventListener('" << kv.key << "', " << formatFunctionValue(kv.value) << ")";
    
    return code.str();
}

bool CHTLJSKeyValueManager::isValidKey(const std::string& key, const std::string& function_name) {
    auto all_keys = getAllKeys(function_name);
    return std::find(all_keys.begin(), all_keys.end(), key) != all_keys.end();
}

std::string CHTLJSKeyValueManager::inferValueType(const std::string& value) {
    if (value.find("=>") != std::string::npos || 
        value.find("function") != std::string::npos) {
        return "function";
    }
    
    if (value.find('"') != std::string::npos || value.find('\'') != std::string::npos) {
        return "string";
    }
    
    if (std::isdigit(value[0]) || value == "true" || value == "false") {
        return std::isdigit(value[0]) ? "number" : "boolean";
    }
    
    return "identifier";
}

std::string CHTLJSKeyValueManager::formatFunctionValue(const std::string& value) {
    // 如果已经是函数格式，直接返回
    if (value.find("=>") != std::string::npos || value.find("function") != std::string::npos) {
        return value;
    }
    
    // 如果是标识符，假设是函数引用
    return value;
}

std::string CHTLJSKeyValueManager::escapeJavaScript(const std::string& input) {
    std::string result = input;
    
    // 转义特殊字符
    size_t pos = 0;
    while ((pos = result.find('\\', pos)) != std::string::npos) {
        result.replace(pos, 1, "\\\\");
        pos += 2;
    }
    
    pos = 0;
    while ((pos = result.find('"', pos)) != std::string::npos) {
        result.replace(pos, 1, "\\\"");
        pos += 2;
    }
    
    return result;
}

} // namespace chtljs
} // namespace chtl