// 这是一个简化的CHTLJSParser清理说明文件
// 需要从CHTLJSParser.cpp中移除以下错误的Token引用：

/*
错误的Token（不是CHTL JS的内容）：
- TokenType::I_NEVER_AWAY
- TokenType::FUNCTION  
- TokenType::CONST
- TokenType::LET
- TokenType::VAR
- TokenType::VOID
- TokenType::VOID_STATE

这些Token应该从以下位置移除：
1. ParseStatement方法中的case语句
2. ParseINeverAwayBlock方法（整个方法应该简化）
3. ParseVariableDeclaration方法（整个方法应该简化）
4. ParseFunctionDefinition方法（整个方法应该简化）
5. SkipToNextStatement方法中的case语句
6. 任何其他引用这些Token的地方

修正原则：
- 这些是JavaScript的关键字，不是CHTL JS的内容
- iNeverAway是CJMOD扩展功能，不是CHTL JS核心语法
- CHTL JS应该有自己独特的语法，不应该包含这些JavaScript关键字
*/

// 清理完成后，CHTL JS应该只保留真正的CHTL JS语法：
// - vir (虚对象关键字)
// - listen (监听器关键字)  
// - delegate (事件委托关键字)
// - animate (动画关键字)
// - 增强选择器 {{}}
// - 箭头操作符 ->
// - 其他CHTL JS特有的语法元素