
// Generated from external/grammars/JavaScript.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "JavaScriptListener.h"


/**
 * This class provides an empty implementation of JavaScriptListener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class  JavaScriptBaseListener : public JavaScriptListener {
public:

  virtual void enterProgram(JavaScriptParser::ProgramContext * /*ctx*/) override { }
  virtual void exitProgram(JavaScriptParser::ProgramContext * /*ctx*/) override { }

  virtual void enterSourceElements(JavaScriptParser::SourceElementsContext * /*ctx*/) override { }
  virtual void exitSourceElements(JavaScriptParser::SourceElementsContext * /*ctx*/) override { }

  virtual void enterSourceElement(JavaScriptParser::SourceElementContext * /*ctx*/) override { }
  virtual void exitSourceElement(JavaScriptParser::SourceElementContext * /*ctx*/) override { }

  virtual void enterStatement(JavaScriptParser::StatementContext * /*ctx*/) override { }
  virtual void exitStatement(JavaScriptParser::StatementContext * /*ctx*/) override { }

  virtual void enterBlock(JavaScriptParser::BlockContext * /*ctx*/) override { }
  virtual void exitBlock(JavaScriptParser::BlockContext * /*ctx*/) override { }

  virtual void enterStatementList(JavaScriptParser::StatementListContext * /*ctx*/) override { }
  virtual void exitStatementList(JavaScriptParser::StatementListContext * /*ctx*/) override { }

  virtual void enterVariableStatement(JavaScriptParser::VariableStatementContext * /*ctx*/) override { }
  virtual void exitVariableStatement(JavaScriptParser::VariableStatementContext * /*ctx*/) override { }

  virtual void enterVariableDeclarationList(JavaScriptParser::VariableDeclarationListContext * /*ctx*/) override { }
  virtual void exitVariableDeclarationList(JavaScriptParser::VariableDeclarationListContext * /*ctx*/) override { }

  virtual void enterVariableDeclaration(JavaScriptParser::VariableDeclarationContext * /*ctx*/) override { }
  virtual void exitVariableDeclaration(JavaScriptParser::VariableDeclarationContext * /*ctx*/) override { }

  virtual void enterEmptyStatement(JavaScriptParser::EmptyStatementContext * /*ctx*/) override { }
  virtual void exitEmptyStatement(JavaScriptParser::EmptyStatementContext * /*ctx*/) override { }

  virtual void enterExpressionStatement(JavaScriptParser::ExpressionStatementContext * /*ctx*/) override { }
  virtual void exitExpressionStatement(JavaScriptParser::ExpressionStatementContext * /*ctx*/) override { }

  virtual void enterIfStatement(JavaScriptParser::IfStatementContext * /*ctx*/) override { }
  virtual void exitIfStatement(JavaScriptParser::IfStatementContext * /*ctx*/) override { }

  virtual void enterIterationStatement(JavaScriptParser::IterationStatementContext * /*ctx*/) override { }
  virtual void exitIterationStatement(JavaScriptParser::IterationStatementContext * /*ctx*/) override { }

  virtual void enterContinueStatement(JavaScriptParser::ContinueStatementContext * /*ctx*/) override { }
  virtual void exitContinueStatement(JavaScriptParser::ContinueStatementContext * /*ctx*/) override { }

  virtual void enterBreakStatement(JavaScriptParser::BreakStatementContext * /*ctx*/) override { }
  virtual void exitBreakStatement(JavaScriptParser::BreakStatementContext * /*ctx*/) override { }

  virtual void enterReturnStatement(JavaScriptParser::ReturnStatementContext * /*ctx*/) override { }
  virtual void exitReturnStatement(JavaScriptParser::ReturnStatementContext * /*ctx*/) override { }

  virtual void enterWithStatement(JavaScriptParser::WithStatementContext * /*ctx*/) override { }
  virtual void exitWithStatement(JavaScriptParser::WithStatementContext * /*ctx*/) override { }

  virtual void enterSwitchStatement(JavaScriptParser::SwitchStatementContext * /*ctx*/) override { }
  virtual void exitSwitchStatement(JavaScriptParser::SwitchStatementContext * /*ctx*/) override { }

  virtual void enterCaseBlock(JavaScriptParser::CaseBlockContext * /*ctx*/) override { }
  virtual void exitCaseBlock(JavaScriptParser::CaseBlockContext * /*ctx*/) override { }

  virtual void enterCaseClauses(JavaScriptParser::CaseClausesContext * /*ctx*/) override { }
  virtual void exitCaseClauses(JavaScriptParser::CaseClausesContext * /*ctx*/) override { }

  virtual void enterCaseClause(JavaScriptParser::CaseClauseContext * /*ctx*/) override { }
  virtual void exitCaseClause(JavaScriptParser::CaseClauseContext * /*ctx*/) override { }

  virtual void enterDefaultClause(JavaScriptParser::DefaultClauseContext * /*ctx*/) override { }
  virtual void exitDefaultClause(JavaScriptParser::DefaultClauseContext * /*ctx*/) override { }

  virtual void enterLabelledStatement(JavaScriptParser::LabelledStatementContext * /*ctx*/) override { }
  virtual void exitLabelledStatement(JavaScriptParser::LabelledStatementContext * /*ctx*/) override { }

  virtual void enterThrowStatement(JavaScriptParser::ThrowStatementContext * /*ctx*/) override { }
  virtual void exitThrowStatement(JavaScriptParser::ThrowStatementContext * /*ctx*/) override { }

  virtual void enterTryStatement(JavaScriptParser::TryStatementContext * /*ctx*/) override { }
  virtual void exitTryStatement(JavaScriptParser::TryStatementContext * /*ctx*/) override { }

  virtual void enterCatchProduction(JavaScriptParser::CatchProductionContext * /*ctx*/) override { }
  virtual void exitCatchProduction(JavaScriptParser::CatchProductionContext * /*ctx*/) override { }

  virtual void enterFinallyProduction(JavaScriptParser::FinallyProductionContext * /*ctx*/) override { }
  virtual void exitFinallyProduction(JavaScriptParser::FinallyProductionContext * /*ctx*/) override { }

  virtual void enterDebuggerStatement(JavaScriptParser::DebuggerStatementContext * /*ctx*/) override { }
  virtual void exitDebuggerStatement(JavaScriptParser::DebuggerStatementContext * /*ctx*/) override { }

  virtual void enterFunctionDeclaration(JavaScriptParser::FunctionDeclarationContext * /*ctx*/) override { }
  virtual void exitFunctionDeclaration(JavaScriptParser::FunctionDeclarationContext * /*ctx*/) override { }

  virtual void enterFormalParameterList(JavaScriptParser::FormalParameterListContext * /*ctx*/) override { }
  virtual void exitFormalParameterList(JavaScriptParser::FormalParameterListContext * /*ctx*/) override { }

  virtual void enterFunctionBody(JavaScriptParser::FunctionBodyContext * /*ctx*/) override { }
  virtual void exitFunctionBody(JavaScriptParser::FunctionBodyContext * /*ctx*/) override { }

  virtual void enterExpressionSequence(JavaScriptParser::ExpressionSequenceContext * /*ctx*/) override { }
  virtual void exitExpressionSequence(JavaScriptParser::ExpressionSequenceContext * /*ctx*/) override { }

  virtual void enterTernaryExpression(JavaScriptParser::TernaryExpressionContext * /*ctx*/) override { }
  virtual void exitTernaryExpression(JavaScriptParser::TernaryExpressionContext * /*ctx*/) override { }

  virtual void enterLogicalAndExpression(JavaScriptParser::LogicalAndExpressionContext * /*ctx*/) override { }
  virtual void exitLogicalAndExpression(JavaScriptParser::LogicalAndExpressionContext * /*ctx*/) override { }

  virtual void enterPreIncrementExpression(JavaScriptParser::PreIncrementExpressionContext * /*ctx*/) override { }
  virtual void exitPreIncrementExpression(JavaScriptParser::PreIncrementExpressionContext * /*ctx*/) override { }

  virtual void enterObjectLiteralExpression(JavaScriptParser::ObjectLiteralExpressionContext * /*ctx*/) override { }
  virtual void exitObjectLiteralExpression(JavaScriptParser::ObjectLiteralExpressionContext * /*ctx*/) override { }

  virtual void enterInExpression(JavaScriptParser::InExpressionContext * /*ctx*/) override { }
  virtual void exitInExpression(JavaScriptParser::InExpressionContext * /*ctx*/) override { }

  virtual void enterLogicalOrExpression(JavaScriptParser::LogicalOrExpressionContext * /*ctx*/) override { }
  virtual void exitLogicalOrExpression(JavaScriptParser::LogicalOrExpressionContext * /*ctx*/) override { }

  virtual void enterNotExpression(JavaScriptParser::NotExpressionContext * /*ctx*/) override { }
  virtual void exitNotExpression(JavaScriptParser::NotExpressionContext * /*ctx*/) override { }

  virtual void enterPreDecreaseExpression(JavaScriptParser::PreDecreaseExpressionContext * /*ctx*/) override { }
  virtual void exitPreDecreaseExpression(JavaScriptParser::PreDecreaseExpressionContext * /*ctx*/) override { }

  virtual void enterThisExpression(JavaScriptParser::ThisExpressionContext * /*ctx*/) override { }
  virtual void exitThisExpression(JavaScriptParser::ThisExpressionContext * /*ctx*/) override { }

  virtual void enterFunctionExpression(JavaScriptParser::FunctionExpressionContext * /*ctx*/) override { }
  virtual void exitFunctionExpression(JavaScriptParser::FunctionExpressionContext * /*ctx*/) override { }

  virtual void enterUnaryMinusExpression(JavaScriptParser::UnaryMinusExpressionContext * /*ctx*/) override { }
  virtual void exitUnaryMinusExpression(JavaScriptParser::UnaryMinusExpressionContext * /*ctx*/) override { }

  virtual void enterAssignmentExpression(JavaScriptParser::AssignmentExpressionContext * /*ctx*/) override { }
  virtual void exitAssignmentExpression(JavaScriptParser::AssignmentExpressionContext * /*ctx*/) override { }

  virtual void enterPostDecreaseExpression(JavaScriptParser::PostDecreaseExpressionContext * /*ctx*/) override { }
  virtual void exitPostDecreaseExpression(JavaScriptParser::PostDecreaseExpressionContext * /*ctx*/) override { }

  virtual void enterTypeofExpression(JavaScriptParser::TypeofExpressionContext * /*ctx*/) override { }
  virtual void exitTypeofExpression(JavaScriptParser::TypeofExpressionContext * /*ctx*/) override { }

  virtual void enterInstanceofExpression(JavaScriptParser::InstanceofExpressionContext * /*ctx*/) override { }
  virtual void exitInstanceofExpression(JavaScriptParser::InstanceofExpressionContext * /*ctx*/) override { }

  virtual void enterUnaryPlusExpression(JavaScriptParser::UnaryPlusExpressionContext * /*ctx*/) override { }
  virtual void exitUnaryPlusExpression(JavaScriptParser::UnaryPlusExpressionContext * /*ctx*/) override { }

  virtual void enterDeleteExpression(JavaScriptParser::DeleteExpressionContext * /*ctx*/) override { }
  virtual void exitDeleteExpression(JavaScriptParser::DeleteExpressionContext * /*ctx*/) override { }

  virtual void enterEqualityExpression(JavaScriptParser::EqualityExpressionContext * /*ctx*/) override { }
  virtual void exitEqualityExpression(JavaScriptParser::EqualityExpressionContext * /*ctx*/) override { }

  virtual void enterBitXOrExpression(JavaScriptParser::BitXOrExpressionContext * /*ctx*/) override { }
  virtual void exitBitXOrExpression(JavaScriptParser::BitXOrExpressionContext * /*ctx*/) override { }

  virtual void enterMultiplicativeExpression(JavaScriptParser::MultiplicativeExpressionContext * /*ctx*/) override { }
  virtual void exitMultiplicativeExpression(JavaScriptParser::MultiplicativeExpressionContext * /*ctx*/) override { }

  virtual void enterCallExpression(JavaScriptParser::CallExpressionContext * /*ctx*/) override { }
  virtual void exitCallExpression(JavaScriptParser::CallExpressionContext * /*ctx*/) override { }

  virtual void enterBitShiftExpression(JavaScriptParser::BitShiftExpressionContext * /*ctx*/) override { }
  virtual void exitBitShiftExpression(JavaScriptParser::BitShiftExpressionContext * /*ctx*/) override { }

  virtual void enterParenthesizedExpression(JavaScriptParser::ParenthesizedExpressionContext * /*ctx*/) override { }
  virtual void exitParenthesizedExpression(JavaScriptParser::ParenthesizedExpressionContext * /*ctx*/) override { }

  virtual void enterAdditiveExpression(JavaScriptParser::AdditiveExpressionContext * /*ctx*/) override { }
  virtual void exitAdditiveExpression(JavaScriptParser::AdditiveExpressionContext * /*ctx*/) override { }

  virtual void enterRelationalExpression(JavaScriptParser::RelationalExpressionContext * /*ctx*/) override { }
  virtual void exitRelationalExpression(JavaScriptParser::RelationalExpressionContext * /*ctx*/) override { }

  virtual void enterPostIncrementExpression(JavaScriptParser::PostIncrementExpressionContext * /*ctx*/) override { }
  virtual void exitPostIncrementExpression(JavaScriptParser::PostIncrementExpressionContext * /*ctx*/) override { }

  virtual void enterBitNotExpression(JavaScriptParser::BitNotExpressionContext * /*ctx*/) override { }
  virtual void exitBitNotExpression(JavaScriptParser::BitNotExpressionContext * /*ctx*/) override { }

  virtual void enterNewExpression(JavaScriptParser::NewExpressionContext * /*ctx*/) override { }
  virtual void exitNewExpression(JavaScriptParser::NewExpressionContext * /*ctx*/) override { }

  virtual void enterLiteralExpression(JavaScriptParser::LiteralExpressionContext * /*ctx*/) override { }
  virtual void exitLiteralExpression(JavaScriptParser::LiteralExpressionContext * /*ctx*/) override { }

  virtual void enterArrayLiteralExpression(JavaScriptParser::ArrayLiteralExpressionContext * /*ctx*/) override { }
  virtual void exitArrayLiteralExpression(JavaScriptParser::ArrayLiteralExpressionContext * /*ctx*/) override { }

  virtual void enterMemberDotExpression(JavaScriptParser::MemberDotExpressionContext * /*ctx*/) override { }
  virtual void exitMemberDotExpression(JavaScriptParser::MemberDotExpressionContext * /*ctx*/) override { }

  virtual void enterMemberIndexExpression(JavaScriptParser::MemberIndexExpressionContext * /*ctx*/) override { }
  virtual void exitMemberIndexExpression(JavaScriptParser::MemberIndexExpressionContext * /*ctx*/) override { }

  virtual void enterIdentifierExpression(JavaScriptParser::IdentifierExpressionContext * /*ctx*/) override { }
  virtual void exitIdentifierExpression(JavaScriptParser::IdentifierExpressionContext * /*ctx*/) override { }

  virtual void enterBitAndExpression(JavaScriptParser::BitAndExpressionContext * /*ctx*/) override { }
  virtual void exitBitAndExpression(JavaScriptParser::BitAndExpressionContext * /*ctx*/) override { }

  virtual void enterBitOrExpression(JavaScriptParser::BitOrExpressionContext * /*ctx*/) override { }
  virtual void exitBitOrExpression(JavaScriptParser::BitOrExpressionContext * /*ctx*/) override { }

  virtual void enterAssignmentOperatorExpression(JavaScriptParser::AssignmentOperatorExpressionContext * /*ctx*/) override { }
  virtual void exitAssignmentOperatorExpression(JavaScriptParser::AssignmentOperatorExpressionContext * /*ctx*/) override { }

  virtual void enterVoidExpression(JavaScriptParser::VoidExpressionContext * /*ctx*/) override { }
  virtual void exitVoidExpression(JavaScriptParser::VoidExpressionContext * /*ctx*/) override { }

  virtual void enterAssignmentOperator(JavaScriptParser::AssignmentOperatorContext * /*ctx*/) override { }
  virtual void exitAssignmentOperator(JavaScriptParser::AssignmentOperatorContext * /*ctx*/) override { }

  virtual void enterNullLiteral(JavaScriptParser::NullLiteralContext * /*ctx*/) override { }
  virtual void exitNullLiteral(JavaScriptParser::NullLiteralContext * /*ctx*/) override { }

  virtual void enterBooleanLiteral(JavaScriptParser::BooleanLiteralContext * /*ctx*/) override { }
  virtual void exitBooleanLiteral(JavaScriptParser::BooleanLiteralContext * /*ctx*/) override { }

  virtual void enterStringLiteral(JavaScriptParser::StringLiteralContext * /*ctx*/) override { }
  virtual void exitStringLiteral(JavaScriptParser::StringLiteralContext * /*ctx*/) override { }

  virtual void enterNumericLiteralExpression(JavaScriptParser::NumericLiteralExpressionContext * /*ctx*/) override { }
  virtual void exitNumericLiteralExpression(JavaScriptParser::NumericLiteralExpressionContext * /*ctx*/) override { }

  virtual void enterNumericLiteral(JavaScriptParser::NumericLiteralContext * /*ctx*/) override { }
  virtual void exitNumericLiteral(JavaScriptParser::NumericLiteralContext * /*ctx*/) override { }

  virtual void enterIdentifierName(JavaScriptParser::IdentifierNameContext * /*ctx*/) override { }
  virtual void exitIdentifierName(JavaScriptParser::IdentifierNameContext * /*ctx*/) override { }

  virtual void enterReservedWord(JavaScriptParser::ReservedWordContext * /*ctx*/) override { }
  virtual void exitReservedWord(JavaScriptParser::ReservedWordContext * /*ctx*/) override { }

  virtual void enterKeyword(JavaScriptParser::KeywordContext * /*ctx*/) override { }
  virtual void exitKeyword(JavaScriptParser::KeywordContext * /*ctx*/) override { }

  virtual void enterArrayLiteral(JavaScriptParser::ArrayLiteralContext * /*ctx*/) override { }
  virtual void exitArrayLiteral(JavaScriptParser::ArrayLiteralContext * /*ctx*/) override { }

  virtual void enterElementList(JavaScriptParser::ElementListContext * /*ctx*/) override { }
  virtual void exitElementList(JavaScriptParser::ElementListContext * /*ctx*/) override { }

  virtual void enterElision(JavaScriptParser::ElisionContext * /*ctx*/) override { }
  virtual void exitElision(JavaScriptParser::ElisionContext * /*ctx*/) override { }

  virtual void enterObjectLiteral(JavaScriptParser::ObjectLiteralContext * /*ctx*/) override { }
  virtual void exitObjectLiteral(JavaScriptParser::ObjectLiteralContext * /*ctx*/) override { }

  virtual void enterPropertyNameAndValueList(JavaScriptParser::PropertyNameAndValueListContext * /*ctx*/) override { }
  virtual void exitPropertyNameAndValueList(JavaScriptParser::PropertyNameAndValueListContext * /*ctx*/) override { }

  virtual void enterPropertyExpressionAssignment(JavaScriptParser::PropertyExpressionAssignmentContext * /*ctx*/) override { }
  virtual void exitPropertyExpressionAssignment(JavaScriptParser::PropertyExpressionAssignmentContext * /*ctx*/) override { }

  virtual void enterPropertyGetter(JavaScriptParser::PropertyGetterContext * /*ctx*/) override { }
  virtual void exitPropertyGetter(JavaScriptParser::PropertyGetterContext * /*ctx*/) override { }

  virtual void enterPropertySetter(JavaScriptParser::PropertySetterContext * /*ctx*/) override { }
  virtual void exitPropertySetter(JavaScriptParser::PropertySetterContext * /*ctx*/) override { }

  virtual void enterPropertyName(JavaScriptParser::PropertyNameContext * /*ctx*/) override { }
  virtual void exitPropertyName(JavaScriptParser::PropertyNameContext * /*ctx*/) override { }

  virtual void enterPropertySetParameterList(JavaScriptParser::PropertySetParameterListContext * /*ctx*/) override { }
  virtual void exitPropertySetParameterList(JavaScriptParser::PropertySetParameterListContext * /*ctx*/) override { }

  virtual void enterArguments(JavaScriptParser::ArgumentsContext * /*ctx*/) override { }
  virtual void exitArguments(JavaScriptParser::ArgumentsContext * /*ctx*/) override { }

  virtual void enterArgumentList(JavaScriptParser::ArgumentListContext * /*ctx*/) override { }
  virtual void exitArgumentList(JavaScriptParser::ArgumentListContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

