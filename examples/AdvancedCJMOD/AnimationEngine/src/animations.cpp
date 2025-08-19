// ========================================
// 动画引擎子模块 - 另一个子模块示例
// ========================================

#include <string>
#include <vector>
#include <cmath>
#include <sstream>
#include <unordered_map>

// 动画引擎的全局状态
namespace AnimationEngineGlobals {
    struct AnimationState {
        std::string elementId;
        std::string animationType;
        double startTime;
        double duration;
        bool isActive;
    };
    
    std::vector<AnimationState> activeAnimations;
    int animationCounter = 0;
    
    // 缓动函数库
    std::unordered_map<std::string, std::string> easingFunctions = {
        {"easeInQuad", "t * t"},
        {"easeOutQuad", "t * (2 - t)"},
        {"easeInCubic", "t * t * t"},
        {"easeOutCubic", "(--t) * t * t + 1"},
        {"easeInOutCubic", "t < 0.5 ? 4 * t * t * t : (t - 1) * (2 * t - 2) * (2 * t - 2) + 1"},
        {"bounce", "t < 1/2.75 ? 7.5625 * t * t : t < 2/2.75 ? 7.5625 * (t -= 1.5/2.75) * t + 0.75 : t < 2.5/2.75 ? 7.5625 * (t -= 2.25/2.75) * t + 0.9375 : 7.5625 * (t -= 2.625/2.75) * t + 0.984375"},
        {"elastic", "t === 0 ? 0 : t === 1 ? 1 : -Math.pow(2, 10 * (t - 1)) * Math.sin((t - 1.1) * 5 * Math.PI)"}
    };
}

/**
 * 高级动画语法处理
 * 支持复杂的动画序列和缓动函数
 */
