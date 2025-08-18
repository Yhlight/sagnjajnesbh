
// Generated from external/grammars/CSS3.g4 by ANTLR 4.13.2


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
      "stylesheet", "rule", "at_rule", "qualified_rule", "declaration_list", 
      "declaration", "component_value", "block", "function", "preserved_token"
    },
    std::vector<std::string>{
      "", "'{'", "'}'", "'['", "']'", "'('", "')'", "", "", "", "", "", 
      "", "", "", "", "", "'~='", "'|='", "'^='", "'$='", "'*='", "'||'", 
      "'<!--'", "'-->'", "", "';'", "':'", "','"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "", "AT_KEYWORD", "IDENT", "FUNCTION", "STRING", 
      "HASH", "NUMBER", "PERCENTAGE", "DIMENSION", "URI", "UNICODE_RANGE", 
      "INCLUDES", "DASHMATCH", "PREFIX_MATCH", "SUFFIX_MATCH", "SUBSTRING_MATCH", 
      "COLUMN", "CDO", "CDC", "IMPORTANT", "SEMICOLON", "COLON", "COMMA", 
      "DELIM", "WS", "COMMENT"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,31,117,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,1,0,5,0,22,8,0,10,0,12,0,25,9,0,1,0,1,0,1,1,1,1,
  	3,1,31,8,1,1,2,1,2,5,2,35,8,2,10,2,12,2,38,9,2,1,2,1,2,1,2,1,2,1,2,3,
  	2,45,8,2,1,3,5,3,48,8,3,10,3,12,3,51,9,3,1,3,1,3,1,3,1,3,1,4,1,4,1,4,
  	5,4,60,8,4,10,4,12,4,63,9,4,1,5,1,5,1,5,4,5,68,8,5,11,5,12,5,69,1,5,3,
  	5,73,8,5,1,6,1,6,1,6,3,6,78,8,6,1,7,1,7,5,7,82,8,7,10,7,12,7,85,9,7,1,
  	7,1,7,1,7,5,7,90,8,7,10,7,12,7,93,9,7,1,7,1,7,1,7,5,7,98,8,7,10,7,12,
  	7,101,9,7,1,7,3,7,104,8,7,1,8,1,8,5,8,108,8,8,10,8,12,8,111,9,8,1,8,1,
  	8,1,9,1,9,1,9,0,0,10,0,2,4,6,8,10,12,14,16,18,0,1,3,0,8,8,10,24,26,30,
  	122,0,23,1,0,0,0,2,30,1,0,0,0,4,32,1,0,0,0,6,49,1,0,0,0,8,56,1,0,0,0,
  	10,64,1,0,0,0,12,77,1,0,0,0,14,103,1,0,0,0,16,105,1,0,0,0,18,114,1,0,
  	0,0,20,22,3,2,1,0,21,20,1,0,0,0,22,25,1,0,0,0,23,21,1,0,0,0,23,24,1,0,
  	0,0,24,26,1,0,0,0,25,23,1,0,0,0,26,27,5,0,0,1,27,1,1,0,0,0,28,31,3,6,
  	3,0,29,31,3,4,2,0,30,28,1,0,0,0,30,29,1,0,0,0,31,3,1,0,0,0,32,36,5,7,
  	0,0,33,35,3,12,6,0,34,33,1,0,0,0,35,38,1,0,0,0,36,34,1,0,0,0,36,37,1,
  	0,0,0,37,44,1,0,0,0,38,36,1,0,0,0,39,40,5,1,0,0,40,41,3,8,4,0,41,42,5,
  	2,0,0,42,45,1,0,0,0,43,45,5,26,0,0,44,39,1,0,0,0,44,43,1,0,0,0,45,5,1,
  	0,0,0,46,48,3,12,6,0,47,46,1,0,0,0,48,51,1,0,0,0,49,47,1,0,0,0,49,50,
  	1,0,0,0,50,52,1,0,0,0,51,49,1,0,0,0,52,53,5,1,0,0,53,54,3,8,4,0,54,55,
  	5,2,0,0,55,7,1,0,0,0,56,61,3,10,5,0,57,58,5,26,0,0,58,60,3,10,5,0,59,
  	57,1,0,0,0,60,63,1,0,0,0,61,59,1,0,0,0,61,62,1,0,0,0,62,9,1,0,0,0,63,
  	61,1,0,0,0,64,65,5,8,0,0,65,67,5,27,0,0,66,68,3,12,6,0,67,66,1,0,0,0,
  	68,69,1,0,0,0,69,67,1,0,0,0,69,70,1,0,0,0,70,72,1,0,0,0,71,73,5,25,0,
  	0,72,71,1,0,0,0,72,73,1,0,0,0,73,11,1,0,0,0,74,78,3,18,9,0,75,78,3,14,
  	7,0,76,78,3,16,8,0,77,74,1,0,0,0,77,75,1,0,0,0,77,76,1,0,0,0,78,13,1,
  	0,0,0,79,83,5,1,0,0,80,82,3,12,6,0,81,80,1,0,0,0,82,85,1,0,0,0,83,81,
  	1,0,0,0,83,84,1,0,0,0,84,86,1,0,0,0,85,83,1,0,0,0,86,104,5,2,0,0,87,91,
  	5,3,0,0,88,90,3,12,6,0,89,88,1,0,0,0,90,93,1,0,0,0,91,89,1,0,0,0,91,92,
  	1,0,0,0,92,94,1,0,0,0,93,91,1,0,0,0,94,104,5,4,0,0,95,99,5,5,0,0,96,98,
  	3,12,6,0,97,96,1,0,0,0,98,101,1,0,0,0,99,97,1,0,0,0,99,100,1,0,0,0,100,
  	102,1,0,0,0,101,99,1,0,0,0,102,104,5,6,0,0,103,79,1,0,0,0,103,87,1,0,
  	0,0,103,95,1,0,0,0,104,15,1,0,0,0,105,109,5,9,0,0,106,108,3,12,6,0,107,
  	106,1,0,0,0,108,111,1,0,0,0,109,107,1,0,0,0,109,110,1,0,0,0,110,112,1,
  	0,0,0,111,109,1,0,0,0,112,113,5,6,0,0,113,17,1,0,0,0,114,115,7,0,0,0,
  	115,19,1,0,0,0,14,23,30,36,44,49,61,69,72,77,83,91,99,103,109
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

std::vector<CSS3Parser::RuleContext *> CSS3Parser::StylesheetContext::rule_() {
  return getRuleContexts<CSS3Parser::RuleContext>();
}

CSS3Parser::RuleContext* CSS3Parser::StylesheetContext::rule_(size_t i) {
  return getRuleContext<CSS3Parser::RuleContext>(i);
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
    setState(23);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 2113929130) != 0)) {
      setState(20);
      rule_();
      setState(25);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(26);
    match(CSS3Parser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- RuleContext ------------------------------------------------------------------

CSS3Parser::RuleContext::RuleContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

CSS3Parser::Qualified_ruleContext* CSS3Parser::RuleContext::qualified_rule() {
  return getRuleContext<CSS3Parser::Qualified_ruleContext>(0);
}

CSS3Parser::At_ruleContext* CSS3Parser::RuleContext::at_rule() {
  return getRuleContext<CSS3Parser::At_ruleContext>(0);
}


size_t CSS3Parser::RuleContext::getRuleIndex() const {
  return CSS3Parser::RuleRule;
}

void CSS3Parser::RuleContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterRule(this);
}

void CSS3Parser::RuleContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitRule(this);
}

