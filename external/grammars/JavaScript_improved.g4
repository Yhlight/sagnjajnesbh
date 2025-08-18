// Improved JavaScript grammar based on official ANTLR grammars-v4
// Combined lexer and parser for CHTL project needs

grammar JavaScript_improved;

// Parser rules
program
    : statement* EOF
    ;

statement
    : variableStatement
    | expressionStatement
    | ifStatement
    | whileStatement
    | forStatement
    | functionDeclaration
    | returnStatement
    | blockStatement
    | ';'
    ;

blockStatement
    : '{' statement* '}'
    ;

variableStatement
    : ('var' | 'let' | 'const') variableDeclaration (',' variableDeclaration)* ';'?
    ;

variableDeclaration
    : Identifier ('=' expression)?
    ;

functionDeclaration
    : 'function' Identifier '(' parameterList? ')' blockStatement
    ;

parameterList
    : Identifier (',' Identifier)*
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

expression
    : assignmentExpression
    ;

assignmentExpression
    : conditionalExpression (AssignmentOperator assignmentExpression)?
    ;

conditionalExpression
    : logicalOrExpression ('?' assignmentExpression ':' conditionalExpression)?
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
    : additiveExpression (('<' | '>' | '<=' | '>=') additiveExpression)*
    ;

additiveExpression
    : multiplicativeExpression (('+' | '-') multiplicativeExpression)*
    ;

multiplicativeExpression
    : unaryExpression (('*' | '/' | '%') unaryExpression)*
    ;

unaryExpression
    : postfixExpression
    | ('++' | '--' | '+' | '-' | '!' | '~') unaryExpression
    ;

postfixExpression
    : primaryExpression (('++' | '--' | '[' expression ']' | '.' Identifier | '(' argumentList? ')'))*
    ;

primaryExpression
    : 'this'
    | Identifier
    | Literal
    | '(' expression ')'
    | arrayLiteral
    | objectLiteral
    ;

arrayLiteral
    : '[' (expression (',' expression)*)? ']'
    ;

objectLiteral
    : '{' (propertyAssignment (',' propertyAssignment)*)? '}'
    ;

propertyAssignment
    : Identifier ':' expression
    | StringLiteral ':' expression
    | NumericLiteral ':' expression
    ;

argumentList
    : expression (',' expression)*
    ;

// Lexer rules
MultiLineComment : '/*' .*? '*/' -> channel(HIDDEN);
SingleLineComment : '//' ~[\r\n]* -> channel(HIDDEN);
WhiteSpace : [ \t\r\n\f]+ -> channel(HIDDEN);

// Keywords
Break : 'break';
Case : 'case';
Catch : 'catch';
Class : 'class';
Const : 'const';
Continue : 'continue';
Debugger : 'debugger';
Default : 'default';
Delete : 'delete';
Do : 'do';
Else : 'else';
Export : 'export';
Extends : 'extends';
Finally : 'finally';
For : 'for';
Function : 'function';
If : 'if';
Import : 'import';
In : 'in';
Instanceof : 'instanceof';
Let : 'let';
New : 'new';
Return : 'return';
Super : 'super';
Switch : 'switch';
This : 'this';
Throw : 'throw';
Try : 'try';
Typeof : 'typeof';
Var : 'var';
Void : 'void';
While : 'while';
With : 'with';
Yield : 'yield';

// Literals
BooleanLiteral : 'true' | 'false';
NullLiteral : 'null';
UndefinedLiteral : 'undefined';

// Numbers
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

// Strings
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
    : CharacterEscapeSequence
    | HexEscapeSequence
    | UnicodeEscapeSequence
    ;

fragment CharacterEscapeSequence
    : SingleEscapeCharacter
    | NonEscapeCharacter
    ;

fragment SingleEscapeCharacter
    : ['"\\bfnrtv]
    ;

fragment NonEscapeCharacter
    : ~['"\\bfnrtv0-9xu\r\n]
    ;

fragment HexEscapeSequence
    : 'x' [0-9a-fA-F] [0-9a-fA-F]
    ;

fragment UnicodeEscapeSequence
    : 'u' [0-9a-fA-F] [0-9a-fA-F] [0-9a-fA-F] [0-9a-fA-F]
    ;

// Identifiers
Identifier
    : IdentifierStart IdentifierPart*
    ;

fragment IdentifierStart
    : [a-zA-Z]
    | '$'
    | '_'
    ;

fragment IdentifierPart
    : IdentifierStart
    | [0-9]
    ;

// Operators and punctuation
AssignmentOperator : '=' | '*=' | '/=' | '%=' | '+=' | '-=' | '<<=' | '>>=' | '>>>=' | '&=' | '^=' | '|=';

// Punctuation
OpenBracket : '[';
CloseBracket : ']';
OpenParen : '(';
CloseParen : ')';
OpenBrace : '{';
CloseBrace : '}';
SemiColon : ';';
Comma : ',';
Dot : '.';
Question : '?';
Colon : ':';

// Combined literal for compatibility
Literal
    : NullLiteral
    | BooleanLiteral
    | NumericLiteral
    | StringLiteral
    ;

// Generic token for unmatched characters
Any : .;