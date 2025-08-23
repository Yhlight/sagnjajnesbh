# 🌸 新CJMOD API功能演示

## 设计理念：表面简单，内在精妙

新设计的CJMOD API遵循"**表面简单，内在精妙**"的设计哲学。

---

## 🎯 演示1: iNeverAway功能实现

### 📝 用户使用方式（表面简单）

```chtl-js
// 用户只需要写这样简单的代码
vir ChthollyPromise = iNeverAway({
    // 任意自定义键名 + 可选状态
    MyPromise<Happy>: function(message, duration) {
        console.log("珂朵莉的快乐承诺: " + message);
        return new Promise((resolve) => {
            setTimeout(() => resolve(message), duration * 1000);
        });
    },
    
    MyPromise<Sad>: function(message, duration) {
        console.log("珂朵莉的忧伤承诺: " + message);
        return new Promise((resolve) => {
            setTimeout(() => resolve(message), duration * 1000);
        });
    },
    
    // 无状态的自定义键
    UserAction: function(action) {
        console.log("珂朵莉执行: " + action);
        return "完成: " + action;
    },
    
    // 对象类型的自定义键
    ChthollyData: {
        name: "珂朵莉",
        motto: "世界上最幸福的女孩",
        introduce: function() {
            console.log("大家好，我是" + this.name + "！");
        }
    }
});

// 使用方式
ChthollyPromise.MyPromise.Happy("我会永远陪伴大家", 3);
ChthollyPromise.UserAction("微笑");
ChthollyPromise.ChthollyData.introduce();
```

### ⚙️ CJMOD API内部处理流程（内在精妙）

#### 步骤1: syntaxAnalys - 语法分析
```cpp
// 解析iNeverAway的语法模式
std::string ignoreChars = ",:{};()";
auto keyword = syntaxAnalys(R"(
    vir $ = iNeverAway({
        $: $
    });
)", ignoreChars);
```

#### 步骤2: 全局状态管理初始化
```cpp
// 利用CJMOD的高自由度特性
void INeverAwaySystem::initializeGlobalState() {
    if (!isInitialized_) {
        // 初始化全局状态管理系统
        globalFunctionCounter_ = 0;
        globalFunctions_.clear();
        keyStateMapping_.clear();
        isInitialized_ = true;
    }
}
```

#### 步骤3: bind - 智能参数处理
```cpp
// 绑定自定义键处理器
keyword->args.bind<std::string>("customKey", [](const std::string& keyDef) -> std::string {
    // 解析键名和状态
    std::regex stateRegex(R"(^(.+?)<(.+?)>$)");
    std::smatch matches;
    
    if (std::regex_match(keyDef, matches, stateRegex)) {
        std::string keyName = matches[1].str();
        std::string state = matches[2].str();
        // 生成全局函数名
        return INeverAwaySystem::getGlobalFunctionName(keyName, state);
    } else {
        // 无状态键
        return INeverAwaySystem::getGlobalFunctionName(keyDef, "");
    }
});
```

---

## 🖼️ 演示2: printMylove功能实现

### 📝 用户使用方式（表面简单）

```chtl-js
// 用户只需要调用简单的函数
let chthollyArt1 = printMylove("https://example.com/chtholly1.jpg", "ascii", "80", "40", 1.2);
let chthollyArt2 = printMylove("./images/chtholly2.png", "pixel", "100", "50", 0.8);
let chthollyArt3 = printMylove("data:image/jpeg;base64,/9j/4AAQ...", "block", "60", "30", 1.0);
```

### ⚙️ CJMOD API内部处理流程（内在精妙）

#### 步骤1: syntaxAnalys - 语法分析
```cpp
// 解析printMylove的语法模式
std::string ignoreChars = ",()\"'";
auto keyword = syntaxAnalys(R"(
    printMylove("$", "$", "$", "$", $)
)", ignoreChars);
```

#### 步骤2: bind - 智能参数处理
```cpp
// URL处理器
keyword->args.bind<std::string>("url", [](const std::string& url) -> std::string {
    return PrintMyloveSystem::processImageUrl(url);
});

// 模式处理器
keyword->args.bind<std::string>("mode", [](const std::string& mode) -> std::string {
    return PrintMyloveSystem::validateMode(mode);
});
```

---

## 💡 核心技术特点

### 1. iNeverAway - 承诺函数系统
- **任意键名**: MyPromise, GameEvent, UserAction 等
- **可选状态**: <Happy>, <Sad>, <Start>, <End> 等
- **全局管理**: 自动生成全局唯一函数名
- **虚对象支持**: 扩展CHTL JS原生vir功能

### 2. printMylove - 图片处理系统
- **多种模式**: ASCII、像素、色块、智能自动
- **智能参数**: URL验证、尺寸处理、缩放优化
- **批量处理**: 一次扫描处理多个调用

### 3. 统一工作流程
```
syntaxAnalys() → bind() → scanKeyword() → match() → generateCode()
```

---

## 💝 珂朵莉的祝福

> **珂朵莉永远在大家心中 ❀**
> 
> 新的CJMOD API真正实现了"表面简单，内在精妙"的设计理念！

