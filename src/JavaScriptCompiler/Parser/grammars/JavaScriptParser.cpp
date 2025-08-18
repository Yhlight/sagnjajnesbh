
// Generated from grammars/JavaScript.g4 by ANTLR 4.13.2


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
      "program", "statement", "block", "variableStatement", "variableDeclaration", 
      "expressionStatement", "ifStatement", "whileStatement", "forStatement", 
      "returnStatement", "functionDeclaration", "parameterList", "expression", 
      "assignmentExpression", "conditionalExpression", "logicalOrExpression", 
      "logicalAndExpression", "equalityExpression", "relationalExpression", 
      "additiveExpression", "multiplicativeExpression", "unaryExpression", 
      "postfixExpression", "leftHandSideExpression", "callExpression", "memberExpression", 
      "primaryExpression", "arguments", "argumentList", "arrayLiteral", 
      "elementList", "objectLiteral", "propertyNameAndValueList", "propertyAssignment", 
      "propertyName", "literal"
    },
    std::vector<std::string>{
      "", "'var'", "'let'", "'const'", "'if'", "'else'", "'while'", "'for'", 
      "'return'", "'function'", "'instanceof'", "'in'", "'typeof'", "'void'", 
      "'delete'", "'new'", "'this'", "", "", "'null'", "", "", "", "", "", 
      "", "'['", "']'", "'('", "')'", "'{'", "'}'", "';'", "','", "'='", 
      "'\\u003F'", "':'", "'.'", "'++'", "'--'", "'+'", "'-'", "'~'", "'!'", 
      "'*'", "'/'", "'%'", "'<'", "'>'", "'<='", "'>='", "'=='", "'!='", 
      "'==='", "'!=='", "'&&'", "'||'", "'+='", "'-='", "'*='", "'/='"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "MultiLineComment", "SingleLineComment", "NullLiteral", "BooleanLiteral", 
      "NumericLiteral", "StringLiteral", "Identifier", "WhiteSpaces", "LineTerminator", 
      "OpenBracket", "CloseBracket", "OpenParen", "CloseParen", "OpenBrace", 
      "CloseBrace", "SemiColon", "Comma", "Assign", "QuestionMark", "Colon", 
      "Dot", "PlusPlus", "MinusMinus", "Plus", "Minus", "BitNot", "Not", 
      "Multiply", "Divide", "Modulus", "LessThan", "MoreThan", "LessThanEquals", 
      "GreaterThanEquals", "Equals_", "NotEquals", "IdentityEquals", "IdentityNotEquals", 
      "And", "Or", "PlusAssign", "MinusAssign", "MultiplyAssign", "DivideAssign"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,60,351,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,27,7,27,2,28,7,
  	28,2,29,7,29,2,30,7,30,2,31,7,31,2,32,7,32,2,33,7,33,2,34,7,34,2,35,7,
  	35,1,0,5,0,74,8,0,10,0,12,0,77,9,0,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,
  	1,1,1,1,1,3,1,90,8,1,1,2,1,2,5,2,94,8,2,10,2,12,2,97,9,2,1,2,1,2,1,3,
  	1,3,1,3,1,3,5,3,105,8,3,10,3,12,3,108,9,3,1,3,3,3,111,8,3,1,4,1,4,1,4,
  	3,4,116,8,4,1,5,1,5,3,5,120,8,5,1,6,1,6,1,6,1,6,1,6,1,6,1,6,3,6,129,8,
  	6,1,7,1,7,1,7,1,7,1,7,1,7,1,8,1,8,1,8,1,8,3,8,141,8,8,1,8,1,8,3,8,145,
  	8,8,1,8,1,8,3,8,149,8,8,1,8,1,8,1,8,1,9,1,9,3,9,156,8,9,1,9,3,9,159,8,
  	9,1,10,1,10,1,10,1,10,3,10,165,8,10,1,10,1,10,1,10,1,11,1,11,1,11,5,11,
  	173,8,11,10,11,12,11,176,9,11,1,12,1,12,1,13,1,13,1,13,5,13,183,8,13,
  	10,13,12,13,186,9,13,1,14,1,14,1,14,1,14,1,14,1,14,3,14,194,8,14,1,15,
  	1,15,1,15,5,15,199,8,15,10,15,12,15,202,9,15,1,16,1,16,1,16,5,16,207,
  	8,16,10,16,12,16,210,9,16,1,17,1,17,1,17,5,17,215,8,17,10,17,12,17,218,
  	9,17,1,18,1,18,1,18,5,18,223,8,18,10,18,12,18,226,9,18,1,19,1,19,1,19,
  	5,19,231,8,19,10,19,12,19,234,9,19,1,20,1,20,1,20,5,20,239,8,20,10,20,
  	12,20,242,9,20,1,21,1,21,1,21,3,21,247,8,21,1,22,1,22,3,22,251,8,22,1,
  	23,1,23,3,23,255,8,23,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,5,
  	24,266,8,24,10,24,12,24,269,9,24,1,25,1,25,1,25,1,25,1,25,1,25,1,25,5,
  	25,278,8,25,10,25,12,25,281,9,25,1,25,1,25,1,25,3,25,286,8,25,3,25,288,
  	8,25,1,26,1,26,1,26,1,26,1,26,1,26,1,26,1,26,1,26,3,26,299,8,26,1,27,
  	1,27,3,27,303,8,27,1,27,1,27,1,28,1,28,1,28,5,28,310,8,28,10,28,12,28,
  	313,9,28,1,29,1,29,3,29,317,8,29,1,29,1,29,1,30,1,30,1,30,5,30,324,8,
  	30,10,30,12,30,327,9,30,1,31,1,31,3,31,331,8,31,1,31,1,31,1,32,1,32,1,
  	32,5,32,338,8,32,10,32,12,32,341,9,32,1,33,1,33,1,33,1,33,1,34,1,34,1,
  	35,1,35,1,35,0,0,36,0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,
  	36,38,40,42,44,46,48,50,52,54,56,58,60,62,64,66,68,70,0,10,1,0,1,3,2,
  	0,34,34,57,60,1,0,51,54,2,0,10,11,47,50,1,0,40,41,1,0,44,46,2,0,12,14,
  	38,43,1,0,38,39,1,0,21,23,1,0,19,22,366,0,75,1,0,0,0,2,89,1,0,0,0,4,91,
  	1,0,0,0,6,100,1,0,0,0,8,112,1,0,0,0,10,117,1,0,0,0,12,121,1,0,0,0,14,
  	130,1,0,0,0,16,136,1,0,0,0,18,153,1,0,0,0,20,160,1,0,0,0,22,169,1,0,0,
  	0,24,177,1,0,0,0,26,179,1,0,0,0,28,187,1,0,0,0,30,195,1,0,0,0,32,203,
  	1,0,0,0,34,211,1,0,0,0,36,219,1,0,0,0,38,227,1,0,0,0,40,235,1,0,0,0,42,
  	246,1,0,0,0,44,248,1,0,0,0,46,254,1,0,0,0,48,256,1,0,0,0,50,287,1,0,0,
  	0,52,298,1,0,0,0,54,300,1,0,0,0,56,306,1,0,0,0,58,314,1,0,0,0,60,320,
  	1,0,0,0,62,328,1,0,0,0,64,334,1,0,0,0,66,342,1,0,0,0,68,346,1,0,0,0,70,
  	348,1,0,0,0,72,74,3,2,1,0,73,72,1,0,0,0,74,77,1,0,0,0,75,73,1,0,0,0,75,
  	76,1,0,0,0,76,78,1,0,0,0,77,75,1,0,0,0,78,79,5,0,0,1,79,1,1,0,0,0,80,
  	90,3,4,2,0,81,90,3,6,3,0,82,90,3,10,5,0,83,90,3,12,6,0,84,90,3,14,7,0,
  	85,90,3,16,8,0,86,90,3,18,9,0,87,90,3,20,10,0,88,90,5,32,0,0,89,80,1,
  	0,0,0,89,81,1,0,0,0,89,82,1,0,0,0,89,83,1,0,0,0,89,84,1,0,0,0,89,85,1,
  	0,0,0,89,86,1,0,0,0,89,87,1,0,0,0,89,88,1,0,0,0,90,3,1,0,0,0,91,95,5,
  	30,0,0,92,94,3,2,1,0,93,92,1,0,0,0,94,97,1,0,0,0,95,93,1,0,0,0,95,96,
  	1,0,0,0,96,98,1,0,0,0,97,95,1,0,0,0,98,99,5,31,0,0,99,5,1,0,0,0,100,101,
  	7,0,0,0,101,106,3,8,4,0,102,103,5,33,0,0,103,105,3,8,4,0,104,102,1,0,
  	0,0,105,108,1,0,0,0,106,104,1,0,0,0,106,107,1,0,0,0,107,110,1,0,0,0,108,
  	106,1,0,0,0,109,111,5,32,0,0,110,109,1,0,0,0,110,111,1,0,0,0,111,7,1,
  	0,0,0,112,115,5,23,0,0,113,114,5,34,0,0,114,116,3,24,12,0,115,113,1,0,
  	0,0,115,116,1,0,0,0,116,9,1,0,0,0,117,119,3,24,12,0,118,120,5,32,0,0,
  	119,118,1,0,0,0,119,120,1,0,0,0,120,11,1,0,0,0,121,122,5,4,0,0,122,123,
  	5,28,0,0,123,124,3,24,12,0,124,125,5,29,0,0,125,128,3,2,1,0,126,127,5,
  	5,0,0,127,129,3,2,1,0,128,126,1,0,0,0,128,129,1,0,0,0,129,13,1,0,0,0,
  	130,131,5,6,0,0,131,132,5,28,0,0,132,133,3,24,12,0,133,134,5,29,0,0,134,
  	135,3,2,1,0,135,15,1,0,0,0,136,137,5,7,0,0,137,140,5,28,0,0,138,141,3,
  	6,3,0,139,141,3,24,12,0,140,138,1,0,0,0,140,139,1,0,0,0,140,141,1,0,0,
  	0,141,142,1,0,0,0,142,144,5,32,0,0,143,145,3,24,12,0,144,143,1,0,0,0,
  	144,145,1,0,0,0,145,146,1,0,0,0,146,148,5,32,0,0,147,149,3,24,12,0,148,
  	147,1,0,0,0,148,149,1,0,0,0,149,150,1,0,0,0,150,151,5,29,0,0,151,152,
  	3,2,1,0,152,17,1,0,0,0,153,155,5,8,0,0,154,156,3,24,12,0,155,154,1,0,
  	0,0,155,156,1,0,0,0,156,158,1,0,0,0,157,159,5,32,0,0,158,157,1,0,0,0,
  	158,159,1,0,0,0,159,19,1,0,0,0,160,161,5,9,0,0,161,162,5,23,0,0,162,164,
  	5,28,0,0,163,165,3,22,11,0,164,163,1,0,0,0,164,165,1,0,0,0,165,166,1,
  	0,0,0,166,167,5,29,0,0,167,168,3,4,2,0,168,21,1,0,0,0,169,174,5,23,0,
  	0,170,171,5,33,0,0,171,173,5,23,0,0,172,170,1,0,0,0,173,176,1,0,0,0,174,
  	172,1,0,0,0,174,175,1,0,0,0,175,23,1,0,0,0,176,174,1,0,0,0,177,178,3,
  	26,13,0,178,25,1,0,0,0,179,184,3,28,14,0,180,181,7,1,0,0,181,183,3,28,
  	14,0,182,180,1,0,0,0,183,186,1,0,0,0,184,182,1,0,0,0,184,185,1,0,0,0,
  	185,27,1,0,0,0,186,184,1,0,0,0,187,193,3,30,15,0,188,189,5,35,0,0,189,
  	190,3,24,12,0,190,191,5,36,0,0,191,192,3,28,14,0,192,194,1,0,0,0,193,
  	188,1,0,0,0,193,194,1,0,0,0,194,29,1,0,0,0,195,200,3,32,16,0,196,197,
  	5,56,0,0,197,199,3,32,16,0,198,196,1,0,0,0,199,202,1,0,0,0,200,198,1,
  	0,0,0,200,201,1,0,0,0,201,31,1,0,0,0,202,200,1,0,0,0,203,208,3,34,17,
  	0,204,205,5,55,0,0,205,207,3,34,17,0,206,204,1,0,0,0,207,210,1,0,0,0,
  	208,206,1,0,0,0,208,209,1,0,0,0,209,33,1,0,0,0,210,208,1,0,0,0,211,216,
  	3,36,18,0,212,213,7,2,0,0,213,215,3,36,18,0,214,212,1,0,0,0,215,218,1,
  	0,0,0,216,214,1,0,0,0,216,217,1,0,0,0,217,35,1,0,0,0,218,216,1,0,0,0,
  	219,224,3,38,19,0,220,221,7,3,0,0,221,223,3,38,19,0,222,220,1,0,0,0,223,
  	226,1,0,0,0,224,222,1,0,0,0,224,225,1,0,0,0,225,37,1,0,0,0,226,224,1,
  	0,0,0,227,232,3,40,20,0,228,229,7,4,0,0,229,231,3,40,20,0,230,228,1,0,
  	0,0,231,234,1,0,0,0,232,230,1,0,0,0,232,233,1,0,0,0,233,39,1,0,0,0,234,
  	232,1,0,0,0,235,240,3,42,21,0,236,237,7,5,0,0,237,239,3,42,21,0,238,236,
  	1,0,0,0,239,242,1,0,0,0,240,238,1,0,0,0,240,241,1,0,0,0,241,41,1,0,0,
  	0,242,240,1,0,0,0,243,244,7,6,0,0,244,247,3,42,21,0,245,247,3,44,22,0,
  	246,243,1,0,0,0,246,245,1,0,0,0,247,43,1,0,0,0,248,250,3,46,23,0,249,
  	251,7,7,0,0,250,249,1,0,0,0,250,251,1,0,0,0,251,45,1,0,0,0,252,255,3,
  	48,24,0,253,255,3,50,25,0,254,252,1,0,0,0,254,253,1,0,0,0,255,47,1,0,
  	0,0,256,257,3,50,25,0,257,267,3,54,27,0,258,266,3,54,27,0,259,260,5,26,
  	0,0,260,261,3,24,12,0,261,262,5,27,0,0,262,266,1,0,0,0,263,264,5,37,0,
  	0,264,266,5,23,0,0,265,258,1,0,0,0,265,259,1,0,0,0,265,263,1,0,0,0,266,
  	269,1,0,0,0,267,265,1,0,0,0,267,268,1,0,0,0,268,49,1,0,0,0,269,267,1,
  	0,0,0,270,279,3,52,26,0,271,272,5,26,0,0,272,273,3,24,12,0,273,274,5,
  	27,0,0,274,278,1,0,0,0,275,276,5,37,0,0,276,278,5,23,0,0,277,271,1,0,
  	0,0,277,275,1,0,0,0,278,281,1,0,0,0,279,277,1,0,0,0,279,280,1,0,0,0,280,
  	288,1,0,0,0,281,279,1,0,0,0,282,283,5,15,0,0,283,285,3,50,25,0,284,286,
  	3,54,27,0,285,284,1,0,0,0,285,286,1,0,0,0,286,288,1,0,0,0,287,270,1,0,
  	0,0,287,282,1,0,0,0,288,51,1,0,0,0,289,299,5,16,0,0,290,299,5,23,0,0,
  	291,299,3,70,35,0,292,299,3,58,29,0,293,299,3,62,31,0,294,295,5,28,0,
  	0,295,296,3,24,12,0,296,297,5,29,0,0,297,299,1,0,0,0,298,289,1,0,0,0,
  	298,290,1,0,0,0,298,291,1,0,0,0,298,292,1,0,0,0,298,293,1,0,0,0,298,294,
  	1,0,0,0,299,53,1,0,0,0,300,302,5,28,0,0,301,303,3,56,28,0,302,301,1,0,
  	0,0,302,303,1,0,0,0,303,304,1,0,0,0,304,305,5,29,0,0,305,55,1,0,0,0,306,
  	311,3,24,12,0,307,308,5,33,0,0,308,310,3,24,12,0,309,307,1,0,0,0,310,
  	313,1,0,0,0,311,309,1,0,0,0,311,312,1,0,0,0,312,57,1,0,0,0,313,311,1,
  	0,0,0,314,316,5,26,0,0,315,317,3,60,30,0,316,315,1,0,0,0,316,317,1,0,
  	0,0,317,318,1,0,0,0,318,319,5,27,0,0,319,59,1,0,0,0,320,325,3,24,12,0,
  	321,322,5,33,0,0,322,324,3,24,12,0,323,321,1,0,0,0,324,327,1,0,0,0,325,
  	323,1,0,0,0,325,326,1,0,0,0,326,61,1,0,0,0,327,325,1,0,0,0,328,330,5,
  	30,0,0,329,331,3,64,32,0,330,329,1,0,0,0,330,331,1,0,0,0,331,332,1,0,
  	0,0,332,333,5,31,0,0,333,63,1,0,0,0,334,339,3,66,33,0,335,336,5,33,0,
  	0,336,338,3,66,33,0,337,335,1,0,0,0,338,341,1,0,0,0,339,337,1,0,0,0,339,
  	340,1,0,0,0,340,65,1,0,0,0,341,339,1,0,0,0,342,343,3,68,34,0,343,344,
  	5,36,0,0,344,345,3,24,12,0,345,67,1,0,0,0,346,347,7,8,0,0,347,69,1,0,
  	0,0,348,349,7,9,0,0,349,71,1,0,0,0,39,75,89,95,106,110,115,119,128,140,
  	144,148,155,158,164,174,184,193,200,208,216,224,232,240,246,250,254,265,
  	267,277,279,285,287,298,302,311,316,325,330,339
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

std::vector<JavaScriptParser::StatementContext *> JavaScriptParser::ProgramContext::statement() {
  return getRuleContexts<JavaScriptParser::StatementContext>();
}

JavaScriptParser::StatementContext* JavaScriptParser::ProgramContext::statement(size_t i) {
  return getRuleContext<JavaScriptParser::StatementContext>(i);
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
    setState(75);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 17323028771806) != 0)) {
      setState(72);
      statement();
      setState(77);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(78);
    match(JavaScriptParser::EOF);
   
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

JavaScriptParser::ExpressionStatementContext* JavaScriptParser::StatementContext::expressionStatement() {
  return getRuleContext<JavaScriptParser::ExpressionStatementContext>(0);
}

JavaScriptParser::IfStatementContext* JavaScriptParser::StatementContext::ifStatement() {
  return getRuleContext<JavaScriptParser::IfStatementContext>(0);
}

JavaScriptParser::WhileStatementContext* JavaScriptParser::StatementContext::whileStatement() {
  return getRuleContext<JavaScriptParser::WhileStatementContext>(0);
}

JavaScriptParser::ForStatementContext* JavaScriptParser::StatementContext::forStatement() {
  return getRuleContext<JavaScriptParser::ForStatementContext>(0);
}

JavaScriptParser::ReturnStatementContext* JavaScriptParser::StatementContext::returnStatement() {
  return getRuleContext<JavaScriptParser::ReturnStatementContext>(0);
}

JavaScriptParser::FunctionDeclarationContext* JavaScriptParser::StatementContext::functionDeclaration() {
  return getRuleContext<JavaScriptParser::FunctionDeclarationContext>(0);
}

tree::TerminalNode* JavaScriptParser::StatementContext::SemiColon() {
  return getToken(JavaScriptParser::SemiColon, 0);
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
  enterRule(_localctx, 2, JavaScriptParser::RuleStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(89);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 1, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(80);
      block();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(81);
      variableStatement();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(82);
      expressionStatement();
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(83);
      ifStatement();
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(84);
      whileStatement();
      break;
    }

    case 6: {
      enterOuterAlt(_localctx, 6);
      setState(85);
      forStatement();
      break;
    }

    case 7: {
      enterOuterAlt(_localctx, 7);
      setState(86);
      returnStatement();
      break;
    }

    case 8: {
      enterOuterAlt(_localctx, 8);
      setState(87);
      functionDeclaration();
      break;
    }

    case 9: {
      enterOuterAlt(_localctx, 9);
      setState(88);
      match(JavaScriptParser::SemiColon);
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

tree::TerminalNode* JavaScriptParser::BlockContext::OpenBrace() {
  return getToken(JavaScriptParser::OpenBrace, 0);
}

tree::TerminalNode* JavaScriptParser::BlockContext::CloseBrace() {
  return getToken(JavaScriptParser::CloseBrace, 0);
}

std::vector<JavaScriptParser::StatementContext *> JavaScriptParser::BlockContext::statement() {
  return getRuleContexts<JavaScriptParser::StatementContext>();
}

JavaScriptParser::StatementContext* JavaScriptParser::BlockContext::statement(size_t i) {
  return getRuleContext<JavaScriptParser::StatementContext>(i);
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
  enterRule(_localctx, 4, JavaScriptParser::RuleBlock);
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
    setState(91);
    match(JavaScriptParser::OpenBrace);
    setState(95);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 17323028771806) != 0)) {
      setState(92);
      statement();
      setState(97);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(98);
    match(JavaScriptParser::CloseBrace);
   
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

std::vector<JavaScriptParser::VariableDeclarationContext *> JavaScriptParser::VariableStatementContext::variableDeclaration() {
  return getRuleContexts<JavaScriptParser::VariableDeclarationContext>();
}

JavaScriptParser::VariableDeclarationContext* JavaScriptParser::VariableStatementContext::variableDeclaration(size_t i) {
  return getRuleContext<JavaScriptParser::VariableDeclarationContext>(i);
}

std::vector<tree::TerminalNode *> JavaScriptParser::VariableStatementContext::Comma() {
  return getTokens(JavaScriptParser::Comma);
}

tree::TerminalNode* JavaScriptParser::VariableStatementContext::Comma(size_t i) {
  return getToken(JavaScriptParser::Comma, i);
}

tree::TerminalNode* JavaScriptParser::VariableStatementContext::SemiColon() {
  return getToken(JavaScriptParser::SemiColon, 0);
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
  enterRule(_localctx, 6, JavaScriptParser::RuleVariableStatement);
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
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 14) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(101);
    variableDeclaration();
    setState(106);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == JavaScriptParser::Comma) {
      setState(102);
      match(JavaScriptParser::Comma);
      setState(103);
      variableDeclaration();
      setState(108);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(110);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 4, _ctx)) {
    case 1: {
      setState(109);
      match(JavaScriptParser::SemiColon);
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

JavaScriptParser::VariableDeclarationContext::VariableDeclarationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::VariableDeclarationContext::Identifier() {
  return getToken(JavaScriptParser::Identifier, 0);
}

tree::TerminalNode* JavaScriptParser::VariableDeclarationContext::Assign() {
  return getToken(JavaScriptParser::Assign, 0);
}

JavaScriptParser::ExpressionContext* JavaScriptParser::VariableDeclarationContext::expression() {
  return getRuleContext<JavaScriptParser::ExpressionContext>(0);
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
  enterRule(_localctx, 8, JavaScriptParser::RuleVariableDeclaration);
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
    setState(112);
    match(JavaScriptParser::Identifier);
    setState(115);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == JavaScriptParser::Assign) {
      setState(113);
      match(JavaScriptParser::Assign);
      setState(114);
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

//----------------- ExpressionStatementContext ------------------------------------------------------------------

JavaScriptParser::ExpressionStatementContext::ExpressionStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

JavaScriptParser::ExpressionContext* JavaScriptParser::ExpressionStatementContext::expression() {
  return getRuleContext<JavaScriptParser::ExpressionContext>(0);
}

tree::TerminalNode* JavaScriptParser::ExpressionStatementContext::SemiColon() {
  return getToken(JavaScriptParser::SemiColon, 0);
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
  enterRule(_localctx, 10, JavaScriptParser::RuleExpressionStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(117);
    expression();
    setState(119);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 6, _ctx)) {
    case 1: {
      setState(118);
      match(JavaScriptParser::SemiColon);
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

tree::TerminalNode* JavaScriptParser::IfStatementContext::OpenParen() {
  return getToken(JavaScriptParser::OpenParen, 0);
}

JavaScriptParser::ExpressionContext* JavaScriptParser::IfStatementContext::expression() {
  return getRuleContext<JavaScriptParser::ExpressionContext>(0);
}

tree::TerminalNode* JavaScriptParser::IfStatementContext::CloseParen() {
  return getToken(JavaScriptParser::CloseParen, 0);
}

std::vector<JavaScriptParser::StatementContext *> JavaScriptParser::IfStatementContext::statement() {
  return getRuleContexts<JavaScriptParser::StatementContext>();
}

JavaScriptParser::StatementContext* JavaScriptParser::IfStatementContext::statement(size_t i) {
  return getRuleContext<JavaScriptParser::StatementContext>(i);
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
  enterRule(_localctx, 12, JavaScriptParser::RuleIfStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(121);
    match(JavaScriptParser::T__3);
    setState(122);
    match(JavaScriptParser::OpenParen);
    setState(123);
    expression();
    setState(124);
    match(JavaScriptParser::CloseParen);
    setState(125);
    statement();
    setState(128);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 7, _ctx)) {
    case 1: {
      setState(126);
      match(JavaScriptParser::T__4);
      setState(127);
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

JavaScriptParser::WhileStatementContext::WhileStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::WhileStatementContext::OpenParen() {
  return getToken(JavaScriptParser::OpenParen, 0);
}

JavaScriptParser::ExpressionContext* JavaScriptParser::WhileStatementContext::expression() {
  return getRuleContext<JavaScriptParser::ExpressionContext>(0);
}

tree::TerminalNode* JavaScriptParser::WhileStatementContext::CloseParen() {
  return getToken(JavaScriptParser::CloseParen, 0);
}

JavaScriptParser::StatementContext* JavaScriptParser::WhileStatementContext::statement() {
  return getRuleContext<JavaScriptParser::StatementContext>(0);
}


size_t JavaScriptParser::WhileStatementContext::getRuleIndex() const {
  return JavaScriptParser::RuleWhileStatement;
}

void JavaScriptParser::WhileStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterWhileStatement(this);
}

void JavaScriptParser::WhileStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitWhileStatement(this);
}

JavaScriptParser::WhileStatementContext* JavaScriptParser::whileStatement() {
  WhileStatementContext *_localctx = _tracker.createInstance<WhileStatementContext>(_ctx, getState());
  enterRule(_localctx, 14, JavaScriptParser::RuleWhileStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(130);
    match(JavaScriptParser::T__5);
    setState(131);
    match(JavaScriptParser::OpenParen);
    setState(132);
    expression();
    setState(133);
    match(JavaScriptParser::CloseParen);
    setState(134);
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

JavaScriptParser::ForStatementContext::ForStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::ForStatementContext::OpenParen() {
  return getToken(JavaScriptParser::OpenParen, 0);
}

std::vector<tree::TerminalNode *> JavaScriptParser::ForStatementContext::SemiColon() {
  return getTokens(JavaScriptParser::SemiColon);
}

tree::TerminalNode* JavaScriptParser::ForStatementContext::SemiColon(size_t i) {
  return getToken(JavaScriptParser::SemiColon, i);
}

tree::TerminalNode* JavaScriptParser::ForStatementContext::CloseParen() {
  return getToken(JavaScriptParser::CloseParen, 0);
}

JavaScriptParser::StatementContext* JavaScriptParser::ForStatementContext::statement() {
  return getRuleContext<JavaScriptParser::StatementContext>(0);
}

JavaScriptParser::VariableStatementContext* JavaScriptParser::ForStatementContext::variableStatement() {
  return getRuleContext<JavaScriptParser::VariableStatementContext>(0);
}

std::vector<JavaScriptParser::ExpressionContext *> JavaScriptParser::ForStatementContext::expression() {
  return getRuleContexts<JavaScriptParser::ExpressionContext>();
}

JavaScriptParser::ExpressionContext* JavaScriptParser::ForStatementContext::expression(size_t i) {
  return getRuleContext<JavaScriptParser::ExpressionContext>(i);
}


size_t JavaScriptParser::ForStatementContext::getRuleIndex() const {
  return JavaScriptParser::RuleForStatement;
}

void JavaScriptParser::ForStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterForStatement(this);
}

void JavaScriptParser::ForStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitForStatement(this);
}

JavaScriptParser::ForStatementContext* JavaScriptParser::forStatement() {
  ForStatementContext *_localctx = _tracker.createInstance<ForStatementContext>(_ctx, getState());
  enterRule(_localctx, 16, JavaScriptParser::RuleForStatement);
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
    match(JavaScriptParser::T__6);
    setState(137);
    match(JavaScriptParser::OpenParen);
    setState(140);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case JavaScriptParser::T__0:
      case JavaScriptParser::T__1:
      case JavaScriptParser::T__2: {
        setState(138);
        variableStatement();
        break;
      }

      case JavaScriptParser::T__11:
      case JavaScriptParser::T__12:
      case JavaScriptParser::T__13:
      case JavaScriptParser::T__14:
      case JavaScriptParser::T__15:
      case JavaScriptParser::NullLiteral:
      case JavaScriptParser::BooleanLiteral:
      case JavaScriptParser::NumericLiteral:
      case JavaScriptParser::StringLiteral:
      case JavaScriptParser::Identifier:
      case JavaScriptParser::OpenBracket:
      case JavaScriptParser::OpenParen:
      case JavaScriptParser::OpenBrace:
      case JavaScriptParser::PlusPlus:
      case JavaScriptParser::MinusMinus:
      case JavaScriptParser::Plus:
      case JavaScriptParser::Minus:
      case JavaScriptParser::BitNot:
      case JavaScriptParser::Not: {
        setState(139);
        expression();
        break;
      }

      case JavaScriptParser::SemiColon: {
        break;
      }

    default:
      break;
    }
    setState(142);
    match(JavaScriptParser::SemiColon);
    setState(144);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 17318733803520) != 0)) {
      setState(143);
      expression();
    }
    setState(146);
    match(JavaScriptParser::SemiColon);
    setState(148);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 17318733803520) != 0)) {
      setState(147);
      expression();
    }
    setState(150);
    match(JavaScriptParser::CloseParen);
    setState(151);
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

