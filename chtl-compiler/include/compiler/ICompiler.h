#pragma once

#include <string>
#include <memory>
#include <vector>
#include "scanner/CodeFragment.h"

namespace chtl {

/**
 * 编译结果
 */
struct CompileResult {
    bool success;
    std::string output;         // 编译后的输出
    std::string errorMessage;   // 错误信息
    
    CompileResult(bool s = true, const std::string& out = "", const std::string& err = "")
        : success(s), output(out), errorMessage(err) {}
};

/**
 * 编译器接口
 * 所有编译器（CHTL、CHTL JS、CSS、JavaScript）都必须实现此接口
 */
class ICompiler {
public:
    virtual ~ICompiler() = default;
    
    /**
     * 编译代码片段
     * @param fragment 代码片段
     * @return 编译结果
     */
    virtual CompileResult Compile(const CodeFragment& fragment) = 0;
    
    /**
     * 编译多个代码片段
     * @param fragments 代码片段列表
     * @return 编译结果
     */
    virtual CompileResult CompileFragments(const std::vector<CodeFragment>& fragments) = 0;
    
    /**
     * 重置编译器状态
     */
    virtual void Reset() = 0;
    
    /**
     * 获取编译器名称
     */
    virtual std::string GetName() const = 0;
    
    /**
     * 设置调试模式
     */
    virtual void SetDebugMode(bool debug) = 0;
};

} // namespace chtl