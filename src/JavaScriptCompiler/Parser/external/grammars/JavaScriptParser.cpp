
// Generated from external/grammars/JavaScriptParser.g4 by ANTLR 4.13.2


#include "JavaScriptParserListener.h"

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

::antlr4::internal::OnceFlag javascriptparserParserOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
std::unique_ptr<JavaScriptParserStaticData> javascriptparserParserStaticData = nullptr;

void javascriptparserParserInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (javascriptparserParserStaticData != nullptr) {
    return;
  }
#else
  assert(javascriptparserParserStaticData == nullptr);
#endif
  auto staticData = std::make_unique<JavaScriptParserStaticData>(
    std::vector<std::string>{
      "program", "sourceElements", "sourceElement", "statement", "block", 
      "statementList", "variableStatement", "variableDeclarationList", "variableDeclaration", 
      "initializer", "emptyStatement", "expressionStatement", "ifStatement", 
      "iterationStatement", "continueStatement", "breakStatement", "returnStatement", 
      "withStatement", "throwStatement", "tryStatement", "catchProduction", 
      "finallyProduction", "functionDeclaration", "formalParameterList", 
      "functionBody", "singleExpression", "assignmentOperator", "literal", 
      "numericLiteral", "identifierName", "reservedWord", "keyword", "arrayLiteral", 
      "elementList", "objectLiteral", "propertyNameAndValueList", "propertyAssignment", 
      "propertyName", "propertySetParameterList", "arguments", "argumentList", 
      "expressionSequence"
    },
    std::vector<std::string>{
      "", "", "", "", "", "'['", "']'", "'('", "')'", "'{'", "", "'}'", 
      "';'", "','", "'='", "'\\u003F'", "'\\u003F.'", "':'", "'...'", "'.'", 
      "'++'", "'--'", "'+'", "'-'", "'~'", "'!'", "'*'", "'/'", "'%'", "'**'", 
      "'\\u003F\\u003F'", "'#'", "'>>'", "'<<'", "'>>>'", "'<'", "'>'", 
      "'<='", "'>='", "'=='", "'!='", "'==='", "'!=='", "'&'", "'^'", "'|'", 
      "'&&'", "'||'", "'*='", "'/='", "'%='", "'+='", "'-='", "'<<='", "'>>='", 
      "'>>>='", "'&='", "'^='", "'|='", "'**='", "'\\u003F\\u003F='", "'=>'", 
      "'null'", "", "", "", "", "", "", "", "", "", "", "'break'", "'do'", 
      "'instanceof'", "'typeof'", "'case'", "'else'", "'new'", "'var'", 
      "'catch'", "'finally'", "'return'", "'void'", "'continue'", "'for'", 
      "'switch'", "'while'", "'debugger'", "'function'", "'this'", "'with'", 
      "'default'", "'if'", "'throw'", "'delete'", "'in'", "'try'", "'as'", 
      "'from'", "'of'", "'yield'", "'yield*'", "'class'", "'enum'", "'extends'", 
      "'super'", "'const'", "'export'", "'import'", "'async'", "'await'", 
      "'implements'", "", "", "'private'", "'public'", "'interface'", "'package'", 
      "'protected'", "'static'"
    },
    std::vector<std::string>{
      "", "HashBangLine", "MultiLineComment", "SingleLineComment", "RegularExpressionLiteral", 
      "OpenBracket", "CloseBracket", "OpenParen", "CloseParen", "OpenBrace", 
      "TemplateCloseBrace", "CloseBrace", "SemiColon", "Comma", "Assign", 
      "QuestionMark", "QuestionMarkDot", "Colon", "Ellipsis", "Dot", "PlusPlus", 
      "MinusMinus", "Plus", "Minus", "BitNot", "Not", "Multiply", "Divide", 
      "Modulus", "Power", "NullCoalesce", "Hashtag", "RightShiftArithmetic", 
      "LeftShiftArithmetic", "RightShiftLogical", "LessThan", "MoreThan", 
      "LessThanEquals", "GreaterThanEquals", "Equals_", "NotEquals", "IdentityEquals", 
      "IdentityNotEquals", "BitAnd", "BitXOr", "BitOr", "And", "Or", "MultiplyAssign", 
      "DivideAssign", "ModulusAssign", "PlusAssign", "MinusAssign", "LeftShiftArithmeticAssign", 
      "RightShiftArithmeticAssign", "RightShiftLogicalAssign", "BitAndAssign", 
      "BitXorAssign", "BitOrAssign", "PowerAssign", "NullishCoalescingAssign", 
      "ARROW", "NullLiteral", "BooleanLiteral", "DecimalLiteral", "HexIntegerLiteral", 
      "OctalIntegerLiteral", "OctalIntegerLiteral2", "BinaryIntegerLiteral", 
      "BigHexIntegerLiteral", "BigOctalIntegerLiteral", "BigBinaryIntegerLiteral", 
      "BigDecimalIntegerLiteral", "Break", "Do", "Instanceof", "Typeof", 
      "Case", "Else", "New", "Var", "Catch", "Finally", "Return", "Void", 
      "Continue", "For", "Switch", "While", "Debugger", "Function_", "This", 
      "With", "Default", "If", "Throw", "Delete", "In", "Try", "As", "From", 
      "Of", "Yield", "YieldStar", "Class", "Enum", "Extends", "Super", "Const", 
      "Export", "Import", "Async", "Await", "Implements", "StrictLet", "NonStrictLet", 
      "Private", "Public", "Interface", "Package", "Protected", "Static", 
      "Identifier", "StringLiteral", "BackTick", "WhiteSpaces", "LineTerminator", 
      "HtmlComment", "CDataComment", "UnexpectedCharacter", "TemplateStringStartExpression", 
      "TemplateStringAtom", "LBRACE", "RBRACE", "VAR", "SEMICOLON", "COMMA", 
      "ASSIGN", "IF", "LPAREN", "RPAREN", "ELSE", "DO", "WHILE", "FOR", 
      "IN", "CONTINUE", "BREAK", "RETURN", "WITH", "THROW", "TRY", "CATCH", 
      "FINALLY", "FUNCTION", "NEW", "DELETE", "VOID", "TYPEOF", "PLUS_PLUS", 
      "MINUS_MINUS", "PLUS", "MINUS", "TILDE", "NOT", "THIS", "MULTIPLY", 
      "DIVIDE", "MODULO", "LSHIFT", "RSHIFT", "URSHIFT", "LT", "GT", "LE", 
      "GE", "INSTANCEOF", "EQ", "NE", "STRICT_EQ", "STRICT_NE", "BIT_AND", 
      "BIT_XOR", "BIT_OR", "AND", "OR", "QUESTION", "COLON", "LBRACKET", 
      "RBRACKET", "DOT", "MULTIPLY_ASSIGN", "DIVIDE_ASSIGN", "MODULO_ASSIGN", 
      "PLUS_ASSIGN", "MINUS_ASSIGN", "LSHIFT_ASSIGN", "RSHIFT_ASSIGN", "URSHIFT_ASSIGN", 
      "BIT_AND_ASSIGN", "BIT_XOR_ASSIGN", "BIT_OR_ASSIGN", "NULL", "CASE", 
      "SWITCH", "DEBUGGER", "DEFAULT", "GET", "SET"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,208,510,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,27,7,27,2,28,7,
  	28,2,29,7,29,2,30,7,30,2,31,7,31,2,32,7,32,2,33,7,33,2,34,7,34,2,35,7,
  	35,2,36,7,36,2,37,7,37,2,38,7,38,2,39,7,39,2,40,7,40,2,41,7,41,1,0,3,
  	0,86,8,0,1,0,1,0,1,1,4,1,91,8,1,11,1,12,1,92,1,2,1,2,3,2,97,8,2,1,3,1,
  	3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,3,3,111,8,3,1,4,1,4,3,4,115,
  	8,4,1,4,1,4,1,5,4,5,120,8,5,11,5,12,5,121,1,6,1,6,1,6,3,6,127,8,6,1,7,
  	1,7,1,7,5,7,132,8,7,10,7,12,7,135,9,7,1,8,1,8,3,8,139,8,8,1,9,1,9,1,9,
  	1,10,1,10,1,11,1,11,3,11,148,8,11,1,12,1,12,1,12,1,12,1,12,1,12,1,12,
  	3,12,157,8,12,1,13,1,13,1,13,1,13,1,13,1,13,1,13,3,13,166,8,13,1,13,1,
  	13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,3,13,177,8,13,1,13,1,13,3,13,181,
  	8,13,1,13,1,13,3,13,185,8,13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,
  	3,13,195,8,13,1,13,1,13,3,13,199,8,13,1,13,1,13,1,13,1,13,1,13,1,13,1,
  	13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,3,
  	13,221,8,13,1,14,1,14,3,14,225,8,14,1,14,3,14,228,8,14,1,15,1,15,3,15,
  	232,8,15,1,15,3,15,235,8,15,1,16,1,16,3,16,239,8,16,1,16,3,16,242,8,16,
  	1,17,1,17,1,17,1,17,1,17,1,17,1,18,1,18,1,18,3,18,253,8,18,1,19,1,19,
  	1,19,1,19,3,19,259,8,19,1,19,3,19,262,8,19,1,20,1,20,1,20,1,20,1,20,1,
  	20,1,21,1,21,1,21,1,22,1,22,1,22,1,22,3,22,277,8,22,1,22,1,22,1,22,1,
  	22,1,22,1,23,1,23,1,23,5,23,287,8,23,10,23,12,23,290,9,23,1,24,3,24,293,
  	8,24,1,25,1,25,1,25,3,25,298,8,25,1,25,1,25,3,25,302,8,25,1,25,1,25,1,
  	25,1,25,1,25,1,25,1,25,1,25,3,25,312,8,25,1,25,1,25,1,25,1,25,1,25,1,
  	25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,
  	25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,3,25,341,8,25,1,25,1,25,1,25,1,
  	25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,
  	25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,
  	25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,
  	25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,
  	25,1,25,1,25,1,25,5,25,406,8,25,10,25,12,25,409,9,25,1,26,1,26,1,27,1,
  	27,3,27,415,8,27,1,28,1,28,1,29,1,29,3,29,421,8,29,1,30,1,30,1,30,3,30,
  	426,8,30,1,31,1,31,1,32,1,32,3,32,432,8,32,1,32,1,32,1,33,1,33,1,33,5,
  	33,439,8,33,10,33,12,33,442,9,33,1,34,1,34,3,34,446,8,34,1,34,1,34,1,
  	35,1,35,1,35,5,35,453,8,35,10,35,12,35,456,9,35,1,36,1,36,1,36,1,36,1,
  	36,1,36,1,36,1,36,1,36,1,36,1,36,1,36,1,36,1,36,1,36,1,36,1,36,1,36,1,
  	36,1,36,1,36,3,36,479,8,36,1,37,1,37,1,37,3,37,484,8,37,1,38,1,38,1,39,
  	1,39,3,39,490,8,39,1,39,1,39,1,40,1,40,1,40,5,40,497,8,40,10,40,12,40,
  	500,9,40,1,41,1,41,1,41,5,41,505,8,41,10,41,12,41,508,9,41,1,41,0,1,50,
  	42,0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,
  	48,50,52,54,56,58,60,62,64,66,68,70,72,74,76,78,80,82,0,9,1,0,166,168,
  	1,0,161,162,1,0,169,171,1,0,172,175,1,0,177,180,1,0,191,201,4,0,4,4,63,
  	63,123,123,202,202,1,0,64,66,6,0,134,134,138,138,141,158,165,165,176,
  	176,203,206,565,0,85,1,0,0,0,2,90,1,0,0,0,4,96,1,0,0,0,6,110,1,0,0,0,
  	8,112,1,0,0,0,10,119,1,0,0,0,12,123,1,0,0,0,14,128,1,0,0,0,16,136,1,0,
  	0,0,18,140,1,0,0,0,20,143,1,0,0,0,22,145,1,0,0,0,24,149,1,0,0,0,26,220,
  	1,0,0,0,28,222,1,0,0,0,30,229,1,0,0,0,32,236,1,0,0,0,34,243,1,0,0,0,36,
  	249,1,0,0,0,38,254,1,0,0,0,40,263,1,0,0,0,42,269,1,0,0,0,44,272,1,0,0,
  	0,46,283,1,0,0,0,48,292,1,0,0,0,50,340,1,0,0,0,52,410,1,0,0,0,54,414,
  	1,0,0,0,56,416,1,0,0,0,58,420,1,0,0,0,60,425,1,0,0,0,62,427,1,0,0,0,64,
  	429,1,0,0,0,66,435,1,0,0,0,68,443,1,0,0,0,70,449,1,0,0,0,72,478,1,0,0,
  	0,74,483,1,0,0,0,76,485,1,0,0,0,78,487,1,0,0,0,80,493,1,0,0,0,82,501,
  	1,0,0,0,84,86,3,2,1,0,85,84,1,0,0,0,85,86,1,0,0,0,86,87,1,0,0,0,87,88,
  	5,0,0,1,88,1,1,0,0,0,89,91,3,4,2,0,90,89,1,0,0,0,91,92,1,0,0,0,92,90,
  	1,0,0,0,92,93,1,0,0,0,93,3,1,0,0,0,94,97,3,6,3,0,95,97,3,44,22,0,96,94,
  	1,0,0,0,96,95,1,0,0,0,97,5,1,0,0,0,98,111,3,8,4,0,99,111,3,12,6,0,100,
  	111,3,20,10,0,101,111,3,22,11,0,102,111,3,24,12,0,103,111,3,26,13,0,104,
  	111,3,28,14,0,105,111,3,30,15,0,106,111,3,32,16,0,107,111,3,34,17,0,108,
  	111,3,36,18,0,109,111,3,38,19,0,110,98,1,0,0,0,110,99,1,0,0,0,110,100,
  	1,0,0,0,110,101,1,0,0,0,110,102,1,0,0,0,110,103,1,0,0,0,110,104,1,0,0,
  	0,110,105,1,0,0,0,110,106,1,0,0,0,110,107,1,0,0,0,110,108,1,0,0,0,110,
  	109,1,0,0,0,111,7,1,0,0,0,112,114,5,132,0,0,113,115,3,10,5,0,114,113,
  	1,0,0,0,114,115,1,0,0,0,115,116,1,0,0,0,116,117,5,133,0,0,117,9,1,0,0,
  	0,118,120,3,6,3,0,119,118,1,0,0,0,120,121,1,0,0,0,121,119,1,0,0,0,121,
  	122,1,0,0,0,122,11,1,0,0,0,123,124,5,134,0,0,124,126,3,14,7,0,125,127,
  	5,135,0,0,126,125,1,0,0,0,126,127,1,0,0,0,127,13,1,0,0,0,128,133,3,16,
  	8,0,129,130,5,136,0,0,130,132,3,16,8,0,131,129,1,0,0,0,132,135,1,0,0,
  	0,133,131,1,0,0,0,133,134,1,0,0,0,134,15,1,0,0,0,135,133,1,0,0,0,136,
  	138,5,122,0,0,137,139,3,18,9,0,138,137,1,0,0,0,138,139,1,0,0,0,139,17,
  	1,0,0,0,140,141,5,137,0,0,141,142,3,50,25,0,142,19,1,0,0,0,143,144,5,
  	135,0,0,144,21,1,0,0,0,145,147,3,82,41,0,146,148,5,135,0,0,147,146,1,
  	0,0,0,147,148,1,0,0,0,148,23,1,0,0,0,149,150,5,138,0,0,150,151,5,139,
  	0,0,151,152,3,82,41,0,152,153,5,140,0,0,153,156,3,6,3,0,154,155,5,141,
  	0,0,155,157,3,6,3,0,156,154,1,0,0,0,156,157,1,0,0,0,157,25,1,0,0,0,158,
  	159,5,142,0,0,159,160,3,6,3,0,160,161,5,143,0,0,161,162,5,139,0,0,162,
  	163,3,82,41,0,163,165,5,140,0,0,164,166,5,135,0,0,165,164,1,0,0,0,165,
  	166,1,0,0,0,166,221,1,0,0,0,167,168,5,143,0,0,168,169,5,139,0,0,169,170,
  	3,82,41,0,170,171,5,140,0,0,171,172,3,6,3,0,172,221,1,0,0,0,173,174,5,
  	144,0,0,174,176,5,139,0,0,175,177,3,82,41,0,176,175,1,0,0,0,176,177,1,
  	0,0,0,177,178,1,0,0,0,178,180,5,135,0,0,179,181,3,82,41,0,180,179,1,0,
  	0,0,180,181,1,0,0,0,181,182,1,0,0,0,182,184,5,135,0,0,183,185,3,82,41,
  	0,184,183,1,0,0,0,184,185,1,0,0,0,185,186,1,0,0,0,186,187,5,140,0,0,187,
  	221,3,6,3,0,188,189,5,144,0,0,189,190,5,139,0,0,190,191,5,134,0,0,191,
  	192,3,14,7,0,192,194,5,135,0,0,193,195,3,82,41,0,194,193,1,0,0,0,194,
  	195,1,0,0,0,195,196,1,0,0,0,196,198,5,135,0,0,197,199,3,82,41,0,198,197,
  	1,0,0,0,198,199,1,0,0,0,199,200,1,0,0,0,200,201,5,140,0,0,201,202,3,6,
  	3,0,202,221,1,0,0,0,203,204,5,144,0,0,204,205,5,139,0,0,205,206,3,50,
  	25,0,206,207,5,145,0,0,207,208,3,82,41,0,208,209,5,140,0,0,209,210,3,
  	6,3,0,210,221,1,0,0,0,211,212,5,144,0,0,212,213,5,139,0,0,213,214,5,134,
  	0,0,214,215,3,16,8,0,215,216,5,145,0,0,216,217,3,82,41,0,217,218,5,140,
  	0,0,218,219,3,6,3,0,219,221,1,0,0,0,220,158,1,0,0,0,220,167,1,0,0,0,220,
  	173,1,0,0,0,220,188,1,0,0,0,220,203,1,0,0,0,220,211,1,0,0,0,221,27,1,
  	0,0,0,222,224,5,146,0,0,223,225,5,122,0,0,224,223,1,0,0,0,224,225,1,0,
  	0,0,225,227,1,0,0,0,226,228,5,135,0,0,227,226,1,0,0,0,227,228,1,0,0,0,
  	228,29,1,0,0,0,229,231,5,147,0,0,230,232,5,122,0,0,231,230,1,0,0,0,231,
  	232,1,0,0,0,232,234,1,0,0,0,233,235,5,135,0,0,234,233,1,0,0,0,234,235,
  	1,0,0,0,235,31,1,0,0,0,236,238,5,148,0,0,237,239,3,82,41,0,238,237,1,
  	0,0,0,238,239,1,0,0,0,239,241,1,0,0,0,240,242,5,135,0,0,241,240,1,0,0,
  	0,241,242,1,0,0,0,242,33,1,0,0,0,243,244,5,149,0,0,244,245,5,139,0,0,
  	245,246,3,82,41,0,246,247,5,140,0,0,247,248,3,6,3,0,248,35,1,0,0,0,249,
  	250,5,150,0,0,250,252,3,82,41,0,251,253,5,135,0,0,252,251,1,0,0,0,252,
  	253,1,0,0,0,253,37,1,0,0,0,254,255,5,151,0,0,255,261,3,8,4,0,256,258,
  	3,40,20,0,257,259,3,42,21,0,258,257,1,0,0,0,258,259,1,0,0,0,259,262,1,
  	0,0,0,260,262,3,42,21,0,261,256,1,0,0,0,261,260,1,0,0,0,262,39,1,0,0,
  	0,263,264,5,152,0,0,264,265,5,139,0,0,265,266,5,122,0,0,266,267,5,140,
  	0,0,267,268,3,8,4,0,268,41,1,0,0,0,269,270,5,153,0,0,270,271,3,8,4,0,
  	271,43,1,0,0,0,272,273,5,154,0,0,273,274,5,122,0,0,274,276,5,139,0,0,
  	275,277,3,46,23,0,276,275,1,0,0,0,276,277,1,0,0,0,277,278,1,0,0,0,278,
  	279,5,140,0,0,279,280,5,132,0,0,280,281,3,48,24,0,281,282,5,133,0,0,282,
  	45,1,0,0,0,283,288,5,122,0,0,284,285,5,136,0,0,285,287,5,122,0,0,286,
  	284,1,0,0,0,287,290,1,0,0,0,288,286,1,0,0,0,288,289,1,0,0,0,289,47,1,
  	0,0,0,290,288,1,0,0,0,291,293,3,2,1,0,292,291,1,0,0,0,292,293,1,0,0,0,
  	293,49,1,0,0,0,294,295,6,25,-1,0,295,297,5,154,0,0,296,298,5,122,0,0,
  	297,296,1,0,0,0,297,298,1,0,0,0,298,299,1,0,0,0,299,301,5,139,0,0,300,
  	302,3,46,23,0,301,300,1,0,0,0,301,302,1,0,0,0,302,303,1,0,0,0,303,304,
  	5,140,0,0,304,305,5,132,0,0,305,306,3,48,24,0,306,307,5,133,0,0,307,341,
  	1,0,0,0,308,309,5,155,0,0,309,311,3,50,25,0,310,312,3,78,39,0,311,310,
  	1,0,0,0,311,312,1,0,0,0,312,341,1,0,0,0,313,314,5,156,0,0,314,341,3,50,
  	25,30,315,316,5,157,0,0,316,341,3,50,25,29,317,318,5,158,0,0,318,341,
  	3,50,25,28,319,320,5,159,0,0,320,341,3,50,25,27,321,322,5,160,0,0,322,
  	341,3,50,25,26,323,324,5,161,0,0,324,341,3,50,25,25,325,326,5,162,0,0,
  	326,341,3,50,25,24,327,328,5,163,0,0,328,341,3,50,25,23,329,330,5,164,
  	0,0,330,341,3,50,25,22,331,341,5,165,0,0,332,341,5,122,0,0,333,341,3,
  	54,27,0,334,341,3,64,32,0,335,341,3,68,34,0,336,337,5,139,0,0,337,338,
  	3,82,41,0,338,339,5,140,0,0,339,341,1,0,0,0,340,294,1,0,0,0,340,308,1,
  	0,0,0,340,313,1,0,0,0,340,315,1,0,0,0,340,317,1,0,0,0,340,319,1,0,0,0,
  	340,321,1,0,0,0,340,323,1,0,0,0,340,325,1,0,0,0,340,327,1,0,0,0,340,329,
  	1,0,0,0,340,331,1,0,0,0,340,332,1,0,0,0,340,333,1,0,0,0,340,334,1,0,0,
  	0,340,335,1,0,0,0,340,336,1,0,0,0,341,407,1,0,0,0,342,343,10,21,0,0,343,
  	344,7,0,0,0,344,406,3,50,25,22,345,346,10,20,0,0,346,347,7,1,0,0,347,
  	406,3,50,25,21,348,349,10,19,0,0,349,350,7,2,0,0,350,406,3,50,25,20,351,
  	352,10,18,0,0,352,353,7,3,0,0,353,406,3,50,25,19,354,355,10,17,0,0,355,
  	356,5,176,0,0,356,406,3,50,25,18,357,358,10,16,0,0,358,359,5,145,0,0,
  	359,406,3,50,25,17,360,361,10,15,0,0,361,362,7,4,0,0,362,406,3,50,25,
  	16,363,364,10,14,0,0,364,365,5,181,0,0,365,406,3,50,25,15,366,367,10,
  	13,0,0,367,368,5,182,0,0,368,406,3,50,25,14,369,370,10,12,0,0,370,371,
  	5,183,0,0,371,406,3,50,25,13,372,373,10,11,0,0,373,374,5,184,0,0,374,
  	406,3,50,25,12,375,376,10,10,0,0,376,377,5,185,0,0,377,406,3,50,25,11,
  	378,379,10,9,0,0,379,380,5,186,0,0,380,381,3,50,25,0,381,382,5,187,0,
  	0,382,383,3,50,25,10,383,406,1,0,0,0,384,385,10,8,0,0,385,386,5,137,0,
  	0,386,406,3,50,25,9,387,388,10,7,0,0,388,389,3,52,26,0,389,390,3,50,25,
  	8,390,406,1,0,0,0,391,392,10,36,0,0,392,393,5,188,0,0,393,394,3,82,41,
  	0,394,395,5,189,0,0,395,406,1,0,0,0,396,397,10,35,0,0,397,398,5,190,0,
  	0,398,406,3,58,29,0,399,400,10,34,0,0,400,406,3,78,39,0,401,402,10,32,
  	0,0,402,406,5,159,0,0,403,404,10,31,0,0,404,406,5,160,0,0,405,342,1,0,
  	0,0,405,345,1,0,0,0,405,348,1,0,0,0,405,351,1,0,0,0,405,354,1,0,0,0,405,
  	357,1,0,0,0,405,360,1,0,0,0,405,363,1,0,0,0,405,366,1,0,0,0,405,369,1,
  	0,0,0,405,372,1,0,0,0,405,375,1,0,0,0,405,378,1,0,0,0,405,384,1,0,0,0,
  	405,387,1,0,0,0,405,391,1,0,0,0,405,396,1,0,0,0,405,399,1,0,0,0,405,401,
  	1,0,0,0,405,403,1,0,0,0,406,409,1,0,0,0,407,405,1,0,0,0,407,408,1,0,0,
  	0,408,51,1,0,0,0,409,407,1,0,0,0,410,411,7,5,0,0,411,53,1,0,0,0,412,415,
  	7,6,0,0,413,415,3,56,28,0,414,412,1,0,0,0,414,413,1,0,0,0,415,55,1,0,
  	0,0,416,417,7,7,0,0,417,57,1,0,0,0,418,421,5,122,0,0,419,421,3,60,30,
  	0,420,418,1,0,0,0,420,419,1,0,0,0,421,59,1,0,0,0,422,426,3,62,31,0,423,
  	426,5,202,0,0,424,426,5,63,0,0,425,422,1,0,0,0,425,423,1,0,0,0,425,424,
  	1,0,0,0,426,61,1,0,0,0,427,428,7,8,0,0,428,63,1,0,0,0,429,431,5,188,0,
  	0,430,432,3,66,33,0,431,430,1,0,0,0,431,432,1,0,0,0,432,433,1,0,0,0,433,
  	434,5,189,0,0,434,65,1,0,0,0,435,440,3,50,25,0,436,437,5,136,0,0,437,
  	439,3,50,25,0,438,436,1,0,0,0,439,442,1,0,0,0,440,438,1,0,0,0,440,441,
  	1,0,0,0,441,67,1,0,0,0,442,440,1,0,0,0,443,445,5,132,0,0,444,446,3,70,
  	35,0,445,444,1,0,0,0,445,446,1,0,0,0,446,447,1,0,0,0,447,448,5,133,0,
  	0,448,69,1,0,0,0,449,454,3,72,36,0,450,451,5,136,0,0,451,453,3,72,36,
  	0,452,450,1,0,0,0,453,456,1,0,0,0,454,452,1,0,0,0,454,455,1,0,0,0,455,
  	71,1,0,0,0,456,454,1,0,0,0,457,458,3,74,37,0,458,459,5,187,0,0,459,460,
  	3,50,25,0,460,479,1,0,0,0,461,462,5,207,0,0,462,463,3,74,37,0,463,464,
  	5,139,0,0,464,465,5,140,0,0,465,466,5,132,0,0,466,467,3,48,24,0,467,468,
  	5,133,0,0,468,479,1,0,0,0,469,470,5,208,0,0,470,471,3,74,37,0,471,472,
  	5,139,0,0,472,473,3,76,38,0,473,474,5,140,0,0,474,475,5,132,0,0,475,476,
  	3,48,24,0,476,477,5,133,0,0,477,479,1,0,0,0,478,457,1,0,0,0,478,461,1,
  	0,0,0,478,469,1,0,0,0,479,73,1,0,0,0,480,484,3,58,29,0,481,484,5,123,
  	0,0,482,484,3,56,28,0,483,480,1,0,0,0,483,481,1,0,0,0,483,482,1,0,0,0,
  	484,75,1,0,0,0,485,486,5,122,0,0,486,77,1,0,0,0,487,489,5,139,0,0,488,
  	490,3,80,40,0,489,488,1,0,0,0,489,490,1,0,0,0,490,491,1,0,0,0,491,492,
  	5,140,0,0,492,79,1,0,0,0,493,498,3,50,25,0,494,495,5,136,0,0,495,497,
  	3,50,25,0,496,494,1,0,0,0,497,500,1,0,0,0,498,496,1,0,0,0,498,499,1,0,
  	0,0,499,81,1,0,0,0,500,498,1,0,0,0,501,506,3,50,25,0,502,503,5,136,0,
  	0,503,505,3,50,25,0,504,502,1,0,0,0,505,508,1,0,0,0,506,504,1,0,0,0,506,
  	507,1,0,0,0,507,83,1,0,0,0,508,506,1,0,0,0,48,85,92,96,110,114,121,126,
  	133,138,147,156,165,176,180,184,194,198,220,224,227,231,234,238,241,252,
  	258,261,276,288,292,297,301,311,340,405,407,414,420,425,431,440,445,454,
  	478,483,489,498,506
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  javascriptparserParserStaticData = std::move(staticData);
}

}

