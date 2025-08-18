
// Generated from grammars/CSS3.g4 by ANTLR 4.13.2


#include "CSS3Listener.h"

#include "CSS3Parser.h"


using namespace antlrcpp;

using namespace antlr4;

namespace {

struct CSS3ParserStaticData final {
  CSS3ParserStaticData(std::vector<std::string> ruleNames,
                        std::vector<std::string> literalNames,
                        std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  CSS3ParserStaticData(const CSS3ParserStaticData&) = delete;
  CSS3ParserStaticData(CSS3ParserStaticData&&) = delete;
  CSS3ParserStaticData& operator=(const CSS3ParserStaticData&) = delete;
  CSS3ParserStaticData& operator=(CSS3ParserStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag css3ParserOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
std::unique_ptr<CSS3ParserStaticData> css3ParserStaticData = nullptr;

void css3ParserInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (css3ParserStaticData != nullptr) {
    return;
  }
#else
  assert(css3ParserStaticData == nullptr);
#endif
  auto staticData = std::make_unique<CSS3ParserStaticData>(
    std::vector<std::string>{
      "stylesheet", "ruleSet", "atRule", "selector", "combinator", "simpleSelector", 
      "elementName", "declaration", "property", "expr", "term", "function_", 
      "unaryOperator", "operator", "url"
    },
    std::vector<std::string>{
      "", "'!important'", "'url('", "", "", "", "", "", "", "", "", "", 
      "'{'", "'}'", "'('", "')'", "'['", "']'", "';'", "':'", "'.'", "','", 
      "'+'", "'-'", "'*'", "'/'", "'='", "'~'", "'>'", "'|'", "'@'"
    },
    std::vector<std::string>{
      "", "", "", "WHITESPACE", "COMMENT", "STRING", "HASH", "NUMBER", "PERCENTAGE", 
      "DIMENSION", "URL_CHARS", "IDENT", "LBRACE", "RBRACE", "LPAREN", "RPAREN", 
      "LBRACKET", "RBRACKET", "SEMICOLON", "COLON", "DOT", "COMMA", "PLUS", 
      "MINUS", "MULTIPLY", "DIVIDE", "EQUALS", "TILDE", "GREATER", "PIPE", 
      "AT"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,30,153,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,1,0,1,0,1,0,1,0,5,0,35,8,0,10,0,12,0,38,9,0,1,0,1,0,1,1,1,1,1,1,5,
  	1,45,8,1,10,1,12,1,48,9,1,1,1,1,1,1,2,1,2,1,2,1,2,1,2,1,2,3,2,58,8,2,
  	1,2,1,2,1,2,5,2,63,8,2,10,2,12,2,66,9,2,1,2,1,2,3,2,70,8,2,1,3,1,3,1,
  	3,1,3,5,3,76,8,3,10,3,12,3,79,9,3,1,4,1,4,1,5,3,5,84,8,5,1,5,1,5,1,5,
  	1,5,1,5,1,5,1,5,3,5,93,8,5,1,5,1,5,1,5,5,5,98,8,5,10,5,12,5,101,9,5,1,
  	6,1,6,1,7,1,7,1,7,1,7,3,7,109,8,7,1,7,3,7,112,8,7,1,8,1,8,1,9,1,9,1,9,
  	1,9,5,9,120,8,9,10,9,12,9,123,9,9,1,10,3,10,126,8,10,1,10,1,10,1,10,1,
  	10,1,10,1,10,1,10,1,10,3,10,136,8,10,1,11,1,11,1,11,3,11,141,8,11,1,11,
  	1,11,1,12,1,12,1,13,1,13,1,14,1,14,1,14,1,14,1,14,0,0,15,0,2,4,6,8,10,
  	12,14,16,18,20,22,24,26,28,0,6,3,0,3,3,22,22,27,28,2,0,5,5,11,11,2,0,
  	11,11,24,24,1,0,22,23,2,0,21,21,25,25,2,0,5,5,10,10,165,0,36,1,0,0,0,
  	2,41,1,0,0,0,4,51,1,0,0,0,6,71,1,0,0,0,8,80,1,0,0,0,10,83,1,0,0,0,12,
  	102,1,0,0,0,14,104,1,0,0,0,16,113,1,0,0,0,18,115,1,0,0,0,20,125,1,0,0,
  	0,22,137,1,0,0,0,24,144,1,0,0,0,26,146,1,0,0,0,28,148,1,0,0,0,30,35,3,
  	2,1,0,31,35,3,4,2,0,32,35,5,4,0,0,33,35,5,3,0,0,34,30,1,0,0,0,34,31,1,
  	0,0,0,34,32,1,0,0,0,34,33,1,0,0,0,35,38,1,0,0,0,36,34,1,0,0,0,36,37,1,
  	0,0,0,37,39,1,0,0,0,38,36,1,0,0,0,39,40,5,0,0,1,40,1,1,0,0,0,41,42,3,
  	6,3,0,42,46,5,12,0,0,43,45,3,14,7,0,44,43,1,0,0,0,45,48,1,0,0,0,46,44,
  	1,0,0,0,46,47,1,0,0,0,47,49,1,0,0,0,48,46,1,0,0,0,49,50,5,13,0,0,50,3,
  	1,0,0,0,51,52,5,30,0,0,52,57,5,11,0,0,53,54,5,14,0,0,54,55,3,18,9,0,55,
  	56,5,15,0,0,56,58,1,0,0,0,57,53,1,0,0,0,57,58,1,0,0,0,58,69,1,0,0,0,59,
  	64,5,12,0,0,60,63,3,2,1,0,61,63,3,14,7,0,62,60,1,0,0,0,62,61,1,0,0,0,
  	63,66,1,0,0,0,64,62,1,0,0,0,64,65,1,0,0,0,65,67,1,0,0,0,66,64,1,0,0,0,
  	67,70,5,13,0,0,68,70,5,18,0,0,69,59,1,0,0,0,69,68,1,0,0,0,70,5,1,0,0,
  	0,71,77,3,10,5,0,72,73,3,8,4,0,73,74,3,10,5,0,74,76,1,0,0,0,75,72,1,0,
  	0,0,76,79,1,0,0,0,77,75,1,0,0,0,77,78,1,0,0,0,78,7,1,0,0,0,79,77,1,0,
  	0,0,80,81,7,0,0,0,81,9,1,0,0,0,82,84,3,12,6,0,83,82,1,0,0,0,83,84,1,0,
  	0,0,84,99,1,0,0,0,85,98,5,6,0,0,86,87,5,20,0,0,87,98,5,11,0,0,88,89,5,
  	16,0,0,89,92,5,11,0,0,90,91,5,26,0,0,91,93,7,1,0,0,92,90,1,0,0,0,92,93,
  	1,0,0,0,93,94,1,0,0,0,94,98,5,17,0,0,95,96,5,19,0,0,96,98,5,11,0,0,97,
  	85,1,0,0,0,97,86,1,0,0,0,97,88,1,0,0,0,97,95,1,0,0,0,98,101,1,0,0,0,99,
  	97,1,0,0,0,99,100,1,0,0,0,100,11,1,0,0,0,101,99,1,0,0,0,102,103,7,2,0,
  	0,103,13,1,0,0,0,104,105,3,16,8,0,105,106,5,19,0,0,106,108,3,18,9,0,107,
  	109,5,1,0,0,108,107,1,0,0,0,108,109,1,0,0,0,109,111,1,0,0,0,110,112,5,
  	18,0,0,111,110,1,0,0,0,111,112,1,0,0,0,112,15,1,0,0,0,113,114,5,11,0,
  	0,114,17,1,0,0,0,115,121,3,20,10,0,116,117,3,26,13,0,117,118,3,20,10,
  	0,118,120,1,0,0,0,119,116,1,0,0,0,120,123,1,0,0,0,121,119,1,0,0,0,121,
  	122,1,0,0,0,122,19,1,0,0,0,123,121,1,0,0,0,124,126,3,24,12,0,125,124,
  	1,0,0,0,125,126,1,0,0,0,126,135,1,0,0,0,127,136,5,7,0,0,128,136,5,8,0,
  	0,129,136,5,9,0,0,130,136,5,5,0,0,131,136,5,11,0,0,132,136,5,6,0,0,133,
  	136,3,28,14,0,134,136,3,22,11,0,135,127,1,0,0,0,135,128,1,0,0,0,135,129,
  	1,0,0,0,135,130,1,0,0,0,135,131,1,0,0,0,135,132,1,0,0,0,135,133,1,0,0,
  	0,135,134,1,0,0,0,136,21,1,0,0,0,137,138,5,11,0,0,138,140,5,14,0,0,139,
  	141,3,18,9,0,140,139,1,0,0,0,140,141,1,0,0,0,141,142,1,0,0,0,142,143,
  	5,15,0,0,143,23,1,0,0,0,144,145,7,3,0,0,145,25,1,0,0,0,146,147,7,4,0,
  	0,147,27,1,0,0,0,148,149,5,2,0,0,149,150,7,5,0,0,150,151,5,15,0,0,151,
  	29,1,0,0,0,18,34,36,46,57,62,64,69,77,83,92,97,99,108,111,121,125,135,
  	140
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  css3ParserStaticData = std::move(staticData);
}

}

CSS3Parser::CSS3Parser(TokenStream *input) : CSS3Parser(input, antlr4::atn::ParserATNSimulatorOptions()) {}

CSS3Parser::CSS3Parser(TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options) : Parser(input) {
  CSS3Parser::initialize();
  _interpreter = new atn::ParserATNSimulator(this, *css3ParserStaticData->atn, css3ParserStaticData->decisionToDFA, css3ParserStaticData->sharedContextCache, options);
}

CSS3Parser::~CSS3Parser() {
  delete _interpreter;
}

const atn::ATN& CSS3Parser::getATN() const {
  return *css3ParserStaticData->atn;
}

std::string CSS3Parser::getGrammarFileName() const {
  return "CSS3.g4";
}

const std::vector<std::string>& CSS3Parser::getRuleNames() const {
  return css3ParserStaticData->ruleNames;
}

const dfa::Vocabulary& CSS3Parser::getVocabulary() const {
  return css3ParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView CSS3Parser::getSerializedATN() const {
  return css3ParserStaticData->serializedATN;
}


//----------------- StylesheetContext ------------------------------------------------------------------

CSS3Parser::StylesheetContext::StylesheetContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSS3Parser::StylesheetContext::EOF() {
  return getToken(CSS3Parser::EOF, 0);
}

std::vector<CSS3Parser::RuleSetContext *> CSS3Parser::StylesheetContext::ruleSet() {
  return getRuleContexts<CSS3Parser::RuleSetContext>();
}

CSS3Parser::RuleSetContext* CSS3Parser::StylesheetContext::ruleSet(size_t i) {
  return getRuleContext<CSS3Parser::RuleSetContext>(i);
}

std::vector<CSS3Parser::AtRuleContext *> CSS3Parser::StylesheetContext::atRule() {
  return getRuleContexts<CSS3Parser::AtRuleContext>();
}

CSS3Parser::AtRuleContext* CSS3Parser::StylesheetContext::atRule(size_t i) {
  return getRuleContext<CSS3Parser::AtRuleContext>(i);
}

std::vector<tree::TerminalNode *> CSS3Parser::StylesheetContext::COMMENT() {
  return getTokens(CSS3Parser::COMMENT);
}

tree::TerminalNode* CSS3Parser::StylesheetContext::COMMENT(size_t i) {
  return getToken(CSS3Parser::COMMENT, i);
}

std::vector<tree::TerminalNode *> CSS3Parser::StylesheetContext::WHITESPACE() {
  return getTokens(CSS3Parser::WHITESPACE);
}

tree::TerminalNode* CSS3Parser::StylesheetContext::WHITESPACE(size_t i) {
  return getToken(CSS3Parser::WHITESPACE, i);
}


size_t CSS3Parser::StylesheetContext::getRuleIndex() const {
  return CSS3Parser::RuleStylesheet;
}

void CSS3Parser::StylesheetContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStylesheet(this);
}

void CSS3Parser::StylesheetContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStylesheet(this);
}

CSS3Parser::StylesheetContext* CSS3Parser::stylesheet() {
  StylesheetContext *_localctx = _tracker.createInstance<StylesheetContext>(_ctx, getState());
  enterRule(_localctx, 0, CSS3Parser::RuleStylesheet);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(36);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 1499011160) != 0)) {
      setState(34);
      _errHandler->sync(this);
      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 0, _ctx)) {
      case 1: {
        setState(30);
        ruleSet();
        break;
      }

      case 2: {
        setState(31);
        atRule();
        break;
      }

      case 3: {
        setState(32);
        match(CSS3Parser::COMMENT);
        break;
      }

      case 4: {
        setState(33);
        match(CSS3Parser::WHITESPACE);
        break;
      }

      default:
        break;
      }
      setState(38);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(39);
    match(CSS3Parser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- RuleSetContext ------------------------------------------------------------------

CSS3Parser::RuleSetContext::RuleSetContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

CSS3Parser::SelectorContext* CSS3Parser::RuleSetContext::selector() {
  return getRuleContext<CSS3Parser::SelectorContext>(0);
}

tree::TerminalNode* CSS3Parser::RuleSetContext::LBRACE() {
  return getToken(CSS3Parser::LBRACE, 0);
}

tree::TerminalNode* CSS3Parser::RuleSetContext::RBRACE() {
  return getToken(CSS3Parser::RBRACE, 0);
}

std::vector<CSS3Parser::DeclarationContext *> CSS3Parser::RuleSetContext::declaration() {
  return getRuleContexts<CSS3Parser::DeclarationContext>();
}

CSS3Parser::DeclarationContext* CSS3Parser::RuleSetContext::declaration(size_t i) {
  return getRuleContext<CSS3Parser::DeclarationContext>(i);
}


size_t CSS3Parser::RuleSetContext::getRuleIndex() const {
  return CSS3Parser::RuleRuleSet;
}

void CSS3Parser::RuleSetContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterRuleSet(this);
}

