#!/bin/bash

# Find ANTLR4
if command -v antlr4 &> /dev/null; then
    ANTLR4=antlr4
elif [ -f "../../venv/bin/antlr4" ]; then
    ANTLR4="../../venv/bin/antlr4"
elif [ -f "../../../venv/bin/antlr4" ]; then
    ANTLR4="../../../venv/bin/antlr4"
elif [ -f "/home/fuad/Documents/my_compiler/venv/bin/antlr4" ]; then
    ANTLR4="/home/fuad/Documents/my_compiler/venv/bin/antlr4"
else
    ANTLR4=antlr4
fi

echo "Generating parser and visitor files..."
$ANTLR4 -v 4.13.2 -Dlanguage=Cpp -visitor -no-listener CSubset.g4

echo "Compiling compiler executable..."
g++ -std=c++17 -w -I/usr/local/include/antlr4-runtime *.cpp -L/usr/local/lib/ -lantlr4-runtime -pthread -o compiler.out

if [ $? -ne 0 ]; then
    echo "Build failed."
    exit 1
fi

INPUT_FILE=${1:-"../Assignment 4 ICG Resources-20260827T121511Z-1-001/Assignment 4 ICG Resources/input/test1_i.c"}

echo "Running compiler on $INPUT_FILE..."
LD_LIBRARY_PATH=/usr/local/lib ./compiler.out "$INPUT_FILE"

# Find FASM
FASM_BIN=""
if command -v fasm &> /dev/null; then
    FASM_BIN="fasm"
elif [ -f "$HOME/.local/bin/fasm" ]; then
    FASM_BIN="$HOME/.local/bin/fasm"
elif [ -f "/home/fuad/.local/bin/fasm" ]; then
    FASM_BIN="/home/fuad/.local/bin/fasm"
fi

if [ -n "$FASM_BIN" ] && [ -f "code.asm" ]; then
    echo "Assembling code.asm with FASM..."
    $FASM_BIN code.asm code_bin
    chmod +x code_bin
    echo "--- Output of code.asm execution ---"
    ./code_bin
    echo "------------------------------------"

    if [ -f "optimized_code.asm" ]; then
        echo "Assembling optimized_code.asm with FASM..."
        $FASM_BIN optimized_code.asm opt_code_bin
        chmod +x opt_code_bin
        echo "--- Output of optimized_code.asm execution ---"
        ./opt_code_bin
        echo "---------------------------------------------"
    fi
fi