JavaScriptParser::JavaScriptParser(TokenStream *input) : JavaScriptParser(input, antlr4::atn::ParserATNSimulatorOptions()) {}

JavaScriptParser::JavaScriptParser(TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options) : Parser(input) {
  JavaScriptParser::initialize();
  _interpreter = new atn::ParserATNSimulator(this, *javascriptparserParserStaticData->atn, javascriptparserParserStaticData->decisionToDFA, javascriptparserParserStaticData->sharedContextCache, options);
}

JavaScriptParser::~JavaScriptParser() {
  delete _interpreter;
}

const atn::ATN& JavaScriptParser::getATN() const {
  return *javascriptparserParserStaticData->atn;
}

std::string JavaScriptParser::getGrammarFileName() const {
  return "JavaScriptParser.g4";
}

const std::vector<std::string>& JavaScriptParser::getRuleNames() const {
  return javascriptparserParserStaticData->ruleNames;
}

const dfa::Vocabulary& JavaScriptParser::getVocabulary() const {
  return javascriptparserParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView JavaScriptParser::getSerializedATN() const {
  return javascriptparserParserStaticData->serializedATN;
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterProgram(this);
}

void JavaScriptParser::ProgramContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
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
    setState(85);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (((((_la - 4) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 4)) & 8646911284551352321) != 0) || ((((_la - 122) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 122)) & 17588955591683) != 0) || _la == JavaScriptParser::LBRACKET

    || _la == JavaScriptParser::NULL_) {
      setState(84);
      sourceElements();
    }
    setState(87);
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSourceElements(this);
}