CSS3Parser::RuleContext* CSS3Parser::rule_() {
  RuleContext *_localctx = _tracker.createInstance<RuleContext>(_ctx, getState());
  enterRule(_localctx, 2, CSS3Parser::RuleRule);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(30);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CSS3Parser::T__0:
      case CSS3Parser::T__2:
      case CSS3Parser::T__4:
      case CSS3Parser::IDENT:
      case CSS3Parser::FUNCTION:
      case CSS3Parser::STRING:
      case CSS3Parser::HASH:
      case CSS3Parser::NUMBER:
      case CSS3Parser::PERCENTAGE:
      case CSS3Parser::DIMENSION:
      case CSS3Parser::URI:
      case CSS3Parser::UNICODE_RANGE:
      case CSS3Parser::INCLUDES:
      case CSS3Parser::DASHMATCH:
      case CSS3Parser::PREFIX_MATCH:
      case CSS3Parser::SUFFIX_MATCH:
      case CSS3Parser::SUBSTRING_MATCH:
      case CSS3Parser::COLUMN:
      case CSS3Parser::CDO:
      case CSS3Parser::CDC:
      case CSS3Parser::SEMICOLON:
      case CSS3Parser::COLON:
      case CSS3Parser::COMMA:
      case CSS3Parser::DELIM:
      case CSS3Parser::WS: {
        enterOuterAlt(_localctx, 1);
        setState(28);
        qualified_rule();
        break;
      }

      case CSS3Parser::AT_KEYWORD: {
        enterOuterAlt(_localctx, 2);
        setState(29);
        at_rule();
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

//----------------- At_ruleContext ------------------------------------------------------------------

CSS3Parser::At_ruleContext::At_ruleContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSS3Parser::At_ruleContext::AT_KEYWORD() {
  return getToken(CSS3Parser::AT_KEYWORD, 0);
}

CSS3Parser::Declaration_listContext* CSS3Parser::At_ruleContext::declaration_list() {
  return getRuleContext<CSS3Parser::Declaration_listContext>(0);
}

tree::TerminalNode* CSS3Parser::At_ruleContext::SEMICOLON() {
  return getToken(CSS3Parser::SEMICOLON, 0);
}

std::vector<CSS3Parser::Component_valueContext *> CSS3Parser::At_ruleContext::component_value() {
  return getRuleContexts<CSS3Parser::Component_valueContext>();
}

CSS3Parser::Component_valueContext* CSS3Parser::At_ruleContext::component_value(size_t i) {
  return getRuleContext<CSS3Parser::Component_valueContext>(i);
}


size_t CSS3Parser::At_ruleContext::getRuleIndex() const {
  return CSS3Parser::RuleAt_rule;
}

void CSS3Parser::At_ruleContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAt_rule(this);
}

void CSS3Parser::At_ruleContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAt_rule(this);
}

