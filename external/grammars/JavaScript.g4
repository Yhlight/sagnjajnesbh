/*
 * JavaScript Grammar for ANTLR 4
 * Combined lexer and parser grammar for C++ target compatibility
 * Based on the official ANTLR JavaScript grammar
 */

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
    | labelledStatement
    | switchStatement
    | throwStatement
    | tryStatement
    | debuggerStatement
    ;

block
    : '{' statementList? '}'
    ;

statementList
    : statement+
    ;

variableStatement
    : VAR variableDeclarationList ';'?
    | LET variableDeclarationList ';'?
    | CONST variableDeclarationList ';'?
    ;

variableDeclarationList
    : variableDeclaration (',' variableDeclaration)*
    ;

variableDeclaration
    : Identifier ('=' singleExpression)?
    ;

emptyStatement
    : ';'
    ;

expressionStatement
    : singleExpression ';'?
    ;

ifStatement
    : IF '(' expressionSequence ')' statement (ELSE statement)?
    ;

iterationStatement
    : FOR '(' expressionSequence? ';' expressionSequence? ';' expressionSequence? ')' statement
    | FOR '(' VAR variableDeclarationList ';' expressionSequence? ';' expressionSequence? ')' statement
    | FOR '(' singleExpression IN expressionSequence ')' statement
    | FOR '(' VAR variableDeclaration IN expressionSequence ')' statement
    | WHILE '(' expressionSequence ')' statement
    | DO statement WHILE '(' expressionSequence ')' ';'?
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

switchStatement
    : SWITCH '(' expressionSequence ')' caseBlock
    ;

caseBlock
    : '{' caseClauses? (defaultClause caseClauses?)? '}'
    ;

caseClauses
    : caseClause+
    ;

caseClause
    : CASE expressionSequence ':' statementList?
    ;

defaultClause
    : DEFAULT ':' statementList?
    ;

labelledStatement
    : Identifier ':' statement
    ;

throwStatement
    : THROW expressionSequence ';'?
    ;

tryStatement
    : TRY block (catchProduction finallyProduction? | finallyProduction)
    ;

catchProduction
    : CATCH '(' Identifier ')' block
    ;

finallyProduction
    : FINALLY block
    ;

debuggerStatement
    : DEBUGGER ';'?
    ;

functionDeclaration
    : FUNCTION Identifier '(' formalParameterList? ')' '{' functionBody '}'
    ;

formalParameterList
    : Identifier (',' Identifier)*
    ;

functionBody
    : sourceElements?
    ;

expressionSequence
    : singleExpression (',' singleExpression)*
    ;

singleExpression
    : FUNCTION Identifier? '(' formalParameterList? ')' '{' functionBody '}'                    # FunctionExpression
    | singleExpression '[' expressionSequence ']'                                               # MemberIndexExpression
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
    | singleExpression ('*' | '/' | '%') singleExpression                                      # MultiplicativeExpression
    | singleExpression ('+' | '-') singleExpression                                            # AdditiveExpression
    | singleExpression ('<<' | '>>' | '>>>') singleExpression                                  # BitShiftExpression
    | singleExpression ('<' | '>' | '<=' | '>=') singleExpression                              # RelationalExpression
    | singleExpression INSTANCEOF singleExpression                                              # InstanceofExpression
    | singleExpression IN singleExpression                                                      # InExpression
    | singleExpression ('==' | '!=' | '===' | '!==') singleExpression                         # EqualityExpression
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
    : '*='
    | '/='
    | '%='
    | '+='
    | '-='
    | '<<='
    | '>>='
    | '>>>='
    | '&='
    | '^='
    | '|='
    ;

literal
    : NULL                                                                                      # NullLiteral
    | BooleanLiteral                                                                           # BooleanLiteral
    | StringLiteral                                                                            # StringLiteral
    | numericLiteral                                                                           # NumericLiteralExpression
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
    : BREAK
    | CASE
    | CATCH
    | CONTINUE
    | DEBUGGER
    | DEFAULT
    | DELETE
    | DO
    | ELSE
    | FINALLY
    | FOR
    | FUNCTION
    | IF
    | IN
    | INSTANCEOF
    | NEW
    | RETURN
    | SWITCH
    | THIS
    | THROW
    | TRY
    | TYPEOF
    | VAR
    | VOID
    | WHILE
    | WITH
    | CONST
    | LET
    ;