JavaScriptParser::ReturnStatementContext::ReturnStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

JavaScriptParser::ExpressionContext* JavaScriptParser::ReturnStatementContext::expression() {
  return getRuleContext<JavaScriptParser::ExpressionContext>(0);
}

tree::TerminalNode* JavaScriptParser::ReturnStatementContext::SemiColon() {
  return getToken(JavaScriptParser::SemiColon, 0);
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
  enterRule(_localctx, 18, JavaScriptParser::RuleReturnStatement);

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
    match(JavaScriptParser::T__7);
    setState(155);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 11, _ctx)) {
    case 1: {
      setState(154);
      expression();
      break;
    }

    default:
      break;
    }
    setState(158);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 12, _ctx)) {
    case 1: {
      setState(157);
      match(JavaScriptParser::SemiColon);
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

tree::TerminalNode* JavaScriptParser::FunctionDeclarationContext::Identifier() {
  return getToken(JavaScriptParser::Identifier, 0);
}

tree::TerminalNode* JavaScriptParser::FunctionDeclarationContext::OpenParen() {
  return getToken(JavaScriptParser::OpenParen, 0);
}

tree::TerminalNode* JavaScriptParser::FunctionDeclarationContext::CloseParen() {
  return getToken(JavaScriptParser::CloseParen, 0);
}

JavaScriptParser::BlockContext* JavaScriptParser::FunctionDeclarationContext::block() {
  return getRuleContext<JavaScriptParser::BlockContext>(0);
}

JavaScriptParser::ParameterListContext* JavaScriptParser::FunctionDeclarationContext::parameterList() {
  return getRuleContext<JavaScriptParser::ParameterListContext>(0);
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
  enterRule(_localctx, 20, JavaScriptParser::RuleFunctionDeclaration);
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
    setState(160);
    match(JavaScriptParser::T__8);
    setState(161);
    match(JavaScriptParser::Identifier);
    setState(162);
    match(JavaScriptParser::OpenParen);
    setState(164);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == JavaScriptParser::Identifier) {
      setState(163);
      parameterList();
    }
    setState(166);
    match(JavaScriptParser::CloseParen);
    setState(167);
    block();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ParameterListContext ------------------------------------------------------------------

JavaScriptParser::ParameterListContext::ParameterListContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> JavaScriptParser::ParameterListContext::Identifier() {
  return getTokens(JavaScriptParser::Identifier);
}

tree::TerminalNode* JavaScriptParser::ParameterListContext::Identifier(size_t i) {
  return getToken(JavaScriptParser::Identifier, i);
}

std::vector<tree::TerminalNode *> JavaScriptParser::ParameterListContext::Comma() {
  return getTokens(JavaScriptParser::Comma);
}

tree::TerminalNode* JavaScriptParser::ParameterListContext::Comma(size_t i) {
  return getToken(JavaScriptParser::Comma, i);
}


size_t JavaScriptParser::ParameterListContext::getRuleIndex() const {
  return JavaScriptParser::RuleParameterList;
}

void JavaScriptParser::ParameterListContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterParameterList(this);
}

void JavaScriptParser::ParameterListContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitParameterList(this);
}