CSS3Parser::At_ruleContext* CSS3Parser::at_rule() {
  At_ruleContext *_localctx = _tracker.createInstance<At_ruleContext>(_ctx, getState());
  enterRule(_localctx, 4, CSS3Parser::RuleAt_rule);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(32);
    match(CSS3Parser::AT_KEYWORD);
    setState(36);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 2, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(33);
        component_value(); 
      }
      setState(38);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 2, _ctx);
    }
    setState(44);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CSS3Parser::T__0: {
        setState(39);
        match(CSS3Parser::T__0);
        setState(40);
        declaration_list();
        setState(41);
        match(CSS3Parser::T__1);
        break;
      }

      case CSS3Parser::SEMICOLON: {
        setState(43);
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

//----------------- Qualified_ruleContext ------------------------------------------------------------------

CSS3Parser::Qualified_ruleContext::Qualified_ruleContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

CSS3Parser::Declaration_listContext* CSS3Parser::Qualified_ruleContext::declaration_list() {
  return getRuleContext<CSS3Parser::Declaration_listContext>(0);
}

std::vector<CSS3Parser::Component_valueContext *> CSS3Parser::Qualified_ruleContext::component_value() {
  return getRuleContexts<CSS3Parser::Component_valueContext>();
}

CSS3Parser::Component_valueContext* CSS3Parser::Qualified_ruleContext::component_value(size_t i) {
  return getRuleContext<CSS3Parser::Component_valueContext>(i);
}


size_t CSS3Parser::Qualified_ruleContext::getRuleIndex() const {
  return CSS3Parser::RuleQualified_rule;
}

void CSS3Parser::Qualified_ruleContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterQualified_rule(this);
}

void CSS3Parser::Qualified_ruleContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitQualified_rule(this);
}

CSS3Parser::Qualified_ruleContext* CSS3Parser::qualified_rule() {
  Qualified_ruleContext *_localctx = _tracker.createInstance<Qualified_ruleContext>(_ctx, getState());
  enterRule(_localctx, 6, CSS3Parser::RuleQualified_rule);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(49);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 4, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(46);
        component_value(); 
      }
      setState(51);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 4, _ctx);
    }
    setState(52);
    match(CSS3Parser::T__0);
    setState(53);
    declaration_list();
    setState(54);
    match(CSS3Parser::T__1);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Declaration_listContext ------------------------------------------------------------------

CSS3Parser::Declaration_listContext::Declaration_listContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<CSS3Parser::DeclarationContext *> CSS3Parser::Declaration_listContext::declaration() {
  return getRuleContexts<CSS3Parser::DeclarationContext>();
}

