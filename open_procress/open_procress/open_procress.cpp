#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"") //������ڵ�ַ - ����ʾ����̨ 
#include "stdafx.h"
#include <windows.h>
#include <iostream>  
#include <string.h>
#include  <direct.h> 
#include  <io.h>
#include  <stdio.h>
#include  <stdlib.h>
#include <algorithm>
using namespace std;



//�ļ�·������
string& filePathHandle(string& path) {
	//cout << path.c_str() << endl;
	string::size_type pos = 0;

	pos = 0;
	while ((pos = path.find_first_of("/", pos)) != string::npos)
	{
		path.replace(pos, 1, "\\");//�滻  
		pos++;
	}
	pos = 0;
	path.erase(0, path.find_first_not_of(" "));
	path.erase(path.find_last_not_of(" ") + 1);
	while ((pos = path.find_first_of(" ", pos)) != string::npos)
	{
		path.replace(pos, 1, "\040");
		pos = pos++;
	}
	return path;
}

//��һ������
void openProcress(const char* path,const char* type) {

	//cout << path <<" "<<type<<endl;
	int flag = 0;//��ʾĬ�ϵĲ�����Ĭ�ϵĲ�������ʾ������δ����������Ǵ��ڳ��֣����������صĳ���
	if (strcmp(type, "console") == 0) {
		flag = CREATE_NEW_CONSOLE;
	}
	else if (strcmp(type,"server") == 0) {
		flag = CREATE_NO_WINDOW;
	}
	DWORD dwCreationFlags = flag;


	string filePath = path;
	filePathHandle(filePath);
	TCHAR file[100];
	MultiByteToWideChar(CP_ACP, 0, filePath.c_str(), -1, file, 100);
	
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;
	bool bRet = CreateProcess(
		file,
		NULL,
		NULL,
		NULL,
		FALSE,
		dwCreationFlags,//CREATE_NEW_CONSOLE,//CREATE_NO_WINDOW,
		NULL,
		NULL,
		&si,
		&pi);
	if (bRet)
	{
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
}



int main(int argc, char *argv[])
{
	for (int i = 0; i < argc;i< i++) {
		cout << argv[i] << endl;
		if (strcmp(argv[i],"open")==0) {
			openProcress(argv[i+1], argv[i+2]);
			break;
		}
	}
    return 0;
}

