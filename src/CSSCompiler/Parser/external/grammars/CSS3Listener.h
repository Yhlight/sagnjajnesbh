
// Generated from external/grammars/CSS3.g4 by ANTLR 4.13.2

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

  virtual void enterRule(CSS3Parser::RuleContext *ctx) = 0;
  virtual void exitRule(CSS3Parser::RuleContext *ctx) = 0;

  virtual void enterAt_rule(CSS3Parser::At_ruleContext *ctx) = 0;
  virtual void exitAt_rule(CSS3Parser::At_ruleContext *ctx) = 0;

  virtual void enterQualified_rule(CSS3Parser::Qualified_ruleContext *ctx) = 0;
  virtual void exitQualified_rule(CSS3Parser::Qualified_ruleContext *ctx) = 0;

  virtual void enterDeclaration_list(CSS3Parser::Declaration_listContext *ctx) = 0;
  virtual void exitDeclaration_list(CSS3Parser::Declaration_listContext *ctx) = 0;

  virtual void enterDeclaration(CSS3Parser::DeclarationContext *ctx) = 0;
  virtual void exitDeclaration(CSS3Parser::DeclarationContext *ctx) = 0;

  virtual void enterComponent_value(CSS3Parser::Component_valueContext *ctx) = 0;
  virtual void exitComponent_value(CSS3Parser::Component_valueContext *ctx) = 0;

  virtual void enterBlock(CSS3Parser::BlockContext *ctx) = 0;
  virtual void exitBlock(CSS3Parser::BlockContext *ctx) = 0;

  virtual void enterFunction(CSS3Parser::FunctionContext *ctx) = 0;
  virtual void exitFunction(CSS3Parser::FunctionContext *ctx) = 0;

  virtual void enterPreserved_token(CSS3Parser::Preserved_tokenContext *ctx) = 0;
  virtual void exitPreserved_token(CSS3Parser::Preserved_tokenContext *ctx) = 0;


};