CSS3Parser::DeclarationContext* CSS3Parser::Declaration_listContext::declaration(size_t i) {
  return getRuleContext<CSS3Parser::DeclarationContext>(i);
}

std::vector<tree::TerminalNode *> CSS3Parser::Declaration_listContext::SEMICOLON() {
  return getTokens(CSS3Parser::SEMICOLON);
}

tree::TerminalNode* CSS3Parser::Declaration_listContext::SEMICOLON(size_t i) {
  return getToken(CSS3Parser::SEMICOLON, i);
}


size_t CSS3Parser::Declaration_listContext::getRuleIndex() const {
  return CSS3Parser::RuleDeclaration_list;
}

void CSS3Parser::Declaration_listContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterDeclaration_list(this);
}

void CSS3Parser::Declaration_listContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitDeclaration_list(this);
}

CSS3Parser::Declaration_listContext* CSS3Parser::declaration_list() {
  Declaration_listContext *_localctx = _tracker.createInstance<Declaration_listContext>(_ctx, getState());
  enterRule(_localctx, 8, CSS3Parser::RuleDeclaration_list);
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
    setState(56);
    declaration();
    setState(61);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == CSS3Parser::SEMICOLON) {
      setState(57);
      match(CSS3Parser::SEMICOLON);
      setState(58);
      declaration();
      setState(63);
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

//----------------- DeclarationContext ------------------------------------------------------------------

CSS3Parser::DeclarationContext::DeclarationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSS3Parser::DeclarationContext::IDENT() {
  return getToken(CSS3Parser::IDENT, 0);
}

tree::TerminalNode* CSS3Parser::DeclarationContext::COLON() {
  return getToken(CSS3Parser::COLON, 0);
}

std::vector<CSS3Parser::Component_valueContext *> CSS3Parser::DeclarationContext::component_value() {
  return getRuleContexts<CSS3Parser::Component_valueContext>();
}

CSS3Parser::Component_valueContext* CSS3Parser::DeclarationContext::component_value(size_t i) {
  return getRuleContext<CSS3Parser::Component_valueContext>(i);
}

tree::TerminalNode* CSS3Parser::DeclarationContext::IMPORTANT() {
  return getToken(CSS3Parser::IMPORTANT, 0);
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
  enterRule(_localctx, 10, CSS3Parser::RuleDeclaration);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(64);
    match(CSS3Parser::IDENT);
    setState(65);
    match(CSS3Parser::COLON);
    setState(67); 
    _errHandler->sync(this);
    alt = 1;
    do {
      switch (alt) {
        case 1: {
              setState(66);
              component_value();
              break;
            }

      default:
        throw NoViableAltException(this);
      }
      setState(69); 
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 6, _ctx);
    } while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER);
    setState(72);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == CSS3Parser::IMPORTANT) {
      setState(71);
      match(CSS3Parser::IMPORTANT);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Component_valueContext ------------------------------------------------------------------

CSS3Parser::Component_valueContext::Component_valueContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

CSS3Parser::Preserved_tokenContext* CSS3Parser::Component_valueContext::preserved_token() {
  return getRuleContext<CSS3Parser::Preserved_tokenContext>(0);
}

CSS3Parser::BlockContext* CSS3Parser::Component_valueContext::block() {
  return getRuleContext<CSS3Parser::BlockContext>(0);
}

CSS3Parser::FunctionContext* CSS3Parser::Component_valueContext::function() {
  return getRuleContext<CSS3Parser::FunctionContext>(0);
}


size_t CSS3Parser::Component_valueContext::getRuleIndex() const {
  return CSS3Parser::RuleComponent_value;
}

void CSS3Parser::Component_valueContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterComponent_value(this);
}

void CSS3Parser::Component_valueContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitComponent_value(this);
}

