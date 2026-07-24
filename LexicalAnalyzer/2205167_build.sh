#!/usr/bin/bash

flex 2205167.l
g++ -Wall -Wextra -O3 -std=c++17 lex.yy.c -o lexer.out
./lexer.out input2.txt
