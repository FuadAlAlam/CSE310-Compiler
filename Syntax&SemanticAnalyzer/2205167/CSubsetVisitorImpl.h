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
    std::any visitStartRule(CSubsetParser::StartRuleContext *ctx) override;
    std::any visitProgramUnit(CSubsetParser::ProgramUnitContext *ctx) override;
    std::any visitUnitOnly(CSubsetParser::UnitOnlyContext *ctx) override;

    //unit
    std::any visitUnitVarDecl(CSubsetParser::UnitVarDeclContext *ctx) override;
    std::any visitUnitFuncDecl(CSubsetParser::UnitFuncDeclContext *ctx) override;
    std::any visitUnitFuncDef(CSubsetParser::UnitFuncDefContext *ctx) override;

    //function declaration
    std::any visitFuncDeclWithParams(CSubsetParser::FuncDeclWithParamsContext *ctx) override;
    std::any visitFuncDeclNoParams(CSubsetParser::FuncDeclNoParamsContext *ctx) override;

    //function definition
    std::any visitFuncDefWithParams(CSubsetParser::FuncDefWithParamsContext *ctx) override;
    std::any visitFuncDefErrorParams(CSubsetParser::FuncDefErrorParamsContext *ctx) override;
    std::any visitFuncDefNoParams(CSubsetParser::FuncDefNoParamsContext *ctx) override;

    //parameter list
    std::any visitParamListAdd(CSubsetParser::ParamListAddContext *ctx) override;
    std::any visitParamListAddNoName(CSubsetParser::ParamListAddNoNameContext *ctx) override;
    std::any visitParamSingle(CSubsetParser::ParamSingleContext *ctx) override;
    std::any visitParamSingleNoName(CSubsetParser::ParamSingleNoNameContext *ctx) override;

    //compound statement
    std::any visitCompoundStmtBody(CSubsetParser::CompoundStmtBodyContext *ctx) override;
    std::any visitCompoundStmtEmpty(CSubsetParser::CompoundStmtEmptyContext *ctx) override;

    //variable declaration
    std::any visitVarDecl(CSubsetParser::VarDeclContext *ctx) override;

    //type specifiers
    std::any visitTypeInt(CSubsetParser::TypeIntContext *ctx) override;
    std::any visitTypeFloat(CSubsetParser::TypeFloatContext *ctx) override;
    std::any visitTypeVoid(CSubsetParser::TypeVoidContext *ctx) override;

    //declaration list
    std::any visitDeclListVar(CSubsetParser::DeclListVarContext *ctx) override;
    std::any visitDeclListArray(CSubsetParser::DeclListArrayContext *ctx) override;
    std::any visitDeclVar(CSubsetParser::DeclVarContext *ctx) override;
    std::any visitDeclArray(CSubsetParser::DeclArrayContext *ctx) override;
    std::any visitDeclListErrorVar(CSubsetParser::DeclListErrorVarContext *ctx) override;
    std::any visitDeclListErrorArray(CSubsetParser::DeclListErrorArrayContext *ctx) override;

    //statements
    std::any visitStmtsSingle(CSubsetParser::StmtsSingleContext *ctx) override;
    std::any visitStmtsAdd(CSubsetParser::StmtsAddContext *ctx) override;

    //statement
    std::any visitStmtVarDecl(CSubsetParser::StmtVarDeclContext *ctx) override;
    std::any visitStmtExpr(CSubsetParser::StmtExprContext *ctx) override;
    std::any visitStmtCompound(CSubsetParser::StmtCompoundContext *ctx) override;
    std::any visitStmtFor(CSubsetParser::StmtForContext *ctx) override;
    std::any visitStmtIf(CSubsetParser::StmtIfContext *ctx) override;
    std::any visitStmtWhile(CSubsetParser::StmtWhileContext *ctx) override;
    std::any visitStmtPrint(CSubsetParser::StmtPrintContext *ctx) override;
    std::any visitStmtReturn(CSubsetParser::StmtReturnContext *ctx) override;

    //expression statement
    std::any visitExprStmtSemicolon(CSubsetParser::ExprStmtSemicolonContext *ctx) override;
    std::any visitExprStmtExpr(CSubsetParser::ExprStmtExprContext *ctx) override;
    std::any visitExprStmtNoSemicolon(CSubsetParser::ExprStmtNoSemicolonContext *ctx) override;

    //variable
    std::any visitVarSimple(CSubsetParser::VarSimpleContext *ctx) override;
    std::any visitVarArray(CSubsetParser::VarArrayContext *ctx) override;

    //expression
    std::any visitExprLogic(CSubsetParser::ExprLogicContext *ctx) override;
    std::any visitExprAssign(CSubsetParser::ExprAssignContext *ctx) override;

    //logic expression
    std::any visitLogicRel(CSubsetParser::LogicRelContext *ctx) override;
    std::any visitLogicOp(CSubsetParser::LogicOpContext *ctx) override;

    //relational expression
    std::any visitRelSimple(CSubsetParser::RelSimpleContext *ctx) override;
    std::any visitRelOp(CSubsetParser::RelOpContext *ctx) override;

    //simple expression
    std::any visitSimpleTerm(CSubsetParser::SimpleTermContext *ctx) override;
    std::any visitSimpleAdd(CSubsetParser::SimpleAddContext *ctx) override;
    std::any visitSimpleAddErrorAssign(CSubsetParser::SimpleAddErrorAssignContext *ctx) override;

    //term
    std::any visitTermUnary(CSubsetParser::TermUnaryContext *ctx) override;
    std::any visitTermMul(CSubsetParser::TermMulContext *ctx) override;

    //unary expression
    std::any visitUnaryAdd(CSubsetParser::UnaryAddContext *ctx) override;
    std::any visitUnaryNot(CSubsetParser::UnaryNotContext *ctx) override;
    std::any visitUnaryFactor(CSubsetParser::UnaryFactorContext *ctx) override;

    //factor
    std::any visitFactorVar(CSubsetParser::FactorVarContext *ctx) override;
    std::any visitFactorFuncCall(CSubsetParser::FactorFuncCallContext *ctx) override;
    std::any visitFactorParen(CSubsetParser::FactorParenContext *ctx) override;
    std::any visitFactorInt(CSubsetParser::FactorIntContext *ctx) override;
    std::any visitFactorFloat(CSubsetParser::FactorFloatContext *ctx) override;
    std::any visitFactorInc(CSubsetParser::FactorIncContext *ctx) override;
    std::any visitFactorDec(CSubsetParser::FactorDecContext *ctx) override;

    //argument list
    std::any visitArgListNotEmpty(CSubsetParser::ArgListNotEmptyContext *ctx) override;
    std::any visitArgListEmpty(CSubsetParser::ArgListEmptyContext *ctx) override;

    //arguments
    std::any visitArgsSingle(CSubsetParser::ArgsSingleContext *ctx) override;
    std::any visitArgsAdd(CSubsetParser::ArgsAddContext *ctx) override;
};
