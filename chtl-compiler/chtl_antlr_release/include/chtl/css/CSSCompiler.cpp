#include "CSSCompiler.h"
#include <antlr4-runtime.h>
#include "../../../third_party/generated/css3/css3Lexer.h"
#include "../../../third_party/generated/css3/css3Parser.h"
#include "../../../third_party/generated/css3/css3BaseVisitor.h"
#include <sstream>

namespace chtl {
namespace css {

// CSS AST Visitor implementation
class CSSVisitor : public css3::css3BaseVisitor {
public:
    CSSVisitor(bool minify = false) : m_Minify(minify) {}
    
    std::string GetOutput() const { return m_Output.str(); }
    
    antlrcpp::Any visitStylesheet(css3::css3Parser::StylesheetContext *ctx) override {
        return visitChildren(ctx);
    }
    
    antlrcpp::Any visitRule(css3::css3Parser::RuleContext *ctx) override {
        return visitChildren(ctx);
    }
    
    // Add more visitor methods as needed
    
private:
    std::stringstream m_Output;
    bool m_Minify;
};

// CSS Error Listener
class CSSErrorListener : public antlr4::BaseErrorListener {
public:
    void syntaxError(antlr4::Recognizer *recognizer, antlr4::Token *offendingSymbol,
                     size_t line, size_t charPositionInLine,
                     const std::string &msg, std::exception_ptr e) override {
        std::stringstream ss;
        ss << "CSS Error at " << line << ":" << charPositionInLine << " - " << msg;
        m_Errors.push_back(ss.str());
    }
    
    const std::vector<std::string>& GetErrors() const { return m_Errors; }
    void ClearErrors() { m_Errors.clear(); }
    
private:
    std::vector<std::string> m_Errors;
};

// CSSCompiler implementation
class CSSCompiler::Impl {
public:
    bool Compile(const std::string& source, const std::string& filename, bool minify) {
        try {
            // Create input stream
            antlr4::ANTLRInputStream input(source);
            
            // Create lexer
            css3::css3Lexer lexer(&input);
            antlr4::CommonTokenStream tokens(&lexer);
            
            // Create parser
            css3::css3Parser parser(&tokens);
            
            // Add error listener
            m_ErrorListener.ClearErrors();
            parser.removeErrorListeners();
            parser.addErrorListener(&m_ErrorListener);
            
            // Parse
            auto tree = parser.stylesheet();
            
            // Check for errors
            if (!m_ErrorListener.GetErrors().empty()) {
                return false;
            }
            
            // Visit tree
            CSSVisitor visitor(minify);
            visitor.visit(tree);
            
            m_Output = visitor.GetOutput();
            return true;
            
        } catch (const std::exception& e) {
            std::vector<std::string> errors = { std::string("CSS Compilation failed: ") + e.what() };
            m_ErrorListener.ClearErrors();
            for (const auto& error : errors) {
                m_ErrorListener.syntaxError(nullptr, nullptr, 0, 0, error, nullptr);
            }
            return false;
        }
    }
    
    std::string GetOutput() const { return m_Output; }
    std::vector<std::string> GetErrors() const { return m_ErrorListener.GetErrors(); }
    
private:
    CSSErrorListener m_ErrorListener;
    std::string m_Output;
};

CSSCompiler::CSSCompiler() : m_Impl(std::make_unique<Impl>()) {}

CSSCompiler::~CSSCompiler() = default;

bool CSSCompiler::Compile(const std::string& source, const std::string& filename) {
    return m_Impl->Compile(source, filename, m_Minify);
}

std::string CSSCompiler::GetOutput() const {
    return m_Impl->GetOutput();
}

std::vector<std::string> CSSCompiler::GetErrors() const {
    return m_Impl->GetErrors();
}

void CSSCompiler::Reset() {
    m_Impl = std::make_unique<Impl>();
}

std::string CSSCompiler::GetMinifiedCSS() const {
    return GetOutput();
}

} // namespace css
} // namespace chtl