
// Generated from external/grammars/CSS3Parser.g4 by ANTLR 4.13.2


#include "CSS3ParserListener.h"

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

::antlr4::internal::OnceFlag css3parserParserOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
std::unique_ptr<CSS3ParserStaticData> css3parserParserStaticData = nullptr;

void css3parserParserInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (css3parserParserStaticData != nullptr) {
    return;
  }
#else
  assert(css3parserParserStaticData == nullptr);
#endif
  auto staticData = std::make_unique<CSS3ParserStaticData>(
    std::vector<std::string>{
      "stylesheet", "charset", "importRule", "mediaList", "statement", "atRule", 
      "block", "ruleset", "selector", "combinator", "simpleSelector", "elementName", 
      "attrib", "pseudo", "declaration", "property", "expr", "term", "function", 
      "hexcolor", "operator", "unaryOperator", "any"
    },
    std::vector<std::string>{
      "", "'['", "']'", "'('", "')'", "'{'", "'}'", "';'", "'='", "':'", 
      "'.'", "'*'", "'/'", "'|'", "'_'", "'+'", "'>'", "'~'", "','", "'-'", 
      "'only'", "'not'", "'and'", "'important'", "'from'", "'to'"
    },
    std::vector<std::string>{
      "", "OpenBracket", "CloseBracket", "OpenParen", "CloseParen", "OpenBrace", 
      "CloseBrace", "SemiColon", "Equal", "Colon", "Dot", "Multiply", "Divide", 
      "Pipe", "Underscore", "Plus", "Greater", "Tilde", "Comma", "Minus", 
      "Only", "Not", "And", "Important", "From", "To", "Comment", "Whitespace", 
      "Hash", "AtKeyword", "Ident", "String_", "Number", "Percentage", "Dimension", 
      "Uri", "UnicodeRange", "Any", "CHARSET", "STRING", "SEMICOLON", "IMPORT", 
      "URI", "IDENTIFIER", "COMMA", "ATKEYWORD", "LBRACE", "RBRACE", "PLUS", 
      "GREATER", "TILDE", "WS", "HASH", "CLASS", "STAR", "LBRACKET", "EQUALS", 
      "TILDE_EQUALS", "PIPE_EQUALS", "RBRACKET", "COLON", "IMPORTANT", "NUMBER", 
      "PERCENTAGE", "LENGTH", "EMS", "EXS", "ANGLE", "TIME", "FREQ", "FUNCTION", 
      "RPAREN", "SLASH", "MINUS", "UNICODE_RANGE", "LPAREN"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,75,256,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,2,22,7,22,1,0,1,0,1,0,5,0,50,8,0,10,0,12,0,53,9,0,1,0,1,0,1,1,1,1,
  	1,1,1,1,1,2,1,2,1,2,3,2,64,8,2,1,2,1,2,1,3,1,3,1,3,5,3,71,8,3,10,3,12,
  	3,74,9,3,1,4,1,4,3,4,78,8,4,1,5,1,5,5,5,82,8,5,10,5,12,5,85,9,5,1,5,1,
  	5,3,5,89,8,5,1,6,1,6,1,6,1,6,1,6,5,6,96,8,6,10,6,12,6,99,9,6,1,6,1,6,
  	1,7,1,7,1,7,5,7,106,8,7,10,7,12,7,109,9,7,1,7,1,7,3,7,113,8,7,1,7,1,7,
  	3,7,117,8,7,5,7,119,8,7,10,7,12,7,122,9,7,1,7,1,7,1,8,1,8,1,8,1,8,5,8,
  	130,8,8,10,8,12,8,133,9,8,1,9,1,9,1,10,1,10,1,10,1,10,1,10,5,10,142,8,
  	10,10,10,12,10,145,9,10,1,10,1,10,1,10,1,10,4,10,151,8,10,11,10,12,10,
  	152,3,10,155,8,10,1,11,1,11,1,12,1,12,1,12,1,12,3,12,163,8,12,1,12,1,
  	12,1,13,1,13,3,13,169,8,13,1,13,1,13,3,13,173,8,13,1,14,1,14,1,14,1,14,
  	3,14,179,8,14,1,15,1,15,1,16,1,16,3,16,185,8,16,1,16,5,16,188,8,16,10,
  	16,12,16,191,9,16,1,17,3,17,194,8,17,1,17,1,17,1,17,1,17,1,17,1,17,1,
  	17,1,17,1,17,1,17,1,17,1,17,1,17,3,17,209,8,17,1,18,1,18,1,18,1,18,1,
  	19,1,19,1,20,1,20,1,21,1,21,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,
  	22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,5,22,240,8,22,10,
  	22,12,22,243,9,22,1,22,1,22,1,22,5,22,248,8,22,10,22,12,22,251,9,22,1,
  	22,3,22,254,8,22,1,22,0,0,23,0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,
  	30,32,34,36,38,40,42,44,0,7,2,0,39,39,42,42,1,0,48,51,2,0,43,43,54,54,
  	1,0,56,58,2,0,39,39,43,43,2,0,44,44,72,72,2,0,48,48,73,73,297,0,51,1,
  	0,0,0,2,56,1,0,0,0,4,60,1,0,0,0,6,67,1,0,0,0,8,77,1,0,0,0,10,79,1,0,0,
  	0,12,90,1,0,0,0,14,102,1,0,0,0,16,125,1,0,0,0,18,134,1,0,0,0,20,154,1,
  	0,0,0,22,156,1,0,0,0,24,158,1,0,0,0,26,166,1,0,0,0,28,174,1,0,0,0,30,
  	180,1,0,0,0,32,182,1,0,0,0,34,193,1,0,0,0,36,210,1,0,0,0,38,214,1,0,0,
  	0,40,216,1,0,0,0,42,218,1,0,0,0,44,253,1,0,0,0,46,50,3,2,1,0,47,50,3,
  	4,2,0,48,50,3,8,4,0,49,46,1,0,0,0,49,47,1,0,0,0,49,48,1,0,0,0,50,53,1,
  	0,0,0,51,49,1,0,0,0,51,52,1,0,0,0,52,54,1,0,0,0,53,51,1,0,0,0,54,55,5,
  	0,0,1,55,1,1,0,0,0,56,57,5,38,0,0,57,58,5,39,0,0,58,59,5,40,0,0,59,3,
  	1,0,0,0,60,61,5,41,0,0,61,63,7,0,0,0,62,64,3,6,3,0,63,62,1,0,0,0,63,64,
  	1,0,0,0,64,65,1,0,0,0,65,66,5,40,0,0,66,5,1,0,0,0,67,72,5,43,0,0,68,69,
  	5,44,0,0,69,71,5,43,0,0,70,68,1,0,0,0,71,74,1,0,0,0,72,70,1,0,0,0,72,
  	73,1,0,0,0,73,7,1,0,0,0,74,72,1,0,0,0,75,78,3,14,7,0,76,78,3,10,5,0,77,
  	75,1,0,0,0,77,76,1,0,0,0,78,9,1,0,0,0,79,83,5,45,0,0,80,82,3,44,22,0,
  	81,80,1,0,0,0,82,85,1,0,0,0,83,81,1,0,0,0,83,84,1,0,0,0,84,88,1,0,0,0,
  	85,83,1,0,0,0,86,89,3,12,6,0,87,89,5,40,0,0,88,86,1,0,0,0,88,87,1,0,0,
  	0,89,11,1,0,0,0,90,97,5,46,0,0,91,96,3,44,22,0,92,96,3,12,6,0,93,96,5,
  	45,0,0,94,96,5,40,0,0,95,91,1,0,0,0,95,92,1,0,0,0,95,93,1,0,0,0,95,94,
  	1,0,0,0,96,99,1,0,0,0,97,95,1,0,0,0,97,98,1,0,0,0,98,100,1,0,0,0,99,97,
  	1,0,0,0,100,101,5,47,0,0,101,13,1,0,0,0,102,107,3,16,8,0,103,104,5,44,
  	0,0,104,106,3,16,8,0,105,103,1,0,0,0,106,109,1,0,0,0,107,105,1,0,0,0,
  	107,108,1,0,0,0,108,110,1,0,0,0,109,107,1,0,0,0,110,112,5,46,0,0,111,
  	113,3,28,14,0,112,111,1,0,0,0,112,113,1,0,0,0,113,120,1,0,0,0,114,116,
  	5,40,0,0,115,117,3,28,14,0,116,115,1,0,0,0,116,117,1,0,0,0,117,119,1,
  	0,0,0,118,114,1,0,0,0,119,122,1,0,0,0,120,118,1,0,0,0,120,121,1,0,0,0,
  	121,123,1,0,0,0,122,120,1,0,0,0,123,124,5,47,0,0,124,15,1,0,0,0,125,131,
  	3,20,10,0,126,127,3,18,9,0,127,128,3,20,10,0,128,130,1,0,0,0,129,126,
  	1,0,0,0,130,133,1,0,0,0,131,129,1,0,0,0,131,132,1,0,0,0,132,17,1,0,0,
  	0,133,131,1,0,0,0,134,135,7,1,0,0,135,19,1,0,0,0,136,143,3,22,11,0,137,
  	142,5,52,0,0,138,142,5,53,0,0,139,142,3,24,12,0,140,142,3,26,13,0,141,
  	137,1,0,0,0,141,138,1,0,0,0,141,139,1,0,0,0,141,140,1,0,0,0,142,145,1,
  	0,0,0,143,141,1,0,0,0,143,144,1,0,0,0,144,155,1,0,0,0,145,143,1,0,0,0,
  	146,151,5,52,0,0,147,151,5,53,0,0,148,151,3,24,12,0,149,151,3,26,13,0,
  	150,146,1,0,0,0,150,147,1,0,0,0,150,148,1,0,0,0,150,149,1,0,0,0,151,152,
  	1,0,0,0,152,150,1,0,0,0,152,153,1,0,0,0,153,155,1,0,0,0,154,136,1,0,0,
  	0,154,150,1,0,0,0,155,21,1,0,0,0,156,157,7,2,0,0,157,23,1,0,0,0,158,159,
  	5,55,0,0,159,162,5,43,0,0,160,161,7,3,0,0,161,163,7,4,0,0,162,160,1,0,
  	0,0,162,163,1,0,0,0,163,164,1,0,0,0,164,165,5,59,0,0,165,25,1,0,0,0,166,
  	168,5,60,0,0,167,169,5,60,0,0,168,167,1,0,0,0,168,169,1,0,0,0,169,172,
  	1,0,0,0,170,173,5,43,0,0,171,173,3,36,18,0,172,170,1,0,0,0,172,171,1,
  	0,0,0,173,27,1,0,0,0,174,175,3,30,15,0,175,176,5,60,0,0,176,178,3,32,
  	16,0,177,179,5,61,0,0,178,177,1,0,0,0,178,179,1,0,0,0,179,29,1,0,0,0,
  	180,181,5,43,0,0,181,31,1,0,0,0,182,189,3,34,17,0,183,185,3,40,20,0,184,
  	183,1,0,0,0,184,185,1,0,0,0,185,186,1,0,0,0,186,188,3,34,17,0,187,184,
  	1,0,0,0,188,191,1,0,0,0,189,187,1,0,0,0,189,190,1,0,0,0,190,33,1,0,0,
  	0,191,189,1,0,0,0,192,194,3,42,21,0,193,192,1,0,0,0,193,194,1,0,0,0,194,
  	208,1,0,0,0,195,209,5,62,0,0,196,209,5,63,0,0,197,209,5,64,0,0,198,209,
  	5,65,0,0,199,209,5,66,0,0,200,209,5,67,0,0,201,209,5,68,0,0,202,209,5,
  	69,0,0,203,209,5,39,0,0,204,209,5,43,0,0,205,209,5,42,0,0,206,209,3,38,
  	19,0,207,209,3,36,18,0,208,195,1,0,0,0,208,196,1,0,0,0,208,197,1,0,0,
  	0,208,198,1,0,0,0,208,199,1,0,0,0,208,200,1,0,0,0,208,201,1,0,0,0,208,
  	202,1,0,0,0,208,203,1,0,0,0,208,204,1,0,0,0,208,205,1,0,0,0,208,206,1,
  	0,0,0,208,207,1,0,0,0,209,35,1,0,0,0,210,211,5,70,0,0,211,212,3,32,16,
  	0,212,213,5,71,0,0,213,37,1,0,0,0,214,215,5,52,0,0,215,39,1,0,0,0,216,
  	217,7,5,0,0,217,41,1,0,0,0,218,219,7,6,0,0,219,43,1,0,0,0,220,254,5,43,
  	0,0,221,254,5,62,0,0,222,254,5,63,0,0,223,254,5,64,0,0,224,254,5,65,0,
  	0,225,254,5,66,0,0,226,254,5,67,0,0,227,254,5,68,0,0,228,254,5,69,0,0,
  	229,254,5,39,0,0,230,254,5,42,0,0,231,254,5,52,0,0,232,254,5,74,0,0,233,
  	254,5,57,0,0,234,254,5,58,0,0,235,254,5,60,0,0,236,254,5,70,0,0,237,241,
  	5,75,0,0,238,240,3,44,22,0,239,238,1,0,0,0,240,243,1,0,0,0,241,239,1,
  	0,0,0,241,242,1,0,0,0,242,244,1,0,0,0,243,241,1,0,0,0,244,254,5,71,0,
  	0,245,249,5,55,0,0,246,248,3,44,22,0,247,246,1,0,0,0,248,251,1,0,0,0,
  	249,247,1,0,0,0,249,250,1,0,0,0,250,252,1,0,0,0,251,249,1,0,0,0,252,254,
  	5,59,0,0,253,220,1,0,0,0,253,221,1,0,0,0,253,222,1,0,0,0,253,223,1,0,
  	0,0,253,224,1,0,0,0,253,225,1,0,0,0,253,226,1,0,0,0,253,227,1,0,0,0,253,
  	228,1,0,0,0,253,229,1,0,0,0,253,230,1,0,0,0,253,231,1,0,0,0,253,232,1,
  	0,0,0,253,233,1,0,0,0,253,234,1,0,0,0,253,235,1,0,0,0,253,236,1,0,0,0,
  	253,237,1,0,0,0,253,245,1,0,0,0,254,45,1,0,0,0,30,49,51,63,72,77,83,88,
  	95,97,107,112,116,120,131,141,143,150,152,154,162,168,172,178,184,189,
  	193,208,241,249,253
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  css3parserParserStaticData = std::move(staticData);
}

}

