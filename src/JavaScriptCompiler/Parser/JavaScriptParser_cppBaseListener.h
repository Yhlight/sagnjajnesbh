
#include "JavaScriptLexerBase.h"
#include "JavaScriptParserBase.h"



// Generated from JavaScriptParser_cpp.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "JavaScriptParser_cppListener.h"


/**
 * This class provides an empty implementation of JavaScriptParser_cppListener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class  JavaScriptParser_cppBaseListener : public JavaScriptParser_cppListener {
public:

  virtual void enterProgram(JavaScriptParser_cpp::ProgramContext * /*ctx*/) override { }
  virtual void exitProgram(JavaScriptParser_cpp::ProgramContext * /*ctx*/) override { }

  virtual void enterSourceElement(JavaScriptParser_cpp::SourceElementContext * /*ctx*/) override { }
  virtual void exitSourceElement(JavaScriptParser_cpp::SourceElementContext * /*ctx*/) override { }

  virtual void enterStatement(JavaScriptParser_cpp::StatementContext * /*ctx*/) override { }
  virtual void exitStatement(JavaScriptParser_cpp::StatementContext * /*ctx*/) override { }

  virtual void enterBlock(JavaScriptParser_cpp::BlockContext * /*ctx*/) override { }
  virtual void exitBlock(JavaScriptParser_cpp::BlockContext * /*ctx*/) override { }

  virtual void enterStatementList(JavaScriptParser_cpp::StatementListContext * /*ctx*/) override { }
  virtual void exitStatementList(JavaScriptParser_cpp::StatementListContext * /*ctx*/) override { }

  virtual void enterImportStatement(JavaScriptParser_cpp::ImportStatementContext * /*ctx*/) override { }
  virtual void exitImportStatement(JavaScriptParser_cpp::ImportStatementContext * /*ctx*/) override { }

  virtual void enterImportFromBlock(JavaScriptParser_cpp::ImportFromBlockContext * /*ctx*/) override { }
  virtual void exitImportFromBlock(JavaScriptParser_cpp::ImportFromBlockContext * /*ctx*/) override { }

  virtual void enterImportModuleItems(JavaScriptParser_cpp::ImportModuleItemsContext * /*ctx*/) override { }
  virtual void exitImportModuleItems(JavaScriptParser_cpp::ImportModuleItemsContext * /*ctx*/) override { }

  virtual void enterImportAliasName(JavaScriptParser_cpp::ImportAliasNameContext * /*ctx*/) override { }
  virtual void exitImportAliasName(JavaScriptParser_cpp::ImportAliasNameContext * /*ctx*/) override { }

  virtual void enterModuleExportName(JavaScriptParser_cpp::ModuleExportNameContext * /*ctx*/) override { }
  virtual void exitModuleExportName(JavaScriptParser_cpp::ModuleExportNameContext * /*ctx*/) override { }

  virtual void enterImportedBinding(JavaScriptParser_cpp::ImportedBindingContext * /*ctx*/) override { }
  virtual void exitImportedBinding(JavaScriptParser_cpp::ImportedBindingContext * /*ctx*/) override { }

  virtual void enterImportDefault(JavaScriptParser_cpp::ImportDefaultContext * /*ctx*/) override { }
  virtual void exitImportDefault(JavaScriptParser_cpp::ImportDefaultContext * /*ctx*/) override { }

  virtual void enterImportNamespace(JavaScriptParser_cpp::ImportNamespaceContext * /*ctx*/) override { }
  virtual void exitImportNamespace(JavaScriptParser_cpp::ImportNamespaceContext * /*ctx*/) override { }

  virtual void enterImportFrom(JavaScriptParser_cpp::ImportFromContext * /*ctx*/) override { }
  virtual void exitImportFrom(JavaScriptParser_cpp::ImportFromContext * /*ctx*/) override { }

  virtual void enterAliasName(JavaScriptParser_cpp::AliasNameContext * /*ctx*/) override { }
  virtual void exitAliasName(JavaScriptParser_cpp::AliasNameContext * /*ctx*/) override { }

  virtual void enterExportDeclaration(JavaScriptParser_cpp::ExportDeclarationContext * /*ctx*/) override { }
  virtual void exitExportDeclaration(JavaScriptParser_cpp::ExportDeclarationContext * /*ctx*/) override { }

  virtual void enterExportDefaultDeclaration(JavaScriptParser_cpp::ExportDefaultDeclarationContext * /*ctx*/) override { }
  virtual void exitExportDefaultDeclaration(JavaScriptParser_cpp::ExportDefaultDeclarationContext * /*ctx*/) override { }

  virtual void enterExportFromBlock(JavaScriptParser_cpp::ExportFromBlockContext * /*ctx*/) override { }
  virtual void exitExportFromBlock(JavaScriptParser_cpp::ExportFromBlockContext * /*ctx*/) override { }

  virtual void enterExportModuleItems(JavaScriptParser_cpp::ExportModuleItemsContext * /*ctx*/) override { }
  virtual void exitExportModuleItems(JavaScriptParser_cpp::ExportModuleItemsContext * /*ctx*/) override { }

  virtual void enterExportAliasName(JavaScriptParser_cpp::ExportAliasNameContext * /*ctx*/) override { }
  virtual void exitExportAliasName(JavaScriptParser_cpp::ExportAliasNameContext * /*ctx*/) override { }

  virtual void enterDeclaration(JavaScriptParser_cpp::DeclarationContext * /*ctx*/) override { }
  virtual void exitDeclaration(JavaScriptParser_cpp::DeclarationContext * /*ctx*/) override { }

  virtual void enterVariableStatement(JavaScriptParser_cpp::VariableStatementContext * /*ctx*/) override { }
  virtual void exitVariableStatement(JavaScriptParser_cpp::VariableStatementContext * /*ctx*/) override { }

  virtual void enterVariableDeclarationList(JavaScriptParser_cpp::VariableDeclarationListContext * /*ctx*/) override { }
  virtual void exitVariableDeclarationList(JavaScriptParser_cpp::VariableDeclarationListContext * /*ctx*/) override { }

  virtual void enterVariableDeclaration(JavaScriptParser_cpp::VariableDeclarationContext * /*ctx*/) override { }
  virtual void exitVariableDeclaration(JavaScriptParser_cpp::VariableDeclarationContext * /*ctx*/) override { }

  virtual void enterEmptyStatement_(JavaScriptParser_cpp::EmptyStatement_Context * /*ctx*/) override { }
  virtual void exitEmptyStatement_(JavaScriptParser_cpp::EmptyStatement_Context * /*ctx*/) override { }

  virtual void enterExpressionStatement(JavaScriptParser_cpp::ExpressionStatementContext * /*ctx*/) override { }
  virtual void exitExpressionStatement(JavaScriptParser_cpp::ExpressionStatementContext * /*ctx*/) override { }

  virtual void enterIfStatement(JavaScriptParser_cpp::IfStatementContext * /*ctx*/) override { }
  virtual void exitIfStatement(JavaScriptParser_cpp::IfStatementContext * /*ctx*/) override { }

  virtual void enterDoStatement(JavaScriptParser_cpp::DoStatementContext * /*ctx*/) override { }
  virtual void exitDoStatement(JavaScriptParser_cpp::DoStatementContext * /*ctx*/) override { }

  virtual void enterWhileStatement(JavaScriptParser_cpp::WhileStatementContext * /*ctx*/) override { }
  virtual void exitWhileStatement(JavaScriptParser_cpp::WhileStatementContext * /*ctx*/) override { }

  virtual void enterForStatement(JavaScriptParser_cpp::ForStatementContext * /*ctx*/) override { }
  virtual void exitForStatement(JavaScriptParser_cpp::ForStatementContext * /*ctx*/) override { }

  virtual void enterForInStatement(JavaScriptParser_cpp::ForInStatementContext * /*ctx*/) override { }
  virtual void exitForInStatement(JavaScriptParser_cpp::ForInStatementContext * /*ctx*/) override { }

  virtual void enterForOfStatement(JavaScriptParser_cpp::ForOfStatementContext * /*ctx*/) override { }
  virtual void exitForOfStatement(JavaScriptParser_cpp::ForOfStatementContext * /*ctx*/) override { }

  virtual void enterVarModifier(JavaScriptParser_cpp::VarModifierContext * /*ctx*/) override { }
  virtual void exitVarModifier(JavaScriptParser_cpp::VarModifierContext * /*ctx*/) override { }

  virtual void enterContinueStatement(JavaScriptParser_cpp::ContinueStatementContext * /*ctx*/) override { }
  virtual void exitContinueStatement(JavaScriptParser_cpp::ContinueStatementContext * /*ctx*/) override { }

  virtual void enterBreakStatement(JavaScriptParser_cpp::BreakStatementContext * /*ctx*/) override { }
  virtual void exitBreakStatement(JavaScriptParser_cpp::BreakStatementContext * /*ctx*/) override { }

  virtual void enterReturnStatement(JavaScriptParser_cpp::ReturnStatementContext * /*ctx*/) override { }
  virtual void exitReturnStatement(JavaScriptParser_cpp::ReturnStatementContext * /*ctx*/) override { }

  virtual void enterYieldStatement(JavaScriptParser_cpp::YieldStatementContext * /*ctx*/) override { }
  virtual void exitYieldStatement(JavaScriptParser_cpp::YieldStatementContext * /*ctx*/) override { }

  virtual void enterWithStatement(JavaScriptParser_cpp::WithStatementContext * /*ctx*/) override { }
  virtual void exitWithStatement(JavaScriptParser_cpp::WithStatementContext * /*ctx*/) override { }

  virtual void enterSwitchStatement(JavaScriptParser_cpp::SwitchStatementContext * /*ctx*/) override { }
  virtual void exitSwitchStatement(JavaScriptParser_cpp::SwitchStatementContext * /*ctx*/) override { }

  virtual void enterCaseBlock(JavaScriptParser_cpp::CaseBlockContext * /*ctx*/) override { }
  virtual void exitCaseBlock(JavaScriptParser_cpp::CaseBlockContext * /*ctx*/) override { }

  virtual void enterCaseClauses(JavaScriptParser_cpp::CaseClausesContext * /*ctx*/) override { }
  virtual void exitCaseClauses(JavaScriptParser_cpp::CaseClausesContext * /*ctx*/) override { }

  virtual void enterCaseClause(JavaScriptParser_cpp::CaseClauseContext * /*ctx*/) override { }
  virtual void exitCaseClause(JavaScriptParser_cpp::CaseClauseContext * /*ctx*/) override { }

  virtual void enterDefaultClause(JavaScriptParser_cpp::DefaultClauseContext * /*ctx*/) override { }
  virtual void exitDefaultClause(JavaScriptParser_cpp::DefaultClauseContext * /*ctx*/) override { }

  virtual void enterLabelledStatement(JavaScriptParser_cpp::LabelledStatementContext * /*ctx*/) override { }
  virtual void exitLabelledStatement(JavaScriptParser_cpp::LabelledStatementContext * /*ctx*/) override { }

  virtual void enterThrowStatement(JavaScriptParser_cpp::ThrowStatementContext * /*ctx*/) override { }
  virtual void exitThrowStatement(JavaScriptParser_cpp::ThrowStatementContext * /*ctx*/) override { }

  virtual void enterTryStatement(JavaScriptParser_cpp::TryStatementContext * /*ctx*/) override { }
  virtual void exitTryStatement(JavaScriptParser_cpp::TryStatementContext * /*ctx*/) override { }

  virtual void enterCatchProduction(JavaScriptParser_cpp::CatchProductionContext * /*ctx*/) override { }
  virtual void exitCatchProduction(JavaScriptParser_cpp::CatchProductionContext * /*ctx*/) override { }

  virtual void enterFinallyProduction(JavaScriptParser_cpp::FinallyProductionContext * /*ctx*/) override { }
  virtual void exitFinallyProduction(JavaScriptParser_cpp::FinallyProductionContext * /*ctx*/) override { }

  virtual void enterDebuggerStatement(JavaScriptParser_cpp::DebuggerStatementContext * /*ctx*/) override { }
  virtual void exitDebuggerStatement(JavaScriptParser_cpp::DebuggerStatementContext * /*ctx*/) override { }

  virtual void enterFunctionDeclaration(JavaScriptParser_cpp::FunctionDeclarationContext * /*ctx*/) override { }
  virtual void exitFunctionDeclaration(JavaScriptParser_cpp::FunctionDeclarationContext * /*ctx*/) override { }

  virtual void enterClassDeclaration(JavaScriptParser_cpp::ClassDeclarationContext * /*ctx*/) override { }
  virtual void exitClassDeclaration(JavaScriptParser_cpp::ClassDeclarationContext * /*ctx*/) override { }

  virtual void enterClassTail(JavaScriptParser_cpp::ClassTailContext * /*ctx*/) override { }
  virtual void exitClassTail(JavaScriptParser_cpp::ClassTailContext * /*ctx*/) override { }

  virtual void enterClassElement(JavaScriptParser_cpp::ClassElementContext * /*ctx*/) override { }
  virtual void exitClassElement(JavaScriptParser_cpp::ClassElementContext * /*ctx*/) override { }

  virtual void enterMethodDefinition(JavaScriptParser_cpp::MethodDefinitionContext * /*ctx*/) override { }
  virtual void exitMethodDefinition(JavaScriptParser_cpp::MethodDefinitionContext * /*ctx*/) override { }

  virtual void enterFieldDefinition(JavaScriptParser_cpp::FieldDefinitionContext * /*ctx*/) override { }
  virtual void exitFieldDefinition(JavaScriptParser_cpp::FieldDefinitionContext * /*ctx*/) override { }

  virtual void enterClassElementName(JavaScriptParser_cpp::ClassElementNameContext * /*ctx*/) override { }
  virtual void exitClassElementName(JavaScriptParser_cpp::ClassElementNameContext * /*ctx*/) override { }

  virtual void enterPrivateIdentifier(JavaScriptParser_cpp::PrivateIdentifierContext * /*ctx*/) override { }
  virtual void exitPrivateIdentifier(JavaScriptParser_cpp::PrivateIdentifierContext * /*ctx*/) override { }

  virtual void enterFormalParameterList(JavaScriptParser_cpp::FormalParameterListContext * /*ctx*/) override { }
  virtual void exitFormalParameterList(JavaScriptParser_cpp::FormalParameterListContext * /*ctx*/) override { }

  virtual void enterFormalParameterArg(JavaScriptParser_cpp::FormalParameterArgContext * /*ctx*/) override { }
  virtual void exitFormalParameterArg(JavaScriptParser_cpp::FormalParameterArgContext * /*ctx*/) override { }

  virtual void enterLastFormalParameterArg(JavaScriptParser_cpp::LastFormalParameterArgContext * /*ctx*/) override { }
  virtual void exitLastFormalParameterArg(JavaScriptParser_cpp::LastFormalParameterArgContext * /*ctx*/) override { }

  virtual void enterFunctionBody(JavaScriptParser_cpp::FunctionBodyContext * /*ctx*/) override { }
  virtual void exitFunctionBody(JavaScriptParser_cpp::FunctionBodyContext * /*ctx*/) override { }

  virtual void enterSourceElements(JavaScriptParser_cpp::SourceElementsContext * /*ctx*/) override { }
  virtual void exitSourceElements(JavaScriptParser_cpp::SourceElementsContext * /*ctx*/) override { }

  virtual void enterArrayLiteral(JavaScriptParser_cpp::ArrayLiteralContext * /*ctx*/) override { }
  virtual void exitArrayLiteral(JavaScriptParser_cpp::ArrayLiteralContext * /*ctx*/) override { }

  virtual void enterElementList(JavaScriptParser_cpp::ElementListContext * /*ctx*/) override { }
  virtual void exitElementList(JavaScriptParser_cpp::ElementListContext * /*ctx*/) override { }

  virtual void enterArrayElement(JavaScriptParser_cpp::ArrayElementContext * /*ctx*/) override { }
  virtual void exitArrayElement(JavaScriptParser_cpp::ArrayElementContext * /*ctx*/) override { }

  virtual void enterPropertyExpressionAssignment(JavaScriptParser_cpp::PropertyExpressionAssignmentContext * /*ctx*/) override { }
  virtual void exitPropertyExpressionAssignment(JavaScriptParser_cpp::PropertyExpressionAssignmentContext * /*ctx*/) override { }

  virtual void enterComputedPropertyExpressionAssignment(JavaScriptParser_cpp::ComputedPropertyExpressionAssignmentContext * /*ctx*/) override { }
  virtual void exitComputedPropertyExpressionAssignment(JavaScriptParser_cpp::ComputedPropertyExpressionAssignmentContext * /*ctx*/) override { }

  virtual void enterFunctionProperty(JavaScriptParser_cpp::FunctionPropertyContext * /*ctx*/) override { }
  virtual void exitFunctionProperty(JavaScriptParser_cpp::FunctionPropertyContext * /*ctx*/) override { }

  virtual void enterPropertyGetter(JavaScriptParser_cpp::PropertyGetterContext * /*ctx*/) override { }
  virtual void exitPropertyGetter(JavaScriptParser_cpp::PropertyGetterContext * /*ctx*/) override { }

  virtual void enterPropertySetter(JavaScriptParser_cpp::PropertySetterContext * /*ctx*/) override { }
  virtual void exitPropertySetter(JavaScriptParser_cpp::PropertySetterContext * /*ctx*/) override { }

  virtual void enterPropertyShorthand(JavaScriptParser_cpp::PropertyShorthandContext * /*ctx*/) override { }
  virtual void exitPropertyShorthand(JavaScriptParser_cpp::PropertyShorthandContext * /*ctx*/) override { }

  virtual void enterPropertyName(JavaScriptParser_cpp::PropertyNameContext * /*ctx*/) override { }
  virtual void exitPropertyName(JavaScriptParser_cpp::PropertyNameContext * /*ctx*/) override { }

  virtual void enterArguments(JavaScriptParser_cpp::ArgumentsContext * /*ctx*/) override { }
  virtual void exitArguments(JavaScriptParser_cpp::ArgumentsContext * /*ctx*/) override { }

  virtual void enterArgument(JavaScriptParser_cpp::ArgumentContext * /*ctx*/) override { }
  virtual void exitArgument(JavaScriptParser_cpp::ArgumentContext * /*ctx*/) override { }

  virtual void enterExpressionSequence(JavaScriptParser_cpp::ExpressionSequenceContext * /*ctx*/) override { }
  virtual void exitExpressionSequence(JavaScriptParser_cpp::ExpressionSequenceContext * /*ctx*/) override { }

  virtual void enterTemplateStringExpression(JavaScriptParser_cpp::TemplateStringExpressionContext * /*ctx*/) override { }
  virtual void exitTemplateStringExpression(JavaScriptParser_cpp::TemplateStringExpressionContext * /*ctx*/) override { }

  virtual void enterTernaryExpression(JavaScriptParser_cpp::TernaryExpressionContext * /*ctx*/) override { }
  virtual void exitTernaryExpression(JavaScriptParser_cpp::TernaryExpressionContext * /*ctx*/) override { }

  virtual void enterLogicalAndExpression(JavaScriptParser_cpp::LogicalAndExpressionContext * /*ctx*/) override { }
  virtual void exitLogicalAndExpression(JavaScriptParser_cpp::LogicalAndExpressionContext * /*ctx*/) override { }

  virtual void enterPowerExpression(JavaScriptParser_cpp::PowerExpressionContext * /*ctx*/) override { }
  virtual void exitPowerExpression(JavaScriptParser_cpp::PowerExpressionContext * /*ctx*/) override { }

  virtual void enterPreIncrementExpression(JavaScriptParser_cpp::PreIncrementExpressionContext * /*ctx*/) override { }
  virtual void exitPreIncrementExpression(JavaScriptParser_cpp::PreIncrementExpressionContext * /*ctx*/) override { }

  virtual void enterObjectLiteralExpression(JavaScriptParser_cpp::ObjectLiteralExpressionContext * /*ctx*/) override { }
  virtual void exitObjectLiteralExpression(JavaScriptParser_cpp::ObjectLiteralExpressionContext * /*ctx*/) override { }

  virtual void enterMetaExpression(JavaScriptParser_cpp::MetaExpressionContext * /*ctx*/) override { }
  virtual void exitMetaExpression(JavaScriptParser_cpp::MetaExpressionContext * /*ctx*/) override { }

  virtual void enterInExpression(JavaScriptParser_cpp::InExpressionContext * /*ctx*/) override { }
  virtual void exitInExpression(JavaScriptParser_cpp::InExpressionContext * /*ctx*/) override { }

  virtual void enterLogicalOrExpression(JavaScriptParser_cpp::LogicalOrExpressionContext * /*ctx*/) override { }
  virtual void exitLogicalOrExpression(JavaScriptParser_cpp::LogicalOrExpressionContext * /*ctx*/) override { }

  virtual void enterOptionalChainExpression(JavaScriptParser_cpp::OptionalChainExpressionContext * /*ctx*/) override { }
  virtual void exitOptionalChainExpression(JavaScriptParser_cpp::OptionalChainExpressionContext * /*ctx*/) override { }

  virtual void enterNotExpression(JavaScriptParser_cpp::NotExpressionContext * /*ctx*/) override { }
  virtual void exitNotExpression(JavaScriptParser_cpp::NotExpressionContext * /*ctx*/) override { }

  virtual void enterPreDecreaseExpression(JavaScriptParser_cpp::PreDecreaseExpressionContext * /*ctx*/) override { }
  virtual void exitPreDecreaseExpression(JavaScriptParser_cpp::PreDecreaseExpressionContext * /*ctx*/) override { }

  virtual void enterArgumentsExpression(JavaScriptParser_cpp::ArgumentsExpressionContext * /*ctx*/) override { }
  virtual void exitArgumentsExpression(JavaScriptParser_cpp::ArgumentsExpressionContext * /*ctx*/) override { }

  virtual void enterAwaitExpression(JavaScriptParser_cpp::AwaitExpressionContext * /*ctx*/) override { }
  virtual void exitAwaitExpression(JavaScriptParser_cpp::AwaitExpressionContext * /*ctx*/) override { }

  virtual void enterThisExpression(JavaScriptParser_cpp::ThisExpressionContext * /*ctx*/) override { }
  virtual void exitThisExpression(JavaScriptParser_cpp::ThisExpressionContext * /*ctx*/) override { }

  virtual void enterFunctionExpression(JavaScriptParser_cpp::FunctionExpressionContext * /*ctx*/) override { }
  virtual void exitFunctionExpression(JavaScriptParser_cpp::FunctionExpressionContext * /*ctx*/) override { }

  virtual void enterUnaryMinusExpression(JavaScriptParser_cpp::UnaryMinusExpressionContext * /*ctx*/) override { }
  virtual void exitUnaryMinusExpression(JavaScriptParser_cpp::UnaryMinusExpressionContext * /*ctx*/) override { }

  virtual void enterAssignmentExpression(JavaScriptParser_cpp::AssignmentExpressionContext * /*ctx*/) override { }
  virtual void exitAssignmentExpression(JavaScriptParser_cpp::AssignmentExpressionContext * /*ctx*/) override { }

  virtual void enterPostDecreaseExpression(JavaScriptParser_cpp::PostDecreaseExpressionContext * /*ctx*/) override { }
  virtual void exitPostDecreaseExpression(JavaScriptParser_cpp::PostDecreaseExpressionContext * /*ctx*/) override { }

  virtual void enterTypeofExpression(JavaScriptParser_cpp::TypeofExpressionContext * /*ctx*/) override { }
  virtual void exitTypeofExpression(JavaScriptParser_cpp::TypeofExpressionContext * /*ctx*/) override { }

  virtual void enterInstanceofExpression(JavaScriptParser_cpp::InstanceofExpressionContext * /*ctx*/) override { }
  virtual void exitInstanceofExpression(JavaScriptParser_cpp::InstanceofExpressionContext * /*ctx*/) override { }

  virtual void enterUnaryPlusExpression(JavaScriptParser_cpp::UnaryPlusExpressionContext * /*ctx*/) override { }
  virtual void exitUnaryPlusExpression(JavaScriptParser_cpp::UnaryPlusExpressionContext * /*ctx*/) override { }

  virtual void enterDeleteExpression(JavaScriptParser_cpp::DeleteExpressionContext * /*ctx*/) override { }
  virtual void exitDeleteExpression(JavaScriptParser_cpp::DeleteExpressionContext * /*ctx*/) override { }

  virtual void enterImportExpression(JavaScriptParser_cpp::ImportExpressionContext * /*ctx*/) override { }
  virtual void exitImportExpression(JavaScriptParser_cpp::ImportExpressionContext * /*ctx*/) override { }

  virtual void enterEqualityExpression(JavaScriptParser_cpp::EqualityExpressionContext * /*ctx*/) override { }
  virtual void exitEqualityExpression(JavaScriptParser_cpp::EqualityExpressionContext * /*ctx*/) override { }

  virtual void enterBitXOrExpression(JavaScriptParser_cpp::BitXOrExpressionContext * /*ctx*/) override { }
  virtual void exitBitXOrExpression(JavaScriptParser_cpp::BitXOrExpressionContext * /*ctx*/) override { }

  virtual void enterSuperExpression(JavaScriptParser_cpp::SuperExpressionContext * /*ctx*/) override { }
  virtual void exitSuperExpression(JavaScriptParser_cpp::SuperExpressionContext * /*ctx*/) override { }

  virtual void enterMultiplicativeExpression(JavaScriptParser_cpp::MultiplicativeExpressionContext * /*ctx*/) override { }
  virtual void exitMultiplicativeExpression(JavaScriptParser_cpp::MultiplicativeExpressionContext * /*ctx*/) override { }

  virtual void enterBitShiftExpression(JavaScriptParser_cpp::BitShiftExpressionContext * /*ctx*/) override { }
  virtual void exitBitShiftExpression(JavaScriptParser_cpp::BitShiftExpressionContext * /*ctx*/) override { }

  virtual void enterParenthesizedExpression(JavaScriptParser_cpp::ParenthesizedExpressionContext * /*ctx*/) override { }
  virtual void exitParenthesizedExpression(JavaScriptParser_cpp::ParenthesizedExpressionContext * /*ctx*/) override { }

  virtual void enterAdditiveExpression(JavaScriptParser_cpp::AdditiveExpressionContext * /*ctx*/) override { }
  virtual void exitAdditiveExpression(JavaScriptParser_cpp::AdditiveExpressionContext * /*ctx*/) override { }

  virtual void enterRelationalExpression(JavaScriptParser_cpp::RelationalExpressionContext * /*ctx*/) override { }
  virtual void exitRelationalExpression(JavaScriptParser_cpp::RelationalExpressionContext * /*ctx*/) override { }

  virtual void enterPostIncrementExpression(JavaScriptParser_cpp::PostIncrementExpressionContext * /*ctx*/) override { }
  virtual void exitPostIncrementExpression(JavaScriptParser_cpp::PostIncrementExpressionContext * /*ctx*/) override { }

  virtual void enterYieldExpression(JavaScriptParser_cpp::YieldExpressionContext * /*ctx*/) override { }
  virtual void exitYieldExpression(JavaScriptParser_cpp::YieldExpressionContext * /*ctx*/) override { }

  virtual void enterBitNotExpression(JavaScriptParser_cpp::BitNotExpressionContext * /*ctx*/) override { }
  virtual void exitBitNotExpression(JavaScriptParser_cpp::BitNotExpressionContext * /*ctx*/) override { }

  virtual void enterNewExpression(JavaScriptParser_cpp::NewExpressionContext * /*ctx*/) override { }
  virtual void exitNewExpression(JavaScriptParser_cpp::NewExpressionContext * /*ctx*/) override { }

  virtual void enterLiteralExpression(JavaScriptParser_cpp::LiteralExpressionContext * /*ctx*/) override { }
  virtual void exitLiteralExpression(JavaScriptParser_cpp::LiteralExpressionContext * /*ctx*/) override { }

  virtual void enterArrayLiteralExpression(JavaScriptParser_cpp::ArrayLiteralExpressionContext * /*ctx*/) override { }
  virtual void exitArrayLiteralExpression(JavaScriptParser_cpp::ArrayLiteralExpressionContext * /*ctx*/) override { }

  virtual void enterMemberDotExpression(JavaScriptParser_cpp::MemberDotExpressionContext * /*ctx*/) override { }
  virtual void exitMemberDotExpression(JavaScriptParser_cpp::MemberDotExpressionContext * /*ctx*/) override { }

  virtual void enterClassExpression(JavaScriptParser_cpp::ClassExpressionContext * /*ctx*/) override { }
  virtual void exitClassExpression(JavaScriptParser_cpp::ClassExpressionContext * /*ctx*/) override { }

  virtual void enterMemberIndexExpression(JavaScriptParser_cpp::MemberIndexExpressionContext * /*ctx*/) override { }
  virtual void exitMemberIndexExpression(JavaScriptParser_cpp::MemberIndexExpressionContext * /*ctx*/) override { }

  virtual void enterIdentifierExpression(JavaScriptParser_cpp::IdentifierExpressionContext * /*ctx*/) override { }
  virtual void exitIdentifierExpression(JavaScriptParser_cpp::IdentifierExpressionContext * /*ctx*/) override { }

  virtual void enterBitAndExpression(JavaScriptParser_cpp::BitAndExpressionContext * /*ctx*/) override { }
  virtual void exitBitAndExpression(JavaScriptParser_cpp::BitAndExpressionContext * /*ctx*/) override { }

  virtual void enterBitOrExpression(JavaScriptParser_cpp::BitOrExpressionContext * /*ctx*/) override { }
  virtual void exitBitOrExpression(JavaScriptParser_cpp::BitOrExpressionContext * /*ctx*/) override { }

  virtual void enterAssignmentOperatorExpression(JavaScriptParser_cpp::AssignmentOperatorExpressionContext * /*ctx*/) override { }
  virtual void exitAssignmentOperatorExpression(JavaScriptParser_cpp::AssignmentOperatorExpressionContext * /*ctx*/) override { }

  virtual void enterVoidExpression(JavaScriptParser_cpp::VoidExpressionContext * /*ctx*/) override { }
  virtual void exitVoidExpression(JavaScriptParser_cpp::VoidExpressionContext * /*ctx*/) override { }

  virtual void enterCoalesceExpression(JavaScriptParser_cpp::CoalesceExpressionContext * /*ctx*/) override { }
  virtual void exitCoalesceExpression(JavaScriptParser_cpp::CoalesceExpressionContext * /*ctx*/) override { }

  virtual void enterInitializer(JavaScriptParser_cpp::InitializerContext * /*ctx*/) override { }
  virtual void exitInitializer(JavaScriptParser_cpp::InitializerContext * /*ctx*/) override { }

  virtual void enterAssignable(JavaScriptParser_cpp::AssignableContext * /*ctx*/) override { }
  virtual void exitAssignable(JavaScriptParser_cpp::AssignableContext * /*ctx*/) override { }

  virtual void enterObjectLiteral(JavaScriptParser_cpp::ObjectLiteralContext * /*ctx*/) override { }
  virtual void exitObjectLiteral(JavaScriptParser_cpp::ObjectLiteralContext * /*ctx*/) override { }

  virtual void enterNamedFunction(JavaScriptParser_cpp::NamedFunctionContext * /*ctx*/) override { }
  virtual void exitNamedFunction(JavaScriptParser_cpp::NamedFunctionContext * /*ctx*/) override { }

  virtual void enterAnonymousFunctionDecl(JavaScriptParser_cpp::AnonymousFunctionDeclContext * /*ctx*/) override { }
  virtual void exitAnonymousFunctionDecl(JavaScriptParser_cpp::AnonymousFunctionDeclContext * /*ctx*/) override { }

  virtual void enterArrowFunction(JavaScriptParser_cpp::ArrowFunctionContext * /*ctx*/) override { }
  virtual void exitArrowFunction(JavaScriptParser_cpp::ArrowFunctionContext * /*ctx*/) override { }

  virtual void enterArrowFunctionParameters(JavaScriptParser_cpp::ArrowFunctionParametersContext * /*ctx*/) override { }
  virtual void exitArrowFunctionParameters(JavaScriptParser_cpp::ArrowFunctionParametersContext * /*ctx*/) override { }

  virtual void enterArrowFunctionBody(JavaScriptParser_cpp::ArrowFunctionBodyContext * /*ctx*/) override { }
  virtual void exitArrowFunctionBody(JavaScriptParser_cpp::ArrowFunctionBodyContext * /*ctx*/) override { }

  virtual void enterAssignmentOperator(JavaScriptParser_cpp::AssignmentOperatorContext * /*ctx*/) override { }
  virtual void exitAssignmentOperator(JavaScriptParser_cpp::AssignmentOperatorContext * /*ctx*/) override { }

  virtual void enterLiteral(JavaScriptParser_cpp::LiteralContext * /*ctx*/) override { }
  virtual void exitLiteral(JavaScriptParser_cpp::LiteralContext * /*ctx*/) override { }

  virtual void enterTemplateStringLiteral(JavaScriptParser_cpp::TemplateStringLiteralContext * /*ctx*/) override { }
  virtual void exitTemplateStringLiteral(JavaScriptParser_cpp::TemplateStringLiteralContext * /*ctx*/) override { }

  virtual void enterTemplateStringAtom(JavaScriptParser_cpp::TemplateStringAtomContext * /*ctx*/) override { }
  virtual void exitTemplateStringAtom(JavaScriptParser_cpp::TemplateStringAtomContext * /*ctx*/) override { }

  virtual void enterNumericLiteral(JavaScriptParser_cpp::NumericLiteralContext * /*ctx*/) override { }
  virtual void exitNumericLiteral(JavaScriptParser_cpp::NumericLiteralContext * /*ctx*/) override { }

  virtual void enterBigintLiteral(JavaScriptParser_cpp::BigintLiteralContext * /*ctx*/) override { }
  virtual void exitBigintLiteral(JavaScriptParser_cpp::BigintLiteralContext * /*ctx*/) override { }

  virtual void enterGetter(JavaScriptParser_cpp::GetterContext * /*ctx*/) override { }
  virtual void exitGetter(JavaScriptParser_cpp::GetterContext * /*ctx*/) override { }

  virtual void enterSetter(JavaScriptParser_cpp::SetterContext * /*ctx*/) override { }
  virtual void exitSetter(JavaScriptParser_cpp::SetterContext * /*ctx*/) override { }

  virtual void enterIdentifierName(JavaScriptParser_cpp::IdentifierNameContext * /*ctx*/) override { }
  virtual void exitIdentifierName(JavaScriptParser_cpp::IdentifierNameContext * /*ctx*/) override { }

  virtual void enterIdentifier(JavaScriptParser_cpp::IdentifierContext * /*ctx*/) override { }
  virtual void exitIdentifier(JavaScriptParser_cpp::IdentifierContext * /*ctx*/) override { }

  virtual void enterReservedWord(JavaScriptParser_cpp::ReservedWordContext * /*ctx*/) override { }
  virtual void exitReservedWord(JavaScriptParser_cpp::ReservedWordContext * /*ctx*/) override { }

  virtual void enterKeyword(JavaScriptParser_cpp::KeywordContext * /*ctx*/) override { }
  virtual void exitKeyword(JavaScriptParser_cpp::KeywordContext * /*ctx*/) override { }

  virtual void enterLet_(JavaScriptParser_cpp::Let_Context * /*ctx*/) override { }
  virtual void exitLet_(JavaScriptParser_cpp::Let_Context * /*ctx*/) override { }

  virtual void enterEos(JavaScriptParser_cpp::EosContext * /*ctx*/) override { }
  virtual void exitEos(JavaScriptParser_cpp::EosContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

