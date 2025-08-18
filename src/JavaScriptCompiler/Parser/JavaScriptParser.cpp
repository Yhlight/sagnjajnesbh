
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
      "program", "sourceElements", "sourceElement", "statement", "declaration", 
      "block", "statementList", "variableStatement", "variableDeclaration", 
      "variableDeclarationList", "variableDeclarator", "emptyStatement", 
      "expressionStatement", "ifStatement", "iterationStatement", "returnStatement", 
      "switchStatement", "caseBlock", "caseClauses", "caseClause", "defaultClause", 
      "throwStatement", "tryStatement", "catchProduction", "finallyProduction", 
      "functionDeclaration", "classDeclaration", "classElement", "methodDefinition", 
      "formalParameterList", "formalParameterArg", "functionBody", "arrayLiteral", 
      "elementList", "arrayElement", "objectLiteral", "propertyAssignment", 
      "propertyName", "arguments", "argument", "expressionSequence", "singleExpression", 
      "anonymousFunction", "arrowFunctionParameters", "arrowFunctionBody", 
      "assignmentOperator", "literal", "numericLiteral", "getter", "setter", 
      "identifierName", "reservedWord", "keyword"
    },
    std::vector<std::string>{
      "", "'break'", "'case'", "'catch'", "'class'", "'const'", "'continue'", 
      "'debugger'", "'default'", "'delete'", "'do'", "'else'", "'enum'", 
      "'export'", "'extends'", "'finally'", "'for'", "'function'", "'if'", 
      "'import'", "'in'", "'instanceof'", "'new'", "'return'", "'super'", 
      "'switch'", "'this'", "'throw'", "'try'", "'typeof'", "'var'", "'void'", 
      "'while'", "'with'", "'yield'", "'async'", "'await'", "'let'", "'of'", 
      "'static'", "'get'", "'set'", "'null'", "", "", "", "", "", "", "", 
      "", "", "'='", "'\\u003F'", "':'", "'=>'", "'...'", "'.'", "'++'", 
      "'--'", "'+'", "'-'", "'~'", "'!'", "'*'", "'/'", "'%'", "'**'", "'\\u003F\\u003F'", 
      "'<<'", "'>>'", "'>>>'", "'<'", "'>'", "'<='", "'>='", "'=='", "'!='", 
      "'==='", "'!=='", "'&'", "'^'", "'|'", "'&&'", "'||'", "'*='", "'/='", 
      "'%='", "'+='", "'-='", "'<<='", "'>>='", "'>>>='", "'&='", "'^='", 
      "'|='", "'**='", "';'", "','", "'('", "')'", "'{'", "'}'", "'['", 
      "']'"
    },
    std::vector<std::string>{
      "", "BREAK", "CASE", "CATCH", "CLASS", "CONST", "CONTINUE", "DEBUGGER", 
      "DEFAULT", "DELETE", "DO", "ELSE", "ENUM", "EXPORT", "EXTENDS", "FINALLY", 
      "FOR", "FUNCTION", "IF", "IMPORT", "IN", "INSTANCEOF", "NEW", "RETURN", 
      "SUPER", "SWITCH", "THIS", "THROW", "TRY", "TYPEOF", "VAR", "VOID", 
      "WHILE", "WITH", "YIELD", "ASYNC", "AWAIT", "LET", "OF", "STATIC", 
      "GET", "SET", "NullLiteral", "BooleanLiteral", "DecimalLiteral", "HexIntegerLiteral", 
      "OctalIntegerLiteral", "BinaryIntegerLiteral", "StringLiteral", "TemplateStringLiteral", 
      "RegularExpressionLiteral", "Identifier", "ASSIGN", "QUESTION", "COLON", 
      "ARROW", "ELLIPSIS", "DOT", "PLUSPLUS", "MINUSMINUS", "PLUS", "MINUS", 
      "BITNOT", "NOT", "STAR", "SLASH", "MODULUS", "POWER", "NULLCOALESCE", 
      "LSHIFT", "RSHIFT", "URSHIFT", "LT", "GT", "LE", "GE", "EQ", "NE", 
      "SEQ", "SNE", "BITAND", "BITXOR", "BITOR", "AND", "OR", "STARASSIGN", 
      "SLASHASSIGN", "MODULUSASSIGN", "PLUSASSIGN", "MINUSASSIGN", "LSHIFTASSIGN", 
      "RSHIFTASSIGN", "URSHIFTASSIGN", "BITANDASSIGN", "BITXORASSIGN", "BITORASSIGN", 
      "POWERASSIGN", "SEMICOLON", "COMMA", "LPAREN", "RPAREN", "LBRACE", 
      "RBRACE", "LBRACKET", "RBRACKET", "WS", "MultiLineComment", "SingleLineComment"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,107,636,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,27,7,27,2,28,7,
  	28,2,29,7,29,2,30,7,30,2,31,7,31,2,32,7,32,2,33,7,33,2,34,7,34,2,35,7,
  	35,2,36,7,36,2,37,7,37,2,38,7,38,2,39,7,39,2,40,7,40,2,41,7,41,2,42,7,
  	42,2,43,7,43,2,44,7,44,2,45,7,45,2,46,7,46,2,47,7,47,2,48,7,48,2,49,7,
  	49,2,50,7,50,2,51,7,51,2,52,7,52,1,0,3,0,108,8,0,1,0,1,0,1,1,4,1,113,
  	8,1,11,1,12,1,114,1,2,1,2,3,2,119,8,2,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,
  	1,3,1,3,3,3,131,8,3,1,4,1,4,1,4,3,4,136,8,4,1,5,1,5,3,5,140,8,5,1,5,1,
  	5,1,6,4,6,145,8,6,11,6,12,6,146,1,7,1,7,3,7,151,8,7,1,8,1,8,1,8,1,9,1,
  	9,1,9,5,9,159,8,9,10,9,12,9,162,9,9,1,10,1,10,1,10,3,10,167,8,10,1,11,
  	1,11,1,12,1,12,3,12,173,8,12,1,13,1,13,1,13,1,13,1,13,1,13,1,13,3,13,
  	182,8,13,1,14,1,14,1,14,1,14,1,14,1,14,1,14,1,14,1,14,1,14,3,14,194,8,
  	14,1,14,1,14,3,14,198,8,14,1,14,1,14,3,14,202,8,14,1,14,1,14,1,14,1,14,
  	1,14,1,14,3,14,210,8,14,1,14,1,14,1,14,1,14,1,14,3,14,217,8,14,1,15,1,
  	15,3,15,221,8,15,1,15,3,15,224,8,15,1,16,1,16,1,16,1,16,1,16,1,16,1,17,
  	1,17,3,17,234,8,17,1,17,1,17,3,17,238,8,17,3,17,240,8,17,1,17,1,17,1,
  	18,4,18,245,8,18,11,18,12,18,246,1,19,1,19,1,19,1,19,3,19,253,8,19,1,
  	20,1,20,1,20,3,20,258,8,20,1,21,1,21,1,21,3,21,263,8,21,1,22,1,22,1,22,
  	1,22,3,22,269,8,22,1,22,3,22,272,8,22,1,23,1,23,1,23,3,23,277,8,23,1,
  	23,1,23,1,23,1,24,1,24,1,24,1,25,3,25,286,8,25,1,25,1,25,3,25,290,8,25,
  	1,25,1,25,1,25,3,25,295,8,25,1,25,1,25,1,25,1,26,1,26,1,26,1,26,3,26,
  	304,8,26,1,26,1,26,5,26,308,8,26,10,26,12,26,311,9,26,1,26,1,26,1,27,
  	3,27,316,8,27,1,27,1,27,3,27,320,8,27,1,28,3,28,323,8,28,1,28,1,28,1,
  	28,3,28,328,8,28,1,28,1,28,1,28,1,28,1,28,1,28,1,28,1,28,1,28,1,28,1,
  	28,1,28,1,28,1,28,3,28,344,8,28,1,29,1,29,1,29,5,29,349,8,29,10,29,12,
  	29,352,9,29,1,30,1,30,1,30,3,30,357,8,30,1,31,1,31,3,31,361,8,31,1,31,
  	1,31,1,32,1,32,3,32,367,8,32,1,32,1,32,1,33,1,33,1,33,5,33,374,8,33,10,
  	33,12,33,377,9,33,1,34,1,34,1,35,1,35,1,35,1,35,5,35,385,8,35,10,35,12,
  	35,388,9,35,3,35,390,8,35,1,35,1,35,1,36,1,36,1,36,1,36,1,36,1,36,1,36,
  	1,36,1,36,1,36,1,36,3,36,405,8,36,1,36,1,36,1,36,3,36,410,8,36,1,36,1,
  	36,1,36,1,36,1,36,1,36,1,36,1,36,1,36,1,36,1,36,1,36,1,36,1,36,3,36,426,
  	8,36,1,37,1,37,1,37,1,37,1,37,1,37,1,37,3,37,435,8,37,1,38,1,38,1,38,
  	1,38,5,38,441,8,38,10,38,12,38,444,9,38,3,38,446,8,38,1,38,1,38,1,39,
  	1,39,1,40,1,40,1,40,5,40,455,8,40,10,40,12,40,458,9,40,1,41,1,41,1,41,
  	1,41,1,41,3,41,465,8,41,1,41,1,41,1,41,1,41,1,41,1,41,1,41,1,41,1,41,
  	1,41,1,41,1,41,1,41,1,41,1,41,1,41,1,41,1,41,1,41,1,41,1,41,1,41,1,41,
  	1,41,1,41,1,41,1,41,1,41,1,41,1,41,3,41,497,8,41,1,41,1,41,1,41,1,41,
  	1,41,1,41,1,41,1,41,1,41,1,41,1,41,1,41,1,41,1,41,1,41,1,41,1,41,1,41,
  	1,41,1,41,1,41,1,41,1,41,1,41,1,41,1,41,1,41,1,41,1,41,1,41,1,41,1,41,
  	1,41,1,41,1,41,1,41,1,41,1,41,1,41,1,41,1,41,1,41,1,41,1,41,1,41,1,41,
  	1,41,1,41,1,41,1,41,1,41,1,41,1,41,1,41,1,41,1,41,1,41,1,41,1,41,1,41,
  	1,41,1,41,1,41,1,41,1,41,1,41,1,41,1,41,1,41,5,41,568,8,41,10,41,12,41,
  	571,9,41,1,42,3,42,574,8,42,1,42,1,42,3,42,578,8,42,1,42,1,42,3,42,582,
  	8,42,1,42,1,42,1,42,3,42,587,8,42,1,42,1,42,1,42,1,42,3,42,593,8,42,1,
  	43,1,43,1,43,3,43,598,8,43,1,43,3,43,601,8,43,1,44,1,44,3,44,605,8,44,
  	1,45,1,45,1,46,1,46,1,46,1,46,1,46,1,46,3,46,615,8,46,1,47,1,47,1,48,
  	1,48,1,48,1,49,1,49,1,49,1,50,1,50,3,50,627,8,50,1,51,1,51,1,51,3,51,
  	632,8,51,1,52,1,52,1,52,0,1,82,53,0,2,4,6,8,10,12,14,16,18,20,22,24,26,
  	28,30,32,34,36,38,40,42,44,46,48,50,52,54,56,58,60,62,64,66,68,70,72,
  	74,76,78,80,82,84,86,88,90,92,94,96,98,100,102,104,0,10,3,0,5,5,30,30,
  	37,37,2,0,20,20,38,38,1,0,64,66,1,0,60,61,1,0,69,71,1,0,72,75,1,0,76,
  	79,1,0,85,96,1,0,44,47,1,0,1,39,709,0,107,1,0,0,0,2,112,1,0,0,0,4,118,
  	1,0,0,0,6,130,1,0,0,0,8,135,1,0,0,0,10,137,1,0,0,0,12,144,1,0,0,0,14,
  	148,1,0,0,0,16,152,1,0,0,0,18,155,1,0,0,0,20,163,1,0,0,0,22,168,1,0,0,
  	0,24,170,1,0,0,0,26,174,1,0,0,0,28,216,1,0,0,0,30,218,1,0,0,0,32,225,
  	1,0,0,0,34,231,1,0,0,0,36,244,1,0,0,0,38,248,1,0,0,0,40,254,1,0,0,0,42,
  	259,1,0,0,0,44,264,1,0,0,0,46,273,1,0,0,0,48,281,1,0,0,0,50,285,1,0,0,
  	0,52,299,1,0,0,0,54,319,1,0,0,0,56,343,1,0,0,0,58,345,1,0,0,0,60,353,
  	1,0,0,0,62,358,1,0,0,0,64,364,1,0,0,0,66,370,1,0,0,0,68,378,1,0,0,0,70,
  	380,1,0,0,0,72,425,1,0,0,0,74,434,1,0,0,0,76,436,1,0,0,0,78,449,1,0,0,
  	0,80,451,1,0,0,0,82,496,1,0,0,0,84,592,1,0,0,0,86,600,1,0,0,0,88,604,
  	1,0,0,0,90,606,1,0,0,0,92,614,1,0,0,0,94,616,1,0,0,0,96,618,1,0,0,0,98,
  	621,1,0,0,0,100,626,1,0,0,0,102,631,1,0,0,0,104,633,1,0,0,0,106,108,3,
  	2,1,0,107,106,1,0,0,0,107,108,1,0,0,0,108,109,1,0,0,0,109,110,5,0,0,1,
  	110,1,1,0,0,0,111,113,3,4,2,0,112,111,1,0,0,0,113,114,1,0,0,0,114,112,
  	1,0,0,0,114,115,1,0,0,0,115,3,1,0,0,0,116,119,3,6,3,0,117,119,3,8,4,0,
  	118,116,1,0,0,0,118,117,1,0,0,0,119,5,1,0,0,0,120,131,3,10,5,0,121,131,
  	3,14,7,0,122,131,3,22,11,0,123,131,3,24,12,0,124,131,3,26,13,0,125,131,
  	3,28,14,0,126,131,3,30,15,0,127,131,3,32,16,0,128,131,3,42,21,0,129,131,
  	3,44,22,0,130,120,1,0,0,0,130,121,1,0,0,0,130,122,1,0,0,0,130,123,1,0,
  	0,0,130,124,1,0,0,0,130,125,1,0,0,0,130,126,1,0,0,0,130,127,1,0,0,0,130,
  	128,1,0,0,0,130,129,1,0,0,0,131,7,1,0,0,0,132,136,3,16,8,0,133,136,3,
  	50,25,0,134,136,3,52,26,0,135,132,1,0,0,0,135,133,1,0,0,0,135,134,1,0,
  	0,0,136,9,1,0,0,0,137,139,5,101,0,0,138,140,3,12,6,0,139,138,1,0,0,0,
  	139,140,1,0,0,0,140,141,1,0,0,0,141,142,5,102,0,0,142,11,1,0,0,0,143,
  	145,3,6,3,0,144,143,1,0,0,0,145,146,1,0,0,0,146,144,1,0,0,0,146,147,1,
  	0,0,0,147,13,1,0,0,0,148,150,3,16,8,0,149,151,5,97,0,0,150,149,1,0,0,
  	0,150,151,1,0,0,0,151,15,1,0,0,0,152,153,7,0,0,0,153,154,3,18,9,0,154,
  	17,1,0,0,0,155,160,3,20,10,0,156,157,5,98,0,0,157,159,3,20,10,0,158,156,
  	1,0,0,0,159,162,1,0,0,0,160,158,1,0,0,0,160,161,1,0,0,0,161,19,1,0,0,
  	0,162,160,1,0,0,0,163,166,5,51,0,0,164,165,5,52,0,0,165,167,3,82,41,0,
  	166,164,1,0,0,0,166,167,1,0,0,0,167,21,1,0,0,0,168,169,5,97,0,0,169,23,
  	1,0,0,0,170,172,3,80,40,0,171,173,5,97,0,0,172,171,1,0,0,0,172,173,1,
  	0,0,0,173,25,1,0,0,0,174,175,5,18,0,0,175,176,5,99,0,0,176,177,3,80,40,
  	0,177,178,5,100,0,0,178,181,3,6,3,0,179,180,5,11,0,0,180,182,3,6,3,0,
  	181,179,1,0,0,0,181,182,1,0,0,0,182,27,1,0,0,0,183,184,5,32,0,0,184,185,
  	5,99,0,0,185,186,3,80,40,0,186,187,5,100,0,0,187,188,3,6,3,0,188,217,
  	1,0,0,0,189,190,5,16,0,0,190,193,5,99,0,0,191,194,3,80,40,0,192,194,3,
  	16,8,0,193,191,1,0,0,0,193,192,1,0,0,0,193,194,1,0,0,0,194,195,1,0,0,
  	0,195,197,5,97,0,0,196,198,3,80,40,0,197,196,1,0,0,0,197,198,1,0,0,0,
  	198,199,1,0,0,0,199,201,5,97,0,0,200,202,3,80,40,0,201,200,1,0,0,0,201,
  	202,1,0,0,0,202,203,1,0,0,0,203,204,5,100,0,0,204,217,3,6,3,0,205,206,
  	5,16,0,0,206,209,5,99,0,0,207,210,3,82,41,0,208,210,3,16,8,0,209,207,
  	1,0,0,0,209,208,1,0,0,0,210,211,1,0,0,0,211,212,7,1,0,0,212,213,3,80,
  	40,0,213,214,5,100,0,0,214,215,3,6,3,0,215,217,1,0,0,0,216,183,1,0,0,
  	0,216,189,1,0,0,0,216,205,1,0,0,0,217,29,1,0,0,0,218,220,5,23,0,0,219,
  	221,3,80,40,0,220,219,1,0,0,0,220,221,1,0,0,0,221,223,1,0,0,0,222,224,
  	5,97,0,0,223,222,1,0,0,0,223,224,1,0,0,0,224,31,1,0,0,0,225,226,5,25,
  	0,0,226,227,5,99,0,0,227,228,3,80,40,0,228,229,5,100,0,0,229,230,3,34,
  	17,0,230,33,1,0,0,0,231,233,5,101,0,0,232,234,3,36,18,0,233,232,1,0,0,
  	0,233,234,1,0,0,0,234,239,1,0,0,0,235,237,3,40,20,0,236,238,3,36,18,0,
  	237,236,1,0,0,0,237,238,1,0,0,0,238,240,1,0,0,0,239,235,1,0,0,0,239,240,
  	1,0,0,0,240,241,1,0,0,0,241,242,5,102,0,0,242,35,1,0,0,0,243,245,3,38,
  	19,0,244,243,1,0,0,0,245,246,1,0,0,0,246,244,1,0,0,0,246,247,1,0,0,0,
  	247,37,1,0,0,0,248,249,5,2,0,0,249,250,3,80,40,0,250,252,5,54,0,0,251,
  	253,3,12,6,0,252,251,1,0,0,0,252,253,1,0,0,0,253,39,1,0,0,0,254,255,5,
  	8,0,0,255,257,5,54,0,0,256,258,3,12,6,0,257,256,1,0,0,0,257,258,1,0,0,
  	0,258,41,1,0,0,0,259,260,5,27,0,0,260,262,3,80,40,0,261,263,5,97,0,0,
  	262,261,1,0,0,0,262,263,1,0,0,0,263,43,1,0,0,0,264,265,5,28,0,0,265,271,
  	3,10,5,0,266,268,3,46,23,0,267,269,3,48,24,0,268,267,1,0,0,0,268,269,
  	1,0,0,0,269,272,1,0,0,0,270,272,3,48,24,0,271,266,1,0,0,0,271,270,1,0,
  	0,0,272,45,1,0,0,0,273,274,5,3,0,0,274,276,5,99,0,0,275,277,5,51,0,0,
  	276,275,1,0,0,0,276,277,1,0,0,0,277,278,1,0,0,0,278,279,5,100,0,0,279,
  	280,3,10,5,0,280,47,1,0,0,0,281,282,5,15,0,0,282,283,3,10,5,0,283,49,
  	1,0,0,0,284,286,5,35,0,0,285,284,1,0,0,0,285,286,1,0,0,0,286,287,1,0,
  	0,0,287,289,5,17,0,0,288,290,5,64,0,0,289,288,1,0,0,0,289,290,1,0,0,0,
  	290,291,1,0,0,0,291,292,5,51,0,0,292,294,5,99,0,0,293,295,3,58,29,0,294,
  	293,1,0,0,0,294,295,1,0,0,0,295,296,1,0,0,0,296,297,5,100,0,0,297,298,
  	3,62,31,0,298,51,1,0,0,0,299,300,5,4,0,0,300,303,5,51,0,0,301,302,5,14,
  	0,0,302,304,3,82,41,0,303,301,1,0,0,0,303,304,1,0,0,0,304,305,1,0,0,0,
  	305,309,5,101,0,0,306,308,3,54,27,0,307,306,1,0,0,0,308,311,1,0,0,0,309,
  	307,1,0,0,0,309,310,1,0,0,0,310,312,1,0,0,0,311,309,1,0,0,0,312,313,5,
  	102,0,0,313,53,1,0,0,0,314,316,5,39,0,0,315,314,1,0,0,0,315,316,1,0,0,
  	0,316,317,1,0,0,0,317,320,3,56,28,0,318,320,3,22,11,0,319,315,1,0,0,0,
  	319,318,1,0,0,0,320,55,1,0,0,0,321,323,5,64,0,0,322,321,1,0,0,0,322,323,
  	1,0,0,0,323,324,1,0,0,0,324,325,3,74,37,0,325,327,5,99,0,0,326,328,3,
  	58,29,0,327,326,1,0,0,0,327,328,1,0,0,0,328,329,1,0,0,0,329,330,5,100,
  	0,0,330,331,3,62,31,0,331,344,1,0,0,0,332,333,3,96,48,0,333,334,5,99,
  	0,0,334,335,5,100,0,0,335,336,3,62,31,0,336,344,1,0,0,0,337,338,3,98,
  	49,0,338,339,5,99,0,0,339,340,3,58,29,0,340,341,5,100,0,0,341,342,3,62,
  	31,0,342,344,1,0,0,0,343,322,1,0,0,0,343,332,1,0,0,0,343,337,1,0,0,0,
  	344,57,1,0,0,0,345,350,3,60,30,0,346,347,5,98,0,0,347,349,3,60,30,0,348,
  	346,1,0,0,0,349,352,1,0,0,0,350,348,1,0,0,0,350,351,1,0,0,0,351,59,1,
  	0,0,0,352,350,1,0,0,0,353,356,5,51,0,0,354,355,5,52,0,0,355,357,3,82,
  	41,0,356,354,1,0,0,0,356,357,1,0,0,0,357,61,1,0,0,0,358,360,5,101,0,0,
  	359,361,3,2,1,0,360,359,1,0,0,0,360,361,1,0,0,0,361,362,1,0,0,0,362,363,
  	5,102,0,0,363,63,1,0,0,0,364,366,5,103,0,0,365,367,3,66,33,0,366,365,
  	1,0,0,0,366,367,1,0,0,0,367,368,1,0,0,0,368,369,5,104,0,0,369,65,1,0,
  	0,0,370,375,3,68,34,0,371,372,5,98,0,0,372,374,3,68,34,0,373,371,1,0,
  	0,0,374,377,1,0,0,0,375,373,1,0,0,0,375,376,1,0,0,0,376,67,1,0,0,0,377,
  	375,1,0,0,0,378,379,3,82,41,0,379,69,1,0,0,0,380,389,5,101,0,0,381,386,
  	3,72,36,0,382,383,5,98,0,0,383,385,3,72,36,0,384,382,1,0,0,0,385,388,
  	1,0,0,0,386,384,1,0,0,0,386,387,1,0,0,0,387,390,1,0,0,0,388,386,1,0,0,
  	0,389,381,1,0,0,0,389,390,1,0,0,0,390,391,1,0,0,0,391,392,5,102,0,0,392,
  	71,1,0,0,0,393,394,3,74,37,0,394,395,5,54,0,0,395,396,3,82,41,0,396,426,
  	1,0,0,0,397,398,5,103,0,0,398,399,3,82,41,0,399,400,5,104,0,0,400,401,
  	5,54,0,0,401,402,3,82,41,0,402,426,1,0,0,0,403,405,5,64,0,0,404,403,1,
  	0,0,0,404,405,1,0,0,0,405,406,1,0,0,0,406,407,3,74,37,0,407,409,5,99,
  	0,0,408,410,3,58,29,0,409,408,1,0,0,0,409,410,1,0,0,0,410,411,1,0,0,0,
  	411,412,5,100,0,0,412,413,3,62,31,0,413,426,1,0,0,0,414,415,3,96,48,0,
  	415,416,5,99,0,0,416,417,5,100,0,0,417,418,3,62,31,0,418,426,1,0,0,0,
  	419,420,3,98,49,0,420,421,5,99,0,0,421,422,3,60,30,0,422,423,5,100,0,
  	0,423,424,3,62,31,0,424,426,1,0,0,0,425,393,1,0,0,0,425,397,1,0,0,0,425,
  	404,1,0,0,0,425,414,1,0,0,0,425,419,1,0,0,0,426,73,1,0,0,0,427,435,3,
  	100,50,0,428,435,5,48,0,0,429,435,3,94,47,0,430,431,5,103,0,0,431,432,
  	3,82,41,0,432,433,5,104,0,0,433,435,1,0,0,0,434,427,1,0,0,0,434,428,1,
  	0,0,0,434,429,1,0,0,0,434,430,1,0,0,0,435,75,1,0,0,0,436,445,5,99,0,0,
  	437,442,3,78,39,0,438,439,5,98,0,0,439,441,3,78,39,0,440,438,1,0,0,0,
  	441,444,1,0,0,0,442,440,1,0,0,0,442,443,1,0,0,0,443,446,1,0,0,0,444,442,
  	1,0,0,0,445,437,1,0,0,0,445,446,1,0,0,0,446,447,1,0,0,0,447,448,5,100,
  	0,0,448,77,1,0,0,0,449,450,3,82,41,0,450,79,1,0,0,0,451,456,3,82,41,0,
  	452,453,5,98,0,0,453,455,3,82,41,0,454,452,1,0,0,0,455,458,1,0,0,0,456,
  	454,1,0,0,0,456,457,1,0,0,0,457,81,1,0,0,0,458,456,1,0,0,0,459,460,6,
  	41,-1,0,460,497,3,84,42,0,461,462,5,22,0,0,462,464,3,82,41,0,463,465,
  	3,76,38,0,464,463,1,0,0,0,464,465,1,0,0,0,465,497,1,0,0,0,466,467,5,9,
  	0,0,467,497,3,82,41,34,468,469,5,31,0,0,469,497,3,82,41,33,470,471,5,
  	29,0,0,471,497,3,82,41,32,472,473,5,58,0,0,473,497,3,82,41,31,474,475,
  	5,59,0,0,475,497,3,82,41,30,476,477,5,60,0,0,477,497,3,82,41,29,478,479,
  	5,61,0,0,479,497,3,82,41,28,480,481,5,62,0,0,481,497,3,82,41,27,482,483,
  	5,63,0,0,483,497,3,82,41,26,484,485,5,36,0,0,485,497,3,82,41,25,486,497,
  	5,26,0,0,487,497,5,51,0,0,488,497,5,24,0,0,489,497,3,92,46,0,490,497,
  	3,64,32,0,491,497,3,70,35,0,492,493,5,99,0,0,493,494,3,80,40,0,494,495,
  	5,100,0,0,495,497,1,0,0,0,496,459,1,0,0,0,496,461,1,0,0,0,496,466,1,0,
  	0,0,496,468,1,0,0,0,496,470,1,0,0,0,496,472,1,0,0,0,496,474,1,0,0,0,496,
  	476,1,0,0,0,496,478,1,0,0,0,496,480,1,0,0,0,496,482,1,0,0,0,496,484,1,
  	0,0,0,496,486,1,0,0,0,496,487,1,0,0,0,496,488,1,0,0,0,496,489,1,0,0,0,
  	496,490,1,0,0,0,496,491,1,0,0,0,496,492,1,0,0,0,497,569,1,0,0,0,498,499,
  	10,24,0,0,499,500,5,67,0,0,500,568,3,82,41,25,501,502,10,23,0,0,502,503,
  	7,2,0,0,503,568,3,82,41,24,504,505,10,22,0,0,505,506,7,3,0,0,506,568,
  	3,82,41,23,507,508,10,21,0,0,508,509,7,4,0,0,509,568,3,82,41,22,510,511,
  	10,20,0,0,511,512,7,5,0,0,512,568,3,82,41,21,513,514,10,19,0,0,514,515,
  	5,21,0,0,515,568,3,82,41,20,516,517,10,18,0,0,517,518,5,20,0,0,518,568,
  	3,82,41,19,519,520,10,17,0,0,520,521,7,6,0,0,521,568,3,82,41,18,522,523,
  	10,16,0,0,523,524,5,80,0,0,524,568,3,82,41,17,525,526,10,15,0,0,526,527,
  	5,81,0,0,527,568,3,82,41,16,528,529,10,14,0,0,529,530,5,82,0,0,530,568,
  	3,82,41,15,531,532,10,13,0,0,532,533,5,83,0,0,533,568,3,82,41,14,534,
  	535,10,12,0,0,535,536,5,84,0,0,536,568,3,82,41,13,537,538,10,11,0,0,538,
  	539,5,68,0,0,539,568,3,82,41,12,540,541,10,10,0,0,541,542,5,53,0,0,542,
  	543,3,82,41,0,543,544,5,54,0,0,544,545,3,82,41,11,545,568,1,0,0,0,546,
  	547,10,9,0,0,547,548,5,52,0,0,548,568,3,82,41,10,549,550,10,8,0,0,550,
  	551,3,90,45,0,551,552,3,82,41,9,552,568,1,0,0,0,553,554,10,40,0,0,554,
  	555,5,103,0,0,555,556,3,80,40,0,556,557,5,104,0,0,557,568,1,0,0,0,558,
  	559,10,39,0,0,559,560,5,57,0,0,560,568,3,100,50,0,561,562,10,38,0,0,562,
  	568,3,76,38,0,563,564,10,36,0,0,564,568,5,58,0,0,565,566,10,35,0,0,566,
  	568,5,59,0,0,567,498,1,0,0,0,567,501,1,0,0,0,567,504,1,0,0,0,567,507,
  	1,0,0,0,567,510,1,0,0,0,567,513,1,0,0,0,567,516,1,0,0,0,567,519,1,0,0,
  	0,567,522,1,0,0,0,567,525,1,0,0,0,567,528,1,0,0,0,567,531,1,0,0,0,567,
  	534,1,0,0,0,567,537,1,0,0,0,567,540,1,0,0,0,567,546,1,0,0,0,567,549,1,
  	0,0,0,567,553,1,0,0,0,567,558,1,0,0,0,567,561,1,0,0,0,567,563,1,0,0,0,
  	567,565,1,0,0,0,568,571,1,0,0,0,569,567,1,0,0,0,569,570,1,0,0,0,570,83,
  	1,0,0,0,571,569,1,0,0,0,572,574,5,35,0,0,573,572,1,0,0,0,573,574,1,0,
  	0,0,574,575,1,0,0,0,575,577,5,17,0,0,576,578,5,64,0,0,577,576,1,0,0,0,
  	577,578,1,0,0,0,578,579,1,0,0,0,579,581,5,99,0,0,580,582,3,58,29,0,581,
  	580,1,0,0,0,581,582,1,0,0,0,582,583,1,0,0,0,583,584,5,100,0,0,584,593,
  	3,62,31,0,585,587,5,35,0,0,586,585,1,0,0,0,586,587,1,0,0,0,587,588,1,
  	0,0,0,588,589,3,86,43,0,589,590,5,55,0,0,590,591,3,88,44,0,591,593,1,
  	0,0,0,592,573,1,0,0,0,592,586,1,0,0,0,593,85,1,0,0,0,594,601,5,51,0,0,
  	595,597,5,99,0,0,596,598,3,58,29,0,597,596,1,0,0,0,597,598,1,0,0,0,598,
  	599,1,0,0,0,599,601,5,100,0,0,600,594,1,0,0,0,600,595,1,0,0,0,601,87,
  	1,0,0,0,602,605,3,82,41,0,603,605,3,62,31,0,604,602,1,0,0,0,604,603,1,
  	0,0,0,605,89,1,0,0,0,606,607,7,7,0,0,607,91,1,0,0,0,608,615,5,42,0,0,
  	609,615,5,43,0,0,610,615,5,48,0,0,611,615,5,49,0,0,612,615,5,50,0,0,613,
  	615,3,94,47,0,614,608,1,0,0,0,614,609,1,0,0,0,614,610,1,0,0,0,614,611,
  	1,0,0,0,614,612,1,0,0,0,614,613,1,0,0,0,615,93,1,0,0,0,616,617,7,8,0,
  	0,617,95,1,0,0,0,618,619,5,40,0,0,619,620,3,74,37,0,620,97,1,0,0,0,621,
  	622,5,41,0,0,622,623,3,74,37,0,623,99,1,0,0,0,624,627,5,51,0,0,625,627,
  	3,102,51,0,626,624,1,0,0,0,626,625,1,0,0,0,627,101,1,0,0,0,628,632,3,
  	104,52,0,629,632,5,42,0,0,630,632,5,43,0,0,631,628,1,0,0,0,631,629,1,
  	0,0,0,631,630,1,0,0,0,632,103,1,0,0,0,633,634,7,9,0,0,634,105,1,0,0,0,
  	68,107,114,118,130,135,139,146,150,160,166,172,181,193,197,201,209,216,
  	220,223,233,237,239,246,252,257,262,268,271,276,285,289,294,303,309,315,
  	319,322,327,343,350,356,360,366,375,386,389,404,409,425,434,442,445,456,
  	464,496,567,569,573,577,581,586,592,597,600,604,614,626,631
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
    setState(107);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & -283730925466484176) != 0) || ((((_la - 97) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 97)) & 85) != 0)) {
      setState(106);
      sourceElements();
    }
    setState(109);
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
    setState(112); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(111);
      sourceElement();
      setState(114); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & -283730925466484176) != 0) || ((((_la - 97) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 97)) & 85) != 0));
   
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

