
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

  virtual void enterAt_rule(CSS3Parser::At_ruleContext * /*ctx*/) override { }
  virtual void exitAt_rule(CSS3Parser::At_ruleContext * /*ctx*/) override { }

  virtual void enterRuleset(CSS3Parser::RulesetContext * /*ctx*/) override { }
  virtual void exitRuleset(CSS3Parser::RulesetContext * /*ctx*/) override { }

  virtual void enterSelector_list(CSS3Parser::Selector_listContext * /*ctx*/) override { }
  virtual void exitSelector_list(CSS3Parser::Selector_listContext * /*ctx*/) override { }

  virtual void enterSelector(CSS3Parser::SelectorContext * /*ctx*/) override { }
  virtual void exitSelector(CSS3Parser::SelectorContext * /*ctx*/) override { }

  virtual void enterCombinator(CSS3Parser::CombinatorContext * /*ctx*/) override { }
  virtual void exitCombinator(CSS3Parser::CombinatorContext * /*ctx*/) override { }

  virtual void enterSimple_selector(CSS3Parser::Simple_selectorContext * /*ctx*/) override { }
  virtual void exitSimple_selector(CSS3Parser::Simple_selectorContext * /*ctx*/) override { }

  virtual void enterElement_name(CSS3Parser::Element_nameContext * /*ctx*/) override { }
  virtual void exitElement_name(CSS3Parser::Element_nameContext * /*ctx*/) override { }

  virtual void enterSelector_modifier(CSS3Parser::Selector_modifierContext * /*ctx*/) override { }
  virtual void exitSelector_modifier(CSS3Parser::Selector_modifierContext * /*ctx*/) override { }

  virtual void enterClass_selector(CSS3Parser::Class_selectorContext * /*ctx*/) override { }
  virtual void exitClass_selector(CSS3Parser::Class_selectorContext * /*ctx*/) override { }

  virtual void enterAttrib(CSS3Parser::AttribContext * /*ctx*/) override { }
  virtual void exitAttrib(CSS3Parser::AttribContext * /*ctx*/) override { }

  virtual void enterAttrib_operator(CSS3Parser::Attrib_operatorContext * /*ctx*/) override { }
  virtual void exitAttrib_operator(CSS3Parser::Attrib_operatorContext * /*ctx*/) override { }

  virtual void enterPseudo(CSS3Parser::PseudoContext * /*ctx*/) override { }
  virtual void exitPseudo(CSS3Parser::PseudoContext * /*ctx*/) override { }

  virtual void enterFunctional_pseudo(CSS3Parser::Functional_pseudoContext * /*ctx*/) override { }
  virtual void exitFunctional_pseudo(CSS3Parser::Functional_pseudoContext * /*ctx*/) override { }

  virtual void enterBlock(CSS3Parser::BlockContext * /*ctx*/) override { }
  virtual void exitBlock(CSS3Parser::BlockContext * /*ctx*/) override { }

  virtual void enterDeclaration_list(CSS3Parser::Declaration_listContext * /*ctx*/) override { }
  virtual void exitDeclaration_list(CSS3Parser::Declaration_listContext * /*ctx*/) override { }

  virtual void enterDeclaration(CSS3Parser::DeclarationContext * /*ctx*/) override { }
  virtual void exitDeclaration(CSS3Parser::DeclarationContext * /*ctx*/) override { }

  virtual void enterProperty(CSS3Parser::PropertyContext * /*ctx*/) override { }
  virtual void exitProperty(CSS3Parser::PropertyContext * /*ctx*/) override { }

  virtual void enterValue_list(CSS3Parser::Value_listContext * /*ctx*/) override { }
  virtual void exitValue_list(CSS3Parser::Value_listContext * /*ctx*/) override { }

  virtual void enterValue(CSS3Parser::ValueContext * /*ctx*/) override { }
  virtual void exitValue(CSS3Parser::ValueContext * /*ctx*/) override { }

  virtual void enterUnit(CSS3Parser::UnitContext * /*ctx*/) override { }
  virtual void exitUnit(CSS3Parser::UnitContext * /*ctx*/) override { }

  virtual void enterAny_value(CSS3Parser::Any_valueContext * /*ctx*/) override { }
  virtual void exitAny_value(CSS3Parser::Any_valueContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

