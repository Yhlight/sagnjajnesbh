#include "../../include/common/SourceLocation.h"
#include <sstream>

namespace chtl {

std::string SourceLocation::toString() const {
    std::stringstream ss;
    
    if (!filename.empty()) {
        ss << filename << ":";
    }
    
    ss << startLine << ":" << startColumn;
    
    if (endLine != startLine || endColumn != startColumn) {
        ss << "-";
        if (endLine != startLine) {
            ss << endLine << ":";
        }
        ss << endColumn;
    }
    
    return ss.str();
}

} // namespace chtl