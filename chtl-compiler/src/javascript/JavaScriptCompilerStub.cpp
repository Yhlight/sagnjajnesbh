#include "javascript/JavaScriptCompiler.h"
#include "utils/Logger.h"
#include <unordered_set>
#include <regex>

namespace chtl {
namespace javascript {

// 暂时的简化实现，待ANTLR集成完成后替换

JavaScriptCompiler::JavaScriptCompiler() 
    : m_Minify(false)
    , m_SourceMap(false)
    , m_StrictMode(false)
    , m_TargetES("ES2015")
    , m_ModuleType("esm") {}

JavaScriptCompiler::~JavaScriptCompiler() = default;

compiler::CompileResult JavaScriptCompiler::Compile(const std::string& sourceCode,
                                                  const std::string& filename) {
    compiler::CompileResult result;
    result.Success = true;
    result.OutputContent = sourceCode; // 暂时直接返回源代码
    
    utils::Logger::GetInstance().Info("JavaScript编译器: 处理 " + filename);
    
    return result;
}

compiler::CompileResult JavaScriptCompiler::CompileFragments(const std::vector<CodeFragment>& fragments) {
    compiler::CompileResult result;
    result.Success = true;
    
    std::stringstream combined;
    for (const auto& fragment : fragments) {
        if (fragment.GetType() == CodeFragmentType::JAVASCRIPT) {
            combined << fragment.GetContent() << "\n";
        }
    }
    
    result.OutputContent = combined.str();
    return result;
}

void JavaScriptCompiler::Reset() {
    ClearErrors();
}

std::unique_ptr<antlr4::tree::ParseTree> JavaScriptCompiler::ParseJS(const std::string& js,
                                                                   const std::string& filename) {
    // 暂时返回空
    return nullptr;
}

std::string JavaScriptCompiler::ProcessParseTree(antlr4::tree::ParseTree* tree) {
    return "";
}

void JavaScriptCompiler::ReportError(const std::string& message,
                                   const std::string& filename,
                                   size_t line,
                                   size_t column) {
    std::stringstream ss;
    ss << "[JS错误] " << message;
    m_Errors.push_back(ss.str());
}

std::string JavaScriptCompiler::MinifyJS(const std::string& js) {
    return js;
}

std::string JavaScriptCompiler::TranspileES(const std::string& js) {
    return js;
}

std::string JavaScriptCompiler::ProcessModules(const std::string& js) {
    return js;
}

std::string JavaScriptCompiler::GenerateSourceMap(const std::string& js,
                                                const std::string& filename) {
    return "";
}

// JSUtils
bool JSUtils::IsReservedWord(const std::string& word) {
    static const std::unordered_set<std::string> reserved = {
        "break", "case", "catch", "class", "const", "continue",
        "debugger", "default", "delete", "do", "else", "export",
        "extends", "finally", "for", "function", "if", "import",
        "in", "instanceof", "let", "new", "return", "super",
        "switch", "this", "throw", "try", "typeof", "var",
        "void", "while", "with", "yield", "async", "await"
    };
    
    return reserved.find(word) != reserved.end();
}

bool JSUtils::IsValidIdentifier(const std::string& identifier) {
    if (identifier.empty()) return false;
    
    char first = identifier[0];
    if (!std::isalpha(first) && first != '_' && first != '$') {
        return false;
    }
    
    return !IsReservedWord(identifier);
}

std::string JSUtils::EscapeString(const std::string& str) {
    return str;
}

std::string JSUtils::NormalizeLineEndings(const std::string& code) {
    return code;
}

bool JSUtils::IsES6Feature(const std::string& feature) {
    return true;
}

bool JSUtils::NeedsSemicolon(const std::string& line) {
    return true;
}

// JSModuleResolver
JSModuleResolver::JSModuleResolver() {}

JSModuleResolver::~JSModuleResolver() = default;

void JSModuleResolver::AddSearchPath(const std::string& path) {
    m_SearchPaths.push_back(path);
}

std::string JSModuleResolver::ResolveModule(const std::string& moduleName,
                                           const std::string& fromFile) {
    return moduleName;
}

std::vector<std::string> JSModuleResolver::GetDependencies(const std::string& code) {
    return {};
}

std::string JSModuleResolver::ConvertModule(const std::string& code,
                                           const std::string& fromType,
                                           const std::string& toType) {
    return code;
}

} // namespace javascript
} // namespace chtl