void CSS3Parser::RuleSetContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitRuleSet(this);
}

CSS3Parser::RuleSetContext* CSS3Parser::ruleSet() {
  RuleSetContext *_localctx = _tracker.createInstance<RuleSetContext>(_ctx, getState());
  enterRule(_localctx, 2, CSS3Parser::RuleRuleSet);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(41);
    selector();
    setState(42);
    match(CSS3Parser::LBRACE);
    setState(46);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == CSS3Parser::IDENT) {
      setState(43);
      declaration();
      setState(48);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(49);
    match(CSS3Parser::RBRACE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AtRuleContext ------------------------------------------------------------------

CSS3Parser::AtRuleContext::AtRuleContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSS3Parser::AtRuleContext::AT() {
  return getToken(CSS3Parser::AT, 0);
}

tree::TerminalNode* CSS3Parser::AtRuleContext::IDENT() {
  return getToken(CSS3Parser::IDENT, 0);
}

tree::TerminalNode* CSS3Parser::AtRuleContext::LBRACE() {
  return getToken(CSS3Parser::LBRACE, 0);
}

tree::TerminalNode* CSS3Parser::AtRuleContext::RBRACE() {
  return getToken(CSS3Parser::RBRACE, 0);
}

tree::TerminalNode* CSS3Parser::AtRuleContext::SEMICOLON() {
  return getToken(CSS3Parser::SEMICOLON, 0);
}

tree::TerminalNode* CSS3Parser::AtRuleContext::LPAREN() {
  return getToken(CSS3Parser::LPAREN, 0);
}

CSS3Parser::ExprContext* CSS3Parser::AtRuleContext::expr() {
  return getRuleContext<CSS3Parser::ExprContext>(0);
}

tree::TerminalNode* CSS3Parser::AtRuleContext::RPAREN() {
  return getToken(CSS3Parser::RPAREN, 0);
}

std::vector<CSS3Parser::RuleSetContext *> CSS3Parser::AtRuleContext::ruleSet() {
  return getRuleContexts<CSS3Parser::RuleSetContext>();
}

CSS3Parser::RuleSetContext* CSS3Parser::AtRuleContext::ruleSet(size_t i) {
  return getRuleContext<CSS3Parser::RuleSetContext>(i);
}

std::vector<CSS3Parser::DeclarationContext *> CSS3Parser::AtRuleContext::declaration() {
  return getRuleContexts<CSS3Parser::DeclarationContext>();
}

CSS3Parser::DeclarationContext* CSS3Parser::AtRuleContext::declaration(size_t i) {
  return getRuleContext<CSS3Parser::DeclarationContext>(i);
}


size_t CSS3Parser::AtRuleContext::getRuleIndex() const {
  return CSS3Parser::RuleAtRule;
}

void CSS3Parser::AtRuleContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAtRule(this);
}

void CSS3Parser::AtRuleContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAtRule(this);
}

