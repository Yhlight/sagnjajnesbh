# CHTL JS开发指南

## 概述

CHTL JS是CHTL的JavaScript扩展语法，为开发者提供更强大的前端交互能力。本指南介绍如何开发和扩展CHTL JS功能。

---

## CHTL JS架构

### 核心组件

```
CHTLJSCompiler/
├── Core/                   # 核心组件
│   ├── AutoAdditionManager # 自动添加管理器
│   └── JSEventRegistry     # JS事件注册表
├── Event/                  # 事件系统
│   ├── ListenSystemProcessor    # listen函数处理器
│   └── DelegateSystemProcessor  # delegate函数处理器
├── Selector/               # 选择器系统
│   └── EnhancedSelectorProcessor # 增强选择器处理器
├── Virtual/                # 虚对象系统
│   └── VirtualObjectProcessor   # 虚对象处理器
├── Animation/              # 动画系统
│   └── AnimateSystemProcessor   # 动画处理器
└── Module/                 # 模块和扩展
    └── ProfessionalCJMODInterface # CJMOD扩展接口
```

### 编译流程

1. **扫描**: 统一扫描器识别CHTL JS语法
2. **解析**: 各个处理器解析特定语法
3. **转换**: 转换为标准JavaScript代码
4. **优化**: 代码优化和压缩
5. **输出**: 生成最终JavaScript文件

---

## 增强选择器开发

### 选择器语法

CHTL JS增强选择器使用 `{{}}` 语法：

```chtl
script {
    {{.button}}      // 类选择器
    {{#header}}      // ID选择器  
    {{div}}          // 标签选择器
    {{button[0]}}    // 索引选择器
    {{&}}            // 当前元素
    {{.nav a}}       // 后代选择器
}
```

### 开发增强选择器处理器

```cpp
#include "EnhancedSelectorProcessor.h"

class MyCustomSelectorProcessor : public EnhancedSelectorProcessor {
public:
    std::string processSelector(const std::string& selector) override {
        if (selector.starts_with(".")) {
            // 处理类选择器
            return "document.querySelectorAll('" + selector + "')";
        } else if (selector.starts_with("#")) {
            // 处理ID选择器
            return "document.getElementById('" + selector.substr(1) + "')";
        }
        
        return EnhancedSelectorProcessor::processSelector(selector);
    }
};
```

### 注册自定义选择器

```cpp
void registerCustomSelector() {
    auto processor = std::make_shared<MyCustomSelectorProcessor>();
    CHTLJSCompilerCore::getInstance().registerSelectorProcessor(processor);
}
```

---

## 事件系统开发

### JS事件注册表

CHTL JS支持所有82个标准JavaScript事件：

```cpp
// 查看支持的事件
JSEventRegistry& registry = JSEventRegistry::getInstance();
auto events = registry.getAllEvents();

// 添加自定义事件
registry.addCustomEvent("myCustomEvent", EventCategory::CUSTOM);
```

### 开发listen函数扩展

```cpp
class CustomListenProcessor : public ListenSystemProcessor {
public:
    std::string processListenCall(const std::string& target, 
                                 const std::map<std::string, std::string>& events) override {
        std::string code = target + ".addEventListener(";
        
        for (const auto& [event, handler] : events) {
            if (isValidEvent(event)) {
                code += "'" + event + "', " + handler + ");";
            }
        }
        
        return code;
    }
    
private:
    bool isValidEvent(const std::string& event) {
        return JSEventRegistry::getInstance().hasEvent(event);
    }
};
```

### 开发delegate函数扩展

```cpp
class CustomDelegateProcessor : public DelegateSystemProcessor {
public:
    std::string processDelegateCall(const std::string& container,
                                   const std::string& target,
                                   const std::map<std::string, std::string>& events) override {
        std::string code = container + ".addEventListener('click', function(e) {";
        code += "if (e.target.matches('" + target + "')) {";
        
        for (const auto& [event, handler] : events) {
            code += "(" + handler + ")(e);";
        }
        
        code += "}});";
        return code;
    }
};
```

---

## 虚对象系统开发

### 虚对象概念

虚对象是编译时语法糖，不创建运行时对象：

```chtl
script {
    vir Manager = listen({
        init: function() { /* 初始化 */ },
        destroy: function() { /* 销毁 */ }
    });
    
    Manager->init();     // 编译为: Manager_init();
    Manager->destroy();  // 编译为: Manager_destroy();
}
```

### 开发虚对象处理器

```cpp
class VirtualObjectProcessor {
public:
    struct VirtualObject {
        std::string name;
        std::unordered_map<std::string, std::string> methods;
        std::string baseFunction; // listen, delegate, animate等
    };
    
    std::string processVirtualObject(const VirtualObject& vobj) {
        std::string code = "";
        
        // 生成实际函数
        for (const auto& [methodName, methodBody] : vobj.methods) {
            std::string functionName = vobj.name + "_" + methodName;
            code += "function " + functionName + "() {" + methodBody + "}\n";
        }
        
        return code;
    }
    
    std::string processVirtualCall(const std::string& objName, const std::string& methodName) {
        return objName + "_" + methodName + "()";
    }
};
```

