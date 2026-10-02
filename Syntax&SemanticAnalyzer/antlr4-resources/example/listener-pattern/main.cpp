#include<iostream>
#include<fstream>
#include"antlr4-runtime.h"
#include"ExpressionLexer.h"
#include"ExpressionParser.h"
#include"EvalListener.h"

using namespace antlr4;
using namespace tree;
using namespace std;

int main(int argc, const char* argv[]){
    if(argc < 2){
        cerr<<"Usage: "<<argv[0]<<" <input_file>"<<endl;
        return 1;
    }

    // ---- Input File ----
    ifstream inputFile(argv[1]);
    if(!inputFile.is_open()){
        cerr<<"Error opening input file: "<<argv[1]<<endl;
        return 1;
    }

    // ---- Parsing Flow ----
    ANTLRInputStream input(inputFile);
    ExpressionLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    ExpressionParser parser(&tokens);

    // this is necessary to avoid the default error listener and use our custom error handling
    parser.removeErrorListeners();

    // start parsing at the 'start' rule
    ParseTree *tree = parser.start();

    // ---- Listener Flow ----
    EvalListener listener;
    ParseTreeWalker::DEFAULT.walk(&listener, tree);
    cout<<listener.result()<<endl;

    // clean up
    inputFile.close();
    cout<<"Parsing completed"<<endl;
    return 0;
}
