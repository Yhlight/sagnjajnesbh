
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
      "stylesheet", "at_rule", "ruleset", "selector_list", "selector", "combinator", 
      "simple_selector", "element_name", "selector_modifier", "class_selector", 
      "attrib", "attrib_operator", "pseudo", "functional_pseudo", "block", 
      "declaration_list", "declaration", "property", "value_list", "value", 
      "unit", "any_value"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "", "", "", "'~='", "'|='", "'^='", "'$='", 
      "'*='", "'='", "';'", "'{'", "'}'", "'('", "')'", "'['", "']'", "','", 
      "':'", "'.'", "'+'", "'-'", "'*'", "'/'", "'>'", "'~'"
    },
    std::vector<std::string>{
      "", "AT_KEYWORD", "HASH", "STRING", "URI", "FUNCTION", "NUMBER", "PERCENTAGE", 
      "IDENT", "INCLUDES", "DASHMATCH", "PREFIXMATCH", "SUFFIXMATCH", "SUBSTRINGMATCH", 
      "EQUALS", "SEMICOLON", "LBRACE", "RBRACE", "LPAREN", "RPAREN", "LBRACKET", 
      "RBRACKET", "COMMA", "COLON", "DOT", "PLUS", "MINUS", "STAR", "SLASH", 
      "GREATER", "TILDE", "IMPORTANT", "WS", "COMMENT"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,33,233,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,1,0,1,0,5,0,47,8,0,10,0,12,0,50,9,0,1,0,1,0,1,1,1,1,5,1,56,8,1,10,
  	1,12,1,59,9,1,1,1,1,1,3,1,63,8,1,1,2,1,2,1,2,1,3,1,3,1,3,5,3,71,8,3,10,
  	3,12,3,74,9,3,1,4,1,4,1,4,1,4,5,4,80,8,4,10,4,12,4,83,9,4,1,5,1,5,1,6,
  	1,6,5,6,89,8,6,10,6,12,6,92,9,6,1,6,4,6,95,8,6,11,6,12,6,96,3,6,99,8,
  	6,1,7,1,7,1,8,1,8,1,8,1,8,3,8,107,8,8,1,9,1,9,1,9,1,10,1,10,1,10,1,10,
  	1,10,3,10,117,8,10,1,10,1,10,1,11,1,11,1,12,1,12,3,12,125,8,12,1,12,1,
  	12,3,12,129,8,12,1,13,1,13,5,13,133,8,13,10,13,12,13,136,9,13,1,13,1,
  	13,1,14,1,14,1,14,1,14,1,15,1,15,1,15,3,15,147,8,15,5,15,149,8,15,10,
  	15,12,15,152,9,15,1,16,1,16,1,16,1,16,3,16,158,8,16,1,17,1,17,1,18,4,
  	18,163,8,18,11,18,12,18,164,1,19,1,19,3,19,169,8,19,1,19,1,19,1,19,1,
  	19,1,19,1,19,1,19,5,19,178,8,19,10,19,12,19,181,9,19,1,19,1,19,1,19,5,
  	19,186,8,19,10,19,12,19,189,9,19,1,19,1,19,1,19,5,19,194,8,19,10,19,12,
  	19,197,9,19,1,19,3,19,200,8,19,1,20,1,20,1,21,1,21,3,21,206,8,21,1,21,
  	1,21,1,21,1,21,1,21,1,21,1,21,1,21,1,21,1,21,1,21,1,21,1,21,1,21,1,21,
  	1,21,1,21,1,21,1,21,1,21,1,21,1,21,1,21,3,21,231,8,21,1,21,0,0,22,0,2,
  	4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,0,4,3,0,25,25,
  	29,30,32,32,2,0,8,8,27,27,2,0,3,3,8,8,1,0,9,14,266,0,48,1,0,0,0,2,53,
  	1,0,0,0,4,64,1,0,0,0,6,67,1,0,0,0,8,75,1,0,0,0,10,84,1,0,0,0,12,98,1,
  	0,0,0,14,100,1,0,0,0,16,106,1,0,0,0,18,108,1,0,0,0,20,111,1,0,0,0,22,
  	120,1,0,0,0,24,122,1,0,0,0,26,130,1,0,0,0,28,139,1,0,0,0,30,143,1,0,0,
  	0,32,153,1,0,0,0,34,159,1,0,0,0,36,162,1,0,0,0,38,199,1,0,0,0,40,201,
  	1,0,0,0,42,230,1,0,0,0,44,47,3,4,2,0,45,47,3,2,1,0,46,44,1,0,0,0,46,45,
  	1,0,0,0,47,50,1,0,0,0,48,46,1,0,0,0,48,49,1,0,0,0,49,51,1,0,0,0,50,48,
  	1,0,0,0,51,52,5,0,0,1,52,1,1,0,0,0,53,57,5,1,0,0,54,56,3,42,21,0,55,54,
  	1,0,0,0,56,59,1,0,0,0,57,55,1,0,0,0,57,58,1,0,0,0,58,62,1,0,0,0,59,57,
  	1,0,0,0,60,63,5,15,0,0,61,63,3,28,14,0,62,60,1,0,0,0,62,61,1,0,0,0,63,
  	3,1,0,0,0,64,65,3,6,3,0,65,66,3,28,14,0,66,5,1,0,0,0,67,72,3,8,4,0,68,
  	69,5,22,0,0,69,71,3,8,4,0,70,68,1,0,0,0,71,74,1,0,0,0,72,70,1,0,0,0,72,
  	73,1,0,0,0,73,7,1,0,0,0,74,72,1,0,0,0,75,81,3,12,6,0,76,77,3,10,5,0,77,
  	78,3,12,6,0,78,80,1,0,0,0,79,76,1,0,0,0,80,83,1,0,0,0,81,79,1,0,0,0,81,
  	82,1,0,0,0,82,9,1,0,0,0,83,81,1,0,0,0,84,85,7,0,0,0,85,11,1,0,0,0,86,
  	90,3,14,7,0,87,89,3,16,8,0,88,87,1,0,0,0,89,92,1,0,0,0,90,88,1,0,0,0,
  	90,91,1,0,0,0,91,99,1,0,0,0,92,90,1,0,0,0,93,95,3,16,8,0,94,93,1,0,0,
  	0,95,96,1,0,0,0,96,94,1,0,0,0,96,97,1,0,0,0,97,99,1,0,0,0,98,86,1,0,0,
  	0,98,94,1,0,0,0,99,13,1,0,0,0,100,101,7,1,0,0,101,15,1,0,0,0,102,107,
  	5,2,0,0,103,107,3,18,9,0,104,107,3,20,10,0,105,107,3,24,12,0,106,102,
  	1,0,0,0,106,103,1,0,0,0,106,104,1,0,0,0,106,105,1,0,0,0,107,17,1,0,0,
  	0,108,109,5,24,0,0,109,110,5,8,0,0,110,19,1,0,0,0,111,112,5,20,0,0,112,
  	116,5,8,0,0,113,114,3,22,11,0,114,115,7,2,0,0,115,117,1,0,0,0,116,113,
  	1,0,0,0,116,117,1,0,0,0,117,118,1,0,0,0,118,119,5,21,0,0,119,21,1,0,0,
  	0,120,121,7,3,0,0,121,23,1,0,0,0,122,124,5,23,0,0,123,125,5,23,0,0,124,
  	123,1,0,0,0,124,125,1,0,0,0,125,128,1,0,0,0,126,129,5,8,0,0,127,129,3,
  	26,13,0,128,126,1,0,0,0,128,127,1,0,0,0,129,25,1,0,0,0,130,134,5,5,0,
  	0,131,133,3,42,21,0,132,131,1,0,0,0,133,136,1,0,0,0,134,132,1,0,0,0,134,
  	135,1,0,0,0,135,137,1,0,0,0,136,134,1,0,0,0,137,138,5,19,0,0,138,27,1,
  	0,0,0,139,140,5,16,0,0,140,141,3,30,15,0,141,142,5,17,0,0,142,29,1,0,
  	0,0,143,150,3,32,16,0,144,146,5,15,0,0,145,147,3,32,16,0,146,145,1,0,
  	0,0,146,147,1,0,0,0,147,149,1,0,0,0,148,144,1,0,0,0,149,152,1,0,0,0,150,
  	148,1,0,0,0,150,151,1,0,0,0,151,31,1,0,0,0,152,150,1,0,0,0,153,154,3,
  	34,17,0,154,155,5,23,0,0,155,157,3,36,18,0,156,158,5,31,0,0,157,156,1,
  	0,0,0,157,158,1,0,0,0,158,33,1,0,0,0,159,160,5,8,0,0,160,35,1,0,0,0,161,
  	163,3,38,19,0,162,161,1,0,0,0,163,164,1,0,0,0,164,162,1,0,0,0,164,165,
  	1,0,0,0,165,37,1,0,0,0,166,168,5,6,0,0,167,169,3,40,20,0,168,167,1,0,
  	0,0,168,169,1,0,0,0,169,200,1,0,0,0,170,200,5,7,0,0,171,200,5,3,0,0,172,
  	200,5,8,0,0,173,200,5,2,0,0,174,200,5,4,0,0,175,179,5,5,0,0,176,178,3,
  	42,21,0,177,176,1,0,0,0,178,181,1,0,0,0,179,177,1,0,0,0,179,180,1,0,0,
  	0,180,182,1,0,0,0,181,179,1,0,0,0,182,200,5,19,0,0,183,187,5,18,0,0,184,
  	186,3,42,21,0,185,184,1,0,0,0,186,189,1,0,0,0,187,185,1,0,0,0,187,188,
  	1,0,0,0,188,190,1,0,0,0,189,187,1,0,0,0,190,200,5,19,0,0,191,195,5,20,
  	0,0,192,194,3,42,21,0,193,192,1,0,0,0,194,197,1,0,0,0,195,193,1,0,0,0,
  	195,196,1,0,0,0,196,198,1,0,0,0,197,195,1,0,0,0,198,200,5,21,0,0,199,
  	166,1,0,0,0,199,170,1,0,0,0,199,171,1,0,0,0,199,172,1,0,0,0,199,173,1,
  	0,0,0,199,174,1,0,0,0,199,175,1,0,0,0,199,183,1,0,0,0,199,191,1,0,0,0,
  	200,39,1,0,0,0,201,202,5,8,0,0,202,41,1,0,0,0,203,205,5,6,0,0,204,206,
  	3,40,20,0,205,204,1,0,0,0,205,206,1,0,0,0,206,231,1,0,0,0,207,231,5,7,
  	0,0,208,231,5,3,0,0,209,231,5,8,0,0,210,231,5,2,0,0,211,231,5,4,0,0,212,
  	231,5,5,0,0,213,231,5,18,0,0,214,231,5,19,0,0,215,231,5,20,0,0,216,231,
  	5,21,0,0,217,231,5,16,0,0,218,231,5,17,0,0,219,231,5,15,0,0,220,231,5,
  	23,0,0,221,231,5,22,0,0,222,231,5,25,0,0,223,231,5,26,0,0,224,231,5,27,
  	0,0,225,231,5,28,0,0,226,231,5,14,0,0,227,231,5,24,0,0,228,231,5,29,0,
  	0,229,231,5,30,0,0,230,203,1,0,0,0,230,207,1,0,0,0,230,208,1,0,0,0,230,
  	209,1,0,0,0,230,210,1,0,0,0,230,211,1,0,0,0,230,212,1,0,0,0,230,213,1,
  	0,0,0,230,214,1,0,0,0,230,215,1,0,0,0,230,216,1,0,0,0,230,217,1,0,0,0,
  	230,218,1,0,0,0,230,219,1,0,0,0,230,220,1,0,0,0,230,221,1,0,0,0,230,222,
  	1,0,0,0,230,223,1,0,0,0,230,224,1,0,0,0,230,225,1,0,0,0,230,226,1,0,0,
  	0,230,227,1,0,0,0,230,228,1,0,0,0,230,229,1,0,0,0,231,43,1,0,0,0,25,46,
  	48,57,62,72,81,90,96,98,106,116,124,128,134,146,150,157,164,168,179,187,
  	195,199,205,230
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

std::vector<CSS3Parser::RulesetContext *> CSS3Parser::StylesheetContext::ruleset() {
  return getRuleContexts<CSS3Parser::RulesetContext>();
}

CSS3Parser::RulesetContext* CSS3Parser::StylesheetContext::ruleset(size_t i) {
  return getRuleContext<CSS3Parser::RulesetContext>(i);
}

std::vector<CSS3Parser::At_ruleContext *> CSS3Parser::StylesheetContext::at_rule() {
  return getRuleContexts<CSS3Parser::At_ruleContext>();
}

CSS3Parser::At_ruleContext* CSS3Parser::StylesheetContext::at_rule(size_t i) {
  return getRuleContext<CSS3Parser::At_ruleContext>(i);
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
    setState(48);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 160432390) != 0)) {
      setState(46);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case CSS3Parser::HASH:
        case CSS3Parser::IDENT:
        case CSS3Parser::LBRACKET:
        case CSS3Parser::COLON:
        case CSS3Parser::DOT:
        case CSS3Parser::STAR: {
          setState(44);
          ruleset();
          break;
        }

        case CSS3Parser::AT_KEYWORD: {
          setState(45);
          at_rule();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(50);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(51);
    match(CSS3Parser::EOF);
   
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

tree::TerminalNode* CSS3Parser::At_ruleContext::SEMICOLON() {
  return getToken(CSS3Parser::SEMICOLON, 0);
}

CSS3Parser::BlockContext* CSS3Parser::At_ruleContext::block() {
  return getRuleContext<CSS3Parser::BlockContext>(0);
}

std::vector<CSS3Parser::Any_valueContext *> CSS3Parser::At_ruleContext::any_value() {
  return getRuleContexts<CSS3Parser::Any_valueContext>();
}

CSS3Parser::Any_valueContext* CSS3Parser::At_ruleContext::any_value(size_t i) {
  return getRuleContext<CSS3Parser::Any_valueContext>(i);
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
  enterRule(_localctx, 2, CSS3Parser::RuleAt_rule);

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
    setState(53);
    match(CSS3Parser::AT_KEYWORD);
    setState(57);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 2, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(54);
        any_value(); 
      }
      setState(59);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 2, _ctx);
    }
    setState(62);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CSS3Parser::SEMICOLON: {
        setState(60);
        match(CSS3Parser::SEMICOLON);
        break;
      }

      case CSS3Parser::LBRACE: {
        setState(61);
        block();
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

//----------------- RulesetContext ------------------------------------------------------------------

CSS3Parser::RulesetContext::RulesetContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

CSS3Parser::Selector_listContext* CSS3Parser::RulesetContext::selector_list() {
  return getRuleContext<CSS3Parser::Selector_listContext>(0);
}

CSS3Parser::BlockContext* CSS3Parser::RulesetContext::block() {
  return getRuleContext<CSS3Parser::BlockContext>(0);
}


size_t CSS3Parser::RulesetContext::getRuleIndex() const {
  return CSS3Parser::RuleRuleset;
}

void CSS3Parser::RulesetContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterRuleset(this);
}