JavaScriptParser::DeclarationContext* JavaScriptParser::SourceElementContext::declaration() {
  return getRuleContext<JavaScriptParser::DeclarationContext>(0);
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
    setState(118);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 2, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(116);
      statement();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(117);
      declaration();
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

JavaScriptParser::ReturnStatementContext* JavaScriptParser::StatementContext::returnStatement() {
  return getRuleContext<JavaScriptParser::ReturnStatementContext>(0);
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
    setState(130);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 3, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(120);
      block();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(121);
      variableStatement();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(122);
      emptyStatement();
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(123);
      expressionStatement();
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(124);
      ifStatement();
      break;
    }

    case 6: {
      enterOuterAlt(_localctx, 6);
      setState(125);
      iterationStatement();
      break;
    }

    case 7: {
      enterOuterAlt(_localctx, 7);
      setState(126);
      returnStatement();
      break;
    }

    case 8: {
      enterOuterAlt(_localctx, 8);
      setState(127);
      switchStatement();
      break;
    }

    case 9: {
      enterOuterAlt(_localctx, 9);
      setState(128);
      throwStatement();
      break;
    }

    case 10: {
      enterOuterAlt(_localctx, 10);
      setState(129);
      tryStatement();
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

//----------------- DeclarationContext ------------------------------------------------------------------

JavaScriptParser::DeclarationContext::DeclarationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

JavaScriptParser::VariableDeclarationContext* JavaScriptParser::DeclarationContext::variableDeclaration() {
  return getRuleContext<JavaScriptParser::VariableDeclarationContext>(0);
}

JavaScriptParser::FunctionDeclarationContext* JavaScriptParser::DeclarationContext::functionDeclaration() {
  return getRuleContext<JavaScriptParser::FunctionDeclarationContext>(0);
}

JavaScriptParser::ClassDeclarationContext* JavaScriptParser::DeclarationContext::classDeclaration() {
  return getRuleContext<JavaScriptParser::ClassDeclarationContext>(0);
}


size_t JavaScriptParser::DeclarationContext::getRuleIndex() const {
  return JavaScriptParser::RuleDeclaration;
}

void JavaScriptParser::DeclarationContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterDeclaration(this);
}

void JavaScriptParser::DeclarationContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitDeclaration(this);
}

