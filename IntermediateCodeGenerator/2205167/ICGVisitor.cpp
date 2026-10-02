#include"ICGVisitor.h"
#include<fstream>
#include<iostream>

using namespace std;

ICGVisitor::ICGVisitor(ofstream& out, const string& printLibPath)
    : codeOut(out), printProcLibPath(printLibPath), symbolTable(30){
    labelCount = 1;
    currentLocalOffset = 0;
    currentFuncExitLabel = "";
    currentFuncName = "";
    currentFuncParamCount = 0;
    inDataSegment = false;
    inCodeSegment = false;

    codeOut<<"format ELF executable 3\n";
    codeOut<<"entry main\n";
    codeOut.flush();
}

void ICGVisitor::collectParams(CSubsetParser::Parameter_listContext *ctx, vector<pair<string, string>>& params){
    if(ctx == nullptr) return;
    auto namedList = dynamic_cast<CSubsetParser::ParamListNamedContext*>(ctx);
    if(namedList != nullptr){
        collectParams(namedList->parameter_list(), params);
        string pType = namedList->type_specifier()->getText();
        string pName = namedList->ID()->getText();
        params.push_back({pType, pName});
        return;
    }
    auto unnamedList = dynamic_cast<CSubsetParser::ParamListUnnamedContext*>(ctx);
    if(unnamedList != nullptr){
        collectParams(unnamedList->parameter_list(), params);
        string pType = unnamedList->type_specifier()->getText();
        params.push_back({pType, ""});
        return;
    }
    auto namedSingle = dynamic_cast<CSubsetParser::ParamSingleNamedContext*>(ctx);
    if(namedSingle != nullptr){
        string pType = namedSingle->type_specifier()->getText();
        string pName = namedSingle->ID()->getText();
        params.push_back({pType, pName});
        return;
    }
    auto unnamedSingle = dynamic_cast<CSubsetParser::ParamSingleUnnamedContext*>(ctx);
    if(unnamedSingle != nullptr){
        string pType = unnamedSingle->type_specifier()->getText();
        params.push_back({pType, ""});
        return;
    }
}

void ICGVisitor::collectArgs(CSubsetParser::ArgumentsContext *ctx, vector<CSubsetParser::Logic_expressionContext*>& args){
    if(ctx == nullptr) return;
    auto multi = dynamic_cast<CSubsetParser::ArgsMultipleContext*>(ctx);
    if(multi != nullptr){
        collectArgs(multi->arguments(), args);
        args.push_back(multi->logic_expression());
        return;
    }
    auto single = dynamic_cast<CSubsetParser::ArgsSingleContext*>(ctx);
    if(single != nullptr){
        args.push_back(single->logic_expression());
        return;
    }
}

antlrcpp::Any ICGVisitor::visitStartRule(CSubsetParser::StartRuleContext *ctx){
    visit(ctx->program());

    codeOut<<";-------------------------------\n";
    codeOut<<";         print library         \n";
    codeOut<<";-------------------------------\n";
    ifstream libFile(printProcLibPath);
    if(libFile.is_open()){
        string line;
        while(getline(libFile, line)){
            codeOut<<line<<"\n";
        }
        libFile.close();
    } else{
        codeOut<<"print_number:\n"
               <<"\tpush eax\n\tpush ebx\n\tpush ecx\n\tpush edx\n\tpush esi\n\tpush edi\n"
               <<"\tsub esp, 32\n\ttest eax, eax\n\tjns .positive\n"
               <<"\tpush eax\n\tsub esp, 1\n\tmov byte [esp], '-'\n"
               <<"\tmov eax, 4\n\tmov ebx, 1\n\tmov ecx, esp\n\tmov edx, 1\n\tint 0x80\n"
               <<"\tadd esp, 1\n\tpop eax\n\tneg eax\n"
               <<".positive:\n"
               <<"\tmov ebx, 10\n\tlea esi, [esp + 31]\n\tmov byte [esi], 10\n\tdec esi\n"
               <<".convert:\n"
               <<"\txor edx, edx\n\tdiv ebx\n\tadd dl, '0'\n\tmov [esi], dl\n\tdec esi\n"
               <<"\ttest eax, eax\n\tjnz .convert\n\tinc esi\n\tlea edx, [esp + 32]\n\tsub edx, esi\n"
               <<"\tmov eax, 4\n\tmov ebx, 1\n\tmov ecx, esi\n\tint 0x80\n\tadd esp, 32\n"
               <<"\tpop edi\n\tpop esi\n\tpop edx\n\tpop ecx\n\tpop ebx\n\tpop eax\n\tret\n";
    }
    codeOut<<";-------------------------------\n";
    codeOut.flush();

    return std::any();
}

