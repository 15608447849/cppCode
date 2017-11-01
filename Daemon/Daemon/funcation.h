#include <iostream>      
#include <string.h> 
#include <time.h>
#include <list>  
#include <map>  
#include <windows.h> 
#include <TlHelp32.h> 
#include  <direct.h> 
#include  <io.h>
#include  <stdio.h>
#include  <stdlib.h>
#include <fstream>
#include <sstream>
#include <algorithm>
using namespace std;
string& filePathHandle(string& path);
void printFile(string content);
string getTime();
void all_procress(list<std::string> &procList);
void eachTargetProcExistList(map<string, string> &moniterMaps, list<std::string> &procList);