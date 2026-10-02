#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
#include "CSubsetBaseVisitor.h"
#include "SymbolTable.h"

using namespace std;

struct NodeResult{
    string text = "";
    string type = "";
    string name = "";
    bool isArray = false;
    int arraySize = 0;
    int intVal = 0;
    bool hasIntVal = false;
    vector<string> paramTypes;
    vector<string> paramNames;
    vector<bool> paramIsArray;
    vector<NodeResult> items;
};

class CSubsetVisitorImpl : public CSubsetBaseVisitor{
private:
    SymbolTable symbolTable;
    ofstream& logFile;
    ofstream& errorFile;
    int errorCount;
    int lineCount;

    void reportError(int line, const string& message){
        errorCount++;
        errorFile << "Error at line " << line << ": " << message << "\n\n";
        logFile << "Error at line " << line << ": " << message << "\n\n";
    }

    void logRule(int line, const string& rule, const string& code, bool extraNewline = false){
        logFile << "Line " << line << ": " << rule << "\n\n" << code << "\n\n";
        if(extraNewline){
            logFile << "\n";
        }
    }

public:
    CSubsetVisitorImpl(ofstream& log, ofstream& err, int totalLines)
        : symbolTable(30), logFile(log), errorFile(err), errorCount(0), lineCount(totalLines){}

    int getErrorCount() const { return errorCount; }

    //start and program
    any visitStartRule(CSubsetParser::StartRuleContext *ctx) override;
    any visitProgramUnit(CSubsetParser::ProgramUnitContext *ctx) override;
    any visitUnitOnly(CSubsetParser::UnitOnlyContext *ctx) override;

    //unit
    any visitUnitVarDecl(CSubsetParser::UnitVarDeclContext *ctx) override;
    any visitUnitFuncDecl(CSubsetParser::UnitFuncDeclContext *ctx) override;
    any visitUnitFuncDef(CSubsetParser::UnitFuncDefContext *ctx) override;

    //function declaration
    any visitFuncDeclWithParams(CSubsetParser::FuncDeclWithParamsContext *ctx) override;
    any visitFuncDeclNoParams(CSubsetParser::FuncDeclNoParamsContext *ctx) override;

    //function definition
    any visitFuncDefWithParams(CSubsetParser::FuncDefWithParamsContext *ctx) override;
    any visitFuncDefErrorParams(CSubsetParser::FuncDefErrorParamsContext *ctx) override;
    any visitFuncDefNoParams(CSubsetParser::FuncDefNoParamsContext *ctx) override;

    //parameter list
    any visitParamListAdd(CSubsetParser::ParamListAddContext *ctx) override;
    any visitParamListAddNoName(CSubsetParser::ParamListAddNoNameContext *ctx) override;
    any visitParamSingle(CSubsetParser::ParamSingleContext *ctx) override;
    any visitParamSingleNoName(CSubsetParser::ParamSingleNoNameContext *ctx) override;

    //compound statement
    any visitCompoundStmtBody(CSubsetParser::CompoundStmtBodyContext *ctx) override;
    any visitCompoundStmtEmpty(CSubsetParser::CompoundStmtEmptyContext *ctx) override;

    //variable declaration
    any visitVarDecl(CSubsetParser::VarDeclContext *ctx) override;

    //type specifiers
    any visitTypeInt(CSubsetParser::TypeIntContext *ctx) override;
    any visitTypeFloat(CSubsetParser::TypeFloatContext *ctx) override;
    any visitTypeVoid(CSubsetParser::TypeVoidContext *ctx) override;

    //declaration list
    any visitDeclListVar(CSubsetParser::DeclListVarContext *ctx) override;
    any visitDeclListArray(CSubsetParser::DeclListArrayContext *ctx) override;
    any visitDeclVar(CSubsetParser::DeclVarContext *ctx) override;
    any visitDeclArray(CSubsetParser::DeclArrayContext *ctx) override;
    any visitDeclListErrorVar(CSubsetParser::DeclListErrorVarContext *ctx) override;
    any visitDeclListErrorArray(CSubsetParser::DeclListErrorArrayContext *ctx) override;

