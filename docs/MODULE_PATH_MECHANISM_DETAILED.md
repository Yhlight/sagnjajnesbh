# 🗂️ CHTL模块路径机制详细说明

## 📋 概述

CHTL模块系统支持**三层路径结构**和**六种格式变体**，提供灵活且强大的模块管理机制。

---

## 🎯 三层路径结构

### 🏛️ 第一层：官方模块目录
**位置**: `<编译器二进制文件所在目录>/module/`

**特点**:
- 🏆 **最高优先级**
- 📦 **预编译模块**
- 🌐 **全局可用**
- 🔒 **只读访问**

**结构**:
```
/usr/local/bin/chtl          # 编译器二进制文件
/usr/local/bin/module/       # 官方模块目录
├── CMOD/                    # 大写格式CMOD模块
│   ├── Chtholly.cmod
│   ├── Yuigahama.cmod
│   └── ThirdParty.cmod
├── Cmod/                    # 首字母大写格式
│   └── Community.cmod
├── cmod/                    # 小写格式
│   └── simple.cmod
├── CJMOD/                   # 大写格式CJMOD扩展
│   ├── Chtholly.cjmod
│   └── Advanced.cjmod
├── CJmod/                   # 驼峰格式
│   └── Utils.cjmod
└── cjmod/                   # 小写格式
    └── basic.cjmod
```

### 👤 第二层：用户模块目录
**位置**: `<要编译的CHTL文件所在目录>/module/`

**特点**:
- 🎯 **中等优先级**
- 📁 **项目特定**
- 🔧 **可读写**
- 🏠 **本地模块**

**结构**:
```
/home/user/myproject/
├── main.chtl               # 用户主文件
├── components.chtl         # 用户组件文件
└── module/                 # 用户模块目录
    ├── CMOD/               # 用户的CMOD模块
    │   └── MyTheme.cmod
    ├── Cmod/
    ├── cmod/
    ├── CJMOD/              # 用户的CJMOD扩展
    │   └── MyExtension.cjmod
    ├── CJmod/
    └── cjmod/
```

### 🔧 源码模块目录 (仅用于开发分类)
**位置**: `<项目根目录>/src/Module/`

**特点**:
- 🛠️ **仅用于开发时分类**
- 📝 **不参与运行时搜索**
- 🔄 **模块开发和组织**
- 🧪 **打包前的源码管理**

**重要说明**: 此目录不会被模块搜索算法扫描，只用于开发者组织和分类模块源码。

**结构**:
```
<project-root>/src/Module/
├── CMOD/                   # 开发中的CMOD模块
│   ├── Chtholly/
│   │   ├── src/
│   │   │   ├── Chtholly.chtl
│   │   │   ├── ChthollyButton.chtl
│   │   │   └── Effects/    # 子模块必须在src内部
│   │   │       ├── src/
│   │   │       │   └── Effects.chtl
│   │   │       └── info/
│   │   │           └── Effects.chtl
│   │   └── info/
│   │       └── Chtholly.chtl
│   └── Yuigahama/
│       ├── src/
│       │   └── Yuigahama.chtl
│       └── info/
│           └── Yuigahama.chtl
├── Cmod/                   # 首字母大写格式
│   └── TestModule/
├── cmod/                   # 小写格式
│   └── AnotherTest/
├── CJMOD/                  # 开发中的CJMOD扩展
│   └── Chtholly/
│       ├── src/
│       │   ├── chtholly_extensions.cpp
│       │   └── simple_extensions.cpp
│       └── info/
│           └── Chtholly.chtl
├── CJmod/                  # 驼峰格式
│   └── ExtensionTest/
└── cjmod/                  # 小写格式
    └── SimpleExt/
```

---

## 🔍 搜索算法详解

### 📍 模块搜索流程

对于导入语句：`[Import] @Chtl from ModuleName`

#### 🏛️ 阶段1：官方模块搜索
```
1. <compiler-dir>/module/CMOD/ModuleName.cmod
2. <compiler-dir>/module/Cmod/ModuleName.cmod
3. <compiler-dir>/module/cmod/ModuleName.cmod
4. <compiler-dir>/module/CMOD/ModuleName/     # 未打包目录
5. <compiler-dir>/module/Cmod/ModuleName/
6. <compiler-dir>/module/cmod/ModuleName/
```

#### 👤 阶段2：用户模块搜索
```
7. <chtl-file-dir>/module/CMOD/ModuleName.cmod
8. <chtl-file-dir>/module/Cmod/ModuleName.cmod
9. <chtl-file-dir>/module/cmod/ModuleName.cmod
10. <chtl-file-dir>/module/CMOD/ModuleName/
11. <chtl-file-dir>/module/Cmod/ModuleName/
12. <chtl-file-dir>/module/cmod/ModuleName/
```

#### 🔧 阶段3：源码模块搜索
```
13. <project-root>/src/Module/CMOD/ModuleName/
14. <project-root>/src/Module/Cmod/ModuleName/
15. <project-root>/src/Module/cmod/ModuleName/
```

### ⚡ CJMOD搜索流程

对于导入语句：`[Import] @CJmod from ExtensionName`

#### 🏛️ 阶段1：官方扩展搜索
```
1. <compiler-dir>/module/CJMOD/ExtensionName.cjmod
2. <compiler-dir>/module/CJmod/ExtensionName.cjmod
3. <compiler-dir>/module/cjmod/ExtensionName.cjmod
```

