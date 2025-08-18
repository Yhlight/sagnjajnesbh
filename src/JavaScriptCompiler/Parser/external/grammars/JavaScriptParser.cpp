
// Generated from external/grammars/JavaScript.g4 by ANTLR 4.13.2


#include "JavaScriptListener.h"

#include "JavaScriptParser.h"


using namespace antlrcpp;

using namespace antlr4;

namespace {

struct JavaScriptParserStaticData final {
  JavaScriptParserStaticData(std::vector<std::string> ruleNames,
                        std::vector<std::string> literalNames,
                        std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  JavaScriptParserStaticData(const JavaScriptParserStaticData&) = delete;
  JavaScriptParserStaticData(JavaScriptParserStaticData&&) = delete;
  JavaScriptParserStaticData& operator=(const JavaScriptParserStaticData&) = delete;
  JavaScriptParserStaticData& operator=(JavaScriptParserStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag javascriptParserOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
std::unique_ptr<JavaScriptParserStaticData> javascriptParserStaticData = nullptr;

void javascriptParserInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (javascriptParserStaticData != nullptr) {
    return;
  }
#else
  assert(javascriptParserStaticData == nullptr);
#endif
  auto staticData = std::make_unique<JavaScriptParserStaticData>(
    std::vector<std::string>{
      "program", "sourceElements", "sourceElement", "statement", "block", 
      "statementList", "variableStatement", "variableDeclarationList", "variableDeclaration", 
      "emptyStatement", "expressionStatement", "ifStatement", "iterationStatement", 
      "continueStatement", "breakStatement", "returnStatement", "withStatement", 
      "switchStatement", "caseBlock", "caseClauses", "caseClause", "defaultClause", 
      "labelledStatement", "throwStatement", "tryStatement", "catchProduction", 
      "finallyProduction", "debuggerStatement", "functionDeclaration", "formalParameterList", 
      "functionBody", "expressionSequence", "singleExpression", "assignmentOperator", 
      "literal", "numericLiteral", "identifierName", "reservedWord", "keyword", 
      "arrayLiteral", "elementList", "elision", "objectLiteral", "propertyNameAndValueList", 
      "propertyAssignment", "propertyName", "propertySetParameterList", 
      "arguments", "argumentList"
    },
    std::vector<std::string>{
      "", "'{'", "'}'", "';'", "','", "'='", "'('", "')'", "':'", "'['", 
      "']'", "'.'", "'++'", "'--'", "'+'", "'-'", "'~'", "'!'", "'*'", "'/'", 
      "'%'", "'<<'", "'>>'", "'>>>'", "'<'", "'>'", "'<='", "'>='", "'=='", 
      "'!='", "'==='", "'!=='", "'&'", "'^'", "'|'", "'&&'", "'||'", "'\\u003F'", 
      "'*='", "'/='", "'%='", "'+='", "'-='", "'<<='", "'>>='", "'>>>='", 
      "'&='", "'^='", "'|='", "'null'", "", "", "", "", "'break'", "'case'", 
      "'catch'", "'continue'", "'debugger'", "'default'", "'delete'", "'do'", 
      "'else'", "'finally'", "'for'", "'function'", "'if'", "'in'", "'instanceof'", 
      "'new'", "'return'", "'switch'", "'this'", "'throw'", "'try'", "'typeof'", 
      "'var'", "'void'", "'while'", "'with'", "'const'", "'let'", "'get'", 
      "'set'"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "NULL", 
      "BooleanLiteral", "DecimalLiteral", "HexIntegerLiteral", "OctalIntegerLiteral", 
      "BREAK", "CASE", "CATCH", "CONTINUE", "DEBUGGER", "DEFAULT", "DELETE", 
      "DO", "ELSE", "FINALLY", "FOR", "FUNCTION", "IF", "IN", "INSTANCEOF", 
      "NEW", "RETURN", "SWITCH", "THIS", "THROW", "TRY", "TYPEOF", "VAR", 
      "VOID", "WHILE", "WITH", "CONST", "LET", "GETTER", "SETTER", "StringLiteral", 
      "Identifier", "WhiteSpaces", "LineTerminator", "SingleLineComment", 
      "MultiLineComment"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,89,590,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,27,7,27,2,28,7,
  	28,2,29,7,29,2,30,7,30,2,31,7,31,2,32,7,32,2,33,7,33,2,34,7,34,2,35,7,
  	35,2,36,7,36,2,37,7,37,2,38,7,38,2,39,7,39,2,40,7,40,2,41,7,41,2,42,7,
  	42,2,43,7,43,2,44,7,44,2,45,7,45,2,46,7,46,2,47,7,47,2,48,7,48,1,0,3,
  	0,100,8,0,1,0,1,0,1,1,4,1,105,8,1,11,1,12,1,106,1,2,1,2,3,2,111,8,2,1,
  	3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,3,3,128,8,3,
  	1,4,1,4,3,4,132,8,4,1,4,1,4,1,5,4,5,137,8,5,11,5,12,5,138,1,6,1,6,1,6,
  	3,6,144,8,6,1,6,1,6,1,6,3,6,149,8,6,1,6,1,6,1,6,3,6,154,8,6,3,6,156,8,
  	6,1,7,1,7,1,7,5,7,161,8,7,10,7,12,7,164,9,7,1,8,1,8,1,8,3,8,169,8,8,1,
  	9,1,9,1,10,1,10,3,10,175,8,10,1,11,1,11,1,11,1,11,1,11,1,11,1,11,3,11,
  	184,8,11,1,12,1,12,1,12,3,12,189,8,12,1,12,1,12,3,12,193,8,12,1,12,1,
  	12,3,12,197,8,12,1,12,1,12,1,12,1,12,1,12,1,12,1,12,1,12,3,12,207,8,12,
  	1,12,1,12,3,12,211,8,12,1,12,1,12,1,12,1,12,1,12,1,12,1,12,1,12,1,12,
  	1,12,1,12,1,12,1,12,1,12,1,12,1,12,1,12,1,12,1,12,1,12,1,12,1,12,1,12,
  	1,12,1,12,1,12,1,12,1,12,1,12,1,12,1,12,1,12,1,12,3,12,246,8,12,3,12,
  	248,8,12,1,13,1,13,3,13,252,8,13,1,13,3,13,255,8,13,1,14,1,14,3,14,259,
  	8,14,1,14,3,14,262,8,14,1,15,1,15,3,15,266,8,15,1,15,3,15,269,8,15,1,
  	16,1,16,1,16,1,16,1,16,1,16,1,17,1,17,1,17,1,17,1,17,1,17,1,18,1,18,3,
  	18,285,8,18,1,18,1,18,3,18,289,8,18,3,18,291,8,18,1,18,1,18,1,19,4,19,
  	296,8,19,11,19,12,19,297,1,20,1,20,1,20,1,20,3,20,304,8,20,1,21,1,21,
  	1,21,3,21,309,8,21,1,22,1,22,1,22,1,22,1,23,1,23,1,23,3,23,318,8,23,1,
  	24,1,24,1,24,1,24,3,24,324,8,24,1,24,3,24,327,8,24,1,25,1,25,1,25,1,25,
  	1,25,1,25,1,26,1,26,1,26,1,27,1,27,3,27,340,8,27,1,28,1,28,1,28,1,28,
  	3,28,346,8,28,1,28,1,28,1,28,1,28,1,28,1,29,1,29,1,29,5,29,356,8,29,10,
  	29,12,29,359,9,29,1,30,3,30,362,8,30,1,31,1,31,1,31,5,31,367,8,31,10,
  	31,12,31,370,9,31,1,32,1,32,1,32,3,32,375,8,32,1,32,1,32,3,32,379,8,32,
  	1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,3,32,389,8,32,1,32,1,32,1,32,
  	1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,
  	1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,3,32,418,8,32,1,32,
  	1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,
  	1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,
  	1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,
  	1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,
  	1,32,1,32,1,32,1,32,1,32,1,32,5,32,483,8,32,10,32,12,32,486,9,32,1,33,
  	1,33,1,34,1,34,1,34,1,34,3,34,494,8,34,1,35,1,35,1,36,1,36,3,36,500,8,
  	36,1,37,1,37,1,37,3,37,505,8,37,1,38,1,38,1,39,1,39,3,39,511,8,39,1,39,
  	1,39,1,40,3,40,516,8,40,1,40,1,40,1,40,1,40,5,40,522,8,40,10,40,12,40,
  	525,9,40,1,41,1,41,1,42,1,42,3,42,531,8,42,1,42,3,42,534,8,42,1,42,1,
  	42,1,43,1,43,1,43,5,43,541,8,43,10,43,12,43,544,9,43,1,44,1,44,1,44,1,
  	44,1,44,1,44,1,44,1,44,1,44,1,44,1,44,1,44,1,44,1,44,1,44,1,44,1,44,1,
  	44,1,44,1,44,1,44,3,44,567,8,44,1,45,1,45,1,45,3,45,572,8,45,1,46,1,46,
  	1,47,1,47,3,47,578,8,47,1,47,1,47,1,48,1,48,1,48,5,48,585,8,48,10,48,
  	12,48,588,9,48,1,48,0,1,64,49,0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,
  	30,32,34,36,38,40,42,44,46,48,50,52,54,56,58,60,62,64,66,68,70,72,74,
  	76,78,80,82,84,86,88,90,92,94,96,0,8,1,0,18,20,1,0,14,15,1,0,21,23,1,
  	0,24,27,1,0,28,31,1,0,38,48,1,0,51,53,1,0,54,81,656,0,99,1,0,0,0,2,104,
  	1,0,0,0,4,110,1,0,0,0,6,127,1,0,0,0,8,129,1,0,0,0,10,136,1,0,0,0,12,155,
  	1,0,0,0,14,157,1,0,0,0,16,165,1,0,0,0,18,170,1,0,0,0,20,172,1,0,0,0,22,
  	176,1,0,0,0,24,247,1,0,0,0,26,249,1,0,0,0,28,256,1,0,0,0,30,263,1,0,0,
  	0,32,270,1,0,0,0,34,276,1,0,0,0,36,282,1,0,0,0,38,295,1,0,0,0,40,299,
  	1,0,0,0,42,305,1,0,0,0,44,310,1,0,0,0,46,314,1,0,0,0,48,319,1,0,0,0,50,
  	328,1,0,0,0,52,334,1,0,0,0,54,337,1,0,0,0,56,341,1,0,0,0,58,352,1,0,0,
  	0,60,361,1,0,0,0,62,363,1,0,0,0,64,417,1,0,0,0,66,487,1,0,0,0,68,493,
  	1,0,0,0,70,495,1,0,0,0,72,499,1,0,0,0,74,504,1,0,0,0,76,506,1,0,0,0,78,
  	508,1,0,0,0,80,515,1,0,0,0,82,526,1,0,0,0,84,528,1,0,0,0,86,537,1,0,0,
  	0,88,566,1,0,0,0,90,571,1,0,0,0,92,573,1,0,0,0,94,575,1,0,0,0,96,581,
  	1,0,0,0,98,100,3,2,1,0,99,98,1,0,0,0,99,100,1,0,0,0,100,101,1,0,0,0,101,
  	102,5,0,0,1,102,1,1,0,0,0,103,105,3,4,2,0,104,103,1,0,0,0,105,106,1,0,
  	0,0,106,104,1,0,0,0,106,107,1,0,0,0,107,3,1,0,0,0,108,111,3,6,3,0,109,
  	111,3,56,28,0,110,108,1,0,0,0,110,109,1,0,0,0,111,5,1,0,0,0,112,128,3,
  	8,4,0,113,128,3,12,6,0,114,128,3,18,9,0,115,128,3,20,10,0,116,128,3,22,
  	11,0,117,128,3,24,12,0,118,128,3,26,13,0,119,128,3,28,14,0,120,128,3,
  	30,15,0,121,128,3,32,16,0,122,128,3,44,22,0,123,128,3,34,17,0,124,128,
  	3,46,23,0,125,128,3,48,24,0,126,128,3,54,27,0,127,112,1,0,0,0,127,113,
  	1,0,0,0,127,114,1,0,0,0,127,115,1,0,0,0,127,116,1,0,0,0,127,117,1,0,0,
  	0,127,118,1,0,0,0,127,119,1,0,0,0,127,120,1,0,0,0,127,121,1,0,0,0,127,
  	122,1,0,0,0,127,123,1,0,0,0,127,124,1,0,0,0,127,125,1,0,0,0,127,126,1,
  	0,0,0,128,7,1,0,0,0,129,131,5,1,0,0,130,132,3,10,5,0,131,130,1,0,0,0,
  	131,132,1,0,0,0,132,133,1,0,0,0,133,134,5,2,0,0,134,9,1,0,0,0,135,137,
  	3,6,3,0,136,135,1,0,0,0,137,138,1,0,0,0,138,136,1,0,0,0,138,139,1,0,0,
  	0,139,11,1,0,0,0,140,141,5,76,0,0,141,143,3,14,7,0,142,144,5,3,0,0,143,
  	142,1,0,0,0,143,144,1,0,0,0,144,156,1,0,0,0,145,146,5,81,0,0,146,148,
  	3,14,7,0,147,149,5,3,0,0,148,147,1,0,0,0,148,149,1,0,0,0,149,156,1,0,
  	0,0,150,151,5,80,0,0,151,153,3,14,7,0,152,154,5,3,0,0,153,152,1,0,0,0,
  	153,154,1,0,0,0,154,156,1,0,0,0,155,140,1,0,0,0,155,145,1,0,0,0,155,150,
  	1,0,0,0,156,13,1,0,0,0,157,162,3,16,8,0,158,159,5,4,0,0,159,161,3,16,
  	8,0,160,158,1,0,0,0,161,164,1,0,0,0,162,160,1,0,0,0,162,163,1,0,0,0,163,
  	15,1,0,0,0,164,162,1,0,0,0,165,168,5,85,0,0,166,167,5,5,0,0,167,169,3,
  	64,32,0,168,166,1,0,0,0,168,169,1,0,0,0,169,17,1,0,0,0,170,171,5,3,0,
  	0,171,19,1,0,0,0,172,174,3,64,32,0,173,175,5,3,0,0,174,173,1,0,0,0,174,
  	175,1,0,0,0,175,21,1,0,0,0,176,177,5,66,0,0,177,178,5,6,0,0,178,179,3,
  	62,31,0,179,180,5,7,0,0,180,183,3,6,3,0,181,182,5,62,0,0,182,184,3,6,
  	3,0,183,181,1,0,0,0,183,184,1,0,0,0,184,23,1,0,0,0,185,186,5,64,0,0,186,
  	188,5,6,0,0,187,189,3,62,31,0,188,187,1,0,0,0,188,189,1,0,0,0,189,190,
  	1,0,0,0,190,192,5,3,0,0,191,193,3,62,31,0,192,191,1,0,0,0,192,193,1,0,
  	0,0,193,194,1,0,0,0,194,196,5,3,0,0,195,197,3,62,31,0,196,195,1,0,0,0,
  	196,197,1,0,0,0,197,198,1,0,0,0,198,199,5,7,0,0,199,248,3,6,3,0,200,201,
  	5,64,0,0,201,202,5,6,0,0,202,203,5,76,0,0,203,204,3,14,7,0,204,206,5,
  	3,0,0,205,207,3,62,31,0,206,205,1,0,0,0,206,207,1,0,0,0,207,208,1,0,0,
  	0,208,210,5,3,0,0,209,211,3,62,31,0,210,209,1,0,0,0,210,211,1,0,0,0,211,
  	212,1,0,0,0,212,213,5,7,0,0,213,214,3,6,3,0,214,248,1,0,0,0,215,216,5,
  	64,0,0,216,217,5,6,0,0,217,218,3,64,32,0,218,219,5,67,0,0,219,220,3,62,
  	31,0,220,221,5,7,0,0,221,222,3,6,3,0,222,248,1,0,0,0,223,224,5,64,0,0,
  	224,225,5,6,0,0,225,226,5,76,0,0,226,227,3,16,8,0,227,228,5,67,0,0,228,
  	229,3,62,31,0,229,230,5,7,0,0,230,231,3,6,3,0,231,248,1,0,0,0,232,233,
  	5,78,0,0,233,234,5,6,0,0,234,235,3,62,31,0,235,236,5,7,0,0,236,237,3,
  	6,3,0,237,248,1,0,0,0,238,239,5,61,0,0,239,240,3,6,3,0,240,241,5,78,0,
  	0,241,242,5,6,0,0,242,243,3,62,31,0,243,245,5,7,0,0,244,246,5,3,0,0,245,
  	244,1,0,0,0,245,246,1,0,0,0,246,248,1,0,0,0,247,185,1,0,0,0,247,200,1,
  	0,0,0,247,215,1,0,0,0,247,223,1,0,0,0,247,232,1,0,0,0,247,238,1,0,0,0,
  	248,25,1,0,0,0,249,251,5,57,0,0,250,252,5,85,0,0,251,250,1,0,0,0,251,
  	252,1,0,0,0,252,254,1,0,0,0,253,255,5,3,0,0,254,253,1,0,0,0,254,255,1,
  	0,0,0,255,27,1,0,0,0,256,258,5,54,0,0,257,259,5,85,0,0,258,257,1,0,0,
  	0,258,259,1,0,0,0,259,261,1,0,0,0,260,262,5,3,0,0,261,260,1,0,0,0,261,
  	262,1,0,0,0,262,29,1,0,0,0,263,265,5,70,0,0,264,266,3,62,31,0,265,264,
  	1,0,0,0,265,266,1,0,0,0,266,268,1,0,0,0,267,269,5,3,0,0,268,267,1,0,0,
  	0,268,269,1,0,0,0,269,31,1,0,0,0,270,271,5,79,0,0,271,272,5,6,0,0,272,
  	273,3,62,31,0,273,274,5,7,0,0,274,275,3,6,3,0,275,33,1,0,0,0,276,277,
  	5,71,0,0,277,278,5,6,0,0,278,279,3,62,31,0,279,280,5,7,0,0,280,281,3,
  	36,18,0,281,35,1,0,0,0,282,284,5,1,0,0,283,285,3,38,19,0,284,283,1,0,
  	0,0,284,285,1,0,0,0,285,290,1,0,0,0,286,288,3,42,21,0,287,289,3,38,19,
  	0,288,287,1,0,0,0,288,289,1,0,0,0,289,291,1,0,0,0,290,286,1,0,0,0,290,
  	291,1,0,0,0,291,292,1,0,0,0,292,293,5,2,0,0,293,37,1,0,0,0,294,296,3,
  	40,20,0,295,294,1,0,0,0,296,297,1,0,0,0,297,295,1,0,0,0,297,298,1,0,0,
  	0,298,39,1,0,0,0,299,300,5,55,0,0,300,301,3,62,31,0,301,303,5,8,0,0,302,
  	304,3,10,5,0,303,302,1,0,0,0,303,304,1,0,0,0,304,41,1,0,0,0,305,306,5,
  	59,0,0,306,308,5,8,0,0,307,309,3,10,5,0,308,307,1,0,0,0,308,309,1,0,0,
  	0,309,43,1,0,0,0,310,311,5,85,0,0,311,312,5,8,0,0,312,313,3,6,3,0,313,
  	45,1,0,0,0,314,315,5,73,0,0,315,317,3,62,31,0,316,318,5,3,0,0,317,316,
  	1,0,0,0,317,318,1,0,0,0,318,47,1,0,0,0,319,320,5,74,0,0,320,326,3,8,4,
  	0,321,323,3,50,25,0,322,324,3,52,26,0,323,322,1,0,0,0,323,324,1,0,0,0,
  	324,327,1,0,0,0,325,327,3,52,26,0,326,321,1,0,0,0,326,325,1,0,0,0,327,
  	49,1,0,0,0,328,329,5,56,0,0,329,330,5,6,0,0,330,331,5,85,0,0,331,332,
  	5,7,0,0,332,333,3,8,4,0,333,51,1,0,0,0,334,335,5,63,0,0,335,336,3,8,4,
  	0,336,53,1,0,0,0,337,339,5,58,0,0,338,340,5,3,0,0,339,338,1,0,0,0,339,
  	340,1,0,0,0,340,55,1,0,0,0,341,342,5,65,0,0,342,343,5,85,0,0,343,345,
  	5,6,0,0,344,346,3,58,29,0,345,344,1,0,0,0,345,346,1,0,0,0,346,347,1,0,
  	0,0,347,348,5,7,0,0,348,349,5,1,0,0,349,350,3,60,30,0,350,351,5,2,0,0,
  	351,57,1,0,0,0,352,357,5,85,0,0,353,354,5,4,0,0,354,356,5,85,0,0,355,
  	353,1,0,0,0,356,359,1,0,0,0,357,355,1,0,0,0,357,358,1,0,0,0,358,59,1,
  	0,0,0,359,357,1,0,0,0,360,362,3,2,1,0,361,360,1,0,0,0,361,362,1,0,0,0,
  	362,61,1,0,0,0,363,368,3,64,32,0,364,365,5,4,0,0,365,367,3,64,32,0,366,
  	364,1,0,0,0,367,370,1,0,0,0,368,366,1,0,0,0,368,369,1,0,0,0,369,63,1,
  	0,0,0,370,368,1,0,0,0,371,372,6,32,-1,0,372,374,5,65,0,0,373,375,5,85,
  	0,0,374,373,1,0,0,0,374,375,1,0,0,0,375,376,1,0,0,0,376,378,5,6,0,0,377,
  	379,3,58,29,0,378,377,1,0,0,0,378,379,1,0,0,0,379,380,1,0,0,0,380,381,
  	5,7,0,0,381,382,5,1,0,0,382,383,3,60,30,0,383,384,5,2,0,0,384,418,1,0,
  	0,0,385,386,5,69,0,0,386,388,3,64,32,0,387,389,3,94,47,0,388,387,1,0,
  	0,0,388,389,1,0,0,0,389,418,1,0,0,0,390,391,5,60,0,0,391,418,3,64,32,
  	30,392,393,5,77,0,0,393,418,3,64,32,29,394,395,5,75,0,0,395,418,3,64,
  	32,28,396,397,5,12,0,0,397,418,3,64,32,27,398,399,5,13,0,0,399,418,3,
  	64,32,26,400,401,5,14,0,0,401,418,3,64,32,25,402,403,5,15,0,0,403,418,
  	3,64,32,24,404,405,5,16,0,0,405,418,3,64,32,23,406,407,5,17,0,0,407,418,
  	3,64,32,22,408,418,5,72,0,0,409,418,5,85,0,0,410,418,3,68,34,0,411,418,
  	3,78,39,0,412,418,3,84,42,0,413,414,5,6,0,0,414,415,3,62,31,0,415,416,
  	5,7,0,0,416,418,1,0,0,0,417,371,1,0,0,0,417,385,1,0,0,0,417,390,1,0,0,
  	0,417,392,1,0,0,0,417,394,1,0,0,0,417,396,1,0,0,0,417,398,1,0,0,0,417,
  	400,1,0,0,0,417,402,1,0,0,0,417,404,1,0,0,0,417,406,1,0,0,0,417,408,1,
  	0,0,0,417,409,1,0,0,0,417,410,1,0,0,0,417,411,1,0,0,0,417,412,1,0,0,0,
  	417,413,1,0,0,0,418,484,1,0,0,0,419,420,10,21,0,0,420,421,7,0,0,0,421,
  	483,3,64,32,22,422,423,10,20,0,0,423,424,7,1,0,0,424,483,3,64,32,21,425,
  	426,10,19,0,0,426,427,7,2,0,0,427,483,3,64,32,20,428,429,10,18,0,0,429,
  	430,7,3,0,0,430,483,3,64,32,19,431,432,10,17,0,0,432,433,5,68,0,0,433,
  	483,3,64,32,18,434,435,10,16,0,0,435,436,5,67,0,0,436,483,3,64,32,17,
  	437,438,10,15,0,0,438,439,7,4,0,0,439,483,3,64,32,16,440,441,10,14,0,
  	0,441,442,5,32,0,0,442,483,3,64,32,15,443,444,10,13,0,0,444,445,5,33,
  	0,0,445,483,3,64,32,14,446,447,10,12,0,0,447,448,5,34,0,0,448,483,3,64,
  	32,13,449,450,10,11,0,0,450,451,5,35,0,0,451,483,3,64,32,12,452,453,10,
  	10,0,0,453,454,5,36,0,0,454,483,3,64,32,11,455,456,10,9,0,0,456,457,5,
  	37,0,0,457,458,3,64,32,0,458,459,5,8,0,0,459,460,3,64,32,10,460,483,1,
  	0,0,0,461,462,10,8,0,0,462,463,5,5,0,0,463,483,3,64,32,9,464,465,10,7,
  	0,0,465,466,3,66,33,0,466,467,3,64,32,8,467,483,1,0,0,0,468,469,10,36,
  	0,0,469,470,5,9,0,0,470,471,3,62,31,0,471,472,5,10,0,0,472,483,1,0,0,
  	0,473,474,10,35,0,0,474,475,5,11,0,0,475,483,3,72,36,0,476,477,10,34,
  	0,0,477,483,3,94,47,0,478,479,10,32,0,0,479,483,5,12,0,0,480,481,10,31,
  	0,0,481,483,5,13,0,0,482,419,1,0,0,0,482,422,1,0,0,0,482,425,1,0,0,0,
  	482,428,1,0,0,0,482,431,1,0,0,0,482,434,1,0,0,0,482,437,1,0,0,0,482,440,
  	1,0,0,0,482,443,1,0,0,0,482,446,1,0,0,0,482,449,1,0,0,0,482,452,1,0,0,
  	0,482,455,1,0,0,0,482,461,1,0,0,0,482,464,1,0,0,0,482,468,1,0,0,0,482,
  	473,1,0,0,0,482,476,1,0,0,0,482,478,1,0,0,0,482,480,1,0,0,0,483,486,1,
  	0,0,0,484,482,1,0,0,0,484,485,1,0,0,0,485,65,1,0,0,0,486,484,1,0,0,0,
  	487,488,7,5,0,0,488,67,1,0,0,0,489,494,5,49,0,0,490,494,5,50,0,0,491,
  	494,5,84,0,0,492,494,3,70,35,0,493,489,1,0,0,0,493,490,1,0,0,0,493,491,
  	1,0,0,0,493,492,1,0,0,0,494,69,1,0,0,0,495,496,7,6,0,0,496,71,1,0,0,0,
  	497,500,5,85,0,0,498,500,3,74,37,0,499,497,1,0,0,0,499,498,1,0,0,0,500,
  	73,1,0,0,0,501,505,3,76,38,0,502,505,5,49,0,0,503,505,5,50,0,0,504,501,
  	1,0,0,0,504,502,1,0,0,0,504,503,1,0,0,0,505,75,1,0,0,0,506,507,7,7,0,
  	0,507,77,1,0,0,0,508,510,5,9,0,0,509,511,3,80,40,0,510,509,1,0,0,0,510,
  	511,1,0,0,0,511,512,1,0,0,0,512,513,5,10,0,0,513,79,1,0,0,0,514,516,3,
  	82,41,0,515,514,1,0,0,0,515,516,1,0,0,0,516,517,1,0,0,0,517,523,3,64,
  	32,0,518,519,3,82,41,0,519,520,3,64,32,0,520,522,1,0,0,0,521,518,1,0,
  	0,0,522,525,1,0,0,0,523,521,1,0,0,0,523,524,1,0,0,0,524,81,1,0,0,0,525,
  	523,1,0,0,0,526,527,5,4,0,0,527,83,1,0,0,0,528,530,5,1,0,0,529,531,3,
  	86,43,0,530,529,1,0,0,0,530,531,1,0,0,0,531,533,1,0,0,0,532,534,5,4,0,
  	0,533,532,1,0,0,0,533,534,1,0,0,0,534,535,1,0,0,0,535,536,5,2,0,0,536,
  	85,1,0,0,0,537,542,3,88,44,0,538,539,5,4,0,0,539,541,3,88,44,0,540,538,
  	1,0,0,0,541,544,1,0,0,0,542,540,1,0,0,0,542,543,1,0,0,0,543,87,1,0,0,
  	0,544,542,1,0,0,0,545,546,3,90,45,0,546,547,5,8,0,0,547,548,3,64,32,0,
  	548,567,1,0,0,0,549,550,5,82,0,0,550,551,3,90,45,0,551,552,5,6,0,0,552,
  	553,5,7,0,0,553,554,5,1,0,0,554,555,3,60,30,0,555,556,5,2,0,0,556,567,
  	1,0,0,0,557,558,5,83,0,0,558,559,3,90,45,0,559,560,5,6,0,0,560,561,3,
  	92,46,0,561,562,5,7,0,0,562,563,5,1,0,0,563,564,3,60,30,0,564,565,5,2,
  	0,0,565,567,1,0,0,0,566,545,1,0,0,0,566,549,1,0,0,0,566,557,1,0,0,0,567,
  	89,1,0,0,0,568,572,3,72,36,0,569,572,5,84,0,0,570,572,3,70,35,0,571,568,
  	1,0,0,0,571,569,1,0,0,0,571,570,1,0,0,0,572,91,1,0,0,0,573,574,5,85,0,
  	0,574,93,1,0,0,0,575,577,5,6,0,0,576,578,3,96,48,0,577,576,1,0,0,0,577,
  	578,1,0,0,0,578,579,1,0,0,0,579,580,5,7,0,0,580,95,1,0,0,0,581,586,3,
  	64,32,0,582,583,5,4,0,0,583,585,3,64,32,0,584,582,1,0,0,0,585,588,1,0,
  	0,0,586,584,1,0,0,0,586,587,1,0,0,0,587,97,1,0,0,0,588,586,1,0,0,0,60,
  	99,106,110,127,131,138,143,148,153,155,162,168,174,183,188,192,196,206,
  	210,245,247,251,254,258,261,265,268,284,288,290,297,303,308,317,323,326,
  	339,345,357,361,368,374,378,388,417,482,484,493,499,504,510,515,523,530,
  	533,542,566,571,577,586
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  javascriptParserStaticData = std::move(staticData);
}

}

JavaScriptParser::JavaScriptParser(TokenStream *input) : JavaScriptParser(input, antlr4::atn::ParserATNSimulatorOptions()) {}

JavaScriptParser::JavaScriptParser(TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options) : Parser(input) {
  JavaScriptParser::initialize();
  _interpreter = new atn::ParserATNSimulator(this, *javascriptParserStaticData->atn, javascriptParserStaticData->decisionToDFA, javascriptParserStaticData->sharedContextCache, options);
}

JavaScriptParser::~JavaScriptParser() {
  delete _interpreter;
}

const atn::ATN& JavaScriptParser::getATN() const {
  return *javascriptParserStaticData->atn;
}

std::string JavaScriptParser::getGrammarFileName() const {
  return "JavaScript.g4";
}

const std::vector<std::string>& JavaScriptParser::getRuleNames() const {
  return javascriptParserStaticData->ruleNames;
}

const dfa::Vocabulary& JavaScriptParser::getVocabulary() const {
  return javascriptParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView JavaScriptParser::getSerializedATN() const {
  return javascriptParserStaticData->serializedATN;
}


//----------------- ProgramContext ------------------------------------------------------------------

JavaScriptParser::ProgramContext::ProgramContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::ProgramContext::EOF() {
  return getToken(JavaScriptParser::EOF, 0);
}

JavaScriptParser::SourceElementsContext* JavaScriptParser::ProgramContext::sourceElements() {
  return getRuleContext<JavaScriptParser::SourceElementsContext>(0);
}


size_t JavaScriptParser::ProgramContext::getRuleIndex() const {
  return JavaScriptParser::RuleProgram;
}

void JavaScriptParser::ProgramContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterProgram(this);
}

void JavaScriptParser::ProgramContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitProgram(this);
}

JavaScriptParser::ProgramContext* JavaScriptParser::program() {
  ProgramContext *_localctx = _tracker.createInstance<ProgramContext>(_ctx, getState());
  enterRule(_localctx, 0, JavaScriptParser::RuleProgram);
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
    setState(99);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 3926575925113909834) != 0) || ((((_la - 64) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 64)) & 3407847) != 0)) {
      setState(98);
      sourceElements();
    }
    setState(101);
    match(JavaScriptParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- SourceElementsContext ------------------------------------------------------------------

JavaScriptParser::SourceElementsContext::SourceElementsContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<JavaScriptParser::SourceElementContext *> JavaScriptParser::SourceElementsContext::sourceElement() {
  return getRuleContexts<JavaScriptParser::SourceElementContext>();
}

JavaScriptParser::SourceElementContext* JavaScriptParser::SourceElementsContext::sourceElement(size_t i) {
  return getRuleContext<JavaScriptParser::SourceElementContext>(i);
}


size_t JavaScriptParser::SourceElementsContext::getRuleIndex() const {
  return JavaScriptParser::RuleSourceElements;
}

void JavaScriptParser::SourceElementsContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSourceElements(this);
}

