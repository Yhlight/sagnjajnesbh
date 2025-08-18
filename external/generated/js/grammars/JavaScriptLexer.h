
// Generated from grammars/JavaScript.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  JavaScriptLexer : public antlr4::Lexer {
public:
  enum {
    BREAK = 1, CASE = 2, CATCH = 3, CLASS = 4, CONST = 5, CONTINUE = 6, 
    DEBUGGER = 7, DEFAULT = 8, DELETE = 9, DO = 10, ELSE = 11, ENUM = 12, 
    EXPORT = 13, EXTENDS = 14, FINALLY = 15, FOR = 16, FUNCTION = 17, IF = 18, 
    IMPORT = 19, IN = 20, INSTANCEOF = 21, NEW = 22, RETURN = 23, SUPER = 24, 
    SWITCH = 25, THIS = 26, THROW = 27, TRY = 28, TYPEOF = 29, VAR = 30, 
    VOID = 31, WHILE = 32, WITH = 33, YIELD = 34, ASYNC = 35, AWAIT = 36, 
    LET = 37, OF = 38, STATIC = 39, GET = 40, SET = 41, NullLiteral = 42, 
    BooleanLiteral = 43, DecimalLiteral = 44, HexIntegerLiteral = 45, OctalIntegerLiteral = 46, 
    BinaryIntegerLiteral = 47, StringLiteral = 48, TemplateStringLiteral = 49, 
    RegularExpressionLiteral = 50, Identifier = 51, ASSIGN = 52, QUESTION = 53, 
    COLON = 54, ARROW = 55, ELLIPSIS = 56, DOT = 57, PLUSPLUS = 58, MINUSMINUS = 59, 
    PLUS = 60, MINUS = 61, BITNOT = 62, NOT = 63, STAR = 64, SLASH = 65, 
    MODULUS = 66, POWER = 67, NULLCOALESCE = 68, LSHIFT = 69, RSHIFT = 70, 
    URSHIFT = 71, LT = 72, GT = 73, LE = 74, GE = 75, EQ = 76, NE = 77, 
    SEQ = 78, SNE = 79, BITAND = 80, BITXOR = 81, BITOR = 82, AND = 83, 
    OR = 84, STARASSIGN = 85, SLASHASSIGN = 86, MODULUSASSIGN = 87, PLUSASSIGN = 88, 
    MINUSASSIGN = 89, LSHIFTASSIGN = 90, RSHIFTASSIGN = 91, URSHIFTASSIGN = 92, 
    BITANDASSIGN = 93, BITXORASSIGN = 94, BITORASSIGN = 95, POWERASSIGN = 96, 
    SEMICOLON = 97, COMMA = 98, LPAREN = 99, RPAREN = 100, LBRACE = 101, 
    RBRACE = 102, LBRACKET = 103, RBRACKET = 104, WS = 105, MultiLineComment = 106, 
    SingleLineComment = 107
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

