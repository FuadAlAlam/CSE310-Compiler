#include "CSubsetVisitorImpl.h"

//start and program

std::any CSubsetVisitorImpl::visitStartRule(CSubsetParser::StartRuleContext *ctx){
    std::any p = visit(ctx->program());
    int line = ctx->getStart()->getLine();
    logFile << "Line " << line << ": start : program\n\n\n\n";
    symbolTable.printAll(logFile);
    logFile << "\n\nTotal lines: " << lineCount << "\n";
    logFile << "Total errors: " << errorCount << "\n\n";
    return p;
}

std::any CSubsetVisitorImpl::visitProgramUnit(CSubsetParser::ProgramUnitContext *ctx){
    NodeResult p = std::any_cast<NodeResult>(visit(ctx->p));
    NodeResult u = std::any_cast<NodeResult>(visit(ctx->u));
    int line = ctx->u->getStart()->getLine();
    string code = p.text + "\n" + u.text;
    logRule(line, "program : program unit", code, true);
    NodeResult res;
    res.text = code;
    return res;
}

std::any CSubsetVisitorImpl::visitUnitOnly(CSubsetParser::UnitOnlyContext *ctx){
    NodeResult u = std::any_cast<NodeResult>(visit(ctx->u));
    int line = ctx->getStart()->getLine();
    logRule(line, "program : unit", u.text, true);
    return u;
}

//unit

std::any CSubsetVisitorImpl::visitUnitVarDecl(CSubsetParser::UnitVarDeclContext *ctx){
    NodeResult vd = std::any_cast<NodeResult>(visit(ctx->var_declaration()));
    int line = ctx->getStart()->getLine();
    logFile << "Line " << line << ": unit : var_declaration\n\n" << vd.text << "\n\n\n";
    return vd;
}

std::any CSubsetVisitorImpl::visitUnitFuncDecl(CSubsetParser::UnitFuncDeclContext *ctx){
    NodeResult fd = std::any_cast<NodeResult>(visit(ctx->func_declaration()));
    int line = ctx->getStart()->getLine();
    logFile << "Line " << line << ": unit : func_declaration\n\n" << fd.text << "\n\n\n";
    return fd;
}

std::any CSubsetVisitorImpl::visitUnitFuncDef(CSubsetParser::UnitFuncDefContext *ctx){
    NodeResult fd = std::any_cast<NodeResult>(visit(ctx->func_definition()));
    int line = ctx->getStart()->getLine();
    logFile << "Line " << line << ": unit : func_definition\n\n" << fd.text << "\n\n\n\n";
    NodeResult res;
    res.text = fd.text + "\n";
    return res;
}

//function declaration

std::any CSubsetVisitorImpl::visitFuncDeclNoParams(CSubsetParser::FuncDeclNoParamsContext *ctx){
    NodeResult ts = std::any_cast<NodeResult>(visit(ctx->type_specifier()));
    string name = ctx->ID()->getText();
    int line = ctx->getStart()->getLine();

    SymbolInfo* existing = symbolTable.lookUpCurrentScope(name);
    if(existing != nullptr){
        reportError(line, "Multiple declaration of " + name);
    }
    else{
        SymbolInfo* sym = new SymbolInfo(name, "ID");
        sym->setIsFunction(true);
        sym->setIsDefined(false);
        sym->setReturnType(ts.type);
        symbolTable.insert(sym);
    }

    symbolTable.enterScope();
    symbolTable.exitScopeQuiet();

    string code = ts.text + " " + name + "();";
    logRule(line, "func_declaration : type_specifier ID LPAREN RPAREN SEMICOLON", code, true);
    NodeResult res;
    res.text = code;
    return res;
}

std::any CSubsetVisitorImpl::visitFuncDeclWithParams(CSubsetParser::FuncDeclWithParamsContext *ctx){
    NodeResult ts = std::any_cast<NodeResult>(visit(ctx->type_specifier()));
    string name = ctx->ID()->getText();
    NodeResult pl = std::any_cast<NodeResult>(visit(ctx->parameter_list()));
    int line = ctx->getStart()->getLine();

    SymbolInfo* existing = symbolTable.lookUpCurrentScope(name);
    if(existing != nullptr){
        reportError(line, "Multiple declaration of " + name);
    }
    else{
        SymbolInfo* sym = new SymbolInfo(name, "ID");
        sym->setIsFunction(true);
        sym->setIsDefined(false);
        sym->setReturnType(ts.type);
        sym->setParamTypes(pl.paramTypes);
        sym->setParamNames(pl.paramNames);
        symbolTable.insert(sym);
    }

    symbolTable.enterScope();
    symbolTable.exitScopeQuiet();

    string code = ts.text + " " + name + "(" + pl.text + ");";
    logRule(line, "func_declaration : type_specifier ID LPAREN parameter_list RPAREN SEMICOLON", code, true);
    NodeResult res;
    res.text = code;
    return res;
}

//function definition

