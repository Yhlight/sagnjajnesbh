#include "CJMOD/Extensions/INeverAwayExtension.h"
#include "Utils/ErrorHandler.h"
#include "Utils/StringUtils.h"
#include <sstream>
#include <regex>

namespace CHTL {
namespace CJMOD {
namespace Extensions {

INeverAwayExtension::INeverAwayExtension() 
    : scanner_(nullptr), globalCounter_(0) {}

bool INeverAwayExtension::Initialize(CJMODScanner& scanner) {
    scanner_ = &scanner;
    
    // 定义iNeverAway语法模式（语法文档第1491-1507行示例）
    std::string ignoreChars = ",:{};()";
    iNeverAwaySyntax_ = SyntaxAnalys(R"(
        vir $ = iNeverAway({
            $: function($, $) {
                $
            },
            $: function($, $) {
                $
            },
            $: {
                $
            },
            $: {
                $
            }
        });
    )", ignoreChars);
    
    // 定义虚对象调用语法（语法文档第1509行示例）
    virtualCallSyntax_ = SyntaxAnalys(R"(
        $->$<$>();
    )", ignoreChars);
    
    // 注册iNeverAway关键字扫描
    scanner.ScanKeyword("iNeverAway", [this]() {
        HandleINeverAway();
    });
    
    // 注册vir关键字扫描
    scanner.ScanKeyword("vir", [this]() {
        HandleINeverAway();
    });
    
    // 注册箭头操作符扫描（用于虚对象调用）
    scanner.ScanKeyword("->", [this]() {
        HandleVirtualObjectCall();
    });
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "INeverAway扩展初始化成功 - 函数重载和虚对象系统已就绪"
    );
    
