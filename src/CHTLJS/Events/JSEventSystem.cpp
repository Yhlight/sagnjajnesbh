#include "CHTLJS/Events/JSEventSystem.h"
#include "Utils/StringUtils.h"
#include "Utils/ErrorHandler.h"
#include <regex>
#include <algorithm>

namespace CHTLJS {
namespace Events {

// JSEventManager实现
JSEventManager::JSEventManager() : eventMapInitialized_(false) {
    InitializeEventMap();
}

void JSEventManager::InitializeEventMap() {
    if (eventMapInitialized_) return;
    
    eventMap_ = {
        // 鼠标事件
        {"click", JSEventType::CLICK},
        {"dblclick", JSEventType::DBLCLICK},
        {"mousedown", JSEventType::MOUSEDOWN},
        {"mouseup", JSEventType::MOUSEUP},
        {"mouseover", JSEventType::MOUSEOVER},
        {"mouseout", JSEventType::MOUSEOUT},
        {"mousemove", JSEventType::MOUSEMOVE},
        {"mouseenter", JSEventType::MOUSEENTER},
        {"mouseleave", JSEventType::MOUSELEAVE},
        {"contextmenu", JSEventType::CONTEXTMENU},
        {"wheel", JSEventType::WHEEL},
        
        // 键盘事件
        {"keydown", JSEventType::KEYDOWN},
        {"keyup", JSEventType::KEYUP},
        {"keypress", JSEventType::KEYPRESS},
        
        // 表单事件
        {"submit", JSEventType::SUBMIT},
        {"reset", JSEventType::RESET},
        {"focus", JSEventType::FOCUS},
        {"blur", JSEventType::BLUR},
        {"change", JSEventType::CHANGE},
        {"input", JSEventType::INPUT},
        {"select", JSEventType::SELECT},
        
        // 窗口事件
        {"load", JSEventType::LOAD},
        {"unload", JSEventType::UNLOAD},
        {"beforeunload", JSEventType::BEFOREUNLOAD},
        {"resize", JSEventType::RESIZE},
        {"scroll", JSEventType::SCROLL},
        
        // 触摸事件
        {"touchstart", JSEventType::TOUCHSTART},
        {"touchend", JSEventType::TOUCHEND},
        {"touchmove", JSEventType::TOUCHMOVE},
        {"touchcancel", JSEventType::TOUCHCANCEL},
        
        // 拖拽事件
        {"drag", JSEventType::DRAG},
        {"dragstart", JSEventType::DRAGSTART},
        {"dragend", JSEventType::DRAGEND},
        {"dragover", JSEventType::DRAGOVER},
        {"dragenter", JSEventType::DRAGENTER},
        {"dragleave", JSEventType::DRAGLEAVE},
        {"drop", JSEventType::DROP},
        
        // 媒体事件
        {"play", JSEventType::PLAY},
        {"pause", JSEventType::PAUSE},
        {"ended", JSEventType::ENDED},
        {"timeupdate", JSEventType::TIMEUPDATE},
        {"volumechange", JSEventType::VOLUMECHANGE},
        {"loadstart", JSEventType::LOADSTART},
        {"loadeddata", JSEventType::LOADEDDATA},
        {"loadedmetadata", JSEventType::LOADEDMETADATA},
        {"canplay", JSEventType::CANPLAY},
        {"canplaythrough", JSEventType::CANPLAYTHROUGH},
        
        // 网络事件
        {"online", JSEventType::ONLINE},
        {"offline", JSEventType::OFFLINE},
        
        // 其他事件
        {"error", JSEventType::ERROR},
        {"abort", JSEventType::ABORT},
        {"toggle", JSEventType::TOGGLE},
        {"show", JSEventType::SHOW},
        {"hide", JSEventType::HIDE},
        {"open", JSEventType::OPEN},
        {"close", JSEventType::CLOSE}
    };
    
    eventMapInitialized_ = true;
}

bool JSEventManager::IsValidJSEvent(const std::string& eventName) const {
    // 检查标准事件
    if (eventMap_.find(eventName) != eventMap_.end()) {
        return true;
    }
    
    // 检查自定义事件
    if (customEvents_.find(eventName) != customEvents_.end()) {
        return true;
    }
    
    return false;
}

JSEventType JSEventManager::GetEventType(const std::string& eventName) const {
    auto it = eventMap_.find(eventName);
    if (it != eventMap_.end()) {
        return it->second;
    }
    
    // 自定义事件返回CUSTOM类型
    if (customEvents_.find(eventName) != customEvents_.end()) {
        return JSEventType::CUSTOM;
    }
    
    return JSEventType::CUSTOM; // 默认为自定义事件
}

std::vector<std::string> JSEventManager::GetAllEventNames() const {
    std::vector<std::string> eventNames;
    
    // 添加标准事件
    for (const auto& pair : eventMap_) {
        eventNames.push_back(pair.first);
    }
    
    // 添加自定义事件
    for (const auto& customEvent : customEvents_) {
        eventNames.push_back(customEvent);
    }
    
    // 排序
    std::sort(eventNames.begin(), eventNames.end());
    
    return eventNames;
}

std::string JSEventManager::GetEventCategory(JSEventType eventType) const {
    switch (eventType) {
        case JSEventType::CLICK:
        case JSEventType::DBLCLICK:
        case JSEventType::MOUSEDOWN:
        case JSEventType::MOUSEUP:
        case JSEventType::MOUSEOVER:
        case JSEventType::MOUSEOUT:
        case JSEventType::MOUSEMOVE:
        case JSEventType::MOUSEENTER:
        case JSEventType::MOUSELEAVE:
        case JSEventType::CONTEXTMENU:
        case JSEventType::WHEEL:
            return "鼠标事件";
            
        case JSEventType::KEYDOWN:
        case JSEventType::KEYUP:
        case JSEventType::KEYPRESS:
            return "键盘事件";
            
        case JSEventType::SUBMIT:
        case JSEventType::RESET:
        case JSEventType::FOCUS:
        case JSEventType::BLUR:
        case JSEventType::CHANGE:
        case JSEventType::INPUT:
        case JSEventType::SELECT:
            return "表单事件";
            
        case JSEventType::LOAD:
        case JSEventType::UNLOAD:
        case JSEventType::BEFOREUNLOAD:
        case JSEventType::RESIZE:
        case JSEventType::SCROLL:
            return "窗口事件";
            
        case JSEventType::TOUCHSTART:
        case JSEventType::TOUCHEND:
        case JSEventType::TOUCHMOVE:
        case JSEventType::TOUCHCANCEL:
            return "触摸事件";
            
        case JSEventType::DRAG:
        case JSEventType::DRAGSTART:
        case JSEventType::DRAGEND:
        case JSEventType::DRAGOVER:
        case JSEventType::DRAGENTER:
        case JSEventType::DRAGLEAVE:
        case JSEventType::DROP:
            return "拖拽事件";
            
        case JSEventType::PLAY:
        case JSEventType::PAUSE:
        case JSEventType::ENDED:
        case JSEventType::TIMEUPDATE:
        case JSEventType::VOLUMECHANGE:
        case JSEventType::LOADSTART:
        case JSEventType::LOADEDDATA:
        case JSEventType::LOADEDMETADATA:
        case JSEventType::CANPLAY:
        case JSEventType::CANPLAYTHROUGH:
            return "媒体事件";
            
        case JSEventType::ONLINE:
        case JSEventType::OFFLINE:
            return "网络事件";
            
        case JSEventType::CUSTOM:
            return "自定义事件";
            
        default:
            return "其他事件";
    }
}

void JSEventManager::AddCustomEvent(const std::string& eventName) {
    if (!eventName.empty() && eventMap_.find(eventName) == eventMap_.end()) {
        customEvents_.insert(eventName);
        
        Utils::ErrorHandler::GetInstance().LogInfo(
            "添加自定义JS事件: " + eventName
        );
    }
}

bool JSEventManager::IsCustomEvent(const std::string& eventName) const {
    return customEvents_.find(eventName) != customEvents_.end();
}

// KeyValueFlexibilityManager实现
KeyValueFlexibilityManager::KeyValueFlexibilityManager() {
    // 设置常用的默认值
    defaultValues_ = {
        {"duration", "300"},
        {"easing", "ease"},
        {"delay", "0"},
        {"loop", "false"},
        {"direction", "normal"},
        {"opacity", "1"},
        {"scale", "1"},
        {"rotate", "0"},
        {"x", "0"},
        {"y", "0"},
        {"width", "auto"},
        {"height", "auto"}
    };
}

std::unordered_map<std::string, std::string> KeyValueFlexibilityManager::ParseKeyValuePairs(
    const std::string& keyValueString,
    const std::vector<std::string>& requiredKeys,
    const std::vector<std::string>& optionalKeys) {
    
    std::unordered_map<std::string, std::string> result;
    
    if (keyValueString.empty()) {
        return result;
    }
    
    // 解析键值对：支持无序和可选
    // 格式：{ key1: value1, key2: value2, ... }
    
    std::string cleanString = keyValueString;
    cleanString = Utils::StringUtils::Trim(cleanString);
    
    // 移除外层大括号
    if (cleanString.front() == '{') cleanString = cleanString.substr(1);
    if (cleanString.back() == '}') cleanString = cleanString.substr(0, cleanString.length() - 1);
    
    cleanString = Utils::StringUtils::Trim(cleanString);
    
    // 解析键值对
    std::regex keyValueRegex(R"((\w+)\s*:\s*([^,}]+))");
    std::sregex_iterator iter(cleanString.begin(), cleanString.end(), keyValueRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        const std::smatch& match = *iter;
        std::string key = Utils::StringUtils::Trim(match[1].str());
        std::string value = Utils::StringUtils::Trim(match[2].str());
        
        // 移除值的引号
        if ((value.front() == '"' && value.back() == '"') ||
            (value.front() == '\'' && value.back() == '\'')) {
            value = value.substr(1, value.length() - 2);
        }
        
        result[key] = value;
    }
    
    // 添加缺失的可选键的默认值
    for (const auto& optionalKey : optionalKeys) {
        if (result.find(optionalKey) == result.end()) {
            std::string defaultValue = GetDefaultValue(optionalKey);
            if (!defaultValue.empty()) {
                result[optionalKey] = defaultValue;
            }
        }
    }
    
    return result;
}

bool KeyValueFlexibilityManager::ValidateKeyValuePairs(
    const std::unordered_map<std::string, std::string>& keyValueMap,
    const std::vector<std::string>& requiredKeys) const {
    
    // 检查所有必需的键是否存在
    for (const auto& requiredKey : requiredKeys) {
        if (keyValueMap.find(requiredKey) == keyValueMap.end()) {
            Utils::ErrorHandler::GetInstance().LogError(
                "缺少必需的键: " + requiredKey
            );
            return false;
        }
    }
    
    return true;
}

std::string KeyValueFlexibilityManager::GetDefaultValue(const std::string& key) const {
    auto it = defaultValues_.find(key);
    return (it != defaultValues_.end()) ? it->second : "";
}

void KeyValueFlexibilityManager::SetDefaultValue(const std::string& key, const std::string& defaultValue) {
    defaultValues_[key] = defaultValue;
}

// UndecoratedLiteralManager实现
UndecoratedLiteralManager::UndecoratedLiteralManager() {}

bool UndecoratedLiteralManager::IsUndecoratedLiteral(const std::string& literal) const {
    if (literal.empty()) return false;
    
    // 无修饰字面量：没有引号、没有特殊前缀的字面量
    // 包括：数字、布尔值、标识符等
    
    // 排除已有引号的字符串
    if ((literal.front() == '"' && literal.back() == '"') ||
        (literal.front() == '\'' && literal.back() == '\'')) {
        return false;
    }
    
    // 排除特殊语法
    if (literal.find("{{") != std::string::npos || 
        literal.find("}}") != std::string::npos ||
        literal.find("->") != std::string::npos) {
        return false;
    }
    
    // 检查是否为有效的无修饰字面量
    return IsNumberLiteral(literal) || IsBooleanLiteral(literal) || IsIdentifier(literal);
}

std::string UndecoratedLiteralManager::ParseUndecoratedLiteral(const std::string& literal) const {
    if (!IsUndecoratedLiteral(literal)) {
        return literal; // 不是无修饰字面量，直接返回
    }
    
    // 根据类型解析
    if (IsNumberLiteral(literal)) {
        return literal; // 数字直接返回
    } else if (IsBooleanLiteral(literal)) {
        return literal; // 布尔值直接返回
    } else if (IsIdentifier(literal)) {
        return literal; // 标识符直接返回
    }
    
    return literal;
}

std::string UndecoratedLiteralManager::GetLiteralType(const std::string& literal) const {
    if (IsNumberLiteral(literal)) {
        return "number";
    } else if (IsBooleanLiteral(literal)) {
        return "boolean";
    } else if (IsIdentifier(literal)) {
        return "identifier";
    } else {
        return "unknown";
    }
}

bool UndecoratedLiteralManager::IsNumberLiteral(const std::string& literal) const {
    if (literal.empty()) return false;
    
    // 检查是否为数字（整数或浮点数）
    std::regex numberRegex(R"(^[+-]?(\d+\.?\d*|\.\d+)([eE][+-]?\d+)?$)");
    return std::regex_match(literal, numberRegex);
}

bool UndecoratedLiteralManager::IsBooleanLiteral(const std::string& literal) const {
    return literal == "true" || literal == "false";
}

bool UndecoratedLiteralManager::IsIdentifier(const std::string& literal) const {
    if (literal.empty()) return false;
    
    // 检查是否为有效的标识符
    std::regex identifierRegex(R"(^[a-zA-Z_$][a-zA-Z0-9_$]*$)");
    return std::regex_match(literal, identifierRegex);
}

} // namespace Events
} // namespace CHTLJS