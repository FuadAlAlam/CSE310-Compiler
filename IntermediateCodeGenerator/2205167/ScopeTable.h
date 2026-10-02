#pragma once
#include<iostream>
#include<fstream>
#include<string>
#include"SymbolInfo.h"

using namespace std;

class ScopeTable{
private:
    unsigned int num_buckets;
    string id;
    int child_count;
    ScopeTable* parent_scope;
    SymbolInfo** buckets;

    unsigned int SDBMHash(const string& str){
        unsigned int hash = 0;
        for(char ch : str){
            hash = (hash + (unsigned char)ch) % num_buckets;
        }
        return hash;
    }

public:
    ScopeTable(unsigned int total_buckets, string scope_id, ScopeTable* parent = nullptr){
        this->num_buckets = total_buckets;
        this->id = scope_id;
        this->child_count = 0;
        this->parent_scope = parent;
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
    int getNextChildCount(){ return ++child_count; }
    ScopeTable* getParentScope() const{ return parent_scope; }

    SymbolInfo* lookUp(const string& name){
        unsigned int b_idx = SDBMHash(name);
        SymbolInfo* temp = buckets[b_idx];
        while(temp != nullptr){
            if(temp->getName() == name){
                return temp;
            }
            temp = temp->getNext();
        }
        return nullptr;
    }

    bool insert(SymbolInfo* sym){
        if(lookUp(sym->getName()) != nullptr){
            return false;
        }
        unsigned int b_idx = SDBMHash(sym->getName());
        if(buckets[b_idx] == nullptr){
            buckets[b_idx] = sym;
            return true;
        }
        SymbolInfo* temp = buckets[b_idx];
        while(temp->getNext() != nullptr){
            temp = temp->getNext();
        }
        temp->setNext(sym);
        return true;
    }

    bool remove(const string& name){
        unsigned int b_idx = SDBMHash(name);
        SymbolInfo* temp = buckets[b_idx];
        SymbolInfo* prev = nullptr;
        while(temp != nullptr){
            if(temp->getName() == name){
                if(prev == nullptr){
                    buckets[b_idx] = temp->getNext();
                } else{
                    prev->setNext(temp->getNext());
                }
                delete temp;
                return true;
            }
            prev = temp;
            temp = temp->getNext();
        }
        return false;
    }

    void print(ofstream& out){
        out<<"ScopeTable # "<<id<<"\n";
        for(unsigned int i = 0; i < num_buckets; i++){
            if(buckets[i] != nullptr){
                out<<" "<<i<<" --> ";
                SymbolInfo* temp = buckets[i];
                while(temp != nullptr){
                    out<<"< "<<temp->getName()<<" , "<<temp->getType()<<" >";
                    if(!temp->getIsFunction()){
                        out<<" ";
                    }
                    temp = temp->getNext();
                }
                out<<"\n";
            }
        }
    }
};
