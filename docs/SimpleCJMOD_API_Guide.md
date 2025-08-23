# 简洁强大的CJMOD API使用指南

## 设计理念

**使用简单，功能强大，很好用！**

- 只有**三个核心类**：`Syntax`、`Arg`、`CHTLJSFunction`
- 既有**标准流程**又有**简化流程**
- 内置正确的**双指针扫描机制**和**前置截取机制**

## 核心架构

### 1. Arg类 - 参数处理核心

```cpp
class Arg {
public:
    // 核心特性：operator()重载
    std::string operator()(const std::string& value);
    
    // 智能bind：支持任意类型处理器
    template<typename T>
    void bind(const std::function<std::string(T)>& processor);
    
    // 自动类型转换：string, int, double, bool
};
```

**使用方式**：
```cpp
arg.bind<string>([](const string& url) { return "\"" + url + "\""; });
arg.bind<int>([](int width) { return to_string(width); });

// 直接处理值
string result = arg("input_value");
```

### 2. Syntax类 - 语法分析结果

```cpp
class Syntax {
public:
    // 索引访问：syntax[0]
    Arg& operator[](size_t index);
    
    // 链式绑定：支持连续bind操作
    template<typename T>
    Syntax& bind(const std::function<std::string(T)>& processor);
    
    // 批量处理：一次性处理所有参数
    void match(const std::vector<std::string>& values);
    std::string result() const;
};
```

**使用方式**：
```cpp
syntax->bind<string>([](const string& s) { return process(s); })
      ->bind<int>([](int n) { return to_string(n); })
      ->bind<double>([](double d) { return to_string(d); });
```

### 3. CHTLJSFunction类 - 简化流程的核心

```cpp
class CHTLJSFunction {
public:
    // 极简设计：函数名 + 参数名
    CHTLJSFunction(const std::string& functionName, 
                   const std::vector<std::string>& paramNames = {});
    
    // 链式绑定：一步到位的处理器绑定
    template<typename T>
    CHTLJSFunction& bind(const std::function<std::string(T)>& processor);
    
    // 一键处理：标准流程自动化
    std::string process(const std::string& sourceCode, 
                       const std::vector<std::string>& values = {});
};
```

## 使用模式

### 模式1：标准流程（手动控制）

```cpp
// 1. 语法分析
auto syntax = syntaxAnalys(R"(
    printMylove({
        url: $,
        mode: $,
        width: $
    });
)");

// 2. 链式绑定
syntax->bind<string>([](const string& url) { return "\"" + url + "\""; })
      ->bind<string>([](const string& mode) { return "\"" + mode + "\""; })
      ->bind<int>([](int width) { return to_string(width); });

// 3. 匹配值
syntax->match({"avatar.png", "center", "800"});

// 4. 生成代码
string result = generateCode(*syntax);
```

### 模式2：简化流程（一步到位）

```cpp
// 1. 创建函数
auto func = createCHTLJSFunction("printMylove", {"url", "mode", "width"});

// 2. 链式绑定
func->bind<string>("url", [](const string& url) { return "loadImage(\"" + url + "\")"; })
    ->bind<string>("mode", [](const string& mode) { return "\"" + mode + "\""; })
    ->bind<int>("width", [](int width) { return to_string(width); });

// 3. 一键处理
string result = func->process(sourceCode, {"avatar.png", "center", "800"});
```

### 模式3：占位符模式（最简单）

```cpp
// 自动占位符
auto func = createCHTLJSFunction("animate");

func->bind<string>([](const string& element) { return "querySelector('" + element + "')"; })
    ->bind<int>([](int duration) { return to_string(duration) + "ms"; });

string result = func->process("animate('.btn', 300);", {".btn", "300"});
```

## 高级特性

### 1. 正确的双指针扫描机制

```cpp
// 前指针和后指针都从位置0开始
size_t frontPtr = 0;    // 前指针：寻找CJMOD语法
size_t backPtr = 0;     // 后指针：标记已处理位置

// 1. 扫描初始范围，确定不存在语法片段
// 2. 前指针移动到合适位置
// 3. 前后指针同步向前，前指针寻找关键字
// 4. 前指针检测到关键字时通知后指针收集
// 5. 能够收集到完整的代码片段
```

### 2. 前置截取机制

解决 `arg ** function` 语法问题：
- 统一扫描器扫描到 `**` 时才启动CJMOD收集
- 但第一个 `arg` 已经在统一扫描器的片段中
- 前置截取机制自动截取这部分，避免发送给编译器

### 3. 智能类型转换

```cpp
// 自动类型转换
arg.bind<int>([](int value) { /* 自动将string转换为int */ });
arg.bind<double>([](double value) { /* 自动将string转换为double */ });
arg.bind<bool>([](bool value) { /* 自动将string转换为bool */ });
```

## 实际应用示例

### 图像处理函数

```cpp
auto imageProcessor = createCHTLJSFunction("processImage", {"src", "filters", "output"});

imageProcessor
    ->bind<string>("src", [](const string& src) {
        return "ImageLoader.load(\"" + src + "\")";
    })
    ->bind<string>("filters", [](const string& filters) {
        return "FilterChain.create(" + filters + ")";
    })
    ->bind<string>("output", [](const string& output) {
        return "OutputManager.save(\"" + output + "\")";
    });

string sourceCode = R"(
    arg imageData ** processImage({
        src: "input.jpg",
        filters: ["blur", "sharpen"],
        output: "processed.jpg"
    });
)";

string result = imageProcessor->process(sourceCode);
```

### 动画控制函数

```cpp
auto animator = createCHTLJSFunction("animate");

animator
    ->bind<string>([](const string& element) {
        return "document.querySelector('" + element + "')";
    })
    ->bind<int>([](int duration) {
        return to_string(duration) + "ms";
    })
    ->bind<string>([](const string& easing) {
        return "ease-" + easing;
    });

string result = animator->process("animate('.button', 300, 'in-out');", 
                                {".button", "300", "in-out"});
```

### 数据获取函数

```cpp
auto fetcher = createCHTLJSFunction("fetchData", {"endpoint", "method", "headers"});

fetcher
    ->bind<string>("endpoint", [](const string& url) {
        return "API_BASE + \"" + url + "\"";
    })
    ->bind<string>("method", [](const string& method) {
        return "\"" + method + "\"";
    })
    ->bind<string>("headers", [](const string& headers) {
        return "{ ...DEFAULT_HEADERS, " + headers + " }";
    });

string result = fetcher->process("", {"/users", "GET", "Accept: 'application/json'"});
```

## 总结

### 核心优势

1. **简洁**：只有三个类，API设计直观
2. **强大**：支持复杂的语法处理和代码生成
3. **好用**：链式调用，类型安全，错误处理
4. **灵活**：既有标准流程又有简化流程
5. **正确**：实现了正确的双指针扫描和前置截取机制

### 设计哲学

- **表面简单，内在精妙**
- **用户接口极简，内部实现强大**
- **一套标准流程，一套简化流程**
- **使用简单，功能强大，很好用！**

这就是CJMOD API的完整设计 - 真正做到了简洁而强大，很好用！