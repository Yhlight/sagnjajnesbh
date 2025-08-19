# ANTLR 4集成指南

## 概述

CHTL选择集成ANTLR 4来提供原生的CSS和JavaScript解析能力。本指南详细介绍ANTLR 4在CHTL中的作用、集成方式和构建过程。

---

## 为什么选择ANTLR 4

### 技术优势

1. **成熟稳定**: ANTLR 4是业界标准的解析器生成工具
2. **性能优秀**: 基于ALL(*)算法，解析性能优异
3. **语法完整**: 提供官方维护的CSS3和JavaScript语法
4. **跨平台**: 支持多种编程语言和平台
5. **社区支持**: 活跃的开源社区和丰富的资源

### CHTL的需求

CHTL需要解析嵌入的CSS和JavaScript代码：

```chtl
style {
    /* 需要解析CSS语法 */
    .button {
        background: linear-gradient(45deg, #ff6b6b, #4ecdc4);
        transform: translateX(calc(100% - 20px));
    }
}

script {
    /* 需要解析JavaScript语法 */
    const data = await fetch('/api/data');
    const result = data.map(item => ({ ...item, processed: true }));
}
```

### 替代方案对比

| 方案 | 优势 | 劣势 | 选择理由 |
|------|------|------|----------|
| 自研解析器 | 完全控制 | 开发成本高，兼容性差 | ❌ 重复造轮子 |
| 正则表达式 | 简单快速 | 无法处理复杂语法 | ❌ 功能受限 |
| Lex/Yacc | 传统工具 | 语法定义复杂 | ❌ 学习成本高 |
| **ANTLR 4** | **成熟、标准、高性能** | **依赖Java运行时** | ✅ **最佳选择** |

---

## ANTLR 4在CHTL中的应用

### 使用的ANTLR功能

#### 1. CSS3语法解析

**使用的语法文件**: `css3Lexer.g4`, `css3Parser.g4`

```antlr
// css3Lexer.g4 (部分)
grammar css3Lexer;

// CSS标识符
Ident: IdentifierStart IdentifierPart*;
IdentifierStart: [a-zA-Z] | '_' | '-' | NonAscii | Escape;

// CSS数值
Number: [0-9]+ ('.' [0-9]+)?;
Percentage: Number '%';
Dimension: Number Ident;

// CSS字符串
String: '"' (~["\n\r\f\\] | '\\' .)* '"'
      | '\'' (~['\n\r\f\\] | '\\' .)* '\'';
```

**生成的C++文件**:
- `css3Lexer.h/cpp`: CSS词法分析器
- `css3Parser.h/cpp`: CSS语法分析器
- `css3ParserListener.h/cpp`: 解析事件监听器

#### 2. JavaScript语法解析

**使用的语法文件**: `JavaScriptLexer.g4`, `JavaScriptParser.g4`

```antlr
// JavaScriptParser.g4 (部分)
grammar JavaScriptParser;

program: sourceElements? EOF;

sourceElements: sourceElement+;

sourceElement: statement | functionDeclaration | classDeclaration;

statement: block | variableStatement | expressionStatement | ifStatement
         | iterationStatement | returnStatement | tryStatement;

expressionStatement: expression ';';
```

**生成的C++文件**:
- `JavaScriptLexer_cpp.h/cpp`: JS词法分析器
- `JavaScriptParser_cpp.h/cpp`: JS语法分析器
- `JavaScriptParser_cppListener.h/cpp`: 解析事件监听器

#### 3. 自定义语法扩展

CHTL扩展了标准语法以支持CHTL特有功能：

```antlr
// CHTL扩展的CSS语法
cssRule: selector '{' chtlEnhancedProperties '}';

chtlEnhancedProperties: 
    cssProperty* 
    inheritStatement* 
    deleteStatement*;

inheritStatement: 'inherit' '@Style' Identifier ';';
deleteStatement: 'delete' propertyList ';';
```

---

## ANTLR集成架构

### 集成层次结构

```
CHTL编译器
    ↓
CSSCompiler / JavaScriptCompiler
    ↓
ANTLR生成的解析器
    ↓
原生CSS/JS语法解析
```

### 文件组织