    //statements
    any visitStmtsSingle(CSubsetParser::StmtsSingleContext *ctx) override;
    any visitStmtsAdd(CSubsetParser::StmtsAddContext *ctx) override;

    //statement
    any visitStmtVarDecl(CSubsetParser::StmtVarDeclContext *ctx) override;
    any visitStmtExpr(CSubsetParser::StmtExprContext *ctx) override;
    any visitStmtCompound(CSubsetParser::StmtCompoundContext *ctx) override;
    any visitStmtFor(CSubsetParser::StmtForContext *ctx) override;
    any visitStmtIf(CSubsetParser::StmtIfContext *ctx) override;
    any visitStmtWhile(CSubsetParser::StmtWhileContext *ctx) override;
    any visitStmtPrint(CSubsetParser::StmtPrintContext *ctx) override;
    any visitStmtReturn(CSubsetParser::StmtReturnContext *ctx) override;

    //expression statement
    any visitExprStmtSemicolon(CSubsetParser::ExprStmtSemicolonContext *ctx) override;
    any visitExprStmtExpr(CSubsetParser::ExprStmtExprContext *ctx) override;
    any visitExprStmtNoSemicolon(CSubsetParser::ExprStmtNoSemicolonContext *ctx) override;

    //variable
    any visitVarSimple(CSubsetParser::VarSimpleContext *ctx) override;
    any visitVarArray(CSubsetParser::VarArrayContext *ctx) override;

    //expression
    any visitExprLogic(CSubsetParser::ExprLogicContext *ctx) override;
    any visitExprAssign(CSubsetParser::ExprAssignContext *ctx) override;

    //logic expression
    any visitLogicRel(CSubsetParser::LogicRelContext *ctx) override;
    any visitLogicOp(CSubsetParser::LogicOpContext *ctx) override;

    //relational expression
    any visitRelSimple(CSubsetParser::RelSimpleContext *ctx) override;
    any visitRelOp(CSubsetParser::RelOpContext *ctx) override;

    //simple expression
    any visitSimpleTerm(CSubsetParser::SimpleTermContext *ctx) override;
    any visitSimpleAdd(CSubsetParser::SimpleAddContext *ctx) override;
    any visitSimpleAddErrorAssign(CSubsetParser::SimpleAddErrorAssignContext *ctx) override;

    //term
    any visitTermUnary(CSubsetParser::TermUnaryContext *ctx) override;
    any visitTermMul(CSubsetParser::TermMulContext *ctx) override;

    //unary expression
    any visitUnaryAdd(CSubsetParser::UnaryAddContext *ctx) override;
    any visitUnaryNot(CSubsetParser::UnaryNotContext *ctx) override;
    any visitUnaryFactor(CSubsetParser::UnaryFactorContext *ctx) override;

    //factor
    any visitFactorVar(CSubsetParser::FactorVarContext *ctx) override;
    any visitFactorFuncCall(CSubsetParser::FactorFuncCallContext *ctx) override;
    any visitFactorParen(CSubsetParser::FactorParenContext *ctx) override;
    any visitFactorInt(CSubsetParser::FactorIntContext *ctx) override;
    any visitFactorFloat(CSubsetParser::FactorFloatContext *ctx) override;
    any visitFactorInc(CSubsetParser::FactorIncContext *ctx) override;
    any visitFactorDec(CSubsetParser::FactorDecContext *ctx) override;

    //argument list
    any visitArgListNotEmpty(CSubsetParser::ArgListNotEmptyContext *ctx) override;
    any visitArgListEmpty(CSubsetParser::ArgListEmptyContext *ctx) override;

    //arguments
    any visitArgsSingle(CSubsetParser::ArgsSingleContext *ctx) override;
    any visitArgsAdd(CSubsetParser::ArgsAddContext *ctx) override;
};