void JavaScriptParser::SourceElementsContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
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
    setState(90); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(89);
      sourceElement();
      setState(92); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (((((_la - 4) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 4)) & 8646911284551352321) != 0) || ((((_la - 122) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 122)) & 17588955591683) != 0) || _la == JavaScriptParser::LBRACKET

    || _la == JavaScriptParser::NULL_);
   
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSourceElement(this);
}

void JavaScriptParser::SourceElementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
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
    setState(96);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 2, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(94);
      statement();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(95);
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStatement(this);
}

void JavaScriptParser::StatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
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
    setState(110);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 3, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(98);
      block();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(99);
      variableStatement();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(100);
      emptyStatement();
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(101);
      expressionStatement();
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(102);
      ifStatement();
      break;
    }

    case 6: {
      enterOuterAlt(_localctx, 6);
      setState(103);
      iterationStatement();
      break;
    }

    case 7: {
      enterOuterAlt(_localctx, 7);
      setState(104);
      continueStatement();
      break;
    }

    case 8: {
      enterOuterAlt(_localctx, 8);
      setState(105);
      breakStatement();
      break;
    }

    case 9: {
      enterOuterAlt(_localctx, 9);
      setState(106);
      returnStatement();
      break;
    }

    case 10: {
      enterOuterAlt(_localctx, 10);
      setState(107);
      withStatement();
      break;
    }

    case 11: {
      enterOuterAlt(_localctx, 11);
      setState(108);
      throwStatement();
      break;
    }

    case 12: {
      enterOuterAlt(_localctx, 12);
      setState(109);
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBlock(this);
}

void JavaScriptParser::BlockContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
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
    setState(112);
    match(JavaScriptParser::LBRACE);
    setState(114);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (((((_la - 4) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 4)) & 8646911284551352321) != 0) || ((((_la - 122) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 122)) & 17588955591683) != 0) || _la == JavaScriptParser::LBRACKET

    || _la == JavaScriptParser::NULL_) {
      setState(113);
      statementList();
    }
    setState(116);
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStatementList(this);
}

void JavaScriptParser::StatementListContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
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
    setState(119); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(118);
      statement();
      setState(121); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (((((_la - 4) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 4)) & 8646911284551352321) != 0) || ((((_la - 122) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 122)) & 17588955591683) != 0) || _la == JavaScriptParser::LBRACKET

    || _la == JavaScriptParser::NULL_);
   
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

tree::TerminalNode* JavaScriptParser::VariableStatementContext::SEMICOLON() {
  return getToken(JavaScriptParser::SEMICOLON, 0);
}


size_t JavaScriptParser::VariableStatementContext::getRuleIndex() const {
  return JavaScriptParser::RuleVariableStatement;
}

void JavaScriptParser::VariableStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterVariableStatement(this);
}

void JavaScriptParser::VariableStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
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
    enterOuterAlt(_localctx, 1);
    setState(123);
    match(JavaScriptParser::VAR);
    setState(124);
    variableDeclarationList();
    setState(126);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 6, _ctx)) {
    case 1: {
      setState(125);
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterVariableDeclarationList(this);
}

void JavaScriptParser::VariableDeclarationListContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
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
    setState(128);
    variableDeclaration();
    setState(133);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == JavaScriptParser::COMMA) {
      setState(129);
      match(JavaScriptParser::COMMA);
      setState(130);
      variableDeclaration();
      setState(135);
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

JavaScriptParser::InitializerContext* JavaScriptParser::VariableDeclarationContext::initializer() {
  return getRuleContext<JavaScriptParser::InitializerContext>(0);
}


size_t JavaScriptParser::VariableDeclarationContext::getRuleIndex() const {
  return JavaScriptParser::RuleVariableDeclaration;
}

void JavaScriptParser::VariableDeclarationContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterVariableDeclaration(this);
}

void JavaScriptParser::VariableDeclarationContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
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
    setState(136);
    match(JavaScriptParser::Identifier);
    setState(138);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == JavaScriptParser::ASSIGN) {
      setState(137);
      initializer();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- InitializerContext ------------------------------------------------------------------

JavaScriptParser::InitializerContext::InitializerContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::InitializerContext::ASSIGN() {
  return getToken(JavaScriptParser::ASSIGN, 0);
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::InitializerContext::singleExpression() {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(0);
}


size_t JavaScriptParser::InitializerContext::getRuleIndex() const {
  return JavaScriptParser::RuleInitializer;
}

void JavaScriptParser::InitializerContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterInitializer(this);
}

void JavaScriptParser::InitializerContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitInitializer(this);
}

JavaScriptParser::InitializerContext* JavaScriptParser::initializer() {
  InitializerContext *_localctx = _tracker.createInstance<InitializerContext>(_ctx, getState());
  enterRule(_localctx, 18, JavaScriptParser::RuleInitializer);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(140);
    match(JavaScriptParser::ASSIGN);
    setState(141);
    singleExpression(0);
   
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterEmptyStatement(this);
}

void JavaScriptParser::EmptyStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitEmptyStatement(this);
}

JavaScriptParser::EmptyStatementContext* JavaScriptParser::emptyStatement() {
  EmptyStatementContext *_localctx = _tracker.createInstance<EmptyStatementContext>(_ctx, getState());
  enterRule(_localctx, 20, JavaScriptParser::RuleEmptyStatement);

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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterExpressionStatement(this);
}

void JavaScriptParser::ExpressionStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitExpressionStatement(this);
}

JavaScriptParser::ExpressionStatementContext* JavaScriptParser::expressionStatement() {
  ExpressionStatementContext *_localctx = _tracker.createInstance<ExpressionStatementContext>(_ctx, getState());
  enterRule(_localctx, 22, JavaScriptParser::RuleExpressionStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(145);
    expressionSequence();
    setState(147);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 9, _ctx)) {
    case 1: {
      setState(146);
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterIfStatement(this);
}

void JavaScriptParser::IfStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitIfStatement(this);
}