```
external/
├── antlr-4.13.2-complete.jar          # ANTLR工具JAR
├── antlr4/                             # ANTLR 4源码
├── antlr4-cross-platform/              # 跨平台集成
│   ├── precompiled/                    # 预编译库
│   │   ├── linux-x64/
│   │   ├── macos-x64/
│   │   └── windows-x64/
│   └── scripts/                        # 构建脚本
└── grammars-v4/                        # 官方语法库

src/
├── CSSCompiler/Parser/                 # CSS解析器
│   ├── css3Lexer.g4                   # CSS词法语法
│   ├── css3Parser.g4                  # CSS解析语法
│   ├── css3Lexer.h/cpp                # 生成的词法分析器
│   └── css3Parser.h/cpp               # 生成的语法分析器
└── JavaScriptCompiler/Parser/          # JavaScript解析器
    ├── JavaScriptLexer.g4             # JS词法语法
    ├── JavaScriptParser.g4            # JS解析语法
    ├── JavaScriptLexer_cpp.h/cpp      # 生成的词法分析器
    └── JavaScriptParser_cpp.h/cpp     # 生成的语法分析器
```

---

## ANTLR构建过程

### 1. 准备ANTLR工具

```bash
# 下载ANTLR JAR文件
wget https://www.antlr.org/download/antlr-4.13.2-complete.jar -O external/antlr-4.13.2-complete.jar

# 验证Java环境
java -jar external/antlr-4.13.2-complete.jar

# 设置ANTLR别名
alias antlr4='java -jar external/antlr-4.13.2-complete.jar'
```

### 2. 生成CSS解析器

```bash
# 进入CSS解析器目录
cd src/CSSCompiler/Parser

# 生成C++解析器
antlr4 -Dlanguage=Cpp \
       -o . \
       -package chtl::css \
       -listener \
       -visitor \
       css3Lexer.g4 css3Parser.g4

# 验证生成的文件
ls -la css3*.h css3*.cpp
```

### 3. 生成JavaScript解析器

```bash
# 进入JavaScript解析器目录
cd src/JavaScriptCompiler/Parser

# 生成C++解析器
antlr4 -Dlanguage=Cpp \
       -o . \
       -package chtl::javascript \
       -listener \
       -visitor \
       JavaScriptLexer.g4 JavaScriptParser.g4

# 验证生成的文件
ls -la JavaScript*.h JavaScript*.cpp
```

### 4. 编译ANTLR运行时

```bash
# 下载ANTLR C++运行时源码
git clone https://github.com/antlr/antlr4.git external/antlr4

# 编译运行时库
cd external/antlr4/runtime/Cpp
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# 安装运行时库
sudo make install
```

---

## CHTL中的ANTLR集成

### CSS编译器集成

```cpp
// CSSCompilerCore.cpp
#include "Parser/css3Lexer.h"
#include "Parser/css3Parser.h"
#include "Parser/css3ParserListener.h"

class CHTLCSSListener : public css3ParserListener {
public:
    void enterStyleRule(css3Parser::StyleRuleContext* ctx) override {
        // 处理CSS规则
        std::string selector = ctx->selector()->getText();
        std::string properties = ctx->declarationList()->getText();
        
        // 集成CHTL特有功能
        processCHTLEnhancements(selector, properties);
    }
    
private:
    void processCHTLEnhancements(const std::string& selector, const std::string& properties) {
        // 处理inherit语句
        if (properties.find("inherit") != std::string::npos) {
            processInheritStatement(properties);
        }
        
        // 处理delete语句
        if (properties.find("delete") != std::string::npos) {
            processDeleteStatement(properties);
        }
        
        // 处理变量组使用
        processVariableGroups(properties);
    }
};

std::string CSSCompilerCore::compile(const std::string& cssCode) {
    // 创建ANTLR输入流
    antlr4::ANTLRInputStream input(cssCode);
    
    // 创建词法分析器
    css3Lexer lexer(&input);
    
    // 创建token流
    antlr4::CommonTokenStream tokens(&lexer);
    
    // 创建语法分析器
    css3Parser parser(&tokens);
    
    // 创建监听器
    CHTLCSSListener listener;
    
    // 解析并遍历
    antlr4::tree::ParseTreeWalker walker;
    walker.walk(&listener, parser.stylesheet());
    
    return listener.getGeneratedCSS();
}
```

### JavaScript编译器集成