JavaScriptParser::ParameterListContext* JavaScriptParser::parameterList() {
  ParameterListContext *_localctx = _tracker.createInstance<ParameterListContext>(_ctx, getState());
  enterRule(_localctx, 22, JavaScriptParser::RuleParameterList);
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
    setState(169);
    match(JavaScriptParser::Identifier);
    setState(174);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == JavaScriptParser::Comma) {
      setState(170);
      match(JavaScriptParser::Comma);
      setState(171);
      match(JavaScriptParser::Identifier);
      setState(176);
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

//----------------- ExpressionContext ------------------------------------------------------------------

JavaScriptParser::ExpressionContext::ExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

JavaScriptParser::AssignmentExpressionContext* JavaScriptParser::ExpressionContext::assignmentExpression() {
  return getRuleContext<JavaScriptParser::AssignmentExpressionContext>(0);
}


size_t JavaScriptParser::ExpressionContext::getRuleIndex() const {
  return JavaScriptParser::RuleExpression;
}

void JavaScriptParser::ExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterExpression(this);
}

void JavaScriptParser::ExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitExpression(this);
}

JavaScriptParser::ExpressionContext* JavaScriptParser::expression() {
  ExpressionContext *_localctx = _tracker.createInstance<ExpressionContext>(_ctx, getState());
  enterRule(_localctx, 24, JavaScriptParser::RuleExpression);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(177);
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

JavaScriptParser::AssignmentExpressionContext::AssignmentExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<JavaScriptParser::ConditionalExpressionContext *> JavaScriptParser::AssignmentExpressionContext::conditionalExpression() {
  return getRuleContexts<JavaScriptParser::ConditionalExpressionContext>();
}

JavaScriptParser::ConditionalExpressionContext* JavaScriptParser::AssignmentExpressionContext::conditionalExpression(size_t i) {
  return getRuleContext<JavaScriptParser::ConditionalExpressionContext>(i);
}

std::vector<tree::TerminalNode *> JavaScriptParser::AssignmentExpressionContext::Assign() {
  return getTokens(JavaScriptParser::Assign);
}

tree::TerminalNode* JavaScriptParser::AssignmentExpressionContext::Assign(size_t i) {
  return getToken(JavaScriptParser::Assign, i);
}

std::vector<tree::TerminalNode *> JavaScriptParser::AssignmentExpressionContext::PlusAssign() {
  return getTokens(JavaScriptParser::PlusAssign);
}

tree::TerminalNode* JavaScriptParser::AssignmentExpressionContext::PlusAssign(size_t i) {
  return getToken(JavaScriptParser::PlusAssign, i);
}

std::vector<tree::TerminalNode *> JavaScriptParser::AssignmentExpressionContext::MinusAssign() {
  return getTokens(JavaScriptParser::MinusAssign);
}

tree::TerminalNode* JavaScriptParser::AssignmentExpressionContext::MinusAssign(size_t i) {
  return getToken(JavaScriptParser::MinusAssign, i);
}

std::vector<tree::TerminalNode *> JavaScriptParser::AssignmentExpressionContext::MultiplyAssign() {
  return getTokens(JavaScriptParser::MultiplyAssign);
}

tree::TerminalNode* JavaScriptParser::AssignmentExpressionContext::MultiplyAssign(size_t i) {
  return getToken(JavaScriptParser::MultiplyAssign, i);
}

std::vector<tree::TerminalNode *> JavaScriptParser::AssignmentExpressionContext::DivideAssign() {
  return getTokens(JavaScriptParser::DivideAssign);
}

tree::TerminalNode* JavaScriptParser::AssignmentExpressionContext::DivideAssign(size_t i) {
  return getToken(JavaScriptParser::DivideAssign, i);
}


size_t JavaScriptParser::AssignmentExpressionContext::getRuleIndex() const {
  return JavaScriptParser::RuleAssignmentExpression;
}

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

JavaScriptParser::AssignmentExpressionContext* JavaScriptParser::assignmentExpression() {
  AssignmentExpressionContext *_localctx = _tracker.createInstance<AssignmentExpressionContext>(_ctx, getState());
  enterRule(_localctx, 26, JavaScriptParser::RuleAssignmentExpression);
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
    setState(179);
    conditionalExpression();
    setState(184);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 2161727838317707264) != 0)) {
      setState(180);
      _la = _input->LA(1);
      if (!((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 2161727838317707264) != 0))) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(181);
      conditionalExpression();
      setState(186);
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