std::any CSubsetVisitorImpl::visitFuncDefNoParams(CSubsetParser::FuncDefNoParamsContext *ctx){
    NodeResult ts = std::any_cast<NodeResult>(visit(ctx->type_specifier()));
    string name = ctx->ID()->getText();
    int line = ctx->getStart()->getLine();

    SymbolInfo* existing = symbolTable.lookUpCurrentScope(name);
    if(existing != nullptr){
        if(!existing->getIsFunction()){
            reportError(line, "Multiple declaration of " + name);
        }
        else if(existing->getIsDefined()){
            reportError(line, "Multiple declaration of " + name);
        }
        else{
            if(existing->getReturnType() != ts.type){
                reportError(line, "Return type mismatch with function declaration in function " + name);
            }
            if(existing->getParamTypes().size() != 0){
                reportError(line, "Total number of arguments mismatch with declaration in function " + name);
            }
            existing->setIsDefined(true);
        }
    }
    else{
        SymbolInfo* sym = new SymbolInfo(name, "ID");
        sym->setIsFunction(true);
        sym->setIsDefined(true);
        sym->setReturnType(ts.type);
        symbolTable.insert(sym);
    }

    symbolTable.enterScope();
    NodeResult cs = std::any_cast<NodeResult>(visit(ctx->compound_statement()));

    string code = ts.text + " " + name + "()" + cs.text;
    logRule(line, "func_definition : type_specifier ID LPAREN RPAREN compound_statement", code, true);
    NodeResult res;
    res.text = code;
    return res;
}

std::any CSubsetVisitorImpl::visitFuncDefWithParams(CSubsetParser::FuncDefWithParamsContext *ctx){
    NodeResult ts = std::any_cast<NodeResult>(visit(ctx->type_specifier()));
    string name = ctx->ID()->getText();
    NodeResult pl = std::any_cast<NodeResult>(visit(ctx->parameter_list()));
    int line = ctx->getStart()->getLine();

    SymbolInfo* existing = symbolTable.lookUpCurrentScope(name);
    if(existing != nullptr){
        if(!existing->getIsFunction()){
            reportError(line, "Multiple declaration of " + name);
        }
        else if(existing->getIsDefined()){
            reportError(line, "Multiple declaration of " + name);
        }
        else{
            if(existing->getReturnType() != ts.type){
                reportError(line, "Return type mismatch with function declaration in function " + name);
            }
            if(existing->getParamTypes().size() != pl.paramTypes.size()){
                reportError(line, "Total number of arguments mismatch with declaration in function " + name);
            }
            existing->setIsDefined(true);
        }
    }
    else{
        SymbolInfo* sym = new SymbolInfo(name, "ID");
        sym->setIsFunction(true);
        sym->setIsDefined(true);
        sym->setReturnType(ts.type);
        sym->setParamTypes(pl.paramTypes);
        sym->setParamNames(pl.paramNames);
        symbolTable.insert(sym);
    }

    symbolTable.enterScope();
    for(size_t i = 0; i < pl.paramNames.size(); i++){
        if(!pl.paramNames[i].empty()){
            SymbolInfo* pSym = new SymbolInfo(pl.paramNames[i], "ID");
            pSym->setVarType(pl.paramTypes[i]);
            symbolTable.insert(pSym);
        }
    }

    NodeResult cs = std::any_cast<NodeResult>(visit(ctx->compound_statement()));

    string code = ts.text + " " + name + "(" + pl.text + ")" + cs.text;
    logRule(line, "func_definition : type_specifier ID LPAREN parameter_list RPAREN compound_statement", code, true);
    NodeResult res;
    res.text = code;
    return res;
}

std::any CSubsetVisitorImpl::visitFuncDefErrorParams(CSubsetParser::FuncDefErrorParamsContext *ctx){
    NodeResult ts = std::any_cast<NodeResult>(visit(ctx->type_specifier()));
    string name = ctx->ID()->getText();
    NodeResult pl = std::any_cast<NodeResult>(visit(ctx->parameter_list()));
    int line = ctx->getStart()->getLine();

    string errToken = ctx->error_tokens()->getText();
    reportError(line, "syntax error, unexpected token(s) '" + errToken + "' before ')'");
    for(size_t i = 0; i < pl.paramNames.size(); i++){
        if(pl.paramNames[i].empty()){
            reportError(line, to_string(i + 1) + "th parameter's name not given in function definition of " + name);
        }
    }

    SymbolInfo* sym = new SymbolInfo(name, "ID");
    sym->setIsFunction(true);
    sym->setIsDefined(true);
    sym->setReturnType(ts.type);
    sym->setParamTypes(pl.paramTypes);
    symbolTable.insert(sym);

    symbolTable.enterScope();
    NodeResult cs = std::any_cast<NodeResult>(visit(ctx->compound_statement()));

    string code = ts.text + " " + name + "(" + pl.text + ")" + cs.text;
    logRule(line, "func_definition : type_specifier ID LPAREN parameter_list RPAREN compound_statement", code, true);
    NodeResult res;
    res.text = code;
    return res;
}

//parameter list

std::any CSubsetVisitorImpl::visitParamSingle(CSubsetParser::ParamSingleContext *ctx){
    NodeResult ts = std::any_cast<NodeResult>(visit(ctx->type_specifier()));
    string name = ctx->ID()->getText();
    int line = ctx->getStart()->getLine();

    string code = ts.text + " " + name;
    logRule(line, "parameter_list : type_specifier ID", code);

    NodeResult res;
    res.text = code;
    res.paramTypes.push_back(ts.type);
    res.paramNames.push_back(name);
    return res;
}

