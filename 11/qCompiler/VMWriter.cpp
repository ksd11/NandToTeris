#include"VMWriter.h"

VMWriter::VMWriter(string file){
  out.open(file);
}

VMWriter::~VMWriter(){
  out.close();
}

void VMWriter::writePush(Segment segment, int index){
  out<<"push "<<segment<<" "<<index<<endl;
}

void VMWriter::writePop(Segment segment,int index){
  out<<"pop "<<segment<<" "<<index<<endl;
}

void VMWriter::writeArithmetic(Command command){
  switch(command){
    case ADD:
      out<<"and\n";
      break;
    case SUB:
      out<<"sub\n";
      break;
    case NEG:
      out<<"neg\n";
      break;
    case EQ:
      out<<"eq\n";
      break;
    case GT:
      out<<"gt\n";
      break;
    case LT:
      out<<"lt\n";
      break;
    case AND:
      out<<"and\n";
      break;
    case OR:
      out<<"or\n";
      break;
    case NOT:
      out<<"not\n";
      break;
  }
}

void VMWriter::writeLabel(string label){
  out<<"label "<<label<<endl;
}

void VMWriter::writeGoto(string label){
  out<<"goto "<<label<<endl;
}

void VMWriter::writeIf(string label){
  out<<"if-goto "<<label<<endl;
}

//nArgs为参数个数
void VMWriter::writeCall(string name, int nArgs){
  out<<"call "<<name<<" "<<nArgs<<endl;
}

//nArgs为局部变量个数
void VMWriter::writeFunction(string name,int nArgs){
  out<<"function "<<name<<" "<<nArgs<<endl;
}
  
void VMWriter::writeReturn(){
  out<<"return"<<endl;
}
