#pragma once

#include <string>
#include <vector>
#include <locale>
#include <codecvt>

namespace CHTL {
namespace Utils {

/**
 * @brief UTF-8编码处理工具类
 */
class UTF8Utils {
public:
    /**
     * @brief 检查字符串是否为有效的UTF-8编码
     * @param str 要检查的字符串
     * @return 是否为有效UTF-8
     */
    static bool IsValidUTF8(const std::string& str);
    
    /**
     * @brief 转换UTF-8字符串为宽字符
     * @param utf8str UTF-8字符串
     * @return 宽字符串
     */
    static std::wstring UTF8ToWString(const std::string& utf8str);
    
    /**
     * @brief 转换宽字符串为UTF-8
     * @param wstr 宽字符串
     * @return UTF-8字符串
     */
    static std::string WStringToUTF8(const std::wstring& wstr);
    
    /**
     * @brief 获取UTF-8字符串的字符数（不是字节数）
     * @param utf8str UTF-8字符串
     * @return 字符数
     */
    static size_t UTF8Length(const std::string& utf8str);
    
    /**
     * @brief 安全地截取UTF-8字符串
     * @param utf8str 源字符串
     * @param start 起始字符位置
     * @param length 截取字符数
     * @return 截取的字符串
     */
    static std::string UTF8Substr(const std::string& utf8str, size_t start, size_t length = std::string::npos);
    
    /**
     * @brief 修复可能损坏的UTF-8字符串
     * @param str 原字符串
     * @return 修复后的字符串
     */
    static std::string FixUTF8String(const std::string& str);
    
    /**
     * @brief 检查字符是否为UTF-8中文字符
     * @param utf8char UTF-8字符的开始位置
     * @param size 字符大小
     * @return 是否为中文字符
     */
    static bool IsChineseCharacter(const char* utf8char, size_t size);
    
private:
    /**
     * @brief 获取UTF-8字符的字节数
     * @param byte 首字节
     * @return 字符的字节数，0表示无效
     */
    static size_t GetUTF8CharSize(unsigned char byte);
};

} // namespace Utils
} // namespace CHTL