```cpp
// JavaScriptCompilerCore.cpp
class CHTLJavaScriptListener : public JavaScriptParser_cppListener {
public:
    void enterFunctionDeclaration(JavaScriptParser_cpp::FunctionDeclarationContext* ctx) override {
        // 处理函数声明
        std::string functionName = ctx->Identifier()->getText();
        
        // 检查是否为CHTL JS生成的函数
        if (isCHTLJSFunction(functionName)) {
            processCHTLJSFunction(ctx);
        }
    }
    
    void enterExpressionStatement(JavaScriptParser_cpp::ExpressionStatementContext* ctx) override {
        // 处理表达式语句
        std::string expression = ctx->expression()->getText();
        
        // 检查CHTL JS特有语法
        if (expression.find("{{") != std::string::npos) {
            processCHTLJSSelector(expression);
        }
        
        if (expression.find("->") != std::string::npos) {
            processArrowOperator(expression);
        }
    }
};
```

---

## 自定义语法扩展

### 扩展CSS语法

```antlr
// 扩展css3Parser.g4
grammar css3Parser;

// 原有CSS规则
stylesheet: cssRule*;
cssRule: selector '{' declarationList '}';

// CHTL扩展规则
declarationList: 
    declaration*
    chtlStatement*;

chtlStatement:
    inheritStatement
    | deleteStatement
    | variableStatement;

inheritStatement: 'inherit' '@Style' Identifier ';';
deleteStatement: 'delete' identifierList ';';
variableStatement: Identifier '(' Identifier ')';

identifierList: Identifier (',' Identifier)*;
```

### 扩展JavaScript语法

```antlr
// 扩展JavaScriptParser.g4
grammar JavaScriptParser;

// 原有JS规则
program: sourceElements? EOF;

// CHTL JS扩展
sourceElements: sourceElement+;
sourceElement: 
    statement 
    | chtlJsStatement;

chtlJsStatement:
    chtlJsSelector
    | chtlJsFunction
    | virStatement;

chtlJsSelector: '{{' selectorExpression '}}';
chtlJsFunction: ('listen' | 'delegate' | 'animate') '(' objectLiteral ')';
virStatement: 'vir' Identifier '=' chtlJsFunction;
```

---

## ANTLR构建工具

### 自动化构建脚本

```bash
#!/bin/bash
# scripts/integration/build_antlr_parsers.sh

set -e

ANTLR_JAR="external/antlr-4.13.2-complete.jar"
ANTLR_CMD="java -jar $ANTLR_JAR"

echo "🔧 构建ANTLR解析器"

# 检查ANTLR JAR
if [ ! -f "$ANTLR_JAR" ]; then
    echo "❌ ANTLR JAR文件不存在: $ANTLR_JAR"
    exit 1
fi

# 检查Java环境
if ! command -v java &> /dev/null; then
    echo "❌ Java未安装或未在PATH中"
    exit 1
fi

echo "✅ 环境检查通过"

# 生成CSS解析器
echo "🎨 生成CSS解析器..."
cd src/CSSCompiler/Parser

$ANTLR_CMD -Dlanguage=Cpp \
           -o . \
           -package chtl::css \
           -listener \
           -visitor \
           css3Lexer.g4 css3Parser.g4

echo "✅ CSS解析器生成完成"

# 生成JavaScript解析器
echo "⚡ 生成JavaScript解析器..."
cd ../../JavaScriptCompiler/Parser

$ANTLR_CMD -Dlanguage=Cpp \
           -o . \
           -package chtl::javascript \
           -listener \
           -visitor \
           JavaScriptLexer.g4 JavaScriptParser.g4

echo "✅ JavaScript解析器生成完成"

cd ../../..
echo "🎉 所有ANTLR解析器构建完成"
```

### 跨平台构建

```bash
#!/bin/bash
# scripts/integration/build_antlr_cross_platform.sh

# 检测平台
PLATFORM=$(uname -s)
ARCH=$(uname -m)

case "$PLATFORM" in
    Linux)
        PLATFORM_DIR="linux-x64"
        ;;
    Darwin)
        PLATFORM_DIR="macos-x64"
        ;;
    MINGW*|MSYS*)
        PLATFORM_DIR="windows-x64"
        ;;
    *)
        echo "❌ 不支持的平台: $PLATFORM"
        exit 1
        ;;
esac

echo "🔍 检测到平台: $PLATFORM ($ARCH)"

# 使用预编译库或编译
PRECOMPILED_DIR="external/antlr4-cross-platform/precompiled/$PLATFORM_DIR"

if [ -d "$PRECOMPILED_DIR" ]; then
    echo "✅ 使用预编译ANTLR库: $PRECOMPILED_DIR"
    export ANTLR4_ROOT="$PRECOMPILED_DIR"
else
    echo "🔧 编译ANTLR运行时库..."
    ./scripts/integration/compile_antlr_runtime.sh
fi
```

