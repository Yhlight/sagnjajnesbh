// ========================================
// CJMOD语法扩展器 - 用C++编写新的CHTL JS语法
// 这些C++代码会被CHTL JS编译器调用，生成对应的JavaScript代码
// ========================================

#include <string>
#include <vector>
#include <unordered_map>
#include <regex>
#include <sstream>

// ========================================
// 1. 扩展新语法：{{animate}}增强版
// ========================================

/**
 * 处理animate语法扩展
 * CHTL JS: animate({target: {{#box}}, effect: 'bounce', duration: 1000})
 * 生成JS: 复杂的动画序列代码
 */
std::string processAnimateExtension(const std::string& chtljs_code) {
    // 解析CHTL JS参数
    std::regex animateRegex(R"(animate\s*\(\s*\{([^}]+)\}\s*\))");
    std::smatch matches;
    
    if (!std::regex_search(chtljs_code, matches, animateRegex)) {
        return "";
    }
    
    std::string params = matches[1].str();
    
    // 解析参数
    std::string target = "document.body";
    std::string effect = "fade";
    int duration = 1000;
    
    // 简化的参数解析
    if (params.find("target:") != std::string::npos) {
        // 提取target
        std::regex targetRegex(R"(target:\s*\{\{([^}]+)\}\})");
        std::smatch targetMatch;
        if (std::regex_search(params, targetMatch, targetRegex)) {
            std::string selector = targetMatch[1].str();
            if (selector[0] == '#') {
                target = "document.getElementById('" + selector.substr(1) + "')";
            } else if (selector[0] == '.') {
                target = "document.querySelector('." + selector.substr(1) + "')";
            } else {
                target = "document.querySelector('" + selector + "')";
            }
        }
    }
    
    if (params.find("effect:") != std::string::npos) {
        std::regex effectRegex(R"(effect:\s*['"](.*?)['"])");
        std::smatch effectMatch;
        if (std::regex_search(params, effectMatch, effectRegex)) {
            effect = effectMatch[1].str();
        }
    }
    
    if (params.find("duration:") != std::string::npos) {
        std::regex durationRegex(R"(duration:\s*(\d+))");
        std::smatch durationMatch;
        if (std::regex_search(params, durationMatch, durationRegex)) {
            duration = std::stoi(durationMatch[1].str());
        }
    }
    
    // 生成对应的JavaScript代码
    std::stringstream js;
    js << "(() => {\n";
    js << "    const element = " << target << ";\n";
    js << "    if (!element) return;\n";
    js << "    \n";
    
    if (effect == "bounce") {
        js << "    // 弹跳动画效果\n";
        js << "    element.animate([\n";
        js << "        { transform: 'translateY(0px)' },\n";
        js << "        { transform: 'translateY(-20px)' },\n";
        js << "        { transform: 'translateY(0px)' },\n";
        js << "        { transform: 'translateY(-10px)' },\n";
        js << "        { transform: 'translateY(0px)' }\n";
        js << "    ], {\n";
        js << "        duration: " << duration << ",\n";
        js << "        easing: 'cubic-bezier(0.68, -0.55, 0.265, 1.55)'\n";
        js << "    });\n";
    } else if (effect == "shake") {
        js << "    // 震动动画效果\n";
        js << "    element.animate([\n";
        js << "        { transform: 'translateX(0px)' },\n";
        js << "        { transform: 'translateX(-10px)' },\n";
        js << "        { transform: 'translateX(10px)' },\n";
        js << "        { transform: 'translateX(-5px)' },\n";
        js << "        { transform: 'translateX(5px)' },\n";
        js << "        { transform: 'translateX(0px)' }\n";
        js << "    ], {\n";
        js << "        duration: " << duration << ",\n";
        js << "        easing: 'ease-in-out'\n";
        js << "    });\n";
    } else if (effect == "pulse") {
        js << "    // 脉冲动画效果\n";
        js << "    element.animate([\n";
        js << "        { transform: 'scale(1)', opacity: 1 },\n";
        js << "        { transform: 'scale(1.1)', opacity: 0.8 },\n";
        js << "        { transform: 'scale(1)', opacity: 1 }\n";
        js << "    ], {\n";
        js << "        duration: " << duration << ",\n";
        js << "        iterations: Infinity,\n";
        js << "        direction: 'alternate'\n";
        js << "    });\n";
    } else {
        // 默认淡入效果
        js << "    // 淡入动画效果\n";
        js << "    element.animate([\n";
        js << "        { opacity: 0 },\n";
        js << "        { opacity: 1 }\n";
        js << "    ], {\n";
        js << "        duration: " << duration << ",\n";
        js << "        fill: 'forwards'\n";
        js << "    });\n";
    }
    
    js << "})();";
    
    return js.str();
}

// ========================================
// 2. 扩展新语法：{{storage}}本地存储
// ========================================

/**
 * 处理storage语法扩展
 * CHTL JS: storage.set('key', value) 或 storage.get('key')
 * 生成JS: localStorage的增强版本，支持对象、过期时间等
 */
std::string processStorageExtension(const std::string& chtljs_code) {
    std::stringstream js;
    
    // 检查是否是storage.set操作
    std::regex setRegex(R"(storage\.set\s*\(\s*['"](.*?)['"],\s*(.*?)\s*\))");
    std::smatch setMatch;
    
    if (std::regex_search(chtljs_code, setMatch, setRegex)) {
        std::string key = setMatch[1].str();
        std::string value = setMatch[2].str();
        
        js << "(() => {\n";
        js << "    try {\n";
        js << "        const data = {\n";
        js << "            value: " << value << ",\n";
        js << "            timestamp: Date.now(),\n";
        js << "            type: typeof(" << value << ")\n";
        js << "        };\n";
        js << "        localStorage.setItem('" << key << "', JSON.stringify(data));\n";
        js << "        console.log('💾 存储成功:', '" << key << "', data.value);\n";
        js << "        return true;\n";
        js << "    } catch (e) {\n";
        js << "        console.error('❌ 存储失败:', e);\n";
        js << "        return false;\n";
        js << "    }\n";
        js << "})()";
        
        return js.str();
    }
    
    // 检查是否是storage.get操作
    std::regex getRegex(R"(storage\.get\s*\(\s*['"](.*?)['"](?:,\s*(.*?))?\s*\))");
    std::smatch getMatch;
    
    if (std::regex_search(chtljs_code, getMatch, getRegex)) {
        std::string key = getMatch[1].str();
        std::string defaultValue = getMatch.size() > 2 ? getMatch[2].str() : "null";
        
        js << "(() => {\n";
        js << "    try {\n";
        js << "        const stored = localStorage.getItem('" << key << "');\n";
        js << "        if (!stored) return " << defaultValue << ";\n";
        js << "        \n";
        js << "        const data = JSON.parse(stored);\n";
        js << "        console.log('📁 读取成功:', '" << key << "', data.value);\n";
        js << "        \n";
        js << "        // 类型还原\n";
        js << "        if (data.type === 'number') return Number(data.value);\n";
        js << "        if (data.type === 'boolean') return Boolean(data.value);\n";
        js << "        return data.value;\n";
        js << "    } catch (e) {\n";
        js << "        console.error('❌ 读取失败:', e);\n";
        js << "        return " << defaultValue << ";\n";
        js << "    }\n";
        js << "})()";
        
        return js.str();
    }
    
    return "";
}

// ========================================
// 3. 扩展新语法：{{http}}网络请求
// ========================================

/**
 * 处理http语法扩展
 * CHTL JS: http.get('url') 或 http.post('url', data)
 * 生成JS: 现代fetch API的简化版本
 */
std::string processHttpExtension(const std::string& chtljs_code) {
    std::stringstream js;
    
    // 检查GET请求
    std::regex getRegex(R"(http\.get\s*\(\s*['"](.*?)['"](?:,\s*\{([^}]*)\})?\s*\))");
    std::smatch getMatch;
    
    if (std::regex_search(chtljs_code, getMatch, getRegex)) {
        std::string url = getMatch[1].str();
        std::string options = getMatch.size() > 2 ? getMatch[2].str() : "";
        
        js << "(async () => {\n";
        js << "    try {\n";
        js << "        console.log('🌐 发送GET请求:', '" << url << "');\n";
        js << "        \n";
        js << "        const response = await fetch('" << url << "', {\n";
        js << "            method: 'GET',\n";
        js << "            headers: {\n";
        js << "                'Content-Type': 'application/json',\n";
        js << "                'User-Agent': 'CHTL-JS/1.0'\n";
        js << "            }\n";
        js << "        });\n";
        js << "        \n";
        js << "        if (!response.ok) {\n";
        js << "            throw new Error(`HTTP ${response.status}: ${response.statusText}`);\n";
        js << "        }\n";
        js << "        \n";
        js << "        const data = await response.json();\n";
        js << "        console.log('✅ 请求成功:', data);\n";
        js << "        return data;\n";
        js << "    } catch (error) {\n";
        js << "        console.error('❌ 请求失败:', error);\n";
        js << "        return { error: error.message };\n";
        js << "    }\n";
        js << "})()";
        
        return js.str();
    }
    
    // 检查POST请求
    std::regex postRegex(R"(http\.post\s*\(\s*['"](.*?)['"],\s*(.*?)\s*\))");
    std::smatch postMatch;
    
    if (std::regex_search(chtljs_code, postMatch, postRegex)) {
        std::string url = postMatch[1].str();
        std::string data = postMatch[2].str();
        
        js << "(async () => {\n";
        js << "    try {\n";
        js << "        console.log('🌐 发送POST请求:', '" << url << "');\n";
        js << "        \n";
        js << "        const response = await fetch('" << url << "', {\n";
        js << "            method: 'POST',\n";
        js << "            headers: {\n";
        js << "                'Content-Type': 'application/json',\n";
        js << "                'User-Agent': 'CHTL-JS/1.0'\n";
        js << "            },\n";
        js << "            body: JSON.stringify(" << data << ")\n";
        js << "        });\n";
        js << "        \n";
        js << "        if (!response.ok) {\n";
        js << "            throw new Error(`HTTP ${response.status}: ${response.statusText}`);\n";
        js << "        }\n";
        js << "        \n";
        js << "        const result = await response.json();\n";
        js << "        console.log('✅ 请求成功:', result);\n";
        js << "        return result;\n";
        js << "    } catch (error) {\n";
        js << "        console.error('❌ 请求失败:', error);\n";
        js << "        return { error: error.message };\n";
        js << "    }\n";
        js << "})()";
        
        return js.str();
    }
    
    return "";
}

// ========================================
// 4. 扩展新语法：{{timer}}定时器管理
// ========================================

/**
 * 处理timer语法扩展
 * CHTL JS: timer.after(1000, callback) 或 timer.every(500, callback)
 * 生成JS: 增强的定时器管理代码
 */
std::string processTimerExtension(const std::string& chtljs_code) {
    std::stringstream js;
    
    // 检查timer.after
    std::regex afterRegex(R"(timer\.after\s*\(\s*(\d+),\s*(.*?)\s*\))");
    std::smatch afterMatch;
    
    if (std::regex_search(chtljs_code, afterMatch, afterRegex)) {
        std::string delay = afterMatch[1].str();
        std::string callback = afterMatch[2].str();
        
        js << "(() => {\n";
        js << "    const timerId = setTimeout(() => {\n";
        js << "        try {\n";
        js << "            console.log('⏰ 定时器触发:', " << delay << "ms);\n";
        js << "            (" << callback << ")();\n";
        js << "        } catch (error) {\n";
        js << "            console.error('❌ 定时器回调错误:', error);\n";
        js << "        }\n";
        js << "    }, " << delay << ");\n";
        js << "    \n";
        js << "    // 返回取消函数\n";
        js << "    return () => {\n";
        js << "        clearTimeout(timerId);\n";
        js << "        console.log('⏹️ 定时器已取消');\n";
        js << "    };\n";
        js << "})()";
        
        return js.str();
    }
    
    // 检查timer.every
    std::regex everyRegex(R"(timer\.every\s*\(\s*(\d+),\s*(.*?)\s*\))");
    std::smatch everyMatch;
    
    if (std::regex_search(chtljs_code, everyMatch, everyRegex)) {
        std::string interval = everyMatch[1].str();
        std::string callback = everyMatch[2].str();
        
        js << "(() => {\n";
        js << "    let count = 0;\n";
        js << "    const intervalId = setInterval(() => {\n";
        js << "        try {\n";
        js << "            count++;\n";
        js << "            console.log('🔄 定时器执行:', count, '次');\n";
        js << "            (" << callback << ")(count);\n";
        js << "        } catch (error) {\n";
        js << "            console.error('❌ 定时器回调错误:', error);\n";
        js << "        }\n";
        js << "    }, " << interval << ");\n";
        js << "    \n";
        js << "    // 返回控制对象\n";
        js << "    return {\n";
        js << "        stop: () => {\n";
        js << "            clearInterval(intervalId);\n";
        js << "            console.log('⏹️ 定时器已停止，共执行', count, '次');\n";
        js << "        },\n";
        js << "        getCount: () => count\n";
        js << "    };\n";
        js << "})()";
        
        return js.str();
    }
    
    return "";
}

// ========================================
// 5. 扩展新语法：{{validator}}表单验证
// ========================================

/**
 * 处理validator语法扩展
 * CHTL JS: validator.email({{#email-input}}) 或 validator.phone(value)
 * 生成JS: 智能表单验证代码
 */
std::string processValidatorExtension(const std::string& chtljs_code) {
    std::stringstream js;
    
    // 检查邮箱验证
    std::regex emailRegex(R"(validator\.email\s*\(\s*\{\{([^}]+)\}\}\s*\))");
    std::smatch emailMatch;
    
    if (std::regex_search(chtljs_code, emailMatch, emailRegex)) {
        std::string selector = emailMatch[1].str();
        
        js << "(() => {\n";
        js << "    const element = ";
        if (selector[0] == '#') {
            js << "document.getElementById('" << selector.substr(1) << "')";
        } else if (selector[0] == '.') {
            js << "document.querySelector('." << selector.substr(1) + "')";
        } else {
            js << "document.querySelector('" << selector << "')";
        }
        js << ";\n";
        js << "    \n";
        js << "    if (!element) {\n";
        js << "        console.error('❌ 验证元素未找到:', '" << selector << "');\n";
        js << "        return false;\n";
        js << "    }\n";
        js << "    \n";
        js << "    const value = element.value || element.textContent || '';\n";
        js << "    const emailRegex = /^[^\\s@]+@[^\\s@]+\\.[^\\s@]+$/;\n";
        js << "    const isValid = emailRegex.test(value);\n";
        js << "    \n";
        js << "    // 添加视觉反馈\n";
        js << "    element.style.borderColor = isValid ? '#4caf50' : '#f44336';\n";
        js << "    element.style.boxShadow = isValid ? \n";
        js << "        '0 0 5px rgba(76, 175, 80, 0.5)' : \n";
        js << "        '0 0 5px rgba(244, 67, 54, 0.5)';\n";
        js << "    \n";
        js << "    // 显示验证消息\n";
        js << "    const messageId = '" << selector << "_validation_message';\n";
        js << "    let messageEl = document.getElementById(messageId);\n";
        js << "    if (!messageEl) {\n";
        js << "        messageEl = document.createElement('div');\n";
        js << "        messageEl.id = messageId;\n";
        js << "        messageEl.style.fontSize = '12px';\n";
        js << "        messageEl.style.marginTop = '5px';\n";
        js << "        element.parentNode.appendChild(messageEl);\n";
        js << "    }\n";
        js << "    \n";
        js << "    messageEl.textContent = isValid ? \n";
        js << "        '✅ 邮箱格式正确' : \n";
        js << "        '❌ 请输入有效的邮箱地址';\n";
        js << "    messageEl.style.color = isValid ? '#4caf50' : '#f44336';\n";
        js << "    \n";
        js << "    console.log('📧 邮箱验证:', value, isValid ? '通过' : '失败');\n";
        js << "    return isValid;\n";
        js << "})()";
        
        return js.str();
    }
    
    return "";
}

// ========================================
// 6. 扩展新语法：{{notification}}通知系统
// ========================================

/**
 * 处理notification语法扩展
 * CHTL JS: notification.show('标题', '内容', 'success')
 * 生成JS: 美观的通知系统代码
 */
std::string processNotificationExtension(const std::string& chtljs_code) {
    std::regex notifyRegex(R"(notification\.show\s*\(\s*['"](.*?)['"],\s*['"](.*?)['"](?:,\s*['"](.*?)['"])?\s*\))");
    std::smatch notifyMatch;
    
    if (!std::regex_search(chtljs_code, notifyMatch, notifyRegex)) {
        return "";
    }
    
    std::string title = notifyMatch[1].str();
    std::string content = notifyMatch[2].str();
    std::string type = notifyMatch.size() > 3 ? notifyMatch[3].str() : "info";
    
    std::stringstream js;
    js << "(() => {\n";
    js << "    // 创建通知容器（如果不存在）\n";
    js << "    let container = document.getElementById('chtl_notification_container');\n";
    js << "    if (!container) {\n";
    js << "        container = document.createElement('div');\n";
    js << "        container.id = 'chtl_notification_container';\n";
    js << "        container.style.position = 'fixed';\n";
    js << "        container.style.top = '20px';\n";
    js << "        container.style.right = '20px';\n";
    js << "        container.style.zIndex = '9999';\n";
    js << "        document.body.appendChild(container);\n";
    js << "    }\n";
    js << "    \n";
    js << "    // 创建通知元素\n";
    js << "    const notification = document.createElement('div');\n";
    js << "    const notificationId = 'notification_' + Date.now();\n";
    js << "    notification.id = notificationId;\n";
    js << "    \n";
    js << "    // 设置样式\n";
    js << "    const colors = {\n";
    js << "        success: { bg: '#4caf50', icon: '✅' },\n";
    js << "        error: { bg: '#f44336', icon: '❌' },\n";
    js << "        warning: { bg: '#ff9800', icon: '⚠️' },\n";
    js << "        info: { bg: '#2196f3', icon: 'ℹ️' }\n";
    js << "    };\n";
    js << "    \n";
    js << "    const color = colors['" << type << "'] || colors.info;\n";
    js << "    \n";
    js << "    notification.style.cssText = `\n";
    js << "        background: ${color.bg};\n";
    js << "        color: white;\n";
    js << "        padding: 15px 20px;\n";
    js << "        border-radius: 8px;\n";
    js << "        margin-bottom: 10px;\n";
    js << "        box-shadow: 0 4px 12px rgba(0,0,0,0.2);\n";
    js << "        transform: translateX(100%);\n";
    js << "        transition: all 0.3s ease;\n";
    js << "        cursor: pointer;\n";
    js << "        max-width: 350px;\n";
    js << "        word-wrap: break-word;\n";
    js << "    `;\n";
    js << "    \n";
    js << "    notification.innerHTML = `\n";
    js << "        <div style=\"display: flex; align-items: center; gap: 10px;\">\n";
    js << "            <span style=\"font-size: 18px;\">${color.icon}</span>\n";
    js << "            <div>\n";
    js << "                <div style=\"font-weight: bold; margin-bottom: 4px;\">" << title << "</div>\n";
    js << "                <div style=\"font-size: 14px; opacity: 0.9;\">" << content << "</div>\n";
    js << "            </div>\n";
    js << "            <span style=\"margin-left: auto; opacity: 0.7; cursor: pointer;\">×</span>\n";
    js << "        </div>\n";
    js << "    `;\n";
    js << "    \n";
    js << "    // 添加到容器\n";
    js << "    container.appendChild(notification);\n";
    js << "    \n";
    js << "    // 动画进入\n";
    js << "    setTimeout(() => {\n";
    js << "        notification.style.transform = 'translateX(0)';\n";
    js << "    }, 10);\n";
    js << "    \n";
    js << "    // 点击关闭\n";
    js << "    notification.addEventListener('click', () => {\n";
    js << "        notification.style.transform = 'translateX(100%)';\n";
    js << "        setTimeout(() => {\n";
    js << "            if (notification.parentNode) {\n";
    js << "                notification.parentNode.removeChild(notification);\n";
    js << "            }\n";
    js << "        }, 300);\n";
    js << "    });\n";
    js << "    \n";
    js << "    // 自动消失\n";
    js << "    setTimeout(() => {\n";
    js << "        if (notification.parentNode) {\n";
    js << "            notification.style.transform = 'translateX(100%)';\n";
    js << "            setTimeout(() => {\n";
    js << "                if (notification.parentNode) {\n";
    js << "                    notification.parentNode.removeChild(notification);\n";
    js << "                }\n";
    js << "            }, 300);\n";
    js << "        }\n";
    js << "    }, 4000);\n";
    js << "    \n";
    js << "    console.log('🔔 通知已显示:', '" << title << "');\n";
    js << "    return notificationId;\n";
    js << "})()";
    
    return js.str();
}

// ========================================
// 7. 扩展新语法：{{math}}数学计算增强
// ========================================

/**
 * 处理math语法扩展
 * CHTL JS: math.calculate('2 + 3 * 4') 或 math.random(1, 100)
 * 生成JS: 安全的数学表达式计算
 */
std::string processMathExtension(const std::string& chtljs_code) {
    std::stringstream js;
    
    // 检查math.calculate
    std::regex calcRegex(R"(math\.calculate\s*\(\s*['"](.*?)['"](?:,\s*(\d+))?\s*\))");
    std::smatch calcMatch;
    
    if (std::regex_search(chtljs_code, calcMatch, calcRegex)) {
        std::string expression = calcMatch[1].str();
        std::string precision = calcMatch.size() > 2 ? calcMatch[2].str() : "2";
        
        js << "(() => {\n";
        js << "    try {\n";
        js << "        const expression = '" << expression << "';\n";
        js << "        console.log('🧮 计算表达式:', expression);\n";
        js << "        \n";
        js << "        // 安全的表达式验证\n";
        js << "        const safeExpression = expression.replace(/[^0-9+\\-*/.() ]/g, '');\n";
        js << "        if (safeExpression !== expression) {\n";
        js << "            throw new Error('表达式包含非法字符');\n";
        js << "        }\n";
        js << "        \n";
        js << "        // 计算结果\n";
        js << "        const result = Function('\"use strict\"; return (' + safeExpression + ')')();\n";
        js << "        const roundedResult = Number(result.toFixed(" << precision << "));\n";
        js << "        \n";
        js << "        console.log('✅ 计算结果:', expression, '=', roundedResult);\n";
        js << "        return roundedResult;\n";
        js << "    } catch (error) {\n";
        js << "        console.error('❌ 计算错误:', error);\n";
        js << "        return NaN;\n";
        js << "    }\n";
        js << "})()";
        
        return js.str();
    }
    
    // 检查math.random
    std::regex randomRegex(R"(math\.random\s*\(\s*(\d+),\s*(\d+)\s*\))");
    std::smatch randomMatch;
    
    if (std::regex_search(chtljs_code, randomMatch, randomRegex)) {
        std::string min = randomMatch[1].str();
        std::string max = randomMatch[2].str();
        
        js << "(() => {\n";
        js << "    const min = " << min << ";\n";
        js << "    const max = " << max << ";\n";
        js << "    const result = Math.floor(Math.random() * (max - min + 1)) + min;\n";
        js << "    console.log('🎲 随机数生成:', min, '-', max, '=', result);\n";
        js << "    return result;\n";
        js << "})()";
        
        return js.str();
    }
    
    return "";
}

// ========================================
// 8. 扩展新语法：{{format}}格式化工具
// ========================================

/**
 * 处理format语法扩展
 * CHTL JS: format.date(new Date()) 或 format.currency(123.45)
 * 生成JS: 智能格式化代码
 */
std::string processFormatExtension(const std::string& chtljs_code) {
    std::stringstream js;
    
    // 检查日期格式化
    std::regex dateRegex(R"(format\.date\s*\(\s*(.*?)(?:,\s*['"](.*?)['"])?\s*\))");
    std::smatch dateMatch;
    
    if (std::regex_search(chtljs_code, dateMatch, dateRegex)) {
        std::string dateValue = dateMatch[1].str();
        std::string format = dateMatch.size() > 2 ? dateMatch[2].str() : "YYYY-MM-DD HH:mm:ss";
        
        js << "(() => {\n";
        js << "    try {\n";
        js << "        const date = new Date(" << dateValue << ");\n";
        js << "        const format = '" << format << "';\n";
        js << "        \n";
        js << "        // 简化的日期格式化\n";
        js << "        const year = date.getFullYear();\n";
        js << "        const month = String(date.getMonth() + 1).padStart(2, '0');\n";
        js << "        const day = String(date.getDate()).padStart(2, '0');\n";
        js << "        const hours = String(date.getHours()).padStart(2, '0');\n";
        js << "        const minutes = String(date.getMinutes()).padStart(2, '0');\n";
        js << "        const seconds = String(date.getSeconds()).padStart(2, '0');\n";
        js << "        \n";
        js << "        let result = format\n";
        js << "            .replace('YYYY', year)\n";
        js << "            .replace('MM', month)\n";
        js << "            .replace('DD', day)\n";
        js << "            .replace('HH', hours)\n";
        js << "            .replace('mm', minutes)\n";
        js << "            .replace('ss', seconds);\n";
        js << "        \n";
        js << "        console.log('📅 日期格式化:', result);\n";
        js << "        return result;\n";
        js << "    } catch (error) {\n";
        js << "        console.error('❌ 日期格式化错误:', error);\n";
        js << "        return '';\n";
        js << "    }\n";
        js << "})()";
        
        return js.str();
    }
    
    // 检查货币格式化
    std::regex currencyRegex(R"(format\.currency\s*\(\s*(.*?)(?:,\s*['"](.*?)['"])?\s*\))");
    std::smatch currencyMatch;
    
    if (std::regex_search(chtljs_code, currencyMatch, currencyRegex)) {
        std::string amount = currencyMatch[1].str();
        std::string currency = currencyMatch.size() > 2 ? currencyMatch[2].str() : "CNY";
        
        js << "(() => {\n";
        js << "    try {\n";
        js << "        const amount = Number(" << amount << ");\n";
        js << "        const currency = '" << currency << "';\n";
        js << "        \n";
        js << "        // 使用Intl.NumberFormat进行格式化\n";
        js << "        const formatter = new Intl.NumberFormat('zh-CN', {\n";
        js << "            style: 'currency',\n";
        js << "            currency: currency,\n";
        js << "            minimumFractionDigits: 2\n";
        js << "        });\n";
        js << "        \n";
        js << "        const result = formatter.format(amount);\n";
        js << "        console.log('💰 货币格式化:', amount, '→', result);\n";
        js << "        return result;\n";
        js << "    } catch (error) {\n";
        js << "        console.error('❌ 货币格式化错误:', error);\n";
        js << "        return String(" << amount << ");\n";
        js << "    }\n";
        js << "})()";
        
        return js.str();
    }
    
    return "";
}

// ========================================
// CJMOD主入口函数
// CHTL JS编译器会调用这个函数来处理扩展语法
// ========================================

/**
 * CJMOD语法处理主函数
 * 输入：CHTL JS代码片段
 * 输出：对应的JavaScript代码
 */
extern "C" std::string processCJMODSyntax(const std::string& chtljs_code) {
    // 尝试各种语法扩展处理器
    std::string result;
    
    // 1. 动画扩展
    result = processAnimateExtension(chtljs_code);
    if (!result.empty()) return result;
    
    // 2. 存储扩展
    result = processStorageExtension(chtljs_code);
    if (!result.empty()) return result;
    
    // 3. 网络请求扩展
    result = processHttpExtension(chtljs_code);
    if (!result.empty()) return result;
    
    // 4. 定时器扩展
    result = processTimerExtension(chtljs_code);
    if (!result.empty()) return result;
    
    // 5. 验证器扩展
    result = processValidatorExtension(chtljs_code);
    if (!result.empty()) return result;
    
    // 6. 通知系统扩展
    result = processNotificationExtension(chtljs_code);
    if (!result.empty()) return result;
    
    // 7. 数学计算扩展
    result = processMathExtension(chtljs_code);
    if (!result.empty()) return result;
    
    // 8. 格式化工具扩展
    result = processFormatExtension(chtljs_code);
    if (!result.empty()) return result;
    
    // 如果没有匹配的扩展，返回原代码
    return chtljs_code;
}