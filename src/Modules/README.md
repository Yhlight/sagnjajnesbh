# Modules 源码目录结构说明

本目录用于存放模块文件的源码，支持两种组织模式：

## 🔄 混合模式（默认）

所有类型的模块源码文件放在一起：

```
src/Modules/
├── ModuleName1.chtl
├── ModuleName2.cmod
├── ModuleName3.cjmod
├── Component.chtl
├── Utils.cmod
└── Extensions.cjmod
```

## 📁 分类模式

按模块类型分类存放源码文件：

```
src/Modules/
├── CMOD/          # 或者 Cmod/ 或 cmod/
│   ├── ModuleName1.chtl
│   ├── ModuleName2.cmod
│   └── Component.chtl
└── CJMOD/         # 或者 CJmod/ 或 cjmod/
    ├── Extensions.cjmod
    └── Plugins.cjmod
```

## 🎯 支持的分类目录名

- **CMOD类**：`CMOD/` 或 `Cmod/` 或 `cmod/`
- **CJMOD类**：`CJMOD/` 或 `CJmod/` 或 `cjmod/`

## 📦 编译输出

无论使用哪种源码组织模式，编译器都会生成对应的module文件夹，也支持两种结构：

### 编译生成的module文件夹（混合模式）
```
module/
├── ModuleName1.chtl
├── ModuleName2.cmod
└── Extensions.cjmod
```

### 编译生成的module文件夹（分类模式，默认）
```
module/
├── CMOD/
│   ├── ModuleName1.chtl
│   └── ModuleName2.cmod
└── CJMOD/
    └── Extensions.cjmod
```

## 🔍 Import路径搜索

**重要澄清**：本`src/Modules/`目录是**源码存放目录**，`[Import]`系统**不会**搜索此目录。

`[Import]`系统的实际搜索顺序：
1. **官方模块目录**（编译器同一文件夹下的module目录）
2. **当前目录module文件夹** 
3. **当前目录**

## 📦 编译流程

源码文件从`src/Modules/`编译后生成到`module/`文件夹：

```
src/Modules/          →编译→    module/
├── CMOD/                      ├── CMOD/
│   └── *.chtl/*.cmod          │   └── *.chtl/*.cmod
└── CJMOD/                     └── CJMOD/
    └── *.cjmod                    └── *.cjmod
```

## 📝 注意事项

- **src/Modules/**：源码存放，开发时编辑
- **module/**：编译输出，Import时搜索
- 分类模式是默认推荐的组织方式
- `module/`文件夹现在支持分类结构（`CMOD/Cmod/cmod`和`CJMOD/CJmod/cjmod`）
- 两种模式可以混用，编译器会智能识别