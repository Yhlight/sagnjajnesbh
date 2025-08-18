
// Generated from external/grammars/JavaScript_improved.g4 by ANTLR 4.13.2


#include "JavaScript_improvedListener.h"

#include "JavaScript_improvedParser.h"


using namespace antlrcpp;

using namespace antlr4;

namespace {

struct JavaScript_improvedParserStaticData final {
  JavaScript_improvedParserStaticData(std::vector<std::string> ruleNames,
                        std::vector<std::string> literalNames,
                        std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  JavaScript_improvedParserStaticData(const JavaScript_improvedParserStaticData&) = delete;
  JavaScript_improvedParserStaticData(JavaScript_improvedParserStaticData&&) = delete;
  JavaScript_improvedParserStaticData& operator=(const JavaScript_improvedParserStaticData&) = delete;
  JavaScript_improvedParserStaticData& operator=(JavaScript_improvedParserStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag javascript_improvedParserOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
std::unique_ptr<JavaScript_improvedParserStaticData> javascript_improvedParserStaticData = nullptr;

void javascript_improvedParserInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (javascript_improvedParserStaticData != nullptr) {
    return;
  }
#else
  assert(javascript_improvedParserStaticData == nullptr);
#endif
  auto staticData = std::make_unique<JavaScript_improvedParserStaticData>(
    std::vector<std::string>{
      "program", "statement", "blockStatement", "variableStatement", "variableDeclaration", 
      "functionDeclaration", "parameterList", "expressionStatement", "ifStatement", 
      "whileStatement", "forStatement", "returnStatement", "expression", 
      "assignmentExpression", "conditionalExpression", "logicalOrExpression", 
      "logicalAndExpression", "equalityExpression", "relationalExpression", 
      "additiveExpression", "multiplicativeExpression", "unaryExpression", 
      "postfixExpression", "primaryExpression", "arrayLiteral", "objectLiteral", 
      "propertyAssignment", "argumentList"
    },
    std::vector<std::string>{
      "", "'='", "'||'", "'&&'", "'=='", "'!='", "'==='", "'!=='", "'<'", 
      "'>'", "'<='", "'>='", "'+'", "'-'", "'*'", "'/'", "'%'", "'++'", 
      "'--'", "'!'", "'~'", "", "", "", "'break'", "'case'", "'catch'", 
      "'class'", "'const'", "'continue'", "'debugger'", "'default'", "'delete'", 
      "'do'", "'else'", "'export'", "'extends'", "'finally'", "'for'", "'function'", 
      "'if'", "'import'", "'in'", "'instanceof'", "'let'", "'new'", "'return'", 
      "'super'", "'switch'", "'this'", "'throw'", "'try'", "'typeof'", "'var'", 
      "'void'", "'while'", "'with'", "'yield'", "", "'null'", "'undefined'", 
      "", "", "", "", "'['", "']'", "'('", "')'", "'{'", "'}'", "';'", "','", 
      "'.'", "'\\u003F'", "':'"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "MultiLineComment", "SingleLineComment", "WhiteSpace", 
      "Break", "Case", "Catch", "Class", "Const", "Continue", "Debugger", 
      "Default", "Delete", "Do", "Else", "Export", "Extends", "Finally", 
      "For", "Function", "If", "Import", "In", "Instanceof", "Let", "New", 
      "Return", "Super", "Switch", "This", "Throw", "Try", "Typeof", "Var", 
      "Void", "While", "With", "Yield", "BooleanLiteral", "NullLiteral", 
      "UndefinedLiteral", "NumericLiteral", "StringLiteral", "Identifier", 
      "AssignmentOperator", "OpenBracket", "CloseBracket", "OpenParen", 
      "CloseParen", "OpenBrace", "CloseBrace", "SemiColon", "Comma", "Dot", 
      "Question", "Colon", "Literal", "Any"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,77,307,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,27,7,27,1,0,5,
  	0,58,8,0,10,0,12,0,61,9,0,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  	3,1,74,8,1,1,2,1,2,5,2,78,8,2,10,2,12,2,81,9,2,1,2,1,2,1,3,1,3,1,3,1,
  	3,5,3,89,8,3,10,3,12,3,92,9,3,1,3,3,3,95,8,3,1,4,1,4,1,4,3,4,100,8,4,
  	1,5,1,5,1,5,1,5,3,5,106,8,5,1,5,1,5,1,5,1,6,1,6,1,6,5,6,114,8,6,10,6,
  	12,6,117,9,6,1,7,1,7,3,7,121,8,7,1,8,1,8,1,8,1,8,1,8,1,8,1,8,3,8,130,
  	8,8,1,9,1,9,1,9,1,9,1,9,1,9,1,10,1,10,1,10,1,10,3,10,142,8,10,1,10,1,
  	10,3,10,146,8,10,1,10,1,10,3,10,150,8,10,1,10,1,10,1,10,1,11,1,11,3,11,
  	157,8,11,1,11,3,11,160,8,11,1,12,1,12,1,13,1,13,1,13,3,13,167,8,13,1,
  	14,1,14,1,14,1,14,1,14,1,14,3,14,175,8,14,1,15,1,15,1,15,5,15,180,8,15,
  	10,15,12,15,183,9,15,1,16,1,16,1,16,5,16,188,8,16,10,16,12,16,191,9,16,
  	1,17,1,17,1,17,5,17,196,8,17,10,17,12,17,199,9,17,1,18,1,18,1,18,5,18,
  	204,8,18,10,18,12,18,207,9,18,1,19,1,19,1,19,5,19,212,8,19,10,19,12,19,
  	215,9,19,1,20,1,20,1,20,5,20,220,8,20,10,20,12,20,223,9,20,1,21,1,21,
  	1,21,3,21,228,8,21,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,
  	1,22,3,22,241,8,22,1,22,3,22,244,8,22,5,22,246,8,22,10,22,12,22,249,9,
  	22,1,23,1,23,1,23,1,23,1,23,1,23,1,23,1,23,1,23,3,23,260,8,23,1,24,1,
  	24,1,24,1,24,5,24,266,8,24,10,24,12,24,269,9,24,3,24,271,8,24,1,24,1,
  	24,1,25,1,25,1,25,1,25,5,25,279,8,25,10,25,12,25,282,9,25,3,25,284,8,
  	25,1,25,1,25,1,26,1,26,1,26,1,26,1,26,1,26,1,26,1,26,1,26,3,26,297,8,
  	26,1,27,1,27,1,27,5,27,302,8,27,10,27,12,27,305,9,27,1,27,0,0,28,0,2,
  	4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,
  	52,54,0,6,3,0,28,28,44,44,53,53,1,0,4,7,1,0,8,11,1,0,12,13,1,0,14,16,
  	2,0,12,13,17,20,328,0,59,1,0,0,0,2,73,1,0,0,0,4,75,1,0,0,0,6,84,1,0,0,
  	0,8,96,1,0,0,0,10,101,1,0,0,0,12,110,1,0,0,0,14,118,1,0,0,0,16,122,1,
  	0,0,0,18,131,1,0,0,0,20,137,1,0,0,0,22,154,1,0,0,0,24,161,1,0,0,0,26,
  	163,1,0,0,0,28,168,1,0,0,0,30,176,1,0,0,0,32,184,1,0,0,0,34,192,1,0,0,
  	0,36,200,1,0,0,0,38,208,1,0,0,0,40,216,1,0,0,0,42,227,1,0,0,0,44,229,
  	1,0,0,0,46,259,1,0,0,0,48,261,1,0,0,0,50,274,1,0,0,0,52,296,1,0,0,0,54,
  	298,1,0,0,0,56,58,3,2,1,0,57,56,1,0,0,0,58,61,1,0,0,0,59,57,1,0,0,0,59,
  	60,1,0,0,0,60,62,1,0,0,0,61,59,1,0,0,0,62,63,5,0,0,1,63,1,1,0,0,0,64,
  	74,3,6,3,0,65,74,3,14,7,0,66,74,3,16,8,0,67,74,3,18,9,0,68,74,3,20,10,
  	0,69,74,3,10,5,0,70,74,3,22,11,0,71,74,3,4,2,0,72,74,5,71,0,0,73,64,1,
  	0,0,0,73,65,1,0,0,0,73,66,1,0,0,0,73,67,1,0,0,0,73,68,1,0,0,0,73,69,1,
  	0,0,0,73,70,1,0,0,0,73,71,1,0,0,0,73,72,1,0,0,0,74,3,1,0,0,0,75,79,5,
  	69,0,0,76,78,3,2,1,0,77,76,1,0,0,0,78,81,1,0,0,0,79,77,1,0,0,0,79,80,
  	1,0,0,0,80,82,1,0,0,0,81,79,1,0,0,0,82,83,5,70,0,0,83,5,1,0,0,0,84,85,
  	7,0,0,0,85,90,3,8,4,0,86,87,5,72,0,0,87,89,3,8,4,0,88,86,1,0,0,0,89,92,
  	1,0,0,0,90,88,1,0,0,0,90,91,1,0,0,0,91,94,1,0,0,0,92,90,1,0,0,0,93,95,
  	5,71,0,0,94,93,1,0,0,0,94,95,1,0,0,0,95,7,1,0,0,0,96,99,5,63,0,0,97,98,
  	5,1,0,0,98,100,3,24,12,0,99,97,1,0,0,0,99,100,1,0,0,0,100,9,1,0,0,0,101,
  	102,5,39,0,0,102,103,5,63,0,0,103,105,5,67,0,0,104,106,3,12,6,0,105,104,
  	1,0,0,0,105,106,1,0,0,0,106,107,1,0,0,0,107,108,5,68,0,0,108,109,3,4,
  	2,0,109,11,1,0,0,0,110,115,5,63,0,0,111,112,5,72,0,0,112,114,5,63,0,0,
  	113,111,1,0,0,0,114,117,1,0,0,0,115,113,1,0,0,0,115,116,1,0,0,0,116,13,
  	1,0,0,0,117,115,1,0,0,0,118,120,3,24,12,0,119,121,5,71,0,0,120,119,1,
  	0,0,0,120,121,1,0,0,0,121,15,1,0,0,0,122,123,5,40,0,0,123,124,5,67,0,
  	0,124,125,3,24,12,0,125,126,5,68,0,0,126,129,3,2,1,0,127,128,5,34,0,0,
  	128,130,3,2,1,0,129,127,1,0,0,0,129,130,1,0,0,0,130,17,1,0,0,0,131,132,
  	5,55,0,0,132,133,5,67,0,0,133,134,3,24,12,0,134,135,5,68,0,0,135,136,
  	3,2,1,0,136,19,1,0,0,0,137,138,5,38,0,0,138,141,5,67,0,0,139,142,3,6,
  	3,0,140,142,3,24,12,0,141,139,1,0,0,0,141,140,1,0,0,0,141,142,1,0,0,0,
  	142,143,1,0,0,0,143,145,5,71,0,0,144,146,3,24,12,0,145,144,1,0,0,0,145,
  	146,1,0,0,0,146,147,1,0,0,0,147,149,5,71,0,0,148,150,3,24,12,0,149,148,
  	1,0,0,0,149,150,1,0,0,0,150,151,1,0,0,0,151,152,5,68,0,0,152,153,3,2,
  	1,0,153,21,1,0,0,0,154,156,5,46,0,0,155,157,3,24,12,0,156,155,1,0,0,0,
  	156,157,1,0,0,0,157,159,1,0,0,0,158,160,5,71,0,0,159,158,1,0,0,0,159,
  	160,1,0,0,0,160,23,1,0,0,0,161,162,3,26,13,0,162,25,1,0,0,0,163,166,3,
  	28,14,0,164,165,5,64,0,0,165,167,3,26,13,0,166,164,1,0,0,0,166,167,1,
  	0,0,0,167,27,1,0,0,0,168,174,3,30,15,0,169,170,5,74,0,0,170,171,3,26,
  	13,0,171,172,5,75,0,0,172,173,3,28,14,0,173,175,1,0,0,0,174,169,1,0,0,
  	0,174,175,1,0,0,0,175,29,1,0,0,0,176,181,3,32,16,0,177,178,5,2,0,0,178,
  	180,3,32,16,0,179,177,1,0,0,0,180,183,1,0,0,0,181,179,1,0,0,0,181,182,
  	1,0,0,0,182,31,1,0,0,0,183,181,1,0,0,0,184,189,3,34,17,0,185,186,5,3,
  	0,0,186,188,3,34,17,0,187,185,1,0,0,0,188,191,1,0,0,0,189,187,1,0,0,0,
  	189,190,1,0,0,0,190,33,1,0,0,0,191,189,1,0,0,0,192,197,3,36,18,0,193,
  	194,7,1,0,0,194,196,3,36,18,0,195,193,1,0,0,0,196,199,1,0,0,0,197,195,
  	1,0,0,0,197,198,1,0,0,0,198,35,1,0,0,0,199,197,1,0,0,0,200,205,3,38,19,
  	0,201,202,7,2,0,0,202,204,3,38,19,0,203,201,1,0,0,0,204,207,1,0,0,0,205,
  	203,1,0,0,0,205,206,1,0,0,0,206,37,1,0,0,0,207,205,1,0,0,0,208,213,3,
  	40,20,0,209,210,7,3,0,0,210,212,3,40,20,0,211,209,1,0,0,0,212,215,1,0,
  	0,0,213,211,1,0,0,0,213,214,1,0,0,0,214,39,1,0,0,0,215,213,1,0,0,0,216,
  	221,3,42,21,0,217,218,7,4,0,0,218,220,3,42,21,0,219,217,1,0,0,0,220,223,
  	1,0,0,0,221,219,1,0,0,0,221,222,1,0,0,0,222,41,1,0,0,0,223,221,1,0,0,
  	0,224,228,3,44,22,0,225,226,7,5,0,0,226,228,3,42,21,0,227,224,1,0,0,0,
  	227,225,1,0,0,0,228,43,1,0,0,0,229,247,3,46,23,0,230,244,5,17,0,0,231,
  	244,5,18,0,0,232,233,5,65,0,0,233,234,3,24,12,0,234,235,5,66,0,0,235,
  	244,1,0,0,0,236,237,5,73,0,0,237,244,5,63,0,0,238,240,5,67,0,0,239,241,
  	3,54,27,0,240,239,1,0,0,0,240,241,1,0,0,0,241,242,1,0,0,0,242,244,5,68,
  	0,0,243,230,1,0,0,0,243,231,1,0,0,0,243,232,1,0,0,0,243,236,1,0,0,0,243,
  	238,1,0,0,0,244,246,1,0,0,0,245,243,1,0,0,0,246,249,1,0,0,0,247,245,1,
  	0,0,0,247,248,1,0,0,0,248,45,1,0,0,0,249,247,1,0,0,0,250,260,5,49,0,0,
  	251,260,5,63,0,0,252,260,5,76,0,0,253,254,5,67,0,0,254,255,3,24,12,0,
  	255,256,5,68,0,0,256,260,1,0,0,0,257,260,3,48,24,0,258,260,3,50,25,0,
  	259,250,1,0,0,0,259,251,1,0,0,0,259,252,1,0,0,0,259,253,1,0,0,0,259,257,
  	1,0,0,0,259,258,1,0,0,0,260,47,1,0,0,0,261,270,5,65,0,0,262,267,3,24,
  	12,0,263,264,5,72,0,0,264,266,3,24,12,0,265,263,1,0,0,0,266,269,1,0,0,
  	0,267,265,1,0,0,0,267,268,1,0,0,0,268,271,1,0,0,0,269,267,1,0,0,0,270,
  	262,1,0,0,0,270,271,1,0,0,0,271,272,1,0,0,0,272,273,5,66,0,0,273,49,1,
  	0,0,0,274,283,5,69,0,0,275,280,3,52,26,0,276,277,5,72,0,0,277,279,3,52,
  	26,0,278,276,1,0,0,0,279,282,1,0,0,0,280,278,1,0,0,0,280,281,1,0,0,0,
  	281,284,1,0,0,0,282,280,1,0,0,0,283,275,1,0,0,0,283,284,1,0,0,0,284,285,
  	1,0,0,0,285,286,5,70,0,0,286,51,1,0,0,0,287,288,5,63,0,0,288,289,5,75,
  	0,0,289,297,3,24,12,0,290,291,5,62,0,0,291,292,5,75,0,0,292,297,3,24,
  	12,0,293,294,5,61,0,0,294,295,5,75,0,0,295,297,3,24,12,0,296,287,1,0,
  	0,0,296,290,1,0,0,0,296,293,1,0,0,0,297,53,1,0,0,0,298,303,3,24,12,0,
  	299,300,5,72,0,0,300,302,3,24,12,0,301,299,1,0,0,0,302,305,1,0,0,0,303,
  	301,1,0,0,0,303,304,1,0,0,0,304,55,1,0,0,0,305,303,1,0,0,0,34,59,73,79,
  	90,94,99,105,115,120,129,141,145,149,156,159,166,174,181,189,197,205,
  	213,221,227,240,243,247,259,267,270,280,283,296,303
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  javascript_improvedParserStaticData = std::move(staticData);
}

}

JavaScript_improvedParser::JavaScript_improvedParser(TokenStream *input) : JavaScript_improvedParser(input, antlr4::atn::ParserATNSimulatorOptions()) {}

JavaScript_improvedParser::JavaScript_improvedParser(TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options) : Parser(input) {
  JavaScript_improvedParser::initialize();
  _interpreter = new atn::ParserATNSimulator(this, *javascript_improvedParserStaticData->atn, javascript_improvedParserStaticData->decisionToDFA, javascript_improvedParserStaticData->sharedContextCache, options);
}

JavaScript_improvedParser::~JavaScript_improvedParser() {
  delete _interpreter;
}

const atn::ATN& JavaScript_improvedParser::getATN() const {
  return *javascript_improvedParserStaticData->atn;
}

std::string JavaScript_improvedParser::getGrammarFileName() const {
  return "JavaScript_improved.g4";
}

const std::vector<std::string>& JavaScript_improvedParser::getRuleNames() const {
  return javascript_improvedParserStaticData->ruleNames;
}

const dfa::Vocabulary& JavaScript_improvedParser::getVocabulary() const {
  return javascript_improvedParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView JavaScript_improvedParser::getSerializedATN() const {
  return javascript_improvedParserStaticData->serializedATN;
}


//----------------- ProgramContext ------------------------------------------------------------------

JavaScript_improvedParser::ProgramContext::ProgramContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScript_improvedParser::ProgramContext::EOF() {
  return getToken(JavaScript_improvedParser::EOF, 0);
}

std::vector<JavaScript_improvedParser::StatementContext *> JavaScript_improvedParser::ProgramContext::statement() {
  return getRuleContexts<JavaScript_improvedParser::StatementContext>();
}

JavaScript_improvedParser::StatementContext* JavaScript_improvedParser::ProgramContext::statement(size_t i) {
  return getRuleContext<JavaScript_improvedParser::StatementContext>(i);
}


size_t JavaScript_improvedParser::ProgramContext::getRuleIndex() const {
  return JavaScript_improvedParser::RuleProgram;
}

void JavaScript_improvedParser::ProgramContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterProgram(this);
}

void JavaScript_improvedParser::ProgramContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitProgram(this);
}

JavaScript_improvedParser::ProgramContext* JavaScript_improvedParser::program() {
  ProgramContext *_localctx = _tracker.createInstance<ProgramContext>(_ctx, getState());
  enterRule(_localctx, 0, JavaScript_improvedParser::RuleProgram);
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
    setState(59);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & -9177683205281665024) != 0) || ((((_la - 65) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 65)) & 2133) != 0)) {
      setState(56);
      statement();
      setState(61);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(62);
    match(JavaScript_improvedParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StatementContext ------------------------------------------------------------------

JavaScript_improvedParser::StatementContext::StatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

JavaScript_improvedParser::VariableStatementContext* JavaScript_improvedParser::StatementContext::variableStatement() {
  return getRuleContext<JavaScript_improvedParser::VariableStatementContext>(0);
}

JavaScript_improvedParser::ExpressionStatementContext* JavaScript_improvedParser::StatementContext::expressionStatement() {
  return getRuleContext<JavaScript_improvedParser::ExpressionStatementContext>(0);
}

JavaScript_improvedParser::IfStatementContext* JavaScript_improvedParser::StatementContext::ifStatement() {
  return getRuleContext<JavaScript_improvedParser::IfStatementContext>(0);
}

JavaScript_improvedParser::WhileStatementContext* JavaScript_improvedParser::StatementContext::whileStatement() {
  return getRuleContext<JavaScript_improvedParser::WhileStatementContext>(0);
}

JavaScript_improvedParser::ForStatementContext* JavaScript_improvedParser::StatementContext::forStatement() {
  return getRuleContext<JavaScript_improvedParser::ForStatementContext>(0);
}

JavaScript_improvedParser::FunctionDeclarationContext* JavaScript_improvedParser::StatementContext::functionDeclaration() {
  return getRuleContext<JavaScript_improvedParser::FunctionDeclarationContext>(0);
}

JavaScript_improvedParser::ReturnStatementContext* JavaScript_improvedParser::StatementContext::returnStatement() {
  return getRuleContext<JavaScript_improvedParser::ReturnStatementContext>(0);
}

JavaScript_improvedParser::BlockStatementContext* JavaScript_improvedParser::StatementContext::blockStatement() {
  return getRuleContext<JavaScript_improvedParser::BlockStatementContext>(0);
}

tree::TerminalNode* JavaScript_improvedParser::StatementContext::SemiColon() {
  return getToken(JavaScript_improvedParser::SemiColon, 0);
}


size_t JavaScript_improvedParser::StatementContext::getRuleIndex() const {
  return JavaScript_improvedParser::RuleStatement;
}

void JavaScript_improvedParser::StatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStatement(this);
}

void JavaScript_improvedParser::StatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStatement(this);
}