JavaScriptParser::DeclarationContext* JavaScriptParser::declaration() {
  DeclarationContext *_localctx = _tracker.createInstance<DeclarationContext>(_ctx, getState());
  enterRule(_localctx, 8, JavaScriptParser::RuleDeclaration);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(135);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case JavaScriptParser::CONST:
      case JavaScriptParser::VAR:
      case JavaScriptParser::LET: {
        enterOuterAlt(_localctx, 1);
        setState(132);
        variableDeclaration();
        break;
      }

      case JavaScriptParser::FUNCTION:
      case JavaScriptParser::ASYNC: {
        enterOuterAlt(_localctx, 2);
        setState(133);
        functionDeclaration();
        break;
      }

      case JavaScriptParser::CLASS: {
        enterOuterAlt(_localctx, 3);
        setState(134);
        classDeclaration();
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

JavaScriptParser::BlockContext::BlockContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::BlockContext::LBRACE() {
  return getToken(JavaScriptParser::LBRACE, 0);
}

tree::TerminalNode* JavaScriptParser::BlockContext::RBRACE() {
  return getToken(JavaScriptParser::RBRACE, 0);
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
  enterRule(_localctx, 10, JavaScriptParser::RuleBlock);
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
    match(JavaScriptParser::LBRACE);
    setState(139);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & -283730925466484192) != 0) || ((((_la - 97) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 97)) & 85) != 0)) {
      setState(138);
      statementList();
    }
    setState(141);
    match(JavaScriptParser::RBRACE);
   
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
  enterRule(_localctx, 12, JavaScriptParser::RuleStatementList);
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
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(143);
      statement();
      setState(146); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & -283730925466484192) != 0) || ((((_la - 97) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 97)) & 85) != 0));
   
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

JavaScriptParser::VariableDeclarationContext* JavaScriptParser::VariableStatementContext::variableDeclaration() {
  return getRuleContext<JavaScriptParser::VariableDeclarationContext>(0);
}

