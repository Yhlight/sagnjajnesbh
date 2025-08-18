/*
 * CSS3 Grammar for ANTLR 4
 * Based on W3C CSS Specification and improved for C++ target compatibility
 */

grammar CSS3_improved;

// Parser Rules
stylesheet
    : ( charset | imports | namespace_rule | rule )* EOF
    ;

charset
    : CHARSET_SYM STRING ';'
    ;

imports
    : IMPORT_SYM STRING media_list? ';'
    | IMPORT_SYM URI media_list? ';'
    ;

namespace_rule
    : NAMESPACE_SYM namespace_prefix? ( STRING | URI ) ';'
    ;

namespace_prefix
    : IDENT
    ;

media_list
    : medium ( COMMA medium )*
    ;

medium
    : IDENT
    ;

rule
    : ruleset
    | media_rule
    | page_rule
    | font_face_rule
    ;

media_rule
    : MEDIA_SYM media_list '{' ruleset* '}'
    ;

page_rule
    : PAGE_SYM pseudo_page? '{' declaration_list '}'
    ;

pseudo_page
    : ':' IDENT
    ;

font_face_rule
    : FONT_FACE_SYM '{' declaration_list '}'
    ;

ruleset
    : selector_list '{' declaration_list '}'
    ;

selector_list
    : selector ( COMMA selector )*
    ;

selector
    : simple_selector ( combinator simple_selector )*
    ;

combinator
    : PLUS
    | GREATER
    | TILDE
    | S
    ;

simple_selector
    : element_name ( HASH | class | attrib | pseudo )*
    | ( HASH | class | attrib | pseudo )+
    ;

element_name
    : IDENT
    | '*'
    ;

class
    : '.' IDENT
    ;

attrib
    : '[' IDENT ( ( '=' | INCLUDES | DASHMATCH ) ( IDENT | STRING ) )? ']'
    ;

pseudo
    : ':' ( IDENT | FUNCTION ( IDENT | STRING | NUMBER | DIMENSION )* ')' )
    ;

declaration_list
    : declaration ( ';' declaration )*
    ;

declaration
    : property ':' expr prio?
    ;

prio
    : IMPORTANT_SYM
    ;

expr
    : term ( operator term )*
    ;

operator
    : '/'
    | COMMA
    ;

term
    : unary_operator? ( NUMBER | PERCENTAGE | LENGTH | EMS | EXS | ANGLE | TIME | FREQ )
    | STRING
    | IDENT
    | URI
    | hexcolor
    | function
    ;

function
    : FUNCTION expr ')'
    ;

unary_operator
    : '-'
    | '+'
    ;

hexcolor
    : HASH
    ;

property
    : IDENT
    ;

// Lexer Rules
fragment H          : [0-9a-fA-F] ;
fragment NONASCII   : [\u0080-\uFFFF] ;
fragment UNICODE    : '\\' H H H H H H ( '\r\n' | [ \t\r\n\f] )? ;
fragment ESCAPE     : UNICODE | ( '\\' ~[\r\n\f0-9a-fA-F] ) ;
fragment NMSTART    : [_a-zA-Z] | NONASCII | ESCAPE ;
fragment NMCHAR     : [_a-zA-Z0-9-] | NONASCII | ESCAPE ;
fragment STRING1    : '"' ( ~[\n\r\f\\"] | '\\' NEWLINE | ESCAPE )* '"' ;
fragment STRING2    : '\'' ( ~[\n\r\f\\'] | '\\' NEWLINE | ESCAPE )* '\'' ;
fragment INVALID1   : '"' ( ~[\n\r\f\\"] | '\\' NEWLINE | ESCAPE )* ;
fragment INVALID2   : '\'' ( ~[\n\r\f\\'] | '\\' NEWLINE | ESCAPE )* ;
fragment COMMENT    : '/*' .*? '*/' ;
fragment IDENT      : '-'? NMSTART NMCHAR* ;
fragment NAME       : NMCHAR+ ;
fragment NUM        : [0-9]+ | [0-9]* '.' [0-9]+ ;
fragment STRING     : STRING1 | STRING2 ;
fragment INVALID    : INVALID1 | INVALID2 ;
fragment NEWLINE    : '\n' | '\r\n' | '\r' | '\f' ;
fragment RANGE      : '\\u' H H H H H H '-' '\\u' H H H H H H ;
fragment W          : [ \t\r\n\f]* ;

// Tokens
CHARSET_SYM     : '@charset' ;
IMPORT_SYM      : '@import' ;
MEDIA_SYM       : '@media' ;
PAGE_SYM        : '@page' ;
FONT_FACE_SYM   : '@font-face' ;
NAMESPACE_SYM   : '@namespace' ;
IMPORTANT_SYM   : '!' W 'important' ;

NUMBER          : NUM ;
PERCENTAGE      : NUM '%' ;
LENGTH          : NUM ( 'em' | 'ex' | 'px' | 'cm' | 'mm' | 'in' | 'pt' | 'pc' ) ;
EMS             : NUM 'em' ;
EXS             : NUM 'ex' ;
ANGLE           : NUM ( 'deg' | 'rad' | 'grad' ) ;
TIME            : NUM ( 'ms' | 's' ) ;
FREQ            : NUM ( 'hz' | 'khz' ) ;
DIMENSION       : NUM IDENT ;

STRING          : STRING1 | STRING2 ;
IDENT           : IDENT ;
HASH            : '#' NAME ;
URI             : 'url(' W ( STRING | ( ~[)\\]+ | '\\' . )* ) W ')' ;
FUNCTION        : IDENT '(' ;

INCLUDES        : '~=' ;
DASHMATCH       : '|=' ;

COMMA           : ',' ;
PLUS            : '+' ;
GREATER         : '>' ;
TILDE           : '~' ;

S               : [ \t\r\n\f]+ -> channel(HIDDEN) ;
COMMENT         : COMMENT -> channel(HIDDEN) ;

// Error handling
INVALID         : INVALID ;