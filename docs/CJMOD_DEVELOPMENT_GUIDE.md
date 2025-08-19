# CJMOD开发指南

## 概述

CJMOD (CHTL JavaScript Module) 是使用C++编写的CHTL JS语法扩展系统。通过CJMOD，开发者可以创建新的CHTL JS语法，扩展语言的表达能力。

---

## CJMOD核心概念

### 什么是CJMOD

CJMOD允许开发者：
- 使用C++定义新的CHTL JS语法
- 将自定义语法编译为标准JavaScript
- 扩展CHTL JS的功能边界
- 创建领域特定的语法糖

### CJMOD工作原理

```
CHTL JS源码 -> CJMOD扩展处理 -> 标准JavaScript代码
```

CJMOD本质上是一个语法转换器，将自定义的CHTL JS语法转换为浏览器可执行的JavaScript代码。

---

## 开发环境设置

### 前置要求

- C++17编译器
- CHTL开发环境
- 对JavaScript的深入理解
- 熟悉CHTL JS语法

### 创建CJMOD项目

```bash
# 创建CJMOD目录结构
mkdir MyExtension
cd MyExtension

mkdir src info
touch src/my_extension.cpp
touch src/my_extension.h
touch info/MyExtension.chtl
```

### 目录结构

```
MyExtension/
├── src/
│   ├── my_extension.cpp    # C++实现文件
│   ├── my_extension.h      # C++头文件
│   └── utils.cpp           # 工具函数（可选）
└── info/
    └── MyExtension.chtl    # 扩展信息文件
```

**二同名规则**: 扩展文件夹名和信息文件名必须相同。

---

## CJMOD专业级接口

### 核心API概览

CJMOD使用专业级接口系统，解决了transform等技术难题：

```cpp
#include "ProfessionalCJMODInterface.h"
using namespace chtl::cjmod;

// 完整的API流程
auto keyword = syntaxAnalys(pattern, ignoreChars);
keyword.args.bind(name, processor);
keyword.args.transform(name, transformer);
scanner.scanKeyword(keyword, callback);
keyword.args.match(name, value);
std::string code = generateCode(keyword);
```

### 1. syntaxAnalys - 语法分析

定义要扩展的CHTL JS语法模式：

```cpp
// 基础语法分析
const auto keyword1 = syntaxAnalys("myFunction arg1 arg2");

// 复杂语法分析（带忽略字符）
std::string ignoreChars = ",:{};()";
const auto keyword2 = syntaxAnalys(R"(
    printMylove({
        url: $,
        mode: $,
        width: $,
        height: $
    });
)", ignoreChars);

// 占位符语法
const auto keyword3 = syntaxAnalys("$ ** $");
```

**关键特性**:
- 支持占位符 `$`
- 支持忽略字符
- 自动参数解析
- 通配符 `**` 支持

### 2. bind - 参数绑定

为参数设置预处理函数：

```cpp
// 名称绑定
keyword.args.bind("url", [](const std::string& value) {
    // URL预处理：去除引号，验证格式
    std::string url = value;
    if (url.front() == '"') url = url.substr(1, url.length()-2);
    return "\"" + url + "\"";
});

// 占位符自动计数绑定
keyword.args.bind("$", [](const std::string& value) {
    // 自动绑定到下一个$占位符
    return "processed_" + value;
});

// 索引绑定
keyword.args[0].bind([](const std::string& value) {
    return "index0_" + value;
});
```

### 3. transform - 延迟转换（核心创新）

设置参数转换规则，在match时执行：

```cpp
// 函数转换 - 在match时获取实际值
keyword.args.transform("url", [](const std::string& processedValue) {
    return "const imageUrl = " + processedValue + ";";
});

// 智能转换 - 根据值类型选择转换策略
keyword.args.transform("width", [](const std::string& value) {
    if (value.find("%") != std::string::npos) {
        std::string num = value.substr(0, value.length()-1);
        return "const width = Math.floor(window.innerWidth * " + num + " / 100);";
    } else if (value.find("px") != std::string::npos) {
        return "const width = parseInt('" + value + "');";
    }
    return "const width = " + value + ";";
});

// 模板转换
keyword.args.transform("mode", "const renderMode = ${VALUE};");

// 条件转换
keyword.args.transformIf("param",
    [](const std::string& value) { return isFunction(value); },
    [](const std::string& value) { return "const callback = " + value + ";"; }
);
```

### 4. scanKeyword - 扫描器集成

注册语法到扫描器，获取上下文信息：