JavaScriptParser::IfStatementContext* JavaScriptParser::ifStatement() {
  IfStatementContext *_localctx = _tracker.createInstance<IfStatementContext>(_ctx, getState());
  enterRule(_localctx, 24, JavaScriptParser::RuleIfStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(149);
    match(JavaScriptParser::IF);
    setState(150);
    match(JavaScriptParser::LPAREN);
    setState(151);
    expressionSequence();
    setState(152);
    match(JavaScriptParser::RPAREN);
    setState(153);
    statement();
    setState(156);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 10, _ctx)) {
    case 1: {
      setState(154);
      match(JavaScriptParser::ELSE);
      setState(155);
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

tree::TerminalNode* JavaScriptParser::IterationStatementContext::DO() {
  return getToken(JavaScriptParser::DO, 0);
}

JavaScriptParser::StatementContext* JavaScriptParser::IterationStatementContext::statement() {
  return getRuleContext<JavaScriptParser::StatementContext>(0);
}

tree::TerminalNode* JavaScriptParser::IterationStatementContext::WHILE() {
  return getToken(JavaScriptParser::WHILE, 0);
}

tree::TerminalNode* JavaScriptParser::IterationStatementContext::LPAREN() {
  return getToken(JavaScriptParser::LPAREN, 0);
}

std::vector<JavaScriptParser::ExpressionSequenceContext *> JavaScriptParser::IterationStatementContext::expressionSequence() {
  return getRuleContexts<JavaScriptParser::ExpressionSequenceContext>();
}

JavaScriptParser::ExpressionSequenceContext* JavaScriptParser::IterationStatementContext::expressionSequence(size_t i) {
  return getRuleContext<JavaScriptParser::ExpressionSequenceContext>(i);
}

tree::TerminalNode* JavaScriptParser::IterationStatementContext::RPAREN() {
  return getToken(JavaScriptParser::RPAREN, 0);
}

std::vector<tree::TerminalNode *> JavaScriptParser::IterationStatementContext::SEMICOLON() {
  return getTokens(JavaScriptParser::SEMICOLON);
}

tree::TerminalNode* JavaScriptParser::IterationStatementContext::SEMICOLON(size_t i) {
  return getToken(JavaScriptParser::SEMICOLON, i);
}

tree::TerminalNode* JavaScriptParser::IterationStatementContext::FOR() {
  return getToken(JavaScriptParser::FOR, 0);
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


size_t JavaScriptParser::IterationStatementContext::getRuleIndex() const {
  return JavaScriptParser::RuleIterationStatement;
}

void JavaScriptParser::IterationStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterIterationStatement(this);
}

void JavaScriptParser::IterationStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitIterationStatement(this);
}

JavaScriptParser::IterationStatementContext* JavaScriptParser::iterationStatement() {
  IterationStatementContext *_localctx = _tracker.createInstance<IterationStatementContext>(_ctx, getState());
  enterRule(_localctx, 26, JavaScriptParser::RuleIterationStatement);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(220);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 17, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(158);
      match(JavaScriptParser::DO);
      setState(159);
      statement();
      setState(160);
      match(JavaScriptParser::WHILE);
      setState(161);
      match(JavaScriptParser::LPAREN);
      setState(162);
      expressionSequence();
      setState(163);
      match(JavaScriptParser::RPAREN);
      setState(165);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 11, _ctx)) {
      case 1: {
        setState(164);
        match(JavaScriptParser::SEMICOLON);
        break;
      }

      default:
        break;
      }
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(167);
      match(JavaScriptParser::WHILE);
      setState(168);
      match(JavaScriptParser::LPAREN);
      setState(169);
      expressionSequence();
      setState(170);
      match(JavaScriptParser::RPAREN);
      setState(171);
      statement();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(173);
      match(JavaScriptParser::FOR);
      setState(174);
      match(JavaScriptParser::LPAREN);
      setState(176);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (((((_la - 4) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 4)) & 8646911284551352321) != 0) || ((((_la - 122) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 122)) & 17587891209219) != 0) || _la == JavaScriptParser::LBRACKET

      || _la == JavaScriptParser::NULL_) {
        setState(175);
        expressionSequence();
      }
      setState(178);
      match(JavaScriptParser::SEMICOLON);
      setState(180);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (((((_la - 4) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 4)) & 8646911284551352321) != 0) || ((((_la - 122) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 122)) & 17587891209219) != 0) || _la == JavaScriptParser::LBRACKET

      || _la == JavaScriptParser::NULL_) {
        setState(179);
        expressionSequence();
      }
      setState(182);
      match(JavaScriptParser::SEMICOLON);
      setState(184);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (((((_la - 4) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 4)) & 8646911284551352321) != 0) || ((((_la - 122) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 122)) & 17587891209219) != 0) || _la == JavaScriptParser::LBRACKET

      || _la == JavaScriptParser::NULL_) {
        setState(183);
        expressionSequence();
      }
      setState(186);
      match(JavaScriptParser::RPAREN);
      setState(187);
      statement();
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(188);
      match(JavaScriptParser::FOR);
      setState(189);
      match(JavaScriptParser::LPAREN);
      setState(190);
      match(JavaScriptParser::VAR);
      setState(191);
      variableDeclarationList();
      setState(192);
      match(JavaScriptParser::SEMICOLON);
      setState(194);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (((((_la - 4) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 4)) & 8646911284551352321) != 0) || ((((_la - 122) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 122)) & 17587891209219) != 0) || _la == JavaScriptParser::LBRACKET

      || _la == JavaScriptParser::NULL_) {
        setState(193);
        expressionSequence();
      }
      setState(196);
      match(JavaScriptParser::SEMICOLON);
      setState(198);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (((((_la - 4) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 4)) & 8646911284551352321) != 0) || ((((_la - 122) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 122)) & 17587891209219) != 0) || _la == JavaScriptParser::LBRACKET

      || _la == JavaScriptParser::NULL_) {
        setState(197);
        expressionSequence();
      }
      setState(200);
      match(JavaScriptParser::RPAREN);
      setState(201);
      statement();
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(203);
      match(JavaScriptParser::FOR);
      setState(204);
      match(JavaScriptParser::LPAREN);
      setState(205);
      singleExpression(0);
      setState(206);
      match(JavaScriptParser::IN);
      setState(207);
      expressionSequence();
      setState(208);
      match(JavaScriptParser::RPAREN);
      setState(209);
      statement();
      break;
    }

    case 6: {
      enterOuterAlt(_localctx, 6);
      setState(211);
      match(JavaScriptParser::FOR);
      setState(212);
      match(JavaScriptParser::LPAREN);
      setState(213);
      match(JavaScriptParser::VAR);
      setState(214);
      variableDeclaration();
      setState(215);
      match(JavaScriptParser::IN);
      setState(216);
      expressionSequence();
      setState(217);
      match(JavaScriptParser::RPAREN);
      setState(218);
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

tree::TerminalNode* JavaScriptParser::ContinueStatementContext::SEMICOLON() {
  return getToken(JavaScriptParser::SEMICOLON, 0);
}


size_t JavaScriptParser::ContinueStatementContext::getRuleIndex() const {
  return JavaScriptParser::RuleContinueStatement;
}

void JavaScriptParser::ContinueStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterContinueStatement(this);
}

void JavaScriptParser::ContinueStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitContinueStatement(this);
}

JavaScriptParser::ContinueStatementContext* JavaScriptParser::continueStatement() {
  ContinueStatementContext *_localctx = _tracker.createInstance<ContinueStatementContext>(_ctx, getState());
  enterRule(_localctx, 28, JavaScriptParser::RuleContinueStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(222);
    match(JavaScriptParser::CONTINUE);
    setState(224);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 18, _ctx)) {
    case 1: {
      setState(223);
      match(JavaScriptParser::Identifier);
      break;
    }

    default:
      break;
    }
    setState(227);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 19, _ctx)) {
    case 1: {
      setState(226);
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

tree::TerminalNode* JavaScriptParser::BreakStatementContext::SEMICOLON() {
  return getToken(JavaScriptParser::SEMICOLON, 0);
}


size_t JavaScriptParser::BreakStatementContext::getRuleIndex() const {
  return JavaScriptParser::RuleBreakStatement;
}

void JavaScriptParser::BreakStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBreakStatement(this);
}

void JavaScriptParser::BreakStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBreakStatement(this);
}

JavaScriptParser::BreakStatementContext* JavaScriptParser::breakStatement() {
  BreakStatementContext *_localctx = _tracker.createInstance<BreakStatementContext>(_ctx, getState());
  enterRule(_localctx, 30, JavaScriptParser::RuleBreakStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(229);
    match(JavaScriptParser::BREAK);
    setState(231);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 20, _ctx)) {
    case 1: {
      setState(230);
      match(JavaScriptParser::Identifier);
      break;
    }

    default:
      break;
    }
    setState(234);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 21, _ctx)) {
    case 1: {
      setState(233);
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterReturnStatement(this);
}

void JavaScriptParser::ReturnStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitReturnStatement(this);
}

JavaScriptParser::ReturnStatementContext* JavaScriptParser::returnStatement() {
  ReturnStatementContext *_localctx = _tracker.createInstance<ReturnStatementContext>(_ctx, getState());
  enterRule(_localctx, 32, JavaScriptParser::RuleReturnStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(236);
    match(JavaScriptParser::RETURN);
    setState(238);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 22, _ctx)) {
    case 1: {
      setState(237);
      expressionSequence();
      break;
    }

    default:
      break;
    }
    setState(241);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 23, _ctx)) {
    case 1: {
      setState(240);
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

//----------------- WithStatementContext ------------------------------------------------------------------

JavaScriptParser::WithStatementContext::WithStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* JavaScriptParser::WithStatementContext::WITH() {
  return getToken(JavaScriptParser::WITH, 0);
}

tree::TerminalNode* JavaScriptParser::WithStatementContext::LPAREN() {
  return getToken(JavaScriptParser::LPAREN, 0);
}

JavaScriptParser::ExpressionSequenceContext* JavaScriptParser::WithStatementContext::expressionSequence() {
  return getRuleContext<JavaScriptParser::ExpressionSequenceContext>(0);
}

tree::TerminalNode* JavaScriptParser::WithStatementContext::RPAREN() {
  return getToken(JavaScriptParser::RPAREN, 0);
}

JavaScriptParser::StatementContext* JavaScriptParser::WithStatementContext::statement() {
  return getRuleContext<JavaScriptParser::StatementContext>(0);
}


size_t JavaScriptParser::WithStatementContext::getRuleIndex() const {
  return JavaScriptParser::RuleWithStatement;
}

void JavaScriptParser::WithStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterWithStatement(this);
}

void JavaScriptParser::WithStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitWithStatement(this);
}

JavaScriptParser::WithStatementContext* JavaScriptParser::withStatement() {
  WithStatementContext *_localctx = _tracker.createInstance<WithStatementContext>(_ctx, getState());
  enterRule(_localctx, 34, JavaScriptParser::RuleWithStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(243);
    match(JavaScriptParser::WITH);
    setState(244);
    match(JavaScriptParser::LPAREN);
    setState(245);
    expressionSequence();
    setState(246);
    match(JavaScriptParser::RPAREN);
    setState(247);
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

tree::TerminalNode* JavaScriptParser::ThrowStatementContext::SEMICOLON() {
  return getToken(JavaScriptParser::SEMICOLON, 0);
}


size_t JavaScriptParser::ThrowStatementContext::getRuleIndex() const {
  return JavaScriptParser::RuleThrowStatement;
}

void JavaScriptParser::ThrowStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterThrowStatement(this);
}

void JavaScriptParser::ThrowStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitThrowStatement(this);
}

JavaScriptParser::ThrowStatementContext* JavaScriptParser::throwStatement() {
  ThrowStatementContext *_localctx = _tracker.createInstance<ThrowStatementContext>(_ctx, getState());
  enterRule(_localctx, 36, JavaScriptParser::RuleThrowStatement);

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
    match(JavaScriptParser::THROW);
    setState(250);
    expressionSequence();
    setState(252);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 24, _ctx)) {
    case 1: {
      setState(251);
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTryStatement(this);
}

void JavaScriptParser::TryStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTryStatement(this);
}

JavaScriptParser::TryStatementContext* JavaScriptParser::tryStatement() {
  TryStatementContext *_localctx = _tracker.createInstance<TryStatementContext>(_ctx, getState());
  enterRule(_localctx, 38, JavaScriptParser::RuleTryStatement);
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
    match(JavaScriptParser::TRY);
    setState(255);
    block();
    setState(261);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case JavaScriptParser::CATCH: {
        setState(256);
        catchProduction();
        setState(258);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == JavaScriptParser::FINALLY) {
          setState(257);
          finallyProduction();
        }
        break;
      }

      case JavaScriptParser::FINALLY: {
        setState(260);
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

tree::TerminalNode* JavaScriptParser::CatchProductionContext::Identifier() {
  return getToken(JavaScriptParser::Identifier, 0);
}

tree::TerminalNode* JavaScriptParser::CatchProductionContext::RPAREN() {
  return getToken(JavaScriptParser::RPAREN, 0);
}

JavaScriptParser::BlockContext* JavaScriptParser::CatchProductionContext::block() {
  return getRuleContext<JavaScriptParser::BlockContext>(0);
}


size_t JavaScriptParser::CatchProductionContext::getRuleIndex() const {
  return JavaScriptParser::RuleCatchProduction;
}

void JavaScriptParser::CatchProductionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCatchProduction(this);
}

void JavaScriptParser::CatchProductionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCatchProduction(this);
}

JavaScriptParser::CatchProductionContext* JavaScriptParser::catchProduction() {
  CatchProductionContext *_localctx = _tracker.createInstance<CatchProductionContext>(_ctx, getState());
  enterRule(_localctx, 40, JavaScriptParser::RuleCatchProduction);

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
    match(JavaScriptParser::CATCH);
    setState(264);
    match(JavaScriptParser::LPAREN);
    setState(265);
    match(JavaScriptParser::Identifier);
    setState(266);
    match(JavaScriptParser::RPAREN);
    setState(267);
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFinallyProduction(this);
}

void JavaScriptParser::FinallyProductionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFinallyProduction(this);
}

JavaScriptParser::FinallyProductionContext* JavaScriptParser::finallyProduction() {
  FinallyProductionContext *_localctx = _tracker.createInstance<FinallyProductionContext>(_ctx, getState());
  enterRule(_localctx, 42, JavaScriptParser::RuleFinallyProduction);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(269);
    match(JavaScriptParser::FINALLY);
    setState(270);
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

tree::TerminalNode* JavaScriptParser::FunctionDeclarationContext::LBRACE() {
  return getToken(JavaScriptParser::LBRACE, 0);
}

JavaScriptParser::FunctionBodyContext* JavaScriptParser::FunctionDeclarationContext::functionBody() {
  return getRuleContext<JavaScriptParser::FunctionBodyContext>(0);
}

tree::TerminalNode* JavaScriptParser::FunctionDeclarationContext::RBRACE() {
  return getToken(JavaScriptParser::RBRACE, 0);
}

JavaScriptParser::FormalParameterListContext* JavaScriptParser::FunctionDeclarationContext::formalParameterList() {
  return getRuleContext<JavaScriptParser::FormalParameterListContext>(0);
}


size_t JavaScriptParser::FunctionDeclarationContext::getRuleIndex() const {
  return JavaScriptParser::RuleFunctionDeclaration;
}

void JavaScriptParser::FunctionDeclarationContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFunctionDeclaration(this);
}

