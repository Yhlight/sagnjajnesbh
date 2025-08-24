#include "CHTL/Compiler/CHTLCompiler.h"
#include "Utils/ErrorHandler.h"
#include "Utils/StringUtils.h"
#include <regex>
#include <sstream>

namespace CHTL {
namespace Compiler {

CHTLCompiler::CHTLCompiler(const CHTLCompilerConfig& config) 
    : config_(config) {
    InitializeComponents();
}

CHTLCompiler::~CHTLCompiler() {
    Cleanup();
}

void CHTLCompiler::InitializeComponents() {
    try {
        // 初始化核心组件
        globalMap_ = std::make_unique<Core::CHTLGlobalMap>();
        stateManager_ = std::make_unique<Core::CHTLState>(*globalMap_);
        
        // 初始化编译器组件
        lexer_ = std::make_unique<Lexer::CHTLLexer>();
        parser_ = std::make_unique<Parser::CHTLParser>(*globalMap_, *stateManager_);
        
        // 初始化生成器（需要CMOD管理器）
        // 暂时使用空的CMOD管理器
        auto emptyCMODManager = std::make_unique<CMOD::CompleteCMODManager>("", "");
        generator_ = std::make_unique<Generator::CHTLGenerator>(*globalMap_, std::move(emptyCMODManager));
        
        if (config_.enableDebug) {
            Utils::ErrorHandler::GetInstance().LogInfo("CHTL编译器组件初始化完成");
        }
    } catch (const std::exception& e) {
        Utils::ErrorHandler::GetInstance().LogError("CHTL编译器初始化失败: " + std::string(e.what()));
        throw;
    }
}

void CHTLCompiler::Cleanup() {
    generator_.reset();
    parser_.reset();
    lexer_.reset();
    stateManager_.reset();
    globalMap_.reset();
}

CHTLCompilationResult CHTLCompiler::Compile(const std::string& chtlCode, const std::string& fileName) {
    CHTLCompilationResult result;
    
    if (chtlCode.empty()) {
        HandleCompilationError("输入的CHTL代码为空", result);
        return result;
    }
    
    try {
        if (config_.enableDebug) {
            Utils::ErrorHandler::GetInstance().LogInfo("开始编译CHTL代码: " + fileName);
        }
        
        // 1. 预处理代码
        std::string processedCode = PreprocessCode(chtlCode);
        
        // 2. 检查是否为局部style块
        if (IsLocalStyleBlock(processedCode)) {
            return CompileLocalStyle(processedCode, fileName);
        }
        
        // 3. 词法分析
        auto tokens = lexer_->Tokenize(processedCode, fileName);
        
        if (config_.enableDebug) {
            Utils::ErrorHandler::GetInstance().LogInfo("词法分析完成，Token数量: " + std::to_string(tokens.size()));
        }
        
        // 4. 语法分析
        auto ast = parser_->Parse(tokens, fileName);
        
        if (!ast) {
            HandleCompilationError("语法分析失败：无法生成AST", result);
            return result;
        }
        
        if (config_.enableDebug) {
            Utils::ErrorHandler::GetInstance().LogInfo("语法分析完成");
        }
        
        // 5. 代码生成
        result.htmlOutput = generator_->Generate(ast);
        
        // 6. 后处理
        if (config_.optimizeOutput) {
            result.htmlOutput = PostprocessHTML(result.htmlOutput);
        }
        
        result.success = true;
        
        if (config_.enableDebug) {
            Utils::ErrorHandler::GetInstance().LogInfo("CHTL编译完成，生成HTML长度: " + std::to_string(result.htmlOutput.length()));
        }
        
    } catch (const std::exception& e) {
        HandleCompilationError("编译过程中发生异常: " + std::string(e.what()), result);
    }
    
    return result;
}

CHTLCompilationResult CHTLCompiler::CompileLocalStyle(const std::string& styleCode, const std::string& fileName) {
    CHTLCompilationResult result;
    
    try {
        if (config_.enableDebug) {
            Utils::ErrorHandler::GetInstance().LogInfo("开始编译局部style块: " + fileName);
        }
        
        // 提取style块中的CSS内容
        std::string cssContent = ExtractStyleContent(styleCode);
        
        // 处理CHTL变量模板（如ThemeColor(), Colors()等）
        // 这里需要解析CHTL语法并替换为实际值
        std::string processedCSS = ProcessCHTLVariablesInCSS(cssContent);
        
        result.cssOutput = processedCSS;
        result.success = true;
        
        if (config_.enableDebug) {
            Utils::ErrorHandler::GetInstance().LogInfo("局部style编译完成，生成CSS长度: " + std::to_string(result.cssOutput.length()));
        }
        
    } catch (const std::exception& e) {
        HandleCompilationError("局部style编译失败: " + std::string(e.what()), result);
    }
    
    return result;
}

bool CHTLCompiler::Validate(const std::string& chtlCode) {
    try {
        auto tokens = lexer_->Tokenize(chtlCode, "validation");
        auto ast = parser_->Parse(tokens, "validation");
        return ast != nullptr;
    } catch (const std::exception&) {
        return false;
    }
}

void CHTLCompiler::SetConfig(const CHTLCompilerConfig& config) {
    config_ = config;
}

std::vector<std::string> CHTLCompiler::GetSupportedFeatures() {
    return {
        "HTML元素生成",
        "局部样式块处理",
        "CHTL变量模板",
        "命名空间支持",
        "模板系统",
        "自定义组件",
        "继承机制",
        "约束验证",
        "选择器自动化"
    };
}

std::string CHTLCompiler::PreprocessCode(const std::string& code) {
    std::string processed = code;
    
    // 移除多余的空白字符
    processed = Utils::StringUtils::Trim(processed);
    
    // 标准化换行符
    std::regex newlineRegex("\r\n|\r");
    processed = std::regex_replace(processed, newlineRegex, "\n");
    
    return processed;
}

std::string CHTLCompiler::PostprocessHTML(const std::string& html) {
    std::string processed = html;
    
    if (config_.optimizeOutput) {
        // 移除多余的空白
        std::regex whitespaceRegex("\\s+");
        processed = std::regex_replace(processed, whitespaceRegex, " ");
        
        // 移除标签间的空白
        std::regex tagSpaceRegex(">\\s+<");
        processed = std::regex_replace(processed, tagSpaceRegex, "><");
    }
    
    return Utils::StringUtils::Trim(processed);
}

void CHTLCompiler::HandleCompilationError(const std::string& error, CHTLCompilationResult& result) {
    result.success = false;
    result.errors.push_back(error);
    
    Utils::ErrorHandler::GetInstance().LogError("CHTL编译错误: " + error);
}

bool CHTLCompiler::IsLocalStyleBlock(const std::string& code) {
    // 检查是否为局部style块
    std::string trimmed = Utils::StringUtils::Trim(code);
    
    // 简单检查：以style开头且包含大括号
    return trimmed.find("style") == 0 && 
           trimmed.find("{") != std::string::npos &&
           trimmed.find("}") != std::string::npos;
}

std::string CHTLCompiler::ExtractStyleContent(const std::string& styleBlock) {
    std::string content = styleBlock;
    
    // 移除style关键字和外层大括号
    size_t startPos = content.find("{");
    size_t endPos = content.rfind("}");
    
    if (startPos != std::string::npos && endPos != std::string::npos && startPos < endPos) {
        content = content.substr(startPos + 1, endPos - startPos - 1);
    }
    
    return Utils::StringUtils::Trim(content);
}

std::string CHTLCompiler::ProcessCHTLVariablesInCSS(const std::string& cssContent) {
    std::string processed = cssContent;
    
    // 处理CHTL变量模板，如ThemeColor(), Colors().primary等
    // 这里需要实际的变量解析逻辑
    
    // 暂时的简单替换示例
    std::regex themeColorRegex("ThemeColor\\(\\)");
    processed = std::regex_replace(processed, themeColorRegex, "#007bff");
    
    std::regex colorsRegex("Colors\\(\\)\\.primary");
    processed = std::regex_replace(processed, colorsRegex, "#007bff");
    
    std::regex spacingRegex("Spacing\\(\\)\\.medium");
    processed = std::regex_replace(processed, spacingRegex, "16px");
    
    return processed;
}

} // namespace Compiler
} // namespace CHTL