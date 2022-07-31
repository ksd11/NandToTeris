#include<iostream>
#include<map>
#include<string>
#include<cstdint>
using namespace std;

class SymbolTable{
public:
  void addEntry(string,uint16_t);
  bool contains(string);
  int getAddress(string);
  void setAddress(string, uint16_t);
private:
  map<string,uint16_t> table;
};