antlrcpp::Any ICGVisitor::visitProgramMultiple(CSubsetParser::ProgramMultipleContext *ctx){
    visit(ctx->program());
    visit(ctx->unit());
    return std::any();
}

antlrcpp::Any ICGVisitor::visitProgramSingle(CSubsetParser::ProgramSingleContext *ctx){
    visit(ctx->unit());
    return std::any();
}

antlrcpp::Any ICGVisitor::visitUnitVarDecl(CSubsetParser::UnitVarDeclContext *ctx){
    if(!inDataSegment){
        inDataSegment = true;
        codeOut<<"segment readable writeable\n";
        codeOut.flush();
    }
    visit(ctx->var_declaration());
    return std::any();
}

antlrcpp::Any ICGVisitor::visitUnitFuncDecl(CSubsetParser::UnitFuncDeclContext *ctx){
    visit(ctx->func_declaration());
    return std::any();
}

antlrcpp::Any ICGVisitor::visitUnitFuncDef(CSubsetParser::UnitFuncDefContext *ctx){
    visit(ctx->func_definition());
    return std::any();
}

antlrcpp::Any ICGVisitor::visitFuncDeclWithParams(CSubsetParser::FuncDeclWithParamsContext *ctx){
    return std::any();
}

antlrcpp::Any ICGVisitor::visitFuncDeclNoParams(CSubsetParser::FuncDeclNoParamsContext *ctx){
    return std::any();
}

antlrcpp::Any ICGVisitor::visitFuncDefWithParams(CSubsetParser::FuncDefWithParamsContext *ctx){
    if(!inCodeSegment){
        inCodeSegment = true;
        codeOut<<"segment readable executable\n";
        codeOut.flush();
    }

    string funcName = ctx->ID()->getText();
    currentFuncName = funcName;
    currentLocalOffset = 0;
    currentFuncExitLabel = newLabel();

    vector<pair<string, string>> params;
    collectParams(ctx->parameter_list(), params);
    currentFuncParamCount = params.size();

    codeOut<<funcName<<":\n";
    codeOut<<"\tPUSH EBP\n";
    codeOut<<"\tMOV EBP, ESP\n";
    codeOut.flush();

    symbolTable.enterScope();

    for(size_t i = 0; i < params.size(); i++){
        if(!params[i].second.empty()){
            SymbolInfo* sym = new SymbolInfo(params[i].second, "ID");
            sym->setIsParam(true);
            sym->setStackOffset(8 + 4 * i);
            sym->setVarType(params[i].first);
            symbolTable.insert(sym);
        }
    }

    visit(ctx->compound_statement());

    codeOut<<currentFuncExitLabel<<":\n";
    codeOut<<"\tADD ESP, "<<currentLocalOffset<<"\n";
    codeOut<<"\tPOP EBP\n";

    if(funcName == "main"){
        codeOut<<"\tMOV EAX, 1\n";
        codeOut<<"\tXOR EBX, EBX\n";
        codeOut<<"\tINT 0x80\n";
        codeOut<<"\tPOP EBP\n";
        codeOut<<"\tRET\n";
    } else{
        if(currentFuncParamCount > 0){
            codeOut<<"\tRET "<<(currentFuncParamCount * 4)<<"\n";
        } else{
            codeOut<<"\tRET\n";
        }
    }
    codeOut.flush();

    symbolTable.exitScope();
    return std::any();
}

antlrcpp::Any ICGVisitor::visitFuncDefNoParams(CSubsetParser::FuncDefNoParamsContext *ctx){
    if(!inCodeSegment){
        inCodeSegment = true;
        codeOut<<"segment readable executable\n";
        codeOut.flush();
    }

    string funcName = ctx->ID()->getText();
    currentFuncName = funcName;
    currentLocalOffset = 0;
    currentFuncParamCount = 0;
    currentFuncExitLabel = newLabel();

    codeOut<<funcName<<":\n";
    codeOut<<"\tPUSH EBP\n";
    codeOut<<"\tMOV EBP, ESP\n";
    codeOut.flush();

    symbolTable.enterScope();

    visit(ctx->compound_statement());

    codeOut<<currentFuncExitLabel<<":\n";
    codeOut<<"\tADD ESP, "<<currentLocalOffset<<"\n";
    codeOut<<"\tPOP EBP\n";

    if(funcName == "main"){
        codeOut<<"\tMOV EAX, 1\n";
        codeOut<<"\tXOR EBX, EBX\n";
        codeOut<<"\tINT 0x80\n";
        codeOut<<"\tPOP EBP\n";
        codeOut<<"\tRET\n";
    } else{
        codeOut<<"\tRET\n";
    }
    codeOut.flush();

    symbolTable.exitScope();
    return std::any();
}