#### 👤 阶段2：用户扩展搜索
```
4. <chtl-file-dir>/module/CJMOD/ExtensionName.cjmod
5. <chtl-file-dir>/module/CJmod/ExtensionName.cjmod
6. <chtl-file-dir>/module/cjmod/ExtensionName.cjmod
```

#### 🔧 阶段3：源码扩展搜索
```
7. <project-root>/src/Module/CJMOD/ExtensionName/
8. <project-root>/src/Module/CJmod/ExtensionName/
9. <project-root>/src/Module/cjmod/ExtensionName/
```

---

## 🎯 具体路径示例

### 📝 实际场景1：开发者环境

**环境设置**:
- 编译器位置: `/usr/local/bin/chtl`
- 项目位置: `/home/dev/myproject/`
- 主文件: `/home/dev/myproject/src/main.chtl`

**导入语句**: `[Import] @Chtl from Chtholly`

**搜索路径**:
```
1. /usr/local/bin/module/CMOD/Chtholly.cmod     ⭐ 官方预编译
2. /usr/local/bin/module/Cmod/Chtholly.cmod
3. /usr/local/bin/module/cmod/Chtholly.cmod
4. /home/dev/myproject/src/module/CMOD/Chtholly.cmod  💼 项目特定
5. /home/dev/myproject/src/module/Cmod/Chtholly.cmod
6. /home/dev/myproject/src/module/cmod/Chtholly.cmod
7. /home/dev/myproject/src/Module/CMOD/Chtholly/      🔧 开发源码
8. /home/dev/myproject/src/Module/Cmod/Chtholly/
9. /home/dev/myproject/src/Module/cmod/Chtholly/
```

### 📝 实际场景2：生产环境

**环境设置**:
- 编译器位置: `/opt/chtl/bin/chtl`
- 用户项目: `/var/www/website/`
- 主文件: `/var/www/website/index.chtl`

**导入语句**: `[Import] @CJmod from Analytics`

**搜索路径**:
```
1. /opt/chtl/bin/module/CJMOD/Analytics.cjmod   ⭐ 官方扩展
2. /opt/chtl/bin/module/CJmod/Analytics.cjmod
3. /opt/chtl/bin/module/cjmod/Analytics.cjmod
4. /var/www/website/module/CJMOD/Analytics.cjmod  💼 网站特定
5. /var/www/website/module/CJmod/Analytics.cjmod
6. /var/www/website/module/cjmod/Analytics.cjmod
```

---

## 🔄 模块解析优化

### ⚡ 缓存机制
- 📋 **模块索引缓存**: 避免重复扫描
- 🎯 **路径解析缓存**: 加速重复导入
- 🔄 **增量更新**: 只扫描变更目录

### 🎨 智能匹配
- 🔍 **模糊匹配**: 支持部分名称匹配
- 📏 **版本兼容**: 自动选择兼容版本
- 🏷️ **标签过滤**: 按类别、作者过滤

### 🚀 性能优化
- 🧵 **并行扫描**: 多线程目录扫描
- 💾 **内存映射**: 大文件快速读取
- 🗜️ **压缩缓存**: 减少内存占用

---

## 🛠️ 开发工具链

### 📦 打包工具
```bash
# 打包CMOD模块
chtl pack-cmod src/Module/CMOD/MyModule packages/MyModule.cmod

# 打包CJMOD扩展
chtl pack-cjmod src/Module/CJMOD/MyExtension packages/MyExtension.cjmod

# 批量打包所有格式
./scripts/packaging/pack_modules_enhanced.sh
```

### 🔧 构建工具
```bash
# 完整构建（包含ANTLR）
./scripts/build/build_complete_with_antlr.sh

# 仅构建核心
./scripts/build/build_core_only.sh

# 构建并打包
./scripts/build/build_and_package.sh
```

### 🧪 测试工具
```bash
# 模块系统测试
./build/test_module_classification

# 链式语法测试
./build/test_chain_syntax_and_modules

# 完整合规性测试
./build/bin/test_complete_chtl_compliance
```

---

## 🎉 总结

### ✅ **CHTL模块系统特点**

1. **🎯 三层路径结构**
   - 官方模块 (最高优先级)
   - 用户模块 (中等优先级)
   - 源码模块 (最低优先级)

2. **🎨 六种格式变体**
   - CMOD/Cmod/cmod (CHTL组件模块)
   - CJMOD/CJmod/cjmod (C++语法扩展)

3. **🔍 智能搜索算法**
   - 按优先级搜索
   - 格式变体兼容
   - 自动路径解析

4. **📦 完整工具链**
   - 打包/解包工具
   - 构建脚本
   - 测试验证

5. **🔗 ANTLR集成**
   - 原生CSS解析
   - 原生JavaScript解析
   - 跨平台支持

### ✅ **链式语法支持**

- **✅ except**: `except @Html, [Template], [Custom];`
- **✅ delete**: `delete .class1, .class2, #id1;`
- **✅ inherit**: `inherit Style1(base), Style2(hover);`
- **❌ insert**: 不支持链式，每次只能插入一个位置

这个设计确保了CHTL模块系统的**可扩展性**、**可维护性**和**易用性**！