#pragma once

#include "PracticalCJMODApi.h"
#include "EnhancedCJMODApi.h"
#include "AutoFillCJMODApi.h"

namespace CHTL {
namespace CJMOD {

/**
 * @brief CJMOD API使用模式枚举
 */
enum class APIMode {
    PRACTICAL,   // 实用模式（默认）- 最接近原始API设计
    ENHANCED,    // 增强模式 - 支持链式调用的流畅API
    AUTOFILL     // 智能模式 - 自动填充和智能分析
};

/**
 * @brief CJMOD模式选择器和统一入口
 * 
 * 这个类作为三种API模式的统一入口，让用户可以选择最适合的使用方式：
 * 
 * 1. PRACTICAL模式（默认）：
 *    - 最接近您原始的API设计理念
 *    - bind()专门处理值，transform()专门转换JS，operator()处理peekKeyword
 *    - 代码简洁明了，性能最佳
 *    - 适合: 追求性能和明确控制的场景
 * 
 * 2. ENHANCED模式：
 *    - 在PRACTICAL基础上增加链式调用
 *    - 支持流畅的方法链，提升开发体验
 *    - 保持核心设计不变，只是语法更现代
 *    - 适合: 希望代码更流畅但保持控制力的场景
 * 
 * 3. AUTOFILL模式：
 *    - 智能自动填充和分析
 *    - 双指针扫描+前置截取的完整实现
 *    - 减少手动配置，智能推断参数
 *    - 适合: 快速原型和复杂扫描场景
 */
class CJMODModeSelector {
public:
    /**
     * @brief 获取当前默认API模式
     */
    static APIMode getDefaultMode() {
        return APIMode::PRACTICAL;  // 默认使用最接近原始API的实用模式
    }
    
    /**
     * @brief 设置全局默认API模式
     */
    static void setDefaultMode(APIMode mode) {
        defaultMode_ = mode;
    }
    
    /**
     * @brief 获取当前全局默认模式
     */
    static APIMode getCurrentDefaultMode() {
        return defaultMode_;
    }
    
    /**
     * @brief 创建指定模式的Syntax对象
     */
    template<APIMode Mode = APIMode::PRACTICAL>
    static auto createSyntax(const std::string& pattern, const std::string& ignoreChars = ",:{};()") {
        if constexpr (Mode == APIMode::PRACTICAL) {
            return Practical::syntaxAnalys(pattern, ignoreChars);
        } else if constexpr (Mode == APIMode::ENHANCED) {
            return Enhanced::syntaxAnalys(pattern, ignoreChars);
        } else if constexpr (Mode == APIMode::AUTOFILL) {
            return AutoFill::syntaxAnalys(pattern, ignoreChars);
        }
    }
    
    /**
     * @brief 创建指定模式的CHTLJSFunction对象
     */
    template<APIMode Mode = APIMode::PRACTICAL>
    static auto createFunction(const std::string& functionName, const std::vector<std::string>& paramNames = {}) {
        if constexpr (Mode == APIMode::PRACTICAL) {
            return std::make_unique<Practical::CHTLJSFunction>(functionName, paramNames);
        } else if constexpr (Mode == APIMode::ENHANCED) {
            return std::make_unique<Enhanced::CHTLJSFunction>(functionName, paramNames);
        } else if constexpr (Mode == APIMode::AUTOFILL) {
            return std::make_unique<AutoFill::CHTLJSFunction>(functionName, paramNames);
        }
    }

private:
    static APIMode defaultMode_;
};

// 静态成员定义
APIMode CJMODModeSelector::defaultMode_ = APIMode::PRACTICAL;

/**
 * @brief 便捷的模式特化命名空间
 */
namespace Practical = CHTL::CJMOD;  // 默认指向PracticalCJMODApi
namespace Enhanced {
    using namespace CHTL::CJMOD;
    // Enhanced API的类型别名（如果有命名冲突的话）
}
namespace AutoFill {
    using namespace CHTL::CJMOD;
    // AutoFill API的类型别名（如果有命名冲突的话）
}

/**
 * @brief 用户友好的宏定义，简化模式选择
 */
#define CJMOD_USE_PRACTICAL_MODE() using namespace CHTL::CJMOD::Practical;
#define CJMOD_USE_ENHANCED_MODE()  using namespace CHTL::CJMOD::Enhanced;
#define CJMOD_USE_AUTOFILL_MODE()  using namespace CHTL::CJMOD::AutoFill;

#define CJMOD_SET_DEFAULT_MODE(mode) CHTL::CJMOD::CJMODModeSelector::setDefaultMode(CHTL::CJMOD::APIMode::mode)

/**
 * @brief 通用工厂函数，根据默认模式创建对象
 */
template<typename... Args>
auto createSyntax(Args&&... args) {
    switch (CJMODModeSelector::getCurrentDefaultMode()) {
        case APIMode::PRACTICAL:
            return CJMODModeSelector::createSyntax<APIMode::PRACTICAL>(std::forward<Args>(args)...);
        case APIMode::ENHANCED:
            return CJMODModeSelector::createSyntax<APIMode::ENHANCED>(std::forward<Args>(args)...);
        case APIMode::AUTOFILL:
            return CJMODModeSelector::createSyntax<APIMode::AUTOFILL>(std::forward<Args>(args)...);
        default:
            return CJMODModeSelector::createSyntax<APIMode::PRACTICAL>(std::forward<Args>(args)...);
    }
}

template<typename... Args>
auto createFunction(Args&&... args) {
    switch (CJMODModeSelector::getCurrentDefaultMode()) {
        case APIMode::PRACTICAL:
            return CJMODModeSelector::createFunction<APIMode::PRACTICAL>(std::forward<Args>(args)...);
        case APIMode::ENHANCED:
            return CJMODModeSelector::createFunction<APIMode::ENHANCED>(std::forward<Args>(args)...);
        case APIMode::AUTOFILL:
            return CJMODModeSelector::createFunction<APIMode::AUTOFILL>(std::forward<Args>(args)...);
        default:
            return CJMODModeSelector::createFunction<APIMode::PRACTICAL>(std::forward<Args>(args)...);
    }
}

/**
 * @brief 模式信息查询
 */
class CJMODModeInfo {
public:
    struct ModeDescription {
        std::string name;
        std::string description;
        std::string useCase;
        std::string performance;
        std::string complexity;
        std::vector<std::string> features;
        std::vector<std::string> examples;
    };
    