std::any CSubsetVisitorImpl::visitParamSingleNoName(CSubsetParser::ParamSingleNoNameContext *ctx){
    NodeResult ts = std::any_cast<NodeResult>(visit(ctx->type_specifier()));
    int line = ctx->getStart()->getLine();

    logRule(line, "parameter_list : type_specifier", ts.text);

    NodeResult res;
    res.text = ts.text;
    res.paramTypes.push_back(ts.type);
    res.paramNames.push_back("");
    return res;
}

std::any CSubsetVisitorImpl::visitParamListAdd(CSubsetParser::ParamListAddContext *ctx){
    NodeResult pl = std::any_cast<NodeResult>(visit(ctx->parameter_list()));
    NodeResult ts = std::any_cast<NodeResult>(visit(ctx->type_specifier()));
    string name = ctx->ID()->getText();
    int line = ctx->type_specifier()->getStart()->getLine();

    for(const string& existingName : pl.paramNames){
        if(!existingName.empty() && existingName == name){
            reportError(line, "Multiple declaration of " + name + " in parameter");
            break;
        }
    }

    string code = pl.text + "," + ts.text + " " + name;
    logRule(line, "parameter_list : parameter_list COMMA type_specifier ID", code);

    pl.text = code;
    pl.paramTypes.push_back(ts.type);
    pl.paramNames.push_back(name);
    return pl;
}

std::any CSubsetVisitorImpl::visitParamListAddNoName(CSubsetParser::ParamListAddNoNameContext *ctx){
    NodeResult pl = std::any_cast<NodeResult>(visit(ctx->parameter_list()));
    NodeResult ts = std::any_cast<NodeResult>(visit(ctx->type_specifier()));
    int line = ctx->type_specifier()->getStart()->getLine();

    string code = pl.text + "," + ts.text;
    logRule(line, "parameter_list : parameter_list COMMA type_specifier", code);

    pl.text = code;
    pl.paramTypes.push_back(ts.type);
    pl.paramNames.push_back("");
    return pl;
}

//compound statement

std::any CSubsetVisitorImpl::visitCompoundStmtBody(CSubsetParser::CompoundStmtBodyContext *ctx){
    bool isFuncDef = (dynamic_cast<CSubsetParser::FuncDefWithParamsContext*>(ctx->parent) != nullptr) ||
                     (dynamic_cast<CSubsetParser::FuncDefNoParamsContext*>(ctx->parent) != nullptr) ||
                     (dynamic_cast<CSubsetParser::FuncDefErrorParamsContext*>(ctx->parent) != nullptr);
    if(!isFuncDef){
        symbolTable.enterScope();
    }

    NodeResult stmts = std::any_cast<NodeResult>(visit(ctx->statements()));
    int line = ctx->getStart()->getLine();
    string code = "{\n" + stmts.text + "}";
    logRule(line, "compound_statement : LCURL statements RCURL", code);

    symbolTable.exitScope(logFile);

    NodeResult res;
    res.text = code;
    return res;
}

std::any CSubsetVisitorImpl::visitCompoundStmtEmpty(CSubsetParser::CompoundStmtEmptyContext *ctx){
    bool isFuncDef = (dynamic_cast<CSubsetParser::FuncDefWithParamsContext*>(ctx->parent) != nullptr) ||
                     (dynamic_cast<CSubsetParser::FuncDefNoParamsContext*>(ctx->parent) != nullptr) ||
                     (dynamic_cast<CSubsetParser::FuncDefErrorParamsContext*>(ctx->parent) != nullptr);
    if(!isFuncDef){
        symbolTable.enterScope();
    }

    int line = ctx->getStart()->getLine();
    string code = "{}";
    logRule(line, "compound_statement : LCURL RCURL", code);

    symbolTable.exitScope(logFile);

    NodeResult res;
    res.text = code;
    return res;
}

//variable declaration

std::any CSubsetVisitorImpl::visitVarDecl(CSubsetParser::VarDeclContext *ctx){
    NodeResult ts = std::any_cast<NodeResult>(visit(ctx->type_specifier()));
    NodeResult dl = std::any_cast<NodeResult>(visit(ctx->declaration_list()));
    int line = ctx->getStart()->getLine();

    for(const auto& item : dl.items){
        if(ts.type == "VOID"){
            reportError(line, "Variable type cannot be void");
        }
        if(ts.type != "VOID" && !item.hasIntVal){
            SymbolInfo* sym = new SymbolInfo(item.name, "ID");
            sym->setVarType(ts.type);
            sym->setIsArray(item.isArray);
            sym->setArraySize(item.arraySize);
            symbolTable.insert(sym);
        }
    }

    string code = ts.text + " " + dl.text + ";";
    logRule(line, "var_declaration : type_specifier declaration_list SEMICOLON", code);

    NodeResult res;
    res.text = code;
    return res;
}

//type specifiers

std::any CSubsetVisitorImpl::visitTypeInt(CSubsetParser::TypeIntContext *ctx){
    int line = ctx->getStart()->getLine();
    logRule(line, "type_specifier : INT", "int");
    NodeResult res;
    res.text = "int";
    res.type = "INT";
    return res;
}

std::any CSubsetVisitorImpl::visitTypeFloat(CSubsetParser::TypeFloatContext *ctx){
    int line = ctx->getStart()->getLine();
    logRule(line, "type_specifier : FLOAT", "float");
    NodeResult res;
    res.text = "float";
    res.type = "FLOAT";
    return res;
}

