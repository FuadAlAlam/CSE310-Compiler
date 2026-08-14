#!/bin/bash

if [ -f "2205167_CSubset.g4" ]; then
    rm -f CSubset.g4
fi
if [ -f "2205167_Lexer.g4" ]; then
    rm -f Lexer.g4
fi

rm -f CSubsetBaseVisitor.* CSubsetVisitor.* CSubsetLexer.* CSubsetParser.*
rm -f Lexer.interp Lexer.tokens CSubset.interp CSubset.tokens
rm -f *.interp *.tokens

rm -f *.o *.out compiler.out a.out
rm -f log.txt error.txt lexLogFile.txt

shopt -s extglob 2>/dev/null
for file in !(*.sh|*.g4|main.cpp|CSubsetVisitorImpl.h|CSubsetVisitorImpl.cpp|SymbolInfo.h|ScopeTable.h|SymbolTable.h); do
    if [[ -f "$file" ]]; then
        rm -f "$file"
    fi
done