JavaScript_improvedParser::StatementContext* JavaScript_improvedParser::statement() {
  StatementContext *_localctx = _tracker.createInstance<StatementContext>(_ctx, getState());
  enterRule(_localctx, 2, JavaScript_improvedParser::RuleStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(73);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 1, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(64);
      variableStatement();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(65);
      expressionStatement();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(66);
      ifStatement();
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(67);
      whileStatement();
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(68);
      forStatement();
      break;
    }

    case 6: {
      enterOuterAlt(_localctx, 6);
      setState(69);
      functionDeclaration();
      break;
    }

    case 7: {
      enterOuterAlt(_localctx, 7);
      setState(70);
      returnStatement();
      break;
    }

    case 8: {
      enterOuterAlt(_localctx, 8);
      setState(71);
      blockStatement();
      break;
    }

    case 9: {
      enterOuterAlt(_localctx, 9);
      setState(72);
      match(JavaScript_improvedParser::SemiColon);
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

//----------------- BlockStatementContext ------------------------------------------------------------------

JavaScript_improvedParser::BlockStatementContext::BlockStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScript_improvedParser::BlockStatementContext::OpenBrace() {
  return getToken(JavaScript_improvedParser::OpenBrace, 0);
}

tree::TerminalNode* JavaScript_improvedParser::BlockStatementContext::CloseBrace() {
  return getToken(JavaScript_improvedParser::CloseBrace, 0);
}

std::vector<JavaScript_improvedParser::StatementContext *> JavaScript_improvedParser::BlockStatementContext::statement() {
  return getRuleContexts<JavaScript_improvedParser::StatementContext>();
}

JavaScript_improvedParser::StatementContext* JavaScript_improvedParser::BlockStatementContext::statement(size_t i) {
  return getRuleContext<JavaScript_improvedParser::StatementContext>(i);
}


size_t JavaScript_improvedParser::BlockStatementContext::getRuleIndex() const {
  return JavaScript_improvedParser::RuleBlockStatement;
}

void JavaScript_improvedParser::BlockStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBlockStatement(this);
}

void JavaScript_improvedParser::BlockStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBlockStatement(this);
}

