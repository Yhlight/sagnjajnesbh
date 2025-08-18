// JavaScript Lexer Grammar
lexer grammar JavaScriptLexer;

// Literals
NULL                      : 'null';
BooleanLiteral            : 'true' | 'false';

// Numeric Literals
DecimalLiteral            : DecimalIntegerLiteral '.' [0-9]* ExponentPart?
                          | '.' [0-9]+ ExponentPart?
                          | DecimalIntegerLiteral ExponentPart?
                          ;

HexIntegerLiteral         : '0' [xX] [0-9a-fA-F]+;
OctalIntegerLiteral       : '0' [0-7]+;

fragment DecimalIntegerLiteral
                          : '0'
                          | [1-9] [0-9]*
                          ;

fragment ExponentPart     : [eE] [+-]? [0-9]+;

// String Literals
StringLiteral             : '"' DoubleStringCharacter* '"'
                          | '\'' SingleStringCharacter* '\''
                          ;

fragment DoubleStringCharacter
                          : ~["\\\r\n]
                          | '\\' EscapeSequence
                          | LineContinuation
                          ;

fragment SingleStringCharacter
                          : ~['\\\r\n]
                          | '\\' EscapeSequence
                          | LineContinuation
                          ;

fragment EscapeSequence   : CharacterEscapeSequence
                          | '0' 
                          | HexEscapeSequence
                          | UnicodeEscapeSequence
                          ;

fragment CharacterEscapeSequence
                          : SingleEscapeCharacter
                          | NonEscapeCharacter
                          ;

fragment HexEscapeSequence: 'x' [0-9a-fA-F] [0-9a-fA-F];

fragment UnicodeEscapeSequence
                          : 'u' [0-9a-fA-F] [0-9a-fA-F] [0-9a-fA-F] [0-9a-fA-F];

fragment SingleEscapeCharacter
                          : ['"\\bfnrtv];

fragment NonEscapeCharacter
                          : ~['"\\bfnrtv0-9xu\r\n];

fragment LineContinuation : '\\' [\r\n\u2028\u2029];

// Regular Expression Literals
RegularExpressionLiteral  : '/' RegularExpressionBody '/' RegularExpressionFlags;

fragment RegularExpressionBody
                          : RegularExpressionFirstChar RegularExpressionChar*;

fragment RegularExpressionFirstChar
                          : ~[*\r\n\u2028\u2029\\/[]
                          | RegularExpressionBackslashSequence
                          | RegularExpressionClass
                          ;

fragment RegularExpressionChar
                          : ~[\r\n\u2028\u2029\\/[]
                          | RegularExpressionBackslashSequence
                          | RegularExpressionClass
                          ;

fragment RegularExpressionClassChar
                          : ~[\r\n\u2028\u2029\\\]]
                          | RegularExpressionBackslashSequence
                          ;

fragment RegularExpressionClass
                          : '[' RegularExpressionClassChar* ']';

fragment RegularExpressionBackslashSequence
                          : '\\' ~[\r\n\u2028\u2029];

fragment RegularExpressionFlags
                          : [gimsuxy]*;

// Keywords
BREAK                     : 'break';
DO                        : 'do';
INSTANCEOF                : 'instanceof';
TYPEOF                    : 'typeof';
CASE                      : 'case';
ELSE                      : 'else';
NEW                       : 'new';
VAR                       : 'var';
CATCH                     : 'catch';
FINALLY                   : 'finally';
RETURN                    : 'return';
VOID                      : 'void';
CONTINUE                  : 'continue';
FOR                       : 'for';
SWITCH                    : 'switch';
WHILE                     : 'while';
DEBUGGER                  : 'debugger';
FUNCTION                  : 'function';
THIS                      : 'this';
WITH                      : 'with';
DEFAULT                   : 'default';
IF                        : 'if';
THROW                     : 'throw';
DELETE                    : 'delete';
IN                        : 'in';
TRY                       : 'try';
GET                       : 'get';
SET                       : 'set';

// Punctuators
LBRACE                    : '{';
RBRACE                    : '}';
LPAREN                    : '(';
RPAREN                    : ')';
LBRACKET                  : '[';
RBRACKET                  : ']';
SEMICOLON                 : ';';
COMMA                     : ',';
DOT                       : '.';
QUESTION                  : '?';
COLON                     : ':';
PLUS_PLUS                 : '++';
MINUS_MINUS               : '--';
PLUS                      : '+';
MINUS                     : '-';
MULTIPLY                  : '*';
DIVIDE                    : '/';
MODULO                    : '%';
LSHIFT                    : '<<';
RSHIFT                    : '>>';
URSHIFT                   : '>>>';
LT                        : '<';
GT                        : '>';
LE                        : '<=';
GE                        : '>=';
EQ                        : '==';
NE                        : '!=';
STRICT_EQ                 : '===';
STRICT_NE                 : '!==';
BIT_AND                   : '&';
BIT_XOR                   : '^';
BIT_OR                    : '|';
AND                       : '&&';
OR                        : '||';
ASSIGN                    : '=';
PLUS_ASSIGN               : '+=';
MINUS_ASSIGN              : '-=';
MULTIPLY_ASSIGN           : '*=';
DIVIDE_ASSIGN             : '/=';
MODULO_ASSIGN             : '%=';
LSHIFT_ASSIGN             : '<<=';
RSHIFT_ASSIGN             : '>>=';
URSHIFT_ASSIGN            : '>>>=';
BIT_AND_ASSIGN            : '&=';
BIT_XOR_ASSIGN            : '^=';
BIT_OR_ASSIGN             : '|=';
TILDE                     : '~';
NOT                       : '!';

// Identifiers
Identifier                : IdentifierStart IdentifierPart*;

fragment IdentifierStart  : [a-zA-Z$_]
                          | '\\' UnicodeEscapeSequence
                          ;

fragment IdentifierPart   : IdentifierStart
                          | [0-9]
                          ;

// White Space
WhiteSpaces               : [\t\u000B\u000C\u0020\u00A0]+ -> skip;

// Line Terminators
LineTerminator            : [\r\n\u2028\u2029] -> skip;

// Comments
MultiLineComment          : '/*' .*? '*/' -> skip;
SingleLineComment         : '//' ~[\r\n\u2028\u2029]* -> skip;