#include "CHTL/Compiler/CHTLCompiler.h"
#include "CHTL/Lexer/Lexer.h"
#include "CHTL/Parser/Parser.h"
#include "CHTL/Generator/Generator.h"
#include "CHTL/Compiler/Context.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>

namespace CHTL {
namespace Compiler {

class CHTLCompiler::Impl {
public:
    CHTLCompilerConfig config;
    
    explicit Impl(const CHTLCompilerConfig& cfg) : config(cfg) {}
    
    CompileResult CompileFile(const std::string& filePath) {
        // 读取文件
        std::ifstream file(filePath);
        if (!file.is_open()) {
            CompileResult result;
            result.success = false;
            result.errors.push_back("无法打开文件: " + filePath);
            return result;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        
        return CompileString(buffer.str(), filePath);
    }
    
    CompileResult CompileString(const std::string& source, const std::string& filename) {
        CompileResult result;
        
        // 创建编译上下文
        auto context = std::make_unique<CompileContext>();
        context->SetCurrentFile(filename);
        
        // 设置上下文配置
        ContextConfig ctxConfig;
        ctxConfig.debugMode = config.enableDebug;
        context->SetConfig(ctxConfig);
        
        // 添加模块搜索路径
        context->AddModulePath(config.moduleSearchPath);
        
        try {
            // 词法分析
            Lexer lexer;
            auto tokens = lexer.Tokenize(source);
            
            if (lexer.HasError()) {
                result.success = false;
                result.errors.push_back(lexer.GetLastError());
                return result;
            }
            
            if (config.enableDebug) {
                std::cout << "=== 词法分析完成 ===" << std::endl;
                std::cout << "生成了 " << tokens.size() << " 个token" << std::endl;
            }
            
            // 语法分析
            Parser parser(context.get());
            auto ast = parser.Parse(tokens);
            
            if (parser.HasErrors()) {
                result.success = false;
                result.errors = parser.GetErrors();
                return result;
            }
            
            if (config.enableDebug) {
                std::cout << "\n=== 语法分析完成 ===" << std::endl;
                std::cout << "AST结构：" << std::endl;
                ast->Dump();
            }
            
            // 代码生成
            GeneratorConfig genConfig;
            genConfig.prettyPrint = config.prettyPrint;
            genConfig.generateComments = true;
            
            Generator generator(context.get(), genConfig);
            result.html = generator.Generate(ast);
            result.css = generator.GetGeneratedCSS();
            result.javascript = generator.GetGeneratedJS();
            
            // 获取错误和警告
            result.errors = context->GetErrors();
            result.warnings = context->GetWarnings();
            result.success = result.errors.empty();
            
            if (config.enableDebug) {
                std::cout << "\n=== 代码生成完成 ===" << std::endl;
                context->DumpContext();
            }
            
        } catch (const std::exception& e) {
            result.success = false;
            result.errors.push_back("编译错误: " + std::string(e.what()));
        }
        
        return result;
    }
    
    bool PackModule(const std::string& folderPath, const std::string& outputPath) {
        // TODO: 实现CMOD打包
        std::cerr << "CMOD打包功能尚未实现" << std::endl;
        return false;
    }
    
    bool UnpackModule(const std::string& cmodPath, const std::string& outputPath) {
        // TODO: 实现CMOD解包
        std::cerr << "CMOD解包功能尚未实现" << std::endl;
        return false;
    }
};

// CHTLCompiler公共接口实现
CHTLCompiler::CHTLCompiler(const CHTLCompilerConfig& config)
    : pImpl(std::make_unique<Impl>(config)) {}

CHTLCompiler::~CHTLCompiler() = default;

CompileResult CHTLCompiler::CompileFile(const std::string& filePath) {
    return pImpl->CompileFile(filePath);
}

CompileResult CHTLCompiler::CompileString(const std::string& source, const std::string& filename) {
    return pImpl->CompileString(source, filename);
}

bool CHTLCompiler::PackModule(const std::string& folderPath, const std::string& outputPath) {
    return pImpl->PackModule(folderPath, outputPath);
}

bool CHTLCompiler::UnpackModule(const std::string& cmodPath, const std::string& outputPath) {
    return pImpl->UnpackModule(cmodPath, outputPath);
}

} // namespace Compiler
} // namespace CHTL