antlrcpp::Any ICGVisitor::visitParamSingleUnnamed(CSubsetParser::ParamSingleUnnamedContext *ctx){
    return std::any();
}

antlrcpp::Any ICGVisitor::visitParamListUnnamed(CSubsetParser::ParamListUnnamedContext *ctx){
    return std::any();
}

antlrcpp::Any ICGVisitor::visitParamSingleNamed(CSubsetParser::ParamSingleNamedContext *ctx){
    return std::any();
}

antlrcpp::Any ICGVisitor::visitParamListNamed(CSubsetParser::ParamListNamedContext *ctx){
    return std::any();
}

antlrcpp::Any ICGVisitor::visitCompoundWithStmts(CSubsetParser::CompoundWithStmtsContext *ctx){
    visit(ctx->statements());
    return std::any();
}

antlrcpp::Any ICGVisitor::visitCompoundEmpty(CSubsetParser::CompoundEmptyContext *ctx){
    return std::any();
}

antlrcpp::Any ICGVisitor::visitVarDecl(CSubsetParser::VarDeclContext *ctx){
    visit(ctx->declaration_list());
    return std::any();
}

antlrcpp::Any ICGVisitor::visitTypeInt(CSubsetParser::TypeIntContext *ctx){
    return std::any();
}

antlrcpp::Any ICGVisitor::visitTypeFloat(CSubsetParser::TypeFloatContext *ctx){
    return std::any();
}

antlrcpp::Any ICGVisitor::visitTypeVoid(CSubsetParser::TypeVoidContext *ctx){
    return std::any();
}

antlrcpp::Any ICGVisitor::visitDeclListSingle(CSubsetParser::DeclListSingleContext *ctx){
    string varName = ctx->ID()->getText();
    if(symbolTable.isRootScope()){
        if(!inDataSegment){
            inDataSegment = true;
            codeOut<<"segment readable writeable\n";
        }
        codeOut<<"\t"<<varName<<" dd 1 DUP (0)\n";
        codeOut.flush();

        SymbolInfo* sym = new SymbolInfo(varName, "ID");
        sym->setIsGlobal(true);
        sym->setVarType("int");
        symbolTable.insert(sym);
    } else{
        currentLocalOffset += 4;
        SymbolInfo* sym = new SymbolInfo(varName, "ID");
        sym->setIsGlobal(false);
        sym->setStackOffset(currentLocalOffset);
        sym->setVarType("int");
        symbolTable.insert(sym);

        codeOut<<"\tSUB ESP, 4\n";
        codeOut.flush();
    }
    return std::any();
}

antlrcpp::Any ICGVisitor::visitDeclListMultiple(CSubsetParser::DeclListMultipleContext *ctx){
    visit(ctx->declaration_list());
    string varName = ctx->ID()->getText();
    if(symbolTable.isRootScope()){
        if(!inDataSegment){
            inDataSegment = true;
            codeOut<<"segment readable writeable\n";
        }
        codeOut<<"\t"<<varName<<" dd 1 DUP (0)\n";
        codeOut.flush();

        SymbolInfo* sym = new SymbolInfo(varName, "ID");
        sym->setIsGlobal(true);
        sym->setVarType("int");
        symbolTable.insert(sym);
    } else{
        currentLocalOffset += 4;
        SymbolInfo* sym = new SymbolInfo(varName, "ID");
        sym->setIsGlobal(false);
        sym->setStackOffset(currentLocalOffset);
        sym->setVarType("int");
        symbolTable.insert(sym);

        codeOut<<"\tSUB ESP, 4\n";
        codeOut.flush();
    }
    return std::any();
}

