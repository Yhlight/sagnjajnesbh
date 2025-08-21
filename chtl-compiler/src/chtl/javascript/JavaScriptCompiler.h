#ifndef CHTL_JAVASCRIPT_COMPILER_H
#define CHTL_JAVASCRIPT_COMPILER_H

#include <string>
#include <memory>
#include <vector>
#include "../interfaces/ICompiler.h"

namespace chtl {
namespace javascript {

class JavaScriptCompiler : public ICompiler {
public:
    JavaScriptCompiler();
    ~JavaScriptCompiler() override;
    
    // ICompiler interface
    bool Compile(const std::string& source, const std::string& filename = "") override;
    std::string GetOutput() const override;
    std::vector<std::string> GetErrors() const override;
    void Reset() override;
    
    // JavaScript specific methods
    std::string GetMinifiedJS() const;
    void SetMinify(bool minify) { m_Minify = minify; }
    void SetStrictMode(bool strict) { m_StrictMode = strict; }
    
private:
    class Impl;
    std::unique_ptr<Impl> m_Impl;
    bool m_Minify = false;
    bool m_StrictMode = true;
};

} // namespace javascript
} // namespace chtl

#endif // CHTL_JAVASCRIPT_COMPILER_H