tree::TerminalNode* JavaScriptParser::VariableStatementContext::SEMICOLON() {
  return getToken(JavaScriptParser::SEMICOLON, 0);
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
  enterRule(_localctx, 14, JavaScriptParser::RuleVariableStatement);

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
    variableDeclaration();
    setState(150);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 7, _ctx)) {
    case 1: {
      setState(149);
      match(JavaScriptParser::SEMICOLON);
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

JavaScriptParser::VariableDeclarationListContext* JavaScriptParser::VariableDeclarationContext::variableDeclarationList() {
  return getRuleContext<JavaScriptParser::VariableDeclarationListContext>(0);
}

tree::TerminalNode* JavaScriptParser::VariableDeclarationContext::VAR() {
  return getToken(JavaScriptParser::VAR, 0);
}

tree::TerminalNode* JavaScriptParser::VariableDeclarationContext::LET() {
  return getToken(JavaScriptParser::LET, 0);
}

tree::TerminalNode* JavaScriptParser::VariableDeclarationContext::CONST() {
  return getToken(JavaScriptParser::CONST, 0);
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
    setState(152);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 138512695328) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(153);
    variableDeclarationList();
   
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

std::vector<JavaScriptParser::VariableDeclaratorContext *> JavaScriptParser::VariableDeclarationListContext::variableDeclarator() {
  return getRuleContexts<JavaScriptParser::VariableDeclaratorContext>();
}

JavaScriptParser::VariableDeclaratorContext* JavaScriptParser::VariableDeclarationListContext::variableDeclarator(size_t i) {
  return getRuleContext<JavaScriptParser::VariableDeclaratorContext>(i);
}

std::vector<tree::TerminalNode *> JavaScriptParser::VariableDeclarationListContext::COMMA() {
  return getTokens(JavaScriptParser::COMMA);
}

tree::TerminalNode* JavaScriptParser::VariableDeclarationListContext::COMMA(size_t i) {
  return getToken(JavaScriptParser::COMMA, i);
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
  enterRule(_localctx, 18, JavaScriptParser::RuleVariableDeclarationList);
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
    setState(155);
    variableDeclarator();
    setState(160);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == JavaScriptParser::COMMA) {
      setState(156);
      match(JavaScriptParser::COMMA);
      setState(157);
      variableDeclarator();
      setState(162);
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

//----------------- VariableDeclaratorContext ------------------------------------------------------------------

JavaScriptParser::VariableDeclaratorContext::VariableDeclaratorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::VariableDeclaratorContext::Identifier() {
  return getToken(JavaScriptParser::Identifier, 0);
}

tree::TerminalNode* JavaScriptParser::VariableDeclaratorContext::ASSIGN() {
  return getToken(JavaScriptParser::ASSIGN, 0);
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::VariableDeclaratorContext::singleExpression() {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(0);
}


size_t JavaScriptParser::VariableDeclaratorContext::getRuleIndex() const {
  return JavaScriptParser::RuleVariableDeclarator;
}

void JavaScriptParser::VariableDeclaratorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterVariableDeclarator(this);
}

void JavaScriptParser::VariableDeclaratorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitVariableDeclarator(this);
}

JavaScriptParser::VariableDeclaratorContext* JavaScriptParser::variableDeclarator() {
  VariableDeclaratorContext *_localctx = _tracker.createInstance<VariableDeclaratorContext>(_ctx, getState());
  enterRule(_localctx, 20, JavaScriptParser::RuleVariableDeclarator);
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
    match(JavaScriptParser::Identifier);
    setState(166);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == JavaScriptParser::ASSIGN) {
      setState(164);
      match(JavaScriptParser::ASSIGN);
      setState(165);
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

tree::TerminalNode* JavaScriptParser::EmptyStatementContext::SEMICOLON() {
  return getToken(JavaScriptParser::SEMICOLON, 0);
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
  enterRule(_localctx, 22, JavaScriptParser::RuleEmptyStatement);

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
    match(JavaScriptParser::SEMICOLON);
   
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

JavaScriptParser::ExpressionSequenceContext* JavaScriptParser::ExpressionStatementContext::expressionSequence() {
  return getRuleContext<JavaScriptParser::ExpressionSequenceContext>(0);
}

tree::TerminalNode* JavaScriptParser::ExpressionStatementContext::SEMICOLON() {
  return getToken(JavaScriptParser::SEMICOLON, 0);
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
  enterRule(_localctx, 24, JavaScriptParser::RuleExpressionStatement);

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
    expressionSequence();
    setState(172);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 10, _ctx)) {
    case 1: {
      setState(171);
      match(JavaScriptParser::SEMICOLON);
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

tree::TerminalNode* JavaScriptParser::IfStatementContext::LPAREN() {
  return getToken(JavaScriptParser::LPAREN, 0);
}

JavaScriptParser::ExpressionSequenceContext* JavaScriptParser::IfStatementContext::expressionSequence() {
  return getRuleContext<JavaScriptParser::ExpressionSequenceContext>(0);
}

tree::TerminalNode* JavaScriptParser::IfStatementContext::RPAREN() {
  return getToken(JavaScriptParser::RPAREN, 0);
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
  enterRule(_localctx, 26, JavaScriptParser::RuleIfStatement);

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
    match(JavaScriptParser::IF);
    setState(175);
    match(JavaScriptParser::LPAREN);
    setState(176);
    expressionSequence();
    setState(177);
    match(JavaScriptParser::RPAREN);
    setState(178);
    statement();
    setState(181);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 11, _ctx)) {
    case 1: {
      setState(179);
      match(JavaScriptParser::ELSE);
      setState(180);
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


size_t JavaScriptParser::IterationStatementContext::getRuleIndex() const {
  return JavaScriptParser::RuleIterationStatement;
}

void JavaScriptParser::IterationStatementContext::copyFrom(IterationStatementContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- WhileStatementContext ------------------------------------------------------------------

tree::TerminalNode* JavaScriptParser::WhileStatementContext::WHILE() {
  return getToken(JavaScriptParser::WHILE, 0);
}

tree::TerminalNode* JavaScriptParser::WhileStatementContext::LPAREN() {
  return getToken(JavaScriptParser::LPAREN, 0);
}

JavaScriptParser::ExpressionSequenceContext* JavaScriptParser::WhileStatementContext::expressionSequence() {
  return getRuleContext<JavaScriptParser::ExpressionSequenceContext>(0);
}

tree::TerminalNode* JavaScriptParser::WhileStatementContext::RPAREN() {
  return getToken(JavaScriptParser::RPAREN, 0);
}

JavaScriptParser::StatementContext* JavaScriptParser::WhileStatementContext::statement() {
  return getRuleContext<JavaScriptParser::StatementContext>(0);
}

JavaScriptParser::WhileStatementContext::WhileStatementContext(IterationStatementContext *ctx) { copyFrom(ctx); }

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
//----------------- ForStatementContext ------------------------------------------------------------------

tree::TerminalNode* JavaScriptParser::ForStatementContext::FOR() {
  return getToken(JavaScriptParser::FOR, 0);
}

tree::TerminalNode* JavaScriptParser::ForStatementContext::LPAREN() {
  return getToken(JavaScriptParser::LPAREN, 0);
}

std::vector<tree::TerminalNode *> JavaScriptParser::ForStatementContext::SEMICOLON() {
  return getTokens(JavaScriptParser::SEMICOLON);
}

tree::TerminalNode* JavaScriptParser::ForStatementContext::SEMICOLON(size_t i) {
  return getToken(JavaScriptParser::SEMICOLON, i);
}

tree::TerminalNode* JavaScriptParser::ForStatementContext::RPAREN() {
  return getToken(JavaScriptParser::RPAREN, 0);
}

JavaScriptParser::StatementContext* JavaScriptParser::ForStatementContext::statement() {
  return getRuleContext<JavaScriptParser::StatementContext>(0);
}

std::vector<JavaScriptParser::ExpressionSequenceContext *> JavaScriptParser::ForStatementContext::expressionSequence() {
  return getRuleContexts<JavaScriptParser::ExpressionSequenceContext>();
}

JavaScriptParser::ExpressionSequenceContext* JavaScriptParser::ForStatementContext::expressionSequence(size_t i) {
  return getRuleContext<JavaScriptParser::ExpressionSequenceContext>(i);
}

JavaScriptParser::VariableDeclarationContext* JavaScriptParser::ForStatementContext::variableDeclaration() {
  return getRuleContext<JavaScriptParser::VariableDeclarationContext>(0);
}

JavaScriptParser::ForStatementContext::ForStatementContext(IterationStatementContext *ctx) { copyFrom(ctx); }

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
//----------------- ForInStatementContext ------------------------------------------------------------------

tree::TerminalNode* JavaScriptParser::ForInStatementContext::FOR() {
  return getToken(JavaScriptParser::FOR, 0);
}

tree::TerminalNode* JavaScriptParser::ForInStatementContext::LPAREN() {
  return getToken(JavaScriptParser::LPAREN, 0);
}

JavaScriptParser::ExpressionSequenceContext* JavaScriptParser::ForInStatementContext::expressionSequence() {
  return getRuleContext<JavaScriptParser::ExpressionSequenceContext>(0);
}

tree::TerminalNode* JavaScriptParser::ForInStatementContext::RPAREN() {
  return getToken(JavaScriptParser::RPAREN, 0);
}

JavaScriptParser::StatementContext* JavaScriptParser::ForInStatementContext::statement() {
  return getRuleContext<JavaScriptParser::StatementContext>(0);
}

tree::TerminalNode* JavaScriptParser::ForInStatementContext::IN() {
  return getToken(JavaScriptParser::IN, 0);
}

tree::TerminalNode* JavaScriptParser::ForInStatementContext::OF() {
  return getToken(JavaScriptParser::OF, 0);
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::ForInStatementContext::singleExpression() {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(0);
}

JavaScriptParser::VariableDeclarationContext* JavaScriptParser::ForInStatementContext::variableDeclaration() {
  return getRuleContext<JavaScriptParser::VariableDeclarationContext>(0);
}

JavaScriptParser::ForInStatementContext::ForInStatementContext(IterationStatementContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::ForInStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterForInStatement(this);
}
void JavaScriptParser::ForInStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitForInStatement(this);
}
JavaScriptParser::IterationStatementContext* JavaScriptParser::iterationStatement() {
  IterationStatementContext *_localctx = _tracker.createInstance<IterationStatementContext>(_ctx, getState());
  enterRule(_localctx, 28, JavaScriptParser::RuleIterationStatement);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(216);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 16, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<JavaScriptParser::WhileStatementContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(183);
      match(JavaScriptParser::WHILE);
      setState(184);
      match(JavaScriptParser::LPAREN);
      setState(185);
      expressionSequence();
      setState(186);
      match(JavaScriptParser::RPAREN);
      setState(187);
      statement();
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<JavaScriptParser::ForStatementContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(189);
      match(JavaScriptParser::FOR);
      setState(190);
      match(JavaScriptParser::LPAREN);
      setState(193);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case JavaScriptParser::DELETE:
        case JavaScriptParser::FUNCTION:
        case JavaScriptParser::NEW:
        case JavaScriptParser::SUPER:
        case JavaScriptParser::THIS:
        case JavaScriptParser::TYPEOF:
        case JavaScriptParser::VOID:
        case JavaScriptParser::ASYNC:
        case JavaScriptParser::AWAIT:
        case JavaScriptParser::NullLiteral:
        case JavaScriptParser::BooleanLiteral:
        case JavaScriptParser::DecimalLiteral:
        case JavaScriptParser::HexIntegerLiteral:
        case JavaScriptParser::OctalIntegerLiteral:
        case JavaScriptParser::BinaryIntegerLiteral:
        case JavaScriptParser::StringLiteral:
        case JavaScriptParser::TemplateStringLiteral:
        case JavaScriptParser::RegularExpressionLiteral:
        case JavaScriptParser::Identifier:
        case JavaScriptParser::PLUSPLUS:
        case JavaScriptParser::MINUSMINUS:
        case JavaScriptParser::PLUS:
        case JavaScriptParser::MINUS:
        case JavaScriptParser::BITNOT:
        case JavaScriptParser::NOT:
        case JavaScriptParser::LPAREN:
        case JavaScriptParser::LBRACE:
        case JavaScriptParser::LBRACKET: {
          setState(191);
          expressionSequence();
          break;
        }

        case JavaScriptParser::CONST:
        case JavaScriptParser::VAR:
        case JavaScriptParser::LET: {
          setState(192);
          variableDeclaration();
          break;
        }

        case JavaScriptParser::SEMICOLON: {
          break;
        }

      default:
        break;
      }
      setState(195);
      match(JavaScriptParser::SEMICOLON);
      setState(197);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if ((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & -283731068719070720) != 0) || ((((_la - 99) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 99)) & 21) != 0)) {
        setState(196);
        expressionSequence();
      }
      setState(199);
      match(JavaScriptParser::SEMICOLON);
      setState(201);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if ((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & -283731068719070720) != 0) || ((((_la - 99) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 99)) & 21) != 0)) {
        setState(200);
        expressionSequence();
      }
      setState(203);
      match(JavaScriptParser::RPAREN);
      setState(204);
      statement();
      break;
    }

    case 3: {
      _localctx = _tracker.createInstance<JavaScriptParser::ForInStatementContext>(_localctx);
      enterOuterAlt(_localctx, 3);
      setState(205);
      match(JavaScriptParser::FOR);
      setState(206);
      match(JavaScriptParser::LPAREN);
      setState(209);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case JavaScriptParser::DELETE:
        case JavaScriptParser::FUNCTION:
        case JavaScriptParser::NEW:
        case JavaScriptParser::SUPER:
        case JavaScriptParser::THIS:
        case JavaScriptParser::TYPEOF:
        case JavaScriptParser::VOID:
        case JavaScriptParser::ASYNC:
        case JavaScriptParser::AWAIT:
        case JavaScriptParser::NullLiteral:
        case JavaScriptParser::BooleanLiteral:
        case JavaScriptParser::DecimalLiteral:
        case JavaScriptParser::HexIntegerLiteral:
        case JavaScriptParser::OctalIntegerLiteral:
        case JavaScriptParser::BinaryIntegerLiteral:
        case JavaScriptParser::StringLiteral:
        case JavaScriptParser::TemplateStringLiteral:
        case JavaScriptParser::RegularExpressionLiteral:
        case JavaScriptParser::Identifier:
        case JavaScriptParser::PLUSPLUS:
        case JavaScriptParser::MINUSMINUS:
        case JavaScriptParser::PLUS:
        case JavaScriptParser::MINUS:
        case JavaScriptParser::BITNOT:
        case JavaScriptParser::NOT:
        case JavaScriptParser::LPAREN:
        case JavaScriptParser::LBRACE:
        case JavaScriptParser::LBRACKET: {
          setState(207);
          singleExpression(0);
          break;
        }

        case JavaScriptParser::CONST:
        case JavaScriptParser::VAR:
        case JavaScriptParser::LET: {
          setState(208);
          variableDeclaration();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(211);
      _la = _input->LA(1);
      if (!(_la == JavaScriptParser::IN

      || _la == JavaScriptParser::OF)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(212);
      expressionSequence();
      setState(213);
      match(JavaScriptParser::RPAREN);
      setState(214);
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

tree::TerminalNode* JavaScriptParser::ReturnStatementContext::SEMICOLON() {
  return getToken(JavaScriptParser::SEMICOLON, 0);
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
    setState(218);
    match(JavaScriptParser::RETURN);
    setState(220);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 17, _ctx)) {
    case 1: {
      setState(219);
      expressionSequence();
      break;
    }

    default:
      break;
    }
    setState(223);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 18, _ctx)) {
    case 1: {
      setState(222);
      match(JavaScriptParser::SEMICOLON);
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

//----------------- SwitchStatementContext ------------------------------------------------------------------

JavaScriptParser::SwitchStatementContext::SwitchStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::SwitchStatementContext::SWITCH() {
  return getToken(JavaScriptParser::SWITCH, 0);
}

tree::TerminalNode* JavaScriptParser::SwitchStatementContext::LPAREN() {
  return getToken(JavaScriptParser::LPAREN, 0);
}

JavaScriptParser::ExpressionSequenceContext* JavaScriptParser::SwitchStatementContext::expressionSequence() {
  return getRuleContext<JavaScriptParser::ExpressionSequenceContext>(0);
}

tree::TerminalNode* JavaScriptParser::SwitchStatementContext::RPAREN() {
  return getToken(JavaScriptParser::RPAREN, 0);
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
  enterRule(_localctx, 32, JavaScriptParser::RuleSwitchStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(225);
    match(JavaScriptParser::SWITCH);
    setState(226);
    match(JavaScriptParser::LPAREN);
    setState(227);
    expressionSequence();
    setState(228);
    match(JavaScriptParser::RPAREN);
    setState(229);
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

tree::TerminalNode* JavaScriptParser::CaseBlockContext::LBRACE() {
  return getToken(JavaScriptParser::LBRACE, 0);
}

tree::TerminalNode* JavaScriptParser::CaseBlockContext::RBRACE() {
  return getToken(JavaScriptParser::RBRACE, 0);
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
  enterRule(_localctx, 34, JavaScriptParser::RuleCaseBlock);
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
    setState(231);
    match(JavaScriptParser::LBRACE);
    setState(233);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == JavaScriptParser::CASE) {
      setState(232);
      caseClauses();
    }
    setState(239);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == JavaScriptParser::DEFAULT) {
      setState(235);
      defaultClause();
      setState(237);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == JavaScriptParser::CASE) {
        setState(236);
        caseClauses();
      }
    }
    setState(241);
    match(JavaScriptParser::RBRACE);
   
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
  enterRule(_localctx, 36, JavaScriptParser::RuleCaseClauses);
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
    setState(244); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(243);
      caseClause();
      setState(246); 
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

tree::TerminalNode* JavaScriptParser::CaseClauseContext::COLON() {
  return getToken(JavaScriptParser::COLON, 0);
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
  enterRule(_localctx, 38, JavaScriptParser::RuleCaseClause);
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
    match(JavaScriptParser::CASE);
    setState(249);
    expressionSequence();
    setState(250);
    match(JavaScriptParser::COLON);
    setState(252);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & -283730925466484192) != 0) || ((((_la - 97) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 97)) & 85) != 0)) {
      setState(251);
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

tree::TerminalNode* JavaScriptParser::DefaultClauseContext::COLON() {
  return getToken(JavaScriptParser::COLON, 0);
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
  enterRule(_localctx, 40, JavaScriptParser::RuleDefaultClause);
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
    setState(254);
    match(JavaScriptParser::DEFAULT);
    setState(255);
    match(JavaScriptParser::COLON);
    setState(257);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & -283730925466484192) != 0) || ((((_la - 97) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 97)) & 85) != 0)) {
      setState(256);
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

tree::TerminalNode* JavaScriptParser::ThrowStatementContext::SEMICOLON() {
  return getToken(JavaScriptParser::SEMICOLON, 0);
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
  enterRule(_localctx, 42, JavaScriptParser::RuleThrowStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(259);
    match(JavaScriptParser::THROW);
    setState(260);
    expressionSequence();
    setState(262);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 25, _ctx)) {
    case 1: {
      setState(261);
      match(JavaScriptParser::SEMICOLON);
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
  enterRule(_localctx, 44, JavaScriptParser::RuleTryStatement);
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
    setState(264);
    match(JavaScriptParser::TRY);
    setState(265);
    block();
    setState(271);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case JavaScriptParser::CATCH: {
        setState(266);
        catchProduction();
        setState(268);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == JavaScriptParser::FINALLY) {
          setState(267);
          finallyProduction();
        }
        break;
      }

      case JavaScriptParser::FINALLY: {
        setState(270);
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

tree::TerminalNode* JavaScriptParser::CatchProductionContext::LPAREN() {
  return getToken(JavaScriptParser::LPAREN, 0);
}

tree::TerminalNode* JavaScriptParser::CatchProductionContext::RPAREN() {
  return getToken(JavaScriptParser::RPAREN, 0);
}

JavaScriptParser::BlockContext* JavaScriptParser::CatchProductionContext::block() {
  return getRuleContext<JavaScriptParser::BlockContext>(0);
}

tree::TerminalNode* JavaScriptParser::CatchProductionContext::Identifier() {
  return getToken(JavaScriptParser::Identifier, 0);
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
  enterRule(_localctx, 46, JavaScriptParser::RuleCatchProduction);
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
    setState(273);
    match(JavaScriptParser::CATCH);
    setState(274);
    match(JavaScriptParser::LPAREN);
    setState(276);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == JavaScriptParser::Identifier) {
      setState(275);
      match(JavaScriptParser::Identifier);
    }
    setState(278);
    match(JavaScriptParser::RPAREN);
    setState(279);
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
  enterRule(_localctx, 48, JavaScriptParser::RuleFinallyProduction);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(281);
    match(JavaScriptParser::FINALLY);
    setState(282);
    block();
   
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

tree::TerminalNode* JavaScriptParser::FunctionDeclarationContext::LPAREN() {
  return getToken(JavaScriptParser::LPAREN, 0);
}

tree::TerminalNode* JavaScriptParser::FunctionDeclarationContext::RPAREN() {
  return getToken(JavaScriptParser::RPAREN, 0);
}

JavaScriptParser::FunctionBodyContext* JavaScriptParser::FunctionDeclarationContext::functionBody() {
  return getRuleContext<JavaScriptParser::FunctionBodyContext>(0);
}

tree::TerminalNode* JavaScriptParser::FunctionDeclarationContext::ASYNC() {
  return getToken(JavaScriptParser::ASYNC, 0);
}

tree::TerminalNode* JavaScriptParser::FunctionDeclarationContext::STAR() {
  return getToken(JavaScriptParser::STAR, 0);
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
  enterRule(_localctx, 50, JavaScriptParser::RuleFunctionDeclaration);
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
    setState(285);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == JavaScriptParser::ASYNC) {
      setState(284);
      match(JavaScriptParser::ASYNC);
    }
    setState(287);
    match(JavaScriptParser::FUNCTION);
    setState(289);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == JavaScriptParser::STAR) {
      setState(288);
      match(JavaScriptParser::STAR);
    }
    setState(291);
    match(JavaScriptParser::Identifier);
    setState(292);
    match(JavaScriptParser::LPAREN);
    setState(294);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == JavaScriptParser::Identifier) {
      setState(293);
      formalParameterList();
    }
    setState(296);
    match(JavaScriptParser::RPAREN);
    setState(297);
    functionBody();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ClassDeclarationContext ------------------------------------------------------------------

JavaScriptParser::ClassDeclarationContext::ClassDeclarationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::ClassDeclarationContext::CLASS() {
  return getToken(JavaScriptParser::CLASS, 0);
}

tree::TerminalNode* JavaScriptParser::ClassDeclarationContext::Identifier() {
  return getToken(JavaScriptParser::Identifier, 0);
}

tree::TerminalNode* JavaScriptParser::ClassDeclarationContext::LBRACE() {
  return getToken(JavaScriptParser::LBRACE, 0);
}

tree::TerminalNode* JavaScriptParser::ClassDeclarationContext::RBRACE() {
  return getToken(JavaScriptParser::RBRACE, 0);
}

tree::TerminalNode* JavaScriptParser::ClassDeclarationContext::EXTENDS() {
  return getToken(JavaScriptParser::EXTENDS, 0);
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::ClassDeclarationContext::singleExpression() {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(0);
}

std::vector<JavaScriptParser::ClassElementContext *> JavaScriptParser::ClassDeclarationContext::classElement() {
  return getRuleContexts<JavaScriptParser::ClassElementContext>();
}

JavaScriptParser::ClassElementContext* JavaScriptParser::ClassDeclarationContext::classElement(size_t i) {
  return getRuleContext<JavaScriptParser::ClassElementContext>(i);
}


size_t JavaScriptParser::ClassDeclarationContext::getRuleIndex() const {
  return JavaScriptParser::RuleClassDeclaration;
}

void JavaScriptParser::ClassDeclarationContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterClassDeclaration(this);
}

void JavaScriptParser::ClassDeclarationContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitClassDeclaration(this);
}

JavaScriptParser::ClassDeclarationContext* JavaScriptParser::classDeclaration() {
  ClassDeclarationContext *_localctx = _tracker.createInstance<ClassDeclarationContext>(_ctx, getState());
  enterRule(_localctx, 52, JavaScriptParser::RuleClassDeclaration);
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
    match(JavaScriptParser::CLASS);
    setState(300);
    match(JavaScriptParser::Identifier);
    setState(303);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == JavaScriptParser::EXTENDS) {
      setState(301);
      match(JavaScriptParser::EXTENDS);
      setState(302);
      singleExpression(0);
    }
    setState(305);
    match(JavaScriptParser::LBRACE);
    setState(309);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 2814749767106558) != 0) || ((((_la - 64) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 64)) & 558345748481) != 0)) {
      setState(306);
      classElement();
      setState(311);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(312);
    match(JavaScriptParser::RBRACE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ClassElementContext ------------------------------------------------------------------

JavaScriptParser::ClassElementContext::ClassElementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

JavaScriptParser::MethodDefinitionContext* JavaScriptParser::ClassElementContext::methodDefinition() {
  return getRuleContext<JavaScriptParser::MethodDefinitionContext>(0);
}

tree::TerminalNode* JavaScriptParser::ClassElementContext::STATIC() {
  return getToken(JavaScriptParser::STATIC, 0);
}

JavaScriptParser::EmptyStatementContext* JavaScriptParser::ClassElementContext::emptyStatement() {
  return getRuleContext<JavaScriptParser::EmptyStatementContext>(0);
}


size_t JavaScriptParser::ClassElementContext::getRuleIndex() const {
  return JavaScriptParser::RuleClassElement;
}

void JavaScriptParser::ClassElementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterClassElement(this);
}

void JavaScriptParser::ClassElementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitClassElement(this);
}

JavaScriptParser::ClassElementContext* JavaScriptParser::classElement() {
  ClassElementContext *_localctx = _tracker.createInstance<ClassElementContext>(_ctx, getState());
  enterRule(_localctx, 54, JavaScriptParser::RuleClassElement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(319);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case JavaScriptParser::BREAK:
      case JavaScriptParser::CASE:
      case JavaScriptParser::CATCH:
      case JavaScriptParser::CLASS:
      case JavaScriptParser::CONST:
      case JavaScriptParser::CONTINUE:
      case JavaScriptParser::DEBUGGER:
      case JavaScriptParser::DEFAULT:
      case JavaScriptParser::DELETE:
      case JavaScriptParser::DO:
      case JavaScriptParser::ELSE:
      case JavaScriptParser::ENUM:
      case JavaScriptParser::EXPORT:
      case JavaScriptParser::EXTENDS:
      case JavaScriptParser::FINALLY:
      case JavaScriptParser::FOR:
      case JavaScriptParser::FUNCTION:
      case JavaScriptParser::IF:
      case JavaScriptParser::IMPORT:
      case JavaScriptParser::IN:
      case JavaScriptParser::INSTANCEOF:
      case JavaScriptParser::NEW:
      case JavaScriptParser::RETURN:
      case JavaScriptParser::SUPER:
      case JavaScriptParser::SWITCH:
      case JavaScriptParser::THIS:
      case JavaScriptParser::THROW:
      case JavaScriptParser::TRY:
      case JavaScriptParser::TYPEOF:
      case JavaScriptParser::VAR:
      case JavaScriptParser::VOID:
      case JavaScriptParser::WHILE:
      case JavaScriptParser::WITH:
      case JavaScriptParser::YIELD:
      case JavaScriptParser::ASYNC:
      case JavaScriptParser::AWAIT:
      case JavaScriptParser::LET:
      case JavaScriptParser::OF:
      case JavaScriptParser::STATIC:
      case JavaScriptParser::GET:
      case JavaScriptParser::SET:
      case JavaScriptParser::NullLiteral:
      case JavaScriptParser::BooleanLiteral:
      case JavaScriptParser::DecimalLiteral:
      case JavaScriptParser::HexIntegerLiteral:
      case JavaScriptParser::OctalIntegerLiteral:
      case JavaScriptParser::BinaryIntegerLiteral:
      case JavaScriptParser::StringLiteral:
      case JavaScriptParser::Identifier:
      case JavaScriptParser::STAR:
      case JavaScriptParser::LBRACKET: {
        enterOuterAlt(_localctx, 1);
        setState(315);
        _errHandler->sync(this);

        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 34, _ctx)) {
        case 1: {
          setState(314);
          match(JavaScriptParser::STATIC);
          break;
        }

        default:
          break;
        }
        setState(317);
        methodDefinition();
        break;
      }

      case JavaScriptParser::SEMICOLON: {
        enterOuterAlt(_localctx, 2);
        setState(318);
        emptyStatement();
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

//----------------- MethodDefinitionContext ------------------------------------------------------------------

JavaScriptParser::MethodDefinitionContext::MethodDefinitionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

JavaScriptParser::PropertyNameContext* JavaScriptParser::MethodDefinitionContext::propertyName() {
  return getRuleContext<JavaScriptParser::PropertyNameContext>(0);
}

tree::TerminalNode* JavaScriptParser::MethodDefinitionContext::LPAREN() {
  return getToken(JavaScriptParser::LPAREN, 0);
}

tree::TerminalNode* JavaScriptParser::MethodDefinitionContext::RPAREN() {
  return getToken(JavaScriptParser::RPAREN, 0);
}

JavaScriptParser::FunctionBodyContext* JavaScriptParser::MethodDefinitionContext::functionBody() {
  return getRuleContext<JavaScriptParser::FunctionBodyContext>(0);
}

tree::TerminalNode* JavaScriptParser::MethodDefinitionContext::STAR() {
  return getToken(JavaScriptParser::STAR, 0);
}

JavaScriptParser::FormalParameterListContext* JavaScriptParser::MethodDefinitionContext::formalParameterList() {
  return getRuleContext<JavaScriptParser::FormalParameterListContext>(0);
}

JavaScriptParser::GetterContext* JavaScriptParser::MethodDefinitionContext::getter() {
  return getRuleContext<JavaScriptParser::GetterContext>(0);
}

JavaScriptParser::SetterContext* JavaScriptParser::MethodDefinitionContext::setter() {
  return getRuleContext<JavaScriptParser::SetterContext>(0);
}


size_t JavaScriptParser::MethodDefinitionContext::getRuleIndex() const {
  return JavaScriptParser::RuleMethodDefinition;
}

void JavaScriptParser::MethodDefinitionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMethodDefinition(this);
}