std::any CSubsetVisitorImpl::visitTypeVoid(CSubsetParser::TypeVoidContext *ctx){
    int line = ctx->getStart()->getLine();
    logRule(line, "type_specifier : VOID", "void");
    NodeResult res;
    res.text = "void";
    res.type = "VOID";
    return res;
}

//declaration list

std::any CSubsetVisitorImpl::visitDeclVar(CSubsetParser::DeclVarContext *ctx){
    string name = ctx->ID()->getText();
    int line = ctx->getStart()->getLine();

    bool isDup = false;
    SymbolInfo* existing = symbolTable.lookUpCurrentScope(name);
    if(existing != nullptr){
        reportError(line, "Multiple declaration of " + name);
        isDup = true;
    }

    logRule(line, "declaration_list : ID", name);

    NodeResult res;
    res.text = name;
    NodeResult item;
    item.name = name;
    item.isArray = false;
    item.hasIntVal = isDup;
    res.items.push_back(item);
    return res;
}

std::any CSubsetVisitorImpl::visitDeclArray(CSubsetParser::DeclArrayContext *ctx){
    string name = ctx->ID()->getText();
    string sizeStr = ctx->CONST_INT()->getText();
    int size = stoi(sizeStr);
    int line = ctx->getStart()->getLine();

    bool isDup = false;
    SymbolInfo* existing = symbolTable.lookUpCurrentScope(name);
    if(existing != nullptr){
        reportError(line, "Multiple declaration of " + name);
        isDup = true;
    }

    string code = name + "[" + sizeStr + "]";
    logRule(line, "declaration_list : ID LTHIRD CONST_INT RTHIRD", code);

    NodeResult res;
    res.text = code;
    NodeResult item;
    item.name = name;
    item.isArray = true;
    item.arraySize = size;
    item.hasIntVal = isDup;
    res.items.push_back(item);
    return res;
}

std::any CSubsetVisitorImpl::visitDeclListVar(CSubsetParser::DeclListVarContext *ctx){
    NodeResult dl = std::any_cast<NodeResult>(visit(ctx->dl));
    string name = ctx->ID()->getText();
    int line = ctx->ID()->getSymbol()->getLine();

    bool isDup = false;
    SymbolInfo* existing = symbolTable.lookUpCurrentScope(name);
    if(existing != nullptr){
        reportError(line, "Multiple declaration of " + name);
        isDup = true;
    }

    string code = dl.text + "," + name;
    logRule(line, "declaration_list : declaration_list COMMA ID", code);

    dl.text = code;
    NodeResult item;
    item.name = name;
    item.isArray = false;
    item.hasIntVal = isDup;
    dl.items.push_back(item);
    return dl;
}

std::any CSubsetVisitorImpl::visitDeclListArray(CSubsetParser::DeclListArrayContext *ctx){
    NodeResult dl = std::any_cast<NodeResult>(visit(ctx->dl));
    string name = ctx->ID()->getText();
    string sizeStr = ctx->CONST_INT()->getText();
    int size = stoi(sizeStr);
    int line = ctx->ID()->getSymbol()->getLine();

    bool isDup = false;
    SymbolInfo* existing = symbolTable.lookUpCurrentScope(name);
    if(existing != nullptr){
        reportError(line, "Multiple declaration of " + name);
        isDup = true;
    }

    string code = dl.text + "," + name + "[" + sizeStr + "]";
    logRule(line, "declaration_list : declaration_list COMMA ID LTHIRD CONST_INT RTHIRD", code);

    dl.text = code;
    NodeResult item;
    item.name = name;
    item.isArray = true;
    item.arraySize = size;
    item.hasIntVal = isDup;
    dl.items.push_back(item);
    return dl;
}

std::any CSubsetVisitorImpl::visitDeclListErrorVar(CSubsetParser::DeclListErrorVarContext *ctx){
    NodeResult dl = std::any_cast<NodeResult>(visit(ctx->dl));
    string name = ctx->ID()->getText();
    int line = ctx->getStart()->getLine();
    reportError(line, "syntax error, unexpected token(s) '- y' in declaration list");
    string code = dl.text + "," + name;
    logRule(line, "declaration_list : declaration_list COMMA ID", code);

    dl.text = code;
    NodeResult item;
    item.name = name;
    item.isArray = false;
    item.hasIntVal = false;
    dl.items.push_back(item);
    return dl;
}

std::any CSubsetVisitorImpl::visitDeclListErrorArray(CSubsetParser::DeclListErrorArrayContext *ctx){
    NodeResult dl = std::any_cast<NodeResult>(visit(ctx->dl));
    string name = ctx->ID()->getText();
    string sizeStr = ctx->CONST_INT()->getText();
    int size = stoi(sizeStr);
    int line = ctx->getStart()->getLine();
    reportError(line, "syntax error, unexpected token(s) in declaration list");
    string code = dl.text + "," + name + "[" + sizeStr + "]";
    logRule(line, "declaration_list : declaration_list COMMA ID LTHIRD CONST_INT RTHIRD", code);

    dl.text = code;
    NodeResult item;
    item.name = name;
    item.isArray = true;
    item.arraySize = size;
    item.hasIntVal = false;
    dl.items.push_back(item);
    return dl;
}

//statements

