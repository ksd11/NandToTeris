#include"JackTokenizer.h"

char JackTokenizer::getc(){
  if(idx==len){ //fill
    if(!fillstringbuf()) return -1;
  }
  return stringbuf[idx++];
}

bool JackTokenizer::fillstringbuf(){
  if(getline(in,stringbuf)){
    stringbuf += '\n';
    idx = 0;
    len = stringbuf.length();
    return true;
  }
  return false;
}

void JackTokenizer::undoc(){
  idx--;
}


JackTokenizer::JackTokenizer(string filename){
  in.open(filename); //open file
  if(!in){
    cout<<filename<<" not exist!\n";
    exit(1);
  }
}

JackTokenizer::~JackTokenizer(){
  in.close();
}

static bool checkblank(char ch){
  return ch==' '||ch=='\t'||ch=='\n';
}

static bool checkalnum_(char ch){
  return isalnum(ch) || ch=='_';
}

bool JackTokenizer::hasMoreTokens(){
  char ch,ch2;
  while((ch = getc())!=-1){
    if(checkblank(ch)) continue;
    //注释
    else if(ch=='/'){
      ch2 = getc();
      if(ch2=='/'){
        while((ch=getc())!='\n');//后面全是注释
      }else if(ch2=='*'){
        while(1){
          while((ch=getc())!='*'){
            if(ch==-1){
              cout<<"parse error\n";
              exit(1);
            }
          }
          if((ch=getc()) == '/'){
            break;
          }else{
            undoc();
          }
        }
        
      }else{
        undoc();
        undoc();   //  除号
        return true;
      }
    }else{
      undoc();
      return true;
    }
  }
  return false;
}

void JackTokenizer::advance(){
  string s;
  char ch = getc();
  if(symbols.count(ch)){  //symbol
    sb = ch;
    tt = SYMBOL;

  }else if(ch>='0' && ch<='9'){ //int
    s = ch;
    ch=getc();
    while(isdigit(ch)){
      s+=ch;
      ch=getc();
    }
    undoc();
    sscanf(s.c_str(), "%hu", &intval);
    tt = INT_CONST;

  }else if(ch=='"'){ //string
    while((ch = getc())!='"'){
      if(ch=='\n'){
        cout<< "not match quotes\n";
      }
      s+=ch;
    }
    stringval = s;
    tt=STRING_CONST;

  }else if(ch=='_'||isalpha(ch)){
    s = ch;
    ch = getc();
    while(checkalnum_(ch)){
      s+=ch;
      ch=getc();
    }
    undoc();

    if(keywords.count(s)){ //keyword
      kw = s;
      tt = KEYWORD;
    }else{  //identifier
      id = s;
      tt = IDENTIFIER;
    }
  }else{
    cout<<"unkonw character: "<<ch<<endl;
    exit(1);
  }
}

TokenType JackTokenizer::tokenType(){
  return tt;
}

string JackTokenizer::keyword(){
  return kw;
}

char JackTokenizer::symbol(){
  return sb;
}

string JackTokenizer::identifier(){
  return id;
}

uint16_t JackTokenizer::intVal(){
  return intval;
}

string JackTokenizer::stringVal(){
  return stringval;
}
