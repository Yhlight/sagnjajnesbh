# 统一扫描器体系说明

## 概述

统一扫描器 (CHTLUnifiedScanner) 是CHTL编译器的底层核心系统，负责智能扫描和解析CHTL源码。本指南详细介绍扫描器的设计原理、工作机制和技术实现。

---

## 设计理念

### 为什么需要统一扫描器

传统的编译器通常使用固定长度的词法分析，但CHTL面临独特的挑战：

1. **混合语法**: CHTL、CHTL JS、CSS、JavaScript混合在一个文件中
2. **上下文敏感**: 相同的语法在不同上下文中有不同含义
3. **嵌套结构**: 复杂的嵌套块结构
4. **无修饰字面量**: 需要基于上下文判断，而非简单的语法规则

### 核心设计原则

1. **上下文感知**: 基于上下文进行智能判断
2. **变量长度切片**: 动态调整扫描长度
3. **统一接口**: 为所有编译器提供统一的扫描接口
4. **高性能**: 优化的扫描算法

---

## 扫描器架构

### 核心组件

```
CHTLUnifiedScanner
├── ContextManager          # 上下文管理器
├── SlicingEngine          # 切片引擎
├── TokenClassifier        # 标记分类器
├── ContextDetector        # 上下文检测器
└── FragmentProcessor      # 片段处理器
```

### 扫描流程

```
源码输入
    ↓
上下文检测 (CHTL/CHTL JS/CSS/JS)
    ↓
变量长度切片 (动态调整扫描长度)
    ↓
标记分类 (关键字/字面量/操作符等)
    ↓
上下文验证 (验证标记在当前上下文的有效性)
    ↓
输出到对应编译器
```

---

## 变量长度切片技术

### 传统固定长度扫描的问题

```cpp
// 传统方式：固定长度扫描
char buffer[1024];
while (read(buffer, 1024)) {
    // 问题：可能截断完整的语法结构
    // 例如：{ color: red; } 可能被截断为 { color: r
}
```

### CHTL的变量长度切片

```cpp
class VariableLengthSlicer {
public:
    struct Slice {
        std::string content;
        size_t startPos;
        size_t length;
        bool isComplete;
        std::string context;
    };
    
    std::vector<Slice> performSlicing(const std::string& source) {
        std::vector<Slice> slices;
        size_t pos = 0;
        
        while (pos < source.length()) {
            // 检测当前上下文
            Context context = detectContext(source, pos);
            
            // 根据上下文确定切片长度
            size_t sliceLength = determineSliceLength(source, pos, context);
            
            // 确保切片完整性
            sliceLength = ensureCompleteness(source, pos, sliceLength, context);
            
            // 创建切片
            Slice slice;
            slice.content = source.substr(pos, sliceLength);
            slice.startPos = pos;
            slice.length = sliceLength;
            slice.isComplete = true;
            slice.context = contextToString(context);
            
            slices.push_back(slice);
            pos += sliceLength;
        }
        
        return slices;
    }
    
private:
    size_t determineSliceLength(const std::string& source, size_t pos, Context context) {
        switch (context) {
            case Context::CHTL_ELEMENT:
                return findElementEnd(source, pos);
            case Context::CHTL_JS:
                return findScriptBlockEnd(source, pos);
            case Context::CSS:
                return findStyleBlockEnd(source, pos);
            case Context::TEXT:
                return findTextNodeEnd(source, pos);
            default:
                return findBasicTokenEnd(source, pos);
        }
    }
};
```

---

## 上下文检测系统

### 上下文类型

```cpp
enum class Context {
    UNKNOWN,            // 未知上下文
    CHTL_ELEMENT,       // CHTL元素上下文
    CHTL_ATTRIBUTE,     // CHTL属性上下文
    CHTL_TEXT,          // CHTL文本上下文
    CHTL_JS,            // CHTL JS上下文
    CSS,                // CSS上下文
    JAVASCRIPT,         // JavaScript上下文
    TEMPLATE,           // 模板上下文
    CUSTOM,             // 自定义上下文
    IMPORT,             // 导入上下文
    NAMESPACE,          // 命名空间上下文
    CONFIGURATION       // 配置上下文
};
```

### 上下文检测算法

