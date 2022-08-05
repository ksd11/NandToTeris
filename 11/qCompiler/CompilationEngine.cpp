#include"CompilationEngine.h"

// #define BUILD_XML

CompilationEngine::CompilationEngine(string input_file,string output_file){
  jk = new JackTokenizer(input_file);
#ifdef BUILD_XML
  out.open(output_file);
#endif
  st = new SymbolTable();
  vm = new VMWriter(output_file);
}

CompilationEngine::~CompilationEngine(){
  delete jk;
#ifdef BUILD_XML
  out.close();
#endif
  delete vm;
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

// method/constructor/method -> className.xxx
static string className;

static string general_label(){
  static int index = 0;
  return "qCompiler.build."+className +"."+ to_string(index++);
}

static bool isConstructor;
static int classVarNum = 0;

void CompilationEngine::push_this_to_stack(){
  if(isConstructor){
    vm->writePush(POINTER, 0);
  }else{
    vm->writePush(ARG, 0);
  }
}

void CompilationEngine::subroutineCall(string id1, string id2){
  string functionName;
  int nArgs=0;

  if(st->isExist(id1)){ // obj
    functionName = st->typeOf(id1)+"."+id2;
    if(st->kindOf(id1) == "field"){
      push_this_to_stack();
      vm->writePop(POINTER, 0);
      vm->writePush(THIS,st->indexOf(id1));
    }else{
      vm->writePush(st->kindOf(id1),st->indexOf(id1));
    }
    nArgs++;
    //st->define("this",st->typeOf(id1),"argument");
  }else if(id2.length()>0){ //class
    functionName = id1+"."+id2;
  }else{ // routine
    functionName = className+"."+id1;
    push_this_to_stack();
    nArgs++;
    //st->define("this",className,"argument");
  }
  readToken_symbol('(');
  writeXml(SYMBOL);
  writeXml("<expressionList>");
  nArgs += compileExpressionList();
  writeXml("</expressionList>");
  readToken_symbol(')');
  writeXml(SYMBOL);
  vm->writeCall(functionName, nArgs);
}

/******* 上面都是helper函数 **/

void CompilationEngine::compileClass(){
  classVarNum = 0;
  writeXml("<class>");

  readToken_keyword("class"); // 'class'
  writeXml(KEYWORD);

  readToken_identifier();  // className
  writeXml(IDENTIFIER);
  className = jk->identifier();

  readToken_symbol('{'); // '{'
  writeXml(SYMBOL);
  
  readToken(); // classVarDec*
  while(jk->tokenType()==KEYWORD && (jk->keyword()=="static" || jk->keyword()=="field")){
    writeXml("<classVarDec>");
    rollback = true;
    classVarNum += compileClassVarDec();
    writeXml("</classVarDec>");
    readToken();
  }
  // st->printGlobalTable();

  // subroutineDec*
  while(jk->tokenType()==KEYWORD && (jk->keyword()=="constructor"||jk->keyword()=="function"||jk->keyword()=="method")){
    writeXml("<subroutineDec>");
    st->startSubroutine();
    rollback = true;
    compileSubroutine();
    writeXml("</subroutineDec>");
    readToken();
    // st->printLocalTable();
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

int CompilationEngine::compileClassVarDec(){
  int res = 0;
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
    res++;

    readToken();
    while(is_symbol(',')){
      writeXml(SYMBOL);
      readToken_identifier(); // varName
      writeXml(IDENTIFIER);
      var_name = jk->identifier();
      st->define(var_name,var_type,var_kind);
      res++;
      readToken();
    }
    rollback = true;
    readToken_symbol(';');
    writeXml(SYMBOL);
  }else{
    cout<<"compile classVarDec error...\n";
    exit(1);
  }
  return var_kind=="field"? res : 0;
}

void CompilationEngine::compileSubroutine(){
  string functionName = className+".";
  int nArgs = 0; //局部变量的数量
  isConstructor = false;

  static set<string> statements = {"let","if","else","while","do","return"}; 

  readToken_keyword(set<string>({"constructor","function","method"}));
  writeXml(KEYWORD);
  string method_type = jk->keyword();
  if(method_type=="constructor") isConstructor=true;

  readToken();
  int t = type();
  if(t || (jk->tokenType()==KEYWORD && jk->keyword()=="void")){
    if(t==1 || (jk->tokenType()==KEYWORD && jk->keyword()=="void")) writeXml(KEYWORD); 
    else writeXml(IDENTIFIER);

    readToken_identifier(); // subroutineName
    writeXml(IDENTIFIER);
    functionName += jk->identifier();

    readToken_symbol('(');
    writeXml(SYMBOL);

    if(method_type == "method"){ // 定义this为0号元素
      st->define("this",className,"argument");
    }

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
      nArgs+=compileVarDec();
      writeXml("</varDec>");
      readToken();
    }
    vm->writeFunction(functionName, nArgs);

    if(isConstructor){
      vm->writePush(CONST,classVarNum);
      vm->writeCall("Memory.alloc", 1); //返回时this的地址压入栈中
      vm->writePop(POINTER, 0);  //临时保存this地址
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
    exit(1);
  }

}

//((type varName)(',' type varName)*)?
void CompilationEngine::compileParameterList(){
  string var_type;
  string var_name;
  string var_kind = "argument";

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
int CompilationEngine::compileVarDec(){
  int res = 0;
  string var_type;
  string var_name;
  string var_kind = "var";

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
    res++;

    readToken();
    while(jk->tokenType()==SYMBOL && jk->symbol()==','){
      writeXml(SYMBOL);
      readToken_identifier();
      writeXml(IDENTIFIER);
      var_name = jk->identifier();
      st->define(var_name,var_type,var_kind);
      res++;
      readToken();
    }

    rollback = true;
    readToken_symbol(';');
    writeXml(SYMBOL);
  }else{
    cout<<"compile VarDec error...\n";
    exit(1);
  }
  return res;
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
  string id1,id2;
  readToken_keyword("do");
  writeXml(KEYWORD);
  readToken_identifier(); //subroutineName / class/var
  writeXml(IDENTIFIER);
  id1 = jk->identifier();

  readToken();
  rollback = true;
  if(jk->tokenType()==SYMBOL && jk->symbol()=='.'){
    readToken_symbol('.');
    writeXml(SYMBOL);
    readToken_identifier(); // functionName
    writeXml(IDENTIFIER);
    id2 = jk->identifier();
  }

  //下面这段代码确定是 Class.function/var.method/method 的函数调用，并执行
  subroutineCall(id1, id2);

  readToken_symbol(';');
  writeXml(SYMBOL);

  vm->writePop(TEMP,0); //忽略返回值
}

// 'let' varName ('[' expression ']')? '=' expression ';'
void CompilationEngine::compileLet(){
  string segment;
  int index = 0;
  bool subindex = false; //是否有 []下标

  readToken_keyword("let");
  writeXml(KEYWORD);

  readToken_identifier(); // varName
  writeXml(IDENTIFIER);

  string varName = jk->identifier();
  if(!st->isExist(varName)){
    cout<<"compile error: "<<jk->identifier()<<" not defined!"<<endl;
    exit(1);
  }

  segment = st->kindOf(varName);// field、static、var、argument
  index = st->indexOf(varName);
  
  readToken();
  if(is_symbol('[')){
    writeXml(SYMBOL);

    writeXml("<expression>");
    compileExpression(); 
    writeXml("</expression>");

    readToken_symbol(']');
    writeXml(SYMBOL);
    subindex = true;
  }else{
    rollback = true;
  }
  readToken_symbol('=');
  writeXml(SYMBOL);

  writeXml("<expression>");
  compileExpression();
  writeXml("</expression>");
  /*
    a = b => 栈顶 b
    a[i] = b => 栈顶 b i
  */
  if(subindex){ //Array
    vm->writePop(TEMP, 0);
    if(segment=="field"){  // field Array a;
      push_this_to_stack();
      vm->writePop(POINTER,0);
      vm->writePush(THIS,index); //a
      
    }else{ 
      vm->writePush(segment,index);
    }
    vm->writeArithmetic('+');  //a[i]
    vm->writePop(POINTER,1);
    vm->writePush(TEMP,0);
    vm->writePop(THAT,0);
  }else{
    if(segment=="field"){
      push_this_to_stack();
      vm->writePop(POINTER,0);
      vm->writePop(THIS,index);
    }else{
      vm->writePop(segment,index);
    }
  }

  readToken_symbol(';');
  writeXml(SYMBOL);
}

void CompilationEngine::compileWhile(){
  string l1 = general_label();
  string l2 = general_label();

  readToken_keyword("while");
  writeXml(KEYWORD);
  readToken_symbol('(');
  writeXml(SYMBOL);
  
  vm->writeLabel(l1);
  writeXml("<expression>");
  compileExpression();
  writeXml("</expression>");
  readToken_symbol(')');
  writeXml(SYMBOL);
  readToken_symbol('{');
  writeXml(SYMBOL);
  vm->writeArithmetic('~');
  vm->writeIf(l2);

  writeXml("<statements>");
  compileStatements();
  writeXml("</statements>");
  readToken_symbol('}');
  writeXml(SYMBOL);
  vm->writeGoto(l1);
  vm->writeLabel(l2);
}

void CompilationEngine::compileReturn(){
  readToken_keyword("return");
  writeXml(KEYWORD);
  readToken();
  if(is_symbol(';')){
    writeXml(SYMBOL);
    vm->writePush(CONST, 0);  // 返回类型为void必须返回0作为返回值
    vm->writeReturn();
    return;
  }
  rollback = true;
  writeXml("<expression>");
  compileExpression();
  writeXml("</expression>");
  readToken_symbol(';');
  writeXml(SYMBOL);
  vm->writeReturn();
}

void CompilationEngine::compileIf(){
  string l1 = general_label();
  string l2 = general_label();

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
  vm->writeArithmetic('~');
  vm->writeIf(l1);
  writeXml("<statements>");
  compileStatements();
  writeXml("</statements>");
  readToken_symbol('}');
  writeXml(SYMBOL);

  vm->writeGoto(l2);
  vm->writeLabel(l1);
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
  vm->writeLabel(l2);
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
  while(jk->tokenType()==SYMBOL && ops.count(jk->symbol())){
    writeXml(SYMBOL);
    char op = jk->symbol();
    writeXml("<term>");
    compileTerm();
    writeXml("</term>");
    if(op=='*'){
      vm->writeCall("Math.multiply",2);
    }else if(op=='/'){
      vm->writeCall("Math.divide",2);
    }else{
      vm->writeArithmetic(op);
    }
    readToken();
  }
  rollback = true;
}

void CompilationEngine::compileTerm(){
  static set<string> keyword_constant = {
    "true","false","null","this"
  };
  readToken();
  string varName;
  string id1,id2;
  int index;
  string kind;
  string kw;
  string functionName;
  int nArgs;
  string str;
  int slen;

  switch(jk->tokenType()){
    case IDENTIFIER: 
      writeXml(IDENTIFIER);
      id1 = jk->identifier();
      readToken();
      if(is_symbol('[')){ //varName '[' expression ']'
        writeXml(SYMBOL);
        writeXml("<expression>");
        compileExpression();
        writeXml("</expression>");
        readToken_symbol(']');
        writeXml(SYMBOL);
        kind = st->kindOf(id1);
        index = st->indexOf(id1);
        if(kind=="field"){
          push_this_to_stack();
          vm->writePop(POINTER,0);
          vm->writePush(THIS,index);
        }else{
          vm->writePush(kind, index);
        }
        vm->writeArithmetic('+');  //a[i]
        vm->writePop(POINTER,1);
        vm->writePush(THAT,0);

      }else if(is_symbol('.') || is_symbol('(')){ //subroutineCall
        id2="";
        if(is_symbol('.')){
          writeXml(SYMBOL);
          readToken_identifier();
          writeXml(IDENTIFIER);
          id2 = jk->identifier();
        }else{
          rollback = true;
        }
        subroutineCall(id1, id2);
      }else{ //varName
        rollback = true;
        varName = jk->identifier();
        kind = st->kindOf(varName);
        index = st->indexOf(varName);
        if(kind=="field"){
          push_this_to_stack();
          vm->writePop(POINTER,0);
          vm->writePush(THIS,index);
        }else{
          vm->writePush(kind, index);
        }
      }
      return;
    case KEYWORD: // keyword constant
      if(!keyword_constant.count(jk->keyword())){
        cout<<"illegal term-keyword error: "<<jk->keyword()<<endl;
        exit(1);
      }
      writeXml(KEYWORD);
      kw = jk->keyword();
      if(kw!="this"){
        vm->writePush(CONST, 0);
        if(kw=="true") vm->writeArithmetic('~');
      }else push_this_to_stack();
      return;
    case INT_CONST: // int const
      writeXml(INT_CONST);
      vm->writePush(CONST,jk->intVal());
      return;
    case STRING_CONST: //string const
      writeXml(STRING_CONST);
      str = jk->stringVal();
      slen = str.length();
      vm->writePush(CONST, slen);
      vm->writeCall("String.new", 1); //this已被压入
      for(int i=0;i<slen;i++){
        vm->writePush(CONST, str[i]);
        vm->writeCall("String.appendChar", 2);
      }
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
        if(sb=='-')
          vm->writeArithmetic('n');
        else
          vm->writeArithmetic(sb);
      }
      return;

  }
  cout<<"illegal term\n";
  exit(1);
}

int CompilationEngine::compileExpressionList(){
  int res = 0;
  readToken();
  if(!is_symbol(')')){
    rollback = true;
    writeXml("<expression>");
    compileExpression();
    res++;
    writeXml("</expression>");
    readToken();
    while(is_symbol(',')){
      writeXml(SYMBOL);
      writeXml("<expression>");
      compileExpression();
      res++;
      writeXml("</expression>");
      readToken();
    }
  }
  rollback = true;
  return res;
}
