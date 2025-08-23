# CMOD和CJMOD语法文档逐行分析

## 📖 CMOD完整要求分析（语法文档第1315-1407行）

### 第1315-1318行：CMOD基本定义
```
### CMOD
CMOD是CHTL提供的一种模块化方式  
CHTL编译器 / 打包脚本能够将符合CMOD格式的文件夹打包成.cmod文件  
你需要获取CHTL编译器的源码，CHTL源码中包含了打包脚本，如果你会编译CHTL编译器，那么更推荐你使用CHTL编译器来打包CMOD
```

**分析要求**:
1. ✅ CMOD是模块化方式
2. ✅ 需要打包脚本将文件夹打包成.cmod文件
3. ✅ 打包脚本应该集成在CHTL编译器中

### 第1323-1332行：CMOD标准格式
```
#### CMOD标准格式
Chtholly  
    src  
        Chtholly.chtl  
        Other.chtl  
    info  
        Chtholly.chtl  

src是模块的源码，而info则是存放模块信息的文件夹  
CHTL要求，模块文件夹，主模块chtl文件，模块信息chtl文件必须同名
```

**分析要求**:
1. ✅ **强制命名规则**: 模块文件夹、主模块chtl文件、模块信息chtl文件必须同名
2. ✅ **src文件夹**: 存放模块源码
3. ✅ **info文件夹**: 存放模块信息
4. ✅ **主模块文件**: 与模块文件夹同名的.chtl文件

### 第1334-1337行：主模块文件规则
```
在没有子模块的情况下，主模块chtl文件必须存在  

如果src中存在子模块，则src文件夹内部的主模块chtl文件可以省略(推荐)，也可以保留，保留的意义并不大  
最好是让主模块作为一个框架
```

**分析要求**:
1. ✅ **无子模块**: 主模块chtl文件必须存在
2. ✅ **有子模块**: 主模块chtl文件可以省略（推荐）
3. ✅ **主模块作用**: 作为框架使用

### 第1339-1381行：信息文件格式
```
##### 信息
info文件夹中的chtl文件主要提供两个功能  
提供模块的信息与提供外部查询表  

如下所示  
[Info]
{
    name = "chtholly";
    version = "1.0.0";
    description = "珂朵莉主题模块 - 世界上最幸福的女孩";
    author = "CHTL Team";
    license = "MIT";
    dependencies = "";
    category = "theme";
    minCHTLVersion = "1.0.0";
    maxCHTLVersion = "2.0.0";
}

// 外部查询表，优化性能用，无实际作用
// 不需要写这个，此由系统生成
[Export]
{
    [Custom] @Style ChthollyStyle, ChthollyCard, ChthollyButton,
           ChthollyHeader, ChthollyFooter;

    [Custom] @Element ChthollyPage, ChthollySection, ChthollyNav,
             ChthollyModal, ChthollyGallery;

    [Custom] @Var ChthollyColors, ChthollyFonts, ChthollySpacing,
         ChthollyBreakpoints, ChthollyAnimations;

    [Template] @Style ...;
    [Template] @Element ...;
    [Template] @Var ...;
    [Origin] @Html ...;
    [Origin] @Style ...;
    [Origin] @Javascript ...;
    [Origin] @Vue ...;  // 一样支持导出自定义类型的原始嵌入
    [Configuration] @Config ...;
}
```

**分析要求**:
1. ✅ **[Info]块**: 包含模块基本信息
   - name: 模块名称
   - version: 版本号
   - description: 描述
   - author: 作者
   - license: 许可证
   - dependencies: 依赖
   - category: 分类
   - minCHTLVersion: 最小CHTL版本
   - maxCHTLVersion: 最大CHTL版本

2. ✅ **[Export]块**: 系统自动生成，优化性能用
   - 支持所有类型的导出：[Custom]、[Template]、[Origin]、[Configuration]
   - 必须使用全缀名（如[Custom] @Style）
   - 支持自定义类型的原始嵌入（如@Vue）