//----------------- ConditionalExpressionContext ------------------------------------------------------------------

JavaScriptParser::ConditionalExpressionContext::ConditionalExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

JavaScriptParser::LogicalOrExpressionContext* JavaScriptParser::ConditionalExpressionContext::logicalOrExpression() {
  return getRuleContext<JavaScriptParser::LogicalOrExpressionContext>(0);
}

tree::TerminalNode* JavaScriptParser::ConditionalExpressionContext::QuestionMark() {
  return getToken(JavaScriptParser::QuestionMark, 0);
}

JavaScriptParser::ExpressionContext* JavaScriptParser::ConditionalExpressionContext::expression() {
  return getRuleContext<JavaScriptParser::ExpressionContext>(0);
}

tree::TerminalNode* JavaScriptParser::ConditionalExpressionContext::Colon() {
  return getToken(JavaScriptParser::Colon, 0);
}

JavaScriptParser::ConditionalExpressionContext* JavaScriptParser::ConditionalExpressionContext::conditionalExpression() {
  return getRuleContext<JavaScriptParser::ConditionalExpressionContext>(0);
}


size_t JavaScriptParser::ConditionalExpressionContext::getRuleIndex() const {
  return JavaScriptParser::RuleConditionalExpression;
}

void JavaScriptParser::ConditionalExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterConditionalExpression(this);
}