void JavaScriptParser::SourceElementsContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSourceElements(this);
}

JavaScriptParser::SourceElementsContext* JavaScriptParser::sourceElements() {
  SourceElementsContext *_localctx = _tracker.createInstance<SourceElementsContext>(_ctx, getState());
  enterRule(_localctx, 2, JavaScriptParser::RuleSourceElements);
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
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(103);
      sourceElement();
      setState(106); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 3926575925113909834) != 0) || ((((_la - 64) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 64)) & 3407847) != 0));
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- SourceElementContext ------------------------------------------------------------------

JavaScriptParser::SourceElementContext::SourceElementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

JavaScriptParser::StatementContext* JavaScriptParser::SourceElementContext::statement() {
  return getRuleContext<JavaScriptParser::StatementContext>(0);
}

JavaScriptParser::FunctionDeclarationContext* JavaScriptParser::SourceElementContext::functionDeclaration() {
  return getRuleContext<JavaScriptParser::FunctionDeclarationContext>(0);
}


size_t JavaScriptParser::SourceElementContext::getRuleIndex() const {
  return JavaScriptParser::RuleSourceElement;
}

void JavaScriptParser::SourceElementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSourceElement(this);
}

void JavaScriptParser::SourceElementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSourceElement(this);
}

JavaScriptParser::SourceElementContext* JavaScriptParser::sourceElement() {
  SourceElementContext *_localctx = _tracker.createInstance<SourceElementContext>(_ctx, getState());
  enterRule(_localctx, 4, JavaScriptParser::RuleSourceElement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(110);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 2, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(108);
      statement();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(109);
      functionDeclaration();
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

//----------------- StatementContext ------------------------------------------------------------------

JavaScriptParser::StatementContext::StatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

JavaScriptParser::BlockContext* JavaScriptParser::StatementContext::block() {
  return getRuleContext<JavaScriptParser::BlockContext>(0);
}

JavaScriptParser::VariableStatementContext* JavaScriptParser::StatementContext::variableStatement() {
  return getRuleContext<JavaScriptParser::VariableStatementContext>(0);
}

JavaScriptParser::EmptyStatementContext* JavaScriptParser::StatementContext::emptyStatement() {
  return getRuleContext<JavaScriptParser::EmptyStatementContext>(0);
}

JavaScriptParser::ExpressionStatementContext* JavaScriptParser::StatementContext::expressionStatement() {
  return getRuleContext<JavaScriptParser::ExpressionStatementContext>(0);
}

JavaScriptParser::IfStatementContext* JavaScriptParser::StatementContext::ifStatement() {
  return getRuleContext<JavaScriptParser::IfStatementContext>(0);
}

JavaScriptParser::IterationStatementContext* JavaScriptParser::StatementContext::iterationStatement() {
  return getRuleContext<JavaScriptParser::IterationStatementContext>(0);
}

JavaScriptParser::ContinueStatementContext* JavaScriptParser::StatementContext::continueStatement() {
  return getRuleContext<JavaScriptParser::ContinueStatementContext>(0);
}

JavaScriptParser::BreakStatementContext* JavaScriptParser::StatementContext::breakStatement() {
  return getRuleContext<JavaScriptParser::BreakStatementContext>(0);
}

JavaScriptParser::ReturnStatementContext* JavaScriptParser::StatementContext::returnStatement() {
  return getRuleContext<JavaScriptParser::ReturnStatementContext>(0);
}

JavaScriptParser::WithStatementContext* JavaScriptParser::StatementContext::withStatement() {
  return getRuleContext<JavaScriptParser::WithStatementContext>(0);
}

JavaScriptParser::LabelledStatementContext* JavaScriptParser::StatementContext::labelledStatement() {
  return getRuleContext<JavaScriptParser::LabelledStatementContext>(0);
}

JavaScriptParser::SwitchStatementContext* JavaScriptParser::StatementContext::switchStatement() {
  return getRuleContext<JavaScriptParser::SwitchStatementContext>(0);
}

JavaScriptParser::ThrowStatementContext* JavaScriptParser::StatementContext::throwStatement() {
  return getRuleContext<JavaScriptParser::ThrowStatementContext>(0);
}

JavaScriptParser::TryStatementContext* JavaScriptParser::StatementContext::tryStatement() {
  return getRuleContext<JavaScriptParser::TryStatementContext>(0);
}

JavaScriptParser::DebuggerStatementContext* JavaScriptParser::StatementContext::debuggerStatement() {
  return getRuleContext<JavaScriptParser::DebuggerStatementContext>(0);
}


size_t JavaScriptParser::StatementContext::getRuleIndex() const {
  return JavaScriptParser::RuleStatement;
}

void JavaScriptParser::StatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStatement(this);
}

void JavaScriptParser::StatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStatement(this);
}

