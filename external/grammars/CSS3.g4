// Simplified CSS3 Grammar for CHTL CSS Compiler
// Based on official ANTLR grammars but simplified to avoid generation issues

grammar CSS3;

// Parser Rules
stylesheet
    : (ruleSet | atRule | COMMENT | WHITESPACE)* EOF
    ;

ruleSet
    : selector '{' declaration* '}'
    ;

atRule
    : '@' IDENT ('(' expr ')')? ('{' (ruleSet | declaration)* '}' | ';')
    ;

selector
    : simpleSelector (combinator simpleSelector)*
    ;

combinator
    : '+' | '>' | '~' | WHITESPACE
    ;

simpleSelector
    : elementName? (HASH | '.' IDENT | '[' IDENT ('=' (IDENT | STRING))? ']' | ':' IDENT)*
    ;

elementName
    : IDENT | '*'
    ;

declaration
    : property ':' expr ('!important')? ';'?
    ;

property
    : IDENT
    ;

expr
    : term (operator term)*
    ;

term
    : unaryOperator? (NUMBER | PERCENTAGE | DIMENSION | STRING | IDENT | HASH | url | function_)
    ;

function_
    : IDENT '(' expr? ')'
    ;

unaryOperator
    : '+' | '-'
    ;

operator
    : '/' | ','
    ;

url
    : 'url(' (STRING | URL_CHARS) ')'
    ;

// Lexer Rules
WHITESPACE : [ \t\r\n\f]+ -> channel(HIDDEN);

COMMENT : '/*' .*? '*/' -> channel(HIDDEN);

STRING
    : '"' (~["\r\n\f\\] | '\\' .)* '"'
    | '\'' (~['\r\n\f\\] | '\\' .)* '\''
    ;

HASH : '#' NAME;

NUMBER : [0-9]+ ('.' [0-9]+)?;

PERCENTAGE : NUMBER '%';

DIMENSION : NUMBER IDENT;

URL_CHARS : (~[ \t\r\n\f()'"])*;

IDENT : NMSTART NMCHAR*;

fragment NMSTART : [_a-zA-Z] | NONASCII | ESCAPE;

fragment NMCHAR : [_a-zA-Z0-9\-] | NONASCII | ESCAPE;

fragment NAME : NMCHAR+;

fragment NONASCII : [\u0080-\uFFFF];

// Fixed Unicode handling - no {1,6} quantifier
fragment UNICODE : '\\' HEX HEX? HEX? HEX? HEX? HEX? WS?;

fragment ESCAPE : UNICODE | '\\' ~[\r\n\f0-9a-fA-F];

fragment HEX : [0-9a-fA-F];

fragment WS : [ \t\r\n\f];

// Punctuation
LBRACE : '{';
RBRACE : '}';
LPAREN : '(';
RPAREN : ')';
LBRACKET : '[';
RBRACKET : ']';
SEMICOLON : ';';
COLON : ':';
DOT : '.';
COMMA : ',';
PLUS : '+';
MINUS : '-';
MULTIPLY : '*';
DIVIDE : '/';
EQUALS : '=';
TILDE : '~';
GREATER : '>';
PIPE : '|';
AT : '@';