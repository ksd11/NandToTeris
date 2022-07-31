#include<iostream>
#include<regex>
using namespace std;

int main(){
  // string pattern = "\\s*(\\w.*?)\\s*(//.*)?";
  // string pattern = "@\\s*(\\d+|[a-zA-Z_.$:][a-zA-Z_0-9.$:]+)\\s*";
  string re = "\\(\\s*([a-zA-Z][\\w-.$:]*)\\s*\\)";
  string text = "(ball.new)";
  // string text = "  // aaaa";
  // string text = "  // aaaa";
  // string text = "  // aaaa";
  // string text = "  // aaaa";
  // string text = "  // aaaa";

  regex r(re);
  smatch results;
  if(regex_match(text, results, r)){
    cout<<results.str(1)<<endl;
    cout<<results.str(2)<<endl;
    cout<<results.str(3)<<endl;
  }else{
    cout<<"no match"<<endl;
  }


  return 0;
}