void CSS3Parser::RulesetContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitRuleset(this);
}

CSS3Parser::RulesetContext* CSS3Parser::ruleset() {
  RulesetContext *_localctx = _tracker.createInstance<RulesetContext>(_ctx, getState());
  enterRule(_localctx, 4, CSS3Parser::RuleRuleset);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(64);
    selector_list();
    setState(65);
    block();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Selector_listContext ------------------------------------------------------------------

CSS3Parser::Selector_listContext::Selector_listContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<CSS3Parser::SelectorContext *> CSS3Parser::Selector_listContext::selector() {
  return getRuleContexts<CSS3Parser::SelectorContext>();
}

CSS3Parser::SelectorContext* CSS3Parser::Selector_listContext::selector(size_t i) {
  return getRuleContext<CSS3Parser::SelectorContext>(i);
}

std::vector<tree::TerminalNode *> CSS3Parser::Selector_listContext::COMMA() {
  return getTokens(CSS3Parser::COMMA);
}

tree::TerminalNode* CSS3Parser::Selector_listContext::COMMA(size_t i) {
  return getToken(CSS3Parser::COMMA, i);
}


size_t CSS3Parser::Selector_listContext::getRuleIndex() const {
  return CSS3Parser::RuleSelector_list;
}

void CSS3Parser::Selector_listContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSelector_list(this);
}