JavaScript_improvedParser::BlockStatementContext* JavaScript_improvedParser::blockStatement() {
  BlockStatementContext *_localctx = _tracker.createInstance<BlockStatementContext>(_ctx, getState());
  enterRule(_localctx, 4, JavaScript_improvedParser::RuleBlockStatement);
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
    match(JavaScript_improvedParser::OpenBrace);
    setState(79);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & -9177683205281665024) != 0) || ((((_la - 65) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 65)) & 2133) != 0)) {
      setState(76);
      statement();
      setState(81);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(82);
    match(JavaScript_improvedParser::CloseBrace);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- VariableStatementContext ------------------------------------------------------------------

JavaScript_improvedParser::VariableStatementContext::VariableStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<JavaScript_improvedParser::VariableDeclarationContext *> JavaScript_improvedParser::VariableStatementContext::variableDeclaration() {
  return getRuleContexts<JavaScript_improvedParser::VariableDeclarationContext>();
}

JavaScript_improvedParser::VariableDeclarationContext* JavaScript_improvedParser::VariableStatementContext::variableDeclaration(size_t i) {
  return getRuleContext<JavaScript_improvedParser::VariableDeclarationContext>(i);
}

tree::TerminalNode* JavaScript_improvedParser::VariableStatementContext::Var() {
  return getToken(JavaScript_improvedParser::Var, 0);
}

tree::TerminalNode* JavaScript_improvedParser::VariableStatementContext::Let() {
  return getToken(JavaScript_improvedParser::Let, 0);
}

tree::TerminalNode* JavaScript_improvedParser::VariableStatementContext::Const() {
  return getToken(JavaScript_improvedParser::Const, 0);
}

std::vector<tree::TerminalNode *> JavaScript_improvedParser::VariableStatementContext::Comma() {
  return getTokens(JavaScript_improvedParser::Comma);
}

tree::TerminalNode* JavaScript_improvedParser::VariableStatementContext::Comma(size_t i) {
  return getToken(JavaScript_improvedParser::Comma, i);
}

tree::TerminalNode* JavaScript_improvedParser::VariableStatementContext::SemiColon() {
  return getToken(JavaScript_improvedParser::SemiColon, 0);
}


size_t JavaScript_improvedParser::VariableStatementContext::getRuleIndex() const {
  return JavaScript_improvedParser::RuleVariableStatement;
}

void JavaScript_improvedParser::VariableStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterVariableStatement(this);
}

void JavaScript_improvedParser::VariableStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitVariableStatement(this);
}

JavaScript_improvedParser::VariableStatementContext* JavaScript_improvedParser::variableStatement() {
  VariableStatementContext *_localctx = _tracker.createInstance<VariableStatementContext>(_ctx, getState());
  enterRule(_localctx, 6, JavaScript_improvedParser::RuleVariableStatement);
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
      ((1ULL << _la) & 9024791709220864) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(85);
    variableDeclaration();
    setState(90);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == JavaScript_improvedParser::Comma) {
      setState(86);
      match(JavaScript_improvedParser::Comma);
      setState(87);
      variableDeclaration();
      setState(92);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(94);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 4, _ctx)) {
    case 1: {
      setState(93);
      match(JavaScript_improvedParser::SemiColon);
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

//----------------- VariableDeclarationContext ------------------------------------------------------------------

JavaScript_improvedParser::VariableDeclarationContext::VariableDeclarationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScript_improvedParser::VariableDeclarationContext::Identifier() {
  return getToken(JavaScript_improvedParser::Identifier, 0);
}

JavaScript_improvedParser::ExpressionContext* JavaScript_improvedParser::VariableDeclarationContext::expression() {
  return getRuleContext<JavaScript_improvedParser::ExpressionContext>(0);
}


size_t JavaScript_improvedParser::VariableDeclarationContext::getRuleIndex() const {
  return JavaScript_improvedParser::RuleVariableDeclaration;
}

void JavaScript_improvedParser::VariableDeclarationContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterVariableDeclaration(this);
}

void JavaScript_improvedParser::VariableDeclarationContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitVariableDeclaration(this);
}

JavaScript_improvedParser::VariableDeclarationContext* JavaScript_improvedParser::variableDeclaration() {
  VariableDeclarationContext *_localctx = _tracker.createInstance<VariableDeclarationContext>(_ctx, getState());
  enterRule(_localctx, 8, JavaScript_improvedParser::RuleVariableDeclaration);
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
    setState(96);
    match(JavaScript_improvedParser::Identifier);
    setState(99);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == JavaScript_improvedParser::T__0) {
      setState(97);
      match(JavaScript_improvedParser::T__0);
      setState(98);
      expression();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FunctionDeclarationContext ------------------------------------------------------------------

JavaScript_improvedParser::FunctionDeclarationContext::FunctionDeclarationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScript_improvedParser::FunctionDeclarationContext::Function() {
  return getToken(JavaScript_improvedParser::Function, 0);
}

tree::TerminalNode* JavaScript_improvedParser::FunctionDeclarationContext::Identifier() {
  return getToken(JavaScript_improvedParser::Identifier, 0);
}

tree::TerminalNode* JavaScript_improvedParser::FunctionDeclarationContext::OpenParen() {
  return getToken(JavaScript_improvedParser::OpenParen, 0);
}

tree::TerminalNode* JavaScript_improvedParser::FunctionDeclarationContext::CloseParen() {
  return getToken(JavaScript_improvedParser::CloseParen, 0);
}

JavaScript_improvedParser::BlockStatementContext* JavaScript_improvedParser::FunctionDeclarationContext::blockStatement() {
  return getRuleContext<JavaScript_improvedParser::BlockStatementContext>(0);
}

JavaScript_improvedParser::ParameterListContext* JavaScript_improvedParser::FunctionDeclarationContext::parameterList() {
  return getRuleContext<JavaScript_improvedParser::ParameterListContext>(0);
}


size_t JavaScript_improvedParser::FunctionDeclarationContext::getRuleIndex() const {
  return JavaScript_improvedParser::RuleFunctionDeclaration;
}

void JavaScript_improvedParser::FunctionDeclarationContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFunctionDeclaration(this);
}

