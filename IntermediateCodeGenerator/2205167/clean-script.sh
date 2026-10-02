#!/bin/bash

rm -f CSubsetBaseVisitor.* CSubsetVisitor.* CSubsetLexer.* CSubsetParser.*
rm -f Lexer.interp Lexer.tokens CSubset.interp CSubset.tokens
rm -f *.interp *.tokens

rm -f *.o *.out compiler.out a.out code_bin opt_code_bin
rm -f code.asm optimized_code.asm code_P1.asm opt_code_P1.asm optcode.asm
rm -f log.txt error.txt lexLogFile.txt

echo "Cleaned generated files."