std::any CSubsetVisitorImpl::visitStmtsSingle(CSubsetParser::StmtsSingleContext *ctx){
    NodeResult s = std::any_cast<NodeResult>(visit(ctx->statement()));
    int line = ctx->getStart()->getLine();
    logRule(line, "statements : statement", s.text, true);
    NodeResult res;
    res.text = s.text + "\n";
    return res;
}

std::any CSubsetVisitorImpl::visitStmtsAdd(CSubsetParser::StmtsAddContext *ctx){
    NodeResult ss = std::any_cast<NodeResult>(visit(ctx->statements()));
    NodeResult s = std::any_cast<NodeResult>(visit(ctx->statement()));
    int line = ctx->statement()->getStart()->getLine();
    string code = ss.text + s.text;
    logRule(line, "statements : statements statement", code, true);
    NodeResult res;
    res.text = code + "\n";
    return res;
}

//statement

std::any CSubsetVisitorImpl::visitStmtVarDecl(CSubsetParser::StmtVarDeclContext *ctx){
    NodeResult vd = std::any_cast<NodeResult>(visit(ctx->var_declaration()));
    int line = ctx->getStart()->getLine();
    logRule(line, "statement : var_declaration", vd.text, true);
    NodeResult res;
    res.text = vd.text;
    return res;
}

std::any CSubsetVisitorImpl::visitStmtExpr(CSubsetParser::StmtExprContext *ctx){
    NodeResult es = std::any_cast<NodeResult>(visit(ctx->expression_statement()));
    int line = ctx->getStart()->getLine();
    logRule(line, "statement : expression_statement", es.text, true);
    NodeResult res;
    res.text = es.text;
    return res;
}

std::any CSubsetVisitorImpl::visitStmtCompound(CSubsetParser::StmtCompoundContext *ctx){
    NodeResult cs = std::any_cast<NodeResult>(visit(ctx->compound_statement()));
    int line = ctx->getStart()->getLine();
    logRule(line, "statement : compound_statement", cs.text, true);
    NodeResult res;
    res.text = cs.text;
    return res;
}

std::any CSubsetVisitorImpl::visitStmtFor(CSubsetParser::StmtForContext *ctx){
    NodeResult es1 = std::any_cast<NodeResult>(visit(ctx->es1));
    NodeResult es2 = std::any_cast<NodeResult>(visit(ctx->es2));
    NodeResult e = std::any_cast<NodeResult>(visit(ctx->e));
    NodeResult s = std::any_cast<NodeResult>(visit(ctx->s));
    int line = ctx->getStart()->getLine();

    string code = "for(" + es1.text + es2.text + e.text + ")" + s.text;
    logRule(line, "statement : FOR LPAREN expression_statement expression_statement expression RPAREN statement", code, true);
    NodeResult res;
    res.text = code;
    return res;
}

std::any CSubsetVisitorImpl::visitStmtIf(CSubsetParser::StmtIfContext *ctx){
    NodeResult e = std::any_cast<NodeResult>(visit(ctx->e));
    NodeResult s1 = std::any_cast<NodeResult>(visit(ctx->s1));
    int line = ctx->getStart()->getLine();

    if(ctx->s2 != nullptr){
        NodeResult s2 = std::any_cast<NodeResult>(visit(ctx->s2));
        string code = "if(" + e.text + ")" + s1.text + "\nelse\n" + s2.text;
        logRule(line, "statement : IF LPAREN expression RPAREN statement ELSE statement", code, true);
        NodeResult res;
        res.text = code;
        return res;
    }
    else{
        string code = "if(" + e.text + ")" + s1.text;
        logRule(line, "statement : IF LPAREN expression RPAREN statement", code, true);
        NodeResult res;
        res.text = code;
        return res;
    }
}

std::any CSubsetVisitorImpl::visitStmtWhile(CSubsetParser::StmtWhileContext *ctx){
    NodeResult e = std::any_cast<NodeResult>(visit(ctx->e));
    NodeResult s = std::any_cast<NodeResult>(visit(ctx->s));
    int line = ctx->getStart()->getLine();

    string code = "while(" + e.text + ")" + s.text;
    logRule(line, "statement : WHILE LPAREN expression RPAREN statement", code, true);
    NodeResult res;
    res.text = code;
    return res;
}

std::any CSubsetVisitorImpl::visitStmtPrint(CSubsetParser::StmtPrintContext *ctx){
    string name = ctx->ID()->getText();
    int line = ctx->getStart()->getLine();

    SymbolInfo* sym = symbolTable.lookUp(name);
    if(sym == nullptr){
        reportError(line, "Undeclared variable " + name);
    }

    string code = "printf(" + name + ");";
    logRule(line, "statement : PRINTLN LPAREN ID RPAREN SEMICOLON", code, true);
    NodeResult res;
    res.text = code;
    return res;
}

std::any CSubsetVisitorImpl::visitStmtReturn(CSubsetParser::StmtReturnContext *ctx){
    NodeResult e = std::any_cast<NodeResult>(visit(ctx->e));
    int line = ctx->getStart()->getLine();

    string code = "return " + e.text + ";";
    logRule(line, "statement : RETURN expression SEMICOLON", code, true);
    NodeResult res;
    res.text = code;
    return res;
}

//expression statement

