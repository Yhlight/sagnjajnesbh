grammar JavaScript;

// 简化的JavaScript语法定义 - 专为CHTL优化

program
    : sourceElements? EOF
    ;

sourceElements
    : sourceElement+
    ;

sourceElement
    : statement
    | declaration
    ;

statement
    : block
    | variableStatement
    | emptyStatement
    | expressionStatement
    | ifStatement
    | iterationStatement
    | returnStatement
    | switchStatement
    | throwStatement
    | tryStatement
    ;

declaration
    : variableDeclaration
    | functionDeclaration
    | classDeclaration
    ;

block
    : LBRACE statementList? RBRACE
    ;

statementList
    : statement+
    ;

variableStatement
    : variableDeclaration SEMICOLON?
    ;

variableDeclaration
    : (VAR | LET | CONST) variableDeclarationList
    ;

variableDeclarationList
    : variableDeclarator (COMMA variableDeclarator)*
    ;

variableDeclarator
    : Identifier (ASSIGN singleExpression)?
    ;

emptyStatement
    : SEMICOLON
    ;

expressionStatement
    : expressionSequence SEMICOLON?
    ;

ifStatement
    : IF LPAREN expressionSequence RPAREN statement (ELSE statement)?
    ;

iterationStatement
    : WHILE LPAREN expressionSequence RPAREN statement                                    # WhileStatement
    | FOR LPAREN (expressionSequence | variableDeclaration)? SEMICOLON expressionSequence? SEMICOLON expressionSequence? RPAREN statement # ForStatement
    | FOR LPAREN (singleExpression | variableDeclaration) (IN | OF) expressionSequence RPAREN statement # ForInStatement
    ;

returnStatement
    : RETURN expressionSequence? SEMICOLON?
    ;

switchStatement
    : SWITCH LPAREN expressionSequence RPAREN caseBlock
    ;

caseBlock
    : LBRACE caseClauses? (defaultClause caseClauses?)? RBRACE
    ;

caseClauses
    : caseClause+
    ;

caseClause
    : CASE expressionSequence COLON statementList?
    ;

defaultClause
    : DEFAULT COLON statementList?
    ;

throwStatement
    : THROW expressionSequence SEMICOLON?
    ;

tryStatement
    : TRY block (catchProduction finallyProduction? | finallyProduction)
    ;

catchProduction
    : CATCH LPAREN Identifier? RPAREN block
    ;

finallyProduction
    : FINALLY block
    ;

functionDeclaration
    : ASYNC? FUNCTION STAR? Identifier LPAREN formalParameterList? RPAREN functionBody
    ;

classDeclaration
    : CLASS Identifier (EXTENDS singleExpression)? LBRACE classElement* RBRACE
    ;

classElement
    : STATIC? methodDefinition
    | emptyStatement
    ;

methodDefinition
    : STAR? propertyName LPAREN formalParameterList? RPAREN functionBody
    | getter LPAREN RPAREN functionBody
    | setter LPAREN formalParameterList RPAREN functionBody
    ;

formalParameterList
    : formalParameterArg (COMMA formalParameterArg)*
    ;

formalParameterArg
    : Identifier (ASSIGN singleExpression)?
    ;

functionBody
    : LBRACE sourceElements? RBRACE
    ;

arrayLiteral
    : LBRACKET elementList? RBRACKET
    ;

elementList
    : arrayElement (COMMA arrayElement)*
    ;

arrayElement
    : singleExpression
    ;

objectLiteral
    : LBRACE (propertyAssignment (COMMA propertyAssignment)*)? RBRACE
    ;

propertyAssignment
    : propertyName COLON singleExpression                           # PropertyExpressionAssignment
    | LBRACKET singleExpression RBRACKET COLON singleExpression    # ComputedPropertyExpressionAssignment
    | STAR? propertyName LPAREN formalParameterList? RPAREN functionBody # FunctionProperty
    | getter LPAREN RPAREN functionBody                            # PropertyGetter
    | setter LPAREN formalParameterArg RPAREN functionBody        # PropertySetter
    ;

propertyName
    : identifierName
    | StringLiteral
    | numericLiteral
    | LBRACKET singleExpression RBRACKET
    ;

arguments
    : LPAREN (argument (COMMA argument)*)? RPAREN
    ;

argument
    : singleExpression
    ;

expressionSequence
    : singleExpression (COMMA singleExpression)*
    ;