void JavaScript_improvedParser::FunctionDeclarationContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFunctionDeclaration(this);
}

JavaScript_improvedParser::FunctionDeclarationContext* JavaScript_improvedParser::functionDeclaration() {
  FunctionDeclarationContext *_localctx = _tracker.createInstance<FunctionDeclarationContext>(_ctx, getState());
  enterRule(_localctx, 10, JavaScript_improvedParser::RuleFunctionDeclaration);
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
    setState(101);
    match(JavaScript_improvedParser::Function);
    setState(102);
    match(JavaScript_improvedParser::Identifier);
    setState(103);
    match(JavaScript_improvedParser::OpenParen);
    setState(105);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == JavaScript_improvedParser::Identifier) {
      setState(104);
      parameterList();
    }
    setState(107);
    match(JavaScript_improvedParser::CloseParen);
    setState(108);
    blockStatement();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ParameterListContext ------------------------------------------------------------------

JavaScript_improvedParser::ParameterListContext::ParameterListContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> JavaScript_improvedParser::ParameterListContext::Identifier() {
  return getTokens(JavaScript_improvedParser::Identifier);
}

tree::TerminalNode* JavaScript_improvedParser::ParameterListContext::Identifier(size_t i) {
  return getToken(JavaScript_improvedParser::Identifier, i);
}

std::vector<tree::TerminalNode *> JavaScript_improvedParser::ParameterListContext::Comma() {
  return getTokens(JavaScript_improvedParser::Comma);
}

tree::TerminalNode* JavaScript_improvedParser::ParameterListContext::Comma(size_t i) {
  return getToken(JavaScript_improvedParser::Comma, i);
}


size_t JavaScript_improvedParser::ParameterListContext::getRuleIndex() const {
  return JavaScript_improvedParser::RuleParameterList;
}

void JavaScript_improvedParser::ParameterListContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterParameterList(this);
}

void JavaScript_improvedParser::ParameterListContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitParameterList(this);
}

JavaScript_improvedParser::ParameterListContext* JavaScript_improvedParser::parameterList() {
  ParameterListContext *_localctx = _tracker.createInstance<ParameterListContext>(_ctx, getState());
  enterRule(_localctx, 12, JavaScript_improvedParser::RuleParameterList);
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
    setState(110);
    match(JavaScript_improvedParser::Identifier);
    setState(115);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == JavaScript_improvedParser::Comma) {
      setState(111);
      match(JavaScript_improvedParser::Comma);
      setState(112);
      match(JavaScript_improvedParser::Identifier);
      setState(117);
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

//----------------- ExpressionStatementContext ------------------------------------------------------------------

JavaScript_improvedParser::ExpressionStatementContext::ExpressionStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

JavaScript_improvedParser::ExpressionContext* JavaScript_improvedParser::ExpressionStatementContext::expression() {
  return getRuleContext<JavaScript_improvedParser::ExpressionContext>(0);
}

tree::TerminalNode* JavaScript_improvedParser::ExpressionStatementContext::SemiColon() {
  return getToken(JavaScript_improvedParser::SemiColon, 0);
}


size_t JavaScript_improvedParser::ExpressionStatementContext::getRuleIndex() const {
  return JavaScript_improvedParser::RuleExpressionStatement;
}

void JavaScript_improvedParser::ExpressionStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterExpressionStatement(this);
}

void JavaScript_improvedParser::ExpressionStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitExpressionStatement(this);
}

JavaScript_improvedParser::ExpressionStatementContext* JavaScript_improvedParser::expressionStatement() {
  ExpressionStatementContext *_localctx = _tracker.createInstance<ExpressionStatementContext>(_ctx, getState());
  enterRule(_localctx, 14, JavaScript_improvedParser::RuleExpressionStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(118);
    expression();
    setState(120);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 8, _ctx)) {
    case 1: {
      setState(119);
      match(JavaScript_improvedParser::SemiColon);
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

//----------------- IfStatementContext ------------------------------------------------------------------

JavaScript_improvedParser::IfStatementContext::IfStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScript_improvedParser::IfStatementContext::If() {
  return getToken(JavaScript_improvedParser::If, 0);
}

tree::TerminalNode* JavaScript_improvedParser::IfStatementContext::OpenParen() {
  return getToken(JavaScript_improvedParser::OpenParen, 0);
}

JavaScript_improvedParser::ExpressionContext* JavaScript_improvedParser::IfStatementContext::expression() {
  return getRuleContext<JavaScript_improvedParser::ExpressionContext>(0);
}

tree::TerminalNode* JavaScript_improvedParser::IfStatementContext::CloseParen() {
  return getToken(JavaScript_improvedParser::CloseParen, 0);
}

std::vector<JavaScript_improvedParser::StatementContext *> JavaScript_improvedParser::IfStatementContext::statement() {
  return getRuleContexts<JavaScript_improvedParser::StatementContext>();
}

JavaScript_improvedParser::StatementContext* JavaScript_improvedParser::IfStatementContext::statement(size_t i) {
  return getRuleContext<JavaScript_improvedParser::StatementContext>(i);
}

tree::TerminalNode* JavaScript_improvedParser::IfStatementContext::Else() {
  return getToken(JavaScript_improvedParser::Else, 0);
}


size_t JavaScript_improvedParser::IfStatementContext::getRuleIndex() const {
  return JavaScript_improvedParser::RuleIfStatement;
}

void JavaScript_improvedParser::IfStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterIfStatement(this);
}

void JavaScript_improvedParser::IfStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitIfStatement(this);
}

JavaScript_improvedParser::IfStatementContext* JavaScript_improvedParser::ifStatement() {
  IfStatementContext *_localctx = _tracker.createInstance<IfStatementContext>(_ctx, getState());
  enterRule(_localctx, 16, JavaScript_improvedParser::RuleIfStatement);

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
    match(JavaScript_improvedParser::If);
    setState(123);
    match(JavaScript_improvedParser::OpenParen);
    setState(124);
    expression();
    setState(125);
    match(JavaScript_improvedParser::CloseParen);
    setState(126);
    statement();
    setState(129);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 9, _ctx)) {
    case 1: {
      setState(127);
      match(JavaScript_improvedParser::Else);
      setState(128);
      statement();
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

//----------------- WhileStatementContext ------------------------------------------------------------------

JavaScript_improvedParser::WhileStatementContext::WhileStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScript_improvedParser::WhileStatementContext::While() {
  return getToken(JavaScript_improvedParser::While, 0);
}

tree::TerminalNode* JavaScript_improvedParser::WhileStatementContext::OpenParen() {
  return getToken(JavaScript_improvedParser::OpenParen, 0);
}

JavaScript_improvedParser::ExpressionContext* JavaScript_improvedParser::WhileStatementContext::expression() {
  return getRuleContext<JavaScript_improvedParser::ExpressionContext>(0);
}

tree::TerminalNode* JavaScript_improvedParser::WhileStatementContext::CloseParen() {
  return getToken(JavaScript_improvedParser::CloseParen, 0);
}

JavaScript_improvedParser::StatementContext* JavaScript_improvedParser::WhileStatementContext::statement() {
  return getRuleContext<JavaScript_improvedParser::StatementContext>(0);
}


size_t JavaScript_improvedParser::WhileStatementContext::getRuleIndex() const {
  return JavaScript_improvedParser::RuleWhileStatement;
}

void JavaScript_improvedParser::WhileStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterWhileStatement(this);
}

void JavaScript_improvedParser::WhileStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitWhileStatement(this);
}

JavaScript_improvedParser::WhileStatementContext* JavaScript_improvedParser::whileStatement() {
  WhileStatementContext *_localctx = _tracker.createInstance<WhileStatementContext>(_ctx, getState());
  enterRule(_localctx, 18, JavaScript_improvedParser::RuleWhileStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(131);
    match(JavaScript_improvedParser::While);
    setState(132);
    match(JavaScript_improvedParser::OpenParen);
    setState(133);
    expression();
    setState(134);
    match(JavaScript_improvedParser::CloseParen);
    setState(135);
    statement();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ForStatementContext ------------------------------------------------------------------

JavaScript_improvedParser::ForStatementContext::ForStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScript_improvedParser::ForStatementContext::For() {
  return getToken(JavaScript_improvedParser::For, 0);
}

tree::TerminalNode* JavaScript_improvedParser::ForStatementContext::OpenParen() {
  return getToken(JavaScript_improvedParser::OpenParen, 0);
}

std::vector<tree::TerminalNode *> JavaScript_improvedParser::ForStatementContext::SemiColon() {
  return getTokens(JavaScript_improvedParser::SemiColon);
}

tree::TerminalNode* JavaScript_improvedParser::ForStatementContext::SemiColon(size_t i) {
  return getToken(JavaScript_improvedParser::SemiColon, i);
}

tree::TerminalNode* JavaScript_improvedParser::ForStatementContext::CloseParen() {
  return getToken(JavaScript_improvedParser::CloseParen, 0);
}

JavaScript_improvedParser::StatementContext* JavaScript_improvedParser::ForStatementContext::statement() {
  return getRuleContext<JavaScript_improvedParser::StatementContext>(0);
}

JavaScript_improvedParser::VariableStatementContext* JavaScript_improvedParser::ForStatementContext::variableStatement() {
  return getRuleContext<JavaScript_improvedParser::VariableStatementContext>(0);
}

std::vector<JavaScript_improvedParser::ExpressionContext *> JavaScript_improvedParser::ForStatementContext::expression() {
  return getRuleContexts<JavaScript_improvedParser::ExpressionContext>();
}

JavaScript_improvedParser::ExpressionContext* JavaScript_improvedParser::ForStatementContext::expression(size_t i) {
  return getRuleContext<JavaScript_improvedParser::ExpressionContext>(i);
}


size_t JavaScript_improvedParser::ForStatementContext::getRuleIndex() const {
  return JavaScript_improvedParser::RuleForStatement;
}

void JavaScript_improvedParser::ForStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterForStatement(this);
}

void JavaScript_improvedParser::ForStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitForStatement(this);
}

