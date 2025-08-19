#ifndef CHTL_COMPILER_DISPATCHER_H
#define CHTL_COMPILER_DISPATCHER_H

#include <memory>
#include <vector>
#include <unordered_map>
#include "dispatcher/code_fragment.h"
#include "compilers/chtl/chtl_compiler.h"
#include "compilers/chtl_js/chtl_js_compiler.h"
#include "compilers/css/css_compiler.h"
#include "compilers/javascript/javascript_compiler.h"

namespace chtl {

/**
 * 编译结果
 */
struct CompileResult {
    bool success;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
};

/**
 * CompilerDispatcher - 编译器调度器
 * 负责将不同类型的代码片段分发到对应的编译器
 */
class CompilerDispatcher {
public:
    CompilerDispatcher();
    ~CompilerDispatcher();
    
    /**
     * 分发代码片段到对应的编译器进行编译
     * @param fragments 代码片段列表
     * @return 编译结果
     */
    CompileResult dispatch(std::vector<std::unique_ptr<CodeFragment>>& fragments);
    
    /**
     * 获取编译后的HTML输出
     * @return HTML字符串
     */
    std::string getCompiledHTML() const;
    
    /**
     * 重置调度器状态
     */
    void reset();
    
    /**
     * 设置编译选项
     */
    void setOption(const std::string& key, const std::string& value);
    
private:
    // 编译器实例
    std::unique_ptr<CHTLCompiler> chtlCompiler_;
    std::unique_ptr<CHTLJSCompiler> chtlJsCompiler_;
    std::unique_ptr<CSSCompiler> cssCompiler_;
    std::unique_ptr<JavaScriptCompiler> jsCompiler_;
    
    // 编译选项
    std::unordered_map<std::string, std::string> options_;
    
    // 编译结果缓存
    std::vector<std::string> htmlFragments_;
    std::vector<std::string> globalStyles_;
    std::vector<std::string> globalScripts_;
    
    // 处理不同类型的片段
    bool processCHTLFragment(CHTLFragment* fragment, CompileResult& result);
    bool processCHTLJSFragment(CHTLJSFragment* fragment, CompileResult& result);
    bool processCSSFragment(CSSFragment* fragment, CompileResult& result);
    bool processJavaScriptFragment(JavaScriptFragment* fragment, CompileResult& result);
    bool processLocalStyleFragment(LocalStyleFragment* fragment, CompileResult& result);
    
    // 合并编译结果
    void mergeResults();
};

} // namespace chtl

#endif // CHTL_COMPILER_DISPATCHER_H