void CSS3Parser::Selector_listContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSelector_list(this);
}

CSS3Parser::Selector_listContext* CSS3Parser::selector_list() {
  Selector_listContext *_localctx = _tracker.createInstance<Selector_listContext>(_ctx, getState());
  enterRule(_localctx, 6, CSS3Parser::RuleSelector_list);
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
    setState(67);
    selector();
    setState(72);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == CSS3Parser::COMMA) {
      setState(68);
      match(CSS3Parser::COMMA);
      setState(69);
      selector();
      setState(74);
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

//----------------- SelectorContext ------------------------------------------------------------------

CSS3Parser::SelectorContext::SelectorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<CSS3Parser::Simple_selectorContext *> CSS3Parser::SelectorContext::simple_selector() {
  return getRuleContexts<CSS3Parser::Simple_selectorContext>();
}

CSS3Parser::Simple_selectorContext* CSS3Parser::SelectorContext::simple_selector(size_t i) {
  return getRuleContext<CSS3Parser::Simple_selectorContext>(i);
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
  enterRule(_localctx, 8, CSS3Parser::RuleSelector);
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
    setState(75);
    simple_selector();
    setState(81);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 5939134464) != 0)) {
      setState(76);
      combinator();
      setState(77);
      simple_selector();
      setState(83);
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

tree::TerminalNode* CSS3Parser::CombinatorContext::WS() {
  return getToken(CSS3Parser::WS, 0);
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
  enterRule(_localctx, 10, CSS3Parser::RuleCombinator);
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
    setState(84);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 5939134464) != 0))) {
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

//----------------- Simple_selectorContext ------------------------------------------------------------------

CSS3Parser::Simple_selectorContext::Simple_selectorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

CSS3Parser::Element_nameContext* CSS3Parser::Simple_selectorContext::element_name() {
  return getRuleContext<CSS3Parser::Element_nameContext>(0);
}

std::vector<CSS3Parser::Selector_modifierContext *> CSS3Parser::Simple_selectorContext::selector_modifier() {
  return getRuleContexts<CSS3Parser::Selector_modifierContext>();
}

CSS3Parser::Selector_modifierContext* CSS3Parser::Simple_selectorContext::selector_modifier(size_t i) {
  return getRuleContext<CSS3Parser::Selector_modifierContext>(i);
}


size_t CSS3Parser::Simple_selectorContext::getRuleIndex() const {
  return CSS3Parser::RuleSimple_selector;
}

void CSS3Parser::Simple_selectorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSimple_selector(this);
}