JavaScript_improvedParser::ForStatementContext* JavaScript_improvedParser::forStatement() {
  ForStatementContext *_localctx = _tracker.createInstance<ForStatementContext>(_ctx, getState());
  enterRule(_localctx, 20, JavaScript_improvedParser::RuleForStatement);
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
    match(JavaScript_improvedParser::For);
    setState(138);
    match(JavaScript_improvedParser::OpenParen);
    setState(141);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case JavaScript_improvedParser::Const:
      case JavaScript_improvedParser::Let:
      case JavaScript_improvedParser::Var: {
        setState(139);
        variableStatement();
        break;
      }

      case JavaScript_improvedParser::T__11:
      case JavaScript_improvedParser::T__12:
      case JavaScript_improvedParser::T__16:
      case JavaScript_improvedParser::T__17:
      case JavaScript_improvedParser::T__18:
      case JavaScript_improvedParser::T__19:
      case JavaScript_improvedParser::This:
      case JavaScript_improvedParser::Identifier:
      case JavaScript_improvedParser::OpenBracket:
      case JavaScript_improvedParser::OpenParen:
      case JavaScript_improvedParser::OpenBrace:
      case JavaScript_improvedParser::Literal: {
        setState(140);
        expression();
        break;
      }

      case JavaScript_improvedParser::SemiColon: {
        break;
      }

    default:
      break;
    }
    setState(143);
    match(JavaScript_improvedParser::SemiColon);
    setState(145);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & -9222809086899376128) != 0) || ((((_la - 65) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 65)) & 2069) != 0)) {
      setState(144);
      expression();
    }
    setState(147);
    match(JavaScript_improvedParser::SemiColon);
    setState(149);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & -9222809086899376128) != 0) || ((((_la - 65) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 65)) & 2069) != 0)) {
      setState(148);
      expression();
    }
    setState(151);
    match(JavaScript_improvedParser::CloseParen);
    setState(152);
    statement();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ReturnStatementContext ------------------------------------------------------------------

JavaScript_improvedParser::ReturnStatementContext::ReturnStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScript_improvedParser::ReturnStatementContext::Return() {
  return getToken(JavaScript_improvedParser::Return, 0);
}

JavaScript_improvedParser::ExpressionContext* JavaScript_improvedParser::ReturnStatementContext::expression() {
  return getRuleContext<JavaScript_improvedParser::ExpressionContext>(0);
}

tree::TerminalNode* JavaScript_improvedParser::ReturnStatementContext::SemiColon() {
  return getToken(JavaScript_improvedParser::SemiColon, 0);
}


size_t JavaScript_improvedParser::ReturnStatementContext::getRuleIndex() const {
  return JavaScript_improvedParser::RuleReturnStatement;
}

void JavaScript_improvedParser::ReturnStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterReturnStatement(this);
}

void JavaScript_improvedParser::ReturnStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitReturnStatement(this);
}

JavaScript_improvedParser::ReturnStatementContext* JavaScript_improvedParser::returnStatement() {
  ReturnStatementContext *_localctx = _tracker.createInstance<ReturnStatementContext>(_ctx, getState());
  enterRule(_localctx, 22, JavaScript_improvedParser::RuleReturnStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(154);
    match(JavaScript_improvedParser::Return);
    setState(156);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 13, _ctx)) {
    case 1: {
      setState(155);
      expression();
      break;
    }

    default:
      break;
    }
    setState(159);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 14, _ctx)) {
    case 1: {
      setState(158);
      match(JavaScript_improvedParser::SemiColon);
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

//----------------- ExpressionContext ------------------------------------------------------------------

JavaScript_improvedParser::ExpressionContext::ExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

JavaScript_improvedParser::AssignmentExpressionContext* JavaScript_improvedParser::ExpressionContext::assignmentExpression() {
  return getRuleContext<JavaScript_improvedParser::AssignmentExpressionContext>(0);
}


size_t JavaScript_improvedParser::ExpressionContext::getRuleIndex() const {
  return JavaScript_improvedParser::RuleExpression;
}

void JavaScript_improvedParser::ExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterExpression(this);
}

void JavaScript_improvedParser::ExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitExpression(this);
}

JavaScript_improvedParser::ExpressionContext* JavaScript_improvedParser::expression() {
  ExpressionContext *_localctx = _tracker.createInstance<ExpressionContext>(_ctx, getState());
  enterRule(_localctx, 24, JavaScript_improvedParser::RuleExpression);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(161);
    assignmentExpression();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AssignmentExpressionContext ------------------------------------------------------------------

JavaScript_improvedParser::AssignmentExpressionContext::AssignmentExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

JavaScript_improvedParser::ConditionalExpressionContext* JavaScript_improvedParser::AssignmentExpressionContext::conditionalExpression() {
  return getRuleContext<JavaScript_improvedParser::ConditionalExpressionContext>(0);
}

tree::TerminalNode* JavaScript_improvedParser::AssignmentExpressionContext::AssignmentOperator() {
  return getToken(JavaScript_improvedParser::AssignmentOperator, 0);
}

JavaScript_improvedParser::AssignmentExpressionContext* JavaScript_improvedParser::AssignmentExpressionContext::assignmentExpression() {
  return getRuleContext<JavaScript_improvedParser::AssignmentExpressionContext>(0);
}


size_t JavaScript_improvedParser::AssignmentExpressionContext::getRuleIndex() const {
  return JavaScript_improvedParser::RuleAssignmentExpression;
}

void JavaScript_improvedParser::AssignmentExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAssignmentExpression(this);
}

void JavaScript_improvedParser::AssignmentExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAssignmentExpression(this);
}

JavaScript_improvedParser::AssignmentExpressionContext* JavaScript_improvedParser::assignmentExpression() {
  AssignmentExpressionContext *_localctx = _tracker.createInstance<AssignmentExpressionContext>(_ctx, getState());
  enterRule(_localctx, 26, JavaScript_improvedParser::RuleAssignmentExpression);
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
    setState(163);
    conditionalExpression();
    setState(166);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == JavaScript_improvedParser::AssignmentOperator) {
      setState(164);
      match(JavaScript_improvedParser::AssignmentOperator);
      setState(165);
      assignmentExpression();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ConditionalExpressionContext ------------------------------------------------------------------

JavaScript_improvedParser::ConditionalExpressionContext::ConditionalExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

JavaScript_improvedParser::LogicalOrExpressionContext* JavaScript_improvedParser::ConditionalExpressionContext::logicalOrExpression() {
  return getRuleContext<JavaScript_improvedParser::LogicalOrExpressionContext>(0);
}

tree::TerminalNode* JavaScript_improvedParser::ConditionalExpressionContext::Question() {
  return getToken(JavaScript_improvedParser::Question, 0);
}

JavaScript_improvedParser::AssignmentExpressionContext* JavaScript_improvedParser::ConditionalExpressionContext::assignmentExpression() {
  return getRuleContext<JavaScript_improvedParser::AssignmentExpressionContext>(0);
}

tree::TerminalNode* JavaScript_improvedParser::ConditionalExpressionContext::Colon() {
  return getToken(JavaScript_improvedParser::Colon, 0);
}

JavaScript_improvedParser::ConditionalExpressionContext* JavaScript_improvedParser::ConditionalExpressionContext::conditionalExpression() {
  return getRuleContext<JavaScript_improvedParser::ConditionalExpressionContext>(0);
}


size_t JavaScript_improvedParser::ConditionalExpressionContext::getRuleIndex() const {
  return JavaScript_improvedParser::RuleConditionalExpression;
}

void JavaScript_improvedParser::ConditionalExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterConditionalExpression(this);
}

void JavaScript_improvedParser::ConditionalExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitConditionalExpression(this);
}

JavaScript_improvedParser::ConditionalExpressionContext* JavaScript_improvedParser::conditionalExpression() {
  ConditionalExpressionContext *_localctx = _tracker.createInstance<ConditionalExpressionContext>(_ctx, getState());
  enterRule(_localctx, 28, JavaScript_improvedParser::RuleConditionalExpression);
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
    setState(168);
    logicalOrExpression();
    setState(174);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == JavaScript_improvedParser::Question) {
      setState(169);
      match(JavaScript_improvedParser::Question);
      setState(170);
      assignmentExpression();
      setState(171);
      match(JavaScript_improvedParser::Colon);
      setState(172);
      conditionalExpression();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LogicalOrExpressionContext ------------------------------------------------------------------

JavaScript_improvedParser::LogicalOrExpressionContext::LogicalOrExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<JavaScript_improvedParser::LogicalAndExpressionContext *> JavaScript_improvedParser::LogicalOrExpressionContext::logicalAndExpression() {
  return getRuleContexts<JavaScript_improvedParser::LogicalAndExpressionContext>();
}

JavaScript_improvedParser::LogicalAndExpressionContext* JavaScript_improvedParser::LogicalOrExpressionContext::logicalAndExpression(size_t i) {
  return getRuleContext<JavaScript_improvedParser::LogicalAndExpressionContext>(i);
}


size_t JavaScript_improvedParser::LogicalOrExpressionContext::getRuleIndex() const {
  return JavaScript_improvedParser::RuleLogicalOrExpression;
}

void JavaScript_improvedParser::LogicalOrExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLogicalOrExpression(this);
}

