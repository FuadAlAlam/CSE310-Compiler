#pragma once
#include<string>
#include<vector>

using namespace std;

class SymbolInfo{
private:
    string name;
    string type;
    string varType;
    bool isArray;
    int arraySize;
    bool isFunction;
    bool isDefined;
    string returnType;
    vector<string> paramTypes;
    vector<string> paramNames;
    bool isGlobal;
    bool isParam;
    int stackOffset;
    SymbolInfo* next;

public:
    SymbolInfo(string name = "", string type = ""){
        this->name = name;
        this->type = type;
        this->varType = "";
        this->isArray = false;
        this->arraySize = 0;
        this->isFunction = false;
        this->isDefined = false;
        this->returnType = "";
        this->isGlobal = false;
        this->isParam = false;
        this->stackOffset = 0;
        this->next = nullptr;
    }

    ~SymbolInfo(){}

    void setName(string name){ this->name = name; }
    void setType(string type){ this->type = type; }
    void setVarType(string varType){ this->varType = varType; }
    void setIsArray(bool isArray){ this->isArray = isArray; }
    void setArraySize(int size){ this->arraySize = size; }
    void setIsFunction(bool isFunction){ this->isFunction = isFunction; }
    void setIsDefined(bool isDefined){ this->isDefined = isDefined; }
    void setReturnType(string returnType){ this->returnType = returnType; }
    void setParamTypes(const vector<string>& pTypes){ this->paramTypes = pTypes; }
    void setParamNames(const vector<string>& pNames){ this->paramNames = pNames; }
    void addParam(string pType, string pName = ""){
        this->paramTypes.push_back(pType);
        this->paramNames.push_back(pName);
    }
    void setIsGlobal(bool isGlobal){ this->isGlobal = isGlobal; }
    void setIsParam(bool isParam){ this->isParam = isParam; }
    void setStackOffset(int offset){ this->stackOffset = offset; }
    void setNext(SymbolInfo* next){ this->next = next; }

    string getName() const{ return name; }
    string getType() const{ return type; }
    string getVarType() const{ return varType; }
    bool getIsArray() const{ return isArray; }
    int getArraySize() const{ return arraySize; }
    bool getIsFunction() const{ return isFunction; }
    bool getIsDefined() const{ return isDefined; }
    string getReturnType() const{ return returnType; }
    const vector<string>& getParamTypes() const{ return paramTypes; }
    const vector<string>& getParamNames() const{ return paramNames; }
    bool getIsGlobal() const{ return isGlobal; }
    bool getIsParam() const{ return isParam; }
    int getStackOffset() const{ return stackOffset; }
    SymbolInfo* getNext() const{ return next; }

    string getAsmOperand() const{
        if(isGlobal){
            return "[" + name + "]";
        } else if(isParam){
            return "[EBP+" + to_string(stackOffset) + "]";
        } else{
            return "[EBP-" + to_string(stackOffset) + "]";
        }
    }
};
