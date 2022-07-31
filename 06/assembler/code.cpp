#include"code.h"
#include<map>



uint16_t Code::dest(string dst){
  if(dst=="") return 0;
  
  uint16_t res = 0;
  size_t idx = dst.find("D");
  if(idx != string::npos){
    res |= 0B0000'0000'0001'0000;
  }
  
  idx = dst.find("A");
  if(idx != string::npos){
    res |= 0B0000'0000'0010'0000;
  }

  idx = dst.find("M");
  if(idx != string::npos){
    res |= 0B0000'0000'0000'1000;
  }

  return res;
}

uint16_t Code::comp(string expr){
  if(expr=="") return 0; 
  removeBlank(expr);
  if(expr=="0"){
    return 0B0000'1010'1000'0000;
  }else if(expr=="1"){
    return 0B0000'1111'1100'0000;
  }else if(expr=="-1"){
    return 0B0000'1110'1000'0000;
  }else if(expr=="D"){
    return 0B0000'0011'0000'0000;
  }else if(expr=="A"){
    return 0B0000'1100'0000'0000;
  }else if(expr=="M"){
    return 0B0001'1100'0000'0000;
  }else if(expr=="!D"){
    return 0B0000'0011'0100'0000;
  }else if(expr=="!A"){
    return 0B0000'1100'0100'0000;
  }else if(expr=="!M"){
    return 0B0001'1100'0100'0000;
  }else if(expr=="-D"){
    return 0B0000'0011'1100'0000;
  }else if(expr=="-A"){
    return 0B0000'1100'1100'0000;
  }else if(expr=="-M"){
    return 0B0001'1100'1100'0000;
  }else if(expr=="D+1"){
    return 0B0000'0111'1100'0000;
  }else if(expr=="A+1"){
    return 0B0000'1101'1100'0000;
  }else if(expr=="M+1"){
    return 0B0001'1101'1100'0000;
  }else if(expr=="D-1"){
    return 0B0000'0011'1000'0000;
  }else if(expr=="A-1"){
    return 0B0000'1100'1000'0000;
  }else if(expr=="M-1"){
    return 0B0001'1100'1000'0000;
  }else if(expr=="D+A"){
    return 0B0000'0000'1000'0000;
  }else if(expr=="D+M"){
    return 0B0001'0000'1000'0000;
  }else if(expr=="D-A"){
    return 0B0000'0100'1100'0000;
  }else if(expr=="D-M"){
    return 0B0001'0100'1100'0000;
  }else if(expr=="A-D"){
    return 0B0000'0001'1100'0000;
  }else if(expr=="M-D"){
    return 0B0001'0001'1100'0000;
  }else if(expr=="D&A"){
    return 0B0000'0000'0000'0000;
  }else if(expr=="D&M"){
    return 0B0001'0000'0000'0000;
  }else if(expr=="D|A"){
    return 0B0000'0101'0100'0000;
  }else if(expr=="D|M"){
    return 0B0001'0101'0100'0000;
  }
  // assert(0);
  return 0;
}

uint16_t Code::jump(string str){
  if(str=="") return 0;
  uint16_t res = 0;
  map<string,uint16_t> mp = {
    {"JGT",0B0000'0000'0000'0001},
    {"JEQ",0B0000'0000'0000'0010},
    {"JGE",0B0000'0000'0000'0011},
    {"JLT",0B0000'0000'0000'0100},
    {"JNE",0B0000'0000'0000'0101},
    {"JLE",0B0000'0000'0000'0110},
    {"JMP",0B0000'0000'0000'0111},
  };


  map<string,uint16_t>::iterator it;
  for(it=mp.begin(); it!=mp.end();it++){
    if(str.find(it->first)!=string::npos){
      return it->second;
    }
  }
  return 0;
}
