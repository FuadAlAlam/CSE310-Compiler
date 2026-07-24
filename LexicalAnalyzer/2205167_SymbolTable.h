#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class SymbolInfo{
    string name;
    string type;
    SymbolInfo* next;

public:
    SymbolInfo(string name, string type){
        this->name = name;
        this->type = type;
        this->next = nullptr;
    }

    ~SymbolInfo(){}

    void setName(string name){ this->name = name; }
    void setType(string type){ this->type = type; }
    void setNext(SymbolInfo* next){ this->next = next; }

    string getName() const{ return name; }
    string getType() const{ return type; }
    SymbolInfo* getNext(){ return next; }
};

class ScopeTable{
    unsigned int num_buckets;
    string id;
    ScopeTable* parent_scope;
    SymbolInfo** buckets;

public:
    int num_children;

    unsigned int SDBMHash(string str){
        unsigned int hash = 0;
        for(size_t i = 0; i < str.length(); i++){
            hash = (str[i]) + (hash << 6) + (hash << 16) - hash;
        }
        return hash % num_buckets;
    }

    ScopeTable(unsigned int total_buckets, string scope_id, ScopeTable* parent = nullptr){
        this->num_buckets = total_buckets;
        this->id = scope_id;
        this->parent_scope = parent;
        this->num_children = 0;
        this->buckets = new SymbolInfo*[num_buckets];
        for(unsigned int i = 0; i < num_buckets; i++){
            buckets[i] = nullptr;
        }
    }

    ~ScopeTable(){
        for(unsigned int i = 0; i < num_buckets; i++){
            SymbolInfo* curr = buckets[i];
            while(curr != nullptr){
                SymbolInfo* nextNode = curr->getNext();
                delete curr;
                curr = nextNode;
            }
        }
        delete[] buckets;
    }

    string getId() const{ return id; }
    ScopeTable* getParentScope() const{ return parent_scope; }

    SymbolInfo* lookUp(string name, unsigned int& b_idx, unsigned int& s_idx){
        b_idx = SDBMHash(name);
        SymbolInfo* temp = buckets[b_idx];
        s_idx = 0;
        while(temp != nullptr){
            if(temp->getName() == name){
                return temp;
            }
            temp = temp->getNext();
            s_idx++;
        }
        return nullptr;
    }

    bool insert(SymbolInfo* sym, unsigned int& b_idx, unsigned int& s_idx){
        if(lookUp(sym->getName(), b_idx, s_idx) != nullptr){
            return false;
        }

        b_idx = SDBMHash(sym->getName());
        s_idx = 0;

        if(buckets[b_idx] == nullptr){
            buckets[b_idx] = sym;
            return true;
        }

        SymbolInfo* temp = buckets[b_idx];
        while(temp->getNext() != nullptr){
            temp = temp->getNext();
            s_idx++;
        }
        temp->setNext(sym);
        s_idx++;
        return true;
    }

    void print(ofstream& out){
        out << "ScopeTable # " << id << "\n";
        for(unsigned int i = 0; i < num_buckets; i++){
            if(buckets[i] == nullptr) continue;
            out << i << " --> ";
            SymbolInfo* temp = buckets[i];
            while(temp != nullptr){
                out << "< " << temp->getName() << " : " << temp->getType() << " >";
                temp = temp->getNext();
            }
            out << "\n";
        }
    }
};

class SymbolTable{
    ScopeTable* curr_scope;
    unsigned int total_buckets;

public:
    SymbolTable(unsigned int buckets){
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

    ScopeTable* getcurrScope(){ return curr_scope; }

    void enterScope(){
        curr_scope->num_children++;
        string new_id = curr_scope->getId() + "." + to_string(curr_scope->num_children);
        ScopeTable* new_scope = new ScopeTable(total_buckets, new_id, curr_scope);
        curr_scope = new_scope;
    }

    bool exitScope(){
        if(curr_scope->getParentScope() == nullptr){
            return false;
        }
        ScopeTable* temp = curr_scope;
        curr_scope = curr_scope->getParentScope();
        delete temp;
        return true;
    }

    bool insert(SymbolInfo* symbol, unsigned int& b_idx, unsigned int& s_idx){
        return curr_scope->insert(symbol, b_idx, s_idx);
    }

    void printAll(ofstream& out){
        ScopeTable* temp = curr_scope;
        while(temp != nullptr){
            temp->print(out);
            temp = temp->getParentScope();
        }
    }
};

#endif