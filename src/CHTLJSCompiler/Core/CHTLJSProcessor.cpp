// ========================================
// CHTL JS处理器实现 - 严格按照CHTL语法文档
// ========================================

#include "CHTLJSProcessor.h"
#include <regex>
#include <algorithm>
#include <iostream>

namespace chtl {
namespace chtljs {

// 静态成员初始化
bool CHTLJSProcessor::initialized = false;
std::vector<std::function<std::string(const std::string&)>> CHTLJSProcessor::processors;
std::vector<std::pair<std::string, int>> LocalScriptProcessor::localScripts;
std::unordered_set<std::string> DelegateProcessor::registeredDelegates;
std::unordered_map<std::string, std::unordered_map<std::string, std::string>> VirProcessor::virObjects;

/**
 * 增强选择器处理器实现
 */
std::string EnhancedSelectorProcessor::processEnhancedSelectors(const std::string& code) {
    std::string result = code;
    
    // 匹配 {{selector}} 模式
    std::regex selectorRegex(R"(\{\{([^}]+)\}\})");
    std::smatch match;
    
    std::string::const_iterator start = result.cbegin();
    std::stringstream processed;
    
    while (std::regex_search(start, result.cend(), match, selectorRegex)) {
        // 添加匹配前的内容
        processed << std::string(start, match[0].first);
        
        std::string selector = match[1].str();
        
        // 处理选择器
        if (isValidSelector(selector)) {
            processed << generateSelectorCode(selector);
        } else {
            // 无效选择器，保持原样
            processed << match[0].str();
        }
        
        start = match[0].second;
    }
    
    // 添加剩余内容
    processed << std::string(start, result.cend());
    
    return processed.str();
}

std::string EnhancedSelectorProcessor::processSingleSelector(const std::string& selector) {
    std::string trimmed = selector;
    // 移除首尾空格
    trimmed.erase(0, trimmed.find_first_not_of(" \t"));
    trimmed.erase(trimmed.find_last_not_of(" \t") + 1);
    
    return generateSelectorCode(trimmed);
}

bool EnhancedSelectorProcessor::isValidSelector(const std::string& selector) {
    if (selector.empty()) return false;
    
    // 支持的选择器类型：
    // 1. .class
    // 2. #id  
    // 3. tag
    // 4. tag.class
    // 5. tag#id
    // 6. .class tag
    // 7. #id tag
    // 8. selector[index]
    
    // 简单验证 - 检查是否包含有效字符
    std::regex validPattern(R"(^[a-zA-Z0-9\.\#\[\]\s\-_]+$)");
    return std::regex_match(selector, validPattern);
}

std::string EnhancedSelectorProcessor::generateSelectorCode(const std::string& selector) {
    std::stringstream js;
    
    // 检查是否有索引访问 [index]
    std::regex indexRegex(R"(^(.+)\[(\d+)\]$)");
    std::smatch indexMatch;
    
    if (std::regex_match(selector, indexMatch, indexRegex)) {
        std::string baseSelector = indexMatch[1].str();
        std::string index = indexMatch[2].str();
        
        js << "(function() {\n";
        js << "    const elements = document.querySelectorAll('" << baseSelector << "');\n";
        js << "    return elements[" << index << "] || null;\n";
        js << "})()";
    } else {
        // 根据选择器类型生成不同的代码
        if (selector.find(' ') != std::string::npos) {
            // 复合选择器
            js << "document.querySelectorAll('" << selector << "')";
        } else if (selector.front() == '#') {
            // ID选择器
            js << "document.getElementById('" << selector.substr(1) << "')";
        } else if (selector.front() == '.') {
            // 类选择器
            js << "document.getElementsByClassName('" << selector.substr(1) << "')[0]";
        } else {
            // 标签选择器或其他
            // 先检查是否为标签名，然后查找类名/id
            js << "(function() {\n";
            js << "    // 先尝试标签选择器\n";
            js << "    let element = document.getElementsByTagName('" << selector << "')[0];\n";
            js << "    if (element) return element;\n";
            js << "    \n";
            js << "    // 再尝试类名选择器 (id优先)\n";
            js << "    element = document.getElementById('" << selector << "');\n";
            js << "    if (element) return element;\n";
            js << "    \n";
            js << "    element = document.getElementsByClassName('" << selector << "')[0];\n";
            js << "    if (element) return element;\n";
            js << "    \n";
            js << "    return null;\n";
            js << "})()";
        }
    }
    
    return js.str();
}

/**
 * 箭头操作符处理器实现
 */
std::string ArrowOperatorProcessor::processArrowOperator(const std::string& code) {
    std::string result = code;
    
    // 查找所有的 -> 并替换为 .
    size_t pos = 0;
    while ((pos = result.find("->", pos)) != std::string::npos) {
        // 检查是否在字符串字面量或注释中
        if (!isInStringLiteral(result, pos) && !isInComment(result, pos)) {
            result.replace(pos, 2, ".");
            std::cout << "🔄 箭头操作符转换: -> → ." << std::endl;
        }
        pos += 1;
    }
    
    return result;
}

bool ArrowOperatorProcessor::isInStringLiteral(const std::string& code, size_t pos) {
    bool inSingle = false;
    bool inDouble = false;
    
    for (size_t i = 0; i < pos; i++) {
        if (code[i] == '\'' && (i == 0 || code[i-1] != '\\')) {
            if (!inDouble) inSingle = !inSingle;
        } else if (code[i] == '"' && (i == 0 || code[i-1] != '\\')) {
            if (!inSingle) inDouble = !inDouble;
        }
    }
    
    return inSingle || inDouble;
}

bool ArrowOperatorProcessor::isInComment(const std::string& code, size_t pos) {
    // 检查是否在 // 注释中
    size_t lineStart = code.rfind('\n', pos);
    if (lineStart == std::string::npos) lineStart = 0;
    
    size_t commentPos = code.find("//", lineStart);
    if (commentPos != std::string::npos && commentPos < pos) {
        return true;
    }
    
    // 检查是否在 /* */ 注释中
    size_t blockStart = code.rfind("/*", pos);
    size_t blockEnd = code.rfind("*/", pos);
    
    return blockStart != std::string::npos && (blockEnd == std::string::npos || blockStart > blockEnd);
}

/**
 * Listen处理器实现
 */
std::string ListenProcessor::processListen(const std::string& code) {
    std::string result = code;
    
    // 匹配 selector.listen({...}) 或 {{selector}}.listen({...})
    std::regex listenRegex(R"((\{\{[^}]+\}\}|\w+)\.listen\s*\(\s*\{([^}]*(?:\{[^}]*\}[^}]*)*)\}\s*\))");
    std::smatch match;
    
    std::string::const_iterator start = result.cbegin();
    std::stringstream processed;
    
    while (std::regex_search(start, result.cend(), match, listenRegex)) {
        processed << std::string(start, match[0].first);
        
        std::string selector = match[1].str();
        std::string events = match[2].str();
        
        processed << generateListenCode(selector, events);
        
        start = match[0].second;
    }
    
    processed << std::string(start, result.cend());
    
    return processed.str();
}

std::vector<std::string> ListenProcessor::getAllJSEvents() {
    return {
        // 鼠标事件
        "click", "dblclick", "mousedown", "mouseup", "mouseover", "mouseout",
        "mousemove", "mouseenter", "mouseleave", "contextmenu", "wheel",
        
        // 键盘事件
        "keydown", "keyup", "keypress",
        
        // 表单事件
        "submit", "change", "input", "focus", "blur", "select", "reset",
        
        // 窗口事件
        "load", "unload", "resize", "scroll", "error",
        
        // 触摸事件
        "touchstart", "touchmove", "touchend", "touchcancel",
        
        // 拖拽事件
        "dragstart", "drag", "dragend", "dragover", "dragenter", "dragleave", "drop",
        
        // 媒体事件
        "play", "pause", "ended", "volumechange", "timeupdate",
        
        // 其他常用事件
        "animationstart", "animationend", "transitionend"
    };
}

std::string ListenProcessor::generateListenCode(const std::string& selector, const std::string& events) {
    std::stringstream js;
    
    js << "(function() {\n";
    js << "    const element = " << selector << ";\n";
    js << "    if (!element) {\n";
    js << "        console.warn('🔍 Listen: 找不到元素', '" << selector << "');\n";
    js << "        return;\n";
    js << "    }\n";
    js << "    \n";
    js << "    const events = " << parseEventObject(events) << ";\n";
    js << "    \n";
    js << "    // 绑定所有事件\n";
    js << "    Object.keys(events).forEach(eventType => {\n";
    js << "        if (typeof events[eventType] === 'function') {\n";
    js << "            element.addEventListener(eventType, events[eventType]);\n";
    js << "            console.log('🎧 绑定事件:', eventType, '到', element);\n";
    js << "        }\n";
    js << "    });\n";
    js << "    \n";
    js << "    return element;\n";
    js << "})()";
    
    return js.str();
}

std::string ListenProcessor::parseEventObject(const std::string& eventObj) {
    std::stringstream parsed;
    parsed << "{\n";
    
    // 简单解析事件对象
    std::string trimmed = eventObj;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n") + 1);
    
    // 分割事件定义
    std::stringstream ss(trimmed);
    std::string line;
    bool first = true;
    
    while (std::getline(ss, line, ',')) {
        if (!first) parsed << ",\n";
        first = false;
        
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string eventName = line.substr(0, colonPos);
            std::string eventHandler = line.substr(colonPos + 1);
            
            // 清理空白
            eventName.erase(0, eventName.find_first_not_of(" \t"));
            eventName.erase(eventName.find_last_not_of(" \t") + 1);
            eventHandler.erase(0, eventHandler.find_first_not_of(" \t"));
            eventHandler.erase(eventHandler.find_last_not_of(" \t") + 1);
            
            // 验证事件名称
            if (isValidEvent(eventName)) {
                parsed << "        " << eventName << ": " << eventHandler;
            }
        }
    }
    