antlrcpp::Any ICGVisitor::visitDeclListSingleArr(CSubsetParser::DeclListSingleArrContext *ctx){
    string varName = ctx->ID()->getText();
    int arraySize = stoi(ctx->CONST_INT()->getText());
    if(symbolTable.isRootScope()){
        if(!inDataSegment){
            inDataSegment = true;
            codeOut<<"segment readable writeable\n";
        }
        codeOut<<"\t"<<varName<<" dd "<<arraySize<<" DUP (0)\n";
        codeOut.flush();

        SymbolInfo* sym = new SymbolInfo(varName, "ID");
        sym->setIsGlobal(true);
        sym->setIsArray(true);
        sym->setArraySize(arraySize);
        sym->setVarType("int");
        symbolTable.insert(sym);
    } else{
        currentLocalOffset += arraySize * 4;
        SymbolInfo* sym = new SymbolInfo(varName, "ID");
        sym->setIsGlobal(false);
        sym->setIsArray(true);
        sym->setArraySize(arraySize);
        sym->setStackOffset(currentLocalOffset);
        sym->setVarType("int");
        symbolTable.insert(sym);

        codeOut<<"\tSUB ESP, "<<(arraySize * 4)<<"\n";
        codeOut.flush();
    }
    return std::any();
}

antlrcpp::Any ICGVisitor::visitDeclListMultipleArr(CSubsetParser::DeclListMultipleArrContext *ctx){
    visit(ctx->declaration_list());
    string varName = ctx->ID()->getText();
    int arraySize = stoi(ctx->CONST_INT()->getText());
    if(symbolTable.isRootScope()){
        if(!inDataSegment){
            inDataSegment = true;
            codeOut<<"segment readable writeable\n";
        }
        codeOut<<"\t"<<varName<<" dd "<<arraySize<<" DUP (0)\n";
        codeOut.flush();

        SymbolInfo* sym = new SymbolInfo(varName, "ID");
        sym->setIsGlobal(true);
        sym->setIsArray(true);
        sym->setArraySize(arraySize);
        sym->setVarType("int");
        symbolTable.insert(sym);
    } else{
        currentLocalOffset += arraySize * 4;
        SymbolInfo* sym = new SymbolInfo(varName, "ID");
        sym->setIsGlobal(false);
        sym->setIsArray(true);
        sym->setArraySize(arraySize);
        sym->setStackOffset(currentLocalOffset);
        sym->setVarType("int");
        symbolTable.insert(sym);

        codeOut<<"\tSUB ESP, "<<(arraySize * 4)<<"\n";
        codeOut.flush();
    }
    return std::any();
}

antlrcpp::Any ICGVisitor::visitStmtsSingle(CSubsetParser::StmtsSingleContext *ctx){
    visit(ctx->statement());
    return std::any();
}

antlrcpp::Any ICGVisitor::visitStmtsMultiple(CSubsetParser::StmtsMultipleContext *ctx){
    visit(ctx->statements());
    visit(ctx->statement());
    return std::any();
}

antlrcpp::Any ICGVisitor::visitStmtVarDecl(CSubsetParser::StmtVarDeclContext *ctx){
    visit(ctx->var_declaration());
    return std::any();
}

antlrcpp::Any ICGVisitor::visitStmtExpr(CSubsetParser::StmtExprContext *ctx){
    emitLabel(newLabel());
    visit(ctx->expression_statement());
    return std::any();
}

antlrcpp::Any ICGVisitor::visitStmtCompound(CSubsetParser::StmtCompoundContext *ctx){
    symbolTable.enterScope();
    visit(ctx->compound_statement());
    symbolTable.exitScope();
    return std::any();
}

antlrcpp::Any ICGVisitor::visitStmtFor(CSubsetParser::StmtForContext *ctx){
    emitLabel(newLabel());
    visit(ctx->expression_statement(0));

    string lStart = newLabel();
    string lEnd = newLabel();

    emitLabel(lStart);
    visit(ctx->expression_statement(1));
    emitInstruction("CMP EAX, 0");
    emitInstruction("JE " + lEnd);

    visit(ctx->statement());

    visit(ctx->expression());
    emitInstruction("JMP " + lStart);

    emitLabel(lEnd);
    return std::any();
}

antlrcpp::Any ICGVisitor::visitStmtIfElse(CSubsetParser::StmtIfElseContext *ctx){
    emitLabel(newLabel());
    visit(ctx->expression());
    emitInstruction("CMP EAX, 0");

    string lElse = newLabel();
    string lEnd = newLabel();

    emitInstruction("JE " + lElse);
    visit(ctx->statement(0));
    emitInstruction("JMP " + lEnd);

    emitLabel(lElse);
    visit(ctx->statement(1));

    emitLabel(lEnd);
    return std::any();
}

antlrcpp::Any ICGVisitor::visitStmtIf(CSubsetParser::StmtIfContext *ctx){
    emitLabel(newLabel());
    visit(ctx->expression());
    emitInstruction("CMP EAX, 0");

    string lEnd = newLabel();
    emitInstruction("JE " + lEnd);

    visit(ctx->statement());

    emitLabel(lEnd);
    return std::any();
}

