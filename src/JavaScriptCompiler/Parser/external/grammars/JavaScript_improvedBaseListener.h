
// Generated from external/grammars/JavaScript_improved.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "JavaScript_improvedListener.h"


/**
 * This class provides an empty implementation of JavaScript_improvedListener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class  JavaScript_improvedBaseListener : public JavaScript_improvedListener {
public:

  virtual void enterProgram(JavaScript_improvedParser::ProgramContext * /*ctx*/) override { }
  virtual void exitProgram(JavaScript_improvedParser::ProgramContext * /*ctx*/) override { }

  virtual void enterStatement(JavaScript_improvedParser::StatementContext * /*ctx*/) override { }
  virtual void exitStatement(JavaScript_improvedParser::StatementContext * /*ctx*/) override { }

  virtual void enterBlockStatement(JavaScript_improvedParser::BlockStatementContext * /*ctx*/) override { }
  virtual void exitBlockStatement(JavaScript_improvedParser::BlockStatementContext * /*ctx*/) override { }

  virtual void enterVariableStatement(JavaScript_improvedParser::VariableStatementContext * /*ctx*/) override { }
  virtual void exitVariableStatement(JavaScript_improvedParser::VariableStatementContext * /*ctx*/) override { }

  virtual void enterVariableDeclaration(JavaScript_improvedParser::VariableDeclarationContext * /*ctx*/) override { }
  virtual void exitVariableDeclaration(JavaScript_improvedParser::VariableDeclarationContext * /*ctx*/) override { }

  virtual void enterFunctionDeclaration(JavaScript_improvedParser::FunctionDeclarationContext * /*ctx*/) override { }
  virtual void exitFunctionDeclaration(JavaScript_improvedParser::FunctionDeclarationContext * /*ctx*/) override { }

  virtual void enterParameterList(JavaScript_improvedParser::ParameterListContext * /*ctx*/) override { }
  virtual void exitParameterList(JavaScript_improvedParser::ParameterListContext * /*ctx*/) override { }

  virtual void enterExpressionStatement(JavaScript_improvedParser::ExpressionStatementContext * /*ctx*/) override { }
  virtual void exitExpressionStatement(JavaScript_improvedParser::ExpressionStatementContext * /*ctx*/) override { }

  virtual void enterIfStatement(JavaScript_improvedParser::IfStatementContext * /*ctx*/) override { }
  virtual void exitIfStatement(JavaScript_improvedParser::IfStatementContext * /*ctx*/) override { }

  virtual void enterWhileStatement(JavaScript_improvedParser::WhileStatementContext * /*ctx*/) override { }
  virtual void exitWhileStatement(JavaScript_improvedParser::WhileStatementContext * /*ctx*/) override { }

  virtual void enterForStatement(JavaScript_improvedParser::ForStatementContext * /*ctx*/) override { }
  virtual void exitForStatement(JavaScript_improvedParser::ForStatementContext * /*ctx*/) override { }

  virtual void enterReturnStatement(JavaScript_improvedParser::ReturnStatementContext * /*ctx*/) override { }
  virtual void exitReturnStatement(JavaScript_improvedParser::ReturnStatementContext * /*ctx*/) override { }

  virtual void enterExpression(JavaScript_improvedParser::ExpressionContext * /*ctx*/) override { }
  virtual void exitExpression(JavaScript_improvedParser::ExpressionContext * /*ctx*/) override { }

  virtual void enterAssignmentExpression(JavaScript_improvedParser::AssignmentExpressionContext * /*ctx*/) override { }
  virtual void exitAssignmentExpression(JavaScript_improvedParser::AssignmentExpressionContext * /*ctx*/) override { }

  virtual void enterConditionalExpression(JavaScript_improvedParser::ConditionalExpressionContext * /*ctx*/) override { }
  virtual void exitConditionalExpression(JavaScript_improvedParser::ConditionalExpressionContext * /*ctx*/) override { }

  virtual void enterLogicalOrExpression(JavaScript_improvedParser::LogicalOrExpressionContext * /*ctx*/) override { }
  virtual void exitLogicalOrExpression(JavaScript_improvedParser::LogicalOrExpressionContext * /*ctx*/) override { }

  virtual void enterLogicalAndExpression(JavaScript_improvedParser::LogicalAndExpressionContext * /*ctx*/) override { }
  virtual void exitLogicalAndExpression(JavaScript_improvedParser::LogicalAndExpressionContext * /*ctx*/) override { }

  virtual void enterEqualityExpression(JavaScript_improvedParser::EqualityExpressionContext * /*ctx*/) override { }
  virtual void exitEqualityExpression(JavaScript_improvedParser::EqualityExpressionContext * /*ctx*/) override { }

  virtual void enterRelationalExpression(JavaScript_improvedParser::RelationalExpressionContext * /*ctx*/) override { }
  virtual void exitRelationalExpression(JavaScript_improvedParser::RelationalExpressionContext * /*ctx*/) override { }

  virtual void enterAdditiveExpression(JavaScript_improvedParser::AdditiveExpressionContext * /*ctx*/) override { }
  virtual void exitAdditiveExpression(JavaScript_improvedParser::AdditiveExpressionContext * /*ctx*/) override { }

  virtual void enterMultiplicativeExpression(JavaScript_improvedParser::MultiplicativeExpressionContext * /*ctx*/) override { }
  virtual void exitMultiplicativeExpression(JavaScript_improvedParser::MultiplicativeExpressionContext * /*ctx*/) override { }

  virtual void enterUnaryExpression(JavaScript_improvedParser::UnaryExpressionContext * /*ctx*/) override { }
  virtual void exitUnaryExpression(JavaScript_improvedParser::UnaryExpressionContext * /*ctx*/) override { }

  virtual void enterPostfixExpression(JavaScript_improvedParser::PostfixExpressionContext * /*ctx*/) override { }
  virtual void exitPostfixExpression(JavaScript_improvedParser::PostfixExpressionContext * /*ctx*/) override { }

  virtual void enterPrimaryExpression(JavaScript_improvedParser::PrimaryExpressionContext * /*ctx*/) override { }
  virtual void exitPrimaryExpression(JavaScript_improvedParser::PrimaryExpressionContext * /*ctx*/) override { }

  virtual void enterArrayLiteral(JavaScript_improvedParser::ArrayLiteralContext * /*ctx*/) override { }
  virtual void exitArrayLiteral(JavaScript_improvedParser::ArrayLiteralContext * /*ctx*/) override { }

  virtual void enterObjectLiteral(JavaScript_improvedParser::ObjectLiteralContext * /*ctx*/) override { }
  virtual void exitObjectLiteral(JavaScript_improvedParser::ObjectLiteralContext * /*ctx*/) override { }

  virtual void enterPropertyAssignment(JavaScript_improvedParser::PropertyAssignmentContext * /*ctx*/) override { }
  virtual void exitPropertyAssignment(JavaScript_improvedParser::PropertyAssignmentContext * /*ctx*/) override { }

  virtual void enterArgumentList(JavaScript_improvedParser::ArgumentListContext * /*ctx*/) override { }
  virtual void exitArgumentList(JavaScript_improvedParser::ArgumentListContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

