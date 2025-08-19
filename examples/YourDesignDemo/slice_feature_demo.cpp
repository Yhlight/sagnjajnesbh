// ========================================
// slice功能专门演示
// 展示如何使用slice处理复杂参数
// ========================================

#include "../src/CHTLJSCompiler/Module/YourCJMODDesign.h"
#include <iostream>

using namespace chtl::your_cjmod_design;

int main() {
    std::cout << "✂️ slice功能专门演示" << std::endl;
    std::cout << "===================" << std::endl;
    std::cout << "展示各种slice使用场景" << std::endl;
    std::cout << std::endl;
    
    // 🎯 场景1：函数体切片处理
    std::cout << "📝 场景1：函数体切片处理" << std::endl;
    std::cout << "======================" << std::endl;
    
    const auto funcSliceKeyword = YourSyntaxAnalyzer::syntaxAnalys(R"(
        processFunction({
            validator: $,
            transformer: $,
            handler: $,
        });
    )", ",:{};()");
    
    // ✂️ 对validator函数的前50个字符进行处理
    funcSliceKeyword.args.slice("validator", 0, 50, [](const std::string& slice) -> std::string {
        std::cout << "✂️ 验证器函数前50字符: " << slice << std::endl;
        
        // 可以对切片进行特殊处理
        if (slice.find("return") != std::string::npos) {
            return "/* 包含返回语句 */ function() { " + slice + " }";
        } else {
            return "/* 无返回语句 */ function() { " + slice + "; return true; }";
        }
    });
    
    // ✂️ 对transformer函数的中间部分进行处理
    funcSliceKeyword.args.slice("transformer", 10, -10, [](const std::string& slice) -> std::string {
        std::cout << "✂️ 转换器函数中间部分: " << slice << std::endl;
        
        // 提取核心逻辑
        return "function(data) { const core = (() => { " + slice + " })(); return core; }";
    });
    
    // ✂️ 对handler函数的最后30个字符进行处理
    funcSliceKeyword.args.slice("handler", -30, -1, [](const std::string& slice) -> std::string {
        std::cout << "✂️ 处理器函数最后30字符: " << slice << std::endl;
        
        // 添加错误处理
        return "function() { try { " + slice + " } catch(e) { console.error('处理器错误:', e); } }";
    });
    
    std::cout << "✅ 函数切片处理设置完成" << std::endl;
    std::cout << std::endl;
    
    // 🎯 场景2：字符串切片处理
    std::cout << "📝 场景2：字符串切片处理" << std::endl;
    std::cout << "======================" << std::endl;
    
    const auto stringSliceKeyword = YourSyntaxAnalyzer::syntaxAnalys(R"(
        parseString({
            content: $,
            prefix: $,
            suffix: $,
        });
    )", ",:{};()");
    
    // ✂️ 提取内容的前10个字符作为摘要
    stringSliceKeyword.args.slice("content", 0, 10, [](const std::string& slice) -> std::string {
        std::cout << "✂️ 内容摘要(前10字符): " << slice << std::endl;
        return "'" + slice + "...'";
    });
    
    // ✂️ 提取前缀的前5个字符
    stringSliceKeyword.args.slice("prefix", 0, 5, [](const std::string& slice) -> std::string {
        std::cout << "✂️ 前缀切片(前5字符): " << slice << std::endl;
        return "'" + slice + "'";
    });
    
    // ✂️ 提取后缀的最后3个字符
    stringSliceKeyword.args.slice("suffix", -3, -1, [](const std::string& slice) -> std::string {
        std::cout << "✂️ 后缀切片(最后3字符): " << slice << std::endl;
        return "'" + slice + "'";
    });
    
    std::cout << "✅ 字符串切片处理设置完成" << std::endl;
    std::cout << std::endl;
    
    // 🎯 场景3：数组切片处理
    std::cout << "📝 场景3：数组切片处理" << std::endl;
    std::cout << "====================" << std::endl;
    
    const auto arraySliceKeyword = YourSyntaxAnalyzer::syntaxAnalys(R"(
        processArray({
            items: $,
            firstFew: $,
            lastFew: $,
        });
    )", ",:{};()");
    
    // ✂️ 获取数组的前3个元素
    arraySliceKeyword.args.slice("firstFew", 0, 3, [](const std::string& slice) -> std::string {
        std::cout << "✂️ 数组前3个元素切片处理" << std::endl;
        return "Array.from(" + slice + ").slice(0, 3)";
    });
    
    // ✂️ 获取数组的最后5个元素
    arraySliceKeyword.args.slice("lastFew", -5, -1, [](const std::string& slice) -> std::string {
        std::cout << "✂️ 数组最后5个元素切片处理" << std::endl;
        return "Array.from(" + slice + ").slice(-5)";
    });
    
    std::cout << "✅ 数组切片处理设置完成" << std::endl;
    std::cout << std::endl;
    
    // 🎯 场景4：复杂对象属性切片
    std::cout << "📝 场景4：复杂对象属性切片" << std::endl;
    std::cout << "========================" << std::endl;
    
    const auto objSliceKeyword = YourSyntaxAnalyzer::syntaxAnalys(R"(
        processObject({
            config: $,
            metadata: $,
            data: $,
        });
    )", ",:{};()");
    
    // 🚀 对配置对象进行高级处理，包含切片功能
    objSliceKeyword.args.bindAdvanced("config", [](const AdvancedParamValue& param) -> std::string {
        if (param.type == AdvancedParamType::OBJECT) {
            std::cout << "📦 处理配置对象" << std::endl;
            
            std::stringstream configJS;
            configJS << "{\n";
            
            for (const auto& prop : param.objectProps) {
                std::string key = prop.first;
                std::string value = prop.second;
                
                // 对长字符串值进行切片
                if (value.length() > 20) {
                    std::string slice = value.substr(0, 15) + "...";
                    std::cout << "✂️ 配置项 " << key << " 值过长，切片为: " << slice << std::endl;
                    configJS << "    " << key << ": '" << slice << "',\n";
                } else {
                    configJS << "    " << key << ": " << value << ",\n";
                }
            }
            
            configJS << "}";
            return configJS.str();
        }
        
        return param.processedValue;
    });
    
    std::cout << "✅ 复杂对象切片处理设置完成" << std::endl;
    std::cout << std::endl;
    
    // 🎯 测试所有切片功能
    std::cout << "📝 测试所有切片功能" << std::endl;
    std::cout << "==================" << std::endl;
    
    CJMODScanner scanner;
    
    // 测试函数切片
    scanner.scanKeyword(funcSliceKeyword.args[0], [&]() {
        std::cout << "🔍 测试函数切片处理" << std::endl;
        
        Keyword validatorKw("validator", "parameter");
        validatorKw.value = "function(data) { return data && data.length > 0 && typeof data === 'string'; }";
        
        Keyword transformerKw("transformer", "parameter");
        transformerKw.value = "function(input) { const processed = input.trim().toLowerCase(); const result = processed.replace(/\\s+/g, '-'); return result; }";
        
        Keyword handlerKw("handler", "parameter");
        handlerKw.value = "function(result) { console.log('处理结果:', result); return result; }";
        
        funcSliceKeyword.args.match("validator", validatorKw);
        funcSliceKeyword.args.match("transformer", transformerKw);
        funcSliceKeyword.args.match("handler", handlerKw);
    });
    
    // 测试字符串切片
    scanner.scanKeyword(stringSliceKeyword.args[0], [&]() {
        std::cout << "🔍 测试字符串切片处理" << std::endl;
        
        Keyword contentKw("content", "parameter");
        contentKw.value = "这是一段很长的文本内容，用于测试字符串切片功能的效果";
        
        Keyword prefixKw("prefix", "parameter");
        prefixKw.value = "PREFIX_VERY_LONG";
        
        Keyword suffixKw("suffix", "parameter");
        suffixKw.value = "VERY_LONG_SUFFIX";
        
        stringSliceKeyword.args.match("content", contentKw);
        stringSliceKeyword.args.match("prefix", prefixKw);
        stringSliceKeyword.args.match("suffix", suffixKw);
    });
    
    // 执行扫描
    std::string testCode1 = R"(
        processFunction({
            validator: function(data) { return data && data.length > 0; },
            transformer: function(input) { return input.trim(); },
            handler: function(result) { console.log(result); },
        });
    )";
    
    std::string testCode2 = R"(
        parseString({
            content: '这是测试内容',
            prefix: 'TEST_PREFIX',
            suffix: 'TEST_SUFFIX',
        });
    )";
    
    scanner.executeScan(testCode1, funcSliceKeyword);
    scanner.executeScan(testCode2, stringSliceKeyword);
    
    // 生成代码
    std::string funcCode = YourCodeGenerator::generateCode(funcSliceKeyword);
    std::string stringCode = YourCodeGenerator::generateCode(stringSliceKeyword);
    
    std::cout << "\n🎉 函数切片处理结果：" << std::endl;
    std::cout << "=====================" << std::endl;
    std::cout << funcCode << std::endl;
    
    std::cout << "\n🎉 字符串切片处理结果：" << std::endl;
    std::cout << "======================" << std::endl;
    std::cout << stringCode << std::endl;
    
    // 🎊 总结
    std::cout << "\n🎊 slice功能总结：" << std::endl;
    std::cout << "=================" << std::endl;
    std::cout << "✂️ 函数体切片：处理函数的特定部分" << std::endl;
    std::cout << "✂️ 字符串切片：提取字符串的指定范围" << std::endl;
    std::cout << "✂️ 数组切片：获取数组的特定元素" << std::endl;
    std::cout << "✂️ 对象属性切片：处理复杂对象的属性值" << std::endl;
    std::cout << "✂️ 支持负数索引：从末尾开始计算" << std::endl;
    std::cout << "✂️ 智能处理：根据参数类型自动选择切片策略" << std::endl;
    std::cout << "🌟 slice功能完美解决了复杂参数处理问题！" << std::endl;
    
    return 0;
}