void CSS3Parser::Simple_selectorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSimple_selector(this);
}

CSS3Parser::Simple_selectorContext* CSS3Parser::simple_selector() {
  Simple_selectorContext *_localctx = _tracker.createInstance<Simple_selectorContext>(_ctx, getState());
  enterRule(_localctx, 12, CSS3Parser::RuleSimple_selector);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(98);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CSS3Parser::IDENT:
      case CSS3Parser::STAR: {
        enterOuterAlt(_localctx, 1);
        setState(86);
        element_name();
        setState(90);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & 26214404) != 0)) {
          setState(87);
          selector_modifier();
          setState(92);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        break;
      }

      case CSS3Parser::HASH:
      case CSS3Parser::LBRACKET:
      case CSS3Parser::COLON:
      case CSS3Parser::DOT: {
        enterOuterAlt(_localctx, 2);
        setState(94); 
        _errHandler->sync(this);
        _la = _input->LA(1);
        do {
          setState(93);
          selector_modifier();
          setState(96); 
          _errHandler->sync(this);
          _la = _input->LA(1);
        } while ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & 26214404) != 0));
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

//----------------- Element_nameContext ------------------------------------------------------------------

CSS3Parser::Element_nameContext::Element_nameContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSS3Parser::Element_nameContext::IDENT() {
  return getToken(CSS3Parser::IDENT, 0);
}

tree::TerminalNode* CSS3Parser::Element_nameContext::STAR() {
  return getToken(CSS3Parser::STAR, 0);
}


size_t CSS3Parser::Element_nameContext::getRuleIndex() const {
  return CSS3Parser::RuleElement_name;
}

void CSS3Parser::Element_nameContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterElement_name(this);
}

void CSS3Parser::Element_nameContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitElement_name(this);
}

CSS3Parser::Element_nameContext* CSS3Parser::element_name() {
  Element_nameContext *_localctx = _tracker.createInstance<Element_nameContext>(_ctx, getState());
  enterRule(_localctx, 14, CSS3Parser::RuleElement_name);
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
    setState(100);
    _la = _input->LA(1);
    if (!(_la == CSS3Parser::IDENT

    || _la == CSS3Parser::STAR)) {
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

//----------------- Selector_modifierContext ------------------------------------------------------------------

CSS3Parser::Selector_modifierContext::Selector_modifierContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSS3Parser::Selector_modifierContext::HASH() {
  return getToken(CSS3Parser::HASH, 0);
}

CSS3Parser::Class_selectorContext* CSS3Parser::Selector_modifierContext::class_selector() {
  return getRuleContext<CSS3Parser::Class_selectorContext>(0);
}

CSS3Parser::AttribContext* CSS3Parser::Selector_modifierContext::attrib() {
  return getRuleContext<CSS3Parser::AttribContext>(0);
}

CSS3Parser::PseudoContext* CSS3Parser::Selector_modifierContext::pseudo() {
  return getRuleContext<CSS3Parser::PseudoContext>(0);
}


size_t CSS3Parser::Selector_modifierContext::getRuleIndex() const {
  return CSS3Parser::RuleSelector_modifier;
}

void CSS3Parser::Selector_modifierContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSelector_modifier(this);
}

void CSS3Parser::Selector_modifierContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSelector_modifier(this);
}

CSS3Parser::Selector_modifierContext* CSS3Parser::selector_modifier() {
  Selector_modifierContext *_localctx = _tracker.createInstance<Selector_modifierContext>(_ctx, getState());
  enterRule(_localctx, 16, CSS3Parser::RuleSelector_modifier);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(106);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CSS3Parser::HASH: {
        enterOuterAlt(_localctx, 1);
        setState(102);
        match(CSS3Parser::HASH);
        break;
      }

      case CSS3Parser::DOT: {
        enterOuterAlt(_localctx, 2);
        setState(103);
        class_selector();
        break;
      }

      case CSS3Parser::LBRACKET: {
        enterOuterAlt(_localctx, 3);
        setState(104);
        attrib();
        break;
      }

      case CSS3Parser::COLON: {
        enterOuterAlt(_localctx, 4);
        setState(105);
        pseudo();
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

//----------------- Class_selectorContext ------------------------------------------------------------------

CSS3Parser::Class_selectorContext::Class_selectorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSS3Parser::Class_selectorContext::DOT() {
  return getToken(CSS3Parser::DOT, 0);
}

tree::TerminalNode* CSS3Parser::Class_selectorContext::IDENT() {
  return getToken(CSS3Parser::IDENT, 0);
}


size_t CSS3Parser::Class_selectorContext::getRuleIndex() const {
  return CSS3Parser::RuleClass_selector;
}

void CSS3Parser::Class_selectorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterClass_selector(this);
}

void CSS3Parser::Class_selectorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitClass_selector(this);
}

