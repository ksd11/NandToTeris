#ifndef CODE_WRITER_H
#define CODE_WRITER_H

#include<fstream>
#include<string>
#include"Parser.h"
using namespace std;

class CodeWriter{
public:
  CodeWriter(string filename);
  ~CodeWriter();
  // void setFileName(string filename);
  void writeArithmetic(string command);
  void writePushPop(CType c, string segment,string index);
  void writeDeathLoop();
  void writeLabel(string label);
  void writeGoto(string label);
  void writeIf(string label);
  void writeCall(string functionName, string numArgs);
  void writeReturn();
  void writeFunction(string functionName, string numLocals);
  void writeInit();
  void writeString(string str);
  // void close();
private:
  ofstream out;
};

#endif