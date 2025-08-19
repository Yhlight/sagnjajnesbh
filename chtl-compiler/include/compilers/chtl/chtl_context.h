#ifndef CHTL_CONTEXT_H
#define CHTL_CONTEXT_H

#include <string>
#include <memory>
#include <vector>
#include <stack>
#include "compilers/chtl/chtl_state.h"
#include "compilers/chtl/chtl_global_map.h"
#include "compilers/chtl/chtl_token.h"

namespace chtl {

/**
 * 编译上下文信息
 */
struct CompileContext {
    std::string sourceFile;      // 源文件名
    std::string sourceContent;   // 源代码内容
    size_t currentLine;          // 当前行号
    size_t currentColumn;        // 当前列号
    size_t currentPos;           // 当前位置
    
    // 错误和警告
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // 生成的代码
    std::string generatedCode;
    
    CompileContext() : currentLine(1), currentColumn(1), currentPos(0) {}
};

/**
 * 作用域类型
 */
enum class ScopeType {
    GLOBAL,      // 全局作用域
    TEMPLATE,    // 模板作用域
    CUSTOM,      // 自定义作用域
    ELEMENT,     // 元素作用域
    STYLE,       // 样式作用域
    SCRIPT       // 脚本作用域
};

/**
 * 作用域信息
 */
struct Scope {
    ScopeType type;
    std::string name;
    std::unordered_map<std::string, std::string> localVariables;  // 局部变量
    std::unordered_map<std::string, std::string> attributes;      // 属性
    
    Scope(ScopeType t, const std::string& n = "") : type(t), name(n) {}
};

/**
 * CHTL编译器上下文
 * 管理编译过程中的所有状态和数据
 */
class CHTLContext {
public:
    CHTLContext();
    ~CHTLContext();
    
    /**
     * 初始化上下文
     */
    void initialize(const std::string& sourceFile, const std::string& sourceContent);
    
    /**
     * 获取状态管理器
     */
    CHTLStateManager& getStateManager() { return stateManager_; }
    const CHTLStateManager& getStateManager() const { return stateManager_; }
    
    /**
     * 获取全局符号表
     */
    CHTLGlobalMap& getGlobalMap() { return globalMap_; }
    const CHTLGlobalMap& getGlobalMap() const { return globalMap_; }
    
    /**
     * 获取编译上下文
     */
    CompileContext& getCompileContext() { return compileContext_; }
    const CompileContext& getCompileContext() const { return compileContext_; }
    
    /**
     * 作用域管理
     */
    void enterScope(ScopeType type, const std::string& name = "");
    void exitScope();
    Scope* getCurrentScope();
    const Scope* getCurrentScope() const;
    
    /**
     * 错误和警告管理
     */
    void addError(const std::string& message, size_t line = 0, size_t column = 0);
    void addWarning(const std::string& message, size_t line = 0, size_t column = 0);
    bool hasErrors() const { return !compileContext_.errors.empty(); }
    const std::vector<std::string>& getErrors() const { return compileContext_.errors; }
    const std::vector<std::string>& getWarnings() const { return compileContext_.warnings; }
    
    /**
     * 位置更新
     */
    void updatePosition(const CHTLToken& token);
    void updatePosition(size_t line, size_t column, size_t pos);
    
    /**
     * 代码生成
     */
    void appendGeneratedCode(const std::string& code);
    const std::string& getGeneratedCode() const { return compileContext_.generatedCode; }
    
    /**
     * 获取当前位置信息
     */
    size_t getCurrentLine() const { return compileContext_.currentLine; }
    size_t getCurrentColumn() const { return compileContext_.currentColumn; }
    size_t getCurrentPos() const { return compileContext_.currentPos; }
    
    /**
     * 重置上下文
     */
    void reset();
    
    /**
     * 设置/获取编译选项
     */
    void setOption(const std::string& key, const std::string& value);
    std::string getOption(const std::string& key, const std::string& defaultValue = "") const;
    
    /**
     * 检查是否在特定上下文中
     */
    bool isInTemplateContext() const;
    bool isInCustomContext() const;
    bool isInElementContext() const;
    bool isInStyleContext() const;
    bool isInScriptContext() const;
    
    /**
     * 获取当前元素信息（如果在元素内）
     */
    std::string getCurrentElementName() const;
    std::string getCurrentElementId() const;
    std::vector<std::string> getCurrentElementClasses() const;
    
private:
    CHTLStateManager stateManager_;
    CHTLGlobalMap globalMap_;
    CompileContext compileContext_;
    
    // 作用域栈
    std::stack<Scope> scopeStack_;
    
    // 编译选项
    std::unordered_map<std::string, std::string> options_;
    
    // 辅助方法
    std::string formatError(const std::string& message, size_t line, size_t column) const;
    std::string formatWarning(const std::string& message, size_t line, size_t column) const;
};

} // namespace chtl

#endif // CHTL_CONTEXT_H