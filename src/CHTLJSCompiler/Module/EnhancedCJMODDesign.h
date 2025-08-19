// ========================================
// 增强CJMOD设计 - 解决transform问题
// 提供多种transform实现方案
// ========================================

#pragma once
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <memory>

namespace enhanced_cjmod_design {

// ========================================
// 解决方案1: 延迟转换（推荐）
// ========================================

/**
 * 延迟转换器 - 在match时进行转换
 */
class DelayedTransformer {
public:
    using TransformFunc = std::function<std::string(const std::string&)>;
    
    void setTransform(const std::string& argName, TransformFunc transformer) {
        transformers_[argName] = transformer;
    }
    
    std::string applyTransform(const std::string& argName, const std::string& value) {
        auto it = transformers_.find(argName);
        if (it != transformers_.end()) {
            return it->second(value);
        }
        return value; // 无转换规则，返回原值
    }
    
private:
    std::unordered_map<std::string, TransformFunc> transformers_;
};

// ========================================
// 解决方案2: 模板转换
// ========================================

/**
 * 模板转换器 - 使用占位符模板
 */
class TemplateTransformer {
public:
    void setTemplate(const std::string& argName, const std::string& template_str) {
        templates_[argName] = template_str;
    }
    
    std::string applyTemplate(const std::string& argName, const std::string& value) {
        auto it = templates_.find(argName);
        if (it != templates_.end()) {
            std::string result = it->second;
            // 替换占位符
            size_t pos = result.find("${VALUE}");
            if (pos != std::string::npos) {
                result.replace(pos, 8, value);
            }
            return result;
        }
        return value;
    }
    
private:
    std::unordered_map<std::string, std::string> templates_;
};

// ========================================
// 解决方案3: 条件转换
// ========================================

/**
 * 条件转换器 - 根据值的类型进行不同转换
 */
class ConditionalTransformer {
public:
    struct TransformRule {
        std::function<bool(const std::string&)> condition;
        std::function<std::string(const std::string&)> transformer;
        std::string description;
    };
    
    void addRule(const std::string& argName, const TransformRule& rule) {
        rules_[argName].push_back(rule);
    }
    
    std::string applyConditionalTransform(const std::string& argName, const std::string& value) {
        auto it = rules_.find(argName);
        if (it != rules_.end()) {
            for (const auto& rule : it->second) {
                if (rule.condition(value)) {
                    return rule.transformer(value);
                }
            }
        }
        return value;
    }
    
private:
    std::unordered_map<std::string, std::vector<TransformRule>> rules_;
};

// ========================================
// 增强的Arg类 - 集成所有转换方案
// ========================================

class EnhancedArg {
public:
    std::string name;
    std::string placeholder; // $ 或实际内容
    bool isPlaceholder;
    
    // 绑定函数
    std::function<std::string(const std::string&)> bindFunc;
    
    // 转换规则
    DelayedTransformer delayedTransformer;
    TemplateTransformer templateTransformer;
    ConditionalTransformer conditionalTransformer;
    
    EnhancedArg(const std::string& n, const std::string& p) 
        : name(n), placeholder(p), isPlaceholder(p == "$") {}
    
    // ========================================
    // 方案1: 延迟转换（推荐使用）
    // ========================================
    
    /**
     * 设置延迟转换函数
     */
    void transform(std::function<std::string(const std::string&)> transformer) {
        delayedTransformer.setTransform(name, transformer);
    }
    
    /**
     * 在match时应用转换
     */
    std::string matchWithTransform(const std::string& value) {
        std::string processedValue = value;
        
        // 1. 先应用bind函数（如果有）
        if (bindFunc) {
            processedValue = bindFunc(processedValue);
        }
        
        // 2. 再应用transform（关键！）
        processedValue = delayedTransformer.applyTransform(name, processedValue);
        
        return processedValue;
    }
    
    // ========================================
    // 方案2: 模板转换
    // ========================================
    
    /**
     * 设置转换模板
     */
    void transformTemplate(const std::string& template_str) {
        templateTransformer.setTemplate(name, template_str);
    }
    
    /**
     * 应用模板转换
     */
    std::string matchWithTemplate(const std::string& value) {
        std::string processedValue = bindFunc ? bindFunc(value) : value;
        return templateTransformer.applyTemplate(name, processedValue);
    }
    
    // ========================================
    // 方案3: 条件转换
    // ========================================
    
