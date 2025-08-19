# 🗂️ CHTL模块系统完整机制说明

## 📋 目录
- [模块类型](#模块类型)
- [目录结构](#目录结构)
- [路径解析机制](#路径解析机制)
- [模块搜索顺序](#模块搜索顺序)
- [模块格式变体](#模块格式变体)
- [打包和分发](#打包和分发)
- [导入机制](#导入机制)
- [实际示例](#实际示例)

---

## 🎯 模块类型

CHTL支持两种主要模块类型：

### 📦 CMOD (CHTL Module)
- **用途**: CHTL组件和模板模块
- **内容**: `.chtl`文件，包含模板、自定义、样式组、变量组等
- **文件格式**: `.cmod` (打包后)
- **开发格式**: 文件夹结构

### ⚡ CJMOD (CHTL JavaScript Module)  
- **用途**: C++编写的CHTL JS语法扩展
- **内容**: `.cpp`和`.h`文件，扩展CHTL JS语法
- **文件格式**: `.cjmod` (打包后)
- **开发格式**: 文件夹结构

---

## 📁 目录结构

### 🏗️ 项目目录结构
```
project-root/
├── src/                           # 源码目录
│   └── Module/                    # 开发中的模块
│       ├── CMOD/                  # CMOD模块分类 (大写)
│       │   ├── Chtholly/
│       │   └── Yuigahama/
│       ├── Cmod/                  # CMOD模块分类 (首字母大写)
│       │   └── TestModule/
│       ├── cmod/                  # CMOD模块分类 (小写)
│       │   └── AnotherTest/
│       ├── CJMOD/                 # CJMOD模块分类 (大写)
│       │   └── Chtholly/
│       ├── CJmod/                 # CJMOD模块分类 (首字母大写)
│       │   └── ExtensionTest/
│       └── cjmod/                 # CJMOD模块分类 (小写)
│           └── SimpleExt/
├── module/                        # 官方模块目录 (编译器二进制所在目录)
│   ├── CMOD/                      # 官方CMOD模块
│   │   ├── Chtholly.cmod
│   │   └── Yuigahama.cmod
│   ├── Cmod/                      # 官方Cmod模块
│   ├── cmod/                      # 官方cmod模块
│   ├── CJMOD/                     # 官方CJMOD模块
│   │   └── Chtholly.cjmod
│   ├── CJmod/                     # 官方CJmod模块
│   └── cjmod/                     # 官方cjmod模块
└── user-project/                  # 用户项目目录
    ├── main.chtl                  # 用户主文件
    └── module/                    # 用户模块目录 (用户文件所在目录)
        ├── CMOD/                  # 用户CMOD模块
        ├── Cmod/
        ├── cmod/
        ├── CJMOD/                 # 用户CJMOD模块
        ├── CJmod/
        └── cjmod/
```

### 🏛️ CMOD模块内部结构
```
ModuleName/                        # 模块根目录
├── src/                           # 源码目录
│   ├── ModuleName.chtl           # 主模块文件 (可选)
│   ├── Component1.chtl           # 组件文件
│   ├── Component2.chtl           # 组件文件
│   └── SubModule/                # 子模块 (必须在src内部)
│       ├── src/
│       │   └── SubModule.chtl
│       └── info/
│           └── SubModule.chtl
└── info/                         # 模块信息目录
    └── ModuleName.chtl           # 模块信息文件
```

**三同名规则 (CMOD)**:
- 模块文件夹名: `ModuleName`
- 主模块文件名: `ModuleName.chtl`
- 模块信息文件名: `ModuleName.chtl`

### ⚡ CJMOD模块内部结构
```
ExtensionName/                     # 扩展根目录
├── src/                           # C++源码目录
│   ├── extension_main.cpp        # 主扩展文件
│   ├── extension_utils.cpp       # 工具文件
│   └── extension_utils.h         # 头文件
└── info/                         # 扩展信息目录
    └── ExtensionName.chtl        # 扩展信息文件
```

**二同名规则 (CJMOD)**:
- 扩展文件夹名: `ExtensionName`
- 扩展信息文件名: `ExtensionName.chtl`

---

## 🔍 路径解析机制

### 📍 搜索路径优先级

1. **官方模块目录** (编译器二进制文件所在目录)
   ```
   <compiler-binary-dir>/module/
   ```

2. **用户模块目录** (要编译的CHTL文件所在目录)
   ```
   <chtl-file-dir>/module/
   ```

3. **源码模块目录** (开发时)
   ```
   <project-root>/src/Module/
   ```

### 🔄 模块搜索顺序

对于导入语句 `[Import] @Chtl from ModuleName`：

1. **官方模块搜索**:
   ```
   <compiler-dir>/module/CMOD/ModuleName.cmod
   <compiler-dir>/module/Cmod/ModuleName.cmod
   <compiler-dir>/module/cmod/ModuleName.cmod
   ```

2. **用户模块搜索**:
   ```
   <chtl-file-dir>/module/CMOD/ModuleName.cmod
   <chtl-file-dir>/module/Cmod/ModuleName.cmod
   <chtl-file-dir>/module/cmod/ModuleName.cmod
   ```

3. **源码模块搜索**:
   ```
   <project-root>/src/Module/CMOD/ModuleName/
   <project-root>/src/Module/Cmod/ModuleName/
   <project-root>/src/Module/cmod/ModuleName/
   ```

### 🎯 具体路径示例

假设编译器在 `/usr/local/bin/chtl`，用户文件在 `/home/user/project/main.chtl`：

```
导入: [Import] @Chtl from Chtholly

搜索顺序:
1. /usr/local/bin/module/CMOD/Chtholly.cmod
2. /usr/local/bin/module/Cmod/Chtholly.cmod  
3. /usr/local/bin/module/cmod/Chtholly.cmod
4. /home/user/project/module/CMOD/Chtholly.cmod
5. /home/user/project/module/Cmod/Chtholly.cmod
6. /home/user/project/module/cmod/Chtholly.cmod
7. <project-root>/src/Module/CMOD/Chtholly/
8. <project-root>/src/Module/Cmod/Chtholly/
9. <project-root>/src/Module/cmod/Chtholly/
```

---

## 🎨 模块格式变体

### 📦 CMOD格式变体支持
- **CMOD** (全大写) - 官方推荐格式
- **Cmod** (首字母大写) - 友好格式  
- **cmod** (全小写) - 简洁格式

### ⚡ CJMOD格式变体支持
- **CJMOD** (全大写) - 官方推荐格式
- **CJmod** (驼峰格式) - 友好格式
- **cjmod** (全小写) - 简洁格式

### 🔄 格式兼容性

所有格式变体完全兼容，编译器会按优先级搜索：
1. 大写格式 (CMOD/CJMOD)
2. 首字母大写格式 (Cmod/CJmod)  
3. 小写格式 (cmod/cjmod)

---

## 📦 打包和分发

### 🏗️ CMOD打包过程

1. **验证模块结构**
   ```bash
   chtl pack-cmod src/Module/CMOD/Chtholly/ packages/Chtholly.cmod
   ```

2. **三同名规则验证**
   - 文件夹名 = 主文件名 = info文件名

3. **内容打包**
   - 所有 `.chtl` 文件
   - 子模块结构
   - 模块信息

4. **生成 `.cmod` 文件**

### ⚡ CJMOD打包过程

1. **C++源码编译**
   ```bash
   g++ -std=c++17 -fPIC -shared src/*.cpp -o libExtension.so
   ```

2. **验证模块结构**
   ```bash
   chtl pack-cjmod src/Module/CJMOD/Extension/ packages/Extension.cjmod
   ```

3. **内容打包**
   - 编译后的动态库 (`.so`/`.dll`)
   - 模块信息文件
   - 必要的头文件

4. **生成 `.cjmod` 文件**

---

## 📥 导入机制

### 🎯 基本导入语法

```chtl
// 导入CMOD模块
[Import] @Chtl from ModuleName

// 导入CJMOD扩展
[Import] @CJmod from ExtensionName

// 导入特定组件
[Import] [Custom] @Element ComponentName from ModuleName

// 导入所有模板
[Import] [Template] from ModuleName

// 通配符导入
[Import] @Chtl from ModuleName.*
[Import] @Chtl from path/*.cmod
```

### 🔄 路径解析规则

1. **名称导入** (不带后缀)
   ```chtl
   [Import] @Chtl from Chtholly
   ```
   - 搜索 `Chtholly.cmod` 文件
   - 按格式变体优先级搜索

2. **具体文件导入** (带后缀)
   ```chtl
   [Import] @Chtl from Chtholly.cmod
   ```
   - 直接搜索指定文件

3. **路径导入** (含路径信息)
   ```chtl
   [Import] @Chtl from themes/Chtholly
   ```
   - 在指定路径下搜索

4. **通配符导入**
   ```chtl
   [Import] @Chtl from themes/*
   ```
   - 导入目录下所有模块

---

## 🏷️ 命名空间和作用域

### 🎯 模块命名空间

每个模块自动创建命名空间：
```chtl
// 导入后自动创建命名空间
[Import] @Chtl from Chtholly

// 使用命名空间访问
Chtholly::ChthollyButton
```

### 🔄 命名冲突解决

1. **自动命名空间**
   ```chtl
   [Import] @Chtl from ModuleA
   [Import] @Chtl from ModuleB
   
   // 使用时指定命名空间
   ModuleA::Component
   ModuleB::Component
   ```

2. **别名导入**
   ```chtl
   [Import] @Chtl from Chtholly as ChthollyTheme
   
   // 使用别名
   ChthollyTheme::Button
   ```

---

## 🛠️ 实际示例

### 📦 完整的CMOD模块示例

**目录结构:**
```
src/Module/CMOD/MyTheme/
├── src/
│   ├── MyTheme.chtl
│   ├── Button.chtl
│   ├── Card.chtl
│   └── SubTheme/
│       ├── src/
│       │   └── SubTheme.chtl
│       └── info/
│           └── SubTheme.chtl
└── info/
    └── MyTheme.chtl
```

**info/MyTheme.chtl:**
```chtl
[Info] {
    name = "MyTheme";
    version = "1.0.0";
    description = "我的主题模块";
    author = "CHTL Team";
    license = "MIT";
    category = "theme";
}

[Export] {
    [Custom] @Element MyButton, MyCard;
    [Custom] @Style MyThemeColors;
    [Template] @Element BaseComponent;
}
```

**src/MyTheme.chtl:**
```chtl
[Custom] @Var MyThemeColors {
    primary: #3b82f6;
    secondary: #64748b;
    accent: #f59e0b;
}

[Template] @Element BaseComponent {
    div {
        style {
            padding: 16px;
            border-radius: 8px;
            color: MyThemeColors(primary);
        }
    }
}
```

### ⚡ 完整的CJMOD扩展示例

**目录结构:**
```
src/Module/CJMOD/MyExtension/
├── src/
│   ├── my_extension.cpp
│   └── my_extension.h
└── info/
    └── MyExtension.chtl
```

**src/my_extension.cpp:**
```cpp
#include "../../../../../../src/CHTLJSCompiler/Module/YourCJMODDesign.h"

using namespace your_cjmod_design;

void registerMyExtension() {
    // 定义新语法
    auto keyword = syntaxAnalys("myFunction( arg )");
    
    // 绑定参数处理
    keyword.args.bind("arg", [](const std::string& value) {
        return "console.log('MyExtension: " + value + "');";
    });
}
```

**info/MyExtension.chtl:**
```chtl
[Info] {
    name = "MyExtension";
    version = "1.0.0";
    description = "我的CHTL JS扩展";
    author = "CHTL Team";
    license = "MIT";
    category = "extension";
}

[Export] {
    [Function] myFunction;
}
```

### 🎯 使用示例

**用户项目中使用:**
```chtl
// 导入模块
[Import] @Chtl from MyTheme
[Import] @CJmod from MyExtension

html {
    body {
        // 使用CMOD组件
        MyTheme::BaseComponent {
            style {
                inherit MyTheme::MyThemeColors(primary);
            }
        }
        
        script {
            // 使用CJMOD扩展
            myFunction("Hello World");
        }
    }
}
```

---

## 🎉 总结

CHTL模块系统提供了：

### ✅ **完整的模块管理**
- 📦 CMOD (CHTL组件模块)
- ⚡ CJMOD (C++语法扩展)
- 🎯 多格式变体支持 (CMOD/Cmod/cmod)

### ✅ **智能路径解析**
- 🏛️ 官方模块优先
- 👤 用户模块次之  
- 🔧 源码模块最后
- 🔍 自动格式变体搜索

### ✅ **灵活的导入机制**
- 🎯 名称导入、路径导入、通配符导入
- 🏷️ 自动命名空间管理
- 🔄 命名冲突解决

### ✅ **严格的规范**
- 📏 三同名规则 (CMOD)
- 📏 二同名规则 (CJMOD)  
- 📦 标准化打包格式
- 🔧 完整的开发工具链

这个模块系统确保了CHTL项目的可维护性、可扩展性和团队协作的便利性！