CSS3Parser::Class_selectorContext* CSS3Parser::class_selector() {
  Class_selectorContext *_localctx = _tracker.createInstance<Class_selectorContext>(_ctx, getState());
  enterRule(_localctx, 18, CSS3Parser::RuleClass_selector);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(108);
    match(CSS3Parser::DOT);
    setState(109);
    match(CSS3Parser::IDENT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AttribContext ------------------------------------------------------------------

CSS3Parser::AttribContext::AttribContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSS3Parser::AttribContext::LBRACKET() {
  return getToken(CSS3Parser::LBRACKET, 0);
}

std::vector<tree::TerminalNode *> CSS3Parser::AttribContext::IDENT() {
  return getTokens(CSS3Parser::IDENT);
}

tree::TerminalNode* CSS3Parser::AttribContext::IDENT(size_t i) {
  return getToken(CSS3Parser::IDENT, i);
}

tree::TerminalNode* CSS3Parser::AttribContext::RBRACKET() {
  return getToken(CSS3Parser::RBRACKET, 0);
}

CSS3Parser::Attrib_operatorContext* CSS3Parser::AttribContext::attrib_operator() {
  return getRuleContext<CSS3Parser::Attrib_operatorContext>(0);
}

tree::TerminalNode* CSS3Parser::AttribContext::STRING() {
  return getToken(CSS3Parser::STRING, 0);
}


size_t CSS3Parser::AttribContext::getRuleIndex() const {
  return CSS3Parser::RuleAttrib;
}

void CSS3Parser::AttribContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAttrib(this);
}

void CSS3Parser::AttribContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAttrib(this);
}

CSS3Parser::AttribContext* CSS3Parser::attrib() {
  AttribContext *_localctx = _tracker.createInstance<AttribContext>(_ctx, getState());
  enterRule(_localctx, 20, CSS3Parser::RuleAttrib);
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
    setState(111);
    match(CSS3Parser::LBRACKET);
    setState(112);
    match(CSS3Parser::IDENT);
    setState(116);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 32256) != 0)) {
      setState(113);
      attrib_operator();
      setState(114);
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
    setState(118);
    match(CSS3Parser::RBRACKET);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Attrib_operatorContext ------------------------------------------------------------------

CSS3Parser::Attrib_operatorContext::Attrib_operatorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSS3Parser::Attrib_operatorContext::EQUALS() {
  return getToken(CSS3Parser::EQUALS, 0);
}

tree::TerminalNode* CSS3Parser::Attrib_operatorContext::INCLUDES() {
  return getToken(CSS3Parser::INCLUDES, 0);
}

tree::TerminalNode* CSS3Parser::Attrib_operatorContext::DASHMATCH() {
  return getToken(CSS3Parser::DASHMATCH, 0);
}

tree::TerminalNode* CSS3Parser::Attrib_operatorContext::PREFIXMATCH() {
  return getToken(CSS3Parser::PREFIXMATCH, 0);
}

tree::TerminalNode* CSS3Parser::Attrib_operatorContext::SUFFIXMATCH() {
  return getToken(CSS3Parser::SUFFIXMATCH, 0);
}

tree::TerminalNode* CSS3Parser::Attrib_operatorContext::SUBSTRINGMATCH() {
  return getToken(CSS3Parser::SUBSTRINGMATCH, 0);
}


size_t CSS3Parser::Attrib_operatorContext::getRuleIndex() const {
  return CSS3Parser::RuleAttrib_operator;
}

void CSS3Parser::Attrib_operatorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAttrib_operator(this);
}

void CSS3Parser::Attrib_operatorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAttrib_operator(this);
}

CSS3Parser::Attrib_operatorContext* CSS3Parser::attrib_operator() {
  Attrib_operatorContext *_localctx = _tracker.createInstance<Attrib_operatorContext>(_ctx, getState());
  enterRule(_localctx, 22, CSS3Parser::RuleAttrib_operator);
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
    setState(120);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 32256) != 0))) {
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

//----------------- PseudoContext ------------------------------------------------------------------

CSS3Parser::PseudoContext::PseudoContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> CSS3Parser::PseudoContext::COLON() {
  return getTokens(CSS3Parser::COLON);
}

tree::TerminalNode* CSS3Parser::PseudoContext::COLON(size_t i) {
  return getToken(CSS3Parser::COLON, i);
}

tree::TerminalNode* CSS3Parser::PseudoContext::IDENT() {
  return getToken(CSS3Parser::IDENT, 0);
}

CSS3Parser::Functional_pseudoContext* CSS3Parser::PseudoContext::functional_pseudo() {
  return getRuleContext<CSS3Parser::Functional_pseudoContext>(0);
}


size_t CSS3Parser::PseudoContext::getRuleIndex() const {
  return CSS3Parser::RulePseudo;
}

void CSS3Parser::PseudoContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPseudo(this);
}

void CSS3Parser::PseudoContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPseudo(this);
}

CSS3Parser::PseudoContext* CSS3Parser::pseudo() {
  PseudoContext *_localctx = _tracker.createInstance<PseudoContext>(_ctx, getState());
  enterRule(_localctx, 24, CSS3Parser::RulePseudo);
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
    setState(122);
    match(CSS3Parser::COLON);
    setState(124);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == CSS3Parser::COLON) {
      setState(123);
      match(CSS3Parser::COLON);
    }
    setState(128);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CSS3Parser::IDENT: {
        setState(126);
        match(CSS3Parser::IDENT);
        break;
      }

      case CSS3Parser::FUNCTION: {
        setState(127);
        functional_pseudo();
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

//----------------- Functional_pseudoContext ------------------------------------------------------------------

CSS3Parser::Functional_pseudoContext::Functional_pseudoContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSS3Parser::Functional_pseudoContext::FUNCTION() {
  return getToken(CSS3Parser::FUNCTION, 0);
}

tree::TerminalNode* CSS3Parser::Functional_pseudoContext::RPAREN() {
  return getToken(CSS3Parser::RPAREN, 0);
}

std::vector<CSS3Parser::Any_valueContext *> CSS3Parser::Functional_pseudoContext::any_value() {
  return getRuleContexts<CSS3Parser::Any_valueContext>();
}

CSS3Parser::Any_valueContext* CSS3Parser::Functional_pseudoContext::any_value(size_t i) {
  return getRuleContext<CSS3Parser::Any_valueContext>(i);
}


size_t CSS3Parser::Functional_pseudoContext::getRuleIndex() const {
  return CSS3Parser::RuleFunctional_pseudo;
}

void CSS3Parser::Functional_pseudoContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFunctional_pseudo(this);
}