void JavaScript_improvedParser::LogicalOrExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLogicalOrExpression(this);
}

JavaScript_improvedParser::LogicalOrExpressionContext* JavaScript_improvedParser::logicalOrExpression() {
  LogicalOrExpressionContext *_localctx = _tracker.createInstance<LogicalOrExpressionContext>(_ctx, getState());
  enterRule(_localctx, 30, JavaScript_improvedParser::RuleLogicalOrExpression);
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
    setState(176);
    logicalAndExpression();
    setState(181);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == JavaScript_improvedParser::T__1) {
      setState(177);
      match(JavaScript_improvedParser::T__1);
      setState(178);
      logicalAndExpression();
      setState(183);
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

//----------------- LogicalAndExpressionContext ------------------------------------------------------------------

JavaScript_improvedParser::LogicalAndExpressionContext::LogicalAndExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<JavaScript_improvedParser::EqualityExpressionContext *> JavaScript_improvedParser::LogicalAndExpressionContext::equalityExpression() {
  return getRuleContexts<JavaScript_improvedParser::EqualityExpressionContext>();
}

JavaScript_improvedParser::EqualityExpressionContext* JavaScript_improvedParser::LogicalAndExpressionContext::equalityExpression(size_t i) {
  return getRuleContext<JavaScript_improvedParser::EqualityExpressionContext>(i);
}


size_t JavaScript_improvedParser::LogicalAndExpressionContext::getRuleIndex() const {
  return JavaScript_improvedParser::RuleLogicalAndExpression;
}

void JavaScript_improvedParser::LogicalAndExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLogicalAndExpression(this);
}

void JavaScript_improvedParser::LogicalAndExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLogicalAndExpression(this);
}

JavaScript_improvedParser::LogicalAndExpressionContext* JavaScript_improvedParser::logicalAndExpression() {
  LogicalAndExpressionContext *_localctx = _tracker.createInstance<LogicalAndExpressionContext>(_ctx, getState());
  enterRule(_localctx, 32, JavaScript_improvedParser::RuleLogicalAndExpression);
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
    setState(184);
    equalityExpression();
    setState(189);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == JavaScript_improvedParser::T__2) {
      setState(185);
      match(JavaScript_improvedParser::T__2);
      setState(186);
      equalityExpression();
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

//----------------- EqualityExpressionContext ------------------------------------------------------------------

JavaScript_improvedParser::EqualityExpressionContext::EqualityExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<JavaScript_improvedParser::RelationalExpressionContext *> JavaScript_improvedParser::EqualityExpressionContext::relationalExpression() {
  return getRuleContexts<JavaScript_improvedParser::RelationalExpressionContext>();
}

JavaScript_improvedParser::RelationalExpressionContext* JavaScript_improvedParser::EqualityExpressionContext::relationalExpression(size_t i) {
  return getRuleContext<JavaScript_improvedParser::RelationalExpressionContext>(i);
}


size_t JavaScript_improvedParser::EqualityExpressionContext::getRuleIndex() const {
  return JavaScript_improvedParser::RuleEqualityExpression;
}

void JavaScript_improvedParser::EqualityExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterEqualityExpression(this);
}

void JavaScript_improvedParser::EqualityExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitEqualityExpression(this);
}

JavaScript_improvedParser::EqualityExpressionContext* JavaScript_improvedParser::equalityExpression() {
  EqualityExpressionContext *_localctx = _tracker.createInstance<EqualityExpressionContext>(_ctx, getState());
  enterRule(_localctx, 34, JavaScript_improvedParser::RuleEqualityExpression);
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
    setState(192);
    relationalExpression();
    setState(197);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 240) != 0)) {
      setState(193);
      _la = _input->LA(1);
      if (!((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 240) != 0))) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(194);
      relationalExpression();
      setState(199);
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

//----------------- RelationalExpressionContext ------------------------------------------------------------------

JavaScript_improvedParser::RelationalExpressionContext::RelationalExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<JavaScript_improvedParser::AdditiveExpressionContext *> JavaScript_improvedParser::RelationalExpressionContext::additiveExpression() {
  return getRuleContexts<JavaScript_improvedParser::AdditiveExpressionContext>();
}

JavaScript_improvedParser::AdditiveExpressionContext* JavaScript_improvedParser::RelationalExpressionContext::additiveExpression(size_t i) {
  return getRuleContext<JavaScript_improvedParser::AdditiveExpressionContext>(i);
}


size_t JavaScript_improvedParser::RelationalExpressionContext::getRuleIndex() const {
  return JavaScript_improvedParser::RuleRelationalExpression;
}

void JavaScript_improvedParser::RelationalExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterRelationalExpression(this);
}

void JavaScript_improvedParser::RelationalExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitRelationalExpression(this);
}

JavaScript_improvedParser::RelationalExpressionContext* JavaScript_improvedParser::relationalExpression() {
  RelationalExpressionContext *_localctx = _tracker.createInstance<RelationalExpressionContext>(_ctx, getState());
  enterRule(_localctx, 36, JavaScript_improvedParser::RuleRelationalExpression);
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
    setState(200);
    additiveExpression();
    setState(205);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 3840) != 0)) {
      setState(201);
      _la = _input->LA(1);
      if (!((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 3840) != 0))) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(202);
      additiveExpression();
      setState(207);
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

//----------------- AdditiveExpressionContext ------------------------------------------------------------------

JavaScript_improvedParser::AdditiveExpressionContext::AdditiveExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<JavaScript_improvedParser::MultiplicativeExpressionContext *> JavaScript_improvedParser::AdditiveExpressionContext::multiplicativeExpression() {
  return getRuleContexts<JavaScript_improvedParser::MultiplicativeExpressionContext>();
}

JavaScript_improvedParser::MultiplicativeExpressionContext* JavaScript_improvedParser::AdditiveExpressionContext::multiplicativeExpression(size_t i) {
  return getRuleContext<JavaScript_improvedParser::MultiplicativeExpressionContext>(i);
}


size_t JavaScript_improvedParser::AdditiveExpressionContext::getRuleIndex() const {
  return JavaScript_improvedParser::RuleAdditiveExpression;
}

void JavaScript_improvedParser::AdditiveExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAdditiveExpression(this);
}

void JavaScript_improvedParser::AdditiveExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAdditiveExpression(this);
}

JavaScript_improvedParser::AdditiveExpressionContext* JavaScript_improvedParser::additiveExpression() {
  AdditiveExpressionContext *_localctx = _tracker.createInstance<AdditiveExpressionContext>(_ctx, getState());
  enterRule(_localctx, 38, JavaScript_improvedParser::RuleAdditiveExpression);
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
    setState(208);
    multiplicativeExpression();
    setState(213);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 21, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(209);
        _la = _input->LA(1);
        if (!(_la == JavaScript_improvedParser::T__11

        || _la == JavaScript_improvedParser::T__12)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(210);
        multiplicativeExpression(); 
      }
      setState(215);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 21, _ctx);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- MultiplicativeExpressionContext ------------------------------------------------------------------

JavaScript_improvedParser::MultiplicativeExpressionContext::MultiplicativeExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<JavaScript_improvedParser::UnaryExpressionContext *> JavaScript_improvedParser::MultiplicativeExpressionContext::unaryExpression() {
  return getRuleContexts<JavaScript_improvedParser::UnaryExpressionContext>();
}

JavaScript_improvedParser::UnaryExpressionContext* JavaScript_improvedParser::MultiplicativeExpressionContext::unaryExpression(size_t i) {
  return getRuleContext<JavaScript_improvedParser::UnaryExpressionContext>(i);
}


size_t JavaScript_improvedParser::MultiplicativeExpressionContext::getRuleIndex() const {
  return JavaScript_improvedParser::RuleMultiplicativeExpression;
}

void JavaScript_improvedParser::MultiplicativeExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMultiplicativeExpression(this);
}

void JavaScript_improvedParser::MultiplicativeExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMultiplicativeExpression(this);
}

JavaScript_improvedParser::MultiplicativeExpressionContext* JavaScript_improvedParser::multiplicativeExpression() {
  MultiplicativeExpressionContext *_localctx = _tracker.createInstance<MultiplicativeExpressionContext>(_ctx, getState());
  enterRule(_localctx, 40, JavaScript_improvedParser::RuleMultiplicativeExpression);
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
    unaryExpression();
    setState(221);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 114688) != 0)) {
      setState(217);
      _la = _input->LA(1);
      if (!((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 114688) != 0))) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(218);
      unaryExpression();
      setState(223);
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

//----------------- UnaryExpressionContext ------------------------------------------------------------------

JavaScript_improvedParser::UnaryExpressionContext::UnaryExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

JavaScript_improvedParser::PostfixExpressionContext* JavaScript_improvedParser::UnaryExpressionContext::postfixExpression() {
  return getRuleContext<JavaScript_improvedParser::PostfixExpressionContext>(0);
}

JavaScript_improvedParser::UnaryExpressionContext* JavaScript_improvedParser::UnaryExpressionContext::unaryExpression() {
  return getRuleContext<JavaScript_improvedParser::UnaryExpressionContext>(0);
}


size_t JavaScript_improvedParser::UnaryExpressionContext::getRuleIndex() const {
  return JavaScript_improvedParser::RuleUnaryExpression;
}

void JavaScript_improvedParser::UnaryExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterUnaryExpression(this);
}

void JavaScript_improvedParser::UnaryExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitUnaryExpression(this);
}

