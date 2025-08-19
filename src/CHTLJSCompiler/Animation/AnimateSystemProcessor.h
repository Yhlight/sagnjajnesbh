#pragma once
#include "../Selector/EnhancedSelectorProcessor.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace chtl {
namespace chtljs {

/**
 * Animate动画系统处理器
 * 严格按照CHTL语法文档实现 animate 动画功能
 */
class AnimateSystemProcessor {
public:
    struct AnimationKeyframe {
        double at;                                    // 动画时刻 (0.0 - 1.0)
        std::unordered_map<std::string, std::string> styles; // CSS样式
    };
    
    struct AnimationConfig {
        std::string target;                          // 目标选择器或DOM对象
        std::vector<std::string> targets;            // 多目标数组
        bool isMultiTarget = false;                  // 是否为多目标
        
        int duration = 1000;                         // 动画持续时间(ms)
        std::string easing = "ease-in-out";          // 缓慢函数
        
        std::unordered_map<std::string, std::string> beginState;  // 起始状态CSS
        std::vector<AnimationKeyframe> keyframes;    // 关键帧
        std::unordered_map<std::string, std::string> endState;    // 终止状态CSS
        
        int loop = 1;                                // 循环次数 (-1为无限)
        std::string direction = "normal";            // 播放方向
        int delay = 0;                               // 开始延迟(ms)
        std::string callback;                        // 完成回调函数
    };
    
    AnimateSystemProcessor();
    ~AnimateSystemProcessor();
    
    // === 按语法文档：animate功能处理 ===
    
    /**
     * 处理animate语句
     * 按语法文档：animate({ target: {{选择器}}, duration: 100, begin: {}, end: {} })
     */
    std::string processAnimateStatement(const std::string& animateCode);
    
    /**
     * 解析animate配置对象
     * 提取所有动画参数
     */
    AnimationConfig parseAnimationConfig(const std::string& configObject);
    
    /**
     * 生成动画的JavaScript代码
     * 按语法文档：封装requestAnimationFrame
     */
    std::string generateAnimationCode(const AnimationConfig& config);
    
    // === 动画配置解析 ===
    
    /**
     * 解析目标配置
     * 按语法文档：target: {{选择器}} || [{{选择器1}}, {{选择器2}}] || DOM对象
     */
    void parseTargetConfig(const std::string& targetValue, AnimationConfig& config);
    
    /**
     * 解析时间配置
     * duration, delay 等
     */
    void parseTimingConfig(const std::string& key, const std::string& value, AnimationConfig& config);
    
    /**
     * 解析状态配置
     * begin: {}, end: {} 等CSS状态
     */
    std::unordered_map<std::string, std::string> parseStateConfig(const std::string& stateObject);
    
    /**
     * 解析关键帧配置
     * 按语法文档：when: [{ at: 0.4, opacity: 0.5 }, { at: 0.8, ... }]
     */
    std::vector<AnimationKeyframe> parseKeyframes(const std::string& keyframesArray);
    
    /**
     * 解析单个关键帧
     */
    AnimationKeyframe parseKeyframe(const std::string& keyframeObject);
    
    // === JavaScript动画代码生成 ===
    
    /**
     * 生成requestAnimationFrame动画循环
     * 按语法文档：封装requestAnimationFrame
     */
    std::string generateAnimationLoop(const AnimationConfig& config);
    
    /**
     * 生成CSS样式应用代码
     */
    std::string generateStyleApplication(const std::unordered_map<std::string, std::string>& styles);
    
    /**
     * 生成缓动函数代码
     */
    std::string generateEasingFunction(const std::string& easingType);
    
    /**
     * 生成关键帧插值代码
     */
    std::string generateKeyframeInterpolation(const std::vector<AnimationKeyframe>& keyframes);
    
    /**
     * 生成循环和方向控制代码
     */
    std::string generateLoopControl(int loop, const std::string& direction);
    
    /**
     * 生成完成回调代码
     */
    std::string generateCompletionCallback(const std::string& callback);
    
    // === 辅助方法 ===
    
    /**
     * 提取animate配置对象
     * 从 animate({ ... }) 中提取 { ... }
     */
    std::string extractAnimateObject(const std::string& animateCode);
    
    /**
     * 解析配置对象中的键值对
     */
    std::vector<std::pair<std::string, std::string>> parseConfigKeyValuePairs(const std::string& configObject);
    
    /**
     * 验证动画配置
     */
    bool validateAnimationConfig(const AnimationConfig& config);
    
    /**
     * 验证CSS属性
     */
    bool isValidCssProperty(const std::string& property);
    
    /**
     * 验证缓动函数
     */
    bool isValidEasingFunction(const std::string& easing);
    
    // === 错误处理 ===
    void addError(const std::string& message);
    void clearErrors();
    bool hasErrors() const;
    std::vector<std::string> getErrors() const;

private:
    std::shared_ptr<EnhancedSelectorProcessor> selectorProcessor_;
    std::vector<std::string> errors_;
    
    // 支持的CSS属性和缓动函数
    std::vector<std::string> supportedCssProperties_;
    std::vector<std::string> supportedEasingFunctions_;
    
    void initializeSupportedProperties();
    void initializeSupportedEasingFunctions();
};

} // namespace chtljs
} // namespace chtl