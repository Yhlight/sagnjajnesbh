// 示例：字符串工具模块
// 展示更复杂的CJMOD功能

#include "../CJMODFramework.h"
#include <algorithm>
#include <sstream>
#include <regex>

using namespace chtl::cjmod;

// 字符串工具模块
CJMOD_MODULE(StringUtils)
    
    module->setDescription("强大的字符串处理工具集")
          .setAuthor("CHTL Team")
          .setVersion("1.2.0");
    
    // 字符串反转
    module->addFunction("reverse", [](const JSContext& ctx) -> JSValue {
        if (!helpers::validateArgCount(ctx, 1)) {
            return helpers::makeError("需要1个字符串参数");
        }
        
        std::string str = ctx.getArg(0).toString();
        std::reverse(str.begin(), str.end());
        
        return JSValue(str);
    }, "反转字符串");
    
    // 字符串分割
    module->addFunction("split", [](const JSContext& ctx) -> JSValue {
        if (!helpers::validateArgCount(ctx, 2)) {
            return helpers::makeError("需要字符串和分隔符两个参数");
        }
        
        std::string str = ctx.getArg(0).toString();
        std::string delimiter = ctx.getArg(1).toString();
        
        std::vector<JSValue> parts;
        size_t start = 0;
        size_t end = str.find(delimiter);
        
        while (end != std::string::npos) {
            parts.emplace_back(str.substr(start, end - start));
            start = end + delimiter.length();
            end = str.find(delimiter, start);
        }
        parts.emplace_back(str.substr(start));
        
        return JSValue(parts);
    }, "分割字符串为数组");
    
    // 字符串连接
    module->addFunction("join", [](const JSContext& ctx) -> JSValue {
        if (!helpers::validateArgCount(ctx, 2)) {
            return helpers::makeError("需要数组和连接符两个参数");
        }
        
        auto array = ctx.getArg(0).toArray();
        std::string separator = ctx.getArg(1).toString();
        
        std::ostringstream result;
        for (size_t i = 0; i < array.size(); ++i) {
            if (i > 0) result << separator;
            result << array[i].toString();
        }
        
        return JSValue(result.str());
    }, "用指定分隔符连接数组元素");
    
    // 正则表达式匹配
    module->addFunction("match", [](const JSContext& ctx) -> JSValue {
        if (!helpers::validateArgCount(ctx, 2)) {
            return helpers::makeError("需要字符串和正则表达式两个参数");
        }
        
        std::string text = ctx.getArg(0).toString();
        std::string pattern = ctx.getArg(1).toString();
        
        try {
            std::regex regex(pattern);
            std::smatch matches;
            
            JSValue result;
            if (std::regex_search(text, matches, regex)) {
                result["found"] = JSValue(true);
                result["match"] = JSValue(matches[0].str());
                result["position"] = JSValue(static_cast<int>(matches.position()));
                
                // 捕获组
                std::vector<JSValue> groups;
                for (size_t i = 1; i < matches.size(); ++i) {
                    groups.emplace_back(matches[i].str());
                }
                result["groups"] = JSValue(groups);
            } else {
                result["found"] = JSValue(false);
            }
            
            return result;
        } catch (const std::exception& e) {
            return helpers::makeError("正则表达式错误: " + std::string(e.what()));
        }
    }, "正则表达式匹配");
    
    // 字符串替换
    module->addFunction("replace", [](const JSContext& ctx) -> JSValue {
        if (!helpers::validateArgCount(ctx, 3)) {
            return helpers::makeError("需要原字符串、查找字符串、替换字符串三个参数");
        }
        
        std::string text = ctx.getArg(0).toString();
        std::string search = ctx.getArg(1).toString();
        std::string replacement = ctx.getArg(2).toString();
        
        size_t pos = text.find(search);
        if (pos != std::string::npos) {
            text.replace(pos, search.length(), replacement);
        }
        
        return JSValue(text);
    }, "替换字符串中的第一个匹配项");
    
    // 全部替换
    module->addFunction("replaceAll", [](const JSContext& ctx) -> JSValue {
        if (!helpers::validateArgCount(ctx, 3)) {
            return helpers::makeError("需要原字符串、查找字符串、替换字符串三个参数");
        }
        
        std::string text = ctx.getArg(0).toString();
        std::string search = ctx.getArg(1).toString();
        std::string replacement = ctx.getArg(2).toString();
        
        size_t pos = 0;
        while ((pos = text.find(search, pos)) != std::string::npos) {
            text.replace(pos, search.length(), replacement);
            pos += replacement.length();
        }
        
        return JSValue(text);
    }, "替换字符串中的所有匹配项");
    
    // 字符串格式化
    module->addFunction("format", [](const JSContext& ctx) -> JSValue {
        if (ctx.getArgCount() < 1) {
            return helpers::makeError("至少需要1个参数（模板字符串）");
        }
        
        std::string template_str = ctx.getArg(0).toString();
        
        // 简单的 {} 占位符替换
        std::string result = template_str;
        for (size_t i = 1; i < ctx.getArgCount(); ++i) {
            size_t pos = result.find("{}");
            if (pos != std::string::npos) {
                result.replace(pos, 2, ctx.getArg(i).toString());
            }
        }
        
        return JSValue(result);
    }, "格式化字符串（使用{}占位符）");
    
    // 字符串统计
    module->addFunction("stats", [](const JSContext& ctx) -> JSValue {
        if (!helpers::validateArgCount(ctx, 1)) {
            return helpers::makeError("需要1个字符串参数");
        }
        
        std::string text = ctx.getArg(0).toString();
        
        JSValue stats;
        stats["length"] = JSValue(static_cast<int>(text.length()));
        
        // 字符计数
        std::unordered_map<char, int> char_count;
        int letters = 0, digits = 0, spaces = 0, others = 0;
        
        for (char c : text) {
            char_count[c]++;
            
            if (std::isalpha(c)) letters++;
            else if (std::isdigit(c)) digits++;
            else if (std::isspace(c)) spaces++;
            else others++;
        }
        
        stats["letters"] = JSValue(letters);
        stats["digits"] = JSValue(digits);
        stats["spaces"] = JSValue(spaces);
        stats["others"] = JSValue(others);
        
        // 单词计数（简单版）
        std::istringstream iss(text);
        std::string word;
        int word_count = 0;
        while (iss >> word) {
            word_count++;
        }
        stats["words"] = JSValue(word_count);
        
        return stats;
    }, "获取字符串的详细统计信息");

CJMOD_MODULE_END(StringUtils)