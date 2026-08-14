#include <iostream>
#include <fstream>
#include <string>
#include "antlr4-runtime.h"
#include "CSubsetLexer.h"
#include "CSubsetParser.h"
#include "CSubsetVisitorImpl.h"

using namespace antlr4;
using namespace std;

ofstream lexLogFile;

int countLines(const string& filename){
    ifstream file(filename);
    if(!file.is_open()) return 0;
    string line;
    int count = 0;
    while(getline(file, line)){
        count++;
    }
    return count;
}

int main(int argc, const char* argv[]){
    if(argc < 2){
        cerr << "Usage: " << argv[0] << " <input_file>" << endl;
        return 1;
    }

    string inputFilename = argv[1];
    int totalLines = countLines(inputFilename);

    ifstream inputFile(inputFilename);
    if(!inputFile.is_open()){
        cerr << "Error opening input file: " << inputFilename << endl;
        return 1;
    }

    ofstream logFile("log.txt");
    ofstream errorFile("error.txt");

    if(!logFile.is_open() || !errorFile.is_open()){
        cerr << "Error opening output files (log.txt / error.txt)" << endl;
        return 1;
    }

    ANTLRInputStream input(inputFile);
    CSubsetLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    CSubsetParser parser(&tokens);

    parser.removeErrorListeners();

    CSubsetParser::StartContext* tree = parser.start();

    CSubsetVisitorImpl visitor(logFile, errorFile, totalLines);
    visitor.visit(tree);

    inputFile.close();
    logFile.close();
    errorFile.close();
    if(lexLogFile.is_open()){
        lexLogFile.close();
    }

    cout << "Parsing completed." << endl;
    return 0;
}