---

## 动画系统开发

### animate函数架构

```cpp
class AnimateSystemProcessor {
public:
    struct AnimationConfig {
        std::string target;
        int duration;
        std::string easing;
        std::map<std::string, std::vector<std::string>> properties;
        std::string onComplete;
    };
    
    std::string processAnimate(const AnimationConfig& config) {
        std::string code = "requestAnimationFrame(function animate() {";
        code += "const target = " + config.target + ";";
        code += "const duration = " + std::to_string(config.duration) + ";";
        
        // 生成动画逻辑
        code += generateAnimationLogic(config);
        
        code += "});";
        return code;
    }
    
private:
    std::string generateAnimationLogic(const AnimationConfig& config) {
        // 实现动画逻辑生成
        return "/* 动画实现 */";
    }
};
```

---

## 自动添加系统开发

### 自动添加规则

```cpp
class AutoAdditionManager {
public:
    enum class AdditionType {
        CLASS,
        ID,
        BOTH
    };
    
    struct AdditionRule {
        std::string selector;      // 触发选择器
        AdditionType type;         // 添加类型
        std::string priority;      // 优先级规则
    };
    
    void processAutoAddition(const std::string& elementContext) {
        // 分析元素上下文
        ElementInfo info = analyzeElement(elementContext);
        
        // 应用自动添加规则
        if (shouldAddClass(info)) {
            addClassToElement(info);
        }
        
        if (shouldAddId(info)) {
            addIdToElement(info);
        }
    }
    
private:
    bool shouldAddClass(const ElementInfo& info) {
        // 实现"缺什么，自动添加什么"逻辑
        return !info.hasClass && info.hasClassSelector;
    }
    
    bool shouldAddId(const ElementInfo& info) {
        return !info.hasId && info.hasIdSelector;
    }
};
```

---

## 性能优化

### 编译优化

1. **AST缓存**: 缓存解析结果
2. **代码压缩**: 移除不必要的空白和注释
3. **死代码消除**: 移除未使用的代码
4. **内联优化**: 内联小函数

### 运行时优化

1. **事件委托**: 减少事件监听器数量
2. **选择器缓存**: 缓存DOM查询结果
3. **动画优化**: 使用requestAnimationFrame
4. **内存管理**: 及时清理事件监听器

---

## 扩展开发

### 创建新的CHTL JS语法

1. **设计语法**: 定义新的语法规则
2. **创建处理器**: 实现语法处理器
3. **集成编译器**: 注册到编译器核心
4. **编写测试**: 验证功能正确性
5. **更新文档**: 添加语法文档

### 示例：创建新的事件语法

```cpp
// 1. 定义语法处理器
class CustomEventProcessor {
public:
    std::string processCustomEvent(const std::string& syntax) {
        // 解析自定义事件语法
        // 生成标准JavaScript代码
        return generatedCode;
    }
};

// 2. 注册处理器
void registerCustomEventProcessor() {
    auto processor = std::make_shared<CustomEventProcessor>();
    CHTLJSCompilerCore::getInstance().registerProcessor("customEvent", processor);
}

// 3. 在模块初始化时注册
extern "C" void initializeModule() {
    registerCustomEventProcessor();
}
```

---

## 调试和诊断

### CHTL JS调试

```cpp
// 启用CHTL JS调试模式
CHTLJSCompilerCore::getInstance().enableDebugMode(true);

// 查看编译过程
auto debugInfo = CHTLJSCompilerCore::getInstance().getDebugInfo();
std::cout << debugInfo << std::endl;
```

### 性能分析

```cpp
// 性能计时
auto start = std::chrono::high_resolution_clock::now();
// ... 执行编译
auto end = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
std::cout << "编译时间: " << duration.count() << "ms" << std::endl;
```

---

## 最佳实践

### 代码组织

1. **单一职责**: 每个处理器专注一个功能
2. **松耦合**: 减少组件间的依赖
3. **高内聚**: 相关功能组织在一起
4. **可测试**: 编写可测试的代码

### 错误处理

1. **优雅降级**: 遇到错误时提供合理的默认行为
2. **详细信息**: 提供有用的错误信息
3. **恢复机制**: 尽可能从错误中恢复
4. **用户友好**: 错误信息对用户友好

### 性能考虑

1. **避免重复工作**: 缓存计算结果
2. **延迟计算**: 只在需要时计算
3. **内存效率**: 及时释放不需要的资源
4. **算法效率**: 选择高效的算法和数据结构

---

CHTL JS为Web开发提供了强大的交互能力，通过本指南，开发者可以深入理解CHTL JS的工作原理，并参与到CHTL JS的发展中来。