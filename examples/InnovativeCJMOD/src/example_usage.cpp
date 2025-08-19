// ========================================
// 创新CJMOD示例 - 使用您的语法分析设计
// 展示syntaxAnalys + lambda处理函数的强大能力
// ========================================

#include "SyntaxAnalyzer.h"
#include <iostream>
#include <string>
#include <sstream>

using namespace chtl::cjmod;

/**
 * 示例1：粒子效果语法
 * CHTL JS: particleEffect({{#container}}, 100, 'fireworks')
 * 模式: "selector number string"
 */
void setupParticleEffectSyntax() {
    // 🚀 使用您的创新设计！
    const auto keyword = SyntaxAnalyzer::syntaxAnalys("selector number string");
    
    // 设置参数处理器
    keyword.arg[0] = [](const ArgValue& val) -> std::string {
        // 处理选择器参数 {{#container}}
        std::string selector = val.rawValue;
        if (selector.find("{{") != std::string::npos) {
            // 移除{{}}包装
            selector = selector.substr(2, selector.length() - 4);
            
            if (selector[0] == '#') {
                return "document.getElementById('" + selector.substr(1) + "')";
            } else if (selector[0] == '.') {
                return "document.querySelector('." + selector.substr(1) + "')";
            } else {
                return "document.querySelector('" + selector + "')";
            }
        }
        return "null";
    };
    
    keyword.arg[1] = [](int num) -> std::string {
        // 处理粒子数量参数
        if (num <= 0) num = 50; // 默认值
        if (num > 1000) num = 1000; // 最大限制
        
        return std::to_string(num);
    };
    
    keyword.arg[2] = [](const std::string& effectType) -> std::string {
        // 处理效果类型参数
        std::unordered_map<std::string, std::string> effectConfigs = {
            {"fireworks", "{ colors: ['#ff6b6b', '#4ecdc4', '#45b7d1'], gravity: 0.5, spread: 360 }"},
            {"rain", "{ colors: ['#74b9ff', '#0984e3'], gravity: 2.0, spread: 30 }"},
            {"snow", "{ colors: ['#ffffff', '#ddd'], gravity: 0.3, spread: 180 }"},
            {"explosion", "{ colors: ['#fd79a8', '#fdcb6e', '#e17055'], gravity: 1.0, spread: 360 }"}
        };
        
        return effectConfigs.count(effectType) ? effectConfigs[effectType] : effectConfigs["fireworks"];
    };
    
    // 生成代码
    std::string jsCode = CodeGenerator::generateCode(keyword);
    
    std::cout << "🎆 粒子效果语法生成的JavaScript代码：" << std::endl;
    std::cout << jsCode << std::endl;
}

/**
 * 示例2：智能表单验证语法
 * CHTL JS: smartValidate({{#form}}, **, callback)
 * 模式: "selector ** function"
 */
void setupSmartValidationSyntax() {
    // 🚀 使用通配符 ** 的强大功能
    const auto keyword = SyntaxAnalyzer::syntaxAnalys("selector ** function");
    
    keyword.arg[0] = [](const ArgValue& val) -> std::string {
        // 处理表单选择器
        return "document.querySelector('" + val.rawValue + "')";
    };
    
    keyword.arg[1] = []() -> std::string {
        // ** 通配符处理器 - 可以返回任意复杂的逻辑
        return R"({
            rules: {
                email: /^[^\s@]+@[^\s@]+\.[^\s@]+$/,
                phone: /^1[3-9]\d{9}$/,
                password: /.{8,}/,
                required: /.+/
            },
            messages: {
                email: '请输入有效的邮箱地址',
                phone: '请输入有效的手机号码', 
                password: '密码至少8位',
                required: '此字段为必填项'
            },
            validate: function(input, rule) {
                const value = input.value.trim();
                const isValid = this.rules[rule].test(value);
                
                // 视觉反馈
                input.style.borderColor = isValid ? '#4caf50' : '#f44336';
                input.style.boxShadow = isValid ? 
                    '0 0 5px rgba(76, 175, 80, 0.3)' : 
                    '0 0 5px rgba(244, 67, 54, 0.3)';
                
                // 显示消息
                let msgEl = input.nextElementSibling;
                if (!msgEl || !msgEl.classList.contains('validation-message')) {
                    msgEl = document.createElement('div');
                    msgEl.className = 'validation-message';
                    msgEl.style.cssText = 'font-size: 12px; margin-top: 5px;';
                    input.parentNode.insertBefore(msgEl, input.nextSibling);
                }
                
                msgEl.textContent = isValid ? '✅ 验证通过' : '❌ ' + this.messages[rule];
                msgEl.style.color = isValid ? '#4caf50' : '#f44336';
                
                return isValid;
            }
        })";
    };
    
    keyword.arg[2] = [](const ArgValue& val) -> std::string {
        // 处理回调函数
        return val.rawValue;
    };
    
    std::string jsCode = CodeGenerator::generateCode(keyword);
    
    std::cout << "✅ 智能验证语法生成的JavaScript代码：" << std::endl;
    std::cout << jsCode << std::endl;
}

