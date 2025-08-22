#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <memory>
#include <cstdio>

namespace CHTL {
namespace Utils {

/**
 * @brief 字符串操作工具类
 */
class StringUtils {
public:
    /**
     * @brief 去除字符串两端的空白字符
     * @param str 输入字符串
     * @return 去除空白字符后的字符串
     */
    static std::string Trim(const std::string& str);
    
    /**
     * @brief 去除字符串左端的空白字符
     * @param str 输入字符串
     * @return 去除左端空白字符后的字符串
     */
    static std::string TrimLeft(const std::string& str);
    
    /**
     * @brief 去除字符串右端的空白字符
     * @param str 输入字符串
     * @return 去除右端空白字符后的字符串
     */
    static std::string TrimRight(const std::string& str);
    
    /**
     * @brief 分割字符串
     * @param str 输入字符串
     * @param delimiter 分隔符
     * @return 分割后的字符串数组
     */
    static std::vector<std::string> Split(const std::string& str, const std::string& delimiter);
    
    /**
     * @brief 连接字符串数组
     * @param strings 字符串数组
     * @param delimiter 连接符
     * @return 连接后的字符串
     */
    static std::string Join(const std::vector<std::string>& strings, const std::string& delimiter);
    
    /**
     * @brief 替换字符串中的所有匹配项
     * @param str 输入字符串
     * @param from 要替换的字符串
     * @param to 替换成的字符串
     * @return 替换后的字符串
     */
    static std::string ReplaceAll(const std::string& str, const std::string& from, const std::string& to);
    
    /**
     * @brief 转换为小写
     * @param str 输入字符串
     * @return 小写字符串
     */
    static std::string ToLower(const std::string& str);
    
    /**
     * @brief 转换为大写
     * @param str 输入字符串
     * @return 大写字符串
     */
    static std::string ToUpper(const std::string& str);
    
    /**
     * @brief 检查字符串是否以指定前缀开始
     * @param str 输入字符串
     * @param prefix 前缀
     * @return 是否以前缀开始
     */
    static bool StartsWith(const std::string& str, const std::string& prefix);
    
    /**
     * @brief 检查字符串是否以指定后缀结束
     * @param str 输入字符串
     * @param suffix 后缀
     * @return 是否以后缀结束
     */
    static bool EndsWith(const std::string& str, const std::string& suffix);
    
    /**
     * @brief 检查字符串是否包含指定子串
     * @param str 输入字符串
     * @param substr 子串
     * @return 是否包含子串
     */
    static bool Contains(const std::string& str, const std::string& substr);
    
    /**
     * @brief 检查字符是否为空白字符
     * @param ch 字符
     * @return 是否为空白字符
     */
    static bool IsWhitespace(char ch);
    
    /**
     * @brief 检查字符是否为字母
     * @param ch 字符
     * @return 是否为字母
     */
    static bool IsAlpha(char ch);
    
    /**
     * @brief 检查字符是否为数字
     * @param ch 字符
     * @return 是否为数字
     */
    static bool IsDigit(char ch);
    
    /**
     * @brief 检查字符是否为字母或数字
     * @param ch 字符
     * @return 是否为字母或数字
     */
    static bool IsAlphaNumeric(char ch);
    
    /**
     * @brief 转义HTML特殊字符
     * @param str 输入字符串
     * @return 转义后的字符串
     */
    static std::string EscapeHtml(const std::string& str);
    
    /**
     * @brief 转义CSS特殊字符
     * @param str 输入字符串
     * @return 转义后的字符串
     */
    static std::string EscapeCss(const std::string& str);
    
    /**
     * @brief 转义JavaScript特殊字符
     * @param str 输入字符串
     * @return 转义后的字符串
     */
    static std::string EscapeJavaScript(const std::string& str);
    
    /**
     * @brief 格式化字符串
     * @param format 格式字符串
     * @param args 参数
     * @return 格式化后的字符串
     */
    template<typename... Args>
    static std::string Format(const std::string& format, Args... args) {
        size_t size = snprintf(nullptr, 0, format.c_str(), args...) + 1;
        std::unique_ptr<char[]> buf(new char[size]);
        snprintf(buf.get(), size, format.c_str(), args...);
        return std::string(buf.get(), buf.get() + size - 1);
    }
};

} // namespace Utils
} // namespace CHTL