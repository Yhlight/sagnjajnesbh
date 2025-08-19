# 🚀 CJMOD实际创建和使用指南

## 📋 **完整的CJMOD开发流程**

### 第一步：创建CJMOD项目

```bash
# 编译CJMOD创建工具
cd tools
g++ -std=c++17 create_cjmod.cpp -o create_cjmod

# 创建新的CJMOD项目
./create_cjmod MyAwesomeExtension ./projects
```

这会创建以下结构：
```
MyAwesomeExtension/
├── src/
│   └── main.cpp           # C++语法扩展代码
├── info/
│   └── MyAwesomeExtension.chtl  # 模块信息 (二同名规则)
├── CMakeLists.txt         # 编译配置
├── build.sh              # Linux/macOS编译脚本
├── build.bat              # Windows编译脚本
└── example.chtl           # 使用示例
```

### 第二步：编写语法扩展

编辑 `src/main.cpp`：

```cpp
#include <string>
#include <regex>
#include <sstream>

// 创建你的语法扩展
std::string processMyCustomSyntax(const std::string& chtljs_code) {
    // 1. 用正则表达式匹配你的CHTL JS语法
    std::regex myRegex(R"(myCustomFunction\s*\(\s*['"](.*?)['"],\s*(.*?)\s*\))");
    std::smatch matches;
    
    if (!std::regex_search(chtljs_code, matches, myRegex)) {
        return ""; // 不匹配就返回空
    }
    
    // 2. 提取参数
    std::string param1 = matches[1].str();
    std::string param2 = matches[2].str();
    
    // 3. 生成对应的JavaScript代码
    std::stringstream js;
    js << "(() => {\n";
    js << "    console.log('🚀 My Custom Function:', '" << param1 << "', " << param2 << ");\n";
    js << "    // 在这里写你想要生成的JavaScript逻辑\n";
    js << "    const result = '" << param1 << "' + '_processed_' + " << param2 << ";\n";
    js << "    return result;\n";
    js << "})()";
    
    return js.str();
}

// CJMOD入口函数 (必须有这个函数)
extern "C" std::string processMyAwesomeExtensionSyntax(const std::string& chtljs_code) {
    std::string result;
    
    // 尝试你的语法扩展
    result = processMyCustomSyntax(chtljs_code);
    if (!result.empty()) return result;
    
    // 可以添加更多语法扩展...
    
    return chtljs_code; // 不匹配就返回原代码
}
```

### 第三步：编译CJMOD

```bash
# Linux/macOS
cd MyAwesomeExtension
./build.sh

# Windows
cd MyAwesomeExtension
build.bat
```

编译成功后会生成：`MyAwesomeExtension.cjmod`

### 第四步：安装和使用CJMOD

```bash
# 1. 复制到CHTL项目的module目录
cp MyAwesomeExtension.cjmod /path/to/your/chtl/project/module/

# 2. 在你的CHTL文件中导入
```

在你的 `.chtl` 文件中：

```chtl
// 导入CJMOD
[Import] @CJmod from MyAwesomeExtension

html {
    body {
        div {
            button {
                id: test-btn;
                text { 测试我的扩展 }
            }
            
            script {
                {{#test-btn}}->listen({
                    click: function() {
                        // 🚀 使用你创建的新语法！
                        const result = myCustomFunction('Hello', 123);
                        console.log('结果:', result);
                    }
                });
            }
        }
    }
}
```

### 第五步：CHTL编译器处理

当CHTL编译器遇到 `myCustomFunction('Hello', 123)` 时：

1. **识别CJMOD语法** - 发现这不是标准CHTL JS语法
2. **调用C++函数** - 调用 `processMyAwesomeExtensionSyntax()`
3. **获得JavaScript代码** - C++函数返回生成的JS代码
4. **插入到输出** - 将生成的JS代码插入到最终的HTML中

最终生成的HTML包含：

```javascript
// 原来的CHTL JS: myCustomFunction('Hello', 123)
// 变成了这样的JavaScript代码:
(() => {
    console.log('🚀 My Custom Function:', 'Hello', 123);
    const result = 'Hello' + '_processed_' + 123;
    return result;
})()
```

## 🎯 **实际开发示例**

### 创建一个表单验证扩展：

```cpp
// src/main.cpp
std::string processValidateExtension(const std::string& chtljs_code) {
    std::regex validateRegex(R"(validate\s*\(\s*\{\{([^}]+)\}\},\s*['"](.*?)['\"]\s*\))");
    std::smatch matches;
    
    if (std::regex_search(chtljs_code, matches, validateRegex)) {
        std::string selector = matches[1].str();
        std::string rule = matches[2].str();
        
        std::stringstream js;
        js << "(() => {\n";
        js << "    const element = document.querySelector('" << selector << "');\n";
        js << "    const value = element.value;\n";
        
        if (rule == "email") {
            js << "    const isValid = /^[^\\s@]+@[^\\s@]+\\.[^\\s@]+$/.test(value);\n";
        } else if (rule == "phone") {
            js << "    const isValid = /^1[3-9]\\d{9}$/.test(value);\n";
        } else {
            js << "    const isValid = value.length > 0;\n";
        }
        
        js << "    element.style.borderColor = isValid ? 'green' : 'red';\n";
        js << "    return isValid;\n";
        js << "})()";
        
        return js.str();
    }
    
    return "";
}
```

### 在CHTL JS中使用：

```chtl
script {
    {{#submit-btn}}->listen({
        click: function() {
            // 🚀 使用你的验证扩展语法
            const emailValid = validate({{#email}}, 'email');
            const phoneValid = validate({{#phone}}, 'phone');
            
            if (emailValid && phoneValid) {
                alert('表单验证通过！');
            }
        }
    });
}
```

## 💡 **关键理解**

CJMOD的核心是：
1. **C++处理语法** - 解析CHTL JS，生成JavaScript
2. **编译时转换** - 在编译阶段完成转换
3. **运行时执行** - 浏览器执行标准JavaScript

这样开发者可以：
- ✅ **创造新语法** - 用C++的强大能力
- ✅ **简化使用** - CHTL JS语法简洁直观
- ✅ **保证兼容** - 最终是标准JavaScript
- ✅ **提高效率** - 复杂逻辑用C++处理，简单使用

**这就是CJMOD的实际创建和使用方式！** 🎉