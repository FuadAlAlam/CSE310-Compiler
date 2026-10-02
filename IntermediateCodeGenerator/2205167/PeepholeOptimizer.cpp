#include"PeepholeOptimizer.h"
#include<iostream>
#include<fstream>
#include<sstream>
#include<algorithm>
#include<unordered_map>
#include<unordered_set>

using namespace std;

string PeepholeOptimizer::trim(const string& str){
    size_t first = str.find_first_not_of(" \t\r\n");
    if(first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

string PeepholeOptimizer::stripComment(const string& line){
    size_t pos = line.find(';');
    if(pos != string::npos){
        return trim(line.substr(0, pos));
    }
    return trim(line);
}

bool PeepholeOptimizer::isLabel(const string& line, string& labelName){
    string clean = stripComment(line);
    if(clean.empty()) return false;
    if(clean.back() == ':'){
        labelName = clean.substr(0, clean.length() - 1);
        return true;
    }
    return false;
}

bool PeepholeOptimizer::isInstruction(const string& line){
    string clean = stripComment(line);
    if(clean.empty()) return false;
    if(clean.back() == ':') return false;
    if(clean.find("format ") == 0 || clean.find("entry ") == 0 || clean.find("segment ") == 0) return false;
    return true;
}

bool PeepholeOptimizer::optimizePass(vector<string>& lines){
    bool changed = false;
    vector<string> newLines;

    int printLibStart = -1;
    for(size_t i = 0; i < lines.size(); i++){
        if(lines[i].find("print_number:") != string::npos || 
            lines[i].find(";         print library") != string::npos){
            printLibStart = (int)i;
            break;
        }
    }

    size_t codeLimit = (printLibStart >= 0) ? (size_t)printLibStart : lines.size();

    unordered_map<string, string> labelAliases;
    vector<bool> removeFlag(lines.size(), false);

    for(size_t i = 0; i < codeLimit; i++){
        string lbl1;
        if(isLabel(lines[i], lbl1)){
            size_t j = i + 1;
            while(j < codeLimit && stripComment(lines[j]).empty()){
                j++;
            }
            if(j < codeLimit){
                string lbl2;
                if(isLabel(lines[j], lbl2)){
                    labelAliases[lbl1] = lbl2;
                    removeFlag[i] = true;
                    changed = true;
                }
            }
        }
    }

    if(!labelAliases.empty()){
        for(auto& pair : labelAliases){
            string curr = pair.second;
            while(labelAliases.find(curr) != labelAliases.end()){
                curr = labelAliases[curr];
            }
            pair.second = curr;
        }

        for(size_t i = 0; i < codeLimit; i++){
            for(const auto& pair : labelAliases){
                string target = pair.first;
                string replacement = pair.second;
                size_t pos = 0;
                while((pos = lines[i].find(target, pos)) != string::npos){
                    bool leftOk = (pos == 0 || (!isalnum(lines[i][pos - 1]) && lines[i][pos - 1] != '_' && lines[i][pos - 1] != '.'));
                    bool rightOk = (pos + target.length() >= lines[i].length() || 
                                    (!isalnum(lines[i][pos + target.length()]) && lines[i][pos + target.length()] != '_' && lines[i][pos + target.length()] != '.'));
                    if(leftOk && rightOk && lines[i].find(target + ":") == string::npos){
                        lines[i].replace(pos, target.length(), replacement);
                        pos += replacement.length();
                    } else{
                        pos += target.length();
                    }
                }
            }
        }
    }

    for(size_t i = 0; i < lines.size(); i++){
        if(removeFlag[i]) continue;

        if(i >= codeLimit){
            newLines.push_back(lines[i]);
            continue;
        }

        string clean1 = stripComment(lines[i]);

        if(i + 1 < codeLimit){
            string clean2 = stripComment(lines[i + 1]);
            if(clean1.find("PUSH ") == 0 && clean2.find("POP ") == 0){
                string reg1 = trim(clean1.substr(5));
                string reg2 = trim(clean2.substr(4));
                if(reg1 == reg2 && !reg1.empty()){
                    i++;
                    changed = true;
                    continue;
                }
            }
        }

        if(i + 1 < codeLimit){
            string clean2 = stripComment(lines[i + 1]);
            if(clean1.find("MOV ") == 0 && clean2.find("MOV ") == 0){
                size_t c1 = clean1.find(',');
                size_t c2 = clean2.find(',');
                if(c1 != string::npos && c2 != string::npos){
                    string a1 = trim(clean1.substr(4, c1 - 4));
                    string b1 = trim(clean1.substr(c1 + 1));
                    string a2 = trim(clean2.substr(4, c2 - 4));
                    string b2 = trim(clean2.substr(c2 + 1));
                    if(a1 == b2 && b1 == a2){
                        newLines.push_back(lines[i]);
                        i++;
                        changed = true;
                        continue;
                    }
                }
            }
        }

        if(i + 1 < codeLimit){
            string clean2 = stripComment(lines[i + 1]);
            if(clean1.find("MOV ") == 0 && clean2.find("MOV ") == 0){
                size_t c1 = clean1.find(',');
                size_t c2 = clean2.find(',');
                if(c1 != string::npos && c2 != string::npos){
                    string dst1 = trim(clean1.substr(4, c1 - 4));
                    string src1 = trim(clean1.substr(c1 + 1));
                    string dst2 = trim(clean2.substr(4, c2 - 4));
                    string src2 = trim(clean2.substr(c2 + 1));
                    if(dst1 == src2 && src1 == dst2){
                        newLines.push_back(lines[i]);
                        i++;
                        changed = true;
                        continue;
                    }
                }
            }
        }

        if(clean1.find("ADD ") == 0 || clean1.find("SUB ") == 0){
            size_t comma = clean1.find(',');
            if(comma != string::npos){
                string val = trim(clean1.substr(comma + 1));
                if(val == "0"){
                    changed = true;
                    continue;
                }
            }
        }

        if(i + 1 < codeLimit){
            string clean2 = stripComment(lines[i + 1]);
            if(clean1 == "MOV EBX, 1" && (clean2 == "MUL EBX" || clean2 == "IMUL EBX")){
                i++;
                changed = true;
                continue;
            }
        }

        if(clean1.find("JMP ") == 0){
            newLines.push_back(lines[i]);
            size_t j = i + 1;
            while(j < codeLimit){
                string nextClean = stripComment(lines[j]);
                string dummyLbl;
                if(isLabel(lines[j], dummyLbl) || nextClean.empty()){
                    break;
                }
                j++;
                changed = true;
            }
            i = j - 1;
            continue;
        }

        newLines.push_back(lines[i]);
    }

    lines = newLines;
    return changed;
}

void PeepholeOptimizer::optimize(const string& inputFilename, const string& outputFilename){
    ifstream inFile(inputFilename);
    if(!inFile.is_open()){
        cerr<<"Error opening input file for optimization: "<<inputFilename<<endl;
        return;
    }

    vector<string> lines;
    string line;
    while(getline(inFile, line)){
        lines.push_back(line);
    }
    inFile.close();

    int pass = 0;
    while(optimizePass(lines) && pass < 20){
        pass++;
    }

    ofstream outFile(outputFilename);
    if(!outFile.is_open()){
        cerr<<"Error opening output file for optimization: "<<outputFilename<<endl;
        return;
    }

    for(const auto& l : lines){
        outFile<<l<<"\n";
    }
    outFile.close();
}
