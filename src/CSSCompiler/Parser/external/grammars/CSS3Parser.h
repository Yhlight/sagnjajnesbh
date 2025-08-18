
// Generated from external/grammars/CSS3Parser.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  CSS3Parser : public antlr4::Parser {
public:
  enum {
    OpenBracket = 1, CloseBracket = 2, OpenParen = 3, CloseParen = 4, OpenBrace = 5, 
    CloseBrace = 6, SemiColon = 7, Equal = 8, Colon = 9, Dot = 10, Multiply = 11, 
    Divide = 12, Pipe = 13, Underscore = 14, Plus = 15, Greater = 16, Tilde = 17, 
    Comma = 18, Minus = 19, Only = 20, Not = 21, And = 22, Important = 23, 
    From = 24, To = 25, Comment = 26, Whitespace = 27, Hash = 28, AtKeyword = 29, 
    Ident = 30, String_ = 31, Number = 32, Percentage = 33, Dimension = 34, 
    Uri = 35, UnicodeRange = 36, Any = 37, CHARSET = 38, STRING = 39, SEMICOLON = 40, 
    IMPORT = 41, URI = 42, IDENTIFIER = 43, COMMA = 44, ATKEYWORD = 45, 
    LBRACE = 46, RBRACE = 47, PLUS = 48, GREATER = 49, TILDE = 50, WS = 51, 
    HASH = 52, CLASS = 53, STAR = 54, LBRACKET = 55, EQUALS = 56, TILDE_EQUALS = 57, 
    PIPE_EQUALS = 58, RBRACKET = 59, COLON = 60, IMPORTANT = 61, NUMBER = 62, 
    PERCENTAGE = 63, LENGTH = 64, EMS = 65, EXS = 66, ANGLE = 67, TIME = 68, 
    FREQ = 69, FUNCTION = 70, RPAREN = 71, SLASH = 72, MINUS = 73, UNICODE_RANGE = 74, 
    LPAREN = 75
  };

  enum {
    RuleStylesheet = 0, RuleCharset = 1, RuleImportRule = 2, RuleMediaList = 3, 
    RuleStatement = 4, RuleAtRule = 5, RuleBlock = 6, RuleRuleset = 7, RuleSelector = 8, 
    RuleCombinator = 9, RuleSimpleSelector = 10, RuleElementName = 11, RuleAttrib = 12, 
    RulePseudo = 13, RuleDeclaration = 14, RuleProperty = 15, RuleExpr = 16, 
    RuleTerm = 17, RuleFunction = 18, RuleHexcolor = 19, RuleOperator = 20, 
    RuleUnaryOperator = 21, RuleAny = 22
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
  class CharsetContext;
  class ImportRuleContext;
  class MediaListContext;
  class StatementContext;
  class AtRuleContext;
  class BlockContext;
  class RulesetContext;
  class SelectorContext;
  class CombinatorContext;
  class SimpleSelectorContext;
  class ElementNameContext;
  class AttribContext;
  class PseudoContext;
  class DeclarationContext;
  class PropertyContext;
  class ExprContext;
  class TermContext;
  class FunctionContext;
  class HexcolorContext;
  class OperatorContext;
  class UnaryOperatorContext;
  class AnyContext; 

  class  StylesheetContext : public antlr4::ParserRuleContext {
  public:
    StylesheetContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *EOF();
    std::vector<CharsetContext *> charset();
    CharsetContext* charset(size_t i);
    std::vector<ImportRuleContext *> importRule();
    ImportRuleContext* importRule(size_t i);
    std::vector<StatementContext *> statement();
    StatementContext* statement(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  StylesheetContext* stylesheet();

  class  CharsetContext : public antlr4::ParserRuleContext {
  public:
    CharsetContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *CHARSET();
    antlr4::tree::TerminalNode *STRING();
    antlr4::tree::TerminalNode *SEMICOLON();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  CharsetContext* charset();

  class  ImportRuleContext : public antlr4::ParserRuleContext {
  public:
    ImportRuleContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IMPORT();
    antlr4::tree::TerminalNode *SEMICOLON();
    antlr4::tree::TerminalNode *STRING();
    antlr4::tree::TerminalNode *URI();
    MediaListContext *mediaList();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ImportRuleContext* importRule();

  class  MediaListContext : public antlr4::ParserRuleContext {
  public:
    MediaListContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> IDENTIFIER();
    antlr4::tree::TerminalNode* IDENTIFIER(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  MediaListContext* mediaList();

  class  StatementContext : public antlr4::ParserRuleContext {
  public:
    StatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    RulesetContext *ruleset();
    AtRuleContext *atRule();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  StatementContext* statement();

  class  AtRuleContext : public antlr4::ParserRuleContext {
  public:
    AtRuleContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ATKEYWORD();
    BlockContext *block();
    antlr4::tree::TerminalNode *SEMICOLON();
    std::vector<AnyContext *> any();
    AnyContext* any(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  AtRuleContext* atRule();

  class  BlockContext : public antlr4::ParserRuleContext {
  public:
    BlockContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    std::vector<AnyContext *> any();
    AnyContext* any(size_t i);
    std::vector<BlockContext *> block();
    BlockContext* block(size_t i);
    std::vector<antlr4::tree::TerminalNode *> ATKEYWORD();
    antlr4::tree::TerminalNode* ATKEYWORD(size_t i);
    std::vector<antlr4::tree::TerminalNode *> SEMICOLON();
    antlr4::tree::TerminalNode* SEMICOLON(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  BlockContext* block();

  class  RulesetContext : public antlr4::ParserRuleContext {
  public:
    RulesetContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<SelectorContext *> selector();
    SelectorContext* selector(size_t i);
    antlr4::tree::TerminalNode *LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    std::vector<DeclarationContext *> declaration();
    DeclarationContext* declaration(size_t i);
    std::vector<antlr4::tree::TerminalNode *> SEMICOLON();
    antlr4::tree::TerminalNode* SEMICOLON(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  RulesetContext* ruleset();

  class  SelectorContext : public antlr4::ParserRuleContext {
  public:
    SelectorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<SimpleSelectorContext *> simpleSelector();
    SimpleSelectorContext* simpleSelector(size_t i);
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

  class  SimpleSelectorContext : public antlr4::ParserRuleContext {
  public:
    SimpleSelectorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ElementNameContext *elementName();
    std::vector<antlr4::tree::TerminalNode *> HASH();
    antlr4::tree::TerminalNode* HASH(size_t i);
    std::vector<antlr4::tree::TerminalNode *> CLASS();
    antlr4::tree::TerminalNode* CLASS(size_t i);
    std::vector<AttribContext *> attrib();
    AttribContext* attrib(size_t i);
    std::vector<PseudoContext *> pseudo();
    PseudoContext* pseudo(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  SimpleSelectorContext* simpleSelector();

  class  ElementNameContext : public antlr4::ParserRuleContext {
  public:
    ElementNameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENTIFIER();
    antlr4::tree::TerminalNode *STAR();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ElementNameContext* elementName();

  class  AttribContext : public antlr4::ParserRuleContext {
  public:
    AttribContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LBRACKET();
    std::vector<antlr4::tree::TerminalNode *> IDENTIFIER();
    antlr4::tree::TerminalNode* IDENTIFIER(size_t i);
    antlr4::tree::TerminalNode *RBRACKET();
    antlr4::tree::TerminalNode *EQUALS();
    antlr4::tree::TerminalNode *TILDE_EQUALS();
    antlr4::tree::TerminalNode *PIPE_EQUALS();
    antlr4::tree::TerminalNode *STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  AttribContext* attrib();

  class  PseudoContext : public antlr4::ParserRuleContext {
  public:
    PseudoContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> COLON();
    antlr4::tree::TerminalNode* COLON(size_t i);
    antlr4::tree::TerminalNode *IDENTIFIER();
    FunctionContext *function();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  PseudoContext* pseudo();

  class  DeclarationContext : public antlr4::ParserRuleContext {
  public:
    DeclarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    PropertyContext *property();
    antlr4::tree::TerminalNode *COLON();
    ExprContext *expr();
    antlr4::tree::TerminalNode *IMPORTANT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  DeclarationContext* declaration();

  class  PropertyContext : public antlr4::ParserRuleContext {
  public:
    PropertyContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENTIFIER();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  PropertyContext* property();

  class  ExprContext : public antlr4::ParserRuleContext {
  public:
    ExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<TermContext *> term();
    TermContext* term(size_t i);
    std::vector<OperatorContext *> operator_();
    OperatorContext* operator_(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ExprContext* expr();

  class  TermContext : public antlr4::ParserRuleContext {
  public:
    TermContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NUMBER();
    antlr4::tree::TerminalNode *PERCENTAGE();
    antlr4::tree::TerminalNode *LENGTH();
    antlr4::tree::TerminalNode *EMS();
    antlr4::tree::TerminalNode *EXS();
    antlr4::tree::TerminalNode *ANGLE();
    antlr4::tree::TerminalNode *TIME();
    antlr4::tree::TerminalNode *FREQ();
    antlr4::tree::TerminalNode *STRING();
    antlr4::tree::TerminalNode *IDENTIFIER();
    antlr4::tree::TerminalNode *URI();
    HexcolorContext *hexcolor();
    FunctionContext *function();
    UnaryOperatorContext *unaryOperator();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  TermContext* term();

  class  FunctionContext : public antlr4::ParserRuleContext {
  public:
    FunctionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *FUNCTION();
    ExprContext *expr();
    antlr4::tree::TerminalNode *RPAREN();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  FunctionContext* function();

  class  HexcolorContext : public antlr4::ParserRuleContext {
  public:
    HexcolorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *HASH();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  HexcolorContext* hexcolor();

  class  OperatorContext : public antlr4::ParserRuleContext {
  public:
    OperatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *SLASH();
    antlr4::tree::TerminalNode *COMMA();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  OperatorContext* operator_();

  class  UnaryOperatorContext : public antlr4::ParserRuleContext {
  public:
    UnaryOperatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *MINUS();
    antlr4::tree::TerminalNode *PLUS();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  UnaryOperatorContext* unaryOperator();

  class  AnyContext : public antlr4::ParserRuleContext {
  public:
    AnyContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENTIFIER();
    antlr4::tree::TerminalNode *NUMBER();
    antlr4::tree::TerminalNode *PERCENTAGE();
    antlr4::tree::TerminalNode *LENGTH();
    antlr4::tree::TerminalNode *EMS();
    antlr4::tree::TerminalNode *EXS();
    antlr4::tree::TerminalNode *ANGLE();
    antlr4::tree::TerminalNode *TIME();
    antlr4::tree::TerminalNode *FREQ();
    antlr4::tree::TerminalNode *STRING();
    antlr4::tree::TerminalNode *URI();
    antlr4::tree::TerminalNode *HASH();
    antlr4::tree::TerminalNode *UNICODE_RANGE();
    antlr4::tree::TerminalNode *TILDE_EQUALS();
    antlr4::tree::TerminalNode *PIPE_EQUALS();
    antlr4::tree::TerminalNode *COLON();
    antlr4::tree::TerminalNode *FUNCTION();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();
    std::vector<AnyContext *> any();
    AnyContext* any(size_t i);
    antlr4::tree::TerminalNode *LBRACKET();
    antlr4::tree::TerminalNode *RBRACKET();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  AnyContext* any();


  // By default the static state used to implement the parser is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:
};