---

## ANTLR运行时集成

### CMake集成

```cmake
# CMakeLists.txt中的ANTLR配置
find_package(PkgConfig REQUIRED)

# 查找ANTLR 4库
find_path(ANTLR4_INCLUDE_DIR 
    NAMES antlr4-runtime.h
    PATHS ${ANTLR4_ROOT}/include
          /usr/local/include/antlr4-runtime
          /usr/include/antlr4-runtime
)

find_library(ANTLR4_LIBRARY
    NAMES antlr4-runtime
    PATHS ${ANTLR4_ROOT}/lib
          /usr/local/lib
          /usr/lib
)

if(ANTLR4_INCLUDE_DIR AND ANTLR4_LIBRARY)
    message(STATUS "✅ 找到ANTLR 4: ${ANTLR4_LIBRARY}")
    
    # 创建ANTLR目标
    add_library(antlr4-runtime SHARED IMPORTED)
    set_target_properties(antlr4-runtime PROPERTIES
        IMPORTED_LOCATION ${ANTLR4_LIBRARY}
        INTERFACE_INCLUDE_DIRECTORIES ${ANTLR4_INCLUDE_DIR}
    )
    
    # 链接到编译器
    target_link_libraries(chtl_css_compiler antlr4-runtime)
    target_link_libraries(chtl_js_compiler antlr4-runtime)
else()
    message(FATAL_ERROR "❌ 未找到ANTLR 4库")
endif()
```

### 编译器集成

```cpp
// CSSCompilerCore.h
#include <antlr4-runtime.h>
#include "Parser/css3Lexer.h"
#include "Parser/css3Parser.h"

class CSSCompilerCore {
public:
    std::string compileCSS(const std::string& cssCode);
    
private:
    std::unique_ptr<css3Lexer> lexer_;
    std::unique_ptr<css3Parser> parser_;
    std::unique_ptr<antlr4::CommonTokenStream> tokens_;
};

// CSSCompilerCore.cpp
std::string CSSCompilerCore::compileCSS(const std::string& cssCode) {
    try {
        // 创建输入流
        antlr4::ANTLRInputStream input(cssCode);
        
        // 创建词法分析器
        css3Lexer lexer(&input);
        
        // 创建token流
        antlr4::CommonTokenStream tokens(&lexer);
        
        // 创建语法分析器
        css3Parser parser(&tokens);
        
        // 解析CSS
        auto tree = parser.stylesheet();
        
        // 遍历语法树并生成代码
        CHTLCSSVisitor visitor;
        return visitor.visit(tree);
        
    } catch (const std::exception& e) {
        throw std::runtime_error("CSS解析失败: " + std::string(e.what()));
    }
}
```

---

## 语法定义维护

### 更新语法文件

```bash
# 从官方仓库更新CSS语法
cd external/grammars-v4
git pull origin master

# 复制最新的CSS语法
cp css3/css3Lexer.g4 ../../src/CSSCompiler/Parser/
cp css3/css3Parser.g4 ../../src/CSSCompiler/Parser/

# 复制最新的JavaScript语法
cp javascript/JavaScriptLexer.g4 ../../src/JavaScriptCompiler/Parser/
cp javascript/JavaScriptParser.g4 ../../src/JavaScriptCompiler/Parser/
```

### 自定义语法修改

```antlr
// 在css3Parser.g4中添加CHTL扩展
parser grammar css3Parser;

// 原有规则...

// CHTL扩展规则
chtlEnhancedDeclaration:
    inheritDeclaration
    | deleteDeclaration
    | variableDeclaration;

inheritDeclaration: 
    'inherit' '@Style' ws* identifier ws* ';';

deleteDeclaration: 
    'delete' ws* identifierList ws* ';';

variableDeclaration:
    identifier '(' identifier ('=' expression)? ')';

identifierList: 
    identifier (ws* ',' ws* identifier)*;
```

---

## 性能优化

### 解析器性能优化

