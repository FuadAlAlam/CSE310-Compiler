grammar Expression;
import Lexer;

@parser::header {
#include <cstdlib>
#include <iostream>
#include <string>
  using namespace std;
}

start : expression { cout << $expression.value << endl; };

expression returns[int value] : e = expression PLUS term {
  $value = $e.value + $term.value;
} |
term { $value = $term.value; };

term returns[int value] : t = term TIMES factor {
  $value = $t.value * $factor.value;
} |
factor { $value = $factor.value; };

factor returns[int value] : LPAREN expression RPAREN {
  $value = $expression.value;
} |
INT { $value = stoi($INT.text); };
