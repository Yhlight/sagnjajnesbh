
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

  virtual void enterAt_rule(CSS3Parser::At_ruleContext *ctx) = 0;
  virtual void exitAt_rule(CSS3Parser::At_ruleContext *ctx) = 0;

  virtual void enterRuleset(CSS3Parser::RulesetContext *ctx) = 0;
  virtual void exitRuleset(CSS3Parser::RulesetContext *ctx) = 0;

  virtual void enterSelector_list(CSS3Parser::Selector_listContext *ctx) = 0;
  virtual void exitSelector_list(CSS3Parser::Selector_listContext *ctx) = 0;

  virtual void enterSelector(CSS3Parser::SelectorContext *ctx) = 0;
  virtual void exitSelector(CSS3Parser::SelectorContext *ctx) = 0;

  virtual void enterCombinator(CSS3Parser::CombinatorContext *ctx) = 0;
  virtual void exitCombinator(CSS3Parser::CombinatorContext *ctx) = 0;

  virtual void enterSimple_selector(CSS3Parser::Simple_selectorContext *ctx) = 0;
  virtual void exitSimple_selector(CSS3Parser::Simple_selectorContext *ctx) = 0;

  virtual void enterElement_name(CSS3Parser::Element_nameContext *ctx) = 0;
  virtual void exitElement_name(CSS3Parser::Element_nameContext *ctx) = 0;

  virtual void enterSelector_modifier(CSS3Parser::Selector_modifierContext *ctx) = 0;
  virtual void exitSelector_modifier(CSS3Parser::Selector_modifierContext *ctx) = 0;

  virtual void enterClass_selector(CSS3Parser::Class_selectorContext *ctx) = 0;
  virtual void exitClass_selector(CSS3Parser::Class_selectorContext *ctx) = 0;

  virtual void enterAttrib(CSS3Parser::AttribContext *ctx) = 0;
  virtual void exitAttrib(CSS3Parser::AttribContext *ctx) = 0;

  virtual void enterAttrib_operator(CSS3Parser::Attrib_operatorContext *ctx) = 0;
  virtual void exitAttrib_operator(CSS3Parser::Attrib_operatorContext *ctx) = 0;

  virtual void enterPseudo(CSS3Parser::PseudoContext *ctx) = 0;
  virtual void exitPseudo(CSS3Parser::PseudoContext *ctx) = 0;

  virtual void enterFunctional_pseudo(CSS3Parser::Functional_pseudoContext *ctx) = 0;
  virtual void exitFunctional_pseudo(CSS3Parser::Functional_pseudoContext *ctx) = 0;

  virtual void enterBlock(CSS3Parser::BlockContext *ctx) = 0;
  virtual void exitBlock(CSS3Parser::BlockContext *ctx) = 0;

  virtual void enterDeclaration_list(CSS3Parser::Declaration_listContext *ctx) = 0;
  virtual void exitDeclaration_list(CSS3Parser::Declaration_listContext *ctx) = 0;

  virtual void enterDeclaration(CSS3Parser::DeclarationContext *ctx) = 0;
  virtual void exitDeclaration(CSS3Parser::DeclarationContext *ctx) = 0;

  virtual void enterProperty(CSS3Parser::PropertyContext *ctx) = 0;
  virtual void exitProperty(CSS3Parser::PropertyContext *ctx) = 0;

  virtual void enterValue_list(CSS3Parser::Value_listContext *ctx) = 0;
  virtual void exitValue_list(CSS3Parser::Value_listContext *ctx) = 0;

  virtual void enterValue(CSS3Parser::ValueContext *ctx) = 0;
  virtual void exitValue(CSS3Parser::ValueContext *ctx) = 0;

  virtual void enterUnit(CSS3Parser::UnitContext *ctx) = 0;
  virtual void exitUnit(CSS3Parser::UnitContext *ctx) = 0;

  virtual void enterAny_value(CSS3Parser::Any_valueContext *ctx) = 0;
  virtual void exitAny_value(CSS3Parser::Any_valueContext *ctx) = 0;


};

