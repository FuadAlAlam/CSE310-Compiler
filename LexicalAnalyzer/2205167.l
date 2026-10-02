%{
#include<iostream>
#include<fstream>
#include<string>
#include"2205167_SymbolTable.h"

using namespace std;

int line_count = 1;
int error_count = 0;

ofstream token_file;
ofstream log_file;

SymbolTable* symbol_table;

string string_desc_buf;
string string_tok_buf;
int string_start_line;

string comment_buf;
int comment_start_line;

string char_buf;
%}

%option noyywrap

%x STRING_STATE
%x SINGLE_COMMENT_STATE
%x MULTI_COMMENT_STATE

%%

"if"       { log_file<<"Line no "<<line_count<<": Token <IF> Lexeme "<<yytext<<" found\n\n"; token_file<<"<IF> "; }
"else"     { log_file<<"Line no "<<line_count<<": Token <ELSE> Lexeme "<<yytext<<" found\n\n"; token_file<<"<ELSE> "; }
"for"      { log_file<<"Line no "<<line_count<<": Token <FOR> Lexeme "<<yytext<<" found\n\n"; token_file<<"<FOR> "; }
"while"    { log_file<<"Line no "<<line_count<<": Token <WHILE> Lexeme "<<yytext<<" found\n\n"; token_file<<"<WHILE> "; }
"do"       { log_file<<"Line no "<<line_count<<": Token <DO> Lexeme "<<yytext<<" found\n\n"; token_file<<"<DO> "; }
"break"    { log_file<<"Line no "<<line_count<<": Token <BREAK> Lexeme "<<yytext<<" found\n\n"; token_file<<"<BREAK> "; }
"int"      { log_file<<"Line no "<<line_count<<": Token <INT> Lexeme "<<yytext<<" found\n\n"; token_file<<"<INT> "; }
"char"     { log_file<<"Line no "<<line_count<<": Token <CHAR> Lexeme "<<yytext<<" found\n\n"; token_file<<"<CHAR> "; }
"float"    { log_file<<"Line no "<<line_count<<": Token <FLOAT> Lexeme "<<yytext<<" found\n\n"; token_file<<"<FLOAT> "; }
"double"   { log_file<<"Line no "<<line_count<<": Token <DOUBLE> Lexeme "<<yytext<<" found\n\n"; token_file<<"<DOUBLE> "; }
"void"     { log_file<<"Line no "<<line_count<<": Token <VOID> Lexeme "<<yytext<<" found\n\n"; token_file<<"<VOID> "; }
"return"   { log_file<<"Line no "<<line_count<<": Token <RETURN> Lexeme "<<yytext<<" found\n\n"; token_file<<"<RETURN> "; }
"switch"   { log_file<<"Line no "<<line_count<<": Token <SWITCH> Lexeme "<<yytext<<" found\n\n"; token_file<<"<SWITCH> "; }
"case"     { log_file<<"Line no "<<line_count<<": Token <CASE> Lexeme "<<yytext<<" found\n\n"; token_file<<"<CASE> "; }
"default"  { log_file<<"Line no "<<line_count<<": Token <DEFAULT> Lexeme "<<yytext<<" found\n\n"; token_file<<"<DEFAULT> "; }
"continue" { log_file<<"Line no "<<line_count<<": Token <CONTINUE> Lexeme "<<yytext<<" found\n\n"; token_file<<"<CONTINUE> "; }
"goto"     { log_file<<"Line no "<<line_count<<": Token <GOTO> Lexeme "<<yytext<<" found\n\n"; token_file<<"<GOTO> "; }
"long"     { log_file<<"Line no "<<line_count<<": Token <LONG> Lexeme "<<yytext<<" found\n\n"; token_file<<"<LONG> "; }
"short"    { log_file<<"Line no "<<line_count<<": Token <SHORT> Lexeme "<<yytext<<" found\n\n"; token_file<<"<SHORT> "; }
"static"   { log_file<<"Line no "<<line_count<<": Token <STATIC> Lexeme "<<yytext<<" found\n\n"; token_file<<"<STATIC> "; }
"unsigned" { log_file<<"Line no "<<line_count<<": Token <UNSIGNED> Lexeme "<<yytext<<" found\n\n"; token_file<<"<UNSIGNED> "; }