### 第1383-1401行：子模块结构
```
#### 子模块
具有子模块的CMOD文件结构并没有太大的改变  
相当于嵌套了一层结构  

Chtholly  
    src  
        (Chtholly.chtl)  
        Other  
            src  
                Other.chtl
            info  
                Other.chtl
        Space  
            src  
                Space.chtl
            info  
                Space.chtl
    info  
        Chtholly.chtl
```

**分析要求**:
1. ✅ **嵌套结构**: 子模块在src内嵌套
2. ✅ **子模块格式**: 每个子模块都有自己的src和info文件夹
3. ✅ **主模块可选**: 有子模块时主模块文件可以省略

### 第1403-1407行：CMOD使用方式
```
#### CMOD的使用
如何导入模块？使用`Import @Chtl from 模块名`即可  
会导入所有的子模块，也可以精确到具体的子模块，例如Import @Chtl from Chtholly.Space  
如何使用其他人开发的Cmod呢？你需要在需要执行的文件处，新建module文件夹，并将Cmod放置文件夹之中
```

**分析要求**:
1. ✅ **导入语法**: `[Import] @Chtl from 模块名`
2. ✅ **批量导入**: 导入所有子模块
3. ✅ **精确导入**: `[Import] @Chtl from Chtholly.Space`
4. ✅ **模块放置**: 在执行文件处新建module文件夹

## 📖 CJMOD完整要求分析（语法文档第1408-1520行）

### 第1408-1411行：CJMOD基本定义
```
### CJMOD
CJMOD是CHTL提供扩展CHTL JS语法的一种方式，允许通过CHTL JS编译器的一些接口使用C++自由实现CHTL JS语法  
让创建无限的JS语法成为一种可能，CJMOD的文件结构与CMOD类似  
CJMOD让扩展CHTL JS语法变得更简单，无需阅读源码即可实现CHTL JS语法的扩展，并通过CJMOD的方式发布
```

**分析要求**:
1. ✅ **扩展CHTL JS语法**: 通过C++实现
2. ✅ **编译器接口**: 使用CHTL JS编译器接口
3. ✅ **无限语法**: 让创建无限的JS语法成为可能
4. ✅ **简化扩展**: 无需阅读源码即可扩展

### 第1413-1424行：CJMOD结构
```
CJmod结构  

CJmod文件夹  
    src/xxx.cpp xxx.h  
        Box  
            src/xxx.cpp xxx.h  
            info/Box.chtl  
        Box2  
            src/xxx.cpp xxx.h  
            info/Box2.chtl  
    info/CJmod文件夹名称.chtl
```

**分析要求**:
1. ✅ **C++源文件**: src/xxx.cpp xxx.h
2. ✅ **子模块结构**: 每个子模块有自己的src和info
3. ✅ **信息文件**: info/子模块名称.chtl
4. ✅ **主信息文件**: info/CJmod文件夹名称.chtl

### 第1425行：CJMOD自由度
```
你无需害怕CJMOD存在局限，你可以自由包含头文件，编写全局变量管理状态
```

**分析要求**:
1. ✅ **完全自由**: 可以自由包含头文件
2. ✅ **状态管理**: 可以编写全局变量管理状态

### 第1427-1453行：CMOD + CJMOD混合模块
```
### CMOD + CJMOD
如果你想要提供组件的同时提供扩展CHTL JS语法  
我们建议使用如下的结构  

模块名称  
    CMOD / Cmod / cmod  
        Box  
            src/Box.chtl, Other.chtl  
            info/Box.chtl  
    CJMOD / CJmod / cjmod  
        Box  
            src/xxx.cpp xxx.h  
            info/Box.chtl  

对于子模块，你只需这样创建  
模块名称  
    CMOD  
        Box  
            Box1  
                src/  
                info/  
            Box2  
                src/  
                info/  

如何使用？例如这个模块叫Box，那么如果我想要调用Box的CMOD模块时，我们直接使用[Import] @Chtl即可  
如果需要使用CJMOD，需要使用[Import] @CJmod，CHTL不会对此进行统一处理，我们不推荐使用@Chtl同时管理CMOD和CJMOD
```

