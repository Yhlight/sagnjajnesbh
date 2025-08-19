#ifndef CHTL_SLICE_BUFFER_H
#define CHTL_SLICE_BUFFER_H

#include <string>
#include <vector>
#include <memory>

namespace chtl {

/**
 * 切片缓冲区 - 支持可变长度的代码片段读取
 */
class SliceBuffer {
public:
    SliceBuffer(const std::string& source, size_t initialSliceSize = 1024);
    ~SliceBuffer();
    
    /**
     * 获取当前切片内容
     */
    std::string getCurrentSlice() const;
    
    /**
     * 扩展当前切片
     * @param expandSize 扩展的字节数
     * @return 是否成功扩展（到达文件末尾时返回false）
     */
    bool expandSlice(size_t expandSize);
    
    /**
     * 移动到下一个切片
     * @param offset 从当前位置开始的偏移量
     */
    void moveToNext(size_t offset);
    
    /**
     * 获取当前切片在源代码中的起始位置
     */
    size_t getSliceStart() const { return sliceStart_; }
    
    /**
     * 获取当前切片的结束位置
     */
    size_t getSliceEnd() const { return sliceEnd_; }
    
    /**
     * 检查是否到达源代码末尾
     */
    bool isAtEnd() const { return sliceStart_ >= source_.length(); }
    
    /**
     * 获取指定位置的行号和列号
     */
    std::pair<size_t, size_t> getLineColumn(size_t position) const;
    
    /**
     * 预览下一个切片的内容（不移动位置）
     * @param previewSize 预览的字节数
     */
    std::string peekNextSlice(size_t previewSize) const;
    
private:
    std::string source_;
    size_t sliceStart_;
    size_t sliceEnd_;
    size_t defaultSliceSize_;
    
    // 行号缓存，用于快速定位
    mutable std::vector<size_t> lineStarts_;
    mutable bool linesCached_;
    
    void buildLineCache() const;
};

} // namespace chtl

#endif // CHTL_SLICE_BUFFER_H