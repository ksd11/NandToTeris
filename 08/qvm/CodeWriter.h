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
  // void close();
private:
  ofstream out;
};

#endif