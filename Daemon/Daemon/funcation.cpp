#include "funcation.h"







//文件路径处理
string& filePathHandle(string& path) {
	//cout << path.c_str() << endl;
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
		path.replace(pos, 1,"\040"); 
		pos = pos++;
	}
	return path;
}



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
	printTargerFile(fileName,content);
}


string getTime(){
	     time_t timep;
		 time(&timep);
	     char tmp[64];
	     strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&timep));
	     return tmp;
}

std::string MCHAT_TO_STRING(TCHAR* p_tchar) {
	int iLen = WideCharToMultiByte(CP_ACP, 0, p_tchar, -1, NULL, 0, NULL, NULL);
	char* chRtn = new char[iLen * sizeof(char)];
	WideCharToMultiByte(CP_ACP, 0, p_tchar, -1, chRtn, iLen, NULL, NULL);
	std::string str(chRtn);
	return str;
}
void all_procress(list<std::string> &procList)
{
	
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE) {
		std::cout << "CreateToolhelp32Snapshot Error!" << std::endl;;
		return;
	}
	BOOL bResult = Process32First(hProcessSnap, &pe32);
	procList.clear();
	while (bResult)
	{

		TCHAR* name = pe32.szExeFile;
		std::string str = MCHAT_TO_STRING(name);
		//int id = pe32.th32ProcessID;
		procList.push_back(str); //存储
		bResult = Process32Next(hProcessSnap, &pe32);
	}
	CloseHandle(hProcessSnap);
}


//int->string 
string int2string(int mInt) {
	stringstream ss;
	string str;
	ss << mInt;
	ss >> str;
	return str;
}

bool isRunning(string procName, list<std::string>& procList) {

	list<string>::iterator iterList;
	string cProcName;
	for (iterList = procList.begin(); iterList != procList.end(); iterList++) {
		cProcName = *iterList;
		if (cProcName.compare(procName) == 0) {
			return true;
		}
	}
	return false;
}
//打开一个程序
void openProcress(const char* path) {
	//cout << getTime().c_str() << " >> command: " << path  << endl;
	string exePath;
	string param;
	string str = path;
	//字符串截取 
	string::size_type pos = 0;
	if ((pos = str.find("@")) != string::npos)
	{
		exePath = str.substr(0, pos);
		param = str.substr(pos+1);
		cout << exePath.c_str()<<endl;
		cout << param.c_str() << endl;
	}
	else {
		return;
	}

	TCHAR file[256];
	MultiByteToWideChar(CP_ACP, 0, exePath.c_str(), -1, file, 256);

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
		CREATE_NO_WINDOW,//CREATE_NEW_CONSOLE,//CREATE_NO_WINDOW,
		NULL,
		NULL,
		&si,
		&pi);
	int errorCode = GetLastError();
	if (bRet)
	{
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
	else {
		string errorStr = getTime() + " >> Command: " + path + " , error code: "+ int2string(errorCode);
		printFile(errorStr);
	}
}

//遍历指定程序是否在运行中列表
void eachTargetProcExistList(map<string, string>& moniterMaps, list<std::string>& procList)
{
	map <string, string >::iterator  iterMaps;
	pair<string, string> tPair;
	for (iterMaps = moniterMaps.begin(); iterMaps != moniterMaps.end(); iterMaps++)
	{
		tPair = *iterMaps;
		if(!isRunning(tPair.first,procList)){
			openProcress(tPair.second.c_str());
		}
	}
}


