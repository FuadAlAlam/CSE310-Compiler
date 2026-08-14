#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include "ScopeTable.h"

using namespace std;

class SymbolTable{
private:
    ScopeTable* curr_scope;
    unsigned int total_buckets;

public:
    SymbolTable(unsigned int buckets = 30){
        total_buckets = buckets;
        curr_scope = new ScopeTable(total_buckets, "1", nullptr);
    }

    ~SymbolTable(){
        while(curr_scope != nullptr){
            ScopeTable* temp = curr_scope->getParentScope();
            delete curr_scope;
            curr_scope = temp;
        }
    }

    ScopeTable* getCurrScope(){ return curr_scope; }

    void enterScope(){
        string child_id = curr_scope->getId() + "." + to_string(curr_scope->getNextChildCount());
        ScopeTable* new_scope = new ScopeTable(total_buckets, child_id, curr_scope);
        curr_scope = new_scope;
    }

    bool exitScope(ofstream& out){
        if(curr_scope == nullptr) return false;
        out << "\n\n\n";
        printAll(out);
        out << "\n\n";
        
        ScopeTable* temp = curr_scope;
        curr_scope = curr_scope->getParentScope();
        delete temp;
        return true;
    }

    bool exitScopeQuiet(){
        if(curr_scope == nullptr) return false;
        ScopeTable* temp = curr_scope;
        curr_scope = curr_scope->getParentScope();
        delete temp;
        return true;
    }

    bool insert(SymbolInfo* symbol){
        if(curr_scope == nullptr) return false;
        return curr_scope->insert(symbol);
    }

    bool remove(const string& name){
        if(curr_scope == nullptr) return false;
        return curr_scope->remove(name);
    }

    SymbolInfo* lookUp(const string& name){
        ScopeTable* temp = curr_scope;
        while(temp != nullptr){
            SymbolInfo* found = temp->lookUp(name);
            if(found != nullptr){
                return found;
            }
            temp = temp->getParentScope();
        }
        return nullptr;
    }

    SymbolInfo* lookUpCurrentScope(const string& name){
        if(curr_scope == nullptr) return nullptr;
        return curr_scope->lookUp(name);
    }

    void printCurrent(ofstream& out){
        if(curr_scope != nullptr){
            curr_scope->print(out);
        }
    }

    void printAll(ofstream& out){
        ScopeTable* temp = curr_scope;
        while(temp != nullptr){
            temp->print(out);
            if(temp->getParentScope() != nullptr){
                out << "\n\n\n";
            }
            temp = temp->getParentScope();
        }
    }
};
