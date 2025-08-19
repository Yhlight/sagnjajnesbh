#include "FinalCJMODInterface.h"
#include <sstream>
#include <algorithm>
#include <cctype>

namespace final_cjmod_design {

// 全局scanner实例
Scanner scanner;

// ========================================
// 主要函数实现
// ========================================

Keyword syntaxAnalys(const std::string& pattern, const std::string& ignoreChars) {
    Keyword keyword(pattern);
    
    // 简化的标记化
    std::string current = "";
    for (char c : pattern) {
        if (std::isspace(c) || ignoreChars.find(c) != std::string::npos) {
            if (!current.empty()) {
                keyword.addArg(current, current);
                current = "";
            }
        } else {
            current += c;
        }
    }
    
    if (!current.empty()) {
        keyword.addArg(current, current);
    }
    
    return keyword;
}

std::string generateCode(const Keyword& keyword) {
    std::string result = keyword.args.result();
    
    // 为printMylove添加完整实现
    if (keyword.originalPattern.find("printMylove") != std::string::npos) {
        result += R"(
// printMylove核心实现
function printMyloveCore(imageUrl, mode, width, height, scale) {
    const canvas = document.createElement('canvas');
    const ctx = canvas.getContext('2d');
    
    const img = new Image();
    img.onload = function() {
        canvas.width = width;
        canvas.height = height;
        ctx.drawImage(img, 0, 0, width, height);
        
        const imageData = ctx.getImageData(0, 0, width, height);
        const pixels = imageData.data;
        
        let output = '';
        
        if (mode === 'ascii') {
            // ASCII字符画 - 使用符号
            const chars = ['.', ':', '*', '#', '@'];
            for (let y = 0; y < height; y++) {
                for (let x = 0; x < width; x++) {
                    const i = (y * width + x) * 4;
                    const brightness = (pixels[i] + pixels[i+1] + pixels[i+2]) / 3;
                    const charIndex = Math.floor(brightness / 255 * (chars.length - 1));
                    output += chars[charIndex];
                }
                output += '\n';
            }
        } else if (mode === 'pixel') {
            // Pixel像素画 - 使用像素块
            const chars = ['█', '▓', '▒', '░'];
            for (let y = 0; y < height; y++) {
                for (let x = 0; x < width; x++) {
                    const i = (y * width + x) * 4;
                    const brightness = (pixels[i] + pixels[i+1] + pixels[i+2]) / 3;
                    const charIndex = Math.floor(brightness / 255 * (chars.length - 1));
                    output += chars[charIndex];
                }
                output += '\n';
            }
        }
        
        console.log(output);
        return output;
    };
    
    img.src = imageUrl;
}

// 调用printMylove
printMyloveCore(imageUrl, renderMode, width, height, scale);
)";
    }
    
    return result;
}

// ========================================
// 辅助函数实现
// ========================================

bool isObject(const std::string& value) {
    if (value.empty()) return false;
    
    std::string trimmed = value;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
    
    return !trimmed.empty() && trimmed.front() == '{' && trimmed.back() == '}';
}

bool isFunction(const std::string& value) {
    return value.find("function") != std::string::npos || 
           value.find("=>") != std::string::npos ||
           (value.find("(") != std::string::npos && 
            value.find(")") != std::string::npos && 
            value.find("{") != std::string::npos);
}

std::string slice(const std::string& content, int start, int length) {
    if (start < 0 || start >= content.length()) {
        return "";
    }
    
    if (length == -1) {
        return content.substr(start);
    }
    
    if (start + length > content.length()) {
        length = content.length() - start;
    }
    
    return content.substr(start, length);
}

} // namespace final_cjmod_design