
// Generated from grammars/JavaScript.g4 by ANTLR 4.13.2

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

  virtual void enterStatement(JavaScriptParser::StatementContext * /*ctx*/) override { }
  virtual void exitStatement(JavaScriptParser::StatementContext * /*ctx*/) override { }

  virtual void enterBlock(JavaScriptParser::BlockContext * /*ctx*/) override { }
  virtual void exitBlock(JavaScriptParser::BlockContext * /*ctx*/) override { }

  virtual void enterVariableStatement(JavaScriptParser::VariableStatementContext * /*ctx*/) override { }
  virtual void exitVariableStatement(JavaScriptParser::VariableStatementContext * /*ctx*/) override { }

  virtual void enterVariableDeclaration(JavaScriptParser::VariableDeclarationContext * /*ctx*/) override { }
  virtual void exitVariableDeclaration(JavaScriptParser::VariableDeclarationContext * /*ctx*/) override { }

  virtual void enterExpressionStatement(JavaScriptParser::ExpressionStatementContext * /*ctx*/) override { }
  virtual void exitExpressionStatement(JavaScriptParser::ExpressionStatementContext * /*ctx*/) override { }

  virtual void enterIfStatement(JavaScriptParser::IfStatementContext * /*ctx*/) override { }
  virtual void exitIfStatement(JavaScriptParser::IfStatementContext * /*ctx*/) override { }

  virtual void enterWhileStatement(JavaScriptParser::WhileStatementContext * /*ctx*/) override { }
  virtual void exitWhileStatement(JavaScriptParser::WhileStatementContext * /*ctx*/) override { }

  virtual void enterForStatement(JavaScriptParser::ForStatementContext * /*ctx*/) override { }
  virtual void exitForStatement(JavaScriptParser::ForStatementContext * /*ctx*/) override { }

  virtual void enterReturnStatement(JavaScriptParser::ReturnStatementContext * /*ctx*/) override { }
  virtual void exitReturnStatement(JavaScriptParser::ReturnStatementContext * /*ctx*/) override { }

  virtual void enterFunctionDeclaration(JavaScriptParser::FunctionDeclarationContext * /*ctx*/) override { }
  virtual void exitFunctionDeclaration(JavaScriptParser::FunctionDeclarationContext * /*ctx*/) override { }

  virtual void enterParameterList(JavaScriptParser::ParameterListContext * /*ctx*/) override { }
  virtual void exitParameterList(JavaScriptParser::ParameterListContext * /*ctx*/) override { }

  virtual void enterExpression(JavaScriptParser::ExpressionContext * /*ctx*/) override { }
  virtual void exitExpression(JavaScriptParser::ExpressionContext * /*ctx*/) override { }

  virtual void enterAssignmentExpression(JavaScriptParser::AssignmentExpressionContext * /*ctx*/) override { }
  virtual void exitAssignmentExpression(JavaScriptParser::AssignmentExpressionContext * /*ctx*/) override { }

  virtual void enterConditionalExpression(JavaScriptParser::ConditionalExpressionContext * /*ctx*/) override { }
  virtual void exitConditionalExpression(JavaScriptParser::ConditionalExpressionContext * /*ctx*/) override { }

  virtual void enterLogicalOrExpression(JavaScriptParser::LogicalOrExpressionContext * /*ctx*/) override { }
  virtual void exitLogicalOrExpression(JavaScriptParser::LogicalOrExpressionContext * /*ctx*/) override { }

  virtual void enterLogicalAndExpression(JavaScriptParser::LogicalAndExpressionContext * /*ctx*/) override { }
  virtual void exitLogicalAndExpression(JavaScriptParser::LogicalAndExpressionContext * /*ctx*/) override { }

  virtual void enterEqualityExpression(JavaScriptParser::EqualityExpressionContext * /*ctx*/) override { }
  virtual void exitEqualityExpression(JavaScriptParser::EqualityExpressionContext * /*ctx*/) override { }

  virtual void enterRelationalExpression(JavaScriptParser::RelationalExpressionContext * /*ctx*/) override { }
  virtual void exitRelationalExpression(JavaScriptParser::RelationalExpressionContext * /*ctx*/) override { }

  virtual void enterAdditiveExpression(JavaScriptParser::AdditiveExpressionContext * /*ctx*/) override { }
  virtual void exitAdditiveExpression(JavaScriptParser::AdditiveExpressionContext * /*ctx*/) override { }

  virtual void enterMultiplicativeExpression(JavaScriptParser::MultiplicativeExpressionContext * /*ctx*/) override { }
  virtual void exitMultiplicativeExpression(JavaScriptParser::MultiplicativeExpressionContext * /*ctx*/) override { }

  virtual void enterUnaryExpression(JavaScriptParser::UnaryExpressionContext * /*ctx*/) override { }
  virtual void exitUnaryExpression(JavaScriptParser::UnaryExpressionContext * /*ctx*/) override { }

  virtual void enterPostfixExpression(JavaScriptParser::PostfixExpressionContext * /*ctx*/) override { }
  virtual void exitPostfixExpression(JavaScriptParser::PostfixExpressionContext * /*ctx*/) override { }

  virtual void enterLeftHandSideExpression(JavaScriptParser::LeftHandSideExpressionContext * /*ctx*/) override { }
  virtual void exitLeftHandSideExpression(JavaScriptParser::LeftHandSideExpressionContext * /*ctx*/) override { }

  virtual void enterCallExpression(JavaScriptParser::CallExpressionContext * /*ctx*/) override { }
  virtual void exitCallExpression(JavaScriptParser::CallExpressionContext * /*ctx*/) override { }

  virtual void enterMemberExpression(JavaScriptParser::MemberExpressionContext * /*ctx*/) override { }
  virtual void exitMemberExpression(JavaScriptParser::MemberExpressionContext * /*ctx*/) override { }

  virtual void enterPrimaryExpression(JavaScriptParser::PrimaryExpressionContext * /*ctx*/) override { }
  virtual void exitPrimaryExpression(JavaScriptParser::PrimaryExpressionContext * /*ctx*/) override { }

  virtual void enterArguments(JavaScriptParser::ArgumentsContext * /*ctx*/) override { }
  virtual void exitArguments(JavaScriptParser::ArgumentsContext * /*ctx*/) override { }

  virtual void enterArgumentList(JavaScriptParser::ArgumentListContext * /*ctx*/) override { }
  virtual void exitArgumentList(JavaScriptParser::ArgumentListContext * /*ctx*/) override { }

  virtual void enterArrayLiteral(JavaScriptParser::ArrayLiteralContext * /*ctx*/) override { }
  virtual void exitArrayLiteral(JavaScriptParser::ArrayLiteralContext * /*ctx*/) override { }

  virtual void enterElementList(JavaScriptParser::ElementListContext * /*ctx*/) override { }
  virtual void exitElementList(JavaScriptParser::ElementListContext * /*ctx*/) override { }

  virtual void enterObjectLiteral(JavaScriptParser::ObjectLiteralContext * /*ctx*/) override { }
  virtual void exitObjectLiteral(JavaScriptParser::ObjectLiteralContext * /*ctx*/) override { }

  virtual void enterPropertyNameAndValueList(JavaScriptParser::PropertyNameAndValueListContext * /*ctx*/) override { }
  virtual void exitPropertyNameAndValueList(JavaScriptParser::PropertyNameAndValueListContext * /*ctx*/) override { }

  virtual void enterPropertyAssignment(JavaScriptParser::PropertyAssignmentContext * /*ctx*/) override { }
  virtual void exitPropertyAssignment(JavaScriptParser::PropertyAssignmentContext * /*ctx*/) override { }

  virtual void enterPropertyName(JavaScriptParser::PropertyNameContext * /*ctx*/) override { }
  virtual void exitPropertyName(JavaScriptParser::PropertyNameContext * /*ctx*/) override { }

  virtual void enterLiteral(JavaScriptParser::LiteralContext * /*ctx*/) override { }
  virtual void exitLiteral(JavaScriptParser::LiteralContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

