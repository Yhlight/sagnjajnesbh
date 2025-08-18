// CHTL CSS3 Combined Grammar
// Based on official ANTLR grammars-v4 CSS3 grammar
// Combined lexer and parser for simplified integration

grammar CSS3Combined;

// Parser rules
stylesheet: ws* (charset ws*)? (imports ws*)? (namespace ws*)? (nestedStatement ws*)* EOF;

charset: '@charset' ws* STRING ws* ';';

imports: '@import' ws* (STRING | URI) ws* mediaQueryList? ';';

namespace: '@namespace' ws* (namespacePrefixName ws*)? (STRING | URI) ws* ';';

namespacePrefixName: IDENT;

media: '@media' ws* mediaQueryList ws* groupRuleBody;

mediaQueryList: ws* mediaQuery (ws* ',' ws* mediaQuery)*;

mediaQuery: ('only' ws* | 'not' ws*)? mediaType (ws* 'and' ws* mediaExpression)*
          | mediaExpression (ws* 'and' ws* mediaExpression)*;

mediaType: IDENT;

mediaExpression: '(' ws* mediaFeature (ws* ':' ws* expr)? ws* ')';

mediaFeature: IDENT;

selectorGroup: selector (ws* ',' ws* selector)*;

selector: simpleSelectorSequence (combinator simpleSelectorSequence)*;

combinator: ws* ('+' | '>' | '~') ws* | ws+;

simpleSelectorSequence: (typeSelector | universal) (HASH | className | attrib | pseudo)*
                      | (HASH | className | attrib | pseudo)+;

typeSelector: namespacePrefix? elementName;

namespacePrefix: (IDENT | '*')? '|';

elementName: IDENT;

universal: namespacePrefix? '*';

className: '.' IDENT;

attrib: '[' ws* namespacePrefix? IDENT ws* (('=' | '~=' | '|=' | '^=' | '$=' | '*=') ws* (IDENT | STRING) ws*)? ']';

pseudo: ':' ':'? (IDENT | functionalPseudo);

functionalPseudo: IDENT '(' ws* expr ws* ')';

declaration: property ws* ':' ws* expr prio?;

prio: '!' ws* 'important';

property: IDENT;

ruleset: selectorGroup ws* '{' ws* declarationList? ws* '}';

declarationList: declaration (ws* ';' ws* declaration?)*;

expr: term (ws* operator? ws* term)*;

term: unaryOperator? (NUMBER | PERCENTAGE | DIMENSION | STRING | IDENT | URI | HASH | hexColor | function_);

function_: IDENT '(' ws* expr? ws* ')';

hexColor: HASH;

unaryOperator: '-' | '+';

operator: '/' | ',';

groupRuleBody: '{' ws* (ruleset | nestedStatement)* ws* '}';

nestedStatement: ruleset | media | atRule;

atRule: '@' IDENT ws* (~[;{])* ('{' ws* (~[{}])* ws* '}' | ';');

ws: WS | COMMENT;

// Lexer rules
fragment HEX: [0-9a-fA-F];
fragment NEWLINE: '\r\n' | [ \t\r\n\f];
fragment UNICODE: '\\' HEX HEX? HEX? HEX? HEX? HEX? NEWLINE?;
fragment ESCAPE: UNICODE | '\\' ~[\r\n\f0-9a-fA-F];
fragment NMSTART: [_a-zA-Z] | NONASCII | ESCAPE;
fragment NMCHAR: [_a-zA-Z0-9\-] | NONASCII | ESCAPE;
fragment NAME: NMCHAR+;
fragment NONASCII: ~[\u0000-\u007F];

WS: [ \t\r\n\f]+ -> channel(HIDDEN);

COMMENT: '/*' .*? '*/' -> channel(HIDDEN);

HASH: '#' NAME;

IDENT: NMSTART NMCHAR*;

STRING: 
    '"' ( ~[\n\r\f"\\] | '\\' NEWLINE | ESCAPE )* '"'
    | '\'' ( ~[\n\r\f'\\] | '\\' NEWLINE | ESCAPE )* '\'';

NUMBER: [+-]? ( [0-9]* '.' [0-9]+ | [0-9]+ ) ( [eE] [+-]? [0-9]+ )?;

PERCENTAGE: NUMBER '%';

DIMENSION: NUMBER IDENT;

URI: [uU][rR][lL] '(' WS* ( STRING | ( ~[') \t\r\n\f] | ESCAPE )* ) WS* ')';

UNICODERANGE: [uU] '+' ( HEX HEX? HEX? HEX? HEX? HEX? | HEX HEX? HEX? HEX? HEX? '?' | HEX HEX? HEX? HEX? '?' '?' | HEX HEX? HEX? '?' '?' '?' | HEX HEX? '?' '?' '?' '?' | HEX '?' '?' '?' '?' '?' | '?' '?' '?' '?' '?' '?' );

// Catch-all for any other character
ANY: .;