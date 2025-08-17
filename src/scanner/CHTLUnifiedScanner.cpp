#include "../../include/CHTLUnifiedScanner.h"
#include <iostream>

namespace chtl {

CHTLUnifiedScanner::CHTLUnifiedScanner(const CompilerOptions& options) 
    : options_(options), debugMode_(options.debugMode) {
    if (debugMode_) {
        std::cout << "[CHTLUnifiedScanner] 初始化完成" << std::endl;
    }
}

ScanResult CHTLUnifiedScanner::scan(const String& source, const String& filename) {
    ScanResult result;
    result.success = true;
    
    // 简化实现 - 将整个源代码作为一个CHTL片段
    CodeFragment fragment;
    fragment.type = FragmentType::CHTL;
    fragment.content = source;
    fragment.startLine = 1;
    fragment.startColumn = 1;
    fragment.endLine = 1;
    fragment.endColumn = source.length();
    
    result.fragments.push_back(fragment);
    
    if (debugMode_) {
        std::cout << "[CHTLUnifiedScanner] 扫描完成，生成 " << result.fragments.size() << " 个片段" << std::endl;
    }
    
    return result;
}

} // namespace chtl