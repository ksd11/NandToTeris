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
 

//D = *(*label+off)
static string m_to_d(string label, string off){
  return "@"+label+"\n"
         "D=M\n"
         "@"+off+"\n"
         "A=D+A\n"
         "D=M\n";
}

//D = *(*label)
static string m_to_d(string label){
  return "@"+label+"\n"
         "A=M\n"
         "D=M\n";
}

// push d
static string push_d_to_stack(){
  return "@SP\n"
         "A=M\n"
         "M=D\n"
         "@SP\n"
         "M=M+1\n";
}

//push *(*label)
static string push_to_stack(string label){
  return m_to_d(label)+push_d_to_stack();
}

//push *(*label+off)
static string push_to_stack(string label, string off){
  return m_to_d(label,off)+push_d_to_stack();
}

//push *label
static string push_to_stack_label(string label){
  return "@"+label+"\n"
         "D=M\n" + push_d_to_stack();
}

//only constant 0 or 1
static string push_constant_to_stack(string c){
  return "@SP\n"
         "A=M\n"
         "M="+c+"\n"
         "@SP\n"
         "M=M+1\n";
}

//连续push相同的constant   n>=1
static string push_constant_to_stack_n_times(string c, int n){
  if(n==0) return "";
  string s="";
  if(c!="0" && c!="1"){
    s += "@"+c+"\n"
         "D=A\n";
    c = "D";
  }

  s += "@SP\n"
       "A=M\n"
       "M="+c+"\n";

  for(int i=2;i<=n;i++){
    s += "@SP\n"
          "AM=M+1\n"
          "M="+c+"\n";
  }

  return s + "@SP\nM=M+1\n";
}
 

static string pop_d_from_stack(){
  return "@SP\n"
         "AM=M-1\n"
         "D=M\n";
}

//只借助r13寄存器进行pop
// *(*label+off) 
static string pop_from_stack(string label, string off){
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

// *(off)
static string pop_from_stack(string off){
  return "@"+off+"\n"
         "D=A"+"\n"
         "@13\n"
         "M=D\n"
         +pop_d_from_stack()+
         "@13\n"
         "A=M\n"
         "M=D\n";
}

// *(*label)
static string pop_from_stack_label(string label){
  return pop_d_from_stack()+
         "@"+label+"\n"
         "A=M\n"
         "M=D\n";
}


//calc addr  ex. SP+1  、 ARG+1
static string calc_addr_to_d(string label, string n,string op){
  return "@" + label +"\n"
         "D=M\n"
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
static string push_to_stack_value(string value){
  return "@"+value+"\n"
         "D=A\n"+push_d_to_stack();
}


void CodeWriter::writePushPop(CType c, string segment,string index){
  uint16_t idx;
  switch(c){
    case C_PUSH:
      if(segment=="constant"){
        extern int constant_times;
        out << push_constant_to_stack_n_times(index, constant_times);
      }else if(segment=="local"){
        if(index=="0")
          out << push_to_stack("LCL");
        else
          out << push_to_stack("LCL", index);

      }else if(segment=="argument"){
        if(index=="0")
          out << push_to_stack("ARG");
        else
          out << push_to_stack("ARG", index);

      }else if(segment=="this"){
        if(index=="0")
          out << push_to_stack("THIS");
        else
          out << push_to_stack("THIS", index);

      }else if(segment=="that"){
        if(index=="0")
          out << push_to_stack("THAT");
        else
          out << push_to_stack("THAT", index);

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
        if(index=="0")
          out << pop_from_stack_label("LCL");  // 为了省一点空间
        else
          out << pop_from_stack("LCL",index);

      }else if(segment=="argument"){
        if(index=="0")
          out << pop_from_stack_label("ARG");
        else
          out << pop_from_stack("ARG", index);

      }else if(segment=="this"){
        if(index=="0")
          out << pop_from_stack_label("THIS");
        else
          out << pop_from_stack("THIS", index);

      }else if(segment=="that"){
        if(index=="0")
          out << pop_from_stack_label("THAT");
        else
          out << pop_from_stack("THAT", index);

      }else if(segment == "pointer"){
        index = index=="0"?"3":"4";
        out << pop_from_stack(index);
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
  out << push_to_stack_value(symbol) +   //push return-address
         push_to_stack_label("LCL") +    //push LCL
         push_to_stack_label("ARG") +    //push ARG
         push_to_stack_label("THIS") +   //push THIS
         push_to_stack_label("THAT") +   //push THAT
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

        pop_from_stack_label("ARG") +
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
  extern int constant_times;
  sscanf(numLocals.c_str(),"%d", &constant_times); 

  out<< "("+functionName+")\n" +
        push_constant_to_stack_n_times("0", constant_times);
  constant_times = 1;
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