CSS3Parser::CSS3Parser(TokenStream *input) : CSS3Parser(input, antlr4::atn::ParserATNSimulatorOptions()) {}

CSS3Parser::CSS3Parser(TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options) : Parser(input) {
  CSS3Parser::initialize();
  _interpreter = new atn::ParserATNSimulator(this, *css3parserParserStaticData->atn, css3parserParserStaticData->decisionToDFA, css3parserParserStaticData->sharedContextCache, options);
}

CSS3Parser::~CSS3Parser() {
  delete _interpreter;
}

const atn::ATN& CSS3Parser::getATN() const {
  return *css3parserParserStaticData->atn;
}

std::string CSS3Parser::getGrammarFileName() const {
  return "CSS3Parser.g4";
}

const std::vector<std::string>& CSS3Parser::getRuleNames() const {
  return css3parserParserStaticData->ruleNames;
}

const dfa::Vocabulary& CSS3Parser::getVocabulary() const {
  return css3parserParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView CSS3Parser::getSerializedATN() const {
  return css3parserParserStaticData->serializedATN;
}


//----------------- StylesheetContext ------------------------------------------------------------------

CSS3Parser::StylesheetContext::StylesheetContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSS3Parser::StylesheetContext::EOF() {
  return getToken(CSS3Parser::EOF, 0);
}

std::vector<CSS3Parser::CharsetContext *> CSS3Parser::StylesheetContext::charset() {
  return getRuleContexts<CSS3Parser::CharsetContext>();
}

CSS3Parser::CharsetContext* CSS3Parser::StylesheetContext::charset(size_t i) {
  return getRuleContext<CSS3Parser::CharsetContext>(i);
}

std::vector<CSS3Parser::ImportRuleContext *> CSS3Parser::StylesheetContext::importRule() {
  return getRuleContexts<CSS3Parser::ImportRuleContext>();
}

CSS3Parser::ImportRuleContext* CSS3Parser::StylesheetContext::importRule(size_t i) {
  return getRuleContext<CSS3Parser::ImportRuleContext>(i);
}

std::vector<CSS3Parser::StatementContext *> CSS3Parser::StylesheetContext::statement() {
  return getRuleContexts<CSS3Parser::StatementContext>();
}

CSS3Parser::StatementContext* CSS3Parser::StylesheetContext::statement(size_t i) {
  return getRuleContext<CSS3Parser::StatementContext>(i);
}


size_t CSS3Parser::StylesheetContext::getRuleIndex() const {
  return CSS3Parser::RuleStylesheet;
}

void CSS3Parser::StylesheetContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStylesheet(this);
}

