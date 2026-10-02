#pragma once

#include<string>
#include<vector>

using namespace std;

class PeepholeOptimizer{
public:
    static void optimize(const string& inputFilename, const string& outputFilename);

private:
    static bool optimizePass(vector<string>& lines);
    static string trim(const string& str);
    static string stripComment(const string& line);
    static bool isLabel(const string& line, string& labelName);
    static bool isInstruction(const string& line);
};
