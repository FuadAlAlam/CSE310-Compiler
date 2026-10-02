#include<iostream>
#include<fstream>
#include<string>
#include"antlr4-runtime.h"
#include"CSubsetLexer.h"
#include"CSubsetParser.h"
#include"ICGVisitor.h"
#include"PeepholeOptimizer.h"

using namespace antlr4;
using namespace std;

ofstream lexLogFile;

int main(int argc, const char* argv[]){
    if(argc < 2){
        cerr<<"Usage: "<<argv[0]<<" <input_file>"<<endl;
        return 1;
    }

    string inputFilename = argv[1];
    ifstream inputFile(inputFilename);
    if(!inputFile.is_open()){
        cerr<<"Error opening input file: "<<inputFilename<<endl;
        return 1;
    }

    ofstream codeFile("code.asm");
    if(!codeFile.is_open()){
        cerr<<"Error opening code.asm for output"<<endl;
        return 1;
    }

    ANTLRInputStream input(inputFile);
    CSubsetLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    CSubsetParser parser(&tokens);

    parser.removeErrorListeners();

    auto tree = parser.start();

    string printProcPath = "printProc.lib";
    ifstream testLib(printProcPath);
    if(!testLib.is_open()){
        printProcPath = "../Assignment 4 ICG Resources/printProc.lib";
    } else{
        testLib.close();
    }

    ICGVisitor visitor(codeFile, printProcPath);
    visitor.visit(tree);

    inputFile.close();
    codeFile.close();
    if(lexLogFile.is_open()){
        lexLogFile.close();
    }

    PeepholeOptimizer::optimize("code.asm", "optimized_code.asm");

    cout<<"Compilation and code generation completed successfully."<<endl;
    return 0;
}