singleExpression
    : anonymousFunction                                                      # AnonymousFunctionExpression
    | singleExpression LBRACKET expressionSequence RBRACKET                # MemberIndexExpression
    | singleExpression DOT identifierName                                   # MemberDotExpression
    | singleExpression arguments                                            # CallExpression
    | NEW singleExpression arguments?                                       # NewExpression
    | singleExpression PLUSPLUS                                             # PostIncrementExpression
    | singleExpression MINUSMINUS                                           # PostDecreaseExpression
    | DELETE singleExpression                                               # DeleteExpression
    | VOID singleExpression                                                 # VoidExpression
    | TYPEOF singleExpression                                               # TypeofExpression
    | PLUSPLUS singleExpression                                             # PreIncrementExpression
    | MINUSMINUS singleExpression                                           # PreDecreaseExpression
    | PLUS singleExpression                                                 # UnaryPlusExpression
    | MINUS singleExpression                                                # UnaryMinusExpression
    | BITNOT singleExpression                                               # BitNotExpression
    | NOT singleExpression                                                  # NotExpression
    | AWAIT singleExpression                                                # AwaitExpression
    | singleExpression POWER singleExpression                              # PowerExpression
    | singleExpression (STAR | SLASH | MODULUS) singleExpression          # MultiplicativeExpression
    | singleExpression (PLUS | MINUS) singleExpression                     # AdditiveExpression
    | singleExpression (LSHIFT | RSHIFT | URSHIFT) singleExpression        # BitShiftExpression
    | singleExpression (LT | GT | LE | GE) singleExpression                # RelationalExpression
    | singleExpression INSTANCEOF singleExpression                          # InstanceofExpression
    | singleExpression IN singleExpression                                  # InExpression
    | singleExpression (EQ | NE | SEQ | SNE) singleExpression              # EqualityExpression
    | singleExpression BITAND singleExpression                             # BitAndExpression
    | singleExpression BITXOR singleExpression                             # BitXOrExpression
    | singleExpression BITOR singleExpression                              # BitOrExpression
    | singleExpression AND singleExpression                                # LogicalAndExpression
    | singleExpression OR singleExpression                                 # LogicalOrExpression
    | singleExpression NULLCOALESCE singleExpression                       # CoalesceExpression
    | singleExpression QUESTION singleExpression COLON singleExpression   # TernaryExpression
    | singleExpression ASSIGN singleExpression                             # AssignmentExpression
    | singleExpression assignmentOperator singleExpression                 # AssignmentOperatorExpression
    | THIS                                                                  # ThisExpression
    | Identifier                                                            # IdentifierExpression
    | SUPER                                                                 # SuperExpression
    | literal                                                               # LiteralExpression
    | arrayLiteral                                                          # ArrayLiteralExpression
    | objectLiteral                                                         # ObjectLiteralExpression
    | LPAREN expressionSequence RPAREN                                     # ParenthesizedExpression
    ;

anonymousFunction
    : ASYNC? FUNCTION STAR? LPAREN formalParameterList? RPAREN functionBody
    | ASYNC? arrowFunctionParameters ARROW arrowFunctionBody
    ;

arrowFunctionParameters
    : Identifier
    | LPAREN formalParameterList? RPAREN
    ;

arrowFunctionBody
    : singleExpression
    | functionBody
    ;

assignmentOperator
    : STARASSIGN
    | SLASHASSIGN
    | MODULUSASSIGN
    | PLUSASSIGN
    | MINUSASSIGN
    | LSHIFTASSIGN
    | RSHIFTASSIGN
    | URSHIFTASSIGN
    | BITANDASSIGN
    | BITXORASSIGN
    | BITORASSIGN
    | POWERASSIGN
    ;

literal
    : NullLiteral
    | BooleanLiteral
    | StringLiteral
    | TemplateStringLiteral
    | RegularExpressionLiteral
    | numericLiteral
    ;

numericLiteral
    : DecimalLiteral
    | HexIntegerLiteral
    | OctalIntegerLiteral
    | BinaryIntegerLiteral
    ;

getter
    : GET propertyName
    ;

setter
    : SET propertyName
    ;

identifierName
    : Identifier
    | reservedWord
    ;

reservedWord
    : keyword
    | NullLiteral
    | BooleanLiteral
    ;

keyword
    : BREAK | CASE | CATCH | CLASS | CONST | CONTINUE | DEBUGGER | DEFAULT | DELETE | DO | ELSE | ENUM | EXPORT | EXTENDS | FINALLY | FOR | FUNCTION | IF | IMPORT | IN | INSTANCEOF | NEW | RETURN | SUPER | SWITCH | THIS | THROW | TRY | TYPEOF | VAR | VOID | WHILE | WITH | YIELD | ASYNC | AWAIT | LET | OF | STATIC
    ;

// Lexer Rules

