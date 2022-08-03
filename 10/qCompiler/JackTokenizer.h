#ifndef JACKTOKENIZER_H
#define JACKTOKENIZER_H


#include<string>
#include<iostream>
#include<cstdint>
#include<fstream>
#include<set>
using namespace std;

enum TokenType{
  KEYWORD,
  SYMBOL,        // { + } () ...
  IDENTIFIER,    // 字母、数字、下划线
  INT_CONST,     // 0-32767
  STRING_CONST,  // "abc"
};

static set<char> symbols = {
  '(',')','[',']',
  '{','}',',',';',
  '=','.','+','-',
  '*','/','&','|',
  '~','<','>'
};

static set<string> keywords = {
  "class","method","int","function",
  "boolean","constructor","char","void",
  "var","static","field","let","do",
  "if","else","while","return","true",
  "false","null","this"
};

class JackTokenizer{
public:
  JackTokenizer(string filename);
  ~JackTokenizer();
  bool hasMoreTokens();
  void advance();
  TokenType tokenType();
  string keyword();
  char symbol();
  string identifier();
  uint16_t intVal();
  string stringVal();

  char getc();  //读取一个字符
  void undoc(); //撤销读取的字符
private:
  ifstream in;
  TokenType tt;
  string kw;
  char sb;
  string id;
  uint16_t intval;
  string stringval;
  
  string stringbuf;
  bool fillstringbuf(); //填充stringbuff
  int idx=0;
  int len=0;
};

#endif