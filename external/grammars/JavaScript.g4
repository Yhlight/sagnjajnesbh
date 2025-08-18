// Simplified JavaScript Grammar for CHTL project
// Removed semantic predicates and base class dependencies

grammar JavaScript;

// Parser Rules
program
    : sourceElements? EOF
    ;

sourceElements
    : sourceElement+
    ;

sourceElement
    : statement
    | functionDeclaration
    ;

statement
    : block
    | variableStatement
    | emptyStatement
    | expressionStatement
    | ifStatement
    | iterationStatement
    | continueStatement
    | breakStatement
    | returnStatement
    | withStatement
    | throwStatement
    | tryStatement
    ;

block
    : '{' statementList? '}'
    ;

statementList
    : statement+
    ;

variableStatement
    : VAR variableDeclarationList ';'?
    ;

variableDeclarationList
    : variableDeclaration ( ',' variableDeclaration )*
    ;

variableDeclaration
    : Identifier initializer?
    ;

initializer
    : '=' singleExpression
    ;

emptyStatement
    : ';'
    ;

expressionStatement
    : expressionSequence ';'?
    ;

ifStatement
    : IF '(' expressionSequence ')' statement ( ELSE statement )?
    ;

iterationStatement
    : DO statement WHILE '(' expressionSequence ')' ';'?
    | WHILE '(' expressionSequence ')' statement
    | FOR '(' expressionSequence? ';' expressionSequence? ';' expressionSequence? ')' statement
    | FOR '(' VAR variableDeclarationList ';' expressionSequence? ';' expressionSequence? ')' statement
    | FOR '(' singleExpression IN expressionSequence ')' statement
    | FOR '(' VAR variableDeclaration IN expressionSequence ')' statement
    ;

continueStatement
    : CONTINUE Identifier? ';'?
    ;

breakStatement
    : BREAK Identifier? ';'?
    ;

returnStatement
    : RETURN expressionSequence? ';'?
    ;

withStatement
    : WITH '(' expressionSequence ')' statement
    ;

throwStatement
    : THROW expressionSequence ';'?
    ;

tryStatement
    : TRY block ( catchProduction finallyProduction? | finallyProduction )
    ;

catchProduction
    : CATCH '(' Identifier ')' block
    ;

finallyProduction
    : FINALLY block
    ;

functionDeclaration
    : FUNCTION Identifier '(' formalParameterList? ')' '{' functionBody '}'
    ;

formalParameterList
    : Identifier ( ',' Identifier )*
    ;

functionBody
    : sourceElements?
    ;

