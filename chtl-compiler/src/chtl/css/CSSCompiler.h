#ifndef CHTL_CSS_COMPILER_H
#define CHTL_CSS_COMPILER_H

#include <string>
#include <memory>
#include <vector>
#include "../interfaces/ICompiler.h"

namespace chtl {
namespace css {

class CSSCompiler : public ICompiler {
public:
    CSSCompiler();
    ~CSSCompiler() override;
    
    // ICompiler interface
    bool Compile(const std::string& source, const std::string& filename = "") override;
    std::string GetOutput() const override;
    std::vector<std::string> GetErrors() const override;
    void Reset() override;
    
    // CSS specific methods
    std::string GetMinifiedCSS() const;
    void SetMinify(bool minify) { m_Minify = minify; }
    
private:
    class Impl;
    std::unique_ptr<Impl> m_Impl;
    bool m_Minify = false;
};

} // namespace css
} // namespace chtl

#endif // CHTL_CSS_COMPILER_H