void JavaScriptParser::MethodDefinitionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMethodDefinition(this);
}

JavaScriptParser::MethodDefinitionContext* JavaScriptParser::methodDefinition() {
  MethodDefinitionContext *_localctx = _tracker.createInstance<MethodDefinitionContext>(_ctx, getState());
  enterRule(_localctx, 56, JavaScriptParser::RuleMethodDefinition);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(343);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case JavaScriptParser::BREAK:
      case JavaScriptParser::CASE:
      case JavaScriptParser::CATCH:
      case JavaScriptParser::CLASS:
      case JavaScriptParser::CONST:
      case JavaScriptParser::CONTINUE:
      case JavaScriptParser::DEBUGGER:
      case JavaScriptParser::DEFAULT:
      case JavaScriptParser::DELETE:
      case JavaScriptParser::DO:
      case JavaScriptParser::ELSE:
      case JavaScriptParser::ENUM:
      case JavaScriptParser::EXPORT:
      case JavaScriptParser::EXTENDS:
      case JavaScriptParser::FINALLY:
      case JavaScriptParser::FOR:
      case JavaScriptParser::FUNCTION:
      case JavaScriptParser::IF:
      case JavaScriptParser::IMPORT:
      case JavaScriptParser::IN:
      case JavaScriptParser::INSTANCEOF:
      case JavaScriptParser::NEW:
      case JavaScriptParser::RETURN:
      case JavaScriptParser::SUPER:
      case JavaScriptParser::SWITCH:
      case JavaScriptParser::THIS:
      case JavaScriptParser::THROW:
      case JavaScriptParser::TRY:
      case JavaScriptParser::TYPEOF:
      case JavaScriptParser::VAR:
      case JavaScriptParser::VOID:
      case JavaScriptParser::WHILE:
      case JavaScriptParser::WITH:
      case JavaScriptParser::YIELD:
      case JavaScriptParser::ASYNC:
      case JavaScriptParser::AWAIT:
      case JavaScriptParser::LET:
      case JavaScriptParser::OF:
      case JavaScriptParser::STATIC:
      case JavaScriptParser::NullLiteral:
      case JavaScriptParser::BooleanLiteral:
      case JavaScriptParser::DecimalLiteral:
      case JavaScriptParser::HexIntegerLiteral:
      case JavaScriptParser::OctalIntegerLiteral:
      case JavaScriptParser::BinaryIntegerLiteral:
      case JavaScriptParser::StringLiteral:
      case JavaScriptParser::Identifier:
      case JavaScriptParser::STAR:
      case JavaScriptParser::LBRACKET: {
        enterOuterAlt(_localctx, 1);
        setState(322);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == JavaScriptParser::STAR) {
          setState(321);
          match(JavaScriptParser::STAR);
        }
        setState(324);
        propertyName();
        setState(325);
        match(JavaScriptParser::LPAREN);
        setState(327);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == JavaScriptParser::Identifier) {
          setState(326);
          formalParameterList();
        }
        setState(329);
        match(JavaScriptParser::RPAREN);
        setState(330);
        functionBody();
        break;
      }

      case JavaScriptParser::GET: {
        enterOuterAlt(_localctx, 2);
        setState(332);
        getter();
        setState(333);
        match(JavaScriptParser::LPAREN);
        setState(334);
        match(JavaScriptParser::RPAREN);
        setState(335);
        functionBody();
        break;
      }

      case JavaScriptParser::SET: {
        enterOuterAlt(_localctx, 3);
        setState(337);
        setter();
        setState(338);
        match(JavaScriptParser::LPAREN);
        setState(339);
        formalParameterList();
        setState(340);
        match(JavaScriptParser::RPAREN);
        setState(341);
        functionBody();
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

//----------------- FormalParameterListContext ------------------------------------------------------------------

JavaScriptParser::FormalParameterListContext::FormalParameterListContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<JavaScriptParser::FormalParameterArgContext *> JavaScriptParser::FormalParameterListContext::formalParameterArg() {
  return getRuleContexts<JavaScriptParser::FormalParameterArgContext>();
}

JavaScriptParser::FormalParameterArgContext* JavaScriptParser::FormalParameterListContext::formalParameterArg(size_t i) {
  return getRuleContext<JavaScriptParser::FormalParameterArgContext>(i);
}

std::vector<tree::TerminalNode *> JavaScriptParser::FormalParameterListContext::COMMA() {
  return getTokens(JavaScriptParser::COMMA);
}

tree::TerminalNode* JavaScriptParser::FormalParameterListContext::COMMA(size_t i) {
  return getToken(JavaScriptParser::COMMA, i);
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
    setState(345);
    formalParameterArg();
    setState(350);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == JavaScriptParser::COMMA) {
      setState(346);
      match(JavaScriptParser::COMMA);
      setState(347);
      formalParameterArg();
      setState(352);
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

//----------------- FormalParameterArgContext ------------------------------------------------------------------

JavaScriptParser::FormalParameterArgContext::FormalParameterArgContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::FormalParameterArgContext::Identifier() {
  return getToken(JavaScriptParser::Identifier, 0);
}

tree::TerminalNode* JavaScriptParser::FormalParameterArgContext::ASSIGN() {
  return getToken(JavaScriptParser::ASSIGN, 0);
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::FormalParameterArgContext::singleExpression() {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(0);
}


size_t JavaScriptParser::FormalParameterArgContext::getRuleIndex() const {
  return JavaScriptParser::RuleFormalParameterArg;
}

void JavaScriptParser::FormalParameterArgContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFormalParameterArg(this);
}

void JavaScriptParser::FormalParameterArgContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFormalParameterArg(this);
}

JavaScriptParser::FormalParameterArgContext* JavaScriptParser::formalParameterArg() {
  FormalParameterArgContext *_localctx = _tracker.createInstance<FormalParameterArgContext>(_ctx, getState());
  enterRule(_localctx, 60, JavaScriptParser::RuleFormalParameterArg);
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
    setState(353);
    match(JavaScriptParser::Identifier);
    setState(356);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == JavaScriptParser::ASSIGN) {
      setState(354);
      match(JavaScriptParser::ASSIGN);
      setState(355);
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

//----------------- FunctionBodyContext ------------------------------------------------------------------

JavaScriptParser::FunctionBodyContext::FunctionBodyContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::FunctionBodyContext::LBRACE() {
  return getToken(JavaScriptParser::LBRACE, 0);
}

tree::TerminalNode* JavaScriptParser::FunctionBodyContext::RBRACE() {
  return getToken(JavaScriptParser::RBRACE, 0);
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
  enterRule(_localctx, 62, JavaScriptParser::RuleFunctionBody);
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
    setState(358);
    match(JavaScriptParser::LBRACE);
    setState(360);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & -283730925466484176) != 0) || ((((_la - 97) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 97)) & 85) != 0)) {
      setState(359);
      sourceElements();
    }
    setState(362);
    match(JavaScriptParser::RBRACE);
   
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

tree::TerminalNode* JavaScriptParser::ArrayLiteralContext::LBRACKET() {
  return getToken(JavaScriptParser::LBRACKET, 0);
}

tree::TerminalNode* JavaScriptParser::ArrayLiteralContext::RBRACKET() {
  return getToken(JavaScriptParser::RBRACKET, 0);
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
  enterRule(_localctx, 64, JavaScriptParser::RuleArrayLiteral);
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
    setState(364);
    match(JavaScriptParser::LBRACKET);
    setState(366);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & -283731068719070720) != 0) || ((((_la - 99) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 99)) & 21) != 0)) {
      setState(365);
      elementList();
    }
    setState(368);
    match(JavaScriptParser::RBRACKET);
   
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

std::vector<JavaScriptParser::ArrayElementContext *> JavaScriptParser::ElementListContext::arrayElement() {
  return getRuleContexts<JavaScriptParser::ArrayElementContext>();
}

JavaScriptParser::ArrayElementContext* JavaScriptParser::ElementListContext::arrayElement(size_t i) {
  return getRuleContext<JavaScriptParser::ArrayElementContext>(i);
}

std::vector<tree::TerminalNode *> JavaScriptParser::ElementListContext::COMMA() {
  return getTokens(JavaScriptParser::COMMA);
}

tree::TerminalNode* JavaScriptParser::ElementListContext::COMMA(size_t i) {
  return getToken(JavaScriptParser::COMMA, i);
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
  enterRule(_localctx, 66, JavaScriptParser::RuleElementList);
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
    setState(370);
    arrayElement();
    setState(375);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == JavaScriptParser::COMMA) {
      setState(371);
      match(JavaScriptParser::COMMA);
      setState(372);
      arrayElement();
      setState(377);
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

//----------------- ArrayElementContext ------------------------------------------------------------------

JavaScriptParser::ArrayElementContext::ArrayElementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::ArrayElementContext::singleExpression() {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(0);
}


size_t JavaScriptParser::ArrayElementContext::getRuleIndex() const {
  return JavaScriptParser::RuleArrayElement;
}

void JavaScriptParser::ArrayElementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterArrayElement(this);
}

void JavaScriptParser::ArrayElementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitArrayElement(this);
}

