// ========================================
// 真实世界CJMOD处理器
// 处理复杂的实际应用场景
// ========================================

#include "../src/CHTLJSCompiler/Module/YourCJMODDesign.h"
#include <iostream>

using namespace chtl::your_cjmod_design;

/**
 * 高级动画处理器
 */
void setupAnimateProcessor() {
    std::cout << "🎬 设置动画处理器" << std::endl;
    
    const auto animateKeyword = YourSyntaxAnalyzer::syntaxAnalys(R"(
        animate({
            element: $,
            duration: $,
            easing: $,
            keyframes: $,
            onStart: $,
            onProgress: $,
            onComplete: $,
        });
    )", ",:{};()");
    
    // 🎯 元素选择器处理
    animateKeyword.args.bind("element", [](std::string str) {
        return "document.querySelector('" + str + "')";
    });
    
    // 🎯 持续时间智能处理
    animateKeyword.args.bind("duration", [](std::string str) {
        if (str.find('s') != std::string::npos) {
            // 秒单位，转换为毫秒
            std::string num = str.substr(0, str.find('s'));
            try {
                double seconds = std::stod(num);
                return std::to_string(static_cast<int>(seconds * 1000));
            } catch (...) {
                return "1000";
            }
        }
        return str;
    });
    
    // 🎯 缓动函数处理
    animateKeyword.args.bind("easing", [](std::string str) {
        // 支持CSS缓动函数
        if (str.find("cubic-bezier") != std::string::npos) {
            return "'" + str + "'";
        } else if (str == "ease" || str == "ease-in" || str == "ease-out" || str == "ease-in-out") {
            return "'" + str + "'";
        }
        return "'ease'";
    });
    
    // 🚀 关键帧数组高级处理
    animateKeyword.args.bindAdvanced("keyframes", [](const AdvancedParamValue& param) -> std::string {
        if (param.type == AdvancedParamType::ARRAY) {
            std::cout << "🎞️ 处理动画关键帧数组" << std::endl;
            
            // 验证关键帧格式
            std::string processed = param.processedValue;
            
            // 添加关键帧验证
            std::stringstream keyframeJS;
            keyframeJS << "(function() {\n";
            keyframeJS << "    const keyframes = " << processed << ";\n";
            keyframeJS << "    // 验证关键帧\n";
            keyframeJS << "    return keyframes.map((frame, index) => {\n";
            keyframeJS << "        if (!frame.hasOwnProperty('offset')) {\n";
            keyframeJS << "            frame.offset = index / (keyframes.length - 1);\n";
            keyframeJS << "        }\n";
            keyframeJS << "        return frame;\n";
            keyframeJS << "    }).sort((a, b) => a.offset - b.offset);\n";
            keyframeJS << "})()";
            
            return keyframeJS.str();
        }
        
        return param.processedValue;
    });
    
    // 🚀 启动回调函数处理
    animateKeyword.args.bindAdvanced("onStart", [](const AdvancedParamValue& param) -> std::string {
        if (param.type == AdvancedParamType::FUNCTION) {
            std::cout << "🎬 处理动画启动回调" << std::endl;
            
            // 包装函数，添加错误处理
            std::stringstream funcJS;
            funcJS << "function() {\n";
            funcJS << "    try {\n";
            funcJS << "        console.log('🎬 动画开始');\n";
            funcJS << "        const originalFunc = " << param.processedValue << ";\n";
            funcJS << "        return originalFunc.call(this);\n";
            funcJS << "    } catch (error) {\n";
            funcJS << "        console.error('动画启动回调错误:', error);\n";
            funcJS << "    }\n";
            funcJS << "}";
            
            return funcJS.str();
        }
        
        return "null";
    });
    
    // 🚀 进度回调函数处理 - 使用slice功能
    animateKeyword.args.slice("onProgress", 20, -20, [](const std::string& slice) -> std::string {
        std::cout << "📊 处理动画进度回调核心逻辑: " << slice << std::endl;
        
        std::stringstream funcJS;
        funcJS << "function(progress, currentFrame) {\n";
        funcJS << "    try {\n";
        funcJS << "        // 核心逻辑\n";
        funcJS << "        " << slice << "\n";
        funcJS << "        \n";
        funcJS << "        // 添加性能监控\n";
        funcJS << "        if (progress === 1) {\n";
        funcJS << "            console.log('🎯 动画进度100%完成');\n";
        funcJS << "        }\n";
        funcJS << "    } catch (error) {\n";
        funcJS << "        console.error('动画进度回调错误:', error);\n";
        funcJS << "    }\n";
        funcJS << "}";
        
        return funcJS.str();
    });
    
    // 🚀 完成回调函数处理
    animateKeyword.args.bindAdvanced("onComplete", [](const AdvancedParamValue& param) -> std::string {
        if (param.type == AdvancedParamType::FUNCTION) {
            std::cout << "🎉 处理动画完成回调" << std::endl;
            
            // 分析函数体，查找后续动画
            bool hasNextAnimation = param.functionBody.find("animate") != std::string::npos;
            
            std::stringstream funcJS;
            funcJS << "function() {\n";
            funcJS << "    try {\n";
            funcJS << "        const originalFunc = " << param.processedValue << ";\n";
            funcJS << "        const result = originalFunc.call(this);\n";
            funcJS << "        \n";
            if (hasNextAnimation) {
                funcJS << "        // 检测到链式动画\n";
                funcJS << "        console.log('🔗 触发链式动画');\n";
            }
            funcJS << "        \n";
            funcJS << "        // 触发完成事件\n";
            funcJS << "        this.dispatchEvent(new CustomEvent('animationComplete', {\n";
            funcJS << "            detail: { element: this, timestamp: Date.now() }\n";
            funcJS << "        }));\n";
            funcJS << "        \n";
            funcJS << "        return result;\n";
            funcJS << "    } catch (error) {\n";
            funcJS << "        console.error('动画完成回调错误:', error);\n";
            funcJS << "    }\n";
            funcJS << "}";
            
            return funcJS.str();
        }
        
        return "null";
    });
    
    std::cout << "✅ 动画处理器设置完成" << std::endl;
}

