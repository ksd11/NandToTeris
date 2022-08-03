#include<string>
#include<fstream>
#include"JackTokenizer.h"
#include<set>
using namespace std;

class CompilationEngine{
public:
  CompilationEngine(string input_file,string output_file);
  ~CompilationEngine();
  void compileClass();
  void compileClassVarDec();
  void compileSubroutine();
  void compileParameterList();
  void compileVarDec(); 
  void compileStatements();
  void compileDo();
  void compileLet();
  void compileWhile();
  void compileReturn();
  void compileIf();
  void compileExpression();
  void compileTerm();
  void compileExpressionList();

private:
  JackTokenizer* jk;
  ofstream out;
  void readToken();
  void readToken_symbol(char ch);
  void readToken_identifier();
  void readToken_keyword(set<string> kws);
  void readToken_keyword(string kw);
  bool is_symbol(char ch);
  bool is_keyword(string kw);
  int type();
};

CompilationEngine::CompilationEngine(string input_file,string output_file){
  jk = new JackTokenizer(input_file);
  out.open(output_file);
}

CompilationEngine::~CompilationEngine(){
  delete jk;
  out.close();
}

static bool rollback = false;
void CompilationEngine::readToken(){
  if(rollback){  //如果设置了rollback,不重新读取
    rollback = false;
    return;  
  }

  if(jk->hasMoreTokens()){
    jk->advance();
  }else{
    cout<<"No token error.\n";
    exit(1);
  }
}

void CompilationEngine::readToken_symbol(char ch){
  readToken();
  if(jk->tokenType()!=SYMBOL || jk->symbol()!=ch){
    cout<<"read SYMBOL '"<<ch<<"' error.\n";
    cout<<"get SYMBOL '"<<jk->symbol()<<"'\n";
    exit(1);
  }
}

void CompilationEngine::readToken_identifier(){
  readToken();
  if(jk->tokenType()!=IDENTIFIER){
    cout<<"read IDENTIFIER error\n";
    exit(1);
  }
}
void CompilationEngine::readToken_keyword(set<string> kws){
  readToken();
  if(jk->tokenType()!=KEYWORD || !kws.count(jk->keyword())){
    cout<<"read KEYWORDS error\n";
    exit(1);
  }
}

void CompilationEngine::readToken_keyword(string kw){
  readToken();
  if(jk->tokenType()!=KEYWORD || jk->keyword()!= kw){
    cout<<"read KEYWORD "<<kw<<" error\n";
    exit(1);
  }
}

bool CompilationEngine::is_symbol(char ch){
  return jk->tokenType()==SYMBOL && jk->symbol()==ch;
}

bool CompilationEngine::is_keyword(string kw){
  return jk->tokenType()==KEYWORD && jk->keyword()==kw;
}


/******* 上面都是helper函数 **/

void CompilationEngine::compileClass(){
  readToken_keyword("class");
  readToken_identifier();
  readToken_symbol('{');
  
  readToken();
  while(jk->tokenType()==KEYWORD && (jk->keyword()=="static" || jk->keyword()=="field")){
    rollback = true;
    compileClassVarDec();
    readToken();
  }

  while(jk->tokenType()==KEYWORD && (jk->keyword()=="constructor"||jk->keyword()=="function"||jk->keyword()=="method")){
    rollback = true;
    compileSubroutine();
    readToken();
  }

  rollback = true;
  readToken_symbol('}');
}

int CompilationEngine::type(){
  static set<string> types = {"int","char","boolean"};
  TokenType tt = jk->tokenType();
  string kw = jk->keyword();
  string id = jk->identifier();
  if(tt==KEYWORD && types.count(kw)) return 1; //内建类型
  else if(tt==IDENTIFIER) return 2;  //自定义类型 
  return 0;
} 

void CompilationEngine::compileClassVarDec(){
  readToken_keyword(set<string>({"static","field"}));

  readToken(); //type
  if(type()){
    readToken_identifier(); //varname

    readToken();
    while(is_symbol(',')){
      readToken_identifier(); // varName
      readToken();
    }
    rollback = true;
    readToken_symbol(';');
  }else{
    cout<<"compile classVarDec error...\n";
  }
}



void CompilationEngine::compileSubroutine(){
  static set<string> statements = {"let","if","else","while","do","return"}; 

  readToken_keyword(set<string>({"constructor","function","method"}));

  readToken();
  if(type() || (jk->tokenType()==KEYWORD && jk->keyword()=="void")){
    readToken_identifier(); // name
    readToken_symbol('(');
    compileParameterList();

    readToken_symbol(')');
    readToken_symbol('{');
    //varDec* 
    readToken();
    while(jk->tokenType()==KEYWORD && jk->keyword()=="var"){
      rollback = true;
      compileVarDec();
      readToken();
    }
    // statements
    rollback = true;
    compileStatements();

    readToken_symbol('}');

  }else{
    cout<<"compile subroutine error...\n";
  }

}

