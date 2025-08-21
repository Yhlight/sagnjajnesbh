#ifndef CHTL_ICOMPILER_H
#define CHTL_ICOMPILER_H

#include <string>
#include <vector>

namespace chtl {

/**
 * @brief Base interface for all CHTL compiler components
 */
class ICompiler {
public:
    virtual ~ICompiler() = default;
    
    /**
     * @brief Compile the source code
     * @param source The source code to compile
     * @param filename Optional filename for error reporting
     * @return true if compilation succeeded, false otherwise
     */
    virtual bool Compile(const std::string& source, const std::string& filename = "") = 0;
    
    /**
     * @brief Get the compiled output
     * @return The compiled output as a string
     */
    virtual std::string GetOutput() const = 0;
    
    /**
     * @brief Get compilation errors
     * @return Vector of error messages
     */
    virtual std::vector<std::string> GetErrors() const = 0;
    
    /**
     * @brief Reset the compiler state
     */
    virtual void Reset() = 0;
};

} // namespace chtl

#endif // CHTL_ICOMPILER_H