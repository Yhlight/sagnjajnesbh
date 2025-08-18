
// Generated from grammars/JavaScriptLexer.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  JavaScriptLexer : public antlr4::Lexer {
public:
  enum {
    MultiLineComment = 1, SingleLineComment = 2, RegularExpressionLiteral = 3, 
    Break = 4, Do = 5, Instanceof = 6, Typeof = 7, Case = 8, Else = 9, New = 10, 
    Var = 11, Catch = 12, Finally = 13, Return = 14, Void = 15, Continue = 16, 
    For = 17, Switch = 18, While = 19, Debugger = 20, Function = 21, This = 22, 
    With = 23, Default = 24, If = 25, Throw = 26, Delete = 27, In = 28, 
    Try = 29, Class = 30, Enum = 31, Extends = 32, Super = 33, Const = 34, 
    Export = 35, Import = 36, Async = 37, Await = 38, Yield = 39, Let = 40, 
    Static = 41, Implements = 42, Interface = 43, Package = 44, Private = 45, 
    Protected = 46, Public = 47, NullLiteral = 48, BooleanLiteral = 49, 
    DecimalLiteral = 50, HexIntegerLiteral = 51, OctalIntegerLiteral = 52, 
    OctalIntegerLiteral2 = 53, BinaryIntegerLiteral = 54, OpenBracket = 55, 
    CloseBracket = 56, OpenParen = 57, CloseParen = 58, OpenBrace = 59, 
    CloseBrace = 60, SemiColon = 61, Comma = 62, Assign = 63, QuestionMark = 64, 
    Colon = 65, Ellipsis = 66, Dot = 67, PlusPlus = 68, MinusMinus = 69, 
    Plus = 70, Minus = 71, BitNot = 72, Not = 73, Multiply = 74, Divide = 75, 
    Modulus = 76, Power = 77, NullCoalesce = 78, Hashtag = 79, RightShiftArithmetic = 80, 
    LeftShiftArithmetic = 81, RightShiftLogical = 82, LessThan = 83, MoreThan = 84, 
    LessThanEquals = 85, GreaterThanEquals = 86, Equals_ = 87, NotEquals = 88, 
    IdentityEquals = 89, IdentityNotEquals = 90, BitAnd = 91, BitXOr = 92, 
    BitOr = 93, And = 94, Or = 95, MultiplyAssign = 96, DivideAssign = 97, 
    ModulusAssign = 98, PlusAssign = 99, MinusAssign = 100, LeftShiftArithmeticAssign = 101, 
    RightShiftArithmeticAssign = 102, RightShiftLogicalAssign = 103, BitAndAssign = 104, 
    BitXorAssign = 105, BitOrAssign = 106, PowerAssign = 107, Identifier = 108, 
    StringLiteral = 109, TemplateStringLiteral = 110, WhiteSpaces = 111, 
    LineTerminator = 112
  };

  enum {
    ERROR = 2
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