void JavaScriptParser::FunctionDeclarationContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFunctionDeclaration(this);
}

JavaScriptParser::FunctionDeclarationContext* JavaScriptParser::functionDeclaration() {
  FunctionDeclarationContext *_localctx = _tracker.createInstance<FunctionDeclarationContext>(_ctx, getState());
  enterRule(_localctx, 44, JavaScriptParser::RuleFunctionDeclaration);
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
    setState(272);
    match(JavaScriptParser::FUNCTION);
    setState(273);
    match(JavaScriptParser::Identifier);
    setState(274);
    match(JavaScriptParser::LPAREN);
    setState(276);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == JavaScriptParser::Identifier) {
      setState(275);
      formalParameterList();
    }
    setState(278);
    match(JavaScriptParser::RPAREN);
    setState(279);
    match(JavaScriptParser::LBRACE);
    setState(280);
    functionBody();
    setState(281);
    match(JavaScriptParser::RBRACE);
   
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFormalParameterList(this);
}

void JavaScriptParser::FormalParameterListContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFormalParameterList(this);
}

JavaScriptParser::FormalParameterListContext* JavaScriptParser::formalParameterList() {
  FormalParameterListContext *_localctx = _tracker.createInstance<FormalParameterListContext>(_ctx, getState());
  enterRule(_localctx, 46, JavaScriptParser::RuleFormalParameterList);
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
    setState(283);
    match(JavaScriptParser::Identifier);
    setState(288);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == JavaScriptParser::COMMA) {
      setState(284);
      match(JavaScriptParser::COMMA);
      setState(285);
      match(JavaScriptParser::Identifier);
      setState(290);
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFunctionBody(this);
}

void JavaScriptParser::FunctionBodyContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFunctionBody(this);
}

JavaScriptParser::FunctionBodyContext* JavaScriptParser::functionBody() {
  FunctionBodyContext *_localctx = _tracker.createInstance<FunctionBodyContext>(_ctx, getState());
  enterRule(_localctx, 48, JavaScriptParser::RuleFunctionBody);
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
    setState(292);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (((((_la - 4) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 4)) & 8646911284551352321) != 0) || ((((_la - 122) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 122)) & 17588955591683) != 0) || _la == JavaScriptParser::LBRACKET

    || _la == JavaScriptParser::NULL_) {
      setState(291);
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTernaryExpression(this);
}
void JavaScriptParser::TernaryExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLogicalAndExpression(this);
}
void JavaScriptParser::LogicalAndExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLogicalAndExpression(this);
}
//----------------- PreIncrementExpressionContext ------------------------------------------------------------------

tree::TerminalNode* JavaScriptParser::PreIncrementExpressionContext::PLUS_PLUS() {
  return getToken(JavaScriptParser::PLUS_PLUS, 0);
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::PreIncrementExpressionContext::singleExpression() {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(0);
}

JavaScriptParser::PreIncrementExpressionContext::PreIncrementExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::PreIncrementExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPreIncrementExpression(this);
}
void JavaScriptParser::PreIncrementExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPreIncrementExpression(this);
}
//----------------- ObjectLiteralExpressionContext ------------------------------------------------------------------

JavaScriptParser::ObjectLiteralContext* JavaScriptParser::ObjectLiteralExpressionContext::objectLiteral() {
  return getRuleContext<JavaScriptParser::ObjectLiteralContext>(0);
}

JavaScriptParser::ObjectLiteralExpressionContext::ObjectLiteralExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::ObjectLiteralExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterObjectLiteralExpression(this);
}
void JavaScriptParser::ObjectLiteralExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterInExpression(this);
}
void JavaScriptParser::InExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLogicalOrExpression(this);
}
void JavaScriptParser::LogicalOrExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterNotExpression(this);
}
void JavaScriptParser::NotExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitNotExpression(this);
}
//----------------- PreDecreaseExpressionContext ------------------------------------------------------------------

tree::TerminalNode* JavaScriptParser::PreDecreaseExpressionContext::MINUS_MINUS() {
  return getToken(JavaScriptParser::MINUS_MINUS, 0);
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::PreDecreaseExpressionContext::singleExpression() {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(0);
}

JavaScriptParser::PreDecreaseExpressionContext::PreDecreaseExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::PreDecreaseExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPreDecreaseExpression(this);
}
void JavaScriptParser::PreDecreaseExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPreDecreaseExpression(this);
}
//----------------- ThisExpressionContext ------------------------------------------------------------------

tree::TerminalNode* JavaScriptParser::ThisExpressionContext::THIS() {
  return getToken(JavaScriptParser::THIS, 0);
}

JavaScriptParser::ThisExpressionContext::ThisExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::ThisExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterThisExpression(this);
}
void JavaScriptParser::ThisExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitThisExpression(this);
}
//----------------- FunctionExpressionContext ------------------------------------------------------------------

tree::TerminalNode* JavaScriptParser::FunctionExpressionContext::FUNCTION() {
  return getToken(JavaScriptParser::FUNCTION, 0);
}

tree::TerminalNode* JavaScriptParser::FunctionExpressionContext::LPAREN() {
  return getToken(JavaScriptParser::LPAREN, 0);
}

tree::TerminalNode* JavaScriptParser::FunctionExpressionContext::RPAREN() {
  return getToken(JavaScriptParser::RPAREN, 0);
}

tree::TerminalNode* JavaScriptParser::FunctionExpressionContext::LBRACE() {
  return getToken(JavaScriptParser::LBRACE, 0);
}

JavaScriptParser::FunctionBodyContext* JavaScriptParser::FunctionExpressionContext::functionBody() {
  return getRuleContext<JavaScriptParser::FunctionBodyContext>(0);
}

tree::TerminalNode* JavaScriptParser::FunctionExpressionContext::RBRACE() {
  return getToken(JavaScriptParser::RBRACE, 0);
}

tree::TerminalNode* JavaScriptParser::FunctionExpressionContext::Identifier() {
  return getToken(JavaScriptParser::Identifier, 0);
}

JavaScriptParser::FormalParameterListContext* JavaScriptParser::FunctionExpressionContext::formalParameterList() {
  return getRuleContext<JavaScriptParser::FormalParameterListContext>(0);
}

JavaScriptParser::FunctionExpressionContext::FunctionExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::FunctionExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFunctionExpression(this);
}
void JavaScriptParser::FunctionExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFunctionExpression(this);
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterUnaryMinusExpression(this);
}
void JavaScriptParser::UnaryMinusExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAssignmentExpression(this);
}
void JavaScriptParser::AssignmentExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAssignmentExpression(this);
}
//----------------- PostDecreaseExpressionContext ------------------------------------------------------------------

JavaScriptParser::SingleExpressionContext* JavaScriptParser::PostDecreaseExpressionContext::singleExpression() {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(0);
}

tree::TerminalNode* JavaScriptParser::PostDecreaseExpressionContext::MINUS_MINUS() {
  return getToken(JavaScriptParser::MINUS_MINUS, 0);
}

JavaScriptParser::PostDecreaseExpressionContext::PostDecreaseExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::PostDecreaseExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPostDecreaseExpression(this);
}
void JavaScriptParser::PostDecreaseExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTypeofExpression(this);
}
void JavaScriptParser::TypeofExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterInstanceofExpression(this);
}
void JavaScriptParser::InstanceofExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterUnaryPlusExpression(this);
}
void JavaScriptParser::UnaryPlusExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterDeleteExpression(this);
}
void JavaScriptParser::DeleteExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
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

tree::TerminalNode* JavaScriptParser::EqualityExpressionContext::STRICT_EQ() {
  return getToken(JavaScriptParser::STRICT_EQ, 0);
}

tree::TerminalNode* JavaScriptParser::EqualityExpressionContext::STRICT_NE() {
  return getToken(JavaScriptParser::STRICT_NE, 0);
}

JavaScriptParser::EqualityExpressionContext::EqualityExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::EqualityExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterEqualityExpression(this);
}
void JavaScriptParser::EqualityExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
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

tree::TerminalNode* JavaScriptParser::BitXOrExpressionContext::BIT_XOR() {
  return getToken(JavaScriptParser::BIT_XOR, 0);
}

JavaScriptParser::BitXOrExpressionContext::BitXOrExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::BitXOrExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBitXOrExpression(this);
}
void JavaScriptParser::BitXOrExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
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

tree::TerminalNode* JavaScriptParser::MultiplicativeExpressionContext::MULTIPLY() {
  return getToken(JavaScriptParser::MULTIPLY, 0);
}

tree::TerminalNode* JavaScriptParser::MultiplicativeExpressionContext::DIVIDE() {
  return getToken(JavaScriptParser::DIVIDE, 0);
}

tree::TerminalNode* JavaScriptParser::MultiplicativeExpressionContext::MODULO() {
  return getToken(JavaScriptParser::MODULO, 0);
}

JavaScriptParser::MultiplicativeExpressionContext::MultiplicativeExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::MultiplicativeExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMultiplicativeExpression(this);
}
void JavaScriptParser::MultiplicativeExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCallExpression(this);
}
void JavaScriptParser::CallExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBitShiftExpression(this);
}
void JavaScriptParser::BitShiftExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterParenthesizedExpression(this);
}
void JavaScriptParser::ParenthesizedExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAdditiveExpression(this);
}
void JavaScriptParser::AdditiveExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterRelationalExpression(this);
}
void JavaScriptParser::RelationalExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitRelationalExpression(this);
}
//----------------- PostIncrementExpressionContext ------------------------------------------------------------------

JavaScriptParser::SingleExpressionContext* JavaScriptParser::PostIncrementExpressionContext::singleExpression() {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(0);
}

tree::TerminalNode* JavaScriptParser::PostIncrementExpressionContext::PLUS_PLUS() {
  return getToken(JavaScriptParser::PLUS_PLUS, 0);
}

JavaScriptParser::PostIncrementExpressionContext::PostIncrementExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::PostIncrementExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPostIncrementExpression(this);
}
void JavaScriptParser::PostIncrementExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPostIncrementExpression(this);
}
//----------------- BitNotExpressionContext ------------------------------------------------------------------

tree::TerminalNode* JavaScriptParser::BitNotExpressionContext::TILDE() {
  return getToken(JavaScriptParser::TILDE, 0);
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::BitNotExpressionContext::singleExpression() {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(0);
}

JavaScriptParser::BitNotExpressionContext::BitNotExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::BitNotExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBitNotExpression(this);
}
void JavaScriptParser::BitNotExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterNewExpression(this);
}
void JavaScriptParser::NewExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitNewExpression(this);
}
//----------------- LiteralExpressionContext ------------------------------------------------------------------

JavaScriptParser::LiteralContext* JavaScriptParser::LiteralExpressionContext::literal() {
  return getRuleContext<JavaScriptParser::LiteralContext>(0);
}

JavaScriptParser::LiteralExpressionContext::LiteralExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::LiteralExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLiteralExpression(this);
}
void JavaScriptParser::LiteralExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLiteralExpression(this);
}
//----------------- ArrayLiteralExpressionContext ------------------------------------------------------------------

JavaScriptParser::ArrayLiteralContext* JavaScriptParser::ArrayLiteralExpressionContext::arrayLiteral() {
  return getRuleContext<JavaScriptParser::ArrayLiteralContext>(0);
}

JavaScriptParser::ArrayLiteralExpressionContext::ArrayLiteralExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::ArrayLiteralExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterArrayLiteralExpression(this);
}
void JavaScriptParser::ArrayLiteralExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMemberDotExpression(this);
}
void JavaScriptParser::MemberDotExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMemberIndexExpression(this);
}
void JavaScriptParser::MemberIndexExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMemberIndexExpression(this);
}
//----------------- IdentifierExpressionContext ------------------------------------------------------------------

tree::TerminalNode* JavaScriptParser::IdentifierExpressionContext::Identifier() {
  return getToken(JavaScriptParser::Identifier, 0);
}

JavaScriptParser::IdentifierExpressionContext::IdentifierExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::IdentifierExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterIdentifierExpression(this);
}
void JavaScriptParser::IdentifierExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
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

tree::TerminalNode* JavaScriptParser::BitAndExpressionContext::BIT_AND() {
  return getToken(JavaScriptParser::BIT_AND, 0);
}

