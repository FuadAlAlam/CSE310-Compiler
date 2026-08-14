lexer grammar Lexer;

INT: [0-9]+ ;
PLUS: '+' ;
TIMES: '*' ;
LPAREN: '(';
RPAREN: ')';

WS: [ \r\n]+ -> skip ;
