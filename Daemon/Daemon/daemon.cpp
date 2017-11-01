#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"") //设置入口地址 - 不显示控制台 
#include "funcation.h"

#include "json\json.h"





int main()
{
	//互斥锁
	LPCWSTR mutex_name = TEXT("my_dameon_singer_value");
	HANDLE mutex;
	mutex = ::OpenMutex(MUTEX_ALL_ACCESS, FALSE, mutex_name);
	if (mutex == NULL)
	{
		mutex = ::CreateMutex(NULL, TRUE, mutex_name);
	}else {
		return 0;
	}
	//获取根目录
	char root_path[MAX_PATH];
	getcwd(root_path, MAX_PATH);

	//临时路径变量
	char path[MAX_PATH];

	//打开程序的exe
	char OPEN_EXE[20] = "\\open_procress.exe ";
	memset(path, 0, sizeof(path));
	strcpy(path, root_path);
	strcat(path, OPEN_EXE);
	if ((_access(path, 0)) == -1)
	{

		string str = path;
		printFile(getTime() + " >> procress ( " + str + " ) not exist!");
		return -1;
	}
	string open_exe_path = path;
	filePathHandle(open_exe_path);
	
	//守护自己的进程
	const char DAEMON_SELF[20] = "Daemon_moniter.exe";
	memset(path, 0, sizeof(path));
	strcpy(path, root_path);
	strcat(path, "\\");
	strcat(path, DAEMON_SELF);
	//如果存在->添加到 监听列表
	if ((_access(path, 0)) == -1)
	{
		string str = path;
		printFile(getTime() + " >> procress ( " + str + " ) not exist!");
		return -1;
	}

	//需要监听的进程列表
	map<string, string> moniterMaps;
	//添加 守护自己的守护进程
	moniterMaps.insert(pair<string, string>(
		DAEMON_SELF,
		open_exe_path +"@\"open\" "+ string(path) + " \"server\"")
	);

	//读取配置文件
	const char FILE_NAME[20] = "\\res\\monitor.conf";
	memset(path, 0, sizeof(path));
	strcpy(path, root_path);
	strcat(path,FILE_NAME);
	ifstream* in = new ifstream(path, ios::binary);
	if (!in->is_open())
	{
		string str = path;
		printFile(getTime()+ " >> config ( " + str +" ) not exist!");
		return -1;
	}
	//json读取
	Json::Reader *reader = new Json::Reader();
	Json::Value *root = new Json::Value();
	if (!reader->parse(*in, *root, false)) {
		//解析失败
		printFile(getTime() + " >> json parse error.");
		return -1;
	}
	for (int i = 0; i<root->size(); ++i)
	{
		

		moniterMaps.insert(pair<string, string>(
			(*root)[i]["procName"].asString(),
			open_exe_path +"@\"open\" \""+ filePathHandle((*root)[i]["procPath"].asString()) +"\" \""+ (*root)[i]["procType"].asString()+"\"")
		);
	}
	in->close();
	delete(in);
	delete(reader);
	delete(root);

	map <string, string >::iterator  iterMaps;
	pair<string, string> tPair;
	for (iterMaps = moniterMaps.begin(); iterMaps != moniterMaps.end(); iterMaps++) {
		tPair = *iterMaps;
		cout << tPair.first.c_str() << "\n" << tPair.second << endl;
	}
	
	//当前所有进程名列表
	list<string> procNames;
	while (true) {
		
		all_procress(procNames);
		eachTargetProcExistList(moniterMaps, procNames);
		Sleep(2*1000);
	}
	//system("pause");
	return 0;
}