"++"|"--"  { log_file<<"Line no "<<line_count<<": Token <INCOP> Lexeme "<<yytext<<" found\n\n"; token_file<<"<INCOP, "<<yytext<<"> "; }
"<="|">="|"=="|"!="{ log_file<<"Line no "<<line_count<<": Token <RELOP> Lexeme "<<yytext<<" found\n\n"; token_file<<"<RELOP, "<<yytext<<"> "; }
"&&"|"||"  { log_file<<"Line no "<<line_count<<": Token <LOGICOP> Lexeme "<<yytext<<" found\n\n"; token_file<<"<LOGICOP, "<<yytext<<"> "; }
"+"|"-"    { log_file<<"Line no "<<line_count<<": Token <ADDOP> Lexeme "<<yytext<<" found\n\n"; token_file<<"<ADDOP, "<<yytext<<"> "; }
"*"|"/"|"%"{ log_file<<"Line no "<<line_count<<": Token <MULOP> Lexeme "<<yytext<<" found\n\n"; token_file<<"<MULOP, "<<yytext<<"> "; }
"<"|">"    { log_file<<"Line no "<<line_count<<": Token <RELOP> Lexeme "<<yytext<<" found\n\n"; token_file<<"<RELOP, "<<yytext<<"> "; }
"="        { log_file<<"Line no "<<line_count<<": Token <ASSIGNOP> Lexeme "<<yytext<<" found\n\n"; token_file<<"<ASSIGNOP, "<<yytext<<"> "; }
"!"        { log_file<<"Line no "<<line_count<<": Token <NOT> Lexeme "<<yytext<<" found\n\n"; token_file<<"<NOT, "<<yytext<<"> "; }
"("        { log_file<<"Line no "<<line_count<<": Token <LPAREN> Lexeme "<<yytext<<" found\n\n"; token_file<<"<LPAREN, "<<yytext<<"> "; }
")"        { log_file<<"Line no "<<line_count<<": Token <RPAREN> Lexeme "<<yytext<<" found\n\n"; token_file<<"<RPAREN, "<<yytext<<"> "; }
"{"        { 
                log_file<<"Line no "<<line_count<<": Token <LCURL> Lexeme "<<yytext<<" found\n\n"; 
                token_file<<"<LCURL, "<<yytext<<"> "; 
                symbol_table->enterScope(); 
            }
"}"        { 
                log_file<<"Line no "<<line_count<<": Token <RCURL> Lexeme "<<yytext<<" found\n\n"; 
                token_file<<"<RCURL, "<<yytext<<"> "; 
                symbol_table->exitScope(); 
            }
"["        { log_file<<"Line no "<<line_count<<": Token <LTHIRD> Lexeme "<<yytext<<" found\n\n"; token_file<<"<LTHIRD, "<<yytext<<"> "; }
"]"        { log_file<<"Line no "<<line_count<<": Token <RTHIRD> Lexeme "<<yytext<<" found\n\n"; token_file<<"<RTHIRD, "<<yytext<<"> "; }
","        { log_file<<"Line no "<<line_count<<": Token <COMMA> Lexeme "<<yytext<<" found\n\n"; token_file<<"<COMMA, "<<yytext<<"> "; }
";"        { log_file<<"Line no "<<line_count<<": Token <SEMICOLON> Lexeme "<<yytext<<" found\n\n"; token_file<<"<SEMICOLON, "<<yytext<<"> "; }