void CSS3Parser::StylesheetContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
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
    setState(51);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 1220521953383677952) != 0)) {
      setState(49);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case CSS3Parser::CHARSET: {
          setState(46);
          charset();
          break;
        }

        case CSS3Parser::IMPORT: {
          setState(47);
          importRule();
          break;
        }

        case CSS3Parser::IDENTIFIER:
        case CSS3Parser::ATKEYWORD:
        case CSS3Parser::HASH:
        case CSS3Parser::CLASS:
        case CSS3Parser::STAR:
        case CSS3Parser::LBRACKET:
        case CSS3Parser::COLON: {
          setState(48);
          statement();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(53);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(54);
    match(CSS3Parser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CharsetContext ------------------------------------------------------------------

CSS3Parser::CharsetContext::CharsetContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSS3Parser::CharsetContext::CHARSET() {
  return getToken(CSS3Parser::CHARSET, 0);
}

tree::TerminalNode* CSS3Parser::CharsetContext::STRING() {
  return getToken(CSS3Parser::STRING, 0);
}

tree::TerminalNode* CSS3Parser::CharsetContext::SEMICOLON() {
  return getToken(CSS3Parser::SEMICOLON, 0);
}


size_t CSS3Parser::CharsetContext::getRuleIndex() const {
  return CSS3Parser::RuleCharset;
}

void CSS3Parser::CharsetContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCharset(this);
}

void CSS3Parser::CharsetContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCharset(this);
}

CSS3Parser::CharsetContext* CSS3Parser::charset() {
  CharsetContext *_localctx = _tracker.createInstance<CharsetContext>(_ctx, getState());
  enterRule(_localctx, 2, CSS3Parser::RuleCharset);

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
    match(CSS3Parser::CHARSET);
    setState(57);
    match(CSS3Parser::STRING);
    setState(58);
    match(CSS3Parser::SEMICOLON);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ImportRuleContext ------------------------------------------------------------------

CSS3Parser::ImportRuleContext::ImportRuleContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSS3Parser::ImportRuleContext::IMPORT() {
  return getToken(CSS3Parser::IMPORT, 0);
}

tree::TerminalNode* CSS3Parser::ImportRuleContext::SEMICOLON() {
  return getToken(CSS3Parser::SEMICOLON, 0);
}

tree::TerminalNode* CSS3Parser::ImportRuleContext::STRING() {
  return getToken(CSS3Parser::STRING, 0);
}

tree::TerminalNode* CSS3Parser::ImportRuleContext::URI() {
  return getToken(CSS3Parser::URI, 0);
}

CSS3Parser::MediaListContext* CSS3Parser::ImportRuleContext::mediaList() {
  return getRuleContext<CSS3Parser::MediaListContext>(0);
}


size_t CSS3Parser::ImportRuleContext::getRuleIndex() const {
  return CSS3Parser::RuleImportRule;
}

void CSS3Parser::ImportRuleContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterImportRule(this);
}

void CSS3Parser::ImportRuleContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitImportRule(this);
}

CSS3Parser::ImportRuleContext* CSS3Parser::importRule() {
  ImportRuleContext *_localctx = _tracker.createInstance<ImportRuleContext>(_ctx, getState());
  enterRule(_localctx, 4, CSS3Parser::RuleImportRule);
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
    setState(60);
    match(CSS3Parser::IMPORT);
    setState(61);
    _la = _input->LA(1);
    if (!(_la == CSS3Parser::STRING

    || _la == CSS3Parser::URI)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(63);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == CSS3Parser::IDENTIFIER) {
      setState(62);
      mediaList();
    }
    setState(65);
    match(CSS3Parser::SEMICOLON);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- MediaListContext ------------------------------------------------------------------

CSS3Parser::MediaListContext::MediaListContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> CSS3Parser::MediaListContext::IDENTIFIER() {
  return getTokens(CSS3Parser::IDENTIFIER);
}

tree::TerminalNode* CSS3Parser::MediaListContext::IDENTIFIER(size_t i) {
  return getToken(CSS3Parser::IDENTIFIER, i);
}

std::vector<tree::TerminalNode *> CSS3Parser::MediaListContext::COMMA() {
  return getTokens(CSS3Parser::COMMA);
}

tree::TerminalNode* CSS3Parser::MediaListContext::COMMA(size_t i) {
  return getToken(CSS3Parser::COMMA, i);
}


size_t CSS3Parser::MediaListContext::getRuleIndex() const {
  return CSS3Parser::RuleMediaList;
}

void CSS3Parser::MediaListContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMediaList(this);
}

void CSS3Parser::MediaListContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMediaList(this);
}