JavaScriptParser::ArrayElementContext* JavaScriptParser::arrayElement() {
  ArrayElementContext *_localctx = _tracker.createInstance<ArrayElementContext>(_ctx, getState());
  enterRule(_localctx, 68, JavaScriptParser::RuleArrayElement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(378);
    singleExpression(0);
   
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

tree::TerminalNode* JavaScriptParser::ObjectLiteralContext::LBRACE() {
  return getToken(JavaScriptParser::LBRACE, 0);
}

tree::TerminalNode* JavaScriptParser::ObjectLiteralContext::RBRACE() {
  return getToken(JavaScriptParser::RBRACE, 0);
}

std::vector<JavaScriptParser::PropertyAssignmentContext *> JavaScriptParser::ObjectLiteralContext::propertyAssignment() {
  return getRuleContexts<JavaScriptParser::PropertyAssignmentContext>();
}

JavaScriptParser::PropertyAssignmentContext* JavaScriptParser::ObjectLiteralContext::propertyAssignment(size_t i) {
  return getRuleContext<JavaScriptParser::PropertyAssignmentContext>(i);
}

std::vector<tree::TerminalNode *> JavaScriptParser::ObjectLiteralContext::COMMA() {
  return getTokens(JavaScriptParser::COMMA);
}

tree::TerminalNode* JavaScriptParser::ObjectLiteralContext::COMMA(size_t i) {
  return getToken(JavaScriptParser::COMMA, i);
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
  enterRule(_localctx, 70, JavaScriptParser::RuleObjectLiteral);
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
    setState(380);
    match(JavaScriptParser::LBRACE);
    setState(389);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 2814749767106558) != 0) || _la == JavaScriptParser::STAR

    || _la == JavaScriptParser::LBRACKET) {
      setState(381);
      propertyAssignment();
      setState(386);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == JavaScriptParser::COMMA) {
        setState(382);
        match(JavaScriptParser::COMMA);
        setState(383);
        propertyAssignment();
        setState(388);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
    setState(391);
    match(JavaScriptParser::RBRACE);
   
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

tree::TerminalNode* JavaScriptParser::PropertyExpressionAssignmentContext::COLON() {
  return getToken(JavaScriptParser::COLON, 0);
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
//----------------- ComputedPropertyExpressionAssignmentContext ------------------------------------------------------------------

tree::TerminalNode* JavaScriptParser::ComputedPropertyExpressionAssignmentContext::LBRACKET() {
  return getToken(JavaScriptParser::LBRACKET, 0);
}

std::vector<JavaScriptParser::SingleExpressionContext *> JavaScriptParser::ComputedPropertyExpressionAssignmentContext::singleExpression() {
  return getRuleContexts<JavaScriptParser::SingleExpressionContext>();
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::ComputedPropertyExpressionAssignmentContext::singleExpression(size_t i) {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(i);
}

tree::TerminalNode* JavaScriptParser::ComputedPropertyExpressionAssignmentContext::RBRACKET() {
  return getToken(JavaScriptParser::RBRACKET, 0);
}

tree::TerminalNode* JavaScriptParser::ComputedPropertyExpressionAssignmentContext::COLON() {
  return getToken(JavaScriptParser::COLON, 0);
}

JavaScriptParser::ComputedPropertyExpressionAssignmentContext::ComputedPropertyExpressionAssignmentContext(PropertyAssignmentContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::ComputedPropertyExpressionAssignmentContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterComputedPropertyExpressionAssignment(this);
}
void JavaScriptParser::ComputedPropertyExpressionAssignmentContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitComputedPropertyExpressionAssignment(this);
}
//----------------- PropertySetterContext ------------------------------------------------------------------

JavaScriptParser::SetterContext* JavaScriptParser::PropertySetterContext::setter() {
  return getRuleContext<JavaScriptParser::SetterContext>(0);
}

tree::TerminalNode* JavaScriptParser::PropertySetterContext::LPAREN() {
  return getToken(JavaScriptParser::LPAREN, 0);
}

JavaScriptParser::FormalParameterArgContext* JavaScriptParser::PropertySetterContext::formalParameterArg() {
  return getRuleContext<JavaScriptParser::FormalParameterArgContext>(0);
}

tree::TerminalNode* JavaScriptParser::PropertySetterContext::RPAREN() {
  return getToken(JavaScriptParser::RPAREN, 0);
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

JavaScriptParser::GetterContext* JavaScriptParser::PropertyGetterContext::getter() {
  return getRuleContext<JavaScriptParser::GetterContext>(0);
}

tree::TerminalNode* JavaScriptParser::PropertyGetterContext::LPAREN() {
  return getToken(JavaScriptParser::LPAREN, 0);
}

tree::TerminalNode* JavaScriptParser::PropertyGetterContext::RPAREN() {
  return getToken(JavaScriptParser::RPAREN, 0);
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
//----------------- FunctionPropertyContext ------------------------------------------------------------------

JavaScriptParser::PropertyNameContext* JavaScriptParser::FunctionPropertyContext::propertyName() {
  return getRuleContext<JavaScriptParser::PropertyNameContext>(0);
}

tree::TerminalNode* JavaScriptParser::FunctionPropertyContext::LPAREN() {
  return getToken(JavaScriptParser::LPAREN, 0);
}

tree::TerminalNode* JavaScriptParser::FunctionPropertyContext::RPAREN() {
  return getToken(JavaScriptParser::RPAREN, 0);
}

JavaScriptParser::FunctionBodyContext* JavaScriptParser::FunctionPropertyContext::functionBody() {
  return getRuleContext<JavaScriptParser::FunctionBodyContext>(0);
}

tree::TerminalNode* JavaScriptParser::FunctionPropertyContext::STAR() {
  return getToken(JavaScriptParser::STAR, 0);
}

JavaScriptParser::FormalParameterListContext* JavaScriptParser::FunctionPropertyContext::formalParameterList() {
  return getRuleContext<JavaScriptParser::FormalParameterListContext>(0);
}

JavaScriptParser::FunctionPropertyContext::FunctionPropertyContext(PropertyAssignmentContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::FunctionPropertyContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFunctionProperty(this);
}
void JavaScriptParser::FunctionPropertyContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFunctionProperty(this);
}
JavaScriptParser::PropertyAssignmentContext* JavaScriptParser::propertyAssignment() {
  PropertyAssignmentContext *_localctx = _tracker.createInstance<PropertyAssignmentContext>(_ctx, getState());
  enterRule(_localctx, 72, JavaScriptParser::RulePropertyAssignment);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(425);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 48, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<JavaScriptParser::PropertyExpressionAssignmentContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(393);
      propertyName();
      setState(394);
      match(JavaScriptParser::COLON);
      setState(395);
      singleExpression(0);
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<JavaScriptParser::ComputedPropertyExpressionAssignmentContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(397);
      match(JavaScriptParser::LBRACKET);
      setState(398);
      singleExpression(0);
      setState(399);
      match(JavaScriptParser::RBRACKET);
      setState(400);
      match(JavaScriptParser::COLON);
      setState(401);
      singleExpression(0);
      break;
    }

    case 3: {
      _localctx = _tracker.createInstance<JavaScriptParser::FunctionPropertyContext>(_localctx);
      enterOuterAlt(_localctx, 3);
      setState(404);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == JavaScriptParser::STAR) {
        setState(403);
        match(JavaScriptParser::STAR);
      }
      setState(406);
      propertyName();
      setState(407);
      match(JavaScriptParser::LPAREN);
      setState(409);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == JavaScriptParser::Identifier) {
        setState(408);
        formalParameterList();
      }
      setState(411);
      match(JavaScriptParser::RPAREN);
      setState(412);
      functionBody();
      break;
    }

    case 4: {
      _localctx = _tracker.createInstance<JavaScriptParser::PropertyGetterContext>(_localctx);
      enterOuterAlt(_localctx, 4);
      setState(414);
      getter();
      setState(415);
      match(JavaScriptParser::LPAREN);
      setState(416);
      match(JavaScriptParser::RPAREN);
      setState(417);
      functionBody();
      break;
    }

    case 5: {
      _localctx = _tracker.createInstance<JavaScriptParser::PropertySetterContext>(_localctx);
      enterOuterAlt(_localctx, 5);
      setState(419);
      setter();
      setState(420);
      match(JavaScriptParser::LPAREN);
      setState(421);
      formalParameterArg();
      setState(422);
      match(JavaScriptParser::RPAREN);
      setState(423);
      functionBody();
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

tree::TerminalNode* JavaScriptParser::PropertyNameContext::LBRACKET() {
  return getToken(JavaScriptParser::LBRACKET, 0);
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::PropertyNameContext::singleExpression() {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(0);
}

tree::TerminalNode* JavaScriptParser::PropertyNameContext::RBRACKET() {
  return getToken(JavaScriptParser::RBRACKET, 0);
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
  enterRule(_localctx, 74, JavaScriptParser::RulePropertyName);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(434);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case JavaScriptParser::BREAK:
      case JavaScriptParser::CASE:
      case JavaScriptParser::CATCH:
      case JavaScriptParser::CLASS:
      case JavaScriptParser::CONST:
      case JavaScriptParser::CONTINUE:
      case JavaScriptParser::DEBUGGER:
      case JavaScriptParser::DEFAULT:
      case JavaScriptParser::DELETE:
      case JavaScriptParser::DO:
      case JavaScriptParser::ELSE:
      case JavaScriptParser::ENUM:
      case JavaScriptParser::EXPORT:
      case JavaScriptParser::EXTENDS:
      case JavaScriptParser::FINALLY:
      case JavaScriptParser::FOR:
      case JavaScriptParser::FUNCTION:
      case JavaScriptParser::IF:
      case JavaScriptParser::IMPORT:
      case JavaScriptParser::IN:
      case JavaScriptParser::INSTANCEOF:
      case JavaScriptParser::NEW:
      case JavaScriptParser::RETURN:
      case JavaScriptParser::SUPER:
      case JavaScriptParser::SWITCH:
      case JavaScriptParser::THIS:
      case JavaScriptParser::THROW:
      case JavaScriptParser::TRY:
      case JavaScriptParser::TYPEOF:
      case JavaScriptParser::VAR:
      case JavaScriptParser::VOID:
      case JavaScriptParser::WHILE:
      case JavaScriptParser::WITH:
      case JavaScriptParser::YIELD:
      case JavaScriptParser::ASYNC:
      case JavaScriptParser::AWAIT:
      case JavaScriptParser::LET:
      case JavaScriptParser::OF:
      case JavaScriptParser::STATIC:
      case JavaScriptParser::NullLiteral:
      case JavaScriptParser::BooleanLiteral:
      case JavaScriptParser::Identifier: {
        enterOuterAlt(_localctx, 1);
        setState(427);
        identifierName();
        break;
      }

      case JavaScriptParser::StringLiteral: {
        enterOuterAlt(_localctx, 2);
        setState(428);
        match(JavaScriptParser::StringLiteral);
        break;
      }

      case JavaScriptParser::DecimalLiteral:
      case JavaScriptParser::HexIntegerLiteral:
      case JavaScriptParser::OctalIntegerLiteral:
      case JavaScriptParser::BinaryIntegerLiteral: {
        enterOuterAlt(_localctx, 3);
        setState(429);
        numericLiteral();
        break;
      }

      case JavaScriptParser::LBRACKET: {
        enterOuterAlt(_localctx, 4);
        setState(430);
        match(JavaScriptParser::LBRACKET);
        setState(431);
        singleExpression(0);
        setState(432);
        match(JavaScriptParser::RBRACKET);
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

tree::TerminalNode* JavaScriptParser::ArgumentsContext::LPAREN() {
  return getToken(JavaScriptParser::LPAREN, 0);
}

tree::TerminalNode* JavaScriptParser::ArgumentsContext::RPAREN() {
  return getToken(JavaScriptParser::RPAREN, 0);
}

std::vector<JavaScriptParser::ArgumentContext *> JavaScriptParser::ArgumentsContext::argument() {
  return getRuleContexts<JavaScriptParser::ArgumentContext>();
}

JavaScriptParser::ArgumentContext* JavaScriptParser::ArgumentsContext::argument(size_t i) {
  return getRuleContext<JavaScriptParser::ArgumentContext>(i);
}

std::vector<tree::TerminalNode *> JavaScriptParser::ArgumentsContext::COMMA() {
  return getTokens(JavaScriptParser::COMMA);
}

tree::TerminalNode* JavaScriptParser::ArgumentsContext::COMMA(size_t i) {
  return getToken(JavaScriptParser::COMMA, i);
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
  enterRule(_localctx, 76, JavaScriptParser::RuleArguments);
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
    setState(436);
    match(JavaScriptParser::LPAREN);
    setState(445);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & -283731068719070720) != 0) || ((((_la - 99) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 99)) & 21) != 0)) {
      setState(437);
      argument();
      setState(442);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == JavaScriptParser::COMMA) {
        setState(438);
        match(JavaScriptParser::COMMA);
        setState(439);
        argument();
        setState(444);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
    setState(447);
    match(JavaScriptParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ArgumentContext ------------------------------------------------------------------

JavaScriptParser::ArgumentContext::ArgumentContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::ArgumentContext::singleExpression() {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(0);
}


size_t JavaScriptParser::ArgumentContext::getRuleIndex() const {
  return JavaScriptParser::RuleArgument;
}

void JavaScriptParser::ArgumentContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterArgument(this);
}

void JavaScriptParser::ArgumentContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitArgument(this);
}

JavaScriptParser::ArgumentContext* JavaScriptParser::argument() {
  ArgumentContext *_localctx = _tracker.createInstance<ArgumentContext>(_ctx, getState());
  enterRule(_localctx, 78, JavaScriptParser::RuleArgument);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(449);
    singleExpression(0);
   
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

std::vector<tree::TerminalNode *> JavaScriptParser::ExpressionSequenceContext::COMMA() {
  return getTokens(JavaScriptParser::COMMA);
}

tree::TerminalNode* JavaScriptParser::ExpressionSequenceContext::COMMA(size_t i) {
  return getToken(JavaScriptParser::COMMA, i);
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
  enterRule(_localctx, 80, JavaScriptParser::RuleExpressionSequence);
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
    setState(451);
    singleExpression(0);
    setState(456);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == JavaScriptParser::COMMA) {
      setState(452);
      match(JavaScriptParser::COMMA);
      setState(453);
      singleExpression(0);
      setState(458);
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

tree::TerminalNode* JavaScriptParser::TernaryExpressionContext::QUESTION() {
  return getToken(JavaScriptParser::QUESTION, 0);
}

tree::TerminalNode* JavaScriptParser::TernaryExpressionContext::COLON() {
  return getToken(JavaScriptParser::COLON, 0);
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

tree::TerminalNode* JavaScriptParser::LogicalAndExpressionContext::AND() {
  return getToken(JavaScriptParser::AND, 0);
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
//----------------- PowerExpressionContext ------------------------------------------------------------------

std::vector<JavaScriptParser::SingleExpressionContext *> JavaScriptParser::PowerExpressionContext::singleExpression() {
  return getRuleContexts<JavaScriptParser::SingleExpressionContext>();
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::PowerExpressionContext::singleExpression(size_t i) {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(i);
}

tree::TerminalNode* JavaScriptParser::PowerExpressionContext::POWER() {
  return getToken(JavaScriptParser::POWER, 0);
}

JavaScriptParser::PowerExpressionContext::PowerExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::PowerExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPowerExpression(this);
}
void JavaScriptParser::PowerExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPowerExpression(this);
}
//----------------- PreIncrementExpressionContext ------------------------------------------------------------------

tree::TerminalNode* JavaScriptParser::PreIncrementExpressionContext::PLUSPLUS() {
  return getToken(JavaScriptParser::PLUSPLUS, 0);
}

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

tree::TerminalNode* JavaScriptParser::LogicalOrExpressionContext::OR() {
  return getToken(JavaScriptParser::OR, 0);
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

tree::TerminalNode* JavaScriptParser::NotExpressionContext::NOT() {
  return getToken(JavaScriptParser::NOT, 0);
}

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

tree::TerminalNode* JavaScriptParser::PreDecreaseExpressionContext::MINUSMINUS() {
  return getToken(JavaScriptParser::MINUSMINUS, 0);
}

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
//----------------- AnonymousFunctionExpressionContext ------------------------------------------------------------------

JavaScriptParser::AnonymousFunctionContext* JavaScriptParser::AnonymousFunctionExpressionContext::anonymousFunction() {
  return getRuleContext<JavaScriptParser::AnonymousFunctionContext>(0);
}

JavaScriptParser::AnonymousFunctionExpressionContext::AnonymousFunctionExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::AnonymousFunctionExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAnonymousFunctionExpression(this);
}
void JavaScriptParser::AnonymousFunctionExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAnonymousFunctionExpression(this);
}
//----------------- AwaitExpressionContext ------------------------------------------------------------------

tree::TerminalNode* JavaScriptParser::AwaitExpressionContext::AWAIT() {
  return getToken(JavaScriptParser::AWAIT, 0);
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::AwaitExpressionContext::singleExpression() {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(0);
}

JavaScriptParser::AwaitExpressionContext::AwaitExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::AwaitExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAwaitExpression(this);
}
void JavaScriptParser::AwaitExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAwaitExpression(this);
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
//----------------- UnaryMinusExpressionContext ------------------------------------------------------------------

tree::TerminalNode* JavaScriptParser::UnaryMinusExpressionContext::MINUS() {
  return getToken(JavaScriptParser::MINUS, 0);
}

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

tree::TerminalNode* JavaScriptParser::AssignmentExpressionContext::ASSIGN() {
  return getToken(JavaScriptParser::ASSIGN, 0);
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

tree::TerminalNode* JavaScriptParser::PostDecreaseExpressionContext::MINUSMINUS() {
  return getToken(JavaScriptParser::MINUSMINUS, 0);
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

tree::TerminalNode* JavaScriptParser::UnaryPlusExpressionContext::PLUS() {
  return getToken(JavaScriptParser::PLUS, 0);
}

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

tree::TerminalNode* JavaScriptParser::EqualityExpressionContext::EQ() {
  return getToken(JavaScriptParser::EQ, 0);
}

tree::TerminalNode* JavaScriptParser::EqualityExpressionContext::NE() {
  return getToken(JavaScriptParser::NE, 0);
}

tree::TerminalNode* JavaScriptParser::EqualityExpressionContext::SEQ() {
  return getToken(JavaScriptParser::SEQ, 0);
}

tree::TerminalNode* JavaScriptParser::EqualityExpressionContext::SNE() {
  return getToken(JavaScriptParser::SNE, 0);
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

tree::TerminalNode* JavaScriptParser::BitXOrExpressionContext::BITXOR() {
  return getToken(JavaScriptParser::BITXOR, 0);
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
//----------------- SuperExpressionContext ------------------------------------------------------------------

tree::TerminalNode* JavaScriptParser::SuperExpressionContext::SUPER() {
  return getToken(JavaScriptParser::SUPER, 0);
}

JavaScriptParser::SuperExpressionContext::SuperExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::SuperExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSuperExpression(this);
}
void JavaScriptParser::SuperExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSuperExpression(this);
}
//----------------- MultiplicativeExpressionContext ------------------------------------------------------------------

std::vector<JavaScriptParser::SingleExpressionContext *> JavaScriptParser::MultiplicativeExpressionContext::singleExpression() {
  return getRuleContexts<JavaScriptParser::SingleExpressionContext>();
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::MultiplicativeExpressionContext::singleExpression(size_t i) {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(i);
}

tree::TerminalNode* JavaScriptParser::MultiplicativeExpressionContext::STAR() {
  return getToken(JavaScriptParser::STAR, 0);
}

tree::TerminalNode* JavaScriptParser::MultiplicativeExpressionContext::SLASH() {
  return getToken(JavaScriptParser::SLASH, 0);
}

tree::TerminalNode* JavaScriptParser::MultiplicativeExpressionContext::MODULUS() {
  return getToken(JavaScriptParser::MODULUS, 0);
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

tree::TerminalNode* JavaScriptParser::BitShiftExpressionContext::LSHIFT() {
  return getToken(JavaScriptParser::LSHIFT, 0);
}

tree::TerminalNode* JavaScriptParser::BitShiftExpressionContext::RSHIFT() {
  return getToken(JavaScriptParser::RSHIFT, 0);
}

tree::TerminalNode* JavaScriptParser::BitShiftExpressionContext::URSHIFT() {
  return getToken(JavaScriptParser::URSHIFT, 0);
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

tree::TerminalNode* JavaScriptParser::ParenthesizedExpressionContext::LPAREN() {
  return getToken(JavaScriptParser::LPAREN, 0);
}

JavaScriptParser::ExpressionSequenceContext* JavaScriptParser::ParenthesizedExpressionContext::expressionSequence() {
  return getRuleContext<JavaScriptParser::ExpressionSequenceContext>(0);
}

tree::TerminalNode* JavaScriptParser::ParenthesizedExpressionContext::RPAREN() {
  return getToken(JavaScriptParser::RPAREN, 0);
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

tree::TerminalNode* JavaScriptParser::AdditiveExpressionContext::PLUS() {
  return getToken(JavaScriptParser::PLUS, 0);
}

tree::TerminalNode* JavaScriptParser::AdditiveExpressionContext::MINUS() {
  return getToken(JavaScriptParser::MINUS, 0);
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

tree::TerminalNode* JavaScriptParser::RelationalExpressionContext::LT() {
  return getToken(JavaScriptParser::LT, 0);
}

tree::TerminalNode* JavaScriptParser::RelationalExpressionContext::GT() {
  return getToken(JavaScriptParser::GT, 0);
}

tree::TerminalNode* JavaScriptParser::RelationalExpressionContext::LE() {
  return getToken(JavaScriptParser::LE, 0);
}

tree::TerminalNode* JavaScriptParser::RelationalExpressionContext::GE() {
  return getToken(JavaScriptParser::GE, 0);
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

tree::TerminalNode* JavaScriptParser::PostIncrementExpressionContext::PLUSPLUS() {
  return getToken(JavaScriptParser::PLUSPLUS, 0);
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

tree::TerminalNode* JavaScriptParser::BitNotExpressionContext::BITNOT() {
  return getToken(JavaScriptParser::BITNOT, 0);
}

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

tree::TerminalNode* JavaScriptParser::MemberDotExpressionContext::DOT() {
  return getToken(JavaScriptParser::DOT, 0);
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

tree::TerminalNode* JavaScriptParser::MemberIndexExpressionContext::LBRACKET() {
  return getToken(JavaScriptParser::LBRACKET, 0);
}

JavaScriptParser::ExpressionSequenceContext* JavaScriptParser::MemberIndexExpressionContext::expressionSequence() {
  return getRuleContext<JavaScriptParser::ExpressionSequenceContext>(0);
}

tree::TerminalNode* JavaScriptParser::MemberIndexExpressionContext::RBRACKET() {
  return getToken(JavaScriptParser::RBRACKET, 0);
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

tree::TerminalNode* JavaScriptParser::BitAndExpressionContext::BITAND() {
  return getToken(JavaScriptParser::BITAND, 0);
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

tree::TerminalNode* JavaScriptParser::BitOrExpressionContext::BITOR() {
  return getToken(JavaScriptParser::BITOR, 0);
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
//----------------- CoalesceExpressionContext ------------------------------------------------------------------

std::vector<JavaScriptParser::SingleExpressionContext *> JavaScriptParser::CoalesceExpressionContext::singleExpression() {
  return getRuleContexts<JavaScriptParser::SingleExpressionContext>();
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::CoalesceExpressionContext::singleExpression(size_t i) {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(i);
}

tree::TerminalNode* JavaScriptParser::CoalesceExpressionContext::NULLCOALESCE() {
  return getToken(JavaScriptParser::NULLCOALESCE, 0);
}

JavaScriptParser::CoalesceExpressionContext::CoalesceExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::CoalesceExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCoalesceExpression(this);
}
void JavaScriptParser::CoalesceExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCoalesceExpression(this);
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
  size_t startState = 82;
  enterRecursionRule(_localctx, 82, JavaScriptParser::RuleSingleExpression, precedence);

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
    setState(496);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 54, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<AnonymousFunctionExpressionContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;

      setState(460);
      anonymousFunction();
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<NewExpressionContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(461);
      match(JavaScriptParser::NEW);
      setState(462);
      singleExpression(0);
      setState(464);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 53, _ctx)) {
      case 1: {
        setState(463);
        arguments();
        break;
      }

      default:
        break;
      }
      break;
    }

    case 3: {
      _localctx = _tracker.createInstance<DeleteExpressionContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(466);
      match(JavaScriptParser::DELETE);
      setState(467);
      singleExpression(34);
      break;
    }

    case 4: {
      _localctx = _tracker.createInstance<VoidExpressionContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(468);
      match(JavaScriptParser::VOID);
      setState(469);
      singleExpression(33);
      break;
    }

    case 5: {
      _localctx = _tracker.createInstance<TypeofExpressionContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(470);
      match(JavaScriptParser::TYPEOF);
      setState(471);
      singleExpression(32);
      break;
    }

    case 6: {
      _localctx = _tracker.createInstance<PreIncrementExpressionContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(472);
      match(JavaScriptParser::PLUSPLUS);
      setState(473);
      singleExpression(31);
      break;
    }

    case 7: {
      _localctx = _tracker.createInstance<PreDecreaseExpressionContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(474);
      match(JavaScriptParser::MINUSMINUS);
      setState(475);
      singleExpression(30);
      break;
    }

    case 8: {
      _localctx = _tracker.createInstance<UnaryPlusExpressionContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(476);
      match(JavaScriptParser::PLUS);
      setState(477);
      singleExpression(29);
      break;
    }

    case 9: {
      _localctx = _tracker.createInstance<UnaryMinusExpressionContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(478);
      match(JavaScriptParser::MINUS);
      setState(479);
      singleExpression(28);
      break;
    }

    case 10: {
      _localctx = _tracker.createInstance<BitNotExpressionContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(480);
      match(JavaScriptParser::BITNOT);
      setState(481);
      singleExpression(27);
      break;
    }

    case 11: {
      _localctx = _tracker.createInstance<NotExpressionContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(482);
      match(JavaScriptParser::NOT);
      setState(483);
      singleExpression(26);
      break;
    }

    case 12: {
      _localctx = _tracker.createInstance<AwaitExpressionContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(484);
      match(JavaScriptParser::AWAIT);
      setState(485);
      singleExpression(25);
      break;
    }

    case 13: {
      _localctx = _tracker.createInstance<ThisExpressionContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(486);
      match(JavaScriptParser::THIS);
      break;
    }

    case 14: {
      _localctx = _tracker.createInstance<IdentifierExpressionContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(487);
      match(JavaScriptParser::Identifier);
      break;
    }

    case 15: {
      _localctx = _tracker.createInstance<SuperExpressionContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(488);
      match(JavaScriptParser::SUPER);
      break;
    }

    case 16: {
      _localctx = _tracker.createInstance<LiteralExpressionContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(489);
      literal();
      break;
    }

    case 17: {
      _localctx = _tracker.createInstance<ArrayLiteralExpressionContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(490);
      arrayLiteral();
      break;
    }

    case 18: {
      _localctx = _tracker.createInstance<ObjectLiteralExpressionContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(491);
      objectLiteral();
      break;
    }

    case 19: {
      _localctx = _tracker.createInstance<ParenthesizedExpressionContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(492);
      match(JavaScriptParser::LPAREN);
      setState(493);
      expressionSequence();
      setState(494);
      match(JavaScriptParser::RPAREN);
      break;
    }

    default:
      break;
    }
    _ctx->stop = _input->LT(-1);
    setState(569);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 56, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(567);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 55, _ctx)) {
        case 1: {
          auto newContext = _tracker.createInstance<PowerExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(498);

          if (!(precpred(_ctx, 24))) throw FailedPredicateException(this, "precpred(_ctx, 24)");
          setState(499);
          match(JavaScriptParser::POWER);
          setState(500);
          singleExpression(25);
          break;
        }

        case 2: {
          auto newContext = _tracker.createInstance<MultiplicativeExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(501);

          if (!(precpred(_ctx, 23))) throw FailedPredicateException(this, "precpred(_ctx, 23)");
          setState(502);
          _la = _input->LA(1);
          if (!(((((_la - 64) & ~ 0x3fULL) == 0) &&
            ((1ULL << (_la - 64)) & 7) != 0))) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(503);
          singleExpression(24);
          break;
        }

        case 3: {
          auto newContext = _tracker.createInstance<AdditiveExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(504);

          if (!(precpred(_ctx, 22))) throw FailedPredicateException(this, "precpred(_ctx, 22)");
          setState(505);
          _la = _input->LA(1);
          if (!(_la == JavaScriptParser::PLUS

          || _la == JavaScriptParser::MINUS)) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(506);
          singleExpression(23);
          break;
        }

        case 4: {
          auto newContext = _tracker.createInstance<BitShiftExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(507);

          if (!(precpred(_ctx, 21))) throw FailedPredicateException(this, "precpred(_ctx, 21)");
          setState(508);
          _la = _input->LA(1);
          if (!(((((_la - 69) & ~ 0x3fULL) == 0) &&
            ((1ULL << (_la - 69)) & 7) != 0))) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(509);
          singleExpression(22);
          break;
        }

        case 5: {
          auto newContext = _tracker.createInstance<RelationalExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(510);

          if (!(precpred(_ctx, 20))) throw FailedPredicateException(this, "precpred(_ctx, 20)");
          setState(511);
          _la = _input->LA(1);
          if (!(((((_la - 72) & ~ 0x3fULL) == 0) &&
            ((1ULL << (_la - 72)) & 15) != 0))) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(512);
          singleExpression(21);
          break;
        }

        case 6: {
          auto newContext = _tracker.createInstance<InstanceofExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(513);

          if (!(precpred(_ctx, 19))) throw FailedPredicateException(this, "precpred(_ctx, 19)");
          setState(514);
          match(JavaScriptParser::INSTANCEOF);
          setState(515);
          singleExpression(20);
          break;
        }

        case 7: {
          auto newContext = _tracker.createInstance<InExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(516);

          if (!(precpred(_ctx, 18))) throw FailedPredicateException(this, "precpred(_ctx, 18)");
          setState(517);
          match(JavaScriptParser::IN);
          setState(518);
          singleExpression(19);
          break;
        }

        case 8: {
          auto newContext = _tracker.createInstance<EqualityExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(519);

          if (!(precpred(_ctx, 17))) throw FailedPredicateException(this, "precpred(_ctx, 17)");
          setState(520);
          _la = _input->LA(1);
          if (!(((((_la - 76) & ~ 0x3fULL) == 0) &&
            ((1ULL << (_la - 76)) & 15) != 0))) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(521);
          singleExpression(18);
          break;
        }

        case 9: {
          auto newContext = _tracker.createInstance<BitAndExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(522);

          if (!(precpred(_ctx, 16))) throw FailedPredicateException(this, "precpred(_ctx, 16)");
          setState(523);
          match(JavaScriptParser::BITAND);
          setState(524);
          singleExpression(17);
          break;
        }

        case 10: {
          auto newContext = _tracker.createInstance<BitXOrExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(525);

          if (!(precpred(_ctx, 15))) throw FailedPredicateException(this, "precpred(_ctx, 15)");
          setState(526);
          match(JavaScriptParser::BITXOR);
          setState(527);
          singleExpression(16);
          break;
        }

        case 11: {
          auto newContext = _tracker.createInstance<BitOrExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(528);

          if (!(precpred(_ctx, 14))) throw FailedPredicateException(this, "precpred(_ctx, 14)");
          setState(529);
          match(JavaScriptParser::BITOR);
          setState(530);
          singleExpression(15);
          break;
        }

        case 12: {
          auto newContext = _tracker.createInstance<LogicalAndExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(531);

          if (!(precpred(_ctx, 13))) throw FailedPredicateException(this, "precpred(_ctx, 13)");
          setState(532);
          match(JavaScriptParser::AND);
          setState(533);
          singleExpression(14);
          break;
        }

        case 13: {
          auto newContext = _tracker.createInstance<LogicalOrExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(534);

          if (!(precpred(_ctx, 12))) throw FailedPredicateException(this, "precpred(_ctx, 12)");
          setState(535);
          match(JavaScriptParser::OR);
          setState(536);
          singleExpression(13);
          break;
        }

        case 14: {
          auto newContext = _tracker.createInstance<CoalesceExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(537);

          if (!(precpred(_ctx, 11))) throw FailedPredicateException(this, "precpred(_ctx, 11)");
          setState(538);
          match(JavaScriptParser::NULLCOALESCE);
          setState(539);
          singleExpression(12);
          break;
        }

        case 15: {
          auto newContext = _tracker.createInstance<TernaryExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(540);

          if (!(precpred(_ctx, 10))) throw FailedPredicateException(this, "precpred(_ctx, 10)");
          setState(541);
          match(JavaScriptParser::QUESTION);
          setState(542);
          singleExpression(0);
          setState(543);
          match(JavaScriptParser::COLON);
          setState(544);
          singleExpression(11);
          break;
        }

        case 16: {
          auto newContext = _tracker.createInstance<AssignmentExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(546);

          if (!(precpred(_ctx, 9))) throw FailedPredicateException(this, "precpred(_ctx, 9)");
          setState(547);
          match(JavaScriptParser::ASSIGN);
          setState(548);
          singleExpression(10);
          break;
        }

        case 17: {
          auto newContext = _tracker.createInstance<AssignmentOperatorExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(549);

          if (!(precpred(_ctx, 8))) throw FailedPredicateException(this, "precpred(_ctx, 8)");
          setState(550);
          assignmentOperator();
          setState(551);
          singleExpression(9);
          break;
        }

        case 18: {
          auto newContext = _tracker.createInstance<MemberIndexExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(553);

          if (!(precpred(_ctx, 40))) throw FailedPredicateException(this, "precpred(_ctx, 40)");
          setState(554);
          match(JavaScriptParser::LBRACKET);
          setState(555);
          expressionSequence();
          setState(556);
          match(JavaScriptParser::RBRACKET);
          break;
        }

        case 19: {
          auto newContext = _tracker.createInstance<MemberDotExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(558);

          if (!(precpred(_ctx, 39))) throw FailedPredicateException(this, "precpred(_ctx, 39)");
          setState(559);
          match(JavaScriptParser::DOT);
          setState(560);
          identifierName();
          break;
        }

        case 20: {
          auto newContext = _tracker.createInstance<CallExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(561);

          if (!(precpred(_ctx, 38))) throw FailedPredicateException(this, "precpred(_ctx, 38)");
          setState(562);
          arguments();
          break;
        }

        case 21: {
          auto newContext = _tracker.createInstance<PostIncrementExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(563);

          if (!(precpred(_ctx, 36))) throw FailedPredicateException(this, "precpred(_ctx, 36)");
          setState(564);
          match(JavaScriptParser::PLUSPLUS);
          break;
        }

        case 22: {
          auto newContext = _tracker.createInstance<PostDecreaseExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(565);

          if (!(precpred(_ctx, 35))) throw FailedPredicateException(this, "precpred(_ctx, 35)");
          setState(566);
          match(JavaScriptParser::MINUSMINUS);
          break;
        }

        default:
          break;
        } 
      }
      setState(571);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 56, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- AnonymousFunctionContext ------------------------------------------------------------------

JavaScriptParser::AnonymousFunctionContext::AnonymousFunctionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::AnonymousFunctionContext::FUNCTION() {
  return getToken(JavaScriptParser::FUNCTION, 0);
}

tree::TerminalNode* JavaScriptParser::AnonymousFunctionContext::LPAREN() {
  return getToken(JavaScriptParser::LPAREN, 0);
}

tree::TerminalNode* JavaScriptParser::AnonymousFunctionContext::RPAREN() {
  return getToken(JavaScriptParser::RPAREN, 0);
}

JavaScriptParser::FunctionBodyContext* JavaScriptParser::AnonymousFunctionContext::functionBody() {
  return getRuleContext<JavaScriptParser::FunctionBodyContext>(0);
}

tree::TerminalNode* JavaScriptParser::AnonymousFunctionContext::ASYNC() {
  return getToken(JavaScriptParser::ASYNC, 0);
}

tree::TerminalNode* JavaScriptParser::AnonymousFunctionContext::STAR() {
  return getToken(JavaScriptParser::STAR, 0);
}

JavaScriptParser::FormalParameterListContext* JavaScriptParser::AnonymousFunctionContext::formalParameterList() {
  return getRuleContext<JavaScriptParser::FormalParameterListContext>(0);
}

JavaScriptParser::ArrowFunctionParametersContext* JavaScriptParser::AnonymousFunctionContext::arrowFunctionParameters() {
  return getRuleContext<JavaScriptParser::ArrowFunctionParametersContext>(0);
}

tree::TerminalNode* JavaScriptParser::AnonymousFunctionContext::ARROW() {
  return getToken(JavaScriptParser::ARROW, 0);
}

JavaScriptParser::ArrowFunctionBodyContext* JavaScriptParser::AnonymousFunctionContext::arrowFunctionBody() {
  return getRuleContext<JavaScriptParser::ArrowFunctionBodyContext>(0);
}


size_t JavaScriptParser::AnonymousFunctionContext::getRuleIndex() const {
  return JavaScriptParser::RuleAnonymousFunction;
}

void JavaScriptParser::AnonymousFunctionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAnonymousFunction(this);
}

void JavaScriptParser::AnonymousFunctionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAnonymousFunction(this);
}

JavaScriptParser::AnonymousFunctionContext* JavaScriptParser::anonymousFunction() {
  AnonymousFunctionContext *_localctx = _tracker.createInstance<AnonymousFunctionContext>(_ctx, getState());
  enterRule(_localctx, 84, JavaScriptParser::RuleAnonymousFunction);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(592);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 61, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(573);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == JavaScriptParser::ASYNC) {
        setState(572);
        match(JavaScriptParser::ASYNC);
      }
      setState(575);
      match(JavaScriptParser::FUNCTION);
      setState(577);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == JavaScriptParser::STAR) {
        setState(576);
        match(JavaScriptParser::STAR);
      }
      setState(579);
      match(JavaScriptParser::LPAREN);
      setState(581);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == JavaScriptParser::Identifier) {
        setState(580);
        formalParameterList();
      }
      setState(583);
      match(JavaScriptParser::RPAREN);
      setState(584);
      functionBody();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(586);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == JavaScriptParser::ASYNC) {
        setState(585);
        match(JavaScriptParser::ASYNC);
      }
      setState(588);
      arrowFunctionParameters();
      setState(589);
      match(JavaScriptParser::ARROW);
      setState(590);
      arrowFunctionBody();
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

//----------------- ArrowFunctionParametersContext ------------------------------------------------------------------

JavaScriptParser::ArrowFunctionParametersContext::ArrowFunctionParametersContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::ArrowFunctionParametersContext::Identifier() {
  return getToken(JavaScriptParser::Identifier, 0);
}

tree::TerminalNode* JavaScriptParser::ArrowFunctionParametersContext::LPAREN() {
  return getToken(JavaScriptParser::LPAREN, 0);
}

tree::TerminalNode* JavaScriptParser::ArrowFunctionParametersContext::RPAREN() {
  return getToken(JavaScriptParser::RPAREN, 0);
}

JavaScriptParser::FormalParameterListContext* JavaScriptParser::ArrowFunctionParametersContext::formalParameterList() {
  return getRuleContext<JavaScriptParser::FormalParameterListContext>(0);
}


size_t JavaScriptParser::ArrowFunctionParametersContext::getRuleIndex() const {
  return JavaScriptParser::RuleArrowFunctionParameters;
}

void JavaScriptParser::ArrowFunctionParametersContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterArrowFunctionParameters(this);
}

void JavaScriptParser::ArrowFunctionParametersContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitArrowFunctionParameters(this);
}

JavaScriptParser::ArrowFunctionParametersContext* JavaScriptParser::arrowFunctionParameters() {
  ArrowFunctionParametersContext *_localctx = _tracker.createInstance<ArrowFunctionParametersContext>(_ctx, getState());
  enterRule(_localctx, 86, JavaScriptParser::RuleArrowFunctionParameters);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(600);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case JavaScriptParser::Identifier: {
        enterOuterAlt(_localctx, 1);
        setState(594);
        match(JavaScriptParser::Identifier);
        break;
      }

      case JavaScriptParser::LPAREN: {
        enterOuterAlt(_localctx, 2);
        setState(595);
        match(JavaScriptParser::LPAREN);
        setState(597);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == JavaScriptParser::Identifier) {
          setState(596);
          formalParameterList();
        }
        setState(599);
        match(JavaScriptParser::RPAREN);
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

//----------------- ArrowFunctionBodyContext ------------------------------------------------------------------

JavaScriptParser::ArrowFunctionBodyContext::ArrowFunctionBodyContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::ArrowFunctionBodyContext::singleExpression() {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(0);
}

JavaScriptParser::FunctionBodyContext* JavaScriptParser::ArrowFunctionBodyContext::functionBody() {
  return getRuleContext<JavaScriptParser::FunctionBodyContext>(0);
}


size_t JavaScriptParser::ArrowFunctionBodyContext::getRuleIndex() const {
  return JavaScriptParser::RuleArrowFunctionBody;
}

void JavaScriptParser::ArrowFunctionBodyContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterArrowFunctionBody(this);
}

void JavaScriptParser::ArrowFunctionBodyContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitArrowFunctionBody(this);
}

JavaScriptParser::ArrowFunctionBodyContext* JavaScriptParser::arrowFunctionBody() {
  ArrowFunctionBodyContext *_localctx = _tracker.createInstance<ArrowFunctionBodyContext>(_ctx, getState());
  enterRule(_localctx, 88, JavaScriptParser::RuleArrowFunctionBody);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(604);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 64, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(602);
      singleExpression(0);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(603);
      functionBody();
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

//----------------- AssignmentOperatorContext ------------------------------------------------------------------

JavaScriptParser::AssignmentOperatorContext::AssignmentOperatorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::AssignmentOperatorContext::STARASSIGN() {
  return getToken(JavaScriptParser::STARASSIGN, 0);
}

tree::TerminalNode* JavaScriptParser::AssignmentOperatorContext::SLASHASSIGN() {
  return getToken(JavaScriptParser::SLASHASSIGN, 0);
}

tree::TerminalNode* JavaScriptParser::AssignmentOperatorContext::MODULUSASSIGN() {
  return getToken(JavaScriptParser::MODULUSASSIGN, 0);
}

tree::TerminalNode* JavaScriptParser::AssignmentOperatorContext::PLUSASSIGN() {
  return getToken(JavaScriptParser::PLUSASSIGN, 0);
}

tree::TerminalNode* JavaScriptParser::AssignmentOperatorContext::MINUSASSIGN() {
  return getToken(JavaScriptParser::MINUSASSIGN, 0);
}

tree::TerminalNode* JavaScriptParser::AssignmentOperatorContext::LSHIFTASSIGN() {
  return getToken(JavaScriptParser::LSHIFTASSIGN, 0);
}

tree::TerminalNode* JavaScriptParser::AssignmentOperatorContext::RSHIFTASSIGN() {
  return getToken(JavaScriptParser::RSHIFTASSIGN, 0);
}

tree::TerminalNode* JavaScriptParser::AssignmentOperatorContext::URSHIFTASSIGN() {
  return getToken(JavaScriptParser::URSHIFTASSIGN, 0);
}

tree::TerminalNode* JavaScriptParser::AssignmentOperatorContext::BITANDASSIGN() {
  return getToken(JavaScriptParser::BITANDASSIGN, 0);
}

tree::TerminalNode* JavaScriptParser::AssignmentOperatorContext::BITXORASSIGN() {
  return getToken(JavaScriptParser::BITXORASSIGN, 0);
}

tree::TerminalNode* JavaScriptParser::AssignmentOperatorContext::BITORASSIGN() {
  return getToken(JavaScriptParser::BITORASSIGN, 0);
}

tree::TerminalNode* JavaScriptParser::AssignmentOperatorContext::POWERASSIGN() {
  return getToken(JavaScriptParser::POWERASSIGN, 0);
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
  enterRule(_localctx, 90, JavaScriptParser::RuleAssignmentOperator);
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
    setState(606);
    _la = _input->LA(1);
    if (!(((((_la - 85) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 85)) & 4095) != 0))) {
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

tree::TerminalNode* JavaScriptParser::LiteralContext::TemplateStringLiteral() {
  return getToken(JavaScriptParser::TemplateStringLiteral, 0);
}

tree::TerminalNode* JavaScriptParser::LiteralContext::RegularExpressionLiteral() {
  return getToken(JavaScriptParser::RegularExpressionLiteral, 0);
}

JavaScriptParser::NumericLiteralContext* JavaScriptParser::LiteralContext::numericLiteral() {
  return getRuleContext<JavaScriptParser::NumericLiteralContext>(0);
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
  enterRule(_localctx, 92, JavaScriptParser::RuleLiteral);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(614);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case JavaScriptParser::NullLiteral: {
        enterOuterAlt(_localctx, 1);
        setState(608);
        match(JavaScriptParser::NullLiteral);
        break;
      }

      case JavaScriptParser::BooleanLiteral: {
        enterOuterAlt(_localctx, 2);
        setState(609);
        match(JavaScriptParser::BooleanLiteral);
        break;
      }

      case JavaScriptParser::StringLiteral: {
        enterOuterAlt(_localctx, 3);
        setState(610);
        match(JavaScriptParser::StringLiteral);
        break;
      }

      case JavaScriptParser::TemplateStringLiteral: {
        enterOuterAlt(_localctx, 4);
        setState(611);
        match(JavaScriptParser::TemplateStringLiteral);
        break;
      }

      case JavaScriptParser::RegularExpressionLiteral: {
        enterOuterAlt(_localctx, 5);
        setState(612);
        match(JavaScriptParser::RegularExpressionLiteral);
        break;
      }

      case JavaScriptParser::DecimalLiteral:
      case JavaScriptParser::HexIntegerLiteral:
      case JavaScriptParser::OctalIntegerLiteral:
      case JavaScriptParser::BinaryIntegerLiteral: {
        enterOuterAlt(_localctx, 6);
        setState(613);
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

tree::TerminalNode* JavaScriptParser::NumericLiteralContext::BinaryIntegerLiteral() {
  return getToken(JavaScriptParser::BinaryIntegerLiteral, 0);
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
  enterRule(_localctx, 94, JavaScriptParser::RuleNumericLiteral);
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
    setState(616);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 263882790666240) != 0))) {
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

//----------------- GetterContext ------------------------------------------------------------------

JavaScriptParser::GetterContext::GetterContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::GetterContext::GET() {
  return getToken(JavaScriptParser::GET, 0);
}

JavaScriptParser::PropertyNameContext* JavaScriptParser::GetterContext::propertyName() {
  return getRuleContext<JavaScriptParser::PropertyNameContext>(0);
}


size_t JavaScriptParser::GetterContext::getRuleIndex() const {
  return JavaScriptParser::RuleGetter;
}

void JavaScriptParser::GetterContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterGetter(this);
}

void JavaScriptParser::GetterContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitGetter(this);
}

JavaScriptParser::GetterContext* JavaScriptParser::getter() {
  GetterContext *_localctx = _tracker.createInstance<GetterContext>(_ctx, getState());
  enterRule(_localctx, 96, JavaScriptParser::RuleGetter);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(618);
    match(JavaScriptParser::GET);
    setState(619);
    propertyName();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- SetterContext ------------------------------------------------------------------

JavaScriptParser::SetterContext::SetterContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::SetterContext::SET() {
  return getToken(JavaScriptParser::SET, 0);
}

JavaScriptParser::PropertyNameContext* JavaScriptParser::SetterContext::propertyName() {
  return getRuleContext<JavaScriptParser::PropertyNameContext>(0);
}


size_t JavaScriptParser::SetterContext::getRuleIndex() const {
  return JavaScriptParser::RuleSetter;
}

void JavaScriptParser::SetterContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSetter(this);
}

void JavaScriptParser::SetterContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSetter(this);
}

JavaScriptParser::SetterContext* JavaScriptParser::setter() {
  SetterContext *_localctx = _tracker.createInstance<SetterContext>(_ctx, getState());
  enterRule(_localctx, 98, JavaScriptParser::RuleSetter);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(621);
    match(JavaScriptParser::SET);
    setState(622);
    propertyName();
   
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
  enterRule(_localctx, 100, JavaScriptParser::RuleIdentifierName);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(626);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case JavaScriptParser::Identifier: {
        enterOuterAlt(_localctx, 1);
        setState(624);
        match(JavaScriptParser::Identifier);
        break;
      }

      case JavaScriptParser::BREAK:
      case JavaScriptParser::CASE:
      case JavaScriptParser::CATCH:
      case JavaScriptParser::CLASS:
      case JavaScriptParser::CONST:
      case JavaScriptParser::CONTINUE:
      case JavaScriptParser::DEBUGGER:
      case JavaScriptParser::DEFAULT:
      case JavaScriptParser::DELETE:
      case JavaScriptParser::DO:
      case JavaScriptParser::ELSE:
      case JavaScriptParser::ENUM:
      case JavaScriptParser::EXPORT:
      case JavaScriptParser::EXTENDS:
      case JavaScriptParser::FINALLY:
      case JavaScriptParser::FOR:
      case JavaScriptParser::FUNCTION:
      case JavaScriptParser::IF:
      case JavaScriptParser::IMPORT:
      case JavaScriptParser::IN:
      case JavaScriptParser::INSTANCEOF:
      case JavaScriptParser::NEW:
      case JavaScriptParser::RETURN:
      case JavaScriptParser::SUPER:
      case JavaScriptParser::SWITCH:
      case JavaScriptParser::THIS:
      case JavaScriptParser::THROW:
      case JavaScriptParser::TRY:
      case JavaScriptParser::TYPEOF:
      case JavaScriptParser::VAR:
      case JavaScriptParser::VOID:
      case JavaScriptParser::WHILE:
      case JavaScriptParser::WITH:
      case JavaScriptParser::YIELD:
      case JavaScriptParser::ASYNC:
      case JavaScriptParser::AWAIT:
      case JavaScriptParser::LET:
      case JavaScriptParser::OF:
      case JavaScriptParser::STATIC:
      case JavaScriptParser::NullLiteral:
      case JavaScriptParser::BooleanLiteral: {
        enterOuterAlt(_localctx, 2);
        setState(625);
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

tree::TerminalNode* JavaScriptParser::ReservedWordContext::NullLiteral() {
  return getToken(JavaScriptParser::NullLiteral, 0);
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
  enterRule(_localctx, 102, JavaScriptParser::RuleReservedWord);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(631);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case JavaScriptParser::BREAK:
      case JavaScriptParser::CASE:
      case JavaScriptParser::CATCH:
      case JavaScriptParser::CLASS:
      case JavaScriptParser::CONST:
      case JavaScriptParser::CONTINUE:
      case JavaScriptParser::DEBUGGER:
      case JavaScriptParser::DEFAULT:
      case JavaScriptParser::DELETE:
      case JavaScriptParser::DO:
      case JavaScriptParser::ELSE:
      case JavaScriptParser::ENUM:
      case JavaScriptParser::EXPORT:
      case JavaScriptParser::EXTENDS:
      case JavaScriptParser::FINALLY:
      case JavaScriptParser::FOR:
      case JavaScriptParser::FUNCTION:
      case JavaScriptParser::IF:
      case JavaScriptParser::IMPORT:
      case JavaScriptParser::IN:
      case JavaScriptParser::INSTANCEOF:
      case JavaScriptParser::NEW:
      case JavaScriptParser::RETURN:
      case JavaScriptParser::SUPER:
      case JavaScriptParser::SWITCH:
      case JavaScriptParser::THIS:
      case JavaScriptParser::THROW:
      case JavaScriptParser::TRY:
      case JavaScriptParser::TYPEOF:
      case JavaScriptParser::VAR:
      case JavaScriptParser::VOID:
      case JavaScriptParser::WHILE:
      case JavaScriptParser::WITH:
      case JavaScriptParser::YIELD:
      case JavaScriptParser::ASYNC:
      case JavaScriptParser::AWAIT:
      case JavaScriptParser::LET:
      case JavaScriptParser::OF:
      case JavaScriptParser::STATIC: {
        enterOuterAlt(_localctx, 1);
        setState(628);
        keyword();
        break;
      }

      case JavaScriptParser::NullLiteral: {
        enterOuterAlt(_localctx, 2);
        setState(629);
        match(JavaScriptParser::NullLiteral);
        break;
      }

      case JavaScriptParser::BooleanLiteral: {
        enterOuterAlt(_localctx, 3);
        setState(630);
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

tree::TerminalNode* JavaScriptParser::KeywordContext::CLASS() {
  return getToken(JavaScriptParser::CLASS, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::CONST() {
  return getToken(JavaScriptParser::CONST, 0);
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

tree::TerminalNode* JavaScriptParser::KeywordContext::ENUM() {
  return getToken(JavaScriptParser::ENUM, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::EXPORT() {
  return getToken(JavaScriptParser::EXPORT, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::EXTENDS() {
  return getToken(JavaScriptParser::EXTENDS, 0);
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

tree::TerminalNode* JavaScriptParser::KeywordContext::IMPORT() {
  return getToken(JavaScriptParser::IMPORT, 0);
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

tree::TerminalNode* JavaScriptParser::KeywordContext::SUPER() {
  return getToken(JavaScriptParser::SUPER, 0);
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

tree::TerminalNode* JavaScriptParser::KeywordContext::YIELD() {
  return getToken(JavaScriptParser::YIELD, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::ASYNC() {
  return getToken(JavaScriptParser::ASYNC, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::AWAIT() {
  return getToken(JavaScriptParser::AWAIT, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::LET() {
  return getToken(JavaScriptParser::LET, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::OF() {
  return getToken(JavaScriptParser::OF, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::STATIC() {
  return getToken(JavaScriptParser::STATIC, 0);
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
  enterRule(_localctx, 104, JavaScriptParser::RuleKeyword);
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
    setState(633);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 1099511627774) != 0))) {
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

bool JavaScriptParser::sempred(RuleContext *context, size_t ruleIndex, size_t predicateIndex) {
  switch (ruleIndex) {
    case 41: return singleExpressionSempred(antlrcpp::downCast<SingleExpressionContext *>(context), predicateIndex);

  default:
    break;
  }
  return true;
}

bool JavaScriptParser::singleExpressionSempred(SingleExpressionContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 0: return precpred(_ctx, 24);
    case 1: return precpred(_ctx, 23);
    case 2: return precpred(_ctx, 22);
    case 3: return precpred(_ctx, 21);
    case 4: return precpred(_ctx, 20);
    case 5: return precpred(_ctx, 19);
    case 6: return precpred(_ctx, 18);
    case 7: return precpred(_ctx, 17);
    case 8: return precpred(_ctx, 16);
    case 9: return precpred(_ctx, 15);
    case 10: return precpred(_ctx, 14);
    case 11: return precpred(_ctx, 13);
    case 12: return precpred(_ctx, 12);
    case 13: return precpred(_ctx, 11);
    case 14: return precpred(_ctx, 10);
    case 15: return precpred(_ctx, 9);
    case 16: return precpred(_ctx, 8);
    case 17: return precpred(_ctx, 40);
    case 18: return precpred(_ctx, 39);
    case 19: return precpred(_ctx, 38);
    case 20: return precpred(_ctx, 36);
    case 21: return precpred(_ctx, 35);

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