antlrcpp::Any ICGVisitor::visitStmtWhile(CSubsetParser::StmtWhileContext *ctx){
    string lStart = newLabel();
    string lEnd = newLabel();

    emitLabel(lStart);
    visit(ctx->expression());
    emitInstruction("CMP EAX, 0");
    emitInstruction("JE " + lEnd);

    visit(ctx->statement());
    emitInstruction("JMP " + lStart);

    emitLabel(lEnd);
    return std::any();
}

antlrcpp::Any ICGVisitor::visitStmtPrintln(CSubsetParser::StmtPrintlnContext *ctx){
    emitLabel(newLabel());
    string id = ctx->ID()->getText();
    SymbolInfo* sym = symbolTable.lookUp(id);
    int line = ctx->getStart()->getLine();
    if(sym != nullptr){
        emitInstruction("PUSH EAX");
        emitInstruction("MOV EAX, " + sym->getAsmOperand(), line);
        emitInstruction("CALL print_number");
        emitInstruction("POP EAX");
    }
    return std::any();
}

antlrcpp::Any ICGVisitor::visitStmtReturn(CSubsetParser::StmtReturnContext *ctx){
    emitLabel(newLabel());
    int line = ctx->getStart()->getLine();
    visit(ctx->expression());
    emitInstruction("JMP " + currentFuncExitLabel, line);
    return std::any();
}

antlrcpp::Any ICGVisitor::visitExprStmtEmpty(CSubsetParser::ExprStmtEmptyContext *ctx){
    return std::any();
}

antlrcpp::Any ICGVisitor::visitExprStmtExpr(CSubsetParser::ExprStmtExprContext *ctx){
    visit(ctx->expression());
    return std::any();
}

antlrcpp::Any ICGVisitor::visitVarSimple(CSubsetParser::VarSimpleContext *ctx){
    string varName = ctx->ID()->getText();
    SymbolInfo* sym = symbolTable.lookUp(varName);
    int line = ctx->getStart()->getLine();
    if(sym != nullptr){
        emitInstruction("MOV EAX, " + sym->getAsmOperand(), line);
    }
    return std::any();
}

antlrcpp::Any ICGVisitor::visitVarArray(CSubsetParser::VarArrayContext *ctx){
    string varName = ctx->ID()->getText();
    SymbolInfo* sym = symbolTable.lookUp(varName);
    int line = ctx->getStart()->getLine();

    visit(ctx->expression());
    emitInstruction("PUSH EAX");
    emitInstruction("POP EBX");
    emitInstruction("MOV EAX, 4", line);
    emitInstruction("MUL EBX");
    emitInstruction("MOV EBX, EAX");

    if(sym != nullptr){
        if(sym->getIsGlobal()){
            emitInstruction("MOV EAX, [" + varName + "+EBX]");
        } else{
            emitInstruction("MOV EAX, " + to_string(sym->getStackOffset()));
            emitInstruction("SUB EAX, EBX");
            emitInstruction("MOV ESI, EAX");
            emitInstruction("NEG ESI");
            emitInstruction("MOV EAX, [EBP+ESI]");
        }
    }
    return std::any();
}

antlrcpp::Any ICGVisitor::visitExprLogic(CSubsetParser::ExprLogicContext *ctx){
    visit(ctx->logic_expression());
    return std::any();
}

antlrcpp::Any ICGVisitor::visitExprAssign(CSubsetParser::ExprAssignContext *ctx){
    int line = ctx->getStart()->getLine();
    auto varCtx = ctx->variable();
    auto arrCtx = dynamic_cast<CSubsetParser::VarArrayContext*>(varCtx);

    if(arrCtx != nullptr){
        string varName = arrCtx->ID()->getText();
        SymbolInfo* sym = symbolTable.lookUp(varName);

        visit(ctx->logic_expression());
        emitInstruction("PUSH EAX");

        visit(arrCtx->expression());
        emitInstruction("PUSH EAX");
        emitInstruction("POP EBX");
        emitInstruction("MOV EAX, 4");
        emitInstruction("MUL EBX");
        emitInstruction("MOV EBX, EAX");

        if(sym != nullptr){
            if(sym->getIsGlobal()){
                emitInstruction("POP EAX");
                emitInstruction("MOV [" + varName + "+EBX], EAX", line);
                emitInstruction("PUSH EAX");
                emitInstruction("POP EAX");
            } else{
                emitInstruction("MOV EAX, " + to_string(sym->getStackOffset()));
                emitInstruction("SUB EAX, EBX");
                emitInstruction("MOV ESI, EAX");
                emitInstruction("NEG ESI");
                emitInstruction("POP EAX");
                emitInstruction("MOV [EBP+ESI], EAX", line);
                emitInstruction("PUSH EAX");
                emitInstruction("POP EAX");
            }
        }
    } else{
        visit(ctx->logic_expression());
        string varName = ctx->variable()->getText();
        SymbolInfo* sym = symbolTable.lookUp(varName);
        if(sym != nullptr){
            emitInstruction("MOV " + sym->getAsmOperand() + ", EAX", line);
            emitInstruction("PUSH EAX");
            emitInstruction("POP EAX");
        }
    }
    return std::any();
}