[0-9]*(\.[0-9]*){2,}{
    log_file<<"Error at line no "<<line_count<<": Too many decimal points "<<yytext<<"\n\n\n";
    error_count++;
}

[0-9]+(\.[0-9]+)?[eE][+-]?[0-9]+\.[0-9.]*{
    log_file<<"Error at line no "<<line_count<<": Ill formed number "<<yytext<<"\n\n\n";
    error_count++;
}

[0-9]+[a-zA-Z_][a-zA-Z0-9_]*{
    log_file<<"Error at line no "<<line_count<<": Invalid prefix on ID or invalid suffix on Number "<<yytext<<"\n\n";
    error_count++;
}

[0-9]*\.[0-9]+([eE][+-]?[0-9]+)?|[0-9]+\.[eE][+-]?[0-9]+|[0-9]+\.|[0-9]+[eE][+-]?[0-9]+{
    log_file<<"Line no "<<line_count<<": Token <CONST_FLOAT> Lexeme "<<yytext<<" found\n\n";
    token_file<<"<CONST_FLOAT, "<<yytext<<"> ";
    
    unsigned int b_idx, s_idx;
    SymbolInfo* sym = new SymbolInfo(yytext, "CONST_FLOAT");
    if(symbol_table->insert(sym, b_idx, s_idx)){
        symbol_table->printAll(log_file);
        log_file<<"\n";
    } else{
        log_file<<"< "<<yytext<<" : CONST_FLOAT > already exists in ScopeTable# "<<symbol_table->getcurrScope()->getId()<<" at position "<<b_idx<<", "<<s_idx<<"\n\n";
        delete sym;
    }
}

[0-9]+{
    log_file<<"Line no "<<line_count<<": Token <CONST_INT> Lexeme "<<yytext<<" found\n\n";
    token_file<<"<CONST_INT, "<<yytext<<"> ";
    
    unsigned int b_idx, s_idx;
    SymbolInfo* sym = new SymbolInfo(yytext, "CONST_INT");
    if(symbol_table->insert(sym, b_idx, s_idx)){
        symbol_table->printAll(log_file);
        log_file<<"\n";
    } else{
        log_file<<"< "<<yytext<<" : CONST_INT > already exists in ScopeTable# "<<symbol_table->getcurrScope()->getId()<<" at position "<<b_idx<<", "<<s_idx<<"\n\n";
        delete sym;
    }
}

"''"{
    log_file<<"Error at line no "<<line_count<<": Empty character constant error "<<yytext<<"\n\n\n";
    error_count++;
}

'[^'\\\n]'{
    log_file<<"Line no "<<line_count<<": Token <CONST_CHAR> Lexeme "<<yytext<<" found --> <CONST_CHAR, "<<yytext[1]<<">\n\n";
    token_file<<"<CONST_CHAR, "<<yytext[1]<<"> ";
    
    unsigned int b_idx, s_idx;
    SymbolInfo* sym = new SymbolInfo(yytext, "CONST_CHAR");
    if(symbol_table->insert(sym, b_idx, s_idx)){
        symbol_table->printAll(log_file);
        log_file<<"\n";
    } else{
        log_file<<"< "<<yytext<<" : CONST_CHAR > already exists in ScopeTable# "<<symbol_table->getcurrScope()->getId()<<" at position "<<b_idx<<", "<<s_idx<<"\n\n";
        delete sym;
    }
}

