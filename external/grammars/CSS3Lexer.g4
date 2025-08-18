// CSS3 Lexer Grammar - Based on official ANTLR grammars-v4
// Adapted for CHTL project requirements
// Avoids problematic quantifier syntax that causes C++ generation issues

lexer grammar CSS3Lexer;

channels {
    ERROR
}

// Basic punctuation
OpenBracket  : '[';
CloseBracket : ']';
OpenParen    : '(';
CloseParen   : ')';
OpenBrace    : '{';
CloseBrace   : '}';
SemiColon    : ';';
Equal        : '=';
Colon        : ':';
Dot          : '.';
Comma        : ',';
Multiply     : '*';
Divide       : '/';
Plus         : '+';
Minus        : '-';
Greater      : '>';
Tilde        : '~';
Pipe         : '|';
Underscore   : '_';
Hash         : '#' Name;

// Fragments
fragment At: '@';
fragment Hex: [0-9a-fA-F];
fragment NewlineOrSpace: '\r\n' | [ \t\r\n\f] |;

// Fixed Unicode rule - expanded to avoid {1,6} quantifier issue
fragment Unicode: '\\' Hex Hex? Hex? Hex? Hex? Hex? NewlineOrSpace;
fragment Escape: Unicode | '\\' ~[\r\n\f0-9a-fA-F];
fragment Nmstart: [_a-zA-Z] | Nonascii | Escape;
fragment Nmchar: [_a-zA-Z0-9\-] | Nonascii | Escape;
fragment Name: Nmchar+;
fragment Nonascii: ~[\u0000-\u007F];

// Whitespace
Whitespace: [ \t\r\n\f]+;
Comment: '/*' .*? '*/';

// Keywords
CHARSET: C H A R S E T;
IMPORT: I M P O R T;
NAMESPACE: N A M E S P A C E;
MEDIA: M E D I A;
SUPPORTS: S U P P O R T S;
PAGE: P A G E;
KEYFRAMES: K E Y F R A M E S;
FONT_FACE: F O N T '-' F A C E;

// Identifiers and values
IDENT: Nmstart Nmchar*;
FUNCTION: IDENT '(';
AT_KEYWORD: At IDENT;
STRING: '"' (~["\\\r\n\f] | Escape)* '"' | '\'' (~['\\\r\n\f] | Escape)* '\'';
NUMBER: [0-9]+ ('.' [0-9]+)?;
PERCENTAGE: NUMBER '%';
DIMENSION: NUMBER IDENT;
URI: U R L '(' Whitespace? (STRING | (~[ \t\r\n\f'")\\] | Escape)*) Whitespace? ')';
UNICODE_RANGE: U '+' (Hex | '?')+;
CDO: '<!--';
CDC: '-->';

// Case-insensitive fragments for keywords
fragment A: [aA];
fragment B: [bB];
fragment C: [cC];
fragment D: [dD];
fragment E: [eE];
fragment F: [fF];
fragment G: [gG];
fragment H: [hH];
fragment I: [iI];
fragment J: [jJ];
fragment K: [kK];
fragment L: [lL];
fragment M: [mM];
fragment N: [nN];
fragment O: [oO];
fragment P: [pP];
fragment Q: [qQ];
fragment R: [rR];
fragment S: [sS];
fragment T: [tT];
fragment U: [uU];
fragment V: [vV];
fragment W: [wW];
fragment X: [xX];
fragment Y: [yY];
fragment Z: [zZ];