antlrcpp::Any ICGVisitor::visitLogicExprRel(CSubsetParser::LogicExprRelContext *ctx){
    visit(ctx->rel_expression());
    return std::any();
}

antlrcpp::Any ICGVisitor::visitLogicExprOp(CSubsetParser::LogicExprOpContext *ctx){
    string logicOp = ctx->LOGICOP()->getText();
    int line = ctx->getStart()->getLine();

    if(logicOp == "||"){
        string lTrue = newLabel();
        string lNext = newLabel();
        string lFalse = newLabel();
        string lEnd = newLabel();

        visit(ctx->rel_expression(0));
        emitInstruction("CMP EAX, 0", line);
        emitInstruction("JNE " + lTrue);
        emitInstruction("JMP " + lNext);

        emitLabel(lNext);
        visit(ctx->rel_expression(1));
        emitInstruction("CMP EAX, 0");
        emitInstruction("JNE " + lTrue);
        emitInstruction("JMP " + lFalse);

        emitLabel(lTrue);
        emitInstruction("MOV EAX, 1", line);
        emitInstruction("JMP " + lEnd);

        emitLabel(lFalse);
        emitInstruction("MOV EAX, 0");

        emitLabel(lEnd);
    } else if(logicOp == "&&"){
        string lNext = newLabel();
        string lTrue = newLabel();
        string lFalse = newLabel();
        string lEnd = newLabel();

        visit(ctx->rel_expression(0));
        emitInstruction("CMP EAX, 0", line);
        emitInstruction("JNE " + lNext);
        emitInstruction("JMP " + lFalse);

        emitLabel(lNext);
        visit(ctx->rel_expression(1));
        emitInstruction("CMP EAX, 0");
        emitInstruction("JNE " + lTrue);
        emitInstruction("JMP " + lFalse);

        emitLabel(lTrue);
        emitInstruction("MOV EAX, 1", line);
        emitInstruction("JMP " + lEnd);

        emitLabel(lFalse);
        emitInstruction("MOV EAX, 0");

        emitLabel(lEnd);
    }
    return std::any();
}

antlrcpp::Any ICGVisitor::visitRelExprSimple(CSubsetParser::RelExprSimpleContext *ctx){
    visit(ctx->simple_expression());
    return std::any();
}

antlrcpp::Any ICGVisitor::visitRelExprOp(CSubsetParser::RelExprOpContext *ctx){
    string relop = ctx->RELOP()->getText();
    int line = ctx->getStart()->getLine();

    visit(ctx->simple_expression(1));
    emitInstruction("PUSH EAX");

    visit(ctx->simple_expression(0));
    emitInstruction("POP EBX");
    emitInstruction("CMP EAX, EBX");

    string lTrue = newLabel();
    string lFalse = newLabel();
    string lEnd = newLabel();

    string jmpInst;
    if(relop == "<") jmpInst = "JL";
    else if(relop == "<=") jmpInst = "JLE";
    else if(relop == ">") jmpInst = "JG";
    else if(relop == ">=") jmpInst = "JGE";
    else if(relop == "==") jmpInst = "JE";
    else if(relop == "!=") jmpInst = "JNE";

    emitInstruction(jmpInst + " " + lTrue);
    emitInstruction("JMP " + lFalse);

    emitLabel(lTrue);
    emitInstruction("MOV EAX, 1", line);
    emitInstruction("JMP " + lEnd);

    emitLabel(lFalse);
    emitInstruction("MOV EAX, 0");

    emitLabel(lEnd);

    return std::any();
}

antlrcpp::Any ICGVisitor::visitSimpleExprTerm(CSubsetParser::SimpleExprTermContext *ctx){
    visit(ctx->term());
    return std::any();
}