//((type varName)(',' type varName)*)?
void CompilationEngine::compileParameterList(){
  readToken();  // type
  while(type()){
    readToken_identifier();
    readToken();
    if(jk->tokenType()==SYMBOL && jk->symbol()==','){
      readToken();
    }else{
      rollback = true;
      return;
    }
  }
  rollback = true;
}


// 'var' type varName (',' varName)* ';'
void CompilationEngine::compileVarDec(){
  readToken_keyword("var");
  readToken();
  if(type()){
    readToken_identifier();
    readToken();
    while(jk->tokenType()==SYMBOL && jk->symbol()==','){
      readToken_identifier();
    }

    rollback = true;
    readToken_symbol(';');
  }else{
    cout<<"compile VarDec error...\n";
  }
}

// statement*
void CompilationEngine::compileStatements(){
  while(1){
    readToken();
    if(jk->tokenType()==KEYWORD && jk->keyword()=="let"){
      rollback = true;
      compileLet();
    }else if(jk->tokenType()==KEYWORD && jk->keyword()=="if"){
      rollback = true;
      compileIf();
    }else if(jk->tokenType()==KEYWORD && jk->keyword()=="while"){
      rollback = true;
      compileWhile();
    }else if(jk->tokenType()==KEYWORD && jk->keyword()=="do"){
      rollback = true;
      compileDo();
    }else if(jk->tokenType()==KEYWORD && jk->keyword()=="return"){
      rollback = true;
      compileReturn();
    }else{
      rollback = true;
      return;
    }
  }
  
}

// 'do' subroutineCall ';'
void CompilationEngine::compileDo(){
  readToken_keyword("do");
  readToken_identifier(); //subroutineName / class/var

  readToken();
  rollback = true;
  if(jk->tokenType()==SYMBOL && jk->symbol()=='.'){
    readToken_symbol('.');
    readToken_identifier();
    readToken_symbol('(');
    compileExpressionList(); 
    readToken_symbol(')');
  }else{
    readToken_symbol('(');
    compileExpressionList();
    readToken_symbol(')');
  }

  readToken_symbol(';');
}

// 'let' varName ('[' expression ']')? '=' expression ';'
void CompilationEngine::compileLet(){
  readToken_keyword("let");
  readToken_identifier(); // varName
  readToken();
  if(is_symbol('[')){
    compileExpression(); 
    readToken_symbol(']');  
  }else{
    rollback = true;
  }
  readToken_symbol('=');
  compileExpression();
  readToken_symbol(';');
}

void CompilationEngine::compileWhile(){
  readToken_keyword("while");
  readToken_symbol('(');
  compileExpression();
  readToken_symbol(')');
  readToken_symbol('{');
  compileStatements();
  readToken_symbol('}');
}

void CompilationEngine::compileReturn(){
  readToken_keyword("return");
  readToken();
  if(is_symbol(';')){
    return;
  }
  rollback = true;
  compileExpression();
  readToken_symbol(';');
}

void CompilationEngine::compileIf(){
  readToken_keyword("if");
  readToken_symbol('(');
  compileExpression();
  readToken_symbol(')');
  readToken_symbol('{');
  compileStatements();
  readToken_symbol('}');

  readToken();  //预取后一个token
  if(is_keyword("else")){
    readToken_symbol('{');
    compileStatements();
    readToken_symbol('}');
  }else{
    rollback = true;
  }
}


void CompilationEngine::compileExpression(){
  readToken();
  if(jk->tokenType() == IDENTIFIER || jk->tokenType()==KEYWORD){
    return;  
  }
  // readToken_identifier();
  cout<<"illegal expression\n";
  exit(1);
}

void CompilationEngine::compileTerm(){

}

void CompilationEngine::compileExpressionList(){
  readToken();
  if(!is_symbol(')')){
    rollback = true;
    compileExpression();
    readToken();
    while(is_symbol(',')){
      compileExpression();
      readToken();
    }
  }
  rollback = true;
}

int main(int argc, char const *argv[])
{
  string in = "../test/ExpressionLessSquare/SquareGame.jack";
  string out = "../test/ExpressionLessSquare/Main.xml";
  CompilationEngine ce(in, out);
  ce.compileClass();
  return 0;
}


