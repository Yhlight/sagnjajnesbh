/*
 * Simplified CSS3 Grammar for ANTLR 4
 * C++ target compatible
 */

grammar CSS3;

// Parser Rules
stylesheet
    : rule* EOF
    ;

rule
    : qualified_rule
    | at_rule
    ;

at_rule
    : AT_KEYWORD component_value* ( '{' declaration_list '}' | ';' )
    ;

qualified_rule
    : component_value* '{' declaration_list '}'
    ;

declaration_list
    : declaration ( ';' declaration )*
    ;

declaration
    : IDENT ':' component_value+ IMPORTANT?
    ;

component_value
    : preserved_token
    | block
    | function
    ;

block
    : '{' component_value* '}'
    | '[' component_value* ']'
    | '(' component_value* ')'
    ;

function
    : FUNCTION component_value* ')'
    ;

preserved_token
    : IDENT
    | STRING
    | DELIM
    | NUMBER
    | PERCENTAGE
    | DIMENSION
    | URI
    | HASH
    | UNICODE_RANGE
    | INCLUDES
    | DASHMATCH
    | PREFIX_MATCH
    | SUFFIX_MATCH
    | SUBSTRING_MATCH
    | COLUMN
    | CDO
    | CDC
    | COLON
    | SEMICOLON
    | COMMA
    | WS
    ;

// Lexer Rules
fragment NEWLINE        : '\r\n' | [ \r\n\f] ;
fragment WHITESPACE     : [ \t] | NEWLINE ;
fragment HEX            : [0-9a-fA-F] ;
fragment ESCAPE         : '\\' ( HEX HEX HEX HEX HEX HEX | HEX HEX HEX HEX HEX | HEX HEX HEX HEX | HEX HEX HEX | HEX HEX | HEX ) WHITESPACE?
                        | '\\' ~[0-9a-fA-F\r\n\f]
                        ;
fragment NMSTART        : [a-zA-Z_] | ESCAPE ;
fragment NMCHAR         : [a-zA-Z0-9_-] | ESCAPE ;

// Tokens
AT_KEYWORD      : '@' NMSTART NMCHAR* ;
IDENT           : '-'? NMSTART NMCHAR* ;
FUNCTION        : IDENT '(' ;
STRING          : '"' ( ~[\n\r\f\\"] | '\\' NEWLINE | ESCAPE )* '"'
                | '\'' ( ~[\n\r\f\\'] | '\\' NEWLINE | ESCAPE )* '\''
                ;
HASH            : '#' NMCHAR+ ;
NUMBER          : [+-]? ( [0-9]* '.' [0-9]+ | [0-9]+ ) ( [eE] [+-]? [0-9]+ )? ;
PERCENTAGE      : NUMBER '%' ;
DIMENSION       : NUMBER IDENT ;
URI             : 'url(' WHITESPACE* STRING WHITESPACE* ')'
                | 'url(' WHITESPACE* ( ~[)\\] | '\\' . )* WHITESPACE* ')'
                ;
UNICODE_RANGE   : 'U+' HEX HEX HEX HEX HEX HEX '-' HEX HEX HEX HEX HEX HEX
                | 'U+' HEX HEX HEX HEX HEX HEX
                | 'U+' HEX HEX HEX HEX HEX
                | 'U+' HEX HEX HEX HEX
                | 'U+' HEX HEX HEX
                | 'U+' HEX HEX
                | 'U+' HEX
                ;

// Operators
INCLUDES        : '~=' ;
DASHMATCH       : '|=' ;
PREFIX_MATCH    : '^=' ;
SUFFIX_MATCH    : '$=' ;
SUBSTRING_MATCH : '*=' ;
COLUMN          : '||' ;

// Special tokens
CDO             : '<!--' ;
CDC             : '-->' ;
IMPORTANT       : '!' WHITESPACE* 'important' ;

// Delimiters
SEMICOLON       : ';' ;
COLON           : ':' ;
COMMA           : ',' ;
DELIM           : . ;

// Whitespace
WS              : WHITESPACE+ -> channel(HIDDEN) ;
COMMENT         : '/*' .*? '*/' -> channel(HIDDEN) ;