JavaScriptParser::StatementContext* JavaScriptParser::statement() {
  StatementContext *_localctx = _tracker.createInstance<StatementContext>(_ctx, getState());
  enterRule(_localctx, 6, JavaScriptParser::RuleStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(127);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 3, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(112);
      block();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(113);
      variableStatement();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(114);
      emptyStatement();
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(115);
      expressionStatement();
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(116);
      ifStatement();
      break;
    }

    case 6: {
      enterOuterAlt(_localctx, 6);
      setState(117);
      iterationStatement();
      break;
    }

    case 7: {
      enterOuterAlt(_localctx, 7);
      setState(118);
      continueStatement();
      break;
    }

    case 8: {
      enterOuterAlt(_localctx, 8);
      setState(119);
      breakStatement();
      break;
    }

    case 9: {
      enterOuterAlt(_localctx, 9);
      setState(120);
      returnStatement();
      break;
    }

    case 10: {
      enterOuterAlt(_localctx, 10);
      setState(121);
      withStatement();
      break;
    }

    case 11: {
      enterOuterAlt(_localctx, 11);
      setState(122);
      labelledStatement();
      break;
    }

    case 12: {
      enterOuterAlt(_localctx, 12);
      setState(123);
      switchStatement();
      break;
    }

    case 13: {
      enterOuterAlt(_localctx, 13);
      setState(124);
      throwStatement();
      break;
    }

    case 14: {
      enterOuterAlt(_localctx, 14);
      setState(125);
      tryStatement();
      break;
    }

    case 15: {
      enterOuterAlt(_localctx, 15);
      setState(126);
      debuggerStatement();
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

//----------------- BlockContext ------------------------------------------------------------------

JavaScriptParser::BlockContext::BlockContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

JavaScriptParser::StatementListContext* JavaScriptParser::BlockContext::statementList() {
  return getRuleContext<JavaScriptParser::StatementListContext>(0);
}


size_t JavaScriptParser::BlockContext::getRuleIndex() const {
  return JavaScriptParser::RuleBlock;
}

void JavaScriptParser::BlockContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBlock(this);
}

void JavaScriptParser::BlockContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBlock(this);
}

JavaScriptParser::BlockContext* JavaScriptParser::block() {
  BlockContext *_localctx = _tracker.createInstance<BlockContext>(_ctx, getState());
  enterRule(_localctx, 8, JavaScriptParser::RuleBlock);
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
    setState(129);
    match(JavaScriptParser::T__0);
    setState(131);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 3926575925113909834) != 0) || ((((_la - 64) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 64)) & 3407847) != 0)) {
      setState(130);
      statementList();
    }
    setState(133);
    match(JavaScriptParser::T__1);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StatementListContext ------------------------------------------------------------------

JavaScriptParser::StatementListContext::StatementListContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<JavaScriptParser::StatementContext *> JavaScriptParser::StatementListContext::statement() {
  return getRuleContexts<JavaScriptParser::StatementContext>();
}

JavaScriptParser::StatementContext* JavaScriptParser::StatementListContext::statement(size_t i) {
  return getRuleContext<JavaScriptParser::StatementContext>(i);
}


size_t JavaScriptParser::StatementListContext::getRuleIndex() const {
  return JavaScriptParser::RuleStatementList;
}

void JavaScriptParser::StatementListContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStatementList(this);
}

void JavaScriptParser::StatementListContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStatementList(this);
}

JavaScriptParser::StatementListContext* JavaScriptParser::statementList() {
  StatementListContext *_localctx = _tracker.createInstance<StatementListContext>(_ctx, getState());
  enterRule(_localctx, 10, JavaScriptParser::RuleStatementList);
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
    setState(136); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(135);
      statement();
      setState(138); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 3926575925113909834) != 0) || ((((_la - 64) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 64)) & 3407847) != 0));
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- VariableStatementContext ------------------------------------------------------------------

JavaScriptParser::VariableStatementContext::VariableStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::VariableStatementContext::VAR() {
  return getToken(JavaScriptParser::VAR, 0);
}

JavaScriptParser::VariableDeclarationListContext* JavaScriptParser::VariableStatementContext::variableDeclarationList() {
  return getRuleContext<JavaScriptParser::VariableDeclarationListContext>(0);
}

tree::TerminalNode* JavaScriptParser::VariableStatementContext::LET() {
  return getToken(JavaScriptParser::LET, 0);
}

tree::TerminalNode* JavaScriptParser::VariableStatementContext::CONST() {
  return getToken(JavaScriptParser::CONST, 0);
}


size_t JavaScriptParser::VariableStatementContext::getRuleIndex() const {
  return JavaScriptParser::RuleVariableStatement;
}

void JavaScriptParser::VariableStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterVariableStatement(this);
}

void JavaScriptParser::VariableStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitVariableStatement(this);
}

JavaScriptParser::VariableStatementContext* JavaScriptParser::variableStatement() {
  VariableStatementContext *_localctx = _tracker.createInstance<VariableStatementContext>(_ctx, getState());
  enterRule(_localctx, 12, JavaScriptParser::RuleVariableStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(155);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case JavaScriptParser::VAR: {
        enterOuterAlt(_localctx, 1);
        setState(140);
        match(JavaScriptParser::VAR);
        setState(141);
        variableDeclarationList();
        setState(143);
        _errHandler->sync(this);

        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 6, _ctx)) {
        case 1: {
          setState(142);
          match(JavaScriptParser::T__2);
          break;
        }

        default:
          break;
        }
        break;
      }

      case JavaScriptParser::LET: {
        enterOuterAlt(_localctx, 2);
        setState(145);
        match(JavaScriptParser::LET);
        setState(146);
        variableDeclarationList();
        setState(148);
        _errHandler->sync(this);

        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 7, _ctx)) {
        case 1: {
          setState(147);
          match(JavaScriptParser::T__2);
          break;
        }

        default:
          break;
        }
        break;
      }

      case JavaScriptParser::CONST: {
        enterOuterAlt(_localctx, 3);
        setState(150);
        match(JavaScriptParser::CONST);
        setState(151);
        variableDeclarationList();
        setState(153);
        _errHandler->sync(this);

        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 8, _ctx)) {
        case 1: {
          setState(152);
          match(JavaScriptParser::T__2);
          break;
        }

        default:
          break;
        }
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

//----------------- VariableDeclarationListContext ------------------------------------------------------------------

JavaScriptParser::VariableDeclarationListContext::VariableDeclarationListContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<JavaScriptParser::VariableDeclarationContext *> JavaScriptParser::VariableDeclarationListContext::variableDeclaration() {
  return getRuleContexts<JavaScriptParser::VariableDeclarationContext>();
}

JavaScriptParser::VariableDeclarationContext* JavaScriptParser::VariableDeclarationListContext::variableDeclaration(size_t i) {
  return getRuleContext<JavaScriptParser::VariableDeclarationContext>(i);
}


size_t JavaScriptParser::VariableDeclarationListContext::getRuleIndex() const {
  return JavaScriptParser::RuleVariableDeclarationList;
}

void JavaScriptParser::VariableDeclarationListContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterVariableDeclarationList(this);
}

void JavaScriptParser::VariableDeclarationListContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitVariableDeclarationList(this);
}

JavaScriptParser::VariableDeclarationListContext* JavaScriptParser::variableDeclarationList() {
  VariableDeclarationListContext *_localctx = _tracker.createInstance<VariableDeclarationListContext>(_ctx, getState());
  enterRule(_localctx, 14, JavaScriptParser::RuleVariableDeclarationList);
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
    setState(157);
    variableDeclaration();
    setState(162);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == JavaScriptParser::T__3) {
      setState(158);
      match(JavaScriptParser::T__3);
      setState(159);
      variableDeclaration();
      setState(164);
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

//----------------- VariableDeclarationContext ------------------------------------------------------------------

JavaScriptParser::VariableDeclarationContext::VariableDeclarationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::VariableDeclarationContext::Identifier() {
  return getToken(JavaScriptParser::Identifier, 0);
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::VariableDeclarationContext::singleExpression() {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(0);
}


size_t JavaScriptParser::VariableDeclarationContext::getRuleIndex() const {
  return JavaScriptParser::RuleVariableDeclaration;
}

void JavaScriptParser::VariableDeclarationContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterVariableDeclaration(this);
}

void JavaScriptParser::VariableDeclarationContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitVariableDeclaration(this);
}

JavaScriptParser::VariableDeclarationContext* JavaScriptParser::variableDeclaration() {
  VariableDeclarationContext *_localctx = _tracker.createInstance<VariableDeclarationContext>(_ctx, getState());
  enterRule(_localctx, 16, JavaScriptParser::RuleVariableDeclaration);
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
    setState(165);
    match(JavaScriptParser::Identifier);
    setState(168);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == JavaScriptParser::T__4) {
      setState(166);
      match(JavaScriptParser::T__4);
      setState(167);
      singleExpression(0);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- EmptyStatementContext ------------------------------------------------------------------

JavaScriptParser::EmptyStatementContext::EmptyStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t JavaScriptParser::EmptyStatementContext::getRuleIndex() const {
  return JavaScriptParser::RuleEmptyStatement;
}

void JavaScriptParser::EmptyStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterEmptyStatement(this);
}

void JavaScriptParser::EmptyStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitEmptyStatement(this);
}

JavaScriptParser::EmptyStatementContext* JavaScriptParser::emptyStatement() {
  EmptyStatementContext *_localctx = _tracker.createInstance<EmptyStatementContext>(_ctx, getState());
  enterRule(_localctx, 18, JavaScriptParser::RuleEmptyStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(170);
    match(JavaScriptParser::T__2);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExpressionStatementContext ------------------------------------------------------------------

JavaScriptParser::ExpressionStatementContext::ExpressionStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::ExpressionStatementContext::singleExpression() {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(0);
}


size_t JavaScriptParser::ExpressionStatementContext::getRuleIndex() const {
  return JavaScriptParser::RuleExpressionStatement;
}

void JavaScriptParser::ExpressionStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterExpressionStatement(this);
}

void JavaScriptParser::ExpressionStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitExpressionStatement(this);
}

JavaScriptParser::ExpressionStatementContext* JavaScriptParser::expressionStatement() {
  ExpressionStatementContext *_localctx = _tracker.createInstance<ExpressionStatementContext>(_ctx, getState());
  enterRule(_localctx, 20, JavaScriptParser::RuleExpressionStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(172);
    singleExpression(0);
    setState(174);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 12, _ctx)) {
    case 1: {
      setState(173);
      match(JavaScriptParser::T__2);
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

JavaScriptParser::IfStatementContext::IfStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::IfStatementContext::IF() {
  return getToken(JavaScriptParser::IF, 0);
}

JavaScriptParser::ExpressionSequenceContext* JavaScriptParser::IfStatementContext::expressionSequence() {
  return getRuleContext<JavaScriptParser::ExpressionSequenceContext>(0);
}

std::vector<JavaScriptParser::StatementContext *> JavaScriptParser::IfStatementContext::statement() {
  return getRuleContexts<JavaScriptParser::StatementContext>();
}

JavaScriptParser::StatementContext* JavaScriptParser::IfStatementContext::statement(size_t i) {
  return getRuleContext<JavaScriptParser::StatementContext>(i);
}

tree::TerminalNode* JavaScriptParser::IfStatementContext::ELSE() {
  return getToken(JavaScriptParser::ELSE, 0);
}


size_t JavaScriptParser::IfStatementContext::getRuleIndex() const {
  return JavaScriptParser::RuleIfStatement;
}

void JavaScriptParser::IfStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterIfStatement(this);
}

void JavaScriptParser::IfStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitIfStatement(this);
}

JavaScriptParser::IfStatementContext* JavaScriptParser::ifStatement() {
  IfStatementContext *_localctx = _tracker.createInstance<IfStatementContext>(_ctx, getState());
  enterRule(_localctx, 22, JavaScriptParser::RuleIfStatement);

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
    match(JavaScriptParser::IF);
    setState(177);
    match(JavaScriptParser::T__5);
    setState(178);
    expressionSequence();
    setState(179);
    match(JavaScriptParser::T__6);
    setState(180);
    statement();
    setState(183);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 13, _ctx)) {
    case 1: {
      setState(181);
      match(JavaScriptParser::ELSE);
      setState(182);
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

//----------------- IterationStatementContext ------------------------------------------------------------------

JavaScriptParser::IterationStatementContext::IterationStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::IterationStatementContext::FOR() {
  return getToken(JavaScriptParser::FOR, 0);
}

JavaScriptParser::StatementContext* JavaScriptParser::IterationStatementContext::statement() {
  return getRuleContext<JavaScriptParser::StatementContext>(0);
}

std::vector<JavaScriptParser::ExpressionSequenceContext *> JavaScriptParser::IterationStatementContext::expressionSequence() {
  return getRuleContexts<JavaScriptParser::ExpressionSequenceContext>();
}

JavaScriptParser::ExpressionSequenceContext* JavaScriptParser::IterationStatementContext::expressionSequence(size_t i) {
  return getRuleContext<JavaScriptParser::ExpressionSequenceContext>(i);
}

tree::TerminalNode* JavaScriptParser::IterationStatementContext::VAR() {
  return getToken(JavaScriptParser::VAR, 0);
}

JavaScriptParser::VariableDeclarationListContext* JavaScriptParser::IterationStatementContext::variableDeclarationList() {
  return getRuleContext<JavaScriptParser::VariableDeclarationListContext>(0);
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::IterationStatementContext::singleExpression() {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(0);
}

tree::TerminalNode* JavaScriptParser::IterationStatementContext::IN() {
  return getToken(JavaScriptParser::IN, 0);
}

JavaScriptParser::VariableDeclarationContext* JavaScriptParser::IterationStatementContext::variableDeclaration() {
  return getRuleContext<JavaScriptParser::VariableDeclarationContext>(0);
}

tree::TerminalNode* JavaScriptParser::IterationStatementContext::WHILE() {
  return getToken(JavaScriptParser::WHILE, 0);
}

tree::TerminalNode* JavaScriptParser::IterationStatementContext::DO() {
  return getToken(JavaScriptParser::DO, 0);
}


size_t JavaScriptParser::IterationStatementContext::getRuleIndex() const {
  return JavaScriptParser::RuleIterationStatement;
}

void JavaScriptParser::IterationStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterIterationStatement(this);
}

void JavaScriptParser::IterationStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitIterationStatement(this);
}

JavaScriptParser::IterationStatementContext* JavaScriptParser::iterationStatement() {
  IterationStatementContext *_localctx = _tracker.createInstance<IterationStatementContext>(_ctx, getState());
  enterRule(_localctx, 24, JavaScriptParser::RuleIterationStatement);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(247);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 20, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(185);
      match(JavaScriptParser::FOR);
      setState(186);
      match(JavaScriptParser::T__5);
      setState(188);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if ((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 1170372953163166274) != 0) || ((((_la - 65) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 65)) & 1578129) != 0)) {
        setState(187);
        expressionSequence();
      }
      setState(190);
      match(JavaScriptParser::T__2);
      setState(192);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if ((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 1170372953163166274) != 0) || ((((_la - 65) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 65)) & 1578129) != 0)) {
        setState(191);
        expressionSequence();
      }
      setState(194);
      match(JavaScriptParser::T__2);
      setState(196);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if ((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 1170372953163166274) != 0) || ((((_la - 65) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 65)) & 1578129) != 0)) {
        setState(195);
        expressionSequence();
      }
      setState(198);
      match(JavaScriptParser::T__6);
      setState(199);
      statement();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(200);
      match(JavaScriptParser::FOR);
      setState(201);
      match(JavaScriptParser::T__5);
      setState(202);
      match(JavaScriptParser::VAR);
      setState(203);
      variableDeclarationList();
      setState(204);
      match(JavaScriptParser::T__2);
      setState(206);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if ((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 1170372953163166274) != 0) || ((((_la - 65) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 65)) & 1578129) != 0)) {
        setState(205);
        expressionSequence();
      }
      setState(208);
      match(JavaScriptParser::T__2);
      setState(210);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if ((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 1170372953163166274) != 0) || ((((_la - 65) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 65)) & 1578129) != 0)) {
        setState(209);
        expressionSequence();
      }
      setState(212);
      match(JavaScriptParser::T__6);
      setState(213);
      statement();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(215);
      match(JavaScriptParser::FOR);
      setState(216);
      match(JavaScriptParser::T__5);
      setState(217);
      singleExpression(0);
      setState(218);
      match(JavaScriptParser::IN);
      setState(219);
      expressionSequence();
      setState(220);
      match(JavaScriptParser::T__6);
      setState(221);
      statement();
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(223);
      match(JavaScriptParser::FOR);
      setState(224);
      match(JavaScriptParser::T__5);
      setState(225);
      match(JavaScriptParser::VAR);
      setState(226);
      variableDeclaration();
      setState(227);
      match(JavaScriptParser::IN);
      setState(228);
      expressionSequence();
      setState(229);
      match(JavaScriptParser::T__6);
      setState(230);
      statement();
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(232);
      match(JavaScriptParser::WHILE);
      setState(233);
      match(JavaScriptParser::T__5);
      setState(234);
      expressionSequence();
      setState(235);
      match(JavaScriptParser::T__6);
      setState(236);
      statement();
      break;
    }

    case 6: {
      enterOuterAlt(_localctx, 6);
      setState(238);
      match(JavaScriptParser::DO);
      setState(239);
      statement();
      setState(240);
      match(JavaScriptParser::WHILE);
      setState(241);
      match(JavaScriptParser::T__5);
      setState(242);
      expressionSequence();
      setState(243);
      match(JavaScriptParser::T__6);
      setState(245);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 19, _ctx)) {
      case 1: {
        setState(244);
        match(JavaScriptParser::T__2);
        break;
      }

      default:
        break;
      }
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

