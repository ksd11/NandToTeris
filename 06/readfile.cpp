#include<iostream>
#include<string>
#include<fstream>
using namespace std;

int main(int argc,char *argv[]){
  ifstream in("1.txt");
  string s;
  while(getline(in, s)){
    cout<<s<<endl;
  }
  return 0;
}