    /**
     * 添加条件转换规则
     */
    void transformIf(std::function<bool(const std::string&)> condition, 
                     std::function<std::string(const std::string&)> transformer,
                     const std::string& description = "") {
        ConditionalTransformer::TransformRule rule;
        rule.condition = condition;
        rule.transformer = transformer;
        rule.description = description;
        conditionalTransformer.addRule(name, rule);
    }
    
    /**
     * 应用条件转换
     */
    std::string matchWithCondition(const std::string& value) {
        std::string processedValue = bindFunc ? bindFunc(value) : value;
        return conditionalTransformer.applyConditionalTransform(name, processedValue);
    }
    
    // ========================================
    // 统一的match接口
    // ========================================
    
    /**
     * 智能match - 自动选择最佳转换方式
     */
    std::string smartMatch(const std::string& value) {
        // 优先级：条件转换 > 延迟转换 > 模板转换 > bind函数 > 原值
        
        std::string result = conditionalTransformer.applyConditionalTransform(name, value);
        if (result != value) return result;
        
        result = delayedTransformer.applyTransform(name, value);
        if (result != value) return result;
        
        result = templateTransformer.applyTemplate(name, value);
        if (result != value) return result;
        
        if (bindFunc) {
            return bindFunc(value);
        }
        
        return value;
    }
};

// ========================================
// 增强的ArgsManager
// ========================================

class EnhancedArgsManager {
public:
    std::vector<EnhancedArg> args;
    
    // 索引访问
    EnhancedArg& operator[](int index) {
        return args[index];
    }
    
    // 名称访问
    EnhancedArg* find(const std::string& name) {
        for (auto& arg : args) {
            if (arg.name == name) {
                return &arg;
            }
        }
        return nullptr;
    }
    
    // ========================================
    // 解决transform问题的关键方法
    // ========================================
    
    /**
     * 方案1: 延迟转换绑定
     */
    void bind(const std::string& name, std::function<std::string(const std::string&)> processor) {
        auto* arg = find(name);
        if (arg) {
            arg->bindFunc = processor;
        }
    }
    
    /**
     * 方案1: 延迟转换设置
     */
    void transform(const std::string& name, std::function<std::string(const std::string&)> transformer) {
        auto* arg = find(name);
        if (arg) {
            arg->transform(transformer);
        }
    }
    
    /**
     * 方案2: 模板转换
     */
    void transformTemplate(const std::string& name, const std::string& template_str) {
        auto* arg = find(name);
        if (arg) {
            arg->transformTemplate(template_str);
        }
    }
    
    /**
     * 方案3: 条件转换
     */
    void transformIf(const std::string& name, 
                     std::function<bool(const std::string&)> condition,
                     std::function<std::string(const std::string&)> transformer) {
        auto* arg = find(name);
        if (arg) {
            arg->transformIf(condition, transformer);
        }
    }
    
    /**
     * 智能match - 在获取到值时进行转换
     */
    void match(const std::string& name, const std::string& value) {
        auto* arg = find(name);
        if (arg) {
            std::string transformedValue = arg->smartMatch(value);
            // 存储转换后的值
            matchedValues_[name] = transformedValue;
        }
    }
    
    /**
     * 获取转换后的结果
     */
    std::string result() {
        std::string code = "";
        for (const auto& arg : args) {
            auto it = matchedValues_.find(arg.name);
            if (it != matchedValues_.end()) {
                code += it->second + " ";
            } else {
                code += arg.placeholder + " ";
            }
        }
        return code;
    }
    