```cpp
// 使用解析器池
class ANTLRParserPool {
private:
    std::queue<std::unique_ptr<css3Parser>> cssParserPool_;
    std::queue<std::unique_ptr<JavaScriptParser_cpp>> jsParserPool_;
    std::mutex poolMutex_;
    
public:
    std::unique_ptr<css3Parser> getCSSParser() {
        std::lock_guard<std::mutex> lock(poolMutex_);
        
        if (!cssParserPool_.empty()) {
            auto parser = std::move(cssParserPool_.front());
            cssParserPool_.pop();
            return parser;
        }
        
        return std::make_unique<css3Parser>(nullptr);
    }
    
    void returnCSSParser(std::unique_ptr<css3Parser> parser) {
        std::lock_guard<std::mutex> lock(poolMutex_);
        parser->reset(); // 重置解析器状态
        cssParserPool_.push(std::move(parser));
    }
};
```

### 内存优化

```cpp
// 使用共享语法树
class SharedParseTree {
private:
    static std::unordered_map<std::string, std::shared_ptr<antlr4::tree::ParseTree>> cache_;
    
public:
    static std::shared_ptr<antlr4::tree::ParseTree> getOrParse(const std::string& code) {
        auto hash = std::hash<std::string>{}(code);
        auto key = std::to_string(hash);
        
        auto it = cache_.find(key);
        if (it != cache_.end()) {
            return it->second; // 返回缓存的语法树
        }
        
        // 解析并缓存
        auto tree = parseCode(code);
        cache_[key] = tree;
        return tree;
    }
};
```

---

## 错误处理

### ANTLR错误处理

```cpp
// 自定义错误监听器
class CHTLANTLRErrorListener : public antlr4::BaseErrorListener {
public:
    void syntaxError(antlr4::Recognizer* recognizer,
                     antlr4::Token* offendingSymbol,
                     size_t line,
                     size_t charPositionInLine,
                     const std::string& msg,
                     std::exception_ptr e) override {
        
        std::string errorMsg = "语法错误 (行 " + std::to_string(line) + 
                              ", 列 " + std::to_string(charPositionInLine) + "): " + msg;
        
        // 集成到CHTL错误处理系统
        ErrorHandler::getInstance().reportError(
            ErrorType::SYNTAX_ERROR,
            "ANTLR_PARSE_ERROR",
            errorMsg,
            "", line, charPositionInLine
        );
    }
};

// 使用自定义错误监听器
void setupErrorHandling(css3Parser& parser) {
    parser.removeErrorListeners(); // 移除默认监听器
    parser.addErrorListener(new CHTLANTLRErrorListener());
}
```

---

## 调试和诊断

### ANTLR调试

```bash
# 启用ANTLR调试模式
export ANTLR_DEBUG=1

# 生成调试版本的解析器
antlr4 -Dlanguage=Cpp -debug css3Lexer.g4 css3Parser.g4

# 查看解析过程
./build/chtl_compiler --debug-antlr test.chtl
```

### 语法树可视化

```cpp
// 输出语法树
void visualizeParseTree(antlr4::tree::ParseTree* tree) {
    std::function<void(antlr4::tree::ParseTree*, int)> printTree = 
        [&](antlr4::tree::ParseTree* node, int depth) {
            std::string indent(depth * 2, ' ');
            std::cout << indent << node->getText() << std::endl;
            
            for (size_t i = 0; i < node->children.size(); i++) {
                printTree(node->children[i], depth + 1);
            }
        };
    
    printTree(tree, 0);
}
```

---

## 测试ANTLR集成

### 单元测试

```cpp
// 测试CSS解析
TEST(ANTLRIntegration, CSSParsing) {
    CSSCompilerCore compiler;
    
    std::string cssCode = R"(
        .button {
            background: #3b82f6;
            inherit @Style ButtonBase;
            delete margin, padding;
        }
    )";
    
    std::string result = compiler.compileCSS(cssCode);
    
    EXPECT_TRUE(result.find("background") != std::string::npos);
    EXPECT_TRUE(result.find("ButtonBase") != std::string::npos);
}

// 测试JavaScript解析
TEST(ANTLRIntegration, JavaScriptParsing) {
    JavaScriptCompilerCore compiler;
    
    std::string jsCode = R"(
        {{.button}}->listen({
            click: function(e) {
                console.log('clicked');
            }
        });
    )";
    
    std::string result = compiler.compileJS(jsCode);
    
    EXPECT_TRUE(result.find("addEventListener") != std::string::npos);
    EXPECT_TRUE(result.find("click") != std::string::npos);
}
```

### 集成测试

```bash
# 测试完整的ANTLR集成
./build/test_antlr_integration

# 测试CSS和JS混合编译
echo '
div {
    style {
        .test { color: red; }
    }
    script {
        {{.test}}->listen({ click: handler });
    }
}
' | ./build/chtl_compiler --test-antlr
```

