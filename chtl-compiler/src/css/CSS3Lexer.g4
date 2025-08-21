lexer grammar CSS3Lexer;

// 基于 https://github.com/antlr/grammars-v4/tree/master/css3
// 简化版本，用于CHTL编译器

// 跳过空白和注释
Space         : [ \t\r\n\f]+ -> skip;
Comment       : '/*' .*? '*/' -> skip;

// 字符串
String        : '"' (~["\r\n])* '"'
              | '\'' (~['\r\n])* '\''
              ;

// 数字和单位
Number        : [0-9]+ ('.' [0-9]+)?;
Percentage    : Number '%';
Dimension     : Number Ident;

// 颜色
Hash          : '#' [0-9a-fA-F]+;

// 标识符
Ident         : '-'? IdentStart IdentPart*;
fragment IdentStart : [a-zA-Z_] | NonAscii | Escape;
fragment IdentPart  : [a-zA-Z0-9_-] | NonAscii | Escape;
fragment NonAscii   : ~[\u0000-\u007F];
fragment Escape     : '\\' (~[\r\n\f] | '\r\n' | '\r' | '\n' | '\f');

// 函数
Function      : Ident '(';

// URI
Uri           : 'url(' (String | (~[)'"\r\n])* ) ')';

// 操作符和分隔符
Plus          : '+';
Minus         : '-';
Star          : '*';
Slash         : '/';
Comma         : ',';
Dot           : '.';
Colon         : ':';
SemiColon     : ';';
LParen        : '(';
RParen        : ')';
LBrace        : '{';
RBrace        : '}';
LBracket      : '[';
RBracket      : ']';

// 组合器
Greater       : '>';
Plus2         : '+' -> type(Plus);
Tilde         : '~';

// 属性选择器操作符
PrefixMatch   : '^=';
SuffixMatch   : '$=';
SubstringMatch: '*=';
DashMatch     : '|=';
IncludeMatch  : '~=';

// 伪类/伪元素
PseudoDouble  : '::';
PseudoSingle  : ':';

// 重要性
Important     : '!' Space* 'important';

// At规则
AtKeyword     : '@' Ident;

// Unicode范围
UnicodeRange  : 'U+' [0-9a-fA-F?]+ ('-' [0-9a-fA-F]+)?;