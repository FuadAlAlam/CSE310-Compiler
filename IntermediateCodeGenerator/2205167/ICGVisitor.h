#pragma once

#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<utility>
#include"CSubsetBaseVisitor.h"
#include"SymbolTable.h"

using namespace std;

class ICGVisitor : public CSubsetBaseVisitor{
private:
    ofstream& codeOut;
    SymbolTable symbolTable;
    int labelCount;
    int currentLocalOffset;
    string currentFuncExitLabel;
    string currentFuncName;
    int currentFuncParamCount;
    bool inDataSegment;
    bool inCodeSegment;
    string printProcLibPath;

    string newLabel(){
        return ".L" + to_string(labelCount++);
    }

    void emitLabel(const string& label){
        codeOut<<label<<":"<<"\n";
        codeOut.flush();
    }

    void emitInstruction(const string& inst, int line = -1){
        codeOut<<"\t"<<inst;
        if(line > 0){
            codeOut<<"       ; Line "<<line;
        }
        codeOut<<"\n";
        codeOut.flush();
    }

    void collectParams(CSubsetParser::Parameter_listContext *ctx, vector<pair<string, string>>& params);
    void collectArgs(CSubsetParser::ArgumentsContext *ctx, vector<CSubsetParser::Logic_expressionContext*>& args);

public:
    ICGVisitor(ofstream& out, const string& printLibPath = "printProc.lib");
    ~ICGVisitor() override{}

    antlrcpp::Any visitStartRule(CSubsetParser::StartRuleContext *ctx) override;
    antlrcpp::Any visitProgramMultiple(CSubsetParser::ProgramMultipleContext *ctx) override;
    antlrcpp::Any visitProgramSingle(CSubsetParser::ProgramSingleContext *ctx) override;

    antlrcpp::Any visitUnitVarDecl(CSubsetParser::UnitVarDeclContext *ctx) override;
    antlrcpp::Any visitUnitFuncDecl(CSubsetParser::UnitFuncDeclContext *ctx) override;
    antlrcpp::Any visitUnitFuncDef(CSubsetParser::UnitFuncDefContext *ctx) override;

    antlrcpp::Any visitFuncDeclWithParams(CSubsetParser::FuncDeclWithParamsContext *ctx) override;
    antlrcpp::Any visitFuncDeclNoParams(CSubsetParser::FuncDeclNoParamsContext *ctx) override;
    antlrcpp::Any visitFuncDefWithParams(CSubsetParser::FuncDefWithParamsContext *ctx) override;
    antlrcpp::Any visitFuncDefNoParams(CSubsetParser::FuncDefNoParamsContext *ctx) override;

    antlrcpp::Any visitParamSingleUnnamed(CSubsetParser::ParamSingleUnnamedContext *ctx) override;
    antlrcpp::Any visitParamListUnnamed(CSubsetParser::ParamListUnnamedContext *ctx) override;
    antlrcpp::Any visitParamSingleNamed(CSubsetParser::ParamSingleNamedContext *ctx) override;
    antlrcpp::Any visitParamListNamed(CSubsetParser::ParamListNamedContext *ctx) override;

    antlrcpp::Any visitCompoundWithStmts(CSubsetParser::CompoundWithStmtsContext *ctx) override;
    antlrcpp::Any visitCompoundEmpty(CSubsetParser::CompoundEmptyContext *ctx) override;

    antlrcpp::Any visitVarDecl(CSubsetParser::VarDeclContext *ctx) override;
    antlrcpp::Any visitTypeInt(CSubsetParser::TypeIntContext *ctx) override;
    antlrcpp::Any visitTypeFloat(CSubsetParser::TypeFloatContext *ctx) override;
    antlrcpp::Any visitTypeVoid(CSubsetParser::TypeVoidContext *ctx) override;

    antlrcpp::Any visitDeclListSingle(CSubsetParser::DeclListSingleContext *ctx) override;
    antlrcpp::Any visitDeclListMultiple(CSubsetParser::DeclListMultipleContext *ctx) override;
    antlrcpp::Any visitDeclListSingleArr(CSubsetParser::DeclListSingleArrContext *ctx) override;
    antlrcpp::Any visitDeclListMultipleArr(CSubsetParser::DeclListMultipleArrContext *ctx) override;

