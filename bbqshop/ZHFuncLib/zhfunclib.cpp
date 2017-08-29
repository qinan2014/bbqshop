#include "zhfunclib.h"
#include <QCoreApplication>

#pragma comment(lib ,"user32.lib")
#include <Tlhelp32.h>
#include <time.h>
#pragma comment (lib, "ws2_32.lib")
#include <QString>
#include <QFile>
#include "AllExeName.h"
#include <QSettings>

ZHFuncLib::ZHFuncLib()
{

}

ZHFuncLib::~ZHFuncLib()
{

}

std::string ZHFuncLib::GetWorkPath()
{
	static QString _workPath;
	if (_workPath.isEmpty())
	{
		_workPath = QCoreApplication::applicationDirPath();
		QFile tmpFile(_workPath + "/" + BBQSHOPEXE);
		if (!tmpFile.exists())
		{
			QSettings *reg = new QSettings(REGEDITRUN,QSettings::NativeFormat);
			QString startpath = reg->value(APPLICATIONNAME).toString();
			startpath = startpath.replace("\\", "/");
			int exepos = startpath.lastIndexOf('/');
			if (exepos > 0)
			{
				_workPath = startpath.left(exepos);
			}
		}
	}
	return _workPath.toStdString();
}

void ZHFuncLib::GetTargetProcessIds(std::string inTarget, std::vector<int > &outIds)
{
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(hProcessSnap == INVALID_HANDLE_VALUE)
		return;
	BOOL bProcess = Process32First(hProcessSnap, &pe32);
	int targetNameLen = inTarget.length();
	while(bProcess)
	{
		int searchLen = wcslen(pe32.szExeFile);
		if (targetNameLen == searchLen)
		{
			bool isEqualName = true;
			for (int i = 0; i < targetNameLen; ++i)
			{
				if (inTarget[i] != pe32.szExeFile[i])
				{
					isEqualName = false;
					break;
				}
			}
			if (isEqualName)
				outIds.push_back(pe32.th32ProcessID);
		}
		
		// 继续查找
		bProcess = Process32Next(hProcessSnap,&pe32);
	}
	CloseHandle(hProcessSnap);
}

void ZHFuncLib::GetAllProcesses(std::wstring inTarget, int &outTargetIndex, std::vector<std::wstring > &outAllProcess, std::vector<int > &outIds)
{
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(hProcessSnap == INVALID_HANDLE_VALUE)
		return;
	BOOL bProcess = Process32First(hProcessSnap, &pe32);
	int indextarget = 0;
	while(bProcess)
	{
		outAllProcess.push_back(pe32.szExeFile);
		outIds.push_back(pe32.th32ProcessID);

		//NativeLog("", WstringToString(pe32.szExeFile).c_str(), "a");
		//NativeLog("", WstringToString(inTarget).c_str(), "a");
		if (wcscmp(pe32.szExeFile, inTarget.c_str()) == 0)
		{
			outTargetIndex = indextarget;
		}
		++indextarget;
		// 继续查找
		bProcess = Process32Next(hProcessSnap,&pe32);
	}
	CloseHandle(hProcessSnap);
}

bool ZHFuncLib::InjectDllByProcessID(const std::wstring dllPath, unsigned long inProcessID)
{
	wchar_t* DirPath = new wchar_t[MAX_PATH];
	wchar_t* FullPath = new wchar_t[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, DirPath);
	swprintf_s(FullPath, MAX_PATH, dllPath.c_str(), DirPath);

	HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION |
		PROCESS_VM_WRITE, FALSE, inProcessID);
	if (hProcess == NULL)
	{
		delete[] DirPath;
		delete[] FullPath;
		return false;
	}

	LPVOID LoadLibraryAddr = (LPVOID)GetProcAddress(GetModuleHandle(L"kernel32.dll"),
		"LoadLibraryW");
	if (LoadLibraryAddr == NULL)
	{
		CloseHandle(hProcess);
		delete[] DirPath;
		delete[] FullPath;
		return false;
	}

	LPVOID LLParam = (LPVOID)VirtualAllocEx(hProcess, NULL, (wcslen(FullPath) + 1) * sizeof(wchar_t),
		MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	WriteProcessMemory(hProcess, LLParam, FullPath, (wcslen(FullPath) + 1)* sizeof(wchar_t), NULL);

	HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryAddr,
		LLParam, NULL, NULL);

	// 等待远程线程结束    
	::WaitForSingleObject(hRemoteThread, INFINITE);    
	// 清理    
	::VirtualFreeEx(hProcess, LLParam, (wcslen(FullPath) + 1), MEM_DECOMMIT);    
	::CloseHandle(hRemoteThread);    
	::CloseHandle(hProcess);

	delete[] DirPath;
	delete[] FullPath;
	return true;
}

//bool ZHFuncLib::InjectDllByProcessName(const std::wstring dllPath, const std::wstring inProcessName)
//{
//	PROCESSENTRY32 pe32;
//	pe32.dwSize = sizeof(PROCESSENTRY32);
//	HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
//	BOOL bProcess = Process32First(hsnap, &pe32);
//	bool flag = false;
//	if (bProcess == TRUE)
//	{
//		while (Process32Next(hsnap, &pe32) == TRUE)
//		{
//			if (wcscmp(pe32.szExeFile, inProcessName.c_str()) == 0)
//			{
//				flag = InjectDllByProcessID(dllPath, pe32.th32ProcessID);
//				break;
//			}
//		}
//	}
//	return flag;
//}