void CSS3Parser::Functional_pseudoContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFunctional_pseudo(this);
}

CSS3Parser::Functional_pseudoContext* CSS3Parser::functional_pseudo() {
  Functional_pseudoContext *_localctx = _tracker.createInstance<Functional_pseudoContext>(_ctx, getState());
  enterRule(_localctx, 26, CSS3Parser::RuleFunctional_pseudo);

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
    setState(130);
    match(CSS3Parser::FUNCTION);
    setState(134);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 13, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(131);
        any_value(); 
      }
      setState(136);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 13, _ctx);
    }
    setState(137);
    match(CSS3Parser::RPAREN);
   
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

tree::TerminalNode* CSS3Parser::BlockContext::LBRACE() {
  return getToken(CSS3Parser::LBRACE, 0);
}

CSS3Parser::Declaration_listContext* CSS3Parser::BlockContext::declaration_list() {
  return getRuleContext<CSS3Parser::Declaration_listContext>(0);
}

tree::TerminalNode* CSS3Parser::BlockContext::RBRACE() {
  return getToken(CSS3Parser::RBRACE, 0);
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
  enterRule(_localctx, 28, CSS3Parser::RuleBlock);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(139);
    match(CSS3Parser::LBRACE);
    setState(140);
    declaration_list();
    setState(141);
    match(CSS3Parser::RBRACE);
   
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
  enterRule(_localctx, 30, CSS3Parser::RuleDeclaration_list);
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
    setState(143);
    declaration();
    setState(150);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == CSS3Parser::SEMICOLON) {
      setState(144);
      match(CSS3Parser::SEMICOLON);
      setState(146);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == CSS3Parser::IDENT) {
        setState(145);
        declaration();
      }
      setState(152);
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

CSS3Parser::PropertyContext* CSS3Parser::DeclarationContext::property() {
  return getRuleContext<CSS3Parser::PropertyContext>(0);
}

tree::TerminalNode* CSS3Parser::DeclarationContext::COLON() {
  return getToken(CSS3Parser::COLON, 0);
}

CSS3Parser::Value_listContext* CSS3Parser::DeclarationContext::value_list() {
  return getRuleContext<CSS3Parser::Value_listContext>(0);
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
  enterRule(_localctx, 32, CSS3Parser::RuleDeclaration);
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
    setState(153);
    property();
    setState(154);
    match(CSS3Parser::COLON);
    setState(155);
    value_list();
    setState(157);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == CSS3Parser::IMPORTANT) {
      setState(156);
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
  enterRule(_localctx, 34, CSS3Parser::RuleProperty);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(159);
    match(CSS3Parser::IDENT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Value_listContext ------------------------------------------------------------------

CSS3Parser::Value_listContext::Value_listContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<CSS3Parser::ValueContext *> CSS3Parser::Value_listContext::value() {
  return getRuleContexts<CSS3Parser::ValueContext>();
}

CSS3Parser::ValueContext* CSS3Parser::Value_listContext::value(size_t i) {
  return getRuleContext<CSS3Parser::ValueContext>(i);
}


size_t CSS3Parser::Value_listContext::getRuleIndex() const {
  return CSS3Parser::RuleValue_list;
}

void CSS3Parser::Value_listContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterValue_list(this);
}

void CSS3Parser::Value_listContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitValue_list(this);
}

CSS3Parser::Value_listContext* CSS3Parser::value_list() {
  Value_listContext *_localctx = _tracker.createInstance<Value_listContext>(_ctx, getState());
  enterRule(_localctx, 36, CSS3Parser::RuleValue_list);
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
    setState(162); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(161);
      value();
      setState(164); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 1311228) != 0));
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ValueContext ------------------------------------------------------------------

CSS3Parser::ValueContext::ValueContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSS3Parser::ValueContext::NUMBER() {
  return getToken(CSS3Parser::NUMBER, 0);
}

CSS3Parser::UnitContext* CSS3Parser::ValueContext::unit() {
  return getRuleContext<CSS3Parser::UnitContext>(0);
}

tree::TerminalNode* CSS3Parser::ValueContext::PERCENTAGE() {
  return getToken(CSS3Parser::PERCENTAGE, 0);
}

tree::TerminalNode* CSS3Parser::ValueContext::STRING() {
  return getToken(CSS3Parser::STRING, 0);
}

tree::TerminalNode* CSS3Parser::ValueContext::IDENT() {
  return getToken(CSS3Parser::IDENT, 0);
}

tree::TerminalNode* CSS3Parser::ValueContext::HASH() {
  return getToken(CSS3Parser::HASH, 0);
}

tree::TerminalNode* CSS3Parser::ValueContext::URI() {
  return getToken(CSS3Parser::URI, 0);
}

tree::TerminalNode* CSS3Parser::ValueContext::FUNCTION() {
  return getToken(CSS3Parser::FUNCTION, 0);
}

tree::TerminalNode* CSS3Parser::ValueContext::RPAREN() {
  return getToken(CSS3Parser::RPAREN, 0);
}

std::vector<CSS3Parser::Any_valueContext *> CSS3Parser::ValueContext::any_value() {
  return getRuleContexts<CSS3Parser::Any_valueContext>();
}

CSS3Parser::Any_valueContext* CSS3Parser::ValueContext::any_value(size_t i) {
  return getRuleContext<CSS3Parser::Any_valueContext>(i);
}

tree::TerminalNode* CSS3Parser::ValueContext::LPAREN() {
  return getToken(CSS3Parser::LPAREN, 0);
}

tree::TerminalNode* CSS3Parser::ValueContext::LBRACKET() {
  return getToken(CSS3Parser::LBRACKET, 0);
}

tree::TerminalNode* CSS3Parser::ValueContext::RBRACKET() {
  return getToken(CSS3Parser::RBRACKET, 0);
}


size_t CSS3Parser::ValueContext::getRuleIndex() const {
  return CSS3Parser::RuleValue;
}

void CSS3Parser::ValueContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterValue(this);
}

void CSS3Parser::ValueContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitValue(this);
}

