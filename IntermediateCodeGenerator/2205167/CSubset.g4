grammar CSubset;
import Lexer;

start : program # StartRule ;

program
    : program unit # ProgramMultiple
    | unit         # ProgramSingle
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
    : type_specifier ID LPAREN parameter_list RPAREN compound_statement # FuncDefWithParams
    | type_specifier ID LPAREN RPAREN compound_statement                # FuncDefNoParams
    ;

parameter_list
    : parameter_list COMMA type_specifier ID # ParamListNamed
    | parameter_list COMMA type_specifier    # ParamListUnnamed
    | type_specifier ID                      # ParamSingleNamed
    | type_specifier                         # ParamSingleUnnamed
    ;

compound_statement
    : LCURL statements RCURL # CompoundWithStmts
    | LCURL RCURL            # CompoundEmpty
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
    : declaration_list COMMA ID                                # DeclListMultiple
    | declaration_list COMMA ID LTHIRD CONST_INT RTHIRD        # DeclListMultipleArr
    | ID                                                       # DeclListSingle
    | ID LTHIRD CONST_INT RTHIRD                               # DeclListSingleArr
    ;

statements
    : statement            # StmtsSingle
    | statements statement # StmtsMultiple
    ;

statement
    : var_declaration                                                               # StmtVarDecl
    | expression_statement                                                          # StmtExpr
    | compound_statement                                                            # StmtCompound
    | FOR LPAREN expression_statement expression_statement expression RPAREN statement # StmtFor
    | IF LPAREN expression RPAREN statement ELSE statement                          # StmtIfElse
    | IF LPAREN expression RPAREN statement                                         # StmtIf
    | WHILE LPAREN expression RPAREN statement                                      # StmtWhile
    | PRINTLN LPAREN ID RPAREN SEMICOLON                                            # StmtPrintln
    | RETURN expression SEMICOLON                                                   # StmtReturn
    ;

expression_statement
    : SEMICOLON            # ExprStmtEmpty
    | expression SEMICOLON # ExprStmtExpr
    ;

variable
    : ID                          # VarSimple
    | ID LTHIRD expression RTHIRD # VarArray
    ;

expression
    : logic_expression                   # ExprLogic
    | variable ASSIGNOP logic_expression # ExprAssign
    ;

logic_expression
    : rel_expression                         # LogicExprRel
    | rel_expression LOGICOP rel_expression  # LogicExprOp
    ;

rel_expression
    : simple_expression                         # RelExprSimple
    | simple_expression RELOP simple_expression # RelExprOp
    ;

simple_expression
    : term                         # SimpleExprTerm
    | simple_expression ADDOP term # SimpleExprAdd
    ;

term
    : unary_expression            # TermUnary
    | term MULOP unary_expression # TermMul
    ;

unary_expression
    : ADDOP unary_expression # UnaryAdd
    | NOT unary_expression   # UnaryNot
    | factor                 # UnaryFactor
    ;

factor
    : variable                       # FactorVar
    | ID LPAREN argument_list RPAREN # FactorFuncCall
    | LPAREN expression RPAREN       # FactorParen
    | CONST_INT                      # FactorConstInt
    | CONST_FLOAT                    # FactorConstFloat
    | variable INCOP                 # FactorInc
    | variable DECOP                 # FactorDec
    ;

argument_list
    : arguments # ArgListArgs
    |           # ArgListEmpty
    ;

arguments
    : arguments COMMA logic_expression # ArgsMultiple
    | logic_expression                 # ArgsSingle
    ;
