// CSS3 Parser Grammar - Based on official ANTLR grammars-v4
// Adapted for CHTL project requirements
// Simplified for basic CSS parsing needs

parser grammar CSS3Parser;

options {
    tokenVocab = CSS3Lexer;
}

// Main stylesheet rule
stylesheet
    : ws? (charset ws?)? (imports ws?)* (namespace_ ws?)* (nestedStatement ws?)* EOF
    ;

// Character set declaration
charset
    : CHARSET ws? STRING ws? SemiColon ws?
    ;

// Import statements
imports
    : IMPORT ws? (STRING | uri) ws? mediaQueryList? SemiColon ws?
    ;

// Namespace declarations
namespace_
    : NAMESPACE ws? (namespacePrefix ws?)? (STRING | uri) ws? SemiColon ws?
    ;

namespacePrefix
    : IDENT
    ;

// Media queries
media
    : MEDIA ws? mediaQueryList groupRuleBody ws?
    ;

mediaQueryList
    : (mediaQuery (Comma ws? mediaQuery)*)? ws?
    ;

mediaQuery
    : mediaType ws? (AND ws? mediaExpression)*
    | mediaExpression (AND ws? mediaExpression)*
    ;

mediaType
    : IDENT
    ;

mediaExpression
    : OpenParen ws? mediaFeature (Colon ws? expr)? ws? CloseParen
    ;

mediaFeature
    : IDENT
    ;

// Nested statements (rules, at-rules, etc.)
nestedStatement
    : ruleset
    | media
    | page
    | fontFaceRule
    | keyframesRule
    | supportsRule
    | unknownAtRule
    ;

// Rulesets
ruleset
    : selectors_group OpenBrace ws? declarationList? CloseBrace ws?
    ;

selectors_group
    : selector (Comma ws? selector)*
    ;

selector
    : simple_selector_sequence (combinator simple_selector_sequence)*
    ;

combinator
    : Plus ws?
    | Greater ws?
    | Tilde ws?
    | ws
    ;

simple_selector_sequence
    : (type_selector | universal) (hash | class_ | attrib | pseudo | negation)*
    | (hash | class_ | attrib | pseudo | negation)+
    ;

type_selector
    : namespace_prefix? element_name
    ;

namespace_prefix
    : (IDENT | Multiply)? Pipe
    ;

element_name
    : IDENT
    ;

universal
    : namespace_prefix? Multiply
    ;

class_
    : Dot IDENT
    ;

attrib
    : OpenBracket ws? namespace_prefix? IDENT ws? 
      ((Equal | '~=' | '|=' | '^=' | '$=' | '*=') ws? (IDENT | STRING) ws?)? 
      CloseBracket
    ;

pseudo
    : Colon Colon? (IDENT | functional_pseudo)
    ;

functional_pseudo
    : FUNCTION ws? expression CloseParen
    ;

expression
    : (Plus | Minus | DIMENSION | NUMBER | STRING | IDENT) ws?
    ;

negation
    : ':not(' ws? negation_arg ws? CloseParen
    ;

negation_arg
    : type_selector | universal | hash | class_ | attrib | pseudo
    ;

// Declarations
declarationList
    : (declaration? SemiColon ws?)* declaration?
    ;

declaration
    : property ws? Colon ws? expr prio?
    ;

prio
    : '!important'
    ;

property
    : IDENT
    ;

expr
    : term (operator? term)*
    ;

term
    : unary_operator?
      (NUMBER | PERCENTAGE | DIMENSION | STRING | IDENT | uri | hexcolor | function_)
    ;

function_
    : FUNCTION ws? expr CloseParen
    ;

unary_operator
    : Minus | Plus
    ;

operator
    : Divide ws? | Comma ws? | /* empty for space */
    ;

hexcolor
    : Hash
    ;

// At-rules
page
    : PAGE ws? pseudo_page? OpenBrace ws? declarationList? CloseBrace ws?
    ;

pseudo_page
    : Colon IDENT
    ;

fontFaceRule
    : FONT_FACE ws? OpenBrace ws? declarationList? CloseBrace ws?
    ;

keyframesRule
    : KEYFRAMES ws? IDENT ws? OpenBrace ws? keyframes_blocks? CloseBrace ws?
    ;

keyframes_blocks
    : keyframes_block+
    ;

keyframes_block
    : keyframe_selector OpenBrace ws? declarationList? CloseBrace ws?
    ;

keyframe_selector
    : (PERCENTAGE | 'from' | 'to') (Comma ws? (PERCENTAGE | 'from' | 'to'))*
    ;

supportsRule
    : SUPPORTS ws? supports_condition ws? groupRuleBody
    ;

supports_condition
    : 'not' ws? supports_condition_in_parens
    | supports_condition_in_parens (ws? ('and' | 'or') ws? supports_condition_in_parens)*
    ;

supports_condition_in_parens
    : OpenParen ws? supports_condition ws? CloseParen
    | supports_decl
    | general_enclosed
    ;

supports_decl
    : OpenParen ws? declaration CloseParen
    ;

general_enclosed
    : (FUNCTION | OpenParen) (any | unused)* CloseParen
    ;

unknownAtRule
    : AT_KEYWORD ws? any* (SemiColon | groupRuleBody)
    ;

groupRuleBody
    : OpenBrace ws? nestedStatement* CloseBrace ws?
    ;

// Utility rules
uri
    : URI
    ;

any
    : IDENT | NUMBER | PERCENTAGE | DIMENSION | STRING
    | Plus | Minus | Multiply | Divide | Equal | Colon | SemiColon | Comma | Dot
    | OpenParen | CloseParen | OpenBracket | CloseBracket
    | OpenBrace | CloseBrace | Greater | Tilde | Pipe
    ;

unused
    : OpenBrace ws? unused* CloseBrace
    | OpenBracket ws? unused* CloseBracket  
    | OpenParen ws? unused* CloseParen
    | any
    ;

hash
    : Hash
    ;

ws
    : (Whitespace | Comment)+
    ;

// Keywords that need to be handled specially
AND: A N D;