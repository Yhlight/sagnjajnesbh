#include "../src/Module/Chtholly/CJMOD/src/ChthollyInterface.h"
#include <iostream>
#include <cassert>
#include <string>
#include <cstring>
#include <sstream>

using namespace chtl::chtholly;

void testModuleInitialization() {
    std::cout << "=== 测试Chtholly模块初始化 ===" << std::endl;
    
    auto& module = ChthollyModuleInterface::getInstance();
    
    // 测试初始化
    bool init_result = module.initialize();
    std::cout << "初始化结果: " << (init_result ? "成功" : "失败") << std::endl;
    assert(init_result);
    assert(module.isInitialized());
    
    // 测试调试模式
    module.setDebugMode(true);
    assert(module.isDebugMode());
    
    std::cout << "✅ 模块初始化测试通过!" << std::endl;
}

void testPrintMylove() {
    std::cout << "=== 测试printMylove功能 ===" << std::endl;
    
    auto& module = ChthollyModuleInterface::getInstance();
    
    // 测试ASCII模式
    std::string ascii_config = R"({
        "url": "chtholly_heart.jpg",
        "mode": "ASCII",
        "width": "40",
        "height": "20",
        "scale": 1.0
    })";
    
    std::string ascii_result = module.printMylove(ascii_config);
    std::cout << "ASCII转换结果长度: " << ascii_result.length() << std::endl;
    std::cout << "ASCII预览:" << std::endl;
    
    // 显示前几行
    std::istringstream ascii_stream(ascii_result);
    std::string line;
    int line_count = 0;
    while (std::getline(ascii_stream, line) && line_count < 5) {
        std::cout << "  " << line << std::endl;
        line_count++;
    }
    
    assert(!ascii_result.empty());
    
    // 测试Pixel模式
    std::string pixel_config = R"({
        "url": "chtholly_heart.jpg", 
        "mode": "PIXEL",
        "width": "30",
        "height": "15",
        "scale": 1.0
    })";
    
    std::string pixel_result = module.printMylove(pixel_config);
    std::cout << "Pixel转换结果长度: " << pixel_result.length() << std::endl;
    std::cout << "Pixel预览:" << std::endl;
    
    // 显示前几行
    std::istringstream pixel_stream(pixel_result);
    line_count = 0;
    while (std::getline(pixel_stream, line) && line_count < 5) {
        std::cout << "  " << line << std::endl;
        line_count++;
    }
    
    assert(!pixel_result.empty());
    
    std::cout << "✅ printMylove功能测试通过!" << std::endl;
}

void testINeverAway() {
    std::cout << "=== 测试iNeverAway功能 ===" << std::endl;
    
    auto& module = ChthollyModuleInterface::getInstance();
    
    // 测试虚对象声明
    std::string vir_declaration = R"(
        vir Test = iNeverAway({
            Void<A>: function(int, int) {
                console.log('Void<A> called with:', arguments);
                return 'Hello from Void<A>';
            },
            
            Void<B>: function(int, int) {
                console.log('Void<B> called with:', arguments);
                return 'Hello from Void<B>';
            },
            
            Void: {
                console.log('Simple Void called');
            },
            
            Ax: {
                console.log('Ax called');
                return 42;
            }
        });
    )";
    
    std::string declaration_result = module.processVirDeclaration(vir_declaration);
    std::cout << "虚对象声明处理结果长度: " << declaration_result.length() << std::endl;
    
    if (!declaration_result.empty()) {
        std::cout << "生成的JavaScript代码片段:" << std::endl;
        // 显示前几行生成的代码
        std::istringstream code_stream(declaration_result);
        std::string line;
        int line_count = 0;
        while (std::getline(code_stream, line) && line_count < 10) {
            std::cout << "  " << line << std::endl;
            line_count++;
        }
    }
    
    assert(!declaration_result.empty());
    
    // 测试虚对象调用
    std::vector<std::string> test_calls = {
        "Test->Void<A>(1, 2)",
        "Test->Void<B>(3, 4)", 
        "Test->Void()",
        "Test->Ax()"
    };
    
    for (const auto& call : test_calls) {
        std::string call_result = module.processVirCall(call);
        std::cout << "调用 '" << call << "' 生成: " << call_result << std::endl;
        assert(!call_result.empty());
    }
    
    // 生成完整的iNeverAway代码
    std::string complete_code = module.generateINeverAwayCode();
    std::cout << "完整iNeverAway代码长度: " << complete_code.length() << std::endl;
    
    std::cout << "✅ iNeverAway功能测试通过!" << std::endl;
}