**分析要求**:
1. ✅ **分类结构**: CMOD和CJMOD分别放在不同文件夹
2. ✅ **文件夹命名**: 支持CMOD/Cmod/cmod和CJMOD/CJmod/cjmod
3. ✅ **子模块嵌套**: 支持多层子模块嵌套
4. ✅ **独立导入**: CMOD用@Chtl，CJMOD用@CJmod
5. ✅ **不统一处理**: CHTL不会统一管理CMOD和CJMOD

## 📖 珂朵莉模块分析（语法文档第1455-1520行）

### 第1455-1457行：珂朵莉模块定义
```
### Chtholly 珂朵莉模块
珂朵莉对于我来说，是一个很特别的角色，是我一直喜欢着的人物，我希望我能让珂朵莉成为CHTL中重要的模块  
珂朵莉模块采用CMOD + CJMOD的混合模块
```

**分析要求**:
1. ✅ **特殊地位**: 珂朵莉是CHTL中重要的模块
2. ✅ **混合模块**: 采用CMOD + CJMOD的混合模块

### 第1459-1469行：珂朵莉CMOD组件
```
#### CMOD
##### 手风琴
##### 四叶窗相册
##### 备忘录
##### 暖色笔记
##### 樱花雨
##### 鼠标特效
##### 鼠标拖尾
##### 视差滚动背景
##### 右键菜单栏
##### 进度条
```

**分析要求**:
1. ✅ **10个CMOD组件**: 
   - Accordion（手风琴）
   - CloverAlbum（四叶窗相册）
   - Memo（备忘录）
   - WarmNote（暖色笔记）
   - SakuraRain（樱花雨）
   - MouseEffect（鼠标特效）
   - MouseTrail（鼠标拖尾）
   - ParallaxBg（视差滚动背景）
   - ContextMenu（右键菜单栏）
   - ProgressBar（进度条）

### 第1471-1483行：printMylove功能
```
#### CJMOD
##### printMylove
printMylove可以将一张图片变成字符像素块的形式，你可以使用printMylove来把图片转换成字符像素块或ASCII的形式  
然后输出到控制台  
const str = printMylove({
    url: ,
    mode: ,  // 模式可以选择ASCII或Pixel
    width: ,  // 宽度，支持的单位有CSS单位以及百分比，小数，纯数字(像素)
    height: ,  // 高度
    scale:  ,  // 缩放倍数，限定为等比缩放策略
});
```

**分析要求**:
1. ✅ **功能**: 图片转换成字符像素块或ASCII
2. ✅ **输出**: 输出到控制台
3. ✅ **参数**:
   - url: 图片URL
   - mode: ASCII或Pixel模式
   - width: 宽度（CSS单位、百分比、小数、纯数字像素）
   - height: 高度
   - scale: 缩放倍数（等比缩放）

### 第1485-1520行：iNeverAway功能
```
##### iNeverAway
iNeverAway是一个很特别的功能，从名称上面你完全是理解不到这个功能的实际作用的 iNeverAway用于创建一组标记函数  
iNeverAway与其他CHTL JS功能不同，它允许开发者定义键，而不是使用键，并可以使用状态区分同名的键  
iNeverAway需要与虚对象共用  

vir Test = iNeverAway({
    Void<A>: function(int, int) {

    },

    Void<B>: funtion(int, int) {  // 通过状态同名同参重载

    },

    Void: {

    },

    Ax: {

    }
});

Test->Void<A>();

Test是虚拟对象，是不存在的对象，这里并没有创建一个对象  

实现原理：  
iNeverAway  ->  创建一组JS全局函数，名称由CHTL编译器统一管理，在调用时才生成对应的JS函数代码  
vir对象本身不存在，最终转变成相对应的函数的引用  

iNeverAway函数存在的意义其实很迷惑人，这是因为相对于使用iNeverAway，更多人更喜欢使用普通的函数  
这是当然，毕竟iNeverAway存在的意义本身就不是作为实用功能而存在，然而，iNeverAway其实是CHTL JS的一种新方向  
也是对函数重载的重定义
```