CSS3Parser::ValueContext* CSS3Parser::value() {
  ValueContext *_localctx = _tracker.createInstance<ValueContext>(_ctx, getState());
  enterRule(_localctx, 38, CSS3Parser::RuleValue);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    size_t alt;
    setState(199);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CSS3Parser::NUMBER: {
        enterOuterAlt(_localctx, 1);
        setState(166);
        match(CSS3Parser::NUMBER);
        setState(168);
        _errHandler->sync(this);

        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 18, _ctx)) {
        case 1: {
          setState(167);
          unit();
          break;
        }

        default:
          break;
        }
        break;
      }

      case CSS3Parser::PERCENTAGE: {
        enterOuterAlt(_localctx, 2);
        setState(170);
        match(CSS3Parser::PERCENTAGE);
        break;
      }

      case CSS3Parser::STRING: {
        enterOuterAlt(_localctx, 3);
        setState(171);
        match(CSS3Parser::STRING);
        break;
      }

      case CSS3Parser::IDENT: {
        enterOuterAlt(_localctx, 4);
        setState(172);
        match(CSS3Parser::IDENT);
        break;
      }

      case CSS3Parser::HASH: {
        enterOuterAlt(_localctx, 5);
        setState(173);
        match(CSS3Parser::HASH);
        break;
      }

      case CSS3Parser::URI: {
        enterOuterAlt(_localctx, 6);
        setState(174);
        match(CSS3Parser::URI);
        break;
      }

      case CSS3Parser::FUNCTION: {
        enterOuterAlt(_localctx, 7);
        setState(175);
        match(CSS3Parser::FUNCTION);
        setState(179);
        _errHandler->sync(this);
        alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 19, _ctx);
        while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
          if (alt == 1) {
            setState(176);
            any_value(); 
          }
          setState(181);
          _errHandler->sync(this);
          alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 19, _ctx);
        }
        setState(182);
        match(CSS3Parser::RPAREN);
        break;
      }

      case CSS3Parser::LPAREN: {
        enterOuterAlt(_localctx, 8);
        setState(183);
        match(CSS3Parser::LPAREN);
        setState(187);
        _errHandler->sync(this);
        alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 20, _ctx);
        while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
          if (alt == 1) {
            setState(184);
            any_value(); 
          }
          setState(189);
          _errHandler->sync(this);
          alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 20, _ctx);
        }
        setState(190);
        match(CSS3Parser::RPAREN);
        break;
      }

      case CSS3Parser::LBRACKET: {
        enterOuterAlt(_localctx, 9);
        setState(191);
        match(CSS3Parser::LBRACKET);
        setState(195);
        _errHandler->sync(this);
        alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 21, _ctx);
        while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
          if (alt == 1) {
            setState(192);
            any_value(); 
          }
          setState(197);
          _errHandler->sync(this);
          alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 21, _ctx);
        }
        setState(198);
        match(CSS3Parser::RBRACKET);
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

//----------------- UnitContext ------------------------------------------------------------------

CSS3Parser::UnitContext::UnitContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSS3Parser::UnitContext::IDENT() {
  return getToken(CSS3Parser::IDENT, 0);
}


size_t CSS3Parser::UnitContext::getRuleIndex() const {
  return CSS3Parser::RuleUnit;
}

void CSS3Parser::UnitContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterUnit(this);
}

void CSS3Parser::UnitContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitUnit(this);
}

CSS3Parser::UnitContext* CSS3Parser::unit() {
  UnitContext *_localctx = _tracker.createInstance<UnitContext>(_ctx, getState());
  enterRule(_localctx, 40, CSS3Parser::RuleUnit);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(201);
    match(CSS3Parser::IDENT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Any_valueContext ------------------------------------------------------------------

CSS3Parser::Any_valueContext::Any_valueContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSS3Parser::Any_valueContext::NUMBER() {
  return getToken(CSS3Parser::NUMBER, 0);
}

CSS3Parser::UnitContext* CSS3Parser::Any_valueContext::unit() {
  return getRuleContext<CSS3Parser::UnitContext>(0);
}

tree::TerminalNode* CSS3Parser::Any_valueContext::PERCENTAGE() {
  return getToken(CSS3Parser::PERCENTAGE, 0);
}

tree::TerminalNode* CSS3Parser::Any_valueContext::STRING() {
  return getToken(CSS3Parser::STRING, 0);
}

tree::TerminalNode* CSS3Parser::Any_valueContext::IDENT() {
  return getToken(CSS3Parser::IDENT, 0);
}

tree::TerminalNode* CSS3Parser::Any_valueContext::HASH() {
  return getToken(CSS3Parser::HASH, 0);
}

tree::TerminalNode* CSS3Parser::Any_valueContext::URI() {
  return getToken(CSS3Parser::URI, 0);
}

tree::TerminalNode* CSS3Parser::Any_valueContext::FUNCTION() {
  return getToken(CSS3Parser::FUNCTION, 0);
}

tree::TerminalNode* CSS3Parser::Any_valueContext::LPAREN() {
  return getToken(CSS3Parser::LPAREN, 0);
}

tree::TerminalNode* CSS3Parser::Any_valueContext::RPAREN() {
  return getToken(CSS3Parser::RPAREN, 0);
}

tree::TerminalNode* CSS3Parser::Any_valueContext::LBRACKET() {
  return getToken(CSS3Parser::LBRACKET, 0);
}

tree::TerminalNode* CSS3Parser::Any_valueContext::RBRACKET() {
  return getToken(CSS3Parser::RBRACKET, 0);
}

tree::TerminalNode* CSS3Parser::Any_valueContext::LBRACE() {
  return getToken(CSS3Parser::LBRACE, 0);
}

tree::TerminalNode* CSS3Parser::Any_valueContext::RBRACE() {
  return getToken(CSS3Parser::RBRACE, 0);
}

tree::TerminalNode* CSS3Parser::Any_valueContext::SEMICOLON() {
  return getToken(CSS3Parser::SEMICOLON, 0);
}

tree::TerminalNode* CSS3Parser::Any_valueContext::COLON() {
  return getToken(CSS3Parser::COLON, 0);
}

tree::TerminalNode* CSS3Parser::Any_valueContext::COMMA() {
  return getToken(CSS3Parser::COMMA, 0);
}

tree::TerminalNode* CSS3Parser::Any_valueContext::PLUS() {
  return getToken(CSS3Parser::PLUS, 0);
}

tree::TerminalNode* CSS3Parser::Any_valueContext::MINUS() {
  return getToken(CSS3Parser::MINUS, 0);
}

tree::TerminalNode* CSS3Parser::Any_valueContext::STAR() {
  return getToken(CSS3Parser::STAR, 0);
}

tree::TerminalNode* CSS3Parser::Any_valueContext::SLASH() {
  return getToken(CSS3Parser::SLASH, 0);
}

tree::TerminalNode* CSS3Parser::Any_valueContext::EQUALS() {
  return getToken(CSS3Parser::EQUALS, 0);
}

tree::TerminalNode* CSS3Parser::Any_valueContext::DOT() {
  return getToken(CSS3Parser::DOT, 0);
}

tree::TerminalNode* CSS3Parser::Any_valueContext::GREATER() {
  return getToken(CSS3Parser::GREATER, 0);
}

tree::TerminalNode* CSS3Parser::Any_valueContext::TILDE() {
  return getToken(CSS3Parser::TILDE, 0);
}


size_t CSS3Parser::Any_valueContext::getRuleIndex() const {
  return CSS3Parser::RuleAny_value;
}

void CSS3Parser::Any_valueContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAny_value(this);
}

void CSS3Parser::Any_valueContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAny_value(this);
}