JavaScriptParser::BitAndExpressionContext::BitAndExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::BitAndExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBitAndExpression(this);
}
void JavaScriptParser::BitAndExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
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

tree::TerminalNode* JavaScriptParser::BitOrExpressionContext::BIT_OR() {
  return getToken(JavaScriptParser::BIT_OR, 0);
}

JavaScriptParser::BitOrExpressionContext::BitOrExpressionContext(SingleExpressionContext *ctx) { copyFrom(ctx); }

void JavaScriptParser::BitOrExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBitOrExpression(this);
}
void JavaScriptParser::BitOrExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAssignmentOperatorExpression(this);
}
void JavaScriptParser::AssignmentOperatorExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterVoidExpression(this);
}
void JavaScriptParser::VoidExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
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
  size_t startState = 50;
  enterRecursionRule(_localctx, 50, JavaScriptParser::RuleSingleExpression, precedence);

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
    setState(340);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case JavaScriptParser::FUNCTION: {
        _localctx = _tracker.createInstance<FunctionExpressionContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;

        setState(295);
        match(JavaScriptParser::FUNCTION);
        setState(297);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == JavaScriptParser::Identifier) {
          setState(296);
          match(JavaScriptParser::Identifier);
        }
        setState(299);
        match(JavaScriptParser::LPAREN);
        setState(301);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == JavaScriptParser::Identifier) {
          setState(300);
          formalParameterList();
        }
        setState(303);
        match(JavaScriptParser::RPAREN);
        setState(304);
        match(JavaScriptParser::LBRACE);
        setState(305);
        functionBody();
        setState(306);
        match(JavaScriptParser::RBRACE);
        break;
      }

      case JavaScriptParser::NEW: {
        _localctx = _tracker.createInstance<NewExpressionContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(308);
        match(JavaScriptParser::NEW);
        setState(309);
        singleExpression(0);
        setState(311);
        _errHandler->sync(this);

        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 32, _ctx)) {
        case 1: {
          setState(310);
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
        setState(313);
        match(JavaScriptParser::DELETE);
        setState(314);
        singleExpression(30);
        break;
      }

      case JavaScriptParser::VOID: {
        _localctx = _tracker.createInstance<VoidExpressionContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(315);
        match(JavaScriptParser::VOID);
        setState(316);
        singleExpression(29);
        break;
      }

      case JavaScriptParser::TYPEOF: {
        _localctx = _tracker.createInstance<TypeofExpressionContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(317);
        match(JavaScriptParser::TYPEOF);
        setState(318);
        singleExpression(28);
        break;
      }

      case JavaScriptParser::PLUS_PLUS: {
        _localctx = _tracker.createInstance<PreIncrementExpressionContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(319);
        match(JavaScriptParser::PLUS_PLUS);
        setState(320);
        singleExpression(27);
        break;
      }

      case JavaScriptParser::MINUS_MINUS: {
        _localctx = _tracker.createInstance<PreDecreaseExpressionContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(321);
        match(JavaScriptParser::MINUS_MINUS);
        setState(322);
        singleExpression(26);
        break;
      }

      case JavaScriptParser::PLUS: {
        _localctx = _tracker.createInstance<UnaryPlusExpressionContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(323);
        match(JavaScriptParser::PLUS);
        setState(324);
        singleExpression(25);
        break;
      }

      case JavaScriptParser::MINUS: {
        _localctx = _tracker.createInstance<UnaryMinusExpressionContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(325);
        match(JavaScriptParser::MINUS);
        setState(326);
        singleExpression(24);
        break;
      }

      case JavaScriptParser::TILDE: {
        _localctx = _tracker.createInstance<BitNotExpressionContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(327);
        match(JavaScriptParser::TILDE);
        setState(328);
        singleExpression(23);
        break;
      }

      case JavaScriptParser::NOT: {
        _localctx = _tracker.createInstance<NotExpressionContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(329);
        match(JavaScriptParser::NOT);
        setState(330);
        singleExpression(22);
        break;
      }

      case JavaScriptParser::THIS: {
        _localctx = _tracker.createInstance<ThisExpressionContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(331);
        match(JavaScriptParser::THIS);
        break;
      }

      case JavaScriptParser::Identifier: {
        _localctx = _tracker.createInstance<IdentifierExpressionContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(332);
        match(JavaScriptParser::Identifier);
        break;
      }

      case JavaScriptParser::RegularExpressionLiteral:
      case JavaScriptParser::BooleanLiteral:
      case JavaScriptParser::DecimalLiteral:
      case JavaScriptParser::HexIntegerLiteral:
      case JavaScriptParser::OctalIntegerLiteral:
      case JavaScriptParser::StringLiteral:
      case JavaScriptParser::NULL_: {
        _localctx = _tracker.createInstance<LiteralExpressionContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(333);
        literal();
        break;
      }

      case JavaScriptParser::LBRACKET: {
        _localctx = _tracker.createInstance<ArrayLiteralExpressionContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(334);
        arrayLiteral();
        break;
      }

      case JavaScriptParser::LBRACE: {
        _localctx = _tracker.createInstance<ObjectLiteralExpressionContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(335);
        objectLiteral();
        break;
      }

      case JavaScriptParser::LPAREN: {
        _localctx = _tracker.createInstance<ParenthesizedExpressionContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(336);
        match(JavaScriptParser::LPAREN);
        setState(337);
        expressionSequence();
        setState(338);
        match(JavaScriptParser::RPAREN);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    _ctx->stop = _input->LT(-1);
    setState(407);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 35, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(405);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 34, _ctx)) {
        case 1: {
          auto newContext = _tracker.createInstance<MultiplicativeExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(342);

          if (!(precpred(_ctx, 21))) throw FailedPredicateException(this, "precpred(_ctx, 21)");
          setState(343);
          _la = _input->LA(1);
          if (!(((((_la - 166) & ~ 0x3fULL) == 0) &&
            ((1ULL << (_la - 166)) & 7) != 0))) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(344);
          singleExpression(22);
          break;
        }

        case 2: {
          auto newContext = _tracker.createInstance<AdditiveExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(345);

          if (!(precpred(_ctx, 20))) throw FailedPredicateException(this, "precpred(_ctx, 20)");
          setState(346);
          _la = _input->LA(1);
          if (!(_la == JavaScriptParser::PLUS

          || _la == JavaScriptParser::MINUS)) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(347);
          singleExpression(21);
          break;
        }

        case 3: {
          auto newContext = _tracker.createInstance<BitShiftExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(348);

          if (!(precpred(_ctx, 19))) throw FailedPredicateException(this, "precpred(_ctx, 19)");
          setState(349);
          _la = _input->LA(1);
          if (!(((((_la - 169) & ~ 0x3fULL) == 0) &&
            ((1ULL << (_la - 169)) & 7) != 0))) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(350);
          singleExpression(20);
          break;
        }

        case 4: {
          auto newContext = _tracker.createInstance<RelationalExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(351);

          if (!(precpred(_ctx, 18))) throw FailedPredicateException(this, "precpred(_ctx, 18)");
          setState(352);
          _la = _input->LA(1);
          if (!(((((_la - 172) & ~ 0x3fULL) == 0) &&
            ((1ULL << (_la - 172)) & 15) != 0))) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(353);
          singleExpression(19);
          break;
        }

        case 5: {
          auto newContext = _tracker.createInstance<InstanceofExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(354);

          if (!(precpred(_ctx, 17))) throw FailedPredicateException(this, "precpred(_ctx, 17)");
          setState(355);
          match(JavaScriptParser::INSTANCEOF);
          setState(356);
          singleExpression(18);
          break;
        }

        case 6: {
          auto newContext = _tracker.createInstance<InExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(357);

          if (!(precpred(_ctx, 16))) throw FailedPredicateException(this, "precpred(_ctx, 16)");
          setState(358);
          match(JavaScriptParser::IN);
          setState(359);
          singleExpression(17);
          break;
        }

        case 7: {
          auto newContext = _tracker.createInstance<EqualityExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(360);

          if (!(precpred(_ctx, 15))) throw FailedPredicateException(this, "precpred(_ctx, 15)");
          setState(361);
          _la = _input->LA(1);
          if (!(((((_la - 177) & ~ 0x3fULL) == 0) &&
            ((1ULL << (_la - 177)) & 15) != 0))) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(362);
          singleExpression(16);
          break;
        }

        case 8: {
          auto newContext = _tracker.createInstance<BitAndExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(363);

          if (!(precpred(_ctx, 14))) throw FailedPredicateException(this, "precpred(_ctx, 14)");
          setState(364);
          match(JavaScriptParser::BIT_AND);
          setState(365);
          singleExpression(15);
          break;
        }

        case 9: {
          auto newContext = _tracker.createInstance<BitXOrExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(366);

          if (!(precpred(_ctx, 13))) throw FailedPredicateException(this, "precpred(_ctx, 13)");
          setState(367);
          match(JavaScriptParser::BIT_XOR);
          setState(368);
          singleExpression(14);
          break;
        }

        case 10: {
          auto newContext = _tracker.createInstance<BitOrExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(369);

          if (!(precpred(_ctx, 12))) throw FailedPredicateException(this, "precpred(_ctx, 12)");
          setState(370);
          match(JavaScriptParser::BIT_OR);
          setState(371);
          singleExpression(13);
          break;
        }

        case 11: {
          auto newContext = _tracker.createInstance<LogicalAndExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(372);

          if (!(precpred(_ctx, 11))) throw FailedPredicateException(this, "precpred(_ctx, 11)");
          setState(373);
          match(JavaScriptParser::AND);
          setState(374);
          singleExpression(12);
          break;
        }

        case 12: {
          auto newContext = _tracker.createInstance<LogicalOrExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(375);

          if (!(precpred(_ctx, 10))) throw FailedPredicateException(this, "precpred(_ctx, 10)");
          setState(376);
          match(JavaScriptParser::OR);
          setState(377);
          singleExpression(11);
          break;
        }

        case 13: {
          auto newContext = _tracker.createInstance<TernaryExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(378);

          if (!(precpred(_ctx, 9))) throw FailedPredicateException(this, "precpred(_ctx, 9)");
          setState(379);
          match(JavaScriptParser::QUESTION);
          setState(380);
          singleExpression(0);
          setState(381);
          match(JavaScriptParser::COLON);
          setState(382);
          singleExpression(10);
          break;
        }

        case 14: {
          auto newContext = _tracker.createInstance<AssignmentExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(384);

          if (!(precpred(_ctx, 8))) throw FailedPredicateException(this, "precpred(_ctx, 8)");
          setState(385);
          match(JavaScriptParser::ASSIGN);
          setState(386);
          singleExpression(9);
          break;
        }

        case 15: {
          auto newContext = _tracker.createInstance<AssignmentOperatorExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(387);

          if (!(precpred(_ctx, 7))) throw FailedPredicateException(this, "precpred(_ctx, 7)");
          setState(388);
          assignmentOperator();
          setState(389);
          singleExpression(8);
          break;
        }

        case 16: {
          auto newContext = _tracker.createInstance<MemberIndexExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(391);

          if (!(precpred(_ctx, 36))) throw FailedPredicateException(this, "precpred(_ctx, 36)");
          setState(392);
          match(JavaScriptParser::LBRACKET);
          setState(393);
          expressionSequence();
          setState(394);
          match(JavaScriptParser::RBRACKET);
          break;
        }

        case 17: {
          auto newContext = _tracker.createInstance<MemberDotExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(396);

          if (!(precpred(_ctx, 35))) throw FailedPredicateException(this, "precpred(_ctx, 35)");
          setState(397);
          match(JavaScriptParser::DOT);
          setState(398);
          identifierName();
          break;
        }

        case 18: {
          auto newContext = _tracker.createInstance<CallExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(399);

          if (!(precpred(_ctx, 34))) throw FailedPredicateException(this, "precpred(_ctx, 34)");
          setState(400);
          arguments();
          break;
        }

        case 19: {
          auto newContext = _tracker.createInstance<PostIncrementExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(401);

          if (!(precpred(_ctx, 32))) throw FailedPredicateException(this, "precpred(_ctx, 32)");
          setState(402);
          match(JavaScriptParser::PLUS_PLUS);
          break;
        }

        case 20: {
          auto newContext = _tracker.createInstance<PostDecreaseExpressionContext>(_tracker.createInstance<SingleExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleSingleExpression);
          setState(403);

          if (!(precpred(_ctx, 31))) throw FailedPredicateException(this, "precpred(_ctx, 31)");
          setState(404);
          match(JavaScriptParser::MINUS_MINUS);
          break;
        }

        default:
          break;
        } 
      }
      setState(409);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 35, _ctx);
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

tree::TerminalNode* JavaScriptParser::AssignmentOperatorContext::MULTIPLY_ASSIGN() {
  return getToken(JavaScriptParser::MULTIPLY_ASSIGN, 0);
}

