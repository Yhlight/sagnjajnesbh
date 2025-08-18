
// Generated from grammars/CSS3.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  CSS3Lexer : public antlr4::Lexer {
public:
  enum {
    AT_KEYWORD = 1, HASH = 2, STRING = 3, URI = 4, FUNCTION = 5, NUMBER = 6, 
    PERCENTAGE = 7, IDENT = 8, INCLUDES = 9, DASHMATCH = 10, PREFIXMATCH = 11, 
    SUFFIXMATCH = 12, SUBSTRINGMATCH = 13, EQUALS = 14, SEMICOLON = 15, 
    LBRACE = 16, RBRACE = 17, LPAREN = 18, RPAREN = 19, LBRACKET = 20, RBRACKET = 21, 
    COMMA = 22, COLON = 23, DOT = 24, PLUS = 25, MINUS = 26, STAR = 27, 
    SLASH = 28, GREATER = 29, TILDE = 30, IMPORTANT = 31, WS = 32, COMMENT = 33
  };

  explicit CSS3Lexer(antlr4::CharStream *input);

  ~CSS3Lexer() override;


  std::string getGrammarFileName() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const std::vector<std::string>& getChannelNames() const override;

  const std::vector<std::string>& getModeNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;

  const antlr4::atn::ATN& getATN() const override;

  void action(antlr4::RuleContext *context, size_t ruleIndex, size_t actionIndex) override;

  // By default the static state used to implement the lexer is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:

  // Individual action functions triggered by action() above.
  void ESCAPEAction(antlr4::RuleContext *context, size_t actionIndex);

  // Individual semantic predicate functions triggered by sempred() above.

};

