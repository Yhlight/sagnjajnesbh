#ifndef CHTL_JS_CODE_GENERATOR_H
#define CHTL_JS_CODE_GENERATOR_H

#include <string>
#include <memory>
#include <unordered_map>
#include <sstream>
#include "compilers/chtl_js/chtl_js_ast.h"

namespace chtl {

/**
 * CHTL JS代码生成器
 * 将CHTL JS AST转换为JavaScript代码
 */
class CHTLJSCodeGenerator : public CHTLJSASTVisitor {
public:
    CHTLJSCodeGenerator();
    ~CHTLJSCodeGenerator();
    
    /**
     * 生成JavaScript代码
     * @param ast AST根节点
     * @return 生成的JavaScript代码
     */
    std::string generate(std::shared_ptr<CHTLJSASTNode> ast);
    
    // 访问者模式实现
    void visitProgram(CHTLJSProgramNode* node) override;
    void visitEnhancedSelector(EnhancedSelectorNode* node) override;
    void visitVirtualObject(VirtualObjectNode* node) override;
    void visitArrowCall(ArrowCallNode* node) override;
    void visitListenCall(ListenCallNode* node) override;
    void visitDelegateCall(DelegateCallNode* node) override;
    void visitAnimateCall(AnimateCallNode* node) override;
    void visitRegularJS(RegularJSNode* node) override;
    
    /**
     * 注册虚对象
     */
    void registerVirtualObject(const std::string& name, const std::string& type);
    
private:
    std::stringstream output_;
    
    // 虚对象注册表
    std::unordered_map<std::string, std::string> virtualObjects_;
    
    // 生成的函数名计数器
    int functionCounter_;
    
    // 事件委托注册表
    std::unordered_map<std::string, std::vector<std::string>> eventDelegations_;
    
    // 辅助方法
    std::string generateFunctionName(const std::string& prefix);
    std::string translateSelector(const std::string& selector);
    std::string generateSelectorFunction(const std::string& selector);
    std::string generateEventDelegationCode();
    std::string generateAnimationCode(const std::string& config);
    
    // 增强选择器处理
    std::string processEnhancedSelector(const std::string& selector, bool returnAll = false);
    
    // 虚对象处理
    std::string processVirtualObjectMethod(const std::string& virName, const std::string& method);
};

} // namespace chtl

#endif // CHTL_JS_CODE_GENERATOR_H