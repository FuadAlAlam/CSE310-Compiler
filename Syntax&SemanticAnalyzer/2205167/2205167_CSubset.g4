grammar CSubset;
import Lexer;

start : program # StartRule ;

program
    : p=program u=unit # ProgramUnit
    | u=unit           # UnitOnly
    ;

unit
    : var_declaration  # UnitVarDecl
    | func_declaration # UnitFuncDecl
    | func_definition  # UnitFuncDef
    ;

func_declaration
    : type_specifier ID LPAREN parameter_list RPAREN SEMICOLON # FuncDeclWithParams
    | type_specifier ID LPAREN RPAREN SEMICOLON                # FuncDeclNoParams
    ;

func_definition
    : type_specifier ID LPAREN parameter_list RPAREN compound_statement              # FuncDefWithParams
    | type_specifier ID LPAREN parameter_list error_tokens RPAREN compound_statement # FuncDefErrorParams
    | type_specifier ID LPAREN RPAREN compound_statement                             # FuncDefNoParams
    ;

parameter_list
    : parameter_list COMMA type_specifier ID # ParamListAdd
    | parameter_list COMMA type_specifier    # ParamListAddNoName
    | type_specifier ID                      # ParamSingle
    | type_specifier                         # ParamSingleNoName
    ;

compound_statement
    : LCURL statements RCURL # CompoundStmtBody
    | LCURL RCURL            # CompoundStmtEmpty
    ;

var_declaration
    : type_specifier declaration_list SEMICOLON # VarDecl
    ;

type_specifier
    : INT   # TypeInt
    | FLOAT # TypeFloat
    | VOID  # TypeVoid
    ;

declaration_list
    : dl=declaration_list COMMA ID                               # DeclListVar
    | dl=declaration_list COMMA ID LTHIRD CONST_INT RTHIRD       # DeclListArray
    | ID                                                         # DeclVar
    | ID LTHIRD CONST_INT RTHIRD                                 # DeclArray
    | dl=declaration_list error_tokens COMMA ID                  # DeclListErrorVar
    | dl=declaration_list error_tokens COMMA ID LTHIRD CONST_INT RTHIRD # DeclListErrorArray
    ;

error_tokens
    : (ADDOP | MULOP | INCOP | DECOP | NOT | RELOP | LOGICOP | ASSIGNOP | CONST_INT | CONST_FLOAT | ID)+
    ;

statements
    : statement            # StmtsSingle
    | statements statement # StmtsAdd
    ;

statement
    : var_declaration                                                               # StmtVarDecl
    | expression_statement                                                          # StmtExpr
    | compound_statement                                                            # StmtCompound
    | FOR LPAREN es1=expression_statement es2=expression_statement e=expression RPAREN s=statement # StmtFor
    | IF LPAREN e=expression RPAREN s1=statement (ELSE s2=statement)?               # StmtIf
    | WHILE LPAREN e=expression RPAREN s=statement                                  # StmtWhile
    | PRINTLN LPAREN ID RPAREN SEMICOLON                                            # StmtPrint
    | RETURN e=expression SEMICOLON                                                 # StmtReturn
    ;

expression_statement
    : SEMICOLON                   # ExprStmtSemicolon
    | expression SEMICOLON        # ExprStmtExpr
    | expression                  # ExprStmtNoSemicolon
    ;

variable
    : ID                              # VarSimple
    | ID LTHIRD expression RTHIRD     # VarArray
    ;

expression
    : logic_expression                                # ExprLogic
    | variable ASSIGNOP logic_expression              # ExprAssign
    ;

logic_expression
    : rel_expression                                  # LogicRel
    | le1=rel_expression LOGICOP le2=rel_expression   # LogicOp
    ;

rel_expression
    : simple_expression                               # RelSimple
    | se1=simple_expression RELOP se2=simple_expression# RelOp
    ;

simple_expression
    : term                                 # SimpleTerm
    | se=simple_expression ADDOP t=term    # SimpleAdd
    | se=simple_expression ADDOP ASSIGNOP  # SimpleAddErrorAssign
    ;

term
    : unary_expression                  # TermUnary
    | t=term MULOP ue=unary_expression  # TermMul
    ;

unary_expression
    : ADDOP unary_expression            # UnaryAdd
    | NOT unary_expression              # UnaryNot
    | factor                            # UnaryFactor
    ;

factor
    : variable                          # FactorVar
    | ID LPAREN argument_list RPAREN    # FactorFuncCall
    | LPAREN expression RPAREN          # FactorParen
    | CONST_INT                         # FactorInt
    | CONST_FLOAT                       # FactorFloat
    | variable INCOP                    # FactorInc
    | variable DECOP                    # FactorDec
    ;

argument_list
    : arguments                         # ArgListNotEmpty
    |                                   # ArgListEmpty
    ;

arguments
    : arguments COMMA logic_expression  # ArgsAdd
    | logic_expression                  # ArgsSingle
    ;