    return true;
}

std::vector<std::string> INeverAwayExtension::GetSupportedSyntax() const {
    return {
        "vir $ = iNeverAway({...});",
        "$->$<$>();",
        "Void<A>: function(int, int) {...}",
        "Void<B>: function(int, int) {...}",
        "Void: {...}",
        "普通键: {...}"
    };
}

void INeverAwayExtension::HandleINeverAway() {
    if (!scanner_) {
        return;
    }
    
    try {
        // 解析虚对象名称
        std::string objectName;
        
        // 收集虚对象名称（vir后面的标识符）
        scanner_->PolicyChangeBegin("vir", ScanPolicy::COLLECT);
        std::string virDeclaration = scanner_->PolicyChangeEnd("=", ScanPolicy::NORMAL);
        
        // 提取对象名称
        std::regex virRegex(R"(vir\s+(\w+)\s*=)");
        std::smatch match;
        if (std::regex_search(virDeclaration, match, virRegex)) {
            objectName = match[1].str();
        }
        
        if (objectName.empty()) {
            Utils::ErrorHandler::GetInstance().LogError(
                "iNeverAway: 无法解析虚对象名称"
            );
            return;
        }
        
        // 创建虚对象定义
        VirtualObjectDef& virtualObj = virtualObjects_[objectName];
        virtualObj.name = objectName;
        virtualObj.globalFunctionCounter = 0;
        
        // 解析iNeverAway函数定义块
        scanner_->PolicyChangeBegin("{", ScanPolicy::COLLECT);
        std::string functionsBlock = scanner_->PolicyChangeEnd("}", ScanPolicy::NORMAL);
        
        // 解析函数定义
        ParseFunctionDefinitions(functionsBlock, virtualObj);
        
        // 生成JavaScript代码
        std::vector<FunctionDef> functions;
        for (const auto& pair : virtualObj.functions) {
            functions.push_back(pair.second);
        }
        
        std::string jsCode = GenerateINeverAwayCode(objectName, functions);
        
        Utils::ErrorHandler::GetInstance().LogInfo(
            "成功处理iNeverAway虚对象: " + objectName + 
            "，包含 " + std::to_string(functions.size()) + " 个函数"
        );
        
    } catch (const std::exception& e) {
        Utils::ErrorHandler::GetInstance().LogError(
            "iNeverAway处理异常: " + std::string(e.what())
        );
    }
}

void INeverAwayExtension::HandleVirtualObjectCall() {
    if (!scanner_) {
        return;
    }
    
    try {
        // 解析虚对象调用：Test->Void<A>();
        std::string objectName = scanner_->PeekKeyword(-1); // 获取对象名
        std::string functionCall = scanner_->PeekKeyword(1); // 获取函数调用
        
        // 解析函数名和状态
        std::regex callRegex(R"((\w+)<(\w+)>\(\))");
        std::smatch match;
        
        std::string functionName;
        FunctionState state = FunctionState::NORMAL;
        
        if (std::regex_search(functionCall, match, callRegex)) {
            functionName = match[1].str();
            std::string stateStr = "Void<" + match[2].str() + ">";
            state = ParseFunctionState(stateStr);
        } else {
            // 简单函数调用
            std::regex simpleCallRegex(R"((\w+)\(\))");
            if (std::regex_search(functionCall, match, simpleCallRegex)) {
                functionName = match[1].str();
                state = FunctionState::NORMAL;
            }
        }
        
        if (!functionName.empty()) {
            // 生成虚对象调用代码
            std::vector<std::string> args; // 暂时不支持参数
            std::string jsCode = GenerateVirtualCallCode(objectName, functionName, state, args);
            
            Utils::ErrorHandler::GetInstance().LogInfo(
                "成功处理虚对象调用: " + objectName + "->" + functionName
            );
        }
        
    } catch (const std::exception& e) {
        Utils::ErrorHandler::GetInstance().LogError(
            "虚对象调用处理异常: " + std::string(e.what())
        );
    }
}

INeverAwayExtension::FunctionState INeverAwayExtension::ParseFunctionState(const std::string& stateStr) {
    if (stateStr == "Void<A>") {
        return FunctionState::VOID_A;
    } else if (stateStr == "Void<B>") {
        return FunctionState::VOID_B;
    } else if (stateStr == "Void") {
        return FunctionState::VOID;
    } else {
        return FunctionState::NORMAL;
    }
}

std::string INeverAwayExtension::GenerateGlobalFunctionName(const std::string& objectName, 
                                                           const std::string& functionName, 
                                                           FunctionState state) {
    std::string suffix;
    switch (state) {
        case FunctionState::VOID_A:
            suffix = "_VoidA";
            break;
        case FunctionState::VOID_B:
            suffix = "_VoidB";
            break;
        case FunctionState::VOID:
            suffix = "_Void";
            break;
        case FunctionState::NORMAL:
            suffix = "_Normal";
            break;
    }
    
    return "__CHTL_" + objectName + "_" + functionName + suffix + "_" + std::to_string(++globalCounter_);
}

void INeverAwayExtension::ParseFunctionDefinitions(const std::string& functionsBlock, VirtualObjectDef& virtualObj) {
    // 解析函数定义块
    // 语法文档示例：
    // Void<A>: function(int, int) { ... },
    // Void<B>: function(int, int) { ... },
    // Void: { ... },
    // Ax: { ... }
    
    std::regex functionRegex(R"((\w+(?:<\w+>)?)\s*:\s*(function\([^)]*\)\s*\{[^}]*\}|\{[^}]*\}))");
    std::sregex_iterator iter(functionsBlock.begin(), functionsBlock.end(), functionRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        const std::smatch& match = *iter;
        std::string functionKey = match[1].str();
        std::string functionBody = match[2].str();
        
        FunctionDef funcDef;
        funcDef.name = functionKey;
        funcDef.state = ParseFunctionState(functionKey);
        funcDef.body = functionBody;
        funcDef.globalName = GenerateGlobalFunctionName(virtualObj.name, functionKey, funcDef.state);
        
        // 解析参数列表
        std::regex paramRegex(R"(function\(([^)]*)\))");
        std::smatch paramMatch;
        if (std::regex_search(functionBody, paramMatch, paramRegex)) {
            std::string paramStr = paramMatch[1].str();
            if (!paramStr.empty()) {
                std::istringstream iss(paramStr);
                std::string param;
                while (std::getline(iss, param, ',')) {
                    funcDef.params.push_back(Utils::StringUtils::Trim(param));
                }
            }
        }
        
        virtualObj.functions[functionKey] = funcDef;
        virtualObj.globalFunctionCounter++;
    }
}

std::string INeverAwayExtension::GenerateINeverAwayCode(const std::string& objectName, 
                                                       const std::vector<FunctionDef>& functions) {
    std::ostringstream jsCode;
    
    // 生成iNeverAway的JavaScript实现
    jsCode << "// iNeverAway虚对象实现 - 珂朵莉模块\n";
    jsCode << "// 语法文档第1513-1515行：创建一组JS全局函数，名称由CHTL编译器统一管理\n";
    jsCode << "(function() {\n";
    jsCode << "  'use strict';\n";
    jsCode << "  \n";
    jsCode << "  // 虚对象 " << objectName << " 的函数重载系统\n";
    jsCode << "  const __" << objectName << "_StateManager = {\n";
    jsCode << "    currentState: 'NORMAL',\n";
    jsCode << "    stateStack: [],\n";
    jsCode << "    \n";
    jsCode << "    pushState: function(state) {\n";
    jsCode << "      this.stateStack.push(this.currentState);\n";
    jsCode << "      this.currentState = state;\n";
    jsCode << "    },\n";
    jsCode << "    \n";
    jsCode << "    popState: function() {\n";
    jsCode << "      if (this.stateStack.length > 0) {\n";
    jsCode << "        this.currentState = this.stateStack.pop();\n";
    jsCode << "      }\n";
    jsCode << "    },\n";
    jsCode << "    \n";
    jsCode << "    setState: function(state) {\n";
    jsCode << "      this.currentState = state;\n";
    jsCode << "    }\n";
    jsCode << "  };\n";
    jsCode << "  \n";
    
    // 为每个函数生成全局函数
    for (const auto& func : functions) {
        jsCode << "  // " << func.name << " 状态函数\n";
        jsCode << "  window." << func.globalName << " = ";
        
        if (func.body.find("function") != std::string::npos) {
            // 有参数的函数
            jsCode << func.body << ";\n";
        } else {
            // 无参数的对象
            jsCode << "function() {\n";
            jsCode << "    return " << func.body << ";\n";
            jsCode << "  };\n";
        }
        jsCode << "  \n";
    }
    
    // 生成虚对象调用分发器
    jsCode << "  // " << objectName << " 虚对象调用分发器\n";
    jsCode << "  window.__" << objectName << "_Dispatcher = {\n";
    
    // 为每个函数生成调用方法
    std::unordered_map<std::string, std::vector<FunctionDef>> functionGroups;
    for (const auto& func : functions) {
        std::string baseName = func.name;
        // 移除状态标记
        std::regex stateRegex(R"(<\w+>)");
        baseName = std::regex_replace(baseName, stateRegex, "");
        functionGroups[baseName].push_back(func);
    }
    
    bool first = true;
    for (const auto& group : functionGroups) {
        if (!first) jsCode << ",\n";
        first = false;
        
        jsCode << "    " << group.first << ": function() {\n";
        jsCode << "      const state = __" << objectName << "_StateManager.currentState;\n";
        jsCode << "      \n";
        
        // 根据状态分发到不同的函数
        for (const auto& func : group.second) {
            std::string stateCondition;
            switch (func.state) {
                case FunctionState::VOID_A:
                    stateCondition = "state === 'VOID_A'";
                    break;
                case FunctionState::VOID_B:
                    stateCondition = "state === 'VOID_B'";
                    break;
                case FunctionState::VOID:
                    stateCondition = "state === 'VOID'";
                    break;
                case FunctionState::NORMAL:
                    stateCondition = "state === 'NORMAL'";
                    break;
            }
            
            jsCode << "      if (" << stateCondition << ") {\n";
            jsCode << "        return window." << func.globalName << ".apply(this, arguments);\n";
            jsCode << "      }\n";
        }
        
        jsCode << "      \n";
        jsCode << "      // 默认调用普通状态函数\n";
        jsCode << "      console.warn('iNeverAway: 未找到状态匹配的函数 " << group.first << "');\n";
        jsCode << "      return null;\n";
        jsCode << "    }";
    }
    
    jsCode << "\n  };\n";
    jsCode << "  \n";
    
    // 生成状态切换辅助函数
    jsCode << "  // 状态切换辅助函数\n";
    jsCode << "  window." << objectName << " = {\n";
    jsCode << "    // 虚对象调用接口\n";
    
    first = true;
    for (const auto& group : functionGroups) {
        if (!first) jsCode << ",\n";
        first = false;
        
        jsCode << "    " << group.first << ": function() {\n";
        jsCode << "      return __" << objectName << "_Dispatcher." << group.first << ".apply(this, arguments);\n";
        jsCode << "    }";
    }
    
    jsCode << ",\n";
    jsCode << "    \n";
    jsCode << "    // 状态管理接口\n";
    jsCode << "    $setState: function(state) {\n";
    jsCode << "      __" << objectName << "_StateManager.setState(state);\n";
    jsCode << "      return this;\n";
    jsCode << "    },\n";
    jsCode << "    \n";
    jsCode << "    $pushState: function(state) {\n";
    jsCode << "      __" << objectName << "_StateManager.pushState(state);\n";
    jsCode << "      return this;\n";
    jsCode << "    },\n";
    jsCode << "    \n";
    jsCode << "    $popState: function() {\n";
    jsCode << "      __" << objectName << "_StateManager.popState();\n";
    jsCode << "      return this;\n";
    jsCode << "    }\n";
    jsCode << "  };\n";
    jsCode << "  \n";
    
    jsCode << "  console.log('iNeverAway虚对象已创建: " << objectName << "');\n";
    jsCode << "})();\n";
    
    return jsCode.str();
}



std::string INeverAwayExtension::GenerateVirtualCallCode(const std::string& objectName,
                                                        const std::string& functionName,
                                                        FunctionState state,
                                                        const std::vector<std::string>& args) {
    std::ostringstream jsCode;
    
    // 生成虚对象调用的JavaScript代码
    jsCode << "// 虚对象调用: " << objectName << "->" << functionName << "\n";
    jsCode << "(function() {\n";
    jsCode << "  if (typeof window." << objectName << " !== 'undefined') {\n";
    
    // 设置状态（如果需要）
    if (state != FunctionState::NORMAL) {
        std::string stateStr;
        switch (state) {
            case FunctionState::VOID_A:
                stateStr = "VOID_A";
                break;
            case FunctionState::VOID_B:
                stateStr = "VOID_B";
                break;
            case FunctionState::VOID:
                stateStr = "VOID";
                break;
            default:
                stateStr = "NORMAL";
                break;
        }
        
        jsCode << "    // 设置函数状态\n";
        jsCode << "    window." << objectName << ".$setState('" << stateStr << "');\n";
    }
    
    // 生成函数调用
    jsCode << "    // 调用虚对象函数\n";
    jsCode << "    const result = window." << objectName << "." << functionName << "(";
    
    for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0) jsCode << ", ";
        jsCode << args[i];
    }
    
    jsCode << ");\n";
    jsCode << "    \n";
    jsCode << "    // 恢复状态（如果设置了状态）\n";
    if (state != FunctionState::NORMAL) {
        jsCode << "    window." << objectName << ".$popState();\n";
    }
    
    jsCode << "    \n";
    jsCode << "    return result;\n";
    jsCode << "  } else {\n";
    jsCode << "    console.error('iNeverAway: 虚对象 " << objectName << " 未定义');\n";
    jsCode << "    return null;\n";
    jsCode << "  }\n";
    jsCode << "})();\n";
    
    return jsCode.str();
}

} // namespace Extensions
} // namespace CJMOD
} // namespace CHTL