```cpp
scanner.scanKeyword("printMylove", [&keyword]() {
    // 使用peekKeyword获取上下文
    std::string urlValue = scanner.peekKeyword(1);
    std::string modeValue = scanner.peekKeyword(3);
    
    // 应用完整转换链
    keyword.args.match("url", urlValue);
    keyword.args.match("mode", modeValue);
});

// 高级上下文访问
scanner.scanKeyword("complexFunction", []() {
    std::string prev = scanner.peekKeyword(-1);  // 前一个关键字
    std::string next = scanner.peekKeyword(1);   // 后一个关键字
    std::string context = scanner.getContextInfo(); // 上下文信息
});
```

### 5. policy - 策略管理

处理复杂参数结构（函数、对象等）：

```cpp
scanner.scanKeyword("complexFunction", [&keyword]() {
    // 收集函数参数
    scanner.policyChangeBegin(":", Policy::COLLECT);
    std::string functionContent = scanner.policyChangeEnd("}", Policy::NORMAL);
    
    // 收集对象参数
    scanner.policyChangeBegin("{", Policy::COLLECT);
    std::string objectContent = scanner.policyChangeEnd("}", Policy::NORMAL);
    
    // 处理收集的内容
    keyword.args.match("callback", functionContent);
    keyword.args.match("config", objectContent);
});
```

### 6. slice - 参数切片

处理复杂参数的部分内容：

```cpp
// 基础切片
keyword.args.slice("functionParam", 0, 50, [](const std::string& sliced) {
    return "const partialContent = " + sliced + ";";
});

// 函数体切片
keyword.args.sliceFunctionBody("callback", [](const std::string& body) {
    return "const callbackBody = function() {" + body + "};";
});

// 对象属性切片
keyword.args.sliceObject("config", {"width", "height"}, [](const std::string& props) {
    return "const sizeConfig = " + props + ";";
});

// 数组元素切片
keyword.args.sliceArray("items", 0, 3, [](const std::string& elements) {
    return "const firstThree = " + elements + ";";
});
```

### 7. match - 参数匹配

应用完整的转换链（bind -> transform）：

```cpp
// 单个参数匹配
keyword.args.match("url", "avatar.jpg");  // 自动应用bind + transform

// 批量匹配
std::vector<std::string> values = {"avatar.jpg", "ASCII", "80%"};
keyword.args.matchSequential(values);

// 按模式匹配
keyword.args.matchByPattern("url:$,mode:$", {"avatar.jpg", "ASCII"});
```

### 8. result & generateCode - 代码生成

生成最终的JavaScript代码：

```cpp
// 获取转换后的参数代码
std::string argsCode = keyword.args.result();

// 生成完整的JavaScript实现
std::string completeCode = generateCode(keyword);

// 带优化的代码生成
std::string optimizedCode = keyword.generateOptimizedCode();
```

---

## 实战示例：开发printMylove扩展

### 完整实现

```cpp
// my_extension.cpp
#include "ProfessionalCJMODInterface.h"
using namespace chtl::cjmod;

void registerPrintMyloveExtension() {
    // 1. 语法分析
    std::string ignoreChars = ",:{};()";
    auto keyword = syntaxAnalys(R"(
        printMylove({
            url: $,
            mode: $,
            width: $,
            height: $,
            scale: $
        });
    )", ignoreChars);
    
    // 2. 设置元数据
    keyword.setMetadata("description", "图片转字符画扩展");
    keyword.setMetadata("category", "graphics");
    
    // 3. 参数绑定
    keyword.args.bind("url", [](const std::string& value) {
        // URL格式化
        return (value.front() == '"') ? value : ("\"" + value + "\"");
    });
    
    keyword.args.bind("mode", [](const std::string& value) {
        // 模式标准化
        std::string mode = value;
        if (mode.front() == '"') mode = mode.substr(1, mode.length()-2);
        return (mode == "ASCII" || mode == "Pixel") ? ("\"" + mode + "\"") : "\"ASCII\"";
    });
    
    // 4. 转换规则（解决transform问题）
    keyword.args.transform("url", [](const std::string& processedValue) {
        return "const imageUrl = " + processedValue + ";";
    });
    
    keyword.args.transform("mode", [](const std::string& processedValue) {
        return "const renderMode = " + processedValue + ".toLowerCase();";
    });
    
    keyword.args.transform("width", [](const std::string& value) {
        // 智能CSS单位处理
        if (value.find("%") != std::string::npos) {
            std::string num = value.substr(0, value.length()-1);
            return "const width = Math.floor(window.innerWidth * " + num + " / 100);";
        }
        return "const width = " + value + ";";
    });
    
    // 5. 扫描器集成
    scanner.scanKeyword("printMylove", [&keyword]() {
        // 获取参数值并应用转换
        keyword.args.match("url", scanner.peekKeyword(1));
        keyword.args.match("mode", scanner.peekKeyword(3));
        keyword.args.match("width", scanner.peekKeyword(5));
        keyword.args.match("height", scanner.peekKeyword(7));
        keyword.args.match("scale", scanner.peekKeyword(9));
    });
    
    // 6. 完整代码生成
    std::string jsCode = generateCode(keyword);
    // 自动包含printMylove的完整JavaScript实现
}

// 扩展入口点
extern "C" void initializeExtension() {
    registerPrintMyloveExtension();
}
```