CSS3Parser::AtRuleContext* CSS3Parser::atRule() {
  AtRuleContext *_localctx = _tracker.createInstance<AtRuleContext>(_ctx, getState());
  enterRule(_localctx, 4, CSS3Parser::RuleAtRule);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(51);
    match(CSS3Parser::AT);
    setState(52);
    match(CSS3Parser::IDENT);
    setState(57);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == CSS3Parser::LPAREN) {
      setState(53);
      match(CSS3Parser::LPAREN);
      setState(54);
      expr();
      setState(55);
      match(CSS3Parser::RPAREN);
    }
    setState(69);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CSS3Parser::LBRACE: {
        setState(59);
        match(CSS3Parser::LBRACE);
        setState(64);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & 425269320) != 0)) {
          setState(62);
          _errHandler->sync(this);
          switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 4, _ctx)) {
          case 1: {
            setState(60);
            ruleSet();
            break;
          }

          case 2: {
            setState(61);
            declaration();
            break;
          }

          default:
            break;
          }
          setState(66);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        setState(67);
        match(CSS3Parser::RBRACE);
        break;
      }

      case CSS3Parser::SEMICOLON: {
        setState(68);
        match(CSS3Parser::SEMICOLON);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- SelectorContext ------------------------------------------------------------------

CSS3Parser::SelectorContext::SelectorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<CSS3Parser::SimpleSelectorContext *> CSS3Parser::SelectorContext::simpleSelector() {
  return getRuleContexts<CSS3Parser::SimpleSelectorContext>();
}

CSS3Parser::SimpleSelectorContext* CSS3Parser::SelectorContext::simpleSelector(size_t i) {
  return getRuleContext<CSS3Parser::SimpleSelectorContext>(i);
}

std::vector<CSS3Parser::CombinatorContext *> CSS3Parser::SelectorContext::combinator() {
  return getRuleContexts<CSS3Parser::CombinatorContext>();
}

CSS3Parser::CombinatorContext* CSS3Parser::SelectorContext::combinator(size_t i) {
  return getRuleContext<CSS3Parser::CombinatorContext>(i);
}


size_t CSS3Parser::SelectorContext::getRuleIndex() const {
  return CSS3Parser::RuleSelector;
}

void CSS3Parser::SelectorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSelector(this);
}