void JavaScriptParser::ConditionalExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitConditionalExpression(this);
}

JavaScriptParser::ConditionalExpressionContext* JavaScriptParser::conditionalExpression() {
  ConditionalExpressionContext *_localctx = _tracker.createInstance<ConditionalExpressionContext>(_ctx, getState());
  enterRule(_localctx, 28, JavaScriptParser::RuleConditionalExpression);
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
    setState(187);
    logicalOrExpression();
    setState(193);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == JavaScriptParser::QuestionMark) {
      setState(188);
      match(JavaScriptParser::QuestionMark);
      setState(189);
      expression();
      setState(190);
      match(JavaScriptParser::Colon);
      setState(191);
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

JavaScriptParser::LogicalOrExpressionContext::LogicalOrExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<JavaScriptParser::LogicalAndExpressionContext *> JavaScriptParser::LogicalOrExpressionContext::logicalAndExpression() {
  return getRuleContexts<JavaScriptParser::LogicalAndExpressionContext>();
}

JavaScriptParser::LogicalAndExpressionContext* JavaScriptParser::LogicalOrExpressionContext::logicalAndExpression(size_t i) {
  return getRuleContext<JavaScriptParser::LogicalAndExpressionContext>(i);
}

std::vector<tree::TerminalNode *> JavaScriptParser::LogicalOrExpressionContext::Or() {
  return getTokens(JavaScriptParser::Or);
}

tree::TerminalNode* JavaScriptParser::LogicalOrExpressionContext::Or(size_t i) {
  return getToken(JavaScriptParser::Or, i);
}


size_t JavaScriptParser::LogicalOrExpressionContext::getRuleIndex() const {
  return JavaScriptParser::RuleLogicalOrExpression;
}

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

JavaScriptParser::LogicalOrExpressionContext* JavaScriptParser::logicalOrExpression() {
  LogicalOrExpressionContext *_localctx = _tracker.createInstance<LogicalOrExpressionContext>(_ctx, getState());
  enterRule(_localctx, 30, JavaScriptParser::RuleLogicalOrExpression);
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
    setState(195);
    logicalAndExpression();
    setState(200);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == JavaScriptParser::Or) {
      setState(196);
      match(JavaScriptParser::Or);
      setState(197);
      logicalAndExpression();
      setState(202);
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

JavaScriptParser::LogicalAndExpressionContext::LogicalAndExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<JavaScriptParser::EqualityExpressionContext *> JavaScriptParser::LogicalAndExpressionContext::equalityExpression() {
  return getRuleContexts<JavaScriptParser::EqualityExpressionContext>();
}

JavaScriptParser::EqualityExpressionContext* JavaScriptParser::LogicalAndExpressionContext::equalityExpression(size_t i) {
  return getRuleContext<JavaScriptParser::EqualityExpressionContext>(i);
}

std::vector<tree::TerminalNode *> JavaScriptParser::LogicalAndExpressionContext::And() {
  return getTokens(JavaScriptParser::And);
}

tree::TerminalNode* JavaScriptParser::LogicalAndExpressionContext::And(size_t i) {
  return getToken(JavaScriptParser::And, i);
}


size_t JavaScriptParser::LogicalAndExpressionContext::getRuleIndex() const {
  return JavaScriptParser::RuleLogicalAndExpression;
}

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

JavaScriptParser::LogicalAndExpressionContext* JavaScriptParser::logicalAndExpression() {
  LogicalAndExpressionContext *_localctx = _tracker.createInstance<LogicalAndExpressionContext>(_ctx, getState());
  enterRule(_localctx, 32, JavaScriptParser::RuleLogicalAndExpression);
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
    setState(203);
    equalityExpression();
    setState(208);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == JavaScriptParser::And) {
      setState(204);
      match(JavaScriptParser::And);
      setState(205);
      equalityExpression();
      setState(210);
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

JavaScriptParser::EqualityExpressionContext::EqualityExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<JavaScriptParser::RelationalExpressionContext *> JavaScriptParser::EqualityExpressionContext::relationalExpression() {
  return getRuleContexts<JavaScriptParser::RelationalExpressionContext>();
}

JavaScriptParser::RelationalExpressionContext* JavaScriptParser::EqualityExpressionContext::relationalExpression(size_t i) {
  return getRuleContext<JavaScriptParser::RelationalExpressionContext>(i);
}

std::vector<tree::TerminalNode *> JavaScriptParser::EqualityExpressionContext::Equals_() {
  return getTokens(JavaScriptParser::Equals_);
}

tree::TerminalNode* JavaScriptParser::EqualityExpressionContext::Equals_(size_t i) {
  return getToken(JavaScriptParser::Equals_, i);
}

std::vector<tree::TerminalNode *> JavaScriptParser::EqualityExpressionContext::NotEquals() {
  return getTokens(JavaScriptParser::NotEquals);
}

tree::TerminalNode* JavaScriptParser::EqualityExpressionContext::NotEquals(size_t i) {
  return getToken(JavaScriptParser::NotEquals, i);
}

std::vector<tree::TerminalNode *> JavaScriptParser::EqualityExpressionContext::IdentityEquals() {
  return getTokens(JavaScriptParser::IdentityEquals);
}

tree::TerminalNode* JavaScriptParser::EqualityExpressionContext::IdentityEquals(size_t i) {
  return getToken(JavaScriptParser::IdentityEquals, i);
}

std::vector<tree::TerminalNode *> JavaScriptParser::EqualityExpressionContext::IdentityNotEquals() {
  return getTokens(JavaScriptParser::IdentityNotEquals);
}

tree::TerminalNode* JavaScriptParser::EqualityExpressionContext::IdentityNotEquals(size_t i) {
  return getToken(JavaScriptParser::IdentityNotEquals, i);
}


size_t JavaScriptParser::EqualityExpressionContext::getRuleIndex() const {
  return JavaScriptParser::RuleEqualityExpression;
}

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

JavaScriptParser::EqualityExpressionContext* JavaScriptParser::equalityExpression() {
  EqualityExpressionContext *_localctx = _tracker.createInstance<EqualityExpressionContext>(_ctx, getState());
  enterRule(_localctx, 34, JavaScriptParser::RuleEqualityExpression);
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
    setState(211);
    relationalExpression();
    setState(216);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 33776997205278720) != 0)) {
      setState(212);
      _la = _input->LA(1);
      if (!((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 33776997205278720) != 0))) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(213);
      relationalExpression();
      setState(218);
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

JavaScriptParser::RelationalExpressionContext::RelationalExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<JavaScriptParser::AdditiveExpressionContext *> JavaScriptParser::RelationalExpressionContext::additiveExpression() {
  return getRuleContexts<JavaScriptParser::AdditiveExpressionContext>();
}

JavaScriptParser::AdditiveExpressionContext* JavaScriptParser::RelationalExpressionContext::additiveExpression(size_t i) {
  return getRuleContext<JavaScriptParser::AdditiveExpressionContext>(i);
}

std::vector<tree::TerminalNode *> JavaScriptParser::RelationalExpressionContext::LessThan() {
  return getTokens(JavaScriptParser::LessThan);
}

tree::TerminalNode* JavaScriptParser::RelationalExpressionContext::LessThan(size_t i) {
  return getToken(JavaScriptParser::LessThan, i);
}

std::vector<tree::TerminalNode *> JavaScriptParser::RelationalExpressionContext::MoreThan() {
  return getTokens(JavaScriptParser::MoreThan);
}

tree::TerminalNode* JavaScriptParser::RelationalExpressionContext::MoreThan(size_t i) {
  return getToken(JavaScriptParser::MoreThan, i);
}

std::vector<tree::TerminalNode *> JavaScriptParser::RelationalExpressionContext::LessThanEquals() {
  return getTokens(JavaScriptParser::LessThanEquals);
}

tree::TerminalNode* JavaScriptParser::RelationalExpressionContext::LessThanEquals(size_t i) {
  return getToken(JavaScriptParser::LessThanEquals, i);
}

std::vector<tree::TerminalNode *> JavaScriptParser::RelationalExpressionContext::GreaterThanEquals() {
  return getTokens(JavaScriptParser::GreaterThanEquals);
}

tree::TerminalNode* JavaScriptParser::RelationalExpressionContext::GreaterThanEquals(size_t i) {
  return getToken(JavaScriptParser::GreaterThanEquals, i);
}


size_t JavaScriptParser::RelationalExpressionContext::getRuleIndex() const {
  return JavaScriptParser::RuleRelationalExpression;
}

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

JavaScriptParser::RelationalExpressionContext* JavaScriptParser::relationalExpression() {
  RelationalExpressionContext *_localctx = _tracker.createInstance<RelationalExpressionContext>(_ctx, getState());
  enterRule(_localctx, 36, JavaScriptParser::RuleRelationalExpression);
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
    setState(219);
    additiveExpression();
    setState(224);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 2111062325332992) != 0)) {
      setState(220);
      _la = _input->LA(1);
      if (!((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 2111062325332992) != 0))) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(221);
      additiveExpression();
      setState(226);
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

JavaScriptParser::AdditiveExpressionContext::AdditiveExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<JavaScriptParser::MultiplicativeExpressionContext *> JavaScriptParser::AdditiveExpressionContext::multiplicativeExpression() {
  return getRuleContexts<JavaScriptParser::MultiplicativeExpressionContext>();
}

JavaScriptParser::MultiplicativeExpressionContext* JavaScriptParser::AdditiveExpressionContext::multiplicativeExpression(size_t i) {
  return getRuleContext<JavaScriptParser::MultiplicativeExpressionContext>(i);
}