singleExpression
    : FUNCTION Identifier? '(' formalParameterList? ')' '{' functionBody '}'                    # FunctionExpression
    | singleExpression '[' expressionSequence ']'                                              # MemberIndexExpression
    | singleExpression '.' identifierName                                                       # MemberDotExpression
    | singleExpression arguments                                                                # CallExpression
    | NEW singleExpression arguments?                                                           # NewExpression
    | singleExpression '++'                                                                     # PostIncrementExpression
    | singleExpression '--'                                                                     # PostDecreaseExpression
    | DELETE singleExpression                                                                   # DeleteExpression
    | VOID singleExpression                                                                     # VoidExpression
    | TYPEOF singleExpression                                                                   # TypeofExpression
    | '++' singleExpression                                                                     # PreIncrementExpression
    | '--' singleExpression                                                                     # PreDecreaseExpression
    | '+' singleExpression                                                                      # UnaryPlusExpression
    | '-' singleExpression                                                                      # UnaryMinusExpression
    | '~' singleExpression                                                                      # BitNotExpression
    | '!' singleExpression                                                                      # NotExpression
    | singleExpression ( '*' | '/' | '%' ) singleExpression                                    # MultiplicativeExpression
    | singleExpression ( '+' | '-' ) singleExpression                                          # AdditiveExpression
    | singleExpression ( '<<' | '>>' | '>>>' ) singleExpression                                # BitShiftExpression
    | singleExpression ( '<' | '>' | '<=' | '>=' ) singleExpression                           # RelationalExpression
    | singleExpression INSTANCEOF singleExpression                                              # InstanceofExpression
    | singleExpression IN singleExpression                                                      # InExpression
    | singleExpression ( '==' | '!=' | '===' | '!==' ) singleExpression                       # EqualityExpression
    | singleExpression '&' singleExpression                                                     # BitAndExpression
    | singleExpression '^' singleExpression                                                     # BitXOrExpression
    | singleExpression '|' singleExpression                                                     # BitOrExpression
    | singleExpression '&&' singleExpression                                                    # LogicalAndExpression
    | singleExpression '||' singleExpression                                                    # LogicalOrExpression
    | singleExpression '?' singleExpression ':' singleExpression                               # TernaryExpression
    | singleExpression '=' singleExpression                                                     # AssignmentExpression
    | singleExpression assignmentOperator singleExpression                                     # AssignmentOperatorExpression
    | THIS                                                                                      # ThisExpression
    | Identifier                                                                                # IdentifierExpression
    | literal                                                                                   # LiteralExpression
    | arrayLiteral                                                                              # ArrayLiteralExpression
    | objectLiteral                                                                             # ObjectLiteralExpression
    | '(' expressionSequence ')'                                                               # ParenthesizedExpression
    ;

assignmentOperator
    : '*=' | '/=' | '%=' | '+=' | '-=' | '<<=' | '>>=' | '>>>=' | '&=' | '^=' | '|='
    ;

literal
    : ( NULL | BooleanLiteral | StringLiteral | RegularExpressionLiteral )
    | numericLiteral
    ;

numericLiteral
    : DecimalLiteral
    | HexIntegerLiteral
    | OctalIntegerLiteral
    ;

identifierName
    : Identifier
    | reservedWord
    ;

reservedWord
    : keyword
    | NULL
    | BooleanLiteral
    ;

keyword
    : BREAK | DO | INSTANCEOF | TYPEOF | CASE | ELSE | NEW | VAR | CATCH | FINALLY | RETURN | VOID | CONTINUE | FOR | SWITCH | WHILE | DEBUGGER | FUNCTION | THIS | WITH | DEFAULT | IF | THROW | DELETE | IN | TRY
    ;

arrayLiteral
    : '[' elementList? ']'
    ;

elementList
    : singleExpression ( ',' singleExpression )*
    ;

objectLiteral
    : '{' propertyNameAndValueList? '}'
    ;

propertyNameAndValueList
    : propertyAssignment ( ',' propertyAssignment )*
    ;

propertyAssignment
    : propertyName ':' singleExpression
    | GET propertyName '(' ')' '{' functionBody '}'
    | SET propertyName '(' propertySetParameterList ')' '{' functionBody '}'
    ;

propertyName
    : identifierName
    | StringLiteral
    | numericLiteral
    ;

propertySetParameterList
    : Identifier
    ;

arguments
    : '(' argumentList? ')'
    ;

argumentList
    : singleExpression ( ',' singleExpression )*
    ;

expressionSequence
    : singleExpression ( ',' singleExpression )*
    ;

// Lexer Rules

/// Null Literals
NULL                      : 'null';

/// Boolean Literals
BooleanLiteral            : 'true' | 'false';

/// Numeric Literals
DecimalLiteral            : DecimalIntegerLiteral '.' [0-9]* ExponentPart?
                          | '.' [0-9]+ ExponentPart?
                          | DecimalIntegerLiteral ExponentPart?
                          ;

HexIntegerLiteral         : '0' [xX] [0-9a-fA-F]+;

OctalIntegerLiteral       : '0' [0-7]+;

fragment DecimalIntegerLiteral
                          : '0'
                          | [1-9] [0-9]*
                          ;