void CSS3Parser::SelectorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSelector(this);
}

CSS3Parser::SelectorContext* CSS3Parser::selector() {
  SelectorContext *_localctx = _tracker.createInstance<SelectorContext>(_ctx, getState());
  enterRule(_localctx, 6, CSS3Parser::RuleSelector);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(71);
    simpleSelector();
    setState(77);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 406847496) != 0)) {
      setState(72);
      combinator();
      setState(73);
      simpleSelector();
      setState(79);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CombinatorContext ------------------------------------------------------------------

CSS3Parser::CombinatorContext::CombinatorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSS3Parser::CombinatorContext::PLUS() {
  return getToken(CSS3Parser::PLUS, 0);
}

tree::TerminalNode* CSS3Parser::CombinatorContext::GREATER() {
  return getToken(CSS3Parser::GREATER, 0);
}

tree::TerminalNode* CSS3Parser::CombinatorContext::TILDE() {
  return getToken(CSS3Parser::TILDE, 0);
}

tree::TerminalNode* CSS3Parser::CombinatorContext::WHITESPACE() {
  return getToken(CSS3Parser::WHITESPACE, 0);
}


size_t CSS3Parser::CombinatorContext::getRuleIndex() const {
  return CSS3Parser::RuleCombinator;
}

void CSS3Parser::CombinatorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCombinator(this);
}

