#include "AnimateSystemProcessor.h"
#include <iostream>
#include <sstream>
#include <regex>
#include <algorithm>

namespace chtl {
namespace chtljs {

AnimateSystemProcessor::AnimateSystemProcessor() = default;
AnimateSystemProcessor::~AnimateSystemProcessor() = default;

// === 按语法文档：animate功能处理 ===

std::string AnimateSystemProcessor::processAnimateStatement(const std::string& animateCode) {
    // 解析animate语句并生成JavaScript代码
    
    AnimationConfig config;
    if (!parseAnimateConfig(animateCode, config)) {
        return "// animate解析失败: " + animateCode;
    }
    
    return generateAnimationCode(config);
}

bool AnimateSystemProcessor::parseAnimateConfig(const std::string& code, AnimationConfig& config) {
    // 基本的animate配置解析
    // 这是一个简化实现，完整版本需要更复杂的解析逻辑
    
    // 设置默认值
    config.duration = 1000;
    config.easing = "ease-in-out";
    config.loop = 1;
    config.direction = "normal";
    config.delay = 0;
    
    // 解析target
    std::regex targetRegex(R"(target\s*:\s*\{\{([^}]+)\}\})");
    std::smatch targetMatch;
    if (std::regex_search(code, targetMatch, targetRegex)) {
        config.target = targetMatch[1].str();
    }
    
    // 解析duration
    std::regex durationRegex(R"(duration\s*:\s*(\d+))");
    std::smatch durationMatch;
    if (std::regex_search(code, durationMatch, durationRegex)) {
        config.duration = std::stoi(durationMatch[1].str());
    }
    
    // 解析easing（支持无修饰字面量）
    std::regex easingRegex(R"(easing\s*:\s*([a-zA-Z-]+))");
    std::smatch easingMatch;
    if (std::regex_search(code, easingMatch, easingRegex)) {
        config.easing = easingMatch[1].str();
    }
    
    return true;
}

std::string AnimateSystemProcessor::generateAnimationCode(const AnimationConfig& config) {
    std::ostringstream code;
    
    code << "// CHTL JS animate动画系统\n";
    code << "(function() {\n";
    code << "  const target = document.querySelector('" << config.target << "');\n";
    code << "  if (!target) {\n";
    code << "    console.warn('animate目标未找到: " << config.target << "');\n";
    code << "    return;\n";
    code << "  }\n\n";
    
    // 生成动画配置
    code << "  const animationConfig = {\n";
    code << "    duration: " << config.duration << ",\n";
    code << "    easing: '" << config.easing << "',\n";
    code << "    direction: '" << config.direction << "',\n";
    code << "    iterations: " << (config.loop == -1 ? "Infinity" : std::to_string(config.loop)) << ",\n";
    code << "    delay: " << config.delay << "\n";
    code << "  };\n\n";
    
    // 生成关键帧
    if (!config.keyframes.empty()) {
        code << "  const keyframes = [\n";
        for (size_t i = 0; i < config.keyframes.size(); ++i) {
            const auto& frame = config.keyframes[i];
            code << "    { offset: " << frame.at << ", ";
            
            bool first = true;
            for (const auto& [prop, value] : frame.styles) {
                if (!first) code << ", ";
                code << prop << ": '" << value << "'";
                first = false;
            }
            code << " }";
            if (i < config.keyframes.size() - 1) code << ",";
            code << "\n";
        }
        code << "  ];\n\n";
        
        // 使用Web Animations API
        code << "  const animation = target.animate(keyframes, animationConfig);\n";
    } else {
        // 简单的开始到结束动画
        code << "  const keyframes = [{}"; // 开始帧
        if (!config.beginState.empty()) {
            code << ", { ";
            bool first = true;
            for (const auto& [prop, value] : config.beginState) {
                if (!first) code << ", ";
                code << prop << ": '" << value << "'";
                first = false;
            }
            code << " }";
        }
        if (!config.endState.empty()) {
            code << ", { ";
            bool first = true;
            for (const auto& [prop, value] : config.endState) {
                if (!first) code << ", ";
                code << prop << ": '" << value << "'";
                first = false;
            }
            code << " }";
        }
        code << "];\n\n";
        
        code << "  const animation = target.animate(keyframes, animationConfig);\n";
    }
    
    // 添加完成回调
    if (!config.callback.empty()) {
        code << "\n  animation.addEventListener('finish', function() {\n";
        code << "    (" << config.callback << ")();\n";
        code << "  });\n";
    }
    
    code << "})();\n";
    
    return code.str();
}

// === 多目标动画处理 ===

std::string AnimateSystemProcessor::processMultiTargetAnimation(const AnimationConfig& config) {
    std::ostringstream code;
    
    code << "// CHTL JS 多目标动画\n";
    code << "(function() {\n";
    code << "  const targets = [";
    
    for (size_t i = 0; i < config.targets.size(); ++i) {
        code << "document.querySelector('" << config.targets[i] << "')";
        if (i < config.targets.size() - 1) code << ", ";
    }
    
    code << "];\n";
    code << "  targets.forEach(function(target, index) {\n";
    code << "    if (target) {\n";
    code << "      // 为每个目标创建动画\n";
    code << "      " << generateAnimationCode(config) << "\n";
    code << "    }\n";
    code << "  });\n";
    code << "})();\n";
    
    return code.str();
}

// === requestAnimationFrame封装 ===

std::string AnimateSystemProcessor::generateRequestAnimationFrameCode(const std::string& animationFunction) {
    std::ostringstream code;
    
    code << "// CHTL JS requestAnimationFrame封装\n";
    code << "(function() {\n";
    code << "  function chtlAnimate() {\n";
    code << "    " << animationFunction << "\n";
    code << "    requestAnimationFrame(chtlAnimate);\n";
    code << "  }\n";
    code << "  requestAnimationFrame(chtlAnimate);\n";
    code << "})();\n";
    
    return code.str();
}

// === 动画预设处理 ===

std::string AnimateSystemProcessor::applyAnimationPreset(const std::string& presetName, const AnimationConfig& config) {
    AnimationConfig presetConfig = config;
    
    if (presetName == "fadeIn") {
        presetConfig.beginState["opacity"] = "0";
        presetConfig.endState["opacity"] = "1";
    } else if (presetName == "fadeOut") {
        presetConfig.beginState["opacity"] = "1";
        presetConfig.endState["opacity"] = "0";
    } else if (presetName == "slideDown") {
        presetConfig.beginState["transform"] = "translateY(-100%)";
        presetConfig.endState["transform"] = "translateY(0)";
    } else if (presetName == "slideUp") {
        presetConfig.beginState["transform"] = "translateY(100%)";
        presetConfig.endState["transform"] = "translateY(0)";
    } else if (presetName == "scaleIn") {
        presetConfig.beginState["transform"] = "scale(0)";
        presetConfig.endState["transform"] = "scale(1)";
    } else if (presetName == "scaleOut") {
        presetConfig.beginState["transform"] = "scale(1)";
        presetConfig.endState["transform"] = "scale(0)";
    }
    
    return generateAnimationCode(presetConfig);
}

// === 缓慢函数支持 ===

std::string AnimateSystemProcessor::validateEasingFunction(const std::string& easing) {
    // 支持的缓慢函数
    std::vector<std::string> supportedEasing = {
        "ease", "ease-in", "ease-out", "ease-in-out", "linear",
        "cubic-bezier"
    };
    
    for (const auto& supported : supportedEasing) {
        if (easing.find(supported) != std::string::npos) {
            return easing;
        }
    }
    
    return "ease-in-out"; // 默认值
}

// === 动画控制方法 ===

std::string AnimateSystemProcessor::generateAnimationControls(const std::string& animationId) {
    std::ostringstream code;
    
    code << "// CHTL JS 动画控制方法\n";
    code << "window.chtlAnimations = window.chtlAnimations || {};\n";
    code << "window.chtlAnimations['" << animationId << "'] = {\n";
    code << "  play: function() { this.animation && this.animation.play(); },\n";
    code << "  pause: function() { this.animation && this.animation.pause(); },\n";
    code << "  cancel: function() { this.animation && this.animation.cancel(); },\n";
    code << "  reverse: function() { this.animation && this.animation.reverse(); },\n";
    code << "  finish: function() { this.animation && this.animation.finish(); }\n";
    code << "};\n";
    
    return code.str();
}

// === 性能优化 ===

std::string AnimateSystemProcessor::optimizeAnimationPerformance(const AnimationConfig& config) {
    std::ostringstream code;
    
    code << "// CHTL JS 动画性能优化\n";
    code << "(function() {\n";
    
    // 检查是否需要GPU加速
    bool needsGPUAcceleration = false;
    for (const auto& frame : config.keyframes) {
        for (const auto& [prop, value] : frame.styles) {
            if (prop == "transform" || prop == "opacity") {
                needsGPUAcceleration = true;
                break;
            }
        }
        if (needsGPUAcceleration) break;
    }
    
    if (needsGPUAcceleration) {
        code << "  // 启用GPU加速\n";
        code << "  const target = document.querySelector('" << config.target << "');\n";
        code << "  if (target) {\n";
        code << "    target.style.willChange = 'transform, opacity';\n";
        code << "  }\n\n";
    }
    
    code << "  // 动画完成后清理\n";
    code << "  animation.addEventListener('finish', function() {\n";
    if (needsGPUAcceleration) {
        code << "    target.style.willChange = 'auto';\n";
    }
    code << "    // 清理动画引用\n";
    code << "  });\n";
    
    code << "})();\n";
    
    return code.str();
}

// === 动画序列支持 ===

std::string AnimateSystemProcessor::generateAnimationSequence(const std::vector<AnimationConfig>& sequence) {
    std::ostringstream code;
    
    code << "// CHTL JS 动画序列\n";
    code << "(function() {\n";
    code << "  const animationSequence = [];\n\n";
    
    for (size_t i = 0; i < sequence.size(); ++i) {
        code << "  // 序列动画 " << (i + 1) << "\n";
        code << "  animationSequence.push(function() {\n";
        code << "    return new Promise(function(resolve) {\n";
        code << "      " << generateAnimationCode(sequence[i]) << "\n";
        code << "      setTimeout(resolve, " << sequence[i].duration << ");\n";
        code << "    });\n";
        code << "  });\n\n";
    }
    
    code << "  // 顺序执行动画序列\n";
    code << "  async function executeSequence() {\n";
    code << "    for (const animationFunc of animationSequence) {\n";
    code << "      await animationFunc();\n";
    code << "    }\n";
    code << "  }\n\n";
    code << "  executeSequence();\n";
    code << "})();\n";
    
    return code.str();
}

} // namespace chtljs
} // namespace chtl