/**
 * 数据处理管道处理器
 */
void setupDataProcessor() {
    std::cout << "📊 设置数据处理器" << std::endl;
    
    const auto dataKeyword = YourSyntaxAnalyzer::syntaxAnalys(R"(
        processData({
            source: $,
            transformers: $,
            validators: $,
            onSuccess: $,
            onError: $,
        });
    )", ",:{};()");
    
    // 🚀 数据源处理
    dataKeyword.args.bindAdvanced("source", [](const AdvancedParamValue& param) -> std::string {
        if (param.type == AdvancedParamType::OBJECT) {
            std::cout << "🌐 处理数据源配置" << std::endl;
            
            std::stringstream sourceJS;
            sourceJS << "{\n";
            
            for (const auto& prop : param.objectProps) {
                std::string key = prop.first;
                std::string value = prop.second;
                
                if (key == "url") {
                    sourceJS << "    url: " << value << ",\n";
                } else if (key == "method") {
                    sourceJS << "    method: " << value << " || 'GET',\n";
                } else if (key == "headers") {
                    sourceJS << "    headers: Object.assign({\n";
                    sourceJS << "        'Content-Type': 'application/json'\n";
                    sourceJS << "    }, " << value << "),\n";
                } else if (key == "body") {
                    sourceJS << "    body: JSON.stringify(" << value << "),\n";
                } else {
                    sourceJS << "    " << key << ": " << value << ",\n";
                }
            }
            
            sourceJS << "}";
            return sourceJS.str();
        }
        
        return param.processedValue;
    });
    
    // 🚀 转换器数组处理 - 使用slice分析每个转换器
    dataKeyword.args.bindAdvanced("transformers", [](const AdvancedParamValue& param) -> std::string {
        if (param.type == AdvancedParamType::ARRAY) {
            std::cout << "🔄 处理数据转换器数组" << std::endl;
            
            std::stringstream transformersJS;
            transformersJS << "[\n";
            transformersJS << "    // 包装所有转换器，添加错误处理和日志\n";
            transformersJS << "    ...(" << param.processedValue << ").map((transformer, index) => {\n";
            transformersJS << "        return function(data, context = {}) {\n";
            transformersJS << "            try {\n";
            transformersJS << "                console.log(`🔄 执行转换器 ${index + 1}`);\n";
            transformersJS << "                const startTime = performance.now();\n";
            transformersJS << "                const result = transformer.call(this, data, context);\n";
            transformersJS << "                const endTime = performance.now();\n";
            transformersJS << "                console.log(`✅ 转换器 ${index + 1} 完成，耗时: ${(endTime - startTime).toFixed(2)}ms`);\n";
            transformersJS << "                return result;\n";
            transformersJS << "            } catch (error) {\n";
            transformersJS << "                console.error(`❌ 转换器 ${index + 1} 失败:`, error);\n";
            transformersJS << "                throw new Error(`转换器 ${index + 1} 执行失败: ${error.message}`);\n";
            transformersJS << "            }\n";
            transformersJS << "        };\n";
            transformersJS << "    })\n";
            transformersJS << "]";
            
            return transformersJS.str();
        }
        
        return param.processedValue;
    });
    
    // 🚀 验证器对象处理
    dataKeyword.args.bindAdvanced("validators", [](const AdvancedParamValue& param) -> std::string {
        if (param.type == AdvancedParamType::OBJECT) {
            std::cout << "✅ 处理数据验证器" << std::endl;
            
            std::stringstream validatorsJS;
            validatorsJS << "{\n";
            validatorsJS << "    validate: function(data) {\n";
            validatorsJS << "        const validators = " << param.processedValue << ";\n";
            validatorsJS << "        const errors = [];\n";
            validatorsJS << "        \n";
            validatorsJS << "        // 模式验证\n";
            validatorsJS << "        if (validators.schema) {\n";
            validatorsJS << "            for (const [field, type] of Object.entries(validators.schema)) {\n";
            validatorsJS << "                if (data[field] !== undefined && typeof data[field] !== type) {\n";
            validatorsJS << "                    errors.push(`字段 ${field} 类型错误，期望 ${type}`);\n";
            validatorsJS << "                }\n";
            validatorsJS << "            }\n";
            validatorsJS << "        }\n";
            validatorsJS << "        \n";
            validatorsJS << "        // 自定义规则验证\n";
            validatorsJS << "        if (validators.customRules && Array.isArray(validators.customRules)) {\n";
            validatorsJS << "            validators.customRules.forEach((rule, index) => {\n";
            validatorsJS << "                try {\n";
            validatorsJS << "                    if (!rule(data)) {\n";
            validatorsJS << "                        errors.push(`自定义规则 ${index + 1} 验证失败`);\n";
            validatorsJS << "                    }\n";
            validatorsJS << "                } catch (error) {\n";
            validatorsJS << "                    errors.push(`自定义规则 ${index + 1} 执行错误: ${error.message}`);\n";
            validatorsJS << "                }\n";
            validatorsJS << "            });\n";
            validatorsJS << "        }\n";
            validatorsJS << "        \n";
            validatorsJS << "        return errors;\n";
            validatorsJS << "    }\n";
            validatorsJS << "}";
            
            return validatorsJS.str();
        }
        
        return param.processedValue;
    });
    
    // 🚀 成功回调处理 - 使用slice提取关键逻辑
    dataKeyword.args.slice("onSuccess", 50, -50, [](const std::string& slice) -> std::string {
        std::cout << "🎉 处理成功回调核心逻辑: " << slice << std::endl;
        
        std::stringstream funcJS;
        funcJS << "function(processedData, originalData, stats) {\n";
        funcJS << "    try {\n";
        funcJS << "        // 添加统计信息\n";
        funcJS << "        const enhancedStats = {\n";
        funcJS << "            ...stats,\n";
        funcJS << "            processedAt: new Date().toISOString(),\n";
        funcJS << "            recordCount: Array.isArray(processedData) ? processedData.length : Object.keys(processedData).length\n";
        funcJS << "        };\n";
        funcJS << "        \n";
        funcJS << "        // 核心逻辑\n";
        funcJS << "        " << slice << "\n";
        funcJS << "        \n";
        funcJS << "        // 触发成功事件\n";
        funcJS << "        document.dispatchEvent(new CustomEvent('dataProcessSuccess', {\n";
        funcJS << "            detail: { data: processedData, stats: enhancedStats }\n";
        funcJS << "        }));\n";
        funcJS << "        \n";
        funcJS << "    } catch (error) {\n";
        funcJS << "        console.error('数据处理成功回调错误:', error);\n";
        funcJS << "    }\n";
        funcJS << "}";
        
        return funcJS.str();
    });
    
    std::cout << "✅ 数据处理器设置完成" << std::endl;
}