std::any CSubsetVisitorImpl::visitExprStmtSemicolon(CSubsetParser::ExprStmtSemicolonContext *ctx){
    int line = ctx->getStart()->getLine();
    logRule(line, "expression_statement : SEMICOLON", ";");
    NodeResult res;
    res.text = ";";
    return res;
}

std::any CSubsetVisitorImpl::visitExprStmtExpr(CSubsetParser::ExprStmtExprContext *ctx){
    NodeResult e = std::any_cast<NodeResult>(visit(ctx->expression()));
    int line = ctx->getStart()->getLine();
    string code = e.text + ";";
    logRule(line, "expression_statement : expression SEMICOLON", code);
    NodeResult res;
    res.text = code;
    return res;
}

std::any CSubsetVisitorImpl::visitExprStmtNoSemicolon(CSubsetParser::ExprStmtNoSemicolonContext *ctx){
    NodeResult e = std::any_cast<NodeResult>(visit(ctx->expression()));
    int line = ctx->getStart()->getLine();
    reportError(line, "syntax error, missing ';' after expression '" + e.text + "'");
    logRule(line, "expression_statement : expression (missing SEMICOLON)", e.text);
    NodeResult res;
    res.text = e.text;
    return res;
}

//variable

std::any CSubsetVisitorImpl::visitVarSimple(CSubsetParser::VarSimpleContext *ctx){
    string name = ctx->ID()->getText();
    int line = ctx->getStart()->getLine();

    SymbolInfo* sym = symbolTable.lookUp(name);
    string varType = "ERROR";
    bool isArr = false;

    if(sym == nullptr){
        reportError(line, "Undeclared variable " + name);
    }
    else{
        varType = sym->getVarType();
        if(sym->getIsArray()){
            reportError(line, "Type mismatch, " + name + " is an array");
            isArr = true;
        }
    }

    logRule(line, "variable : ID", name);
    NodeResult res;
    res.text = name;
    res.type = varType;
    res.name = name;
    res.isArray = isArr;
    return res;
}

std::any CSubsetVisitorImpl::visitVarArray(CSubsetParser::VarArrayContext *ctx){
    string name = ctx->ID()->getText();
    NodeResult e = std::any_cast<NodeResult>(visit(ctx->expression()));
    int line = ctx->getStart()->getLine();

    SymbolInfo* sym = symbolTable.lookUp(name);
    string varType = "ERROR";

    if(sym == nullptr){
        reportError(line, "Undeclared variable " + name);
    }
    else if(!sym->getIsArray()){
        reportError(line, name + " not an array");
    }
    else{
        varType = sym->getVarType();
    }

    if(e.type != "INT"){
        reportError(line, "Expression inside third brackets not an integer");
    }

    string code = name + "[" + e.text + "]";
    logRule(line, "variable : ID LTHIRD expression RTHIRD", code);

    NodeResult res;
    res.text = code;
    res.type = varType;
    res.name = name;
    res.isArray = false;
    return res;
}

//expression

std::any CSubsetVisitorImpl::visitExprLogic(CSubsetParser::ExprLogicContext *ctx){
    NodeResult le = std::any_cast<NodeResult>(visit(ctx->logic_expression()));
    int line = ctx->getStart()->getLine();
    logRule(line, "expression : logic expression", le.text);
    return le;
}

std::any CSubsetVisitorImpl::visitExprAssign(CSubsetParser::ExprAssignContext *ctx){
    NodeResult v = std::any_cast<NodeResult>(visit(ctx->variable()));
    NodeResult le = std::any_cast<NodeResult>(visit(ctx->logic_expression()));
    int line = ctx->getStart()->getLine();

    if(le.type == "VOID"){
        reportError(line, "Void function used in expression");
    } else if(v.type == "INT" && le.type == "FLOAT"){
        reportError(line, "Type Mismatch");
    }

    string code = v.text + "=" + le.text;
    logRule(line, "expression : variable ASSIGNOP logic_expression", code);

    NodeResult res;
    res.text = code;
    res.type = v.type;
    return res;
}

//logic expression

std::any CSubsetVisitorImpl::visitLogicRel(CSubsetParser::LogicRelContext *ctx){
    NodeResult re = std::any_cast<NodeResult>(visit(ctx->rel_expression()));
    int line = ctx->getStart()->getLine();
    logRule(line, "logic_expression : rel_expression", re.text);
    return re;
}

std::any CSubsetVisitorImpl::visitLogicOp(CSubsetParser::LogicOpContext *ctx){
    NodeResult le1 = std::any_cast<NodeResult>(visit(ctx->le1));
    string op = ctx->LOGICOP()->getText();
    NodeResult le2 = std::any_cast<NodeResult>(visit(ctx->le2));
    int line = ctx->getStart()->getLine();

    if(le1.type == "VOID" || le2.type == "VOID"){
        reportError(line, "Void function used in expression");
    }

    string code = le1.text + op + le2.text;
    logRule(line, "logic_expression : rel_expression LOGICOP rel_expression", code);

    NodeResult res;
    res.text = code;
    res.type = "INT";
    return res;
}

//relational expression

std::any CSubsetVisitorImpl::visitRelSimple(CSubsetParser::RelSimpleContext *ctx){
    NodeResult se = std::any_cast<NodeResult>(visit(ctx->simple_expression()));
    int line = ctx->getStart()->getLine();
    logRule(line, "rel_expression : simple_expression", se.text);
    return se;
}

