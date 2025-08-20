#include "chtl/compiler/CHTLCompiler.h"
#include "chtl/parser/CHTLParser.h"
#include "chtl/generator/CHTLGenerator.h"
#include "scanner/CodeFragment.h"
#include "utils/Logger.h"
#include <sstream>

namespace chtl {
namespace compiler {

using namespace parser;
using namespace generator;

CHTLCompiler::CHTLCompiler()
    : m_DebugMode(false) {
    m_GlobalMap = std::make_unique<CHTLGlobalMap>();
    m_StateMachine = std::make_unique<CHTLStateMachine>();
    m_Context = std::make_unique<CHTLContext>();
    m_Lexer = std::make_unique<CHTLLexer>();
}

CHTLCompiler::~CHTLCompiler() = default;

CompileResult CHTLCompiler::Compile(const CodeFragment& fragment) {
    CompileResult result;
    
    try {
        // 1. 词法分析
        m_Lexer->SetSource(fragment.GetContent(), "");
        auto tokens = m_Lexer->Tokenize();
        
        if (m_Lexer->HasErrors()) {
            result.success = false;
            result.errorMessage = m_Lexer->GetErrors();
            return result;
        }
        
        // 2. 语法分析
        CHTLParser parser;
        parser.SetGlobalMap(m_GlobalMap);
        parser.SetDebugMode(m_DebugMode);
        
        auto ast = parser.Parse(tokens, filename);
        
        if (parser.HasErrors()) {
            result.success = false;
            result.errorMessage = parser.GetErrors();
            return result;
        }
        
        // 3. 代码生成
        CHTLGenerator generator;
        generator.SetGlobalMap(m_GlobalMap);
        generator.SetPrettyPrint(true);
        generator.SetGenerateComments(true);
        
        auto genResult = generator.Generate(ast.get());
        
        if (!genResult.Success) {
            result.success = false;
            result.errorMessage = genResult.Errors;
            return result;
        }
        
        // 4. 组合输出
        result.success = true;
        result.output = BuildFinalOutput(genResult);
        
        // 记录生成的元数据
        for (const auto& cls : genResult.GeneratedClasses) {
            m_GlobalMap->AddGeneratedClass(cls);
        }
        for (const auto& id : genResult.GeneratedIds) {
            m_GlobalMap->AddGeneratedId(id);
        }
        
    } catch (const std::exception& e) {
        result.success = false;
        result.errorMessage.push_back(std::string("编译异常: ") + e.what());
    }
    
    return result;
}

CompileResult CHTLCompiler::CompileFragments(const std::vector<CodeFragment>& fragments) {
    CompileResult result;
    
    // 将所有CHTL片段组合
    std::stringstream combined;
    for (const auto& fragment : fragments) {
        if (fragment.GetType() == CodeFragmentType::CHTL ||
            fragment.GetType() == CodeFragmentType::HTML ||
            fragment.GetType() == CodeFragmentType::TEXT) {
            combined << fragment.GetContent() << "\n";
        }
    }
    
    // 编译组合后的内容
    return Compile(combined.str(), fragments.empty() ? "" : fragments[0].GetFilename());
}

void CHTLCompiler::Reset() {
    m_GlobalMap = std::make_unique<CHTLGlobalMap>();
    m_StateMachine = std::make_unique<CHTLStateMachine>();
    m_Context = std::make_unique<CHTLContext>();
    m_Lexer = std::make_unique<CHTLLexer>();
}

void CHTLCompiler::SetDebugMode(bool debug) {
    m_DebugMode = debug;
    
    if (m_DebugMode) {
        utils::Logger::GetInstance().SetLevel(utils::LogLevel::DEBUG);
    }
}

void CHTLCompiler::SetModulePaths(const std::vector<std::string>& paths) {
    m_ModulePaths = paths;
}

std::string CHTLCompiler::BuildFinalOutput(const generator::GenerateResult& genResult) {
    std::stringstream output;
    
    // 如果有CSS，创建style标签
    if (!genResult.Css.empty()) {
        // 查找</head>标签位置
        size_t headPos = genResult.Html.find("</head>");
        if (headPos != std::string::npos) {
            // 在</head>前插入style
            std::string beforeHead = genResult.Html.substr(0, headPos);
            std::string afterHead = genResult.Html.substr(headPos);
            
            output << beforeHead;
            output << "<style>\n" << genResult.Css << "</style>\n";
            output << afterHead;
        } else {
            // 如果没有head，直接在开始添加
            output << "<style>\n" << genResult.Css << "</style>\n";
            output << genResult.Html;
        }
    } else {
        output << genResult.Html;
    }
    
    // 如果有JavaScript，创建script标签
    if (!genResult.JavaScript.empty()) {
        // 查找</body>标签位置
        size_t bodyPos = output.str().find("</body>");
        if (bodyPos != std::string::npos) {
            // 在</body>前插入script
            std::string result = output.str();
            result.insert(bodyPos, "<script>\n" + genResult.JavaScript + "</script>\n");
            return result;
        } else {
            // 如果没有body，在末尾添加
            output << "<script>\n" << genResult.JavaScript << "</script>\n";
        }
    }
    
    return output.str();
}

// 以下是私有编译方法的占位实现

CompileResult CHTLCompiler::CompileElement(const std::string& code) {
    return Compile(code, "element");
}

CompileResult CHTLCompiler::CompileTemplate(const std::string& code) {
    return Compile(code, "template");
}

CompileResult CHTLCompiler::CompileCustom(const std::string& code) {
    return Compile(code, "custom");
}

CompileResult CHTLCompiler::CompileImport(const std::string& code) {
    CompileResult result;
    result.success = false;
    result.errorMessage.push_back("导入功能尚未实现");
    return result;
}

CompileResult CHTLCompiler::CompileNamespace(const std::string& code) {
    return Compile(code, "namespace");
}

CompileResult CHTLCompiler::CompileConfiguration(const std::string& code) {
    return Compile(code, "configuration");
}

CompileResult CHTLCompiler::CompileOrigin(const std::string& code) {
    return Compile(code, "origin");
}

void CHTLCompiler::ProcessTokens(const std::vector<CHTLToken>& tokens) {
    // 由解析器处理
}

std::string CHTLCompiler::GenerateOutput() {
    // 由生成器处理
    return "";
}

} // namespace compiler
} // namespace chtl