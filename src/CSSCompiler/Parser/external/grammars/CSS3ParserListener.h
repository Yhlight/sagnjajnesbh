
// Generated from external/grammars/CSS3Parser.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "CSS3Parser.h"


/**
 * This interface defines an abstract listener for a parse tree produced by CSS3Parser.
 */
class  CSS3ParserListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterStylesheet(CSS3Parser::StylesheetContext *ctx) = 0;
  virtual void exitStylesheet(CSS3Parser::StylesheetContext *ctx) = 0;

  virtual void enterCharset(CSS3Parser::CharsetContext *ctx) = 0;
  virtual void exitCharset(CSS3Parser::CharsetContext *ctx) = 0;

  virtual void enterImportRule(CSS3Parser::ImportRuleContext *ctx) = 0;
  virtual void exitImportRule(CSS3Parser::ImportRuleContext *ctx) = 0;

  virtual void enterMediaList(CSS3Parser::MediaListContext *ctx) = 0;
  virtual void exitMediaList(CSS3Parser::MediaListContext *ctx) = 0;

  virtual void enterStatement(CSS3Parser::StatementContext *ctx) = 0;
  virtual void exitStatement(CSS3Parser::StatementContext *ctx) = 0;

  virtual void enterAtRule(CSS3Parser::AtRuleContext *ctx) = 0;
  virtual void exitAtRule(CSS3Parser::AtRuleContext *ctx) = 0;

  virtual void enterBlock(CSS3Parser::BlockContext *ctx) = 0;
  virtual void exitBlock(CSS3Parser::BlockContext *ctx) = 0;

  virtual void enterRuleset(CSS3Parser::RulesetContext *ctx) = 0;
  virtual void exitRuleset(CSS3Parser::RulesetContext *ctx) = 0;

  virtual void enterSelector(CSS3Parser::SelectorContext *ctx) = 0;
  virtual void exitSelector(CSS3Parser::SelectorContext *ctx) = 0;

  virtual void enterCombinator(CSS3Parser::CombinatorContext *ctx) = 0;
  virtual void exitCombinator(CSS3Parser::CombinatorContext *ctx) = 0;

  virtual void enterSimpleSelector(CSS3Parser::SimpleSelectorContext *ctx) = 0;
  virtual void exitSimpleSelector(CSS3Parser::SimpleSelectorContext *ctx) = 0;

  virtual void enterElementName(CSS3Parser::ElementNameContext *ctx) = 0;
  virtual void exitElementName(CSS3Parser::ElementNameContext *ctx) = 0;

  virtual void enterAttrib(CSS3Parser::AttribContext *ctx) = 0;
  virtual void exitAttrib(CSS3Parser::AttribContext *ctx) = 0;

  virtual void enterPseudo(CSS3Parser::PseudoContext *ctx) = 0;
  virtual void exitPseudo(CSS3Parser::PseudoContext *ctx) = 0;

  virtual void enterDeclaration(CSS3Parser::DeclarationContext *ctx) = 0;
  virtual void exitDeclaration(CSS3Parser::DeclarationContext *ctx) = 0;

  virtual void enterProperty(CSS3Parser::PropertyContext *ctx) = 0;
  virtual void exitProperty(CSS3Parser::PropertyContext *ctx) = 0;

  virtual void enterExpr(CSS3Parser::ExprContext *ctx) = 0;
  virtual void exitExpr(CSS3Parser::ExprContext *ctx) = 0;

  virtual void enterTerm(CSS3Parser::TermContext *ctx) = 0;
  virtual void exitTerm(CSS3Parser::TermContext *ctx) = 0;

  virtual void enterFunction(CSS3Parser::FunctionContext *ctx) = 0;
  virtual void exitFunction(CSS3Parser::FunctionContext *ctx) = 0;

  virtual void enterHexcolor(CSS3Parser::HexcolorContext *ctx) = 0;
  virtual void exitHexcolor(CSS3Parser::HexcolorContext *ctx) = 0;

  virtual void enterOperator(CSS3Parser::OperatorContext *ctx) = 0;
  virtual void exitOperator(CSS3Parser::OperatorContext *ctx) = 0;

  virtual void enterUnaryOperator(CSS3Parser::UnaryOperatorContext *ctx) = 0;
  virtual void exitUnaryOperator(CSS3Parser::UnaryOperatorContext *ctx) = 0;

  virtual void enterAny(CSS3Parser::AnyContext *ctx) = 0;
  virtual void exitAny(CSS3Parser::AnyContext *ctx) = 0;


};

