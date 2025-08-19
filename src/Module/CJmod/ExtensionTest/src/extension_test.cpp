// CJmod格式测试扩展
#include "../../../../../../src/CHTLJSCompiler/Module/YourCJMODDesign.h"

using namespace your_cjmod_design;

// 测试扩展函数
void registerTestExtension() {
    // 创建测试语法
    auto keyword = syntaxAnalys("testFunction( arg )");
    
    // 绑定参数处理器
    keyword.args.bind("arg", [](const std::string& value) {
        return "console.log('CJmod测试: " + value + "');";
    });
    
    // 注册扫描器
    // scanner.scanKeyword(keyword.args["testFunction"], []() {
    //     // 处理testFunction调用
    // });
}