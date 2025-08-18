
// Generated from external/grammars/JavaScript_improved.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  JavaScript_improvedLexer : public antlr4::Lexer {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, T__6 = 7, 
    T__7 = 8, T__8 = 9, T__9 = 10, T__10 = 11, T__11 = 12, T__12 = 13, T__13 = 14, 
    T__14 = 15, T__15 = 16, T__16 = 17, T__17 = 18, T__18 = 19, T__19 = 20, 
    MultiLineComment = 21, SingleLineComment = 22, WhiteSpace = 23, Break = 24, 
    Case = 25, Catch = 26, Class = 27, Const = 28, Continue = 29, Debugger = 30, 
    Default = 31, Delete = 32, Do = 33, Else = 34, Export = 35, Extends = 36, 
    Finally = 37, For = 38, Function = 39, If = 40, Import = 41, In = 42, 
    Instanceof = 43, Let = 44, New = 45, Return = 46, Super = 47, Switch = 48, 
    This = 49, Throw = 50, Try = 51, Typeof = 52, Var = 53, Void = 54, While = 55, 
    With = 56, Yield = 57, BooleanLiteral = 58, NullLiteral = 59, UndefinedLiteral = 60, 
    NumericLiteral = 61, StringLiteral = 62, Identifier = 63, AssignmentOperator = 64, 
    OpenBracket = 65, CloseBracket = 66, OpenParen = 67, CloseParen = 68, 
    OpenBrace = 69, CloseBrace = 70, SemiColon = 71, Comma = 72, Dot = 73, 
    Question = 74, Colon = 75, Literal = 76, Any = 77
  };

  explicit JavaScript_improvedLexer(antlr4::CharStream *input);

  ~JavaScript_improvedLexer() override;


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