bool ZHFuncLib::UnInjectDll(const std::wstring dllPath, unsigned long inProcessID)   
{
	// 参数无效   
	if (dllPath.length() < 1)
		return false;
	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;   
	HANDLE hProcess = NULL;   
	HANDLE hThread = NULL;   
	// 获取模块快照   
	hModuleSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, inProcessID);   
	if (INVALID_HANDLE_VALUE == hModuleSnap)   
	{   
		return false;   
	}   
	MODULEENTRY32 me32;   
	memset(&me32, 0, sizeof(MODULEENTRY32));   
	me32.dwSize = sizeof(MODULEENTRY32);   
	// 开始遍历   
	if(FALSE == ::Module32First(hModuleSnap, &me32))   
	{   
		::CloseHandle(hModuleSnap);   
		return false;   
	}   
	// 遍历查找指定模块   
	bool isFound = false;
	//NativeLog("", WstringToString(dllPath).c_str(), "a");
	do  
	{
		//NativeLog("", WstringToString(me32.szExePath).c_str(), "a");
		isFound = (0 == wcscmp(me32.szModule, dllPath.c_str()) || 0 == wcscmp(me32.szExePath, dllPath.c_str()));   
		if (isFound) // 找到指定模块   
		{   
			break;   
		}   
	} while (TRUE == ::Module32Next(hModuleSnap, &me32));   
	::CloseHandle(hModuleSnap);   
	if (false == isFound)   
	{   
		return false;   
	}   
	// 获取目标进程句柄   
	hProcess = ::OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION, FALSE, inProcessID);  
	if (NULL == hProcess)   
	{   
		return false;   
	}   
	// 从 Kernel32.dll 中获取 FreeLibrary 函数地址   
	LPTHREAD_START_ROUTINE lpThreadFun = (PTHREAD_START_ROUTINE)::GetProcAddress(::GetModuleHandle(L"Kernel32"), "FreeLibrary");  
	if (NULL == lpThreadFun)   
	{   
		::CloseHandle(hProcess);   
		return false;   
	}   
	// 创建远程线程调用 FreeLibrary   
	hThread = ::CreateRemoteThread(hProcess, NULL, 0, lpThreadFun, me32.modBaseAddr /* 模块地址 */, 0, NULL);   
	if (NULL == hThread)   
	{   
		::CloseHandle(hProcess);   
		return false;   
	}   
	// 等待远程线程结束   
	::WaitForSingleObject(hThread, INFINITE);   
	// 清理   
	::CloseHandle(hThread);   
	::CloseHandle(hProcess);   
	return true;   
}


bool ZHFuncLib::TerminateProcessExceptCurrentOne(std::string inTarget)
{
	std::vector<int > ids;
	GetTargetProcessIds(inTarget, ids);
	DWORD processId = GetCurrentProcessId();//当前进程id

	// 除了当前进程外 其余都结束
	int sz = ids.size();
	bool hasAnotherProcess = false;
	for (int i = 0; i < sz; ++i)
	{
		if (ids[i] == processId)
			continue;
		hasAnotherProcess = true;
		HANDLE Hwnd = OpenProcess(PROCESS_TERMINATE | SYNCHRONIZE,0,ids[i]);
		TerminateProcess(Hwnd, 0);
	}
	return hasAnotherProcess;
}

void ZHFuncLib::NativeLog(const char *extname, const char *content, char *mode)
{
	time_t t = time(NULL);
	struct tm *local = localtime(&t);
	char logbuftxt[200];
	sprintf(logbuftxt, "%s/logdata%s-%d-%d-%d-%d.txt", ZHFuncLib::GetWorkPath().c_str(), extname, local->tm_year, local->tm_mon + 1, local->tm_mday, local->tm_hour);

	FILE * fp = NULL;
	if((fp = fopen(logbuftxt, mode)) != NULL)
	{
		char buf[30];
		sprintf(buf, "time %d:%d:%d------- \r\n", local->tm_hour, local->tm_min, local->tm_sec);
		fwrite(buf, strlen(buf), 1, fp);
		fwrite(content, strlen(content), 1, fp);
		fwrite("\r\n\r\n", strlen("\r\n\r\n"), 1, fp);
		fclose(fp);
	}
}

bool ZHFuncLib::SendProcessMessage(HWND selfWnd, HWND targetWnd, ULONG_PTR dataType, std::string willSendData)
{
	if (::IsWindow(targetWnd))
	{
		char *cstr = new char[willSendData.length() + 1];
		strcpy(cstr, willSendData.c_str());
		COPYDATASTRUCT copydata;
		copydata.dwData = dataType;  // 用户定义数据
		copydata.lpData = cstr;  //数据大小
		copydata.cbData = willSendData.length();  // 指向数据的指针
		::SendMessage(targetWnd, WM_COPYDATA, reinterpret_cast<WPARAM>(selfWnd), reinterpret_cast<LPARAM>(&copydata));
		delete [] cstr;
		return true;
	}
	return false;
}

std::wstring ZHFuncLib::StringToWstring(const std::string str)
{// string转wstring
	unsigned len = str.size() * 2;// 预留字节数
	setlocale(LC_CTYPE, "");     //必须调用此函数
	wchar_t *p = new wchar_t[len];// 申请一段内存存放转换后的字符串
	mbstowcs(p,str.c_str(),len);// 转换
	std::wstring str1(p);
	delete[] p;// 释放申请的内存
	return str1;
}

std::string ZHFuncLib::WstringToString(const std::wstring str)
{// wstring转string
	unsigned len = str.size() * 4;
	setlocale(LC_CTYPE, "");
	char *p = new char[len];
	wcstombs(p,str.c_str(),len);
	std::string str1(p);
	delete[] p;
	return str1;
}
