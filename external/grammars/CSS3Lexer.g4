// CSS3 Lexer Grammar
lexer grammar CSS3Lexer;

// Symbols
CHARSET             : '@charset';
IMPORT              : '@import';
SEMICOLON           : ';';
COMMA               : ',';
LBRACE              : '{';
RBRACE              : '}';
LPAREN              : '(';
RPAREN              : ')';
LBRACKET            : '[';
RBRACKET            : ']';
COLON               : ':';
DOT                 : '.';
HASH_SYMBOL         : '#';
STAR                : '*';
PLUS                : '+';
GREATER             : '>';
TILDE               : '~';
MINUS               : '-';
SLASH               : '/';
PIPE                : '|';
EQUALS              : '=';
TILDE_EQUALS        : '~=';
PIPE_EQUALS         : '|=';
AT                  : '@';

// Keywords
IMPORTANT           : '!important';

// Complex tokens
fragment NONASCII   : ~[\u0000-\u007F];
fragment HEX        : [0-9a-fA-F];
fragment UNICODE    : '\\' HEX HEX? HEX? HEX? HEX? HEX? ( '\r\n' | [ \t\r\n\f] )?;
fragment ESCAPE     : UNICODE | '\\' ~[\r\n\f0-9a-fA-F];
fragment NMSTART    : [_a-zA-Z] | NONASCII | ESCAPE;
fragment NMCHAR     : [_a-zA-Z0-9\-] | NONASCII | ESCAPE;
fragment NAME       : NMCHAR+;

STRING              : '"' (~[\n\r\f\\"] | '\\' NEWLINE | NONASCII | ESCAPE)* '"'
                    | '\'' (~[\n\r\f\\'] | '\\' NEWLINE | NONASCII | ESCAPE)* '\''
                    ;

IDENTIFIER          : NMSTART NMCHAR*;

FUNCTION            : IDENTIFIER LPAREN;

NUMBER              : [0-9]* '.'? [0-9]+;

PERCENTAGE          : NUMBER '%';

LENGTH              : NUMBER ( 'px' | 'cm' | 'mm' | 'in' | 'pt' | 'pc' );

EMS                 : NUMBER 'em';

EXS                 : NUMBER 'ex';

ANGLE               : NUMBER ( 'deg' | 'rad' | 'grad' );

TIME                : NUMBER ( 'ms' | 's' );

FREQ                : NUMBER ( 'hz' | 'khz' );

URI                 : [Uu][Rr][Ll] LPAREN WS ( STRING | (~[\u0000-\u001F\u007F()\\"] | '\\' NEWLINE | NONASCII | ESCAPE)* ) WS RPAREN;

HASH                : HASH_SYMBOL NAME;

CLASS               : DOT IDENTIFIER;

ATKEYWORD           : AT IDENTIFIER;

UNICODE_RANGE       : [Uu] PLUS HEX HEX HEX HEX HEX HEX ( MINUS HEX HEX HEX HEX HEX HEX )?;

fragment NEWLINE    : '\r\n' | [\r\n\f];

WS                  : [ \t\r\n\f]+ -> skip;

COMMENT             : '/*' .*? '*/' -> skip;