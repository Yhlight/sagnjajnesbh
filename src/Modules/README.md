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

Import系统会自动识别这两种结构并进行相应的路径搜索：

1. **官方模块目录**（编译器同一文件夹下的module目录）
2. **当前目录module文件夹**
3. **当前目录**
4. **源码目录Modules**（新增支持）

## 📝 注意事项

- 分类模式是默认推荐的组织方式
- 源码目录Modules现在支持分类结构（之前不支持）
- 两种模式可以混用，编译器会智能识别