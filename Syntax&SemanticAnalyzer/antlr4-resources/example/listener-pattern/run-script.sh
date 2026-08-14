#!/bin/bash

antlr4 -v 4.13.2 -Dlanguage=Cpp Expression.g4
g++ -std=c++17 -w -I/usr/local/include/antlr4-runtime *.cpp -L/usr/local/lib/ -lantlr4-runtime -pthread
LD_LIBRARY_PATH=/usr/local/lib ./a.out $1
