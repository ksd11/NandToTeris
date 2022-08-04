#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include<string>
#include<unordered_map>
#include<iostream>
using namespace std;

enum SymbolKind{
  STATIC,FIELD,ARG,VAR
};

typedef struct {
  string type;
  SymbolKind kind;
  int index;
}Variable;

class SymbolTable{
public:
  SymbolTable();
  ~SymbolTable();
  void startSubroutine();
  void define(string name,string type,SymbolKind kind);
  int varCount(SymbolKind kind);
  SymbolKind kindOf(string name);
  string typeOf(string name);
  int indexOf(string name);
private:
  unordered_map<string,Variable>* global_scope;
  unordered_map<string,Variable>* local_scope;
  Variable& findVar(string name);
};

#endif