    static ModeDescription getPracticalModeInfo() {
        return {
            "Practical Mode",
            "最接近原始API设计的实用模式，功能职责分离明确",
            "追求性能和明确控制的生产环境",
            "最高 - 无额外开销",
            "简单 - 代码直观易懂",
            {
                "bind()专门处理值转换",
                "transform()专门生成JS代码",
                "operator()专门处理peekKeyword结果",
                "支持双指针扫描和前置截取",
                "完整的Policy模式支持"
            },
            {
                "arg.bind<std::string>([](const std::string& s) { return processValue(s); });",
                "arg.transform(\"loadImage('${value}')\");",
                "std::string result = arg(scanner.peekKeyword(-1));"
            }
        };
    }
    
    static ModeDescription getEnhancedModeInfo() {
        return {
            "Enhanced Mode", 
            "在Practical基础上增加链式调用，提升开发体验",
            "希望代码更流畅但保持控制力的场景",
            "高 - 轻微的方法调用开销",
            "中等 - 现代C++链式语法",
            {
                "继承Practical的所有功能",
                "支持流畅的方法链调用",
                "批量操作：bindSequence, transformSequence",
                "返回引用支持连续调用",
                "更好的代码可读性"
            },
            {
                "syntax.args.bind(\"image\", processor).transform(\"image\", template);",
                "args.bindSequence(proc1, proc2, proc3);",
                "args.matchSequence({\"val1\", \"val2\", \"val3\"});"
            }
        };
    }
    
    static ModeDescription getAutoFillModeInfo() {
        return {
            "AutoFill Mode",
            "智能自动填充模式，减少手动配置",
            "快速原型开发和复杂扫描分析场景",
            "中等 - 智能分析有计算开销",
            "复杂 - 多层智能推断逻辑",
            {
                "智能参数类型识别",
                "上下文感知的自动填充",
                "多种填充策略可选",
                "置信度评估系统",
                "深度双指针扫描集成"
            },
            {
                "func.enableAutoFill({ParamType::URL, ParamType::SIZE});",
                "func.smartFill(*scanner).generateCode();",
                "auto result = scanner.autoAnalyzeAndFill(sourceCode);"
            }
        };
    }
    
    static std::vector<ModeDescription> getAllModeInfo() {
        return {
            getPracticalModeInfo(),
            getEnhancedModeInfo(), 
            getAutoFillModeInfo()
        };
    }
};

} // namespace CJMOD
} // namespace CHTL

/**
 * @brief 使用示例和说明
 * 
 * 示例1: 默认模式（Practical）
 * ```cpp
 * #include "UnifiedCJMODApi.h"
 * 
 * // 使用默认的Practical模式
 * auto syntax = CHTL::CJMOD::createSyntax("loadImage $ $");
 * syntax->args[0].bind<std::string>([](const std::string& s) { return "\"" + s + "\""; });
 * syntax->args[0].transform("loadImage(${value})");
 * ```
 * 
 * 示例2: 显式选择Enhanced模式
 * ```cpp
 * #include "UnifiedCJMODApi.h"
 * 
 * CJMOD_USE_ENHANCED_MODE()
 * auto syntax = createSyntax<APIMode::ENHANCED>("loadImage $ $");
 * syntax->args.bind("image", processor).transform("image", template);
 * ```
 * 
 * 示例3: 全局设置AutoFill模式
 * ```cpp
 * #include "UnifiedCJMODApi.h"
 * 
 * CJMOD_SET_DEFAULT_MODE(AUTOFILL)
 * auto func = createFunction("loadImage");
 * func->enableAutoFill().smartFill(*scanner);
 * ```
 * 
 * 示例4: 运行时模式切换
 * ```cpp
 * #include "UnifiedCJMODApi.h"
 * 
 * // 根据配置选择模式
 * if (config.useAdvancedFeatures) {
 *     CHTL::CJMOD::CJMODModeSelector::setDefaultMode(APIMode::ENHANCED);
 * }
 * 
 * auto syntax = CHTL::CJMOD::createSyntax("pattern");  // 使用当前默认模式
 * ```
 */