//----------------- ContinueStatementContext ------------------------------------------------------------------

JavaScriptParser::ContinueStatementContext::ContinueStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::ContinueStatementContext::CONTINUE() {
  return getToken(JavaScriptParser::CONTINUE, 0);
}

tree::TerminalNode* JavaScriptParser::ContinueStatementContext::Identifier() {
  return getToken(JavaScriptParser::Identifier, 0);
}


size_t JavaScriptParser::ContinueStatementContext::getRuleIndex() const {
  return JavaScriptParser::RuleContinueStatement;
}

void JavaScriptParser::ContinueStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterContinueStatement(this);
}

void JavaScriptParser::ContinueStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitContinueStatement(this);
}

JavaScriptParser::ContinueStatementContext* JavaScriptParser::continueStatement() {
  ContinueStatementContext *_localctx = _tracker.createInstance<ContinueStatementContext>(_ctx, getState());
  enterRule(_localctx, 26, JavaScriptParser::RuleContinueStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(249);
    match(JavaScriptParser::CONTINUE);
    setState(251);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 21, _ctx)) {
    case 1: {
      setState(250);
      match(JavaScriptParser::Identifier);
      break;
    }

    default:
      break;
    }
    setState(254);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 22, _ctx)) {
    case 1: {
      setState(253);
      match(JavaScriptParser::T__2);
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

//----------------- BreakStatementContext ------------------------------------------------------------------

JavaScriptParser::BreakStatementContext::BreakStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::BreakStatementContext::BREAK() {
  return getToken(JavaScriptParser::BREAK, 0);
}

tree::TerminalNode* JavaScriptParser::BreakStatementContext::Identifier() {
  return getToken(JavaScriptParser::Identifier, 0);
}


size_t JavaScriptParser::BreakStatementContext::getRuleIndex() const {
  return JavaScriptParser::RuleBreakStatement;
}

void JavaScriptParser::BreakStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBreakStatement(this);
}

void JavaScriptParser::BreakStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBreakStatement(this);
}

JavaScriptParser::BreakStatementContext* JavaScriptParser::breakStatement() {
  BreakStatementContext *_localctx = _tracker.createInstance<BreakStatementContext>(_ctx, getState());
  enterRule(_localctx, 28, JavaScriptParser::RuleBreakStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(256);
    match(JavaScriptParser::BREAK);
    setState(258);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 23, _ctx)) {
    case 1: {
      setState(257);
      match(JavaScriptParser::Identifier);
      break;
    }

    default:
      break;
    }
    setState(261);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 24, _ctx)) {
    case 1: {
      setState(260);
      match(JavaScriptParser::T__2);
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

//----------------- ReturnStatementContext ------------------------------------------------------------------

JavaScriptParser::ReturnStatementContext::ReturnStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::ReturnStatementContext::RETURN() {
  return getToken(JavaScriptParser::RETURN, 0);
}

JavaScriptParser::ExpressionSequenceContext* JavaScriptParser::ReturnStatementContext::expressionSequence() {
  return getRuleContext<JavaScriptParser::ExpressionSequenceContext>(0);
}


size_t JavaScriptParser::ReturnStatementContext::getRuleIndex() const {
  return JavaScriptParser::RuleReturnStatement;
}

void JavaScriptParser::ReturnStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterReturnStatement(this);
}

void JavaScriptParser::ReturnStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitReturnStatement(this);
}

JavaScriptParser::ReturnStatementContext* JavaScriptParser::returnStatement() {
  ReturnStatementContext *_localctx = _tracker.createInstance<ReturnStatementContext>(_ctx, getState());
  enterRule(_localctx, 30, JavaScriptParser::RuleReturnStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(263);
    match(JavaScriptParser::RETURN);
    setState(265);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 25, _ctx)) {
    case 1: {
      setState(264);
      expressionSequence();
      break;
    }

    default:
      break;
    }
    setState(268);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 26, _ctx)) {
    case 1: {
      setState(267);
      match(JavaScriptParser::T__2);
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

//----------------- WithStatementContext ------------------------------------------------------------------

JavaScriptParser::WithStatementContext::WithStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::WithStatementContext::WITH() {
  return getToken(JavaScriptParser::WITH, 0);
}

JavaScriptParser::ExpressionSequenceContext* JavaScriptParser::WithStatementContext::expressionSequence() {
  return getRuleContext<JavaScriptParser::ExpressionSequenceContext>(0);
}

JavaScriptParser::StatementContext* JavaScriptParser::WithStatementContext::statement() {
  return getRuleContext<JavaScriptParser::StatementContext>(0);
}


size_t JavaScriptParser::WithStatementContext::getRuleIndex() const {
  return JavaScriptParser::RuleWithStatement;
}

void JavaScriptParser::WithStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterWithStatement(this);
}

void JavaScriptParser::WithStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitWithStatement(this);
}

JavaScriptParser::WithStatementContext* JavaScriptParser::withStatement() {
  WithStatementContext *_localctx = _tracker.createInstance<WithStatementContext>(_ctx, getState());
  enterRule(_localctx, 32, JavaScriptParser::RuleWithStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(270);
    match(JavaScriptParser::WITH);
    setState(271);
    match(JavaScriptParser::T__5);
    setState(272);
    expressionSequence();
    setState(273);
    match(JavaScriptParser::T__6);
    setState(274);
    statement();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- SwitchStatementContext ------------------------------------------------------------------

JavaScriptParser::SwitchStatementContext::SwitchStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::SwitchStatementContext::SWITCH() {
  return getToken(JavaScriptParser::SWITCH, 0);
}

JavaScriptParser::ExpressionSequenceContext* JavaScriptParser::SwitchStatementContext::expressionSequence() {
  return getRuleContext<JavaScriptParser::ExpressionSequenceContext>(0);
}

JavaScriptParser::CaseBlockContext* JavaScriptParser::SwitchStatementContext::caseBlock() {
  return getRuleContext<JavaScriptParser::CaseBlockContext>(0);
}


size_t JavaScriptParser::SwitchStatementContext::getRuleIndex() const {
  return JavaScriptParser::RuleSwitchStatement;
}

void JavaScriptParser::SwitchStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSwitchStatement(this);
}

void JavaScriptParser::SwitchStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSwitchStatement(this);
}

JavaScriptParser::SwitchStatementContext* JavaScriptParser::switchStatement() {
  SwitchStatementContext *_localctx = _tracker.createInstance<SwitchStatementContext>(_ctx, getState());
  enterRule(_localctx, 34, JavaScriptParser::RuleSwitchStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(276);
    match(JavaScriptParser::SWITCH);
    setState(277);
    match(JavaScriptParser::T__5);
    setState(278);
    expressionSequence();
    setState(279);
    match(JavaScriptParser::T__6);
    setState(280);
    caseBlock();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CaseBlockContext ------------------------------------------------------------------

JavaScriptParser::CaseBlockContext::CaseBlockContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<JavaScriptParser::CaseClausesContext *> JavaScriptParser::CaseBlockContext::caseClauses() {
  return getRuleContexts<JavaScriptParser::CaseClausesContext>();
}

JavaScriptParser::CaseClausesContext* JavaScriptParser::CaseBlockContext::caseClauses(size_t i) {
  return getRuleContext<JavaScriptParser::CaseClausesContext>(i);
}

JavaScriptParser::DefaultClauseContext* JavaScriptParser::CaseBlockContext::defaultClause() {
  return getRuleContext<JavaScriptParser::DefaultClauseContext>(0);
}


size_t JavaScriptParser::CaseBlockContext::getRuleIndex() const {
  return JavaScriptParser::RuleCaseBlock;
}

void JavaScriptParser::CaseBlockContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCaseBlock(this);
}

void JavaScriptParser::CaseBlockContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCaseBlock(this);
}

JavaScriptParser::CaseBlockContext* JavaScriptParser::caseBlock() {
  CaseBlockContext *_localctx = _tracker.createInstance<CaseBlockContext>(_ctx, getState());
  enterRule(_localctx, 36, JavaScriptParser::RuleCaseBlock);
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
    setState(282);
    match(JavaScriptParser::T__0);
    setState(284);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == JavaScriptParser::CASE) {
      setState(283);
      caseClauses();
    }
    setState(290);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == JavaScriptParser::DEFAULT) {
      setState(286);
      defaultClause();
      setState(288);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == JavaScriptParser::CASE) {
        setState(287);
        caseClauses();
      }
    }
    setState(292);
    match(JavaScriptParser::T__1);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CaseClausesContext ------------------------------------------------------------------

JavaScriptParser::CaseClausesContext::CaseClausesContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<JavaScriptParser::CaseClauseContext *> JavaScriptParser::CaseClausesContext::caseClause() {
  return getRuleContexts<JavaScriptParser::CaseClauseContext>();
}

JavaScriptParser::CaseClauseContext* JavaScriptParser::CaseClausesContext::caseClause(size_t i) {
  return getRuleContext<JavaScriptParser::CaseClauseContext>(i);
}


size_t JavaScriptParser::CaseClausesContext::getRuleIndex() const {
  return JavaScriptParser::RuleCaseClauses;
}

void JavaScriptParser::CaseClausesContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCaseClauses(this);
}

void JavaScriptParser::CaseClausesContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCaseClauses(this);
}

JavaScriptParser::CaseClausesContext* JavaScriptParser::caseClauses() {
  CaseClausesContext *_localctx = _tracker.createInstance<CaseClausesContext>(_ctx, getState());
  enterRule(_localctx, 38, JavaScriptParser::RuleCaseClauses);
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
    setState(295); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(294);
      caseClause();
      setState(297); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (_la == JavaScriptParser::CASE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CaseClauseContext ------------------------------------------------------------------

JavaScriptParser::CaseClauseContext::CaseClauseContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::CaseClauseContext::CASE() {
  return getToken(JavaScriptParser::CASE, 0);
}

JavaScriptParser::ExpressionSequenceContext* JavaScriptParser::CaseClauseContext::expressionSequence() {
  return getRuleContext<JavaScriptParser::ExpressionSequenceContext>(0);
}

JavaScriptParser::StatementListContext* JavaScriptParser::CaseClauseContext::statementList() {
  return getRuleContext<JavaScriptParser::StatementListContext>(0);
}


size_t JavaScriptParser::CaseClauseContext::getRuleIndex() const {
  return JavaScriptParser::RuleCaseClause;
}

void JavaScriptParser::CaseClauseContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCaseClause(this);
}

void JavaScriptParser::CaseClauseContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCaseClause(this);
}

JavaScriptParser::CaseClauseContext* JavaScriptParser::caseClause() {
  CaseClauseContext *_localctx = _tracker.createInstance<CaseClauseContext>(_ctx, getState());
  enterRule(_localctx, 40, JavaScriptParser::RuleCaseClause);
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
    setState(299);
    match(JavaScriptParser::CASE);
    setState(300);
    expressionSequence();
    setState(301);
    match(JavaScriptParser::T__7);
    setState(303);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 3926575925113909834) != 0) || ((((_la - 64) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 64)) & 3407847) != 0)) {
      setState(302);
      statementList();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DefaultClauseContext ------------------------------------------------------------------

JavaScriptParser::DefaultClauseContext::DefaultClauseContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::DefaultClauseContext::DEFAULT() {
  return getToken(JavaScriptParser::DEFAULT, 0);
}

JavaScriptParser::StatementListContext* JavaScriptParser::DefaultClauseContext::statementList() {
  return getRuleContext<JavaScriptParser::StatementListContext>(0);
}


size_t JavaScriptParser::DefaultClauseContext::getRuleIndex() const {
  return JavaScriptParser::RuleDefaultClause;
}

void JavaScriptParser::DefaultClauseContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterDefaultClause(this);
}

void JavaScriptParser::DefaultClauseContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitDefaultClause(this);
}

JavaScriptParser::DefaultClauseContext* JavaScriptParser::defaultClause() {
  DefaultClauseContext *_localctx = _tracker.createInstance<DefaultClauseContext>(_ctx, getState());
  enterRule(_localctx, 42, JavaScriptParser::RuleDefaultClause);
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
    setState(305);
    match(JavaScriptParser::DEFAULT);
    setState(306);
    match(JavaScriptParser::T__7);
    setState(308);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 3926575925113909834) != 0) || ((((_la - 64) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 64)) & 3407847) != 0)) {
      setState(307);
      statementList();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LabelledStatementContext ------------------------------------------------------------------

JavaScriptParser::LabelledStatementContext::LabelledStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::LabelledStatementContext::Identifier() {
  return getToken(JavaScriptParser::Identifier, 0);
}

JavaScriptParser::StatementContext* JavaScriptParser::LabelledStatementContext::statement() {
  return getRuleContext<JavaScriptParser::StatementContext>(0);
}


size_t JavaScriptParser::LabelledStatementContext::getRuleIndex() const {
  return JavaScriptParser::RuleLabelledStatement;
}

void JavaScriptParser::LabelledStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLabelledStatement(this);
}

void JavaScriptParser::LabelledStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLabelledStatement(this);
}

JavaScriptParser::LabelledStatementContext* JavaScriptParser::labelledStatement() {
  LabelledStatementContext *_localctx = _tracker.createInstance<LabelledStatementContext>(_ctx, getState());
  enterRule(_localctx, 44, JavaScriptParser::RuleLabelledStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(310);
    match(JavaScriptParser::Identifier);
    setState(311);
    match(JavaScriptParser::T__7);
    setState(312);
    statement();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ThrowStatementContext ------------------------------------------------------------------

JavaScriptParser::ThrowStatementContext::ThrowStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::ThrowStatementContext::THROW() {
  return getToken(JavaScriptParser::THROW, 0);
}

JavaScriptParser::ExpressionSequenceContext* JavaScriptParser::ThrowStatementContext::expressionSequence() {
  return getRuleContext<JavaScriptParser::ExpressionSequenceContext>(0);
}


size_t JavaScriptParser::ThrowStatementContext::getRuleIndex() const {
  return JavaScriptParser::RuleThrowStatement;
}

void JavaScriptParser::ThrowStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterThrowStatement(this);
}

void JavaScriptParser::ThrowStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitThrowStatement(this);
}

JavaScriptParser::ThrowStatementContext* JavaScriptParser::throwStatement() {
  ThrowStatementContext *_localctx = _tracker.createInstance<ThrowStatementContext>(_ctx, getState());
  enterRule(_localctx, 46, JavaScriptParser::RuleThrowStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(314);
    match(JavaScriptParser::THROW);
    setState(315);
    expressionSequence();
    setState(317);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 33, _ctx)) {
    case 1: {
      setState(316);
      match(JavaScriptParser::T__2);
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

//----------------- TryStatementContext ------------------------------------------------------------------

JavaScriptParser::TryStatementContext::TryStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::TryStatementContext::TRY() {
  return getToken(JavaScriptParser::TRY, 0);
}

JavaScriptParser::BlockContext* JavaScriptParser::TryStatementContext::block() {
  return getRuleContext<JavaScriptParser::BlockContext>(0);
}

JavaScriptParser::CatchProductionContext* JavaScriptParser::TryStatementContext::catchProduction() {
  return getRuleContext<JavaScriptParser::CatchProductionContext>(0);
}

JavaScriptParser::FinallyProductionContext* JavaScriptParser::TryStatementContext::finallyProduction() {
  return getRuleContext<JavaScriptParser::FinallyProductionContext>(0);
}


size_t JavaScriptParser::TryStatementContext::getRuleIndex() const {
  return JavaScriptParser::RuleTryStatement;
}

void JavaScriptParser::TryStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTryStatement(this);
}

void JavaScriptParser::TryStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTryStatement(this);
}

JavaScriptParser::TryStatementContext* JavaScriptParser::tryStatement() {
  TryStatementContext *_localctx = _tracker.createInstance<TryStatementContext>(_ctx, getState());
  enterRule(_localctx, 48, JavaScriptParser::RuleTryStatement);
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
    setState(319);
    match(JavaScriptParser::TRY);
    setState(320);
    block();
    setState(326);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case JavaScriptParser::CATCH: {
        setState(321);
        catchProduction();
        setState(323);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == JavaScriptParser::FINALLY) {
          setState(322);
          finallyProduction();
        }
        break;
      }

      case JavaScriptParser::FINALLY: {
        setState(325);
        finallyProduction();
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

//----------------- CatchProductionContext ------------------------------------------------------------------

JavaScriptParser::CatchProductionContext::CatchProductionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::CatchProductionContext::CATCH() {
  return getToken(JavaScriptParser::CATCH, 0);
}

tree::TerminalNode* JavaScriptParser::CatchProductionContext::Identifier() {
  return getToken(JavaScriptParser::Identifier, 0);
}

JavaScriptParser::BlockContext* JavaScriptParser::CatchProductionContext::block() {
  return getRuleContext<JavaScriptParser::BlockContext>(0);
}


size_t JavaScriptParser::CatchProductionContext::getRuleIndex() const {
  return JavaScriptParser::RuleCatchProduction;
}

void JavaScriptParser::CatchProductionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCatchProduction(this);
}

void JavaScriptParser::CatchProductionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCatchProduction(this);
}

JavaScriptParser::CatchProductionContext* JavaScriptParser::catchProduction() {
  CatchProductionContext *_localctx = _tracker.createInstance<CatchProductionContext>(_ctx, getState());
  enterRule(_localctx, 50, JavaScriptParser::RuleCatchProduction);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(328);
    match(JavaScriptParser::CATCH);
    setState(329);
    match(JavaScriptParser::T__5);
    setState(330);
    match(JavaScriptParser::Identifier);
    setState(331);
    match(JavaScriptParser::T__6);
    setState(332);
    block();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FinallyProductionContext ------------------------------------------------------------------

JavaScriptParser::FinallyProductionContext::FinallyProductionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::FinallyProductionContext::FINALLY() {
  return getToken(JavaScriptParser::FINALLY, 0);
}

JavaScriptParser::BlockContext* JavaScriptParser::FinallyProductionContext::block() {
  return getRuleContext<JavaScriptParser::BlockContext>(0);
}


size_t JavaScriptParser::FinallyProductionContext::getRuleIndex() const {
  return JavaScriptParser::RuleFinallyProduction;
}

void JavaScriptParser::FinallyProductionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFinallyProduction(this);
}

