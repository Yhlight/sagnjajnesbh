# CJMOD 源码目录

本目录存放CJMOD类型的模块源码文件，用于扩展CHTL JS语法。

## 📄 支持的文件类型

- **`.cjmod`文件** - CJMOD扩展模块文件

## 📁 目录结构示例

```
src/Module/CJMOD/
├── ReactSupport.cjmod        # React语法扩展
├── VueSupport.cjmod          # Vue语法扩展
├── AnimationExtensions.cjmod # 动画扩展
├── EventExtensions.cjmod     # 事件处理扩展
└── CustomPlugins/            # 自定义插件子模块
    ├── FormValidation.cjmod
    ├── DataBinding.cjmod
    └── StateManagement.cjmod
```

## 🎯 编译输出

这些源码文件会被编译到：
- `module/CJMOD/` 目录（分类模式）
- 或 `module/` 根目录（混合模式）

## 📦 导入方式

```chtl
[Import] @CJmod from ReactSupport       # 导入CJMOD扩展
[Import] @CJmod from VueSupport         # 导入CJMOD扩展
[Import] @CJmod from CustomPlugins.*    # 导入子模块的所有cjmod文件
```

## ⚡ CJMOD特性

CJMOD允许：
- 扩展CHTL JS语法
- 创建自定义JS语法糖
- 通过C++接口实现复杂功能
- 无限扩展CHTL JS能力