JavaScript_improvedParser::UnaryExpressionContext* JavaScript_improvedParser::unaryExpression() {
  UnaryExpressionContext *_localctx = _tracker.createInstance<UnaryExpressionContext>(_ctx, getState());
  enterRule(_localctx, 42, JavaScript_improvedParser::RuleUnaryExpression);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(227);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case JavaScript_improvedParser::This:
      case JavaScript_improvedParser::Identifier:
      case JavaScript_improvedParser::OpenBracket:
      case JavaScript_improvedParser::OpenParen:
      case JavaScript_improvedParser::OpenBrace:
      case JavaScript_improvedParser::Literal: {
        enterOuterAlt(_localctx, 1);
        setState(224);
        postfixExpression();
        break;
      }

      case JavaScript_improvedParser::T__11:
      case JavaScript_improvedParser::T__12:
      case JavaScript_improvedParser::T__16:
      case JavaScript_improvedParser::T__17:
      case JavaScript_improvedParser::T__18:
      case JavaScript_improvedParser::T__19: {
        enterOuterAlt(_localctx, 2);
        setState(225);
        _la = _input->LA(1);
        if (!((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & 1978368) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(226);
        unaryExpression();
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

//----------------- PostfixExpressionContext ------------------------------------------------------------------

JavaScript_improvedParser::PostfixExpressionContext::PostfixExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

JavaScript_improvedParser::PrimaryExpressionContext* JavaScript_improvedParser::PostfixExpressionContext::primaryExpression() {
  return getRuleContext<JavaScript_improvedParser::PrimaryExpressionContext>(0);
}

std::vector<tree::TerminalNode *> JavaScript_improvedParser::PostfixExpressionContext::OpenBracket() {
  return getTokens(JavaScript_improvedParser::OpenBracket);
}

tree::TerminalNode* JavaScript_improvedParser::PostfixExpressionContext::OpenBracket(size_t i) {
  return getToken(JavaScript_improvedParser::OpenBracket, i);
}

std::vector<JavaScript_improvedParser::ExpressionContext *> JavaScript_improvedParser::PostfixExpressionContext::expression() {
  return getRuleContexts<JavaScript_improvedParser::ExpressionContext>();
}

JavaScript_improvedParser::ExpressionContext* JavaScript_improvedParser::PostfixExpressionContext::expression(size_t i) {
  return getRuleContext<JavaScript_improvedParser::ExpressionContext>(i);
}

std::vector<tree::TerminalNode *> JavaScript_improvedParser::PostfixExpressionContext::CloseBracket() {
  return getTokens(JavaScript_improvedParser::CloseBracket);
}

tree::TerminalNode* JavaScript_improvedParser::PostfixExpressionContext::CloseBracket(size_t i) {
  return getToken(JavaScript_improvedParser::CloseBracket, i);
}

std::vector<tree::TerminalNode *> JavaScript_improvedParser::PostfixExpressionContext::Dot() {
  return getTokens(JavaScript_improvedParser::Dot);
}

tree::TerminalNode* JavaScript_improvedParser::PostfixExpressionContext::Dot(size_t i) {
  return getToken(JavaScript_improvedParser::Dot, i);
}

std::vector<tree::TerminalNode *> JavaScript_improvedParser::PostfixExpressionContext::Identifier() {
  return getTokens(JavaScript_improvedParser::Identifier);
}

tree::TerminalNode* JavaScript_improvedParser::PostfixExpressionContext::Identifier(size_t i) {
  return getToken(JavaScript_improvedParser::Identifier, i);
}

std::vector<tree::TerminalNode *> JavaScript_improvedParser::PostfixExpressionContext::OpenParen() {
  return getTokens(JavaScript_improvedParser::OpenParen);
}

tree::TerminalNode* JavaScript_improvedParser::PostfixExpressionContext::OpenParen(size_t i) {
  return getToken(JavaScript_improvedParser::OpenParen, i);
}

std::vector<tree::TerminalNode *> JavaScript_improvedParser::PostfixExpressionContext::CloseParen() {
  return getTokens(JavaScript_improvedParser::CloseParen);
}

tree::TerminalNode* JavaScript_improvedParser::PostfixExpressionContext::CloseParen(size_t i) {
  return getToken(JavaScript_improvedParser::CloseParen, i);
}

std::vector<JavaScript_improvedParser::ArgumentListContext *> JavaScript_improvedParser::PostfixExpressionContext::argumentList() {
  return getRuleContexts<JavaScript_improvedParser::ArgumentListContext>();
}

JavaScript_improvedParser::ArgumentListContext* JavaScript_improvedParser::PostfixExpressionContext::argumentList(size_t i) {
  return getRuleContext<JavaScript_improvedParser::ArgumentListContext>(i);
}


size_t JavaScript_improvedParser::PostfixExpressionContext::getRuleIndex() const {
  return JavaScript_improvedParser::RulePostfixExpression;
}

void JavaScript_improvedParser::PostfixExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPostfixExpression(this);
}

void JavaScript_improvedParser::PostfixExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPostfixExpression(this);
}

JavaScript_improvedParser::PostfixExpressionContext* JavaScript_improvedParser::postfixExpression() {
  PostfixExpressionContext *_localctx = _tracker.createInstance<PostfixExpressionContext>(_ctx, getState());
  enterRule(_localctx, 44, JavaScript_improvedParser::RulePostfixExpression);
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
    setState(229);
    primaryExpression();
    setState(247);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 26, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(243);
        _errHandler->sync(this);
        switch (_input->LA(1)) {
          case JavaScript_improvedParser::T__16: {
            setState(230);
            match(JavaScript_improvedParser::T__16);
            break;
          }

          case JavaScript_improvedParser::T__17: {
            setState(231);
            match(JavaScript_improvedParser::T__17);
            break;
          }

          case JavaScript_improvedParser::OpenBracket: {
            setState(232);
            match(JavaScript_improvedParser::OpenBracket);
            setState(233);
            expression();
            setState(234);
            match(JavaScript_improvedParser::CloseBracket);
            break;
          }

          case JavaScript_improvedParser::Dot: {
            setState(236);
            match(JavaScript_improvedParser::Dot);
            setState(237);
            match(JavaScript_improvedParser::Identifier);
            break;
          }

          case JavaScript_improvedParser::OpenParen: {
            setState(238);
            match(JavaScript_improvedParser::OpenParen);
            setState(240);
            _errHandler->sync(this);

            _la = _input->LA(1);
            if ((((_la & ~ 0x3fULL) == 0) &&
              ((1ULL << _la) & -9222809086899376128) != 0) || ((((_la - 65) & ~ 0x3fULL) == 0) &&
              ((1ULL << (_la - 65)) & 2069) != 0)) {
              setState(239);
              argumentList();
            }
            setState(242);
            match(JavaScript_improvedParser::CloseParen);
            break;
          }

        default:
          throw NoViableAltException(this);
        } 
      }
      setState(249);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 26, _ctx);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PrimaryExpressionContext ------------------------------------------------------------------

JavaScript_improvedParser::PrimaryExpressionContext::PrimaryExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScript_improvedParser::PrimaryExpressionContext::This() {
  return getToken(JavaScript_improvedParser::This, 0);
}

tree::TerminalNode* JavaScript_improvedParser::PrimaryExpressionContext::Identifier() {
  return getToken(JavaScript_improvedParser::Identifier, 0);
}

tree::TerminalNode* JavaScript_improvedParser::PrimaryExpressionContext::Literal() {
  return getToken(JavaScript_improvedParser::Literal, 0);
}

tree::TerminalNode* JavaScript_improvedParser::PrimaryExpressionContext::OpenParen() {
  return getToken(JavaScript_improvedParser::OpenParen, 0);
}

JavaScript_improvedParser::ExpressionContext* JavaScript_improvedParser::PrimaryExpressionContext::expression() {
  return getRuleContext<JavaScript_improvedParser::ExpressionContext>(0);
}

tree::TerminalNode* JavaScript_improvedParser::PrimaryExpressionContext::CloseParen() {
  return getToken(JavaScript_improvedParser::CloseParen, 0);
}

JavaScript_improvedParser::ArrayLiteralContext* JavaScript_improvedParser::PrimaryExpressionContext::arrayLiteral() {
  return getRuleContext<JavaScript_improvedParser::ArrayLiteralContext>(0);
}

JavaScript_improvedParser::ObjectLiteralContext* JavaScript_improvedParser::PrimaryExpressionContext::objectLiteral() {
  return getRuleContext<JavaScript_improvedParser::ObjectLiteralContext>(0);
}


size_t JavaScript_improvedParser::PrimaryExpressionContext::getRuleIndex() const {
  return JavaScript_improvedParser::RulePrimaryExpression;
}

void JavaScript_improvedParser::PrimaryExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPrimaryExpression(this);
}

void JavaScript_improvedParser::PrimaryExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPrimaryExpression(this);
}