CSS3Parser::MediaListContext* CSS3Parser::mediaList() {
  MediaListContext *_localctx = _tracker.createInstance<MediaListContext>(_ctx, getState());
  enterRule(_localctx, 6, CSS3Parser::RuleMediaList);
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
    match(CSS3Parser::IDENTIFIER);
    setState(72);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == CSS3Parser::COMMA) {
      setState(68);
      match(CSS3Parser::COMMA);
      setState(69);
      match(CSS3Parser::IDENTIFIER);
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

//----------------- StatementContext ------------------------------------------------------------------

CSS3Parser::StatementContext::StatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

CSS3Parser::RulesetContext* CSS3Parser::StatementContext::ruleset() {
  return getRuleContext<CSS3Parser::RulesetContext>(0);
}

CSS3Parser::AtRuleContext* CSS3Parser::StatementContext::atRule() {
  return getRuleContext<CSS3Parser::AtRuleContext>(0);
}


size_t CSS3Parser::StatementContext::getRuleIndex() const {
  return CSS3Parser::RuleStatement;
}

void CSS3Parser::StatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStatement(this);
}

void CSS3Parser::StatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStatement(this);
}

CSS3Parser::StatementContext* CSS3Parser::statement() {
  StatementContext *_localctx = _tracker.createInstance<StatementContext>(_ctx, getState());
  enterRule(_localctx, 8, CSS3Parser::RuleStatement);

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
      case CSS3Parser::IDENTIFIER:
      case CSS3Parser::HASH:
      case CSS3Parser::CLASS:
      case CSS3Parser::STAR:
      case CSS3Parser::LBRACKET:
      case CSS3Parser::COLON: {
        enterOuterAlt(_localctx, 1);
        setState(75);
        ruleset();
        break;
      }

      case CSS3Parser::ATKEYWORD: {
        enterOuterAlt(_localctx, 2);
        setState(76);
        atRule();
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

//----------------- AtRuleContext ------------------------------------------------------------------

CSS3Parser::AtRuleContext::AtRuleContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSS3Parser::AtRuleContext::ATKEYWORD() {
  return getToken(CSS3Parser::ATKEYWORD, 0);
}

CSS3Parser::BlockContext* CSS3Parser::AtRuleContext::block() {
  return getRuleContext<CSS3Parser::BlockContext>(0);
}

tree::TerminalNode* CSS3Parser::AtRuleContext::SEMICOLON() {
  return getToken(CSS3Parser::SEMICOLON, 0);
}

std::vector<CSS3Parser::AnyContext *> CSS3Parser::AtRuleContext::any() {
  return getRuleContexts<CSS3Parser::AnyContext>();
}

CSS3Parser::AnyContext* CSS3Parser::AtRuleContext::any(size_t i) {
  return getRuleContext<CSS3Parser::AnyContext>(i);
}


size_t CSS3Parser::AtRuleContext::getRuleIndex() const {
  return CSS3Parser::RuleAtRule;
}

void CSS3Parser::AtRuleContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAtRule(this);
}

void CSS3Parser::AtRuleContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAtRule(this);
}

CSS3Parser::AtRuleContext* CSS3Parser::atRule() {
  AtRuleContext *_localctx = _tracker.createInstance<AtRuleContext>(_ctx, getState());
  enterRule(_localctx, 10, CSS3Parser::RuleAtRule);
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
    setState(79);
    match(CSS3Parser::ATKEYWORD);
    setState(83);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (((((_la - 39) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 39)) & 107368751129) != 0)) {
      setState(80);
      any();
      setState(85);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(88);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CSS3Parser::LBRACE: {
        setState(86);
        block();
        break;
      }

      case CSS3Parser::SEMICOLON: {
        setState(87);
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

//----------------- BlockContext ------------------------------------------------------------------

CSS3Parser::BlockContext::BlockContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSS3Parser::BlockContext::LBRACE() {
  return getToken(CSS3Parser::LBRACE, 0);
}

tree::TerminalNode* CSS3Parser::BlockContext::RBRACE() {
  return getToken(CSS3Parser::RBRACE, 0);
}

std::vector<CSS3Parser::AnyContext *> CSS3Parser::BlockContext::any() {
  return getRuleContexts<CSS3Parser::AnyContext>();
}

CSS3Parser::AnyContext* CSS3Parser::BlockContext::any(size_t i) {
  return getRuleContext<CSS3Parser::AnyContext>(i);
}

std::vector<CSS3Parser::BlockContext *> CSS3Parser::BlockContext::block() {
  return getRuleContexts<CSS3Parser::BlockContext>();
}

CSS3Parser::BlockContext* CSS3Parser::BlockContext::block(size_t i) {
  return getRuleContext<CSS3Parser::BlockContext>(i);
}

std::vector<tree::TerminalNode *> CSS3Parser::BlockContext::ATKEYWORD() {
  return getTokens(CSS3Parser::ATKEYWORD);
}

tree::TerminalNode* CSS3Parser::BlockContext::ATKEYWORD(size_t i) {
  return getToken(CSS3Parser::ATKEYWORD, i);
}

std::vector<tree::TerminalNode *> CSS3Parser::BlockContext::SEMICOLON() {
  return getTokens(CSS3Parser::SEMICOLON);
}

tree::TerminalNode* CSS3Parser::BlockContext::SEMICOLON(size_t i) {
  return getToken(CSS3Parser::SEMICOLON, i);
}


size_t CSS3Parser::BlockContext::getRuleIndex() const {
  return CSS3Parser::RuleBlock;
}

void CSS3Parser::BlockContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBlock(this);
}

void CSS3Parser::BlockContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBlock(this);
}

CSS3Parser::BlockContext* CSS3Parser::block() {
  BlockContext *_localctx = _tracker.createInstance<BlockContext>(_ctx, getState());
  enterRule(_localctx, 12, CSS3Parser::RuleBlock);
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
    setState(90);
    match(CSS3Parser::LBRACE);
    setState(97);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (((((_la - 39) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 39)) & 107368751323) != 0)) {
      setState(95);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case CSS3Parser::STRING:
        case CSS3Parser::URI:
        case CSS3Parser::IDENTIFIER:
        case CSS3Parser::HASH:
        case CSS3Parser::LBRACKET:
        case CSS3Parser::TILDE_EQUALS:
        case CSS3Parser::PIPE_EQUALS:
        case CSS3Parser::COLON:
        case CSS3Parser::NUMBER:
        case CSS3Parser::PERCENTAGE:
        case CSS3Parser::LENGTH:
        case CSS3Parser::EMS:
        case CSS3Parser::EXS:
        case CSS3Parser::ANGLE:
        case CSS3Parser::TIME:
        case CSS3Parser::FREQ:
        case CSS3Parser::FUNCTION:
        case CSS3Parser::UNICODE_RANGE:
        case CSS3Parser::LPAREN: {
          setState(91);
          any();
          break;
        }

        case CSS3Parser::LBRACE: {
          setState(92);
          block();
          break;
        }

        case CSS3Parser::ATKEYWORD: {
          setState(93);
          match(CSS3Parser::ATKEYWORD);
          break;
        }

        case CSS3Parser::SEMICOLON: {
          setState(94);
          match(CSS3Parser::SEMICOLON);
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(99);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(100);
    match(CSS3Parser::RBRACE);
   
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

std::vector<CSS3Parser::SelectorContext *> CSS3Parser::RulesetContext::selector() {
  return getRuleContexts<CSS3Parser::SelectorContext>();
}

CSS3Parser::SelectorContext* CSS3Parser::RulesetContext::selector(size_t i) {
  return getRuleContext<CSS3Parser::SelectorContext>(i);
}

tree::TerminalNode* CSS3Parser::RulesetContext::LBRACE() {
  return getToken(CSS3Parser::LBRACE, 0);
}

tree::TerminalNode* CSS3Parser::RulesetContext::RBRACE() {
  return getToken(CSS3Parser::RBRACE, 0);
}

std::vector<tree::TerminalNode *> CSS3Parser::RulesetContext::COMMA() {
  return getTokens(CSS3Parser::COMMA);
}

tree::TerminalNode* CSS3Parser::RulesetContext::COMMA(size_t i) {
  return getToken(CSS3Parser::COMMA, i);
}

std::vector<CSS3Parser::DeclarationContext *> CSS3Parser::RulesetContext::declaration() {
  return getRuleContexts<CSS3Parser::DeclarationContext>();
}

CSS3Parser::DeclarationContext* CSS3Parser::RulesetContext::declaration(size_t i) {
  return getRuleContext<CSS3Parser::DeclarationContext>(i);
}

std::vector<tree::TerminalNode *> CSS3Parser::RulesetContext::SEMICOLON() {
  return getTokens(CSS3Parser::SEMICOLON);
}

tree::TerminalNode* CSS3Parser::RulesetContext::SEMICOLON(size_t i) {
  return getToken(CSS3Parser::SEMICOLON, i);
}


size_t CSS3Parser::RulesetContext::getRuleIndex() const {
  return CSS3Parser::RuleRuleset;
}

void CSS3Parser::RulesetContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterRuleset(this);
}

void CSS3Parser::RulesetContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitRuleset(this);
}

