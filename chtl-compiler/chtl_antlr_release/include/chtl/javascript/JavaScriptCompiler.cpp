#include "JavaScriptCompiler.h"
#include <antlr4-runtime.h>
#include "../../../third_party/generated/javascript/JavaScriptLexer.h"
#include "../../../third_party/generated/javascript/JavaScriptParser.h"
#include "../../../third_party/generated/javascript/JavaScriptParserBaseVisitor.h"
#include <sstream>

namespace chtl {
namespace javascript {

// JavaScript AST Visitor implementation
class JSVisitor : public javascript::JavaScriptParserBaseVisitor {
public:
    JSVisitor(bool minify = false, bool strict = true) 
        : m_Minify(minify), m_StrictMode(strict) {}
    
    std::string GetOutput() const { return m_Output.str(); }
    
    antlrcpp::Any visitProgram(javascript::JavaScriptParser::ProgramContext *ctx) override {
        if (m_StrictMode) {
            m_Output << "\"use strict\";" << (m_Minify ? "" : "\n");
        }
        return visitChildren(ctx);
    }
    
    antlrcpp::Any visitStatement(javascript::JavaScriptParser::StatementContext *ctx) override {
        return visitChildren(ctx);
    }
    
    // Add more visitor methods as needed
    
private:
    std::stringstream m_Output;
    bool m_Minify;
    bool m_StrictMode;
};

// JavaScript Error Listener
class JSErrorListener : public antlr4::BaseErrorListener {
public:
    void syntaxError(antlr4::Recognizer *recognizer, antlr4::Token *offendingSymbol,
                     size_t line, size_t charPositionInLine,
                     const std::string &msg, std::exception_ptr e) override {
        std::stringstream ss;
        ss << "JavaScript Error at " << line << ":" << charPositionInLine << " - " << msg;
        m_Errors.push_back(ss.str());
    }
    
    const std::vector<std::string>& GetErrors() const { return m_Errors; }
    void ClearErrors() { m_Errors.clear(); }
    
private:
    std::vector<std::string> m_Errors;
};

// JavaScriptCompiler implementation
class JavaScriptCompiler::Impl {
public:
    bool Compile(const std::string& source, const std::string& filename, bool minify, bool strict) {
        try {
            // Create input stream
            antlr4::ANTLRInputStream input(source);
            
            // Create lexer
            javascript::JavaScriptLexer lexer(&input);
            antlr4::CommonTokenStream tokens(&lexer);
            
            // Create parser
            javascript::JavaScriptParser parser(&tokens);
            
            // Add error listener
            m_ErrorListener.ClearErrors();
            parser.removeErrorListeners();
            parser.addErrorListener(&m_ErrorListener);
            
            // Parse
            auto tree = parser.program();
            
            // Check for errors
            if (!m_ErrorListener.GetErrors().empty()) {
                return false;
            }
            
            // Visit tree
            JSVisitor visitor(minify, strict);
            visitor.visit(tree);
            
            m_Output = visitor.GetOutput();
            return true;
            
        } catch (const std::exception& e) {
            std::vector<std::string> errors = { std::string("JavaScript Compilation failed: ") + e.what() };
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
    JSErrorListener m_ErrorListener;
    std::string m_Output;
};

JavaScriptCompiler::JavaScriptCompiler() : m_Impl(std::make_unique<Impl>()) {}

JavaScriptCompiler::~JavaScriptCompiler() = default;

bool JavaScriptCompiler::Compile(const std::string& source, const std::string& filename) {
    return m_Impl->Compile(source, filename, m_Minify, m_StrictMode);
}

std::string JavaScriptCompiler::GetOutput() const {
    return m_Impl->GetOutput();
}

std::vector<std::string> JavaScriptCompiler::GetErrors() const {
    return m_Impl->GetErrors();
}

void JavaScriptCompiler::Reset() {
    m_Impl = std::make_unique<Impl>();
}

std::string JavaScriptCompiler::GetMinifiedJS() const {
    return GetOutput();
}

} // namespace javascript
} // namespace chtl