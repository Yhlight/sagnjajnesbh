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