void JavaScriptParser::FinallyProductionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFinallyProduction(this);
}

JavaScriptParser::FinallyProductionContext* JavaScriptParser::finallyProduction() {
  FinallyProductionContext *_localctx = _tracker.createInstance<FinallyProductionContext>(_ctx, getState());
  enterRule(_localctx, 52, JavaScriptParser::RuleFinallyProduction);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(334);
    match(JavaScriptParser::FINALLY);
    setState(335);
    block();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DebuggerStatementContext ------------------------------------------------------------------

JavaScriptParser::DebuggerStatementContext::DebuggerStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::DebuggerStatementContext::DEBUGGER() {
  return getToken(JavaScriptParser::DEBUGGER, 0);
}


size_t JavaScriptParser::DebuggerStatementContext::getRuleIndex() const {
  return JavaScriptParser::RuleDebuggerStatement;
}

void JavaScriptParser::DebuggerStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterDebuggerStatement(this);
}

void JavaScriptParser::DebuggerStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitDebuggerStatement(this);
}

JavaScriptParser::DebuggerStatementContext* JavaScriptParser::debuggerStatement() {
  DebuggerStatementContext *_localctx = _tracker.createInstance<DebuggerStatementContext>(_ctx, getState());
  enterRule(_localctx, 54, JavaScriptParser::RuleDebuggerStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(337);
    match(JavaScriptParser::DEBUGGER);
    setState(339);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 36, _ctx)) {
    case 1: {
      setState(338);
      match(JavaScriptParser::T__2);
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

//----------------- FunctionDeclarationContext ------------------------------------------------------------------

JavaScriptParser::FunctionDeclarationContext::FunctionDeclarationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::FunctionDeclarationContext::FUNCTION() {
  return getToken(JavaScriptParser::FUNCTION, 0);
}

tree::TerminalNode* JavaScriptParser::FunctionDeclarationContext::Identifier() {
  return getToken(JavaScriptParser::Identifier, 0);
}

JavaScriptParser::FunctionBodyContext* JavaScriptParser::FunctionDeclarationContext::functionBody() {
  return getRuleContext<JavaScriptParser::FunctionBodyContext>(0);
}

JavaScriptParser::FormalParameterListContext* JavaScriptParser::FunctionDeclarationContext::formalParameterList() {
  return getRuleContext<JavaScriptParser::FormalParameterListContext>(0);
}


size_t JavaScriptParser::FunctionDeclarationContext::getRuleIndex() const {
  return JavaScriptParser::RuleFunctionDeclaration;
}

void JavaScriptParser::FunctionDeclarationContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFunctionDeclaration(this);
}

void JavaScriptParser::FunctionDeclarationContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFunctionDeclaration(this);
}

JavaScriptParser::FunctionDeclarationContext* JavaScriptParser::functionDeclaration() {
  FunctionDeclarationContext *_localctx = _tracker.createInstance<FunctionDeclarationContext>(_ctx, getState());
  enterRule(_localctx, 56, JavaScriptParser::RuleFunctionDeclaration);
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
    setState(341);
    match(JavaScriptParser::FUNCTION);
    setState(342);
    match(JavaScriptParser::Identifier);
    setState(343);
    match(JavaScriptParser::T__5);
    setState(345);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == JavaScriptParser::Identifier) {
      setState(344);
      formalParameterList();
    }
    setState(347);
    match(JavaScriptParser::T__6);
    setState(348);
    match(JavaScriptParser::T__0);
    setState(349);
    functionBody();
    setState(350);
    match(JavaScriptParser::T__1);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FormalParameterListContext ------------------------------------------------------------------

JavaScriptParser::FormalParameterListContext::FormalParameterListContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> JavaScriptParser::FormalParameterListContext::Identifier() {
  return getTokens(JavaScriptParser::Identifier);
}

tree::TerminalNode* JavaScriptParser::FormalParameterListContext::Identifier(size_t i) {
  return getToken(JavaScriptParser::Identifier, i);
}


size_t JavaScriptParser::FormalParameterListContext::getRuleIndex() const {
  return JavaScriptParser::RuleFormalParameterList;
}

void JavaScriptParser::FormalParameterListContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFormalParameterList(this);
}

void JavaScriptParser::FormalParameterListContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFormalParameterList(this);
}

JavaScriptParser::FormalParameterListContext* JavaScriptParser::formalParameterList() {
  FormalParameterListContext *_localctx = _tracker.createInstance<FormalParameterListContext>(_ctx, getState());
  enterRule(_localctx, 58, JavaScriptParser::RuleFormalParameterList);
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
    setState(352);
    match(JavaScriptParser::Identifier);
    setState(357);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == JavaScriptParser::T__3) {
      setState(353);
      match(JavaScriptParser::T__3);
      setState(354);
      match(JavaScriptParser::Identifier);
      setState(359);
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

//----------------- FunctionBodyContext ------------------------------------------------------------------

JavaScriptParser::FunctionBodyContext::FunctionBodyContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

JavaScriptParser::SourceElementsContext* JavaScriptParser::FunctionBodyContext::sourceElements() {
  return getRuleContext<JavaScriptParser::SourceElementsContext>(0);
}


size_t JavaScriptParser::FunctionBodyContext::getRuleIndex() const {
  return JavaScriptParser::RuleFunctionBody;
}

void JavaScriptParser::FunctionBodyContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFunctionBody(this);
}

void JavaScriptParser::FunctionBodyContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFunctionBody(this);
}

JavaScriptParser::FunctionBodyContext* JavaScriptParser::functionBody() {
  FunctionBodyContext *_localctx = _tracker.createInstance<FunctionBodyContext>(_ctx, getState());
  enterRule(_localctx, 60, JavaScriptParser::RuleFunctionBody);
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
    setState(361);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 3926575925113909834) != 0) || ((((_la - 64) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 64)) & 3407847) != 0)) {
      setState(360);
      sourceElements();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExpressionSequenceContext ------------------------------------------------------------------

JavaScriptParser::ExpressionSequenceContext::ExpressionSequenceContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<JavaScriptParser::SingleExpressionContext *> JavaScriptParser::ExpressionSequenceContext::singleExpression() {
  return getRuleContexts<JavaScriptParser::SingleExpressionContext>();
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::ExpressionSequenceContext::singleExpression(size_t i) {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(i);
}


size_t JavaScriptParser::ExpressionSequenceContext::getRuleIndex() const {
  return JavaScriptParser::RuleExpressionSequence;
}

void JavaScriptParser::ExpressionSequenceContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterExpressionSequence(this);
}

void JavaScriptParser::ExpressionSequenceContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitExpressionSequence(this);
}

JavaScriptParser::ExpressionSequenceContext* JavaScriptParser::expressionSequence() {
  ExpressionSequenceContext *_localctx = _tracker.createInstance<ExpressionSequenceContext>(_ctx, getState());
  enterRule(_localctx, 62, JavaScriptParser::RuleExpressionSequence);
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
    setState(363);
    singleExpression(0);
    setState(368);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == JavaScriptParser::T__3) {
      setState(364);
      match(JavaScriptParser::T__3);
      setState(365);
      singleExpression(0);
      setState(370);
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

//----------------- SingleExpressionContext ------------------------------------------------------------------

JavaScriptParser::SingleExpressionContext::SingleExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t JavaScriptParser::SingleExpressionContext::getRuleIndex() const {
  return JavaScriptParser::RuleSingleExpression;
}

void JavaScriptParser::SingleExpressionContext::copyFrom(SingleExpressionContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- TernaryExpressionContext ------------------------------------------------------------------

std::vector<JavaScriptParser::SingleExpressionContext *> JavaScriptParser::TernaryExpressionContext::singleExpression() {
  return getRuleContexts<JavaScriptParser::SingleExpressionContext>();
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::TernaryExpressionContext::singleExpression(size_t i) {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(i);
}

JavaScriptParser::TernaryExpressionContext::TernaryExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::TernaryExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTernaryExpression(this);
}
void JavaScriptParser::TernaryExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTernaryExpression(this);
}
//----------------- LogicalAndExpressionContext ------------------------------------------------------------------

std::vector<JavaScriptParser::SingleExpressionContext *> JavaScriptParser::LogicalAndExpressionContext::singleExpression() {
  return getRuleContexts<JavaScriptParser::SingleExpressionContext>();
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::LogicalAndExpressionContext::singleExpression(size_t i) {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(i);
}

JavaScriptParser::LogicalAndExpressionContext::LogicalAndExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::LogicalAndExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLogicalAndExpression(this);
}
void JavaScriptParser::LogicalAndExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLogicalAndExpression(this);
}
//----------------- PreIncrementExpressionContext ------------------------------------------------------------------

JavaScriptParser::SingleExpressionContext* JavaScriptParser::PreIncrementExpressionContext::singleExpression() {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(0);
}

JavaScriptParser::PreIncrementExpressionContext::PreIncrementExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::PreIncrementExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPreIncrementExpression(this);
}
void JavaScriptParser::PreIncrementExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPreIncrementExpression(this);
}
//----------------- ObjectLiteralExpressionContext ------------------------------------------------------------------

JavaScriptParser::ObjectLiteralContext* JavaScriptParser::ObjectLiteralExpressionContext::objectLiteral() {
  return getRuleContext<JavaScriptParser::ObjectLiteralContext>(0);
}

JavaScriptParser::ObjectLiteralExpressionContext::ObjectLiteralExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::ObjectLiteralExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterObjectLiteralExpression(this);
}
void JavaScriptParser::ObjectLiteralExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitObjectLiteralExpression(this);
}
//----------------- InExpressionContext ------------------------------------------------------------------

std::vector<JavaScriptParser::SingleExpressionContext *> JavaScriptParser::InExpressionContext::singleExpression() {
  return getRuleContexts<JavaScriptParser::SingleExpressionContext>();
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::InExpressionContext::singleExpression(size_t i) {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(i);
}

tree::TerminalNode* JavaScriptParser::InExpressionContext::IN() {
  return getToken(JavaScriptParser::IN, 0);
}

JavaScriptParser::InExpressionContext::InExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::InExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterInExpression(this);
}
void JavaScriptParser::InExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitInExpression(this);
}
//----------------- LogicalOrExpressionContext ------------------------------------------------------------------

std::vector<JavaScriptParser::SingleExpressionContext *> JavaScriptParser::LogicalOrExpressionContext::singleExpression() {
  return getRuleContexts<JavaScriptParser::SingleExpressionContext>();
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::LogicalOrExpressionContext::singleExpression(size_t i) {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(i);
}

JavaScriptParser::LogicalOrExpressionContext::LogicalOrExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::LogicalOrExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLogicalOrExpression(this);
}
void JavaScriptParser::LogicalOrExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLogicalOrExpression(this);
}
//----------------- NotExpressionContext ------------------------------------------------------------------

JavaScriptParser::SingleExpressionContext* JavaScriptParser::NotExpressionContext::singleExpression() {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(0);
}

JavaScriptParser::NotExpressionContext::NotExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::NotExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterNotExpression(this);
}
void JavaScriptParser::NotExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitNotExpression(this);
}
//----------------- PreDecreaseExpressionContext ------------------------------------------------------------------

JavaScriptParser::SingleExpressionContext* JavaScriptParser::PreDecreaseExpressionContext::singleExpression() {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(0);
}

JavaScriptParser::PreDecreaseExpressionContext::PreDecreaseExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::PreDecreaseExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPreDecreaseExpression(this);
}
void JavaScriptParser::PreDecreaseExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPreDecreaseExpression(this);
}
//----------------- ThisExpressionContext ------------------------------------------------------------------

tree::TerminalNode* JavaScriptParser::ThisExpressionContext::THIS() {
  return getToken(JavaScriptParser::THIS, 0);
}

JavaScriptParser::ThisExpressionContext::ThisExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::ThisExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterThisExpression(this);
}
void JavaScriptParser::ThisExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitThisExpression(this);
}
//----------------- FunctionExpressionContext ------------------------------------------------------------------

tree::TerminalNode* JavaScriptParser::FunctionExpressionContext::FUNCTION() {
  return getToken(JavaScriptParser::FUNCTION, 0);
}

JavaScriptParser::FunctionBodyContext* JavaScriptParser::FunctionExpressionContext::functionBody() {
  return getRuleContext<JavaScriptParser::FunctionBodyContext>(0);
}

tree::TerminalNode* JavaScriptParser::FunctionExpressionContext::Identifier() {
  return getToken(JavaScriptParser::Identifier, 0);
}

JavaScriptParser::FormalParameterListContext* JavaScriptParser::FunctionExpressionContext::formalParameterList() {
  return getRuleContext<JavaScriptParser::FormalParameterListContext>(0);
}

JavaScriptParser::FunctionExpressionContext::FunctionExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::FunctionExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFunctionExpression(this);
}
void JavaScriptParser::FunctionExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFunctionExpression(this);
}
//----------------- UnaryMinusExpressionContext ------------------------------------------------------------------

JavaScriptParser::SingleExpressionContext* JavaScriptParser::UnaryMinusExpressionContext::singleExpression() {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(0);
}

JavaScriptParser::UnaryMinusExpressionContext::UnaryMinusExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::UnaryMinusExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterUnaryMinusExpression(this);
}
void JavaScriptParser::UnaryMinusExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitUnaryMinusExpression(this);
}
//----------------- AssignmentExpressionContext ------------------------------------------------------------------

std::vector<JavaScriptParser::SingleExpressionContext *> JavaScriptParser::AssignmentExpressionContext::singleExpression() {
  return getRuleContexts<JavaScriptParser::SingleExpressionContext>();
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::AssignmentExpressionContext::singleExpression(size_t i) {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(i);
}

JavaScriptParser::AssignmentExpressionContext::AssignmentExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::AssignmentExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAssignmentExpression(this);
}
void JavaScriptParser::AssignmentExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAssignmentExpression(this);
}
//----------------- PostDecreaseExpressionContext ------------------------------------------------------------------

JavaScriptParser::SingleExpressionContext* JavaScriptParser::PostDecreaseExpressionContext::singleExpression() {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(0);
}

JavaScriptParser::PostDecreaseExpressionContext::PostDecreaseExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::PostDecreaseExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPostDecreaseExpression(this);
}
void JavaScriptParser::PostDecreaseExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPostDecreaseExpression(this);
}
//----------------- TypeofExpressionContext ------------------------------------------------------------------

tree::TerminalNode* JavaScriptParser::TypeofExpressionContext::TYPEOF() {
  return getToken(JavaScriptParser::TYPEOF, 0);
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::TypeofExpressionContext::singleExpression() {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(0);
}

JavaScriptParser::TypeofExpressionContext::TypeofExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::TypeofExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTypeofExpression(this);
}
void JavaScriptParser::TypeofExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTypeofExpression(this);
}
//----------------- InstanceofExpressionContext ------------------------------------------------------------------

std::vector<JavaScriptParser::SingleExpressionContext *> JavaScriptParser::InstanceofExpressionContext::singleExpression() {
  return getRuleContexts<JavaScriptParser::SingleExpressionContext>();
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::InstanceofExpressionContext::singleExpression(size_t i) {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(i);
}

tree::TerminalNode* JavaScriptParser::InstanceofExpressionContext::INSTANCEOF() {
  return getToken(JavaScriptParser::INSTANCEOF, 0);
}

JavaScriptParser::InstanceofExpressionContext::InstanceofExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::InstanceofExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterInstanceofExpression(this);
}
void JavaScriptParser::InstanceofExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitInstanceofExpression(this);
}
//----------------- UnaryPlusExpressionContext ------------------------------------------------------------------

JavaScriptParser::SingleExpressionContext* JavaScriptParser::UnaryPlusExpressionContext::singleExpression() {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(0);
}

JavaScriptParser::UnaryPlusExpressionContext::UnaryPlusExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::UnaryPlusExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterUnaryPlusExpression(this);
}
void JavaScriptParser::UnaryPlusExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitUnaryPlusExpression(this);
}
//----------------- DeleteExpressionContext ------------------------------------------------------------------

tree::TerminalNode* JavaScriptParser::DeleteExpressionContext::DELETE() {
  return getToken(JavaScriptParser::DELETE, 0);
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::DeleteExpressionContext::singleExpression() {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(0);
}

JavaScriptParser::DeleteExpressionContext::DeleteExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::DeleteExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterDeleteExpression(this);
}
void JavaScriptParser::DeleteExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitDeleteExpression(this);
}
//----------------- EqualityExpressionContext ------------------------------------------------------------------

std::vector<JavaScriptParser::SingleExpressionContext *> JavaScriptParser::EqualityExpressionContext::singleExpression() {
  return getRuleContexts<JavaScriptParser::SingleExpressionContext>();
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::EqualityExpressionContext::singleExpression(size_t i) {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(i);
}

JavaScriptParser::EqualityExpressionContext::EqualityExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::EqualityExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterEqualityExpression(this);
}
void JavaScriptParser::EqualityExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitEqualityExpression(this);
}
//----------------- BitXOrExpressionContext ------------------------------------------------------------------

std::vector<JavaScriptParser::SingleExpressionContext *> JavaScriptParser::BitXOrExpressionContext::singleExpression() {
  return getRuleContexts<JavaScriptParser::SingleExpressionContext>();
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::BitXOrExpressionContext::singleExpression(size_t i) {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(i);
}

JavaScriptParser::BitXOrExpressionContext::BitXOrExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::BitXOrExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBitXOrExpression(this);
}
void JavaScriptParser::BitXOrExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBitXOrExpression(this);
}
//----------------- MultiplicativeExpressionContext ------------------------------------------------------------------

std::vector<JavaScriptParser::SingleExpressionContext *> JavaScriptParser::MultiplicativeExpressionContext::singleExpression() {
  return getRuleContexts<JavaScriptParser::SingleExpressionContext>();
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::MultiplicativeExpressionContext::singleExpression(size_t i) {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(i);
}

JavaScriptParser::MultiplicativeExpressionContext::MultiplicativeExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::MultiplicativeExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMultiplicativeExpression(this);
}
void JavaScriptParser::MultiplicativeExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMultiplicativeExpression(this);
}
//----------------- CallExpressionContext ------------------------------------------------------------------

