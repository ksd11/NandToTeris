#include<iostream>
#include<fstream>
#include"CodeWriter.h"
using namespace std;

/* ../StackArithmetic/SimpleAdd/SimpleAdd.vm */
string input_file;
string output_file;

static string vm2asm(string str){
  size_t idx = str.rfind(".vm");
  if(idx == string::npos){
    return str+".asm";
  }
  str.erase(idx,3);
  return str+".asm";
}


int main(int argc, char const *argv[])
{

  if(argc != 2){
    cout<<"usage: prog vm/path\n";
    exit(1);
  }

  input_file = argv[1]; // TODO. need to handle directory
  output_file = vm2asm(input_file);
  ofstream out(output_file);


  Parser p(input_file);
  CodeWriter cw(output_file);

  while(p.hasMoreCommands()){
    p.advance();
    switch(p.commandType()){
      case C_ARITHMETIC:
        // out<<"arithemetic,"<<p.arg1()<<endl;
        cw.writeArithmetic(p.arg1());
        break;
      case C_PUSH:
        // out<<"push,"<<p.arg1()<<","<<p.arg2()<<endl;
        cw.writePushPop(C_PUSH, p.arg1(),p.arg2());
        break;
      case C_POP:
        // out<<"pop,"<<p.arg1()<<","<<p.arg2()<<endl;
        cw.writePushPop(C_POP, p.arg1(),p.arg2());
        break;
      case C_LABEL:
        out<<"label,"<<p.arg1()<<endl;
        break;
      case C_GOTO:
        out<<"goto,"<<p.arg1()<<endl;
      break;
      case C_IF:
        out<<"if,"<<p.arg1()<<endl;
        break;
      case C_FUNCTION:
        out<<"function,"<<p.arg1()<<","<<p.arg2()<<endl;
        break;
      case C_CALL:
        out<<"call,"<<p.arg1()<<","<<p.arg2()<<endl;
        break;
      case C_RETURN:
        out<<"return"<<endl;
        break;
    }
  }

  cw.writeDeathLoop();
  return 0;
}