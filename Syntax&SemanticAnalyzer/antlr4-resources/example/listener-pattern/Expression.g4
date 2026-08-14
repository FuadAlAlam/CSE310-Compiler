grammar Expression;
import Lexer;

start : expression ;

expression : expression PLUS term  # Add
           | term                  # ExprTerm
           ;

term : term TIMES factor # Mul
     | factor             # TermFactor
     ;

factor : LPAREN expression RPAREN # Paren
       | INT                      # Int
       ;