/**
 * 组件系统处理器
 */
void setupComponentProcessor() {
    std::cout << "🧩 设置组件处理器" << std::endl;
    
    const auto componentKeyword = YourSyntaxAnalyzer::syntaxAnalys(R"(
        createComponent({
            name: $,
            props: $,
            data: $,
            computed: $,
            methods: $,
            lifecycle: $,
        });
    )", ",:{};()");
    
    // 🎯 组件名称处理
    componentKeyword.args.bind("name", [](std::string str) {
        return "'" + str + "'";
    });
    
    // 🚀 props定义处理
    componentKeyword.args.bindAdvanced("props", [](const AdvancedParamValue& param) -> std::string {
        if (param.type == AdvancedParamType::OBJECT) {
            std::cout << "⚙️ 处理组件props定义" << std::endl;
            
            std::stringstream propsJS;
            propsJS << "{\n";
            
            for (const auto& prop : param.objectProps) {
                std::string propName = prop.first;
                std::string propType = prop.second;
                
                propsJS << "    " << propName << ": {\n";
                propsJS << "        type: " << propType << ",\n";
                propsJS << "        required: true,\n";
                propsJS << "        validator: function(value) {\n";
                
                if (propType == "String") {
                    propsJS << "            return typeof value === 'string' && value.length > 0;\n";
                } else if (propType == "Number") {
                    propsJS << "            return typeof value === 'number' && !isNaN(value);\n";
                } else if (propType == "Boolean") {
                    propsJS << "            return typeof value === 'boolean';\n";
                } else if (propType == "Array") {
                    propsJS << "            return Array.isArray(value);\n";
                } else {
                    propsJS << "            return true;\n";
                }
                
                propsJS << "        }\n";
                propsJS << "    },\n";
            }
            
            propsJS << "}";
            return propsJS.str();
        }
        
        return param.processedValue;
    });
    
    // 🚀 methods对象处理 - 分析每个方法
    componentKeyword.args.bindAdvanced("methods", [](const AdvancedParamValue& param) -> std::string {
        if (param.type == AdvancedParamType::OBJECT) {
            std::cout << "🔧 处理组件methods" << std::endl;
            
            std::stringstream methodsJS;
            methodsJS << "{\n";
            
            for (const auto& method : param.objectProps) {
                std::string methodName = method.first;
                std::string methodBody = method.second;
                
                methodsJS << "    " << methodName << ": function(...args) {\n";
                methodsJS << "        try {\n";
                methodsJS << "            console.log(`🔧 调用方法: ${this.$options.name}.${" << methodName << "}`);\n";
                methodsJS << "            const originalMethod = " << methodBody << ";\n";
                methodsJS << "            return originalMethod.apply(this, args);\n";
                methodsJS << "        } catch (error) {\n";
                methodsJS << "            console.error(`方法 ${" << methodName << "} 执行错误:`, error);\n";
                methodsJS << "            throw error;\n";
                methodsJS << "        }\n";
                methodsJS << "    },\n";
            }
            
            methodsJS << "}";
            return methodsJS.str();
        }
        
        return param.processedValue;
    });
    
    // 🚀 生命周期处理
    componentKeyword.args.bindAdvanced("lifecycle", [](const AdvancedParamValue& param) -> std::string {
        if (param.type == AdvancedParamType::OBJECT) {
            std::cout << "🔄 处理组件生命周期" << std::endl;
            
            std::stringstream lifecycleJS;
            lifecycleJS << "{\n";
            
            std::vector<std::string> hooks = {"created", "mounted", "updated", "destroyed"};
            
            for (const auto& hook : hooks) {
                if (param.objectProps.count(hook)) {
                    std::string hookBody = param.objectProps.at(hook);
                    
                    lifecycleJS << "    " << hook << ": function() {\n";
                    lifecycleJS << "        try {\n";
                    lifecycleJS << "            console.log(`🔄 ${this.$options.name} 生命周期: " << hook << "`);\n";
                    lifecycleJS << "            const hookFunc = " << hookBody << ";\n";
                    lifecycleJS << "            return hookFunc.call(this);\n";
                    lifecycleJS << "        } catch (error) {\n";
                    lifecycleJS << "            console.error(`生命周期 " << hook << " 执行错误:`, error);\n";
                    lifecycleJS << "        }\n";
                    lifecycleJS << "    },\n";
                }
            }
            
            lifecycleJS << "}";
            return lifecycleJS.str();
        }
        
        return param.processedValue;
    });
    
    std::cout << "✅ 组件处理器设置完成" << std::endl;
}