```cpp
class ContextDetector {
public:
    Context detectContext(const std::string& source, size_t position) {
        // 构建上下文栈
        std::stack<Context> contextStack;
        
        // 从文件开始扫描到当前位置
        for (size_t i = 0; i <= position; i++) {
            char c = source[i];
            
            // 检测块开始
            if (isBlockStart(source, i)) {
                Context newContext = determineBlockContext(source, i);
                contextStack.push(newContext);
            }
            
            // 检测块结束
            if (c == '}' && !contextStack.empty()) {
                contextStack.pop();
            }
        }
        
        return contextStack.empty() ? Context::CHTL_ELEMENT : contextStack.top();
    }
    
private:
    bool isBlockStart(const std::string& source, size_t pos) {
        if (source[pos] != '{') return false;
        
        // 查找块前的关键字
        std::string beforeBlock = extractBeforeBlock(source, pos);
        return isValidBlockKeyword(beforeBlock);
    }
    
    Context determineBlockContext(const std::string& source, size_t pos) {
        std::string keyword = extractBlockKeyword(source, pos);
        
        if (keyword == "style") return Context::CSS;
        if (keyword == "script") return Context::CHTL_JS;
        if (keyword == "text") return Context::CHTL_TEXT;
        if (keyword.find("[Template]") == 0) return Context::TEMPLATE;
        if (keyword.find("[Custom]") == 0) return Context::CUSTOM;
        
        return Context::CHTL_ELEMENT;
    }
};
```

---

## 无修饰字面量处理

### 上下文相关判断

CHTL的一个重要特性是**不将无修饰字面量作为CHTL JS的判断条件**，而是基于上下文：

```cpp
class UnquotedLiteralProcessor {
public:
    bool isValidUnquotedLiteral(const std::string& token, Context context) {
        switch (context) {
            case Context::CHTL_ATTRIBUTE:
                // 在属性值中，无修饰字面量是有效的
                return isValidAttributeValue(token);
                
            case Context::CHTL_TEXT:
                // 在文本节点中，无修饰字面量是有效的
                return true;
                
            case Context::CSS:
                // 在CSS中，无修饰字面量按CSS规则处理
                return isValidCSSValue(token);
                
            case Context::CHTL_JS:
                // 在CHTL JS中，需要更仔细的判断
                return isValidCHTLJSToken(token);
                
            default:
                return false;
        }
    }
    
private:
    bool isValidCHTLJSToken(const std::string& token) {
        // 不能仅仅因为是无修饰字面量就判断为CHTL JS
        // 需要结合语法结构判断
        
        // 检查是否为CHTL JS关键字
        if (isCHTLJSKeyword(token)) return true;
        
        // 检查是否在CHTL JS语法结构中
        if (isInCHTLJSStructure(token)) return true;
        
        // 检查是否为有效的JavaScript标识符
        if (isValidJSIdentifier(token)) return true;
        
        return false;
    }
};
```

### 智能字面量识别

```cpp
class SmartLiteralRecognizer {
public:
    LiteralType recognizeLiteral(const std::string& token, Context context) {
        // CSS上下文中的字面量
        if (context == Context::CSS) {
            if (isCSSColor(token)) return LiteralType::CSS_COLOR;
            if (isCSSUnit(token)) return LiteralType::CSS_UNIT;
            if (isCSSKeyword(token)) return LiteralType::CSS_KEYWORD;
        }
        
        // CHTL JS上下文中的字面量
        if (context == Context::CHTL_JS) {
            if (isJSKeyword(token)) return LiteralType::JS_KEYWORD;
            if (isJSOperator(token)) return LiteralType::JS_OPERATOR;
            if (isCHTLJSSelector(token)) return LiteralType::CHTLJS_SELECTOR;
        }
        
        // 属性上下文中的字面量
        if (context == Context::CHTL_ATTRIBUTE) {
            return LiteralType::ATTRIBUTE_VALUE;
        }
        
        return LiteralType::PLAIN_TEXT;
    }
};
```

---

## 扫描器性能优化

### 缓存机制

