
// Generated from grammars/JavaScript.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  JavaScriptLexer : public antlr4::Lexer {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, T__6 = 7, 
    T__7 = 8, T__8 = 9, T__9 = 10, T__10 = 11, T__11 = 12, T__12 = 13, T__13 = 14, 
    T__14 = 15, T__15 = 16, MultiLineComment = 17, SingleLineComment = 18, 
    NullLiteral = 19, BooleanLiteral = 20, NumericLiteral = 21, StringLiteral = 22, 
    Identifier = 23, WhiteSpaces = 24, LineTerminator = 25, OpenBracket = 26, 
    CloseBracket = 27, OpenParen = 28, CloseParen = 29, OpenBrace = 30, 
    CloseBrace = 31, SemiColon = 32, Comma = 33, Assign = 34, QuestionMark = 35, 
    Colon = 36, Dot = 37, PlusPlus = 38, MinusMinus = 39, Plus = 40, Minus = 41, 
    BitNot = 42, Not = 43, Multiply = 44, Divide = 45, Modulus = 46, LessThan = 47, 
    MoreThan = 48, LessThanEquals = 49, GreaterThanEquals = 50, Equals_ = 51, 
    NotEquals = 52, IdentityEquals = 53, IdentityNotEquals = 54, And = 55, 
    Or = 56, PlusAssign = 57, MinusAssign = 58, MultiplyAssign = 59, DivideAssign = 60
  };

  explicit JavaScriptLexer(antlr4::CharStream *input);

  ~JavaScriptLexer() override;


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