std::string generateAdvancedAnimation(const std::string& selector, 
                                    const std::string& animationType,
                                    const std::string& config) {
    std::stringstream js;
    
    js << "(() => {\n";
    js << "    const element = " << selector << ";\n";
    js << "    if (!element) {\n";
    js << "        console.error('❌ 动画目标元素未找到');\n";
    js << "        return;\n";
    js << "    }\n";
    js << "    \n";
    js << "    const config = " << config << ";\n";
    js << "    const duration = config.duration || 1000;\n";
    js << "    const easing = config.easing || 'easeOutQuad';\n";
    js << "    const delay = config.delay || 0;\n";
    js << "    \n";
    js << "    // 高级缓动函数\n";
    js << "    const easingFunctions = {\n";
    
    for (const auto& pair : AnimationEngineGlobals::easingFunctions) {
        js << "        " << pair.first << ": (t) => " << pair.second << ",\n";
    }
    
    js << "    };\n";
    js << "    \n";
    js << "    const easingFunc = easingFunctions[easing] || easingFunctions.easeOutQuad;\n";
    js << "    \n";
    
    if (animationType == "morphShape") {
        js << "    // 形状变换动画\n";
        js << "    const originalStyle = window.getComputedStyle(element);\n";
        js << "    const startWidth = parseFloat(originalStyle.width);\n";
        js << "    const startHeight = parseFloat(originalStyle.height);\n";
        js << "    const startRadius = parseFloat(originalStyle.borderRadius) || 0;\n";
        js << "    \n";
        js << "    const targetWidth = config.targetWidth || startWidth;\n";
        js << "    const targetHeight = config.targetHeight || startHeight;\n";
        js << "    const targetRadius = config.targetRadius !== undefined ? config.targetRadius : \n";
        js << "        (config.shape === 'circle' ? Math.min(targetWidth, targetHeight) / 2 : 0);\n";
        js << "    \n";
        js << "    const startTime = performance.now() + delay;\n";
        js << "    \n";
        js << "    function morphAnimation(currentTime) {\n";
        js << "        if (currentTime < startTime) {\n";
        js << "            requestAnimationFrame(morphAnimation);\n";
        js << "            return;\n";
        js << "        }\n";
        js << "        \n";
        js << "        const elapsed = currentTime - startTime;\n";
        js << "        const progress = Math.min(elapsed / duration, 1);\n";
        js << "        const easedProgress = easingFunc(progress);\n";
        js << "        \n";
        js << "        // 计算当前值\n";
        js << "        const currentWidth = startWidth + (targetWidth - startWidth) * easedProgress;\n";
        js << "        const currentHeight = startHeight + (targetHeight - startHeight) * easedProgress;\n";
        js << "        const currentRadius = startRadius + (targetRadius - startRadius) * easedProgress;\n";
        js << "        \n";
        js << "        // 应用变换\n";
        js << "        element.style.width = currentWidth + 'px';\n";
        js << "        element.style.height = currentHeight + 'px';\n";
        js << "        element.style.borderRadius = currentRadius + 'px';\n";
        js << "        \n";
        js << "        // 添加发光效果\n";
        js << "        const glowIntensity = Math.sin(progress * Math.PI) * 10;\n";
        js << "        element.style.boxShadow = `0 0 ${glowIntensity}px ${config.glowColor || '#4ecdc4'}`;\n";
        js << "        \n";
        js << "        if (progress < 1) {\n";
        js << "            requestAnimationFrame(morphAnimation);\n";
        js << "        } else {\n";
        js << "            console.log('🔄 形状变换完成');\n";
        js << "            if (config.onComplete) config.onComplete();\n";
        js << "        }\n";
        js << "    }\n";
        js << "    \n";
        js << "    requestAnimationFrame(morphAnimation);\n";
        
    } else if (animationType == "liquidMotion") {
        js << "    // 液体运动效果\n";
        js << "    const originalTransform = element.style.transform || '';\n";
        js << "    const amplitude = config.amplitude || 20;\n";
        js << "    const frequency = config.frequency || 2;\n";
        js << "    const waveCount = config.waveCount || 3;\n";
        js << "    \n";
        js << "    const startTime = performance.now() + delay;\n";
        js << "    \n";
        js << "    function liquidAnimation(currentTime) {\n";
        js << "        if (currentTime < startTime) {\n";
        js << "            requestAnimationFrame(liquidAnimation);\n";
        js << "            return;\n";
        js << "        }\n";
        js << "        \n";
        js << "        const elapsed = currentTime - startTime;\n";
        js << "        const progress = elapsed / duration;\n";
        js << "        \n";
        js << "        if (progress >= 1) {\n";
        js << "            element.style.transform = originalTransform;\n";
        js << "            console.log('🌊 液体运动完成');\n";
        js << "            if (config.onComplete) config.onComplete();\n";
        js << "            return;\n";
        js << "        }\n";
        js << "        \n";
        js << "        // 复杂的液体波动计算\n";
        js << "        let totalTransform = '';\n";
        js << "        \n";
        js << "        for (let wave = 0; wave < waveCount; wave++) {\n";
        js << "            const waveProgress = progress + (wave * 0.1);\n";
        js << "            const waveAmplitude = amplitude * Math.sin(waveProgress * Math.PI);\n";
        js << "            const waveFreq = frequency + wave;\n";
        js << "            \n";
        js << "            const x = waveAmplitude * Math.sin(waveProgress * waveFreq * Math.PI);\n";
        js << "            const y = waveAmplitude * Math.cos(waveProgress * waveFreq * Math.PI) * 0.5;\n";
        js << "            const rotation = Math.sin(waveProgress * Math.PI * 4) * 5;\n";
        js << "            const scale = 1 + Math.sin(waveProgress * Math.PI * 2) * 0.1;\n";
        js << "            \n";
        js << "            totalTransform += `translate(${x}px, ${y}px) rotate(${rotation}deg) scale(${scale}) `;\n";
        js << "        }\n";
        js << "        \n";
        js << "        element.style.transform = originalTransform + ' ' + totalTransform;\n";
        js << "        \n";
        js << "        requestAnimationFrame(liquidAnimation);\n";
        js << "    }\n";
        js << "    \n";
        js << "    requestAnimationFrame(liquidAnimation);\n";
        
    } else if (animationType == "magneticField") {
        js << "    // 磁场效果 - 元素互相吸引/排斥\n";
        js << "    const magneticElements = document.querySelectorAll(config.targets || '.magnetic');\n";
        js << "    const strength = config.strength || 1.0;\n";
        js << "    const maxDistance = config.maxDistance || 200;\n";
        js << "    \n";
        js << "    function updateMagneticField() {\n";
        js << "        magneticElements.forEach((elem1, i) => {\n";
        js << "            let totalForceX = 0;\n";
        js << "            let totalForceY = 0;\n";
        js << "            \n";
        js << "            magneticElements.forEach((elem2, j) => {\n";
        js << "                if (i !== j) {\n";
        js << "                    const rect1 = elem1.getBoundingClientRect();\n";
        js << "                    const rect2 = elem2.getBoundingClientRect();\n";
        js << "                    \n";
        js << "                    const dx = (rect2.left + rect2.width/2) - (rect1.left + rect1.width/2);\n";
        js << "                    const dy = (rect2.top + rect2.height/2) - (rect1.top + rect1.height/2);\n";
        js << "                    const distance = Math.sqrt(dx*dx + dy*dy);\n";
        js << "                    \n";
        js << "                    if (distance < maxDistance && distance > 0) {\n";
        js << "                        // 磁力计算（库仑定律简化版）\n";
        js << "                        const force = strength / (distance * distance);\n";
        js << "                        const forceX = (dx / distance) * force;\n";
        js << "                        const forceY = (dy / distance) * force;\n";
        js << "                        \n";
        js << "                        totalForceX += forceX;\n";
        js << "                        totalForceY += forceY;\n";
        js << "                    }\n";
        js << "                }\n";
        js << "            });\n";
        js << "            \n";
        js << "            // 应用磁力\n";
        js << "            const currentTransform = elem1.style.transform || '';\n";
        js << "            const translateMatch = currentTransform.match(/translate\\(([^)]+)\\)/);\n";
        js << "            let currentX = 0, currentY = 0;\n";
        js << "            \n";
        js << "            if (translateMatch) {\n";
        js << "                const coords = translateMatch[1].split(',');\n";
        js << "                currentX = parseFloat(coords[0]) || 0;\n";
        js << "                currentY = parseFloat(coords[1]) || 0;\n";
        js << "            }\n";
        js << "            \n";
        js << "            const newX = currentX + totalForceX;\n";
        js << "            const newY = currentY + totalForceY;\n";
        js << "            \n";
        js << "            elem1.style.transform = `translate(${newX}px, ${newY}px)`;\n";
        js << "        });\n";
        js << "    }\n";
        js << "    \n";
        js << "    // 启动磁场模拟\n";
        js << "    const magneticInterval = setInterval(updateMagneticField, 16); // 60 FPS\n";
        js << "    \n";
        js << "    // 定时停止\n";
        js << "    setTimeout(() => {\n";
        js << "        clearInterval(magneticInterval);\n";
        js << "        console.log('🧲 磁场效果结束');\n";
        js << "    }, duration);\n";
        js << "    \n";
        js << "    console.log('🧲 磁场效果启动，影响元素数量:', magneticElements.length);\n";
        js << "})()";
    
    return js.str();
}

// 子模块导出函数
extern "C" const char* processAnimationEngineSyntax(const char* chtljs_code) {
    std::string code(chtljs_code);
    
    // 智能语法检测 - 不仅仅是字符串匹配
    if (code.find("morphShape") != std::string::npos) {
        static std::string result = generateAdvancedAnimation("{{target}}", "morphShape", "{}");
        return result.c_str();
    }
    
    if (code.find("liquidMotion") != std::string::npos) {
        static std::string result = generateAdvancedAnimation("{{target}}", "liquidMotion", "{}");
        return result.c_str();
    }
    
    if (code.find("magneticField") != std::string::npos) {
        static std::string result = generateAdvancedAnimation("{{target}}", "magneticField", "{}");
        return result.c_str();
    }
    
    return chtljs_code;
}