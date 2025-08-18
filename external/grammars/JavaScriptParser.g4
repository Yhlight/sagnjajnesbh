// CHTL JavaScript Parser Grammar
// Based on official ANTLR grammars-v4 JavaScript grammar
// Simplified for CHTL compiler integration

parser grammar JavaScriptParser;

options {
    tokenVocab = JavaScriptLexer;
}

program: sourceElements? EOF;

sourceElement: statement;

sourceElements: sourceElement+;

statement: block
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
         | classDeclaration;

block: OpenBrace statementList? CloseBrace;

statementList: statement+;

variableStatement: (Var | Let | Const) variableDeclarationList SemiColon?;

variableDeclarationList: variableDeclaration (Comma variableDeclaration)*;

variableDeclaration: Identifier (Assign singleExpression)?;

emptyStatement: SemiColon;

expressionStatement: expressionSequence SemiColon?;

ifStatement: If OpenParen expressionSequence CloseParen statement (Else statement)?;

iterationStatement: Do statement While OpenParen expressionSequence CloseParen SemiColon?
                  | While OpenParen expressionSequence CloseParen statement
                  | For OpenParen (expressionSequence | variableDeclarationList)? SemiColon expressionSequence? SemiColon expressionSequence? CloseParen statement
                  | For OpenParen (singleExpression | variableDeclarationList) In expressionSequence CloseParen statement;

continueStatement: Continue Identifier? SemiColon?;

breakStatement: Break Identifier? SemiColon?;

returnStatement: Return expressionSequence? SemiColon?;

withStatement: With OpenParen expressionSequence CloseParen statement;

switchStatement: Switch OpenParen expressionSequence CloseParen caseBlock;

caseBlock: OpenBrace caseClauses? (defaultClause caseClauses?)? CloseBrace;

caseClauses: caseClause+;

caseClause: Case expressionSequence Colon statementList?;

defaultClause: Default Colon statementList?;

labelledStatement: Identifier Colon statement;

throwStatement: Throw expressionSequence SemiColon?;

tryStatement: Try block (catchProduction finallyProduction? | finallyProduction);

catchProduction: Catch OpenParen Identifier CloseParen block;

finallyProduction: Finally block;

debuggerStatement: Debugger SemiColon?;

functionDeclaration: Function Identifier OpenParen formalParameterList? CloseParen OpenBrace functionBody CloseBrace;

classDeclaration: Class Identifier (Extends singleExpression)? OpenBrace classBody CloseBrace;

classBody: classElement*;

classElement: (Static)? methodDefinition
            | emptyStatement;

methodDefinition: propertyName OpenParen formalParameterList? CloseParen OpenBrace functionBody CloseBrace;

formalParameterList: Identifier (Comma Identifier)*;

functionBody: sourceElements?;

arrayLiteral: OpenBracket elementList? CloseBracket;

elementList: singleExpression (Comma singleExpression)*;

objectLiteral: OpenBrace (propertyAssignment (Comma propertyAssignment)*)? Comma? CloseBrace;

propertyAssignment: propertyName Colon singleExpression
                  | propertyName OpenParen formalParameterList? CloseParen OpenBrace functionBody CloseBrace;

propertyName: identifierName
            | StringLiteral
            | numericLiteral;

arguments: OpenParen (singleExpression (Comma singleExpression)*)? CloseParen;

expressionSequence: singleExpression (Comma singleExpression)*;

singleExpression: Function Identifier? OpenParen formalParameterList? CloseParen OpenBrace functionBody CloseBrace
                | singleExpression OpenBracket expressionSequence CloseBracket
                | singleExpression Dot identifierName
                | singleExpression arguments
                | New singleExpression arguments?
                | singleExpression PlusPlus
                | singleExpression MinusMinus
                | Delete singleExpression
                | Void singleExpression
                | Typeof singleExpression
                | PlusPlus singleExpression
                | MinusMinus singleExpression
                | Plus singleExpression
                | Minus singleExpression
                | BitNot singleExpression
                | Not singleExpression
                | singleExpression (Multiply | Divide | Modulus) singleExpression
                | singleExpression (Plus | Minus) singleExpression
                | singleExpression (LeftShiftArithmetic | RightShiftArithmetic | RightShiftLogical) singleExpression
                | singleExpression (LessThan | MoreThan | LessThanEquals | GreaterThanEquals) singleExpression
                | singleExpression Instanceof singleExpression
                | singleExpression In singleExpression
                | singleExpression (Equals_ | NotEquals | IdentityEquals | IdentityNotEquals) singleExpression
                | singleExpression BitAnd singleExpression
                | singleExpression BitXOr singleExpression
                | singleExpression BitOr singleExpression
                | singleExpression And singleExpression
                | singleExpression Or singleExpression
                | singleExpression QuestionMark singleExpression Colon singleExpression
                | singleExpression Assign singleExpression
                | singleExpression assignmentOperator singleExpression
                | This
                | Identifier
                | Super
                | literal
                | arrayLiteral
                | objectLiteral
                | OpenParen expressionSequence CloseParen;

assignmentOperator: MultiplyAssign
                  | DivideAssign
                  | ModulusAssign
                  | PlusAssign
                  | MinusAssign
                  | LeftShiftArithmeticAssign
                  | RightShiftArithmeticAssign
                  | RightShiftLogicalAssign
                  | BitAndAssign
                  | BitXorAssign
                  | BitOrAssign;

literal: NullLiteral
       | BooleanLiteral
       | numericLiteral
       | StringLiteral
       | RegularExpressionLiteral;

numericLiteral: DecimalLiteral
              | HexIntegerLiteral
              | OctalIntegerLiteral
              | OctalIntegerLiteral2
              | BinaryIntegerLiteral;

identifierName: Identifier
              | reservedWord;

reservedWord: keyword
            | futureReservedWord
            | NullLiteral
            | BooleanLiteral;

keyword: Break
       | Do
       | Instanceof
       | Typeof
       | Case
       | Else
       | New
       | Var
       | Catch
       | Finally
       | Return
       | Void
       | Continue
       | For
       | Switch
       | While
       | Debugger
       | Function
       | This
       | With
       | Default
       | If
       | Throw
       | Delete
       | In
       | Try
       | Class
       | Enum
       | Extends
       | Super
       | Const
       | Export
       | Import
       | Let
       | Static
       | Yield
       | Async
       | Await;

futureReservedWord: Implements
                  | Interface
                  | Package
                  | Private
                  | Protected
                  | Public;