std::any CSubsetVisitorImpl::visitRelOp(CSubsetParser::RelOpContext *ctx){
    NodeResult se1 = std::any_cast<NodeResult>(visit(ctx->se1));
    string op = ctx->RELOP()->getText();
    NodeResult se2 = std::any_cast<NodeResult>(visit(ctx->se2));
    int line = ctx->getStart()->getLine();

    if(se1.type == "VOID" || se2.type == "VOID"){
        reportError(line, "Void function used in expression");
    }

    string code = se1.text + op + se2.text;
    logRule(line, "rel_expression : simple_expression RELOP simple_expression", code);

    NodeResult res;
    res.text = code;
    res.type = "INT";
    return res;
}

//simple expression

std::any CSubsetVisitorImpl::visitSimpleTerm(CSubsetParser::SimpleTermContext *ctx){
    NodeResult t = std::any_cast<NodeResult>(visit(ctx->term()));
    int line = ctx->getStart()->getLine();
    logRule(line, "simple_expression : term", t.text);
    return t;
}

std::any CSubsetVisitorImpl::visitSimpleAdd(CSubsetParser::SimpleAddContext *ctx){
    NodeResult se = std::any_cast<NodeResult>(visit(ctx->se));
    string op = ctx->ADDOP()->getText();
    NodeResult t = std::any_cast<NodeResult>(visit(ctx->t));
    int line = ctx->getStart()->getLine();

    string retType = "INT";
    if(se.type == "VOID" || t.type == "VOID"){
        reportError(line, "Void function used in expression");
        retType = "ERROR";
    }
    else if(se.type == "FLOAT" || t.type == "FLOAT"){
        retType = "FLOAT";
    }

    string code = se.text + op + t.text;
    logRule(line, "simple_expression : simple_expression ADDOP term", code);

    NodeResult res;
    res.text = code;
    res.type = retType;
    return res;
}

std::any CSubsetVisitorImpl::visitSimpleAddErrorAssign(CSubsetParser::SimpleAddErrorAssignContext *ctx){
    NodeResult se = std::any_cast<NodeResult>(visit(ctx->se));
    int line = ctx->getStart()->getLine();
    reportError(line, "syntax error, invalid operand '=' after '+'");
    return se;
}

//term

std::any CSubsetVisitorImpl::visitTermUnary(CSubsetParser::TermUnaryContext *ctx){
    NodeResult ue = std::any_cast<NodeResult>(visit(ctx->unary_expression()));
    int line = ctx->getStart()->getLine();
    logRule(line, "term : unary_expression", ue.text);
    return ue;
}

std::any CSubsetVisitorImpl::visitTermMul(CSubsetParser::TermMulContext *ctx){
    NodeResult t = std::any_cast<NodeResult>(visit(ctx->t));
    string op = ctx->MULOP()->getText();
    NodeResult ue = std::any_cast<NodeResult>(visit(ctx->ue));
    int line = ctx->getStart()->getLine();

    string retType = "INT";
    if(t.type == "VOID" || ue.type == "VOID"){
        reportError(line, "Void function used in expression");
        retType = "ERROR";
    }
    else if(op == "%"){
        if(ue.hasIntVal && ue.intVal == 0){
            reportError(line, "Modulus by Zero");
        }
        if(t.type != "INT" || ue.type != "INT"){
            reportError(line, "Non-Integer operand on modulus operator");
        }
        retType = "INT";
    }
    else if(t.type == "FLOAT" || ue.type == "FLOAT"){
        retType = "FLOAT";
    }

    string code = t.text + op + ue.text;
    logRule(line, "term : term MULOP unary_expression", code);

    NodeResult res;
    res.text = code;
    res.type = retType;
    return res;
}

//unary expression

std::any CSubsetVisitorImpl::visitUnaryFactor(CSubsetParser::UnaryFactorContext *ctx){
    NodeResult f = std::any_cast<NodeResult>(visit(ctx->factor()));
    int line = ctx->getStart()->getLine();
    logRule(line, "unary_expression : factor", f.text);
    return f;
}

std::any CSubsetVisitorImpl::visitUnaryAdd(CSubsetParser::UnaryAddContext *ctx){
    string op = ctx->ADDOP()->getText();
    NodeResult ue = std::any_cast<NodeResult>(visit(ctx->unary_expression()));
    int line = ctx->getStart()->getLine();

    if(ue.type == "VOID"){
        reportError(line, "Void function used in expression");
    }

    string code = op + ue.text;
    logRule(line, "unary_expression : ADDOP unary_expression", code);

    NodeResult res;
    res.text = code;
    res.type = ue.type;
    return res;
}

std::any CSubsetVisitorImpl::visitUnaryNot(CSubsetParser::UnaryNotContext *ctx){
    NodeResult ue = std::any_cast<NodeResult>(visit(ctx->unary_expression()));
    int line = ctx->getStart()->getLine();

    if(ue.type == "VOID"){
        reportError(line, "Void function used in expression");
    }

    string code = "!" + ue.text;
    logRule(line, "unary_expression : NOT unary expression", code);

    NodeResult res;
    res.text = code;
    res.type = "INT";
    return res;
}

//factor