```cpp
class ScannerCache {
private:
    std::unordered_map<std::string, std::vector<Token>> tokenCache_;
    std::unordered_map<std::string, Context> contextCache_;
    std::unordered_map<size_t, FragmentInfo> fragmentCache_;
    
public:
    std::vector<Token> getCachedTokens(const std::string& content) {
        auto hash = std::hash<std::string>{}(content);
        auto key = std::to_string(hash);
        
        auto it = tokenCache_.find(key);
        if (it != tokenCache_.end()) {
            return it->second; // 缓存命中
        }
        
        // 缓存未命中，进行扫描
        auto tokens = performScanning(content);
        tokenCache_[key] = tokens;
        return tokens;
    }
    
    void invalidateCache(const std::string& content) {
        auto hash = std::hash<std::string>{}(content);
        auto key = std::to_string(hash);
        tokenCache_.erase(key);
    }
};
```

### 并行扫描

```cpp
class ParallelScanner {
public:
    std::vector<Fragment> scanParallel(const std::string& source) {
        // 将源码分割为独立的片段
        auto fragments = divideIntoFragments(source);
        
        // 并行处理每个片段
        std::vector<std::future<Fragment>> futures;
        
        for (auto& fragment : fragments) {
            futures.push_back(std::async(std::launch::async, [&fragment]() {
                return processFragment(fragment);
            }));
        }
        
        // 收集结果
        std::vector<Fragment> results;
        for (auto& future : futures) {
            results.push_back(future.get());
        }
        
        return results;
    }
    
private:
    std::vector<Fragment> divideIntoFragments(const std::string& source) {
        // 智能分割：在安全边界分割
        std::vector<Fragment> fragments;
        size_t pos = 0;
        
        while (pos < source.length()) {
            size_t nextBoundary = findSafeBoundary(source, pos);
            
            Fragment fragment;
            fragment.content = source.substr(pos, nextBoundary - pos);
            fragment.startPos = pos;
            fragments.push_back(fragment);
            
            pos = nextBoundary;
        }
        
        return fragments;
    }
    
    size_t findSafeBoundary(const std::string& source, size_t startPos) {
        // 查找安全的分割点（块边界、语句边界等）
        for (size_t i = startPos; i < source.length(); i++) {
            if (isSafeBoundary(source, i)) {
                return i;
            }
        }
        return source.length();
    }
};
```

---

## 扫描器状态管理

### 状态机设计

```cpp
enum class ScannerState {
    INITIAL,                // 初始状态
    IN_ELEMENT,            // 在元素中
    IN_ATTRIBUTE,          // 在属性中
    IN_TEXT_NODE,          // 在文本节点中
    IN_STYLE_BLOCK,        // 在样式块中
    IN_SCRIPT_BLOCK,       # 在脚本块中
    IN_TEMPLATE,           // 在模板中
    IN_CUSTOM,             // 在自定义中
    IN_IMPORT,             // 在导入中
    IN_NAMESPACE,          // 在命名空间中
    IN_COMMENT,            // 在注释中
    ERROR                  // 错误状态
};

class ScannerStateMachine {
private:
    ScannerState currentState_ = ScannerState::INITIAL;
    std::stack<ScannerState> stateStack_;
    
public:
    void transitionTo(ScannerState newState) {
        stateStack_.push(currentState_);
        currentState_ = newState;
    }
    
    void returnToPreviousState() {
        if (!stateStack_.empty()) {
            currentState_ = stateStack_.top();
            stateStack_.pop();
        }
    }
    
    bool canTransition(ScannerState from, ScannerState to) {
        // 定义有效的状态转换
        static const std::map<ScannerState, std::set<ScannerState>> validTransitions = {
            {ScannerState::INITIAL, {ScannerState::IN_ELEMENT, ScannerState::IN_IMPORT, ScannerState::IN_NAMESPACE}},
            {ScannerState::IN_ELEMENT, {ScannerState::IN_ATTRIBUTE, ScannerState::IN_TEXT_NODE, ScannerState::IN_STYLE_BLOCK, ScannerState::IN_SCRIPT_BLOCK}},
            {ScannerState::IN_STYLE_BLOCK, {ScannerState::IN_ELEMENT}},
            {ScannerState::IN_SCRIPT_BLOCK, {ScannerState::IN_ELEMENT}}
        };
        
        auto it = validTransitions.find(from);
        return it != validTransitions.end() && it->second.count(to) > 0;
    }
};
```

---

## 上下文感知扫描

### 上下文检测算法