    parsed << "\n    }";
    
    return parsed.str();
}

bool ListenProcessor::isValidEvent(const std::string& eventName) {
    auto events = getAllJSEvents();
    return std::find(events.begin(), events.end(), eventName) != events.end();
}

/**
 * Delegate处理器实现
 */
std::string DelegateProcessor::processDelegate(const std::string& code) {
    std::string result = code;
    
    // 匹配 selector.delegate({...})
    std::regex delegateRegex(R"((\{\{[^}]+\}\}|\w+)\.delegate\s*\(\s*\{([^}]*(?:\{[^}]*\}[^}]*)*)\}\s*\))");
    std::smatch match;
    
    std::string::const_iterator start = result.cbegin();
    std::stringstream processed;
    
    while (std::regex_search(start, result.cend(), match, delegateRegex)) {
        processed << std::string(start, match[0].first);
        
        std::string parentSelector = match[1].str();
        std::string config = match[2].str();
        
        processed << generateDelegateCode(parentSelector, config);
        
        start = match[0].second;
    }
    
    processed << std::string(start, result.cend());
    
    return processed.str();
}

std::string DelegateProcessor::generateDelegateRegistry() {
    std::stringstream js;
    
    js << "// 🎯 CHTL JS事件委托全局注册表\n";
    js << "window.__chtl_delegate_registry = window.__chtl_delegate_registry || {\n";
    js << "    delegates: new Map(),\n";
    js << "    \n";
    js << "    register: function(parentElement, targetSelectors, eventType, handler) {\n";
    js << "        const key = parentElement.toString() + '_' + eventType;\n";
    js << "        \n";
    js << "        if (!this.delegates.has(key)) {\n";
    js << "            // 创建新的事件委托\n";
    js << "            const delegateHandler = (event) => {\n";
    js << "                const delegates = this.delegates.get(key) || [];\n";
    js << "                \n";
    js << "                delegates.forEach(delegate => {\n";
    js << "                    delegate.selectors.forEach(selector => {\n";
    js << "                        if (event.target.matches && event.target.matches(selector)) {\n";
    js << "                            delegate.handler.call(event.target, event);\n";
    js << "                        }\n";
    js << "                    });\n";
    js << "                });\n";
    js << "            };\n";
    js << "            \n";
    js << "            parentElement.addEventListener(eventType, delegateHandler);\n";
    js << "            this.delegates.set(key, []);\n";
    js << "            \n";
    js << "            console.log('🎯 创建事件委托:', key);\n";
    js << "        }\n";
    js << "        \n";
    js << "        // 添加到现有委托\n";
    js << "        const delegates = this.delegates.get(key);\n";
    js << "        delegates.push({\n";
    js << "            selectors: targetSelectors,\n";
    js << "            handler: handler\n";
    js << "        });\n";
    js << "        \n";
    js << "        console.log('🔗 添加委托分支:', targetSelectors);\n";
    js << "    }\n";
    js << "};\n";
    
    return js.str();
}

std::string DelegateProcessor::generateDelegateCode(const std::string& parentSelector, const std::string& config) {
    std::stringstream js;
    
    js << "(function() {\n";
    js << "    const parentElement = " << parentSelector << ";\n";
    js << "    if (!parentElement) {\n";
    js << "        console.warn('🎯 Delegate: 找不到父元素', '" << parentSelector << "');\n";
    js << "        return;\n";
    js << "    }\n";
    js << "    \n";
    js << "    const config = " << parseDelegateConfig(config) << ";\n";
    js << "    \n";
    js << "    // 处理target选择器\n";
    js << "    let targetSelectors = [];\n";
    js << "    if (Array.isArray(config.target)) {\n";
    js << "        targetSelectors = config.target;\n";
    js << "    } else if (typeof config.target === 'string') {\n";
    js << "        targetSelectors = [config.target];\n";
    js << "    }\n";
    js << "    \n";
    js << "    // 注册所有事件委托\n";
    js << "    Object.keys(config).forEach(key => {\n";
    js << "        if (key !== 'target' && typeof config[key] === 'function') {\n";
    js << "            window.__chtl_delegate_registry.register(\n";
    js << "                parentElement, targetSelectors, key, config[key]\n";
    js << "            );\n";
    js << "        }\n";
    js << "    });\n";
    js << "    \n";
    js << "    return parentElement;\n";
    js << "})()";
    
    return js.str();
}

std::string DelegateProcessor::parseDelegateConfig(const std::string& config) {
    // 简单解析委托配置对象
    std::stringstream parsed;
    parsed << "{\n" << config << "\n}";
    return parsed.str();
}

/**
 * Animate处理器实现
 */
std::string AnimateProcessor::processAnimate(const std::string& code) {
    std::string result = code;
    
    // 匹配 animate({...})
    std::regex animateRegex(R"(animate\s*\(\s*\{([^}]*(?:\{[^}]*\}[^}]*)*)\}\s*\))");
    std::smatch match;
    
    std::string::const_iterator start = result.cbegin();
    std::stringstream processed;
    
    while (std::regex_search(start, result.cend(), match, animateRegex)) {
        processed << std::string(start, match[0].first);
        
        std::string config = match[1].str();
        processed << generateAnimateCode(config);
        
        start = match[0].second;
    }
    
    processed << std::string(start, result.cend());
    
    return processed.str();
}

std::string AnimateProcessor::generateAnimateCode(const std::string& config) {
    std::stringstream js;
    
    js << "(function() {\n";
    js << "    const config = " << parseAnimateConfig(config) << ";\n";
    js << "    \n";
    js << "    // 获取目标元素\n";
    js << "    let targets = [];\n";
    js << "    if (Array.isArray(config.target)) {\n";
    js << "        targets = config.target;\n";
    js << "    } else if (config.target) {\n";
    js << "        targets = [config.target];\n";
    js << "    }\n";
    js << "    \n";
    js << "    if (targets.length === 0) {\n";
    js << "        console.warn('🎬 Animate: 没有找到目标元素');\n";
    js << "        return null;\n";
    js << "    }\n";
    js << "    \n";
    js << "    // 动画参数\n";
    js << "    const duration = config.duration || 1000;\n";
    js << "    const easing = " << generateEasingFunction("config.easing || 'ease'") << ";\n";
    js << "    const loop = config.loop || 1;\n";
    js << "    const delay = config.delay || 0;\n";
    js << "    \n";
    js << "    // 动画执行函数\n";
    js << "    const runAnimation = () => {\n";
    js << "        let startTime = null;\n";
    js << "        \n";
    js << "        const animate = (currentTime) => {\n";
    js << "            if (!startTime) startTime = currentTime;\n";
    js << "            \n";
    js << "            const elapsed = currentTime - startTime;\n";
    js << "            const progress = Math.min(elapsed / duration, 1);\n";
    js << "            const easedProgress = easing(progress);\n";
    js << "            \n";
    js << "            // 应用动画状态\n";
    js << "            targets.forEach(target => {\n";
    js << "                if (config.begin && progress === 0) {\n";
    js << "                    Object.assign(target.style, config.begin);\n";
    js << "                }\n";
    js << "                \n";
    js << "                // 处理关键帧\n";
    js << "                if (config.when && Array.isArray(config.when)) {\n";
    js << "                    config.when.forEach(keyframe => {\n";
    js << "                        if (Math.abs(progress - keyframe.at) < 0.01) {\n";
    js << "                            Object.keys(keyframe).forEach(prop => {\n";
    js << "                                if (prop !== 'at') {\n";
    js << "                                    target.style[prop] = keyframe[prop];\n";
    js << "                                }\n";
    js << "                            });\n";
    js << "                        }\n";
    js << "                    });\n";
    js << "                }\n";
    js << "                \n";
    js << "                if (config.end && progress === 1) {\n";
    js << "                    Object.assign(target.style, config.end);\n";
    js << "                }\n";
    js << "            });\n";
    js << "            \n";
    js << "            if (progress < 1) {\n";
    js << "                requestAnimationFrame(animate);\n";
    js << "            } else {\n";
    js << "                // 动画完成\n";
    js << "                if (config.callback && typeof config.callback === 'function') {\n";
    js << "                    config.callback();\n";
    js << "                }\n";
    js << "                \n";
    js << "                console.log('🎬 动画完成');\n";
    js << "            }\n";
    js << "        };\n";
    js << "        \n";
    js << "        requestAnimationFrame(animate);\n";
    js << "    };\n";
    js << "    \n";
    js << "    // 延迟执行\n";
    js << "    if (delay > 0) {\n";
    js << "        setTimeout(runAnimation, delay);\n";
    js << "    } else {\n";
    js << "        runAnimation();\n";
    js << "    }\n";
    js << "    \n";
    js << "    return {\n";
    js << "        targets: targets,\n";
    js << "        duration: duration,\n";
    js << "        stop: function() { /* 停止动画的实现 */ }\n";
    js << "    };\n";
    js << "})()";
    
    return js.str();
}

std::string AnimateProcessor::parseAnimateConfig(const std::string& config) {
    std::stringstream parsed;
    parsed << "{\n" << config << "\n}";
    return parsed.str();
}

std::string AnimateProcessor::generateEasingFunction(const std::string& easing) {
    std::stringstream js;
    
    js << "(function(t) {\n";
    js << "    const easingType = " << easing << ";\n";
    js << "    \n";
    js << "    switch(easingType) {\n";
    js << "        case 'ease-in':\n";
    js << "            return t * t;\n";
    js << "        case 'ease-out':\n";
    js << "            return t * (2 - t);\n";
    js << "        case 'ease-in-out':\n";
    js << "            return t < 0.5 ? 2 * t * t : -1 + (4 - 2 * t) * t;\n";
    js << "        default:\n";
    js << "            return t; // linear\n";
    js << "    }\n";
    js << "})";
    
    return js.str();
}

std::string AnimateProcessor::generateKeyframes(const std::string& keyframes) {
    return keyframes; // 简单返回，实际应该解析关键帧
}

/**
 * Vir处理器实现
 */
std::string VirProcessor::processVir(const std::string& code) {
    std::string result = code;
    
    // 匹配 vir name = definition
    std::regex virRegex(R"(vir\s+(\w+)\s*=\s*([^;]+);?)");
    std::smatch match;
    
    std::string::const_iterator start = result.cbegin();
    std::stringstream processed;
    
    while (std::regex_search(start, result.cend(), match, virRegex)) {
        processed << std::string(start, match[0].first);
        
        std::string name = match[1].str();
        std::string definition = match[2].str();
        
        processed << generateVirObject(name, definition);
        
        start = match[0].second;
    }
    
    processed << std::string(start, result.cend());
    
    return processed.str();
}

std::string VirProcessor::processVirCall(const std::string& code) {
    std::string result = code;
    
    // 匹配 virName->method() 调用
    for (const auto& [virName, methods] : virObjects) {
        for (const auto& [methodName, methodCode] : methods) {
            std::string pattern = virName + "->" + methodName;
            size_t pos = 0;
            
            while ((pos = result.find(pattern, pos)) != std::string::npos) {
                std::string replacement = generateVirCallCode(virName, methodName);
                result.replace(pos, pattern.length(), replacement);
                pos += replacement.length();
            }
        }
    }
    
    return result;
}

std::string VirProcessor::generateVirObject(const std::string& name, const std::string& definition) {
    std::stringstream js;
    
    js << "(function() {\n";
    js << "    // 🌟 Vir虚对象: " << name << "\n";
    js << "    const virDefinition = " << definition << ";\n";
    js << "    \n";
    js << "    // 创建全局函数引用表\n";
    js << "    window.__chtl_vir_objects = window.__chtl_vir_objects || {};\n";
    js << "    window.__chtl_vir_objects['" << name << "'] = {};\n";
    js << "    \n";
    js << "    // 解析虚对象中的函数键\n";
    js << "    if (typeof virDefinition === 'object') {\n";
    js << "        Object.keys(virDefinition).forEach(key => {\n";
    js << "            if (typeof virDefinition[key] === 'function') {\n";
    js << "                const globalFuncName = '__chtl_vir_' + '" << name << "' + '_' + key.replace(/[<>]/g, '_');\n";
    js << "                window[globalFuncName] = virDefinition[key];\n";
    js << "                window.__chtl_vir_objects['" << name << "'][key] = globalFuncName;\n";
    js << "                \n";
    js << "                console.log('🌟 注册虚对象函数:', globalFuncName);\n";
    js << "            }\n";
    js << "        });\n";
    js << "    }\n";
    js << "    \n";
    js << "    console.log('✨ 虚对象创建完成:', '" << name << "');\n";
    js << "    return virDefinition;\n";
    js << "})()";
    
    return js.str();
}

std::string VirProcessor::generateVirCallCode(const std::string& name, const std::string& method) {
    std::stringstream js;
    
    js << "(function() {\n";
    js << "    const virObj = window.__chtl_vir_objects && window.__chtl_vir_objects['" << name << "'];\n";
    js << "    if (virObj && virObj['" << method << "']) {\n";
    js << "        return window[virObj['" << method << "']];\n";
    js << "    }\n";
    js << "    console.warn('🌟 虚对象调用失败:', '" << name << "', '" << method << "');\n";
    js << "    return null;\n";
    js << "})()";
    
    return js.str();
}

/**
 * 局部Script处理器实现
 */
std::string LocalScriptProcessor::processLocalScript(const std::string& code, const std::string& elementContext) {
    std::cout << "📝 处理局部script块: " << elementContext << std::endl;
    
    std::string wrappedScript = wrapLocalScript(code);
    addLocalScript(wrappedScript, 100); // 高优先级
    
    return ""; // 局部script不直接输出，而是添加到全局队列
}

std::string LocalScriptProcessor::generateGlobalScriptMerge() {
    std::stringstream js;
    
    js << "// 🚀 CHTL JS局部script全局合并 - 高优先级，不全局污染\n";
    js << "(function() {\n";
    js << "    'use strict';\n";
    js << "    \n";
    js << "    console.log('🚀 执行CHTL JS局部script合并');\n";
    js << "    \n";
    
    // 按优先级排序并合并所有局部script
    std::sort(localScripts.begin(), localScripts.end(), 
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    for (const auto& [script, priority] : localScripts) {
        js << "    // Priority: " << priority << "\n";
        js << "    " << script << "\n";
        js << "    \n";
    }
    
    js << "    console.log('✅ CHTL JS局部script执行完成');\n";
    js << "})();\n";
    
    return js.str();
}

void LocalScriptProcessor::addLocalScript(const std::string& script, int priority) {
    localScripts.emplace_back(script, priority);
    std::cout << "📝 添加局部script到全局队列，优先级: " << priority << std::endl;
}

std::string LocalScriptProcessor::wrapLocalScript(const std::string& script) {
    std::stringstream wrapped;
    
    wrapped << "try {\n";
    wrapped << "    " << script << "\n";
    wrapped << "} catch (error) {\n";
    wrapped << "    console.error('❌ 局部script执行错误:', error);\n";
    wrapped << "}";
    
    return wrapped.str();
}

/**
 * CHTL JS主处理器实现
 */
void CHTLJSProcessor::initialize() {
    if (initialized) return;
    
    std::cout << "🚀 初始化CHTL JS处理器" << std::endl;
    
    registerProcessors();
    initialized = true;
    
    std::cout << "✅ CHTL JS处理器初始化完成" << std::endl;
}

std::string CHTLJSProcessor::processCHTLJS(const std::string& code) {
    if (!initialized) {
        initialize();
    }
    
    std::cout << "🔍 CHTL JS处理器开始处理代码" << std::endl;
    
    if (!isCHTLJSSyntax(code)) {
        std::cout << "📄 检测到标准JS代码，跳过CHTL JS处理" << std::endl;
        return code;
    }
    
    return applyAllProcessors(code);
}

bool CHTLJSProcessor::isCHTLJSSyntax(const std::string& code) {
    // 检查是否包含CHTL JS特有语法
    std::vector<std::string> chtljsKeywords = {
        "{{", "}}", "->", "listen", "delegate", "animate", "vir", "iNeverAway", "printMylove"
    };
    
    for (const auto& keyword : chtljsKeywords) {
        if (code.find(keyword) != std::string::npos) {
            return true;
        }
    }
    
    return false;
}

void CHTLJSProcessor::registerProcessors() {
    std::cout << "📋 注册CHTL JS处理器" << std::endl;
    
    // 按处理顺序注册
    processors.push_back([](const std::string& code) {
        return EnhancedSelectorProcessor::processEnhancedSelectors(code);
    });
    
    processors.push_back([](const std::string& code) {
        return ArrowOperatorProcessor::processArrowOperator(code);
    });
    
    processors.push_back([](const std::string& code) {
        return ListenProcessor::processListen(code);
    });
    
    processors.push_back([](const std::string& code) {
        return DelegateProcessor::processDelegate(code);
    });
    
    processors.push_back([](const std::string& code) {
        return AnimateProcessor::processAnimate(code);
    });
    
    processors.push_back([](const std::string& code) {
        return VirProcessor::processVir(code);
    });
    
    processors.push_back([](const std::string& code) {
        return VirProcessor::processVirCall(code);
    });
    
    std::cout << "✅ 已注册 " << processors.size() << " 个处理器" << std::endl;
}

std::string CHTLJSProcessor::applyAllProcessors(const std::string& code) {
    std::string result = code;
    
    for (size_t i = 0; i < processors.size(); ++i) {
        std::cout << "🔄 应用处理器 " << (i + 1) << "/" << processors.size() << std::endl;
        result = processors[i](result);
    }
    
    std::cout << "✅ 所有CHTL JS处理器应用完成" << std::endl;
    return result;
}

/**
 * 自动添加管理器实现
 */
std::string AutoAdditionManager::processAutoAddition(const std::string& code, const std::string& elementInfo) {
    std::string selectorUsage = extractSelectorUsage(code);
    
    if (selectorUsage.empty() || hasExplicitClassOrId(elementInfo)) {
        return ""; // 不需要自动添加
    }
    
    // 根据语法文档实现自动添加逻辑
    if (selectorUsage.find(".") == 0) {
        // 类选择器
        std::string className = selectorUsage.substr(1);
        return generateAutoAddition("class", className);
    } else if (selectorUsage.find("#") == 0) {
        // ID选择器  
        std::string idName = selectorUsage.substr(1);
        return generateAutoAddition("id", idName);
    }
    
    return "";
}

bool AutoAdditionManager::needsAutoAddition(const std::string& elementInfo, const std::string& selectorUsage) {
    return !selectorUsage.empty() && !hasExplicitClassOrId(elementInfo);
}

std::string AutoAdditionManager::generateAutoAddition(const std::string& selectorType, const std::string& selectorName) {
    if (selectorType == "class") {
        return "class=\"" + selectorName + "\"";
    } else if (selectorType == "id") {
        return "id=\"" + selectorName + "\"";
    }
    
    return "";
}

std::string AutoAdditionManager::extractSelectorUsage(const std::string& code) {
    // 查找第一个类或ID选择器使用
    std::regex classRegex(R"(\.([a-zA-Z_][a-zA-Z0-9_-]*))");
    std::regex idRegex(R"(#([a-zA-Z_][a-zA-Z0-9_-]*))");
    
    std::smatch match;
    
    if (std::regex_search(code, match, idRegex)) {
        return "#" + match[1].str(); // ID优先
    }
    
    if (std::regex_search(code, match, classRegex)) {
        return "." + match[1].str();
    }
    
    return "";
}

bool AutoAdditionManager::hasExplicitClassOrId(const std::string& elementInfo) {
    return elementInfo.find("class=") != std::string::npos || 
           elementInfo.find("id=") != std::string::npos;
}

} // namespace chtljs
} // namespace chtl