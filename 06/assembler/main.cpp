#include"Parser.h"
#include"code.h"
#include"SymbolTable.h"
#include<bitset>
#include<iostream>
#include<fstream>
#include<map>
#include<cctype>
using namespace std;

// static string input_file = "../pong/PongL.asm";
static string input_file = "../add/Add.asm";
static string output_file = "out.hack";

// 全局符号表
SymbolTable symbolTable;

void buildSymbolTable(){
  //1.初始化预定义符号
  map<string,uint16_t> ini = {
    {"SP", 0},{"LCL", 1},{"ARG", 2},{"THIS", 3},{"THAT",4},
    {"R0", 0},{"R1", 1},{"R2", 2},{"R3", 3},{"R4", 4},
    {"R5", 5},{"R6", 6},{"R7", 7},{"R8", 8},{"R9", 9},
    {"R10", 10},{"R11", 11},{"R12", 12},{"R13", 13},{"R14", 14},{"R15",15},{"SCREEN",16384},{"KBD",24576}
  };

  map<string,uint16_t>::iterator it;
  for(it=ini.begin();it!=ini.end();it++){
    symbolTable.addEntry(it->first, it->second);
  }

  //2.扫描符号
  uint16_t line = 0;
  Parser p(input_file);
  while(p.hasMoreCommands()){
    p.advance();
    switch(p.commandType()){
      case A_COMMAND:
        if(!isdigit(p.symbol()[0])){
          if(!symbolTable.contains(p.symbol())){
            symbolTable.addEntry(p.symbol(),-1); // -1标记未定义
          }
        }
      case C_COMMAND:
        line++;
        break;
      case L_COMMAND:
        if(!isdigit(p.symbol()[0])){
          if(!symbolTable.contains(p.symbol())){
            symbolTable.addEntry(p.symbol(),line);
          }else{
            symbolTable.setAddress(p.symbol(), line);
          }
        }
        break;
    }
  }
}

int main(int argc,char *argv[]){
  if(argc != 2){
    cout<<"usage: prog xx.asm\n";
    exit(1);
  }

  input_file = argv[1];
  output_file = "out.hack";

  Parser p(input_file);
  ofstream out(output_file);

  Code code;
  uint16_t res = 0;
  uint16_t dst=0,comp=0,jump=0; 
  uint16_t id = 0B1110'0000'0000'0000;
  uint16_t counter = 0x10;

  buildSymbolTable();

  while(p.hasMoreCommands()){
    p.advance();
    switch(p.commandType()){
      case A_COMMAND:
        if(!isdigit(p.symbol()[0])){ // Symbol
          if(symbolTable.contains(p.symbol())){
            res = symbolTable.getAddress(p.symbol());
            if(res == (uint16_t)-1){
              symbolTable.setAddress(p.symbol(),counter);
              res = counter++;
            } 
          }else{
            cout<<"undefined symbol: "<<p.symbol()<<endl;
            exit(1);
          }
        }else{ // number
          sscanf(p.symbol().c_str(), "%hu", &res);
        }
        out<<bitset<16>(res)<<endl;
        break;
      case C_COMMAND:
        dst = code.dest(p.dest());
        comp = code.comp(p.comp());
        jump = code.jump(p.jump());
        out<<bitset<16>(id|dst|comp|jump)<<endl;
        break;
    }
  }
  return 0;
}