#include "chtl_js/compiler/CHTLJSCompiler.h"
#include "chtl_js/parser/CHTLJSParser.h"
#include "chtl_js/generator/CHTLJSGenerator.h"
#include "scanner/CodeFragment.h"
#include "utils/Logger.h"
#include <sstream>

namespace chtl {
namespace chtljs {

using namespace parser;
using namespace generator;

CHTLJSCompiler::CHTLJSCompiler()
    : m_DebugMode(false) {
    m_GlobalMap = std::make_unique<compiler::CHTLJSGlobalMap>();
    m_StateMachine = std::make_unique<compiler::CHTLJSStateMachine>();
    m_Context = std::make_unique<compiler::CHTLJSContext>();
    m_Lexer = std::make_unique<compiler::CHTLJSLexer>();
}

CHTLJSCompiler::~CHTLJSCompiler() = default;

compiler::CompileResult CHTLJSCompiler::Compile(const std::string& sourceCode, const std::string& filename) {
    compiler::CompileResult result;
    
    try {
        // 1. 词法分析
        m_Lexer->SetSource(sourceCode, filename);
        auto tokens = m_Lexer->Tokenize();
        
        if (m_Lexer->HasErrors()) {
            result.Success = false;
            result.Errors = m_Lexer->GetErrors();
            return result;
        }
        
        // 2. 语法分析
        CHTLJSParser parser;
        parser.SetGlobalMap(m_GlobalMap);
        parser.SetDebugMode(m_DebugMode);
        
        // CHTL JS总是局部脚本
        auto ast = parser.Parse(tokens, true);
        
        if (parser.HasErrors()) {
            result.Success = false;
            result.Errors = parser.GetErrors();
            return result;
        }
        
        // 3. 代码生成
        CHTLJSGenerator generator;
        generator.SetGlobalMap(m_GlobalMap);
        generator.SetPrettyPrint(true);
        
        auto genResult = generator.Generate(ast.get());
        
        if (!genResult.Success) {
            result.Success = false;
            result.Errors = genResult.Errors;
            return result;
        }
        
        // 4. 返回结果
        result.Success = true;
        result.OutputContent = genResult.JavaScript;
        
        // 记录元数据
        for (const auto& func : genResult.GeneratedFunctions) {
            m_GlobalMap->AddBuiltinFunction(func);
        }
        for (const auto& selector : genResult.UsedSelectors) {
            m_GlobalMap->AddEnhancedSelector(selector);
        }
        
    } catch (const std::exception& e) {
        result.Success = false;
        result.Errors.push_back(std::string("CHTL JS编译异常: ") + e.what());
    }
    
    return result;
}

compiler::CompileResult CHTLJSCompiler::CompileFragments(const std::vector<CodeFragment>& fragments) {
    compiler::CompileResult result;
    
    // 将所有CHTL JS片段组合
    std::stringstream combined;
    for (const auto& fragment : fragments) {
        if (fragment.GetType() == CodeFragmentType::CHTL_JS ||
            fragment.GetType() == CodeFragmentType::SCRIPT) {
            combined << fragment.GetContent() << "\n";
        }
    }
    
    // 编译组合后的内容
    return Compile(combined.str(), fragments.empty() ? "" : fragments[0].GetFilename());
}

void CHTLJSCompiler::Reset() {
    m_GlobalMap = std::make_unique<compiler::CHTLJSGlobalMap>();
    m_StateMachine = std::make_unique<compiler::CHTLJSStateMachine>();
    m_Context = std::make_unique<compiler::CHTLJSContext>();
    m_Lexer = std::make_unique<compiler::CHTLJSLexer>();
}

std::string CHTLJSCompiler::GetName() const {
    return "CHTL JS Compiler";
}

void CHTLJSCompiler::SetDebugMode(bool debug) {
    m_DebugMode = debug;
    
    if (m_DebugMode) {
        utils::Logger::GetInstance().SetLevel(utils::LogLevel::DEBUG);
    }
}

const std::vector<std::string>& CHTLJSCompiler::GetErrors() const {
    return m_Lexer->GetErrors();
}

void CHTLJSCompiler::ClearErrors() {
    m_Lexer->ClearErrors();
}

} // namespace chtljs
} // namespace chtl