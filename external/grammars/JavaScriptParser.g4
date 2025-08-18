// JavaScript Parser Grammar
parser grammar JavaScriptParser;

options {
    tokenVocab = JavaScriptLexer;
}

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
    : LBRACE statementList? RBRACE
    ;

statementList
    : statement+
    ;

variableStatement
    : VAR variableDeclarationList SEMICOLON?
    ;

variableDeclarationList
    : variableDeclaration ( COMMA variableDeclaration )*
    ;

variableDeclaration
    : Identifier initializer?
    ;

initializer
    : ASSIGN singleExpression
    ;

emptyStatement
    : SEMICOLON
    ;

expressionStatement
    : expressionSequence SEMICOLON?
    ;

ifStatement
    : IF LPAREN expressionSequence RPAREN statement ( ELSE statement )?
    ;

iterationStatement
    : DO statement WHILE LPAREN expressionSequence RPAREN SEMICOLON?
    | WHILE LPAREN expressionSequence RPAREN statement
    | FOR LPAREN expressionSequence? SEMICOLON expressionSequence? SEMICOLON expressionSequence? RPAREN statement
    | FOR LPAREN VAR variableDeclarationList SEMICOLON expressionSequence? SEMICOLON expressionSequence? RPAREN statement
    | FOR LPAREN singleExpression IN expressionSequence RPAREN statement
    | FOR LPAREN VAR variableDeclaration IN expressionSequence RPAREN statement
    ;

continueStatement
    : CONTINUE Identifier? SEMICOLON?
    ;

breakStatement
    : BREAK Identifier? SEMICOLON?
    ;

returnStatement
    : RETURN expressionSequence? SEMICOLON?
    ;

withStatement
    : WITH LPAREN expressionSequence RPAREN statement
    ;

throwStatement
    : THROW expressionSequence SEMICOLON?
    ;

tryStatement
    : TRY block ( catchProduction finallyProduction? | finallyProduction )
    ;

catchProduction
    : CATCH LPAREN Identifier RPAREN block
    ;

finallyProduction
    : FINALLY block
    ;

functionDeclaration
    : FUNCTION Identifier LPAREN formalParameterList? RPAREN LBRACE functionBody RBRACE
    ;

formalParameterList
    : Identifier ( COMMA Identifier )*
    ;

functionBody
    : sourceElements?
    ;

singleExpression
    : FUNCTION Identifier? LPAREN formalParameterList? RPAREN LBRACE functionBody RBRACE                    # FunctionExpression
    | singleExpression LBRACKET expressionSequence RBRACKET                                              # MemberIndexExpression
    | singleExpression DOT identifierName                                                       # MemberDotExpression
    | singleExpression arguments                                                                # CallExpression
    | NEW singleExpression arguments?                                                           # NewExpression
    | singleExpression PLUS_PLUS                                                                     # PostIncrementExpression
    | singleExpression MINUS_MINUS                                                                     # PostDecreaseExpression
    | DELETE singleExpression                                                                   # DeleteExpression
    | VOID singleExpression                                                                     # VoidExpression
    | TYPEOF singleExpression                                                                   # TypeofExpression
    | PLUS_PLUS singleExpression                                                                     # PreIncrementExpression
    | MINUS_MINUS singleExpression                                                                     # PreDecreaseExpression
    | PLUS singleExpression                                                                      # UnaryPlusExpression
    | MINUS singleExpression                                                                      # UnaryMinusExpression
    | TILDE singleExpression                                                                      # BitNotExpression
    | NOT singleExpression                                                                      # NotExpression
    | singleExpression ( MULTIPLY | DIVIDE | MODULO ) singleExpression                                    # MultiplicativeExpression
    | singleExpression ( PLUS | MINUS ) singleExpression                                          # AdditiveExpression
    | singleExpression ( LSHIFT | RSHIFT | URSHIFT ) singleExpression                                # BitShiftExpression
    | singleExpression ( LT | GT | LE | GE ) singleExpression                           # RelationalExpression
    | singleExpression INSTANCEOF singleExpression                                              # InstanceofExpression
    | singleExpression IN singleExpression                                                      # InExpression
    | singleExpression ( EQ | NE | STRICT_EQ | STRICT_NE ) singleExpression                       # EqualityExpression
    | singleExpression BIT_AND singleExpression                                                     # BitAndExpression
    | singleExpression BIT_XOR singleExpression                                                     # BitXOrExpression
    | singleExpression BIT_OR singleExpression                                                     # BitOrExpression
    | singleExpression AND singleExpression                                                    # LogicalAndExpression
    | singleExpression OR singleExpression                                                    # LogicalOrExpression
    | singleExpression QUESTION singleExpression COLON singleExpression                               # TernaryExpression
    | singleExpression ASSIGN singleExpression                                                     # AssignmentExpression
    | singleExpression assignmentOperator singleExpression                                     # AssignmentOperatorExpression
    | THIS                                                                                      # ThisExpression
    | Identifier                                                                                # IdentifierExpression
    | literal                                                                                   # LiteralExpression
    | arrayLiteral                                                                              # ArrayLiteralExpression
    | objectLiteral                                                                             # ObjectLiteralExpression
    | LPAREN expressionSequence RPAREN                                                               # ParenthesizedExpression
    ;

assignmentOperator
    : MULTIPLY_ASSIGN | DIVIDE_ASSIGN | MODULO_ASSIGN | PLUS_ASSIGN | MINUS_ASSIGN | LSHIFT_ASSIGN | RSHIFT_ASSIGN | URSHIFT_ASSIGN | BIT_AND_ASSIGN | BIT_XOR_ASSIGN | BIT_OR_ASSIGN
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
    : LBRACKET elementList? RBRACKET
    ;

elementList
    : singleExpression ( COMMA singleExpression )*
    ;

objectLiteral
    : LBRACE propertyNameAndValueList? RBRACE
    ;

propertyNameAndValueList
    : propertyAssignment ( COMMA propertyAssignment )*
    ;

propertyAssignment
    : propertyName COLON singleExpression
    | GET propertyName LPAREN RPAREN LBRACE functionBody RBRACE
    | SET propertyName LPAREN propertySetParameterList RPAREN LBRACE functionBody RBRACE
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
    : LPAREN argumentList? RPAREN
    ;

argumentList
    : singleExpression ( COMMA singleExpression )*
    ;

expressionSequence
    : singleExpression ( COMMA singleExpression )*
    ;