CSS3Parser::Any_valueContext* CSS3Parser::any_value() {
  Any_valueContext *_localctx = _tracker.createInstance<Any_valueContext>(_ctx, getState());
  enterRule(_localctx, 42, CSS3Parser::RuleAny_value);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(230);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CSS3Parser::NUMBER: {
        enterOuterAlt(_localctx, 1);
        setState(203);
        match(CSS3Parser::NUMBER);
        setState(205);
        _errHandler->sync(this);

        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 23, _ctx)) {
        case 1: {
          setState(204);
          unit();
          break;
        }

        default:
          break;
        }
        break;
      }

      case CSS3Parser::PERCENTAGE: {
        enterOuterAlt(_localctx, 2);
        setState(207);
        match(CSS3Parser::PERCENTAGE);
        break;
      }

      case CSS3Parser::STRING: {
        enterOuterAlt(_localctx, 3);
        setState(208);
        match(CSS3Parser::STRING);
        break;
      }

      case CSS3Parser::IDENT: {
        enterOuterAlt(_localctx, 4);
        setState(209);
        match(CSS3Parser::IDENT);
        break;
      }

      case CSS3Parser::HASH: {
        enterOuterAlt(_localctx, 5);
        setState(210);
        match(CSS3Parser::HASH);
        break;
      }

      case CSS3Parser::URI: {
        enterOuterAlt(_localctx, 6);
        setState(211);
        match(CSS3Parser::URI);
        break;
      }

      case CSS3Parser::FUNCTION: {
        enterOuterAlt(_localctx, 7);
        setState(212);
        match(CSS3Parser::FUNCTION);
        break;
      }

      case CSS3Parser::LPAREN: {
        enterOuterAlt(_localctx, 8);
        setState(213);
        match(CSS3Parser::LPAREN);
        break;
      }

      case CSS3Parser::RPAREN: {
        enterOuterAlt(_localctx, 9);
        setState(214);
        match(CSS3Parser::RPAREN);
        break;
      }

      case CSS3Parser::LBRACKET: {
        enterOuterAlt(_localctx, 10);
        setState(215);
        match(CSS3Parser::LBRACKET);
        break;
      }

      case CSS3Parser::RBRACKET: {
        enterOuterAlt(_localctx, 11);
        setState(216);
        match(CSS3Parser::RBRACKET);
        break;
      }

      case CSS3Parser::LBRACE: {
        enterOuterAlt(_localctx, 12);
        setState(217);
        match(CSS3Parser::LBRACE);
        break;
      }

      case CSS3Parser::RBRACE: {
        enterOuterAlt(_localctx, 13);
        setState(218);
        match(CSS3Parser::RBRACE);
        break;
      }

      case CSS3Parser::SEMICOLON: {
        enterOuterAlt(_localctx, 14);
        setState(219);
        match(CSS3Parser::SEMICOLON);
        break;
      }

      case CSS3Parser::COLON: {
        enterOuterAlt(_localctx, 15);
        setState(220);
        match(CSS3Parser::COLON);
        break;
      }

      case CSS3Parser::COMMA: {
        enterOuterAlt(_localctx, 16);
        setState(221);
        match(CSS3Parser::COMMA);
        break;
      }

      case CSS3Parser::PLUS: {
        enterOuterAlt(_localctx, 17);
        setState(222);
        match(CSS3Parser::PLUS);
        break;
      }

      case CSS3Parser::MINUS: {
        enterOuterAlt(_localctx, 18);
        setState(223);
        match(CSS3Parser::MINUS);
        break;
      }

      case CSS3Parser::STAR: {
        enterOuterAlt(_localctx, 19);
        setState(224);
        match(CSS3Parser::STAR);
        break;
      }

      case CSS3Parser::SLASH: {
        enterOuterAlt(_localctx, 20);
        setState(225);
        match(CSS3Parser::SLASH);
        break;
      }

      case CSS3Parser::EQUALS: {
        enterOuterAlt(_localctx, 21);
        setState(226);
        match(CSS3Parser::EQUALS);
        break;
      }

      case CSS3Parser::DOT: {
        enterOuterAlt(_localctx, 22);
        setState(227);
        match(CSS3Parser::DOT);
        break;
      }

      case CSS3Parser::GREATER: {
        enterOuterAlt(_localctx, 23);
        setState(228);
        match(CSS3Parser::GREATER);
        break;
      }

      case CSS3Parser::TILDE: {
        enterOuterAlt(_localctx, 24);
        setState(229);
        match(CSS3Parser::TILDE);
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

void CSS3Parser::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  css3ParserInitialize();
#else
  ::antlr4::internal::call_once(css3ParserOnceFlag, css3ParserInitialize);
#endif
}
