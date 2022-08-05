#include"VMWriter.h"

static Command char_to_command(char c){
  switch(c){
    case '+': return ADD;
    case '-': return SUB;
    case 'n': return NEG;
    case '=': return EQ;
    case '>': return GT;
    case '<': return LT;
    case '&': return AND;
    case '|': return OR;
    case '~': return NOT;
  }

  cout<<"char_to_command error: "<<c<<endl;
  exit(1);
}

void VMWriter::writeArithmetic(char c){
  writeArithmetic(char_to_command(c));
}

static Segment string_to_segment(string s){
  if(s=="constant") return CONST;
  else if(s=="argument") return ARG;
  else if(s=="local"||s=="var") return LOCAL;
  else if(s=="static") return STATIC;
  else if(s=="this") return THIS;
  else if(s=="that") return THAT;
  else if(s=="pointer") return POINTER;
  else if(s=="temp") return TEMP;

  cout<<"string_to_segment error: "<<s<<endl;
  exit(1);
}

static string segment_to_string(Segment segment){
  switch(segment){
    case CONST:
      return "constant";
    case ARG:
      return "argument";
    case LOCAL:
      return "local";
    case STATIC:
      return "static";
    case THIS:
      return "this";
    case THAT:
      return "that";
    case POINTER:
      return "pointer";
    case TEMP:
      return "temp";
  }
  cout<<"segment_to_string error: "<<segment<<endl;
  exit(1);
}

VMWriter::VMWriter(string file){
  out.open(file);
}

VMWriter::~VMWriter(){
  out.close();
}

void VMWriter::writePush(Segment segment, int index){
  out<<"push "<<segment_to_string(segment)<<" "<<index<<endl;
}

void VMWriter::writePop(Segment segment,int index){
  out<<"pop "<<segment_to_string(segment)<<" "<<index<<endl;
}

void VMWriter::writeArithmetic(Command command){
  switch(command){
    case ADD:
      out<<"add\n";
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

void VMWriter::writePop(string s,int index){
  writePop(string_to_segment(s), index);
}

void VMWriter::writePush(string s,int index){
  writePush(string_to_segment(s), index);
}
