#include"SymbolTable.h"

static SymbolKind string_to_kind(string s){
  if(s=="static") {
    return STATIC;
  }else if(s=="field"){
    return FIELD;
  }else if(s=="argument"){
    return ARG;
  }else if(s=="var"){
    return VAR;
  }
  cout<<"string_to_kind: never be there!"<<endl;
  exit(1);
}

static string kind_to_string(SymbolKind k){
  switch (k)
  {
  case STATIC:
    return "static";
  case FIELD:
    return "field";
  case ARG:
    return "argument"; 
  case VAR:
    return "var";
  }
  cout<<"kind_to_string: never be there!"<<endl;
  exit(1);
}

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
  scope->insert({name,{type,kind,idx}});
}

void SymbolTable::define(string name,string type,string kindstring){
  SymbolKind kind = string_to_kind(kindstring);
  define(name,type,kind);
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
    exit(1);
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

void SymbolTable::printGlobalTable(){
  cout<<"\n\nGlobal: "<<endl;
  cout<<"name\ttype\tkind\t#\n";
  for(const auto &w : *global_scope){
    cout<<w.first<<"\t"<<w.second.type<<"\t"<<kind_to_string(w.second.kind)<<"\t"<<w.second.index<<endl;
  }
}

void SymbolTable::printLocalTable(){
  cout<<"\n\nLocal: "<<endl;
  cout<<"name\ttype\tkind\t#\n";
  for(const auto &w : *local_scope){
    cout<<w.first<<"\t"<<w.second.type<<"\t"<<kind_to_string(w.second.kind)<<"\t"<<w.second.index<<endl;
  }
}

void SymbolTable::printAll(){
  printGlobalTable();
  printLocalTable();
}
