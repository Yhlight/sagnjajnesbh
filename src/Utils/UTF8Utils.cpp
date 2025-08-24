#include "Utils/UTF8Utils.h"
#include <iostream>
#include <algorithm>

namespace CHTL {
namespace Utils {

bool UTF8Utils::IsValidUTF8(const std::string& str) {
    size_t i = 0;
    while (i < str.length()) {
        unsigned char byte = static_cast<unsigned char>(str[i]);
        size_t charSize = GetUTF8CharSize(byte);
        
        if (charSize == 0) {
            return false; // 无效的UTF-8起始字节
        }
        
        if (i + charSize > str.length()) {
            return false; // 字符串长度不足
        }
        
        // 检查后续字节
        for (size_t j = 1; j < charSize; j++) {
            unsigned char continuationByte = static_cast<unsigned char>(str[i + j]);
            if ((continuationByte & 0xC0) != 0x80) {
                return false; // 无效的续航字节
            }
        }
        
        i += charSize;
    }
    
    return true;
}

std::wstring UTF8Utils::UTF8ToWString(const std::string& utf8str) {
    try {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.from_bytes(utf8str);
    } catch (const std::exception&) {
        // 转换失败，返回空字符串
        return L"";
    }
}

std::string UTF8Utils::WStringToUTF8(const std::wstring& wstr) {
    try {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.to_bytes(wstr);
    } catch (const std::exception&) {
        // 转换失败，返回空字符串
        return "";
    }
}

size_t UTF8Utils::UTF8Length(const std::string& utf8str) {
    size_t length = 0;
    size_t i = 0;
    
    while (i < utf8str.length()) {
        unsigned char byte = static_cast<unsigned char>(utf8str[i]);
        size_t charSize = GetUTF8CharSize(byte);
        
        if (charSize == 0) {
            // 无效UTF-8，跳过这个字节
            i++;
            continue;
        }
        
        if (i + charSize > utf8str.length()) {
            // 字符串长度不足，结束
            break;
        }
        
        i += charSize;
        length++;
    }
    
    return length;
}

std::string UTF8Utils::UTF8Substr(const std::string& utf8str, size_t start, size_t length) {
    if (start >= UTF8Length(utf8str)) {
        return "";
    }
    
    size_t currentChar = 0;
    size_t startByte = 0;
    size_t i = 0;
    
    // 找到起始字符的字节位置
    while (i < utf8str.length() && currentChar < start) {
        unsigned char byte = static_cast<unsigned char>(utf8str[i]);
        size_t charSize = GetUTF8CharSize(byte);
        
        if (charSize == 0) {
            i++;
            continue;
        }
        
        if (currentChar == start) {
            startByte = i;
            break;
        }
        
        i += charSize;
        currentChar++;
    }
    
    if (currentChar < start) {
        return ""; // 起始位置超出字符串
    }
    
    startByte = i;
    
    // 找到结束字符的字节位置
    size_t endByte = utf8str.length();
    if (length != std::string::npos) {
        size_t targetLength = currentChar + length;
        while (i < utf8str.length() && currentChar < targetLength) {
            unsigned char byte = static_cast<unsigned char>(utf8str[i]);
            size_t charSize = GetUTF8CharSize(byte);
            
            if (charSize == 0) {
                i++;
                continue;
            }
            
            i += charSize;
            currentChar++;
        }
        endByte = i;
    }
    
    return utf8str.substr(startByte, endByte - startByte);
}

std::string UTF8Utils::FixUTF8String(const std::string& str) {
    std::string result;
    size_t i = 0;
    
    while (i < str.length()) {
        unsigned char byte = static_cast<unsigned char>(str[i]);
        size_t charSize = GetUTF8CharSize(byte);
        
        if (charSize == 0) {
            // 无效的UTF-8字节，替换为替换字符
            result += "\uFFFD"; // UTF-8替换字符
            i++;
            continue;
        }
        
        if (i + charSize > str.length()) {
            // 字符串长度不足，截断
            result += "\uFFFD";
            break;
        }
        
        // 检查是否是完整的UTF-8字符
        bool isValid = true;
        for (size_t j = 1; j < charSize; j++) {
            unsigned char continuationByte = static_cast<unsigned char>(str[i + j]);
            if ((continuationByte & 0xC0) != 0x80) {
                isValid = false;
                break;
            }
        }
        
        if (isValid) {
            result += str.substr(i, charSize);
        } else {
            result += "\uFFFD";
        }
        
        i += charSize;
    }
    
    return result;
}

bool UTF8Utils::IsChineseCharacter(const char* utf8char, size_t size) {
    if (size < 3) return false; // 中文字符至少3字节
    
    unsigned char byte1 = static_cast<unsigned char>(utf8char[0]);
    unsigned char byte2 = static_cast<unsigned char>(utf8char[1]);
    unsigned char byte3 = static_cast<unsigned char>(utf8char[2]);
    
    // 检查常用中文Unicode范围
    // U+4E00-U+9FFF: CJK统一汉字
    // U+3400-U+4DBF: CJK扩展A
    // U+F900-U+FAFF: CJK兼容汉字
    
    if (size == 3) {
        // 3字节UTF-8字符
        if (byte1 == 0xE4 && byte2 >= 0xB8 && byte2 <= 0xBF) {
            return true; // U+4E00-U+4FFF
        }
        if (byte1 >= 0xE5 && byte1 <= 0xE9) {
            return true; // U+5000-U+9FFF
        }
        if (byte1 == 0xE3 && byte2 >= 0x90 && byte2 <= 0x9F) {
            return true; // U+3400-U+3FFF
        }
        if (byte1 == 0xEF && byte2 >= 0xA4 && byte2 <= 0xAB) {
            return true; // U+F900-U+FAFF
        }
    }
    
    return false;
}

size_t UTF8Utils::GetUTF8CharSize(unsigned char byte) {
    if ((byte & 0x80) == 0) {
        return 1; // ASCII字符
    } else if ((byte & 0xE0) == 0xC0) {
        return 2; // 2字节UTF-8字符
    } else if ((byte & 0xF0) == 0xE0) {
        return 3; // 3字节UTF-8字符
    } else if ((byte & 0xF8) == 0xF0) {
        return 4; // 4字节UTF-8字符
    } else {
        return 0; // 无效字节
    }
}

} // namespace Utils
} // namespace CHTL