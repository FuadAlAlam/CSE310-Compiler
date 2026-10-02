#include<iostream>
#include<fstream>
#include<string>
#include"antlr4-runtime.h"
#include"CSubsetLexer.h"
#include"CSubsetParser.h"

using namespace antlr4;
using namespace std;

ofstream lexLogFile; // used by Lexer.g4's writeIntoLexLogFile()

int main(int argc, const char* argv[]){
    if(argc < 2){
        cerr<<"Usage: "<<argv[0]<<" <input_file>"<<endl;
        return 1;
    }

    ifstream inputFile(argv[1]);
    if(!inputFile.is_open()){
        cerr<<"Error opening input file: "<<argv[1]<<endl;
        return 1;
    }

    ANTLRInputStream input(inputFile);
    CSubsetLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    CSubsetParser parser(&tokens);

    CSubsetParser::StartContext* tree = parser.start();

    cout<<"Parsing completed."<<endl;

    inputFile.close();
    return 0;
}
