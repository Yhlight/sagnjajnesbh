
// Generated from external/grammars/CSS3.g4 by ANTLR 4.13.2

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

  virtual void enterRule(CSS3Parser::RuleContext * /*ctx*/) override { }
  virtual void exitRule(CSS3Parser::RuleContext * /*ctx*/) override { }

  virtual void enterAt_rule(CSS3Parser::At_ruleContext * /*ctx*/) override { }
  virtual void exitAt_rule(CSS3Parser::At_ruleContext * /*ctx*/) override { }

  virtual void enterQualified_rule(CSS3Parser::Qualified_ruleContext * /*ctx*/) override { }
  virtual void exitQualified_rule(CSS3Parser::Qualified_ruleContext * /*ctx*/) override { }

  virtual void enterDeclaration_list(CSS3Parser::Declaration_listContext * /*ctx*/) override { }
  virtual void exitDeclaration_list(CSS3Parser::Declaration_listContext * /*ctx*/) override { }

  virtual void enterDeclaration(CSS3Parser::DeclarationContext * /*ctx*/) override { }
  virtual void exitDeclaration(CSS3Parser::DeclarationContext * /*ctx*/) override { }

  virtual void enterComponent_value(CSS3Parser::Component_valueContext * /*ctx*/) override { }
  virtual void exitComponent_value(CSS3Parser::Component_valueContext * /*ctx*/) override { }

  virtual void enterBlock(CSS3Parser::BlockContext * /*ctx*/) override { }
  virtual void exitBlock(CSS3Parser::BlockContext * /*ctx*/) override { }

  virtual void enterFunction(CSS3Parser::FunctionContext * /*ctx*/) override { }
  virtual void exitFunction(CSS3Parser::FunctionContext * /*ctx*/) override { }

  virtual void enterPreserved_token(CSS3Parser::Preserved_tokenContext * /*ctx*/) override { }
  virtual void exitPreserved_token(CSS3Parser::Preserved_tokenContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

