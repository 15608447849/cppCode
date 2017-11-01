#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
#include <windows.h> 
#include <TlHelp32.h>
#include <iostream>  
#include <string.h>
#include  <direct.h>
#include  <io.h>
#include  <stdio.h>
#include  <stdlib.h>
#include <algorithm>
#include <fstream>
using namespace std;
//向指定文件写入数据
void printTargerFile(string file_paht, string content) {
	ofstream file;
	file.open(file_paht, ios::app);
	if (file.is_open())
	{
		file << content.c_str() << endl;
	}
	file.close();
}

void printFile(string content) {
	const string fileName = "daemon.log";
	printTargerFile(fileName, content);
}
//文件路径处理
string& filePathHandle(string& path) {
	
	string::size_type pos = 0;

	pos = 0;
	while ((pos = path.find_first_of("/", pos)) != string::npos)
	{
		path.replace(pos, 1, "\\");//替换  
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




std::string MCHAT_TO_STRING(TCHAR* p_tchar) {
	int iLen = WideCharToMultiByte(CP_ACP, 0, p_tchar, -1, NULL, 0, NULL, NULL);
	char* chRtn = new char[iLen * sizeof(char)];
	WideCharToMultiByte(CP_ACP, 0, p_tchar, -1, chRtn, iLen, NULL, NULL);
	std::string str(chRtn);
	return str;
}

int check(string procName) {
	int result = -1;
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE) {
		return result;
	}
	BOOL bResult = Process32First(hProcessSnap, &pe32);
	while (bResult)
	{
		TCHAR* name = pe32.szExeFile;
		std::string str = MCHAT_TO_STRING(name);
		/*int pid = pe32.th32ProcessID;
		int ppid = pe32.th32ParentProcessID;
		cout << "程序名: " << str.c_str()<< "		,进程ID: " << pid <<"	,父进程ID: "<< ppid<<  endl;*/
		if (str.compare(procName) == 0) {
			result = 0;
			//break;
		}
		bResult = Process32Next(hProcessSnap, &pe32);
	}
	CloseHandle(hProcessSnap);
	return result;
}

//打开一个程序
void openProcress(string path,string param) {

	TCHAR file[256];
	MultiByteToWideChar(CP_ACP, 0, path.c_str(), -1, file, 256);

	TCHAR commandLine[256];
	MultiByteToWideChar(CP_ACP, 0, param.c_str(), -1, commandLine, 256);

	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;
	bool bRet = CreateProcess(
		file,
		commandLine,
		NULL,
		NULL,
		FALSE,
		CREATE_NO_WINDOW,//CREATE_NO_WINDOW,
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

int main() {
	//互斥锁
	LPCWSTR mutex_name = TEXT("my_dameon_moniter_singer_value");
	HANDLE mutex;
	mutex = ::OpenMutex(MUTEX_ALL_ACCESS, FALSE, mutex_name);
	if (mutex == NULL)
	{
		mutex = ::CreateMutex(NULL, TRUE, mutex_name);
	}
	else {
		
		return 0;
	}

	//获取路径
	char root_path[MAX_PATH];
	getcwd(root_path, MAX_PATH);
	
	char path[MAX_PATH];
	
	//判断 open_procress.exe
	memset(path, 0, sizeof(path));
	strcpy(path, root_path);
	strcat(path, "\\open_procress.exe");
	
	if ((_access(path, 0)) == -1)
	{
		printFile("daemon_moniter.exe >> " + string(path) + " is not exist!");
		return -1;
	}
	const string EXE_PATH = filePathHandle(string(path));
	
	//判断Daemon.exe
	const string Daemon_proc_name = "Daemon.exe";
	memset(path, 0, sizeof(path));
	strcpy(path, root_path);
	strcat(path, "\\");
	strcat(path, Daemon_proc_name.c_str());
	if ((_access(path, 0)) == -1)
	{
		printFile("daemon_moniter.exe >> " + string(path) + " is not exist!");
		return -1;
	}
	const string PARAMS = "\"open\" \""+string(path)+"\""+" \"server\"";

	while (true) {
		//检测
		if (check(Daemon_proc_name) != 0) {
			//打开进程
			openProcress(EXE_PATH, PARAMS);
		}
		Sleep(1000);
	}
	return 0;
}