arrayLiteral
    : '[' elementList? ']'
    ;

elementList
    : elision? singleExpression (elision singleExpression)*
    ;

elision
    : ','
    ;

objectLiteral
    : '{' propertyNameAndValueList? ','? '}'
    ;

propertyNameAndValueList
    : propertyAssignment (',' propertyAssignment)*
    ;

propertyAssignment
    : propertyName ':' singleExpression                                                        # PropertyExpressionAssignment
    | GETTER propertyName '(' ')' '{' functionBody '}'                                        # PropertyGetter
    | SETTER propertyName '(' propertySetParameterList ')' '{' functionBody '}'               # PropertySetter
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
    : singleExpression (',' singleExpression)*
    ;

// Lexer Rules

/// Null Literals
NULL                        : 'null';

/// Boolean Literals
BooleanLiteral              : 'true'
                            | 'false'
                            ;

/// Numeric Literals
DecimalLiteral              : DecimalIntegerLiteral '.' [0-9]* ExponentPart?
                            | '.' [0-9]+ ExponentPart?
                            | DecimalIntegerLiteral ExponentPart?
                            ;

HexIntegerLiteral           : '0' [xX] HexDigit+;

OctalIntegerLiteral         : '0' [0-7]+;

/// Keywords
BREAK                       : 'break';
CASE                        : 'case';
CATCH                       : 'catch';
CONTINUE                    : 'continue';
DEBUGGER                    : 'debugger';
DEFAULT                     : 'default';
DELETE                      : 'delete';
DO                          : 'do';
ELSE                        : 'else';
FINALLY                     : 'finally';
FOR                         : 'for';
FUNCTION                    : 'function';
IF                          : 'if';
IN                          : 'in';
INSTANCEOF                  : 'instanceof';
NEW                         : 'new';
RETURN                      : 'return';
SWITCH                      : 'switch';
THIS                        : 'this';
THROW                       : 'throw';
TRY                         : 'try';
TYPEOF                      : 'typeof';
VAR                         : 'var';
VOID                        : 'void';
WHILE                       : 'while';
WITH                        : 'with';
CONST                       : 'const';
LET                         : 'let';

/// Future Reserved Words
GETTER                      : 'get';
SETTER                      : 'set';

/// String Literals
StringLiteral               : '"' DoubleStringCharacter* '"'
                            | '\'' SingleStringCharacter* '\''
                            ;

/// Identifiers
Identifier                  : IdentifierStart IdentifierPart*;

/// Whitespace and Comments
WhiteSpaces                 : [\t\u000B\u000C\u0020\u00A0]+ -> channel(HIDDEN);
LineTerminator              : [\r\n\u2028\u2029] -> channel(HIDDEN);
SingleLineComment           : '//' ~[\r\n\u2028\u2029]* -> channel(HIDDEN);
MultiLineComment            : '/*' .*? '*/' -> channel(HIDDEN);

/// Fragment rules
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
    | '0'
    | HexEscapeSequence
    | UnicodeEscapeSequence
    ;

fragment CharacterEscapeSequence
    : SingleEscapeCharacter
    | NonEscapeCharacter
    ;

fragment HexEscapeSequence
    : 'x' HexDigit HexDigit
    ;

fragment UnicodeEscapeSequence
    : 'u' HexDigit HexDigit HexDigit HexDigit
    ;

fragment SingleEscapeCharacter
    : ['"\\bfnrtv]
    ;

fragment NonEscapeCharacter
    : ~['"\\bfnrtv0-9xu\r\n]
    ;

fragment HexDigit
    : [0-9a-fA-F]
    ;

fragment DecimalIntegerLiteral
    : '0'
    | [1-9] [0-9]*
    ;

fragment ExponentPart
    : [eE] [+-]? [0-9]+
    ;

fragment IdentifierPart
    : IdentifierStart
    | [0-9]
    ;

fragment IdentifierStart
    : [a-zA-Z]
    | '$'
    | '_'
    ;