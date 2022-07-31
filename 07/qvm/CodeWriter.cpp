#include<fstream>
#include<string>
#include<iostream>
#include"Parser.h"
#include"CodeWriter.h"
#include<map>
using namespace std;

static map<string, string> COMMAND = {
  {
    "add", "@SP\n"
          "AM=M-1\n"       //指向栈顶前一个元素
          "D=M\n"       // d = y
          "A=A-1\n"
          "M=M+D\n"     //x = x+y
  },
  {
    "sub", "@SP\n"
          "AM=M-1\n"
          "D=M\n"      
          "A=A-1\n"
          "M=M-D\n"
  },
  {
    "and", "@SP\n"
          "AM=M-1\n"   
          "D=M\n"      
          "A=A-1\n"
          "M=D&M\n"
  },
  {
    "or", "@SP\n"
          "AM=M-1\n"
          "D=M\n"      
          "A=A-1\n"
          "M=D|M\n" 
  },
  {
    "neg", "@SP\n"
           "A=M-1\n"
           "M=-M\n"
  },
  {
    "not", "@SP\n"
           "A=M-1\n"
           "M=!M\n"
  },
  {
    "check_front", "@SP\n"
          "AM=M-1\n"     
          "D=M\n"       // d = y
          "A=A-1\n"
          "D=M-D\n"    //d = x-y
          "M=-1\n"
  }
};

CodeWriter::CodeWriter(string filename){
  out.open(filename); //open file
  if(!out){
    cout<<filename<<" not exist!\n";
    exit(1);
  }
}

CodeWriter::~CodeWriter(){
  out.close();
}

// static map<string, string> seg2addr = {
//   {"static", "16"},
//   {"constant", ""}
// };

static string generalSymbol(){
  static int cnt = 0;
  return "jump.qvm.build."+to_string(cnt++);
}



void CodeWriter::writeArithmetic(string command){
  if(COMMAND.count(command)){
      out<<COMMAND[command];
  }else{
    if(command=="eq"){
      string symbol = generalSymbol();
      string s = COMMAND["check_front"] +
                "@"+symbol+"\n"
                "D; JEQ\n"
                "@SP\n" 
                "A=M-1\n" 
                "M=0\n"
                "("+symbol+")\n";
      out<<s;
      
    }else if(command=="gt"){
      string symbol = generalSymbol();
      string s = COMMAND["check_front"] +
                "@"+symbol+"\n"
                "D; JGT\n"
                "@SP\n" 
                "A=M-1\n" 
                "M=0\n"
                "("+symbol+")\n";
      out<<s;

    }else if(command=="lt"){
      string symbol = generalSymbol();
      string s = COMMAND["check_front"] +
                "@"+symbol+"\n"
                "D; JLT\n"
                "@SP\n" 
                "A=M-1\n" 
                "M=0\n"
                "("+symbol+")\n";
      out<<s;

    }else{
      cout<<"unkonw arithmetic command: "<<command<<endl;
      exit(1);
    }
  }
}
 
 
 
void CodeWriter::writePushPop(CType c, string segment,string index){
  switch(c){
    case C_PUSH:
      if(segment=="constant"){
        string s = "@"+index+"\n"
                  "D=A\n"
                  "@SP\n"
                  "A=M\n"
                  "M=D\n"
                  "@SP\n"
                  "M=M+1\n";
        out << s;
      }
      break;
    case C_POP:
      break;
  }
}

void CodeWriter::writeDeathLoop(){
  string symbol = generalSymbol();
  string s = "@"+symbol+"\n("+symbol+")\n0;JMP\n";
  out << s;
}