```cpp
class ContextAwareScanner {
public:
    Context detectCurrentContext(const std::string& source, size_t position) {
        // 构建上下文历史
        ContextHistory history = buildContextHistory(source, position);
        
        // 分析当前位置的上下文
        Context current = analyzeCurrentPosition(source, position, history);
        
        // 验证上下文合法性
        if (!isValidContext(current, history)) {
            return Context::ERROR;
        }
        
        return current;
    }
    
private:
    ContextHistory buildContextHistory(const std::string& source, size_t position) {
        ContextHistory history;
        std::stack<Context> contextStack;
        
        for (size_t i = 0; i < position; i++) {
            char c = source[i];
            
            // 检测上下文变化点
            if (isContextChangePoint(source, i)) {
                Context newContext = determineNewContext(source, i);
                
                if (c == '{') {
                    contextStack.push(newContext);
                    history.pushContext(newContext, i);
                } else if (c == '}') {
                    if (!contextStack.empty()) {
                        Context closedContext = contextStack.top();
                        contextStack.pop();
                        history.popContext(closedContext, i);
                    }
                }
            }
        }
        
        return history;
    }
    
    Context analyzeCurrentPosition(const std::string& source, size_t position, const ContextHistory& history) {
        // 获取当前的上下文栈
        auto currentStack = history.getCurrentStack();
        
        if (currentStack.empty()) {
            return Context::CHTL_ELEMENT; // 默认CHTL元素上下文
        }
        
        return currentStack.top();
    }
};
```

### 智能边界检测

```cpp
class BoundaryDetector {
public:
    struct Boundary {
        size_t position;
        BoundaryType type;
        Context beforeContext;
        Context afterContext;
    };
    
    std::vector<Boundary> detectBoundaries(const std::string& source) {
        std::vector<Boundary> boundaries;
        
        for (size_t i = 0; i < source.length(); i++) {
            if (isBoundaryCandidate(source, i)) {
                Boundary boundary = analyzeBoundary(source, i);
                if (boundary.type != BoundaryType::NONE) {
                    boundaries.push_back(boundary);
                }
            }
        }
        
        return boundaries;
    }
    
private:
    bool isBoundaryCandidate(const std::string& source, size_t pos) {
        char c = source[pos];
        
        // 潜在的边界字符
        return c == '{' || c == '}' || c == '[' || c == ']' || 
               c == ';' || c == '\n' || isKeywordStart(source, pos);
    }
    
    Boundary analyzeBoundary(const std::string& source, size_t pos) {
        Boundary boundary;
        boundary.position = pos;
        
        // 分析边界前后的上下文
        boundary.beforeContext = detectContext(source, pos - 1);
        boundary.afterContext = detectContext(source, pos + 1);
        
        // 确定边界类型
        boundary.type = determineBoundaryType(source, pos, boundary.beforeContext, boundary.afterContext);
        
        return boundary;
    }
};
```

---

## 扫描器优化技术

### 预测性扫描

```cpp
class PredictiveScanner {
public:
    struct Prediction {
        std::string expectedToken;
        double probability;
        Context context;
    };
    
    std::vector<Prediction> predictNextTokens(const std::string& source, size_t position) {
        std::vector<Prediction> predictions;
        
        // 基于当前上下文预测
        Context currentContext = detectContext(source, position);
        auto contextPredictions = predictByContext(currentContext);
        
        // 基于历史模式预测
        auto patternPredictions = predictByPattern(source, position);
        
        // 合并预测结果
        predictions.insert(predictions.end(), contextPredictions.begin(), contextPredictions.end());
        predictions.insert(predictions.end(), patternPredictions.begin(), patternPredictions.end());
        
        // 按概率排序
        std::sort(predictions.begin(), predictions.end(), 
                 [](const Prediction& a, const Prediction& b) {
                     return a.probability > b.probability;
                 });
        
        return predictions;
    }
    
private:
    std::vector<Prediction> predictByContext(Context context) {
        static const std::map<Context, std::vector<std::string>> contextTokens = {
            {Context::CHTL_ELEMENT, {"div", "span", "p", "h1", "button", "style", "script", "text"}},
            {Context::CSS, {"color", "background", "margin", "padding", "inherit", "delete"}},
            {Context::CHTL_JS, {"listen", "delegate", "animate", "vir", "{{", "->"}},
            {Context::CHTL_ATTRIBUTE, {"class", "id", "src", "href", "data-"}}
        };
        
        std::vector<Prediction> predictions;
        auto it = contextTokens.find(context);
        
        if (it != contextTokens.end()) {
            for (const auto& token : it->second) {
                predictions.push_back({token, 0.8, context});
            }
        }
        
        return predictions;
    }
};
```