fragment ExponentPart     : [eE] [+-]? [0-9]+;

/// String Literals
StringLiteral             : '"' DoubleStringCharacter* '"'
                          | '\'' SingleStringCharacter* '\''
                          ;

fragment DoubleStringCharacter
                          : ~["\\\r\n]
                          | '\\' EscapeSequence
                          | LineContinuation
                          ;

fragment SingleStringCharacter
                          : ~['\\\r\n]
                          | '\\' EscapeSequence
                          | LineContinuation
                          ;

fragment EscapeSequence   : CharacterEscapeSequence
                          | '0' 
                          | HexEscapeSequence
                          | UnicodeEscapeSequence
                          ;

fragment CharacterEscapeSequence
                          : SingleEscapeCharacter
                          | NonEscapeCharacter
                          ;

fragment HexEscapeSequence: 'x' [0-9a-fA-F] [0-9a-fA-F];

fragment UnicodeEscapeSequence
                          : 'u' [0-9a-fA-F] [0-9a-fA-F] [0-9a-fA-F] [0-9a-fA-F];

fragment SingleEscapeCharacter
                          : ['"\\bfnrtv];

fragment NonEscapeCharacter
                          : ~['"\\bfnrtv0-9xu\r\n];

fragment LineContinuation : '\\' [\r\n\u2028\u2029];

/// Regular Expression Literals
RegularExpressionLiteral  : '/' RegularExpressionBody '/' RegularExpressionFlags;

fragment RegularExpressionBody
                          : RegularExpressionFirstChar RegularExpressionChar*;

fragment RegularExpressionFirstChar
                          : ~[*\r\n\u2028\u2029\\/[]
                          | RegularExpressionBackslashSequence
                          | RegularExpressionClass
                          ;

fragment RegularExpressionChar
                          : ~[\r\n\u2028\u2029\\/[]
                          | RegularExpressionBackslashSequence
                          | RegularExpressionClass
                          ;

fragment RegularExpressionClassChar
                          : ~[\r\n\u2028\u2029\\\]]
                          | RegularExpressionBackslashSequence
                          ;

fragment RegularExpressionClass
                          : '[' RegularExpressionClassChar* ']';

fragment RegularExpressionBackslashSequence
                          : '\\' ~[\r\n\u2028\u2029];

fragment RegularExpressionFlags
                          : [gimsuxy]*;

/// Keywords
BREAK                     : 'break';
DO                        : 'do';
INSTANCEOF                : 'instanceof';
TYPEOF                    : 'typeof';
CASE                      : 'case';
ELSE                      : 'else';
NEW                       : 'new';
VAR                       : 'var';
CATCH                     : 'catch';
FINALLY                   : 'finally';
RETURN                    : 'return';
VOID                      : 'void';
CONTINUE                  : 'continue';
FOR                       : 'for';
SWITCH                    : 'switch';
WHILE                     : 'while';
DEBUGGER                  : 'debugger';
FUNCTION                  : 'function';
THIS                      : 'this';
WITH                      : 'with';
DEFAULT                   : 'default';
IF                        : 'if';
THROW                     : 'throw';
DELETE                    : 'delete';
IN                        : 'in';
TRY                       : 'try';
GET                       : 'get';
SET                       : 'set';

/// Identifiers
Identifier                : IdentifierStart IdentifierPart*;

fragment IdentifierStart  : [a-zA-Z$_]
                          | '\\' UnicodeEscapeSequence
                          ;

fragment IdentifierPart   : IdentifierStart
                          | [0-9]
                          ;

/// White Space
WhiteSpaces               : [\t\u000B\u000C\u0020\u00A0]+ -> skip;

/// Line Terminators
LineTerminator            : [\r\n\u2028\u2029] -> skip;

/// Comments
MultiLineComment          : '/*' .*? '*/' -> skip;
SingleLineComment         : '//' ~[\r\n\u2028\u2029]* -> skip;