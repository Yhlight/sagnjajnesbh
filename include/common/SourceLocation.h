#ifndef CHTL_COMMON_SOURCE_LOCATION_H
#define CHTL_COMMON_SOURCE_LOCATION_H

#include <string>
#include <memory>

namespace chtl {

// 源代码位置信息
class SourceLocation {
public:
    std::string filename;
    size_t startLine;
    size_t startColumn;
    size_t endLine;
    size_t endColumn;
    size_t startOffset;
    size_t endOffset;
    
    SourceLocation()
        : startLine(0), startColumn(0), endLine(0), endColumn(0),
          startOffset(0), endOffset(0) {}
    
    SourceLocation(const std::string& file,
                  size_t sl, size_t sc, size_t el, size_t ec,
                  size_t so, size_t eo)
        : filename(file), startLine(sl), startColumn(sc),
          endLine(el), endColumn(ec), startOffset(so), endOffset(eo) {}
    
    // 从单个位置创建
    SourceLocation(const std::string& file, size_t line, size_t col, size_t offset)
        : filename(file), startLine(line), startColumn(col),
          endLine(line), endColumn(col), startOffset(offset), endOffset(offset) {}
    
    // 合并两个位置
    static SourceLocation merge(const SourceLocation& start, const SourceLocation& end) {
        return SourceLocation(
            start.filename,
            start.startLine, start.startColumn,
            end.endLine, end.endColumn,
            start.startOffset, end.endOffset
        );
    }
    
    // 判断是否有效
    bool isValid() const {
        return startLine > 0 && startColumn > 0;
    }
    
    // 转换为字符串
    std::string toString() const;
};

using SourceLocationPtr = std::shared_ptr<SourceLocation>;

} // namespace chtl

#endif // CHTL_COMMON_SOURCE_LOCATION_H