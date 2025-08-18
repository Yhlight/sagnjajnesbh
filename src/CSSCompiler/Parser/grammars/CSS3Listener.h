
// Generated from grammars/CSS3.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "CSS3Parser.h"


/**
 * This interface defines an abstract listener for a parse tree produced by CSS3Parser.
 */
class  CSS3Listener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterStylesheet(CSS3Parser::StylesheetContext *ctx) = 0;
  virtual void exitStylesheet(CSS3Parser::StylesheetContext *ctx) = 0;

  virtual void enterRuleSet(CSS3Parser::RuleSetContext *ctx) = 0;
  virtual void exitRuleSet(CSS3Parser::RuleSetContext *ctx) = 0;

  virtual void enterAtRule(CSS3Parser::AtRuleContext *ctx) = 0;
  virtual void exitAtRule(CSS3Parser::AtRuleContext *ctx) = 0;

  virtual void enterSelector(CSS3Parser::SelectorContext *ctx) = 0;
  virtual void exitSelector(CSS3Parser::SelectorContext *ctx) = 0;

  virtual void enterCombinator(CSS3Parser::CombinatorContext *ctx) = 0;
  virtual void exitCombinator(CSS3Parser::CombinatorContext *ctx) = 0;

  virtual void enterSimpleSelector(CSS3Parser::SimpleSelectorContext *ctx) = 0;
  virtual void exitSimpleSelector(CSS3Parser::SimpleSelectorContext *ctx) = 0;

  virtual void enterElementName(CSS3Parser::ElementNameContext *ctx) = 0;
  virtual void exitElementName(CSS3Parser::ElementNameContext *ctx) = 0;

  virtual void enterDeclaration(CSS3Parser::DeclarationContext *ctx) = 0;
  virtual void exitDeclaration(CSS3Parser::DeclarationContext *ctx) = 0;

  virtual void enterProperty(CSS3Parser::PropertyContext *ctx) = 0;
  virtual void exitProperty(CSS3Parser::PropertyContext *ctx) = 0;

  virtual void enterExpr(CSS3Parser::ExprContext *ctx) = 0;
  virtual void exitExpr(CSS3Parser::ExprContext *ctx) = 0;

  virtual void enterTerm(CSS3Parser::TermContext *ctx) = 0;
  virtual void exitTerm(CSS3Parser::TermContext *ctx) = 0;

  virtual void enterFunction_(CSS3Parser::Function_Context *ctx) = 0;
  virtual void exitFunction_(CSS3Parser::Function_Context *ctx) = 0;

  virtual void enterUnaryOperator(CSS3Parser::UnaryOperatorContext *ctx) = 0;
  virtual void exitUnaryOperator(CSS3Parser::UnaryOperatorContext *ctx) = 0;

  virtual void enterOperator(CSS3Parser::OperatorContext *ctx) = 0;
  virtual void exitOperator(CSS3Parser::OperatorContext *ctx) = 0;

  virtual void enterUrl(CSS3Parser::UrlContext *ctx) = 0;
  virtual void exitUrl(CSS3Parser::UrlContext *ctx) = 0;


};

