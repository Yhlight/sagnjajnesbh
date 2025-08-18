// CHTL JavaScript Lexer Grammar
// Based on official ANTLR grammars-v4 JavaScript grammar
// Simplified for CHTL compiler integration

lexer grammar JavaScriptLexer;

channels {
    ERROR
}

MultiLineComment  : '/*' .*? '*/'             -> channel(HIDDEN);
SingleLineComment : '//' ~[\r\n\u2028\u2029]* -> channel(HIDDEN);
RegularExpressionLiteral: '/' RegularExpressionBody '/' RegularExpressionFlags;

// Keywords
Break      : 'break';
Do         : 'do';
Instanceof : 'instanceof';
Typeof     : 'typeof';
Case       : 'case';
Else       : 'else';
New        : 'new';
Var        : 'var';
Catch      : 'catch';
Finally    : 'finally';
Return     : 'return';
Void       : 'void';
Continue   : 'continue';
For        : 'for';
Switch     : 'switch';
While      : 'while';
Debugger   : 'debugger';
Function   : 'function';
This       : 'this';
With       : 'with';
Default    : 'default';
If         : 'if';
Throw      : 'throw';
Delete     : 'delete';
In         : 'in';
Try        : 'try';
Class      : 'class';
Enum       : 'enum';
Extends    : 'extends';
Super      : 'super';
Const      : 'const';
Export     : 'export';
Import     : 'import';
Async      : 'async';
Await      : 'await';
Yield      : 'yield';
Let        : 'let';
Static     : 'static';

// Future reserved words
Implements : 'implements';
Interface  : 'interface';
Package    : 'package';
Private    : 'private';
Protected  : 'protected';
Public     : 'public';

// Literals
NullLiteral     : 'null';
BooleanLiteral  : 'true' | 'false';
DecimalLiteral  : DecimalIntegerLiteral '.' DecimalDigit* ExponentPart?
                | '.' DecimalDigit+ ExponentPart?
                | DecimalIntegerLiteral ExponentPart?;

HexIntegerLiteral : '0' [xX] HexDigit+;
OctalIntegerLiteral : '0' [0-7]+;
OctalIntegerLiteral2 : '0' [oO] [0-7]+;
BinaryIntegerLiteral : '0' [bB] [01]+;

// Punctuators
OpenBracket                : '[';
CloseBracket               : ']';
OpenParen                  : '(';
CloseParen                 : ')';
OpenBrace                  : '{';
CloseBrace                 : '}';
SemiColon                  : ';';
Comma                      : ',';
Assign                     : '=';
QuestionMark               : '?';
Colon                      : ':';
Ellipsis                   : '...';
Dot                        : '.';
PlusPlus                   : '++';
MinusMinus                 : '--';
Plus                       : '+';
Minus                      : '-';
BitNot                     : '~';
Not                        : '!';
Multiply                   : '*';
Divide                     : '/';
Modulus                    : '%';
Power                      : '**';
NullCoalesce               : '??';
Hashtag                    : '#';
RightShiftArithmetic       : '>>';
LeftShiftArithmetic        : '<<';
RightShiftLogical          : '>>>';
LessThan                   : '<';
MoreThan                   : '>';
LessThanEquals             : '<=';
GreaterThanEquals          : '>=';
Equals_                    : '==';
NotEquals                  : '!=';
IdentityEquals             : '===';
IdentityNotEquals          : '!==';
BitAnd                     : '&';
BitXOr                     : '^';
BitOr                      : '|';
And                        : '&&';
Or                         : '||';
MultiplyAssign             : '*=';
DivideAssign               : '/=';
ModulusAssign              : '%=';
PlusAssign                 : '+=';
MinusAssign                : '-=';
LeftShiftArithmeticAssign  : '<<=';
RightShiftArithmeticAssign : '>>=';
RightShiftLogicalAssign    : '>>>=';
BitAndAssign               : '&=';
BitXorAssign               : '^=';
BitOrAssign                : '|=';
PowerAssign                : '**=';

// Identifier
Identifier: IdentifierStart IdentifierPart*;

// String literals
StringLiteral: '"' DoubleStringCharacter* '"'
             | '\'' SingleStringCharacter* '\'';

// Template literals
TemplateStringLiteral: '`' ('\\' TemplateStringEscape | '${' | TemplateStringCharacter)* '`';

WhiteSpaces: [\t\u000B\u000C\u0020\u00A0\u1680\u180E\u2000-\u200A\u202F\u205F\u3000\uFEFF]+ -> channel(HIDDEN);

LineTerminator: [\r\n\u2028\u2029] -> channel(HIDDEN);

// Fragment rules
fragment DoubleStringCharacter: ~["\\\r\n] | '\\' EscapeSequence;
fragment SingleStringCharacter: ~['\\\r\n] | '\\' EscapeSequence;
fragment EscapeSequence: CharacterEscapeSequence | '0' | HexEscapeSequence | UnicodeEscapeSequence;
fragment CharacterEscapeSequence: SingleEscapeCharacter | NonEscapeCharacter;
fragment HexEscapeSequence: 'x' HexDigit HexDigit;
fragment UnicodeEscapeSequence: 'u' HexDigit HexDigit HexDigit HexDigit | 'u' '{' HexDigit HexDigit? HexDigit? HexDigit? HexDigit? HexDigit? '}';
fragment SingleEscapeCharacter: ['"\\bfnrtv];
fragment NonEscapeCharacter: ~['"\\bfnrtv0-9xu\r\n];
fragment EscapeCharacter: SingleEscapeCharacter | DecimalDigit | [xu];
fragment DecimalDigit: [0-9];
fragment HexDigit: [0-9a-fA-F];
fragment DecimalIntegerLiteral: '0' | [1-9] DecimalDigit*;
fragment ExponentPart: [eE] [+-]? DecimalDigit+;

fragment RegularExpressionBody: RegularExpressionFirstChar RegularExpressionChar*;
fragment RegularExpressionFirstChar: ~[*\r\n\u2028\u2029\\/[] | RegularExpressionBackslashSequence | '[' RegularExpressionClassChar* ']';
fragment RegularExpressionChar: ~[\r\n\u2028\u2029\\/[] | RegularExpressionBackslashSequence | '[' RegularExpressionClassChar* ']';
fragment RegularExpressionClassChar: ~[\r\n\u2028\u2029\\\]];
fragment RegularExpressionBackslashSequence: '\\' ~[\r\n\u2028\u2029];
fragment RegularExpressionFlags: [gimsuvy]*;

fragment TemplateStringEscape: EscapeSequence;
fragment TemplateStringCharacter: ~['\\`$] | '$' ~'{';

fragment IdentifierStart: [a-zA-Z$_] | UnicodeEscapeSequence;
fragment IdentifierPart: IdentifierStart | DecimalDigit;