antlrcpp::Any ICGVisitor::visitSimpleExprAdd(CSubsetParser::SimpleExprAddContext *ctx){
    string op = ctx->ADDOP()->getText();
    int line = ctx->getStart()->getLine();

    visit(ctx->term());
    emitInstruction("PUSH EAX");

    visit(ctx->simple_expression());
    emitInstruction("POP EBX");

    if(op == "+"){
        emitInstruction("ADD EAX, EBX");
    } else if(op == "-"){
        emitInstruction("SUB EAX, EBX");
    }

    emitInstruction("PUSH EAX");
    emitInstruction("POP EAX", line);

    return std::any();
}

antlrcpp::Any ICGVisitor::visitTermUnary(CSubsetParser::TermUnaryContext *ctx){
    visit(ctx->unary_expression());
    return std::any();
}

antlrcpp::Any ICGVisitor::visitTermMul(CSubsetParser::TermMulContext *ctx){
    string op = ctx->MULOP()->getText();
    int line = ctx->getStart()->getLine();

    visit(ctx->unary_expression());
    emitInstruction("PUSH EAX");

    visit(ctx->term());
    emitInstruction("POP EBX");

    if(op == "*"){
        emitInstruction("IMUL EBX");
        emitInstruction("PUSH EAX");
        emitInstruction("POP EAX", line);
    } else if(op == "/"){
        emitInstruction("CDQ");
        emitInstruction("IDIV EBX");
        emitInstruction("PUSH EAX");
        emitInstruction("POP EAX", line);
    } else if(op == "%"){
        emitInstruction("CDQ");
        emitInstruction("IDIV EBX");
        emitInstruction("MOV EAX, EDX");
        emitInstruction("PUSH EAX");
        emitInstruction("POP EAX", line);
    }

    return std::any();
}

antlrcpp::Any ICGVisitor::visitUnaryAdd(CSubsetParser::UnaryAddContext *ctx){
    string op = ctx->ADDOP()->getText();
    int line = ctx->getStart()->getLine();

    visit(ctx->unary_expression());

    if(op == "-"){
        emitInstruction("NEG EAX");
        emitInstruction("PUSH EAX");
        emitInstruction("POP EAX", line);
    }
    return std::any();
}

antlrcpp::Any ICGVisitor::visitUnaryNot(CSubsetParser::UnaryNotContext *ctx){
    int line = ctx->getStart()->getLine();
    visit(ctx->unary_expression());

    string lTrue = newLabel();
    string lEnd = newLabel();

    emitInstruction("CMP EAX, 0");
    emitInstruction("JNE " + lTrue);
    emitInstruction("MOV EAX, 1", line);
    emitInstruction("JMP " + lEnd);

    emitLabel(lTrue);
    emitInstruction("MOV EAX, 0");

    emitLabel(lEnd);

    return std::any();
}

antlrcpp::Any ICGVisitor::visitUnaryFactor(CSubsetParser::UnaryFactorContext *ctx){
    visit(ctx->factor());
    return std::any();
}

antlrcpp::Any ICGVisitor::visitFactorVar(CSubsetParser::FactorVarContext *ctx){
    visit(ctx->variable());
    return std::any();
}

antlrcpp::Any ICGVisitor::visitFactorFuncCall(CSubsetParser::FactorFuncCallContext *ctx){
    string funcName = ctx->ID()->getText();
    int line = ctx->getStart()->getLine();

    auto argListArgs = dynamic_cast<CSubsetParser::ArgListArgsContext*>(ctx->argument_list());
    if(argListArgs != nullptr && argListArgs->arguments() != nullptr){
        vector<CSubsetParser::Logic_expressionContext*> args;
        collectArgs(argListArgs->arguments(), args);
        for(int i = (int)args.size() - 1; i >= 0; i--){
            visit(args[i]);
            emitInstruction("PUSH EAX");
        }
    }

    emitInstruction("CALL " + funcName, line);
    return std::any();
}

antlrcpp::Any ICGVisitor::visitFactorParen(CSubsetParser::FactorParenContext *ctx){
    visit(ctx->expression());
    return std::any();
}

antlrcpp::Any ICGVisitor::visitFactorConstInt(CSubsetParser::FactorConstIntContext *ctx){
    string val = ctx->CONST_INT()->getText();
    int line = ctx->getStart()->getLine();
    emitInstruction("MOV EAX, " + val, line);
    return std::any();
}

antlrcpp::Any ICGVisitor::visitFactorConstFloat(CSubsetParser::FactorConstFloatContext *ctx){
    string val = ctx->CONST_FLOAT()->getText();
    int line = ctx->getStart()->getLine();
    emitInstruction("MOV EAX, " + val, line);
    return std::any();
}

