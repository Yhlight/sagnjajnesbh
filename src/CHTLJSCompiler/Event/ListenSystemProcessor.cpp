#include "ListenSystemProcessor.h"
#include <iostream>
#include <regex>
#include <algorithm>
#include <sstream>

namespace chtl {
namespace chtljs {

ListenSystemProcessor::ListenSystemProcessor() {
    selectorProcessor_ = std::make_shared<EnhancedSelectorProcessor>();
    initializeSupportedEvents();
}

ListenSystemProcessor::~ListenSystemProcessor() = default;

// === 按语法文档：listen功能处理 ===

std::string ListenSystemProcessor::processListenStatement(const std::string& listenCode) {
    // 按语法文档：{{box}}->listen({ click: () => {}, mouseenter: func })
    
    if (listenCode.empty()) {
        addError("空的listen语句");
        return "";
    }
    
    // 解析listen语句结构
    ListenStatement statement = parseListenStatement(listenCode);
    
    if (statement.targetSelector.empty()) {
        addError("listen语句缺少目标选择器");
        return "";
    }
    
    if (statement.events.empty()) {
        addError("listen语句缺少事件绑定");
        return "";
    }
    
    // 生成JavaScript代码
    std::string jsCode = generateEventBindingCode(statement);
    
    return jsCode;
}

ListenSystemProcessor::ListenStatement ListenSystemProcessor::parseListenStatement(const std::string& listenCode) {
    ListenStatement statement;
    
    // 提取目标选择器：{{box}}->listen(...)
    statement.targetSelector = extractSelector(listenCode);
    
    // 提取事件对象：listen({ ... })
    std::string eventObject = extractEventObject(listenCode);
    
    // 解析事件绑定
    statement.events = parseEventBindings(eventObject);
    
    return statement;
}

std::string ListenSystemProcessor::generateEventBindingCode(const ListenStatement& statement) {
    // 按语法文档：生成事件绑定的JavaScript代码
    
    // 首先处理目标选择器
    std::string targetJs = selectorProcessor_->processEnhancedSelector(statement.targetSelector);
    
    if (targetJs.empty()) {
        addError("无法处理目标选择器: " + statement.targetSelector);
        return "";
    }
    
    // 生成事件绑定代码
    std::string bindingCode;
    
    if (statement.events.size() == 1) {
        // 单个事件绑定
        bindingCode = generateSingleEventBinding(targetJs, statement.events[0]);
    } else {
        // 多个事件绑定
        bindingCode = generateMultiEventBinding(targetJs, statement.events);
    }
    
    // 包装为安全的DOM操作
    return wrapWithSafetyCheck(targetJs, bindingCode);
}

// === 事件类型处理 ===

std::vector<ListenSystemProcessor::EventBinding> ListenSystemProcessor::parseEventBindings(const std::string& eventObject) {
    std::vector<EventBinding> bindings;
    
    // 解析事件对象中的键值对
    auto keyValuePairs = parseEventKeyValuePairs(eventObject);
    
    for (const auto& [eventType, handlerCode] : keyValuePairs) {
        if (!isValidEventType(eventType)) {
            addError("不支持的事件类型: " + eventType);
            continue;
        }
        
        EventBinding binding;
        binding.eventType = eventType;
        binding.handlerCode = handlerCode;
        
        // 判断是否为内联函数
        if (handlerCode.find("=>") != std::string::npos || 
            handlerCode.find("function") != std::string::npos) {
            binding.isInlineFunction = true;
            binding.handlerCode = processInlineFunction(handlerCode);
        } else {
            binding.isInlineFunction = false;
            binding.functionName = handlerCode;
            binding.handlerCode = processExternalFunction(handlerCode);
        }
        
        bindings.push_back(binding);
    }
    
    return bindings;
}

std::string ListenSystemProcessor::processInlineFunction(const std::string& functionCode) {
    // 按语法文档：处理内联函数 () => {} 或 function() {}
    
    std::string cleanedCode = cleanAndFormatCode(functionCode);
    
    // 确保函数格式正确
    if (cleanedCode.find("=>") != std::string::npos) {
        // 箭头函数格式
        return cleanedCode;
    } else if (cleanedCode.find("function") != std::string::npos) {
        // 传统函数格式
        return cleanedCode;
    } else {
        // 简化的函数体，需要包装
        return "function() { " + cleanedCode + " }";
    }
}

std::string ListenSystemProcessor::processExternalFunction(const std::string& functionName) {
    // 按语法文档：处理外部函数引用
    return functionName; // 直接返回函数名
}

bool ListenSystemProcessor::isValidEventType(const std::string& eventType) {
    // 按语法文档：验证事件类型
    return std::find(supportedEvents_.begin(), supportedEvents_.end(), eventType) != supportedEvents_.end();
}

// === JavaScript代码生成 ===

std::string ListenSystemProcessor::generateSingleEventBinding(const std::string& targetJs, const EventBinding& binding) {
    std::string code = "// 按CHTL语法文档：单个事件绑定\n";
    code += "let target = " + targetJs + ";\n";
    code += "if (target) {\n";
    code += "    if (target.length !== undefined) {\n";
    code += "        // 多个元素\n";
    code += "        target.forEach(element => {\n";
    code += "            element.addEventListener('" + binding.eventType + "', " + binding.handlerCode + ");\n";
    code += "        });\n";
    code += "    } else {\n";
    code += "        // 单个元素\n";
    code += "        target.addEventListener('" + binding.eventType + "', " + binding.handlerCode + ");\n";
    code += "    }\n";
    code += "}\n";
    
    return code;
}

std::string ListenSystemProcessor::generateMultiEventBinding(const std::string& targetJs, const std::vector<EventBinding>& bindings) {
    std::string code = "// 按CHTL语法文档：多个事件绑定\n";
    code += "let target = " + targetJs + ";\n";
    code += "if (target) {\n";
    code += "    if (target.length !== undefined) {\n";
    code += "        // 多个元素\n";
    code += "        target.forEach(element => {\n";
    
    for (const auto& binding : bindings) {
        code += "            element.addEventListener('" + binding.eventType + "', " + binding.handlerCode + ");\n";
    }
    
    code += "        });\n";
    code += "    } else {\n";
    code += "        // 单个元素\n";
    
    for (const auto& binding : bindings) {
        code += "        target.addEventListener('" + binding.eventType + "', " + binding.handlerCode + ");\n";
    }
    
    code += "    }\n";
    code += "}\n";
    
    return code;
}

std::string ListenSystemProcessor::wrapWithSafetyCheck(const std::string& targetJs, const std::string& bindingCode) {
    std::string wrappedCode = "(function() {\n";
    wrappedCode += "    // 按CHTL语法文档：安全的DOM操作包装\n";
    wrappedCode += "    try {\n";
    wrappedCode += "        " + bindingCode;
    wrappedCode += "    } catch (error) {\n";
    wrappedCode += "        console.warn('CHTL listen绑定失败:', error);\n";
    wrappedCode += "    }\n";
    wrappedCode += "})();\n";
    
    return wrappedCode;
}

// === 辅助方法 ===

std::string ListenSystemProcessor::extractSelector(const std::string& listenCode) {
    // 从 {{box}}->listen(...) 中提取 {{box}}
    std::regex selectorRegex(R"((\{\{[^}]+\}\})->listen)");
    std::smatch match;
    
    if (std::regex_search(listenCode, match, selectorRegex)) {
        return match[1].str();
    }
    
    return "";
}

std::string ListenSystemProcessor::extractEventObject(const std::string& listenCode) {
    // 从 listen({ ... }) 中提取 { ... }
    
    size_t listenPos = listenCode.find("listen(");
    if (listenPos == std::string::npos) {
        return "";
    }
    
    size_t startPos = listenCode.find('{', listenPos);
    if (startPos == std::string::npos) {
        return "";
    }
    
    // 找到匹配的右大括号
    int braceCount = 1;
    size_t endPos = startPos + 1;
    
    while (endPos < listenCode.length() && braceCount > 0) {
        if (listenCode[endPos] == '{') {
            braceCount++;
        } else if (listenCode[endPos] == '}') {
            braceCount--;
        }
        endPos++;
    }
    
    if (braceCount == 0) {
        return listenCode.substr(startPos, endPos - startPos);
    }
    
    return "";
}

std::vector<std::pair<std::string, std::string>> ListenSystemProcessor::parseEventKeyValuePairs(const std::string& eventObject) {
    std::vector<std::pair<std::string, std::string>> pairs;
    
    // 移除外层大括号
    std::string content = eventObject;
    if (content.front() == '{' && content.back() == '}') {
        content = content.substr(1, content.length() - 2);
    }
    
    // 按逗号分割（需要考虑嵌套的大括号）
    std::vector<std::string> eventEntries = splitEventEntries(content);
    
    for (const auto& entry : eventEntries) {
        size_t colonPos = entry.find(':');
        if (colonPos != std::string::npos) {
            std::string eventType = entry.substr(0, colonPos);
            std::string handlerCode = entry.substr(colonPos + 1);
            
            // 清理空白
            eventType.erase(0, eventType.find_first_not_of(" \t\n"));
            eventType.erase(eventType.find_last_not_of(" \t\n") + 1);
            handlerCode.erase(0, handlerCode.find_first_not_of(" \t\n"));
            handlerCode.erase(handlerCode.find_last_not_of(" \t\n") + 1);
            
            pairs.emplace_back(eventType, handlerCode);
        }
    }
    
    return pairs;
}

std::vector<std::string> ListenSystemProcessor::splitEventEntries(const std::string& content) {
    std::vector<std::string> entries;
    std::string currentEntry;
    int braceLevel = 0;
    int parenLevel = 0;
    
    for (char c : content) {
        if (c == '{') {
            braceLevel++;
        } else if (c == '}') {
            braceLevel--;
        } else if (c == '(') {
            parenLevel++;
        } else if (c == ')') {
            parenLevel--;
        } else if (c == ',' && braceLevel == 0 && parenLevel == 0) {
            // 顶层逗号，分割条目
            if (!currentEntry.empty()) {
                entries.push_back(currentEntry);
                currentEntry.clear();
            }
            continue;
        }
        
        currentEntry += c;
    }
    
    if (!currentEntry.empty()) {
        entries.push_back(currentEntry);
    }
    
    return entries;
}

std::string ListenSystemProcessor::cleanAndFormatCode(const std::string& code) {
    std::string cleaned = code;
    
    // 移除多余的空白
    cleaned.erase(0, cleaned.find_first_not_of(" \t\n"));
    cleaned.erase(cleaned.find_last_not_of(" \t\n") + 1);
    
    return cleaned;
}

void ListenSystemProcessor::initializeSupportedEvents() {
    // 按语法文档：初始化支持的DOM事件类型
    supportedEvents_ = {
        // 鼠标事件
        "click", "dblclick", "mousedown", "mouseup", "mousemove", 
        "mouseenter", "mouseleave", "mouseover", "mouseout",
        "contextmenu", "wheel",
        
        // 键盘事件
        "keydown", "keyup", "keypress",
        
        // 表单事件
        "submit", "reset", "input", "change", "focus", "blur",
        "focusin", "focusout", "select",
        
        // 窗口事件
        "load", "unload", "beforeunload", "resize", "scroll",
        
        // 拖拽事件
        "drag", "dragstart", "dragend", "dragenter", "dragover", 
        "dragleave", "drop",
        
        // 触摸事件
        "touchstart", "touchend", "touchmove", "touchcancel",
        
        // 其他常用事件
        "error", "abort", "canplay", "canplaythrough", "durationchange",
        "emptied", "ended", "loadeddata", "loadedmetadata", "loadstart",
        "pause", "play", "playing", "progress", "ratechange", "seeked",
        "seeking", "stalled", "suspend", "timeupdate", "volumechange",
        "waiting"
    };
}

// === 错误处理 ===

void ListenSystemProcessor::addError(const std::string& message) {
    errors_.push_back("ListenSystem: " + message);
}

void ListenSystemProcessor::clearErrors() {
    errors_.clear();
}

bool ListenSystemProcessor::hasErrors() const {
    return !errors_.empty();
}

std::vector<std::string> ListenSystemProcessor::getErrors() const {
    return errors_;
}

} // namespace chtljs
} // namespace chtl