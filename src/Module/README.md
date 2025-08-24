# CHTL模块源码目录

## 概述

**模块源码目录结构说明**

```
src/Module/
├── CMOD/                    # CMOD扩展源码
│   ├── UIComponents.chtl    # UI组件库源码
│   └── README.md           # CMOD说明文档
├── CJMOD/                   # CJMOD扩展源码  
│   ├── ReactSupport.cjmod   # React支持库源码
│   └── README.md           # CJMOD说明文档
├── Chtholly/               # 珂朵莉模块源码
│   ├── info/               # 模块信息文件
│   ├── src/                # 模块源码文件
│   ├── CJMOD/              # 珂朵莉CJMOD扩展源码
│   └── CMOD/               # 珂朵莉CMOD扩展源码
└── Yuigahama/              # 由比滨模块源码
    ├── info/               # 模块信息文件
    └── src/                # 模块源码文件
```

## 目录结构说明

### 支持的结构类型

1. **无序结构**：chtl文件、cmod、cjmod源码文件夹全部放一起
2. **分类结构**：
   - `cmod/` 或 `Cmod/` 或 `CMOD/` 目录
   - `cjmod/` 或 `CJmod/` 或 `CJMOD/` 目录

### 模块类型

- **CHTL模块**：包含`.chtl`文件的模块
- **CMOD扩展**：组件模块，提供可重用组件
- **CJMOD扩展**：功能模块，提供JavaScript扩展

## 文件类型

- `.chtl` - CHTL源码文件
- `.cmod` - CMOD扩展文件（编译后）
- `.cjmod` - CJMOD扩展文件（编译后）
- `.h/.cpp` - CJMOD扩展的C++源码

## 编译流程

**重要澄清**：本`src/Module/`目录是**源码存放目录**，`[Import]`系统**不会**搜索此目录。

### 编译输出

源码文件从`src/Module/`编译后生成到`module/`文件夹：

```
src/Module/          →编译→    module/
├── CMOD/                     ├── CMOD/
└── CJMOD/                    └── CJMOD/
```

### 目录职责

- **src/Module/**：源码存放，开发时编辑
- **module/**：编译输出，运行时加载

## 开发指南

1. 在`src/Module/`下编写源码
2. 使用构建系统编译到`module/`目录
3. 在CHTL代码中使用`[Import]`导入模块