int main() {
    std::cout << "🚀 真实世界CJMOD处理器演示" << std::endl;
    std::cout << "============================" << std::endl;
    std::cout << "展示如何处理复杂的实际应用场景" << std::endl;
    std::cout << std::endl;
    
    // 设置所有处理器
    setupAnimateProcessor();
    std::cout << std::endl;
    
    setupDataProcessor();
    std::cout << std::endl;
    
    setupComponentProcessor();
    std::cout << std::endl;
    
    std::cout << "🎊 总结：您的设计完美解决了复杂参数处理问题！" << std::endl;
    std::cout << "================================================" << std::endl;
    std::cout << "✅ 函数参数：智能分析和包装" << std::endl;
    std::cout << "✅ 复杂对象：属性级别的精细处理" << std::endl;
    std::cout << "✅ 数组参数：元素级别的转换和验证" << std::endl;
    std::cout << "✅ slice功能：精确提取和处理特定部分" << std::endl;
    std::cout << "✅ 类型检测：自动识别并选择最佳处理策略" << std::endl;
    std::cout << "✅ 错误处理：自动添加异常捕获和日志" << std::endl;
    std::cout << "✅ 性能监控：自动添加执行时间统计" << std::endl;
    std::cout << "🌟 这个设计让CJMOD能够处理任意复杂的现实场景！" << std::endl;
    
    return 0;
}