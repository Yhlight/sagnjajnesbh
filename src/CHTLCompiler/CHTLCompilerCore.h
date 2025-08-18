#pragma once
#include "../Lexer/CHTLLexer.h"
#include "../Parser/CHTLParser.h"
#include "../Generator/CHTLGenerator.h"
#include "../State/CHTLGlobalMap.h"
#include "../Module/CmodManager.h"
#include "../../Shared/SimpleZip/SimpleZip.h"
#include <string>
#include <memory>

namespace chtl {

/**
 * CHTL编译器核心
 * 负责CHTL语法的完整编译流程
 */
class CHTLCompilerCore {
public:
    CHTLCompilerCore();
    ~CHTLCompilerCore();
    
    // 主编译接口
    std::string compile(const std::string& source_code);
    std::string compileFile(const std::string& file_path);
    
    // 编译流程控制
    bool initialize();
    void cleanup();
    void reset();
    
    // 组件访问
    CHTLLexer& getLexer() { return *lexer_; }
    parser::CHTLParser& getParser() { return *parser_; }
    generator::HTMLGenerator& getGenerator() { return *generator_; }
    CmodManager& getModuleManager() { return *module_manager_; }
    
    // 配置管理
    void setDebugMode(bool debug) { debug_mode_ = debug; }
    bool isDebugMode() const { return debug_mode_; }
    
    // 错误处理
    std::vector<std::string> getErrors() const { return errors_; }
    std::vector<std::string> getWarnings() const { return warnings_; }
    void clearErrors() { errors_.clear(); warnings_.clear(); }
    
private:
    std::unique_ptr<CHTLLexer> lexer_;
    std::unique_ptr<parser::CHTLParser> parser_;
    std::unique_ptr<generator::HTMLGenerator> generator_;
    std::unique_ptr<CmodManager> module_manager_;
    
    bool debug_mode_;
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    
    // 内部方法
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
};

} // namespace chtl