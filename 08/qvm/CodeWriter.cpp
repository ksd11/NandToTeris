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
 

//D = (*label+off)
static string m_to_d(string label, string off){
  return "@"+label+"\n"
         "D=M\n"
         "@"+off+"\n"
         "A=D+A\n"
         "D=M\n";
}

//D = (*label)
static string m_to_d(string label){
  return "@"+label+"\n"
         "D=M\n";
}

//(*label+off) = D
static string d_to_m(string label, string off){
  return "@14\n"
         "M=D\n"
         "@"+label+"\n"
         "D=M\n"
         "@"+off+"\n"
         "D=D+A"+"\n"
         "@13\n"
         "M=D\n"
         "@14\n"
         "D=M\n"
         "@13\n"
         "A=M\n"
         "M=D\n";
}

//(off) = D
static string d_to_m(string off){
  return "@14\n"
         "M=D\n"
         "@"+off+"\n"
         "D=A"+"\n"
         "@13\n"
         "M=D\n"
         "@14\n"
         "D=M\n"
         "@13\n"
         "A=M\n"
         "M=D\n";
}

// push d
static string push_d_to_stack(){
  return "@SP\n"
         "A=M\n"
         "M=D\n"
         "@SP\n"
         "M=M+1\n";
}

//push (*label)
static string push_d_to_stack(string label){
  return m_to_d(label)+push_d_to_stack();
}

//push (*label+off)
static string push_d_to_stack(string label, string off){
  return m_to_d(label,off)+push_d_to_stack();
}
 

static string pop_d_from_stack(){
  return "@SP\n"
         "AM=M-1\n"
         "D=M\n";
}

// pop(*label+off)
static string pop_from_stack(string label, string off){
  return  pop_d_from_stack()+d_to_m(label,off);
}

// pop(off)
static string pop_from_stack(string off){
  return pop_d_from_stack()+d_to_m(off);
}

//只借助r13寄存器进行pop
static string pop_from_stack_only_r13(string label, string off){
  return "@"+label+"\n"
         "D=M\n"
         "@"+off+"\n"
         "D=D+A"+"\n"
         "@13\n"
         "M=D\n"
         +pop_d_from_stack()+
         "@13\n"
         "A=M\n"
         "M=D\n";
}


//calc addr  ex. SP+1  、 ARG+1
static string calc_addr_to_d(string label, string n,string op){
  return m_to_d(label) +
         "@"+n+"\n"
         "D=D"+op+"A\n";
}


static string from_d_get_value(){
  return "A=D\n"
         "D=M\n";
}

static string write_d_to_label(string label){
  return "@"+label+"\n"
         "M=D\n";
}

static string write_label_to_d(string label){
  return "@"+label+"\n"
         "D=M\n";
}

//push value
static string push_to_stack(string value){
  return "@"+value+"\n"
         "D=A\n"+push_d_to_stack();
}


void CodeWriter::writePushPop(CType c, string segment,string index){
  uint16_t idx;
  switch(c){
    case C_PUSH:
      if(segment=="constant"){
        out << "@"+index+"\n"
                "D=A\n" + push_d_to_stack();

      }else if(segment=="local"){
        out << push_d_to_stack("LCL", index);

      }else if(segment=="argument"){
        out << push_d_to_stack("ARG", index);

      }else if(segment=="this"){
        out << push_d_to_stack("THIS", index);

      }else if(segment=="that"){
        out << push_d_to_stack("THAT", index);

      }else if(segment == "pointer"){
        index = index=="0"?"3":"4";
        out << "@"+index+"\n"
               "D=M\n" + push_d_to_stack();
      }else if(segment == "temp"){
        sscanf(index.c_str(), "%hu", &idx);
        index = to_string(idx+5);
        out << "@"+index+"\n"
               "D=M\n" + push_d_to_stack();
      }else if(segment == "static"){
        out << "@"+input_file_no_suffix+"."+index+"\n"
               "D=M\n" + push_d_to_stack();
      }else{
        cout<<"error: "<<segment<<endl;
        exit(1);
      }
      break;
    case C_POP:
      if(segment=="local"){
        out << pop_from_stack("LCL", index);

      }else if(segment=="argument"){
        out << pop_from_stack("ARG", index);

      }else if(segment=="this"){
        out << pop_from_stack("THIS", index);

      }else if(segment=="that"){
        out << pop_from_stack("THAT", index);

      }else if(segment == "pointer"){
        index = index=="0"?"3":"4";
        out << pop_from_stack(index);;
      }else if(segment == "temp"){
        sscanf(index.c_str(), "%hu", &idx);
        index = to_string(idx+5);
        out << pop_from_stack(index);;
      }else if(segment == "static"){
        out << pop_from_stack(input_file_no_suffix+"."+index);
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

void CodeWriter::writeCall(string functionName, string numArgs){
  uint16_t n ;
  sscanf(numArgs.c_str(), "%hu", &n);
  string symbol = generalSymbol();
  out << push_to_stack(symbol) +   //push return-address
         push_d_to_stack("LCL") +    //push LCL
         push_d_to_stack("ARG") +    //push ARG
         push_d_to_stack("THIS") +   //push THIS
         push_d_to_stack("THAT") +   //push THAT
         calc_addr_to_d("SP", to_string(n+5), "-") + //D=SP-n-5
         write_d_to_label("ARG") +
         write_label_to_d("SP") +
         write_d_to_label("LCL")+
         "@"+functionName+"\n"
         "0; JMP\n"            //goto functionName
         "("+symbol+")\n";      // (return-address)
}
  
  
void CodeWriter::writeReturn(){
  string frame = "R14";
  string ret = "R15";
  //FRAME: R5  RET: R6
  out<< write_label_to_d("LCL") +
        write_d_to_label(frame) +
        calc_addr_to_d(frame,"5","-") +
        from_d_get_value() +
        write_d_to_label(ret) +

        pop_from_stack_only_r13("ARG","0") +
        calc_addr_to_d("ARG","1","+") +
        write_d_to_label("SP") +              //SP=ARG+1

        calc_addr_to_d(frame, "1","-")+
        from_d_get_value() +
        write_d_to_label("THAT") +

        calc_addr_to_d(frame, "2","-")+
        from_d_get_value() +
        write_d_to_label("THIS") +

        calc_addr_to_d(frame, "3","-")+
        from_d_get_value() +
        write_d_to_label("ARG") +

        calc_addr_to_d(frame, "4","-")+
        from_d_get_value() +
        write_d_to_label("LCL") +

        "@"+ret+"\n"
        "A=M\n"
        "0; JMP\n";
}


void CodeWriter::writeFunction(string functionName, string numLocals){
  string symbol = generalSymbol();
  string symbol2 = generalSymbol();


  out<< "("+functionName+")\n"
        "@"+numLocals+"\n"
        "D=A\n"
        "("+symbol+")\n"
        "@"+symbol2+"\n"
        "D; JEQ\n" 
        "@R13\n"
        "M=D\n" +
        push_to_stack("0") +
        "@R13\n"
        "D=M-1\n"
        "@"+symbol+"\n"
        "0; JMP\n"
        "("+symbol2+")\n";
}

// 设置sp，跳转到Sys.init
void CodeWriter::writeInit(){
  out<< "@256\n"
        "D=A\n"
        "@SP\n"
        "M=D\n";
  writeCall("Sys.init","0");
}

void CodeWriter::writeString(string str){
  out<<str;
}