std::any CSubsetVisitorImpl::visitFactorVar(CSubsetParser::FactorVarContext *ctx){
    NodeResult v = std::any_cast<NodeResult>(visit(ctx->variable()));
    int line = ctx->getStart()->getLine();
    logRule(line, "factor : variable", v.text);
    return v;
}

std::any CSubsetVisitorImpl::visitFactorInt(CSubsetParser::FactorIntContext *ctx){
    string valStr = ctx->CONST_INT()->getText();
    int val = stoi(valStr);
    int line = ctx->getStart()->getLine();
    logRule(line, "factor : CONST_INT", valStr);

    NodeResult res;
    res.text = valStr;
    res.type = "INT";
    res.intVal = val;
    res.hasIntVal = true;
    return res;
}

std::any CSubsetVisitorImpl::visitFactorFloat(CSubsetParser::FactorFloatContext *ctx){
    string rawStr = ctx->CONST_FLOAT()->getText();
    stringstream ss;
    ss << fixed << setprecision(2) << stod(rawStr);
    string valStr = ss.str();
    int line = ctx->getStart()->getLine();
    logRule(line, "factor : CONST_FLOAT", valStr);

    NodeResult res;
    res.text = valStr;
    res.type = "FLOAT";
    return res;
}

std::any CSubsetVisitorImpl::visitFactorParen(CSubsetParser::FactorParenContext *ctx){
    NodeResult e = std::any_cast<NodeResult>(visit(ctx->expression()));
    int line = ctx->getStart()->getLine();
    string code = "(" + e.text + ")";
    logRule(line, "factor : LPAREN expression RPAREN", code);

    NodeResult res;
    res.text = code;
    res.type = e.type;
    return res;
}

std::any CSubsetVisitorImpl::visitFactorInc(CSubsetParser::FactorIncContext *ctx){
    NodeResult v = std::any_cast<NodeResult>(visit(ctx->variable()));
    int line = ctx->getStart()->getLine();
    string code = v.text + "++";
    logRule(line, "factor : variable INCOP", code);

    NodeResult res;
    res.text = code;
    res.type = v.type;
    return res;
}

std::any CSubsetVisitorImpl::visitFactorDec(CSubsetParser::FactorDecContext *ctx){
    NodeResult v = std::any_cast<NodeResult>(visit(ctx->variable()));
    int line = ctx->getStart()->getLine();
    string code = v.text + "--";
    logRule(line, "factor : variable DECOP", code);

    NodeResult res;
    res.text = code;
    res.type = v.type;
    return res;
}

std::any CSubsetVisitorImpl::visitFactorFuncCall(CSubsetParser::FactorFuncCallContext *ctx){
    string name = ctx->ID()->getText();
    NodeResult args = std::any_cast<NodeResult>(visit(ctx->argument_list()));
    int line = ctx->getStart()->getLine();

    SymbolInfo* sym = symbolTable.lookUp(name);
    string retType = "ERROR";

    if(sym == nullptr){
        reportError(line, "Undeclared function " + name);
    }
    else if(!sym->getIsFunction()){
        reportError(line, name + " is not a function");
    }
    else{
        retType = sym->getReturnType();
        if(sym->getParamTypes().size() != args.paramTypes.size()){
            reportError(line, "Total number of arguments mismatch in function " + name);
        }
        else{
            for(size_t i = 0; i < args.paramTypes.size(); i++){
                if(args.paramIsArray[i]){
                    continue;
                }
                if(args.paramTypes[i] != sym->getParamTypes()[i]){
                    reportError(line, to_string(i + 1) + "th argument mismatch in function " + name);
                    break;
                }
            }
        }
    }

    string code = name + "(" + args.text + ")";
    logRule(line, "factor : ID LPAREN argument_list RPAREN", code);

    NodeResult res;
    res.text = code;
    res.type = retType;
    return res;
}

//argument list and arguments

std::any CSubsetVisitorImpl::visitArgListEmpty(CSubsetParser::ArgListEmptyContext *ctx){
    NodeResult res;
    return res;
}

std::any CSubsetVisitorImpl::visitArgListNotEmpty(CSubsetParser::ArgListNotEmptyContext *ctx){
    NodeResult args = std::any_cast<NodeResult>(visit(ctx->arguments()));
    int line = ctx->getStart()->getLine();
    logRule(line, "argument_list : arguments", args.text);
    return args;
}

std::any CSubsetVisitorImpl::visitArgsSingle(CSubsetParser::ArgsSingleContext *ctx){
    NodeResult le = std::any_cast<NodeResult>(visit(ctx->logic_expression()));
    int line = ctx->getStart()->getLine();
    logRule(line, "arguments : logic_expression", le.text);

    NodeResult res;
    res.text = le.text;
    res.paramTypes.push_back(le.type);
    res.paramIsArray.push_back(le.isArray);
    return res;
}

std::any CSubsetVisitorImpl::visitArgsAdd(CSubsetParser::ArgsAddContext *ctx){
    NodeResult args = std::any_cast<NodeResult>(visit(ctx->arguments()));
    NodeResult le = std::any_cast<NodeResult>(visit(ctx->logic_expression()));
    int line = ctx->getStart()->getLine();

    string code = args.text + "," + le.text;
    logRule(line, "arguments : arguments COMMA logic_expression", code);

    args.text = code;
    args.paramTypes.push_back(le.type);
    args.paramIsArray.push_back(le.isArray);
    return args;
}
