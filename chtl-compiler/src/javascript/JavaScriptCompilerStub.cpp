#include "javascript/JavaScriptCompiler.h"
#include "utils/Logger.h"
#include <sstream>
#include <algorithm>

namespace chtl {
namespace javascript {

// 暂时的简化实现，待ANTLR集成完成后替换

JavaScriptCompiler::JavaScriptCompiler()
    : m_Debug(false)
    , m_Minify(false)
    , m_SourceMap(false)
    , m_StrictMode(false)
    , m_TargetES("ES6")
    , m_ModuleType("commonjs") {}

JavaScriptCompiler::~JavaScriptCompiler() = default;

CompileResult JavaScriptCompiler::Compile(const CodeFragment& fragment) {
    CompileResult result;
    result.success = true;
    result.output = fragment.GetContent(); // 暂时直接返回源代码
    
    utils::Logger::GetInstance().Info("JavaScript编译器: 处理代码片段");
    
    return result;
}

CompileResult JavaScriptCompiler::CompileFragments(const std::vector<CodeFragment>& fragments) {
    CompileResult result;
    result.success = true;
    
    std::stringstream combined;
    for (const auto& fragment : fragments) {
        if (fragment.GetType() == FragmentType::JAVASCRIPT) {
            combined << fragment.GetContent() << "\n";
        }
    }
    
    result.output = combined.str();
    return result;
}

void JavaScriptCompiler::Reset() {
    ClearErrors();
}

// SetDebugMode and GetName are defined inline in the header

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
    if (!filename.empty()) {
        ss << " in " << filename;
    }
    if (line > 0) {
        ss << " at line " << line << ":" << column;
    }
    m_Errors.push_back(ss.str());
}

std::string JavaScriptCompiler::MinifyJS(const std::string& js) {
    if (!m_Minify) {
        return js;
    }
    
    // TODO: 实现真正的JS最小化
    return js;
}

std::string JavaScriptCompiler::TranspileES(const std::string& js) {
    // TODO: 实现ES版本转换
    return js;
}

std::string JavaScriptCompiler::ProcessModules(const std::string& js) {
    // TODO: 实现模块处理
    return js;
}

std::string JavaScriptCompiler::GenerateSourceMap(const std::string& js,
                                                 const std::string& filename) {
    if (!m_SourceMap) {
        return "";
    }
    
    // TODO: 实现source map生成
    return "";
}

// JSUtils
bool JSUtils::IsReservedWord(const std::string& word) {
    // TODO: 实现保留字检查
    static const std::vector<std::string> reservedWords = {
        "break", "case", "catch", "class", "const", "continue",
        "debugger", "default", "delete", "do", "else", "export",
        "extends", "finally", "for", "function", "if", "import",
        "in", "instanceof", "let", "new", "return", "super",
        "switch", "this", "throw", "try", "typeof", "var",
        "void", "while", "with", "yield"
    };
    
    return std::find(reservedWords.begin(), reservedWords.end(), word) != reservedWords.end();
}

bool JSUtils::IsValidIdentifier(const std::string& identifier) {
    // TODO: 实现标识符验证
    if (identifier.empty()) return false;
    
    // 简单验证
    char first = identifier[0];
    if (!std::isalpha(first) && first != '_' && first != '$') {
        return false;
    }
    
    return true;
}

std::string JSUtils::EscapeString(const std::string& str) {
    // TODO: 实现字符串转义
    std::stringstream ss;
    for (char c : str) {
        switch (c) {
            case '"': ss << "\\\""; break;
            case '\\': ss << "\\\\"; break;
            case '\n': ss << "\\n"; break;
            case '\r': ss << "\\r"; break;
            case '\t': ss << "\\t"; break;
            default: ss << c; break;
        }
    }
    return ss.str();
}

std::string JSUtils::NormalizeLineEndings(const std::string& code) {
    // TODO: 实现行结束符规范化
    return code;
}

bool JSUtils::IsES6Feature(const std::string& feature) {
    // TODO: 实现ES6特性检查
    return true;
}

bool JSUtils::IsES2015Feature(const std::string& feature) {
    return IsES6Feature(feature);
}

bool JSUtils::IsES2017Feature(const std::string& feature) {
    // TODO: 实现ES2017特性检查
    return true;
}

bool JSUtils::NeedsSemicolon(const std::string& line) {
    // TODO: 实现自动分号插入判断
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
    // TODO: 实现模块解析
    return moduleName;
}

std::vector<std::string> JSModuleResolver::GetDependencies(const std::string& code) {
    std::vector<std::string> deps;
    // TODO: 实现依赖解析
    return deps;
}

std::string JSModuleResolver::ConvertModule(const std::string& code,
                                          const std::string& fromType,
                                          const std::string& toType) {
    // TODO: 实现模块格式转换
    return code;
}

void JSModuleResolver::ParseImports(const std::string& code,
                                  std::vector<std::string>& imports) {
    // TODO: 实现import解析
}

void JSModuleResolver::ParseExports(const std::string& code,
                                  std::vector<std::string>& exports) {
    // TODO: 实现export解析
}

} // namespace javascript
} // namespace chtl