#include "css/CSSCompiler.h"
#include "utils/Logger.h"

namespace chtl {
namespace css {

// 暂时的简化实现，待ANTLR集成完成后替换

CSSCompiler::CSSCompiler() 
    : m_Minify(false)
    , m_SourceMap(false)
    , m_CompatibilityMode("standard") {}

CSSCompiler::~CSSCompiler() = default;

compiler::CompileResult CSSCompiler::Compile(const std::string& sourceCode,
                                            const std::string& filename) {
    compiler::CompileResult result;
    result.Success = true;
    result.OutputContent = sourceCode; // 暂时直接返回源代码
    
    utils::Logger::GetInstance().Info("CSS编译器: 处理 " + filename);
    
    return result;
}

compiler::CompileResult CSSCompiler::CompileFragments(const std::vector<CodeFragment>& fragments) {
    compiler::CompileResult result;
    result.Success = true;
    
    std::stringstream combined;
    for (const auto& fragment : fragments) {
        if (fragment.GetType() == CodeFragmentType::CSS) {
            combined << fragment.GetContent() << "\n";
        }
    }
    
    result.OutputContent = combined.str();
    return result;
}

void CSSCompiler::Reset() {
    ClearErrors();
}

std::unique_ptr<antlr4::tree::ParseTree> CSSCompiler::ParseCSS(const std::string& css,
                                                              const std::string& filename) {
    // 暂时返回空
    return nullptr;
}

std::string CSSCompiler::ProcessParseTree(antlr4::tree::ParseTree* tree) {
    return "";
}

void CSSCompiler::ReportError(const std::string& message,
                            const std::string& filename,
                            size_t line,
                            size_t column) {
    std::stringstream ss;
    ss << "[CSS错误] " << message;
    m_Errors.push_back(ss.str());
}

std::string CSSCompiler::MinifyCSS(const std::string& css) {
    return css;
}

std::string CSSCompiler::GenerateSourceMap(const std::string& css,
                                         const std::string& filename) {
    return "";
}

std::string CSSCompiler::ApplyCompatibility(const std::string& css) {
    return css;
}

// CSSUtils
bool CSSUtils::IsValidProperty(const std::string& property) {
    return true;
}

bool CSSUtils::IsValidValue(const std::string& property, const std::string& value) {
    return true;
}

std::string CSSUtils::NormalizeColor(const std::string& color) {
    return color;
}

std::string CSSUtils::NormalizeUnit(const std::string& value) {
    return value;
}

bool CSSUtils::CheckCompatibility(const std::string& property,
                                const std::string& compatMode) {
    return true;
}

std::vector<std::string> CSSUtils::AddVendorPrefixes(const std::string& property) {
    return {property};
}

} // namespace css
} // namespace chtl