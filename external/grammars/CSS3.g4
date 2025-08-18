grammar CSS3;

// 简化的CSS3语法定义 - 专为CHTL优化

stylesheet
    : (ruleset | at_rule)* EOF
    ;

at_rule
    : AT_KEYWORD any_value* (SEMICOLON | block)
    ;

ruleset
    : selector_list block
    ;

selector_list
    : selector (COMMA selector)*
    ;

selector
    : simple_selector (combinator simple_selector)*
    ;

combinator
    : PLUS
    | GREATER
    | TILDE
    | WS
    ;

simple_selector
    : element_name selector_modifier*
    | selector_modifier+
    ;

element_name
    : IDENT
    | STAR
    ;

selector_modifier
    : HASH
    | class_selector
    | attrib
    | pseudo
    ;

class_selector
    : DOT IDENT
    ;

attrib
    : LBRACKET IDENT (attrib_operator (IDENT | STRING))? RBRACKET
    ;

attrib_operator
    : EQUALS
    | INCLUDES
    | DASHMATCH
    | PREFIXMATCH
    | SUFFIXMATCH
    | SUBSTRINGMATCH
    ;

pseudo
    : COLON COLON? (IDENT | functional_pseudo)
    ;

functional_pseudo
    : FUNCTION any_value* RPAREN
    ;

block
    : LBRACE declaration_list RBRACE
    ;

declaration_list
    : declaration (SEMICOLON declaration?)*
    ;

declaration
    : property COLON value_list (IMPORTANT)?
    ;

property
    : IDENT
    ;

value_list
    : value+
    ;

value
    : NUMBER unit?
    | PERCENTAGE
    | STRING
    | IDENT
    | HASH
    | URI
    | FUNCTION any_value* RPAREN
    | LPAREN any_value* RPAREN
    | LBRACKET any_value* RBRACKET
    ;

unit
    : IDENT
    ;

any_value
    : NUMBER unit?
    | PERCENTAGE
    | STRING
    | IDENT
    | HASH
    | URI
    | FUNCTION
    | LPAREN
    | RPAREN
    | LBRACKET
    | RBRACKET
    | LBRACE
    | RBRACE
    | SEMICOLON
    | COLON
    | COMMA
    | PLUS
    | MINUS
    | STAR
    | SLASH
    | EQUALS
    | DOT
    | GREATER
    | TILDE
    ;

// Lexer Rules
AT_KEYWORD      : '@' IDENT;
HASH            : '#' NMCHAR+;
STRING          : '"' (~["\n\r\f\\] | '\\' .)* '"' | '\'' (~['\n\r\f\\] | '\\' .)* '\'';
URI             : 'url(' WS* (STRING | (~[)\\] | '\\' .)*) WS* ')';
FUNCTION        : IDENT '(';
NUMBER          : [0-9]+ ('.' [0-9]+)?;
PERCENTAGE      : NUMBER '%';
IDENT           : '-'? NMSTART NMCHAR*;

// Operators
INCLUDES        : '~=';
DASHMATCH       : '|=';
PREFIXMATCH     : '^=';
SUFFIXMATCH     : '$=';
SUBSTRINGMATCH  : '*=';
EQUALS          : '=';

// Punctuation
SEMICOLON       : ';';
LBRACE          : '{';
RBRACE          : '}';
LPAREN          : '(';
RPAREN          : ')';
LBRACKET        : '[';
RBRACKET        : ']';
COMMA           : ',';
COLON           : ':';
DOT             : '.';
PLUS            : '+';
MINUS           : '-';
STAR            : '*';
SLASH           : '/';
GREATER         : '>';
TILDE           : '~';

// Special
IMPORTANT       : '!' WS* 'important';

// Whitespace
WS              : [ \t\r\n\f]+ -> channel(HIDDEN);
COMMENT         : '/*' .*? '*/' -> channel(HIDDEN);

// Fragments
fragment NMSTART : [a-zA-Z_] | NON_ASCII | ESCAPE;
fragment NMCHAR  : [a-zA-Z0-9_-] | NON_ASCII | ESCAPE;
fragment NON_ASCII : ~[\u0000-\u007F];
fragment ESCAPE  : '\\' ([0-9a-fA-F]{1,6} WS? | ~[0-9a-fA-F\r\n\f]);