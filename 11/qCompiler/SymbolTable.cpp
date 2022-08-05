#include"SymbolTable.h"
#include<set>
#include<cassert>
using namespace std;

static set<string> kinds = {"static","field","argument","var"}; 

SymbolTable::SymbolTable(){
  global_scope = new unordered_map<string,Variable>();
  local_scope = new unordered_map<string,Variable>();
}

SymbolTable::~SymbolTable(){
  delete global_scope;
  delete local_scope;
}

void SymbolTable::startSubroutine(){
  local_scope->clear();
}

void SymbolTable::define(string name,string type,string kind){
  assert(kinds.count(kind));
  int idx = varCount(kind); //编号
  unordered_map<string,Variable>* scope = local_scope;
  if(kind == "static" || kind == "field"){
    scope = global_scope;
  }

  if(scope->count(name)){
    cout<<name<< " is defined.\n"<<endl;
    exit(1);
  }
  scope->insert({name,{type,kind,idx}});
}

int SymbolTable::varCount(string kind){
  assert(kinds.count(kind));
  unordered_map<string,Variable>* scope = local_scope;
  if(kind == "static" || kind == "field"){
    scope = global_scope;
  }

  int res = 0;
  for(const auto &w : *scope){
    if(w.second.kind == kind) res++;
  }
  return res;
}

Variable& SymbolTable::findVar(string name){
  if(local_scope->count(name)){
    return (*local_scope)[name];
  }else if(global_scope->count(name)){
    return (*global_scope)[name];
  }else{
    cout<<"variable: "<<name<<" is undefined.\n";
    exit(1);
  }
}

string SymbolTable::kindOf(string name){
  return findVar(name).kind;
}
 
string SymbolTable::typeOf(string name){
  return findVar(name).type;
}

int SymbolTable::indexOf(string name){
  return findVar(name).index;
}

bool SymbolTable::isExist(string name){
  if(local_scope->count(name) || global_scope->count(name)){
    return true;
  }
  return false;
}

void SymbolTable::printGlobalTable(){
  cout<<"\n\nGlobal: "<<endl;
  cout<<"name\ttype\tkind\t#\n";
  for(const auto &w : *global_scope){
    cout<<w.first<<"\t"<<w.second.type<<"\t"<<w.second.kind<<"\t"<<w.second.index<<endl;
  }
}

void SymbolTable::printLocalTable(){
  cout<<"\n\nLocal: "<<endl;
  cout<<"name\ttype\tkind\t#\n";
  for(const auto &w : *local_scope){
    cout<<w.first<<"\t"<<w.second.type<<"\t"<<w.second.kind<<"\t"<<w.second.index<<endl;
  }
}

void SymbolTable::printAll(){
  printGlobalTable();
  printLocalTable();
}