tree::TerminalNode* JavaScriptParser::AssignmentOperatorContext::DIVIDE_ASSIGN() {
  return getToken(JavaScriptParser::DIVIDE_ASSIGN, 0);
}

tree::TerminalNode* JavaScriptParser::AssignmentOperatorContext::MODULO_ASSIGN() {
  return getToken(JavaScriptParser::MODULO_ASSIGN, 0);
}

tree::TerminalNode* JavaScriptParser::AssignmentOperatorContext::PLUS_ASSIGN() {
  return getToken(JavaScriptParser::PLUS_ASSIGN, 0);
}

tree::TerminalNode* JavaScriptParser::AssignmentOperatorContext::MINUS_ASSIGN() {
  return getToken(JavaScriptParser::MINUS_ASSIGN, 0);
}

tree::TerminalNode* JavaScriptParser::AssignmentOperatorContext::LSHIFT_ASSIGN() {
  return getToken(JavaScriptParser::LSHIFT_ASSIGN, 0);
}

tree::TerminalNode* JavaScriptParser::AssignmentOperatorContext::RSHIFT_ASSIGN() {
  return getToken(JavaScriptParser::RSHIFT_ASSIGN, 0);
}

tree::TerminalNode* JavaScriptParser::AssignmentOperatorContext::URSHIFT_ASSIGN() {
  return getToken(JavaScriptParser::URSHIFT_ASSIGN, 0);
}

tree::TerminalNode* JavaScriptParser::AssignmentOperatorContext::BIT_AND_ASSIGN() {
  return getToken(JavaScriptParser::BIT_AND_ASSIGN, 0);
}

tree::TerminalNode* JavaScriptParser::AssignmentOperatorContext::BIT_XOR_ASSIGN() {
  return getToken(JavaScriptParser::BIT_XOR_ASSIGN, 0);
}

tree::TerminalNode* JavaScriptParser::AssignmentOperatorContext::BIT_OR_ASSIGN() {
  return getToken(JavaScriptParser::BIT_OR_ASSIGN, 0);
}


size_t JavaScriptParser::AssignmentOperatorContext::getRuleIndex() const {
  return JavaScriptParser::RuleAssignmentOperator;
}

void JavaScriptParser::AssignmentOperatorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAssignmentOperator(this);
}

void JavaScriptParser::AssignmentOperatorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAssignmentOperator(this);
}

JavaScriptParser::AssignmentOperatorContext* JavaScriptParser::assignmentOperator() {
  AssignmentOperatorContext *_localctx = _tracker.createInstance<AssignmentOperatorContext>(_ctx, getState());
  enterRule(_localctx, 52, JavaScriptParser::RuleAssignmentOperator);
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
    setState(410);
    _la = _input->LA(1);
    if (!(((((_la - 191) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 191)) & 2047) != 0))) {
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

tree::TerminalNode* JavaScriptParser::LiteralContext::NULL_() {
  return getToken(JavaScriptParser::NULL_, 0);
}

tree::TerminalNode* JavaScriptParser::LiteralContext::BooleanLiteral() {
  return getToken(JavaScriptParser::BooleanLiteral, 0);
}

tree::TerminalNode* JavaScriptParser::LiteralContext::StringLiteral() {
  return getToken(JavaScriptParser::StringLiteral, 0);
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLiteral(this);
}

void JavaScriptParser::LiteralContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLiteral(this);
}

JavaScriptParser::LiteralContext* JavaScriptParser::literal() {
  LiteralContext *_localctx = _tracker.createInstance<LiteralContext>(_ctx, getState());
  enterRule(_localctx, 54, JavaScriptParser::RuleLiteral);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(414);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case JavaScriptParser::RegularExpressionLiteral:
      case JavaScriptParser::BooleanLiteral:
      case JavaScriptParser::StringLiteral:
      case JavaScriptParser::NULL_: {
        enterOuterAlt(_localctx, 1);
        setState(412);
        _la = _input->LA(1);
        if (!(_la == JavaScriptParser::RegularExpressionLiteral

        || _la == JavaScriptParser::BooleanLiteral || _la == JavaScriptParser::StringLiteral || _la == JavaScriptParser::NULL_)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        break;
      }

      case JavaScriptParser::DecimalLiteral:
      case JavaScriptParser::HexIntegerLiteral:
      case JavaScriptParser::OctalIntegerLiteral: {
        enterOuterAlt(_localctx, 2);
        setState(413);
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterNumericLiteral(this);
}

void JavaScriptParser::NumericLiteralContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitNumericLiteral(this);
}

JavaScriptParser::NumericLiteralContext* JavaScriptParser::numericLiteral() {
  NumericLiteralContext *_localctx = _tracker.createInstance<NumericLiteralContext>(_ctx, getState());
  enterRule(_localctx, 56, JavaScriptParser::RuleNumericLiteral);
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
    setState(416);
    _la = _input->LA(1);
    if (!(((((_la - 64) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 64)) & 7) != 0))) {
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterIdentifierName(this);
}

void JavaScriptParser::IdentifierNameContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitIdentifierName(this);
}

JavaScriptParser::IdentifierNameContext* JavaScriptParser::identifierName() {
  IdentifierNameContext *_localctx = _tracker.createInstance<IdentifierNameContext>(_ctx, getState());
  enterRule(_localctx, 58, JavaScriptParser::RuleIdentifierName);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(420);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case JavaScriptParser::Identifier: {
        enterOuterAlt(_localctx, 1);
        setState(418);
        match(JavaScriptParser::Identifier);
        break;
      }

      case JavaScriptParser::BooleanLiteral:
      case JavaScriptParser::VAR:
      case JavaScriptParser::IF:
      case JavaScriptParser::ELSE:
      case JavaScriptParser::DO:
      case JavaScriptParser::WHILE:
      case JavaScriptParser::FOR:
      case JavaScriptParser::IN:
      case JavaScriptParser::CONTINUE:
      case JavaScriptParser::BREAK:
      case JavaScriptParser::RETURN:
      case JavaScriptParser::WITH:
      case JavaScriptParser::THROW:
      case JavaScriptParser::TRY:
      case JavaScriptParser::CATCH:
      case JavaScriptParser::FINALLY:
      case JavaScriptParser::FUNCTION:
      case JavaScriptParser::NEW:
      case JavaScriptParser::DELETE:
      case JavaScriptParser::VOID:
      case JavaScriptParser::TYPEOF:
      case JavaScriptParser::THIS:
      case JavaScriptParser::INSTANCEOF:
      case JavaScriptParser::NULL_:
      case JavaScriptParser::CASE:
      case JavaScriptParser::SWITCH:
      case JavaScriptParser::DEBUGGER:
      case JavaScriptParser::DEFAULT: {
        enterOuterAlt(_localctx, 2);
        setState(419);
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterReservedWord(this);
}

void JavaScriptParser::ReservedWordContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitReservedWord(this);
}

