#pragma once
#include <memory>
#include <string>
#include <vector>

namespace CHTL {
namespace Compiler {

// CHTL编译器配置
struct CHTLCompilerConfig {
    bool enableDebug = false;
    bool prettyPrint = true;
    bool generateSourceMap = false;
    std::string moduleSearchPath = "./modules";
};

// CHTL编译结果
struct CompileResult {
    bool success;
    std::string html;
    std::string css;
    std::string javascript;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
};

// CHTL编译器主类
class CHTLCompiler {
public:
    explicit CHTLCompiler(const CHTLCompilerConfig& config = CHTLCompilerConfig());
    ~CHTLCompiler();
    
    // 编译CHTL文件
    CompileResult CompileFile(const std::string& filePath);
    
    // 编译CHTL代码
    CompileResult CompileString(const std::string& source, const std::string& filename = "<string>");
    
    // 打包CMOD模块
    bool PackModule(const std::string& folderPath, const std::string& outputPath);
    
    // 解包CMOD模块
    bool UnpackModule(const std::string& cmodPath, const std::string& outputPath);
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace Compiler
} // namespace CHTL