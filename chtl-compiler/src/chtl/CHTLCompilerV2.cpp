#include "CHTLCompilerV2.h"
#include "parser/CHTLParserV2.h"
#include "generator/CHTLGeneratorV2.h"
#include "ast/CHTLASTNodes.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace chtl {

CHTLCompilerV2::CHTLCompilerV2() : m_DebugMode(false) {
    m_Parser = std::make_unique<CHTLParserV2>();
    m_Generator = std::make_unique<CHTLGeneratorV2>();
}

CHTLCompilerV2::~CHTLCompilerV2() = default;

std::string CHTLCompilerV2::Compile(const std::string& source, const std::string& filename) {
    m_Errors.clear();
    m_AST = nullptr;
    
    // 解析
    m_AST = m_Parser->Parse(source, filename);
    
    if (m_Parser->HasErrors()) {
        m_Errors = m_Parser->GetErrors();
        if (m_DebugMode) {
            for (const auto& error : m_Errors) {
                std::cerr << error << std::endl;
            }
        }
        return "";
    }
    
    if (!m_AST) {
        m_Errors.push_back("Failed to parse CHTL source");
        return "";
    }
    
    // 生成 HTML
    return m_Generator->Generate(m_AST);
}

std::string CHTLCompilerV2::CompileFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        m_Errors.push_back("Cannot open file: " + filename);
        if (m_DebugMode) {
            std::cerr << "Error: Cannot open file " << filename << std::endl;
        }
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return Compile(buffer.str(), filename);
}

void CHTLCompilerV2::SetPrettyPrint(bool pretty) {
    m_Generator->SetPrettyPrint(pretty);
}

void CHTLCompilerV2::SetGenerateFullDocument(bool full) {
    m_Generator->SetGenerateFullDocument(full);
}

void CHTLCompilerV2::SetDebugMode(bool debug) {
    m_DebugMode = debug;
}

} // namespace chtl