/**
 * 示例3：复杂动画序列语法
 * CHTL JS: animationSequence(targets, **, duration)
 * 模式: "array ** number"
 */
void setupAnimationSequenceSyntax() {
    const auto keyword = SyntaxAnalyzer::syntaxAnalys("array ** number");
    
    keyword.arg[0] = [](const ArgValue& val) -> std::string {
        // 处理目标数组
        return "document.querySelectorAll('" + val.rawValue + "')";
    };
    
    keyword.arg[1] = []() -> std::string {
        // ** 通配符 - 复杂的动画配置
        return R"({
            sequence: [
                { 
                    transform: 'translateY(-20px) scale(1.1)', 
                    opacity: 0.8,
                    offset: 0 
                },
                { 
                    transform: 'translateY(0px) scale(1.05) rotate(5deg)', 
                    opacity: 1,
                    offset: 0.3 
                },
                { 
                    transform: 'translateY(5px) scale(1) rotate(-2deg)', 
                    opacity: 0.9,
                    offset: 0.7 
                },
                { 
                    transform: 'translateY(0px) scale(1) rotate(0deg)', 
                    opacity: 1,
                    offset: 1 
                }
            ],
            easing: 'cubic-bezier(0.68, -0.55, 0.265, 1.55)',
            fill: 'forwards',
            stagger: 100  // 每个元素延迟100ms
        })";
    };
    
    keyword.arg[2] = [](int duration) -> std::string {
        // 处理持续时间，确保合理范围
        if (duration < 100) duration = 100;
        if (duration > 10000) duration = 10000;
        
        return std::to_string(duration);
    };
    
    std::string jsCode = CodeGenerator::generateCode(keyword);
    
    std::cout << "🎬 动画序列语法生成的JavaScript代码：" << std::endl;
    std::cout << jsCode << std::endl;
}

/**
 * 示例4：AI对话语法
 * CHTL JS: aiChat(prompt, **, model)
 * 模式: "string ** string"
 */
void setupAIChatSyntax() {
    const auto keyword = SyntaxAnalyzer::syntaxAnalys("string ** string");
    
    keyword.arg[0] = [](const std::string& prompt) -> std::string {
        // 处理用户输入的提示词
        std::string safePrompt = prompt;
        // 转义特殊字符
        std::replace(safePrompt.begin(), safePrompt.end(), '"', '\'');
        return "\"" + safePrompt + "\"";
    };
    
    keyword.arg[1] = []() -> std::string {
        // ** 通配符 - 复杂的AI配置
        return R"({
            // AI模型配置
            models: {
                'gpt': {
                    endpoint: '/api/gpt',
                    maxTokens: 150,
                    temperature: 0.7,
                    systemPrompt: '你是一个友善的AI助手'
                },
                'claude': {
                    endpoint: '/api/claude',
                    maxTokens: 200,
                    temperature: 0.5,
                    systemPrompt: '你是一个专业的AI助手'
                }
            },
            
            // 响应处理
            processResponse: function(response, model) {
                // 添加打字机效果
                return new Promise((resolve) => {
                    let index = 0;
                    const text = response.text || response;
                    const targetElement = document.getElementById('ai-response');
                    
                    if (targetElement) {
                        targetElement.textContent = '';
                        
                        const typeInterval = setInterval(() => {
                            if (index < text.length) {
                                targetElement.textContent += text[index];
                                index++;
                            } else {
                                clearInterval(typeInterval);
                                resolve(response);
                            }
                        }, 50);
                    } else {
                        resolve(response);
                    }
                });
            },
            
            // 错误处理
            handleError: function(error) {
                console.error('🤖 AI对话错误:', error);
                return {
                    text: '抱歉，AI助手暂时无法回应。请稍后再试。',
                    error: true
                };
            }
        })";
    };
    
    keyword.arg[2] = [](const std::string& model) -> std::string {
        // 处理AI模型选择
        std::vector<std::string> supportedModels = {"gpt", "claude", "local"};
        
        if (std::find(supportedModels.begin(), supportedModels.end(), model) != supportedModels.end()) {
            return "\"" + model + "\"";
        } else {
            return "\"gpt\""; // 默认模型
        }
    };
    
    std::string jsCode = CodeGenerator::generateCode(keyword);
    
    std::cout << "🤖 AI对话语法生成的JavaScript代码：" << std::endl;
    std::cout << jsCode << std::endl;
}

