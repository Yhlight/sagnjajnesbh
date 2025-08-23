# CMOD 源码目录

本目录存放CMOD类型的模块源码文件，包括：

## 📄 支持的文件类型

- **`.chtl`文件** - CHTL源码文件
- **`.cmod`文件** - CMOD模块文件

## 📁 目录结构示例

```
src/Modules/CMOD/
├── UIComponents.chtl      # CHTL组件库
├── ThemeSystem.cmod       # 主题系统模块
├── LayoutHelpers.chtl     # 布局助手
├── StyleTemplates.cmod    # 样式模板集
└── BaseComponents/        # 子模块示例
    ├── Button.chtl
    ├── Input.chtl
    └── Card.chtl
```

## 🏗️ CMOD子模块特性

**重要**：CMOD一旦拥有子模块，可以省略src文件夹中的主模块chtl文件（如文档第1389行所示）。

### 标准CMOD结构
```
ModuleName/
├── src/
│   └── ModuleName.chtl    # 主模块文件
└── info/
    └── 信息.chtl          # 必须的信息文件
```

### 带子模块的CMOD结构
```
ModuleName/
├── src/
│   ├── (ModuleName.chtl)  # 可选主模块文件（括号表示可省略）
│   ├── SubModule1/
│   │   └── SubModule1.chtl
│   └── SubModule2/
│       └── SubModule2.chtl
└── info/
    └── 信息.chtl          # 必须的信息文件
```

## 🎯 编译输出

这些源码文件会被编译到：
- `module/CMOD/` 目录（分类模式）
- 或 `module/` 根目录（混合模式）

## 📦 导入方式

```chtl
[Import] @Chtl from UIComponents        # 导入CMOD模块
[Import] @Chtl from ThemeSystem         # 导入CMOD模块
[Import] @Chtl from BaseComponents.*    # 导入子模块的所有文件
```