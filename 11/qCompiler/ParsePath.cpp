#include <io.h>
#include <vector>
#include<stdlib.h>
#include<sys/stat.h>
#include<string>
#include<iostream>
#include"ParsePath.h"
using namespace std;

static vector<string> files;
static int cnt = 0;
static int len;
static string input_file_extension = ".jack";
static string output_file_extension = ".xml";

static string change_extension(string str){
  size_t idx = str.rfind(input_file_extension);
  if(idx == string::npos){
    return str+output_file_extension;
  }
  str.erase(idx,input_file_extension.length());
  return str+output_file_extension;
}

static string get_path_name(string str){
  int idx=-1;
  int len = str.length();
  for(int i=len-1;i>=0;i--){
    if(str[i] == '\\' || str[i] == '/'){
      idx = i;
      break;
    }
  }
  if(idx>=0)
    str = str.substr(idx+1);
  return str;
}

static void readDir(string path, vector<string> &res)
{
    string p;
    long hFile = 0;                //文件句柄
    struct _finddata_t fileinfo;    //文件信息
    //查找文件夹中的第一个文件，然后利用句柄进行遍历
    if((hFile = _findfirst(p.assign(path).append("/*"+input_file_extension).c_str(), &fileinfo)) != 0)
    {
        do
        {
            if(!(fileinfo.attrib & _A_SUBDIR)){
              string path_s = p.assign(path).append("/").append(fileinfo.name);
              res.push_back(path_s); 
            }
            
        }
        while(_findnext(hFile, &fileinfo) != -1);
        _findclose(hFile);    //关闭文件
    }
}

void path_init(string path,string &output_file){
  struct _stat buf;
	int result;
	result = _stat(path.c_str(), &buf);
	if (_S_IFDIR & buf.st_mode) {
		readDir(path, files);
    output_file = path+"/"+get_path_name(path)+output_file_extension;
    len=files.size();
	}
	else if (_S_IFREG & buf.st_mode) {
	  files.push_back(path);
    output_file = change_extension(path);
    len = 1;
	}else{
    cout<<path + " don't exist!"<<endl;
    exit(1);
  }
}

bool hasNextFile(){
  return cnt < len;
}

string nextFile(){
  return files[cnt++];
}

string nextFile(string& out){
  string input = files[cnt++];
  out = change_extension(input);
  return input;
}
 