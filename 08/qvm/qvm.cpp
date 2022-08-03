#include<iostream>
#include<fstream>
#include"CodeWriter.h"
#include"ParsePath.h"
using namespace std;

/* ../StackArithmetic/SimpleAdd/SimpleAdd.vm */
static string input_file;
static string output_file;
string input_file_no_suffix;
string functionName = "null";
int constant_times = 1;  //push constant的次数

//设置str没有后缀为input_file_no_suffix
static void erase_file_suffix(string str){
  int idx = str.rfind(".vm");
  if(idx != string::npos){
    str.erase(idx,3);  //去掉后缀
  }
  idx=-1;
  int len = str.length();
  for(int i=len-1;i>=0;i--){
    if(str[i] == '\\' || str[i] == '/'){
      idx = i;
      break;
    }
  }
  if(idx>=0)
    str = str.substr(idx+1);
  input_file_no_suffix = str;
}

int main(int argc, char const *argv[])
{

  if(argc != 2){
    cout<<"usage: prog vm/path\n";
    exit(1);
  }

  path_init(argv[1],output_file);
  CodeWriter cw(output_file);

  cw.writeString("// ************ build by qvm:\n");
  cw.writeInit();

  while(hasNextFile()){
    input_file = nextFile();
    erase_file_suffix(input_file);
    Parser p(input_file);
    cw.writeString("\n\n// "+input_file+"\n");

    string last_constant = "N";
    while(p.hasMoreCommands()){
      p.advance();
      if(p.commandType() == C_PUSH && p.arg1()=="constant"){
        if(last_constant=="N"){
          last_constant=p.arg2();
        }else if(p.arg2() == last_constant){
          constant_times ++;
        }else{
          cw.writePushPop(C_PUSH, "constant", last_constant);  //push一个新的constant
          constant_times=1;
          last_constant = p.arg2();
        }
        continue;
      }
      if(last_constant!="N"){
        //push last_constant  constant_times 次
        cw.writePushPop(C_PUSH, "constant", last_constant);
        constant_times=1;
        last_constant="N";
      }

      switch(p.commandType()){
        case C_ARITHMETIC:
          cw.writeArithmetic(p.arg1());
          break;
        case C_PUSH:
          cw.writePushPop(C_PUSH, p.arg1(),p.arg2());
          break;
        case C_POP:
          cw.writePushPop(C_POP, p.arg1(),p.arg2());
          break;
        case C_LABEL:
          cw.writeLabel(p.arg1());
          break;
        case C_GOTO:
          cw.writeGoto(p.arg1());
        break;
        case C_IF:
          cw.writeIf(p.arg1());
          break;
        case C_FUNCTION:
          functionName = p.arg1();
          cw.writeFunction(p.arg1(),p.arg2());
          break;
        case C_CALL:
          cw.writeCall(p.arg1(), p.arg2());
          break;
        case C_RETURN:
          cw.writeReturn();
          break;
      }
    }
  }

  // cw.writeDeathLoop();
  return 0;
}