CSS3Parser::RulesetContext* CSS3Parser::ruleset() {
  RulesetContext *_localctx = _tracker.createInstance<RulesetContext>(_ctx, getState());
  enterRule(_localctx, 14, CSS3Parser::RuleRuleset);
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
    selector();
    setState(107);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == CSS3Parser::COMMA) {
      setState(103);
      match(CSS3Parser::COMMA);
      setState(104);
      selector();
      setState(109);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(110);
    match(CSS3Parser::LBRACE);
    setState(112);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == CSS3Parser::IDENTIFIER) {
      setState(111);
      declaration();
    }
    setState(120);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == CSS3Parser::SEMICOLON) {
      setState(114);
      match(CSS3Parser::SEMICOLON);
      setState(116);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == CSS3Parser::IDENTIFIER) {
        setState(115);
        declaration();
      }
      setState(122);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(123);
    match(CSS3Parser::RBRACE);
   
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
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSelector(this);
}

void CSS3Parser::SelectorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSelector(this);
}

CSS3Parser::SelectorContext* CSS3Parser::selector() {
  SelectorContext *_localctx = _tracker.createInstance<SelectorContext>(_ctx, getState());
  enterRule(_localctx, 16, CSS3Parser::RuleSelector);
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
    simpleSelector();
    setState(131);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 4222124650659840) != 0)) {
      setState(126);
      combinator();
      setState(127);
      simpleSelector();
      setState(133);
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
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCombinator(this);
}

void CSS3Parser::CombinatorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCombinator(this);
}

CSS3Parser::CombinatorContext* CSS3Parser::combinator() {
  CombinatorContext *_localctx = _tracker.createInstance<CombinatorContext>(_ctx, getState());
  enterRule(_localctx, 18, CSS3Parser::RuleCombinator);
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
    setState(134);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 4222124650659840) != 0))) {
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

std::vector<tree::TerminalNode *> CSS3Parser::SimpleSelectorContext::CLASS() {
  return getTokens(CSS3Parser::CLASS);
}

tree::TerminalNode* CSS3Parser::SimpleSelectorContext::CLASS(size_t i) {
  return getToken(CSS3Parser::CLASS, i);
}

std::vector<CSS3Parser::AttribContext *> CSS3Parser::SimpleSelectorContext::attrib() {
  return getRuleContexts<CSS3Parser::AttribContext>();
}

CSS3Parser::AttribContext* CSS3Parser::SimpleSelectorContext::attrib(size_t i) {
  return getRuleContext<CSS3Parser::AttribContext>(i);
}

std::vector<CSS3Parser::PseudoContext *> CSS3Parser::SimpleSelectorContext::pseudo() {
  return getRuleContexts<CSS3Parser::PseudoContext>();
}

CSS3Parser::PseudoContext* CSS3Parser::SimpleSelectorContext::pseudo(size_t i) {
  return getRuleContext<CSS3Parser::PseudoContext>(i);
}


size_t CSS3Parser::SimpleSelectorContext::getRuleIndex() const {
  return CSS3Parser::RuleSimpleSelector;
}

void CSS3Parser::SimpleSelectorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSimpleSelector(this);
}

void CSS3Parser::SimpleSelectorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSimpleSelector(this);
}

CSS3Parser::SimpleSelectorContext* CSS3Parser::simpleSelector() {
  SimpleSelectorContext *_localctx = _tracker.createInstance<SimpleSelectorContext>(_ctx, getState());
  enterRule(_localctx, 20, CSS3Parser::RuleSimpleSelector);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(154);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CSS3Parser::IDENTIFIER:
      case CSS3Parser::STAR: {
        enterOuterAlt(_localctx, 1);
        setState(136);
        elementName();
        setState(143);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & 1202461100507922432) != 0)) {
          setState(141);
          _errHandler->sync(this);
          switch (_input->LA(1)) {
            case CSS3Parser::HASH: {
              setState(137);
              match(CSS3Parser::HASH);
              break;
            }

            case CSS3Parser::CLASS: {
              setState(138);
              match(CSS3Parser::CLASS);
              break;
            }

            case CSS3Parser::LBRACKET: {
              setState(139);
              attrib();
              break;
            }

            case CSS3Parser::COLON: {
              setState(140);
              pseudo();
              break;
            }

          default:
            throw NoViableAltException(this);
          }
          setState(145);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        break;
      }

      case CSS3Parser::HASH:
      case CSS3Parser::CLASS:
      case CSS3Parser::LBRACKET:
      case CSS3Parser::COLON: {
        enterOuterAlt(_localctx, 2);
        setState(150); 
        _errHandler->sync(this);
        _la = _input->LA(1);
        do {
          setState(150);
          _errHandler->sync(this);
          switch (_input->LA(1)) {
            case CSS3Parser::HASH: {
              setState(146);
              match(CSS3Parser::HASH);
              break;
            }

            case CSS3Parser::CLASS: {
              setState(147);
              match(CSS3Parser::CLASS);
              break;
            }

            case CSS3Parser::LBRACKET: {
              setState(148);
              attrib();
              break;
            }

            case CSS3Parser::COLON: {
              setState(149);
              pseudo();
              break;
            }

          default:
            throw NoViableAltException(this);
          }
          setState(152); 
          _errHandler->sync(this);
          _la = _input->LA(1);
        } while ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & 1202461100507922432) != 0));
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

//----------------- ElementNameContext ------------------------------------------------------------------

CSS3Parser::ElementNameContext::ElementNameContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSS3Parser::ElementNameContext::IDENTIFIER() {
  return getToken(CSS3Parser::IDENTIFIER, 0);
}

tree::TerminalNode* CSS3Parser::ElementNameContext::STAR() {
  return getToken(CSS3Parser::STAR, 0);
}


