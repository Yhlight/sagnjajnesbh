
// Generated from grammars/CSS3.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  CSS3_improvedLexer : public antlr4::Lexer {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, T__6 = 7, 
    T__7 = 8, T__8 = 9, T__9 = 10, T__10 = 11, T__11 = 12, T__12 = 13, T__13 = 14, 
    T__14 = 15, T__15 = 16, T__16 = 17, T__17 = 18, WHITESPACE = 19, COMMENT = 20, 
    STRING = 21, HASH = 22, NUMBER = 23, PERCENTAGE = 24, DIMENSION = 25, 
    URL = 26, FUNCTION = 27, IDENT = 28, COMMA = 29, LBRACE = 30, RBRACE = 31, 
    LPAREN = 32, RPAREN = 33, LBRACKET = 34, RBRACKET = 35, SEMICOLON = 36, 
    COLON = 37, DOT = 38, HASH_SYMBOL = 39, STAR = 40, PLUS = 41, MINUS = 42, 
    SLASH = 43, PIPE = 44, TILDE = 45, GREATER = 46, EQUALS = 47, CARET_EQUALS = 48, 
    DOLLAR_EQUALS = 49, STAR_EQUALS = 50, TILDE_EQUALS = 51, PIPE_EQUALS = 52, 
    EXCLAMATION = 53
  };

  explicit CSS3_improvedLexer(antlr4::CharStream *input);

  ~CSS3_improvedLexer() override;


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