### 增量扫描

```cpp
class IncrementalScanner {
private:
    struct ScanResult {
        std::vector<Token> tokens;
        std::string contentHash;
        std::chrono::time_point<std::chrono::steady_clock> timestamp;
    };
    
    std::unordered_map<std::string, ScanResult> scanCache_;
    
public:
    std::vector<Token> scanIncremental(const std::string& source, 
                                      const std::vector<Change>& changes) {
        // 检查是否可以使用缓存
        if (changes.empty()) {
            return getCachedResult(source);
        }
        
        // 分析变更影响范围
        auto affectedRanges = analyzeChangeImpact(changes);
        
        // 只重新扫描受影响的范围
        std::vector<Token> result = getCachedResult(source);
        
        for (const auto& range : affectedRanges) {
            auto newTokens = scanRange(source, range.start, range.end);
            replaceTokensInRange(result, range, newTokens);
        }
        
        // 更新缓存
        updateCache(source, result);
        
        return result;
    }
};
```

---

## 扫描器接口设计

### 统一扫描接口

```cpp
class CHTLUnifiedScanner {
public:
    struct ScanOptions {
        bool enableContextDetection = true;
        bool enablePredictiveScanning = false;
        bool enableIncrementalScanning = true;
        bool enableCaching = true;
        int maxCacheSize = 1000;
        bool debugMode = false;
    };
    
    // 主扫描接口
    std::vector<Fragment> scan(const std::string& source, const ScanOptions& options = {});
    
    // 上下文感知扫描
    std::vector<Fragment> contextAwareScan(const std::string& source, Context initialContext = Context::UNKNOWN);
    
    // 增量扫描
    std::vector<Fragment> incrementalScan(const std::string& source, const std::vector<Change>& changes);
    
    // 预测性扫描
    std::vector<Prediction> predictiveScaN(const std::string& source, size_t position);
    
    // 调试接口
    std::string getDebugInfo() const;
    void enableDebugMode(bool enable = true);
    
    // 性能接口
    ScannerStatistics getStatistics() const;
    void resetStatistics();
    
    // 配置接口
    void setOptions(const ScanOptions& options);
    ScanOptions getOptions() const;
};
```

### 编译器集成接口

```cpp
class CompilerScannerInterface {
public:
    // 为CHTL编译器提供的接口
    std::vector<CHTLToken> scanForCHTL(const std::string& source);
    
    // 为CHTL JS编译器提供的接口
    std::vector<CHTLJSToken> scanForCHTLJS(const std::string& source);
    
    // 为CSS编译器提供的接口
    std::vector<CSSToken> scanForCSS(const std::string& source);
    
    // 为JavaScript编译器提供的接口
    std::vector<JSToken> scanForJavaScript(const std::string& source);
    
    // 混合扫描接口
    MixedScanResult scanMixed(const std::string& source);
};
```

---

## 扫描器测试

### 单元测试

```cpp
// 测试上下文检测
TEST(UnifiedScanner, ContextDetection) {
    CHTLUnifiedScanner scanner;
    
    std::string source = R"(
        div {
            style {
                color: red;
            }
            script {
                {{.button}}->listen({click: handler});
            }
        }
    )";
    
    auto fragments = scanner.contextAwareScan(source);
    
    // 验证上下文检测正确性
    EXPECT_EQ(fragments[0].context, Context::CHTL_ELEMENT);
    EXPECT_EQ(fragments[1].context, Context::CSS);
    EXPECT_EQ(fragments[2].context, Context::CHTL_JS);
}

// 测试变量长度切片
TEST(UnifiedScanner, VariableLengthSlicing) {
    CHTLUnifiedScanner scanner;
    
    std::string source = "{{.button}}->click()";
    auto fragments = scanner.scan(source);
    
    // 验证切片完整性
    EXPECT_TRUE(fragments[0].isComplete);
    EXPECT_EQ(fragments[0].content, "{{.button}}");
    EXPECT_EQ(fragments[1].content, "->");
    EXPECT_EQ(fragments[2].content, "click()");
}
```

### 性能测试

