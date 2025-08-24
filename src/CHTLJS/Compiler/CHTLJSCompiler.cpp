#include "CHTLJS/Compiler/CHTLJSCompiler.h"
#include "Utils/ErrorHandler.h"
#include "Utils/StringUtils.h"
#include <regex>
#include <sstream>

namespace CHTL {
namespace CHTLJS {
namespace Compiler {

CHTLJSCompiler::CHTLJSCompiler(const CHTLJSCompilerConfig& config) 
    : config_(config) {
    InitializeComponents();
}

CHTLJSCompiler::~CHTLJSCompiler() {
    Cleanup();
}

void CHTLJSCompiler::InitializeComponents() {
    try {
        // 初始化核心组件
        stateManager_ = std::make_unique<Core::CHTLJSState>();
        
        // 初始化编译器组件
        lexer_ = std::make_unique<Lexer::CHTLJSLexer>();
        parser_ = std::make_unique<Parser::CHTLJSParser>(*stateManager_);
        generator_ = std::make_unique<Generator::CHTLJSGenerator>();
        
        if (config_.enableDebug) {
            Utils::ErrorHandler::GetInstance().LogInfo("CHTL JS编译器组件初始化完成");
        }
    } catch (const std::exception& e) {
        Utils::ErrorHandler::GetInstance().LogError("CHTL JS编译器初始化失败: " + std::string(e.what()));
        throw;
    }
}

void CHTLJSCompiler::Cleanup() {
    generator_.reset();
    parser_.reset();
    lexer_.reset();
    stateManager_.reset();
}

CHTLJSCompilationResult CHTLJSCompiler::Compile(const std::string& chtlJSCode, const std::string& fileName) {
    CHTLJSCompilationResult result;
    
    if (chtlJSCode.empty()) {
        HandleCompilationError("输入的CHTL JS代码为空", result);
        return result;
    }
    
    try {
        if (config_.enableDebug) {
            Utils::ErrorHandler::GetInstance().LogInfo("开始编译CHTL JS代码: " + fileName);
        }
        
        // 1. 预处理代码
        std::string processedCode = PreprocessCode(chtlJSCode);
        
        // 2. 检查虚对象语法
        if (IsVirtualObjectSyntax(processedCode)) {
            return CompileVirtualObject(processedCode, fileName);
        }
        
        // 3. 词法分析
        auto tokens = lexer_->Tokenize(processedCode, fileName);
        
        if (config_.enableDebug) {
            Utils::ErrorHandler::GetInstance().LogInfo("CHTL JS词法分析完成，Token数量: " + std::to_string(tokens.Size()));
        }
        
        // 4. 语法分析
        auto ast = parser_->Parse(tokens, fileName);
        
        if (!ast) {
            HandleCompilationError("CHTL JS语法分析失败：无法生成AST", result);
            return result;
        }
        
        if (config_.enableDebug) {
            Utils::ErrorHandler::GetInstance().LogInfo("CHTL JS语法分析完成");
        }
        
        // 5. 代码生成
        result.jsOutput = generator_->Generate(ast);
        
        // 6. 后处理
        if (config_.optimizeOutput) {
            result.jsOutput = PostprocessJavaScript(result.jsOutput);
        }
        
        result.success = true;
        
        if (config_.enableDebug) {
            Utils::ErrorHandler::GetInstance().LogInfo("CHTL JS编译完成，生成JavaScript长度: " + std::to_string(result.jsOutput.length()));
        }
        
    } catch (const std::exception& e) {
        HandleCompilationError("CHTL JS编译过程中发生异常: " + std::string(e.what()), result);
    }
    
    return result;
}

CHTLJSCompilationResult CHTLJSCompiler::CompileVirtualObject(const std::string& virCode, const std::string& fileName) {
    CHTLJSCompilationResult result;
    
    try {
        if (config_.enableDebug) {
            Utils::ErrorHandler::GetInstance().LogInfo("开始编译虚对象: " + fileName);
        }
        
        // 转换vir语法为标准JavaScript
        result.jsOutput = TransformVirtualObject(virCode);
        result.success = true;
        
        if (config_.enableDebug) {
            Utils::ErrorHandler::GetInstance().LogInfo("虚对象编译完成");
        }
        
    } catch (const std::exception& e) {
        HandleCompilationError("虚对象编译失败: " + std::string(e.what()), result);
    }
    
    return result;
}

// 移除了错误的表达式编译方法

bool CHTLJSCompiler::Validate(const std::string& chtlJSCode) {
    try {
        auto tokens = lexer_->Tokenize(chtlJSCode, "validation");
        auto ast = parser_->Parse(tokens, "validation");
        return ast != nullptr;
    } catch (const std::exception&) {
        return false;
    }
}

void CHTLJSCompiler::SetConfig(const CHTLJSCompilerConfig& config) {
    config_ = config;
}

std::vector<std::string> CHTLJSCompiler::GetSupportedFeatures() {
    return {
        "虚对象(vir)语法",
        "表达式{{}}语法",
        "事件处理",
        "函数定义",
        "变量绑定",
        "条件语句",
        "循环语句",
        "对象操作",
        "数组处理"
    };
}

std::string CHTLJSCompiler::PreprocessCode(const std::string& code) {
    std::string processed = code;
    
    // 移除多余的空白字符
    processed = Utils::StringUtils::Trim(processed);
    
    // 标准化换行符
    std::regex newlineRegex("\r\n|\r");
    processed = std::regex_replace(processed, newlineRegex, "\n");
    
    return processed;
}

std::string CHTLJSCompiler::PostprocessJavaScript(const std::string& js) {
    std::string processed = js;
    
    if (config_.optimizeOutput) {
        // 移除多余的空白
        std::regex whitespaceRegex("\\s+");
        processed = std::regex_replace(processed, whitespaceRegex, " ");
        
        // 移除不必要的分号
        std::regex semicolonRegex(";\\s*}");
        processed = std::regex_replace(processed, semicolonRegex, "}");
    }
    
    if (config_.generateComments) {
        processed = "// Generated by CHTL JS Compiler v" + GetVersion() + "\n" + processed;
    }
    
    return Utils::StringUtils::Trim(processed);
}

void CHTLJSCompiler::HandleCompilationError(const std::string& error, CHTLJSCompilationResult& result) {
    result.success = false;
    result.errors.push_back(error);
    
    Utils::ErrorHandler::GetInstance().LogError("CHTL JS编译错误: " + error);
}

bool CHTLJSCompiler::IsVirtualObjectSyntax(const std::string& code) {
    std::string trimmed = Utils::StringUtils::Trim(code);
    return trimmed.find("vir") == 0;
}

// 移除了错误的表达式语法检测

std::string CHTLJSCompiler::TransformVirtualObject(const std::string& virCode) {
    // 解析vir语法：vir objectName { ... }
    std::regex virRegex(R"(vir\s+(\w+)\s*\{(.*)\})");
    std::smatch matches;
    
    if (std::regex_search(virCode, matches, virRegex)) {
        std::string objectName = matches[1].str();
        std::string objectBody = matches[2].str();
        
        // 生成虚对象的JavaScript代码
        std::stringstream js;
        
        if (config_.generateComments) {
            js << "// Virtual object: " << objectName << "\n";
        }
        
        js << "const " << objectName << " = {\n";
        js << objectBody;
        js << "\n};\n";
        
        // 如果启用虚对象支持，添加特殊处理
        if (config_.enableVirtualObjects) {
            js << "// Register virtual object\n";
            js << "if (typeof window !== 'undefined' && window.CHTL) {\n";
            js << "    window.CHTL.registerVirtualObject('" << objectName << "', " << objectName << ");\n";
            js << "}\n";
        }
        
        return js.str();
    }
    
    // 如果解析失败，返回注释版本
    return "// Failed to parse virtual object: " + virCode;
}

// 移除了错误的表达式转换方法

} // namespace Compiler
} // namespace CHTLJS
} // namespace CHTL