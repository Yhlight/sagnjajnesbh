
// Generated from external/grammars/JavaScript.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  JavaScriptLexer : public antlr4::Lexer {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, T__6 = 7, 
    T__7 = 8, T__8 = 9, T__9 = 10, T__10 = 11, T__11 = 12, T__12 = 13, T__13 = 14, 
    T__14 = 15, T__15 = 16, T__16 = 17, T__17 = 18, T__18 = 19, T__19 = 20, 
    T__20 = 21, T__21 = 22, T__22 = 23, T__23 = 24, T__24 = 25, T__25 = 26, 
    T__26 = 27, T__27 = 28, T__28 = 29, T__29 = 30, T__30 = 31, T__31 = 32, 
    T__32 = 33, T__33 = 34, T__34 = 35, T__35 = 36, T__36 = 37, T__37 = 38, 
    T__38 = 39, T__39 = 40, T__40 = 41, T__41 = 42, T__42 = 43, T__43 = 44, 
    T__44 = 45, T__45 = 46, T__46 = 47, T__47 = 48, NULL_ = 49, BooleanLiteral = 50, 
    DecimalLiteral = 51, HexIntegerLiteral = 52, OctalIntegerLiteral = 53, 
    BREAK = 54, CASE = 55, CATCH = 56, CONTINUE = 57, DEBUGGER = 58, DEFAULT = 59, 
    DELETE = 60, DO = 61, ELSE = 62, FINALLY = 63, FOR = 64, FUNCTION = 65, 
    IF = 66, IN = 67, INSTANCEOF = 68, NEW = 69, RETURN = 70, SWITCH = 71, 
    THIS = 72, THROW = 73, TRY = 74, TYPEOF = 75, VAR = 76, VOID = 77, WHILE = 78, 
    WITH = 79, CONST = 80, LET = 81, GETTER = 82, SETTER = 83, StringLiteral = 84, 
    Identifier = 85, WhiteSpaces = 86, LineTerminator = 87, SingleLineComment = 88, 
    MultiLineComment = 89
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

