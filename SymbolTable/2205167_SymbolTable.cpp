#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

class SymbolInfo{

    string name;
    string type;
    SymbolInfo* next;

public:
    SymbolInfo(){
        this->next = nullptr;
    }

    SymbolInfo(string name, string type){
        this->name = name;
        this->type = type;
        this->next = nullptr;
    }

    ~SymbolInfo(){
        
    }

    void setName(string name){
        this->name = name;
    }

    void setType(string type){
        this->type = type;
    }

    void setNext(SymbolInfo* next){
        this->next = next;
    }

    string getName() const{
        return name;
    }

    string getType() const{
        return type;
    }

    SymbolInfo* getNext(){
        return next;
    }
};

class ScopeTable{

    unsigned int num_buckets;
    unsigned int id;
    ScopeTable* parent_scope;
    SymbolInfo** buckets;
 
    unsigned int SDBMHash(string str){
        unsigned int hash = 0;
        for(size_t i = 0; i < str.length(); i++){
            hash = ((str[i]) + (hash << 6) + (hash << 16) - hash) % num_buckets;
        }
        return hash; 
    }

public:
    ScopeTable(unsigned int total_buckets, unsigned int scope_id, ScopeTable* parent = nullptr){
        this->num_buckets = total_buckets;
        this->id = scope_id;
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

    unsigned int getId() const{
        return id;
    }

    ScopeTable* getParentScope() const{
        return parent_scope;
    }

    SymbolInfo* lookUp(string name, unsigned int& b_idx, unsigned int& s_idx){
        b_idx = SDBMHash(name);
        SymbolInfo* temp = buckets[b_idx];
        s_idx = 1;
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
        s_idx = 1;

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

    bool remove(string name, unsigned int& b_idx, unsigned int& s_idx){
        b_idx = SDBMHash(name);
        SymbolInfo* temp = buckets[b_idx];
        SymbolInfo* prev = nullptr;
        s_idx = 1;

        while(temp != nullptr){
            if(temp->getName() == name){
                if(prev == nullptr){
                    buckets[b_idx] = temp->getNext();
                }
                else{
                    prev->setNext(temp->getNext());
                }
                delete temp;
                return true;
            }
            prev = temp;
            temp = temp->getNext();
            s_idx++;
        }
        return false;
    }

    void print(ofstream& out, int indentation_level = 1){
        string tabs = "";
        for(int i = 0; i < indentation_level; i++){
            tabs += "\t";
        }

        out << tabs << "ScopeTable# " << id << "\n";
        for(unsigned int i = 0; i < num_buckets; i++){
            out << tabs << (i + 1) << "--> ";
            SymbolInfo* temp = buckets[i];
            while(temp != nullptr){
                out << "<" << temp->getName() << "," << temp->getType() << "> ";
                temp = temp->getNext();
            }
            out << "\n";
        }
    }
};

class SymbolTable{

    ScopeTable* curr_scope;
    unsigned int total_buckets;
    unsigned int scope_counter;

public:
    SymbolTable(unsigned int buckets){
        total_buckets = buckets;
        scope_counter = 1;
        curr_scope = new ScopeTable(total_buckets, scope_counter, nullptr);
    }

    ~SymbolTable(){
        while(curr_scope != nullptr){
            ScopeTable* temp = curr_scope->getParentScope();
            delete curr_scope;
            curr_scope = temp;
        }
    }

    ScopeTable* getcurrScope(){
        return curr_scope;
    }

    void enterScope(ofstream& out){
        scope_counter++;
        ScopeTable* new_scope = new ScopeTable(total_buckets, scope_counter, curr_scope);
        curr_scope = new_scope;
        out << "\tScopeTable# " << curr_scope->getId() << " created\n";
    }

    bool exitScope(ofstream& out){
        if (curr_scope->getParentScope() == nullptr) {
            return false;
        }
        out << "\tScopeTable# " << curr_scope->getId() << " removed\n";
        ScopeTable* temp = curr_scope;
        curr_scope = curr_scope->getParentScope();
        delete temp;
        return true;
    }

    bool insert(SymbolInfo* symbol, unsigned int& b_idx, unsigned int& s_idx){
        return curr_scope->insert(symbol, b_idx, s_idx);
    }

    bool remove(string name, unsigned int& b_idx, unsigned int& s_idx){
        return curr_scope->remove(name, b_idx, s_idx);
    }

    SymbolInfo* lookUp(string name, unsigned int& scope_id, unsigned int& b_idx, unsigned int& s_idx){
        ScopeTable* temp = curr_scope;
        while(temp != nullptr){
            SymbolInfo* found = temp->lookUp(name, b_idx, s_idx);
            if(found != nullptr){
                scope_id = temp->getId();
                return found;
            }
            temp = temp->getParentScope();
        }
        return nullptr;
    }

    void printCurrent(ofstream& out){
        curr_scope->print(out, 1);
    }

    void printAll(ofstream& out){
        ScopeTable* temp = curr_scope;
        int depth = 1;
        while(temp != nullptr){
            temp->print(out, depth);
            temp = temp->getParentScope();
            depth++;
        }
    }
};

int main(int argc, char* argv[]){
    if(argc < 3){
        cout << "Usage: " << argv[0] << " <input_file> <output_file>" << endl;
        return 1;
    }

    ifstream infile(argv[1]);
    ofstream outfile(argv[2]);

    if(!infile.is_open() || !outfile.is_open()){
        cout << "Error opening files!" << endl;
        return 1;
    }

    string line;
    if(!getline(infile, line)) return 0;

    
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }

    stringstream ss(line);
    unsigned int buckets;
    ss >> buckets;

    SymbolTable st(buckets);
    outfile << "\tScopeTable# 1 created\n";

    unsigned int cmd_counter = 1;

    while(getline(infile, line)){
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        stringstream token_stream(line);
        string temp_tok;
        int token_count = 0;
        while(token_stream >> temp_tok){
            token_count++;
        }

        if(token_count == 0) continue;
 
        token_stream.clear();
        token_stream.seekg(0);
        string* tokens = new string[token_count];
        for(int i = 0; i < token_count; i++){
            token_stream >> tokens[i];
        }

        string opcode = tokens[0];

        if(opcode != "I" && opcode != "L" && opcode != "D" && opcode != "P" && opcode != "S" && opcode != "E" && opcode != "Q"){
            delete[] tokens;
            continue; 
        }

        if(opcode == "P" && token_count >= 2 && tokens[1] != "A" && tokens[1] != "C"){
            delete[] tokens;
            continue;
        }

        if(opcode == "E" && st.getcurrScope()->getParentScope() == nullptr){
            delete[] tokens;
            continue;
        }

        outfile << "Cmd " << cmd_counter++ << ": " << tokens[0];
        for(int i = 1; i < token_count; i++){
            outfile << " " << tokens[i];
        }
        outfile << "\n";

        if(opcode == "Q"){
            while(st.exitScope(outfile)); 
            outfile << "\tScopeTable# 1 removed\n";
            delete[] tokens;
            break;
        }

        if(opcode == "I" && token_count < 3){
            outfile << "\tNumber of parameters mismatch for the command I\n";
            delete[] tokens;
            continue;
        }
        if(opcode == "L" && token_count != 2){
            outfile << "\tNumber of parameters mismatch for the command L\n";
            delete[] tokens;
            continue;
        }
        if(opcode == "D" && token_count != 2){
            outfile << "\tNumber of parameters mismatch for the command D\n";
            delete[] tokens;
            continue;
        }
        if(opcode == "P" && token_count != 2){
            outfile << "\tNumber of parameters mismatch for the command P\n";
            delete[] tokens;
            continue;
        }

        if(opcode == "I"){
            string name = tokens[1];
            string type = tokens[2];
            
            if(type == "FUNCTION"){
                string return_type = tokens[3];
                string param_str = "FUNCTION," + return_type + "<==(";
                for (int i = 4; i < token_count; i++) {
                    param_str += tokens[i];
                    if (i < token_count - 1) param_str += ",";
                }
                param_str += ")";
                type = param_str;
            } 
            else if(type == "STRUCT" || type == "UNION"){
                string comp_str = type + ",{";
                for (int i = 3; i < token_count; i += 2) {
                    comp_str += "(" + tokens[i] + "," + tokens[i+1] + ")";
                    if (i + 2 < token_count) comp_str += ",";
                }
                comp_str += "}";
                type = comp_str;
            }

            SymbolInfo* sym = new SymbolInfo(name, type);
            unsigned int b_idx, s_idx;
            if(st.insert(sym, b_idx, s_idx)){
                outfile << "\tInserted in ScopeTable# " << st.getcurrScope()->getId() 
                        << " at position " << (b_idx + 1) << ", " << s_idx << "\n";
            }
            else{
                outfile << "\t'" << name << "' already exists in the current ScopeTable\n";
                delete sym;
            }
        } 
        else if(opcode == "L"){
            string name = tokens[1];
            unsigned int scope_id, b_idx, s_idx;
            SymbolInfo* found = st.lookUp(name, scope_id, b_idx, s_idx);
            if(found != nullptr){
                outfile << "\t'" << name << "' found in ScopeTable# " << scope_id 
                        << " at position " << (b_idx + 1) << ", " << s_idx << "\n";
            }
            else{
                outfile << "\t'" << name << "' not found in any of the ScopeTables\n";
            }
        } 
        else if(opcode == "D"){
            string name = tokens[1];
            unsigned int b_idx, s_idx;
            if(st.remove(name, b_idx, s_idx)){
                outfile << "\tDeleted '" << name << "' from ScopeTable# " 
                        << st.getcurrScope()->getId() << " at position " << (b_idx + 1) << ", " << s_idx << "\n";
            }
            else{
                outfile << "\tNot found in the current ScopeTable\n";
            }
        } 
        else if(opcode == "P"){
            if(tokens[1] == "C") st.printCurrent(outfile);
            else if(tokens[1] == "A") st.printAll(outfile);
        } 
        else if(opcode == "S"){
            st.enterScope(outfile);
        } 
        else if(opcode == "E"){
            st.exitScope(outfile);
        }

        delete[] tokens;
    }

    infile.close();
    outfile.close();
    return 0;
}