### 信息文件

```chtl
// info/MyExtension.chtl
[Info] {
    name = "MyExtension";
    version = "1.0.0";
    description = "图片转字符画扩展";
    author = "开发者名";
    license = "MIT";
    category = "graphics";
    minCHTLVersion = "1.0.0";
    cjmodInterfaceVersion = "2.0.0";
}

[Export] {
    [Function] printMylove;
}
```

---

## 高级CJMOD功能

### 复杂语法处理

```cpp
// 处理泛型语法
auto keyword = syntaxAnalys("myFunction<T>($)");

// 处理嵌套结构
auto keyword2 = syntaxAnalys(R"(
    complexFunction({
        config: {
            nested: { value: $ }
        },
        callback: function($) { $ }
    });
)");

// 使用policy处理
scanner.scanKeyword("complexFunction", [&keyword2]() {
    // 收集嵌套对象
    scanner.policyChangeBegin("{", Policy::COLLECT);
    std::string nestedContent = scanner.policyChangeEnd("}", Policy::NORMAL);
    
    // 使用slice处理嵌套结构
    keyword2.args.sliceObject("config", {"nested"}, [](const std::string& obj) {
        return "const configObject = " + obj + ";";
    });
});
```

### 类型系统集成

```cpp
// 自动类型推断
keyword.args.inferTypes();

// 类型验证
keyword.args.setValidator("url", [](const std::string& value) {
    return value.find("http") == 0 || value.find("data:") == 0;
});

// 类型转换
keyword.args.transform("param", [](const std::string& value) {
    if (isNumber(value)) {
        return "parseInt(" + value + ")";
    } else if (isString(value)) {
        return value;
    } else if (isObject(value)) {
        return "JSON.parse(" + value + ")";
    }
    return value;
});
```

### 性能优化

```cpp
// 启用结果缓存
keyword.args.enableCaching(true);

// 并行参数处理
keyword.args.enableParallelProcessing(true);

// 代码优化
keyword.setGenerationOption("enableOptimization", true);
keyword.setGenerationOption("enableMinification", true);
```

---

## 实战案例

### 案例1：动画扩展

```cpp
void registerAnimateExtension() {
    auto keyword = syntaxAnalys(R"(
        smoothAnimate({
            target: $,
            duration: $,
            properties: { $: [$, $] }
        });
    )", ",:{};()[]");
    
    // 目标元素处理
    keyword.args.bind("target", [](const std::string& value) {
        if (isCHTLJSSelector(value)) {
            std::string selector = value.substr(2, value.length()-4);
            return "document.querySelector('" + selector + "')";
        }
        return value;
    });
    
    // 转换规则
    keyword.args.transform("target", [](const std::string& processed) {
        return "const animTarget = " + processed + ";";
    });
    
    keyword.args.transform("duration", [](const std::string& value) {
        return "const animDuration = " + value + " || 1000;";
    });
    
    // 使用policy处理属性对象
    scanner.scanKeyword("smoothAnimate", [&keyword]() {
        scanner.policyChangeBegin("properties:", Policy::COLLECT);
        std::string props = scanner.policyChangeEnd("}", Policy::NORMAL);
        
        keyword.args.match("target", scanner.peekKeyword(1));
        keyword.args.match("duration", scanner.peekKeyword(3));
        keyword.args.match("properties", props);
    });
}
```

### 案例2：数据绑定扩展

```cpp
void registerDataBindExtension() {
    auto keyword = syntaxAnalys("bindData($, $)");
    
    // 双向绑定处理
    keyword.args.bind("element", [](const std::string& value) {
        return isCHTLJSSelector(value) ? 
               "document.querySelector('" + value.substr(2, value.length()-4) + "')" : 
               value;
    });
    
    keyword.args.bind("data", [](const std::string& value) {
        return "reactive(" + value + ")";
    });
    
    // 生成双向绑定代码
    keyword.args.transform("element", [](const std::string& element) {
        return "const bindElement = " + element + ";";
    });
    
    keyword.args.transform("data", [](const std::string& data) {
        return "const dataSource = " + data + ";";
    });
    
    scanner.scanKeyword("bindData", [&keyword]() {
        keyword.args.match("element", scanner.peekKeyword(1));
        keyword.args.match("data", scanner.peekKeyword(2));
    });
}
```