antlrcpp::Any ICGVisitor::visitFactorInc(CSubsetParser::FactorIncContext *ctx){
    auto varCtx = ctx->variable();
    auto arrCtx = dynamic_cast<CSubsetParser::VarArrayContext*>(varCtx);
    int line = ctx->getStart()->getLine();

    if(arrCtx != nullptr){
        string varName = arrCtx->ID()->getText();
        SymbolInfo* sym = symbolTable.lookUp(varName);

        visit(arrCtx->expression());
        emitInstruction("PUSH EAX");
        emitInstruction("POP EBX");
        emitInstruction("MOV EAX, 4");
        emitInstruction("MUL EBX");
        emitInstruction("MOV EBX, EAX");

        if(sym != nullptr){
            if(sym->getIsGlobal()){
                emitInstruction("MOV EAX, [" + varName + "+EBX]", line);
                emitInstruction("PUSH EAX");
                emitInstruction("INC EAX");
                emitInstruction("MOV [" + varName + "+EBX], EAX");
                emitInstruction("POP EAX");
            } else{
                emitInstruction("MOV EAX, " + to_string(sym->getStackOffset()));
                emitInstruction("SUB EAX, EBX");
                emitInstruction("MOV ESI, EAX");
                emitInstruction("NEG ESI");
                emitInstruction("MOV EAX, [EBP+ESI]", line);
                emitInstruction("PUSH EAX");
                emitInstruction("INC EAX");
                emitInstruction("MOV [EBP+ESI], EAX");
                emitInstruction("POP EAX");
            }
        }
    } else{
        string varName = ctx->variable()->getText();
        SymbolInfo* sym = symbolTable.lookUp(varName);
        if(sym != nullptr){
            emitInstruction("MOV EAX, " + sym->getAsmOperand(), line);
            emitInstruction("PUSH EAX");
            emitInstruction("INC EAX");
            emitInstruction("MOV " + sym->getAsmOperand() + ", EAX");
            emitInstruction("POP EAX");
        }
    }
    return std::any();
}

antlrcpp::Any ICGVisitor::visitFactorDec(CSubsetParser::FactorDecContext *ctx){
    auto varCtx = ctx->variable();
    auto arrCtx = dynamic_cast<CSubsetParser::VarArrayContext*>(varCtx);
    int line = ctx->getStart()->getLine();

    if(arrCtx != nullptr){
        string varName = arrCtx->ID()->getText();
        SymbolInfo* sym = symbolTable.lookUp(varName);

        visit(arrCtx->expression());
        emitInstruction("PUSH EAX");
        emitInstruction("POP EBX");
        emitInstruction("MOV EAX, 4");
        emitInstruction("MUL EBX");
        emitInstruction("MOV EBX, EAX");

        if(sym != nullptr){
            if(sym->getIsGlobal()){
                emitInstruction("MOV EAX, [" + varName + "+EBX]", line);
                emitInstruction("PUSH EAX");
                emitInstruction("DEC EAX");
                emitInstruction("MOV [" + varName + "+EBX], EAX");
                emitInstruction("POP EAX");
            } else{
                emitInstruction("MOV EAX, " + to_string(sym->getStackOffset()));
                emitInstruction("SUB EAX, EBX");
                emitInstruction("MOV ESI, EAX");
                emitInstruction("NEG ESI");
                emitInstruction("MOV EAX, [EBP+ESI]", line);
                emitInstruction("PUSH EAX");
                emitInstruction("DEC EAX");
                emitInstruction("MOV [EBP+ESI], EAX");
                emitInstruction("POP EAX");
            }
        }
    } else{
        string varName = ctx->variable()->getText();
        SymbolInfo* sym = symbolTable.lookUp(varName);
        if(sym != nullptr){
            emitInstruction("MOV EAX, " + sym->getAsmOperand(), line);
            emitInstruction("PUSH EAX");
            emitInstruction("DEC EAX");
            emitInstruction("MOV " + sym->getAsmOperand() + ", EAX");
            emitInstruction("POP EAX");
        }
    }
    return std::any();
}

antlrcpp::Any ICGVisitor::visitArgListArgs(CSubsetParser::ArgListArgsContext *ctx){
    return std::any();
}

antlrcpp::Any ICGVisitor::visitArgListEmpty(CSubsetParser::ArgListEmptyContext *ctx){
    return std::any();
}

antlrcpp::Any ICGVisitor::visitArgsSingle(CSubsetParser::ArgsSingleContext *ctx){
    return std::any();
}

antlrcpp::Any ICGVisitor::visitArgsMultiple(CSubsetParser::ArgsMultipleContext *ctx){
    return std::any();
}