size_t CSS3Parser::ElementNameContext::getRuleIndex() const {
  return CSS3Parser::RuleElementName;
}

void CSS3Parser::ElementNameContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterElementName(this);
}

void CSS3Parser::ElementNameContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitElementName(this);
}

CSS3Parser::ElementNameContext* CSS3Parser::elementName() {
  ElementNameContext *_localctx = _tracker.createInstance<ElementNameContext>(_ctx, getState());
  enterRule(_localctx, 22, CSS3Parser::RuleElementName);
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
    setState(156);
    _la = _input->LA(1);
    if (!(_la == CSS3Parser::IDENTIFIER

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

//----------------- AttribContext ------------------------------------------------------------------

CSS3Parser::AttribContext::AttribContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSS3Parser::AttribContext::LBRACKET() {
  return getToken(CSS3Parser::LBRACKET, 0);
}

std::vector<tree::TerminalNode *> CSS3Parser::AttribContext::IDENTIFIER() {
  return getTokens(CSS3Parser::IDENTIFIER);
}

tree::TerminalNode* CSS3Parser::AttribContext::IDENTIFIER(size_t i) {
  return getToken(CSS3Parser::IDENTIFIER, i);
}

tree::TerminalNode* CSS3Parser::AttribContext::RBRACKET() {
  return getToken(CSS3Parser::RBRACKET, 0);
}

tree::TerminalNode* CSS3Parser::AttribContext::EQUALS() {
  return getToken(CSS3Parser::EQUALS, 0);
}

tree::TerminalNode* CSS3Parser::AttribContext::TILDE_EQUALS() {
  return getToken(CSS3Parser::TILDE_EQUALS, 0);
}

tree::TerminalNode* CSS3Parser::AttribContext::PIPE_EQUALS() {
  return getToken(CSS3Parser::PIPE_EQUALS, 0);
}

tree::TerminalNode* CSS3Parser::AttribContext::STRING() {
  return getToken(CSS3Parser::STRING, 0);
}


size_t CSS3Parser::AttribContext::getRuleIndex() const {
  return CSS3Parser::RuleAttrib;
}

void CSS3Parser::AttribContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAttrib(this);
}

void CSS3Parser::AttribContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAttrib(this);
}

CSS3Parser::AttribContext* CSS3Parser::attrib() {
  AttribContext *_localctx = _tracker.createInstance<AttribContext>(_ctx, getState());
  enterRule(_localctx, 24, CSS3Parser::RuleAttrib);
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
    setState(158);
    match(CSS3Parser::LBRACKET);
    setState(159);
    match(CSS3Parser::IDENTIFIER);
    setState(162);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 504403158265495552) != 0)) {
      setState(160);
      _la = _input->LA(1);
      if (!((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 504403158265495552) != 0))) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(161);
      _la = _input->LA(1);
      if (!(_la == CSS3Parser::STRING

      || _la == CSS3Parser::IDENTIFIER)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
    }
    setState(164);
    match(CSS3Parser::RBRACKET);
   
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

tree::TerminalNode* CSS3Parser::PseudoContext::IDENTIFIER() {
  return getToken(CSS3Parser::IDENTIFIER, 0);
}

CSS3Parser::FunctionContext* CSS3Parser::PseudoContext::function() {
  return getRuleContext<CSS3Parser::FunctionContext>(0);
}


size_t CSS3Parser::PseudoContext::getRuleIndex() const {
  return CSS3Parser::RulePseudo;
}

void CSS3Parser::PseudoContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPseudo(this);
}

void CSS3Parser::PseudoContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPseudo(this);
}

CSS3Parser::PseudoContext* CSS3Parser::pseudo() {
  PseudoContext *_localctx = _tracker.createInstance<PseudoContext>(_ctx, getState());
  enterRule(_localctx, 26, CSS3Parser::RulePseudo);
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
    setState(166);
    match(CSS3Parser::COLON);
    setState(168);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == CSS3Parser::COLON) {
      setState(167);
      match(CSS3Parser::COLON);
    }
    setState(172);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CSS3Parser::IDENTIFIER: {
        setState(170);
        match(CSS3Parser::IDENTIFIER);
        break;
      }

      case CSS3Parser::FUNCTION: {
        setState(171);
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

tree::TerminalNode* CSS3Parser::DeclarationContext::IMPORTANT() {
  return getToken(CSS3Parser::IMPORTANT, 0);
}


size_t CSS3Parser::DeclarationContext::getRuleIndex() const {
  return CSS3Parser::RuleDeclaration;
}

void CSS3Parser::DeclarationContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterDeclaration(this);
}

void CSS3Parser::DeclarationContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitDeclaration(this);
}

CSS3Parser::DeclarationContext* CSS3Parser::declaration() {
  DeclarationContext *_localctx = _tracker.createInstance<DeclarationContext>(_ctx, getState());
  enterRule(_localctx, 28, CSS3Parser::RuleDeclaration);
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
    setState(174);
    property();
    setState(175);
    match(CSS3Parser::COLON);
    setState(176);
    expr();
    setState(178);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == CSS3Parser::IMPORTANT) {
      setState(177);
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

tree::TerminalNode* CSS3Parser::PropertyContext::IDENTIFIER() {
  return getToken(CSS3Parser::IDENTIFIER, 0);
}


size_t CSS3Parser::PropertyContext::getRuleIndex() const {
  return CSS3Parser::RuleProperty;
}

void CSS3Parser::PropertyContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterProperty(this);
}

void CSS3Parser::PropertyContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitProperty(this);
}

CSS3Parser::PropertyContext* CSS3Parser::property() {
  PropertyContext *_localctx = _tracker.createInstance<PropertyContext>(_ctx, getState());
  enterRule(_localctx, 30, CSS3Parser::RuleProperty);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(180);
    match(CSS3Parser::IDENTIFIER);
   
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
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterExpr(this);
}

void CSS3Parser::ExprContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitExpr(this);
}

CSS3Parser::ExprContext* CSS3Parser::expr() {
  ExprContext *_localctx = _tracker.createInstance<ExprContext>(_ctx, getState());
  enterRule(_localctx, 32, CSS3Parser::RuleExpr);
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
    setState(182);
    term();
    setState(189);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (((((_la - 39) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 39)) & 30056391225) != 0)) {
      setState(184);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == CSS3Parser::COMMA

      || _la == CSS3Parser::SLASH) {
        setState(183);
        operator_();
      }
      setState(186);
      term();
      setState(191);
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

tree::TerminalNode* CSS3Parser::TermContext::LENGTH() {
  return getToken(CSS3Parser::LENGTH, 0);
}

tree::TerminalNode* CSS3Parser::TermContext::EMS() {
  return getToken(CSS3Parser::EMS, 0);
}

tree::TerminalNode* CSS3Parser::TermContext::EXS() {
  return getToken(CSS3Parser::EXS, 0);
}

tree::TerminalNode* CSS3Parser::TermContext::ANGLE() {
  return getToken(CSS3Parser::ANGLE, 0);
}

tree::TerminalNode* CSS3Parser::TermContext::TIME() {
  return getToken(CSS3Parser::TIME, 0);
}

tree::TerminalNode* CSS3Parser::TermContext::FREQ() {
  return getToken(CSS3Parser::FREQ, 0);
}

