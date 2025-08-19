# 📚 CJMOD开发接口参考手册

## 🎯 **核心接口：ICJMODExtension**

所有CJMOD扩展都必须继承这个接口：

```cpp
class ICJMODExtension {
public:
    // 基本信息
    virtual std::string getExtensionName() const = 0;
    virtual std::string getExtensionVersion() const = 0;
    virtual std::vector<std::string> getSupportedSyntax() const = 0;
    
    // 核心功能
    virtual SyntaxMatch parseSyntax(const std::string& chtljs_code) const = 0;
    virtual std::string generateJavaScript(const SyntaxMatch& match, const CJMODContext& context) const = 0;
    
    // 辅助功能
    virtual bool validateParameters(const SyntaxMatch& match, std::vector<CJMODError>& errors) const = 0;
    virtual std::string getSyntaxDocumentation(const std::string& syntaxName) const = 0;
    
    // 生命周期
    virtual bool initialize(const CJMODContext& context) = 0;
    virtual void cleanup() = 0;
};
```

## 🛠️ **开发步骤详解**

### 步骤1：继承接口

```cpp
#include "CJMODInterface.h"

class MyAwesomeExtension : public ICJMODExtension {
public:
    // 实现所有虚函数...
};
```

### 步骤2：实现基本信息

```cpp
std::string getExtensionName() const override {
    return "MyAwesomeExtension";  // 扩展名称
}

std::string getExtensionVersion() const override {
    return "1.0.0";  // 版本号
}

std::vector<std::string> getSupportedSyntax() const override {
    return {
        "myFunction",     // 支持的语法1
        "myOtherSyntax",  // 支持的语法2
        "coolFeature"     // 支持的语法3
    };
}
```

### 步骤3：实现语法解析 ⭐ **最重要**

```cpp
SyntaxMatch parseSyntax(const std::string& chtljs_code) const override {
    SyntaxMatch match;
    
    // 解析你的语法：myFunction('参数1', 参数2)
    std::regex myRegex(R"(myFunction\s*\(\s*['"](.*?)['"],\s*(.*?)\s*\))");
    std::smatch matches;
    
    if (std::regex_search(chtljs_code, matches, myRegex)) {
        match.matched = true;
        match.syntaxName = "myFunction";
        match.parameters.push_back(matches[1].str()); // 参数1
        match.parameters.push_back(matches[2].str()); // 参数2
        return match;
    }
    
    // 没有匹配
    match.matched = false;
    return match;
}
```

### 步骤4：实现JavaScript生成 ⭐ **最核心**

```cpp
std::string generateJavaScript(const SyntaxMatch& match, const CJMODContext& context) const override {
    if (match.syntaxName == "myFunction") {
        std::string param1 = match.parameters[0];
        std::string param2 = match.parameters[1];
        
        // 生成对应的JavaScript代码
        std::stringstream js;
        js << "(() => {\n";
        js << "    console.log('🚀 MyFunction调用:', '" << param1 << "', " << param2 << ");\n";
        js << "    \n";
        js << "    // 在这里写你想要的JavaScript逻辑\n";
        js << "    const result = '" << param1 << "' + '_processed_' + " << param2 << ";\n";
        js << "    \n";
        js << "    // 可以操作DOM\n";
        js << "    const element = document.createElement('div');\n";
        js << "    element.textContent = result;\n";
        js << "    document.body.appendChild(element);\n";
        js << "    \n";
        js << "    return result;\n";
        js << "})()";
        
        return js.str();
    }
    
    return "";
}
```

### 步骤5：注册扩展

```cpp
// 使用宏简化注册
CJMOD_REGISTER_EXTENSION(MyAwesomeExtension)

// 或者手动实现：
extern "C" ICJMODExtension* createCJMODExtension() {
    return new MyAwesomeExtension();
}

extern "C" void destroyCJMODExtension(ICJMODExtension* extension) {
    delete extension;
}
```

## 🔧 **辅助工具：CJMODHelper**

### 选择器解析

```cpp
// 自动处理CHTL JS选择器
std::string selector = CJMODHelper::parseSelector("#myId");
// 结果: "document.getElementById('myId')"

std::string selector2 = CJMODHelper::parseSelector(".myClass");
// 结果: "document.querySelector('.myClass')"

std::string selector3 = CJMODHelper::parseSelector("button[0]");
// 结果: "document.querySelectorAll('button')[0]"
```

