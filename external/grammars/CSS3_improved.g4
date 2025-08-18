// Improved CSS3 Grammar based on official ANTLR grammars-v4
// Combined lexer and parser grammar for CHTL CSS compiler
// Adapted from: https://github.com/antlr/grammars-v4/tree/master/css3

grammar CSS3_improved;

// Parser Rules
stylesheet
    : ws (charset ws*)* (imports ws*)* (namespace_ ws*)* (nestedStatement ws*)* EOF
    ;

charset
    : '@charset' ws STRING ws ';' ws
    ;

imports
    : '@import' ws (STRING | url) ws mediaQueryList? ';' ws
    ;

namespace_
    : '@namespace' ws (namespacePrefix ws)? (STRING | url) ws ';' ws
    ;

namespacePrefix
    : IDENT
    ;

media
    : '@media' ws mediaQueryList groupRuleBody ws
    ;

mediaQueryList
    : (mediaQuery (COMMA ws mediaQuery)*)? ws
    ;

mediaQuery
    : ('only' | 'not')? ws mediaType ws ('and' ws mediaExpression)*
    | mediaExpression ('and' ws mediaExpression)*
    ;

mediaType
    : IDENT
    ;

mediaExpression
    : '(' ws mediaFeature (':' ws expr)? ')' ws
    ;

mediaFeature
    : IDENT ws
    ;

page
    : '@page' ws pseudoPage? '{' ws declaration? (';' ws declaration?)* '}' ws
    ;

pseudoPage
    : ':' IDENT ws
    ;

selectorGroup
    : selector (COMMA ws selector)*
    ;

selector
    : simpleSelectorSequence ws (combinator simpleSelectorSequence ws)*
    ;

combinator
    : '+' ws
    | '>' ws
    | '~' ws
    | ws
    ;

simpleSelectorSequence
    : (typeSelector | universal) (HASH | className | attrib | pseudo)*
    | (HASH | className | attrib | pseudo)+
    ;

typeSelector
    : typeNamespacePrefix? elementName
    ;

typeNamespacePrefix
    : (IDENT | '*')? '|'
    ;

elementName
    : IDENT
    ;

universal
    : typeNamespacePrefix? '*'
    ;

className
    : '.' IDENT
    ;

attrib
    : '[' ws typeNamespacePrefix? IDENT ws (('^=' | '$=' | '*=' | '=' | '~=' | '|=') ws (IDENT | STRING))? ws ']'
    ;

pseudo
    : ':' ':'? (IDENT | functionalPseudo)
    ;

functionalPseudo
    : FUNCTION ws expression ')'
    ;

expression
    : ('+' | '-' | DIMENSION | NUMBER | STRING | IDENT) ws
    ;

negation
    : ':not' ws negationArg ws ')'
    ;

negationArg
    : typeSelector | universal | HASH | className | attrib | pseudo
    ;

nestedStatement
    : ruleset
    | media
    | page
    | fontFaceRule
    | keyframesRule
    | supportsRule
    | atRule
    ;

groupRuleBody
    : '{' ws nestedStatement* '}'
    ;

ruleset
    : selectorGroup '{' ws declarationList? '}' ws
    ;

declarationList
    : declaration (';' ws declaration?)* ws
    ;

declaration
    : property ':' ws expr prio?
    | /* empty for error handling */
    ;

prio
    : '!important' ws
    ;

property
    : IDENT ws
    ;

expr
    : term (operator? term)*
    ;

term
    : unaryOperator? (NUMBER | PERCENTAGE | DIMENSION | STRING | IDENT | url | hexcolor | function_) ws
    ;

function_
    : FUNCTION ws expr ')' ws
    ;

unaryOperator
    : '-' | '+'
    ;

operator
    : '/' ws | ',' ws | /* empty */
    ;

hexcolor
    : HASH ws
    ;

url
    : 'url(' ws (STRING | URL) ws ')'
    ;

fontFaceRule
    : '@font-face' ws '{' ws declarationList? '}' ws
    ;

keyframesRule
    : '@keyframes' ws IDENT ws '{' ws keyframeBlock* '}' ws
    ;

keyframeBlock
    : keyframeSelector '{' ws declarationList? '}' ws
    ;

keyframeSelector
    : ('from' | 'to' | PERCENTAGE) ws
    ;

supportsRule
    : '@supports' ws supportsCondition ws groupRuleBody
    ;

supportsCondition
    : supportsNegation
    | supportsConjunction
    | supportsDisjunction
    | supportsConditionInParens
    ;

supportsConditionInParens
    : '(' ws supportsCondition ws ')'
    | supportsDeclarationCondition
    | generalEnclosed
    ;

supportsNegation
    : 'not' ws supportsConditionInParens
    ;

supportsConjunction
    : supportsConditionInParens (ws 'and' ws supportsConditionInParens)+
    ;

supportsDisjunction
    : supportsConditionInParens (ws 'or' ws supportsConditionInParens)+
    ;

supportsDeclarationCondition
    : '(' ws declaration ')'
    ;

generalEnclosed
    : ('(' ws IDENT ws ')') | ('(' ws STRING ws ')')
    ;

atRule
    : '@' IDENT ws any* (';' | ('{' ws any* '}')) ws
    ;

any
    : IDENT | NUMBER | PERCENTAGE | DIMENSION | STRING | URL | HASH | FUNCTION | '(' | ')' | '[' | ']' | ':' | ';' | '{' | '}' | ws
    ;

ws
    : (WHITESPACE | COMMENT)*
    ;

// Lexer Rules
WHITESPACE : [ \t\r\n\f]+ -> channel(HIDDEN);

COMMENT : '/*' .*? '*/' -> channel(HIDDEN);

STRING
    : '"' (~["\r\n\f\\] | '\\' . | '\\\r\n' | '\\\r' | '\\\n')* '"'
    | '\'' (~['\r\n\f\\] | '\\' . | '\\\r\n' | '\\\r' | '\\\n')* '\''
    ;

HASH : '#' NAME;

NUMBER : [0-9]+ ('.' [0-9]+)?;

PERCENTAGE : NUMBER '%';

DIMENSION : NUMBER IDENT;

URL : 'url(' WS* (STRING | (~[ \t\r\n\f()'"])*) WS* ')';

FUNCTION : IDENT '(';

IDENT : NMSTART NMCHAR*;

fragment NMSTART : [_a-zA-Z] | NONASCII | ESCAPE;

fragment NMCHAR : [_a-zA-Z0-9\-] | NONASCII | ESCAPE;

fragment NAME : NMCHAR+;

fragment NONASCII : [\u0080-\uFFFF];

fragment UNICODE : '\\' HEX HEX? HEX? HEX? HEX? HEX? WS?;

fragment ESCAPE : UNICODE | '\\' ~[\r\n\f0-9a-fA-F];

fragment HEX : [0-9a-fA-F];

fragment WS : [ \t\r\n\f];

COMMA : ',';

// Operators and punctuation
LBRACE : '{';
RBRACE : '}';
LPAREN : '(';
RPAREN : ')';
LBRACKET : '[';
RBRACKET : ']';
SEMICOLON : ';';
COLON : ':';
DOT : '.';
HASH_SYMBOL : '#';
STAR : '*';
PLUS : '+';
MINUS : '-';
SLASH : '/';
PIPE : '|';
TILDE : '~';
GREATER : '>';
EQUALS : '=';
CARET_EQUALS : '^=';
DOLLAR_EQUALS : '$=';
STAR_EQUALS : '*=';
TILDE_EQUALS : '~=';
PIPE_EQUALS : '|=';
EXCLAMATION : '!';