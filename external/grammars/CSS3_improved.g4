// Improved CSS3 grammar based on official ANTLR grammars-v4
// Simplified for CHTL project needs

lexer grammar CSS3_improved;

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
Multiply     : '*';
Divide       : '/';
Plus         : '+';
Minus        : '-';
Hash         : '#';
Comma        : ',';

// Fragments for CSS character classes
fragment Hex: [0-9a-fA-F];
fragment NewlineOrSpace: '\r\n' | [ \t\r\n\f] |;
fragment Unicode: '\\' Hex Hex? Hex? Hex? Hex? Hex? NewlineOrSpace;
fragment Escape: Unicode | '\\' ~[\r\n\f0-9a-fA-F];
fragment Nonascii: ~[\u0000-\u007F];
fragment Nmstart: [_a-zA-Z] | Nonascii | Escape;
fragment Nmchar: [_a-zA-Z0-9\-] | Nonascii | Escape;
fragment Name: Nmchar+;

// Comments
Comment: '/*' .*? '*/';

// Whitespace
Space: [ \t\r\n\f]+ -> channel(HIDDEN);
fragment Whitespace: [ \t\r\n\f]*;

// Identifiers and names
Ident: '-'? Nmstart Nmchar*;

// Numbers
Number: [0-9]* '.'? [0-9]+ ([eE] [+-]? [0-9]+)?;

// Strings
String_: 
    '"' ( ~["\\\r\n\f] | '\\' NewlineOrSpace | Escape )* '"' |
    '\'' ( ~['\\\r\n\f] | '\\' NewlineOrSpace | Escape )* '\'';

// URLs
Url: 'url(' Whitespace ( String_ | ( ~[)'"\\] | Escape )* ) Whitespace ')';

// Units
Percentage: Number '%';
Length: Number ('px' | 'em' | 'rem' | 'vh' | 'vw' | 'pt' | 'pc' | 'in' | 'cm' | 'mm');
Angle: Number ('deg' | 'rad' | 'grad' | 'turn');
Time: Number ('s' | 'ms');
Frequency: Number ('hz' | 'khz');

// Functions
Function_: Ident '(';

// At-rules
AtKeyword: '@' Ident;

// Important
Important: '!' Whitespace 'important';

// Generic tokens
Any: .;