# CHTL语法文档手写总结

## 基础语法理解

### 1. CHTL本质
- CHTL是基于C++实现的超文本语言
- 目的：提供更符合开发者编写HTML代码的方式
- 协议：MIT开源协议

### 2. 注释系统
- `//` - 普通注释，不会被生成器识别，生成的HTML不会带有
- `/* */` - 多行注释，不会被生成器识别
- `--` - 会被生成器识别的注释

### 3. 文本节点
- 语法：`text { "内容" }` 或 `text { 内容 }`
- 支持无修饰字面量（没有引号的字符串）

### 4. CE对等式
- `:` 与 `=` 完全等价
- 可以在推荐情景下使用 `=`

### 5. 元素节点
- 支持HTML所有元素（单标签、双标签、块级、行内、行内块）
- 语法：`元素名 { 属性和子元素 }`

### 6. 属性
- 语法：`属性名: "属性值";` 或 `属性名: 属性值;`

## 局部样式块

### 1. 基本概念
- 语法：`style { CSS代码 }`
- 允许在元素内部嵌套使用
- 支持：内联样式、类选择器、id选择器、伪类、伪元素
- 类选择器等代码会自动添加至全局样式块

### 2. 内联样式
- 直接在style{}内添加属性
- 成为元素的内联样式

### 3. 自动化类名/id
- 可以直接使用`.box`、`#main`等选择器
- CHTL会自动为元素添加对应的class/id属性

### 4. 上下文推导（&引用）
- `&` 表示类名/id（优先类名）
- 根据上下文推导成类名/id
- 用于简化伪类选择器（`&:hover`）和伪元素选择器（`&::before`）

## 模板系统

### 1. 样式组模板
- 语法：`[Template] @Style 组名 { CSS属性 }`
- 使用：`@Style 组名;`

### 2. 元素模板
- 语法：`[Template] @Element 元素名 { 元素结构 }`
- 使用：`@Element 元素名;`

### 3. 变量组模板
- 语法：`[Template] @Var 变量组名 { 变量定义 }`
- 使用：`变量组名(变量名)`
- 注意：无需CSS的--前缀，是值的替换

### 4. 继承
- 组合式继承：`@Style 模板名;`
- 显性继承：`inherit @Style 模板名;`
- 模板可以继承同种类型的模板

## 自定义系统

### 1. 基本概念
- 自定义是模板的扩展
- 具有极高灵活性和更多扩展操作
- 允许特例化操作
- 与模板可以相互继承（类型匹配）

### 2. 自定义样式组
- 无值样式组：属性不具有值，使用时填入
- 特例化：删除属性、删除样式组继承

### 3. 自定义元素
- 特例化操作：
  - 增加样式
  - 索引访问：`元素[index]`
  - 插入元素：`insert 位置 选择器`（after、before、replace、at top、at bottom）
  - 删除元素：`delete 元素;`、`delete 元素[index];`
  - 删除元素继承：`delete @Element 模板名;`

### 4. 变量组特例化
- 语法：`变量组名(变量名 = 值)`

### 5. 全缀名
- 用于访问模板和自定义内容
- 语法：`[Template] @Element 名称`、`[Custom] @Style 名称`
- 用于处理命名冲突

## 原始嵌入系统

### 1. 基本概念
- 语法：`[Origin] @类型 { 原始代码 }`
- 不会被CHTL处理，生成器直接生成
- 兼容处理机制，避免极端问题
- 允许在任意节点中使用

### 2. 基本类型
- `[Origin] @Html { HTML代码 }`
- `[Origin] @Style { CSS代码 }`
- `[Origin] @JavaScript { JS代码 }`

### 3. 增强功能
- 带名原始嵌入：`[Origin] @Html 名称 { 代码 }`
- 使用：`[Origin] @Html 名称;`

### 4. 自定义类型系统
- 可以创建自定义类型：`[Origin] @Vue { Vue代码 }`
- 必须以@为前缀
- 会隐式创建配置块

## 配置组系统

### 1. 基本配置
- 语法：`[Configuration] { 配置项 }`
- 配置项：
  - `INDEX_INITIAL_COUNT = 0` - 索引起始计数
  - `DISABLE_NAME_GROUP = true` - 是否禁用Name配置组
  - `DISABLE_CUSTOM_ORIGIN_TYPE = false` - 是否禁用自定义原始嵌入类型
  - `DEBUG_MODE = false` - DEBUG模式

### 2. Name配置块
- 定义关键字名称，无需改动源码
- 组选项：一个关键字可以有多个值
- 例如：`CUSTOM_STYLE = [@Style, @style, @CSS, @Css, @css];`

