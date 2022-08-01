#include<string>
#include<cstdint>
#include<fstream>
#include<iostream>
#include<regex>
#include"Parser.h"
using namespace std;

Parser::Parser(string filename){
  in.open(filename); //open file
  if(!in){
    cout<<filename<<" not exist!\n";
    exit(1);
  }
}

Parser::~Parser(){
  in.close();
}

static string cline = "\\s*([a-z].*?)\\s*(//.*)?";
static regex cmd_regex(cline);

static string arith = "(add|sub|neg|eq|gt|lt|and|or|not)";
static regex arith_regex(arith); 

static string mem = "(push|pop)\\s+(argument|local|static|constant|this|that|pointer|temp)\\s+(\\d+)";
static regex mem_regex(mem);

static string symbol = "[a-zA-Z_.$:][\\w.$:]*";
static regex control_regex("(label|goto|if-goto)\\s+("+symbol+")");

static regex f_regex("(function|call)\\s+("+symbol+")\\s+(\\d+)");

bool Parser::hasMoreCommands(){
  string tmp;
  while(getline(in,tmp)){
    //忽略注释和空行
    smatch results;
    if(regex_match(tmp, results, cmd_regex)){
      commandline = results.str(1);
    }else{
      continue; //空行或者注释行
    }

    return true;
  }
  return false; 
}


void Parser::advance(){
#ifdef DEBUG
  cout<<commandline<<": ";
#endif
  smatch results;
  if(regex_match(commandline, results, arith_regex)){
    a1 = results.str(1);
    type = C_ARITHMETIC;
  }else if(regex_match(commandline, results,mem_regex)){
    if(results.str(1) == "push"){
      type = C_PUSH;
    }else{
      type = C_POP;
    }
    a1 = results.str(2);
    // sscanf(results.str(3).c_str(),"%hu", &a2);
    a2 = results.str(3);
  }else if(regex_match(commandline,results,control_regex)){
    if(results.str(1) == "label"){
      type = C_LABEL;
    }else if(results.str(1) == "goto"){
      type = C_GOTO;
    }else{
      type = C_IF;
    }
    a1 = results.str(2);
  }else if(regex_match(commandline,results,f_regex)){
    if(results.str(1) == "function"){
      type = C_FUNCTION;
    }else{
      type = C_CALL;
    }
    a1 = results.str(2);
    // sscanf(results.str(3).c_str(),"%hu", &a2);
    a2 = results.str(3);
  }else if(commandline == "return"){
    type = C_RETURN;
  }else{
    cout<<"error: "<<commandline<<endl;
    exit(1);
  }
}


CType Parser::commandType(){
  return type;
}

string Parser::arg1(){
  return a1;
}

string Parser::arg2(){
  return a2;
}
