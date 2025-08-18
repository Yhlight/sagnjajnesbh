// JavaScript Lexer Grammar - Based on official ANTLR grammars-v4
// Adapted for CHTL project requirements
// Simplified for basic JavaScript parsing needs

lexer grammar JavaScriptLexer;

channels {
    ERROR
}

// Comments
MultiLineComment  : '/*' .*? '*/'             -> channel(HIDDEN);
SingleLineComment : '//' ~[\r\n\u2028\u2029]* -> channel(HIDDEN);

// Literals
BooleanLiteral: 'true' | 'false';
DecimalLiteral: DecimalIntegerLiteral '.' [0-9]* ExponentPart?
              | '.' [0-9]+ ExponentPart?
              | DecimalIntegerLiteral ExponentPart?;

HexIntegerLiteral: '0' [xX] [0-9a-fA-F]+;
OctalIntegerLiteral: '0' [0-7]+;
OctalIntegerLiteral2: '0' [oO] [0-7]+;
BinaryIntegerLiteral: '0' [bB] [01]+;

// String literals
StringLiteral: ('"' DoubleStringCharacter* '"'
             | '\'' SingleStringCharacter* '\'')
             {this.ProcessStringLiteral()};

// Template literals
TemplateStringLiteral: '`' ('\\' TemplateStringEscapeSequence | '${' | TemplateStringCharacter)* '`';

// Regular expression literals
RegularExpressionLiteral: '/' RegularExpressionFirstChar RegularExpressionChar* '/' [gimsuvy]*;

// Identifiers
Identifier: IdentifierStart IdentifierPart*;

// Keywords
Break: 'break';
Do: 'do';
Instanceof: 'instanceof';
Typeof: 'typeof';
Case: 'case';
Else: 'else';
New: 'new';
Var: 'var';
Catch: 'catch';
Finally: 'finally';
Return: 'return';
Void: 'void';
Continue: 'continue';
For: 'for';
Switch: 'switch';
While: 'while';
Debugger: 'debugger';
Function: 'function';
This: 'this';
With: 'with';
Default: 'default';
If: 'if';
Throw: 'throw';
Delete: 'delete';
In: 'in';
Try: 'try';

// Future reserved words
Class: 'class';
Enum: 'enum';
Extends: 'extends';
Super: 'super';
Const: 'const';
Export: 'export';
Import: 'import';

// Null and undefined
NullLiteral: 'null';
UndefinedLiteral: 'undefined';

// Punctuators
OpenBracket: '[';
CloseBracket: ']';
OpenParen: '(';
CloseParen: ')';
OpenBrace: '{';
CloseBrace: '}';
SemiColon: ';';
Comma: ',';
Assign: '=';
QuestionMark: '?';
Colon: ':';
Ellipsis: '...';
Dot: '.';
PlusPlus: '++';
MinusMinus: '--';
Plus: '+';
Minus: '-';
BitNot: '~';
Not: '!';
Multiply: '*';
Divide: '/';
Modulus: '%';
RightShiftArithmetic: '>>';
LeftShiftArithmetic: '<<';
RightShiftLogical: '>>>';
LessThan: '<';
MoreThan: '>';
LessThanEquals: '<=';
GreaterThanEquals: '>=';
Equals_: '==';
NotEquals: '!=';
IdentityEquals: '===';
IdentityNotEquals: '!==';
BitAnd: '&';
BitXOr: '^';
BitOr: '|';
And: '&&';
Or: '||';
MultiplyAssign: '*=';
DivideAssign: '/=';
ModulusAssign: '%=';
PlusAssign: '+=';
MinusAssign: '-=';
LeftShiftArithmeticAssign: '<<=';
RightShiftArithmeticAssign: '>>=';
RightShiftLogicalAssign: '>>>=';
BitAndAssign: '&=';
BitXorAssign: '^=';
BitOrAssign: '|=';
Arrow: '=>';

// Whitespace
WhiteSpaces: [\t\u000B\u000C\u0020\u00A0]+ -> channel(HIDDEN);
LineTerminator: [\r\n\u2028\u2029] -> channel(HIDDEN);

// Fragments
fragment DoubleStringCharacter: ~["\\\r\n] | '\\' EscapeSequence;
fragment SingleStringCharacter: ~['\\\r\n] | '\\' EscapeSequence;
fragment EscapeSequence: CharacterEscapeSequence | '0' | HexEscapeSequence | UnicodeEscapeSequence;
fragment CharacterEscapeSequence: SingleEscapeCharacter | NonEscapeCharacter;
fragment SingleEscapeCharacter: ['"\\bfnrtv];
fragment NonEscapeCharacter: ~['"\\bfnrtv0-9xu\r\n];
fragment EscapeCharacter: SingleEscapeCharacter | [0-9] | [xu];
fragment HexEscapeSequence: 'x' [0-9a-fA-F] [0-9a-fA-F];
fragment UnicodeEscapeSequence: 'u' [0-9a-fA-F] [0-9a-fA-F] [0-9a-fA-F] [0-9a-fA-F];
fragment DecimalIntegerLiteral: '0' | [1-9] [0-9]*;
fragment ExponentPart: [eE] [+-]? [0-9]+;

fragment TemplateStringEscapeSequence: '`' | '\\' | '$';
fragment TemplateStringCharacter: ~['`\\$] | '$' ~['{'];

fragment RegularExpressionFirstChar: ~[*\r\n\u2028\u2029\\/[] | RegularExpressionBackslashSequence | RegularExpressionClass;
fragment RegularExpressionChar: ~[\r\n\u2028\u2029\\/[] | RegularExpressionBackslashSequence | RegularExpressionClass;
fragment RegularExpressionBackslashSequence: '\\' ~[\r\n\u2028\u2029];
fragment RegularExpressionClass: '[' RegularExpressionClassChar* ']';
fragment RegularExpressionClassChar: ~[\r\n\u2028\u2029\\\]] | RegularExpressionBackslashSequence;

fragment IdentifierStart: [a-zA-Z$_] | UnicodeEscapeSequence;
fragment IdentifierPart: IdentifierStart | [0-9];