CSS3Parser::Component_valueContext* CSS3Parser::component_value() {
  Component_valueContext *_localctx = _tracker.createInstance<Component_valueContext>(_ctx, getState());
  enterRule(_localctx, 12, CSS3Parser::RuleComponent_value);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(77);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CSS3Parser::IDENT:
      case CSS3Parser::STRING:
      case CSS3Parser::HASH:
      case CSS3Parser::NUMBER:
      case CSS3Parser::PERCENTAGE:
      case CSS3Parser::DIMENSION:
      case CSS3Parser::URI:
      case CSS3Parser::UNICODE_RANGE:
      case CSS3Parser::INCLUDES:
      case CSS3Parser::DASHMATCH:
      case CSS3Parser::PREFIX_MATCH:
      case CSS3Parser::SUFFIX_MATCH:
      case CSS3Parser::SUBSTRING_MATCH:
      case CSS3Parser::COLUMN:
      case CSS3Parser::CDO:
      case CSS3Parser::CDC:
      case CSS3Parser::SEMICOLON:
      case CSS3Parser::COLON:
      case CSS3Parser::COMMA:
      case CSS3Parser::DELIM:
      case CSS3Parser::WS: {
        enterOuterAlt(_localctx, 1);
        setState(74);
        preserved_token();
        break;
      }

      case CSS3Parser::T__0:
      case CSS3Parser::T__2:
      case CSS3Parser::T__4: {
        enterOuterAlt(_localctx, 2);
        setState(75);
        block();
        break;
      }

      case CSS3Parser::FUNCTION: {
        enterOuterAlt(_localctx, 3);
        setState(76);
        function();
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

//----------------- BlockContext ------------------------------------------------------------------

CSS3Parser::BlockContext::BlockContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<CSS3Parser::Component_valueContext *> CSS3Parser::BlockContext::component_value() {
  return getRuleContexts<CSS3Parser::Component_valueContext>();
}

CSS3Parser::Component_valueContext* CSS3Parser::BlockContext::component_value(size_t i) {
  return getRuleContext<CSS3Parser::Component_valueContext>(i);
}


size_t CSS3Parser::BlockContext::getRuleIndex() const {
  return CSS3Parser::RuleBlock;
}

void CSS3Parser::BlockContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBlock(this);
}

void CSS3Parser::BlockContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBlock(this);
}

CSS3Parser::BlockContext* CSS3Parser::block() {
  BlockContext *_localctx = _tracker.createInstance<BlockContext>(_ctx, getState());
  enterRule(_localctx, 14, CSS3Parser::RuleBlock);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(103);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CSS3Parser::T__0: {
        enterOuterAlt(_localctx, 1);
        setState(79);
        match(CSS3Parser::T__0);
        setState(83);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & 2113929002) != 0)) {
          setState(80);
          component_value();
          setState(85);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        setState(86);
        match(CSS3Parser::T__1);
        break;
      }

      case CSS3Parser::T__2: {
        enterOuterAlt(_localctx, 2);
        setState(87);
        match(CSS3Parser::T__2);
        setState(91);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & 2113929002) != 0)) {
          setState(88);
          component_value();
          setState(93);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        setState(94);
        match(CSS3Parser::T__3);
        break;
      }

      case CSS3Parser::T__4: {
        enterOuterAlt(_localctx, 3);
        setState(95);
        match(CSS3Parser::T__4);
        setState(99);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & 2113929002) != 0)) {
          setState(96);
          component_value();
          setState(101);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        setState(102);
        match(CSS3Parser::T__5);
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

//----------------- FunctionContext ------------------------------------------------------------------

CSS3Parser::FunctionContext::FunctionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSS3Parser::FunctionContext::FUNCTION() {
  return getToken(CSS3Parser::FUNCTION, 0);
}

std::vector<CSS3Parser::Component_valueContext *> CSS3Parser::FunctionContext::component_value() {
  return getRuleContexts<CSS3Parser::Component_valueContext>();
}

CSS3Parser::Component_valueContext* CSS3Parser::FunctionContext::component_value(size_t i) {
  return getRuleContext<CSS3Parser::Component_valueContext>(i);
}


size_t CSS3Parser::FunctionContext::getRuleIndex() const {
  return CSS3Parser::RuleFunction;
}

void CSS3Parser::FunctionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFunction(this);
}

void CSS3Parser::FunctionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFunction(this);
}