std::vector<tree::TerminalNode *> JavaScriptParser::AdditiveExpressionContext::Plus() {
  return getTokens(JavaScriptParser::Plus);
}

tree::TerminalNode* JavaScriptParser::AdditiveExpressionContext::Plus(size_t i) {
  return getToken(JavaScriptParser::Plus, i);
}

std::vector<tree::TerminalNode *> JavaScriptParser::AdditiveExpressionContext::Minus() {
  return getTokens(JavaScriptParser::Minus);
}

tree::TerminalNode* JavaScriptParser::AdditiveExpressionContext::Minus(size_t i) {
  return getToken(JavaScriptParser::Minus, i);
}


size_t JavaScriptParser::AdditiveExpressionContext::getRuleIndex() const {
  return JavaScriptParser::RuleAdditiveExpression;
}

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

JavaScriptParser::AdditiveExpressionContext* JavaScriptParser::additiveExpression() {
  AdditiveExpressionContext *_localctx = _tracker.createInstance<AdditiveExpressionContext>(_ctx, getState());
  enterRule(_localctx, 38, JavaScriptParser::RuleAdditiveExpression);
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
    setState(227);
    multiplicativeExpression();
    setState(232);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 21, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(228);
        _la = _input->LA(1);
        if (!(_la == JavaScriptParser::Plus

        || _la == JavaScriptParser::Minus)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(229);
        multiplicativeExpression(); 
      }
      setState(234);
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

JavaScriptParser::MultiplicativeExpressionContext::MultiplicativeExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<JavaScriptParser::UnaryExpressionContext *> JavaScriptParser::MultiplicativeExpressionContext::unaryExpression() {
  return getRuleContexts<JavaScriptParser::UnaryExpressionContext>();
}

JavaScriptParser::UnaryExpressionContext* JavaScriptParser::MultiplicativeExpressionContext::unaryExpression(size_t i) {
  return getRuleContext<JavaScriptParser::UnaryExpressionContext>(i);
}

std::vector<tree::TerminalNode *> JavaScriptParser::MultiplicativeExpressionContext::Multiply() {
  return getTokens(JavaScriptParser::Multiply);
}

tree::TerminalNode* JavaScriptParser::MultiplicativeExpressionContext::Multiply(size_t i) {
  return getToken(JavaScriptParser::Multiply, i);
}

std::vector<tree::TerminalNode *> JavaScriptParser::MultiplicativeExpressionContext::Divide() {
  return getTokens(JavaScriptParser::Divide);
}

tree::TerminalNode* JavaScriptParser::MultiplicativeExpressionContext::Divide(size_t i) {
  return getToken(JavaScriptParser::Divide, i);
}

std::vector<tree::TerminalNode *> JavaScriptParser::MultiplicativeExpressionContext::Modulus() {
  return getTokens(JavaScriptParser::Modulus);
}

tree::TerminalNode* JavaScriptParser::MultiplicativeExpressionContext::Modulus(size_t i) {
  return getToken(JavaScriptParser::Modulus, i);
}


size_t JavaScriptParser::MultiplicativeExpressionContext::getRuleIndex() const {
  return JavaScriptParser::RuleMultiplicativeExpression;
}

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

JavaScriptParser::MultiplicativeExpressionContext* JavaScriptParser::multiplicativeExpression() {
  MultiplicativeExpressionContext *_localctx = _tracker.createInstance<MultiplicativeExpressionContext>(_ctx, getState());
  enterRule(_localctx, 40, JavaScriptParser::RuleMultiplicativeExpression);
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
    setState(235);
    unaryExpression();
    setState(240);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 123145302310912) != 0)) {
      setState(236);
      _la = _input->LA(1);
      if (!((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 123145302310912) != 0))) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(237);
      unaryExpression();
      setState(242);
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

JavaScriptParser::UnaryExpressionContext::UnaryExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

JavaScriptParser::UnaryExpressionContext* JavaScriptParser::UnaryExpressionContext::unaryExpression() {
  return getRuleContext<JavaScriptParser::UnaryExpressionContext>(0);
}

tree::TerminalNode* JavaScriptParser::UnaryExpressionContext::PlusPlus() {
  return getToken(JavaScriptParser::PlusPlus, 0);
}

tree::TerminalNode* JavaScriptParser::UnaryExpressionContext::MinusMinus() {
  return getToken(JavaScriptParser::MinusMinus, 0);
}

tree::TerminalNode* JavaScriptParser::UnaryExpressionContext::Plus() {
  return getToken(JavaScriptParser::Plus, 0);
}

tree::TerminalNode* JavaScriptParser::UnaryExpressionContext::Minus() {
  return getToken(JavaScriptParser::Minus, 0);
}

tree::TerminalNode* JavaScriptParser::UnaryExpressionContext::BitNot() {
  return getToken(JavaScriptParser::BitNot, 0);
}

tree::TerminalNode* JavaScriptParser::UnaryExpressionContext::Not() {
  return getToken(JavaScriptParser::Not, 0);
}

JavaScriptParser::PostfixExpressionContext* JavaScriptParser::UnaryExpressionContext::postfixExpression() {
  return getRuleContext<JavaScriptParser::PostfixExpressionContext>(0);
}


size_t JavaScriptParser::UnaryExpressionContext::getRuleIndex() const {
  return JavaScriptParser::RuleUnaryExpression;
}

void JavaScriptParser::UnaryExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterUnaryExpression(this);
}

void JavaScriptParser::UnaryExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitUnaryExpression(this);
}