---

## 部署ANTLR集成

### 运行时部署

```bash
# 安装ANTLR运行时库
sudo apt install libantlr4-runtime-dev

# 或从源码安装
cd external/antlr4/runtime/Cpp
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
sudo make install
sudo ldconfig
```

### 静态链接部署

```cmake
# 静态链接ANTLR运行时
set(ANTLR4_WITH_STATIC_CRT ON)
target_link_libraries(chtl_compiler 
    ${ANTLR4_STATIC_LIBRARIES}
)
```

---

## 版本管理

### ANTLR版本升级

```bash
# 下载新版本ANTLR
wget https://www.antlr.org/download/antlr-4.14.0-complete.jar

# 备份当前版本
mv external/antlr-4.13.2-complete.jar external/antlr-4.13.2-complete.jar.bak

# 更新版本
mv antlr-4.14.0-complete.jar external/

# 重新生成解析器
./scripts/integration/build_antlr_parsers.sh

# 测试新版本
make test_antlr
```

### 兼容性管理

```cpp
// 版本兼容性检查
#if ANTLR4_VERSION_MAJOR >= 4 && ANTLR4_VERSION_MINOR >= 13
    // 使用新版本API
    auto tree = parser.stylesheet();
#else
    // 使用旧版本API
    auto tree = parser.stylesheetOld();
#endif
```

---

## 故障排除

### 常见ANTLR问题

#### 1. Java版本不兼容

```bash
# 错误: UnsupportedClassVersionError
# 解决: 升级Java版本
sudo apt install openjdk-11-jdk
export JAVA_HOME=/usr/lib/jvm/java-11-openjdk
```

#### 2. 生成的文件编译错误

```bash
# 错误: 生成的C++代码编译失败
# 解决: 检查ANTLR版本和语法文件兼容性

# 清理生成的文件
rm src/CSSCompiler/Parser/css3*.h
rm src/CSSCompiler/Parser/css3*.cpp

# 重新生成
./scripts/integration/build_antlr_parsers.sh
```

#### 3. 运行时库找不到

```bash
# 错误: libantlr4-runtime.so not found
# 解决: 设置库路径
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

# 或安装到系统路径
sudo make install
sudo ldconfig
```

---

## 性能监控

### ANTLR性能分析

```cpp
// 解析性能监控
class ANTLRPerformanceMonitor {
public:
    static void startTiming(const std::string& operation) {
        timings_[operation] = std::chrono::high_resolution_clock::now();
    }
    
    static void endTiming(const std::string& operation) {
        auto end = std::chrono::high_resolution_clock::now();
        auto start = timings_[operation];
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << operation << " 耗时: " << duration.count() << "μs" << std::endl;
    }
    
private:
    static std::unordered_map<std::string, std::chrono::time_point<std::chrono::high_resolution_clock>> timings_;
};

// 使用性能监控
ANTLRPerformanceMonitor::startTiming("CSS解析");
auto result = parser.stylesheet();
ANTLRPerformanceMonitor::endTiming("CSS解析");
```

---

## 未来规划

### ANTLR 5迁移准备

ANTLR 5正在开发中，我们为未来的迁移做准备：

1. **接口抽象**: 创建抽象的解析器接口
2. **版本检测**: 支持多版本ANTLR共存
3. **渐进迁移**: 逐步迁移到新版本
4. **兼容性保证**: 保持向后兼容

### 语法扩展计划

1. **TypeScript支持**: 集成TypeScript语法解析
2. **WebAssembly支持**: 支持WASM语法
3. **CSS新特性**: 跟进CSS新规范
4. **ES新特性**: 支持最新JavaScript特性

---

## 社区贡献

### 语法改进

鼓励社区贡献语法改进：

1. **语法优化**: 改进现有语法定义
2. **性能提升**: 优化解析性能
3. **错误处理**: 改进错误报告
4. **新特性**: 添加新的语法支持

### 质量保证

1. **语法测试**: 全面的语法测试覆盖
2. **性能基准**: 建立性能基准测试
3. **兼容性测试**: 多版本兼容性验证
4. **回归测试**: 防止功能回归

---

ANTLR 4的集成为CHTL提供了强大的CSS和JavaScript解析能力，确保了语法的标准性和兼容性。通过本指南，开发者可以深入理解ANTLR在CHTL中的作用，并参与到相关功能的开发中来。