JavaScriptParser::SingleExpressionContext* JavaScriptParser::CallExpressionContext::singleExpression() {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(0);
}

JavaScriptParser::ArgumentsContext* JavaScriptParser::CallExpressionContext::arguments() {
  return getRuleContext<JavaScriptParser::ArgumentsContext>(0);
}

JavaScriptParser::CallExpressionContext::CallExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::CallExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCallExpression(this);
}
void JavaScriptParser::CallExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCallExpression(this);
}
//----------------- BitShiftExpressionContext ------------------------------------------------------------------

std::vector<JavaScriptParser::SingleExpressionContext *> JavaScriptParser::BitShiftExpressionContext::singleExpression() {
  return getRuleContexts<JavaScriptParser::SingleExpressionContext>();
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::BitShiftExpressionContext::singleExpression(size_t i) {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(i);
}

JavaScriptParser::BitShiftExpressionContext::BitShiftExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::BitShiftExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBitShiftExpression(this);
}
void JavaScriptParser::BitShiftExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBitShiftExpression(this);
}
//----------------- ParenthesizedExpressionContext ------------------------------------------------------------------

JavaScriptParser::ExpressionSequenceContext* JavaScriptParser::ParenthesizedExpressionContext::expressionSequence() {
  return getRuleContext<JavaScriptParser::ExpressionSequenceContext>(0);
}

JavaScriptParser::ParenthesizedExpressionContext::ParenthesizedExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::ParenthesizedExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterParenthesizedExpression(this);
}
void JavaScriptParser::ParenthesizedExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitParenthesizedExpression(this);
}
//----------------- AdditiveExpressionContext ------------------------------------------------------------------

std::vector<JavaScriptParser::SingleExpressionContext *> JavaScriptParser::AdditiveExpressionContext::singleExpression() {
  return getRuleContexts<JavaScriptParser::SingleExpressionContext>();
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::AdditiveExpressionContext::singleExpression(size_t i) {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(i);
}

JavaScriptParser::AdditiveExpressionContext::AdditiveExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::AdditiveExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAdditiveExpression(this);
}
void JavaScriptParser::AdditiveExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAdditiveExpression(this);
}
//----------------- RelationalExpressionContext ------------------------------------------------------------------

std::vector<JavaScriptParser::SingleExpressionContext *> JavaScriptParser::RelationalExpressionContext::singleExpression() {
  return getRuleContexts<JavaScriptParser::SingleExpressionContext>();
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::RelationalExpressionContext::singleExpression(size_t i) {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(i);
}

JavaScriptParser::RelationalExpressionContext::RelationalExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::RelationalExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterRelationalExpression(this);
}
void JavaScriptParser::RelationalExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitRelationalExpression(this);
}
//----------------- PostIncrementExpressionContext ------------------------------------------------------------------

JavaScriptParser::SingleExpressionContext* JavaScriptParser::PostIncrementExpressionContext::singleExpression() {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(0);
}

JavaScriptParser::PostIncrementExpressionContext::PostIncrementExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::PostIncrementExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPostIncrementExpression(this);
}
void JavaScriptParser::PostIncrementExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPostIncrementExpression(this);
}
//----------------- BitNotExpressionContext ------------------------------------------------------------------

JavaScriptParser::SingleExpressionContext* JavaScriptParser::BitNotExpressionContext::singleExpression() {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(0);
}

JavaScriptParser::BitNotExpressionContext::BitNotExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::BitNotExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBitNotExpression(this);
}
void JavaScriptParser::BitNotExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBitNotExpression(this);
}
//----------------- NewExpressionContext ------------------------------------------------------------------

tree::TerminalNode* JavaScriptParser::NewExpressionContext::NEW() {
  return getToken(JavaScriptParser::NEW, 0);
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::NewExpressionContext::singleExpression() {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(0);
}

JavaScriptParser::ArgumentsContext* JavaScriptParser::NewExpressionContext::arguments() {
  return getRuleContext<JavaScriptParser::ArgumentsContext>(0);
}

JavaScriptParser::NewExpressionContext::NewExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::NewExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterNewExpression(this);
}
void JavaScriptParser::NewExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitNewExpression(this);
}
//----------------- LiteralExpressionContext ------------------------------------------------------------------

JavaScriptParser::LiteralContext* JavaScriptParser::LiteralExpressionContext::literal() {
  return getRuleContext<JavaScriptParser::LiteralContext>(0);
}

JavaScriptParser::LiteralExpressionContext::LiteralExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::LiteralExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLiteralExpression(this);
}
void JavaScriptParser::LiteralExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLiteralExpression(this);
}
//----------------- ArrayLiteralExpressionContext ------------------------------------------------------------------

JavaScriptParser::ArrayLiteralContext* JavaScriptParser::ArrayLiteralExpressionContext::arrayLiteral() {
  return getRuleContext<JavaScriptParser::ArrayLiteralContext>(0);
}

JavaScriptParser::ArrayLiteralExpressionContext::ArrayLiteralExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::ArrayLiteralExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterArrayLiteralExpression(this);
}
void JavaScriptParser::ArrayLiteralExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitArrayLiteralExpression(this);
}
//----------------- MemberDotExpressionContext ------------------------------------------------------------------

JavaScriptParser::SingleExpressionContext* JavaScriptParser::MemberDotExpressionContext::singleExpression() {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(0);
}

JavaScriptParser::IdentifierNameContext* JavaScriptParser::MemberDotExpressionContext::identifierName() {
  return getRuleContext<JavaScriptParser::IdentifierNameContext>(0);
}

JavaScriptParser::MemberDotExpressionContext::MemberDotExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::MemberDotExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMemberDotExpression(this);
}
void JavaScriptParser::MemberDotExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMemberDotExpression(this);
}
//----------------- MemberIndexExpressionContext ------------------------------------------------------------------

JavaScriptParser::SingleExpressionContext* JavaScriptParser::MemberIndexExpressionContext::singleExpression() {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(0);
}

JavaScriptParser::ExpressionSequenceContext* JavaScriptParser::MemberIndexExpressionContext::expressionSequence() {
  return getRuleContext<JavaScriptParser::ExpressionSequenceContext>(0);
}

JavaScriptParser::MemberIndexExpressionContext::MemberIndexExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::MemberIndexExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMemberIndexExpression(this);
}
void JavaScriptParser::MemberIndexExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMemberIndexExpression(this);
}
//----------------- IdentifierExpressionContext ------------------------------------------------------------------

tree::TerminalNode* JavaScriptParser::IdentifierExpressionContext::Identifier() {
  return getToken(JavaScriptParser::Identifier, 0);
}

JavaScriptParser::IdentifierExpressionContext::IdentifierExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::IdentifierExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterIdentifierExpression(this);
}
void JavaScriptParser::IdentifierExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitIdentifierExpression(this);
}
//----------------- BitAndExpressionContext ------------------------------------------------------------------

std::vector<JavaScriptParser::SingleExpressionContext *> JavaScriptParser::BitAndExpressionContext::singleExpression() {
  return getRuleContexts<JavaScriptParser::SingleExpressionContext>();
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::BitAndExpressionContext::singleExpression(size_t i) {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(i);
}

JavaScriptParser::BitAndExpressionContext::BitAndExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::BitAndExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBitAndExpression(this);
}
void JavaScriptParser::BitAndExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBitAndExpression(this);
}
//----------------- BitOrExpressionContext ------------------------------------------------------------------

std::vector<JavaScriptParser::SingleExpressionContext *> JavaScriptParser::BitOrExpressionContext::singleExpression() {
  return getRuleContexts<JavaScriptParser::SingleExpressionContext>();
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::BitOrExpressionContext::singleExpression(size_t i) {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(i);
}

JavaScriptParser::BitOrExpressionContext::BitOrExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::BitOrExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBitOrExpression(this);
}
void JavaScriptParser::BitOrExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBitOrExpression(this);
}
//----------------- AssignmentOperatorExpressionContext ------------------------------------------------------------------

std::vector<JavaScriptParser::SingleExpressionContext *> JavaScriptParser::AssignmentOperatorExpressionContext::singleExpression() {
  return getRuleContexts<JavaScriptParser::SingleExpressionContext>();
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::AssignmentOperatorExpressionContext::singleExpression(size_t i) {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(i);
}

JavaScriptParser::AssignmentOperatorContext* JavaScriptParser::AssignmentOperatorExpressionContext::assignmentOperator() {
  return getRuleContext<JavaScriptParser::AssignmentOperatorContext>(0);
}

JavaScriptParser::AssignmentOperatorExpressionContext::AssignmentOperatorExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::AssignmentOperatorExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAssignmentOperatorExpression(this);
}
void JavaScriptParser::AssignmentOperatorExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAssignmentOperatorExpression(this);
}
//----------------- VoidExpressionContext ------------------------------------------------------------------

tree::TerminalNode* JavaScriptParser::VoidExpressionContext::VOID() {
  return getToken(JavaScriptParser::VOID, 0);
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::VoidExpressionContext::singleExpression() {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(0);
}

JavaScriptParser::VoidExpressionContext::VoidExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::VoidExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterVoidExpression(this);
}
void JavaScriptParser::VoidExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitVoidExpression(this);
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::singleExpression() {
   return singleExpression(0);
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::singleExpression(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  JavaScriptParser::SingleExpressionContext *_localctx = _tracker.createInstance<SingleExpressionContext>(_ctx, parentState);
  JavaScriptParser::SingleExpressionContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 64;
  enterRecursionRule(_localctx, 64, JavaScriptParser::RuleSingleExpression, precedence);

    size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(417);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case JavaScriptParser::FUNCTION: {
        _localctx = _tracker.createInstance<FunctionExpressionContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;

        setState(372);
        match(JavaScriptParser::FUNCTION);
        setState(374);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == JavaScriptParser::Identifier) {
          setState(373);
          match(JavaScriptParser::Identifier);
        }
        setState(376);
        match(JavaScriptParser::T__5);
        setState(378);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == JavaScriptParser::Identifier) {
          setState(377);
          formalParameterList();
        }
        setState(380);
        match(JavaScriptParser::T__6);
        setState(381);
        match(JavaScriptParser::T__0);
        setState(382);
        functionBody();
        setState(383);
        match(JavaScriptParser::T__1);
        break;
      }

      case JavaScriptParser::NEW: {
        _localctx = _tracker.createInstance<NewExpressionContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(385);
        match(JavaScriptParser::NEW);
        setState(386);
        singleExpression(0);
        setState(388);
        _errHandler->sync(this);

        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 43, _ctx)) {
        case 1: {
          setState(387);
          arguments();
          break;
        }

        default:
          break;
        }
        break;
      }

      case JavaScriptParser::DELETE: {
        _localctx = _tracker.createInstance<DeleteExpressionContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(390);
        match(JavaScriptParser::DELETE);
        setState(391);
        singleExpression(30);
        break;
      }

      case JavaScriptParser::VOID: {
        _localctx = _tracker.createInstance<VoidExpressionContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(392);
        match(JavaScriptParser::VOID);
        setState(393);
        singleExpression(29);
        break;
      }

      case JavaScriptParser::TYPEOF: {
        _localctx = _tracker.createInstance<TypeofExpressionContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(394);
        match(JavaScriptParser::TYPEOF);
        setState(395);
        singleExpression(28);
        break;
      }

      case JavaScriptParser::T__11: {
        _localctx = _tracker.createInstance<PreIncrementExpressionContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(396);
        match(JavaScriptParser::T__11);
        setState(397);
        singleExpression(27);
        break;
      }

      case JavaScriptParser::T__12: {
        _localctx = _tracker.createInstance<PreDecreaseExpressionContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(398);
        match(JavaScriptParser::T__12);
        setState(399);
        singleExpression(26);
        break;
      }

      case JavaScriptParser::T__13: {
        _localctx = _tracker.createInstance<UnaryPlusExpressionContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(400);
        match(JavaScriptParser::T__13);
        setState(401);
        singleExpression(25);
        break;
      }

      case JavaScriptParser::T__14: {
        _localctx = _tracker.createInstance<UnaryMinusExpressionContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(402);
        match(JavaScriptParser::T__14);
        setState(403);
        singleExpression(24);
        break;
      }

      case JavaScriptParser::T__15: {
        _localctx = _tracker.createInstance<BitNotExpressionContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(404);
        match(JavaScriptParser::T__15);
        setState(405);
        singleExpression(23);
        break;
      }

      case JavaScriptParser::T__16: {
        _localctx = _tracker.createInstance<NotExpressionContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(406);
        match(JavaScriptParser::T__16);
        setState(407);
        singleExpression(22);
        break;
      }

      case JavaScriptParser::THIS: {
        _localctx = _tracker.createInstance<ThisExpressionContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(408);
        match(JavaScriptParser::THIS);
        break;
      }

      case JavaScriptParser::Identifier: {
        _localctx = _tracker.createInstance<IdentifierExpressionContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(409);
        match(JavaScriptParser::Identifier);
        break;
      }

      case JavaScriptParser::NULL_:
      case JavaScriptParser::BooleanLiteral:
      case JavaScriptParser::DecimalLiteral:
      case JavaScriptParser::HexIntegerLiteral:
      case JavaScriptParser::OctalIntegerLiteral:
      case JavaScriptParser::StringLiteral: {
        _localctx = _tracker.createInstance<LiteralExpressionContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(410);
        literal();
        break;
      }

      case JavaScriptParser::T__8: {
        _localctx = _tracker.createInstance<ArrayLiteralExpressionContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(411);
        arrayLiteral();
        break;
      }

      case JavaScriptParser::T__0: {
        _localctx = _tracker.createInstance<ObjectLiteralExpressionContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(412);
        objectLiteral();
        break;
      }

      case JavaScriptParser::T__5: {
        _localctx = _tracker.createInstance<ParenthesizedExpressionContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(413);
        match(JavaScriptParser::T__5);
        setState(414);
        expressionSequence();
        setState(415);
        match(JavaScriptParser::T__6);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    _ctx->stop = _input->LT(-1);
    setState(484);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 46, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(482);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 45, _ctx)) {
        case 1: {
          auto newContext = _tracker.createInstance<MultiplicativeExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(419);

          if (!(precpred(_ctx, 21))) throw FailedPredicateException(this, "precpred(_ctx, 21)");
          setState(420);
          _la = _input->LA(1);
          if (!((((_la & ~ 0x3fULL) == 0) &&
            ((1ULL << _la) & 1835008) != 0))) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(421);
          singleExpression(22);
          break;
        }

        case 2: {
          auto newContext = _tracker.createInstance<AdditiveExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(422);

          if (!(precpred(_ctx, 20))) throw FailedPredicateException(this, "precpred(_ctx, 20)");
          setState(423);
          _la = _input->LA(1);
          if (!(_la == JavaScriptParser::T__13

          || _la == JavaScriptParser::T__14)) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(424);
          singleExpression(21);
          break;
        }

        case 3: {
          auto newContext = _tracker.createInstance<BitShiftExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(425);

          if (!(precpred(_ctx, 19))) throw FailedPredicateException(this, "precpred(_ctx, 19)");
          setState(426);
          _la = _input->LA(1);
          if (!((((_la & ~ 0x3fULL) == 0) &&
            ((1ULL << _la) & 14680064) != 0))) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(427);
          singleExpression(20);
          break;
        }

        case 4: {
          auto newContext = _tracker.createInstance<RelationalExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(428);

          if (!(precpred(_ctx, 18))) throw FailedPredicateException(this, "precpred(_ctx, 18)");
          setState(429);
          _la = _input->LA(1);
          if (!((((_la & ~ 0x3fULL) == 0) &&
            ((1ULL << _la) & 251658240) != 0))) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(430);
          singleExpression(19);
          break;
        }

        case 5: {
          auto newContext = _tracker.createInstance<InstanceofExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(431);

          if (!(precpred(_ctx, 17))) throw FailedPredicateException(this, "precpred(_ctx, 17)");
          setState(432);
          match(JavaScriptParser::INSTANCEOF);
          setState(433);
          singleExpression(18);
          break;
        }

        case 6: {
          auto newContext = _tracker.createInstance<InExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(434);

          if (!(precpred(_ctx, 16))) throw FailedPredicateException(this, "precpred(_ctx, 16)");
          setState(435);
          match(JavaScriptParser::IN);
          setState(436);
          singleExpression(17);
          break;
        }

        case 7: {
          auto newContext = _tracker.createInstance<EqualityExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(437);

          if (!(precpred(_ctx, 15))) throw FailedPredicateException(this, "precpred(_ctx, 15)");
          setState(438);
          _la = _input->LA(1);
          if (!((((_la & ~ 0x3fULL) == 0) &&
            ((1ULL << _la) & 4026531840) != 0))) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(439);
          singleExpression(16);
          break;
        }

        case 8: {
          auto newContext = _tracker.createInstance<BitAndExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(440);

          if (!(precpred(_ctx, 14))) throw FailedPredicateException(this, "precpred(_ctx, 14)");
          setState(441);
          match(JavaScriptParser::T__31);
          setState(442);
          singleExpression(15);
          break;
        }

        case 9: {
          auto newContext = _tracker.createInstance<BitXOrExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(443);

          if (!(precpred(_ctx, 13))) throw FailedPredicateException(this, "precpred(_ctx, 13)");
          setState(444);
          match(JavaScriptParser::T__32);
          setState(445);
          singleExpression(14);
          break;
        }

        case 10: {
          auto newContext = _tracker.createInstance<BitOrExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(446);

          if (!(precpred(_ctx, 12))) throw FailedPredicateException(this, "precpred(_ctx, 12)");
          setState(447);
          match(JavaScriptParser::T__33);
          setState(448);
          singleExpression(13);
          break;
        }

        case 11: {
          auto newContext = _tracker.createInstance<LogicalAndExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(449);

          if (!(precpred(_ctx, 11))) throw FailedPredicateException(this, "precpred(_ctx, 11)");
          setState(450);
          match(JavaScriptParser::T__34);
          setState(451);
          singleExpression(12);
          break;
        }

        case 12: {
          auto newContext = _tracker.createInstance<LogicalOrExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(452);

          if (!(precpred(_ctx, 10))) throw FailedPredicateException(this, "precpred(_ctx, 10)");
          setState(453);
          match(JavaScriptParser::T__35);
          setState(454);
          singleExpression(11);
          break;
        }

        case 13: {
          auto newContext = _tracker.createInstance<TernaryExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(455);

          if (!(precpred(_ctx, 9))) throw FailedPredicateException(this, "precpred(_ctx, 9)");
          setState(456);
          match(JavaScriptParser::T__36);
          setState(457);
          singleExpression(0);
          setState(458);
          match(JavaScriptParser::T__7);
          setState(459);
          singleExpression(10);
          break;
        }

        case 14: {
          auto newContext = _tracker.createInstance<AssignmentExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(461);

          if (!(precpred(_ctx, 8))) throw FailedPredicateException(this, "precpred(_ctx, 8)");
          setState(462);
          match(JavaScriptParser::T__4);
          setState(463);
          singleExpression(9);
          break;
        }

        case 15: {
          auto newContext = _tracker.createInstance<AssignmentOperatorExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(464);

          if (!(precpred(_ctx, 7))) throw FailedPredicateException(this, "precpred(_ctx, 7)");
          setState(465);
          assignmentOperator();
          setState(466);
          singleExpression(8);
          break;
        }

        case 16: {
          auto newContext = _tracker.createInstance<MemberIndexExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(468);

          if (!(precpred(_ctx, 36))) throw FailedPredicateException(this, "precpred(_ctx, 36)");
          setState(469);
          match(JavaScriptParser::T__8);
          setState(470);
          expressionSequence();
          setState(471);
          match(JavaScriptParser::T__9);
          break;
        }

        case 17: {
          auto newContext = _tracker.createInstance<MemberDotExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(473);

          if (!(precpred(_ctx, 35))) throw FailedPredicateException(this, "precpred(_ctx, 35)");
          setState(474);
          match(JavaScriptParser::T__10);
          setState(475);
          identifierName();
          break;
        }

        case 18: {
          auto newContext = _tracker.createInstance<CallExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(476);

          if (!(precpred(_ctx, 34))) throw FailedPredicateException(this, "precpred(_ctx, 34)");
          setState(477);
          arguments();
          break;
        }

        case 19: {
          auto newContext = _tracker.createInstance<PostIncrementExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(478);

          if (!(precpred(_ctx, 32))) throw FailedPredicateException(this, "precpred(_ctx, 32)");
          setState(479);
          match(JavaScriptParser::T__11);
          break;
        }

        case 20: {
          auto newContext = _tracker.createInstance<PostDecreaseExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(480);

          if (!(precpred(_ctx, 31))) throw FailedPredicateException(this, "precpred(_ctx, 31)");
          setState(481);
          match(JavaScriptParser::T__12);
          break;
        }

        default:
          break;
        } 
      }
      setState(486);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 46, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- AssignmentOperatorContext ------------------------------------------------------------------