    int length() const {
        return args.size();
    }

private:
    std::unordered_map<std::string, std::string> matchedValues_;
};

// ========================================
// 使用示例 - 解决transform问题
// ========================================

/**
 * 示例：printMylove的完整实现
 */
void example_printMylove_implementation() {
    std::string str = ",:{};()";
    auto keyword = syntaxAnalys(R"(
        printMylove({
            url: $,
            mode: $,
            width: $,
            height: $,
            scale: $,
        });
    )", str);
    
    // ========================================
    // 方案1: 延迟转换（推荐）
    // ========================================
    
    // 1. 先bind处理函数
    keyword.args.bind("url", [](const std::string& value) {
        return "\"" + value + "\""; // 确保是字符串
    });
    
    keyword.args.bind("mode", [](const std::string& value) {
        return "\"" + value + "\"";
    });
    
    // 2. 再设置transform转换规则
    keyword.args.transform("url", [](const std::string& processedValue) {
        return "const imageUrl = " + processedValue + ";";
    });
    
    keyword.args.transform("mode", [](const std::string& processedValue) {
        return "const renderMode = " + processedValue + ";";
    });
    
    keyword.args.transform("width", [](const std::string& value) {
        // 根据值的类型进行不同转换
        if (value.find("px") != std::string::npos) {
            return "const width = parseInt(" + value + ");";
        } else if (value.find("%") != std::string::npos) {
            return "const width = parseFloat(" + value + ") / 100 * window.innerWidth;";
        } else {
            return "const width = " + value + ";";
        }
    });
    
    // 3. 在match时自动应用所有转换
    // scanner.scanKeyword(keyword.args["printMylove"], []() {
    //     keyword.args.match("url", peekKeyword(1));     // 自动应用 bind + transform
    //     keyword.args.match("mode", peekKeyword(3));    // 自动应用 bind + transform
    //     keyword.args.match("width", peekKeyword(5));   // 自动应用 bind + transform
    // });
    
    // 4. 生成最终代码
    std::string finalCode = keyword.args.result();
}

// ========================================
// 方案2: 模板转换
// ========================================

void example_template_transform() {
    auto keyword = syntaxAnalys("myFunction( arg )");
    
    // 设置转换模板
    keyword.args.transformTemplate("arg", "const processedArg = ${VALUE};");
    
    // match时自动应用模板
    keyword.args.match("arg", "someValue");
    // 结果: "const processedArg = someValue;"
}

// ========================================
// 方案3: 条件转换
// ========================================

void example_conditional_transform() {
    auto keyword = syntaxAnalys("animate( target: $, duration: $ )");
    
    // 为target设置条件转换
    keyword.args.transformIf("target", 
        [](const std::string& value) { 
            return value.find("{{") != std::string::npos; // 是CHTL JS选择器
        },
        [](const std::string& value) {
            return "document.querySelector('" + value.substr(2, value.length()-4) + "')";
        }
    );
    
    keyword.args.transformIf("target",
        [](const std::string& value) { 
            return value[0] == '"'; // 是字符串
        },
        [](const std::string& value) {
            return "document.querySelector(" + value + ")";
        }
    );
    
    // 为duration设置条件转换
    keyword.args.transformIf("duration",
        [](const std::string& value) { 
            return std::isdigit(value[0]); // 是数字
        },
        [](const std::string& value) {
            return "parseInt(" + value + ")";
        }
    );
}

// ========================================
// 方案4: 智能推断转换
// ========================================

/**
 * 智能转换器 - 自动推断类型并转换
 */
class SmartTransformer {
public:
    static std::string autoTransform(const std::string& argName, const std::string& value) {
        // 自动推断值的类型并应用相应转换
        
        if (isFunction(value)) {
            return transformFunction(argName, value);
        } else if (isObject(value)) {
            return transformObject(argName, value);
        } else if (isCHTLJSSelector(value)) {
            return transformSelector(argName, value);
        } else if (isNumber(value)) {
            return transformNumber(argName, value);
        } else if (isString(value)) {
            return transformString(argName, value);
        }
        
        return value;
    }
    
private:
    static bool isFunction(const std::string& value) {
        return value.find("function") != std::string::npos || 
               value.find("=>") != std::string::npos ||
               value.find("() => {") != std::string::npos;
    }
    
    static bool isObject(const std::string& value) {
        return value.front() == '{' && value.back() == '}';
    }
    
    static bool isCHTLJSSelector(const std::string& value) {
        return value.find("{{") != std::string::npos && value.find("}}") != std::string::npos;
    }
    
    static bool isNumber(const std::string& value) {
        return std::all_of(value.begin(), value.end(), [](char c) {
            return std::isdigit(c) || c == '.' || c == '-';
        });
    }
    
    static bool isString(const std::string& value) {
        return (value.front() == '"' && value.back() == '"') ||
               (value.front() == '\'' && value.back() == '\'');
    }
    
    static std::string transformFunction(const std::string& argName, const std::string& value) {
        return "const " + argName + "Func = " + value + ";";
    }
    
    static std::string transformObject(const std::string& argName, const std::string& value) {
        return "const " + argName + "Obj = " + value + ";";
    }
    
