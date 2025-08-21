#ifndef CHTL_ANTLR_PARSERS_H
#define CHTL_ANTLR_PARSERS_H

// Single header version of CHTL ANTLR parsers
// This provides stub implementations for development/testing

#include <string>
#include <vector>
#include <memory>

namespace chtl {

// Base compiler interface
class ICompiler {
public:
    virtual ~ICompiler() = default;
    virtual bool Compile(const std::string& source, const std::string& filename = "") = 0;
    virtual std::string GetOutput() const = 0;
    virtual std::vector<std::string> GetErrors() const = 0;
    virtual void Reset() = 0;
};

namespace css {

// CSS Compiler stub implementation
class CSSCompiler : public ICompiler {
public:
    CSSCompiler() {}
    ~CSSCompiler() override = default;
    
    bool Compile(const std::string& source, const std::string& filename = "") override {
        m_Source = source;
        m_Filename = filename;
        
        // Basic CSS validation
        if (source.empty()) {
            m_Errors.push_back("Empty CSS source");
            return false;
        }
        
        // Simple check for basic CSS structure
        if (source.find("{") == std::string::npos || source.find("}") == std::string::npos) {
            m_Errors.push_back("Invalid CSS structure");
            return false;
        }
        
        // For development: just return the source as-is
        m_Output = source;
        return true;
    }
    
    std::string GetOutput() const override {
        return m_Output;
    }
    
    std::vector<std::string> GetErrors() const override {
        return m_Errors;
    }
    
    void Reset() override {
        m_Source.clear();
        m_Output.clear();
        m_Errors.clear();
        m_Filename.clear();
    }
    
    void SetMinify(bool minify) { m_Minify = minify; }
    std::string GetMinifiedCSS() const { return m_Output; }
    
private:
    std::string m_Source;
    std::string m_Output;
    std::string m_Filename;
    std::vector<std::string> m_Errors;
    bool m_Minify = false;
};

} // namespace css

namespace javascript {

// JavaScript Compiler stub implementation
class JavaScriptCompiler : public ICompiler {
public:
    JavaScriptCompiler() {}
    ~JavaScriptCompiler() override = default;
    
    bool Compile(const std::string& source, const std::string& filename = "") override {
        m_Source = source;
        m_Filename = filename;
        
        // Basic JavaScript validation
        if (source.empty()) {
            m_Errors.push_back("Empty JavaScript source");
            return false;
        }
        
        // Add strict mode if requested
        if (m_StrictMode && source.find("\"use strict\"") == std::string::npos) {
            m_Output = "\"use strict\";\n" + source;
        } else {
            m_Output = source;
        }
        
        return true;
    }
    
    std::string GetOutput() const override {
        return m_Output;
    }
    
    std::vector<std::string> GetErrors() const override {
        return m_Errors;
    }
    
    void Reset() override {
        m_Source.clear();
        m_Output.clear();
        m_Errors.clear();
        m_Filename.clear();
    }
    
    void SetMinify(bool minify) { m_Minify = minify; }
    void SetStrictMode(bool strict) { m_StrictMode = strict; }
    std::string GetMinifiedJS() const { return m_Output; }
    
private:
    std::string m_Source;
    std::string m_Output;
    std::string m_Filename;
    std::vector<std::string> m_Errors;
    bool m_Minify = false;
    bool m_StrictMode = true;
};

} // namespace javascript
} // namespace chtl

#ifdef CHTL_ANTLR_IMPLEMENTATION
// Note: Real implementation requires linking with ANTLR libraries
// This is just a stub for development
#endif

#endif // CHTL_ANTLR_PARSERS_H