#include<iostream>
#include"JackTokenizer.h"
#include"ParsePath.h"
using namespace std;

static string input_file;
static string output_file;


int main(int argc, char* argv[]){
  if(argc != 2){
    cout<<"usage: prog jack/path\n";
    exit(1);
  }

  path_init(argv[1],output_file);
  

  while(hasNextFile()){
    input_file = nextFile(output_file);
    ofstream out(output_file);
    JackTokenizer jk(input_file);
    cout<<"parsing "<<input_file<<"..."<<endl;

    out<<"<tokens>"<<endl;

    while(jk.hasMoreTokens()){
      jk.advance();
      switch (jk.tokenType())
      {
      case KEYWORD:
        out<<"<keyword>";
        out<<jk.keyword();
        out<<"</keyword>\n";
        break;
      
      case SYMBOL:
        out<<"<symbol>";
        if(jk.symbol() == '<'){
          out<<"&lt;"; 
        }else if(jk.symbol() == '>'){
          out<<"&gt;";
        }else if(jk.symbol() == '&'){
          out<<"&amp;";
        }else{
          out<<jk.symbol();
        }
        out<<"</symbol>\n";
        break;
      case IDENTIFIER:
        out<<"<identifier>";
        out<<jk.identifier();
        out<<"</identifier>\n";
        break;
      case INT_CONST:
        out<<"<integerConstant>";
        out<<jk.intVal();
        out<<"</integerConstant>\n";
        break;
      case STRING_CONST:
        out<<"<stringConstant>";
        out<<jk.stringVal();
        out<<"</stringConstant>\n";
        break;
      }

    }
    out<<"</tokens>"<<endl;
  }
}