### 案例3：状态管理扩展

```cpp
void registerStateExtension() {
    auto keyword = syntaxAnalys(R"(
        useState({
            initial: $,
            actions: {
                $: function($) { $ }
            }
        });
    )");
    
    // 状态初始值处理
    keyword.args.bind("initial", [](const std::string& value) {
        return isObject(value) ? value : "{value: " + value + "}";
    });
    
    // 动作函数处理
    keyword.args.bind("actions", [](const std::string& value) {
        return value; // 保持原样，让slice处理
    });
    
    // 使用slice处理动作对象
    scanner.scanKeyword("useState", [&keyword]() {
        // 处理初始状态
        keyword.args.match("initial", scanner.peekKeyword(1));
        
        // 使用policy收集动作
        scanner.policyChangeBegin("actions:", Policy::COLLECT);
        std::string actions = scanner.policyChangeEnd("}", Policy::NORMAL);
        
        // 使用slice处理动作函数
        keyword.args.sliceObject("actions", {"setState", "getState"}, [](const std::string& actionObj) {
            return "const stateActions = " + actionObj + ";";
        });
    });
}
```

---

## CJMOD打包和部署

### 编译CJMOD

```bash
# 编译C++源文件
g++ -std=c++17 -fPIC -shared src/*.cpp -o libMyExtension.so

# 或使用CMake
mkdir build && cd build
cmake ..
make MyExtension
```

### 打包CJMOD

```bash
# 使用CHTL编译器打包
chtl pack-cjmod ./MyExtension ./MyExtension.cjmod

# 或使用打包脚本
./scripts/packaging/pack_modules_enhanced.sh
```

### 安装和使用

```bash
# 安装到官方模块目录
cp MyExtension.cjmod /usr/local/bin/module/CJMOD/

# 或安装到用户模块目录
cp MyExtension.cjmod ./module/CJMOD/
```

在CHTL文件中使用：

```chtl
[Import] @CJmod from MyExtension

script {
    // 使用扩展的语法
    printMylove({
        url: "image.jpg",
        mode: "ASCII",
        width: "100px"
    });
}
```

---

## 调试和测试

### CJMOD调试

```cpp
// 启用调试模式
enableCJMODDebug(true);

// 输出调试信息
logCJMODInfo("Processing parameter: " + paramName);

// 获取诊断信息
std::string diagnostics = getCJMODDiagnostics();
std::cout << diagnostics << std::endl;
```

### 测试CJMOD扩展

```cpp
// 创建测试
void testMyExtension() {
    auto keyword = syntaxAnalys("myFunction($)");
    keyword.args.bind("param", [](const std::string& value) {
        return "test_" + value;
    });
    
    keyword.args.transform("param", [](const std::string& value) {
        return "const result = " + value + ";";
    });
    
    keyword.args.match("param", "input");
    
    std::string result = keyword.args.result();
    assert(result == "const result = test_input;\n");
}
```

---

## 最佳实践

### 设计原则

1. **简洁性**: API设计简洁直观
2. **一致性**: 与CHTL JS语法风格一致
3. **性能**: 生成高效的JavaScript代码
4. **安全性**: 验证输入参数，防止注入

### 错误处理

```cpp
// 参数验证
keyword.args.setValidator("url", [](const std::string& value) {
    if (value.empty()) {
        logCJMODInfo("URL不能为空");
        return false;
    }
    return true;
});

// 异常处理
try {
    std::string code = generateCode(keyword);
} catch (const std::exception& e) {
    logCJMODInfo("代码生成失败: " + std::string(e.what()));
}
```

### 性能考虑

1. **缓存结果**: 避免重复计算
2. **延迟计算**: 只在需要时计算
3. **内存管理**: 及时释放资源
4. **代码优化**: 生成高效的JavaScript

---

## 社区贡献

### 开源CJMOD扩展

1. **选择MIT协议**: 保持开源友好
2. **完整文档**: 提供详细的使用文档
3. **示例代码**: 包含丰富的使用示例
4. **测试覆盖**: 确保扩展的可靠性

### 扩展质量标准

1. **功能完整**: 扩展功能完整可用
2. **性能优秀**: 生成高效的JavaScript代码
3. **兼容性好**: 与现有CHTL JS语法兼容
4. **文档完善**: 提供完整的开发和使用文档

---

通过CJMOD系统，开发者可以无限扩展CHTL JS的能力，创造出更强大、更便捷的Web开发语法。我们期待看到更多创新的CJMOD扩展贡献给社区！