JavaScriptParser::AssignmentOperatorContext::AssignmentOperatorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t JavaScriptParser::AssignmentOperatorContext::getRuleIndex() const {
  return JavaScriptParser::RuleAssignmentOperator;
}

void JavaScriptParser::AssignmentOperatorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAssignmentOperator(this);
}

void JavaScriptParser::AssignmentOperatorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAssignmentOperator(this);
}

JavaScriptParser::AssignmentOperatorContext* JavaScriptParser::assignmentOperator() {
  AssignmentOperatorContext *_localctx = _tracker.createInstance<AssignmentOperatorContext>(_ctx, getState());
  enterRule(_localctx, 66, JavaScriptParser::RuleAssignmentOperator);
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
    setState(487);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 562675075514368) != 0))) {
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

//----------------- LiteralContext ------------------------------------------------------------------

JavaScriptParser::LiteralContext::LiteralContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t JavaScriptParser::LiteralContext::getRuleIndex() const {
  return JavaScriptParser::RuleLiteral;
}

void JavaScriptParser::LiteralContext::copyFrom(LiteralContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- NumericLiteralExpressionContext ------------------------------------------------------------------

JavaScriptParser::NumericLiteralContext* JavaScriptParser::NumericLiteralExpressionContext::numericLiteral() {
  return getRuleContext<JavaScriptParser::NumericLiteralContext>(0);
}

JavaScriptParser::NumericLiteralExpressionContext::NumericLiteralExpressionContext(LiteralContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::NumericLiteralExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterNumericLiteralExpression(this);
}
void JavaScriptParser::NumericLiteralExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitNumericLiteralExpression(this);
}
//----------------- StringLiteralContext ------------------------------------------------------------------

tree::TerminalNode* JavaScriptParser::StringLiteralContext::StringLiteral() {
  return getToken(JavaScriptParser::StringLiteral, 0);
}

JavaScriptParser::StringLiteralContext::StringLiteralContext(LiteralContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::StringLiteralContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStringLiteral(this);
}
void JavaScriptParser::StringLiteralContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStringLiteral(this);
}
//----------------- BooleanLiteralContext ------------------------------------------------------------------

tree::TerminalNode* JavaScriptParser::BooleanLiteralContext::BooleanLiteral() {
  return getToken(JavaScriptParser::BooleanLiteral, 0);
}

JavaScriptParser::BooleanLiteralContext::BooleanLiteralContext(LiteralContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::BooleanLiteralContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBooleanLiteral(this);
}
void JavaScriptParser::BooleanLiteralContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBooleanLiteral(this);
}
//----------------- NullLiteralContext ------------------------------------------------------------------

tree::TerminalNode* JavaScriptParser::NullLiteralContext::NULL_() {
  return getToken(JavaScriptParser::NULL_, 0);
}

JavaScriptParser::NullLiteralContext::NullLiteralContext(LiteralContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::NullLiteralContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterNullLiteral(this);
}
void JavaScriptParser::NullLiteralContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitNullLiteral(this);
}
JavaScriptParser::LiteralContext* JavaScriptParser::literal() {
  LiteralContext *_localctx = _tracker.createInstance<LiteralContext>(_ctx, getState());
  enterRule(_localctx, 68, JavaScriptParser::RuleLiteral);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(493);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case JavaScriptParser::NULL_: {
        _localctx = _tracker.createInstance<JavaScriptParser::NullLiteralContext>(_localctx);
        enterOuterAlt(_localctx, 1);
        setState(489);
        match(JavaScriptParser::NULL_);
        break;
      }

      case JavaScriptParser::BooleanLiteral: {
        _localctx = _tracker.createInstance<JavaScriptParser::BooleanLiteralContext>(_localctx);
        enterOuterAlt(_localctx, 2);
        setState(490);
        match(JavaScriptParser::BooleanLiteral);
        break;
      }

      case JavaScriptParser::StringLiteral: {
        _localctx = _tracker.createInstance<JavaScriptParser::StringLiteralContext>(_localctx);
        enterOuterAlt(_localctx, 3);
        setState(491);
        match(JavaScriptParser::StringLiteral);
        break;
      }

      case JavaScriptParser::DecimalLiteral:
      case JavaScriptParser::HexIntegerLiteral:
      case JavaScriptParser::OctalIntegerLiteral: {
        _localctx = _tracker.createInstance<JavaScriptParser::NumericLiteralExpressionContext>(_localctx);
        enterOuterAlt(_localctx, 4);
        setState(492);
        numericLiteral();
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

//----------------- NumericLiteralContext ------------------------------------------------------------------

JavaScriptParser::NumericLiteralContext::NumericLiteralContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::NumericLiteralContext::DecimalLiteral() {
  return getToken(JavaScriptParser::DecimalLiteral, 0);
}

tree::TerminalNode* JavaScriptParser::NumericLiteralContext::HexIntegerLiteral() {
  return getToken(JavaScriptParser::HexIntegerLiteral, 0);
}

tree::TerminalNode* JavaScriptParser::NumericLiteralContext::OctalIntegerLiteral() {
  return getToken(JavaScriptParser::OctalIntegerLiteral, 0);
}


size_t JavaScriptParser::NumericLiteralContext::getRuleIndex() const {
  return JavaScriptParser::RuleNumericLiteral;
}

void JavaScriptParser::NumericLiteralContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterNumericLiteral(this);
}

void JavaScriptParser::NumericLiteralContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitNumericLiteral(this);
}

JavaScriptParser::NumericLiteralContext* JavaScriptParser::numericLiteral() {
  NumericLiteralContext *_localctx = _tracker.createInstance<NumericLiteralContext>(_ctx, getState());
  enterRule(_localctx, 70, JavaScriptParser::RuleNumericLiteral);
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
    setState(495);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 15762598695796736) != 0))) {
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

//----------------- IdentifierNameContext ------------------------------------------------------------------

JavaScriptParser::IdentifierNameContext::IdentifierNameContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::IdentifierNameContext::Identifier() {
  return getToken(JavaScriptParser::Identifier, 0);
}

JavaScriptParser::ReservedWordContext* JavaScriptParser::IdentifierNameContext::reservedWord() {
  return getRuleContext<JavaScriptParser::ReservedWordContext>(0);
}


size_t JavaScriptParser::IdentifierNameContext::getRuleIndex() const {
  return JavaScriptParser::RuleIdentifierName;
}

void JavaScriptParser::IdentifierNameContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterIdentifierName(this);
}

void JavaScriptParser::IdentifierNameContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitIdentifierName(this);
}

JavaScriptParser::IdentifierNameContext* JavaScriptParser::identifierName() {
  IdentifierNameContext *_localctx = _tracker.createInstance<IdentifierNameContext>(_ctx, getState());
  enterRule(_localctx, 72, JavaScriptParser::RuleIdentifierName);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(499);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case JavaScriptParser::Identifier: {
        enterOuterAlt(_localctx, 1);
        setState(497);
        match(JavaScriptParser::Identifier);
        break;
      }

      case JavaScriptParser::NULL_:
      case JavaScriptParser::BooleanLiteral:
      case JavaScriptParser::BREAK:
      case JavaScriptParser::CASE:
      case JavaScriptParser::CATCH:
      case JavaScriptParser::CONTINUE:
      case JavaScriptParser::DEBUGGER:
      case JavaScriptParser::DEFAULT:
      case JavaScriptParser::DELETE:
      case JavaScriptParser::DO:
      case JavaScriptParser::ELSE:
      case JavaScriptParser::FINALLY:
      case JavaScriptParser::FOR:
      case JavaScriptParser::FUNCTION:
      case JavaScriptParser::IF:
      case JavaScriptParser::IN:
      case JavaScriptParser::INSTANCEOF:
      case JavaScriptParser::NEW:
      case JavaScriptParser::RETURN:
      case JavaScriptParser::SWITCH:
      case JavaScriptParser::THIS:
      case JavaScriptParser::THROW:
      case JavaScriptParser::TRY:
      case JavaScriptParser::TYPEOF:
      case JavaScriptParser::VAR:
      case JavaScriptParser::VOID:
      case JavaScriptParser::WHILE:
      case JavaScriptParser::WITH:
      case JavaScriptParser::CONST:
      case JavaScriptParser::LET: {
        enterOuterAlt(_localctx, 2);
        setState(498);
        reservedWord();
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

//----------------- ReservedWordContext ------------------------------------------------------------------

JavaScriptParser::ReservedWordContext::ReservedWordContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

JavaScriptParser::KeywordContext* JavaScriptParser::ReservedWordContext::keyword() {
  return getRuleContext<JavaScriptParser::KeywordContext>(0);
}

tree::TerminalNode* JavaScriptParser::ReservedWordContext::NULL_() {
  return getToken(JavaScriptParser::NULL_, 0);
}

tree::TerminalNode* JavaScriptParser::ReservedWordContext::BooleanLiteral() {
  return getToken(JavaScriptParser::BooleanLiteral, 0);
}


size_t JavaScriptParser::ReservedWordContext::getRuleIndex() const {
  return JavaScriptParser::RuleReservedWord;
}

void JavaScriptParser::ReservedWordContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterReservedWord(this);
}

void JavaScriptParser::ReservedWordContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitReservedWord(this);
}

