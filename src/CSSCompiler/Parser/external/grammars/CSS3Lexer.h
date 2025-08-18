
// Generated from external/grammars/CSS3Lexer.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  CSS3Lexer : public antlr4::Lexer {
public:
  enum {
    CHARSET = 1, IMPORT = 2, SEMICOLON = 3, COMMA = 4, LBRACE = 5, RBRACE = 6, 
    LPAREN = 7, RPAREN = 8, LBRACKET = 9, RBRACKET = 10, COLON = 11, DOT = 12, 
    HASH_SYMBOL = 13, STAR = 14, PLUS = 15, GREATER = 16, TILDE = 17, MINUS = 18, 
    SLASH = 19, PIPE = 20, EQUALS = 21, TILDE_EQUALS = 22, PIPE_EQUALS = 23, 
    AT = 24, IMPORTANT = 25, STRING = 26, IDENTIFIER = 27, FUNCTION = 28, 
    NUMBER = 29, PERCENTAGE = 30, LENGTH = 31, EMS = 32, EXS = 33, ANGLE = 34, 
    TIME = 35, FREQ = 36, URI = 37, HASH = 38, CLASS = 39, ATKEYWORD = 40, 
    UNICODE_RANGE = 41, WS = 42, COMMENT = 43
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

