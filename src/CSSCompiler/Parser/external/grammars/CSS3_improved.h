
// Generated from external/grammars/CSS3_improved.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  CSS3_improved : public antlr4::Lexer {
public:
  enum {
    OpenBracket = 1, CloseBracket = 2, OpenParen = 3, CloseParen = 4, OpenBrace = 5, 
    CloseBrace = 6, SemiColon = 7, Equal = 8, Colon = 9, Dot = 10, Multiply = 11, 
    Divide = 12, Plus = 13, Minus = 14, Hash = 15, Comma = 16, Comment = 17, 
    Space = 18, Ident = 19, Number = 20, String_ = 21, Url = 22, Percentage = 23, 
    Length = 24, Angle = 25, Time = 26, Frequency = 27, Function_ = 28, 
    AtKeyword = 29, Important = 30, Any = 31
  };

  enum {
    ERROR = 2
  };

  explicit CSS3_improved(antlr4::CharStream *input);

  ~CSS3_improved() override;


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

