// CHTL CSS3 Lexer Grammar
// Based on official ANTLR grammars-v4 CSS3 grammar
// Simplified for CHTL compiler integration

lexer grammar CSS3Lexer;

channels {
    ERROR
}

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
Pipe         : '|';
Underscore   : '_';
Plus         : '+';
Greater      : '>';
Tilde        : '~';
Comma        : ',';
Minus        : '-';

// Keywords
Only         : 'only';
Not          : 'not';
And          : 'and';
Important    : 'important';
From         : 'from';
To           : 'to';

fragment At: '@';
fragment Hex: [0-9a-fA-F];
fragment NewlineOrSpace: '\r\n' | [ \t\r\n\f] |;
fragment Unicode: '\\' Hex Hex? Hex? Hex? Hex? Hex? NewlineOrSpace;
fragment Escape: Unicode | '\\' ~[\r\n\f0-9a-fA-F];
fragment Nmstart: [_a-zA-Z] | Nonascii | Escape;
fragment Nmchar: [_a-zA-Z0-9\-] | Nonascii | Escape;
fragment Name: Nmchar+;
fragment Nonascii: ~[\u0000-\u007F];

Comment: '/*' .*? '*/' -> channel(HIDDEN);

Whitespace: [ \t\r\n\f]+ -> channel(HIDDEN);

Hash: '#' Name;

AtKeyword: At Nmstart Nmchar*;

Ident: Nmstart Nmchar*;

String_: 
    '"' ( ~[\n\r\f"\\] | '\\' NewlineOrSpace | Escape )* '"'
    | '\'' ( ~[\n\r\f'\\] | '\\' NewlineOrSpace | Escape )* '\'';

Number: [+-]? ( [0-9]* '.' [0-9]+ | [0-9]+ ) ( [eE] [+-]? [0-9]+ )?;

Percentage: Number '%';

Dimension: Number Ident;

Uri: U R L '(' Whitespace ( String_ | ( ~[') \t\r\n\f] | Escape )* ) Whitespace ')';

fragment U: [uU];
fragment R: [rR];
fragment L: [lL];

UnicodeRange: [uU] '+' ( Hex Hex? Hex? Hex? Hex? Hex? | Hex Hex? Hex? Hex? Hex? '?' | Hex Hex? Hex? Hex? '?' '?' | Hex Hex? Hex? '?' '?' '?' | Hex Hex? '?' '?' '?' '?' | Hex '?' '?' '?' '?' '?' | '?' '?' '?' '?' '?' '?' );

// Catch-all for any other character
Any: .;