**分析要求**:
1. ✅ **创建标记函数**: 用于创建一组标记函数
2. ✅ **定义键而非使用键**: 与其他CHTL JS功能不同
3. ✅ **状态区分**: 使用状态区分同名键
4. ✅ **与虚对象共用**: 必须与vir配合使用
5. ✅ **状态重载**: 支持Void<A>、Void<B>等状态重载
6. ✅ **实现原理**:
   - 创建JS全局函数
   - 名称由CHTL编译器统一管理
   - 调用时才生成对应JS函数代码
   - vir对象不存在，转变成函数引用
7. ✅ **函数重载重定义**: iNeverAway是CHTL JS的新方向

## 📖 由比滨结衣模块分析（语法文档第1521-1531行）

### 第1521-1522行：模块定义
```
### Yuigahama 由比滨结衣模块
由比滨结衣模块使用CMOD
```

**分析要求**:
1. ✅ **纯CMOD模块**: 只使用CMOD，不使用CJMOD

### 第1524-1531行：由比滨结衣CMOD组件
```
#### 音乐播放器
#### 手风琴
#### 四叶窗相册
#### 备忘录
#### 暖色笔记
#### 樱花雨
#### 鼠标特效
```

**分析要求**:
1. ✅ **7个CMOD组件**:
   - MusicPlayer（音乐播放器）
   - YuiAccordion（手风琴）
   - YuiCloverAlbum（四叶窗相册）
   - YuiMemo（备忘录）
   - YuiWarmNote（暖色笔记）
   - YuiSakuraRain（樱花雨）
   - YuiMouseEffect（鼠标特效）

## 🎯 完整实现要求总结

### CMOD系统要求
1. **模块结构管理**:
   - 标准格式验证（src/info文件夹，同名规则）
   - 子模块嵌套支持
   - 主模块文件可选性管理

2. **信息文件处理**:
   - [Info]块解析和验证
   - [Export]块自动生成
   - 版本兼容性检查

3. **打包和加载**:
   - 文件夹打包成.cmod文件
   - .cmod文件加载和解析
   - 模块依赖管理

4. **导入和使用**:
   - `[Import] @Chtl from 模块名`支持
   - 子模块精确导入
   - module文件夹管理

### CJMOD系统要求
1. **C++扩展接口**:
   - CHTL JS编译器接口
   - C++自由实现CHTL JS语法
   - 头文件和全局变量支持

2. **模块结构管理**:
   - C++源文件管理（.cpp .h）
   - 子模块嵌套支持
   - 信息文件管理

3. **语法扩展能力**:
   - 动态语法扩展
   - 无限JS语法创建
   - 编译时集成

4. **特殊功能实现**:
   - printMylove：图片转ASCII/像素块
   - iNeverAway：状态重载函数系统

### 混合模块要求（珂朵莉）
1. **CMOD/CJMOD分类结构**:
   ```
   Chtholly/
     CMOD/
       Accordion/src/info/
       CloverAlbum/src/info/
       ...
     CJMOD/
       printMylove/src/info/
       iNeverAway/src/info/
   ```

2. **独立导入管理**:
   - CMOD: `[Import] @Chtl from Chtholly`
   - CJMOD: `[Import] @CJmod from Chtholly`

### 纯CMOD模块要求（由比滨结衣）
1. **纯CMOD结构**:
   ```
   Yuigahama/
     src/
       MusicPlayer/src/info/
       YuiAccordion/src/info/
       ...
     info/
       Yuigahama.chtl
   ```

2. **CMOD导入**:
   - `[Import] @Chtl from Yuigahama`

---

**基于语法文档的逐行分析已完成，现在开始完整实现CMOD和CJMOD系统。**