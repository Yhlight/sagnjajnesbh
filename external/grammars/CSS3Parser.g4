// CHTL CSS3 Parser Grammar
// Based on official ANTLR grammars-v4 CSS3 grammar
// Simplified for CHTL compiler integration

parser grammar CSS3Parser;

options {
    tokenVocab = CSS3Lexer;
}

stylesheet: ws* (charset ws*)? (imports ws*)* (namespace ws*)* (nestedStatement ws*)* EOF;

charset: AtKeyword ws* String_ ws* SemiColon;

imports: AtKeyword ws* (String_ | Uri) ws* mediaQueryList? SemiColon;

namespace: AtKeyword ws* (namespacePrefixName ws*)? (String_ | Uri) ws* SemiColon;

namespacePrefixName: Ident;

media: AtKeyword ws* mediaQueryList ws* groupRuleBody;

mediaQueryList: ws* mediaQuery (ws* Comma ws* mediaQuery)*;

mediaQuery: (Only ws* | Not ws*)? mediaType (ws* And ws* mediaExpression)*
          | mediaExpression (ws* And ws* mediaExpression)*;

mediaType: Ident;

mediaExpression: OpenParen ws* mediaFeature (ws* Colon ws* expr)? ws* CloseParen;

mediaFeature: Ident;

page: AtKeyword ws* pseudoPage? ws* OpenBrace ws* declaration? (ws* SemiColon ws* declaration?)* ws* CloseBrace;

pseudoPage: Colon Ident;

selectorGroup: selector (ws* Comma ws* selector)*;

selector: simpleSelectorSequence (combinator simpleSelectorSequence)*;

combinator: ws* (Plus | Greater | Tilde) ws* | ws+;

simpleSelectorSequence: (typeSelector | universal) (Hash | className | attrib | pseudo | negation)*
                      | (Hash | className | attrib | pseudo | negation)+;

typeSelector: namespacePrefix? elementName;

namespacePrefix: (Ident | Multiply)? Pipe;

elementName: Ident;

universal: namespacePrefix? Multiply;

className: Dot Ident;

attrib: OpenBracket ws* namespacePrefix? Ident ws* ((Equal | '~=' | '|=' | '^=' | '$=' | '*=') ws* (Ident | String_) ws*)? CloseBracket;

pseudo: Colon Colon? (Ident | functionalPseudo);

functionalPseudo: Ident OpenParen ws* expr ws* CloseParen;

negation: ':not(' ws* negationArg ws* ')';

negationArg: typeSelector | universal | Hash | className | attrib | pseudo;

declaration: property ws* Colon ws* expr prio?;

prio: '!' ws* 'important';

property: Ident;

ruleset: selectorGroup ws* OpenBrace ws* declarationList? ws* CloseBrace;

declarationList: declaration (ws* SemiColon ws* declaration?)*;

expr: term (ws* operator? ws* term)*;

term: unaryOperator? (Number | Percentage | Dimension | String_ | Ident | Uri | Hash | unicodeRange | hexColor | function_);

function_: Ident OpenParen ws* expr? ws* CloseParen;

hexColor: Hash;

unicodeRange: UnicodeRange;

unaryOperator: Minus | Plus;

operator: Divide | Comma;

groupRuleBody: OpenBrace ws* (ruleset | nestedStatement)* ws* CloseBrace;

nestedStatement: ruleset | media | page | fontFaceRule | keyframesRule | supportsRule | atRule;

fontFaceRule: '@font-face' ws* OpenBrace ws* declarationList? ws* CloseBrace;

keyframesRule: '@keyframes' ws* Ident ws* OpenBrace ws* keyframeBlock* ws* CloseBrace;

keyframeBlock: keyframeSelector (ws* Comma ws* keyframeSelector)* ws* OpenBrace ws* declarationList? ws* CloseBrace;

keyframeSelector: 'from' | 'to' | Percentage;

supportsRule: '@supports' ws* supportsCondition ws* groupRuleBody;

supportsCondition: supportsNegation | supportsConjunction | supportsDisjunction | supportsConditionInParens;

supportsConditionInParens: OpenParen ws* supportsCondition ws* CloseParen | supportsDeclarationCondition | generalEnclosed;

supportsNegation: 'not' ws+ supportsConditionInParens;

supportsConjunction: supportsConditionInParens (ws+ 'and' ws+ supportsConditionInParens)+;

supportsDisjunction: supportsConditionInParens (ws+ 'or' ws+ supportsConditionInParens)+;

supportsDeclarationCondition: OpenParen ws* declaration ws* CloseParen;

generalEnclosed: (Ident | function_) OpenParen ws* Any* ws* CloseParen;

atRule: AtKeyword ws* Any* (OpenBrace ws* Any* ws* CloseBrace | SemiColon);

ws: Whitespace | Comment;