void CSS3Parser::CombinatorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCombinator(this);
}

CSS3Parser::CombinatorContext* CSS3Parser::combinator() {
  CombinatorContext *_localctx = _tracker.createInstance<CombinatorContext>(_ctx, getState());
  enterRule(_localctx, 8, CSS3Parser::RuleCombinator);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(80);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 406847496) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- SimpleSelectorContext ------------------------------------------------------------------

CSS3Parser::SimpleSelectorContext::SimpleSelectorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

CSS3Parser::ElementNameContext* CSS3Parser::SimpleSelectorContext::elementName() {
  return getRuleContext<CSS3Parser::ElementNameContext>(0);
}

std::vector<tree::TerminalNode *> CSS3Parser::SimpleSelectorContext::HASH() {
  return getTokens(CSS3Parser::HASH);
}

tree::TerminalNode* CSS3Parser::SimpleSelectorContext::HASH(size_t i) {
  return getToken(CSS3Parser::HASH, i);
}

std::vector<tree::TerminalNode *> CSS3Parser::SimpleSelectorContext::DOT() {
  return getTokens(CSS3Parser::DOT);
}

tree::TerminalNode* CSS3Parser::SimpleSelectorContext::DOT(size_t i) {
  return getToken(CSS3Parser::DOT, i);
}

std::vector<tree::TerminalNode *> CSS3Parser::SimpleSelectorContext::IDENT() {
  return getTokens(CSS3Parser::IDENT);
}

tree::TerminalNode* CSS3Parser::SimpleSelectorContext::IDENT(size_t i) {
  return getToken(CSS3Parser::IDENT, i);
}

std::vector<tree::TerminalNode *> CSS3Parser::SimpleSelectorContext::LBRACKET() {
  return getTokens(CSS3Parser::LBRACKET);
}

tree::TerminalNode* CSS3Parser::SimpleSelectorContext::LBRACKET(size_t i) {
  return getToken(CSS3Parser::LBRACKET, i);
}

std::vector<tree::TerminalNode *> CSS3Parser::SimpleSelectorContext::RBRACKET() {
  return getTokens(CSS3Parser::RBRACKET);
}

tree::TerminalNode* CSS3Parser::SimpleSelectorContext::RBRACKET(size_t i) {
  return getToken(CSS3Parser::RBRACKET, i);
}

std::vector<tree::TerminalNode *> CSS3Parser::SimpleSelectorContext::COLON() {
  return getTokens(CSS3Parser::COLON);
}

tree::TerminalNode* CSS3Parser::SimpleSelectorContext::COLON(size_t i) {
  return getToken(CSS3Parser::COLON, i);
}

std::vector<tree::TerminalNode *> CSS3Parser::SimpleSelectorContext::EQUALS() {
  return getTokens(CSS3Parser::EQUALS);
}

tree::TerminalNode* CSS3Parser::SimpleSelectorContext::EQUALS(size_t i) {
  return getToken(CSS3Parser::EQUALS, i);
}

std::vector<tree::TerminalNode *> CSS3Parser::SimpleSelectorContext::STRING() {
  return getTokens(CSS3Parser::STRING);
}

tree::TerminalNode* CSS3Parser::SimpleSelectorContext::STRING(size_t i) {
  return getToken(CSS3Parser::STRING, i);
}


size_t CSS3Parser::SimpleSelectorContext::getRuleIndex() const {
  return CSS3Parser::RuleSimpleSelector;
}