JavaScriptParser::ReservedWordContext* JavaScriptParser::reservedWord() {
  ReservedWordContext *_localctx = _tracker.createInstance<ReservedWordContext>(_ctx, getState());
  enterRule(_localctx, 60, JavaScriptParser::RuleReservedWord);

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
    switch (_input->LA(1)) {
      case JavaScriptParser::VAR:
      case JavaScriptParser::IF:
      case JavaScriptParser::ELSE:
      case JavaScriptParser::DO:
      case JavaScriptParser::WHILE:
      case JavaScriptParser::FOR:
      case JavaScriptParser::IN:
      case JavaScriptParser::CONTINUE:
      case JavaScriptParser::BREAK:
      case JavaScriptParser::RETURN:
      case JavaScriptParser::WITH:
      case JavaScriptParser::THROW:
      case JavaScriptParser::TRY:
      case JavaScriptParser::CATCH:
      case JavaScriptParser::FINALLY:
      case JavaScriptParser::FUNCTION:
      case JavaScriptParser::NEW:
      case JavaScriptParser::DELETE:
      case JavaScriptParser::VOID:
      case JavaScriptParser::TYPEOF:
      case JavaScriptParser::THIS:
      case JavaScriptParser::INSTANCEOF:
      case JavaScriptParser::CASE:
      case JavaScriptParser::SWITCH:
      case JavaScriptParser::DEBUGGER:
      case JavaScriptParser::DEFAULT: {
        enterOuterAlt(_localctx, 1);
        setState(422);
        keyword();
        break;
      }

      case JavaScriptParser::NULL_: {
        enterOuterAlt(_localctx, 2);
        setState(423);
        match(JavaScriptParser::NULL_);
        break;
      }

      case JavaScriptParser::BooleanLiteral: {
        enterOuterAlt(_localctx, 3);
        setState(424);
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

tree::TerminalNode* JavaScriptParser::KeywordContext::DO() {
  return getToken(JavaScriptParser::DO, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::INSTANCEOF() {
  return getToken(JavaScriptParser::INSTANCEOF, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::TYPEOF() {
  return getToken(JavaScriptParser::TYPEOF, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::CASE() {
  return getToken(JavaScriptParser::CASE, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::ELSE() {
  return getToken(JavaScriptParser::ELSE, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::NEW() {
  return getToken(JavaScriptParser::NEW, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::VAR() {
  return getToken(JavaScriptParser::VAR, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::CATCH() {
  return getToken(JavaScriptParser::CATCH, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::FINALLY() {
  return getToken(JavaScriptParser::FINALLY, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::RETURN() {
  return getToken(JavaScriptParser::RETURN, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::VOID() {
  return getToken(JavaScriptParser::VOID, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::CONTINUE() {
  return getToken(JavaScriptParser::CONTINUE, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::FOR() {
  return getToken(JavaScriptParser::FOR, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::SWITCH() {
  return getToken(JavaScriptParser::SWITCH, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::WHILE() {
  return getToken(JavaScriptParser::WHILE, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::DEBUGGER() {
  return getToken(JavaScriptParser::DEBUGGER, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::FUNCTION() {
  return getToken(JavaScriptParser::FUNCTION, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::THIS() {
  return getToken(JavaScriptParser::THIS, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::WITH() {
  return getToken(JavaScriptParser::WITH, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::DEFAULT() {
  return getToken(JavaScriptParser::DEFAULT, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::IF() {
  return getToken(JavaScriptParser::IF, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::THROW() {
  return getToken(JavaScriptParser::THROW, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::DELETE() {
  return getToken(JavaScriptParser::DELETE, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::IN() {
  return getToken(JavaScriptParser::IN, 0);
}

tree::TerminalNode* JavaScriptParser::KeywordContext::TRY() {
  return getToken(JavaScriptParser::TRY, 0);
}


size_t JavaScriptParser::KeywordContext::getRuleIndex() const {
  return JavaScriptParser::RuleKeyword;
}

void JavaScriptParser::KeywordContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterKeyword(this);
}

void JavaScriptParser::KeywordContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitKeyword(this);
}

JavaScriptParser::KeywordContext* JavaScriptParser::keyword() {
  KeywordContext *_localctx = _tracker.createInstance<KeywordContext>(_ctx, getState());
  enterRule(_localctx, 62, JavaScriptParser::RuleKeyword);
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
    setState(427);
    _la = _input->LA(1);
    if (!(((((_la - 134) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 134)) & 4400227549073) != 0) || ((((_la - 203) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 203)) & 15) != 0))) {
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterArrayLiteral(this);
}

void JavaScriptParser::ArrayLiteralContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
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
    setState(429);
    match(JavaScriptParser::LBRACKET);
    setState(431);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (((((_la - 4) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 4)) & 8646911284551352321) != 0) || ((((_la - 122) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 122)) & 17587891209219) != 0) || _la == JavaScriptParser::LBRACKET

    || _la == JavaScriptParser::NULL_) {
      setState(430);
      elementList();
    }
    setState(433);
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

std::vector<JavaScriptParser::SingleExpressionContext *> JavaScriptParser::ElementListContext::singleExpression() {
  return getRuleContexts<JavaScriptParser::SingleExpressionContext>();
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::ElementListContext::singleExpression(size_t i) {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(i);
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterElementList(this);
}

void JavaScriptParser::ElementListContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
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
    setState(435);
    singleExpression(0);
    setState(440);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == JavaScriptParser::COMMA) {
      setState(436);
      match(JavaScriptParser::COMMA);
      setState(437);
      singleExpression(0);
      setState(442);
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

tree::TerminalNode* JavaScriptParser::ObjectLiteralContext::LBRACE() {
  return getToken(JavaScriptParser::LBRACE, 0);
}

tree::TerminalNode* JavaScriptParser::ObjectLiteralContext::RBRACE() {
  return getToken(JavaScriptParser::RBRACE, 0);
}

JavaScriptParser::PropertyNameAndValueListContext* JavaScriptParser::ObjectLiteralContext::propertyNameAndValueList() {
  return getRuleContext<JavaScriptParser::PropertyNameAndValueListContext>(0);
}


size_t JavaScriptParser::ObjectLiteralContext::getRuleIndex() const {
  return JavaScriptParser::RuleObjectLiteral;
}

void JavaScriptParser::ObjectLiteralContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterObjectLiteral(this);
}

void JavaScriptParser::ObjectLiteralContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitObjectLiteral(this);
}

JavaScriptParser::ObjectLiteralContext* JavaScriptParser::objectLiteral() {
  ObjectLiteralContext *_localctx = _tracker.createInstance<ObjectLiteralContext>(_ctx, getState());
  enterRule(_localctx, 68, JavaScriptParser::RuleObjectLiteral);
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
    setState(443);
    match(JavaScriptParser::LBRACE);
    setState(445);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (((((_la - 63) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 63)) & 1729382256910270479) != 0) || ((((_la - 134) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 134)) & 4400227549073) != 0) || ((((_la - 202) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 202)) & 127) != 0)) {
      setState(444);
      propertyNameAndValueList();
    }
    setState(447);
    match(JavaScriptParser::RBRACE);
   
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

std::vector<tree::TerminalNode *> JavaScriptParser::PropertyNameAndValueListContext::COMMA() {
  return getTokens(JavaScriptParser::COMMA);
}

tree::TerminalNode* JavaScriptParser::PropertyNameAndValueListContext::COMMA(size_t i) {
  return getToken(JavaScriptParser::COMMA, i);
}


size_t JavaScriptParser::PropertyNameAndValueListContext::getRuleIndex() const {
  return JavaScriptParser::RulePropertyNameAndValueList;
}

void JavaScriptParser::PropertyNameAndValueListContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPropertyNameAndValueList(this);
}

void JavaScriptParser::PropertyNameAndValueListContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPropertyNameAndValueList(this);
}

JavaScriptParser::PropertyNameAndValueListContext* JavaScriptParser::propertyNameAndValueList() {
  PropertyNameAndValueListContext *_localctx = _tracker.createInstance<PropertyNameAndValueListContext>(_ctx, getState());
  enterRule(_localctx, 70, JavaScriptParser::RulePropertyNameAndValueList);
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
    setState(449);
    propertyAssignment();
    setState(454);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == JavaScriptParser::COMMA) {
      setState(450);
      match(JavaScriptParser::COMMA);
      setState(451);
      propertyAssignment();
      setState(456);
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

tree::TerminalNode* JavaScriptParser::PropertyAssignmentContext::COLON() {
  return getToken(JavaScriptParser::COLON, 0);
}

JavaScriptParser::SingleExpressionContext* JavaScriptParser::PropertyAssignmentContext::singleExpression() {
  return getRuleContext<JavaScriptParser::SingleExpressionContext>(0);
}

tree::TerminalNode* JavaScriptParser::PropertyAssignmentContext::GET() {
  return getToken(JavaScriptParser::GET, 0);
}

tree::TerminalNode* JavaScriptParser::PropertyAssignmentContext::LPAREN() {
  return getToken(JavaScriptParser::LPAREN, 0);
}

tree::TerminalNode* JavaScriptParser::PropertyAssignmentContext::RPAREN() {
  return getToken(JavaScriptParser::RPAREN, 0);
}

tree::TerminalNode* JavaScriptParser::PropertyAssignmentContext::LBRACE() {
  return getToken(JavaScriptParser::LBRACE, 0);
}

JavaScriptParser::FunctionBodyContext* JavaScriptParser::PropertyAssignmentContext::functionBody() {
  return getRuleContext<JavaScriptParser::FunctionBodyContext>(0);
}

tree::TerminalNode* JavaScriptParser::PropertyAssignmentContext::RBRACE() {
  return getToken(JavaScriptParser::RBRACE, 0);
}

tree::TerminalNode* JavaScriptParser::PropertyAssignmentContext::SET() {
  return getToken(JavaScriptParser::SET, 0);
}

JavaScriptParser::PropertySetParameterListContext* JavaScriptParser::PropertyAssignmentContext::propertySetParameterList() {
  return getRuleContext<JavaScriptParser::PropertySetParameterListContext>(0);
}


size_t JavaScriptParser::PropertyAssignmentContext::getRuleIndex() const {
  return JavaScriptParser::RulePropertyAssignment;
}

void JavaScriptParser::PropertyAssignmentContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPropertyAssignment(this);
}

void JavaScriptParser::PropertyAssignmentContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPropertyAssignment(this);
}

JavaScriptParser::PropertyAssignmentContext* JavaScriptParser::propertyAssignment() {
  PropertyAssignmentContext *_localctx = _tracker.createInstance<PropertyAssignmentContext>(_ctx, getState());
  enterRule(_localctx, 72, JavaScriptParser::RulePropertyAssignment);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(478);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case JavaScriptParser::BooleanLiteral:
      case JavaScriptParser::DecimalLiteral:
      case JavaScriptParser::HexIntegerLiteral:
      case JavaScriptParser::OctalIntegerLiteral:
      case JavaScriptParser::Identifier:
      case JavaScriptParser::StringLiteral:
      case JavaScriptParser::VAR:
      case JavaScriptParser::IF:
      case JavaScriptParser::ELSE:
      case JavaScriptParser::DO:
      case JavaScriptParser::WHILE:
      case JavaScriptParser::FOR:
      case JavaScriptParser::IN:
      case JavaScriptParser::CONTINUE:
      case JavaScriptParser::BREAK:
      case JavaScriptParser::RETURN:
      case JavaScriptParser::WITH:
      case JavaScriptParser::THROW:
      case JavaScriptParser::TRY:
      case JavaScriptParser::CATCH:
      case JavaScriptParser::FINALLY:
      case JavaScriptParser::FUNCTION:
      case JavaScriptParser::NEW:
      case JavaScriptParser::DELETE:
      case JavaScriptParser::VOID:
      case JavaScriptParser::TYPEOF:
      case JavaScriptParser::THIS:
      case JavaScriptParser::INSTANCEOF:
      case JavaScriptParser::NULL_:
      case JavaScriptParser::CASE:
      case JavaScriptParser::SWITCH:
      case JavaScriptParser::DEBUGGER:
      case JavaScriptParser::DEFAULT: {
        enterOuterAlt(_localctx, 1);
        setState(457);
        propertyName();
        setState(458);
        match(JavaScriptParser::COLON);
        setState(459);
        singleExpression(0);
        break;
      }

      case JavaScriptParser::GET: {
        enterOuterAlt(_localctx, 2);
        setState(461);
        match(JavaScriptParser::GET);
        setState(462);
        propertyName();
        setState(463);
        match(JavaScriptParser::LPAREN);
        setState(464);
        match(JavaScriptParser::RPAREN);
        setState(465);
        match(JavaScriptParser::LBRACE);
        setState(466);
        functionBody();
        setState(467);
        match(JavaScriptParser::RBRACE);
        break;
      }

      case JavaScriptParser::SET: {
        enterOuterAlt(_localctx, 3);
        setState(469);
        match(JavaScriptParser::SET);
        setState(470);
        propertyName();
        setState(471);
        match(JavaScriptParser::LPAREN);
        setState(472);
        propertySetParameterList();
        setState(473);
        match(JavaScriptParser::RPAREN);
        setState(474);
        match(JavaScriptParser::LBRACE);
        setState(475);
        functionBody();
        setState(476);
        match(JavaScriptParser::RBRACE);
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPropertyName(this);
}

void JavaScriptParser::PropertyNameContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
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
    setState(483);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case JavaScriptParser::BooleanLiteral:
      case JavaScriptParser::Identifier:
      case JavaScriptParser::VAR:
      case JavaScriptParser::IF:
      case JavaScriptParser::ELSE:
      case JavaScriptParser::DO:
      case JavaScriptParser::WHILE:
      case JavaScriptParser::FOR:
      case JavaScriptParser::IN:
      case JavaScriptParser::CONTINUE:
      case JavaScriptParser::BREAK:
      case JavaScriptParser::RETURN:
      case JavaScriptParser::WITH:
      case JavaScriptParser::THROW:
      case JavaScriptParser::TRY:
      case JavaScriptParser::CATCH:
      case JavaScriptParser::FINALLY:
      case JavaScriptParser::FUNCTION:
      case JavaScriptParser::NEW:
      case JavaScriptParser::DELETE:
      case JavaScriptParser::VOID:
      case JavaScriptParser::TYPEOF:
      case JavaScriptParser::THIS:
      case JavaScriptParser::INSTANCEOF:
      case JavaScriptParser::NULL_:
      case JavaScriptParser::CASE:
      case JavaScriptParser::SWITCH:
      case JavaScriptParser::DEBUGGER:
      case JavaScriptParser::DEFAULT: {
        enterOuterAlt(_localctx, 1);
        setState(480);
        identifierName();
        break;
      }

      case JavaScriptParser::StringLiteral: {
        enterOuterAlt(_localctx, 2);
        setState(481);
        match(JavaScriptParser::StringLiteral);
        break;
      }

      case JavaScriptParser::DecimalLiteral:
      case JavaScriptParser::HexIntegerLiteral:
      case JavaScriptParser::OctalIntegerLiteral: {
        enterOuterAlt(_localctx, 3);
        setState(482);
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPropertySetParameterList(this);
}

void JavaScriptParser::PropertySetParameterListContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPropertySetParameterList(this);
}

JavaScriptParser::PropertySetParameterListContext* JavaScriptParser::propertySetParameterList() {
  PropertySetParameterListContext *_localctx = _tracker.createInstance<PropertySetParameterListContext>(_ctx, getState());
  enterRule(_localctx, 76, JavaScriptParser::RulePropertySetParameterList);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(485);
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

tree::TerminalNode* JavaScriptParser::ArgumentsContext::LPAREN() {
  return getToken(JavaScriptParser::LPAREN, 0);
}

tree::TerminalNode* JavaScriptParser::ArgumentsContext::RPAREN() {
  return getToken(JavaScriptParser::RPAREN, 0);
}

JavaScriptParser::ArgumentListContext* JavaScriptParser::ArgumentsContext::argumentList() {
  return getRuleContext<JavaScriptParser::ArgumentListContext>(0);
}


size_t JavaScriptParser::ArgumentsContext::getRuleIndex() const {
  return JavaScriptParser::RuleArguments;
}

void JavaScriptParser::ArgumentsContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterArguments(this);
}

void JavaScriptParser::ArgumentsContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitArguments(this);
}

JavaScriptParser::ArgumentsContext* JavaScriptParser::arguments() {
  ArgumentsContext *_localctx = _tracker.createInstance<ArgumentsContext>(_ctx, getState());
  enterRule(_localctx, 78, JavaScriptParser::RuleArguments);
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
    match(JavaScriptParser::LPAREN);
    setState(489);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (((((_la - 4) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 4)) & 8646911284551352321) != 0) || ((((_la - 122) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 122)) & 17587891209219) != 0) || _la == JavaScriptParser::LBRACKET

    || _la == JavaScriptParser::NULL_) {
      setState(488);
      argumentList();
    }
    setState(491);
    match(JavaScriptParser::RPAREN);
   
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

std::vector<tree::TerminalNode *> JavaScriptParser::ArgumentListContext::COMMA() {
  return getTokens(JavaScriptParser::COMMA);
}

tree::TerminalNode* JavaScriptParser::ArgumentListContext::COMMA(size_t i) {
  return getToken(JavaScriptParser::COMMA, i);
}


size_t JavaScriptParser::ArgumentListContext::getRuleIndex() const {
  return JavaScriptParser::RuleArgumentList;
}

void JavaScriptParser::ArgumentListContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterArgumentList(this);
}

void JavaScriptParser::ArgumentListContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitArgumentList(this);
}

JavaScriptParser::ArgumentListContext* JavaScriptParser::argumentList() {
  ArgumentListContext *_localctx = _tracker.createInstance<ArgumentListContext>(_ctx, getState());
  enterRule(_localctx, 80, JavaScriptParser::RuleArgumentList);
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
    setState(493);
    singleExpression(0);
    setState(498);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == JavaScriptParser::COMMA) {
      setState(494);
      match(JavaScriptParser::COMMA);
      setState(495);
      singleExpression(0);
      setState(500);
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
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterExpressionSequence(this);
}

void JavaScriptParser::ExpressionSequenceContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<JavaScriptParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitExpressionSequence(this);
}

JavaScriptParser::ExpressionSequenceContext* JavaScriptParser::expressionSequence() {
  ExpressionSequenceContext *_localctx = _tracker.createInstance<ExpressionSequenceContext>(_ctx, getState());
  enterRule(_localctx, 82, JavaScriptParser::RuleExpressionSequence);
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
    setState(501);
    singleExpression(0);
    setState(506);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == JavaScriptParser::COMMA) {
      setState(502);
      match(JavaScriptParser::COMMA);
      setState(503);
      singleExpression(0);
      setState(508);
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
    case 25: return singleExpressionSempred(antlrcpp::downCast<SingleExpressionContext *>(context), predicateIndex);

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
  javascriptparserParserInitialize();
#else
  ::antlr4::internal::call_once(javascriptparserParserOnceFlag, javascriptparserParserInitialize);
#endif
}
