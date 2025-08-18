
// Generated from grammars/CSS3.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  CSS3Parser : public antlr4::Parser {
public:
  enum {
    AT_KEYWORD = 1, HASH = 2, STRING = 3, URI = 4, FUNCTION = 5, NUMBER = 6, 
    PERCENTAGE = 7, IDENT = 8, INCLUDES = 9, DASHMATCH = 10, PREFIXMATCH = 11, 
    SUFFIXMATCH = 12, SUBSTRINGMATCH = 13, EQUALS = 14, SEMICOLON = 15, 
    LBRACE = 16, RBRACE = 17, LPAREN = 18, RPAREN = 19, LBRACKET = 20, RBRACKET = 21, 
    COMMA = 22, COLON = 23, DOT = 24, PLUS = 25, MINUS = 26, STAR = 27, 
    SLASH = 28, GREATER = 29, TILDE = 30, IMPORTANT = 31, WS = 32, COMMENT = 33
  };

  enum {
    RuleStylesheet = 0, RuleAt_rule = 1, RuleRuleset = 2, RuleSelector_list = 3, 
    RuleSelector = 4, RuleCombinator = 5, RuleSimple_selector = 6, RuleElement_name = 7, 
    RuleSelector_modifier = 8, RuleClass_selector = 9, RuleAttrib = 10, 
    RuleAttrib_operator = 11, RulePseudo = 12, RuleFunctional_pseudo = 13, 
    RuleBlock = 14, RuleDeclaration_list = 15, RuleDeclaration = 16, RuleProperty = 17, 
    RuleValue_list = 18, RuleValue = 19, RuleUnit = 20, RuleAny_value = 21
  };

  explicit CSS3Parser(antlr4::TokenStream *input);

  CSS3Parser(antlr4::TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options);

  ~CSS3Parser() override;

  std::string getGrammarFileName() const override;

  const antlr4::atn::ATN& getATN() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;


  class StylesheetContext;
  class At_ruleContext;
  class RulesetContext;
  class Selector_listContext;
  class SelectorContext;
  class CombinatorContext;
  class Simple_selectorContext;
  class Element_nameContext;
  class Selector_modifierContext;
  class Class_selectorContext;
  class AttribContext;
  class Attrib_operatorContext;
  class PseudoContext;
  class Functional_pseudoContext;
  class BlockContext;
  class Declaration_listContext;
  class DeclarationContext;
  class PropertyContext;
  class Value_listContext;
  class ValueContext;
  class UnitContext;
  class Any_valueContext; 

  class  StylesheetContext : public antlr4::ParserRuleContext {
  public:
    StylesheetContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *EOF();
    std::vector<RulesetContext *> ruleset();
    RulesetContext* ruleset(size_t i);
    std::vector<At_ruleContext *> at_rule();
    At_ruleContext* at_rule(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  StylesheetContext* stylesheet();

  class  At_ruleContext : public antlr4::ParserRuleContext {
  public:
    At_ruleContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *AT_KEYWORD();
    antlr4::tree::TerminalNode *SEMICOLON();
    BlockContext *block();
    std::vector<Any_valueContext *> any_value();
    Any_valueContext* any_value(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  At_ruleContext* at_rule();

  class  RulesetContext : public antlr4::ParserRuleContext {
  public:
    RulesetContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Selector_listContext *selector_list();
    BlockContext *block();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  RulesetContext* ruleset();

  class  Selector_listContext : public antlr4::ParserRuleContext {
  public:
    Selector_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<SelectorContext *> selector();
    SelectorContext* selector(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Selector_listContext* selector_list();

  class  SelectorContext : public antlr4::ParserRuleContext {
  public:
    SelectorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Simple_selectorContext *> simple_selector();
    Simple_selectorContext* simple_selector(size_t i);
    std::vector<CombinatorContext *> combinator();
    CombinatorContext* combinator(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  SelectorContext* selector();

  class  CombinatorContext : public antlr4::ParserRuleContext {
  public:
    CombinatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *PLUS();
    antlr4::tree::TerminalNode *GREATER();
    antlr4::tree::TerminalNode *TILDE();
    antlr4::tree::TerminalNode *WS();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  CombinatorContext* combinator();

  class  Simple_selectorContext : public antlr4::ParserRuleContext {
  public:
    Simple_selectorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Element_nameContext *element_name();
    std::vector<Selector_modifierContext *> selector_modifier();
    Selector_modifierContext* selector_modifier(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Simple_selectorContext* simple_selector();

  class  Element_nameContext : public antlr4::ParserRuleContext {
  public:
    Element_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENT();
    antlr4::tree::TerminalNode *STAR();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Element_nameContext* element_name();

  class  Selector_modifierContext : public antlr4::ParserRuleContext {
  public:
    Selector_modifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *HASH();
    Class_selectorContext *class_selector();
    AttribContext *attrib();
    PseudoContext *pseudo();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Selector_modifierContext* selector_modifier();

  class  Class_selectorContext : public antlr4::ParserRuleContext {
  public:
    Class_selectorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DOT();
    antlr4::tree::TerminalNode *IDENT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Class_selectorContext* class_selector();

  class  AttribContext : public antlr4::ParserRuleContext {
  public:
    AttribContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LBRACKET();
    std::vector<antlr4::tree::TerminalNode *> IDENT();
    antlr4::tree::TerminalNode* IDENT(size_t i);
    antlr4::tree::TerminalNode *RBRACKET();
    Attrib_operatorContext *attrib_operator();
    antlr4::tree::TerminalNode *STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  AttribContext* attrib();

  class  Attrib_operatorContext : public antlr4::ParserRuleContext {
  public:
    Attrib_operatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *EQUALS();
    antlr4::tree::TerminalNode *INCLUDES();
    antlr4::tree::TerminalNode *DASHMATCH();
    antlr4::tree::TerminalNode *PREFIXMATCH();
    antlr4::tree::TerminalNode *SUFFIXMATCH();
    antlr4::tree::TerminalNode *SUBSTRINGMATCH();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Attrib_operatorContext* attrib_operator();

  class  PseudoContext : public antlr4::ParserRuleContext {
  public:
    PseudoContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> COLON();
    antlr4::tree::TerminalNode* COLON(size_t i);
    antlr4::tree::TerminalNode *IDENT();
    Functional_pseudoContext *functional_pseudo();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  PseudoContext* pseudo();

  class  Functional_pseudoContext : public antlr4::ParserRuleContext {
  public:
    Functional_pseudoContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *FUNCTION();
    antlr4::tree::TerminalNode *RPAREN();
    std::vector<Any_valueContext *> any_value();
    Any_valueContext* any_value(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Functional_pseudoContext* functional_pseudo();

  class  BlockContext : public antlr4::ParserRuleContext {
  public:
    BlockContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LBRACE();
    Declaration_listContext *declaration_list();
    antlr4::tree::TerminalNode *RBRACE();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  BlockContext* block();

  class  Declaration_listContext : public antlr4::ParserRuleContext {
  public:
    Declaration_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<DeclarationContext *> declaration();
    DeclarationContext* declaration(size_t i);
    std::vector<antlr4::tree::TerminalNode *> SEMICOLON();
    antlr4::tree::TerminalNode* SEMICOLON(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Declaration_listContext* declaration_list();

  class  DeclarationContext : public antlr4::ParserRuleContext {
  public:
    DeclarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    PropertyContext *property();
    antlr4::tree::TerminalNode *COLON();
    Value_listContext *value_list();
    antlr4::tree::TerminalNode *IMPORTANT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  DeclarationContext* declaration();

  class  PropertyContext : public antlr4::ParserRuleContext {
  public:
    PropertyContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  PropertyContext* property();

  class  Value_listContext : public antlr4::ParserRuleContext {
  public:
    Value_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<ValueContext *> value();
    ValueContext* value(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Value_listContext* value_list();

  class  ValueContext : public antlr4::ParserRuleContext {
  public:
    ValueContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NUMBER();
    UnitContext *unit();
    antlr4::tree::TerminalNode *PERCENTAGE();
    antlr4::tree::TerminalNode *STRING();
    antlr4::tree::TerminalNode *IDENT();
    antlr4::tree::TerminalNode *HASH();
    antlr4::tree::TerminalNode *URI();
    antlr4::tree::TerminalNode *FUNCTION();
    antlr4::tree::TerminalNode *RPAREN();
    std::vector<Any_valueContext *> any_value();
    Any_valueContext* any_value(size_t i);
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *LBRACKET();
    antlr4::tree::TerminalNode *RBRACKET();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ValueContext* value();

  class  UnitContext : public antlr4::ParserRuleContext {
  public:
    UnitContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  UnitContext* unit();

  class  Any_valueContext : public antlr4::ParserRuleContext {
  public:
    Any_valueContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NUMBER();
    UnitContext *unit();
    antlr4::tree::TerminalNode *PERCENTAGE();
    antlr4::tree::TerminalNode *STRING();
    antlr4::tree::TerminalNode *IDENT();
    antlr4::tree::TerminalNode *HASH();
    antlr4::tree::TerminalNode *URI();
    antlr4::tree::TerminalNode *FUNCTION();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *LBRACKET();
    antlr4::tree::TerminalNode *RBRACKET();
    antlr4::tree::TerminalNode *LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    antlr4::tree::TerminalNode *SEMICOLON();
    antlr4::tree::TerminalNode *COLON();
    antlr4::tree::TerminalNode *COMMA();
    antlr4::tree::TerminalNode *PLUS();
    antlr4::tree::TerminalNode *MINUS();
    antlr4::tree::TerminalNode *STAR();
    antlr4::tree::TerminalNode *SLASH();
    antlr4::tree::TerminalNode *EQUALS();
    antlr4::tree::TerminalNode *DOT();
    antlr4::tree::TerminalNode *GREATER();
    antlr4::tree::TerminalNode *TILDE();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Any_valueContext* any_value();


  // By default the static state used to implement the parser is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:
};