void CSS3Parser::SimpleSelectorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSimpleSelector(this);
}

void CSS3Parser::SimpleSelectorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSimpleSelector(this);
}

CSS3Parser::SimpleSelectorContext* CSS3Parser::simpleSelector() {
  SimpleSelectorContext *_localctx = _tracker.createInstance<SimpleSelectorContext>(_ctx, getState());
  enterRule(_localctx, 10, CSS3Parser::RuleSimpleSelector);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(83);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == CSS3Parser::IDENT

    || _la == CSS3Parser::MULTIPLY) {
      setState(82);
      elementName();
    }
    setState(99);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 1638464) != 0)) {
      setState(97);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case CSS3Parser::HASH: {
          setState(85);
          match(CSS3Parser::HASH);
          break;
        }

        case CSS3Parser::DOT: {
          setState(86);
          match(CSS3Parser::DOT);
          setState(87);
          match(CSS3Parser::IDENT);
          break;
        }

        case CSS3Parser::LBRACKET: {
          setState(88);
          match(CSS3Parser::LBRACKET);
          setState(89);
          match(CSS3Parser::IDENT);
          setState(92);
          _errHandler->sync(this);

          _la = _input->LA(1);
          if (_la == CSS3Parser::EQUALS) {
            setState(90);
            match(CSS3Parser::EQUALS);
            setState(91);
            _la = _input->LA(1);
            if (!(_la == CSS3Parser::STRING

            || _la == CSS3Parser::IDENT)) {
            _errHandler->recoverInline(this);
            }
            else {
              _errHandler->reportMatch(this);
              consume();
            }
          }
          setState(94);
          match(CSS3Parser::RBRACKET);
          break;
        }

        case CSS3Parser::COLON: {
          setState(95);
          match(CSS3Parser::COLON);
          setState(96);
          match(CSS3Parser::IDENT);
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(101);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ElementNameContext ------------------------------------------------------------------

CSS3Parser::ElementNameContext::ElementNameContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSS3Parser::ElementNameContext::IDENT() {
  return getToken(CSS3Parser::IDENT, 0);
}

tree::TerminalNode* CSS3Parser::ElementNameContext::MULTIPLY() {
  return getToken(CSS3Parser::MULTIPLY, 0);
}


size_t CSS3Parser::ElementNameContext::getRuleIndex() const {
  return CSS3Parser::RuleElementName;
}

void CSS3Parser::ElementNameContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterElementName(this);
}

void CSS3Parser::ElementNameContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitElementName(this);
}

CSS3Parser::ElementNameContext* CSS3Parser::elementName() {
  ElementNameContext *_localctx = _tracker.createInstance<ElementNameContext>(_ctx, getState());
  enterRule(_localctx, 12, CSS3Parser::RuleElementName);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(102);
    _la = _input->LA(1);
    if (!(_la == CSS3Parser::IDENT

    || _la == CSS3Parser::MULTIPLY)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DeclarationContext ------------------------------------------------------------------

CSS3Parser::DeclarationContext::DeclarationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

CSS3Parser::PropertyContext* CSS3Parser::DeclarationContext::property() {
  return getRuleContext<CSS3Parser::PropertyContext>(0);
}

tree::TerminalNode* CSS3Parser::DeclarationContext::COLON() {
  return getToken(CSS3Parser::COLON, 0);
}

CSS3Parser::ExprContext* CSS3Parser::DeclarationContext::expr() {
  return getRuleContext<CSS3Parser::ExprContext>(0);
}

tree::TerminalNode* CSS3Parser::DeclarationContext::SEMICOLON() {
  return getToken(CSS3Parser::SEMICOLON, 0);
}


size_t CSS3Parser::DeclarationContext::getRuleIndex() const {
  return CSS3Parser::RuleDeclaration;
}

void CSS3Parser::DeclarationContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterDeclaration(this);
}

void CSS3Parser::DeclarationContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitDeclaration(this);
}

CSS3Parser::DeclarationContext* CSS3Parser::declaration() {
  DeclarationContext *_localctx = _tracker.createInstance<DeclarationContext>(_ctx, getState());
  enterRule(_localctx, 14, CSS3Parser::RuleDeclaration);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(104);
    property();
    setState(105);
    match(CSS3Parser::COLON);
    setState(106);
    expr();
    setState(108);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == CSS3Parser::T__0) {
      setState(107);
      match(CSS3Parser::T__0);
    }
    setState(111);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == CSS3Parser::SEMICOLON) {
      setState(110);
      match(CSS3Parser::SEMICOLON);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PropertyContext ------------------------------------------------------------------

CSS3Parser::PropertyContext::PropertyContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSS3Parser::PropertyContext::IDENT() {
  return getToken(CSS3Parser::IDENT, 0);
}


size_t CSS3Parser::PropertyContext::getRuleIndex() const {
  return CSS3Parser::RuleProperty;
}

void CSS3Parser::PropertyContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterProperty(this);
}

