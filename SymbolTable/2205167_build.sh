#!/usr/bin/bash

g++ -Wall -Wextra -O3 -fsanitize=address 2205167_SymbolTable.cpp -o symbol_table.out
./symbol_table.out input.txt output.txt
