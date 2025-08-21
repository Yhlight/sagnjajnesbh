#include "CHTLCompilerV3.h"
#include "parser/CHTLParserV3.h"
#include "generator/CHTLGeneratorV3.h"
#include <fstream>
#include <sstream>

namespace chtl {

CHTLCompilerV3::CHTLCompilerV3() 
    : m_Parser(std::make_unique<CHTLParserV3>()),
      m_Generator(std::make_unique<CHTLGeneratorV3>()) {
}

CHTLCompilerV3::~CHTLCompilerV3() = default;

bool CHTLCompilerV3::CompileFile(const std::string& inputFile, 
                                 const std::string& outputFile,
                                 const CompilerOptions& options) {
    m_Options = options;
    m_Errors.clear();
    m_Warnings.clear();
    
    // 读取输入文件
    std::string source = ReadFile(inputFile);
    if (source.empty() && !m_Errors.empty()) {
        return false;
    }
    
    // 解析
    if (!Parse(source, inputFile)) {
        return false;
    }
    
    // 生成
    std::string output = Generate();
    if (output.empty() && !m_Errors.empty()) {
        return false;
    }
    
    // 写入输出文件
    return WriteFile(outputFile, output);
}

std::string CHTLCompilerV3::CompileString(const std::string& source,
                                          const std::string& filename,
                                          const CompilerOptions& options) {
    m_Options = options;
    m_Errors.clear();
    m_Warnings.clear();
    
    // 解析
    if (!Parse(source, filename)) {
        return "";
    }
    
    // 生成
    return Generate();
}

bool CHTLCompilerV3::Parse(const std::string& source, const std::string& filename) {
    // 设置源代码
    m_Parser->SetSource(source);
    
    // 解析
    m_AST = m_Parser->Parse(source, filename);
    
    // 收集错误
    CollectParserErrors();
    
    return m_AST != nullptr && !HasErrors();
}

std::string CHTLCompilerV3::Generate() {
    if (!m_AST) {
        AddError("No AST available for generation");
        return "";
    }
    
    // 设置生成器选项
    GeneratorOptions genOptions;
    genOptions.prettyPrint = m_Options.prettyPrint;
    genOptions.minify = m_Options.minify;
    genOptions.fragmentOnly = m_Options.fragmentOnly;
    genOptions.debugMode = m_Options.debugMode;
    
    // 生成输出
    return m_Generator->Generate(m_AST, genOptions);
}

std::string CHTLCompilerV3::ReadFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        AddError("Failed to open file: " + filename);
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool CHTLCompilerV3::WriteFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        AddError("Failed to create output file: " + filename);
        return false;
    }
    
    file << content;
    return true;
}

void CHTLCompilerV3::CollectParserErrors() {
    if (m_Parser->HasErrors()) {
        for (const auto& error : m_Parser->GetErrors()) {
            AddError(error);
        }
    }
}

void CHTLCompilerV3::AddError(const std::string& error) {
    m_Errors.push_back(error);
}

void CHTLCompilerV3::AddWarning(const std::string& warning) {
    m_Warnings.push_back(warning);
}

} // namespace chtl