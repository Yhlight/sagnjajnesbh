// CSS3 Parser Grammar
parser grammar CSS3Parser;

options {
    tokenVocab = CSS3Lexer;
}

stylesheet
    : ( charset | importRule | statement )* EOF
    ;

charset
    : CHARSET STRING SEMICOLON
    ;

importRule
    : IMPORT ( STRING | URI ) mediaList? SEMICOLON
    ;

mediaList
    : IDENTIFIER ( COMMA IDENTIFIER )*
    ;

statement
    : ruleset
    | atRule
    ;

atRule
    : ATKEYWORD any* ( block | SEMICOLON )
    ;

block
    : LBRACE ( any | block | ATKEYWORD | SEMICOLON )* RBRACE
    ;

ruleset
    : selector ( COMMA selector )* LBRACE declaration? ( SEMICOLON declaration? )* RBRACE
    ;

selector
    : simpleSelector ( combinator simpleSelector )*
    ;

combinator
    : PLUS | GREATER | TILDE | WS
    ;

simpleSelector
    : elementName ( HASH | CLASS | attrib | pseudo )*
    | ( HASH | CLASS | attrib | pseudo )+
    ;

elementName
    : IDENTIFIER | STAR
    ;

attrib
    : LBRACKET IDENTIFIER ( ( EQUALS | TILDE_EQUALS | PIPE_EQUALS ) ( IDENTIFIER | STRING ) )? RBRACKET
    ;

pseudo
    : COLON COLON? ( IDENTIFIER | function )
    ;

declaration
    : property COLON expr ( IMPORTANT )?
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
    : FUNCTION expr RPAREN 
    ;

hexcolor
    : HASH
    ;

operator
    : SLASH | COMMA
    ;

unaryOperator
    : MINUS | PLUS
    ;

any
    : IDENTIFIER | NUMBER | PERCENTAGE | LENGTH | EMS | EXS | ANGLE | TIME | FREQ | STRING | URI | HASH | UNICODE_RANGE | TILDE_EQUALS | PIPE_EQUALS | COLON | FUNCTION | LPAREN any* RPAREN | LBRACKET any* RBRACKET
    ;