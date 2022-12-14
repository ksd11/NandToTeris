#ifndef COMPILATION_ENGINE_H
#define COMPILATION_ENGINE_H


#include<string>
#include<fstream>
#include"JackTokenizer.h"
#include<set>
#include"SymbolTable.h"
#include"VMWriter.h"
using namespace std;

class CompilationEngine{
public:
  CompilationEngine(string input_file,string output_file);
  ~CompilationEngine();
  void compileClass();
  int compileClassVarDec();
  void compileSubroutine();
  void compileParameterList();
  int compileVarDec(); 
  void compileStatements();
  void compileDo();
  void compileLet();
  void compileWhile();
  void compileReturn();
  void compileIf();
  void compileExpression();
  void compileTerm();
  int compileExpressionList(); // 返回参数个数

private:
  JackTokenizer* jk;
  ofstream out;
  SymbolTable* st;
  VMWriter* vm;
  void readToken();
  void readToken_symbol(char ch);
  void readToken_identifier();
  void readToken_keyword(set<string> kws);
  void readToken_keyword(string kw);
  bool is_symbol(char ch);
  bool is_keyword(string kw);
  int type();
  void writeXml(TokenType t); 
  void writeXml(string str); 
  void push_this_to_stack();
  void subroutineCall(string id1, string id2);
};


#endif