tree::TerminalNode* CSS3Parser::TermContext::STRING() {
  return getToken(CSS3Parser::STRING, 0);
}

tree::TerminalNode* CSS3Parser::TermContext::IDENTIFIER() {
  return getToken(CSS3Parser::IDENTIFIER, 0);
}

tree::TerminalNode* CSS3Parser::TermContext::URI() {
  return getToken(CSS3Parser::URI, 0);
}

CSS3Parser::HexcolorContext* CSS3Parser::TermContext::hexcolor() {
  return getRuleContext<CSS3Parser::HexcolorContext>(0);
}

CSS3Parser::FunctionContext* CSS3Parser::TermContext::function() {
  return getRuleContext<CSS3Parser::FunctionContext>(0);
}

CSS3Parser::UnaryOperatorContext* CSS3Parser::TermContext::unaryOperator() {
  return getRuleContext<CSS3Parser::UnaryOperatorContext>(0);
}


size_t CSS3Parser::TermContext::getRuleIndex() const {
  return CSS3Parser::RuleTerm;
}

void CSS3Parser::TermContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTerm(this);
}

void CSS3Parser::TermContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTerm(this);
}

CSS3Parser::TermContext* CSS3Parser::term() {
  TermContext *_localctx = _tracker.createInstance<TermContext>(_ctx, getState());
  enterRule(_localctx, 34, CSS3Parser::RuleTerm);
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
    setState(193);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == CSS3Parser::PLUS

    || _la == CSS3Parser::MINUS) {
      setState(192);
      unaryOperator();
    }
    setState(208);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CSS3Parser::NUMBER: {
        setState(195);
        match(CSS3Parser::NUMBER);
        break;
      }

      case CSS3Parser::PERCENTAGE: {
        setState(196);
        match(CSS3Parser::PERCENTAGE);
        break;
      }

      case CSS3Parser::LENGTH: {
        setState(197);
        match(CSS3Parser::LENGTH);
        break;
      }

      case CSS3Parser::EMS: {
        setState(198);
        match(CSS3Parser::EMS);
        break;
      }

      case CSS3Parser::EXS: {
        setState(199);
        match(CSS3Parser::EXS);
        break;
      }

      case CSS3Parser::ANGLE: {
        setState(200);
        match(CSS3Parser::ANGLE);
        break;
      }

      case CSS3Parser::TIME: {
        setState(201);
        match(CSS3Parser::TIME);
        break;
      }

      case CSS3Parser::FREQ: {
        setState(202);
        match(CSS3Parser::FREQ);
        break;
      }

      case CSS3Parser::STRING: {
        setState(203);
        match(CSS3Parser::STRING);
        break;
      }

      case CSS3Parser::IDENTIFIER: {
        setState(204);
        match(CSS3Parser::IDENTIFIER);
        break;
      }

      case CSS3Parser::URI: {
        setState(205);
        match(CSS3Parser::URI);
        break;
      }

      case CSS3Parser::HASH: {
        setState(206);
        hexcolor();
        break;
      }

      case CSS3Parser::FUNCTION: {
        setState(207);
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

//----------------- FunctionContext ------------------------------------------------------------------

CSS3Parser::FunctionContext::FunctionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSS3Parser::FunctionContext::FUNCTION() {
  return getToken(CSS3Parser::FUNCTION, 0);
}

CSS3Parser::ExprContext* CSS3Parser::FunctionContext::expr() {
  return getRuleContext<CSS3Parser::ExprContext>(0);
}

tree::TerminalNode* CSS3Parser::FunctionContext::RPAREN() {
  return getToken(CSS3Parser::RPAREN, 0);
}


size_t CSS3Parser::FunctionContext::getRuleIndex() const {
  return CSS3Parser::RuleFunction;
}

void CSS3Parser::FunctionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFunction(this);
}

void CSS3Parser::FunctionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFunction(this);
}

CSS3Parser::FunctionContext* CSS3Parser::function() {
  FunctionContext *_localctx = _tracker.createInstance<FunctionContext>(_ctx, getState());
  enterRule(_localctx, 36, CSS3Parser::RuleFunction);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(210);
    match(CSS3Parser::FUNCTION);
    setState(211);
    expr();
    setState(212);
    match(CSS3Parser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- HexcolorContext ------------------------------------------------------------------

CSS3Parser::HexcolorContext::HexcolorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSS3Parser::HexcolorContext::HASH() {
  return getToken(CSS3Parser::HASH, 0);
}


size_t CSS3Parser::HexcolorContext::getRuleIndex() const {
  return CSS3Parser::RuleHexcolor;
}

void CSS3Parser::HexcolorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterHexcolor(this);
}

void CSS3Parser::HexcolorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitHexcolor(this);
}

CSS3Parser::HexcolorContext* CSS3Parser::hexcolor() {
  HexcolorContext *_localctx = _tracker.createInstance<HexcolorContext>(_ctx, getState());
  enterRule(_localctx, 38, CSS3Parser::RuleHexcolor);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(214);
    match(CSS3Parser::HASH);
   
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

tree::TerminalNode* CSS3Parser::OperatorContext::SLASH() {
  return getToken(CSS3Parser::SLASH, 0);
}

tree::TerminalNode* CSS3Parser::OperatorContext::COMMA() {
  return getToken(CSS3Parser::COMMA, 0);
}


size_t CSS3Parser::OperatorContext::getRuleIndex() const {
  return CSS3Parser::RuleOperator;
}

void CSS3Parser::OperatorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterOperator(this);
}

void CSS3Parser::OperatorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitOperator(this);
}

CSS3Parser::OperatorContext* CSS3Parser::operator_() {
  OperatorContext *_localctx = _tracker.createInstance<OperatorContext>(_ctx, getState());
  enterRule(_localctx, 40, CSS3Parser::RuleOperator);
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
    setState(216);
    _la = _input->LA(1);
    if (!(_la == CSS3Parser::COMMA

    || _la == CSS3Parser::SLASH)) {
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

//----------------- UnaryOperatorContext ------------------------------------------------------------------

CSS3Parser::UnaryOperatorContext::UnaryOperatorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSS3Parser::UnaryOperatorContext::MINUS() {
  return getToken(CSS3Parser::MINUS, 0);
}

tree::TerminalNode* CSS3Parser::UnaryOperatorContext::PLUS() {
  return getToken(CSS3Parser::PLUS, 0);
}


size_t CSS3Parser::UnaryOperatorContext::getRuleIndex() const {
  return CSS3Parser::RuleUnaryOperator;
}

void CSS3Parser::UnaryOperatorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterUnaryOperator(this);
}

void CSS3Parser::UnaryOperatorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitUnaryOperator(this);
}

