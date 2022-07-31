#include<string>
#include<fstream>
using namespace std;

enum CType{
  A_COMMAND,
  C_COMMAND,  //dest=comp;jump
  L_COMMAND,  //(Xxx)
};

class Parser{
  public:
  Parser(string);
  ~Parser();
  bool hasMoreCommands();
  void advance(); //解析命令
  CType commandType(); //返回命令类型
  string symbol();
  string dest();
  string comp();
  string jump();
  private:
  ifstream in;
  string commandline; //将解析的命令行
  CType type;
  string s; //符号或十进制值
  string d;
  string c;
  string j;
};