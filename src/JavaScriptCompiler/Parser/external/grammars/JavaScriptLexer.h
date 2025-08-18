
// Generated from external/grammars/JavaScriptLexer.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  JavaScriptLexer : public antlr4::Lexer {
public:
  enum {
    NULL_ = 1, BooleanLiteral = 2, DecimalLiteral = 3, HexIntegerLiteral = 4, 
    OctalIntegerLiteral = 5, StringLiteral = 6, RegularExpressionLiteral = 7, 
    BREAK = 8, DO = 9, INSTANCEOF = 10, TYPEOF = 11, CASE = 12, ELSE = 13, 
    NEW = 14, VAR = 15, CATCH = 16, FINALLY = 17, RETURN = 18, VOID = 19, 
    CONTINUE = 20, FOR = 21, SWITCH = 22, WHILE = 23, DEBUGGER = 24, FUNCTION = 25, 
    THIS = 26, WITH = 27, DEFAULT = 28, IF = 29, THROW = 30, DELETE = 31, 
    IN = 32, TRY = 33, GET = 34, SET = 35, LBRACE = 36, RBRACE = 37, LPAREN = 38, 
    RPAREN = 39, LBRACKET = 40, RBRACKET = 41, SEMICOLON = 42, COMMA = 43, 
    DOT = 44, QUESTION = 45, COLON = 46, PLUS_PLUS = 47, MINUS_MINUS = 48, 
    PLUS = 49, MINUS = 50, MULTIPLY = 51, DIVIDE = 52, MODULO = 53, LSHIFT = 54, 
    RSHIFT = 55, URSHIFT = 56, LT = 57, GT = 58, LE = 59, GE = 60, EQ = 61, 
    NE = 62, STRICT_EQ = 63, STRICT_NE = 64, BIT_AND = 65, BIT_XOR = 66, 
    BIT_OR = 67, AND = 68, OR = 69, ASSIGN = 70, PLUS_ASSIGN = 71, MINUS_ASSIGN = 72, 
    MULTIPLY_ASSIGN = 73, DIVIDE_ASSIGN = 74, MODULO_ASSIGN = 75, LSHIFT_ASSIGN = 76, 
    RSHIFT_ASSIGN = 77, URSHIFT_ASSIGN = 78, BIT_AND_ASSIGN = 79, BIT_XOR_ASSIGN = 80, 
    BIT_OR_ASSIGN = 81, TILDE = 82, NOT = 83, Identifier = 84, WhiteSpaces = 85, 
    LineTerminator = 86, MultiLineComment = 87, SingleLineComment = 88
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