CSS3Parser::FunctionContext* CSS3Parser::function() {
  FunctionContext *_localctx = _tracker.createInstance<FunctionContext>(_ctx, getState());
  enterRule(_localctx, 16, CSS3Parser::RuleFunction);
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
    setState(105);
    match(CSS3Parser::FUNCTION);
    setState(109);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 2113929002) != 0)) {
      setState(106);
      component_value();
      setState(111);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(112);
    match(CSS3Parser::T__5);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Preserved_tokenContext ------------------------------------------------------------------

CSS3Parser::Preserved_tokenContext::Preserved_tokenContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSS3Parser::Preserved_tokenContext::IDENT() {
  return getToken(CSS3Parser::IDENT, 0);
}

tree::TerminalNode* CSS3Parser::Preserved_tokenContext::STRING() {
  return getToken(CSS3Parser::STRING, 0);
}

tree::TerminalNode* CSS3Parser::Preserved_tokenContext::DELIM() {
  return getToken(CSS3Parser::DELIM, 0);
}

tree::TerminalNode* CSS3Parser::Preserved_tokenContext::NUMBER() {
  return getToken(CSS3Parser::NUMBER, 0);
}

tree::TerminalNode* CSS3Parser::Preserved_tokenContext::PERCENTAGE() {
  return getToken(CSS3Parser::PERCENTAGE, 0);
}

tree::TerminalNode* CSS3Parser::Preserved_tokenContext::DIMENSION() {
  return getToken(CSS3Parser::DIMENSION, 0);
}

tree::TerminalNode* CSS3Parser::Preserved_tokenContext::URI() {
  return getToken(CSS3Parser::URI, 0);
}

tree::TerminalNode* CSS3Parser::Preserved_tokenContext::HASH() {
  return getToken(CSS3Parser::HASH, 0);
}

tree::TerminalNode* CSS3Parser::Preserved_tokenContext::UNICODE_RANGE() {
  return getToken(CSS3Parser::UNICODE_RANGE, 0);
}

tree::TerminalNode* CSS3Parser::Preserved_tokenContext::INCLUDES() {
  return getToken(CSS3Parser::INCLUDES, 0);
}

tree::TerminalNode* CSS3Parser::Preserved_tokenContext::DASHMATCH() {
  return getToken(CSS3Parser::DASHMATCH, 0);
}

tree::TerminalNode* CSS3Parser::Preserved_tokenContext::PREFIX_MATCH() {
  return getToken(CSS3Parser::PREFIX_MATCH, 0);
}

tree::TerminalNode* CSS3Parser::Preserved_tokenContext::SUFFIX_MATCH() {
  return getToken(CSS3Parser::SUFFIX_MATCH, 0);
}

tree::TerminalNode* CSS3Parser::Preserved_tokenContext::SUBSTRING_MATCH() {
  return getToken(CSS3Parser::SUBSTRING_MATCH, 0);
}

tree::TerminalNode* CSS3Parser::Preserved_tokenContext::COLUMN() {
  return getToken(CSS3Parser::COLUMN, 0);
}

tree::TerminalNode* CSS3Parser::Preserved_tokenContext::CDO() {
  return getToken(CSS3Parser::CDO, 0);
}

tree::TerminalNode* CSS3Parser::Preserved_tokenContext::CDC() {
  return getToken(CSS3Parser::CDC, 0);
}

tree::TerminalNode* CSS3Parser::Preserved_tokenContext::COLON() {
  return getToken(CSS3Parser::COLON, 0);
}

tree::TerminalNode* CSS3Parser::Preserved_tokenContext::SEMICOLON() {
  return getToken(CSS3Parser::SEMICOLON, 0);
}

tree::TerminalNode* CSS3Parser::Preserved_tokenContext::COMMA() {
  return getToken(CSS3Parser::COMMA, 0);
}

tree::TerminalNode* CSS3Parser::Preserved_tokenContext::WS() {
  return getToken(CSS3Parser::WS, 0);
}


size_t CSS3Parser::Preserved_tokenContext::getRuleIndex() const {
  return CSS3Parser::RulePreserved_token;
}

void CSS3Parser::Preserved_tokenContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPreserved_token(this);
}

void CSS3Parser::Preserved_tokenContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPreserved_token(this);
}

CSS3Parser::Preserved_tokenContext* CSS3Parser::preserved_token() {
  Preserved_tokenContext *_localctx = _tracker.createInstance<Preserved_tokenContext>(_ctx, getState());
  enterRule(_localctx, 18, CSS3Parser::RulePreserved_token);
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
    setState(114);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 2113928448) != 0))) {
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

void CSS3Parser::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  css3ParserInitialize();
#else
  ::antlr4::internal::call_once(css3ParserOnceFlag, css3ParserInitialize);
#endif
}
