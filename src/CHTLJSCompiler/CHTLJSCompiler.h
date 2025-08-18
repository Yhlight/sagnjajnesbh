#pragma once
#include "Selector/EnhancedSelectorProcessor.h"
#include "Event/ListenSystemProcessor.h"
#include "Event/DelegateSystemProcessor.h"
#include "Animation/AnimateSystemProcessor.h"
#include "Virtual/VirtualObjectProcessor.h"
#include "Operator/ArrowOperatorProcessor.h"
#include "Script/LocalScriptProcessor.h"
// Enhanced目录已删除，功能已集成到其他处理器中
#include <string>
#include <vector>
#include <memory>

namespace chtl {
namespace chtljs {

/**
 * CHTL JS编译器主类
 * 严格按照CHTL语法文档实现完整的CHTL JS编译功能
 */
class CHTLJSCompiler {
public:
    struct CompilationResult {
        std::string compiledCode;           // 编译后的JavaScript代码
        std::vector<std::string> errors;    // 编译错误
        std::vector<std::string> warnings;  // 编译警告
        bool success = false;               // 编译是否成功
        
        // 统计信息
        int selectorsProcessed = 0;         // 处理的选择器数量
        int listenStatementsProcessed = 0;  // 处理的listen语句数量
        int delegateStatementsProcessed = 0; // 处理的delegate语句数量
        int animateStatementsProcessed = 0;  // 处理的animate语句数量
        int virtualObjectsProcessed = 0;     // 处理的虚对象数量
    };
    
    CHTLJSCompiler();
    ~CHTLJSCompiler();
    
    // === 按语法文档：主编译接口 ===
    
    /**
     * 编译CHTL JS代码
     * 按语法文档：将CHTL JS语法转换为标准JavaScript代码
     */
    CompilationResult compile(const std::string& chtljsCode);
    
    /**
     * 编译局部script块
     * 按语法文档：处理元素内部的script{}块
     */
    std::string compileLocalScript(const std::string& scriptContent, const std::string& elementContext);
    
    /**
     * 生成全局script
     * 按语法文档：合并所有局部script为高优先级全局script
     */
    std::string generateGlobalScript();
    
    // === 按语法文档：各种CHTL JS语法编译 ===
    
    /**
     * 编译增强选择器
     * 按语法文档：{{CSS选择器}} → DOM对象
     */
    std::string compileEnhancedSelectors(const std::string& code);
    
    /**
     * 编译箭头操作符
     * 按语法文档：-> 与 . 完全等价
     */
    std::string compileArrowOperators(const std::string& code);
    
    /**
     * 编译listen语句
     * 按语法文档：{{box}}->listen({ click: () => {} })
     */
    std::string compileListenStatements(const std::string& code);
    
    /**
     * 编译delegate语句
     * 按语法文档：{{parent}}->delegate({ target: {{child}}, click: func })
     */
    std::string compileDelegateStatements(const std::string& code);
    
    /**
     * 编译animate语句
     * 按语法文档：animate({ target: {{selector}}, duration: 100 })
     */
    std::string compileAnimateStatements(const std::string& code);
    
    /**
     * 编译虚对象
     * 按语法文档：vir test = listen({ click: () => {} }); test->click();
     */
    std::string compileVirtualObjects(const std::string& code);
    
    /**
     * 编译无修饰字面量
     * 支持CHTL JS中的无引号字符串字面量
     */
    std::string compileUndecoratedLiterals(const std::string& code);
    
    /**
     * 编译无序键值对
     * 允许键值对以任意顺序出现
     */
    std::string compileUnorderedKeyValues(const std::string& code);
    
    /**
     * 编译可选键值对
     * 支持键值对的可选性和默认值
     */
    std::string compileOptionalKeyValues(const std::string& code);
    
    // === 编译流程控制 ===
    
    /**
     * 预处理CHTL JS代码
     * 准备编译环境，检查语法
     */
    std::string preprocessCode(const std::string& code);
    
    /**
     * 后处理编译结果
     * 优化生成的JavaScript代码
     */
    std::string postprocessCode(const std::string& code);
    
    /**
     * 验证编译结果
     * 检查生成的JavaScript代码的有效性
     */
    bool validateCompiledCode(const std::string& code);
    
    // === 配置和选项 ===
    
    /**
     * 设置编译选项
     */
    void setDebugMode(bool enabled);
    void setOptimizationLevel(int level); // 0-3
    void setTargetECMAVersion(const std::string& version); // ES5, ES6, ES2017等
    
    /**
     * 获取编译统计
     */
    CompilationResult getLastCompilationResult() const;
    
    /**
     * 清理编译状态
     */
    void reset();
    
    // === 错误和警告处理 ===
    
    void addError(const std::string& message);
    void addWarning(const std::string& message);
    void clearErrors();
    void clearWarnings();
    
    bool hasErrors() const;
    bool hasWarnings() const;
    std::vector<std::string> getErrors() const;
    std::vector<std::string> getWarnings() const;

private:
    // CHTL JS处理器组件
    std::shared_ptr<EnhancedSelectorProcessor> selectorProcessor_;
    std::shared_ptr<ListenSystemProcessor> listenProcessor_;
    std::shared_ptr<DelegateSystemProcessor> delegateProcessor_;
    std::shared_ptr<AnimateSystemProcessor> animateProcessor_;
    std::shared_ptr<VirtualObjectProcessor> virtualProcessor_;
    std::shared_ptr<ArrowOperatorProcessor> arrowProcessor_;
    std::shared_ptr<LocalScriptProcessor> scriptProcessor_;
    // enhancedProcessor已集成到其他处理器中
    
    // 编译状态
    CompilationResult lastResult_;
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    
    // 编译选项
    bool debugMode_ = false;
    int optimizationLevel_ = 1;
    std::string targetECMAVersion_ = "ES6";
    
    // 内部方法
    void initializeProcessors();
    void updateCompilationStatistics(CompilationResult& result, const std::string& code);
};

} // namespace chtljs
} // namespace chtl