'\\[ntafrbv0\\']'{
    char c = yytext[2];
    if(c == 'n') c = '\n';
    else if(c == 't') c = '\t';
    else if(c == 'a') c = '\a';
    else if(c == 'f') c = '\f';
    else if(c == 'r') c = '\r';
    else if(c == 'b') c = '\b';
    else if(c == 'v') c = '\v';
    else if(c == '0') c = '\0';
    else if(c == '\\') c = '\\';
    else if(c == '\'') c = '\'';

    log_file<<"Line no "<<line_count<<": Token <CONST_CHAR> Lexeme "<<yytext<<" found --> <CONST_CHAR, "<<c<<">\n\n";
    token_file<<"<CONST_CHAR, "<<c<<"> ";
    
    unsigned int b_idx, s_idx;
    SymbolInfo* sym = new SymbolInfo(yytext, "CONST_CHAR");
    if(symbol_table->insert(sym, b_idx, s_idx)){
        symbol_table->printAll(log_file);
        log_file<<"\n";
    } else{
        log_file<<"< "<<yytext<<" : CONST_CHAR > already exists in ScopeTable# "<<symbol_table->getcurrScope()->getId()<<" at position "<<b_idx<<", "<<s_idx<<"\n\n";
        delete sym;
    }
}

'\\[^\n]{
    log_file<<"Error at line no "<<line_count<<": Unterminated character "<<yytext<<"\n\n\n";
    error_count++;
}

'[^'\n]+'{
    log_file<<"Error at line no "<<line_count<<": Multi character constant error "<<yytext<<"\n\n\n";
    error_count++;
}

'[^'\\\n]?{
    log_file<<"Error at line no "<<line_count<<": Unterminated character "<<yytext<<"\n\n\n";
    error_count++;
}

"\""{
    BEGIN(STRING_STATE);
    string_desc_buf = "\"";
    string_tok_buf = "";
    string_start_line = line_count;
}

<STRING_STATE>"\""{
    string_desc_buf += "\"";
    BEGIN(INITIAL);
    log_file<<"Line no "<<line_count<<": Token <STRING> Lexeme "<<string_desc_buf<<" found --> <STRING, "<<string_tok_buf<<">\n\n";
    token_file<<"<STRING, "<<string_tok_buf<<"> ";
}

<STRING_STATE>\\\n{
    string_desc_buf += "\\\n";
    line_count++;
}

<STRING_STATE>\\n { string_desc_buf += "\\n";  string_tok_buf += "\n"; }
<STRING_STATE>\\t { string_desc_buf += "\\t";  string_tok_buf += "\t"; }
<STRING_STATE>\\a { string_desc_buf += "\\a";  string_tok_buf += "\a"; }
<STRING_STATE>\\f { string_desc_buf += "\\f";  string_tok_buf += "\f"; }
<STRING_STATE>\\r { string_desc_buf += "\\r";  string_tok_buf += "\r"; }
<STRING_STATE>\\b { string_desc_buf += "\\b";  string_tok_buf += "\b"; }
<STRING_STATE>\\v { string_desc_buf += "\\v";  string_tok_buf += "\v"; }
<STRING_STATE>\\0 { string_desc_buf += "\\0";  string_tok_buf += "\0"; }
<STRING_STATE>\\\\{ string_desc_buf += "\\\\"; string_tok_buf += "\\"; }
<STRING_STATE>\\\"{ string_desc_buf += "\\\""; string_tok_buf += "\""; }
<STRING_STATE>\\\'{ string_desc_buf += "\\'";  string_tok_buf += "'"; }

<STRING_STATE>\n{
    log_file<<"Error at line no "<<string_start_line<<": Unterminated string "<<string_desc_buf<<"\n\n";
    error_count++;
    line_count++;
    BEGIN(INITIAL);
}

<STRING_STATE><<EOF>>{
    log_file<<"Error at line no "<<string_start_line<<": Unterminated string "<<string_desc_buf<<"\n\n";
    error_count++;
    symbol_table->printAll(log_file);
    log_file<<"\n";
    log_file<<"Total lines: "<<line_count<<"\n";
    log_file<<"Total errors: "<<error_count<<"\n";
    return 0;
}

<STRING_STATE>[^"\\\n]+{
    string_desc_buf += yytext;
    string_tok_buf += yytext;
}

<STRING_STATE>\\.{
    string_desc_buf += yytext;
    if(yytext[1]) string_tok_buf += yytext[1];
}

