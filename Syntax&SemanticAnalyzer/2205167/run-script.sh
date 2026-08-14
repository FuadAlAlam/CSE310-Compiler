#!/bin/bash

if command -v antlr4 &> /dev/null; then
    ANTLR4=antlr4
elif [ -f "../../venv/bin/antlr4" ]; then
    ANTLR4="../../venv/bin/antlr4"
elif [ -f "../../../venv/bin/antlr4" ]; then
    ANTLR4="../../../venv/bin/antlr4"
else
    ANTLR4=antlr4
fi

if [ -f "2205167_CSubset.g4" ]; then
    cp -f 2205167_CSubset.g4 CSubset.g4
fi
if [ -f "2205167_Lexer.g4" ]; then
    cp -f 2205167_Lexer.g4 Lexer.g4
fi

$ANTLR4 -v 4.13.2 -Dlanguage=Cpp -visitor -no-listener CSubset.g4
g++ -std=c++17 -w -I/usr/local/include/antlr4-runtime *.cpp -L/usr/local/lib/ -lantlr4-runtime -pthread -o compiler.out
LD_LIBRARY_PATH=/usr/local/lib ./compiler.out $1