### 3. OriginType配置块
- 定义Origin具有什么自定义类型
- 语法：`ORIGINTYPE_类型名 = @类型名;`

### 4. 命名配置组
- 语法：`[Configuration] @Config 名称 { 配置 }`
- 命名配置组不会被使用
- 不被命名的配置组才会启用
- 服务于导入[Import]

## 导入系统

### 1. Web资源导入
- HTML：`[Import] @Html from "文件路径" as(可选) 命名`
- CSS：`[Import] @Style from "文件路径" as(可选) 命名`
- JS：`[Import] @JavaScript from "文件路径" as(可选) 命名`

### 2. CHTL文件导入
- 整个文件：`[Import] @Chtl from "文件路径"`
- 自定义元素：`[Import] [Custom] @Element 元素名 from "文件路径" as(可选) 命名`
- 自定义样式组：`[Import] [Custom] @Style 样式组名 from "文件路径" as(可选) 命名`
- 自定义变量组：`[Import] [Custom] @Var 变量组名 from "文件路径" as(可选) 命名`
- 模板导入：`[Import] [Template] @Element/Style/Var 名称 from "文件路径" as(可选) 命名`

### 3. 原始嵌入导入
- `[Import] [Origin] @Html 名称 from "文件路径" as(可选) 命名`
- `[Import] [Origin] @Style 名称 from "文件路径" as(可选) 命名`
- `[Import] [Origin] @JavaScript 名称 from "文件路径" as(可选) 命名`

### 4. 批量导入
- 所有模板：`[Import] [Template] from "文件路径"`
- 所有自定义：`[Import] [Custom] from "文件路径"`
- 所有命名原始嵌入：`[Import] [Origin] from "文件路径"`
- 特定类型的所有：`[Import] [Custom] @Element from "文件路径"`

### 5. CJmod导入
- `[Import] @CJmod from "文件路径"`

### 6. 配置导入
- `[Import] @Config 配置组名(可选) from "文件路径"`
- 全缀名：`[Import] [Configuration] @Config 配置组名(可选) from "文件路径"`

## 命名空间系统

### 1. 基本语法
- `[Namespace] 空间名 { 内容 }`
- 如果文件没有命名空间，默认使用文件名作为命名空间

### 2. 使用命名空间
- `@Element 元素名 from 命名空间;`
- 嵌套命名空间：`@Element 元素名 from 空间名.子空间名;`

### 3. 命名空间嵌套
- 支持多层嵌套
- 单层关系可以不写花括号

## 约束系统

### 1. 精确约束
- 作用对象：HTML元素、自定义与模板对象
- 语法：`except 元素名, [Custom] @Element 名称;`

### 2. 类型约束
- 作用对象：@Html、[Custom]、[Template]
- 语法：`except @Html;`、`except [Template] @Var;`、`except [Custom];`

### 3. 全局约束
- 在命名空间内使用
- 语法：`except [Template];`

## CHTL JS系统

### 1. 基本概念
- CHTL JS是CHTL的扩展语法
- **不是JS的超集，也不支持JS的语法**
- JS语法由CHTL内置的JS编译器解析
- CHTL JS语法由CHTL JS编译器解析
- 两者之间并不兼容

### 2. 局部script
- 语法：`script { CHTL JS代码 }`
- 会被添加到具有高优先级的全局script块中

### 3. 增强选择器
- `{{CSS选择器}}` - 创建DOM对象
- 支持的形式：
  - `{{box}}` - 先判断tag，然后查找类名/id=box（id优先）
  - `{{.box}}` - 查找类名为box的元素
  - `{{#box}}` - 查找id为box的元素
  - `{{button}}` - 所有button元素
  - `{{.box button}}` - 类名为box的元素的所有button后代
  - `{{button[0]}}` - 第一个button元素
- **注意**：仅支持上述种类，不支持交集选择器等复杂形式

### 4. 箭头操作符
- `->` 与 `.` 完全等价
- 推荐使用 `->` 明确表示使用了CHTL JS语法
- 支持链式访问

### 5. 增强监听器
- 语法：`{{选择器}}->listen({ 事件映射 })`
- 事件映射：`事件名: 处理函数`
- 支持箭头函数、已存在函数、function函数

### 6. 事件委托
- 语法：`{{父元素选择器}}->delegate({ target: 子元素, 事件映射 })`
- 解决SPA页面动态更新导致事件监听丢失
- 基于事件冒泡机制
- 需要全局注册表管理，避免重复绑定

