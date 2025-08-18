
// Generated from grammars/CSS3Lexer.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  CSS3Lexer : public antlr4::Lexer {
public:
  enum {
    OpenBracket = 1, CloseBracket = 2, OpenParen = 3, CloseParen = 4, OpenBrace = 5, 
    CloseBrace = 6, SemiColon = 7, Equal = 8, Colon = 9, Dot = 10, Multiply = 11, 
    Divide = 12, Pipe = 13, Underscore = 14, Plus = 15, Greater = 16, Tilde = 17, 
    Comma = 18, Minus = 19, Only = 20, Not = 21, And = 22, Important = 23, 
    From = 24, To = 25, Comment = 26, Whitespace = 27, Hash = 28, AtKeyword = 29, 
    Ident = 30, String_ = 31, Number = 32, Percentage = 33, Dimension = 34, 
    Uri = 35, UnicodeRange = 36, Any = 37
  };

  enum {
    ERROR = 2
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

