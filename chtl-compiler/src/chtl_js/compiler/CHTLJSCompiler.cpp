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
    m_GlobalMap = std::make_shared<CHTLJSGlobalMap>();
    m_StateMachine = std::make_unique<CHTLJSStateMachine>();
    m_Context = std::make_unique<CHTLJSContext>();
    m_Lexer = std::make_unique<CHTLJSLexer>();
}

CHTLJSCompiler::~CHTLJSCompiler() = default;

chtl::CompileResult CHTLJSCompiler::Compile(const CodeFragment& fragment) {
    chtl::CompileResult result;
    
    try {
        // 1. 词法分析
        m_Lexer->SetInput(fragment.GetContent(), "");
        auto tokens = m_Lexer->ScanTokens();
        
        if (m_Lexer->HasErrors()) {
            result.success = false;
            result.errorMessage = m_Lexer->GetErrors().empty() ? "词法分析错误" : m_Lexer->GetErrors()[0];
            return result;
        }
        
        // 2. 语法分析
        CHTLJSParser parser;
        parser.SetGlobalMap(m_GlobalMap);
        parser.SetDebugMode(m_DebugMode);
        
        // CHTL JS总是局部脚本
        auto ast = parser.Parse(tokens, true);
        
        if (parser.HasErrors()) {
            result.success = false;
            result.errorMessage = parser.GetErrors().empty() ? "语法分析错误" : parser.GetErrors()[0];
            return result;
        }
        
        // 3. 代码生成
        CHTLJSGenerator generator;
        generator.SetGlobalMap(m_GlobalMap);
        generator.SetPrettyPrint(true);
        
        auto genResult = generator.Generate(ast.get());
        
        if (!genResult.Success) {
            result.success = false;
            result.errorMessage = genResult.Errors.empty() ? "代码生成错误" : genResult.Errors[0];
            return result;
        }
        
        // 4. 返回结果
        result.success = true;
        result.output = genResult.JavaScript;
        
        // 记录元数据
        for (const auto& func : genResult.GeneratedFunctions) {
            m_GlobalMap->AddBuiltinFunction(func);
        }
        for (const auto& selector : genResult.UsedSelectors) {
            m_GlobalMap->AddEnhancedSelector(selector);
        }
        
    } catch (const std::exception& e) {
        result.success = false;
        result.errorMessage = std::string("CHTL JS编译异常: ") + e.what();
    }
    
    return result;
}

chtl::CompileResult CHTLJSCompiler::CompileFragments(const std::vector<CodeFragment>& fragments) {
    chtl::CompileResult result;
    
    // 将所有CHTL JS片段组合
    std::stringstream combined;
    for (const auto& fragment : fragments) {
        if (fragment.GetType() == FragmentType::CHTL_JS ||
            fragment.GetType() == FragmentType::JAVASCRIPT) {
            combined << fragment.GetContent() << "\n";
        }
    }
    
    // 编译组合后的内容
    if (combined.str().empty()) {
        result.success = true;
        result.output = "";
        return result;
    }
    
    // 创建临时fragment
    CodeFragment tempFragment(FragmentType::CHTL_JS, combined.str(), 1, 1, 1, 1);
    return Compile(tempFragment);
}

void CHTLJSCompiler::Reset() {
    m_GlobalMap = std::make_shared<CHTLJSGlobalMap>();
    m_StateMachine = std::make_unique<CHTLJSStateMachine>();
    m_Context = std::make_unique<CHTLJSContext>();
    m_Lexer = std::make_unique<CHTLJSLexer>();
}


void CHTLJSCompiler::SetDebugMode(bool debug) {
    m_DebugMode = debug;
    
    if (m_DebugMode) {
        utils::Logger::GetInstance().SetLevel(utils::LogLevel::DEBUG);
    }
}


} // namespace chtljs
} // namespace chtl