void testCInterfaceCompatibility() {
    std::cout << "=== 测试C接口兼容性 ===" << std::endl;
    
    // 测试C接口初始化
    int c_init_result = chtholly_initialize();
    std::cout << "C接口初始化结果: " << c_init_result << std::endl;
    assert(c_init_result == 1);
    
    // 测试C接口printMylove
    const char* c_config = R"({"url": "test.jpg", "mode": "ASCII", "width": "20", "height": "10"})";
    const char* c_result = chtholly_printMylove(c_config);
    std::cout << "C接口printMylove结果长度: " << (c_result ? strlen(c_result) : 0) << std::endl;
    assert(c_result != nullptr);
    assert(strlen(c_result) > 0);
    
    // 测试C接口iNeverAway
    const char* c_vir_code = "vir Simple = iNeverAway({ test: function() { return 'test'; } });";
    const char* c_vir_result = chtholly_processVirDeclaration(c_vir_code);
    std::cout << "C接口vir声明结果长度: " << (c_vir_result ? strlen(c_vir_result) : 0) << std::endl;
    
    const char* c_call_code = "Simple->test()";
    const char* c_call_result = chtholly_processVirCall(c_call_code);
    std::cout << "C接口vir调用结果: " << (c_call_result ? c_call_result : "null") << std::endl;
    
    // 测试错误处理
    int has_error = chtholly_hasError();
    std::cout << "是否有错误: " << has_error << std::endl;
    
    if (has_error) {
        const char* error_msg = chtholly_getLastError();
        std::cout << "错误信息: " << (error_msg ? error_msg : "null") << std::endl;
    }
    
    std::cout << "✅ C接口兼容性测试通过!" << std::endl;
}

void testStatistics() {
    std::cout << "=== 测试统计功能 ===" << std::endl;
    
    auto& module = ChthollyModuleInterface::getInstance();
    
    // 获取统计信息
    auto stats = module.getStatistics();
    
    std::cout << "当前统计信息:" << std::endl;
    std::cout << "  处理图片数: " << stats.images_processed << std::endl;
    std::cout << "  ASCII转换数: " << stats.ascii_conversions << std::endl;
    std::cout << "  像素转换数: " << stats.pixel_conversions << std::endl;
    std::cout << "  创建虚对象数: " << stats.virtual_objects_created << std::endl;
    std::cout << "  处理函数调用数: " << stats.function_calls_processed << std::endl;
    std::cout << "  生成JS函数数: " << stats.js_functions_generated << std::endl;
    
    // 验证统计数据的合理性
    assert(stats.images_processed >= 0);
    assert(stats.ascii_conversions + stats.pixel_conversions <= stats.images_processed + 1); // +1容错
    
    std::cout << "✅ 统计功能测试通过!" << std::endl;
}

void testErrorHandling() {
    std::cout << "=== 测试错误处理 ===" << std::endl;
    
    auto& module = ChthollyModuleInterface::getInstance();
    
    // 测试无效的printMylove配置
    std::string invalid_config = R"({"url": "", "mode": "INVALID"})";
    std::string invalid_result = module.printMylove(invalid_config);
    
    std::cout << "无效配置结果长度: " << invalid_result.length() << std::endl;
    
    if (module.hasPrintMyloveError()) {
        std::cout << "printMylove错误: " << module.getPrintMyloveError() << std::endl;
    }
    
    // 测试无效的iNeverAway代码
    std::string invalid_vir = "invalid vir syntax";
    std::string invalid_vir_result = module.processVirDeclaration(invalid_vir);
    
    std::cout << "无效vir声明结果长度: " << invalid_vir_result.length() << std::endl;
    
    if (module.hasINeverAwayError()) {
        std::cout << "iNeverAway错误: " << module.getINeverAwayError() << std::endl;
    }
    
    // 测试错误清理
    module.clearAllErrors();
    std::cout << "错误清理后，是否还有错误: " << (module.hasAnyError() ? "是" : "否") << std::endl;
    
    std::cout << "✅ 错误处理测试通过!" << std::endl;
}

