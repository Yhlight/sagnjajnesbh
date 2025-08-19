# 🚀 CJMOD快速入门 - 5分钟创建你的第一个语法扩展

## 🎯 **目标：创建一个简单的颜色选择器语法扩展**

我们要创建一个新的CHTL JS语法：`colorPicker({{#element}}, callback)`

### 步骤1：创建项目 (30秒)

```bash
# 使用CJMOD创建工具
./tools/create_cjmod ColorPicker ./my_extensions

# 进入项目目录
cd my_extensions/ColorPicker
```

### 步骤2：编写C++语法扩展 (2分钟)

编辑 `src/main.cpp`：

```cpp
#include <string>
#include <regex>
#include <sstream>

// 处理colorPicker语法
std::string processColorPickerSyntax(const std::string& chtljs_code) {
    // 匹配: colorPicker({{#element}}, callback)
    std::regex colorRegex(R"(colorPicker\s*\(\s*\{\{([^}]+)\}\},\s*(.*?)\s*\))");
    std::smatch matches;
    
    if (!std::regex_search(chtljs_code, matches, colorRegex)) {
        return "";
    }
    
    std::string selector = matches[1].str();
    std::string callback = matches[2].str();
    
    // 生成JavaScript代码
    std::stringstream js;
    js << "(() => {\n";
    js << "    const element = document.querySelector('" << selector << "');\n";
    js << "    if (!element) return;\n";
    js << "    \n";
    js << "    // 创建颜色选择器\n";
    js << "    const colorInput = document.createElement('input');\n";
    js << "    colorInput.type = 'color';\n";
    js << "    colorInput.value = '#3498db';\n";
    js << "    colorInput.style.cssText = 'margin: 5px; cursor: pointer;';\n";
    js << "    \n";
    js << "    // 添加到页面\n";
    js << "    element.appendChild(colorInput);\n";
    js << "    \n";
    js << "    // 监听颜色变化\n";
    js << "    colorInput.addEventListener('change', function(e) {\n";
    js << "        const selectedColor = e.target.value;\n";
    js << "        console.log('🎨 颜色选择:', selectedColor);\n";
    js << "        \n";
    js << "        // 调用回调函数\n";
    js << "        (" << callback << ")(selectedColor, element);\n";
    js << "    });\n";
    js << "    \n";
    js << "    console.log('🎨 颜色选择器已创建');\n";
    js << "    return colorInput;\n";
    js << "})()";
    
    return js.str();
}

// CJMOD入口函数
extern "C" std::string processColorPickerSyntax(const std::string& chtljs_code) {
    std::string result = processColorPickerSyntax(chtljs_code);
    return result.empty() ? chtljs_code : result;
}
```

### 步骤3：编译CJMOD (30秒)

```bash
# 运行编译脚本
./build.sh

# 看到这个就成功了：
# ✅ 编译成功！
# 📦 CJMOD文件已生成: ColorPicker.cjmod
```

### 步骤4：使用你的扩展 (1分钟)

创建 `test.chtl`：

```chtl
html {
    head {
        title { text { 颜色选择器测试 } }
    }
    
    body {
        div {
            h1 { text { 🎨 我的颜色选择器扩展 } }
            
            div {
                id: color-demo;
                text { 选择颜色: }
                style {
                    padding: 20px;
                    border: 2px solid #ddd;
                    border-radius: 10px;
                    margin: 20px 0;
                }
            }
            
            div {
                id: result;
                text { 选择的颜色会显示在这里 }
                style {
                    padding: 15px;
                    background: #f5f5f5;
                    border-radius: 5px;
                }
            }
            
            script {
                // 🚀 使用你创建的新语法！
                colorPicker({{#color-demo}}, function(color, element) {
                    // 当用户选择颜色时执行
                    {{#result}}->textContent = '选择的颜色: ' + color;
                    {{#result}}->style.backgroundColor = color;
                    {{#result}}->style.color = color === '#000000' ? 'white' : 'black';
                    
                    // 给选择的元素也添加颜色
                    element->style.borderColor = color;
                });
                
                console.log('🎉 颜色选择器扩展加载完成！');
            }
        }
    }
}
```

### 步骤5：编译和运行 (30秒)

```bash
# 1. 将CJMOD复制到module目录
mkdir -p module
cp ColorPicker.cjmod module/

# 2. 编译CHTL文件
chtl compile test.chtl

# 3. 打开生成的HTML文件
# 你会看到一个工作的颜色选择器！
```

## 🔥 **编译过程详解**

### CHTL JS代码：
```chtl
colorPicker({{#color-demo}}, function(color, element) {
    {{#result}}->textContent = '选择的颜色: ' + color;
});
```

### CHTL编译器处理：
1. **识别语法** - 发现 `colorPicker()` 不是标准语法
2. **加载CJMOD** - 从 `module/ColorPicker.cjmod` 加载扩展
3. **调用C++函数** - 调用 `processColorPickerSyntax()`
4. **获得JavaScript** - C++返回生成的JS代码
5. **插入输出** - 将JS代码插入到最终HTML

### 最终生成的JavaScript：
```javascript
(() => {
    const element = document.querySelector('#color-demo');
    if (!element) return;
    
    // 创建颜色选择器
    const colorInput = document.createElement('input');
    colorInput.type = 'color';
    colorInput.value = '#3498db';
    colorInput.style.cssText = 'margin: 5px; cursor: pointer;';
    
    // 添加到页面
    element.appendChild(colorInput);
    
    // 监听颜色变化
    colorInput.addEventListener('change', function(e) {
        const selectedColor = e.target.value;
        console.log('🎨 颜色选择:', selectedColor);
        
        // 调用回调函数
        (function(color, element) {
            document.getElementById('result').textContent = '选择的颜色: ' + color;
        })(selectedColor, element);
    });
    
    console.log('🎨 颜色选择器已创建');
    return colorInput;
})()
```

## 💡 **更多实际例子**

### 创建一个图表扩展：

```cpp
// C++代码
std::string processChartSyntax(const std::string& chtljs_code) {
    // 匹配: chart({{#container}}, {type: 'bar', data: [1,2,3]})
    // 生成: Chart.js的完整初始化代码
}
```

```chtl
// CHTL JS使用
script {
    chart({{#chart-container}}, {
        type: 'bar',
        data: [10, 20, 30, 40],
        labels: ['A', 'B', 'C', 'D']
    });
}
```

### 创建一个地图扩展：

```cpp
// C++代码  
std::string processMapSyntax(const std::string& chtljs_code) {
    // 匹配: map({{#map}}, {center: [lat, lng], zoom: 10})
    // 生成: Google Maps或Leaflet的初始化代码
}
```

```chtl
// CHTL JS使用
script {
    map({{#map-container}}, {
        center: [39.9042, 116.4074],  // 北京
        zoom: 12,
        markers: [{lat: 39.9042, lng: 116.4074, title: '北京'}]
    });
}
```

## 🎊 **CJMOD的实际价值**

1. **简化复杂功能** - 一行CHTL JS = 几十行JavaScript
2. **提高开发效率** - 不需要重复写复杂的JS代码
3. **减少错误** - C++处理复杂逻辑，生成可靠的JS代码
4. **统一API风格** - 所有扩展都有一致的使用方式
5. **社区生态** - 开发者可以分享自己的语法扩展

**这就是CJMOD的实际创建和使用方式！** 🌟