JavaScriptParser::ReservedWordContext* JavaScriptParser::reservedWord() {
  ReservedWordContext *_localctx = _tracker.createInstance<ReservedWordContext>(_ctx, getState());
  enterRule(_localctx, 74, JavaScriptParser::RuleReservedWord);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(504);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case JavaScriptParser::BREAK:
      case JavaScriptParser::CASE:
      case JavaScriptParser::CATCH:
      case JavaScriptParser::CONTINUE:
      case JavaScriptParser::DEBUGGER:
      case JavaScriptParser::DEFAULT:
      case JavaScriptParser::DELETE:
      case JavaScriptParser::DO:
      case JavaScriptParser::ELSE:
      case JavaScriptParser::FINALLY:
      case JavaScriptParser::FOR:
      case JavaScriptParser::FUNCTION:
      case JavaScriptParser::IF:
      case JavaScriptParser::IN:
      case JavaScriptParser::INSTANCEOF:
      case JavaScriptParser::NEW:
      case JavaScriptParser::RETURN:
      case JavaScriptParser::SWITCH:
      case JavaScriptParser::THIS:
      case JavaScriptParser::THROW:
      case JavaScriptParser::TRY:
      case JavaScriptParser::TYPEOF:
      case JavaScriptParser::VAR:
      case JavaScriptParser::VOID:
      case JavaScriptParser::WHILE:
      case JavaScriptParser::WITH:
      case JavaScriptParser::CONST:
      case JavaScriptParser::LET: {
        enterOuterAlt(_localctx, 1);
        setState(501);
        keyword();
        break;
      }

      case JavaScriptParser::NULL_: {
        enterOuterAlt(_localctx, 2);
        setState(502);
        match(JavaScriptParser::NULL_);
        break;
      }

      case JavaScriptParser::BooleanLiteral: {
        enterOuterAlt(_localctx, 3);
        setState(503);
        match(JavaScriptParser::BooleanLiteral);
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

//----------------- KeywordContext ------------------------------------------------------------------

JavaScriptParser::KeywordContext::KeywordContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::KeywordContext::BREAK() {
  return getToken(JavaScriptParser::BREAK, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::CASE() {
  return getToken(JavaScriptParser::CASE, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::CATCH() {
  return getToken(JavaScriptParser::CATCH, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::CONTINUE() {
  return getToken(JavaScriptParser::CONTINUE, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::DEBUGGER() {
  return getToken(JavaScriptParser::DEBUGGER, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::DEFAULT() {
  return getToken(JavaScriptParser::DEFAULT, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::DELETE() {
  return getToken(JavaScriptParser::DELETE, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::DO() {
  return getToken(JavaScriptParser::DO, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::ELSE() {
  return getToken(JavaScriptParser::ELSE, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::FINALLY() {
  return getToken(JavaScriptParser::FINALLY, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::FOR() {
  return getToken(JavaScriptParser::FOR, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::FUNCTION() {
  return getToken(JavaScriptParser::FUNCTION, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::IF() {
  return getToken(JavaScriptParser::IF, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::IN() {
  return getToken(JavaScriptParser::IN, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::INSTANCEOF() {
  return getToken(JavaScriptParser::INSTANCEOF, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::NEW() {
  return getToken(JavaScriptParser::NEW, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::RETURN() {
  return getToken(JavaScriptParser::RETURN, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::SWITCH() {
  return getToken(JavaScriptParser::SWITCH, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::THIS() {
  return getToken(JavaScriptParser::THIS, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::THROW() {
  return getToken(JavaScriptParser::THROW, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::TRY() {
  return getToken(JavaScriptParser::TRY, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::TYPEOF() {
  return getToken(JavaScriptParser::TYPEOF, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::VAR() {
  return getToken(JavaScriptParser::VAR, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::VOID() {
  return getToken(JavaScriptParser::VOID, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::WHILE() {
  return getToken(JavaScriptParser::WHILE, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::WITH() {
  return getToken(JavaScriptParser::WITH, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::CONST() {
  return getToken(JavaScriptParser::CONST, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::LET() {
  return getToken(JavaScriptParser::LET, 0);
}


size_t JavaScriptParser::KeywordContext::getRuleIndex() const {
  return JavaScriptParser::RuleKeyword;
}

void JavaScriptParser::KeywordContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterKeyword(this);
}

void JavaScriptParser::KeywordContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitKeyword(this);
}

JavaScriptParser::KeywordContext* JavaScriptParser::keyword() {
  KeywordContext *_localctx = _tracker.createInstance<KeywordContext>(_ctx, getState());
  enterRule(_localctx, 76, JavaScriptParser::RuleKeyword);
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
    setState(506);
    _la = _input->LA(1);
    if (!(((((_la - 54) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 54)) & 268435455) != 0))) {
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

//----------------- ArrayLiteralContext ------------------------------------------------------------------

JavaScriptParser::ArrayLiteralContext::ArrayLiteralContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

JavaScriptParser::ElementListContext* JavaScriptParser::ArrayLiteralContext::elementList() {
  return getRuleContext<JavaScriptParser::ElementListContext>(0);
}


size_t JavaScriptParser::ArrayLiteralContext::getRuleIndex() const {
  return JavaScriptParser::RuleArrayLiteral;
}

void JavaScriptParser::ArrayLiteralContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterArrayLiteral(this);
}

void JavaScriptParser::ArrayLiteralContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitArrayLiteral(this);
}

JavaScriptParser::ArrayLiteralContext* JavaScriptParser::arrayLiteral() {
  ArrayLiteralContext *_localctx = _tracker.createInstance<ArrayLiteralContext>(_ctx, getState());
  enterRule(_localctx, 78, JavaScriptParser::RuleArrayLiteral);
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
    setState(508);
    match(JavaScriptParser::T__8);
    setState(510);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 1170372953163166290) != 0) || ((((_la - 65) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 65)) & 1578129) != 0)) {
      setState(509);
      elementList();
    }
    setState(512);
    match(JavaScriptParser::T__9);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ElementListContext ------------------------------------------------------------------

JavaScriptParser::ElementListContext::ElementListContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<JavaScriptParser::SingleExpressionContext *> JavaScriptParser::ElementListContext::singleExpression() {
  return getRuleContexts<JavaScriptParser::SingleExpressionContext>();
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::ElementListContext::singleExpression(size_t i) {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(i);
}

std::vector<JavaScriptParser::ElisionContext *> JavaScriptParser::ElementListContext::elision() {
  return getRuleContexts<JavaScriptParser::ElisionContext>();
}

JavaScriptParser::ElisionContext* JavaScriptParser::ElementListContext::elision(size_t i) {
  return getRuleContext<JavaScriptParser::ElisionContext>(i);
}


size_t JavaScriptParser::ElementListContext::getRuleIndex() const {
  return JavaScriptParser::RuleElementList;
}

void JavaScriptParser::ElementListContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterElementList(this);
}

void JavaScriptParser::ElementListContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitElementList(this);
}

JavaScriptParser::ElementListContext* JavaScriptParser::elementList() {
  ElementListContext *_localctx = _tracker.createInstance<ElementListContext>(_ctx, getState());
  enterRule(_localctx, 80, JavaScriptParser::RuleElementList);
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
    setState(515);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == JavaScriptParser::T__3) {
      setState(514);
      elision();
    }
    setState(517);
    singleExpression(0);
    setState(523);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == JavaScriptParser::T__3) {
      setState(518);
      elision();
      setState(519);
      singleExpression(0);
      setState(525);
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

//----------------- ElisionContext ------------------------------------------------------------------

JavaScriptParser::ElisionContext::ElisionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t JavaScriptParser::ElisionContext::getRuleIndex() const {
  return JavaScriptParser::RuleElision;
}

void JavaScriptParser::ElisionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterElision(this);
}

void JavaScriptParser::ElisionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitElision(this);
}

JavaScriptParser::ElisionContext* JavaScriptParser::elision() {
  ElisionContext *_localctx = _tracker.createInstance<ElisionContext>(_ctx, getState());
  enterRule(_localctx, 82, JavaScriptParser::RuleElision);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(526);
    match(JavaScriptParser::T__3);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ObjectLiteralContext ------------------------------------------------------------------

JavaScriptParser::ObjectLiteralContext::ObjectLiteralContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

JavaScriptParser::PropertyNameAndValueListContext* JavaScriptParser::ObjectLiteralContext::propertyNameAndValueList() {
  return getRuleContext<JavaScriptParser::PropertyNameAndValueListContext>(0);
}


size_t JavaScriptParser::ObjectLiteralContext::getRuleIndex() const {
  return JavaScriptParser::RuleObjectLiteral;
}

void JavaScriptParser::ObjectLiteralContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterObjectLiteral(this);
}

void JavaScriptParser::ObjectLiteralContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitObjectLiteral(this);
}

JavaScriptParser::ObjectLiteralContext* JavaScriptParser::objectLiteral() {
  ObjectLiteralContext *_localctx = _tracker.createInstance<ObjectLiteralContext>(_ctx, getState());
  enterRule(_localctx, 84, JavaScriptParser::RuleObjectLiteral);
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
    setState(528);
    match(JavaScriptParser::T__0);
    setState(530);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (((((_la - 49) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 49)) & 137438953471) != 0)) {
      setState(529);
      propertyNameAndValueList();
    }
    setState(533);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == JavaScriptParser::T__3) {
      setState(532);
      match(JavaScriptParser::T__3);
    }
    setState(535);
    match(JavaScriptParser::T__1);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PropertyNameAndValueListContext ------------------------------------------------------------------

JavaScriptParser::PropertyNameAndValueListContext::PropertyNameAndValueListContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<JavaScriptParser::PropertyAssignmentContext *> JavaScriptParser::PropertyNameAndValueListContext::propertyAssignment() {
  return getRuleContexts<JavaScriptParser::PropertyAssignmentContext>();
}

JavaScriptParser::PropertyAssignmentContext* JavaScriptParser::PropertyNameAndValueListContext::propertyAssignment(size_t i) {
  return getRuleContext<JavaScriptParser::PropertyAssignmentContext>(i);
}


size_t JavaScriptParser::PropertyNameAndValueListContext::getRuleIndex() const {
  return JavaScriptParser::RulePropertyNameAndValueList;
}

void JavaScriptParser::PropertyNameAndValueListContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPropertyNameAndValueList(this);
}

void JavaScriptParser::PropertyNameAndValueListContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPropertyNameAndValueList(this);
}

JavaScriptParser::PropertyNameAndValueListContext* JavaScriptParser::propertyNameAndValueList() {
  PropertyNameAndValueListContext *_localctx = _tracker.createInstance<PropertyNameAndValueListContext>(_ctx, getState());
  enterRule(_localctx, 86, JavaScriptParser::RulePropertyNameAndValueList);

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
    setState(537);
    propertyAssignment();
    setState(542);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 55, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(538);
        match(JavaScriptParser::T__3);
        setState(539);
        propertyAssignment(); 
      }
      setState(544);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 55, _ctx);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PropertyAssignmentContext ------------------------------------------------------------------

JavaScriptParser::PropertyAssignmentContext::PropertyAssignmentContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t JavaScriptParser::PropertyAssignmentContext::getRuleIndex() const {
  return JavaScriptParser::RulePropertyAssignment;
}

void JavaScriptParser::PropertyAssignmentContext::copyFrom(PropertyAssignmentContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- PropertyExpressionAssignmentContext ------------------------------------------------------------------

JavaScriptParser::PropertyNameContext* JavaScriptParser::PropertyExpressionAssignmentContext::propertyName() {
  return getRuleContext<JavaScriptParser::PropertyNameContext>(0);
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::PropertyExpressionAssignmentContext::singleExpression() {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(0);
}

JavaScriptParser::PropertyExpressionAssignmentContext::PropertyExpressionAssignmentContext(PropertyAssignmentContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::PropertyExpressionAssignmentContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPropertyExpressionAssignment(this);
}
void JavaScriptParser::PropertyExpressionAssignmentContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPropertyExpressionAssignment(this);
}
//----------------- PropertySetterContext ------------------------------------------------------------------

tree::TerminalNode* JavaScriptParser::PropertySetterContext::SETTER() {
  return getToken(JavaScriptParser::SETTER, 0);
}

JavaScriptParser::PropertyNameContext* JavaScriptParser::PropertySetterContext::propertyName() {
  return getRuleContext<JavaScriptParser::PropertyNameContext>(0);
}

JavaScriptParser::PropertySetParameterListContext* JavaScriptParser::PropertySetterContext::propertySetParameterList() {
  return getRuleContext<JavaScriptParser::PropertySetParameterListContext>(0);
}

JavaScriptParser::FunctionBodyContext* JavaScriptParser::PropertySetterContext::functionBody() {
  return getRuleContext<JavaScriptParser::FunctionBodyContext>(0);
}

JavaScriptParser::PropertySetterContext::PropertySetterContext(PropertyAssignmentContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::PropertySetterContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPropertySetter(this);
}
void JavaScriptParser::PropertySetterContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPropertySetter(this);
}
//----------------- PropertyGetterContext ------------------------------------------------------------------

tree::TerminalNode* JavaScriptParser::PropertyGetterContext::GETTER() {
  return getToken(JavaScriptParser::GETTER, 0);
}

JavaScriptParser::PropertyNameContext* JavaScriptParser::PropertyGetterContext::propertyName() {
  return getRuleContext<JavaScriptParser::PropertyNameContext>(0);
}

JavaScriptParser::FunctionBodyContext* JavaScriptParser::PropertyGetterContext::functionBody() {
  return getRuleContext<JavaScriptParser::FunctionBodyContext>(0);
}

JavaScriptParser::PropertyGetterContext::PropertyGetterContext(PropertyAssignmentContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::PropertyGetterContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPropertyGetter(this);
}
void JavaScriptParser::PropertyGetterContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPropertyGetter(this);
}
JavaScriptParser::PropertyAssignmentContext* JavaScriptParser::propertyAssignment() {
  PropertyAssignmentContext *_localctx = _tracker.createInstance<PropertyAssignmentContext>(_ctx, getState());
  enterRule(_localctx, 88, JavaScriptParser::RulePropertyAssignment);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(566);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case JavaScriptParser::NULL_:
      case JavaScriptParser::BooleanLiteral:
      case JavaScriptParser::DecimalLiteral:
      case JavaScriptParser::HexIntegerLiteral:
      case JavaScriptParser::OctalIntegerLiteral:
      case JavaScriptParser::BREAK:
      case JavaScriptParser::CASE:
      case JavaScriptParser::CATCH:
      case JavaScriptParser::CONTINUE:
      case JavaScriptParser::DEBUGGER:
      case JavaScriptParser::DEFAULT:
      case JavaScriptParser::DELETE:
      case JavaScriptParser::DO:
      case JavaScriptParser::ELSE:
      case JavaScriptParser::FINALLY:
      case JavaScriptParser::FOR:
      case JavaScriptParser::FUNCTION:
      case JavaScriptParser::IF:
      case JavaScriptParser::IN:
      case JavaScriptParser::INSTANCEOF:
      case JavaScriptParser::NEW:
      case JavaScriptParser::RETURN:
      case JavaScriptParser::SWITCH:
      case JavaScriptParser::THIS:
      case JavaScriptParser::THROW:
      case JavaScriptParser::TRY:
      case JavaScriptParser::TYPEOF:
      case JavaScriptParser::VAR:
      case JavaScriptParser::VOID:
      case JavaScriptParser::WHILE:
      case JavaScriptParser::WITH:
      case JavaScriptParser::CONST:
      case JavaScriptParser::LET:
      case JavaScriptParser::StringLiteral:
      case JavaScriptParser::Identifier: {
        _localctx = _tracker.createInstance<JavaScriptParser::PropertyExpressionAssignmentContext>(_localctx);
        enterOuterAlt(_localctx, 1);
        setState(545);
        propertyName();
        setState(546);
        match(JavaScriptParser::T__7);
        setState(547);
        singleExpression(0);
        break;
      }

      case JavaScriptParser::GETTER: {
        _localctx = _tracker.createInstance<JavaScriptParser::PropertyGetterContext>(_localctx);
        enterOuterAlt(_localctx, 2);
        setState(549);
        match(JavaScriptParser::GETTER);
        setState(550);
        propertyName();
        setState(551);
        match(JavaScriptParser::T__5);
        setState(552);
        match(JavaScriptParser::T__6);
        setState(553);
        match(JavaScriptParser::T__0);
        setState(554);
        functionBody();
        setState(555);
        match(JavaScriptParser::T__1);
        break;
      }

      case JavaScriptParser::SETTER: {
        _localctx = _tracker.createInstance<JavaScriptParser::PropertySetterContext>(_localctx);
        enterOuterAlt(_localctx, 3);
        setState(557);
        match(JavaScriptParser::SETTER);
        setState(558);
        propertyName();
        setState(559);
        match(JavaScriptParser::T__5);
        setState(560);
        propertySetParameterList();
        setState(561);
        match(JavaScriptParser::T__6);
        setState(562);
        match(JavaScriptParser::T__0);
        setState(563);
        functionBody();
        setState(564);
        match(JavaScriptParser::T__1);
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

//----------------- PropertyNameContext ------------------------------------------------------------------

JavaScriptParser::PropertyNameContext::PropertyNameContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

JavaScriptParser::IdentifierNameContext* JavaScriptParser::PropertyNameContext::identifierName() {
  return getRuleContext<JavaScriptParser::IdentifierNameContext>(0);
}

tree::TerminalNode* JavaScriptParser::PropertyNameContext::StringLiteral() {
  return getToken(JavaScriptParser::StringLiteral, 0);
}

JavaScriptParser::NumericLiteralContext* JavaScriptParser::PropertyNameContext::numericLiteral() {
  return getRuleContext<JavaScriptParser::NumericLiteralContext>(0);
}


size_t JavaScriptParser::PropertyNameContext::getRuleIndex() const {
  return JavaScriptParser::RulePropertyName;
}

void JavaScriptParser::PropertyNameContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPropertyName(this);
}

void JavaScriptParser::PropertyNameContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPropertyName(this);
}

JavaScriptParser::PropertyNameContext* JavaScriptParser::propertyName() {
  PropertyNameContext *_localctx = _tracker.createInstance<PropertyNameContext>(_ctx, getState());
  enterRule(_localctx, 90, JavaScriptParser::RulePropertyName);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(571);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case JavaScriptParser::NULL_:
      case JavaScriptParser::BooleanLiteral:
      case JavaScriptParser::BREAK:
      case JavaScriptParser::CASE:
      case JavaScriptParser::CATCH:
      case JavaScriptParser::CONTINUE:
      case JavaScriptParser::DEBUGGER:
      case JavaScriptParser::DEFAULT:
      case JavaScriptParser::DELETE:
      case JavaScriptParser::DO:
      case JavaScriptParser::ELSE:
      case JavaScriptParser::FINALLY:
      case JavaScriptParser::FOR:
      case JavaScriptParser::FUNCTION:
      case JavaScriptParser::IF:
      case JavaScriptParser::IN:
      case JavaScriptParser::INSTANCEOF:
      case JavaScriptParser::NEW:
      case JavaScriptParser::RETURN:
      case JavaScriptParser::SWITCH:
      case JavaScriptParser::THIS:
      case JavaScriptParser::THROW:
      case JavaScriptParser::TRY:
      case JavaScriptParser::TYPEOF:
      case JavaScriptParser::VAR:
      case JavaScriptParser::VOID:
      case JavaScriptParser::WHILE:
      case JavaScriptParser::WITH:
      case JavaScriptParser::CONST:
      case JavaScriptParser::LET:
      case JavaScriptParser::Identifier: {
        enterOuterAlt(_localctx, 1);
        setState(568);
        identifierName();
        break;
      }

      case JavaScriptParser::StringLiteral: {
        enterOuterAlt(_localctx, 2);
        setState(569);
        match(JavaScriptParser::StringLiteral);
        break;
      }

      case JavaScriptParser::DecimalLiteral:
      case JavaScriptParser::HexIntegerLiteral:
      case JavaScriptParser::OctalIntegerLiteral: {
        enterOuterAlt(_localctx, 3);
        setState(570);
        numericLiteral();
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

//----------------- PropertySetParameterListContext ------------------------------------------------------------------

JavaScriptParser::PropertySetParameterListContext::PropertySetParameterListContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::PropertySetParameterListContext::Identifier() {
  return getToken(JavaScriptParser::Identifier, 0);
}


size_t JavaScriptParser::PropertySetParameterListContext::getRuleIndex() const {
  return JavaScriptParser::RulePropertySetParameterList;
}

void JavaScriptParser::PropertySetParameterListContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPropertySetParameterList(this);
}

void JavaScriptParser::PropertySetParameterListContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPropertySetParameterList(this);
}

JavaScriptParser::PropertySetParameterListContext* JavaScriptParser::propertySetParameterList() {
  PropertySetParameterListContext *_localctx = _tracker.createInstance<PropertySetParameterListContext>(_ctx, getState());
  enterRule(_localctx, 92, JavaScriptParser::RulePropertySetParameterList);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(573);
    match(JavaScriptParser::Identifier);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ArgumentsContext ------------------------------------------------------------------

JavaScriptParser::ArgumentsContext::ArgumentsContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

JavaScriptParser::ArgumentListContext* JavaScriptParser::ArgumentsContext::argumentList() {
  return getRuleContext<JavaScriptParser::ArgumentListContext>(0);
}


size_t JavaScriptParser::ArgumentsContext::getRuleIndex() const {
  return JavaScriptParser::RuleArguments;
}

void JavaScriptParser::ArgumentsContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterArguments(this);
}

void JavaScriptParser::ArgumentsContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitArguments(this);
}

JavaScriptParser::ArgumentsContext* JavaScriptParser::arguments() {
  ArgumentsContext *_localctx = _tracker.createInstance<ArgumentsContext>(_ctx, getState());
  enterRule(_localctx, 94, JavaScriptParser::RuleArguments);
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
    setState(575);
    match(JavaScriptParser::T__5);
    setState(577);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 1170372953163166274) != 0) || ((((_la - 65) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 65)) & 1578129) != 0)) {
      setState(576);
      argumentList();
    }
    setState(579);
    match(JavaScriptParser::T__6);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ArgumentListContext ------------------------------------------------------------------

JavaScriptParser::ArgumentListContext::ArgumentListContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<JavaScriptParser::SingleExpressionContext *> JavaScriptParser::ArgumentListContext::singleExpression() {
  return getRuleContexts<JavaScriptParser::SingleExpressionContext>();
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::ArgumentListContext::singleExpression(size_t i) {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(i);
}


size_t JavaScriptParser::ArgumentListContext::getRuleIndex() const {
  return JavaScriptParser::RuleArgumentList;
}

void JavaScriptParser::ArgumentListContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterArgumentList(this);
}

void JavaScriptParser::ArgumentListContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitArgumentList(this);
}

JavaScriptParser::ArgumentListContext* JavaScriptParser::argumentList() {
  ArgumentListContext *_localctx = _tracker.createInstance<ArgumentListContext>(_ctx, getState());
  enterRule(_localctx, 96, JavaScriptParser::RuleArgumentList);
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
    setState(581);
    singleExpression(0);
    setState(586);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == JavaScriptParser::T__3) {
      setState(582);
      match(JavaScriptParser::T__3);
      setState(583);
      singleExpression(0);
      setState(588);
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

bool JavaScriptParser::sempred(RuleContext *context, size_t ruleIndex, size_t predicateIndex) {
  switch (ruleIndex) {
    case 32: return singleExpressionSempred(antlrcpp::downCast<SingleExpressionContext *>(context), predicateIndex);

  default:
    break;
  }
  return true;
}

bool JavaScriptParser::singleExpressionSempred(SingleExpressionContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 0: return precpred(_ctx, 21);
    case 1: return precpred(_ctx, 20);
    case 2: return precpred(_ctx, 19);
    case 3: return precpred(_ctx, 18);
    case 4: return precpred(_ctx, 17);
    case 5: return precpred(_ctx, 16);
    case 6: return precpred(_ctx, 15);
    case 7: return precpred(_ctx, 14);
    case 8: return precpred(_ctx, 13);
    case 9: return precpred(_ctx, 12);
    case 10: return precpred(_ctx, 11);
    case 11: return precpred(_ctx, 10);
    case 12: return precpred(_ctx, 9);
    case 13: return precpred(_ctx, 8);
    case 14: return precpred(_ctx, 7);
    case 15: return precpred(_ctx, 36);
    case 16: return precpred(_ctx, 35);
    case 17: return precpred(_ctx, 34);
    case 18: return precpred(_ctx, 32);
    case 19: return precpred(_ctx, 31);

  default:
    break;
  }
  return true;
}

void JavaScriptParser::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  javascriptParserInitialize();
#else
  ::antlr4::internal::call_once(javascriptParserOnceFlag, javascriptParserInitialize);
#endif
}
