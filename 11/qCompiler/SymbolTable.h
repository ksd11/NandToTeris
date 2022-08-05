#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include<string>
#include<unordered_map>
#include<iostream>
using namespace std;

typedef struct {
  string type;
  string kind;
  int index;
}Variable;

class SymbolTable{
public:
  SymbolTable();
  ~SymbolTable();
  void startSubroutine();
  void define(string name,string type,string kindstring);
  int varCount(string kind);
  string kindOf(string name);
  string typeOf(string name);
  int indexOf(string name);
  void printGlobalTable();
  void printLocalTable();
  void printAll();
  bool isExist(string name);
private:
  unordered_map<string,Variable>* global_scope;
  unordered_map<string,Variable>* local_scope;
  Variable& findVar(string name);
};

#endif
