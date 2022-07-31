#include<cstdint>
#include<string>
using namespace std;
// a: 0xxx xxxx xxxx xxxx
// c: 111a cccc ccdd djjj
//                ad mleg

class Code{
public:
  uint16_t dest(string);
  uint16_t comp(string);
  uint16_t jump(string);
private:
  void removeBlank(string& s){
    int i;
    while((i = s.find(" "))!=string::npos){
      s.erase(i,1);
    }
  }
};