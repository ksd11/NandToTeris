#include"CompilationEngine.h"

// #define BUILD_XML

CompilationEngine::CompilationEngine(string input_file,string output_file){
  jk = new JackTokenizer(input_file);
  out.open(output_file);
  st = new SymbolTable();
}

CompilationEngine::~CompilationEngine(){
  delete jk;
  out.close();
  delete st;
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

void CompilationEngine::writeXml(TokenType t){
#ifdef BUILD_XML
  string sb;
  switch(t){
    case KEYWORD:
      out<<"<keyword> "+jk->keyword()+" </keyword>"<<endl;
      break;
    case IDENTIFIER:
      out<<"<identifier> "+jk->identifier()+" </identifier>"<<endl;
      break;
    case SYMBOL:
      sb += jk->symbol();
      if(jk->symbol()=='<') sb="&lt;";
      else if(jk->symbol()== '>') sb="&gt;";
      else if(jk->symbol()== '&') sb="&amp;";

      out<<"<symbol> "+sb+" </symbol>"<<endl;
      break;
    case INT_CONST:
      out<<"<integerConstant> ";
      out<<jk->intVal();
      out<<" </integerConstant>"<<endl;
      break;
    case STRING_CONST:
      out<<"<stringConstant> "+jk->stringVal()+" </stringConstant>"<<endl;
  }
#endif
}

void CompilationEngine::writeXml(string str){
#ifdef BUILD_XML
  out<<str<<endl;
#endif
}

/******* 上面都是helper函数 **/

void CompilationEngine::compileClass(){
  writeXml("<class>");

  readToken_keyword("class"); // 'class'
  writeXml(KEYWORD);

  readToken_identifier();  // className
  writeXml(IDENTIFIER);

  readToken_symbol('{'); // '{'
  writeXml(SYMBOL);
  
  readToken(); // classVarDec*
  while(jk->tokenType()==KEYWORD && (jk->keyword()=="static" || jk->keyword()=="field")){
    writeXml("<classVarDec>");
    rollback = true;
    compileClassVarDec();
    writeXml("</classVarDec>");
    readToken();
  }
  st->printGlobalTable();

  // subroutineDec*
  while(jk->tokenType()==KEYWORD && (jk->keyword()=="constructor"||jk->keyword()=="function"||jk->keyword()=="method")){
    writeXml("<subroutineDec>");
    st->startSubroutine();
    rollback = true;
    compileSubroutine();
    writeXml("</subroutineDec>");
    readToken();
    st->printLocalTable();
  }

  rollback = true;
  readToken_symbol('}'); // '}'
  writeXml(SYMBOL);
  writeXml("</class>");
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
  string var_kind;  //当前变量的kind
  string var_type;  //当前变量的type
  string var_name; //当前变量的name  

  readToken_keyword(set<string>({"static","field"}));//(‘static’|'field')
  writeXml(KEYWORD);
  var_kind = jk->keyword();

  readToken(); //type
  if(int t = type()){
    if(t == 1){
      writeXml(KEYWORD);
      var_type = jk->keyword();
    }else{
      writeXml(IDENTIFIER);
      var_type = jk->identifier();
    } 

    readToken_identifier(); //varname
    writeXml(IDENTIFIER);
    var_name = jk->identifier();
    st->define(var_name,var_type,var_kind);

    readToken();
    while(is_symbol(',')){
      writeXml(SYMBOL);
      readToken_identifier(); // varName
      writeXml(IDENTIFIER);
      var_name = jk->identifier();
      st->define(var_name,var_type,var_kind);
      readToken();
    }
    rollback = true;
    readToken_symbol(';');
    writeXml(SYMBOL);
  }else{
    cout<<"compile classVarDec error...\n";
  }
}

void CompilationEngine::compileSubroutine(){
  static set<string> statements = {"let","if","else","while","do","return"}; 

  readToken_keyword(set<string>({"constructor","function","method"}));
  writeXml(KEYWORD);

  readToken();
  int t = type();
  if(t || (jk->tokenType()==KEYWORD && jk->keyword()=="void")){
    if(t==1 || (jk->tokenType()==KEYWORD && jk->keyword()=="void")) writeXml(KEYWORD);
    else writeXml(IDENTIFIER);

    readToken_identifier(); // subroutineName
    writeXml(IDENTIFIER);

    readToken_symbol('(');
    writeXml(SYMBOL);

    writeXml("<parameterList>");
    compileParameterList();
    writeXml("</parameterList>");

    readToken_symbol(')');
    writeXml(SYMBOL);

    writeXml("<subroutineBody>");
    readToken_symbol('{');
    writeXml(SYMBOL);
    //varDec* 
    readToken();
    while(jk->tokenType()==KEYWORD && jk->keyword()=="var"){
      writeXml("<varDec>");
      rollback = true;
      compileVarDec();
      writeXml("</varDec>");
      readToken();
    }
    // statements
    rollback = true;
    writeXml("<statements>");
    compileStatements();
    writeXml("</statements>");
    readToken_symbol('}');
    writeXml(SYMBOL);
    writeXml("</subroutineBody>");

  }else{
    cout<<"compile subroutine error...\n";
  }

}

//((type varName)(',' type varName)*)?
void CompilationEngine::compileParameterList(){
  string var_type;
  string var_name;
  SymbolKind var_kind = ARG;

  readToken();  // type
  while(int t = type()){
    if(t==1){
      writeXml(KEYWORD);
      var_type = jk->keyword();
    } 
    else{
      writeXml(IDENTIFIER);
      var_type = jk->identifier();
    } 

    readToken_identifier();
    writeXml(IDENTIFIER);
    var_name = jk->identifier();
    st->define(var_name,var_type,var_kind);

    readToken();
    if(jk->tokenType()==SYMBOL && jk->symbol()==','){
      writeXml(SYMBOL);
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
  string var_type;
  string var_name;
  SymbolKind var_kind = VAR;

  readToken_keyword("var");
  writeXml(KEYWORD);

  readToken();
  if(int t = type()){
    if(t==1){
      writeXml(KEYWORD);
      var_type = jk->keyword();
    } 
    else{
      writeXml(IDENTIFIER);
      var_type = jk->identifier();
    } 
    readToken_identifier();
    writeXml(IDENTIFIER);
    var_name = jk->identifier();
    st->define(var_name,var_type,var_kind);

    readToken();
    while(jk->tokenType()==SYMBOL && jk->symbol()==','){
      writeXml(SYMBOL);
      readToken_identifier();
      writeXml(IDENTIFIER);
      var_name = jk->identifier();
      st->define(var_name,var_type,var_kind);
      readToken();
    }

    rollback = true;
    readToken_symbol(';');
    writeXml(SYMBOL);
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
      writeXml("<letStatement>");
      compileLet();
      writeXml("</letStatement>");
    }else if(jk->tokenType()==KEYWORD && jk->keyword()=="if"){
      rollback = true;
      writeXml("<ifStatement>");
      compileIf();
      writeXml("</ifStatement>");
    }else if(jk->tokenType()==KEYWORD && jk->keyword()=="while"){
      rollback = true;
      writeXml("<whileStatement>");
      compileWhile();
      writeXml("</whileStatement>");
    }else if(jk->tokenType()==KEYWORD && jk->keyword()=="do"){
      rollback = true;
      writeXml("<doStatement>");
      compileDo();
      writeXml("</doStatement>");
    }else if(jk->tokenType()==KEYWORD && jk->keyword()=="return"){
      rollback = true;
      writeXml("<returnStatement>");
      compileReturn();
      writeXml("</returnStatement>");
    }else{
      rollback = true;
      return;
    }
  }
  
}

// 'do' subroutineCall ';'
void CompilationEngine::compileDo(){
  readToken_keyword("do");
  writeXml(KEYWORD);
  readToken_identifier(); //subroutineName / class/var
  writeXml(IDENTIFIER);

  readToken();
  rollback = true;
  if(jk->tokenType()==SYMBOL && jk->symbol()=='.'){
    readToken_symbol('.');
    writeXml(SYMBOL);
    readToken_identifier();
    writeXml(IDENTIFIER);
  }
  readToken_symbol('(');
  writeXml(SYMBOL);
  writeXml("<expressionList>");
  compileExpressionList();
  writeXml("</expressionList>");
  readToken_symbol(')');
  writeXml(SYMBOL);

  readToken_symbol(';');
  writeXml(SYMBOL);
}

// 'let' varName ('[' expression ']')? '=' expression ';'
void CompilationEngine::compileLet(){
  readToken_keyword("let");
  writeXml(KEYWORD);

  readToken_identifier(); // varName
  writeXml(IDENTIFIER);
  readToken();
  if(is_symbol('[')){
    writeXml(SYMBOL);

    writeXml("<expression>");
    compileExpression(); 
    writeXml("</expression>");

    readToken_symbol(']');
    writeXml(SYMBOL);
  }else{
    rollback = true;
  }
  readToken_symbol('=');
  writeXml(SYMBOL);

  writeXml("<expression>");
  compileExpression();
  writeXml("</expression>");

  readToken_symbol(';');
  writeXml(SYMBOL);
}

void CompilationEngine::compileWhile(){
  readToken_keyword("while");
  writeXml(KEYWORD);
  readToken_symbol('(');
  writeXml(SYMBOL);
  writeXml("<expression>");
  compileExpression();
  writeXml("</expression>");
  readToken_symbol(')');
  writeXml(SYMBOL);
  readToken_symbol('{');
  writeXml(SYMBOL);
  writeXml("<statements>");
  compileStatements();
  writeXml("</statements>");
  readToken_symbol('}');
  writeXml(SYMBOL);
}

void CompilationEngine::compileReturn(){
  readToken_keyword("return");
  writeXml(KEYWORD);
  readToken();
  if(is_symbol(';')){
    writeXml(SYMBOL);
    return;
  }
  rollback = true;
  writeXml("<expression>");
  compileExpression();
  writeXml("</expression>");
  readToken_symbol(';');
  writeXml(SYMBOL);
}

void CompilationEngine::compileIf(){
  readToken_keyword("if");
  writeXml(KEYWORD);
  readToken_symbol('(');
  writeXml(SYMBOL);
  writeXml("<expression>");
  compileExpression();
  writeXml("</expression>");
  readToken_symbol(')');
  writeXml(SYMBOL);
  readToken_symbol('{');
  writeXml(SYMBOL);
  writeXml("<statements>");
  compileStatements();
  writeXml("</statements>");
  readToken_symbol('}');
  writeXml(SYMBOL);

  readToken();  //预取后一个token
  if(is_keyword("else")){
    writeXml(KEYWORD);
    readToken_symbol('{');
    writeXml(SYMBOL);
    writeXml("<statements>");
    compileStatements();
    writeXml("</statements>");
    readToken_symbol('}');
    writeXml(SYMBOL);
  }else{
    rollback = true;
  }
}

//term (op term)*
void CompilationEngine::compileExpression(){
  static set<char> ops = {
    '+','-','*','/','&','|','<','>','='
  };
  writeXml("<term>");
  compileTerm();
  writeXml("</term>");

  readToken();
  if(jk->tokenType()==SYMBOL && ops.count(jk->symbol())){
    writeXml(SYMBOL);
    writeXml("<term>");
    compileTerm();
    writeXml("</term>");
    readToken();
  }
  rollback = true;
}

void CompilationEngine::compileTerm(){
  static set<string> keyword_constant = {
    "true","false","null","this"
  };
  readToken();
  switch(jk->tokenType()){
    case IDENTIFIER: 
      writeXml(IDENTIFIER);
      readToken();
      if(is_symbol('[')){ //varName '[' expression ']'
        writeXml(SYMBOL);
        writeXml("<expression>");
        compileExpression();
        writeXml("</expression>");
        readToken_symbol(']');
        writeXml(SYMBOL);
      }else if(is_symbol('.') || is_symbol('(')){ //subroutineCall
        if(is_symbol('.')){
          writeXml(SYMBOL);
          readToken_identifier();
          writeXml(IDENTIFIER);
        }else{
          rollback = true;
        }
        readToken_symbol('(');
        writeXml(SYMBOL);
        writeXml("<expressionList>");
        compileExpressionList();
        writeXml("</expressionList>");
        readToken_symbol(')');
        writeXml(SYMBOL);
      }else{ //varName
        rollback = true;
      }
      return;
    case KEYWORD: // integerConstant
      if(!keyword_constant.count(jk->keyword())){
        cout<<"illegal term-keyword error: "<<jk->keyword()<<endl;
        exit(1);
      }
      writeXml(KEYWORD);
      return;
    case INT_CONST: // int const
      writeXml(INT_CONST);
      return;
    case STRING_CONST: //string const
      writeXml(STRING_CONST);
      return;
    case SYMBOL:
      char sb = jk->symbol();
      if(sb == '('){ // '(' expression ')'
        writeXml(SYMBOL);
        writeXml("<expression>");
        compileExpression();
        writeXml("</expression>");
        readToken_symbol(')');
        writeXml(SYMBOL);
      }else if(sb=='-' || sb=='~'){ // unaryOp term
        writeXml(SYMBOL);
        writeXml("<term>");
        compileTerm();
        writeXml("</term>");
      }
      return;

  }
  cout<<"illegal term\n";
  exit(1);
}

void CompilationEngine::compileExpressionList(){
  readToken();
  if(!is_symbol(')')){
    rollback = true;
    writeXml("<expression>");
    compileExpression();
    writeXml("</expression>");
    readToken();
    while(is_symbol(',')){
      writeXml(SYMBOL);
      writeXml("<expression>");
      compileExpression();
      writeXml("</expression>");
      readToken();
    }
  }
  rollback = true;
}
