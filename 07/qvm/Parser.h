#ifndef PARSER_H
#define PARSER_H

#include<string>
#include<cstdint>
#include<fstream>
using namespace std;
enum CType{
  C_ARITHMETIC,
  C_PUSH,
  C_POP,
  C_LABEL,
  C_GOTO,
  C_IF,
  C_FUNCTION,
  C_RETURN,
  C_CALL
};

class Parser{
public:
  Parser(string filename);
  ~Parser();
  bool hasMoreCommands();
  void advance(); //解析命令
  CType commandType(); //返回命令类型
  string arg1();
  string arg2();
private:
  ifstream in;
  string commandline;
  CType type;
  string a1;
  string a2;
};

#endif