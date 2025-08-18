
#include "JavaScriptLexerBase.h"
#include "JavaScriptParserBase.h"
// Enable this macro definition when compiled to a static library
// #define ANTLR4CPP_STATIC

// TODO: add some include


// Generated from grammars/JavaScriptParser_cpp.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "JavaScriptParser_cpp.h"


/**
 * This interface defines an abstract listener for a parse tree produced by JavaScriptParser_cpp.
 */
class  JavaScriptParser_cppListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterProgram(JavaScriptParser_cpp::ProgramContext *ctx) = 0;
  virtual void exitProgram(JavaScriptParser_cpp::ProgramContext *ctx) = 0;

  virtual void enterSourceElement(JavaScriptParser_cpp::SourceElementContext *ctx) = 0;
  virtual void exitSourceElement(JavaScriptParser_cpp::SourceElementContext *ctx) = 0;

  virtual void enterStatement(JavaScriptParser_cpp::StatementContext *ctx) = 0;
  virtual void exitStatement(JavaScriptParser_cpp::StatementContext *ctx) = 0;

  virtual void enterBlock(JavaScriptParser_cpp::BlockContext *ctx) = 0;
  virtual void exitBlock(JavaScriptParser_cpp::BlockContext *ctx) = 0;

  virtual void enterStatementList(JavaScriptParser_cpp::StatementListContext *ctx) = 0;
  virtual void exitStatementList(JavaScriptParser_cpp::StatementListContext *ctx) = 0;

  virtual void enterImportStatement(JavaScriptParser_cpp::ImportStatementContext *ctx) = 0;
  virtual void exitImportStatement(JavaScriptParser_cpp::ImportStatementContext *ctx) = 0;

  virtual void enterImportFromBlock(JavaScriptParser_cpp::ImportFromBlockContext *ctx) = 0;
  virtual void exitImportFromBlock(JavaScriptParser_cpp::ImportFromBlockContext *ctx) = 0;

  virtual void enterImportModuleItems(JavaScriptParser_cpp::ImportModuleItemsContext *ctx) = 0;
  virtual void exitImportModuleItems(JavaScriptParser_cpp::ImportModuleItemsContext *ctx) = 0;

  virtual void enterImportAliasName(JavaScriptParser_cpp::ImportAliasNameContext *ctx) = 0;
  virtual void exitImportAliasName(JavaScriptParser_cpp::ImportAliasNameContext *ctx) = 0;

  virtual void enterModuleExportName(JavaScriptParser_cpp::ModuleExportNameContext *ctx) = 0;
  virtual void exitModuleExportName(JavaScriptParser_cpp::ModuleExportNameContext *ctx) = 0;

  virtual void enterImportedBinding(JavaScriptParser_cpp::ImportedBindingContext *ctx) = 0;
  virtual void exitImportedBinding(JavaScriptParser_cpp::ImportedBindingContext *ctx) = 0;

  virtual void enterImportDefault(JavaScriptParser_cpp::ImportDefaultContext *ctx) = 0;
  virtual void exitImportDefault(JavaScriptParser_cpp::ImportDefaultContext *ctx) = 0;

  virtual void enterImportNamespace(JavaScriptParser_cpp::ImportNamespaceContext *ctx) = 0;
  virtual void exitImportNamespace(JavaScriptParser_cpp::ImportNamespaceContext *ctx) = 0;

  virtual void enterImportFrom(JavaScriptParser_cpp::ImportFromContext *ctx) = 0;
  virtual void exitImportFrom(JavaScriptParser_cpp::ImportFromContext *ctx) = 0;

  virtual void enterAliasName(JavaScriptParser_cpp::AliasNameContext *ctx) = 0;
  virtual void exitAliasName(JavaScriptParser_cpp::AliasNameContext *ctx) = 0;

  virtual void enterExportDeclaration(JavaScriptParser_cpp::ExportDeclarationContext *ctx) = 0;
  virtual void exitExportDeclaration(JavaScriptParser_cpp::ExportDeclarationContext *ctx) = 0;

  virtual void enterExportDefaultDeclaration(JavaScriptParser_cpp::ExportDefaultDeclarationContext *ctx) = 0;
  virtual void exitExportDefaultDeclaration(JavaScriptParser_cpp::ExportDefaultDeclarationContext *ctx) = 0;

  virtual void enterExportFromBlock(JavaScriptParser_cpp::ExportFromBlockContext *ctx) = 0;
  virtual void exitExportFromBlock(JavaScriptParser_cpp::ExportFromBlockContext *ctx) = 0;

  virtual void enterExportModuleItems(JavaScriptParser_cpp::ExportModuleItemsContext *ctx) = 0;
  virtual void exitExportModuleItems(JavaScriptParser_cpp::ExportModuleItemsContext *ctx) = 0;

  virtual void enterExportAliasName(JavaScriptParser_cpp::ExportAliasNameContext *ctx) = 0;
  virtual void exitExportAliasName(JavaScriptParser_cpp::ExportAliasNameContext *ctx) = 0;

  virtual void enterDeclaration(JavaScriptParser_cpp::DeclarationContext *ctx) = 0;
  virtual void exitDeclaration(JavaScriptParser_cpp::DeclarationContext *ctx) = 0;

  virtual void enterVariableStatement(JavaScriptParser_cpp::VariableStatementContext *ctx) = 0;
  virtual void exitVariableStatement(JavaScriptParser_cpp::VariableStatementContext *ctx) = 0;

  virtual void enterVariableDeclarationList(JavaScriptParser_cpp::VariableDeclarationListContext *ctx) = 0;
  virtual void exitVariableDeclarationList(JavaScriptParser_cpp::VariableDeclarationListContext *ctx) = 0;

  virtual void enterVariableDeclaration(JavaScriptParser_cpp::VariableDeclarationContext *ctx) = 0;
  virtual void exitVariableDeclaration(JavaScriptParser_cpp::VariableDeclarationContext *ctx) = 0;

  virtual void enterEmptyStatement_(JavaScriptParser_cpp::EmptyStatement_Context *ctx) = 0;
  virtual void exitEmptyStatement_(JavaScriptParser_cpp::EmptyStatement_Context *ctx) = 0;

  virtual void enterExpressionStatement(JavaScriptParser_cpp::ExpressionStatementContext *ctx) = 0;
  virtual void exitExpressionStatement(JavaScriptParser_cpp::ExpressionStatementContext *ctx) = 0;

  virtual void enterIfStatement(JavaScriptParser_cpp::IfStatementContext *ctx) = 0;
  virtual void exitIfStatement(JavaScriptParser_cpp::IfStatementContext *ctx) = 0;

  virtual void enterDoStatement(JavaScriptParser_cpp::DoStatementContext *ctx) = 0;
  virtual void exitDoStatement(JavaScriptParser_cpp::DoStatementContext *ctx) = 0;

  virtual void enterWhileStatement(JavaScriptParser_cpp::WhileStatementContext *ctx) = 0;
  virtual void exitWhileStatement(JavaScriptParser_cpp::WhileStatementContext *ctx) = 0;

  virtual void enterForStatement(JavaScriptParser_cpp::ForStatementContext *ctx) = 0;
  virtual void exitForStatement(JavaScriptParser_cpp::ForStatementContext *ctx) = 0;

  virtual void enterForInStatement(JavaScriptParser_cpp::ForInStatementContext *ctx) = 0;
  virtual void exitForInStatement(JavaScriptParser_cpp::ForInStatementContext *ctx) = 0;

  virtual void enterForOfStatement(JavaScriptParser_cpp::ForOfStatementContext *ctx) = 0;
  virtual void exitForOfStatement(JavaScriptParser_cpp::ForOfStatementContext *ctx) = 0;

  virtual void enterVarModifier(JavaScriptParser_cpp::VarModifierContext *ctx) = 0;
  virtual void exitVarModifier(JavaScriptParser_cpp::VarModifierContext *ctx) = 0;

  virtual void enterContinueStatement(JavaScriptParser_cpp::ContinueStatementContext *ctx) = 0;
  virtual void exitContinueStatement(JavaScriptParser_cpp::ContinueStatementContext *ctx) = 0;

  virtual void enterBreakStatement(JavaScriptParser_cpp::BreakStatementContext *ctx) = 0;
  virtual void exitBreakStatement(JavaScriptParser_cpp::BreakStatementContext *ctx) = 0;

  virtual void enterReturnStatement(JavaScriptParser_cpp::ReturnStatementContext *ctx) = 0;
  virtual void exitReturnStatement(JavaScriptParser_cpp::ReturnStatementContext *ctx) = 0;

  virtual void enterYieldStatement(JavaScriptParser_cpp::YieldStatementContext *ctx) = 0;
  virtual void exitYieldStatement(JavaScriptParser_cpp::YieldStatementContext *ctx) = 0;

  virtual void enterWithStatement(JavaScriptParser_cpp::WithStatementContext *ctx) = 0;
  virtual void exitWithStatement(JavaScriptParser_cpp::WithStatementContext *ctx) = 0;

  virtual void enterSwitchStatement(JavaScriptParser_cpp::SwitchStatementContext *ctx) = 0;
  virtual void exitSwitchStatement(JavaScriptParser_cpp::SwitchStatementContext *ctx) = 0;

  virtual void enterCaseBlock(JavaScriptParser_cpp::CaseBlockContext *ctx) = 0;
  virtual void exitCaseBlock(JavaScriptParser_cpp::CaseBlockContext *ctx) = 0;

  virtual void enterCaseClauses(JavaScriptParser_cpp::CaseClausesContext *ctx) = 0;
  virtual void exitCaseClauses(JavaScriptParser_cpp::CaseClausesContext *ctx) = 0;

  virtual void enterCaseClause(JavaScriptParser_cpp::CaseClauseContext *ctx) = 0;
  virtual void exitCaseClause(JavaScriptParser_cpp::CaseClauseContext *ctx) = 0;

  virtual void enterDefaultClause(JavaScriptParser_cpp::DefaultClauseContext *ctx) = 0;
  virtual void exitDefaultClause(JavaScriptParser_cpp::DefaultClauseContext *ctx) = 0;

  virtual void enterLabelledStatement(JavaScriptParser_cpp::LabelledStatementContext *ctx) = 0;
  virtual void exitLabelledStatement(JavaScriptParser_cpp::LabelledStatementContext *ctx) = 0;

  virtual void enterThrowStatement(JavaScriptParser_cpp::ThrowStatementContext *ctx) = 0;
  virtual void exitThrowStatement(JavaScriptParser_cpp::ThrowStatementContext *ctx) = 0;

  virtual void enterTryStatement(JavaScriptParser_cpp::TryStatementContext *ctx) = 0;
  virtual void exitTryStatement(JavaScriptParser_cpp::TryStatementContext *ctx) = 0;

  virtual void enterCatchProduction(JavaScriptParser_cpp::CatchProductionContext *ctx) = 0;
  virtual void exitCatchProduction(JavaScriptParser_cpp::CatchProductionContext *ctx) = 0;

  virtual void enterFinallyProduction(JavaScriptParser_cpp::FinallyProductionContext *ctx) = 0;
  virtual void exitFinallyProduction(JavaScriptParser_cpp::FinallyProductionContext *ctx) = 0;

  virtual void enterDebuggerStatement(JavaScriptParser_cpp::DebuggerStatementContext *ctx) = 0;
  virtual void exitDebuggerStatement(JavaScriptParser_cpp::DebuggerStatementContext *ctx) = 0;

  virtual void enterFunctionDeclaration(JavaScriptParser_cpp::FunctionDeclarationContext *ctx) = 0;
  virtual void exitFunctionDeclaration(JavaScriptParser_cpp::FunctionDeclarationContext *ctx) = 0;

  virtual void enterClassDeclaration(JavaScriptParser_cpp::ClassDeclarationContext *ctx) = 0;
  virtual void exitClassDeclaration(JavaScriptParser_cpp::ClassDeclarationContext *ctx) = 0;

  virtual void enterClassTail(JavaScriptParser_cpp::ClassTailContext *ctx) = 0;
  virtual void exitClassTail(JavaScriptParser_cpp::ClassTailContext *ctx) = 0;

  virtual void enterClassElement(JavaScriptParser_cpp::ClassElementContext *ctx) = 0;
  virtual void exitClassElement(JavaScriptParser_cpp::ClassElementContext *ctx) = 0;

  virtual void enterMethodDefinition(JavaScriptParser_cpp::MethodDefinitionContext *ctx) = 0;
  virtual void exitMethodDefinition(JavaScriptParser_cpp::MethodDefinitionContext *ctx) = 0;

  virtual void enterFieldDefinition(JavaScriptParser_cpp::FieldDefinitionContext *ctx) = 0;
  virtual void exitFieldDefinition(JavaScriptParser_cpp::FieldDefinitionContext *ctx) = 0;

  virtual void enterClassElementName(JavaScriptParser_cpp::ClassElementNameContext *ctx) = 0;
  virtual void exitClassElementName(JavaScriptParser_cpp::ClassElementNameContext *ctx) = 0;

  virtual void enterPrivateIdentifier(JavaScriptParser_cpp::PrivateIdentifierContext *ctx) = 0;
  virtual void exitPrivateIdentifier(JavaScriptParser_cpp::PrivateIdentifierContext *ctx) = 0;

  virtual void enterFormalParameterList(JavaScriptParser_cpp::FormalParameterListContext *ctx) = 0;
  virtual void exitFormalParameterList(JavaScriptParser_cpp::FormalParameterListContext *ctx) = 0;

  virtual void enterFormalParameterArg(JavaScriptParser_cpp::FormalParameterArgContext *ctx) = 0;
  virtual void exitFormalParameterArg(JavaScriptParser_cpp::FormalParameterArgContext *ctx) = 0;

  virtual void enterLastFormalParameterArg(JavaScriptParser_cpp::LastFormalParameterArgContext *ctx) = 0;
  virtual void exitLastFormalParameterArg(JavaScriptParser_cpp::LastFormalParameterArgContext *ctx) = 0;

  virtual void enterFunctionBody(JavaScriptParser_cpp::FunctionBodyContext *ctx) = 0;
  virtual void exitFunctionBody(JavaScriptParser_cpp::FunctionBodyContext *ctx) = 0;

  virtual void enterSourceElements(JavaScriptParser_cpp::SourceElementsContext *ctx) = 0;
  virtual void exitSourceElements(JavaScriptParser_cpp::SourceElementsContext *ctx) = 0;

  virtual void enterArrayLiteral(JavaScriptParser_cpp::ArrayLiteralContext *ctx) = 0;
  virtual void exitArrayLiteral(JavaScriptParser_cpp::ArrayLiteralContext *ctx) = 0;

  virtual void enterElementList(JavaScriptParser_cpp::ElementListContext *ctx) = 0;
  virtual void exitElementList(JavaScriptParser_cpp::ElementListContext *ctx) = 0;

  virtual void enterArrayElement(JavaScriptParser_cpp::ArrayElementContext *ctx) = 0;
  virtual void exitArrayElement(JavaScriptParser_cpp::ArrayElementContext *ctx) = 0;

  virtual void enterPropertyExpressionAssignment(JavaScriptParser_cpp::PropertyExpressionAssignmentContext *ctx) = 0;
  virtual void exitPropertyExpressionAssignment(JavaScriptParser_cpp::PropertyExpressionAssignmentContext *ctx) = 0;

  virtual void enterComputedPropertyExpressionAssignment(JavaScriptParser_cpp::ComputedPropertyExpressionAssignmentContext *ctx) = 0;
  virtual void exitComputedPropertyExpressionAssignment(JavaScriptParser_cpp::ComputedPropertyExpressionAssignmentContext *ctx) = 0;

  virtual void enterFunctionProperty(JavaScriptParser_cpp::FunctionPropertyContext *ctx) = 0;
  virtual void exitFunctionProperty(JavaScriptParser_cpp::FunctionPropertyContext *ctx) = 0;

  virtual void enterPropertyGetter(JavaScriptParser_cpp::PropertyGetterContext *ctx) = 0;
  virtual void exitPropertyGetter(JavaScriptParser_cpp::PropertyGetterContext *ctx) = 0;

  virtual void enterPropertySetter(JavaScriptParser_cpp::PropertySetterContext *ctx) = 0;
  virtual void exitPropertySetter(JavaScriptParser_cpp::PropertySetterContext *ctx) = 0;

  virtual void enterPropertyShorthand(JavaScriptParser_cpp::PropertyShorthandContext *ctx) = 0;
  virtual void exitPropertyShorthand(JavaScriptParser_cpp::PropertyShorthandContext *ctx) = 0;

  virtual void enterPropertyName(JavaScriptParser_cpp::PropertyNameContext *ctx) = 0;
  virtual void exitPropertyName(JavaScriptParser_cpp::PropertyNameContext *ctx) = 0;

  virtual void enterArguments(JavaScriptParser_cpp::ArgumentsContext *ctx) = 0;
  virtual void exitArguments(JavaScriptParser_cpp::ArgumentsContext *ctx) = 0;

  virtual void enterArgument(JavaScriptParser_cpp::ArgumentContext *ctx) = 0;
  virtual void exitArgument(JavaScriptParser_cpp::ArgumentContext *ctx) = 0;

  virtual void enterExpressionSequence(JavaScriptParser_cpp::ExpressionSequenceContext *ctx) = 0;
  virtual void exitExpressionSequence(JavaScriptParser_cpp::ExpressionSequenceContext *ctx) = 0;

  virtual void enterTemplateStringExpression(JavaScriptParser_cpp::TemplateStringExpressionContext *ctx) = 0;
  virtual void exitTemplateStringExpression(JavaScriptParser_cpp::TemplateStringExpressionContext *ctx) = 0;

  virtual void enterTernaryExpression(JavaScriptParser_cpp::TernaryExpressionContext *ctx) = 0;
  virtual void exitTernaryExpression(JavaScriptParser_cpp::TernaryExpressionContext *ctx) = 0;

  virtual void enterLogicalAndExpression(JavaScriptParser_cpp::LogicalAndExpressionContext *ctx) = 0;
  virtual void exitLogicalAndExpression(JavaScriptParser_cpp::LogicalAndExpressionContext *ctx) = 0;

  virtual void enterPowerExpression(JavaScriptParser_cpp::PowerExpressionContext *ctx) = 0;
  virtual void exitPowerExpression(JavaScriptParser_cpp::PowerExpressionContext *ctx) = 0;

  virtual void enterPreIncrementExpression(JavaScriptParser_cpp::PreIncrementExpressionContext *ctx) = 0;
  virtual void exitPreIncrementExpression(JavaScriptParser_cpp::PreIncrementExpressionContext *ctx) = 0;

  virtual void enterObjectLiteralExpression(JavaScriptParser_cpp::ObjectLiteralExpressionContext *ctx) = 0;
  virtual void exitObjectLiteralExpression(JavaScriptParser_cpp::ObjectLiteralExpressionContext *ctx) = 0;

  virtual void enterMetaExpression(JavaScriptParser_cpp::MetaExpressionContext *ctx) = 0;
  virtual void exitMetaExpression(JavaScriptParser_cpp::MetaExpressionContext *ctx) = 0;

  virtual void enterInExpression(JavaScriptParser_cpp::InExpressionContext *ctx) = 0;
  virtual void exitInExpression(JavaScriptParser_cpp::InExpressionContext *ctx) = 0;

  virtual void enterLogicalOrExpression(JavaScriptParser_cpp::LogicalOrExpressionContext *ctx) = 0;
  virtual void exitLogicalOrExpression(JavaScriptParser_cpp::LogicalOrExpressionContext *ctx) = 0;

  virtual void enterOptionalChainExpression(JavaScriptParser_cpp::OptionalChainExpressionContext *ctx) = 0;
  virtual void exitOptionalChainExpression(JavaScriptParser_cpp::OptionalChainExpressionContext *ctx) = 0;

  virtual void enterNotExpression(JavaScriptParser_cpp::NotExpressionContext *ctx) = 0;
  virtual void exitNotExpression(JavaScriptParser_cpp::NotExpressionContext *ctx) = 0;

  virtual void enterPreDecreaseExpression(JavaScriptParser_cpp::PreDecreaseExpressionContext *ctx) = 0;
  virtual void exitPreDecreaseExpression(JavaScriptParser_cpp::PreDecreaseExpressionContext *ctx) = 0;

  virtual void enterArgumentsExpression(JavaScriptParser_cpp::ArgumentsExpressionContext *ctx) = 0;
  virtual void exitArgumentsExpression(JavaScriptParser_cpp::ArgumentsExpressionContext *ctx) = 0;

  virtual void enterAwaitExpression(JavaScriptParser_cpp::AwaitExpressionContext *ctx) = 0;
  virtual void exitAwaitExpression(JavaScriptParser_cpp::AwaitExpressionContext *ctx) = 0;

  virtual void enterThisExpression(JavaScriptParser_cpp::ThisExpressionContext *ctx) = 0;
  virtual void exitThisExpression(JavaScriptParser_cpp::ThisExpressionContext *ctx) = 0;

  virtual void enterFunctionExpression(JavaScriptParser_cpp::FunctionExpressionContext *ctx) = 0;
  virtual void exitFunctionExpression(JavaScriptParser_cpp::FunctionExpressionContext *ctx) = 0;

  virtual void enterUnaryMinusExpression(JavaScriptParser_cpp::UnaryMinusExpressionContext *ctx) = 0;
  virtual void exitUnaryMinusExpression(JavaScriptParser_cpp::UnaryMinusExpressionContext *ctx) = 0;

  virtual void enterAssignmentExpression(JavaScriptParser_cpp::AssignmentExpressionContext *ctx) = 0;
  virtual void exitAssignmentExpression(JavaScriptParser_cpp::AssignmentExpressionContext *ctx) = 0;

  virtual void enterPostDecreaseExpression(JavaScriptParser_cpp::PostDecreaseExpressionContext *ctx) = 0;
  virtual void exitPostDecreaseExpression(JavaScriptParser_cpp::PostDecreaseExpressionContext *ctx) = 0;

  virtual void enterTypeofExpression(JavaScriptParser_cpp::TypeofExpressionContext *ctx) = 0;
  virtual void exitTypeofExpression(JavaScriptParser_cpp::TypeofExpressionContext *ctx) = 0;

  virtual void enterInstanceofExpression(JavaScriptParser_cpp::InstanceofExpressionContext *ctx) = 0;
  virtual void exitInstanceofExpression(JavaScriptParser_cpp::InstanceofExpressionContext *ctx) = 0;

  virtual void enterUnaryPlusExpression(JavaScriptParser_cpp::UnaryPlusExpressionContext *ctx) = 0;
  virtual void exitUnaryPlusExpression(JavaScriptParser_cpp::UnaryPlusExpressionContext *ctx) = 0;

  virtual void enterDeleteExpression(JavaScriptParser_cpp::DeleteExpressionContext *ctx) = 0;
  virtual void exitDeleteExpression(JavaScriptParser_cpp::DeleteExpressionContext *ctx) = 0;

  virtual void enterImportExpression(JavaScriptParser_cpp::ImportExpressionContext *ctx) = 0;
  virtual void exitImportExpression(JavaScriptParser_cpp::ImportExpressionContext *ctx) = 0;

  virtual void enterEqualityExpression(JavaScriptParser_cpp::EqualityExpressionContext *ctx) = 0;
  virtual void exitEqualityExpression(JavaScriptParser_cpp::EqualityExpressionContext *ctx) = 0;

  virtual void enterBitXOrExpression(JavaScriptParser_cpp::BitXOrExpressionContext *ctx) = 0;
  virtual void exitBitXOrExpression(JavaScriptParser_cpp::BitXOrExpressionContext *ctx) = 0;

  virtual void enterSuperExpression(JavaScriptParser_cpp::SuperExpressionContext *ctx) = 0;
  virtual void exitSuperExpression(JavaScriptParser_cpp::SuperExpressionContext *ctx) = 0;

  virtual void enterMultiplicativeExpression(JavaScriptParser_cpp::MultiplicativeExpressionContext *ctx) = 0;
  virtual void exitMultiplicativeExpression(JavaScriptParser_cpp::MultiplicativeExpressionContext *ctx) = 0;

  virtual void enterBitShiftExpression(JavaScriptParser_cpp::BitShiftExpressionContext *ctx) = 0;
  virtual void exitBitShiftExpression(JavaScriptParser_cpp::BitShiftExpressionContext *ctx) = 0;

  virtual void enterParenthesizedExpression(JavaScriptParser_cpp::ParenthesizedExpressionContext *ctx) = 0;
  virtual void exitParenthesizedExpression(JavaScriptParser_cpp::ParenthesizedExpressionContext *ctx) = 0;

  virtual void enterAdditiveExpression(JavaScriptParser_cpp::AdditiveExpressionContext *ctx) = 0;
  virtual void exitAdditiveExpression(JavaScriptParser_cpp::AdditiveExpressionContext *ctx) = 0;

  virtual void enterRelationalExpression(JavaScriptParser_cpp::RelationalExpressionContext *ctx) = 0;
  virtual void exitRelationalExpression(JavaScriptParser_cpp::RelationalExpressionContext *ctx) = 0;

  virtual void enterPostIncrementExpression(JavaScriptParser_cpp::PostIncrementExpressionContext *ctx) = 0;
  virtual void exitPostIncrementExpression(JavaScriptParser_cpp::PostIncrementExpressionContext *ctx) = 0;

  virtual void enterYieldExpression(JavaScriptParser_cpp::YieldExpressionContext *ctx) = 0;
  virtual void exitYieldExpression(JavaScriptParser_cpp::YieldExpressionContext *ctx) = 0;

  virtual void enterBitNotExpression(JavaScriptParser_cpp::BitNotExpressionContext *ctx) = 0;
  virtual void exitBitNotExpression(JavaScriptParser_cpp::BitNotExpressionContext *ctx) = 0;

  virtual void enterNewExpression(JavaScriptParser_cpp::NewExpressionContext *ctx) = 0;
  virtual void exitNewExpression(JavaScriptParser_cpp::NewExpressionContext *ctx) = 0;

  virtual void enterLiteralExpression(JavaScriptParser_cpp::LiteralExpressionContext *ctx) = 0;
  virtual void exitLiteralExpression(JavaScriptParser_cpp::LiteralExpressionContext *ctx) = 0;

  virtual void enterArrayLiteralExpression(JavaScriptParser_cpp::ArrayLiteralExpressionContext *ctx) = 0;
  virtual void exitArrayLiteralExpression(JavaScriptParser_cpp::ArrayLiteralExpressionContext *ctx) = 0;

  virtual void enterMemberDotExpression(JavaScriptParser_cpp::MemberDotExpressionContext *ctx) = 0;
  virtual void exitMemberDotExpression(JavaScriptParser_cpp::MemberDotExpressionContext *ctx) = 0;

  virtual void enterClassExpression(JavaScriptParser_cpp::ClassExpressionContext *ctx) = 0;
  virtual void exitClassExpression(JavaScriptParser_cpp::ClassExpressionContext *ctx) = 0;

  virtual void enterMemberIndexExpression(JavaScriptParser_cpp::MemberIndexExpressionContext *ctx) = 0;
  virtual void exitMemberIndexExpression(JavaScriptParser_cpp::MemberIndexExpressionContext *ctx) = 0;

  virtual void enterIdentifierExpression(JavaScriptParser_cpp::IdentifierExpressionContext *ctx) = 0;
  virtual void exitIdentifierExpression(JavaScriptParser_cpp::IdentifierExpressionContext *ctx) = 0;

  virtual void enterBitAndExpression(JavaScriptParser_cpp::BitAndExpressionContext *ctx) = 0;
  virtual void exitBitAndExpression(JavaScriptParser_cpp::BitAndExpressionContext *ctx) = 0;

  virtual void enterBitOrExpression(JavaScriptParser_cpp::BitOrExpressionContext *ctx) = 0;
  virtual void exitBitOrExpression(JavaScriptParser_cpp::BitOrExpressionContext *ctx) = 0;

  virtual void enterAssignmentOperatorExpression(JavaScriptParser_cpp::AssignmentOperatorExpressionContext *ctx) = 0;
  virtual void exitAssignmentOperatorExpression(JavaScriptParser_cpp::AssignmentOperatorExpressionContext *ctx) = 0;

  virtual void enterVoidExpression(JavaScriptParser_cpp::VoidExpressionContext *ctx) = 0;
  virtual void exitVoidExpression(JavaScriptParser_cpp::VoidExpressionContext *ctx) = 0;

  virtual void enterCoalesceExpression(JavaScriptParser_cpp::CoalesceExpressionContext *ctx) = 0;
  virtual void exitCoalesceExpression(JavaScriptParser_cpp::CoalesceExpressionContext *ctx) = 0;

  virtual void enterInitializer(JavaScriptParser_cpp::InitializerContext *ctx) = 0;
  virtual void exitInitializer(JavaScriptParser_cpp::InitializerContext *ctx) = 0;

  virtual void enterAssignable(JavaScriptParser_cpp::AssignableContext *ctx) = 0;
  virtual void exitAssignable(JavaScriptParser_cpp::AssignableContext *ctx) = 0;

  virtual void enterObjectLiteral(JavaScriptParser_cpp::ObjectLiteralContext *ctx) = 0;
  virtual void exitObjectLiteral(JavaScriptParser_cpp::ObjectLiteralContext *ctx) = 0;

  virtual void enterNamedFunction(JavaScriptParser_cpp::NamedFunctionContext *ctx) = 0;
  virtual void exitNamedFunction(JavaScriptParser_cpp::NamedFunctionContext *ctx) = 0;

  virtual void enterAnonymousFunctionDecl(JavaScriptParser_cpp::AnonymousFunctionDeclContext *ctx) = 0;
  virtual void exitAnonymousFunctionDecl(JavaScriptParser_cpp::AnonymousFunctionDeclContext *ctx) = 0;

  virtual void enterArrowFunction(JavaScriptParser_cpp::ArrowFunctionContext *ctx) = 0;
  virtual void exitArrowFunction(JavaScriptParser_cpp::ArrowFunctionContext *ctx) = 0;

  virtual void enterArrowFunctionParameters(JavaScriptParser_cpp::ArrowFunctionParametersContext *ctx) = 0;
  virtual void exitArrowFunctionParameters(JavaScriptParser_cpp::ArrowFunctionParametersContext *ctx) = 0;

  virtual void enterArrowFunctionBody(JavaScriptParser_cpp::ArrowFunctionBodyContext *ctx) = 0;
  virtual void exitArrowFunctionBody(JavaScriptParser_cpp::ArrowFunctionBodyContext *ctx) = 0;

  virtual void enterAssignmentOperator(JavaScriptParser_cpp::AssignmentOperatorContext *ctx) = 0;
  virtual void exitAssignmentOperator(JavaScriptParser_cpp::AssignmentOperatorContext *ctx) = 0;

  virtual void enterLiteral(JavaScriptParser_cpp::LiteralContext *ctx) = 0;
  virtual void exitLiteral(JavaScriptParser_cpp::LiteralContext *ctx) = 0;

  virtual void enterTemplateStringLiteral(JavaScriptParser_cpp::TemplateStringLiteralContext *ctx) = 0;
  virtual void exitTemplateStringLiteral(JavaScriptParser_cpp::TemplateStringLiteralContext *ctx) = 0;

  virtual void enterTemplateStringAtom(JavaScriptParser_cpp::TemplateStringAtomContext *ctx) = 0;
  virtual void exitTemplateStringAtom(JavaScriptParser_cpp::TemplateStringAtomContext *ctx) = 0;

  virtual void enterNumericLiteral(JavaScriptParser_cpp::NumericLiteralContext *ctx) = 0;
  virtual void exitNumericLiteral(JavaScriptParser_cpp::NumericLiteralContext *ctx) = 0;

  virtual void enterBigintLiteral(JavaScriptParser_cpp::BigintLiteralContext *ctx) = 0;
  virtual void exitBigintLiteral(JavaScriptParser_cpp::BigintLiteralContext *ctx) = 0;

  virtual void enterGetter(JavaScriptParser_cpp::GetterContext *ctx) = 0;
  virtual void exitGetter(JavaScriptParser_cpp::GetterContext *ctx) = 0;

  virtual void enterSetter(JavaScriptParser_cpp::SetterContext *ctx) = 0;
  virtual void exitSetter(JavaScriptParser_cpp::SetterContext *ctx) = 0;

  virtual void enterIdentifierName(JavaScriptParser_cpp::IdentifierNameContext *ctx) = 0;
  virtual void exitIdentifierName(JavaScriptParser_cpp::IdentifierNameContext *ctx) = 0;

  virtual void enterIdentifier(JavaScriptParser_cpp::IdentifierContext *ctx) = 0;
  virtual void exitIdentifier(JavaScriptParser_cpp::IdentifierContext *ctx) = 0;

  virtual void enterReservedWord(JavaScriptParser_cpp::ReservedWordContext *ctx) = 0;
  virtual void exitReservedWord(JavaScriptParser_cpp::ReservedWordContext *ctx) = 0;

  virtual void enterKeyword(JavaScriptParser_cpp::KeywordContext *ctx) = 0;
  virtual void exitKeyword(JavaScriptParser_cpp::KeywordContext *ctx) = 0;

  virtual void enterLet_(JavaScriptParser_cpp::Let_Context *ctx) = 0;
  virtual void exitLet_(JavaScriptParser_cpp::Let_Context *ctx) = 0;

  virtual void enterEos(JavaScriptParser_cpp::EosContext *ctx) = 0;
  virtual void exitEos(JavaScriptParser_cpp::EosContext *ctx) = 0;


};

