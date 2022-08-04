#ifndef PARSE_PATH_H
#define PARSE_PATH_H

#include<string>
using namespace std;

void path_init(string path, string &output_file);
bool hasNextFile();
string nextFile();
string nextFile(string& out);
#endif