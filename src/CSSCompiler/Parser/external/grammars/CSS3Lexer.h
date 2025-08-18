
// Generated from external/grammars/CSS3.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  CSS3Lexer : public antlr4::Lexer {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, AT_KEYWORD = 7, 
    IDENT = 8, FUNCTION = 9, STRING = 10, HASH = 11, NUMBER = 12, PERCENTAGE = 13, 
    DIMENSION = 14, URI = 15, UNICODE_RANGE = 16, INCLUDES = 17, DASHMATCH = 18, 
    PREFIX_MATCH = 19, SUFFIX_MATCH = 20, SUBSTRING_MATCH = 21, COLUMN = 22, 
    CDO = 23, CDC = 24, IMPORTANT = 25, SEMICOLON = 26, COLON = 27, COMMA = 28, 
    DELIM = 29, WS = 30, COMMENT = 31
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

