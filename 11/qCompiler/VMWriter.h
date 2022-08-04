#ifndef VMWRITER_H
#define VMWRITER_H

#include<string>
#include<fstream>
using namespace std;

enum Segment{
  CONST,ARG,LOCAL,STATIC,
  THIS,THAT,POINTER,TEMP
};

enum Command{
  ADD,SUB,NEG,EQ,GT,LT,AND,OR,NOT
};

class VMWriter{
public:
  VMWriter(string file);
  ~VMWriter();
  void writePush(Segment segment, int index);
  void writePop(Segment segment,int index);
  void writeArithmetic(Command command);
  void writeLabel(string label);
  void writeGoto(string label);
  void writeIf(string label);
  void writeCall(string name, int nArgs);
  void writeFunction(string name,int nArgs);
  void writeReturn();
private:
  ofstream out;
};

#endif