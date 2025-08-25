#include "Scanner/UnifiedScanner.h"
#include <fstream>
#include <sstream>
#include <regex>
#include <iostream>

namespace CHTL {
namespace Scanner {

// 扫描器实现类
class UnifiedScanner::Impl {
public:
    ScannerConfig config;
    
    // 扫描状态
    struct ScanState {
        size_t line = 1;
        size_t column = 1;
        size_t pos = 0;
        FragmentType currentType = FragmentType::CHTL;
        bool inStyle = false;
        bool inScript = false;
        bool inCHTLJS = false;
        int braceDepth = 0;
        std::string buffer;
    };
    
    explicit Impl(const ScannerConfig& cfg) : config(cfg) {}
    
    std::vector<CodeFragment> Scan(const std::string& sourceCode) {
        std::vector<CodeFragment> fragments;
        ScanState state;
        
        while (state.pos < sourceCode.length()) {
            size_t sliceSize = config.initialSliceSize;
            std::string slice = GetSlice(sourceCode, state.pos, sliceSize);
            
            // 检查切片边界是否合理
            while (NeedExpandSlice(slice, sourceCode, state.pos + slice.length()) && 
                   sliceSize < config.maxSliceSize) {
                sliceSize *= 2;
                slice = GetSlice(sourceCode, state.pos, sliceSize);
            }
            
            // 处理切片
            ProcessSlice(slice, state, fragments);
            
            // 更新位置
            UpdatePosition(slice, state);
            state.pos += slice.length();
        }
        
        // 处理剩余缓冲区
        if (!state.buffer.empty()) {
            FlushBuffer(state, fragments);
        }
        
        return fragments;
    }
    
private:
    std::string GetSlice(const std::string& source, size_t start, size_t size) {
        size_t actualSize = std::min(size, source.length() - start);
        return source.substr(start, actualSize);
    }
    
    bool NeedExpandSlice(const std::string& slice, const std::string& source, size_t nextPos) {
        if (nextPos >= source.length()) return false;
        
        // 检查是否在CHTL或CHTL JS语法中间
        if (slice.length() >= 2) {
            std::string tail = slice.substr(slice.length() - 2);
            
            // 检查 {{ 开始
            if (tail == "{{" || (tail[1] == '{' && nextPos < source.length() && source[nextPos] == '{')) {
                return true;
            }
            
            // 检查 -> 操作符
            if (tail == "->" || (tail[1] == '-' && nextPos < source.length() && source[nextPos] == '>')) {
                return true;
            }
        }
        
        // 检查是否在关键字中间
        if (IsInKeyword(slice)) {
            return true;
        }
        
        return false;
    }
    
    bool IsInKeyword(const std::string& slice) {
        static const std::vector<std::string> keywords = {
            "[Template]", "[Custom]", "[Origin]", "[Import]", "[Namespace]",
            "[Configuration]", "@Style", "@Element", "@Var", "@Html", 
            "@JavaScript", "@Chtl", "@CJmod", "style", "script", "text"
        };
        
        for (const auto& keyword : keywords) {
            if (slice.length() < keyword.length()) {
                std::string tail = slice.substr(std::max(0, (int)slice.length() - (int)keyword.length()));
                if (keyword.find(tail) == 0 && tail.length() < keyword.length()) {
                    return true;
                }
            }
        }
        
        return false;
    }
    
    void ProcessSlice(const std::string& slice, ScanState& state, std::vector<CodeFragment>& fragments) {
        for (size_t i = 0; i < slice.length(); ++i) {
            char ch = slice[i];
            state.buffer += ch;
            
            // 检测片段类型切换
            if (DetectTypeSwitch(state, i, slice)) {
                // 保存当前片段
                if (state.buffer.length() > 1) {
                    std::string content = state.buffer.substr(0, state.buffer.length() - 1);
                    if (!content.empty()) {
                        SaveFragment(content, state, fragments);
                    }
                    state.buffer = std::string(1, ch);
                }
            }
        }
    }
    
    bool DetectTypeSwitch(ScanState& state, size_t pos, const std::string& slice) {
        // 检测style块
        if (state.buffer.length() >= 5 && 
            state.buffer.substr(state.buffer.length() - 5) == "style" && 
            pos + 1 < slice.length() && 
            (slice[pos + 1] == '{' || std::isspace(slice[pos + 1]))) {
            if (!state.inScript) {
                state.inStyle = true;
                state.currentType = FragmentType::CSS;
                return true;
            }
        }
        
        // 检测script块
        if (state.buffer.length() >= 6 && 
            state.buffer.substr(state.buffer.length() - 6) == "script" && 
            pos + 1 < slice.length() && 
            (slice[pos + 1] == '{' || std::isspace(slice[pos + 1]))) {
            if (!state.inStyle) {
                state.inScript = true;
                state.currentType = FragmentType::JavaScript;
                return true;
            }
        }
        
        // 检测CHTL JS语法 {{
        if (state.buffer.length() >= 2 && 
            state.buffer.substr(state.buffer.length() - 2) == "{{") {
            state.inCHTLJS = true;
            state.currentType = FragmentType::CHTLJS;
            return true;
        }
        
        // 检测CHTL JS语法结束 }}
        if (state.inCHTLJS && state.buffer.length() >= 2 && 
            state.buffer.substr(state.buffer.length() - 2) == "}}") {
            state.inCHTLJS = false;
            state.currentType = state.inScript ? FragmentType::JavaScript : FragmentType::CHTL;
            return true;
        }
        
        // 跟踪大括号深度
        if (slice[pos] == '{') {
            state.braceDepth++;
        } else if (slice[pos] == '}') {
            state.braceDepth--;
            if (state.braceDepth == 0) {
                if (state.inStyle) {
                    state.inStyle = false;
                    state.currentType = FragmentType::CHTL;
                    return true;
                } else if (state.inScript) {
                    state.inScript = false;
                    state.currentType = FragmentType::CHTL;
                    return true;
                }
            }
        }
        
        return false;
    }
    
    void SaveFragment(const std::string& content, ScanState& state, std::vector<CodeFragment>& fragments) {
        if (content.empty()) return;
        
        fragments.emplace_back(
            state.currentType,
            content,
            state.line,
            state.column,
            state.line,  // TODO: 计算准确的结束位置
            state.column + content.length()
        );
    }
    
    void FlushBuffer(ScanState& state, std::vector<CodeFragment>& fragments) {
        SaveFragment(state.buffer, state, fragments);
        state.buffer.clear();
    }
    
    void UpdatePosition(const std::string& slice, ScanState& state) {
        for (char ch : slice) {
            if (ch == '\n') {
                state.line++;
                state.column = 1;
            } else {
                state.column++;
            }
        }
    }
};

// UnifiedScanner 公共接口实现
UnifiedScanner::UnifiedScanner(const ScannerConfig& config) 
    : pImpl(std::make_unique<Impl>(config)) {}

UnifiedScanner::~UnifiedScanner() = default;

std::vector<CodeFragment> UnifiedScanner::Scan(const std::string& sourceCode) {
    return pImpl->Scan(sourceCode);
}

std::vector<CodeFragment> UnifiedScanner::ScanFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("无法打开文件: " + filePath);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return Scan(buffer.str());
}

} // namespace Scanner
} // namespace CHTL