### 7. 动画系统
- 语法：`animate({ 动画配置 })`
- 配置项：
  - `target` - 目标选择器或DOM对象
  - `duration` - 持续时间（ms）
  - `easing` - 缓动函数
  - `begin` - 起始状态（CSS代码）
  - `when` - 关键帧数组（at: 时间点）
  - `end` - 终止状态（CSS代码）
  - `loop` - 循环次数
  - `direction` - 播放方向
  - `delay` - 延迟（ms）
  - `callback` - 完成回调

### 8. 虚对象系统
- 语法：`vir 对象名 = CHTL JS函数`
- 用于访问CHTL JS函数中键值为函数的键
- 调用：`对象名->键名();`
- 原理：创建特殊命名的全局函数，解析成函数引用
- vir是编译期语法糖，不涉及JS

## CMOD模块系统

### 1. 基本结构
```
模块名/
  src/
    模块名.chtl
    其他.chtl
  info/
    模块名.chtl
```

### 2. 要求
- 模块文件夹、主模块chtl文件、模块信息chtl文件必须同名
- 主模块chtl文件在没有子模块时必须存在
- 有子模块时可以省略主模块文件

### 3. 信息文件
- `[Info] { 模块信息 }`
- `[Export] { 导出列表 }` - 系统生成，优化性能用

### 4. 子模块结构
```
模块名/
  src/
    (模块名.chtl)
    子模块1/
      src/子模块1.chtl
      info/子模块1.chtl
    子模块2/
      src/子模块2.chtl
      info/子模块2.chtl
  info/
    模块名.chtl
```

### 5. 使用方式
- 导入：`[Import] @Chtl from 模块名`
- 精确导入：`[Import] @Chtl from 模块名.子模块`

## CJMOD扩展系统

### 1. 基本概念
- 允许通过C++扩展CHTL JS语法
- 使用CHTL JS编译器接口
- 让创建无限JS语法成为可能

### 2. 文件结构
```
CJMOD文件夹/
  src/xxx.cpp xxx.h
  子模块/
    src/xxx.cpp xxx.h
    info/子模块.chtl
  info/CJMOD文件夹名称.chtl
```

### 3. CMOD + CJMOD混合模块
```
模块名/
  CMOD/
    子模块/
      src/子模块.chtl
      info/子模块.chtl
  CJMOD/
    子模块/
      src/xxx.cpp xxx.h
      info/子模块.chtl
```

### 4. 使用方式
- CMOD：`[Import] @Chtl from 模块名`
- CJMOD：`[Import] @CJmod from 模块名`
- **不推荐@Chtl同时管理CMOD和CJMOD**

## 特殊模块

### 1. Chtholly珂朵莉模块
- 采用CMOD + CJMOD混合模块
- CMOD组件：手风琴、四叶窗相册、备忘录、暖色笔记、樱花雨、鼠标特效、鼠标拖尾、视差滚动背景、右键菜单栏、进度条
- CJMOD功能：printMylove、iNeverAway

### 2. printMylove功能
- 将图片转换成字符像素块或ASCII形式
- 输出到控制台
- 配置：url、mode（ASCII/Pixel）、width、height、scale

### 3. iNeverAway功能
- 创建一组标记函数
- 允许开发者定义键，使用状态区分同名键
- 需要与虚对象共用
- 支持状态重载：`Void<A>`、`Void<B>`、`Void`
- 实现原理：创建JS全局函数，vir对象转换成函数引用

### 4. Yuigahama由比滨结衣模块
- 使用纯CMOD
- 组件：音乐播放器、手风琴、四叶窗相册、备忘录、暖色笔记、樱花雨、鼠标特效

## 关键理解点

### 1. CHTL与CHTL JS完全分离
- CHTL：处理HTML结构、模板、自定义、原始嵌入等
- CHTL JS：处理增强选择器、监听器、事件委托、动画、虚对象等
- 各自拥有独立的Token、GlobalMap、State、Context、Lexer

### 2. 不是JavaScript
- CHTL JS不是JS的超集
- 不支持JS语法（function、const、let、var等）
- 有自己独特的语法体系

### 3. CJMOD边界
- iNeverAway、Void等属于CJMOD扩展
- 不属于CHTL JS核心语法
- printMylove等是CJMOD提供的功能

### 4. 模块化要求
- 导出必须使用全缀名
- 模块结构必须严格按照规范
- CMOD和CJMOD有明确的分工和结构要求

### 5. 路径搜索规则
- 官方模块目录 → 当前目录module文件夹 → 当前目录
- 支持分类结构（CMOD/CJMOD文件夹）和混杂结构
- 不同类型有不同的文件优先级