/**
 * 主函数 - 演示您的创新语法分析设计
 */
int main() {
    std::cout << "🚀 创新CJMOD语法分析器演示" << std::endl;
    std::cout << "==============================" << std::endl;
    std::cout << "基于您的syntaxAnalys + lambda设计" << std::endl;
    std::cout << std::endl;
    
    // 演示各种语法
    std::cout << "📝 示例1：粒子效果语法" << std::endl;
    std::cout << "========================" << std::endl;
    setupParticleEffectSyntax();
    std::cout << std::endl;
    
    std::cout << "📝 示例2：智能表单验证语法" << std::endl;
    std::cout << "============================" << std::endl;
    setupSmartValidationSyntax();
    std::cout << std::endl;
    
    std::cout << "📝 示例3：复杂动画序列语法" << std::endl;
    std::cout << "============================" << std::endl;
    setupAnimationSequenceSyntax();
    std::cout << std::endl;
    
    std::cout << "📝 示例4：AI对话语法" << std::endl;
    std::cout << "===================" << std::endl;
    setupAIChatSyntax();
    std::cout << std::endl;
    
    std::cout << "🎉 演示完成！" << std::endl;
    std::cout << "✅ 您的设计让CJMOD开发变得极其简单和强大！" << std::endl;
    
    return 0;
}

// ========================================
// CJMOD导出函数 - 供CHTL编译器调用
// ========================================

extern "C" const char* processInnovativeCJMOD(const char* chtljs_code) {
    std::string code(chtljs_code);
    
    // 🎆 处理粒子效果语法
    if (code.find("particleEffect") != std::string::npos) {
        const auto keyword = SyntaxAnalyzer::syntaxAnalys("selector number string");
        
        // 应用到实际代码
        if (SyntaxAnalyzer::applySyntax(keyword, code)) {
            // 设置处理器
            keyword.arg[0] = [](const ArgValue& val) {
                return "document.querySelector('" + val.rawValue + "')";
            };
            keyword.arg[1] = [](int num) {
                return std::to_string(std::max(1, std::min(num, 1000)));
            };
            keyword.arg[2] = [](const std::string& type) {
                return "'" + type + "'";
            };
            
            static std::string result = CodeGenerator::generateCode(keyword);
            return result.c_str();
        }
    }
    
    // 🤖 处理AI对话语法
    if (code.find("aiChat") != std::string::npos) {
        const auto keyword = SyntaxAnalyzer::syntaxAnalys("string ** string");
        
        if (SyntaxAnalyzer::applySyntax(keyword, code)) {
            keyword.arg[0] = [](const std::string& prompt) {
                return "\"" + prompt + "\"";
            };
            keyword.arg[1] = []() {
                return "{ model: 'advanced', streaming: true, context: 'web_chat' }";
            };
            keyword.arg[2] = [](const std::string& model) {
                return "\"" + model + "\"";
            };
            
            static std::string result = CodeGenerator::generateCode(keyword);
            return result.c_str();
        }
    }
    
    // ✅ 处理表单验证语法
    if (code.find("smartValidate") != std::string::npos) {
        const auto keyword = SyntaxAnalyzer::syntaxAnalys("selector ** function");
        
        if (SyntaxAnalyzer::applySyntax(keyword, code)) {
            keyword.arg[0] = [](const ArgValue& val) {
                return "document.querySelector('" + val.rawValue + "')";
            };
            keyword.arg[1] = []() {
                return "{ rules: ['email', 'required'], realtime: true, showMessages: true }";
            };
            keyword.arg[2] = [](const ArgValue& val) {
                return val.rawValue; // 保持回调函数原样
            };
            
            static std::string result = CodeGenerator::generateCode(keyword);
            return result.c_str();
        }
    }
    
    return chtljs_code; // 不匹配就返回原代码
}