"//"{
    BEGIN(SINGLE_COMMENT_STATE);
    comment_buf = "//";
    comment_start_line = line_count;
}

<SINGLE_COMMENT_STATE>\\\n{
    comment_buf += "\\\n";
    line_count++;
}

<SINGLE_COMMENT_STATE>\n{
    BEGIN(INITIAL);
    log_file<<"Line no "<<line_count<<": Token <COMMENT> Lexeme "<<comment_buf<<" found\n\n";
    line_count++;
}

<SINGLE_COMMENT_STATE><<EOF>>{
    log_file<<"Line no "<<line_count<<": Token <COMMENT> Lexeme "<<comment_buf<<" found\n\n";
    symbol_table->printAll(log_file);
    log_file<<"\n";
    log_file<<"Total lines: "<<line_count<<"\n";
    log_file<<"Total errors: "<<error_count<<"\n";
    return 0;
}

<SINGLE_COMMENT_STATE>[^\\\n]+{ comment_buf += yytext; }
<SINGLE_COMMENT_STATE>\\[^\n]  { comment_buf += yytext; }

"/*"{
    BEGIN(MULTI_COMMENT_STATE);
    comment_buf = "/*";
    comment_start_line = line_count;
}

<MULTI_COMMENT_STATE>"*/"{
    comment_buf += "*/";
    BEGIN(INITIAL);
    log_file<<"Line no "<<line_count<<": Token <COMMENT> Lexeme "<<comment_buf<<" found\n\n";
}

<MULTI_COMMENT_STATE>\n{
    comment_buf += "\n";
    line_count++;
}

<MULTI_COMMENT_STATE><<EOF>>{
    log_file<<"Error at line no "<<comment_start_line<<": Unterminated comment "<<comment_buf<<"\n\n";
    error_count++;
    symbol_table->printAll(log_file);
    log_file<<"\n";
    log_file<<"Total lines: "<<line_count<<"\n";
    log_file<<"Total errors: "<<error_count<<"\n";
    return 0;
}

<MULTI_COMMENT_STATE>.{
    comment_buf += yytext;
}

[a-zA-Z_][a-zA-Z0-9_]*{
    log_file<<"Line no "<<line_count<<": Token <ID> Lexeme "<<yytext<<" found\n\n";
    token_file<<"<ID, "<<yytext<<"> ";
    
    unsigned int b_idx, s_idx;
    SymbolInfo* sym = new SymbolInfo(yytext, "ID");
    if(symbol_table->insert(sym, b_idx, s_idx)){
        symbol_table->printAll(log_file);
        log_file<<"\n";
    } else{
        log_file<<"< "<<yytext<<" : ID > already exists in ScopeTable# "<<symbol_table->getcurrScope()->getId()<<" at position "<<b_idx<<", "<<s_idx<<"\n\n";
        delete sym;
    }
}

[ \t\v\r]+{ }
"\n"      { line_count++; }

.{
    log_file<<"Error at line no "<<line_count<<": Unrecognized character "<<yytext<<"\n\n";
    error_count++;
}

<<EOF>>{
    symbol_table->printAll(log_file);
    log_file<<"\n";
    log_file<<"Total lines: "<<line_count<<"\n";
    log_file<<"Total errors: "<<error_count<<"\n";
    return 0;
}

%%

int main(int argc, char* argv[]){
    if(argc < 2){
        cout<<"Please provide input file name via command line."<<endl;
        return 1;
    }

    FILE* fin = fopen(argv[1], "r");
    if(!fin){
        cout<<"Cannot open input file."<<endl;
        return 1;
    }
    
    token_file.open("2205167_token.txt");
    log_file.open("2205167_log.txt");

    symbol_table = new SymbolTable(7);

    yyin = fin;
    yylex();

    fclose(fin);
    token_file.close();
    log_file.close();

    delete symbol_table;
    return 0;
}