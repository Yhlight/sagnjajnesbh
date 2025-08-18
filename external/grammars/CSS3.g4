// Simplified CSS3 Grammar based on official ANTLR grammar
// Adapted for CHTL project without external dependencies

grammar CSS3;

// Parser Rules
stylesheet
    : ( charset | importRule | statement )* EOF
    ;

charset
    : '@charset' STRING ';'
    ;

importRule
    : '@import' ( STRING | URI ) ( MEDIA_LIST )? ';'
    ;

statement
    : ruleset
    | atRule
    ;

atRule
    : '@' IDENTIFIER ( any* )? ( block | ';' )
    ;

block
    : '{' ( any | block | ATKEYWORD | ';' )* '}'
    ;

ruleset
    : selector ( ',' selector )* '{' declaration? ( ';' declaration? )* '}'
    ;

selector
    : simpleSelector ( combinator simpleSelector )*
    ;

combinator
    : '+' | '>' | '~' | ' '
    ;

simpleSelector
    : elementName ( HASH | CLASS | ATTRIB | PSEUDO )*
    | ( HASH | CLASS | ATTRIB | PSEUDO )+
    ;

elementName
    : IDENTIFIER | '*'
    ;

declaration
    : property ':' expr ( '!important' )?
    ;

property
    : IDENTIFIER
    ;

expr
    : term ( operator? term )*
    ;

term
    : unaryOperator?
    ( NUMBER | PERCENTAGE | LENGTH | EMS | EXS | ANGLE | TIME | FREQ | STRING | IDENTIFIER | URI | hexcolor | function )
    ;

function
    : FUNCTION expr ')' 
    ;

hexcolor
    : HASH
    ;

operator
    : '/' | ','
    ;

unaryOperator
    : '-' | '+'
    ;

any
    : IDENTIFIER | NUMBER | PERCENTAGE | LENGTH | EMS | EXS | ANGLE | TIME | FREQ | STRING | URI | HASH | UNICODE_RANGE | INCLUDES | DASHMATCH | ':' | FUNCTION | '(' any* ')' | '[' any* ']'
    ;

// Lexer Rules
fragment NONASCII: ~[\u0000-\u007F];
fragment UNICODE: '\\' HEX HEX? HEX? HEX? HEX? HEX? ( '\r\n' | [ \t\r\n\f] )?;
fragment ESCAPE: UNICODE | '\\' ~[\r\n\f0-9a-fA-F];
fragment NMSTART: [_a-zA-Z] | NONASCII | ESCAPE;
fragment NMCHAR: [_a-zA-Z0-9\-] | NONASCII | ESCAPE;
fragment NAME: NMCHAR+;
fragment HEX: [0-9a-fA-F];

STRING
    : '"' (~[\n\r\f\\"] | '\\' NEWLINE | NONASCII | ESCAPE)* '"'
    | '\'' (~[\n\r\f\\'] | '\\' NEWLINE | NONASCII | ESCAPE)* '\''
    ;

IDENTIFIER: NMSTART NMCHAR*;

FUNCTION: IDENTIFIER '(';

NUMBER: [0-9]* '.'? [0-9]+;

PERCENTAGE: NUMBER '%';

LENGTH: NUMBER ( 'px' | 'cm' | 'mm' | 'in' | 'pt' | 'pc' );

EMS: NUMBER 'em';

EXS: NUMBER 'ex';

ANGLE: NUMBER ( 'deg' | 'rad' | 'grad' );

TIME: NUMBER ( 'ms' | 's' );

FREQ: NUMBER ( 'hz' | 'khz' );

URI: [Uu][Rr][Ll] '(' WS ( STRING | (~[\u0000-\u001F\u007F()\\"] | '\\' NEWLINE | NONASCII | ESCAPE)* ) WS ')';

HASH: '#' NAME;

CLASS: '.' IDENTIFIER;

ATTRIB: '[' WS IDENTIFIER WS ( ( '=' | INCLUDES | DASHMATCH ) WS ( IDENTIFIER | STRING ) WS )? ']';

PSEUDO: ':' ':'? ( IDENTIFIER | FUNCTION any* ')' );

UNICODE_RANGE: [Uu] '+' HEX HEX HEX HEX HEX HEX ( '-' HEX HEX HEX HEX HEX HEX )?;

INCLUDES: '~=';

DASHMATCH: '|=';

ATKEYWORD: '@' IDENTIFIER;

MEDIA_LIST: IDENTIFIER ( ',' WS IDENTIFIER )*;

fragment NEWLINE: '\r\n' | [\r\n\f];

WS: [ \t\r\n\f]+ -> skip;

COMMENT: '/*' .*? '*/' -> skip;