void CSS3Parser::PropertyContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitProperty(this);
}

CSS3Parser::PropertyContext* CSS3Parser::property() {
  PropertyContext *_localctx = _tracker.createInstance<PropertyContext>(_ctx, getState());
  enterRule(_localctx, 16, CSS3Parser::RuleProperty);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(113);
    match(CSS3Parser::IDENT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExprContext ------------------------------------------------------------------

CSS3Parser::ExprContext::ExprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<CSS3Parser::TermContext *> CSS3Parser::ExprContext::term() {
  return getRuleContexts<CSS3Parser::TermContext>();
}

CSS3Parser::TermContext* CSS3Parser::ExprContext::term(size_t i) {
  return getRuleContext<CSS3Parser::TermContext>(i);
}

std::vector<CSS3Parser::OperatorContext *> CSS3Parser::ExprContext::operator_() {
  return getRuleContexts<CSS3Parser::OperatorContext>();
}

CSS3Parser::OperatorContext* CSS3Parser::ExprContext::operator_(size_t i) {
  return getRuleContext<CSS3Parser::OperatorContext>(i);
}


size_t CSS3Parser::ExprContext::getRuleIndex() const {
  return CSS3Parser::RuleExpr;
}

void CSS3Parser::ExprContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterExpr(this);
}

void CSS3Parser::ExprContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitExpr(this);
}

CSS3Parser::ExprContext* CSS3Parser::expr() {
  ExprContext *_localctx = _tracker.createInstance<ExprContext>(_ctx, getState());
  enterRule(_localctx, 18, CSS3Parser::RuleExpr);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(115);
    term();
    setState(121);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == CSS3Parser::COMMA

    || _la == CSS3Parser::DIVIDE) {
      setState(116);
      operator_();
      setState(117);
      term();
      setState(123);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TermContext ------------------------------------------------------------------

CSS3Parser::TermContext::TermContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSS3Parser::TermContext::NUMBER() {
  return getToken(CSS3Parser::NUMBER, 0);
}

tree::TerminalNode* CSS3Parser::TermContext::PERCENTAGE() {
  return getToken(CSS3Parser::PERCENTAGE, 0);
}

tree::TerminalNode* CSS3Parser::TermContext::DIMENSION() {
  return getToken(CSS3Parser::DIMENSION, 0);
}

tree::TerminalNode* CSS3Parser::TermContext::STRING() {
  return getToken(CSS3Parser::STRING, 0);
}

tree::TerminalNode* CSS3Parser::TermContext::IDENT() {
  return getToken(CSS3Parser::IDENT, 0);
}

tree::TerminalNode* CSS3Parser::TermContext::HASH() {
  return getToken(CSS3Parser::HASH, 0);
}

CSS3Parser::UrlContext* CSS3Parser::TermContext::url() {
  return getRuleContext<CSS3Parser::UrlContext>(0);
}

CSS3Parser::Function_Context* CSS3Parser::TermContext::function_() {
  return getRuleContext<CSS3Parser::Function_Context>(0);
}

CSS3Parser::UnaryOperatorContext* CSS3Parser::TermContext::unaryOperator() {
  return getRuleContext<CSS3Parser::UnaryOperatorContext>(0);
}


size_t CSS3Parser::TermContext::getRuleIndex() const {
  return CSS3Parser::RuleTerm;
}

void CSS3Parser::TermContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTerm(this);
}

void CSS3Parser::TermContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTerm(this);
}