JavaScript_improvedParser::PrimaryExpressionContext* JavaScript_improvedParser::primaryExpression() {
  PrimaryExpressionContext *_localctx = _tracker.createInstance<PrimaryExpressionContext>(_ctx, getState());
  enterRule(_localctx, 46, JavaScript_improvedParser::RulePrimaryExpression);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(259);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case JavaScript_improvedParser::This: {
        enterOuterAlt(_localctx, 1);
        setState(250);
        match(JavaScript_improvedParser::This);
        break;
      }

      case JavaScript_improvedParser::Identifier: {
        enterOuterAlt(_localctx, 2);
        setState(251);
        match(JavaScript_improvedParser::Identifier);
        break;
      }

      case JavaScript_improvedParser::Literal: {
        enterOuterAlt(_localctx, 3);
        setState(252);
        match(JavaScript_improvedParser::Literal);
        break;
      }

      case JavaScript_improvedParser::OpenParen: {
        enterOuterAlt(_localctx, 4);
        setState(253);
        match(JavaScript_improvedParser::OpenParen);
        setState(254);
        expression();
        setState(255);
        match(JavaScript_improvedParser::CloseParen);
        break;
      }

      case JavaScript_improvedParser::OpenBracket: {
        enterOuterAlt(_localctx, 5);
        setState(257);
        arrayLiteral();
        break;
      }

      case JavaScript_improvedParser::OpenBrace: {
        enterOuterAlt(_localctx, 6);
        setState(258);
        objectLiteral();
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

//----------------- ArrayLiteralContext ------------------------------------------------------------------

JavaScript_improvedParser::ArrayLiteralContext::ArrayLiteralContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScript_improvedParser::ArrayLiteralContext::OpenBracket() {
  return getToken(JavaScript_improvedParser::OpenBracket, 0);
}

tree::TerminalNode* JavaScript_improvedParser::ArrayLiteralContext::CloseBracket() {
  return getToken(JavaScript_improvedParser::CloseBracket, 0);
}

std::vector<JavaScript_improvedParser::ExpressionContext *> JavaScript_improvedParser::ArrayLiteralContext::expression() {
  return getRuleContexts<JavaScript_improvedParser::ExpressionContext>();
}

JavaScript_improvedParser::ExpressionContext* JavaScript_improvedParser::ArrayLiteralContext::expression(size_t i) {
  return getRuleContext<JavaScript_improvedParser::ExpressionContext>(i);
}

std::vector<tree::TerminalNode *> JavaScript_improvedParser::ArrayLiteralContext::Comma() {
  return getTokens(JavaScript_improvedParser::Comma);
}

tree::TerminalNode* JavaScript_improvedParser::ArrayLiteralContext::Comma(size_t i) {
  return getToken(JavaScript_improvedParser::Comma, i);
}


size_t JavaScript_improvedParser::ArrayLiteralContext::getRuleIndex() const {
  return JavaScript_improvedParser::RuleArrayLiteral;
}

void JavaScript_improvedParser::ArrayLiteralContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterArrayLiteral(this);
}

void JavaScript_improvedParser::ArrayLiteralContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitArrayLiteral(this);
}

JavaScript_improvedParser::ArrayLiteralContext* JavaScript_improvedParser::arrayLiteral() {
  ArrayLiteralContext *_localctx = _tracker.createInstance<ArrayLiteralContext>(_ctx, getState());
  enterRule(_localctx, 48, JavaScript_improvedParser::RuleArrayLiteral);
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
    setState(261);
    match(JavaScript_improvedParser::OpenBracket);
    setState(270);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & -9222809086899376128) != 0) || ((((_la - 65) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 65)) & 2069) != 0)) {
      setState(262);
      expression();
      setState(267);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == JavaScript_improvedParser::Comma) {
        setState(263);
        match(JavaScript_improvedParser::Comma);
        setState(264);
        expression();
        setState(269);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
    setState(272);
    match(JavaScript_improvedParser::CloseBracket);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ObjectLiteralContext ------------------------------------------------------------------

JavaScript_improvedParser::ObjectLiteralContext::ObjectLiteralContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScript_improvedParser::ObjectLiteralContext::OpenBrace() {
  return getToken(JavaScript_improvedParser::OpenBrace, 0);
}

tree::TerminalNode* JavaScript_improvedParser::ObjectLiteralContext::CloseBrace() {
  return getToken(JavaScript_improvedParser::CloseBrace, 0);
}

std::vector<JavaScript_improvedParser::PropertyAssignmentContext *> JavaScript_improvedParser::ObjectLiteralContext::propertyAssignment() {
  return getRuleContexts<JavaScript_improvedParser::PropertyAssignmentContext>();
}

JavaScript_improvedParser::PropertyAssignmentContext* JavaScript_improvedParser::ObjectLiteralContext::propertyAssignment(size_t i) {
  return getRuleContext<JavaScript_improvedParser::PropertyAssignmentContext>(i);
}

std::vector<tree::TerminalNode *> JavaScript_improvedParser::ObjectLiteralContext::Comma() {
  return getTokens(JavaScript_improvedParser::Comma);
}

tree::TerminalNode* JavaScript_improvedParser::ObjectLiteralContext::Comma(size_t i) {
  return getToken(JavaScript_improvedParser::Comma, i);
}


size_t JavaScript_improvedParser::ObjectLiteralContext::getRuleIndex() const {
  return JavaScript_improvedParser::RuleObjectLiteral;
}

void JavaScript_improvedParser::ObjectLiteralContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterObjectLiteral(this);
}

void JavaScript_improvedParser::ObjectLiteralContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitObjectLiteral(this);
}

JavaScript_improvedParser::ObjectLiteralContext* JavaScript_improvedParser::objectLiteral() {
  ObjectLiteralContext *_localctx = _tracker.createInstance<ObjectLiteralContext>(_ctx, getState());
  enterRule(_localctx, 50, JavaScript_improvedParser::RuleObjectLiteral);
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
    setState(274);
    match(JavaScript_improvedParser::OpenBrace);
    setState(283);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & -2305843009213693952) != 0)) {
      setState(275);
      propertyAssignment();
      setState(280);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == JavaScript_improvedParser::Comma) {
        setState(276);
        match(JavaScript_improvedParser::Comma);
        setState(277);
        propertyAssignment();
        setState(282);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
    setState(285);
    match(JavaScript_improvedParser::CloseBrace);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PropertyAssignmentContext ------------------------------------------------------------------

JavaScript_improvedParser::PropertyAssignmentContext::PropertyAssignmentContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScript_improvedParser::PropertyAssignmentContext::Identifier() {
  return getToken(JavaScript_improvedParser::Identifier, 0);
}

tree::TerminalNode* JavaScript_improvedParser::PropertyAssignmentContext::Colon() {
  return getToken(JavaScript_improvedParser::Colon, 0);
}

JavaScript_improvedParser::ExpressionContext* JavaScript_improvedParser::PropertyAssignmentContext::expression() {
  return getRuleContext<JavaScript_improvedParser::ExpressionContext>(0);
}

tree::TerminalNode* JavaScript_improvedParser::PropertyAssignmentContext::StringLiteral() {
  return getToken(JavaScript_improvedParser::StringLiteral, 0);
}

tree::TerminalNode* JavaScript_improvedParser::PropertyAssignmentContext::NumericLiteral() {
  return getToken(JavaScript_improvedParser::NumericLiteral, 0);
}


size_t JavaScript_improvedParser::PropertyAssignmentContext::getRuleIndex() const {
  return JavaScript_improvedParser::RulePropertyAssignment;
}

void JavaScript_improvedParser::PropertyAssignmentContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPropertyAssignment(this);
}

void JavaScript_improvedParser::PropertyAssignmentContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPropertyAssignment(this);
}

JavaScript_improvedParser::PropertyAssignmentContext* JavaScript_improvedParser::propertyAssignment() {
  PropertyAssignmentContext *_localctx = _tracker.createInstance<PropertyAssignmentContext>(_ctx, getState());
  enterRule(_localctx, 52, JavaScript_improvedParser::RulePropertyAssignment);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(296);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case JavaScript_improvedParser::Identifier: {
        enterOuterAlt(_localctx, 1);
        setState(287);
        match(JavaScript_improvedParser::Identifier);
        setState(288);
        match(JavaScript_improvedParser::Colon);
        setState(289);
        expression();
        break;
      }

      case JavaScript_improvedParser::StringLiteral: {
        enterOuterAlt(_localctx, 2);
        setState(290);
        match(JavaScript_improvedParser::StringLiteral);
        setState(291);
        match(JavaScript_improvedParser::Colon);
        setState(292);
        expression();
        break;
      }

      case JavaScript_improvedParser::NumericLiteral: {
        enterOuterAlt(_localctx, 3);
        setState(293);
        match(JavaScript_improvedParser::NumericLiteral);
        setState(294);
        match(JavaScript_improvedParser::Colon);
        setState(295);
        expression();
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

//----------------- ArgumentListContext ------------------------------------------------------------------

JavaScript_improvedParser::ArgumentListContext::ArgumentListContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<JavaScript_improvedParser::ExpressionContext *> JavaScript_improvedParser::ArgumentListContext::expression() {
  return getRuleContexts<JavaScript_improvedParser::ExpressionContext>();
}

JavaScript_improvedParser::ExpressionContext* JavaScript_improvedParser::ArgumentListContext::expression(size_t i) {
  return getRuleContext<JavaScript_improvedParser::ExpressionContext>(i);
}

std::vector<tree::TerminalNode *> JavaScript_improvedParser::ArgumentListContext::Comma() {
  return getTokens(JavaScript_improvedParser::Comma);
}

tree::TerminalNode* JavaScript_improvedParser::ArgumentListContext::Comma(size_t i) {
  return getToken(JavaScript_improvedParser::Comma, i);
}


size_t JavaScript_improvedParser::ArgumentListContext::getRuleIndex() const {
  return JavaScript_improvedParser::RuleArgumentList;
}

void JavaScript_improvedParser::ArgumentListContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterArgumentList(this);
}

void JavaScript_improvedParser::ArgumentListContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScript_improvedListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitArgumentList(this);
}

JavaScript_improvedParser::ArgumentListContext* JavaScript_improvedParser::argumentList() {
  ArgumentListContext *_localctx = _tracker.createInstance<ArgumentListContext>(_ctx, getState());
  enterRule(_localctx, 54, JavaScript_improvedParser::RuleArgumentList);
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
    setState(298);
    expression();
    setState(303);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == JavaScript_improvedParser::Comma) {
      setState(299);
      match(JavaScript_improvedParser::Comma);
      setState(300);
      expression();
      setState(305);
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

void JavaScript_improvedParser::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  javascript_improvedParserInitialize();
#else
  ::antlr4::internal::call_once(javascript_improvedParserOnceFlag, javascript_improvedParserInitialize);
#endif
}
