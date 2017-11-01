#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"") //������ڵ�ַ - ����ʾ����̨ 
#include "funcation.h"

#include "json\json.h"





int main()
{
	//������
	LPCWSTR mutex_name = TEXT("my_dameon_singer_value");
	HANDLE mutex;
	mutex = ::OpenMutex(MUTEX_ALL_ACCESS, FALSE, mutex_name);
	if (mutex == NULL)
	{
		mutex = ::CreateMutex(NULL, TRUE, mutex_name);
	}else {
		return 0;
	}
	//��ȡ��Ŀ¼
	char root_path[MAX_PATH];
	getcwd(root_path, MAX_PATH);

	//��ʱ·������
	char path[MAX_PATH];

	//�򿪳����exe
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
	
	//�ػ��Լ��Ľ���
	const char DAEMON_SELF[20] = "Daemon_moniter.exe";
	memset(path, 0, sizeof(path));
	strcpy(path, root_path);
	strcat(path, "\\");
	strcat(path, DAEMON_SELF);
	//�������->��ӵ� �����б�
	if ((_access(path, 0)) == -1)
	{
		string str = path;
		printFile(getTime() + " >> procress ( " + str + " ) not exist!");
		return -1;
	}

	//��Ҫ�����Ľ����б�
	map<string, string> moniterMaps;
	//��� �ػ��Լ����ػ�����
	moniterMaps.insert(pair<string, string>(
		DAEMON_SELF,
		open_exe_path +"@\"open\" "+ string(path) + " \"server\"")
	);

	//��ȡ�����ļ�
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
	//json��ȡ
	Json::Reader *reader = new Json::Reader();
	Json::Value *root = new Json::Value();
	if (!reader->parse(*in, *root, false)) {
		//����ʧ��
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
	
	//��ǰ���н������б�
	list<string> procNames;
	while (true) {
		
		all_procress(procNames);
		eachTargetProcExistList(moniterMaps, procNames);
		Sleep(2*1000);
	}
	//system("pause");
	return 0;
}











