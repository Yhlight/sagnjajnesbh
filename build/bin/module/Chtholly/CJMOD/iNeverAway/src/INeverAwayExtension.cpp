#include "INeverAwayExtension.h"
#include <iostream>
#include <sstream>

namespace Chtholly {
namespace CJMOD {

// iNeverAway扩展实现 - 使用您设计的CJMOD API
// 语法文档第1485-1520行：创建一组标记函数，允许开发者定义键，使用状态区分同名键

// 全局状态管理 - 语法文档第1425行：可以编写全局变量管理状态
std::unordered_map<std::string, std::string> INeverAwayExtension::globalVirtualObjects_;
int INeverAwayExtension::globalFunctionCounter_ = 0;

bool INeverAwayExtension::Initialize(CHTL::CJMOD::CJMODScanner& scanner) {
    // 使用您的CJMOD API注册iNeverAway语法扫描
    
    // 注册vir关键字扫描
    scanner.ScanKeyword("vir", [this, &scanner]() {
        // 当扫描到vir关键字时的处理
        
        // 查看后续关键字
        std::string nextKeyword = scanner.PeekKeyword(1);  // 获取虚对象名称
        std::string equalSign = scanner.PeekKeyword(2);    // 应该是 =
        std::string iNeverAwayKeyword = scanner.PeekKeyword(3);  // 应该是 iNeverAway
        
        if (equalSign == "=" && iNeverAwayKeyword == "iNeverAway") {
            // 设置收集策略来收集方法定义块
            scanner.PolicyChangeBegin("{", CHTL::CJMOD::ScanPolicy::COLLECT);
            std::string methodBlock = scanner.PolicyChangeEnd("}", CHTL::CJMOD::ScanPolicy::NORMAL);
            
            // 处理iNeverAway定义
            ProcessINeverAwayDefinition(nextKeyword, methodBlock);
        }
    });
    
    // 注册虚对象调用扫描（通过->操作符）
    scanner.ScanKeyword("->", [this, &scanner]() {
        // 处理虚对象方法调用
        std::string objectName = scanner.PeekKeyword(-1);  // 获取对象名称
        std::string methodName = scanner.PeekKeyword(1);   // 获取方法名称
        
        ProcessVirtualObjectCall(objectName, methodName);
    });
    
    std::cout << "iNeverAway扩展初始化完成（使用CJMOD API）" << std::endl;
    return true;
}

std::string INeverAwayExtension::GetName() const {
    return "iNeverAway";
}

std::string INeverAwayExtension::GetVersion() const {
    return "1.0.0";
}

std::vector<std::string> INeverAwayExtension::GetSupportedSyntax() const {
    return {
        "vir $ = iNeverAway({...})",    // 虚对象定义
        "$->$(...)"                     // 虚对象调用
    };
}

void INeverAwayExtension::ProcessINeverAwayDefinition(const std::string& objectName, const std::string& methodBlock) {
    // 使用您的CJMOD API处理iNeverAway定义
    
    // 创建语法分析 - 语法文档第1491-1507行的格式
    auto syntax = CHTL::CJMOD::SyntaxAnalys("Void<$>: function($, $) {}, Void: {}, $: {}", ",:{};()");
    
    if (!syntax) {
        std::cerr << "iNeverAway语法分析失败" << std::endl;
        return;
    }
    
    // 绑定状态方法处理 - 语法文档第1492-1506行
    syntax->GetArgs().Bind("Void", [this, objectName](const CHTL::CJMOD::ArgValue& value) -> std::string {
        // 处理Void状态方法
        std::string methodName = std::get<std::string>(value);
        std::string globalFuncName = GenerateGlobalFunctionName(objectName, methodName);
        
        return "function " + globalFuncName + "() { console.log('调用了 " + methodName + " 方法'); }";
    });
    
    // 处理其他方法
    for (size_t i = 0; i < syntax->GetArgs().Size(); ++i) {
        if (syntax->GetArgs()[i].IsPlaceholder()) {
            syntax->GetArgs().Bind(i, [this, objectName](const CHTL::CJMOD::ArgValue& value) -> std::string {
                std::string methodName = std::get<std::string>(value);
                std::string globalFuncName = GenerateGlobalFunctionName(objectName, methodName);
                
                return "function " + globalFuncName + "() { console.log('调用了 " + methodName + " 方法'); }";
            });
        }
    }
    
    // 生成JavaScript代码
    std::string jsCode = GenerateINeverAwayJS(objectName, *syntax);
    
    // 注册虚对象
    globalVirtualObjects_[objectName] = jsCode;
    
    std::cout << "iNeverAway虚对象定义处理完成: " << objectName << std::endl;
}

void INeverAwayExtension::ProcessVirtualObjectCall(const std::string& objectName, const std::string& methodName) {
    // 处理虚对象调用 - 语法文档第1509行：Test->Void<A>();
    // 语法文档第1515行：vir对象本身不存在，最终转变成相对应的函数的引用
    
    auto it = globalVirtualObjects_.find(objectName);
    if (it != globalVirtualObjects_.end()) {
        std::string globalFuncName = GenerateGlobalFunctionName(objectName, methodName);
        std::string callCode = globalFuncName + "();";
        
        std::cout << "虚对象调用处理: " << objectName << "->" << methodName << " => " << callCode << std::endl;
    } else {
        std::cerr << "未找到虚对象: " << objectName << std::endl;
    }
}

std::string INeverAwayExtension::GenerateGlobalFunctionName(const std::string& objectName, const std::string& methodName) {
    // 生成全局函数名称 - 语法文档第1514行：名称由CHTL编译器统一管理
    return "_CHTL_iNeverAway_" + objectName + "_" + methodName + "_" + std::to_string(++globalFunctionCounter_);
}

std::string INeverAwayExtension::GenerateINeverAwayJS(const std::string& objectName, const CHTL::CJMOD::Syntax& syntax) {
    // 使用您的CJMOD API生成iNeverAway JavaScript代码
    // 语法文档第1514行：创建一组JS全局函数，在调用时才生成对应的JS函数代码
    
    std::ostringstream js;
    
    js << "// iNeverAway扩展 - 虚对象: " << objectName << "\n";
    js << "// 语法文档第1514行：创建一组JS全局函数，名称由CHTL编译器统一管理\n\n";
    
    // 使用您的API生成代码
    js << syntax.GetArgs().Result();
    
    js << "\n// 虚对象 " << objectName << " 的方法映射已生成\n";
    
    return js.str();
}

} // namespace CJMOD
} // namespace Chtholly