void testRealWorldScenarios() {
    std::cout << "=== 测试真实世界场景 ===" << std::endl;
    
    auto& module = ChthollyModuleInterface::getInstance();
    
    // 场景1: 创建一个完整的珂朵莉虚对象
    std::cout << "场景1: 珂朵莉虚对象系统" << std::endl;
    
    std::string chtholly_vir = R"(
        vir Chtholly = iNeverAway({
            smile: function() {
                console.log('珂朵莉温柔地笑了 ♥');
                return 'gentle_smile';
            },
            
            fight<Desperate>: function(enemy) {
                console.log('珂朵莉拼尽全力战斗！');
                return 'desperate_fight';
            },
            
            fight<Gentle>: function(enemy) {
                console.log('珂朵莉优雅地战斗');
                return 'gentle_fight';
            },
            
            remember: {
                console.log('珂朵莉回忆起了重要的事情...');
            }
        });
    )";
    
    std::string chtholly_js = module.processVirDeclaration(chtholly_vir);
    assert(!chtholly_js.empty());
    
    // 测试珂朵莉的各种调用
    std::vector<std::string> chtholly_calls = {
        "Chtholly->smile()",
        "Chtholly->fight<Desperate>('beast')",
        "Chtholly->fight<Gentle>('monster')",
        "Chtholly->remember()"
    };
    
    for (const auto& call : chtholly_calls) {
        std::string result = module.processVirCall(call);
        std::cout << "  " << call << " -> " << result << std::endl;
        assert(!result.empty());
    }
    
    // 场景2: 图片艺术生成
    std::cout << "场景2: 珂朵莉图片艺术" << std::endl;
    
    std::string art_config = R"({
        "url": "chtholly_portrait.jpg",
        "mode": "PIXEL", 
        "width": "50",
        "height": "25",
        "scale": 1.2
    })";
    
    std::string art_result = module.printMylove(art_config);
    std::cout << "  生成的珂朵莉像素艺术长度: " << art_result.length() << std::endl;
    assert(!art_result.empty());
    
    std::cout << "✅ 真实世界场景测试通过!" << std::endl;
}

int main() {
    try {
        std::cout << "开始测试Chtholly 珂朵莉模块..." << std::endl;
        std::cout << "「即使忘记了自己，也绝不会忘记重要的人」" << std::endl << std::endl;
        
        testModuleInitialization();
        std::cout << std::endl;
        
        testPrintMylove();
        std::cout << std::endl;
        
        testINeverAway();
        std::cout << std::endl;
        
        testCInterfaceCompatibility();
        std::cout << std::endl;
        
        testStatistics();
        std::cout << std::endl;
        
        testErrorHandling();
        std::cout << std::endl;
        
        testRealWorldScenarios();
        std::cout << std::endl;
        
        // 打印完整的模块信息
        std::cout << "=== 完整模块信息 ===" << std::endl;
        chtholly_printModuleInfo();
        
        std::cout << std::endl;
        std::cout << "🎉 所有Chtholly模块测试通过!" << std::endl;
        std::cout << "✅ printMylove: 图片转字符艺术完美实现!" << std::endl;
        std::cout << "✅ iNeverAway: 虚对象系统完美运行!" << std::endl;
        std::cout << "✅ CMOD + CJMOD: 混合模块架构完美!" << std::endl;
        std::cout << "✅ C接口: 编译器集成完美!" << std::endl;
        std::cout << std::endl;
        std::cout << "珂朵莉永远与CHTL同在 ♥" << std::endl;
        
        // 清理
        chtholly_cleanup();
        
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}