
// Generated from external/grammars/JavaScript_improved.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "JavaScript_improvedParser.h"


/**
 * This interface defines an abstract listener for a parse tree produced by JavaScript_improvedParser.
 */
class  JavaScript_improvedListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterProgram(JavaScript_improvedParser::ProgramContext *ctx) = 0;
  virtual void exitProgram(JavaScript_improvedParser::ProgramContext *ctx) = 0;

  virtual void enterStatement(JavaScript_improvedParser::StatementContext *ctx) = 0;
  virtual void exitStatement(JavaScript_improvedParser::StatementContext *ctx) = 0;

  virtual void enterBlockStatement(JavaScript_improvedParser::BlockStatementContext *ctx) = 0;
  virtual void exitBlockStatement(JavaScript_improvedParser::BlockStatementContext *ctx) = 0;

  virtual void enterVariableStatement(JavaScript_improvedParser::VariableStatementContext *ctx) = 0;
  virtual void exitVariableStatement(JavaScript_improvedParser::VariableStatementContext *ctx) = 0;

  virtual void enterVariableDeclaration(JavaScript_improvedParser::VariableDeclarationContext *ctx) = 0;
  virtual void exitVariableDeclaration(JavaScript_improvedParser::VariableDeclarationContext *ctx) = 0;

  virtual void enterFunctionDeclaration(JavaScript_improvedParser::FunctionDeclarationContext *ctx) = 0;
  virtual void exitFunctionDeclaration(JavaScript_improvedParser::FunctionDeclarationContext *ctx) = 0;

  virtual void enterParameterList(JavaScript_improvedParser::ParameterListContext *ctx) = 0;
  virtual void exitParameterList(JavaScript_improvedParser::ParameterListContext *ctx) = 0;

  virtual void enterExpressionStatement(JavaScript_improvedParser::ExpressionStatementContext *ctx) = 0;
  virtual void exitExpressionStatement(JavaScript_improvedParser::ExpressionStatementContext *ctx) = 0;

  virtual void enterIfStatement(JavaScript_improvedParser::IfStatementContext *ctx) = 0;
  virtual void exitIfStatement(JavaScript_improvedParser::IfStatementContext *ctx) = 0;

  virtual void enterWhileStatement(JavaScript_improvedParser::WhileStatementContext *ctx) = 0;
  virtual void exitWhileStatement(JavaScript_improvedParser::WhileStatementContext *ctx) = 0;

  virtual void enterForStatement(JavaScript_improvedParser::ForStatementContext *ctx) = 0;
  virtual void exitForStatement(JavaScript_improvedParser::ForStatementContext *ctx) = 0;

  virtual void enterReturnStatement(JavaScript_improvedParser::ReturnStatementContext *ctx) = 0;
  virtual void exitReturnStatement(JavaScript_improvedParser::ReturnStatementContext *ctx) = 0;

  virtual void enterExpression(JavaScript_improvedParser::ExpressionContext *ctx) = 0;
  virtual void exitExpression(JavaScript_improvedParser::ExpressionContext *ctx) = 0;

  virtual void enterAssignmentExpression(JavaScript_improvedParser::AssignmentExpressionContext *ctx) = 0;
  virtual void exitAssignmentExpression(JavaScript_improvedParser::AssignmentExpressionContext *ctx) = 0;

  virtual void enterConditionalExpression(JavaScript_improvedParser::ConditionalExpressionContext *ctx) = 0;
  virtual void exitConditionalExpression(JavaScript_improvedParser::ConditionalExpressionContext *ctx) = 0;

  virtual void enterLogicalOrExpression(JavaScript_improvedParser::LogicalOrExpressionContext *ctx) = 0;
  virtual void exitLogicalOrExpression(JavaScript_improvedParser::LogicalOrExpressionContext *ctx) = 0;

  virtual void enterLogicalAndExpression(JavaScript_improvedParser::LogicalAndExpressionContext *ctx) = 0;
  virtual void exitLogicalAndExpression(JavaScript_improvedParser::LogicalAndExpressionContext *ctx) = 0;

  virtual void enterEqualityExpression(JavaScript_improvedParser::EqualityExpressionContext *ctx) = 0;
  virtual void exitEqualityExpression(JavaScript_improvedParser::EqualityExpressionContext *ctx) = 0;

  virtual void enterRelationalExpression(JavaScript_improvedParser::RelationalExpressionContext *ctx) = 0;
  virtual void exitRelationalExpression(JavaScript_improvedParser::RelationalExpressionContext *ctx) = 0;

  virtual void enterAdditiveExpression(JavaScript_improvedParser::AdditiveExpressionContext *ctx) = 0;
  virtual void exitAdditiveExpression(JavaScript_improvedParser::AdditiveExpressionContext *ctx) = 0;

  virtual void enterMultiplicativeExpression(JavaScript_improvedParser::MultiplicativeExpressionContext *ctx) = 0;
  virtual void exitMultiplicativeExpression(JavaScript_improvedParser::MultiplicativeExpressionContext *ctx) = 0;

  virtual void enterUnaryExpression(JavaScript_improvedParser::UnaryExpressionContext *ctx) = 0;
  virtual void exitUnaryExpression(JavaScript_improvedParser::UnaryExpressionContext *ctx) = 0;

  virtual void enterPostfixExpression(JavaScript_improvedParser::PostfixExpressionContext *ctx) = 0;
  virtual void exitPostfixExpression(JavaScript_improvedParser::PostfixExpressionContext *ctx) = 0;

  virtual void enterPrimaryExpression(JavaScript_improvedParser::PrimaryExpressionContext *ctx) = 0;
  virtual void exitPrimaryExpression(JavaScript_improvedParser::PrimaryExpressionContext *ctx) = 0;

  virtual void enterArrayLiteral(JavaScript_improvedParser::ArrayLiteralContext *ctx) = 0;
  virtual void exitArrayLiteral(JavaScript_improvedParser::ArrayLiteralContext *ctx) = 0;

  virtual void enterObjectLiteral(JavaScript_improvedParser::ObjectLiteralContext *ctx) = 0;
  virtual void exitObjectLiteral(JavaScript_improvedParser::ObjectLiteralContext *ctx) = 0;

  virtual void enterPropertyAssignment(JavaScript_improvedParser::PropertyAssignmentContext *ctx) = 0;
  virtual void exitPropertyAssignment(JavaScript_improvedParser::PropertyAssignmentContext *ctx) = 0;

  virtual void enterArgumentList(JavaScript_improvedParser::ArgumentListContext *ctx) = 0;
  virtual void exitArgumentList(JavaScript_improvedParser::ArgumentListContext *ctx) = 0;


};

