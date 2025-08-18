
// Generated from grammars/CSS3.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  CSS3Lexer : public antlr4::Lexer {
public:
  enum {
    T__0 = 1, T__1 = 2, WHITESPACE = 3, COMMENT = 4, STRING = 5, HASH = 6, 
    NUMBER = 7, PERCENTAGE = 8, DIMENSION = 9, URL_CHARS = 10, IDENT = 11, 
    LBRACE = 12, RBRACE = 13, LPAREN = 14, RPAREN = 15, LBRACKET = 16, RBRACKET = 17, 
    SEMICOLON = 18, COLON = 19, DOT = 20, COMMA = 21, PLUS = 22, MINUS = 23, 
    MULTIPLY = 24, DIVIDE = 25, EQUALS = 26, TILDE = 27, GREATER = 28, PIPE = 29, 
    AT = 30
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

  // By default the static state used to implement the lexer is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:

  // Individual action functions triggered by action() above.

  // Individual semantic predicate functions triggered by sempred() above.

};

