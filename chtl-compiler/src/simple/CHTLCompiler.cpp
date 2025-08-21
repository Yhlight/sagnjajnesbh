#include "CHTLCompiler.h"
#include "Lexer.h"
#include "Parser.h"
#include "Generator.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace chtl {
namespace simple {

CHTLCompiler::CHTLCompiler() : m_DebugMode(false) {
    m_Lexer = std::make_unique<Lexer>();
    m_Parser = std::make_unique<Parser>();
    m_Generator = std::make_unique<Generator>();
}

CHTLCompiler::~CHTLCompiler() = default;

std::string CHTLCompiler::Compile(const std::string& chtlCode) {
    // 1. 词法分析
    m_Lexer->SetInput(chtlCode);
    auto tokens = m_Lexer->Tokenize();
    
    if (m_Lexer->HasErrors()) {
        if (m_DebugMode) {
            for (const auto& error : m_Lexer->GetErrors()) {
                std::cerr << error << std::endl;
            }
        }
        return "";
    }
    
    // 2. 语法分析
    auto ast = m_Parser->Parse(tokens);
    
    if (m_Parser->HasErrors()) {
        if (m_DebugMode) {
            for (const auto& error : m_Parser->GetErrors()) {
                std::cerr << error << std::endl;
            }
        }
        return "";
    }
    
    // 3. 代码生成
    return m_Generator->Generate(ast);
}

std::string CHTLCompiler::CompileFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        if (m_DebugMode) {
            std::cerr << "Error: Cannot open file " << filename << std::endl;
        }
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return Compile(buffer.str());
}

std::vector<std::string> CHTLCompiler::GetAllErrors() const {
    std::vector<std::string> allErrors;
    
    // 收集词法分析错误
    auto lexErrors = m_Lexer->GetErrors();
    allErrors.insert(allErrors.end(), lexErrors.begin(), lexErrors.end());
    
    // 收集语法分析错误
    auto parseErrors = m_Parser->GetErrors();
    allErrors.insert(allErrors.end(), parseErrors.begin(), parseErrors.end());
    
    return allErrors;
}

bool CHTLCompiler::HasErrors() const {
    return m_Lexer->HasErrors() || m_Parser->HasErrors();
}

void CHTLCompiler::SetPrettyPrint(bool pretty) {
    m_Generator->SetPrettyPrint(pretty);
}

void CHTLCompiler::SetGenerateFullDocument(bool full) {
    m_Generator->SetGenerateFullDocument(full);
}

void CHTLCompiler::SetDebugMode(bool debug) {
    m_DebugMode = debug;
}

} // namespace simple
} // namespace chtl