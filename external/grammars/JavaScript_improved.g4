// Improved JavaScript Grammar based on official ANTLR grammars-v4
// Combined lexer and parser grammar for CHTL JavaScript compiler
// Adapted from: https://github.com/antlr/grammars-v4/tree/master/javascript

grammar JavaScript_improved;

// Parser Rules
program
    : sourceElements? EOF
    ;

sourceElements
    : sourceElement+
    ;

sourceElement
    : statement
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
    | functionDeclaration
    ;

block
    : '{' statementList? '}'
    ;

statementList
    : statement+
    ;

variableStatement
    : ('var' | 'let' | 'const') variableDeclarationList ';'?
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
    : {this.notOpenBraceAndNotFunction()}? expressionSequence ';'?
    ;

ifStatement
    : 'if' '(' expressionSequence ')' statement ('else' statement)?
    ;

iterationStatement
    : 'do' statement 'while' '(' expressionSequence ')' ';'?                                                                                  # DoStatement
    | 'while' '(' expressionSequence ')' statement                                                                                           # WhileStatement
    | 'for' '(' expressionSequence? ';' expressionSequence? ';' expressionSequence? ')' statement                                          # ForStatement
    | 'for' '(' ('var' | 'let' | 'const') variableDeclarationList ';' expressionSequence? ';' expressionSequence? ')' statement           # ForVarStatement
    | 'for' '(' singleExpression 'in' expressionSequence ')' statement                                                                      # ForInStatement
    | 'for' '(' ('var' | 'let' | 'const') variableDeclaration 'in' expressionSequence ')' statement                                       # ForVarInStatement
    | 'for' '(' singleExpression 'of' expressionSequence ')' statement                                                                      # ForOfStatement
    | 'for' '(' ('var' | 'let' | 'const') variableDeclaration 'of' expressionSequence ')' statement                                       # ForVarOfStatement
    ;

continueStatement
    : 'continue' ({this.notLineTerminator()}? Identifier)? ';'?
    ;

breakStatement
    : 'break' ({this.notLineTerminator()}? Identifier)? ';'?
    ;

returnStatement
    : 'return' ({this.notLineTerminator()}? expressionSequence)? ';'?
    ;

withStatement
    : 'with' '(' expressionSequence ')' statement
    ;

switchStatement
    : 'switch' '(' expressionSequence ')' caseBlock
    ;

caseBlock
    : '{' caseClauses? (defaultClause caseClauses?)? '}'
    ;

caseClauses
    : caseClause+
    ;

caseClause
    : 'case' expressionSequence ':' statementList?
    ;

defaultClause
    : 'default' ':' statementList?
    ;

labelledStatement
    : Identifier ':' statement
    ;

throwStatement
    : 'throw' {this.notLineTerminator()}? expressionSequence ';'?
    ;

tryStatement
    : 'try' block (catchProduction finallyProduction? | finallyProduction)
    ;

catchProduction
    : 'catch' ('(' Identifier ')')? block
    ;

finallyProduction
    : 'finally' block
    ;

debuggerStatement
    : 'debugger' ';'?
    ;

functionDeclaration
    : 'function' Identifier '(' formalParameterList? ')' functionBody
    ;

formalParameterList
    : formalParameterArg (',' formalParameterArg)*
    ;

formalParameterArg
    : Identifier ('=' singleExpression)?
    ;

functionBody
    : '{' sourceElements? '}'
    ;

expressionSequence
    : singleExpression (',' singleExpression)*
    ;

singleExpression
    : 'function' Identifier? '(' formalParameterList? ')' functionBody                                                                      # FunctionExpression
    | singleExpression '[' expressionSequence ']'                                                                                          # MemberIndexExpression
    | singleExpression '.' identifierName                                                                                                   # MemberDotExpression
    | singleExpression arguments                                                                                                            # CallExpression
    | 'new' singleExpression arguments?                                                                                                     # NewExpression
    | singleExpression '++'                                                                                                                 # PostIncrementExpression
    | singleExpression '--'                                                                                                                 # PostDecreaseExpression
    | 'delete' singleExpression                                                                                                             # DeleteExpression
    | 'void' singleExpression                                                                                                               # VoidExpression
    | 'typeof' singleExpression                                                                                                             # TypeofExpression
    | '++' singleExpression                                                                                                                 # PreIncrementExpression
    | '--' singleExpression                                                                                                                 # PreDecreaseExpression
    | '+' singleExpression                                                                                                                  # UnaryPlusExpression
    | '-' singleExpression                                                                                                                  # UnaryMinusExpression
    | '~' singleExpression                                                                                                                  # BitNotExpression
    | '!' singleExpression                                                                                                                  # NotExpression
    | singleExpression ('*' | '/' | '%') singleExpression                                                                                  # MultiplicativeExpression
    | singleExpression ('+' | '-') singleExpression                                                                                        # AdditiveExpression
    | singleExpression ('<<' | '>>' | '>>>') singleExpression                                                                              # BitShiftExpression
    | singleExpression ('<' | '>' | '<=' | '>=') singleExpression                                                                          # RelationalExpression
    | singleExpression 'instanceof' singleExpression                                                                                       # InstanceofExpression
    | singleExpression 'in' singleExpression                                                                                               # InExpression
    | singleExpression ('==' | '!=' | '===' | '!==') singleExpression                                                                     # EqualityExpression
    | singleExpression '&' singleExpression                                                                                                 # BitAndExpression
    | singleExpression '^' singleExpression                                                                                                 # BitXOrExpression
    | singleExpression '|' singleExpression                                                                                                 # BitOrExpression
    | singleExpression '&&' singleExpression                                                                                                # LogicalAndExpression
    | singleExpression '||' singleExpression                                                                                                # LogicalOrExpression
    | singleExpression '?' singleExpression ':' singleExpression                                                                           # TernaryExpression
    | singleExpression '=' singleExpression                                                                                                 # AssignmentExpression
    | singleExpression assignmentOperator singleExpression                                                                                 # AssignmentOperatorExpression
    | 'this'                                                                                                                                # ThisExpression
    | Identifier                                                                                                                            # IdentifierExpression
    | literal                                                                                                                               # LiteralExpression
    | arrayLiteral                                                                                                                          # ArrayLiteralExpression
    | objectLiteral                                                                                                                         # ObjectLiteralExpression
    | '(' expressionSequence ')'                                                                                                            # ParenthesizedExpression
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
    : NullLiteral
    | BooleanLiteral
    | StringLiteral
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
    | futureReservedWord
    | NullLiteral
    | BooleanLiteral
    ;