    static std::string transformSelector(const std::string& argName, const std::string& value) {
        std::string selector = value.substr(2, value.length() - 4); // 去除{{}}
        return "const " + argName + "Element = document.querySelector('" + selector + "');";
    }
    
    static std::string transformNumber(const std::string& argName, const std::string& value) {
        return "const " + argName + "Value = " + value + ";";
    }
    
    static std::string transformString(const std::string& argName, const std::string& value) {
        return "const " + argName + "String = " + value + ";";
    }
};

// ========================================
// 最终推荐方案: 混合转换策略
// ========================================

/**
 * 混合转换器 - 结合所有方案的优点
 */
class HybridTransformer {
public:
    /**
     * 为您的transform问题提供完美解决方案
     */
    void setupTransform(EnhancedArg& arg, const std::string& transformType = "auto") {
        if (transformType == "auto") {
            // 自动智能转换
            arg.transform([&arg](const std::string& value) {
                return SmartTransformer::autoTransform(arg.name, value);
            });
        } else if (transformType == "template") {
            // 使用模板转换
            arg.transformTemplate("const " + arg.name + " = ${VALUE};");
        } else if (transformType == "custom") {
            // 使用自定义转换规则
            setupCustomTransform(arg);
        }
    }
    
private:
    void setupCustomTransform(EnhancedArg& arg) {
        // 为不同类型的值设置不同的转换规则
        arg.transformIf(
            [](const std::string& value) { return value.find("{{") != std::string::npos; },
            [&arg](const std::string& value) {
                return "const " + arg.name + " = document.querySelector('" + 
                       value.substr(2, value.length()-4) + "');";
            },
            "CHTL JS选择器转换"
        );
        
        arg.transformIf(
            [](const std::string& value) { return std::isdigit(value[0]); },
            [&arg](const std::string& value) {
                return "const " + arg.name + " = " + value + ";";
            },
            "数值转换"
        );
    }
};

// ========================================
// 使用示例 - 完美解决transform问题
// ========================================

void perfect_printMylove_example() {
    std::string str = ",:{};()";
    auto keyword = syntaxAnalys(R"(
        printMylove({
            url: $,
            mode: $,
            width: $,
            height: $,
            scale: $,
        });
    )", str);
    
    // 设置混合转换策略
    HybridTransformer transformer;
    
    // 为每个参数设置转换
    transformer.setupTransform(keyword.args[0], "auto");  // url - 自动转换
    transformer.setupTransform(keyword.args[1], "auto");  // mode - 自动转换
    transformer.setupTransform(keyword.args[2], "auto");  // width - 自动转换
    transformer.setupTransform(keyword.args[3], "auto");  // height - 自动转换
    transformer.setupTransform(keyword.args[4], "auto");  // scale - 自动转换
    
    // 在扫描器中使用
    scanner.scanKeyword(keyword.args["printMylove"], []() {
        // 获取值并自动转换
        std::string url = keyword.args[0].smartMatch(peekKeyword(1));
        std::string mode = keyword.args[1].smartMatch(peekKeyword(3));
        std::string width = keyword.args[2].smartMatch(peekKeyword(5));
        std::string height = keyword.args[3].smartMatch(peekKeyword(7));
        std::string scale = keyword.args[4].smartMatch(peekKeyword(9));
        
        // 或者批量处理
        keyword.args.match("url", peekKeyword(1));
        keyword.args.match("mode", peekKeyword(3));
        keyword.args.match("width", peekKeyword(5));
        keyword.args.match("height", peekKeyword(7));
        keyword.args.match("scale", peekKeyword(9));
    });
    
    // 生成最终代码
    std::string finalCode = generateCode(keyword);
}

} // namespace enhanced_cjmod_design

// ========================================
// 外部接口 - 兼容您的原始设计
// ========================================

using EnhancedKeyword = enhanced_cjmod_design::EnhancedArgsManager;

EnhancedKeyword syntaxAnalys(const std::string& pattern, const std::string& ignoreChars = "") {
    // TODO: 实现语法分析
    EnhancedKeyword keyword;
    return keyword;
}

std::string generateCode(const EnhancedKeyword& keyword) {
    return keyword.result();
}

// 辅助函数
bool isObject(const std::string& value) {
    return enhanced_cjmod_design::SmartTransformer::isObject(value);
}

bool isFunction(const std::string& value) {
    return enhanced_cjmod_design::SmartTransformer::isFunction(value);
}