    antlrcpp::Any visitStmtsSingle(CSubsetParser::StmtsSingleContext *ctx) override;
    antlrcpp::Any visitStmtsMultiple(CSubsetParser::StmtsMultipleContext *ctx) override;

    antlrcpp::Any visitStmtVarDecl(CSubsetParser::StmtVarDeclContext *ctx) override;
    antlrcpp::Any visitStmtExpr(CSubsetParser::StmtExprContext *ctx) override;
    antlrcpp::Any visitStmtCompound(CSubsetParser::StmtCompoundContext *ctx) override;
    antlrcpp::Any visitStmtFor(CSubsetParser::StmtForContext *ctx) override;
    antlrcpp::Any visitStmtIfElse(CSubsetParser::StmtIfElseContext *ctx) override;
    antlrcpp::Any visitStmtIf(CSubsetParser::StmtIfContext *ctx) override;
    antlrcpp::Any visitStmtWhile(CSubsetParser::StmtWhileContext *ctx) override;
    antlrcpp::Any visitStmtPrintln(CSubsetParser::StmtPrintlnContext *ctx) override;
    antlrcpp::Any visitStmtReturn(CSubsetParser::StmtReturnContext *ctx) override;

    antlrcpp::Any visitExprStmtEmpty(CSubsetParser::ExprStmtEmptyContext *ctx) override;
    antlrcpp::Any visitExprStmtExpr(CSubsetParser::ExprStmtExprContext *ctx) override;

    antlrcpp::Any visitVarSimple(CSubsetParser::VarSimpleContext *ctx) override;
    antlrcpp::Any visitVarArray(CSubsetParser::VarArrayContext *ctx) override;

    antlrcpp::Any visitExprLogic(CSubsetParser::ExprLogicContext *ctx) override;
    antlrcpp::Any visitExprAssign(CSubsetParser::ExprAssignContext *ctx) override;

    antlrcpp::Any visitLogicExprRel(CSubsetParser::LogicExprRelContext *ctx) override;
    antlrcpp::Any visitLogicExprOp(CSubsetParser::LogicExprOpContext *ctx) override;

    antlrcpp::Any visitRelExprSimple(CSubsetParser::RelExprSimpleContext *ctx) override;
    antlrcpp::Any visitRelExprOp(CSubsetParser::RelExprOpContext *ctx) override;

    antlrcpp::Any visitSimpleExprTerm(CSubsetParser::SimpleExprTermContext *ctx) override;
    antlrcpp::Any visitSimpleExprAdd(CSubsetParser::SimpleExprAddContext *ctx) override;

    antlrcpp::Any visitTermUnary(CSubsetParser::TermUnaryContext *ctx) override;
    antlrcpp::Any visitTermMul(CSubsetParser::TermMulContext *ctx) override;

    antlrcpp::Any visitUnaryAdd(CSubsetParser::UnaryAddContext *ctx) override;
    antlrcpp::Any visitUnaryNot(CSubsetParser::UnaryNotContext *ctx) override;
    antlrcpp::Any visitUnaryFactor(CSubsetParser::UnaryFactorContext *ctx) override;

    antlrcpp::Any visitFactorVar(CSubsetParser::FactorVarContext *ctx) override;
    antlrcpp::Any visitFactorFuncCall(CSubsetParser::FactorFuncCallContext *ctx) override;
    antlrcpp::Any visitFactorParen(CSubsetParser::FactorParenContext *ctx) override;
    antlrcpp::Any visitFactorConstInt(CSubsetParser::FactorConstIntContext *ctx) override;
    antlrcpp::Any visitFactorConstFloat(CSubsetParser::FactorConstFloatContext *ctx) override;
    antlrcpp::Any visitFactorInc(CSubsetParser::FactorIncContext *ctx) override;
    antlrcpp::Any visitFactorDec(CSubsetParser::FactorDecContext *ctx) override;

    antlrcpp::Any visitArgListArgs(CSubsetParser::ArgListArgsContext *ctx) override;
    antlrcpp::Any visitArgListEmpty(CSubsetParser::ArgListEmptyContext *ctx) override;
    antlrcpp::Any visitArgsSingle(CSubsetParser::ArgsSingleContext *ctx) override;
    antlrcpp::Any visitArgsMultiple(CSubsetParser::ArgsMultipleContext *ctx) override;
};