```cpp
// 扫描器性能基准测试
TEST(UnifiedScanner, PerformanceBenchmark) {
    CHTLUnifiedScanner scanner;
    
    // 生成大型测试文件
    std::string largeSource = generateLargeTestFile(10000); // 10K行
    
    auto start = std::chrono::high_resolution_clock::now();
    auto fragments = scanner.scan(largeSource);
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // 验证性能要求
    EXPECT_LT(duration.count(), 1000); // 应在1秒内完成
    
    // 验证扫描质量
    EXPECT_GT(fragments.size(), 1000); // 应该扫描出足够的片段
}
```

---

## 扫描器调试

### 调试输出

```cpp
class ScannerDebugger {
public:
    void enableVerboseLogging(bool enable = true) {
        verboseLogging_ = enable;
    }
    
    void logScanStep(const std::string& step, const std::string& content, Context context) {
        if (verboseLogging_) {
            std::cout << "[Scanner Debug] " << step << ": " 
                     << content << " (Context: " << contextToString(context) << ")" 
                     << std::endl;
        }
    }
    
    void logContextTransition(Context from, Context to, size_t position) {
        if (verboseLogging_) {
            std::cout << "[Context] " << position << ": " 
                     << contextToString(from) << " -> " << contextToString(to) 
                     << std::endl;
        }
    }
    
    std::string generateScanReport(const std::vector<Fragment>& fragments) {
        std::ostringstream report;
        
        report << "扫描器报告\n";
        report << "==========\n";
        report << "片段数量: " << fragments.size() << "\n";
        
        std::map<Context, int> contextCounts;
        for (const auto& fragment : fragments) {
            contextCounts[fragment.context]++;
        }
        
        report << "\n上下文分布:\n";
        for (const auto& [context, count] : contextCounts) {
            report << "  " << contextToString(context) << ": " << count << "\n";
        }
        
        return report.str();
    }
    
private:
    bool verboseLogging_ = false;
};
```

### 可视化工具

```cpp
// 扫描结果可视化
class ScannerVisualizer {
public:
    std::string generateHTML(const std::vector<Fragment>& fragments) {
        std::ostringstream html;
        
        html << "<!DOCTYPE html><html><head>";
        html << "<style>";
        html << ".fragment { border: 1px solid #ccc; margin: 2px; padding: 4px; }";
        html << ".chtl { background: #e3f2fd; }";
        html << ".chtljs { background: #f3e5f5; }";
        html << ".css { background: #e8f5e8; }";
        html << ".js { background: #fff3e0; }";
        html << "</style></head><body>";
        
        for (const auto& fragment : fragments) {
            html << "<div class='fragment " << getContextClass(fragment.context) << "'>";
            html << "<strong>" << contextToString(fragment.context) << "</strong><br>";
            html << "<code>" << escapeHTML(fragment.content) << "</code>";
            html << "</div>";
        }
        
        html << "</body></html>";
        return html.str();
    }
    
private:
    std::string getContextClass(Context context) {
        switch (context) {
            case Context::CHTL_ELEMENT: return "chtl";
            case Context::CHTL_JS: return "chtljs";
            case Context::CSS: return "css";
            case Context::JAVASCRIPT: return "js";
            default: return "unknown";
        }
    }
};
```

---

## 扩展扫描器

### 添加新的上下文类型

```cpp
// 1. 扩展Context枚举
enum class Context {
    // 现有上下文...
    NEW_CONTEXT,        // 新的上下文类型
};

// 2. 更新上下文检测
Context ContextDetector::determineBlockContext(const std::string& source, size_t pos) {
    std::string keyword = extractBlockKeyword(source, pos);
    
    // 添加新的上下文检测
    if (keyword == "newblock") return Context::NEW_CONTEXT;
    
    // 现有检测逻辑...
    return Context::CHTL_ELEMENT;
}

// 3. 更新扫描逻辑
size_t VariableLengthSlicer::determineSliceLength(const std::string& source, size_t pos, Context context) {
    switch (context) {
        case Context::NEW_CONTEXT:
            return findNewContextEnd(source, pos);
        // 现有逻辑...
    }
}
```

### 自定义扫描器扩展

```cpp
class CustomScannerExtension {
public:
    virtual ~CustomScannerExtension() = default;
    
    // 扩展接口
    virtual bool canHandle(Context context) = 0;
    virtual std::vector<Token> scanContext(const std::string& source, size_t start, size_t end, Context context) = 0;
    virtual Context detectCustomContext(const std::string& source, size_t position) = 0;
};

// 注册扩展
void CHTLUnifiedScanner::registerExtension(std::shared_ptr<CustomScannerExtension> extension) {
    extensions_.push_back(extension);
}
```

