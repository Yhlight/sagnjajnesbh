#include "scanner/slice_buffer.h"
#include <algorithm>

namespace chtl {

SliceBuffer::SliceBuffer(const std::string& source, size_t initialSliceSize)
    : source_(source), 
      sliceStart_(0), 
      sliceEnd_(std::min(initialSliceSize, source.length())),
      defaultSliceSize_(initialSliceSize),
      linesCached_(false) {
}

SliceBuffer::~SliceBuffer() = default;

std::string SliceBuffer::getCurrentSlice() const {
    if (sliceStart_ >= source_.length()) {
        return "";
    }
    return source_.substr(sliceStart_, sliceEnd_ - sliceStart_);
}

bool SliceBuffer::expandSlice(size_t expandSize) {
    if (sliceEnd_ >= source_.length()) {
        return false;
    }
    
    size_t newEnd = std::min(sliceEnd_ + expandSize, source_.length());
    if (newEnd > sliceEnd_) {
        sliceEnd_ = newEnd;
        return true;
    }
    
    return false;
}

void SliceBuffer::moveToNext(size_t offset) {
    sliceStart_ = std::min(sliceStart_ + offset, source_.length());
    sliceEnd_ = std::min(sliceStart_ + defaultSliceSize_, source_.length());
}

std::pair<size_t, size_t> SliceBuffer::getLineColumn(size_t position) const {
    if (!linesCached_) {
        buildLineCache();
    }
    
    // 二分查找行号
    auto it = std::upper_bound(lineStarts_.begin(), lineStarts_.end(), position);
    size_t line = std::distance(lineStarts_.begin(), it);
    
    if (line == 0) {
        return {1, position + 1};
    }
    
    size_t lineStart = lineStarts_[line - 1];
    return {line, position - lineStart + 1};
}

std::string SliceBuffer::peekNextSlice(size_t previewSize) const {
    if (sliceEnd_ >= source_.length()) {
        return "";
    }
    
    size_t peekEnd = std::min(sliceEnd_ + previewSize, source_.length());
    return source_.substr(sliceEnd_, peekEnd - sliceEnd_);
}

void SliceBuffer::buildLineCache() const {
    lineStarts_.clear();
    lineStarts_.push_back(0);
    
    for (size_t i = 0; i < source_.length(); ++i) {
        if (source_[i] == '\n') {
            lineStarts_.push_back(i + 1);
        }
    }
    
    linesCached_ = true;
}

} // namespace chtl