### 字符串处理

```cpp
// 转义JavaScript字符串
std::string safe = CJMODHelper::escapeJavaScriptString("包含'引号'的字符串");
// 结果: "包含\\'引号\\'的字符串"

// 生成安全的标识符
std::string id = CJMODHelper::generateSafeIdentifier("my-cool-name");
// 结果: "my_cool_name"

// 生成唯一ID
std::string uniqueId = CJMODHelper::generateUniqueId("toast");
// 结果: "toast_1234_5678"
```

## 📋 **完整开发模板**

```cpp
#include "CJMODInterface.h"
#include <regex>
#include <sstream>

using namespace chtl::cjmod;

class MyExtension : public ICJMODExtension {
public:
    // 1. 基本信息
    std::string getExtensionName() const override { return "MyExtension"; }
    std::string getExtensionVersion() const override { return "1.0.0"; }
    std::vector<std::string> getSupportedSyntax() const override { 
        return {"mySyntax"}; 
    }
    
    // 2. 语法支持检查
    bool supportsSyntax(const std::string& name) const override { 
        return name == "mySyntax"; 
    }
    
    // 3. 解析CHTL JS语法 ⭐
    SyntaxMatch parseSyntax(const std::string& chtljs_code) const override {
        SyntaxMatch match;
        
        // 你的解析逻辑
        std::regex myRegex(R"(mySyntax\s*\(\s*['"](.*?)['\"]\s*\))");
        std::smatch matches;
        
        if (std::regex_search(chtljs_code, matches, myRegex)) {
            match.matched = true;
            match.syntaxName = "mySyntax";
            match.parameters.push_back(matches[1].str());
        }
        
        return match;
    }
    
    // 4. 生成JavaScript代码 ⭐
    std::string generateJavaScript(const SyntaxMatch& match, const CJMODContext& context) const override {
        if (match.syntaxName == "mySyntax") {
            std::string param = match.parameters[0];
            
            std::stringstream js;
            js << "(() => {\n";
            js << "    console.log('🚀 MySyntax:', '" << param << "');\n";
            js << "    // 你的JavaScript逻辑\n";
            js << "    alert('Hello from MySyntax: " << param << "');\n";
            js << "})()";
            
            return js.str();
        }
        return "";
    }
    
    // 5. 参数验证
    bool validateParameters(const SyntaxMatch& match, std::vector<CJMODError>& errors) const override {
        if (match.parameters.empty()) {
            errors.push_back({"mySyntax需要至少1个参数", "", 0, 0, "error"});
            return false;
        }
        return true;
    }
    
    // 6. 文档
    std::string getSyntaxDocumentation(const std::string& syntaxName) const override {
        return "mySyntax(message) - 显示自定义消息";
    }
    
    // 7. 生命周期
    bool initialize(const CJMODContext& context) override { return true; }
    void cleanup() override {}
};

// 8. 注册扩展
CJMOD_REGISTER_EXTENSION(MyExtension)
```

## 🎯 **实际使用流程**

### 开发者视角：

1. **写C++扩展**：
```cpp
// 解析: myAlert('Hello World')
// 生成: alert('Hello World'); console.log('🚀 Custom Alert');
```

2. **编译成.cjmod**：
```bash
cmake . && make
# 生成: MyExtension.cjmod
```

### 用户视角：

3. **导入CJMOD**：
```chtl
[Import] @CJmod from MyExtension
```

4. **使用新语法**：
```chtl
script {
    myAlert('Hello World');  // 🚀 你创造的新语法！
}
```

### CHTL编译器处理：

5. **编译时转换**：
```
CHTL JS: myAlert('Hello World')
↓ 调用C++函数
↓ 生成JavaScript: alert('Hello World'); console.log('🚀 Custom Alert');
↓ 插入到最终HTML
```

## 💡 **关键理解**

**接口的核心作用**：
- 🎯 **统一标准** - 所有CJMOD都遵循相同接口
- 🔧 **简化开发** - 提供辅助工具和模板
- 🚀 **保证兼容** - 与CHTL编译器完美集成
- 📚 **清晰文档** - 每个方法都有明确用途

**这就是CJMOD的开发接口！** 🌟