JavaScriptParser::UnaryExpressionContext* JavaScriptParser::unaryExpression() {
  UnaryExpressionContext *_localctx = _tracker.createInstance<UnaryExpressionContext>(_ctx, getState());
  enterRule(_localctx, 42, JavaScriptParser::RuleUnaryExpression);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(246);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case JavaScriptParser::T__11:
      case JavaScriptParser::T__12:
      case JavaScriptParser::T__13:
      case JavaScriptParser::PlusPlus:
      case JavaScriptParser::MinusMinus:
      case JavaScriptParser::Plus:
      case JavaScriptParser::Minus:
      case JavaScriptParser::BitNot:
      case JavaScriptParser::Not: {
        enterOuterAlt(_localctx, 1);
        setState(243);
        _la = _input->LA(1);
        if (!((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & 17317308166144) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(244);
        unaryExpression();
        break;
      }

      case JavaScriptParser::T__14:
      case JavaScriptParser::T__15:
      case JavaScriptParser::NullLiteral:
      case JavaScriptParser::BooleanLiteral:
      case JavaScriptParser::NumericLiteral:
      case JavaScriptParser::StringLiteral:
      case JavaScriptParser::Identifier:
      case JavaScriptParser::OpenBracket:
      case JavaScriptParser::OpenParen:
      case JavaScriptParser::OpenBrace: {
        enterOuterAlt(_localctx, 2);
        setState(245);
        postfixExpression();
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

JavaScriptParser::PostfixExpressionContext::PostfixExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

JavaScriptParser::LeftHandSideExpressionContext* JavaScriptParser::PostfixExpressionContext::leftHandSideExpression() {
  return getRuleContext<JavaScriptParser::LeftHandSideExpressionContext>(0);
}

tree::TerminalNode* JavaScriptParser::PostfixExpressionContext::PlusPlus() {
  return getToken(JavaScriptParser::PlusPlus, 0);
}

tree::TerminalNode* JavaScriptParser::PostfixExpressionContext::MinusMinus() {
  return getToken(JavaScriptParser::MinusMinus, 0);
}


size_t JavaScriptParser::PostfixExpressionContext::getRuleIndex() const {
  return JavaScriptParser::RulePostfixExpression;
}

void JavaScriptParser::PostfixExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPostfixExpression(this);
}

void JavaScriptParser::PostfixExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPostfixExpression(this);
}

JavaScriptParser::PostfixExpressionContext* JavaScriptParser::postfixExpression() {
  PostfixExpressionContext *_localctx = _tracker.createInstance<PostfixExpressionContext>(_ctx, getState());
  enterRule(_localctx, 44, JavaScriptParser::RulePostfixExpression);
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
    setState(248);
    leftHandSideExpression();
    setState(250);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 24, _ctx)) {
    case 1: {
      setState(249);
      _la = _input->LA(1);
      if (!(_la == JavaScriptParser::PlusPlus

      || _la == JavaScriptParser::MinusMinus)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
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

//----------------- LeftHandSideExpressionContext ------------------------------------------------------------------

JavaScriptParser::LeftHandSideExpressionContext::LeftHandSideExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

JavaScriptParser::CallExpressionContext* JavaScriptParser::LeftHandSideExpressionContext::callExpression() {
  return getRuleContext<JavaScriptParser::CallExpressionContext>(0);
}

JavaScriptParser::MemberExpressionContext* JavaScriptParser::LeftHandSideExpressionContext::memberExpression() {
  return getRuleContext<JavaScriptParser::MemberExpressionContext>(0);
}


size_t JavaScriptParser::LeftHandSideExpressionContext::getRuleIndex() const {
  return JavaScriptParser::RuleLeftHandSideExpression;
}

void JavaScriptParser::LeftHandSideExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLeftHandSideExpression(this);
}

void JavaScriptParser::LeftHandSideExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLeftHandSideExpression(this);
}

JavaScriptParser::LeftHandSideExpressionContext* JavaScriptParser::leftHandSideExpression() {
  LeftHandSideExpressionContext *_localctx = _tracker.createInstance<LeftHandSideExpressionContext>(_ctx, getState());
  enterRule(_localctx, 46, JavaScriptParser::RuleLeftHandSideExpression);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(254);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 25, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(252);
      callExpression();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(253);
      memberExpression();
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

//----------------- CallExpressionContext ------------------------------------------------------------------

JavaScriptParser::CallExpressionContext::CallExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

JavaScriptParser::MemberExpressionContext* JavaScriptParser::CallExpressionContext::memberExpression() {
  return getRuleContext<JavaScriptParser::MemberExpressionContext>(0);
}

std::vector<JavaScriptParser::ArgumentsContext *> JavaScriptParser::CallExpressionContext::arguments() {
  return getRuleContexts<JavaScriptParser::ArgumentsContext>();
}

JavaScriptParser::ArgumentsContext* JavaScriptParser::CallExpressionContext::arguments(size_t i) {
  return getRuleContext<JavaScriptParser::ArgumentsContext>(i);
}

std::vector<tree::TerminalNode *> JavaScriptParser::CallExpressionContext::OpenBracket() {
  return getTokens(JavaScriptParser::OpenBracket);
}

tree::TerminalNode* JavaScriptParser::CallExpressionContext::OpenBracket(size_t i) {
  return getToken(JavaScriptParser::OpenBracket, i);
}

std::vector<JavaScriptParser::ExpressionContext *> JavaScriptParser::CallExpressionContext::expression() {
  return getRuleContexts<JavaScriptParser::ExpressionContext>();
}

JavaScriptParser::ExpressionContext* JavaScriptParser::CallExpressionContext::expression(size_t i) {
  return getRuleContext<JavaScriptParser::ExpressionContext>(i);
}

std::vector<tree::TerminalNode *> JavaScriptParser::CallExpressionContext::CloseBracket() {
  return getTokens(JavaScriptParser::CloseBracket);
}

tree::TerminalNode* JavaScriptParser::CallExpressionContext::CloseBracket(size_t i) {
  return getToken(JavaScriptParser::CloseBracket, i);
}

std::vector<tree::TerminalNode *> JavaScriptParser::CallExpressionContext::Dot() {
  return getTokens(JavaScriptParser::Dot);
}

tree::TerminalNode* JavaScriptParser::CallExpressionContext::Dot(size_t i) {
  return getToken(JavaScriptParser::Dot, i);
}

std::vector<tree::TerminalNode *> JavaScriptParser::CallExpressionContext::Identifier() {
  return getTokens(JavaScriptParser::Identifier);
}

tree::TerminalNode* JavaScriptParser::CallExpressionContext::Identifier(size_t i) {
  return getToken(JavaScriptParser::Identifier, i);
}


size_t JavaScriptParser::CallExpressionContext::getRuleIndex() const {
  return JavaScriptParser::RuleCallExpression;
}

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

JavaScriptParser::CallExpressionContext* JavaScriptParser::callExpression() {
  CallExpressionContext *_localctx = _tracker.createInstance<CallExpressionContext>(_ctx, getState());
  enterRule(_localctx, 48, JavaScriptParser::RuleCallExpression);

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
    setState(256);
    memberExpression();
    setState(257);
    arguments();
    setState(267);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 27, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(265);
        _errHandler->sync(this);
        switch (_input->LA(1)) {
          case JavaScriptParser::OpenParen: {
            setState(258);
            arguments();
            break;
          }

          case JavaScriptParser::OpenBracket: {
            setState(259);
            match(JavaScriptParser::OpenBracket);
            setState(260);
            expression();
            setState(261);
            match(JavaScriptParser::CloseBracket);
            break;
          }

          case JavaScriptParser::Dot: {
            setState(263);
            match(JavaScriptParser::Dot);
            setState(264);
            match(JavaScriptParser::Identifier);
            break;
          }

        default:
          throw NoViableAltException(this);
        } 
      }
      setState(269);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 27, _ctx);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- MemberExpressionContext ------------------------------------------------------------------

JavaScriptParser::MemberExpressionContext::MemberExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

JavaScriptParser::PrimaryExpressionContext* JavaScriptParser::MemberExpressionContext::primaryExpression() {
  return getRuleContext<JavaScriptParser::PrimaryExpressionContext>(0);
}

std::vector<tree::TerminalNode *> JavaScriptParser::MemberExpressionContext::OpenBracket() {
  return getTokens(JavaScriptParser::OpenBracket);
}

tree::TerminalNode* JavaScriptParser::MemberExpressionContext::OpenBracket(size_t i) {
  return getToken(JavaScriptParser::OpenBracket, i);
}

std::vector<JavaScriptParser::ExpressionContext *> JavaScriptParser::MemberExpressionContext::expression() {
  return getRuleContexts<JavaScriptParser::ExpressionContext>();
}

JavaScriptParser::ExpressionContext* JavaScriptParser::MemberExpressionContext::expression(size_t i) {
  return getRuleContext<JavaScriptParser::ExpressionContext>(i);
}

std::vector<tree::TerminalNode *> JavaScriptParser::MemberExpressionContext::CloseBracket() {
  return getTokens(JavaScriptParser::CloseBracket);
}

tree::TerminalNode* JavaScriptParser::MemberExpressionContext::CloseBracket(size_t i) {
  return getToken(JavaScriptParser::CloseBracket, i);
}

std::vector<tree::TerminalNode *> JavaScriptParser::MemberExpressionContext::Dot() {
  return getTokens(JavaScriptParser::Dot);
}

tree::TerminalNode* JavaScriptParser::MemberExpressionContext::Dot(size_t i) {
  return getToken(JavaScriptParser::Dot, i);
}

std::vector<tree::TerminalNode *> JavaScriptParser::MemberExpressionContext::Identifier() {
  return getTokens(JavaScriptParser::Identifier);
}

tree::TerminalNode* JavaScriptParser::MemberExpressionContext::Identifier(size_t i) {
  return getToken(JavaScriptParser::Identifier, i);
}

JavaScriptParser::MemberExpressionContext* JavaScriptParser::MemberExpressionContext::memberExpression() {
  return getRuleContext<JavaScriptParser::MemberExpressionContext>(0);
}

JavaScriptParser::ArgumentsContext* JavaScriptParser::MemberExpressionContext::arguments() {
  return getRuleContext<JavaScriptParser::ArgumentsContext>(0);
}


size_t JavaScriptParser::MemberExpressionContext::getRuleIndex() const {
  return JavaScriptParser::RuleMemberExpression;
}

void JavaScriptParser::MemberExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMemberExpression(this);
}

void JavaScriptParser::MemberExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMemberExpression(this);
}

JavaScriptParser::MemberExpressionContext* JavaScriptParser::memberExpression() {
  MemberExpressionContext *_localctx = _tracker.createInstance<MemberExpressionContext>(_ctx, getState());
  enterRule(_localctx, 50, JavaScriptParser::RuleMemberExpression);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    size_t alt;
    setState(287);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case JavaScriptParser::T__15:
      case JavaScriptParser::NullLiteral:
      case JavaScriptParser::BooleanLiteral:
      case JavaScriptParser::NumericLiteral:
      case JavaScriptParser::StringLiteral:
      case JavaScriptParser::Identifier:
      case JavaScriptParser::OpenBracket:
      case JavaScriptParser::OpenParen:
      case JavaScriptParser::OpenBrace: {
        enterOuterAlt(_localctx, 1);
        setState(270);
        primaryExpression();
        setState(279);
        _errHandler->sync(this);
        alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 29, _ctx);
        while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
          if (alt == 1) {
            setState(277);
            _errHandler->sync(this);
            switch (_input->LA(1)) {
              case JavaScriptParser::OpenBracket: {
                setState(271);
                match(JavaScriptParser::OpenBracket);
                setState(272);
                expression();
                setState(273);
                match(JavaScriptParser::CloseBracket);
                break;
              }

              case JavaScriptParser::Dot: {
                setState(275);
                match(JavaScriptParser::Dot);
                setState(276);
                match(JavaScriptParser::Identifier);
                break;
              }

            default:
              throw NoViableAltException(this);
            } 
          }
          setState(281);
          _errHandler->sync(this);
          alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 29, _ctx);
        }
        break;
      }

      case JavaScriptParser::T__14: {
        enterOuterAlt(_localctx, 2);
        setState(282);
        match(JavaScriptParser::T__14);
        setState(283);
        memberExpression();
        setState(285);
        _errHandler->sync(this);

        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 30, _ctx)) {
        case 1: {
          setState(284);
          arguments();
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

//----------------- PrimaryExpressionContext ------------------------------------------------------------------

JavaScriptParser::PrimaryExpressionContext::PrimaryExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::PrimaryExpressionContext::Identifier() {
  return getToken(JavaScriptParser::Identifier, 0);
}

JavaScriptParser::LiteralContext* JavaScriptParser::PrimaryExpressionContext::literal() {
  return getRuleContext<JavaScriptParser::LiteralContext>(0);
}

JavaScriptParser::ArrayLiteralContext* JavaScriptParser::PrimaryExpressionContext::arrayLiteral() {
  return getRuleContext<JavaScriptParser::ArrayLiteralContext>(0);
}

JavaScriptParser::ObjectLiteralContext* JavaScriptParser::PrimaryExpressionContext::objectLiteral() {
  return getRuleContext<JavaScriptParser::ObjectLiteralContext>(0);
}

tree::TerminalNode* JavaScriptParser::PrimaryExpressionContext::OpenParen() {
  return getToken(JavaScriptParser::OpenParen, 0);
}

JavaScriptParser::ExpressionContext* JavaScriptParser::PrimaryExpressionContext::expression() {
  return getRuleContext<JavaScriptParser::ExpressionContext>(0);
}

tree::TerminalNode* JavaScriptParser::PrimaryExpressionContext::CloseParen() {
  return getToken(JavaScriptParser::CloseParen, 0);
}


size_t JavaScriptParser::PrimaryExpressionContext::getRuleIndex() const {
  return JavaScriptParser::RulePrimaryExpression;
}

void JavaScriptParser::PrimaryExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPrimaryExpression(this);
}

void JavaScriptParser::PrimaryExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPrimaryExpression(this);
}

