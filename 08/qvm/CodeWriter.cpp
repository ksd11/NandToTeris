#include<fstream>
#include<string>
#include<iostream>
#include"Parser.h"
#include"CodeWriter.h"
#include<map>
using namespace std;

extern string functionName;  //当前正在处理的函数
extern string input_file_no_suffix; //当前正在处理的文件（不含后缀）

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
 

//p将value push到栈顶
static string push2stack(string val){
  return "@SP\n"
         "A=M\n"
         "M="+val+"\n"
         "@SP\n"
         "M=M+1\n";
}
 
// 将栈顶的值弹出放入 dest+index
static string pop2dest(string index, string dest){
  return "@"+dest+"\n"
         "D=M\n"
         "@"+index+"\n"
         "D=D+A\n"
         "@R13\n"
         "M=D\n"        //将目标的地址暂存R13
         "@SP\n"
         "AM=M-1\n"
         "D=M\n"        //取到弹出的值
         "@R13\n"
         "A=M\n"
         "M=D\n";
}

static string pop2index(string index){
  return "@"+index+"\n"
          "D=A\n"
          "@13\n"
          "M=D\n"
          "@SP\n"
          "AM=M-1\n"
          "D=M\n"        //取到弹出的值
          "@R13\n"
          "A=M\n"
          "M=D\n";
}

//将*(dest+index)的值写入D
static string get2D(string index, string dest){
  return "@"+dest+"\n"
         "D=M\n"
         "@"+index+"\n"
         "A=D+A\n"
         "D=M\n";
}

void CodeWriter::writePushPop(CType c, string segment,string index){
  uint16_t idx;
  switch(c){
    case C_PUSH:
      if(segment=="constant"){
        out << "@"+index+"\n"
                "D=A\n" + push2stack("D");

      }else if(segment=="local"){
        out << get2D(index,"LCL") + push2stack("D");

      }else if(segment=="argument"){
        out << get2D(index,"ARG") + push2stack("D");

      }else if(segment=="this"){
        out << get2D(index,"THIS") + push2stack("D");

      }else if(segment=="that"){
        out << get2D(index,"THAT") + push2stack("D");

      }else if(segment == "pointer"){
        index = index=="0"?"3":"4";
        out << "@"+index+"\n"
               "D=M\n" + push2stack("D");
      }else if(segment == "temp"){
        sscanf(index.c_str(), "%hu", &idx);
        index = to_string(idx+5);
        out << "@"+index+"\n"
               "D=M\n" + push2stack("D");
      }else if(segment == "static"){
        out << "@"+input_file_no_suffix+"."+index+"\n"
               "D=M\n" + push2stack("D");
      }else{
        cout<<"error: "<<segment<<endl;
        exit(1);
      }
      break;
    case C_POP:
      if(segment=="local"){
        out << pop2dest(index, "LCL");

      }else if(segment=="argument"){
        out << pop2dest(index,"ARG");

      }else if(segment=="this"){
        out << pop2dest(index,"THIS");

      }else if(segment=="that"){
        out << pop2dest(index,"THAT");

      }else if(segment == "pointer"){
        index = index=="0"?"3":"4";
        out << pop2index(index);
      }else if(segment == "temp"){
        sscanf(index.c_str(), "%hu", &idx);
        index = to_string(idx+5);
        out << pop2index(index);
      }else if(segment == "static"){
        out << pop2index(input_file_no_suffix+"."+index);
      }else{
        cout<<"error: "<<segment<<endl;
        exit(1);
      }
      break;
  }
}

void CodeWriter::writeDeathLoop(){
  string symbol = generalSymbol();
  string s = "@"+symbol+"\n("+symbol+")\n0;JMP\n";
  out << s;
}


//生成一个  functionName$label的标签
void CodeWriter::writeLabel(string label){
  out<< "("+ functionName + "$" + label +")\n";
}


void CodeWriter::writeGoto(string label){
  out<< "@"+ functionName + "$" + label +"\n"
         "0; JMP\n";
}


void CodeWriter::writeIf(string label){
  out << "@SP\n"
         "AM=M-1\n"
         "D=M\n"
         "@"+functionName + "$"+label+"\n"
         "D; JNE\n";
}