#include"SymbolTable.h"

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

void SymbolTable::define(string name,string type,SymbolKind kind){
  int idx = varCount(kind); //编号
  unordered_map<string,Variable>* scope = local_scope;
  if(kind == STATIC || kind == FIELD){
    scope = global_scope;
  }

  if(scope->count(name)){
    cout<<name<< " is defined.\n"<<endl;
    exit(1);
  }
  scope->insert(name,{type,kind,idx});
}

int SymbolTable::varCount(SymbolKind kind){
  unordered_map<string,Variable>* scope = local_scope;
  if(kind == STATIC || kind == FIELD){
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
  }
}

SymbolKind SymbolTable::kindOf(string name){
  return findVar(name).kind;
}
 
string SymbolTable::typeOf(string name){
  return findVar(name).type;
}

int SymbolTable::indexOf(string name){
  return findVar(name).index;
}