---

## 与编译器的协作

### 扫描器-编译器接口

```cpp
class ScannerCompilerBridge {
public:
    // 为编译器提供统一的扫描结果
    CompilerInput prepareScanResultForCompiler(const std::vector<Fragment>& fragments, 
                                              CompilerType targetCompiler) {
        CompilerInput input;
        
        for (const auto& fragment : fragments) {
            if (isRelevantForCompiler(fragment.context, targetCompiler)) {
                input.fragments.push_back(adaptFragmentForCompiler(fragment, targetCompiler));
            }
        }
        
        return input;
    }
    
private:
    bool isRelevantForCompiler(Context context, CompilerType compiler) {
        switch (compiler) {
            case CompilerType::CHTL:
                return context == Context::CHTL_ELEMENT || 
                       context == Context::CHTL_ATTRIBUTE ||
                       context == Context::CHTL_TEXT;
                       
            case CompilerType::CHTL_JS:
                return context == Context::CHTL_JS;
                
            case CompilerType::CSS:
                return context == Context::CSS;
                
            case CompilerType::JAVASCRIPT:
                return context == Context::JAVASCRIPT;
        }
        return false;
    }
};
```

---

## 扫描器监控

### 性能监控

```cpp
class ScannerMonitor {
private:
    struct PerformanceMetrics {
        size_t totalScans = 0;
        size_t totalTokens = 0;
        std::chrono::microseconds totalTime{0};
        size_t cacheHits = 0;
        size_t cacheMisses = 0;
        std::map<Context, size_t> contextCounts;
    };
    
    PerformanceMetrics metrics_;
    
public:
    void recordScan(size_t tokenCount, std::chrono::microseconds duration, 
                   const std::map<Context, size_t>& contexts) {
        metrics_.totalScans++;
        metrics_.totalTokens += tokenCount;
        metrics_.totalTime += duration;
        
        for (const auto& [context, count] : contexts) {
            metrics_.contextCounts[context] += count;
        }
    }
    
    std::string generatePerformanceReport() {
        std::ostringstream report;
        
        report << "扫描器性能报告\n";
        report << "==============\n";
        report << "总扫描次数: " << metrics_.totalScans << "\n";
        report << "总标记数: " << metrics_.totalTokens << "\n";
        report << "平均扫描时间: " << (metrics_.totalTime.count() / metrics_.totalScans) << "μs\n";
        report << "缓存命中率: " << (100.0 * metrics_.cacheHits / (metrics_.cacheHits + metrics_.cacheMisses)) << "%\n";
        
        return report.str();
    }
};
```

---

## 最佳实践

### 扫描器使用

1. **合理配置**: 根据需求配置扫描选项
2. **缓存利用**: 充分利用缓存机制
3. **增量更新**: 使用增量扫描提高性能
4. **错误处理**: 优雅处理扫描错误

### 性能优化

1. **避免重复扫描**: 缓存扫描结果
2. **并行处理**: 利用多核并行扫描
3. **内存管理**: 及时清理不需要的缓存
4. **算法优化**: 使用高效的扫描算法

### 调试技巧

1. **启用调试模式**: 查看详细的扫描过程
2. **使用可视化工具**: 可视化扫描结果
3. **性能分析**: 监控扫描器性能
4. **单元测试**: 编写全面的测试用例

---

## 未来发展

### 技术演进

1. **AI辅助扫描**: 使用机器学习优化上下文检测
2. **语义分析**: 集成语义分析能力
3. **错误恢复**: 更智能的错误恢复机制
4. **并行优化**: 进一步提升并行处理能力

### 功能扩展

1. **多语言支持**: 支持更多嵌入语言
2. **实时扫描**: 支持实时编辑扫描
3. **云端扫描**: 支持云端分布式扫描
4. **插件系统**: 可插拔的扫描器扩展

---

统一扫描器是CHTL编译器的核心基础设施，其创新的变量长度切片和上下文感知技术为CHTL的强大功能提供了坚实的基础。通过本指南，开发者可以深入理解扫描器的工作原理，并参与到扫描器的改进和扩展中来。