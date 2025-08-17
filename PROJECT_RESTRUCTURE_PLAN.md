# CHTL项目大重构计划

## 🎯 重构目标

基于目标规划.ini的要求，使用大驼峰命名法重新组织项目目录结构，实现模块化编程。每个编译器拥有自己的完整组件，不再共用文件。

## 📋 新目录结构设计

```
src/
├── CHTLCompiler/           # CHTL编译器（大驼峰命名）
│   ├── Lexer/
│   │   ├── CHTLLexer.h
│   │   ├── CHTLLexer.cpp
│   │   ├── CHTLToken.h
│   │   └── CHTLToken.cpp
│   ├── Parser/
│   │   ├── CHTLParser.h
│   │   ├── CHTLParser.cpp
│   │   ├── CHTLContext.h
│   │   └── CHTLContext.cpp
│   ├── AST/
│   │   ├── CHTLNodes.h
│   │   ├── CHTLNodes.cpp
│   │   └── CHTLASTManager.h
│   ├── State/
│   │   ├── CHTLState.h
│   │   ├── CHTLState.cpp
│   │   ├── CHTLGlobalMap.h
│   │   └── CHTLGlobalMap.cpp
│   ├── Generator/
│   │   ├── CHTLGenerator.h
│   │   └── CHTLGenerator.cpp
│   ├── Module/             # CMOD系统
│   │   ├── CmodManager.h
│   │   ├── CmodManager.cpp
│   │   └── SimpleZip.h (共享)
│   └── Core/
│       ├── CHTLCompilerCore.h
│       └── CHTLCompilerCore.cpp
│
├── CHTLJSCompiler/         # CHTL JS编译器（大驼峰命名）
│   ├── Lexer/
│   │   ├── CHTLJSLexer.h
│   │   ├── CHTLJSLexer.cpp
│   │   ├── CHTLJSToken.h
│   │   └── CHTLJSToken.cpp
│   ├── Parser/
│   │   ├── CHTLJSParser.h
│   │   ├── CHTLJSParser.cpp
│   │   ├── CHTLJSContext.h
│   │   └── CHTLJSContext.cpp
│   ├── AST/
│   │   ├── CHTLJSNodes.h
│   │   ├── CHTLJSNodes.cpp
│   │   ├── VirtualObjectManager.h
│   │   └── VirtualObjectManager.cpp
│   ├── State/
│   │   ├── CHTLJSState.h
│   │   ├── CHTLJSState.cpp
│   │   ├── CHTLJSGlobalMap.h
│   │   └── CHTLJSGlobalMap.cpp
│   ├── Generator/
│   │   ├── CHTLJSGenerator.h
│   │   └── CHTLJSGenerator.cpp
│   ├── Module/             # CJMOD系统
│   │   ├── CJmodManager.h
│   │   ├── CJmodManager.cpp
│   │   └── SimpleZip.h (共享)
│   └── Core/
│       ├── CHTLJSCompilerCore.h
│       └── CHTLJSCompilerCore.cpp
│
├── CSSCompiler/            # CSS编译器（ANTLR）
│   ├── Grammar/
│   │   └── CSS.g4
│   ├── Generated/          # ANTLR生成的文件
│   ├── Core/
│   │   ├── CSSCompilerCore.h
│   │   └── CSSCompilerCore.cpp
│   └── Utils/
│       ├── CSSUtils.h
│       └── CSSUtils.cpp
│
├── JavaScriptCompiler/     # JavaScript编译器（ANTLR）
│   ├── Grammar/
│   │   └── JavaScript.g4
│   ├── Generated/          # ANTLR生成的文件
│   ├── Core/
│   │   ├── JavaScriptCompilerCore.h
│   │   └── JavaScriptCompilerCore.cpp
│   └── Utils/
│       ├── JavaScriptUtils.h
│       └── JavaScriptUtils.cpp
│
├── UnifiedCore/            # 统一核心组件
│   ├── Scanner/
│   │   ├── CHTLUnifiedScanner.h
│   │   └── CHTLUnifiedScanner.cpp
│   ├── Dispatcher/
│   │   ├── CompilerDispatcher.h
│   │   └── CompilerDispatcher.cpp
│   ├── Constraints/
│   │   ├── SyntaxConstrainer.h
│   │   └── SyntaxConstrainer.cpp
│   └── Import/
│       ├── ImportManager.h
│       └── ImportManager.cpp
│
├── Module/                 # 官方模块源码
│   ├── Standard/           # 标准库模块
│   ├── UI/                 # UI组件模块
│   └── Utils/              # 工具模块
│
└── Shared/                 # 共享工具
    ├── Utils/
    │   ├── FileUtils.h
    │   ├── FileUtils.cpp
    │   ├── StringUtils.h
    │   └── StringUtils.cpp
    ├── SimpleZip/
    │   ├── SimpleZip.h
    │   └── SimpleZip.cpp
    └── Common/
        ├── Types.h
        └── Constants.h
```

## 📊 文件迁移计划

### 1. CHTL编译器迁移
- `common/Lexer.*` → `CHTLCompiler/Lexer/CHTLLexer.*`
- `common/Token.*` → `CHTLCompiler/Lexer/CHTLToken.*`
- `common/Context.*` → `CHTLCompiler/Parser/CHTLContext.*`
- `common/State.*` → `CHTLCompiler/State/CHTLState.*`
- `common/GlobalMap.*` → `CHTLCompiler/State/CHTLGlobalMap.*`
- `parser/Parser.*` → `CHTLCompiler/Parser/CHTLParser.*`
- `generator/HTMLGenerator.*` → `CHTLCompiler/Generator/CHTLGenerator.*`
- `ast/CHTLNodes.h` → `CHTLCompiler/AST/CHTLNodes.*`
- `common/CmodManager.*` → `CHTLCompiler/Module/CmodManager.*`

### 2. CHTL JS编译器迁移
- 创建专用的CHTL JS组件（从CHTL组件衍生）
- `ast/VirtualObjectManager.*` → `CHTLJSCompiler/AST/VirtualObjectManager.*`
- `common/CJmodManager.*` → `CHTLJSCompiler/Module/CJmodManager.*`

### 3. 统一核心迁移
- `common/CHTLUnifiedScanner.*` → `UnifiedCore/Scanner/`
- `common/CompilerDispatcher.*` → `UnifiedCore/Dispatcher/`
- `common/SyntaxConstrainer.*` → `UnifiedCore/Constraints/`
- `common/ImportManager.*` → `UnifiedCore/Import/`

### 4. 共享组件迁移
- `common/SimpleZip.*` → `Shared/SimpleZip/`

## 🔧 重构步骤

1. **创建新目录结构**
2. **迁移CHTL编译器文件**
3. **创建CHTL JS编译器文件**
4. **迁移统一核心文件**
5. **创建CSS和JavaScript编译器框架**
6. **迁移共享组件**
7. **创建Module系统**
8. **更新CMakeLists.txt**
9. **测试编译**

## ⚠️ 注意事项

1. **每个编译器独立** - 不共用文件，各自维护完整组件
2. **大驼峰命名** - 所有目录和主要文件使用大驼峰命名法
3. **模块化设计** - 每个编译器内部也要模块化组织
4. **保持功能完整** - 重构后必须能正常编译和运行
5. **CMOD/CJMOD位置** - 放在各自编译器的Module文件夹中