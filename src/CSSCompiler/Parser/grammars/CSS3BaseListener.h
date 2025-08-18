
// Generated from grammars/CSS3.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "CSS3Listener.h"


/**
 * This class provides an empty implementation of CSS3Listener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class  CSS3BaseListener : public CSS3Listener {
public:

  virtual void enterStylesheet(CSS3Parser::StylesheetContext * /*ctx*/) override { }
  virtual void exitStylesheet(CSS3Parser::StylesheetContext * /*ctx*/) override { }

  virtual void enterRuleSet(CSS3Parser::RuleSetContext * /*ctx*/) override { }
  virtual void exitRuleSet(CSS3Parser::RuleSetContext * /*ctx*/) override { }

  virtual void enterAtRule(CSS3Parser::AtRuleContext * /*ctx*/) override { }
  virtual void exitAtRule(CSS3Parser::AtRuleContext * /*ctx*/) override { }

  virtual void enterSelector(CSS3Parser::SelectorContext * /*ctx*/) override { }
  virtual void exitSelector(CSS3Parser::SelectorContext * /*ctx*/) override { }

  virtual void enterCombinator(CSS3Parser::CombinatorContext * /*ctx*/) override { }
  virtual void exitCombinator(CSS3Parser::CombinatorContext * /*ctx*/) override { }

  virtual void enterSimpleSelector(CSS3Parser::SimpleSelectorContext * /*ctx*/) override { }
  virtual void exitSimpleSelector(CSS3Parser::SimpleSelectorContext * /*ctx*/) override { }

  virtual void enterElementName(CSS3Parser::ElementNameContext * /*ctx*/) override { }
  virtual void exitElementName(CSS3Parser::ElementNameContext * /*ctx*/) override { }

  virtual void enterDeclaration(CSS3Parser::DeclarationContext * /*ctx*/) override { }
  virtual void exitDeclaration(CSS3Parser::DeclarationContext * /*ctx*/) override { }

  virtual void enterProperty(CSS3Parser::PropertyContext * /*ctx*/) override { }
  virtual void exitProperty(CSS3Parser::PropertyContext * /*ctx*/) override { }

  virtual void enterExpr(CSS3Parser::ExprContext * /*ctx*/) override { }
  virtual void exitExpr(CSS3Parser::ExprContext * /*ctx*/) override { }

  virtual void enterTerm(CSS3Parser::TermContext * /*ctx*/) override { }
  virtual void exitTerm(CSS3Parser::TermContext * /*ctx*/) override { }

  virtual void enterFunction_(CSS3Parser::Function_Context * /*ctx*/) override { }
  virtual void exitFunction_(CSS3Parser::Function_Context * /*ctx*/) override { }

  virtual void enterUnaryOperator(CSS3Parser::UnaryOperatorContext * /*ctx*/) override { }
  virtual void exitUnaryOperator(CSS3Parser::UnaryOperatorContext * /*ctx*/) override { }

  virtual void enterOperator(CSS3Parser::OperatorContext * /*ctx*/) override { }
  virtual void exitOperator(CSS3Parser::OperatorContext * /*ctx*/) override { }

  virtual void enterUrl(CSS3Parser::UrlContext * /*ctx*/) override { }
  virtual void exitUrl(CSS3Parser::UrlContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

