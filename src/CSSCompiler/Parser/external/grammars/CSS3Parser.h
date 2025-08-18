
// Generated from external/grammars/CSS3.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  CSS3Parser : public antlr4::Parser {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, AT_KEYWORD = 7, 
    IDENT = 8, FUNCTION = 9, STRING = 10, HASH = 11, NUMBER = 12, PERCENTAGE = 13, 
    DIMENSION = 14, URI = 15, UNICODE_RANGE = 16, INCLUDES = 17, DASHMATCH = 18, 
    PREFIX_MATCH = 19, SUFFIX_MATCH = 20, SUBSTRING_MATCH = 21, COLUMN = 22, 
    CDO = 23, CDC = 24, IMPORTANT = 25, SEMICOLON = 26, COLON = 27, COMMA = 28, 
    DELIM = 29, WS = 30, COMMENT = 31
  };

  enum {
    RuleStylesheet = 0, RuleRule = 1, RuleAt_rule = 2, RuleQualified_rule = 3, 
    RuleDeclaration_list = 4, RuleDeclaration = 5, RuleComponent_value = 6, 
    RuleBlock = 7, RuleFunction = 8, RulePreserved_token = 9
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
  class RuleContext;
  class At_ruleContext;
  class Qualified_ruleContext;
  class Declaration_listContext;
  class DeclarationContext;
  class Component_valueContext;
  class BlockContext;
  class FunctionContext;
  class Preserved_tokenContext; 

  class  StylesheetContext : public antlr4::ParserRuleContext {
  public:
    StylesheetContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *EOF();
    std::vector<RuleContext *> rule_();
    RuleContext* rule_(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  StylesheetContext* stylesheet();

  class  RuleContext : public antlr4::ParserRuleContext {
  public:
    RuleContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Qualified_ruleContext *qualified_rule();
    At_ruleContext *at_rule();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  RuleContext* rule_();

  class  At_ruleContext : public antlr4::ParserRuleContext {
  public:
    At_ruleContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *AT_KEYWORD();
    Declaration_listContext *declaration_list();
    antlr4::tree::TerminalNode *SEMICOLON();
    std::vector<Component_valueContext *> component_value();
    Component_valueContext* component_value(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  At_ruleContext* at_rule();

  class  Qualified_ruleContext : public antlr4::ParserRuleContext {
  public:
    Qualified_ruleContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Declaration_listContext *declaration_list();
    std::vector<Component_valueContext *> component_value();
    Component_valueContext* component_value(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Qualified_ruleContext* qualified_rule();

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
    antlr4::tree::TerminalNode *IDENT();
    antlr4::tree::TerminalNode *COLON();
    std::vector<Component_valueContext *> component_value();
    Component_valueContext* component_value(size_t i);
    antlr4::tree::TerminalNode *IMPORTANT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  DeclarationContext* declaration();

  class  Component_valueContext : public antlr4::ParserRuleContext {
  public:
    Component_valueContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Preserved_tokenContext *preserved_token();
    BlockContext *block();
    FunctionContext *function();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Component_valueContext* component_value();

  class  BlockContext : public antlr4::ParserRuleContext {
  public:
    BlockContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Component_valueContext *> component_value();
    Component_valueContext* component_value(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  BlockContext* block();

  class  FunctionContext : public antlr4::ParserRuleContext {
  public:
    FunctionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *FUNCTION();
    std::vector<Component_valueContext *> component_value();
    Component_valueContext* component_value(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  FunctionContext* function();

  class  Preserved_tokenContext : public antlr4::ParserRuleContext {
  public:
    Preserved_tokenContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENT();
    antlr4::tree::TerminalNode *STRING();
    antlr4::tree::TerminalNode *DELIM();
    antlr4::tree::TerminalNode *NUMBER();
    antlr4::tree::TerminalNode *PERCENTAGE();
    antlr4::tree::TerminalNode *DIMENSION();
    antlr4::tree::TerminalNode *URI();
    antlr4::tree::TerminalNode *HASH();
    antlr4::tree::TerminalNode *UNICODE_RANGE();
    antlr4::tree::TerminalNode *INCLUDES();
    antlr4::tree::TerminalNode *DASHMATCH();
    antlr4::tree::TerminalNode *PREFIX_MATCH();
    antlr4::tree::TerminalNode *SUFFIX_MATCH();
    antlr4::tree::TerminalNode *SUBSTRING_MATCH();
    antlr4::tree::TerminalNode *COLUMN();
    antlr4::tree::TerminalNode *CDO();
    antlr4::tree::TerminalNode *CDC();
    antlr4::tree::TerminalNode *COLON();
    antlr4::tree::TerminalNode *SEMICOLON();
    antlr4::tree::TerminalNode *COMMA();
    antlr4::tree::TerminalNode *WS();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Preserved_tokenContext* preserved_token();


  // By default the static state used to implement the parser is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:
};

