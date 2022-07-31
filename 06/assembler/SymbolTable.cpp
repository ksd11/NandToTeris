
#include"SymbolTable.h"

void SymbolTable::addEntry(string symbol,uint16_t address){
  table.insert({symbol, address});
}

bool SymbolTable::contains(string symbol){
  return table.count(symbol)==1;
}

int SymbolTable::getAddress(string symbol){
  return table[symbol];
}

void SymbolTable::setAddress(string symbol, uint16_t address){
  table[symbol] = address;
}