CSS3Parser::UnaryOperatorContext* CSS3Parser::unaryOperator() {
  UnaryOperatorContext *_localctx = _tracker.createInstance<UnaryOperatorContext>(_ctx, getState());
  enterRule(_localctx, 42, CSS3Parser::RuleUnaryOperator);
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
    setState(218);
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

//----------------- AnyContext ------------------------------------------------------------------

CSS3Parser::AnyContext::AnyContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSS3Parser::AnyContext::IDENTIFIER() {
  return getToken(CSS3Parser::IDENTIFIER, 0);
}

tree::TerminalNode* CSS3Parser::AnyContext::NUMBER() {
  return getToken(CSS3Parser::NUMBER, 0);
}

tree::TerminalNode* CSS3Parser::AnyContext::PERCENTAGE() {
  return getToken(CSS3Parser::PERCENTAGE, 0);
}

tree::TerminalNode* CSS3Parser::AnyContext::LENGTH() {
  return getToken(CSS3Parser::LENGTH, 0);
}

tree::TerminalNode* CSS3Parser::AnyContext::EMS() {
  return getToken(CSS3Parser::EMS, 0);
}

tree::TerminalNode* CSS3Parser::AnyContext::EXS() {
  return getToken(CSS3Parser::EXS, 0);
}

tree::TerminalNode* CSS3Parser::AnyContext::ANGLE() {
  return getToken(CSS3Parser::ANGLE, 0);
}

tree::TerminalNode* CSS3Parser::AnyContext::TIME() {
  return getToken(CSS3Parser::TIME, 0);
}

tree::TerminalNode* CSS3Parser::AnyContext::FREQ() {
  return getToken(CSS3Parser::FREQ, 0);
}

tree::TerminalNode* CSS3Parser::AnyContext::STRING() {
  return getToken(CSS3Parser::STRING, 0);
}

tree::TerminalNode* CSS3Parser::AnyContext::URI() {
  return getToken(CSS3Parser::URI, 0);
}

tree::TerminalNode* CSS3Parser::AnyContext::HASH() {
  return getToken(CSS3Parser::HASH, 0);
}

tree::TerminalNode* CSS3Parser::AnyContext::UNICODE_RANGE() {
  return getToken(CSS3Parser::UNICODE_RANGE, 0);
}

tree::TerminalNode* CSS3Parser::AnyContext::TILDE_EQUALS() {
  return getToken(CSS3Parser::TILDE_EQUALS, 0);
}

tree::TerminalNode* CSS3Parser::AnyContext::PIPE_EQUALS() {
  return getToken(CSS3Parser::PIPE_EQUALS, 0);
}

tree::TerminalNode* CSS3Parser::AnyContext::COLON() {
  return getToken(CSS3Parser::COLON, 0);
}

tree::TerminalNode* CSS3Parser::AnyContext::FUNCTION() {
  return getToken(CSS3Parser::FUNCTION, 0);
}

tree::TerminalNode* CSS3Parser::AnyContext::LPAREN() {
  return getToken(CSS3Parser::LPAREN, 0);
}

tree::TerminalNode* CSS3Parser::AnyContext::RPAREN() {
  return getToken(CSS3Parser::RPAREN, 0);
}

std::vector<CSS3Parser::AnyContext *> CSS3Parser::AnyContext::any() {
  return getRuleContexts<CSS3Parser::AnyContext>();
}

CSS3Parser::AnyContext* CSS3Parser::AnyContext::any(size_t i) {
  return getRuleContext<CSS3Parser::AnyContext>(i);
}

tree::TerminalNode* CSS3Parser::AnyContext::LBRACKET() {
  return getToken(CSS3Parser::LBRACKET, 0);
}

tree::TerminalNode* CSS3Parser::AnyContext::RBRACKET() {
  return getToken(CSS3Parser::RBRACKET, 0);
}


size_t CSS3Parser::AnyContext::getRuleIndex() const {
  return CSS3Parser::RuleAny;
}

void CSS3Parser::AnyContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAny(this);
}

void CSS3Parser::AnyContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSS3ParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAny(this);
}

CSS3Parser::AnyContext* CSS3Parser::any() {
  AnyContext *_localctx = _tracker.createInstance<AnyContext>(_ctx, getState());
  enterRule(_localctx, 44, CSS3Parser::RuleAny);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(253);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CSS3Parser::IDENTIFIER: {
        enterOuterAlt(_localctx, 1);
        setState(220);
        match(CSS3Parser::IDENTIFIER);
        break;
      }

      case CSS3Parser::NUMBER: {
        enterOuterAlt(_localctx, 2);
        setState(221);
        match(CSS3Parser::NUMBER);
        break;
      }

      case CSS3Parser::PERCENTAGE: {
        enterOuterAlt(_localctx, 3);
        setState(222);
        match(CSS3Parser::PERCENTAGE);
        break;
      }

      case CSS3Parser::LENGTH: {
        enterOuterAlt(_localctx, 4);
        setState(223);
        match(CSS3Parser::LENGTH);
        break;
      }

      case CSS3Parser::EMS: {
        enterOuterAlt(_localctx, 5);
        setState(224);
        match(CSS3Parser::EMS);
        break;
      }

      case CSS3Parser::EXS: {
        enterOuterAlt(_localctx, 6);
        setState(225);
        match(CSS3Parser::EXS);
        break;
      }

      case CSS3Parser::ANGLE: {
        enterOuterAlt(_localctx, 7);
        setState(226);
        match(CSS3Parser::ANGLE);
        break;
      }

      case CSS3Parser::TIME: {
        enterOuterAlt(_localctx, 8);
        setState(227);
        match(CSS3Parser::TIME);
        break;
      }

      case CSS3Parser::FREQ: {
        enterOuterAlt(_localctx, 9);
        setState(228);
        match(CSS3Parser::FREQ);
        break;
      }

      case CSS3Parser::STRING: {
        enterOuterAlt(_localctx, 10);
        setState(229);
        match(CSS3Parser::STRING);
        break;
      }

      case CSS3Parser::URI: {
        enterOuterAlt(_localctx, 11);
        setState(230);
        match(CSS3Parser::URI);
        break;
      }

      case CSS3Parser::HASH: {
        enterOuterAlt(_localctx, 12);
        setState(231);
        match(CSS3Parser::HASH);
        break;
      }

      case CSS3Parser::UNICODE_RANGE: {
        enterOuterAlt(_localctx, 13);
        setState(232);
        match(CSS3Parser::UNICODE_RANGE);
        break;
      }

      case CSS3Parser::TILDE_EQUALS: {
        enterOuterAlt(_localctx, 14);
        setState(233);
        match(CSS3Parser::TILDE_EQUALS);
        break;
      }

      case CSS3Parser::PIPE_EQUALS: {
        enterOuterAlt(_localctx, 15);
        setState(234);
        match(CSS3Parser::PIPE_EQUALS);
        break;
      }

      case CSS3Parser::COLON: {
        enterOuterAlt(_localctx, 16);
        setState(235);
        match(CSS3Parser::COLON);
        break;
      }

      case CSS3Parser::FUNCTION: {
        enterOuterAlt(_localctx, 17);
        setState(236);
        match(CSS3Parser::FUNCTION);
        break;
      }

      case CSS3Parser::LPAREN: {
        enterOuterAlt(_localctx, 18);
        setState(237);
        match(CSS3Parser::LPAREN);
        setState(241);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while (((((_la - 39) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 39)) & 107368751129) != 0)) {
          setState(238);
          any();
          setState(243);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        setState(244);
        match(CSS3Parser::RPAREN);
        break;
      }

      case CSS3Parser::LBRACKET: {
        enterOuterAlt(_localctx, 19);
        setState(245);
        match(CSS3Parser::LBRACKET);
        setState(249);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while (((((_la - 39) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 39)) & 107368751129) != 0)) {
          setState(246);
          any();
          setState(251);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        setState(252);
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

void CSS3Parser::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  css3parserParserInitialize();
#else
  ::antlr4::internal::call_once(css3parserParserOnceFlag, css3parserParserInitialize);
#endif
}
