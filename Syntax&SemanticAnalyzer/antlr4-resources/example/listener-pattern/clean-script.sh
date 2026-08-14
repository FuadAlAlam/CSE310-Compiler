#!/bin/bash

# Enable extended globbing for pattern matching
shopt -s extglob

# Loop through all files that do NOT match *.sh, *.g4, main.cpp, EvalListener.*, compile_commands.json, or input.txt
for file in !(*.sh|*.g4|main.cpp|EvalListener.h|EvalListener.cpp|compile_commands.json|input.txt); do
    # Only delete if it's a regular file
    if [[ -f "$file" ]]; then
        rm -f "$file"
    fi
done
