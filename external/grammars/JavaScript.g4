// Simplified JavaScript Grammar for CHTL JavaScript Compiler
// Based on official ANTLR grammars but simplified to avoid generation issues

grammar JavaScript;

// Parser Rules
program
    : statement* EOF
    ;

statement
    : block
    | variableStatement
    | expressionStatement
    | ifStatement
    | whileStatement
    | forStatement
    | returnStatement
    | functionDeclaration
    | ';'
    ;

block
    : '{' statement* '}'
    ;

variableStatement
    : ('var' | 'let' | 'const') variableDeclaration (',' variableDeclaration)* ';'?
    ;

variableDeclaration
    : Identifier ('=' expression)?
    ;

expressionStatement
    : expression ';'?
    ;

ifStatement
    : 'if' '(' expression ')' statement ('else' statement)?
    ;

whileStatement
    : 'while' '(' expression ')' statement
    ;

forStatement
    : 'for' '(' (variableStatement | expression)? ';' expression? ';' expression? ')' statement
    ;

returnStatement
    : 'return' expression? ';'?
    ;

functionDeclaration
    : 'function' Identifier '(' parameterList? ')' block
    ;

parameterList
    : Identifier (',' Identifier)*
    ;

expression
    : assignmentExpression
    ;

assignmentExpression
    : conditionalExpression (('=' | '+=' | '-=' | '*=' | '/=') conditionalExpression)*
    ;

conditionalExpression
    : logicalOrExpression ('?' expression ':' conditionalExpression)?
    ;

logicalOrExpression
    : logicalAndExpression ('||' logicalAndExpression)*
    ;

logicalAndExpression
    : equalityExpression ('&&' equalityExpression)*
    ;

equalityExpression
    : relationalExpression (('==' | '!=' | '===' | '!==') relationalExpression)*
    ;

relationalExpression
    : additiveExpression (('<' | '>' | '<=' | '>=' | 'instanceof' | 'in') additiveExpression)*
    ;

additiveExpression
    : multiplicativeExpression (('+' | '-') multiplicativeExpression)*
    ;

multiplicativeExpression
    : unaryExpression (('*' | '/' | '%') unaryExpression)*
    ;

unaryExpression
    : ('++' | '--' | '+' | '-' | '~' | '!' | 'typeof' | 'void' | 'delete') unaryExpression
    | postfixExpression
    ;

postfixExpression
    : leftHandSideExpression ('++' | '--')?
    ;

leftHandSideExpression
    : callExpression
    | memberExpression
    ;

callExpression
    : memberExpression arguments (arguments | '[' expression ']' | '.' Identifier)*
    ;

memberExpression
    : primaryExpression ('[' expression ']' | '.' Identifier)*
    | 'new' memberExpression arguments?
    ;

primaryExpression
    : 'this'
    | Identifier
    | literal
    | arrayLiteral
    | objectLiteral
    | '(' expression ')'
    ;

arguments
    : '(' argumentList? ')'
    ;

argumentList
    : expression (',' expression)*
    ;

arrayLiteral
    : '[' elementList? ']'
    ;

elementList
    : expression (',' expression)*
    ;

objectLiteral
    : '{' propertyNameAndValueList? '}'
    ;

propertyNameAndValueList
    : propertyAssignment (',' propertyAssignment)*
    ;

propertyAssignment
    : propertyName ':' expression
    ;

propertyName
    : Identifier
    | StringLiteral
    | NumericLiteral
    ;

literal
    : NullLiteral
    | BooleanLiteral
    | StringLiteral
    | NumericLiteral
    ;

// Lexer Rules
MultiLineComment : '/*' .*? '*/' -> channel(HIDDEN);
SingleLineComment : '//' ~[\r\n\u2028\u2029]* -> channel(HIDDEN);

// Keywords
NullLiteral : 'null';
BooleanLiteral : 'true' | 'false';

// Literals
NumericLiteral
    : DecimalLiteral
    | HexIntegerLiteral
    ;

fragment DecimalLiteral
    : DecimalIntegerLiteral '.' [0-9]* ExponentPart?
    | '.' [0-9]+ ExponentPart?
    | DecimalIntegerLiteral ExponentPart?
    ;

fragment DecimalIntegerLiteral
    : '0'
    | [1-9] [0-9]*
    ;

fragment ExponentPart
    : [eE] [+-]? [0-9]+
    ;

fragment HexIntegerLiteral
    : '0' [xX] [0-9a-fA-F]+
    ;

StringLiteral
    : '"' DoubleStringCharacter* '"'
    | '\'' SingleStringCharacter* '\''
    ;

fragment DoubleStringCharacter
    : ~["\\\r\n]
    | '\\' EscapeSequence
    ;

fragment SingleStringCharacter
    : ~['\\\r\n]
    | '\\' EscapeSequence
    ;

fragment EscapeSequence
    : ['"\\bfnrtv]
    | '0'
    | 'x' [0-9a-fA-F] [0-9a-fA-F]
    | 'u' [0-9a-fA-F] [0-9a-fA-F] [0-9a-fA-F] [0-9a-fA-F]
    ;

// Identifiers
Identifier : [a-zA-Z$_] [a-zA-Z0-9$_]*;

// Whitespace
WhiteSpaces : [\t\u000B\u000C\u0020\u00A0]+ -> channel(HIDDEN);
LineTerminator : [\r\n\u2028\u2029] -> channel(HIDDEN);

// Punctuators
OpenBracket : '[';
CloseBracket : ']';
OpenParen : '(';
CloseParen : ')';
OpenBrace : '{';
CloseBrace : '}';
SemiColon : ';';
Comma : ',';
Assign : '=';
QuestionMark : '?';
Colon : ':';
Dot : '.';
PlusPlus : '++';
MinusMinus : '--';
Plus : '+';
Minus : '-';
BitNot : '~';
Not : '!';
Multiply : '*';
Divide : '/';
Modulus : '%';
LessThan : '<';
MoreThan : '>';
LessThanEquals : '<=';
GreaterThanEquals : '>=';
Equals_ : '==';
NotEquals : '!=';
IdentityEquals : '===';
IdentityNotEquals : '!==';
And : '&&';
Or : '||';
PlusAssign : '+=';
MinusAssign : '-=';
MultiplyAssign : '*=';
DivideAssign : '/=';