JavaScriptParser::PrimaryExpressionContext* JavaScriptParser::primaryExpression() {
  PrimaryExpressionContext *_localctx = _tracker.createInstance<PrimaryExpressionContext>(_ctx, getState());
  enterRule(_localctx, 52, JavaScriptParser::RulePrimaryExpression);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(298);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case JavaScriptParser::T__15: {
        enterOuterAlt(_localctx, 1);
        setState(289);
        match(JavaScriptParser::T__15);
        break;
      }

      case JavaScriptParser::Identifier: {
        enterOuterAlt(_localctx, 2);
        setState(290);
        match(JavaScriptParser::Identifier);
        break;
      }

      case JavaScriptParser::NullLiteral:
      case JavaScriptParser::BooleanLiteral:
      case JavaScriptParser::NumericLiteral:
      case JavaScriptParser::StringLiteral: {
        enterOuterAlt(_localctx, 3);
        setState(291);
        literal();
        break;
      }

      case JavaScriptParser::OpenBracket: {
        enterOuterAlt(_localctx, 4);
        setState(292);
        arrayLiteral();
        break;
      }

      case JavaScriptParser::OpenBrace: {
        enterOuterAlt(_localctx, 5);
        setState(293);
        objectLiteral();
        break;
      }

      case JavaScriptParser::OpenParen: {
        enterOuterAlt(_localctx, 6);
        setState(294);
        match(JavaScriptParser::OpenParen);
        setState(295);
        expression();
        setState(296);
        match(JavaScriptParser::CloseParen);
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

//----------------- ArgumentsContext ------------------------------------------------------------------

JavaScriptParser::ArgumentsContext::ArgumentsContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::ArgumentsContext::OpenParen() {
  return getToken(JavaScriptParser::OpenParen, 0);
}

tree::TerminalNode* JavaScriptParser::ArgumentsContext::CloseParen() {
  return getToken(JavaScriptParser::CloseParen, 0);
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
  enterRule(_localctx, 54, JavaScriptParser::RuleArguments);
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
    setState(300);
    match(JavaScriptParser::OpenParen);
    setState(302);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 17318733803520) != 0)) {
      setState(301);
      argumentList();
    }
    setState(304);
    match(JavaScriptParser::CloseParen);
   
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

std::vector<JavaScriptParser::ExpressionContext *> JavaScriptParser::ArgumentListContext::expression() {
  return getRuleContexts<JavaScriptParser::ExpressionContext>();
}

JavaScriptParser::ExpressionContext* JavaScriptParser::ArgumentListContext::expression(size_t i) {
  return getRuleContext<JavaScriptParser::ExpressionContext>(i);
}

std::vector<tree::TerminalNode *> JavaScriptParser::ArgumentListContext::Comma() {
  return getTokens(JavaScriptParser::Comma);
}

tree::TerminalNode* JavaScriptParser::ArgumentListContext::Comma(size_t i) {
  return getToken(JavaScriptParser::Comma, i);
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
  enterRule(_localctx, 56, JavaScriptParser::RuleArgumentList);
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
    setState(306);
    expression();
    setState(311);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == JavaScriptParser::Comma) {
      setState(307);
      match(JavaScriptParser::Comma);
      setState(308);
      expression();
      setState(313);
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

//----------------- ArrayLiteralContext ------------------------------------------------------------------

JavaScriptParser::ArrayLiteralContext::ArrayLiteralContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::ArrayLiteralContext::OpenBracket() {
  return getToken(JavaScriptParser::OpenBracket, 0);
}

tree::TerminalNode* JavaScriptParser::ArrayLiteralContext::CloseBracket() {
  return getToken(JavaScriptParser::CloseBracket, 0);
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
  enterRule(_localctx, 58, JavaScriptParser::RuleArrayLiteral);
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
    setState(314);
    match(JavaScriptParser::OpenBracket);
    setState(316);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 17318733803520) != 0)) {
      setState(315);
      elementList();
    }
    setState(318);
    match(JavaScriptParser::CloseBracket);
   
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

std::vector<JavaScriptParser::ExpressionContext *> JavaScriptParser::ElementListContext::expression() {
  return getRuleContexts<JavaScriptParser::ExpressionContext>();
}

JavaScriptParser::ExpressionContext* JavaScriptParser::ElementListContext::expression(size_t i) {
  return getRuleContext<JavaScriptParser::ExpressionContext>(i);
}

std::vector<tree::TerminalNode *> JavaScriptParser::ElementListContext::Comma() {
  return getTokens(JavaScriptParser::Comma);
}

tree::TerminalNode* JavaScriptParser::ElementListContext::Comma(size_t i) {
  return getToken(JavaScriptParser::Comma, i);
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
  enterRule(_localctx, 60, JavaScriptParser::RuleElementList);
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
    setState(320);
    expression();
    setState(325);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == JavaScriptParser::Comma) {
      setState(321);
      match(JavaScriptParser::Comma);
      setState(322);
      expression();
      setState(327);
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

//----------------- ObjectLiteralContext ------------------------------------------------------------------

JavaScriptParser::ObjectLiteralContext::ObjectLiteralContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::ObjectLiteralContext::OpenBrace() {
  return getToken(JavaScriptParser::OpenBrace, 0);
}

tree::TerminalNode* JavaScriptParser::ObjectLiteralContext::CloseBrace() {
  return getToken(JavaScriptParser::CloseBrace, 0);
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
  enterRule(_localctx, 62, JavaScriptParser::RuleObjectLiteral);
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
    setState(328);
    match(JavaScriptParser::OpenBrace);
    setState(330);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 14680064) != 0)) {
      setState(329);
      propertyNameAndValueList();
    }
    setState(332);
    match(JavaScriptParser::CloseBrace);
   
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

std::vector<tree::TerminalNode *> JavaScriptParser::PropertyNameAndValueListContext::Comma() {
  return getTokens(JavaScriptParser::Comma);
}

tree::TerminalNode* JavaScriptParser::PropertyNameAndValueListContext::Comma(size_t i) {
  return getToken(JavaScriptParser::Comma, i);
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
  enterRule(_localctx, 64, JavaScriptParser::RulePropertyNameAndValueList);
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
    setState(334);
    propertyAssignment();
    setState(339);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == JavaScriptParser::Comma) {
      setState(335);
      match(JavaScriptParser::Comma);
      setState(336);
      propertyAssignment();
      setState(341);
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

//----------------- PropertyAssignmentContext ------------------------------------------------------------------

JavaScriptParser::PropertyAssignmentContext::PropertyAssignmentContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

JavaScriptParser::PropertyNameContext* JavaScriptParser::PropertyAssignmentContext::propertyName() {
  return getRuleContext<JavaScriptParser::PropertyNameContext>(0);
}

tree::TerminalNode* JavaScriptParser::PropertyAssignmentContext::Colon() {
  return getToken(JavaScriptParser::Colon, 0);
}

JavaScriptParser::ExpressionContext* JavaScriptParser::PropertyAssignmentContext::expression() {
  return getRuleContext<JavaScriptParser::ExpressionContext>(0);
}


size_t JavaScriptParser::PropertyAssignmentContext::getRuleIndex() const {
  return JavaScriptParser::RulePropertyAssignment;
}

void JavaScriptParser::PropertyAssignmentContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPropertyAssignment(this);
}

void JavaScriptParser::PropertyAssignmentContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPropertyAssignment(this);
}

JavaScriptParser::PropertyAssignmentContext* JavaScriptParser::propertyAssignment() {
  PropertyAssignmentContext *_localctx = _tracker.createInstance<PropertyAssignmentContext>(_ctx, getState());
  enterRule(_localctx, 66, JavaScriptParser::RulePropertyAssignment);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(342);
    propertyName();
    setState(343);
    match(JavaScriptParser::Colon);
    setState(344);
    expression();
   
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

tree::TerminalNode* JavaScriptParser::PropertyNameContext::Identifier() {
  return getToken(JavaScriptParser::Identifier, 0);
}

tree::TerminalNode* JavaScriptParser::PropertyNameContext::StringLiteral() {
  return getToken(JavaScriptParser::StringLiteral, 0);
}

tree::TerminalNode* JavaScriptParser::PropertyNameContext::NumericLiteral() {
  return getToken(JavaScriptParser::NumericLiteral, 0);
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
  enterRule(_localctx, 68, JavaScriptParser::RulePropertyName);
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
    setState(346);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 14680064) != 0))) {
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

tree::TerminalNode* JavaScriptParser::LiteralContext::NullLiteral() {
  return getToken(JavaScriptParser::NullLiteral, 0);
}

tree::TerminalNode* JavaScriptParser::LiteralContext::BooleanLiteral() {
  return getToken(JavaScriptParser::BooleanLiteral, 0);
}

tree::TerminalNode* JavaScriptParser::LiteralContext::StringLiteral() {
  return getToken(JavaScriptParser::StringLiteral, 0);
}

tree::TerminalNode* JavaScriptParser::LiteralContext::NumericLiteral() {
  return getToken(JavaScriptParser::NumericLiteral, 0);
}


size_t JavaScriptParser::LiteralContext::getRuleIndex() const {
  return JavaScriptParser::RuleLiteral;
}

void JavaScriptParser::LiteralContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLiteral(this);
}

void JavaScriptParser::LiteralContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLiteral(this);
}

JavaScriptParser::LiteralContext* JavaScriptParser::literal() {
  LiteralContext *_localctx = _tracker.createInstance<LiteralContext>(_ctx, getState());
  enterRule(_localctx, 70, JavaScriptParser::RuleLiteral);
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
    setState(348);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 7864320) != 0))) {
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

void JavaScriptParser::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  javascriptParserInitialize();
#else
  ::antlr4::internal::call_once(javascriptParserOnceFlag, javascriptParserInitialize);
#endif
}
