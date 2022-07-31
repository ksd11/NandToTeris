#include<iostream>
#include<string>
using namespace std;


void removeBlank(string& s){
    int i;
    while((i = s.find(" "))!=string::npos){
      s.erase(i,1);
    }
  }

int main(int argc,char *argv[]){
  string s = "A =   D- M";
  removeBlank(s);
  cout<<s;
  return 0;
}