keyword
    : 'break'
    | 'do'
    | 'instanceof'
    | 'typeof'
    | 'case'
    | 'else'
    | 'new'
    | 'var'
    | 'catch'
    | 'finally'
    | 'return'
    | 'void'
    | 'continue'
    | 'for'
    | 'switch'
    | 'while'
    | 'debugger'
    | 'function'
    | 'this'
    | 'with'
    | 'default'
    | 'if'
    | 'throw'
    | 'delete'
    | 'in'
    | 'try'
    | 'let'
    | 'const'
    | 'of'
    ;

futureReservedWord
    : 'class'
    | 'enum'
    | 'extends'
    | 'super'
    | 'const'
    | 'export'
    | 'import'
    | 'implements'
    | 'interface'
    | 'package'
    | 'private'
    | 'protected'
    | 'public'
    | 'static'
    | 'yield'
    ;

arguments
    : '(' (argumentList)? ')'
    ;

argumentList
    : singleExpression (',' singleExpression)*
    ;

arrayLiteral
    : '[' elementList? ']'
    ;

elementList
    : arrayElement (',' arrayElement)*
    ;

arrayElement
    : singleExpression?
    ;

objectLiteral
    : '{' propertyNameAndValueList? ','? '}'
    ;

propertyNameAndValueList
    : propertyAssignment (',' propertyAssignment)*
    ;

propertyAssignment
    : propertyName ':' singleExpression                                                                                                     # PropertyExpressionAssignment
    | 'get' propertyName '(' ')' functionBody                                                                                              # PropertyGetter
    | 'set' propertyName '(' Identifier ')' functionBody                                                                                   # PropertySetter
    ;

propertyName
    : identifierName
    | StringLiteral
    | numericLiteral
    ;

// Lexer Rules
MultiLineComment : '/*' .*? '*/' -> channel(HIDDEN);
SingleLineComment : '//' ~[\r\n\u2028\u2029]* -> channel(HIDDEN);
RegularExpressionLiteral : '/' RegularExpressionBody '/' RegularExpressionFlags;

// Literals
NullLiteral : 'null';
BooleanLiteral : 'true' | 'false';
DecimalLiteral : DecimalIntegerLiteral '.' [0-9]* ExponentPart? | '.' [0-9]+ ExponentPart? | DecimalIntegerLiteral ExponentPart?;

HexIntegerLiteral : '0' [xX] [0-9a-fA-F]+;
OctalIntegerLiteral : '0' [0-7]+;

fragment DecimalIntegerLiteral : '0' | [1-9] [0-9]*;
fragment ExponentPart : [eE] [+-]? [0-9]+;

// String Literals
StringLiteral
    : '"' DoubleStringCharacter* '"'
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

fragment SingleEscapeCharacter
    : ['"\\bfnrtv]
    ;

fragment NonEscapeCharacter
    : ~['"\\bfnrtv0-9xu\r\n]
    ;

fragment EscapeCharacter
    : SingleEscapeCharacter
    | [0-9]
    | [xu]
    ;

fragment LineContinuation
    : '\\' [\r\n\u2028\u2029]
    ;

fragment HexEscapeSequence
    : 'x' [0-9a-fA-F] [0-9a-fA-F]
    ;

fragment UnicodeEscapeSequence
    : 'u' [0-9a-fA-F] [0-9a-fA-F] [0-9a-fA-F] [0-9a-fA-F]
    ;

// Regular Expression
fragment RegularExpressionBody
    : RegularExpressionFirstChar RegularExpressionChar*
    ;

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

fragment RegularExpressionClass
    : '[' RegularExpressionClassChar* ']'
    ;

fragment RegularExpressionClassChar
    : ~[\r\n\u2028\u2029\\\]]
    | RegularExpressionBackslashSequence
    ;

fragment RegularExpressionBackslashSequence
    : '\\' ~[\r\n\u2028\u2029]
    ;

fragment RegularExpressionFlags
    : [gimuy]*
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
Ellipsis : '...';
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
RightShiftArithmetic : '>>';
LeftShiftArithmetic : '<<';
RightShiftLogical : '>>>';
LessThan : '<';
MoreThan : '>';
LessThanEquals : '<=';
GreaterThanEquals : '>=';
Equals_ : '==';
NotEquals : '!=';
IdentityEquals : '===';
IdentityNotEquals : '!==';
BitAnd : '&';
BitXOr : '^';
BitOr : '|';
And : '&&';
Or : '||';
MultiplyAssign : '*=';
DivideAssign : '/=';
ModulusAssign : '%=';
PlusAssign : '+=';
MinusAssign : '-=';
LeftShiftArithmeticAssign : '<<=';
RightShiftArithmeticAssign : '>>=';
RightShiftLogicalAssign : '>>>=';
BitAndAssign : '&=';
BitXorAssign : '^=';
BitOrAssign : '|=';
Arrow : '=>';