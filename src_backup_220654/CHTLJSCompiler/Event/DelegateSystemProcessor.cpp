#include "DelegateSystemProcessor.h"
#include <iostream>
#include <regex>
#include <algorithm>
#include <sstream>

namespace chtl {
namespace chtljs {

DelegateSystemProcessor::DelegateSystemProcessor() {
    selectorProcessor_ = std::make_shared<EnhancedSelectorProcessor>();
    initializeSupportedEvents();
}

DelegateSystemProcessor::~DelegateSystemProcessor() = default;

// === 按语法文档：delegate功能处理 ===

std::string DelegateSystemProcessor::processDelegateStatement(const std::string& delegateCode) {
    // 按语法文档：{{父元素选择器}}->delegate({ target: {{选择器}}, click: 函数 })
    
    if (delegateCode.empty()) {
        addError("空的delegate语句");
        return "";
    }
    
    // 解析delegate语句结构
    DelegateStatement statement = parseDelegateStatement(delegateCode);
    
    if (statement.parentSelector.empty()) {
        addError("delegate语句缺少父元素选择器");
        return "";
    }
    
    if (statement.target.selector.empty() && statement.target.targetSelectors.empty()) {
        addError("delegate语句缺少目标选择器");
        return "";
    }
    
    if (statement.events.empty()) {
        addError("delegate语句缺少事件绑定");
        return "";
    }
    
    // 添加到全局注册表
    addToGlobalRegistry(statement);
    
    // 生成JavaScript代码
    return generateDelegateCode(statement);
}

DelegateSystemProcessor::DelegateStatement DelegateSystemProcessor::parseDelegateStatement(const std::string& delegateCode) {
    DelegateStatement statement;
    
    // 提取父元素选择器
    statement.parentSelector = extractParentSelector(delegateCode);
    
    // 提取委托对象
    std::string delegateObject = extractDelegateObject(delegateCode);
    
    // 解析委托对象内容
    auto keyValuePairs = parseEventKeyValuePairs(delegateObject);
    
    for (const auto& [key, value] : keyValuePairs) {
        if (key == "target") {
            // 解析目标选择器
            statement.target = parseDelegateTarget(value);
        } else {
            // 解析事件绑定
            DelegateBinding binding;
            binding.eventType = key;
            binding.handlerCode = value;
            
            // 判断是否为外部函数
            if (value.find("(") == std::string::npos && value.find("=>") == std::string::npos) {
                binding.functionName = value;
            }
            
            statement.events.push_back(binding);
        }
    }
    
    // 生成委托ID
    for (const auto& binding : statement.events) {
        statement.delegateId = generateDelegateId(statement.parentSelector, binding.eventType);
        break; // 使用第一个事件类型生成ID
    }
    
    return statement;
}

std::string DelegateSystemProcessor::generateDelegateCode(const DelegateStatement& statement) {
    // 按语法文档：生成基于事件冒泡的委托实现
    
    std::string code = "// 按CHTL语法文档：事件委托实现\n";
    code += "(function() {\n";
    code += "    // 获取父元素\n";
    
    std::string parentJs = selectorProcessor_->processEnhancedSelector(statement.parentSelector);
    code += "    let parentElement = " + parentJs + ";\n";
    code += "    \n";
    code += "    if (!parentElement) {\n";
    code += "        console.warn('CHTL delegate: 父元素不存在 " + statement.parentSelector + "');\n";
    code += "        return;\n";
    code += "    }\n";
    code += "    \n";
    
    // 为每个事件类型生成委托处理器
    for (const auto& binding : statement.events) {
        code += "    // 事件委托：" + binding.eventType + "\n";
        code += "    parentElement.addEventListener('" + binding.eventType + "', function(event) {\n";
        code += "        // 按CHTL语法文档：检查事件目标是否匹配\n";
        
        // 生成目标匹配逻辑
        std::string matchingLogic = generateEventMatchingLogic(statement.target);
        code += matchingLogic;
        
        code += "        \n";
        code += "        // 执行事件处理器\n";
        code += "        if (targetMatched) {\n";
        code += "            try {\n";
        
        if (!binding.functionName.empty()) {
            code += "                " + binding.functionName + ".call(event.target, event);\n";
        } else {
            code += "                (" + binding.handlerCode + ").call(event.target, event);\n";
        }
        
        code += "            } catch (error) {\n";
        code += "                console.error('CHTL delegate处理器错误:', error);\n";
        code += "            }\n";
        code += "        }\n";
        code += "    }, true); // 使用捕获阶段以确保委托正常工作\n";
        code += "    \n";
    }
    
    code += "})();\n";
    
    return code;
}

// === 全局注册表管理 ===

void DelegateSystemProcessor::addToGlobalRegistry(const DelegateStatement& statement) {
    // 按语法文档：添加到全局委托注册表
    
    std::string parentKey = statement.parentSelector;
    
    if (globalDelegateRegistry_.find(parentKey) == globalDelegateRegistry_.end()) {
        // 创建新的注册表项
        GlobalDelegateEntry entry;
        entry.parentSelector = statement.parentSelector;
        globalDelegateRegistry_[parentKey] = entry;
    }
    
    // 合并委托绑定
    mergeDelegateBindings(parentKey, statement);
}

void DelegateSystemProcessor::mergeDelegateBindings(const std::string& parentSelector, const DelegateStatement& statement) {
    // 按语法文档：重复绑定父元素的子元素会作为分支合并在同一个事件委托之中
    
    auto& entry = globalDelegateRegistry_[parentSelector];
    
    // 添加目标选择器
    if (statement.target.isArray) {
        for (const auto& selector : statement.target.targetSelectors) {
            entry.childSelectors.insert(selector);
        }
    } else {
        entry.childSelectors.insert(statement.target.selector);
    }
    
    // 合并事件绑定
    for (const auto& binding : statement.events) {
        entry.eventMap[binding.eventType].push_back(binding);
    }
}

std::string DelegateSystemProcessor::generateGlobalDelegateRegistry() {
    // 按语法文档：生成全局委托注册代码
    
    std::string code = "// 按CHTL语法文档：全局事件委托注册表\n";
    code += "(function() {\n";
    code += "    'use strict';\n";
    code += "    \n";
    code += "    // CHTL事件委托全局注册表\n";
    code += "    if (!window.CHTL_DELEGATE_REGISTRY) {\n";
    code += "        window.CHTL_DELEGATE_REGISTRY = new Map();\n";
    code += "    }\n";
    code += "    \n";
    
    for (const auto& [parentSelector, entry] : globalDelegateRegistry_) {
        code += "    // 父元素: " + parentSelector + "\n";
        code += "    (function() {\n";
        
        std::string parentJs = selectorProcessor_->processEnhancedSelector(parentSelector);
        code += "        let parent = " + parentJs + ";\n";
        code += "        if (!parent) return;\n";
        code += "        \n";
        
        // 为每种事件类型生成委托
        for (const auto& [eventType, bindings] : entry.eventMap) {
            code += "        // 事件委托: " + eventType + "\n";
            code += "        parent.addEventListener('" + eventType + "', function(event) {\n";
            code += "            let target = event.target;\n";
            code += "            \n";
            
            // 生成目标匹配逻辑
            code += "            // 检查目标匹配\n";
            for (const auto& selector : entry.childSelectors) {
                std::string cleanSelector = selectorProcessor_->cleanSelector(selector);
                code += "            if (target.matches && target.matches('" + cleanSelector + "')) {\n";
                
                // 为匹配的目标执行所有相关处理器
                for (const auto& binding : bindings) {
                    code += "                try {\n";
                    if (!binding.functionName.empty()) {
                        code += "                    " + binding.functionName + ".call(target, event);\n";
                    } else {
                        code += "                    (" + binding.handlerCode + ").call(target, event);\n";
                    }
                    code += "                } catch (error) {\n";
                    code += "                    console.error('CHTL delegate处理器错误:', error);\n";
                    code += "                }\n";
                }
                
                code += "                event.stopPropagation(); // 防止重复处理\n";
                code += "                return;\n";
                code += "            }\n";
            }
            
            code += "        }, true);\n";
            code += "        \n";
        }
        
        code += "    })();\n";
        code += "    \n";
    }
    
    code += "})();\n";
    
    return code;
}

// === 委托目标处理 ===

DelegateSystemProcessor::DelegateTarget DelegateSystemProcessor::parseDelegateTarget(const std::string& targetDefinition) {
    DelegateTarget target;
    
    std::string cleaned = targetDefinition;
    cleaned.erase(0, cleaned.find_first_not_of(" \t\n"));
    cleaned.erase(cleaned.find_last_not_of(" \t\n") + 1);
    
    if (cleaned.front() == '[' && cleaned.back() == ']') {
        // 数组格式：[{{选择器1}}, {{选择器2}}]
        target = parseArrayTarget(cleaned);
    } else {
        // 单个目标：{{选择器}}
        target = parseSingleTarget(cleaned);
    }
    
    return target;
}

DelegateSystemProcessor::DelegateTarget DelegateSystemProcessor::parseSingleTarget(const std::string& targetSelector) {
    DelegateTarget target;
    target.selector = targetSelector;
    target.isArray = false;
    
    return target;
}

DelegateSystemProcessor::DelegateTarget DelegateSystemProcessor::parseArrayTarget(const std::string& targetArray) {
    DelegateTarget target;
    target.isArray = true;
    
    // 移除外层方括号
    std::string content = targetArray.substr(1, targetArray.length() - 2);
    
    // 按逗号分割选择器
    std::stringstream ss(content);
    std::string selector;
    
    while (std::getline(ss, selector, ',')) {
        selector.erase(0, selector.find_first_not_of(" \t\n"));
        selector.erase(selector.find_last_not_of(" \t\n") + 1);
        
        if (!selector.empty()) {
            target.targetSelectors.push_back(selector);
        }
    }
    
    return target;
}

// === JavaScript代码生成 ===

std::string DelegateSystemProcessor::generateEventMatchingLogic(const DelegateTarget& target) {
    std::string logic = "        let targetMatched = false;\n";
    
    if (target.isArray) {
        // 多目标匹配
        logic += "        // 多目标匹配检查\n";
        for (const auto& selector : target.targetSelectors) {
            std::string cleanSelector = selectorProcessor_->cleanSelector(selector);
            logic += "        if (event.target.matches && event.target.matches('" + cleanSelector + "')) {\n";
            logic += "            targetMatched = true;\n";
            logic += "        }\n";
        }
    } else {
        // 单目标匹配
        std::string cleanSelector = selectorProcessor_->cleanSelector(target.selector);
        logic += "        // 单目标匹配检查\n";
        logic += "        if (event.target.matches && event.target.matches('" + cleanSelector + "')) {\n";
        logic += "            targetMatched = true;\n";
        logic += "        }\n";
    }
    
    return logic;
}

// === 辅助方法 ===

std::string DelegateSystemProcessor::extractParentSelector(const std::string& delegateCode) {
    // 从 {{父元素}}->delegate(...) 中提取 {{父元素}}
    std::regex parentRegex(R"((\{\{[^}]+\}\})->delegate)");
    std::smatch match;
    
    if (std::regex_search(delegateCode, match, parentRegex)) {
        return match[1].str();
    }
    
    return "";
}

std::string DelegateSystemProcessor::extractDelegateObject(const std::string& delegateCode) {
    // 从 delegate({ ... }) 中提取 { ... }
    
    size_t delegatePos = delegateCode.find("delegate(");
    if (delegatePos == std::string::npos) {
        return "";
    }
    
    size_t startPos = delegateCode.find('{', delegatePos);
    if (startPos == std::string::npos) {
        return "";
    }
    
    // 找到匹配的右大括号
    int braceCount = 1;
    size_t endPos = startPos + 1;
    
    while (endPos < delegateCode.length() && braceCount > 0) {
        if (delegateCode[endPos] == '{') {
            braceCount++;
        } else if (delegateCode[endPos] == '}') {
            braceCount--;
        }
        endPos++;
    }
    
    if (braceCount == 0) {
        return delegateCode.substr(startPos, endPos - startPos);
    }
    
    return "";
}

std::vector<std::pair<std::string, std::string>> DelegateSystemProcessor::parseEventKeyValuePairs(const std::string& delegateObject) {
    std::vector<std::pair<std::string, std::string>> pairs;
    
    // 移除外层大括号
    std::string content = delegateObject;
    if (content.front() == '{' && content.back() == '}') {
        content = content.substr(1, content.length() - 2);
    }
    
    // 智能分割（考虑嵌套结构）
    std::vector<std::string> entries = splitDelegateEntries(content);
    
    for (const auto& entry : entries) {
        size_t colonPos = entry.find(':');
        if (colonPos != std::string::npos) {
            std::string key = entry.substr(0, colonPos);
            std::string value = entry.substr(colonPos + 1);
            
            // 清理空白
            key.erase(0, key.find_first_not_of(" \t\n"));
            key.erase(key.find_last_not_of(" \t\n") + 1);
            value.erase(0, value.find_first_not_of(" \t\n"));
            value.erase(value.find_last_not_of(" \t\n") + 1);
            
            pairs.emplace_back(key, value);
        }
    }
    
    return pairs;
}

std::vector<std::string> DelegateSystemProcessor::splitDelegateEntries(const std::string& content) {
    std::vector<std::string> entries;
    std::string currentEntry;
    int braceLevel = 0;
    int parenLevel = 0;
    int bracketLevel = 0;
    
    for (char c : content) {
        if (c == '{') {
            braceLevel++;
        } else if (c == '}') {
            braceLevel--;
        } else if (c == '(') {
            parenLevel++;
        } else if (c == ')') {
            parenLevel--;
        } else if (c == '[') {
            bracketLevel++;
        } else if (c == ']') {
            bracketLevel--;
        } else if (c == ',' && braceLevel == 0 && parenLevel == 0 && bracketLevel == 0) {
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

std::string DelegateSystemProcessor::generateDelegateId(const std::string& parentSelector, const std::string& eventType) {
    // 生成唯一的委托ID
    static int counter = 0;
    std::string cleanParent = selectorProcessor_->cleanSelector(parentSelector);
    return "chtl_delegate_" + cleanParent + "_" + eventType + "_" + std::to_string(++counter);
}

bool DelegateSystemProcessor::hasDelegateForParent(const std::string& parentSelector, const std::string& eventType) {
    auto it = globalDelegateRegistry_.find(parentSelector);
    if (it != globalDelegateRegistry_.end()) {
        return it->second.eventMap.find(eventType) != it->second.eventMap.end();
    }
    return false;
}

void DelegateSystemProcessor::initializeSupportedEvents() {
    // 按语法文档：初始化支持的事件类型（与ListenSystemProcessor相同）
    supportedEvents_ = {
        "click", "dblclick", "mousedown", "mouseup", "mousemove", 
        "mouseenter", "mouseleave", "mouseover", "mouseout",
        "contextmenu", "wheel",
        "keydown", "keyup", "keypress",
        "submit", "reset", "input", "change", "focus", "blur",
        "focusin", "focusout", "select",
        "load", "unload", "beforeunload", "resize", "scroll",
        "drag", "dragstart", "dragend", "dragenter", "dragover", 
        "dragleave", "drop",
        "touchstart", "touchend", "touchmove", "touchcancel"
    };
}

// === 错误处理 ===

void DelegateSystemProcessor::addError(const std::string& message) {
    errors_.push_back("DelegateSystem: " + message);
}

void DelegateSystemProcessor::clearErrors() {
    errors_.clear();
}

bool DelegateSystemProcessor::hasErrors() const {
    return !errors_.empty();
}

std::vector<std::string> DelegateSystemProcessor::getErrors() const {
    return errors_;
}

} // namespace chtljs
} // namespace chtl