CSS3Parser::TermContext* CSS3Parser::term() {
  TermContext *_localctx = _tracker.createInstance<TermContext>(_ctx, getState());
  enterRule(_localctx, 20, CSS3Parser::RuleTerm);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(125);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == CSS3Parser::PLUS

    || _la == CSS3Parser::MINUS) {
      setState(124);
      unaryOperator();
    }
    setState(135);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 16, _ctx)) {
    case 1: {
      setState(127);
      match(CSS3Parser::NUMBER);
      break;
    }

    case 2: {
      setState(128);
      match(CSS3Parser::PERCENTAGE);
      break;
    }

    case 3: {
      setState(129);
      match(CSS3Parser::DIMENSION);
      break;
    }

    case 4: {
      setState(130);
      match(CSS3Parser::STRING);
      break;
    }

    case 5: {
      setState(131);
      match(CSS3Parser::IDENT);
      break;
    }

    case 6: {
      setState(132);
      match(CSS3Parser::HASH);
      break;
    }

    case 7: {
      setState(133);
      url();
      break;
    }

    case 8: {
      setState(134);
      function_();
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Function_Context ------------------------------------------------------------------

CSS3Parser::Function_Context::Function_Context(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSS3Parser::Function_Context::IDENT() {
  return getToken(CSS3Parser::IDENT, 0);
}

tree::TerminalNode* CSS3Parser::Function_Context::LPAREN() {
  return getToken(CSS3Parser::LPAREN, 0);
}

tree::TerminalNode* CSS3Parser::Function_Context::RPAREN() {
  return getToken(CSS3Parser::RPAREN, 0);
}

CSS3Parser::ExprContext* CSS3Parser::Function_Context::expr() {
  return getRuleContext<CSS3Parser::ExprContext>(0);
}


size_t CSS3Parser::Function_Context::getRuleIndex() const {
  return CSS3Parser::RuleFunction_;
}

void CSS3Parser::Function_Context::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFunction_(this);
}

void CSS3Parser::Function_Context::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFunction_(this);
}

CSS3Parser::Function_Context* CSS3Parser::function_() {
  Function_Context *_localctx = _tracker.createInstance<Function_Context>(_ctx, getState());
  enterRule(_localctx, 22, CSS3Parser::RuleFunction_);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(137);
    match(CSS3Parser::IDENT);
    setState(138);
    match(CSS3Parser::LPAREN);
    setState(140);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 12585956) != 0)) {
      setState(139);
      expr();
    }
    setState(142);
    match(CSS3Parser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- UnaryOperatorContext ------------------------------------------------------------------

CSS3Parser::UnaryOperatorContext::UnaryOperatorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSS3Parser::UnaryOperatorContext::PLUS() {
  return getToken(CSS3Parser::PLUS, 0);
}

tree::TerminalNode* CSS3Parser::UnaryOperatorContext::MINUS() {
  return getToken(CSS3Parser::MINUS, 0);
}


size_t CSS3Parser::UnaryOperatorContext::getRuleIndex() const {
  return CSS3Parser::RuleUnaryOperator;
}

void CSS3Parser::UnaryOperatorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterUnaryOperator(this);
}

void CSS3Parser::UnaryOperatorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitUnaryOperator(this);
}

CSS3Parser::UnaryOperatorContext* CSS3Parser::unaryOperator() {
  UnaryOperatorContext *_localctx = _tracker.createInstance<UnaryOperatorContext>(_ctx, getState());
  enterRule(_localctx, 24, CSS3Parser::RuleUnaryOperator);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(144);
    _la = _input->LA(1);
    if (!(_la == CSS3Parser::PLUS

    || _la == CSS3Parser::MINUS)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- OperatorContext ------------------------------------------------------------------

CSS3Parser::OperatorContext::OperatorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSS3Parser::OperatorContext::DIVIDE() {
  return getToken(CSS3Parser::DIVIDE, 0);
}

tree::TerminalNode* CSS3Parser::OperatorContext::COMMA() {
  return getToken(CSS3Parser::COMMA, 0);
}


size_t CSS3Parser::OperatorContext::getRuleIndex() const {
  return CSS3Parser::RuleOperator;
}

void CSS3Parser::OperatorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterOperator(this);
}

void CSS3Parser::OperatorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitOperator(this);
}

CSS3Parser::OperatorContext* CSS3Parser::operator_() {
  OperatorContext *_localctx = _tracker.createInstance<OperatorContext>(_ctx, getState());
  enterRule(_localctx, 26, CSS3Parser::RuleOperator);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(146);
    _la = _input->LA(1);
    if (!(_la == CSS3Parser::COMMA

    || _la == CSS3Parser::DIVIDE)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- UrlContext ------------------------------------------------------------------

CSS3Parser::UrlContext::UrlContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSS3Parser::UrlContext::RPAREN() {
  return getToken(CSS3Parser::RPAREN, 0);
}

tree::TerminalNode* CSS3Parser::UrlContext::STRING() {
  return getToken(CSS3Parser::STRING, 0);
}

tree::TerminalNode* CSS3Parser::UrlContext::URL_CHARS() {
  return getToken(CSS3Parser::URL_CHARS, 0);
}


size_t CSS3Parser::UrlContext::getRuleIndex() const {
  return CSS3Parser::RuleUrl;
}

void CSS3Parser::UrlContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterUrl(this);
}

void CSS3Parser::UrlContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitUrl(this);
}

CSS3Parser::UrlContext* CSS3Parser::url() {
  UrlContext *_localctx = _tracker.createInstance<UrlContext>(_ctx, getState());
  enterRule(_localctx, 28, CSS3Parser::RuleUrl);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(148);
    match(CSS3Parser::T__1);
    setState(149);
    _la = _input->LA(1);
    if (!(_la == CSS3Parser::STRING

    || _la == CSS3Parser::URL_CHARS)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(150);
    match(CSS3Parser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

void CSS3Parser::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  css3ParserInitialize();
#else
  ::antlr4::internal::call_once(css3ParserOnceFlag, css3ParserInitialize);
#endif
}
