#include<iostream>
#include<string>
#include<fstream>
#include<regex>
#include"parser.h"
using namespace std;

static string symbol = "[a-zA-Z_.$:][\\w.$:]*";
static string Acommand = "@\\s*(\\d+|"+symbol+")\\s*"; // @123或@abc
static string dest = "([AMD]{1,3})?";
static string comp = "(0|1|-1|D|A|M|!D|!A|!M|-D|-A|-M|"
            "D\\s*\\+\\s*1|"  //D+1
            "A\\s*\\+\\s*1|"  //A+1
            "M\\s*\\+\\s*1|"  //M+1
            "D\\s*-\\s*1|"    //D-1
            "A\\s*-\\s*1|"    //A-1
            "M\\s*-\\s*1|"    //M-1
            "D\\s*\\+\\s*A|"  //D+A
            "D\\s*\\+\\s*M|"  //D+M
            "D\\s*-\\s*A|"     //D-A
            "D\\s*-\\s*M|"     //D-M
            "A\\s*-\\s*D|"     //A-D
            "M\\s*-\\s*D|"     //M-D
            "D\\s*&\\s*A|"     //D&A
            "D\\s*&\\s*M|"     //D&M
            "D\\s*\\|\\s*M|"   //D|A
            "D\\s*\\|\\s*M"   //D|M
            ")";
static string jump = "(;\\s*(JGT|JEQ|JGE|JLT|JNE|JLE|JMP))?";
static string Ccommand = dest+"\\s*=?\\s*"+comp+"\\s*"+jump;
static string Lcommand = "\\(\\s*("+symbol+")\\s*\\)";

static regex a_regex(Acommand);
static regex c_regex(Ccommand);
static regex l_regex(Lcommand);


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

void Parser::advance(){
#ifdef DEBUG
  cout<<commandline<<": ";
#endif
  smatch results;
  if(regex_match(commandline, results, a_regex)){
    // cout<<"A:"<<results.str(1)<<endl;
    s = results.str(1);
    type = A_COMMAND;
  }else if(regex_match(commandline, results,c_regex)){
    // cout<<"C:"<<results.str(1)<<","<<results.str(2)<<","<<results.str(3)<<endl;
    d = results.str(1);  //dest
    c = results.str(2); //comp
    j = results.str(3); 
    type = C_COMMAND;
  }else if(regex_match(commandline,results,l_regex)){
    // cout<<"L:"<<results.str(1)<<endl;
    s= results.str(1);
    type = L_COMMAND;
  }else{
    cout<<"error: "<<commandline<<endl;
    exit(1);
  }
}

bool Parser::hasMoreCommands(){
  string tmp;
  while(getline(in,tmp)){
    //忽略注释和空行
    string pattern = "\\s*([\\w@(].*?)\\s*(//.*)?";
    regex r(pattern);
    smatch results;
    if(regex_match(tmp, results, r)){
      commandline = results.str(1);
    }else{
      continue; //空行或者注释行
    }

    return true;
  }
  return false; 
}

string Parser::symbol(){
  return s;
}
string Parser::dest(){
  return d;
}
string Parser::comp(){
  return c;
}
string Parser::jump(){
  return j;
}

CType Parser::commandType(){
  return type;
}