// Keywords
BREAK           : 'break';
CASE            : 'case';
CATCH           : 'catch';
CLASS           : 'class';
CONST           : 'const';
CONTINUE        : 'continue';
DEBUGGER        : 'debugger';
DEFAULT         : 'default';
DELETE          : 'delete';
DO              : 'do';
ELSE            : 'else';
ENUM            : 'enum';
EXPORT          : 'export';
EXTENDS         : 'extends';
FINALLY         : 'finally';
FOR             : 'for';
FUNCTION        : 'function';
IF              : 'if';
IMPORT          : 'import';
IN              : 'in';
INSTANCEOF      : 'instanceof';
NEW             : 'new';
RETURN          : 'return';
SUPER           : 'super';
SWITCH          : 'switch';
THIS            : 'this';
THROW           : 'throw';
TRY             : 'try';
TYPEOF          : 'typeof';
VAR             : 'var';
VOID            : 'void';
WHILE           : 'while';
WITH            : 'with';
YIELD           : 'yield';
ASYNC           : 'async';
AWAIT           : 'await';
LET             : 'let';
OF              : 'of';
STATIC          : 'static';
GET             : 'get';
SET             : 'set';

// Literals
NullLiteral         : 'null';
BooleanLiteral      : 'true' | 'false';
DecimalLiteral      : DecimalIntegerLiteral '.' [0-9]* ExponentPart?
                    | '.' [0-9]+ ExponentPart?
                    | DecimalIntegerLiteral ExponentPart?
                    ;
HexIntegerLiteral   : '0' [xX] [0-9a-fA-F]+;
OctalIntegerLiteral : '0' [0-7]+;
BinaryIntegerLiteral: '0' [bB] [01]+;

StringLiteral       : '"' DoubleStringCharacter* '"'
                    | '\'' SingleStringCharacter* '\''
                    ;

TemplateStringLiteral: '`' ('\\' . | '${' .*? '}' | ~[\\`])* '`';

RegularExpressionLiteral: '/' RegularExpressionBody '/' RegularExpressionFlags?;

Identifier          : IdentifierStart IdentifierPart*;

// Operators
ASSIGN              : '=';
QUESTION            : '?';
COLON               : ':';
ARROW               : '=>';
ELLIPSIS            : '...';
DOT                 : '.';
PLUSPLUS            : '++';
MINUSMINUS          : '--';
PLUS                : '+';
MINUS               : '-';
BITNOT              : '~';
NOT                 : '!';
STAR                : '*';
SLASH               : '/';
MODULUS             : '%';
POWER               : '**';
NULLCOALESCE        : '??';

// Shift Operators
LSHIFT              : '<<';
RSHIFT              : '>>';
URSHIFT             : '>>>';

// Relational Operators
LT                  : '<';
GT                  : '>';
LE                  : '<=';
GE                  : '>=';

// Equality Operators
EQ                  : '==';
NE                  : '!=';
SEQ                 : '===';
SNE                 : '!==';

// Bitwise Operators
BITAND              : '&';
BITXOR              : '^';
BITOR               : '|';

// Logical Operators
AND                 : '&&';
OR                  : '||';

// Assignment Operators
STARASSIGN          : '*=';
SLASHASSIGN         : '/=';
MODULUSASSIGN       : '%=';
PLUSASSIGN          : '+=';
MINUSASSIGN         : '-=';
LSHIFTASSIGN        : '<<=';
RSHIFTASSIGN        : '>>=';
URSHIFTASSIGN       : '>>>=';
BITANDASSIGN        : '&=';
BITXORASSIGN        : '^=';
BITORASSIGN         : '|=';
POWERASSIGN         : '**=';

// Punctuation
SEMICOLON           : ';';
COMMA               : ',';
LPAREN              : '(';
RPAREN              : ')';
LBRACE              : '{';
RBRACE              : '}';
LBRACKET            : '[';
RBRACKET            : ']';

// Whitespace and Comments
WS                  : [ \t\r\n\u000C]+ -> channel(HIDDEN);
MultiLineComment    : '/*' .*? '*/' -> channel(HIDDEN);
SingleLineComment   : '//' ~[\r\n]* -> channel(HIDDEN);

// Fragments
fragment DoubleStringCharacter
    : ~["\\\r\n]
    | '\\' .
    ;

fragment SingleStringCharacter
    : ~['\\\r\n]
    | '\\' .
    ;

fragment RegularExpressionBody
    : RegularExpressionFirstChar RegularExpressionChar*
    ;

fragment RegularExpressionFirstChar
    : ~[*\r\n\u2028\u2029\\/[]
    | '\\' .
    | '[' RegularExpressionClassChar* ']'
    ;

fragment RegularExpressionChar
    : ~[\r\n\u2028\u2029\\/[]
    | '\\' .
    | '[' RegularExpressionClassChar* ']'
    ;

fragment RegularExpressionClassChar
    : ~[\r\n\u2028\u2029\\\]]
    | '\\' .
    ;

fragment RegularExpressionFlags
    : [gimsuvy]+
    ;

fragment DecimalIntegerLiteral
    : '0'
    | [1-9] [0-9]*
    ;

fragment ExponentPart
    : [eE] [+-]? [0-9]+
    ;

fragment IdentifierStart
    : [\p{L}]
    | [$_]
    ;

fragment IdentifierPart
    : IdentifierStart
    | [\p{Nd}]
    ;